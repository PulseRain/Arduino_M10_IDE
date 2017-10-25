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

#ifndef _ARDUINO_H
#define _ARDUINO_H

#include "8051.h"

#include "debug.h"
#include "common_type.h"
#include "peripherals.h"


//============================================================================================
// Macro to obtain the number of arguments in __VA_ARGS__
// 
// References:
//  http://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments
//
//============================================================================================

#define PP_NARG(...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N())

#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)

#define PP_ARG_N(_1, _2, _3, _4, N,...) N

#define PP_RSEQ_N() 4,3,2,1,0



//============================================================================================
// Macro to do if/else in C pre-Processor
//
// References:
//  C Pre-Processor Magic, by Jonathan Heathcote, http://jhnet.co.uk/articles/cpp_magic
//============================================================================================


#define CAT(a,b) a ## b

#define IF_ELSE(condition) CAT(_IF_ , condition)


#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_2(...)             _IF_2_ELSE

#define _IF_1_ELSE(...)
#define _IF_2_ELSE(...) __VA_ARGS__


//============================================================================================
// Constant definition
//============================================================================================

#define INPUT  0
#define OUTPUT 1

#define HIGH 1
#define LOW 0

#define DEC 0
#define BIN 1
#define OCT 2
#define HEX 3

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif



//============================================================================================
// Interrupt Index
//============================================================================================
#define INT0_INT_INDEX 0
#define TIMER0_INT_INDEX 1
#define INT1_I2C_INT_INDEX 2
#define TIMER1_INT_INDEX 3
#define ADC_INT_INDEX   5
#define CODEC_INT_INDEX 6

typedef struct {
   void (*begin) (uint32_t); 
   uint8_t (*available)();
   void (*_print) (int32_t num, uint8_t fmt) __reentrant;
   void (*printHex) (uint32_t num);
   void (*_println) (int32_t data, uint8_t fmt) __reentrant;
   void (*writeByte) (uint8_t data);
   
   uint8_t   (*read)();
   uint8_t   (*readBytes) (uint8_t* buf, uint16_t length) __reentrant;
   
   void (*_write) (uint8_t* buf, uint16_t length) __reentrant;
   void (*setTimeout)(uint32_t time_out_in_ms);
   uint8_t readLn(uint8_t* buf, uint16_t max_length) __reentrant;
   void (*end)();   
   
} SERIAL_STRUCT;

#define print(...) IF_ELSE(PP_NARG(__VA_ARGS__))(_print( __VA_ARGS__ , DEC ))(_print( __VA_ARGS__ )) 
#define println(...) IF_ELSE(PP_NARG(__VA_ARGS__))(_println( __VA_ARGS__ , DEC ))(_println( __VA_ARGS__ )) 

#define write(...) IF_ELSE(PP_NARG(__VA_ARGS__))(_write( __VA_ARGS__ , 0 ))(_write( __VA_ARGS__ )) 


extern void pinMode (uint8_t pin, uint8_t mode);
extern void digitalWrite (uint8_t pin, uint8_t value);
extern uint8_t digitalRead (uint8_t pin);

extern uint8_t digital_to_ascii (uint8_t num);


extern const SERIAL_STRUCT Serial;

extern void delay (uint32_t delay_in_ms);
extern void delayMicroseconds (uint32_t delay_in_us);



extern void setup(void);
extern void loop(void);

extern void timer1_isr (void) __interrupt (TIMER1_INT_INDEX);
extern void dog_kick();

extern uint32_t millis ();
extern uint32_t micros ();


extern void single_nop_delay();
extern void nop_delay(uint8_t num);

extern void __int1_i2c__isr (void) __interrupt (INT1_I2C_INT_INDEX);
extern void __adc_isr (void) __interrupt (ADC_INT_INDEX);
extern void __codec_isr (void) __interrupt (CODEC_INT_INDEX);
extern void __int0_isr (void) __interrupt (INT0_INT_INDEX);
extern void __timer0_isr (void) __interrupt (TIMER0_INT_INDEX);


extern void interrupts();
extern void noInterrupts();

extern void attachIsrHandler(uint8_t index,  void (*isr_handler_pointer)());

#endif
