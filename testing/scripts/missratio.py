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
miss = 0
hit = 0
access = 0

def print_results():
    print "Misses: " + str(miss)
    print "Hits:   " + str(hit)
    print "Access: " + str(access)
    print "----------"
    print "Hit ratio: " + str(float(hit)/access)

def process_line(line):
    sline = line.split(":");
    global miss, hit, access
    if int(sline[OP_POS]) == PREFETCH_OPERATE_CODE:
        if int(sline[POC_CACHE_HIT]) > 0:
            hit = hit + 1
        else:
            miss = miss + 1
        access = access + 1
        

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
