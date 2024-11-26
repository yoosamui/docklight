#!/usr/bin/env bash

if [ $EUID != 0 ]; then
	echo "this script must be run as root"
	echo ""
	echo "usage:"
	echo "sudo "$0
	exit $exit_code
   exit 1
fi

ROOT=$(cd $(dirname $0); pwd -P)

sudo apt-get update && sudo apt install libnotify-bin \
build-essential \
cmake \
libgtkmm-3.0-dev \
libwnck-3-dev \
autotools-dev \
autoconf \
libtool \
gettext \
autopoint \
intltool \
libbamf3-dev \
bamfdaemon \
gnupg -y

echo "done."
