// traceinfo.cpp

#include <gfx/traceinfo.h>
#include <base/dbg.h>

#include <algorithm>
#include <math.h>
#include <QVector>

template <class X> Range ti_minmax(X const*xx, int N, int step) {
  // For arbitrary data type X, returns the min and max of N elements

  if (N==0 || !xx) 
    return Range();

  X min=xx[0];
  X max=xx[0];
  X const *yy = xx+step;
  for (int n=1; n<N; n++) {
    if (*yy<min)
      min=*yy;
    else if (*yy>max)
      max=*yy;
    yy+=step;
  }
  return Range(min,max);
}

template <class X> Range ti_minmax99(X const*xx, int N, int step,
				     double frc0, double frc1) {
  // For arbitrary data type X, returns the frc0-th and frc1-th fractiles
  // of N elements.
  // Note that this is slower than ti_minmax, since it needs to copy the
  // data for the purpose of nth_element. Still, it is linear time.
  if (N<=0 || !xx)
    return Range();

  int n0=int(floor(frc0*N));
  if (n0<0)
    n0=0;
  else if (n0>=N)
    n0=N-1;

  if (frc1<=0)
    frc1=1-frc0;
  int n1=int(floor(frc1*N));
  if (n1<0)
    n1=0;
  else if (n1>=N)
    n1=N-1;

  if (n0==0 && n1==N-1)
    return ti_minmax(xx, N, step);

  QVector<X> yy(N);
  X *yyp = yy.data();
  for (int n=0; n<N; n++) {
    *yyp++ = *xx;
    xx+=step;
  }

  yyp = yy.data();
  std::nth_element(yyp, yyp+n0, yyp+N);
  double x0 = yy[n0];
  std::nth_element(yyp, yyp+n1, yyp+N);
  double x1 = yy[n1];

  return Range(x0, x1);
}  

template <class X> void ti_trueblue(X const *data,
				    double datax0, double datadx,
				    int dataN, int step,
				    double x0, double dx, int M,
				    double *outmin, double *outmax) {
  if (dataN==0 || !data) {
    for (int m=0; m<M; m++) {
      outmin[m]=Numbers::inf();
      outmax[m]=-Numbers::inf();
    }
  } else {
    for (int m=0; m<M; m++) {
      int i0=int(floor(((x0+m*dx)-datax0) / datadx));
      if (i0>=dataN || i0<0) {
	outmin[m]=1;//data[(dataN-1)*step];
	outmax[m]=-1;//data[(dataN-1)*step];
      } else {
	int i1=int(floor(((x0+(m+1)*dx)-datax0) / datadx));
	if (i1>dataN)
	  i1=dataN;
	X const *dp = data+i0*step;
	X min = *dp;
	X max = *dp;
	for (int i=i0+1; i<i1; i++) {
	  dp+=step;
	  if (*dp<min)
	    min=*dp;
	  if (*dp>max)
	    max=*dp;
	}
	outmin[m]=min;
	outmax[m]=max;
      }
    }
  }
}

void ti_trueblue_binary(uint32_t const *data, uint32_t bit,
			double datax0, double datadx, int dataN, int step,
			double x0, double dx, int M,
			double *outmin, double *outmax) {
  uint32_t one = 1;
  uint32_t mask = one<<bit;

  if (dataN==0 || !data) {
    for (int m=0; m<M; m++) {
      outmin[m]=Numbers::inf();
      outmax[m]=-Numbers::inf();
    }
  } else {
    for (int m=0; m<M; m++) {
      int i0=int(floor(((x0+m*dx)-datax0) / datadx));
      if (i0>=dataN) {
	double v = (data[(dataN-1)*step]&mask) ? 1 : 0;
	outmin[m]=outmax[m]=v;
      } else {
	if (i0<0)
	  i0=0;
	int i1=int(floor(((x0+(m+1)*dx)-datax0) / datadx));
	if (i1>dataN)
	  i1=dataN;
	uint32_t const *dp = data+i0*step;
	double min = (*dp&mask) ? 1 : 0;
	double max = min;
	for (int i=i0+1; i<i1; i++) {
	  dp+=step;
	  double v = (*dp&mask) ? 1 : 0;
	  if (v<min)
	    min=v;
	  if (v>max)
	    max=v;
	}
	outmin[m]=min;
	outmax[m]=max;
      }
    }
  }
}

TraceInfo::TraceInfo() {
  datax0=0;
  datadx=1;
  N=0;
  step=1;
  scale=1;
  offset=0;
}

void TraceInfo::report() const {
  Dbg() << "TraceInfo("<<this<<")::report"
  	<<" x0="<<datax0<< " dx="<<datadx
  	<<" dp.ptr="<<dataptr.ptr.dp_none<<" dp.typ="<<int(dataptr.typ)
  	<<" N="<<N<<" step="<<step;
  if (N) {
    switch (dataptr.typ) {
    case DataPtr::dataDouble: {
      double mn=dataptr.ptr.dp_double[0];
      double mx=dataptr.ptr.dp_double[0];
      double sumx=0;
      double sumxx=0;
      for (int n=0; n<N; n++) {
	double x = dataptr.ptr.dp_double[n*step];
	if (x<mn)
	  mn=x;
	if (x>mx)
	  mx=x;
	sumx+=x;
	sumxx+=x*x;
      }
      Dbg() << " min:"<<mn<<" max:"<<mx<<" avg:"<<sumx/N<< " var:"<<(sumxx-sumx*sumx/N)/N;
    } break;
    default:
      break;
    }
  }
}

void TraceInfo::setData(double datax0_, double datadx_,
			DataPtr dp_, int N_, int step_) {
  datax0 = datax0_;
  datadx = datadx_;
  dataptr = dp_;
  N = dataptr.ptr.dp_none ? N_ : 0;
  step = step_;
}

void TraceInfo::setScaleFactor(double sf) {
  scale = sf;
}

void TraceInfo::setOffset(double of) {
  offset = of;
}

Range TraceInfo::range() const {
  Range r;
  switch (dataptr.typ) {
  case DataPtr::dataNone:
    return r;
  case DataPtr::dataInt8:
    r = ti_minmax(dataptr.ptr.dp_int8, N, step);
    break;
  case DataPtr::dataUInt8:
    r = ti_minmax(dataptr.ptr.dp_uint8, N, step);
    break;
  case DataPtr::dataInt16:
    r =  ti_minmax(dataptr.ptr.dp_int16, N, step);
    break;
  case DataPtr::dataUInt16:
    r = ti_minmax(dataptr.ptr.dp_uint16, N, step);
    break;
  case DataPtr::dataInt32:
    r = ti_minmax(dataptr.ptr.dp_int32, N, step);
    break;
  case DataPtr::dataUInt32:
    r = ti_minmax(dataptr.ptr.dp_uint32, N, step);
    break;
  case DataPtr::dataFloat:
    r = ti_minmax(dataptr.ptr.dp_float, N, step);
    break;
  case DataPtr::dataDouble:
    r = ti_minmax(dataptr.ptr.dp_double, N, step);
    break;
  case DataPtr::dataBinary:
    r = Range(0,1);
    break;
  default:
    fprintf(stderr,"Warning: TraceInfo::range: unknown data type\n");
    return r;
  }
  r.mul(scale);
  r.add(offset);
  return r;
}

Range TraceInfo::zoomrange99(double x0, double x1,
			     double frc0, double frc1) const {
  if (N==0 || datadx==0)
    return Range();
  int n0 = int((x0-datax0)/datadx);
  int n1 = int((x1-datax0)/datadx);
  if (n0<0)
    n0=0;
  else if (n0>N)
    n0=N;
  if (N>=50 && n0<3 && n1>3)
    n0 = 3;
  if (n1>N)
    n1=N;
  else if (n1<n0)
    n1=n0;
  if (N>=50 && n1>N-3 && n0<N-3)
    n1 = N-3;
  return range99(n0,n1,frc0,frc1);
}

Range TraceInfo::range99(double frc0, double frc1) const {
  //  dbg("TraceInfo(%p)::range99(%g,%g) dp=%p\n      datax0=%hg datadx=%hg dataN=%i step=%i",
  //    this,frc0,frc1,dataptr.dp_none, datax0,datadx,N,step);
  if (N==0 || datadx==0)
    return Range();
  return range99(0,N,frc0,frc1);
}

double TraceInfo::getDatum(int n) const {
  if (!dataptr.ptr.dp_none) {
    //Dbg() << "getDatum on null data";
    return 0;
  }
  if (n<0 || n>=N)
    return 0; // really might throw an exception?
  switch (dataptr.typ) {
  case DataPtr::dataNone:
    return 0;
  case DataPtr::dataInt8:
    return dataptr.ptr.dp_int8[n*step]*scale+offset;
  case DataPtr::dataUInt8:
    return dataptr.ptr.dp_uint8[n*step]*scale+offset;
  case DataPtr::dataInt16:
    return dataptr.ptr.dp_int16[n*step]*scale+offset;
  case DataPtr::dataUInt16:
    return dataptr.ptr.dp_uint16[n*step]*scale+offset;
  case DataPtr::dataInt32:
    return dataptr.ptr.dp_int32[n*step]*scale+offset;
  case DataPtr::dataUInt32:
    return dataptr.ptr.dp_uint32[n*step]*scale+offset;
  case DataPtr::dataFloat:
    return dataptr.ptr.dp_float[n*step]*scale+offset;
  case DataPtr::dataDouble:
    return dataptr.ptr.dp_double[n*step]*scale+offset;
  case DataPtr::dataBinary: {
    uint32_t one = 1;
    uint32_t msk = one<<dataptr.bit;
    return (dataptr.ptr.dp_binary[n*step] & msk) ? (scale+offset) : offset; }
  default:
    return 0;
  }
}
    
    
Range TraceInfo::range99(int n0, int n1, double frc0, double frc1) const {
  Range r;
  if (n1<=n0)
    return r;
  switch (dataptr.typ) {
  case DataPtr::dataNone:
    return r;
  case DataPtr::dataInt8:
    r = ti_minmax99(dataptr.ptr.dp_int8+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataUInt8:
    r = ti_minmax99(dataptr.ptr.dp_uint8+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataInt16:
    r = ti_minmax99(dataptr.ptr.dp_int16+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataUInt16:
    r = ti_minmax99(dataptr.ptr.dp_uint16+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataInt32:
    r = ti_minmax99(dataptr.ptr.dp_int32+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataUInt32:
    r = ti_minmax99(dataptr.ptr.dp_uint32+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataFloat:
    r = ti_minmax99(dataptr.ptr.dp_float+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataDouble:
    r = ti_minmax99(dataptr.ptr.dp_double+n0*step, n1-n0, step, frc0, frc1);
    break;
  case DataPtr::dataBinary:
    r = Range(0,1);
  default:
    fprintf(stderr,"Warning: TraceInfo::range99: unknown data type\n");
    return r;
  }
  r.mul(scale);
  r.add(offset);
  return r;
}

void TraceInfo::trueBlue(double x0, double dx, int M,
			 QVector<double> &outmin,
                         QVector<double> &outmax) const {
  if (outmin.size() != M)
    outmin.resize(M);
  if (outmax.size() != M)
    outmax.resize(M);
  trueBlue(x0, dx, M, outmin.data(), outmax.data());
}
  


void TraceInfo::trueBlue(double x0, double dx, int M,
			 double *outmin, double *outmax) const {
  //dbg("TraceInfo(%p)::trueblue dp=%p\n      datax0=%hg datadx=%hg dataN=%i step=%i\n      x0=%hg dx=%hg M=%i",
  //    this,dataptr.dp_none,datax0,datadx,N,step, x0,dx,M);

  switch (dataptr.typ) {
    break;
  case DataPtr::dataNone:
    for (int m=0; m<M; m++) {
      outmin[m]=Numbers::inf();
      outmax[m]=-Numbers::inf();
    }
    return;
  case DataPtr::dataInt8:
    ti_trueblue(dataptr.ptr.dp_int8, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataUInt8:
    ti_trueblue(dataptr.ptr.dp_uint8, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataInt16:
    ti_trueblue(dataptr.ptr.dp_int16, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataUInt16:
    ti_trueblue(dataptr.ptr.dp_uint16, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataInt32:
    ti_trueblue(dataptr.ptr.dp_int32, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataUInt32:
    ti_trueblue(dataptr.ptr.dp_uint32, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataFloat:
    ti_trueblue(dataptr.ptr.dp_float, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataDouble:
    ti_trueblue(dataptr.ptr.dp_double, datax0, datadx, N, step,
		x0, dx, M, outmin, outmax);
    break;
  case DataPtr::dataBinary:
    ti_trueblue_binary(dataptr.ptr.dp_binary, dataptr.bit,
		       datax0, datadx, N, step,
		       x0, dx, M, outmin, outmax);
    break;
  default:
    fprintf(stderr,"Warning: TraceInfo::trueBlue: unknown data type\n");
    for (int m=0; m<M; m++) {
      outmin[m]=Numbers::inf();
      outmax[m]=-Numbers::inf();
    }
    return;
  }
  if (scale!=1) {
    if (scale>0) {
      for (int m=0; m<M; m++) {
	outmax[m]*=scale;
	outmin[m]*=scale;
      }
    } else {
      for (int m=0; m<M; m++) {
	double o=outmin[m];
	outmin[m]=outmax[m]*scale;
	outmax[m]=o*scale;
      }
    }
  }
  if (offset!=0) {
    for (int m=0; m<M; m++) {
      outmax[m]+=offset;
      outmin[m]+=offset;
    }
  }
}
