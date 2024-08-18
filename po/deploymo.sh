#!/bin/bash

for i in *.po
do
    d=${i%.*}

    path="/usr/lib/docklight/share/locale/$d/LC_MESSAGES"
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
echo "send it!"
