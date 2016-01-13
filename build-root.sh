#!/bin/sh

#Build ROOT
cd $HOME/rootbuild

make -j12 install
cmake --build . --target install
source /usr/local/bin/thisroot.sh
