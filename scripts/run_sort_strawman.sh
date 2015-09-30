#!/bin/bash
NBLOCKS=(256 128 64 32 4) 
ITEMS=(536870912 268435456 134217728 67108864 8388608)
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=4; i>-1; i-- )); do
	cd $DIR
	echo Sorting ${ITEMS[i]} with ${NBLOCKS[i]} blocks

	sed -e "s/num_records:.*/num_records:${ITEMS[i]}/g" $CONFIG > tmp1
	sed -e "s/merge_factor:.*/merge_factor:${NBLOCKS[i]}/g" tmp1 > tmp2
	sed -e "s/mixnet_rounds:.*/mixnet_rounds:2/g" tmp2 > tmp3
        mv tmp3 $CONFIG

	cd ../
	rm -rf data/*
	COMMAND="build/test/datagen > $LOG_DIR/stawman_sorting_1thread_${NBLOCKS[i]}x2_21 2>&1"
	eval $COMMAND
	echo --finish datagen	

	sleep 10
	COMMAND="build/test/sorter >> $LOG_DIR/stawman_sorting_1thread_${NBLOCKS[i]}x2_21 2>&1"
	eval $COMMAND
	sleep 5
done



