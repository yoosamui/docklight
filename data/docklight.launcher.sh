#!/bin/bash

if [ $# -eq 0 ]; then
#    echo " command line contains 0 arguments"
    exit -1	
fi

#echo "$1"
mv "$1" /usr/share/applications


exit 0
