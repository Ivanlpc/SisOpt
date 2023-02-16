if [ ! -f "$i" ] # Chequea si el parámetro de entrada representa un fichero que existe
    then # En caso de no existir, da un mensaje comunicándolo
        echo "$i no existe"
    else # En caso de existir el fichero ...
        fecha=$(date +%g%m%d)
        Version=$(fecha)_$(i) # se compone el nombre del nuevo nombre de
        # fichero (Version) como el nombre original ($1)
        # seguido por "." y el número actual de copia (Num)
        cp $i $Version # Copia el fichero original ($1) con el nombre nuevo
    fi 