#!/bin/sh

git archive --format tar "--prefix=$FULLNAME/" HEAD | gzip > ~/rpmbuild/SOURCES/piecalc-1.0.tar.gz

cp data/pie_calc.spec ~/rpmbuild/SPECS/
cd ~/rpmbuild/SPECS/
rpmbuild -ba pie_calc.spec
cd -
mv ~/rpmbuild/RPMS/`uname -m`/piecalc*.rpm .
