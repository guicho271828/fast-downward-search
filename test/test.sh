#!/bin/bash

for file in output output2 output4
do
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h]),h]))' < $file > $file.tree &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h])]))' < $file > $file.tree1 &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h]),h]))' < $file > $file.dtree &
    ../downward-release --heuristic 'h=lmcut()' --search 'eager(dtree([sum([g(),h])]))' < $file > $file.dtree1 &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h]))' < $file > $file.fifo &
    # ../downward-release --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h],fifo=false))' < $file > $file.lifo &
    # ../downward-release --heuristic 'r=random()' --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h,r]))' < $file > $file.random &
    wait
done
