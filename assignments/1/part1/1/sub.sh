#!/bin/bash
# Job name:
#SBATCH --job-name=Assignment_1_P1.1
#SBATCH --error=job.e%J
#SBATCH --output=job.o%J
#
# Account
#SBATCH -A ISAAC-UTK0319
#
# Partition
#SBATCH --partition=campus
#SBATCH --qos=campus
#
# Number nodes
#SBATCH --nodes=1
#
# CPUs per node
#SBATCH --ntasks-per-node=8
#
# Wall clock limit:
#SBATCH --time=0-00:01:00
#
# Email me when done
#SBATCH --mail-type=ALL
#SBATCH --mail-user=ccanada6@vols.utk.edu
#
## Command(s) to run (example):
srun ./a.out