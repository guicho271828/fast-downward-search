#!/bin/bash

for file in output output2 output4
do
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h]),h]))' < $file > $file.2.tree &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h])]))' < $file > $file.2.tree1 &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=false,open=false))' < $file > $file.2.dtreeff &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=false,open=true))' < $file > $file.2.dtreeft &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=true,open=false))' < $file > $file.2.dtreetf &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h],helpers=true,open=true))' < $file > $file.2.dtreett &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h])]))' < $file > $file.2.dtree1 &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h]))' < $file > $file.2.fifo &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h],fifo=false))' < $file > $file.2.lifo &
    # ../downward-release --heuristic 'r=random()' --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h,r]))' < $file > $file.2.random &
    ../downward-release --heuristic 'h=lmcut()' --search 'eager(loop([sum([g(),h]),h]))' < $file > $file.2.loop &
    ../downward-release --heuristic 'h=lmcut()' --search 'eager(rd([sum([g(),h]),h]))' < $file > $file.2.rd &
    ../downward-release --heuristic 'h=lmcut()' --search 'eager(fd([sum([g(),h]),h]))' < $file > $file.2.fd &
    wait
done
