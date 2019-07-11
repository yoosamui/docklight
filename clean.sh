#!/bin/bash
shopt -s extglob
rm -rf debian/docklight
rm -rf debian/.debhelper
rm -f src/docklight
rm -f src/*.o
rm -rf src/*.swp
rm -rf src/.*-
rm -rf src/.*-
rm -rf src/.*.swo
find . -name '.*~' -delete
find . -name '.dirstamp' -delete
rm -f src/components/*.o
rm -f src/Makefile src/Makefile.in
rm -f src/docklight.data/attachments/*
find 'src/' -type d -name '.deps' -print0 | xargs -0 rm -rf
rm -f data/Makefile
rm -f data/Makefile.in
rm -f data/icons/Makefile
rm -f data/icons/Makefile.in
rm -f src/docklight.data/attachments/*
rm -rf !(clean.sh|autogen.sh|configure.ac|LICENSE|Makefile.am|README.md|data|debian|m4|nbproject|package|po|src)
cd m4
rm -rf !(NOTES)
cd ../po
rm -rf !(*.po|LINGUAS|POTFILES.in|compile_all.sh|deploymo.sh|merge.sh)
cd ..
shopt -u extglob

exit 0
