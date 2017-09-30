// toplevel/checkdaq.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// checkdaq.cpp

#include <QWidget>
#include <stdio.h>
#include <daq/daqbase.h>
#include <base/dbg.h>
#include <QLabel>
#include <xml/connections.h>

static QLabel *lbl = 0;

QString checkdaq() {
  QString vsn = DAQDevice::nidaqVersion();
  QString ltxt = "";
  QString confdev = Connections::deviceID();
  if (confdev=="") {
    QString confser = Connections::deviceSerNo();
    QString conftyp = Connections::deviceType();
    confdev = DAQDevice::search(conftyp, confser);
  }
  DAQDevice *daqdev = &DAQDevice::find(confdev);
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
