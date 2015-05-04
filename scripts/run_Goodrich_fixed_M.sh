#!/bin/bash
NBLOCKS=(16 32 64 128 256) 
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=0; i<5; i++ )); do
	cd $DIR
	echo Sorting ${NBLOCKS[i]}

	let INPUTSIZE=524288*${NBLOCKS[i]}

        sed -e "s/num_records_per_block:.*/num_records_per_block:$INPUTSIZE/g" $CONFIG >tmp1
	mv tmp1 $CONFIG
	
	#run Goodrich first
	cd ../
	rm -rf data/*
	build/test/datagen
	sleep 10
	COMMAND="build/test/goodrich --msize=524288> $LOG_DIR/goodrich_N${NBLOCKS[i]}_M524288 2>&1"
	eval $COMMAND

	sleep 5
done
rm -rf tmp1
