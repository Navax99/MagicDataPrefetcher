#!/bin/bash

ROOT=../..
FILES=$(ls $ROOT/dpc2sim/traces/)

echo "Running all test"

for trace in $FILES; do
  echo "Running trace $trace"
  zcat $ROOT/dpc2sim/traces/$trace | ../prefetcher &> tmp_file
  tracefile=$(ls tracefile-*)
  rm $tracefile
  info=$(cat tmp_file | grep "Simulation Instructions")
  echo $info
  info=$(cat tmp_file | grep "Warmup Instructions")
  echo $info
  info=$(cat tmp_file | grep "Scramble loads")
  echo $info
  info=$(cat tmp_file | grep "Last Level Cache")
  echo $info
  info=$(cat tmp_file | grep "bandwidth")
  echo $info
  info=$(cat tmp_file | grep "Simulation complete")
  echo $info
  echo ""
done

rm tmp_file
