all: vscope_8x8.png vscope_16x16.png vscope_22x22.png vscope_24x24.png vscope_32x32.png vscope_48x48.png	\
     vscope_64x64.png vscope_128x128.png vscope_256x256.png vscope_512x512.png

vscope_%.png: ../../tools/vscope.svg
	inkscape --export-width  $(shell echo $*|sed 's/x.*//') \
		 --export-height $(shell echo $*|sed 's/.*x//') \
		 --export-filename $@ $<

# Shell fragment to convert scalable icon to bitmaps:
# in=vscope_scalable.svg
# for r in 8 16 22 24 32 48 64 128 256 512 ; do
#     echo $r;
#     res=${r}x${r}
#     out=vscope_${res}.png
#     # ImageMagick (convert) messes up the text
#     #convert -geometry ${res} ${in} ${out}
#     # Inkscape gets it right, of course
#     inkscape -w ${r} -h ${r} --export-filename=${out} ${in}
#     # librsvg-2.so.2 (rsvg-convert) messes up the text
#     #rsvg-convert --width ${r} --height ${r} --keep-aspect-ratio ${in} > ${out}
# done

.PHONY: all
