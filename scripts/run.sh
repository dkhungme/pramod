#!/bin/bash
RECORDS=(10000 100000 1000000 10000000 20000000) 
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=0; i<5; i++ )); do
	cd $DIR
	echo Sorting ${RECORDS[i]}
	let BLOCK=${RECORDS[i]}/4
	sed -e "s/num_records:.*/num_records:${RECORDS[i]}/g" $CONFIG >tmp1
	
	#run Goodrich first
	sed -e "s/num_records_per_block:.*/num_records_per_block:${RECORDS[i]}/g" tmp1 > $CONFIG
	cd ../
	rm -rf data/*
	build/test/datagen
	echo Goodrich, M = $BLOCK
	sleep 1
	COMMAND="build/test/goodrich --msize=$BLOCK > $LOG_DIR/goodrich_N${RECORDS[i]}_M$BLOCK 2>&1"
	eval $COMMAND

	sleep 1
	let BLOCK1=10000
	echo Goodrich, M = $BLOCK1
	COMMAND="build/test/goodrich --msize=$BLOCK1 > $LOG_DIR/goodrich_N${RECORDS[i]}_M$BLOCK1 2>&1"
	eval $COMMAND

	#sleep 1
	#let BLOCK2=1000
	#echo Goodrich, M = $BLOCK2
	#COMMAND="build/test/goodrich --msize=$BLOCK2 > $LOG_DIR/goodrich_N${RECORDS[i]}_M$BLOCK2 2>&1"
	#eval $COMMAND


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
done
rm -rf tmp1
