#!/bin/bash
fecha=$(date +%g%m%d)
if [ ! -d "backup" ]
then
    mkdir backup backup/$fecha
fi

if [ ! -d "backup/$fecha" ]
then
    mkdir backup/$fecha
fi

for i in "$@"
do
    
    if [ ! -f "$i" ]
    then
        echo "$i no existe"
    else
        cp $i "./backup/$fecha/$i"
    fi
    
done