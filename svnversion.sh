#!/bin/bash

# Letzte gesendete Version in $version speichern
version=`svn info | grep Rev: | awk '{print $4}'`
# Datum der letzten Version in $datum speichern
datum=`svn info | grep datum | awk '{print $3" "$4}'`

#echo $version;
#echo $datum;

sed -e 's/\$WCREV\$/'$version'/' $1 > $2;
sed -e 's/\$WCDATE\$/'"$datum"'/' $2 > tmp;
mv tmp $2;