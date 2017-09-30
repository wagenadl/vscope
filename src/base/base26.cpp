// base/base26.cpp - This file is part of VScope.
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

// base26.cpp

#include <base/base26.h>

QString num2az(int n) {
  if (n<=0)
    return "@";

  n--;

  char buf[32];
  int L=1;
  int mul=26;
  while (n>=mul) {
    n -= mul;
    mul *= 26;
    L += 1;
  }
  // so now we know we're going to make a string with L letters

  for (int l=0; l<L; l++) {
    buf[L-l-1] = 'a' + (n%26);
    n /= 26;
  }
  buf[L]=0;

  return buf;
}

int az2num(QString s) {
  int L=s.length();
  int num=0;
  int mul=26;
  for (int l=1; l<L; l++) {
    num += mul;
    mul *= 26;
  }
  mul=1;
  for (int l=0; l<L; l++) {
    int digit = s[L-1-l].unicode() - 'a';
    if (digit<0 || digit>=26)
      return -1;
    num += mul*digit;
    mul *= 26;
  }
  return num+1;
}
  

#ifdef TEST
int main(int argc, char **argv) {
  for (int i=-10; i<1000; i++) {
    QString s = num2az(i);
    int j = az2num(s);
    printf("%i -> %s -> %i\n",i,s.toAscii().constData(),j);
  }

  return 0;
}
#endif
