#!/bin/bash
# Job name:
#SBATCH --job-name=Assignment_3_P1.1
#SBATCH --error=job.e%J
#SBATCH --output=job.o%J
#
# Account
#SBATCH --account=ISAAC-UTK0319
#
# Partition
#SBATCH --partition=campus
#SBATCH --qos=campus
#
# Number nodes
#SBATCH --nodes=6
#SBATCH --ntasks=256
#
# Wall clock limit:
#SBATCH --time=0-00:01:00
#
## Command(s) to run (example):
srun ./a.out
