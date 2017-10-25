/*
###############################################################################
# Copyright (c) 2016, PulseRain Technology LLC 
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License (LGPL) as 
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
# or FITNESS FOR A PARTICULAR PURPOSE.  
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################
*/

#ifndef COMMON_TYPE_H
#define COMMON_TYPE_H

#include "debug.h"

/*
typedef unsigned long       U32;
typedef unsigned short      U16;
typedef unsigned char       U8;
typedef signed char         S8;
typedef long                S32;

C_ASSERT(sizeof(S32) == 4);
C_ASSERT(sizeof(U32) == 4);
C_ASSERT(sizeof(U16) == 2);
C_ASSERT(sizeof(U8) == 1);
C_ASSERT(sizeof(S8) == 1);
*/

typedef unsigned long       uint32_t;
typedef long                int32_t;

typedef unsigned short      uint16_t;
typedef short               int16_t;

typedef unsigned char       uint8_t;
typedef signed char         int8_t;

C_ASSERT(sizeof(uint32_t) == 4);
C_ASSERT(sizeof(int32_t) == 4);

C_ASSERT(sizeof(uint16_t) == 2);
C_ASSERT(sizeof(int16_t) == 2);

C_ASSERT(sizeof(uint8_t) == 1);
C_ASSERT(sizeof(int8_t) == 1);

typedef uint8_t byte;
typedef uint16_t word;


#endif
