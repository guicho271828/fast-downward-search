#!/bin/bash

# vg="valgrind --tool=massif "
vg="valgrind --leak-check=full"

for file in output
do
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h]),h]))' < $file
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h])]))' < $file
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=false,open=false))' < $file 
    $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=false,open=true))' < $file > $file.$(date +"%Y-%m-%d-%H-%M").log 2> $file.$(date +"%Y-%m-%d-%H-%M").err
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=true,open=false))' < $file
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=true,open=true))' < $file
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h])]))' < $file
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h]))' < $file
    # $vg ../downward-debug --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h],fifo=false))' < $file
    # $vg ../downward-debug --heuristic 'r=random()' --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h,r]))' < $file
    wait
done


