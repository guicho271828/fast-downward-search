#!/bin/bash

# vg="valgrind --tool=massif "
# export vg="valgrind --leak-check=full"
export vg="valgrind"
# export vg="valgrind --tool=callgrind --trace-children=yes"

k=0.5
export ucb="eager(ucb([sum([g(),h]),h],k=$k),reopen_closed=true)"
export lifo="eager(tiebreaking([sum([g(),h]),h],queue=1),reopen_closed=true)"
export ftree="eager(tree([sum([g(),h]),h],queue=0),reopen_closed=true)"
export ltree="eager(tree([sum([g(),h]),h],queue=1),reopen_closed=true)"
export rtree="eager(tree([sum([g(),h]),h],queue=2),reopen_closed=true)"

export multi="multi([eager(ld([sum([g(),h]),h],queue=0)),eager(rd([sum([g(),h]),h],queue=0))])"

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

export -f ref

[ -z $* ] && {
    echo "valgrind.sh: No input file list!" >&2
    exit 1
}


parallel ./run.sh ::: multi ::: $@

