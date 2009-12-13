#!/bin/bash

# Letzte gesendete Version in $version speichern
version=`svnversion`

#echo $version;
#echo $datum;

sed -e 's/\$WCREV\$/'$version'/' $1 > $2;
