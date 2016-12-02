// tests.h

#ifndef TESTS_H

#define TESTS_H

#include <QString>
#include <QList>
#include <base/dbg.h>
#include <base/exception.h>

extern void addTest(QString name, void (*testfn)());
extern void runTest(QString name);
extern void runAllTests();
extern QList<QString> testNames();

#endif
