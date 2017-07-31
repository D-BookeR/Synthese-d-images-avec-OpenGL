#!/bin/bash

# vérifie que tout se compile et fonctionne
for d in *
do
    if test -d "$d/C++"
    then
        pushd "$d/C++"
        figlet -w 200 $(basename $(dirname $(dirname $PWD)))
        figlet -w 200 $(basename $(dirname $PWD))
        if ! make -j4 $1 ; then exit 1 ; fi
        popd
    fi
done

