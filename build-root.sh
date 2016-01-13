#!/bin/sh

#Install ROOT devDependencies
sudo apt-get update -qq
sudo apt-get install -qq git dpkg-dev make g++ gcc binutils libx11-dev libxpm-dev libxft-dev libxext-dev

#Clone ROOT
cd ..
if [ ! -d root ]; then
  git clone https://github.com/root-mirror/root.git root
fi
cd root
git pull

#Build ROOT
cd ..
if [ -d rootbuild ]; then
  cd rootbuild
fi

if [ ! -d rootbuild ]; then
  mkdir rootbuild
  cd rootbuild
  cmake ../root
fi

make -j8 install
cmake --build . --target install
source /usr/local/bin/thisroot.sh
