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

apt-get update
