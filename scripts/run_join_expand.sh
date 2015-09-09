#!/bin/bash
ITEMS=(536870912 268435456 134217728 67108864 8388608)
DIR=`pwd`
CONFIG=../config
LOG_DIR=logs
mkdir -p ../$LOG_DIR

for (( i=4; i>-1; i-- )); do
	cd $DIR
	echo Join Expanding ${ITEMS[i]}

	sed -e "s/num_records:.*/num_records:${ITEMS[i]}/g" $CONFIG > tmp1
        sed -e "s/num_records_per_block:.*/num_records_per_block:${ITEMS[i]}/g" tmp1 >tmp2
	
        mv tmp2 $CONFIG
	
	cd ../
	rm -rf data/*
	build/test/datagen
	sleep 10
	COMMAND="build/test/join> $LOG_DIR/Join_Expand_${ITEMS[i]} 2>&1"
	eval $COMMAND

	sleep 5
	done
done
rm -rf tmp1
