#!/bin/zsh

cd `dirname $0`
cd ../src

for a in */*.cpp */*.h; do
    ../tools/apply-gpl.pl $a
done

cd ../octave/vscope-1.0
for a in *.m */*.m; do
    ../../tools/apply-gpl.pl $a
done
    
