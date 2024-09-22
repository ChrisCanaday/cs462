#!/bin/bash
# Job name:
#SBATCH --job-name=Assignment_1_P1.2
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
#SBATCH --nodes=2
#
# CPUs per node
#SBATCH --ntasks-per-node=48
#
# Wall clock limit:
#SBATCH --time=00:10:00
#
## Command(s) to run (example):
srun ./a.out