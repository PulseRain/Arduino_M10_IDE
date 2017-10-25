#!/usr/bin/python

import os
import sys
import re


if (len(sys.argv) < 2):
        print ("not enough parameters")
        os._exit(1)

#line = "CSEG                                00000067    0000185E =        6238. bytes (REL,CON,CODE)"
#result = re.match("^CSEG\s+\d+\s+\w+\s+=\s+(\d+)", line)

#line = "DSEG                                00000067    0000185E =        6238. bytes (REL,CON,CODE)"
#result2 = re.match("^CSEG", line)

#print (result)
#print (result.group(1))
#print (result2)

count = 0
cesg = "0000"
desg = "0000"
xseg = "0000"
xiseg = "0000"

code_size = 0
xdata_size = 0
prev_code_match = ""

with open(sys.argv[1]) as f:
    for line in f:
        count = count + 1
        
        result = re.match("^CSEG\s+\w+\s+\w+\s+=\s+(\d+)", line)
        if (result):
            cseg = result.group(1)
            #print ("count, cseg = ", cseg)
        
        result = re.match("^DSEG\s+\w+\s+\w+\s+=\s+(\d+)", line)
        if (result):
            dseg = result.group(1)
            #print ("count, dseg = ", dseg)
            
        result = re.match("^XSEG\s+\w+\s+\w+\s+=\s+(\d+)", line)
        if (result):
            xseg = result.group(1)
            #print ("count, xseg = ", xseg)
            
        result = re.match("^XISEG\s+\w+\s+\w+\s+=\s+(\d+)", line)
        if (result):
            xiseg = result.group(1)
            #print ("count, xseg = ", xseg)
            
        result = re.match("^\w+\s+\w+\s+\w+\s+=\s+(\d+)\.\s+bytes\s+\(REL\,CON\,CODE\)", line)
        if (result):
            if (prev_code_match != line):
                code_size = code_size + int(result.group(1))
            prev_code_match = '%s' % line 
            
        result = re.match("^\w+\s+\w+\s+\w+\s+=\s+(\d+)\.\s+bytes\s+\([\w|\,]+XDATA\)", line)
        if (result):
            xdata_size = xdata_size + int(result.group(1))
            
            
print (".data                      ", dseg, " 00000000")
print (".text                      ", code_size, " 00000000")
#print (".bss                       ", int(xiseg) + int(xseg), " 00000000")            
print (".bss                       ", xdata_size, " 00000000")            

#print ("code = ", code_size)
#print ("xdata = ", xdata_size)
