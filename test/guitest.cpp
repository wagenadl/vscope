// guitest.cpp

#include <toplevel/vsdscopegui.h>
#include "gt_slots.h"
#include <QWidget>
#include <QApplication>

#include <xml/param.h>
#include <xml/paramtree.h>
#include <xml/enumerator.h>
#include <xml/connections.h>
#include <base/exception.h>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>

#include <acq/cameras.h>
#include <acq/trial.h>

void fail(QString x) {
  fprintf(stderr,"xmltest failed: %s\n",qPrintable(x));
  exit(2);
}

void readXML(QDomDocument &d, QString filename) {
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly))
    fail(QString("Cannot open " + filename));
  QString errmsg;
  int l,c;
  if (!d.setContent(&f,&errmsg,&l,&c)) {
    fprintf(stderr,"error in %s: %s at l. %i c. %i\n",
	    qPrintable(filename),qPrintable(errmsg),l,c);
    fail(QString("Cannot read ") + filename);
  }
  f.close();
}

int main(int argc, char **argv) {
  try {
    QApplication app(argc, argv);
    QWidget mw(0);
    mw.setGeometry(400,300,1024,768);
    
    QDomDocument parametersDoc("params");
    readXML(parametersDoc, "toplevel/parameters.xml");

    QDomDocument valuesDoc("values");
    readXML(valuesDoc, "toplevel/local.xml");

    QDomDocument configDoc("config");
    readXML(configDoc, "toplevel/config.xml");
    
    QDomElement pars = parametersDoc.documentElement();
    QDomElement vals = valuesDoc.documentElement();
    QDomElement conf = configDoc.documentElement();

    Enumerator::readAll(pars);
    ParamTree ptree(pars);
    ptree.read(vals);

    // Connections cons;
    // cons.read(vals);
    // 
    // Cameras cams(&cons);
    // 
    // Trial trial(&cons,&cams);
    // trial.prepare(&ptree);
    // trial.write("/tmp");
    
    vsdscopeGui gui(&mw,&ptree,conf);
    gt_slots sl;
    QObject::connect(&gui,SIGNAL(buttonClicked(QString,QString)),
		     &sl,SLOT(clicked(QString)));
    QObject::connect(&gui,SIGNAL(buttonSelected(QString,QString)),
		     &sl,SLOT(selected(QString)));
    QObject::connect(&gui,SIGNAL(buttonDeselected(QString,QString)),
		     &sl,SLOT(deselected(QString)));
    QObject::connect(&gui,SIGNAL(paramChanged(QString,QString)),
		     &sl,SLOT(paramchanged(QString,QString)));
    QObject::connect(&gui,SIGNAL(customValueChanged(QString,int,QString)),
		     &sl,SLOT(customchanged(QString,int,QString)));
    gui.open();
    mw.show();
    return app.exec();
  }  catch (Exception) {
    printf("Caught.\n");
  }
  return 0;
}

