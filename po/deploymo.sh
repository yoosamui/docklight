#!/bin/bash

#mkdir -p /usr/lib/docklight/{share/locale/de/LC_MESSAGES,bin}

   #// mkdir -p "/usr/lib/docklight/share/locale/de/LC_MESSAGES"


for i in *.po
do
	d=${i%.*}

    ###//path="/usr/lib/docklight/{share/locale/$d/LC_MESSAGES}"
    path="/usr/lib/docklight/share/locale/$d/LC_MESSAGES"
    echo $path
    mkdir -p $path

    modir="/usr/lib/docklight/share/locale/$d/LC_MESSAGES"
    mofile="/usr/lib/docklight/share/locale/$d/LC_MESSAGES/docklight.mo"

	if [ ! -d "$modir" ]; then
		mkdir $modir
	fi
	msgfmt -c $i -o $mofile
	#msgfmt -c -o `echo $i | grep -o '^[^.]*'`.mo $i
done
