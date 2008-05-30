#!/usr/bin/env python
"""
	generates a table with 255 values with gamma
	correction = 2
"""

print "const uint8_t gammatable[] = {"

for i in range(1, 256):
	print str(int((i/255.0)**2*255))+",",

print "\n};"
