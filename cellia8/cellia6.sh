#!/bin/bash
#

if [ $# -ne 3 ]; then
	echo "Illegal number of parameters"
	echo "2: Start of seeds"
	echo "3: End of seeds"
	echo "4: FrequencyBand"
	exit 2
fi

runs=`seq $1 1 $2`

for run in $runs
do
	sbatch ./job18.sh $run $3
done
