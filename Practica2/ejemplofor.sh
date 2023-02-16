for i in `ls`
do
    if [ -f $i ]
    then
        echo "$i es un fichero"
    else
        echo "$i es un directorio"
    fi
done