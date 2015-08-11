#!/bin/bash

../downward-release --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h]),h]))' < output4 > tree &
../downward-release --heuristic 'h=lmcut()' --search 'eager(tree([sum([g(),h])]))' < output4 > tree1 &
../downward-release --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h]))' < output4 > fifo &
../downward-release --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h],fifo=false))' < output4 > lifo &
../downward-release --heuristic 'r=random()' --heuristic 'h=lmcut()' --search 'eager(tiebreaking([sum([g(),h]),h,r]))' < output4 > random &

