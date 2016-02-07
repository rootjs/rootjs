#!/bin/bash
cd $WORKSPACE/rootinstall
source ./bin/thisroot.sh
cd $WORKSPACE
rm artifacts/* -R
npm install
npm test
