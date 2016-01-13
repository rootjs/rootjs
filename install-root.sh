#!/bin/sh

#Clone ROOT
cd $HOME
if [ ! -d root ]; then
  git clone https://github.com/root-mirror/root.git root
fi
cd root
git pull

#Build ROOT
cd $HOME
if [ -d rootbuild ]; then
  cd rootbuild
else
  mkdir rootbuild
  cd rootbuild
  cmake ../root
fi
