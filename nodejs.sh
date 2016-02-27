#!/bin/bash
cd $WORKSPACE/rootinstall
source ./bin/thisroot.sh
cd $WORKSPACE
rm artifacts/* -R
ROOTJS_ADD_CFLAGS="-fprofile-arcs -ftest-coverage" ROOTJS_ADD_LDLAGS="-lgcov --coverage" npm install
npm test

lcov --capture --base-directory src/ --directory build/Release/obj.target/rootjs/src/  --output-file coverage.info --no-external
genhtml coverage.info --output-directory coverage
