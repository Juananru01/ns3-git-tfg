#!/bin/bash
#
#SBATCH --output=%A.out
#SBATCH --error=%A.err
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-48:00:00
#SBATCH --partition=batch

if [ $# -ne 2 ]; then
	echo "Illegal number of parameters"
	echo "1: Seed"
	echo "2: FrequencyBand"
	exit 2
fi

srun ../script1stasDrop_queue.sh $1 $2
