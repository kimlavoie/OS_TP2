#!/usr/bin/env python

import sys

if len(sys.argv) < 2:
    print "usage: python generateur1.py memory_available"
    sys.exit(1)

memory_available = int(sys.argv[1])
    
for i in range(memory_available):
    print "write " + str(i*64) + " " + str(i)

for i in range(memory_available):
	print "read " + str(i*64)
