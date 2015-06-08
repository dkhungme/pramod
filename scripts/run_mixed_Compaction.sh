#!/bin/bash
NBLOCKS=(16 32 64 128 256) 
M=(131072 262144 393216 524288)
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=0; i<4; i++ )); do
	for (( j=0; j<5; j++ )); do
		cd $DIR
		

		let INPUTSIZE=${M[i]}*${NBLOCKS[j]}

		echo Compacting ${M[i]} x ${NBLOCKS[j]}

		sed -e "s/num_records_per_block:.*/num_records_per_block:$INPUTSIZE/g" $CONFIG >tmp1
		mv tmp1 $CONFIG

		sed -e "s/num_records:.*/num_records:$INPUTSIZE/g" $CONFIG >tmp1
		mv tmp1 $CONFIG
	
		cd ../
		rm -rf data/*
		#COMMAND="build/test/compact> $LOG_DIR/mixed_Compact_N${M[i]}x${NBLOCKS[j]}_B4096 2>&1"
		#eval $COMMAND

		sleep 5
	done
done
rm -rf tmp1
