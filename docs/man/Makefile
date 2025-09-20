ALL: vscope.1

%.1: %.1.txt
	a2x --doctype manpage --format manpage --no-xmllint --destination-dir=. $<


# a2x is part of the asciidoc package on ubuntu:
#
#   sudo apt install asciidoc
