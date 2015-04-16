#!/bin/bash
cd ../
COMMAND="build/test/mixer > logs/mixed_2g_32b 2>&1"
eval $COMMAND
sleep 2
COMMAND="build/test/sorter >> logs/mixed_2g_32b 2>&1"
eval $COMMAND
rm -rf /data/sober/*


cp config1 config

COMMAND="build/test/datagen > logs/goodrich_2g_32b 2>&1"
eval $COMMAND
sleep 2
COMMAND="build/test/goodrich --msize=524288 >> logs/goodrich_2g_32b 2>&1"
eval $COMMAND
