#!/bin/bash
# a=hello
# b=world
# 1=2
# 2=3
# c=`expr $1 + $2`
# d=$(expr $1 + $2)
# echo $c
# echo $d
a=2
b=1

if [ $a -gt $b ]
then
    echo $a es mayor que $b
else
    echo $a es menor que $b
fi



for i in "$@"
do
    if [ -d $i ]
then
    echo $i es un directorio
else
    echo $i no es un directorio
fi
done