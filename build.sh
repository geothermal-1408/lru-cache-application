#!/bin/sh

#set -xe

CMD=$1
INPUT=$2
CFLAGS="-Wall -Wextra -std=c11 -pedantic -ggdb"

if [ "$CMD" == "run" ]; then
   echo "running project"
   ./lru $INPUT
   
elif [ "$CMD" == "build" ]; then
    echo "building project"
    cc $CFLAGS -o lru main.c
    
else
    echo "Unknown Command: $CMD"
    echo "usage: ./build.sh [build|run]"
    exit 1
    
fi

