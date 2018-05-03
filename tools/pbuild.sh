#!/bin/zsh

# pbuild - build for other version of ubuntu

DIST=xenial

BASETGZ=/data/dw/pbuilder/$DIST.tgz
RESULT=../releases
DSC=`ls $RESULT/*dsc | tail`

mkdir -p $RESULT/$DIST

echo Working with $DSC
echo
echo pbuilder build --basetgz $BASETGZ --distribution $DIST --buildresult $RESULT/$DIST $DSC
sudo pbuilder build --basetgz $BASETGZ --distribution $DIST --buildresult $RESULT/$DIST $DSC
