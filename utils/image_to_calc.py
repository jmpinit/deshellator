#!/usr/bin/env python

import sys, os
import serial
from PIL import Image

if not len(sys.argv) == 2:
    print sys.argv[0], "/path/to/image"
    sys.exit(1)

filepath = sys.argv[1]
im = Image.open(filepath)
rgb_im = im.convert('RGB')

width, height = im.size

if not width == 96 or not height == 64:
    print "Image wrong size. Must be 96x64."
    sys.exit(1)

bytes = []

for y in range(0, height):
    for byteIndex in range(0, width / 8):
        byte = 0

        for bit in range(0, 8):
            x = byteIndex * 8 + bit
            r, g, b = rgb_im.getpixel((x, y))
            v = (r + g + b) / 3.0

            if v < 128:
                byte |= 0x80 >> bit

        bytes += [byte]

ser = serial.Serial("/dev/ttyUSB0", 500000)

for byte in bytes:
    x = ser.write(chr(byte))

ser.close()

print "sent", len(bytes), "bytes"
