#!/usr/bin/env bash

if [ $EUID != 0 ]; then
	echo "this script must be run as root"
	echo ""
	echo "usage:"
	echo "sudo "$0
	exit $exit_code
   exit 1
fi

for i in *.po
do
    d=${i%.*}

    #path="/usr/lib/docklight/share/locale/$d/LC_MESSAGES"
    path="/usr/local/bin/docklight-5/locale/$d/LC_MESSAGES"
    
    echo $path
    mkdir -p $path

    modir=$path
    mofile=$path"/docklight.mo"
    echo $mofile

	if [ ! -d "$modir" ]; then
		mkdir $modir
	fi
	msgfmt -c $i -o $mofile
done
echo "done!"
