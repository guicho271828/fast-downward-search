#!/bin/bash


date=$(date +"%Y-%m-%d-%H-%M")
echo "-*- truncate-lines : t -*-" > $2.$date.$1.log 
echo "-*- truncate-lines : t -*-" > $2.$date.$1.err
$vg ../downward-release --heuristic 'h=lmcut()' --search "$(ref $1)" \
    < $2 >> $2.$date.$1.log 2>> $2.$date.$1.err
