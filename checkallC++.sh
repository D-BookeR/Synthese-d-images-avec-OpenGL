#!/bin/bash

# vérifie que tout se compile et fonctionne
for d in *
do
    if test -f "$d/checkallC++.sh"
    then
        pushd "$d"
        figlet -w 200 $(basename "$d")
        if ! ./checkallC++.sh "$@" ; then exit 1 ; fi
        popd
    fi
done
