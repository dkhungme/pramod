#!/bin/bash
NBLOCKS=(16 32 64 128 256) 
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=0; i<5; i++ )); do
	cd $DIR
	echo Sorting with ${NBLOCKS[i]} blocks

	let INPUTSIZE=131072*${NBLOCKS[i]}
	
	sed -e "s/num_records:.*/num_records:$INPUTSIZE/g" $CONFIG > tmp1
        mv tmp1 $CONFIG

	cd ../
	rm -rf data/*
	COMMAND="build/test/mixer > $LOG_DIR/mixed_131072_r3_b${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	
	sleep 10
	COMMAND="build/test/sorter >> $LOG_DIR/mixed_131072_r3_b${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	sleep 5
done



