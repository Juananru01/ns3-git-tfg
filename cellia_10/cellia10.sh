#!/bin/bash
#

if [ $# -ne 2 ]; then
	echo "Illegal number of parameters"
	echo "2: Start of seeds"
	echo "3: End of seeds"
	exit 1
fi

runs=`seq $1 1 $2`

for run in $runs
do
	sbatch ./job24.sh $run
done
