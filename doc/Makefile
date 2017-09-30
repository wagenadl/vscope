ALL: vscope.1

%.1: ../doc/%.1.txt
	a2x --doctype manpage --format manpage --no-xmllint --destination-dir=../build-doc $<

clean:; rm -f userguide.log userguide.aux
