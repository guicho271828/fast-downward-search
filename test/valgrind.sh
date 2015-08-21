#!/bin/bash

# vg="valgrind --tool=massif "
# vg="valgrind --leak-check=full"
vg="valgrind --tool=callgrind --trace-children=yes"
date=$(date +"%Y-%m-%d-%H-%M")

k=0.5

for file in output
do
    echo "-*- truncate-lines : t -*-" > $file.$date.log 
    echo "-*- truncate-lines : t -*-" > $file.$date.err
    $vg ../downward-debug --heuristic 'h=lmcut()' \
        --search "eager(ucb([sum([g(),h]),h],frontier=true,k=$k),reopen_closed=true)" \
        < $file >> $file.$date.log 2>> $file.$date.err
    wait
done


