#!/usr/bin/env python

import sys, random

if len(sys.argv) < 3:
    print "usage: python generateur2.py memory_available number_of_repetitions"
    sys.exit(1)

memory_available = int(sys.argv[1])
repetitions = int(sys.argv[2])

M = memory_available * 64 - 1

for i in range(repetitions):
	x = random.randint(0, M)
	v = random.randint(0, 255)
	for j in range(x, x + 2 + 10 * random.randint(0,10)):
		print "write " + str(j % M) + " " + str(v % 256)
		v += 1
	
	x = random.randint(0, memory_available * 64 - 1)
	for j in range(x, x + 2 + 10 * random.randint(0,10)):
		print "read " + str(j % M)
