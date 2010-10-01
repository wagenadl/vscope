// videoresp.cpp

#include "videoresp.h"

#include <base/exception.h>
#include <base/dbg.h>
#include <daq/analogin.h>
#include <daq/digitalin.h>
#include <base/analogdata.h>
#include <base/digitaldata.h>

VideoResp::VideoResp(int dline): dline(dline) {
  dmask = 1<<dline;
  active = false;
  complete = error = false;
  adata = 0;
  ddata = 0;
  ain = 0;
  din = 0;
}

VideoResp::~VideoResp() {
  if (active) {
    fprintf(stderr,"Warning! VideoResp destroyed while active");
    try {
      ain->abort();
    } catch (Exception const &e) {
      e.report();
    }
  }
  if (din)
    delete din;
  if (ain)
    delete ain;
  if (ddata)
    delete ddata;
  if (adata)
    delete adata;
}


void VideoResp::prepare() {
  if (active)
    throw Exception("VideoResp","Already active","prepare");

  ddata = new DigitalData(CHUNKLEN);
  adata = new AnalogData(CHUNKLEN,1);

  if (!ain) {
    ain = new AnalogIn;
    ain->setAcqLength(0); // undef. length
    ain->setFrequency(FREQHZ);
    ain->addChannel(0);
    ain->setPollPeriod(1024);
    connect(ain,SIGNAL(dataAvailable(AnalogIn *,int)),
	    this,SLOT(aDataAv(AnalogIn *,int)));
  }
  if (!din) {
    din = new DigitalIn(ain);
    din->addChannel(dline);
    din->setPollPeriod(1024);
    connect(din,SIGNAL(dataAvailable(DigitalIn *,int)),
	    this,SLOT(dDataAv(DigitalIn *,int)));
  }    
  
  bit = bytes = wakeups = 0;
  byte = 0;
  inmsg = inbyte = inpulse = false;
  complete = error = false;

  dbg("videoresp: committing");
  ain->commit();
  dbg("videoresp: committed; starting");
  active = true;
  ctr_a = ctr_d = 0;
  ain->start();
  dbg("videoresp: started");
}

void VideoResp::aDataAv(AnalogIn *ai, int nscans) {
  //fprintf(stderr,"videoresp(%p)::adataav(%p): complete=%i active=%i. nscans=%i ctr_a=%i\n",this,ai,complete,active, nscans, ctr_a);
  if (complete || !active)
    return;
  adata->reshape(nscans,1);
  nscans = ain->read(adata);
  ctr_a += nscans;
}

void VideoResp::dDataAv(DigitalIn *di, int nscans) {
  //fprintf(stderr,"videoresp(%p)::ddataav(%p): complete=%i active=%i. nscans=%i ctr_d=%i\n",this,di,complete,active,nscans,ctr_d);
  if (complete || !active)
    return;
  ddata->reshape(nscans);
  nscans = din->read(ddata);
  ctr_d += nscans;

  if (complete || !active)
    return;
 
  uint32_t *dd = ddata->allData();
  // fprintf(stderr,"videoresp:dataav. nscans=%i dd[0]=%08x\n",nscans,dd[0]);
  // fflush(stderr);
  // uint32_t orr=0;
  for (int k=0; k<nscans; k++) {
    //    orr |= *dd;
    bool pulsenow = (*dd++ & dmask) ? true : false;
    //    ctr_k++;
    if (inpulse && !pulsenow) {
      inpulse = false;
    } else if (!inpulse && pulsenow) {
      //      fprintf(stderr,"pulse at %i\n",ctr_k);
      if (ilastpulse>=STARTSCANS-TOLERANCE &&
	  ilastpulse<=STARTSCANS+TOLERANCE) {
	// wakeup bit
	wakeups++;
	//fprintf(stderr,"-> wakeup n=%i inmsg=%i bytes=%i inbyte=%i bit=%i byte=0x%02x ilast=%i checksum=%08x\n",
	// 	wakeups,inmsg,bytes,inbyte,bit,byte,ilastpulse,checksum);
	if (wakeups>=MINSTARTWAKEUPS) {
	  if ((inmsg && bytes>0) || (inbyte && bit>0)) {
	    complete = error = true; // return statBitError;
	    return;
	  }
	  inmsg = inbyte = true;
	  bit = 0;
	  bytes = 0;
	  byte = 0;
	  checksum = 0;
	} else if (inmsg && wakeups>=MININTERWAKEUPS) {
	  if (inbyte && bit>0) {
	    complete = error = true; // return statBitError;
	    return;
	  }
	  inbyte = true;
	  bit = 0;
	  byte = 0;
	}
	if (inmsg) {
	  if (bit==8) {
	    if (bytes<MAXMSGLEN) {
	      msg[bytes++] = byte;
	      if (bytes<msg[0]+2) {
		checksum = 0x17b*checksum + byte + (checksum>>11);
	      } else {
		complete = true;
		return;
	      }
	    } else {
	      complete = error = true; // return statOverflowError;
	      return;
	    }
	    inbyte = false;
	  } else if (bit>0) {
	    complete = error = true; // return statBitError;
	    return;
	  }
	}
      } else if (ilastpulse>=ZEROSCANS-TOLERANCE &&
		 ilastpulse<=ZEROSCANS+TOLERANCE) {
	// zero bit
	// fprintf(stderr,"-> zero bit inmsg=%i bytes=%i inbyte=%i bit=%i byte=%02x ilastpulse=%i\n",
	//         inmsg,bytes,inbyte,bit,byte,ilastpulse);
	bit++;
	wakeups=0;
      } else if (ilastpulse>=ONESCANS-TOLERANCE &&
		 ilastpulse<=ONESCANS+TOLERANCE) {
	// one bit
	// fprintf(stderr,"-> one bit inmsg=%i bytes=%i inbyte=%i bit=%i byte=%02x ilastpulse=%i\n",
	//          inmsg,bytes,inbyte,bit,byte,ilastpulse);
	byte += (1<<bit);
	bit++;
	wakeups = 0;
      } else if (ilastpulse<DANGERSCANS) {
	// bad interval
	complete = error = true; // return statBitError;
	return;
      } else {
	// long interval
	if (inmsg) {
	  complete = error = true; // return statBitError;
	  return;
	}
      }
      inpulse = true;
      ilastpulse = 0;
    }
    ilastpulse++;
  }
  //  fprintf(stderr,"  videoresp::ddataav: nscans=%i. ctr_k=%i orr=%08x\n",nscans,ctr_k,orr);
}

int VideoResp::retrieveMessage(unsigned char *dest) {
  if (!complete)
    return 0;
  complete = false;
  active = false;
  ain->stop();
  ain->uncommit();
  // delete din; din = 0;
  // delete ain; ain = 0;
  if (error)
    return -1;
  if (msg[bytes-1] != (checksum & 0xff))
    return -1;
  memcpy(dest,msg+1,bytes-2);
  return bytes-2;
}

void VideoResp::abort() {
  active = false;
  complete = false;
  ain->stop();
  ain->uncommit();
}

