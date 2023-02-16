#!bin/bash
if [ ! -d $2 ]
    then
        mkdir $2
    fi
for i in `ls $1`
do
    extension=$(echo $i | cut -d. -f2-)
    author=$(grep -E 'autor' $1/$i | cut -d: -f2)
    title=$(grep -E 'titulo' $1/$i | cut -d: -f2)
    if [ ! -d $2/$author ]
    then
        mkdir $2/$author
    fi
    cp $1/$i "$2/$author/$title.$extension"

done