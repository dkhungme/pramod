#!/bin/bash
NBLOCKS=(2 4 8 16 32) 
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=0; i<5; i++ )); do
	cd $DIR
	echo Sorting with ${NBLOCKS[i]} blocks

	let BLOCKSIZE=16777216/${NBLOCKS[i]}
	
	sed -e "s/num_records_per_block:.*/num_records_per_block:$BLOCKSIZE/g" $CONFIG > tmp1
        mv tmp1 $CONFIG

	cd ../
	rm -rf data/*
	COMMAND="build/test/mixer > $LOG_DIR/mixed_b_${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	sleep 1
	COMMAND="build/test/sorter >> $LOG_DIR/mixed_b_${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	sleep 5
done

echo Sorting with large file (10GB)

rm -rf data/*
cp config1 config
COMMAND="build/test/mixer > $LOG_DIR/mixed_large 2>&1"
eval $COMMAND
sleep 1
COMMAND="build/test/sorter >> $LOG_DIR/mix_large 2>&1"
eval $COMMAND

