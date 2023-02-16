#!/bin/bash
for j in "$@"
do
    for i in `ls $j`
    do
    if [ ! -f "$i" ]
    then
        echo "$i no existe"
    else
        fecha=$(date +%g%m%d)
        Version="$fecha"_$i
        cp $i $Version
    fi
    done
done