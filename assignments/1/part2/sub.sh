#!/bin/bash
# Job name:
#SBATCH --job-name=Assignment_1_P2
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
# Number of MPI tasks needed for use case (example):
#SBATCH --ntasks=8
#
# Processors per task:
#SBATCH --cpus-per-task=1
#
## Number nodes
#SBATCH --nodes=1
#
# Wall clock limit:
#SBATCH --time=00:01:00
#
## Command(s) to run (example):
srun ./a.out