#!/bin/bash
#
#SBATCH --mail-user=nishchaykarle@uchicago.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name=project2-time
#SBATCH --output=./%j.%N.stdout
#SBATCH --error=./%j.%N.stderr
#SBATCH --chdir=/home/nishchaykarle/Quantum-Final/Shor-s-Algorithm-Simulation/
#SBATCH --partition=caslake
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --mem-per-cpu=900
#SBATCH --exclusive
#SBATCH --time=00:45:00
#SBATCH --account=mpcs52018

for i in {1..20}
do
	./test_serial >> time_serial.txt
done

for i in {1..20}
do
	./test_omp >> time_omp.txt
done
