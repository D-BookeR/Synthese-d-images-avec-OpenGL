#!/bin/bash

comparer()
{
    cpp=C++/$1.cpp
    js=WebGL/$1.js
    java=Android/app/src/main/java/com/dbooker/livreopengl/$1.java

    if test -f $cpp -a -f $js
    then
        meld $cpp $js
    fi

    if test -f $cpp -a -f $java
    then
        meld $cpp $java
    fi

    #if test -f $js -a -f $java
    #then
    #    meld $js $java
    #fi
}

if test $# -eq 0
then
    # tout comparer
    for base in WebGL/*.js
    do
        comparer $(basename $base .js)
    done
else
    # comparer seulement les fichiers dont la base est fournie en paramètres
    for base
    do
        comparer $base
    done
fi
