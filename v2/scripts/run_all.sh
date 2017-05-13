#!/bin/bash


ROOT=../..
FILES=$(ls $ROOT/dpc2sim/traces/)

echo "Running test $1"

for trace in $FILES; do
  echo "Running trace $trace"
  zcat $ROOT/dpc2sim/traces/$trace | ../prefetcher &> /dev/null
  tracefile=$(ls tracefile-*)
  ./$1 $tracefile
  rm $tracefile
  echo ""
done

