#!/bin/bash

if [ -d "$WORKSPACE/rootinstall" ]; then
	cd $WORKSPACE/rootinstall
else
	cd /afs/cern.ch/sw/lcg/app/releases/ROOT/6.06.06/x86_64-centos7-gcc48-opt/root
fi
source ./bin/thisroot.sh
cd $WORKSPACE
rm artifacts/* -R
ROOTJS_ADD_CFLAGS="-fprofile-arcs -ftest-coverage" ROOTJS_ADD_LDLAGS="-lgcov --coverage" npm install
npm test

lcov --capture --base-directory src/ --directory build/Release/obj.target/rootjs/src/  --output-file coverage.info --no-external
genhtml coverage.info --output-directory coverage
