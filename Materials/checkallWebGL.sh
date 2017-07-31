#!/bin/bash

# vérifie que tout fonctionne
for d in *
do
    if test -d "$d/WebGL"
    then
        pushd "$d/WebGL"
        firefox *.html
        popd
    fi
done

