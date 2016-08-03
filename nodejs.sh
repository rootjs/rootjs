#!/bin/bash

readonly rootInstallDir="$1"

if [ ! -d "${rootInstallDir}" ] || [ ! -f "${rootInstallDir}/bin/thisroot.sh" ]; then
	echo "Couldn't find a ROOT installation"
	exit 1
fi
source "${rootInstallDir}/bin/thisroot.sh"

rm artifacts/* -R
ROOTJS_ADD_CFLAGS="-fprofile-arcs -ftest-coverage" ROOTJS_ADD_LDLAGS="-lgcov --coverage" npm install
npm test

lcov --capture --base-directory src/ --directory build/Release/obj.target/rootjs/src/  --output-file coverage.info --no-external
genhtml coverage.info --output-directory coverage
