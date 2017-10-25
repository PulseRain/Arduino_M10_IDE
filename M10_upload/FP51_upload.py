#! python3
###############################################################################
# Copyright (c) 2016, PulseRain Technology LLC 
#
# This program is distributed under a dual license: an open source license, 
# and a commercial license. 
# 
# The open source license under which this program is distributed is the 
# GNU Public License version 3 (GPLv3).
#
# And for those who want to use this program in ways that are incompatible
# with the GPLv3, PulseRain Technology LLC offers commercial license instead.
# Please contact PulseRain Technology LLC (www.pulserain.com) for more detail.
#
###############################################################################


import sys, getopt
import math, time

from OCD_8051 import OCD_8051
from ROM_Hex_Format import *
from time import sleep

print ("===============================================================================")
print ("# Copyright (c) 2017, PulseRain Technology LLC ")
print ("# FP51 Code Upload Utility, Version 1.2")

try:
      opts, args = getopt.getopt(sys.argv[1:],"vDp:c:P:C:I:o:b:U:",[])
except (getopt.GetoptError, err):
      print (str(err))
      sys.exit(2)


baud_rate = 115200
com_port = "COM5"
image_file = "sketch.eep"

for opt, args in opts:
    if opt in ('-b'): 
        baud_rate = int (args)
    elif opt in ('-P'):
        com_port = args
    elif opt in ('-U'):
        image_file = args
        
print ("===============================================================================")
print ("baud_rate  = ", baud_rate)
print ("com_port   = ", com_port)
print ("image file = ", image_file)
print ("===============================================================================")

try:
    ocd = OCD_8051 (com_port, baud_rate, verbose=0)
except:
    print ("Failed to open COM port")
    sys.exit(1)




class dummy_console:
    
#############################################################################
# command procedures
#############################################################################
    
    _DEBUG_COUNTER_INDEX_RESET = 1
    _DEBUG_COUNTER_INDEX_SET   = 2
    _TIME_COUNTER_INDEX_RESET  = 3
    _TIME_COUNTER_INDEX_SET    = 4  
    
    def _string_to_data (self, data_string):
        if (data_string.startswith('0x')):
            data = int(data_string[2:], 16)
        else:
            data = int(data_string)
        
        return data
   
    def _do_reset_cpu (self):
        self._ocd._serial.reset_output_buffer() 
        self._ocd._serial.reset_input_buffer()        
        
        self._ocd.cpu_reset()
        
        self._ocd._serial.reset_output_buffer() 
        self._ocd._serial.reset_input_buffer()        
        
    def _do_pause_cpu (self):
        self._ocd.cpu_pause (1, 1)

    def _do_resume_cpu (self):
        self._ocd.cpu_pause (0, 1)
    
    
    def _write_code (self, addr, data):
        offset = 0
        length = len (data)
        addr_end = addr + length
        
        if (addr % 4):
            for i in range (min([(4 - (addr % 4)), length])):
                self._ocd.code_mem_write_byte (addr + offset, data[i])
                offset = offset + 1
        
        total_words = (addr_end - addr - offset) // 4
        total_128byte_frame = total_words //32
        
        for i in range (total_128byte_frame):
            self._ocd.code_mem_write_128byte (addr + offset, data[offset : offset + 128])
            offset = offset + 128
            
        
        for i in range (total_words - total_128byte_frame * 32):
            data_int = (data[offset] << 24) + \
                       (data[offset + 1] << 16) + \
                       (data[offset + 2] << 8) + \
                       (data[offset + 3])
            
            ##print ("write32bit addr = ", addr + offset, "data_int=", hex(data_int))            
            self._ocd.code_mem_write_32bit(addr + offset, data_int)
            offset = offset + 4
        
        for i in range (length - offset):
            self._ocd.code_mem_write_byte (addr + offset, data [offset])
            offset = offset + 1


    
    def _do_load_hex_file (self):
        intel_hex_file =  Intel_Hex(self._args[1])
        
        if (len (intel_hex_file.data_record_list) == 0):
            return
            
        if (len(self._args) > 2):
            try:
                f = open(self._args[2], 'w')
            except IOError:
                print ("Fail to open: ", self._args[2])
                return
                
        #self._do_pause_cpu()
        #print ("CPU paused");
        #print ("CPU reset ...")
        #self._do_reset_cpu()        
        #sleep(0.5)
        print ("Loading...", self._args[1])
        
        last_addr = intel_hex_file.data_record_list[-2].address + len(intel_hex_file.data_record_list[-1].data_list)
        len_completed = 0
        
        address = 0
        merge_data_list = []
        
        start_time = time.clock()
        print_cnt = 0
        print ("Writing | ", end="")
        for record in intel_hex_file.data_record_list:
            #print ("xxxxaddr=", record.address, "data=", record.data_list)
            
            if ((print_cnt % 16) == 0):
                print("#", end="")
                sys.stdout.flush()
                
            print_cnt = print_cnt + 1
                
            if (len(merge_data_list) == 0):
                address = record.address
                merge_data_list = record.data_list
                #print ("YY addr = ", address, " ", len (merge_data_list))
            elif ((address + len (merge_data_list)) == record.address):
                merge_data_list = merge_data_list + record.data_list
                
                #print ("WW addr = ", address, " ", len (merge_data_list))
                #print (merge_data_list)
                
                
            else:
                #print ("XXXXXXXXXXXXXXX ", address, " ", len(merge_data_list))
                self._write_code (address, merge_data_list)
                #print ("YYYYYYYYYYYYYYYY")
                
                len_completed = len_completed + len(merge_data_list)
                
                load_progress = math.ceil(len_completed * 100 / last_addr);
                if (load_progress > 100):
                    load_progress = 100
                
                #print ("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", end="")            
                #print ("%d%% completed" % load_progress, end="")
                
                    
                print("#", end="")
                sys.stdout.flush()
                
                if (len(self._args) > 2):
                    f.write('addr %d\n' % (address))
                    
                    for item in merge_data_list:
                        f.write('%d\n' % (item))
                        
                address = record.address
                merge_data_list = record.data_list



                
        if (len(self._args) > 2):
            f.close()
        
        end_time = time.clock()
        delta_time = end_time - start_time
        print (" | 100% {0:0.2f}s".format(delta_time))        
        self._do_resume_cpu()
        print ("\nCPU reset ...")
        self._do_reset_cpu()        
        print ("Done: ", last_addr, " Byte(s)")
        print ("CPU is running")
    
    
    def _do_uart_select (self):
        self._ocd.uart_select (1 - self.uart_raw_mode_enable)
        
    def _do_uart_switch (self):
        self.uart_raw_mode_enable = 1 - self.uart_raw_mode_enable
        self._ocd._serial.reset_output_buffer()    
        self._ocd._serial.reset_input_buffer()
        self._do_uart_select()
        
        self._ocd._serial.write ([ord('\r')])
        sleep(0.5)
        if (self._ocd._serial.in_waiting):
            r = self._ocd._serial.read (self._ocd._serial.in_waiting)  
            prt_out = ""
            for i in r:
                if (i < 128):
                    prt_out = prt_out + chr(i) 
            #print (prt_out, end="")
            #sys.stdout.flush()    

        
    def _do_load_hex_and_switch (self):
        self._do_load_hex_file()
        self._do_uart_switch()
        
#############################################################################
# static variables
#############################################################################
    
   
   
       
#############################################################################
# Methods
#############################################################################
  
    def __init__ (self, ocd):
        self._ocd = ocd
        self.uart_raw_mode_enable = 0
        self._do_uart_select()
        
        
        
        if (ocd._serial.in_waiting):
            r = ocd._serial.read (ocd._serial.in_waiting) # clear the uart receive buffer 
        
        self._do_pause_cpu()
        print ("CPU paused");
        sleep(0.5)
        print ("CPU reset ...")
        self._do_reset_cpu()        
        sleep(0.5)
        
        self.uart_raw_mode_enable = 0
        self._do_uart_select()
        
        
console = dummy_console(ocd)

console._args = ("load_hex_and_switch " + image_file).split()
console._do_load_hex_and_switch ()

#for k in sys.argv[1:]:
#    if k.startswith("-U"):
#        U = k[2:].split(':')
#        file_name = U[2] + ":" + U[3]
#        print (file_name)
#    elif k.startswith("-b"):
#        baud_rate = int(k[2:])
#        print ("baud_rate = ", baud_rate)
        
#for o, a in opts:
#    print (o)
#    if (o == "-U") :
#        print (a)
       
