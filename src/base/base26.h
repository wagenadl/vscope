// base26.h

#ifndef Base26_H
#define Base26_H

#include <QString>

extern QString num2az(int n);
/*:F num2az
 *:D Maps positive numbers to strings of letters:
       1 -> a
       2 -> b
       26 -> z
       27 -> aa
       28 -> ab
       etc.
 *:N Negative numbers or zero return "@".
 */
extern int az2num(QString s);
/*:F az2num
 *:D The inverse of num2az.
 *:N Bad numbers return -1.
 */

#endif
