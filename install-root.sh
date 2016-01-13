#!/bin/sh


#Install ROOT devDependencies
sudo apt-get update -qq
sudo apt-get install -qq git dpkg-dev make g++ gcc binutils libx11-dev libxpm-dev libxft-dev libxext-dev

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
