#!/bin/bash
#
#SBATCH --output=%A.out
#SBATCH --error=%A.err
#SBATCH --ntasks-per-node=1
#SBATCH --time=7-48:00:00
#SBATCH --partition=batch

if [ $# -ne 1 ]; then
	echo "Illegal number of parameters"
	echo "1: Seed"
	exit 2
fi

seed=$1

srun ../script2stas.sh $1
