#!/usr/bin/env bash

if [ $EUID != 0 ]; then
	echo "this script must be run as root"
	echo ""
	echo "usage:"
	echo "sudo "$0
	exit $exit_code
   exit 1
fi

./autogen.sh
./configure
sudo make install
cd po
./deploymo.sh


