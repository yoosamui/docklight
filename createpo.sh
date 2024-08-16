#!/bin/bash
#
# !!! WARNING !!!
#
# Use only to create po files for the first time.
# Existing po files will be ignore.
#
# find . -type f \( -name '*.cpp' -or -name '*.c' \) -print > ../files.txt
# root: folder e.g docklight_5.0.org

# read po/POTFILES.in and write files.txt
rm -f  files.txt
while IFS= read -r line; do
  filename="${line:3}"
  echo ".$filename" >> files.txt
  printf '%s\n' "$line"
done < po/POTFILES.in

# read po/LINGUAS
while IFS= read -r line; do
  first_char="${line:0:1}"
  if [[ "$first_char" == "#" ]]; then
    continue
  fi

  pofile="$line"$".po"
  if [ ! -f "po/"$pofile  ]; then
    xgettext --files-from=files.txt -c -d translations -p po --language=c++ --from-code=utf-8 --keyword=_ -D src -o $pofile
    echo $pofile
   fi
done < po/LINGUAS


