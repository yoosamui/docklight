#!/bin/bash

for i in *.po
do
	d=${i%.*}

    modir="/usr/lib/docklight/share/locale/$d/LC_MESSAGES"
    mofile="/usr/lib/docklight/share/locale/$d/LC_MESSAGES/docklight.mo"
	
	if [ ! -d "$modir" ]; then
		mkdir $modir  
	fi
	msgfmt -c $i -o $mofile
	#msgfmt -c -o `echo $i | grep -o '^[^.]*'`.mo $i
done
