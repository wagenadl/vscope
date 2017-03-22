// base/types.h - This file is part of VScope.
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

// types.h

#ifndef TYPES_H
#define TYPES_H

/*:H types.h
 *:D Definitions of the most basic types: int8_t, int16_t, int32_t, int64_t and
     uint8_t, uint16_t, uint32_t, and uint64_t represent 8, 16, 32, and 64 
     bits signed and unsigned integers.
*/

#include <QString>

//#include <sys/types.h>
#include <stdint.h>
//typedef u_int8_t uint8_t;
//typedef u_int16_t uint16_t;
//typedef u_int32_t uint32_t;
//typedef u_int64_t uint64_t;

//#ifdef DEFINEMYTYPES
//typedef char int8_t;
//typedef unsigned char *uint8_t;
//typedef short int16_t;
//typedef unsigned short uint16_t;
//typedef int int32_t;
//typedef unsigned int uint32_t;
//typedef long long int64_t;
//typedef unsigned long long uint64_t;
//#endif

extern void checkTypes();
/*:F checkTypes
 *:D Checks that the types defined here really do have the sizes specified;
     throws an exception if any does not.
*/

#endif
