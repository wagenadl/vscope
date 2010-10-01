// xmltest.cpp
 
#include <xml/param.h>
#include <xml/paramtree.h>
#include <base/exception.h>
#include <xml/enumerator.h>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QFile>
#include <stdio.h>
#include <stdlib.h>

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

void writeXML(QDomDocument &d, QString filename) {
  QFile f(filename);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    fail(QString("Cannot open ") + filename + " for writing");
  QString xml = d.toString(); 
  { QTextStream out(&f);
    out << xml;
  }
  f.close();
}
  
  
int main(int, char **) {
  QDomDocument parametersDoc("params");
  readXML(parametersDoc, "../toplevel/parameters.xml");
  QDomDocument valuesDoc("values");
  readXML(valuesDoc, "../toplevel/local.xml");

  try {
    QDomElement pars = parametersDoc.documentElement();
    QDomElement vals = valuesDoc.documentElement();

    Enumerator::readAll(pars);
    
    Param p(pars,"stimEphys/channel/trainPeriod");
    p.report();
    if (p.read(vals,"stimEphys/channel:DO0/trainPeriod"))
      p.report();
    else
      printf("cannot read 'stimEphys/channel:DO0/trainPeriod'\n");

    printf("\n----------------------------------\n\n");
   
    ParamTree ptree(pars);
    ptree.report();

    printf("\n----------------------------------\n\n");

    ptree.read(vals);
    ptree.report();

    printf("\n----------------------------------\n\n");

    ptree.write(vals);
    writeXML(valuesDoc,"local.xml");

  } catch (Exception) {
    printf("Caught.\n");
  }
    
  return 0;
}

