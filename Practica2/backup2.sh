#!/bin/bash
for i in "$@"
do 
    if [ ! -f "$i" ]
    then
        echo "$i no existe"
    else
        fecha=$(date +%g%m%d)
        Version="$fecha"_$i
        cp $i "$fecha"_$i
    fi

done