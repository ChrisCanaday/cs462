#!/bin/bash
# Job name:
#SBATCH --job-name=Assignment_T_P2
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
#SBATCH --ntasks-per-node=32
#
# Wall clock limit:
#SBATCH --time=0-00:01:00
#
## Command(s) to run (example):
srun ./a.out -n 1024 -P 64
