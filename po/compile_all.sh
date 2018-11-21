#!/bin/bash

for i in *.po
do
	d=${i%.*}
	echo $d 
	if [ -d "$d" ]; then
		rm -fr $d  
	fi

	mkdir $d
	msgfmt -c $i -o $d//docklight.mo

	#msgfmt -c -o `echo $i | grep -o '^[^.]*'`.mo $i
done
