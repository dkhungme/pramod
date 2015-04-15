#!/bin/bash
RECORDS=(100000 1000000 10000000 20000000) 
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=0; i<4; i++ )); do
	cd $DIR
	echo Sorting ${RECORDS[i]}
	let BLOCK=${RECORDS[i]}/4
	sed -e "s/num_records:.*/num_records:${RECORDS[i]}/g" $CONFIG >tmp1
	
	#run Goodrich first
	sed -e "s/num_records_per_block:.*/num_records_per_block:${RECORDS[i]}/g" tmp1 > $CONFIG
	cd ../
	rm -rf data/*
	build/test/datagen
	echo Goodrich, M = 1000
	sleep 1
	COMMAND="build/test/goodrich --msize=1000 > $LOG_DIR/goodrich_N${RECORDS[i]}_M1000 2>&1"
	eval $COMMAND

	sleep 1
        rm -rf data/*
	build/test/datagen
	echo Goodrich, M = 10000
	COMMAND="build/test/goodrich --msize=10000 > $LOG_DIR/goodrich_N${RECORDS[i]}_M10000 2>&1"
	eval $COMMAND

	sleep 1
	rm -rf data/*
	build/test/datagen
	#let BLOCK2=1000
	COMMAND="build/test/goodrich --msize=$BLOCK > $LOG_DIR/goodrich_N${RECORDS[i]}_M$BLOCK 2>&1"
	eval $COMMAND


	sleep 1
	cd $DIR

	#run 
	sed -e "s/num_records_per_block:.*/num_records_per_block:$BLOCK/g" tmp1 > $CONFIG
	cd ../
	rm -rf data/*
	COMMAND="build/test/mixer > $LOG_DIR/mixed_${RECORDS[i]} 2>&1"
	eval $COMMAND
	sleep 1
	COMMAND="build/test/sorter 1>> $LOG_DIR/mixed_${RECORDS[i]} 2>&1"
	eval $COMMAND

	sleep 5
done
rm -rf tmp1
