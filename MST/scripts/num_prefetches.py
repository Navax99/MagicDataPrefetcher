#!/usr/bin/python

import sys
import os.path

#codes
INITIALIZE_CODE = 0
PREFETCH_LINE_CODE =  1
PREFETCH_OPERATE_CODE =  2
CACHE_FILL_CODE = 3
FINAL_STATS_CODE = 4

#array location
OP_POS = 1
POC_CACHE_HIT = 5

#variables
num_prefetches = 0

def print_results():
    print "Number of prefetches: " + str(num_prefetches)

def process_line(line):
    sline = line.split(":");
    global num_prefetches
    if int(sline[OP_POS]) == PREFETCH_LINE_CODE:
        num_prefetches  = num_prefetches +1
        

def main(args):
    if len(args) == 0:
        print "No trace provided"
        sys.exit(-1)

    #check path existances
    if not os.path.isfile(args[0]):
        print "File doesnt exist"
        
    #read the data
    with open(args[0],"r") as lines:
        for line in lines:
            process_line(line)

    #print results
    print_results()

if __name__ == "__main__":
    main(sys.argv[1:])
