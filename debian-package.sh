#!/bin/sh

NAME=piecalc
VERSION=1.0

FULLNAME="$NAME-$VERSION"

rm -rf "/tmp/$FULLNAME-package"
mkdir -p "/tmp/$FULLNAME-package"

git archive --format tar "--prefix=$FULLNAME/" master | gzip > "/tmp/$FULLNAME-package/$FULLNAME.orig.tar.gz"

DIR=`pwd`
cd "/tmp/$FULLNAME-package"

tar -axf "$FULLNAME.orig.tar.gz"

cd "$FULLNAME"
mkdir debian
dch --create -v 1.0 --package piecalc --empty
echo 9 > debian/compat

debuild -us -uc

cd "$DIR"

mv "/tmp/$FULLNAME-package/"*.deb .
