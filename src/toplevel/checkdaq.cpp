// checkdaq.cpp

#include <QWidget>
#include <stdio.h>
#include <daq/daqbase.h>
#include <base/dbg.h>
#include <QLabel>

static QLabel *lbl = 0;

QString checkdaq() {
  QString vsn = DAQDevice::nidaqVersion();
  QString ltxt = "";
  QString confdev = "";
  DAQDevice *daqdev = &DAQDevice::find();
  if (DAQDevice::ok(daqdev)) {
    QString dev = daqdev->id();
    confdev = dev;

    ltxt = "Configured device: " + dev + "\n";
    ltxt += "  Device type: " + daqdev->typeName() + "\n"; 
    ltxt += "  Serial number: " + daqdev->serialNo() + "\n";
    ltxt += QString("  AI channel count: %1\n").arg(daqdev->nAIChannels());
    ltxt += QString("  AO channel count: %1\n").arg(daqdev->nAOChannels());
    ltxt += QString("  DIO line count: %1\n").arg(daqdev->nDIOLines());
    // Dbg() << ltxt;
  } else {
    ltxt = "Configured DAQ device not found\n";
  }
  ltxt += "\n";
  QStringList devs = DAQDevice::deviceList();

  if (confdev=="")
    ltxt += "Available devices:\n";
  else
    ltxt += "Other available devices:\n";
  
  bool haveany = false;
  for (QStringList::iterator i=devs.begin(); i!=devs.end(); ++i) {
    if (*i!=confdev) {
      try {
	DAQDevice &daqdev = DAQDevice::find(*i);
	QString typn = daqdev.typeName();
	int typc = daqdev.typeCode();
	QString serno = daqdev.serialNo();
	ltxt += "  " + typn + " (0x" + QString::number(typc,16) + ")";
	ltxt += " #" + serno + "\n";
      } catch(Exception const &) {
	ltxt += "Trouble device: " + *i + "\n";
      }
      haveany = true;
    }
  }
  if (!haveany)
    ltxt += "  none\n";

  ltxt += "\nNI-DAQmx version: " + vsn + "\n";
  return ltxt;
}

void checkdaq(QWidget *dest) {
  if (!lbl)
    lbl = new QLabel(dest);
  lbl->setGeometry(16,16,dest->width()-32, dest->height()-32);

  QString ltxt = checkdaq();
  lbl->setText(ltxt);
  Dbg() << "DAQ status: " << ltxt;
  lbl->show();
}
