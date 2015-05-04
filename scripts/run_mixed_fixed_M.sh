#!/bin/bash
NBLOCKS=(16 32 64 128 256) 
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=0; i<5; i++ )); do
	cd $DIR
	echo Sorting with ${NBLOCKS[i]} blocks and 2 rounds

	let INPUTSIZE=524288*${NBLOCKS[i]}
	
	sed -e "s/num_records:.*/num_records:$INPUTSIZE/g" $CONFIG > tmp1
	sed -e "s/merge_factor:.*/merge_factor:${NBLOCKS[i]}/g" tmp1 > tmp2
	sed -e "s/mixnet_rounds:.*/mixnet_rounds:2/g" tmp2 > tmp3
        mv tmp3 $CONFIG

	cd ../
	rm -rf data/*
	COMMAND="build/test/mixer > $LOG_DIR/mixed_524288_r2_b${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	
	sleep 10
	COMMAND="build/test/sorter >> $LOG_DIR/mixed_524288_r2_b${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	sleep 5
done

for (( i=0; i<5; i++ )); do
	cd $DIR
	echo Sorting with ${NBLOCKS[i]} blocks and 4 rounds

	let INPUTSIZE=524288*${NBLOCKS[i]}
	
	sed -e "s/num_records:.*/num_records:$INPUTSIZE/g" $CONFIG > tmp1
	sed -e "s/merge_factor:.*/merge_factor:${NBLOCKS[i]}/g" tmp1 > tmp2
	sed -e "s/mixnet_rounds:.*/mixnet_rounds:4/g" tmp2 > tmp3
        mv tmp3 $CONFIG

	cd ../
	rm -rf data/*
	COMMAND="build/test/mixer > $LOG_DIR/mixed_524288_r4_b${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	
	sleep 10
	COMMAND="build/test/sorter >> $LOG_DIR/mixed_524288_r4_b${NBLOCKS[i]} 2>&1"
	eval $COMMAND
	sleep 5
done



