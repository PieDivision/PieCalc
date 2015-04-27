#!/bin/sh

make pack
mv piecalc-1.0.tar.gz ~/rpmbuild/SOURCES/
cp data/pie_calc.spec ~/rpmbuild/SPECS/
cd ~/rpmbuild/SPECS/
rpmbuild -ba pie_calc.spec
cd -
mv ~/rpmbuild/RPMS/x86_64/piecalc*.rpm .
