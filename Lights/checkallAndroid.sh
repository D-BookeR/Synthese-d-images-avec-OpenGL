#!/bin/bash

# vérifie que tout fonctionne
for d in *
do
    if test -d "$d/Android"
    then
        pushd "$d/Android"
        figlet -w 200 $(basename $(dirname $(dirname $PWD)))
        figlet -w 200 $(basename $(dirname $PWD))
        ./InstallRun.sh
        read -p "Taper entrée pour continuer, O ou o pour arreter :" reponse
        if test "${reponse:0:1}" = o -o "${reponse:0:1}" = O
        then
            break
        fi
        ./gradlew clean
        rm -fr gradle .gradle
        popd
    fi
done

