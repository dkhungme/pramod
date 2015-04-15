#!/bin/bash
cd ../
COMMAND="build/test/mixer > logs/mixed_large 2>&1"
echo $COMMAND
eval $COMMAND
sleep 2
COMMAND="build/test/sorter >> logs/mixed_large 2>&1"
eval $COMMAND
rm -rf data/*
