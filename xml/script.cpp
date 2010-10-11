// script.cpp

#include "script.h"
#include <xml/param.h>
#include <base/xml.h>
#include <base/dbg.h>
#include <math.h>

Script::Script() {
}

bool Script::setText(QString const &doc) {
  QStringList ss = doc.split("\n");
  lines.clear();
  int lineno=0;
  int lasttrial=-1;
  foreach (QString s, ss) {
    lineno++;
    s.replace(QRegExp("#.*"),"");
    s.replace(QRegExp("\\s+")," ");
    s.replace(QRegExp("^ "),"");
    s.replace(QRegExp(" $"),"");
    if (s.isEmpty())
      continue;
    QStringList bits = s.split(" ");
    QString kw = bits.takeFirst();

    Command c;
    
    if (kw=="set") {
      if (bits.size()<2) {
	err = QString("Syntax error at line %1: SET takes two arguments")
	  .arg(lineno);
	return false;
      }
      c.kwd = KW_SET;
      c.arg1 = bits.takeFirst();
      c.arg2 = bits.join(" ");
    } else if (kw=="trial") {
      c.kwd = KW_TRIAL;
      if (!bits.isEmpty()) {
	err = QString("Syntax error at line %1: TRIAL takes no arguments")
	  .arg(lineno);
	return false;
      }
      lasttrial = lineno;
    } else if (kw=="snap") {
      c.kwd = KW_SNAP;
      if (!bits.isEmpty()) {
	err = QString("Syntax error at line %1: SNAP takes no arguments")
	  .arg(lineno);
	return false;
      }
      lasttrial = lineno;
    } else if (kw=="ival") {
      c.kwd = KW_IVAL;
      QString arg = bits.join(" ");
      if (arg.isEmpty()) {
	err = QString("Syntax error at line %1: IVAL requires an argument")
	  .arg(lineno);
	return false;
      }
      Param p("time");
      try {
	p.set(arg);
	c.argv = p.toDouble();
      } catch (...) {
	err = QString("Illegal time specifation at line %1: '%2'")
	  .arg(lineno).arg(arg);
	return false;
      }
    } else if (kw=="loadsettings") {
      c.kwd = KW_LOADSETTINGS;
      if (bits.size()!=1) {
	err = QString("Syntax error at line %1: LOADSETTINGS takes one argument")
	  .arg(lineno);
	return false;
      }
      c.arg1 = bits.first();
    } else if (kw=="loop") {
      c.kwd = KW_LOOP;
      if (bits.size()>1) {
	err = QString("Syntax error at line %1: LOOP takes at most one argument")
	  .arg(lineno);
	return false;
      }
      if (bits.size()>=1)
	c.argv = floor(bits.takeFirst().toDouble(0));
      else
	c.argv = 1;
      if (c.argv<1 || c.argv>lasttrial) {
	err = QString("Illegal line number for LOOP at line %1: %2")
	  .arg(lineno).arg(c.argv);
	return false;
      }
    } else {
      err = QString("Unknown keyword at line %1: %2")
	.arg(lineno).arg(kw);
      return false;
    }
    lines.append(c);
  }
  return true;
}

QList<Script::Command> const &Script::contents() const {
  return lines;
}

QString Script::firstError() const {
  return err;
}

bool Script::ok(ParamTree &ptree0) {
  try {
    ParamTree ptree(ptree0);
    int lineno=0;
    foreach (Command c, lines) {
      lineno++;
      if (c.kwd==KW_SET) {
	Param *p = ptree.findp(c.arg1);
	if (!p) {
	  err = QString("Unknown parameter at line %1: %2")
	    .arg(lineno).arg(c.arg1);
	  return false;
	}
	try {
	  p->set(c.arg2);
	} catch (...) {
	  err = QString("Illegal value for %1 at line %2: %3")
	    .arg(c.arg1).arg(lineno).arg(c.arg2);
	  return false;
	}
      } else if (c.kwd==KW_LOADSETTINGS) {
	try {
	  XML xml(ptree.find("_filePath").toString()+"/_settings/" + c.arg1 + ".xml");
	  ptree.read(xml.root());
	} catch (...) {
	  err = QString("Cannot read settings file at line %1: %2")
	    .arg(lineno).arg(c.arg1);
	  return false;
	}
      } else if (c.kwd==KW_IVAL) {
	if (c.argv<1000) {
	  err = QString("Illegal interval at line %1: %2 ms is less than 1 s")
	    .arg(lineno).arg(c.argv);
	  return false;
	}
      }
    }
  } catch (...) {
    err = "Error initializing parameter tree";
    return false;
  }
  return true;
}
