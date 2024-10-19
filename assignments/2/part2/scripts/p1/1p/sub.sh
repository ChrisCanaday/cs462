#!/bin/bash
# Job name:
#SBATCH --job-name=Assignment_2_P2.P1.1p
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
#SBATCH --nodes=1
#
# CPUs per node
#SBATCH --ntasks-per-node=1
#
# Wall clock limit:
#SBATCH --time=0-00:01:00
#
## Command(s) to run (example):
srun ./a.out 10000000
