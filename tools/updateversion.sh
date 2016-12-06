#!/bin/sh

OUT=$1
[ -z $OUT ] && OUT=toplevel/version.xml


cd `dirname $0`
cd ../src

if git show -s > foo.tmp; then
    vREVNO=`git rev-list --count --first-parent HEAD`
    vDATE=`git show -s --format=%cd --date=format:"%Y-%m-%d %H:%M:%S %z" HEAD`
    vVERSION=`git describe --long`
    vYEAR=`git show -s --format=%cd --date=format:%Y HEAD`
else
    vREVNO="?"
    vDATE="?"
    vVERSION="?"
    vYEAR=`date '+%Y'`
fi
rm -f foo.tmp

vBUILDDATE=`date '+%Y-%m-%d %H:%M:%S %z' `


echo '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>' > $OUT
echo '<!DOCTYPE vsdscopeVersion>' >> $OUT
echo '<version ' >> $OUT
echo " version='$vVERSION'" >> $OUT
echo " rev='$vREVNO'" >> $OUT
echo " date='$vDATE'" >> $OUT
echo " builddate='$vBUILDDATE'" >> $OUT
echo " year='$vYEAR'" >> $OUT
echo ' />' >> $OUT
