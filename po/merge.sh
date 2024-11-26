#!/bin/bash

if [ ! -f "$1.po" ]; then
    echo "parameter for po file missing e.g (es) for es.po"
    exit 1
fi

intltool-update --pot
if [  -f "docklight.pot" ]; then
    echo "......docklight.po created!"
fi


mergefile="merged-$1.po"
intltool-update --dist --gettext-package=docklight --output-file=$mergefile $1
if [  -f $mergefile ]; then
    echo "......$mergefile created!"
    rm "$1.po"
    echo "......old $1.po  removed!"
    mv $mergefile "$1.po"
    if [  -f "$1.po" ]; then
        echo "......$1.po  merged success!"
    fi
    
fi
 
