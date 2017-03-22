// base/tests.cpp - This file is part of VScope.
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

