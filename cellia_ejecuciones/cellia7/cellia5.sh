#!/bin/bash
#

if [ $# -ne 2 ]; then
	echo "Illegal number of parameters"
	echo "2: Start of seeds"
	echo "3: End of seeds"
	exit 2
fi

runs=`seq $1 1 $2`

for run in $runs
do
	sbatch ./job11.sh $run
	sbatch ./job12.sh $run
	sbatch ./job13.sh $run
	sbatch ./job14.sh $run
	sbatch ./job15.sh $run
	sbatch ./job16.sh $run
done
