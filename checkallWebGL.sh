#!/bin/bash

# vérifie que tout fonctionne
for d in *
do
    if test -f "$d/checkallWebGL.sh"
    then
        pushd "$d"
        ./checkallWebGL.sh "$@"
        popd
        read -p "Taper entrée pour continuer, O ou o pour arreter :" reponse
        if test "${reponse:0:1}" = o -o "${reponse:0:1}" = O
        then
            break
        fi
    fi
done

