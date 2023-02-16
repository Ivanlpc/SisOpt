#!/bin/bash
for i in `find $1 -type f -size +$2c`
do
    size=$(stat -c %s $i)
    echo "$i: $size bytes"
done