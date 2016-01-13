#!/bin/bash
cd $WORKSPACE/rootinstall
source ./bin/thisroot.sh
cd $WORKSPACE
npm install
npm test
