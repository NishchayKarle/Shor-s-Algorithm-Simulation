#!/bin/bash
#
#SBATCH --mail-user=toberoi@uchicago.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name=project2-time
#SBATCH --output=/home/toberoi/project-2-tinaoberoi/proj2/benchmark/data.txt
#SBATCH --error=./%j.%N.stderr
#SBATCH --chdir=/home/toberoi/project-2-tinaoberoi/proj2/editor
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --mem-per-cpu=900
#SBATCH --exclusive
#SBATCH --time=03:45:00