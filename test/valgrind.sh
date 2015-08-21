#!/bin/bash

# vg="valgrind --tool=massif "
# vg="valgrind --leak-check=full"
vg="valgrind --tool=callgrind --trace-children=yes"

k=0.5
ucb="eager(ucb([sum([g(),h]),h],k=$k),reopen_closed=true)"
lifo="eager(tiebreaking([sum([g(),h]),h],queue=1),reopen_closed=true)"

ref (){
    if eval "[ -z \${$1+x} ]"
    then
        echo "undefined variable $1 !" >&2
        i=0; while caller $i >&2 ;do ((i++)) ;done
        exit 1
    else
        eval "echo -n \$$1"
    fi
}

run (){
    date=$(date +"%Y-%m-%d-%H-%M")
    echo "-*- truncate-lines : t -*-" > $2.$date.$1.log 
    echo "-*- truncate-lines : t -*-" > $2.$date.$1.err
    $vg ../downward-debug --heuristic 'h=lmcut()' --search "$(ref $1)" \
        < $2 >> $2.$date.$1.log 2>> $2.$date.$1.err
}

[ -z $* ] && {
    echo "No input file list!" >&2
    exit 1
}

for file in $@
do
    run ucb $file &
    run lifo $file &
done

wait
