#!/bin/bash
for i in "$@"
do
    
    if [ ! -f "$i" ]
    then
        echo "$i no existe"
    else
        fecha=$(date +%g%m%d)
        if [ ! -d "backup" ]
        then
            mkdir backup backup/$fecha
        fi
        if [ ! -d "backup/$fecha" ]
        then
            mkdir backup/$fecha
        fi
        cp $i "./backup/$fecha/$i"
    fi
    
done