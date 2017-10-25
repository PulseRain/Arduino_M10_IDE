/*
###############################################################################
# Copyright (c) 2016, PulseRain Technology LLC 
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or 
# (at your option) any later version.
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

#include "8051.h"

#include "debug.h"
#include "common_type.h"
#include "peripherals.h"

#include "Arduino.h"
 


//----------------------------------------------------------------------------
// pinMode()
//
// Parameters:
//      pin   : pin index
//      mode  : INPUT or OUTPUT
//
// Return Value:
//      None
//
// Remarks:
//      function to set pin mode as INPUT or OUTPUT
//----------------------------------------------------------------------------

void pinMode (uint8_t pin, uint8_t mode)
{
    uint8_t port_index = (pin >> 3);
    uint8_t mask = 1 << (pin & 7);
    
    if (port_index == 0) {
        if (mode) {
            P0_DIRECTION = P0_DIRECTION | mask; 
        } else {
            P0_DIRECTION = P0_DIRECTION & (~mask); 
        }
    } else if (port_index == 1) {
        if (mode) {
            P1_DIRECTION = P1_DIRECTION | mask; 
        } else {
            P1_DIRECTION = P1_DIRECTION & (~mask); 
        }
    }        
} // End of pinMode()

//----------------------------------------------------------------------------
// digitalWrite()
//
// Parameters:
//      pin    : pin index
//      vaule  : value to be set on the pin (HIGH or LOW)
//
// Return Value:
//      None
//
// Remarks:
//      function to set value on the pin
//----------------------------------------------------------------------------

void digitalWrite (uint8_t pin, uint8_t value)
{
    uint8_t port_index = (pin >> 3);
    uint8_t mask = 1 << (pin & 7);
         
    if (port_index == 0) {
        if (value) {
            P0 = P0 | mask;
        } else {
            P0 = P0 & (~mask); 
        } 
    } else if (port_index == 1) {
        if (value) {
            P1 = P1 | mask;
        } else {
            P1 = P1 & (~mask); 
        } 
    }  

} // End of digitalWrite()

//----------------------------------------------------------------------------
// digitalRead()
//
// Parameters:
//      pin    : pin index
//
// Return Value:
//      current value (voltage high / low) on the pin
//
// Remarks:
//      function to read the current value on the pin
//----------------------------------------------------------------------------

uint8_t digitalRead (uint8_t pin)
{
    uint8_t port_index = (pin >> 3);
    uint8_t mask = 1 << (pin & 7);
    
    if (port_index == 0) {
        return ((P0 & mask) ? HIGH : LOW);
    } else if (port_index == 1) {
        return ((P1 & mask) ? HIGH : LOW);
    }
        
    return 0xFF;
} // End of digitalRead()


//----------------------------------------------------------------------------
// serial_putchar()
//
// Parameters:
//      c    : byte to write to the serial port
//
// Return Value:
//      None
//
// Remarks:
//      function to send a byte to the serial port
//----------------------------------------------------------------------------

static void serial_putchar (uint8_t c)
{
     REN = 0;
     SBUF = c;
     while (!TI);
     TI = 0;              
} // serial_putchar()


//----------------------------------------------------------------------------
// digital_to_ascii()
//
// Parameters:
//      num    :  a byte to convert to ascii 
//
// Return Value:
//      converted ascii code
//
// Remarks:
//      function to convert a byte number to its ascii code for display
//----------------------------------------------------------------------------
uint8_t digital_to_ascii (uint8_t num)
{
    if (num < 10) {
        return (num + '0');
    } else {
        return (num - 10 + 'A');
    }
} // End of digital_to_ascii()

//----------------------------------------------------------------------------
// serial_print_dec()
//
// Parameters:
//      num : a byte to be printed to the serial port as decimal number 
//            in ascii 
//
// Return Value:
//      None
//
// Remarks:
//      function to print a byte number to the serial port as decimal number
//      in ascii code
//----------------------------------------------------------------------------

static void serial_print_dec (int8_t num)
{
    uint8_t k;
    
    if (num < 0) {
        serial_putchar ('-');
        num = ~num;
        ++num;
    }
    
    k = num / 100;
    if (k) {
        serial_putchar (digital_to_ascii(k));
    }
    
    k = (num - k * 100) / 10;
    if (k) {
        serial_putchar (digital_to_ascii(k));
    }
    
    k = num % 10;
    serial_putchar (digital_to_ascii(k));
    
} // End of serial_print_dec()

//----------------------------------------------------------------------------
// serial_print_dec_S32()
//
// Parameters:
//      num : 32 bit signed number, to be printed to the serial port as 
//            signed decimal number in ascii 
//
// Return Value:
//      None
//
// Remarks:
//      function to print a signed 32 bit number to the serial port as 
//      decimal number in ascii code
//----------------------------------------------------------------------------

static void serial_print_dec_S32 (int32_t num)
{
    uint8_t i;
    uint8_t tmp [10];
    
    if (num < 0) {
        serial_putchar ('-');
        num = ~num;
        ++num;
    }
    
    for (i = 0; i < 10; ++i) {
        tmp[i] = num % 10;
        num /= 10;
        if (num == 0) {
            break;
        }
    } // End of for loop        

    do {
        serial_putchar (digital_to_ascii(tmp[i]));
    } while (i--);
    
    
    
} // End of serial_print_dec_S32()

//----------------------------------------------------------------------------
// serial_print_hex()
//
// Parameters:
//      num : 32 bit unsigned number, to be printed to the serial port as 
//            hex number in ascii 
//
// Return Value:
//      None
//
// Remarks:
//      function to print a 32 bit number to the serial port as a hex number
//      in ascii code
//----------------------------------------------------------------------------

static void serial_print_hex (uint32_t num)
{
    uint8_t i;
    uint8_t tmp [8];
    
    
    for (i = 0; i < 8; ++i) {
        tmp[i] = (uint8_t)(num & 0xF);
       // EA = 0;
        num = num / 16;
       // EA = 1;
       //  __asm__ ("nop");
       
        if (num == 0) {
            break;
        }
    } // End of for loop        
    
    
    do {
        serial_putchar (digital_to_ascii(tmp[i]));
    } while (i--);
        
} // End of serial_print_hex()

//----------------------------------------------------------------------------
// serial_print_oct()
//
// Parameters:
//      num : 32 bit unsigned number, to be printed to the serial port as 
//            octal number in ascii 
//
// Return Value:
//      None
//
// Remarks:
//      function to print a 32 bit number to the serial port as a octal 
//      number in ascii code
//----------------------------------------------------------------------------
static void serial_print_oct (uint32_t num)
{
    uint8_t i;
    uint8_t tmp [11];
    
    for (i = 0; i < 11; ++i) {
        tmp[i] = (uint8_t)(num & 0x7);
        num = num / 8;
        if (num == 0) {
            break;
        }
    } // End of for loop
    
    do {
        serial_putchar (digital_to_ascii(tmp[i]));
    } while (i--);
        
} // End of serial_print_oct()

//----------------------------------------------------------------------------
// serial_print_bin()
//
// Parameters:
//      num : 32 bit unsigned number, to be printed to the serial port as 
//            binary number in ascii 
//
// Return Value:
//      None
//
// Remarks:
//      function to print a 32 bit number to the serial port as a binary 
//      number in ascii code
//----------------------------------------------------------------------------

static void serial_print_bin (uint32_t num)
{
    uint8_t i;
    uint8_t tmp [32];
    
    for (i = 0; i < 32; ++i) {
        tmp[i] = (uint8_t)(num & 0x1);
        num = num / 2;
        if (num == 0) {
            break;
        }
    } // End of for loop        
    
    do {
        serial_putchar (digital_to_ascii(tmp[i]));
    } while (i--);
        
} // End of serial_print_bin()

//----------------------------------------------------------------------------
// serial_print_int()
//
// Parameters:
//      num : 32 bit number, to be printed to the serial port in ascii
//      fmt : print format,  BIN, HEX, OCT or DEC
//
// Return Value:
//      None
//
// Remarks:
//      function to print a 32 bit number to the serial port in ascii code
//----------------------------------------------------------------------------

static void serial_print_int (int32_t num, uint8_t fmt) __reentrant
{
    if (fmt == BIN) {
        serial_print_bin((uint32_t) num);
    } else if (fmt == HEX) {
        serial_print_hex((uint32_t) num);
    } else if (fmt == OCT) {
        serial_print_oct ((uint32_t) num);
    } else {
        serial_print_dec_S32 (num);    
    }
} // serial_print_int()

//----------------------------------------------------------------------------
// serial_receive()
//
// Parameters:
//      None
//
// Return Value:
//      byte received from the serial port
//
// Remarks:
//      function to receive a byte from the serial port, unblocked fashion
//----------------------------------------------------------------------------
static uint8_t serial_receive ()
{   
    uint8_t k;
   // REN = 1;
   // RI = 0;
  //  while (!RI);
    REN = 1;
    
    k = SBUF;
    RI = 0; 
    return (k);
    
} // End of serial_receive()

//----------------------------------------------------------------------------
// serial_blocking_receive()
//
// Parameters:
//      None
//
// Return Value:
//      byte received from the serial port
//
// Remarks:
//      function to receive a byte from the serial port, blocked fashion
//----------------------------------------------------------------------------

static uint8_t serial_blocking_receive ()
{   
    uint8_t k;

    RI = 0;
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
   
   
    REN = 1;
    
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    
    while (!RI);
 
    k = SBUF;
    RI = 0;
    
    return (k);
    
} // End of serial_blocking_receive()

//----------------------------------------------------------------------------
// serial_available()
//
// Parameters:
//      None
//
// Return Value:
//      1 if the serial port's receiving buffer is not empty
//      0 if the receiving buffer is empty
//
// Remarks:
//      function to check if the serial port's receiving buffer is empty 
//      or not
//----------------------------------------------------------------------------

static uint8_t serial_available()
{
   
    if (REN == 0) {
        REN = 1;
    }
        
    if (RI) {
        return 1;
    } else {
        return 0;
    }
} // End of serial_available()


//----------------------------------------------------------------------------
// delay()
//
// Parameters:
//      delay_in_ms : delay in millisecond
//
// Return Value:
//      None
//
// Remarks:
//      function to delay by milliseconds
//----------------------------------------------------------------------------
static uint32_t serial_rate_factor = 833; // 96e6 / 115200 

void delay (uint32_t delay_in_ms)
{
    uint8_t small_tick = 0;
    
    // uint32_t temp = delay_in_ms * 3125 / 6944;
    
    uint32_t temp = delay_in_ms * 375 / serial_rate_factor; // 96e3 / (833 * 256)
    
    while(temp) {
        TF1 = 0;
        while (!TF1){
          //  k = TCON;
          //  serial_print_hex (k);
          //  serial_putchar ('=');
          
        } // End of while loop
        ++small_tick;
        if (small_tick == 127) {
            --temp;
        }
    } // End of while loop
    
} // delay()

//----------------------------------------------------------------------------
// delayMicroseconds()
//
// Parameters:
//      delay_in_us : delay in microsecond
//
// Return Value:
//      None
//
// Remarks:
//      function to delay by microseconds
//----------------------------------------------------------------------------

void delayMicroseconds (uint32_t delay_in_us)
{
    uint8_t small_tick = 0;
    
    //uint32_t temp = delay_in_us * 25 / 55552;
    
    uint32_t temp = delay_in_us * 3 / (serial_rate_factor * 8); // 96e3 / (833 * 256 * 1000)
    
    while(temp) {
        TF1 = 0;
        while (!TF1){
          //  k = TCON;
          //  serial_print_hex (k);
          //  serial_putchar ('=');
          
        } // End of while loop
        ++small_tick;
        if (small_tick == 127) {
            --temp;
        }
    } // End of while loop   
    
} // End of delayMicroseconds()

//----------------------------------------------------------------------------
// serial_begin()
//
// Parameters:
//      rate : baud rate, set it to 115200
//
// Return Value:
//      None
//
// Remarks:
//      function to init the serial port
//----------------------------------------------------------------------------


static void serial_begin (uint32_t rate)
{
    uint32_t tmp;
    
    //================================
    // If CPU clock is 100MHz
    //================================
    // 100e6 / 115200 = 868
    // 65536 - 868 = 64668 = 0xFC9C
    //TL1 = 0x9C;
    //TH1 = 0xFC;
    
    //================================
    // If CPU clock is 96MHz
    //================================
    // 96e6 / 115200 = 833
    // 65536 - 833 = 64703 = 0xFCBF
    //TL1 = 0xBF;
    //TH1 = 0xFC;
        
    
    //TMOD = 0x10;
    //TR1 = 1;
    
    TR1 = 0;
    
    ET1 = 0;
    EA = 0;
    
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    __asm__ ("nop");
    
    serial_rate_factor = 96000000 / rate; 
    tmp = serial_rate_factor;
    tmp = 65536 - tmp;
    
    TL1 = (uint8_t)(tmp & 0xFF);
    TH1 = (uint8_t)((tmp >> 8) & 0xFF); 
    
    TMOD = 0x11;
    TR1 = 1;
    
    SCON = 0xC0;
    __asm__ ("nop");
    __asm__ ("nop");
    
    ET1 = 1;
    EA = 1;
    
} // End of serial_begin()


//----------------------------------------------------------------------------
// serial_end()
//
// Parameters:
//      None
//
// Return Value:
//      None
//
// Remarks:
//      function to close the serial port
//----------------------------------------------------------------------------
static void serial_end ()
{
    SCON = 0;
    __asm__ ("nop");
    __asm__ ("nop");
    
} // End of serial_end()

//----------------------------------------------------------------------------
// serial_printLn()
//
// Parameters:
//      data  : 32 bit data to be printed to the serial port
//      fmt : print format,  BIN, HEX, OCT or DEC
//
// Return Value:
//      None
//
// Remarks:
//      function to print a 32 bit number to the serial port in ascii code,
//      plus carriage return 
//----------------------------------------------------------------------------
static void serial_printLn(int32_t data, uint8_t fmt) __reentrant 
{
   serial_print_int (data, fmt);
  // serial_putchar ('\r');
   serial_putchar ('\n');
} // End of serial_printLn()

// timeout for the serial port
uint32_t serial_timeout = 0;

//----------------------------------------------------------------------------
// serial_set_timeout()
//
// Parameters:
//      time_out_in_ms  : timeout for the serial port, in millisecond
//
// Return Value:
//      None
//
// Remarks:
//      function to set the timeout value for the serial port 
//----------------------------------------------------------------------------

static void serial_set_timeout(uint32_t time_out_in_ms)
{
    serial_timeout = time_out_in_ms * 375 / 833; // 96e3 / (833 * 256)

} // End of serial_set_timeout()

//----------------------------------------------------------------------------
// serial_readLine()
//
// Parameters:
//      buf        : pointer to the data buffer
//      max_length : size of the data buffer
//
// Return Value:
//      the actual number of bytes valid in the data buffer
//
// Remarks:
//      function to read a line from the serial port 
//----------------------------------------------------------------------------

uint8_t serial_readLine (uint8_t* buf, uint8_t max_length)
{
    uint8_t count = 0;
    uint8_t c;
    
    while (max_length) {
         c = serial_receive();
         if (c == '\r') {
             (*buf++) = '\0';
             break;       
         }
         
         (*buf++) = c;
        --max_length;
        ++count;
    } // End of while loop
    return count;
} // serial_readLine()

//----------------------------------------------------------------------------
// serial_readLine_reentrant()
//
// Parameters:
//      buf        : pointer to the data buffer
//      max_length : size of the data buffer
//
// Return Value:
//      the actual number of bytes valid in the data buffer
//
// Remarks:
//      wrapper function for serial_readLine()
//----------------------------------------------------------------------------

uint8_t serial_readLine_reentrant(uint8_t* buf, uint16_t max_length) __reentrant
{
    return serial_readLine (buf, max_length);
    
} // End of serial_readLine_reentrant()

//----------------------------------------------------------------------------
// serial_readBytes()
//
// Parameters:
//      buf        : pointer to the data buffer
//      length     : the number of bytes to be read
//
// Return Value:
//      0x00 : successful
//      0xFF : timeout
//
// Remarks:
//      function to read designated number of bytes from the serial port
//----------------------------------------------------------------------------
static uint8_t serial_readBytes(uint8_t* buf, uint16_t length) 
{   
    uint8_t small_tick = 0;
    uint32_t temp = serial_timeout;
        
    if (temp == 0) {
        while (length) {
            (*buf++) = serial_blocking_receive();
            --length;
        } // End of while loop             
        
        return 0;
    } else {
        REN = 1;
        while (temp) {
            TF1 = 0;
           
            while (!TF1){
                if (RI) {
                    temp = serial_timeout;
                    
                    (*buf++) = SBUF;
                    RI = 0;
                 //   REN = 0;
                    if ((--length) == 0) {
                        REN = 0;
                        return 0;
                    }
                }
            } // End of while loop
            
            if (RI) {
                temp = serial_timeout;
                
                (*buf++) = SBUF;
                RI = 0;
                
                //REN = 0;
                if ((--length) == 0) {
                    REN = 0;
                    return 0;
                }
            }
                
            ++small_tick;
            if (small_tick == 127) {
                --temp;
            }
        } // End of timeout while loop
        REN = 0;
        return 0xff;
    }
} // End of serial_readBytes()

//----------------------------------------------------------------------------
// serial_readBytes_reentrant()
//
// Parameters:
//      buf        : pointer to the data buffer
//      length     : the number of bytes to be read
//
// Return Value:
//      0x00 : successful
//      0xFF : timeout
//
// Remarks:
//      wrapper function for serial_readBytes()
//----------------------------------------------------------------------------

static uint8_t serial_readBytes_reentrant(uint8_t* buf, uint16_t length) __reentrant
{
    return serial_readBytes (buf, length);
    
} // End of serial_readBytes_reentrant()


//----------------------------------------------------------------------------
// serial_write()
//
// Parameters:
//      buf        : pointer to the data buffer
//      length     : the number of bytes vaild in the buffer
//
// Return Value:
//      None
//
// Remarks:
//      function to write data buffer to the serial port
//----------------------------------------------------------------------------

static void serial_write (uint8_t* buf, uint16_t length)    
{
    if (length) {
        while (length) {
            serial_putchar ((*buf++));
            --length;
        } // End of while loop
    } else {
        while (*buf) {
            serial_putchar ((*buf++));
        } // End of while loop
    }
    
} // End of serial_write()

//----------------------------------------------------------------------------
// serial_write_reentrant()
//
// Parameters:
//      buf        : pointer to the data buffer
//      length     : the number of bytes vaild in the buffer
//
// Return Value:
//      None
//
// Remarks:
//      wrapper function for serial_write()
//----------------------------------------------------------------------------

static void serial_write_reentrant (uint8_t* buf, uint16_t length) __reentrant
{
    serial_write (buf, length);
    
} // End of serial_write_reentrant()

                              
//----------------------------------------------------------------------------
// dog_kick()
//
// Parameters:
//      None
//
// Return Value:
//      None
//
// Remarks:
//      watch dog kick
//----------------------------------------------------------------------------
                              
void dog_kick() 
{
    __asm__ ("mov 192, #1");
} // End of dog_kick()

// tick counter
uint8_t timer1_small_tick = 0;
uint32_t timer1_big_tick = 0;

//----------------------------------------------------------------------------
// timer1_isr()
//
// Parameters:
//      None
//
// Return Value:
//      None
//
// Remarks:
//      ISR for timer
//----------------------------------------------------------------------------

void timer1_isr (void) __interrupt (3)
{
   //== dog_kick();
    __asm__ ("mov 192, #1"); // direct dog kick 
    
    ++timer1_small_tick;
    if (timer1_small_tick == 127) {
        ++timer1_big_tick;
    }
    
    /*jtag_put_char (jtag_out_char);
    if (jtag_out_char == 'Z') {
        jtag_out_char = 'A';
        jtag_put_char ('\r');
        jtag_put_char ('\n');
    } else {
        ++jtag_out_char;
    }*/
} // End of timer1_isr()

//----------------------------------------------------------------------------
// millis()
//
// Parameters:
//      None
//
// Return Value:
//      number of milliseconds passed since reset
//
// Remarks:
//      function to keep track of time since reset
//----------------------------------------------------------------------------

uint32_t millis ()
{
    uint32_t temp;
    
    EA = 0;
    temp = timer1_big_tick;
    EA = 1;
    
    // num_of_tick * 868 * 256 /100e6 * 1000
    // num_of_tick * 868 * 256 /96e6 * 1000
    
    return ((temp * serial_rate_factor) / 375);
} // End of millis()

//----------------------------------------------------------------------------
// micros()
//
// Parameters:
//      None
//
// Return Value:
//      number of microseconds passed since reset
//
// Remarks:
//      function to keep track of time since reset
//----------------------------------------------------------------------------

uint32_t micros ()
{
    uint32_t temp;
    
    EA = 0;
    temp = timer1_big_tick;
    EA = 1;
    
    // num_of_tick * 868 * 256 /100e6 * 1e6
    // num_of_tick * 868 * 256 /96e6 * 1e6
    
    return ((temp * serial_rate_factor * 8) / 3);
} // End of micros()

//----------------------------------------------------------------------------
// Serial wrapper
//----------------------------------------------------------------------------
                           
const SERIAL_STRUCT Serial = {serial_begin, serial_available,
                              serial_print_int, serial_print_hex, serial_printLn,
                              serial_putchar, serial_receive, serial_readBytes_reentrant, 
                              serial_write_reentrant, serial_set_timeout, serial_readLine_reentrant, serial_end};
                        
void single_nop_delay()
{
     __asm__ ("nop");
} // End of single_nop_delay
            
            
void nop_delay(uint8_t num)
{
    uint8_t i;
    
    for (i = 0; i < num; ++i) {
       __asm__ ("nop");
    } // End of for loop
    
} // End of nop_delay

//----------------------------------------------------------------------------
// interrupts()
//
// Parameters:
//      None
//
// Return Value:
//      None
//
// Remarks:
//      function to enable interrupt
//----------------------------------------------------------------------------

void interrupts()
{
    EA = 1;
} // End of interrupts()

//----------------------------------------------------------------------------
// noInterrupts()
//
// Parameters:
//      None
//
// Return Value:
//      None
//
// Remarks:
//      function to disable interrupt
//----------------------------------------------------------------------------

void noInterrupts()
{
   EA = 0;
} // End of noInterrupts()


//----------------------------------------------------------------------------
// attachIsrHandler()
//
// Parameters:
//      index                     : IRQ index
//      codec_isr_handler_pointer : isr function pointer
// 
// Return Value:
//      None
//
// Remarks:
//      setup isr handler
//----------------------------------------------------------------------------
void (*codec_isr_handler_pointer)();
void (*adc_isr_handler_pointer)();
void (*int1_i2c_isr_handler_pointer)();
void (*int0_isr_handler_pointer)();
void (*timer0_isr_handler_pointer)();

void attachIsrHandler(uint8_t index,  void (*isr_handler_pointer)())
{
    if (index == ADC_INT_INDEX) {
        adc_isr_handler_pointer = isr_handler_pointer;
        
        if (isr_handler_pointer) {
            EADC = 1;
        } else {
            EADC = 0;
        }
        
    } else if (index == CODEC_INT_INDEX) {
        codec_isr_handler_pointer = isr_handler_pointer;
        if (isr_handler_pointer) {
            ECODEC = 1;
        } else {
            ECODEC = 0;
        }
    } else if (index == INT1_I2C_INT_INDEX) {
        int1_i2c_isr_handler_pointer = isr_handler_pointer;
        if (isr_handler_pointer) {
            EX1 = 1;
        } else {
            EX1 = 0;
        }
    } else if (index == INT0_INT_INDEX) {
        int0_isr_handler_pointer = isr_handler_pointer;
        if (isr_handler_pointer) {
            EX0 = 1;
        } else {
            EX0 = 0;
        }
    } else if (index == TIMER0_INT_INDEX) {
        timer0_isr_handler_pointer = isr_handler_pointer;
        if (isr_handler_pointer) {
            ET0 = 1;
        } else {
            ET0 = 0;
        }       
    }
} // End of attachISR()


//----------------------------------------------------------------------------
// codec_isr()
//
// Parameters:
//      None
//
// Return Value:
//      None
//
// Remarks:
//      ISR for CODEC
//----------------------------------------------------------------------------

void __codec_isr () __interrupt (CODEC_INT_INDEX) __naked
{
     __asm__ ("nop");
     __asm__ ("nop");
     __asm__ ("nop");
 
     __asm__ ("push psw");
     __asm__ ("push acc");
     __asm__ ("push b");
     __asm__ ("push dpl");
     __asm__ ("push dph");
     __asm__ ("push ar0");
     __asm__ ("push ar1");
     __asm__ ("push ar2");
     __asm__ ("push ar3");
     __asm__ ("push ar4");
     __asm__ ("push ar5");
     __asm__ ("push ar6");
     __asm__ ("push ar7");

    
      __asm__ ("nop");

      if (codec_isr_handler_pointer) {
            codec_isr_handler_pointer();
      }
      
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
      
      __asm__ ("pop ar7");
      __asm__ ("pop ar6");
      __asm__ ("pop ar5");
      __asm__ ("pop ar4");
      __asm__ ("pop ar3");
      __asm__ ("pop ar2");
      __asm__ ("pop ar1");
      __asm__ ("pop ar0");

      __asm__ ("pop dph");
      __asm__ ("pop dpl");
      __asm__ ("pop b");
      __asm__ ("pop acc");
      __asm__ ("pop psw");
     
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
            
      __asm__ ("reti");

} // End of codec_isr()


void __adc_isr (void) __interrupt (ADC_INT_INDEX) __naked
{
  
     __asm__ ("nop");
     __asm__ ("nop");
     __asm__ ("nop");
 
     __asm__ ("push psw");
     __asm__ ("push acc");
     __asm__ ("push b");
     __asm__ ("push dpl");
     __asm__ ("push dph");
     __asm__ ("push ar0");
     __asm__ ("push ar1");
     __asm__ ("push ar2");
     __asm__ ("push ar3");
     __asm__ ("push ar4");
     __asm__ ("push ar5");
     __asm__ ("push ar6");
     __asm__ ("push ar7");

    
      __asm__ ("nop");

      if (adc_isr_handler_pointer) {
            adc_isr_handler_pointer();
      }
      
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
      
      __asm__ ("pop ar7");
      __asm__ ("pop ar6");
      __asm__ ("pop ar5");
      __asm__ ("pop ar4");
      __asm__ ("pop ar3");
      __asm__ ("pop ar2");
      __asm__ ("pop ar1");
      __asm__ ("pop ar0");

      __asm__ ("pop dph");
      __asm__ ("pop dpl");
      __asm__ ("pop b");
      __asm__ ("pop acc");
      __asm__ ("pop psw");
     
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
            
      __asm__ ("reti");
  
    

} // End of adc_isr()



void __int1_i2c__isr (void) __interrupt (INT1_I2C_INT_INDEX) __naked
{
  
     __asm__ ("nop");
     __asm__ ("nop");
     __asm__ ("nop");
 
     __asm__ ("push psw");
     __asm__ ("push acc");
     __asm__ ("push b");
     __asm__ ("push dpl");
     __asm__ ("push dph");
     __asm__ ("push ar0");
     __asm__ ("push ar1");
     __asm__ ("push ar2");
     __asm__ ("push ar3");
     __asm__ ("push ar4");
     __asm__ ("push ar5");
     __asm__ ("push ar6");
     __asm__ ("push ar7");

    
      __asm__ ("nop");

      if (int1_i2c_isr_handler_pointer) {
            int1_i2c_isr_handler_pointer();
      }
      
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
      
      __asm__ ("pop ar7");
      __asm__ ("pop ar6");
      __asm__ ("pop ar5");
      __asm__ ("pop ar4");
      __asm__ ("pop ar3");
      __asm__ ("pop ar2");
      __asm__ ("pop ar1");
      __asm__ ("pop ar0");

      __asm__ ("pop dph");
      __asm__ ("pop dpl");
      __asm__ ("pop b");
      __asm__ ("pop acc");
      __asm__ ("pop psw");
     
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
            
      __asm__ ("reti");
  
    

} // End of __int1_i2c__isr()


void __int0_isr (void) __interrupt (INT0_INT_INDEX) __naked
{
  
     __asm__ ("nop");
     __asm__ ("nop");
     __asm__ ("nop");
 
     __asm__ ("push psw");
     __asm__ ("push acc");
     __asm__ ("push b");
     __asm__ ("push dpl");
     __asm__ ("push dph");
     __asm__ ("push ar0");
     __asm__ ("push ar1");
     __asm__ ("push ar2");
     __asm__ ("push ar3");
     __asm__ ("push ar4");
     __asm__ ("push ar5");
     __asm__ ("push ar6");
     __asm__ ("push ar7");

    
      __asm__ ("nop");

      if (int0_isr_handler_pointer) {
            int0_isr_handler_pointer();
      }
      
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
      
      __asm__ ("pop ar7");
      __asm__ ("pop ar6");
      __asm__ ("pop ar5");
      __asm__ ("pop ar4");
      __asm__ ("pop ar3");
      __asm__ ("pop ar2");
      __asm__ ("pop ar1");
      __asm__ ("pop ar0");

      __asm__ ("pop dph");
      __asm__ ("pop dpl");
      __asm__ ("pop b");
      __asm__ ("pop acc");
      __asm__ ("pop psw");
     
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
            
      __asm__ ("reti");
  
    

} // End of __int0_isr()

void __timer0_isr (void) __interrupt (TIMER0_INT_INDEX) __naked
{
  
     __asm__ ("nop");
     __asm__ ("nop");
     __asm__ ("nop");
 
     __asm__ ("push psw");
     __asm__ ("push acc");
     __asm__ ("push b");
     __asm__ ("push dpl");
     __asm__ ("push dph");
     __asm__ ("push ar0");
     __asm__ ("push ar1");
     __asm__ ("push ar2");
     __asm__ ("push ar3");
     __asm__ ("push ar4");
     __asm__ ("push ar5");
     __asm__ ("push ar6");
     __asm__ ("push ar7");

    
      __asm__ ("nop");

      if (timer0_isr_handler_pointer) {
            timer0_isr_handler_pointer();
      }
      
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
      
      __asm__ ("pop ar7");
      __asm__ ("pop ar6");
      __asm__ ("pop ar5");
      __asm__ ("pop ar4");
      __asm__ ("pop ar3");
      __asm__ ("pop ar2");
      __asm__ ("pop ar1");
      __asm__ ("pop ar0");

      __asm__ ("pop dph");
      __asm__ ("pop dpl");
      __asm__ ("pop b");
      __asm__ ("pop acc");
      __asm__ ("pop psw");
     
      __asm__ ("nop");
      __asm__ ("nop");
      __asm__ ("nop");
            
      __asm__ ("reti");
  
    

} // End of __timer0_isr()

