#!/usr/bin/env python

import sys, os
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
    for x in range(0, width / 8):
        byte = 0

        for bit in range(0, 8):
            r, g, b = rgb_im.getpixel((x + bit, y))
            v = (r + g + b) / 3.0

            if v < 128:
                byte |= 0x80 >> bit

        bytes += [byte]

name = os.path.splitext(filepath)[0].lower()
guard = "H_{uppername}".format(uppername=name.upper())

sys.stdout.write("#ifndef {guard}\r\n".format(guard=guard))
sys.stdout.write("#define {guard}\r\n\r\n".format(guard=guard))
sys.stdout.write("image_{name}[] = {{\r\n\t".format(name=name))

linepos = 0
for i in range(0, len(bytes)):
    sys.stdout.write("{v},\t".format(v=hex(bytes[i])))
    
    linepos += 1

    if linepos > 12:
        sys.stdout.write("\r\n\t")
        linepos = 0

sys.stdout.write("\r\n}\r\n\r\n")
sys.stdout.write("#endif")
