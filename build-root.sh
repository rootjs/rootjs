#!/bin/bash

#Build ROOT
cd $HOME/rootbuild

make -j8 install

mkdir $WORKSPACE/rootinstall
cmake -DCMAKE_INSTALL_PREFIX=$WORKSPACE/rootinstall -P cmake_install.cmake
cd $WORKSPACE/rootinstall
pwd
source ./bin/thisroot.sh
cd $WORKSPACE
