// tests.cpp

#include "tests.h"
#include <QMap>

static QMap<QString, void (*)()> tests;

void addTest(QString name, void (*testfn)()) {
  tests[name] = testfn;
}

void runTest(QString name) {
  if (tests.contains(name)) {
    try {
      Dbg() << "Running test: " << name;
      (*tests[name])();
    } catch (Exception &) {
    };
  } else {
    Dbg() << "No test named " << name;
  }
}

void runAllTests() {
  foreach (QString t, tests.keys())
    runTest(t);
}

QList<QString> testNames() {
  return tests.keys();
}

