import png
import sys

sys.stdout.reconfigure(encoding='utf-8')

# Get the filename from the command line
if len(sys.argv) < 2:
    print("Usage: python gen_image.py <filename>")
    sys.exit(1)
filename = sys.argv[1]

# Open the PNG file and read its contents
with open(filename, 'rb') as f:
    reader = png.Reader(f)

    # Extract the image data
    width, height, pixels, metadata = reader.read()
    bitdepth = metadata['bitdepth']
    color_type = metadata['greyscale'] and 'Grayscale' or 'Color'
    transparent = metadata.get('transparent')

    # Do some verifications
    if width % 16 != 0 or height % 16 != 0:
        print("Image need to contain a set of 16x16 subimages.")
        sys.exit(1)

    if color_type != 'Color' or bitdepth != 8:
        print("Image needs to have a colored 8-bit palette.")
        sys.exit(1)

    # find transparent color
    try:
        transparent = next(i for i,v in enumerate(reader.palette()) if v[3] == 0)
    except:
        transparent = 255

    rows = list(pixels)
    output = []

    # Print the first row of pixels
    i = 0
    for ix in range(0, width, 16):
        for iy in range(0, height, 16):
            output.append("\x1b*" + str(i) + ';')
            i += 1
            if transparent is not None:
                output.append(str(transparent) + ';')
            else:
                output.append('255;')
            px = []
            for x in range(0, 16):
                for y in range(0, 16):
                    px.append(str(rows[ix + x][iy + y]))
            output.append(';'.join(px))
            output.append('i')

    print(''.join(output))
