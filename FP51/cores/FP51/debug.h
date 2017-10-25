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

#ifndef DEBUG_H
#define DEBUG_H

#define C_ASSERT(cond) \
    extern char compile_time_assertion[(cond) ? 1 : -1]



#define FIELD_OFFSET(type, field) \
        ((long)(long*)&(((type *)0)->field)) 
        

#define TYPE_ALIGNMENT( t ) \
        ((long)(sizeof(struct { char x; t test; }) - sizeof(t)))


        
extern void dog_kick();
        
#endif
