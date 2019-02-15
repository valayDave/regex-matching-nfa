#!/bin/bash

#SBATCH -N 1                      		# Number of nodes
#SBATCH -n 4                      		# Number of cores
#SBATCH -q aggressive
#SBATCH -t 0-12:00                 		# Wall time (D-HH:MM)
#SBATCH -A cse330spring2019        		# Account to pull cpu hours from (commented out with double # in front)
#SBATCH -o 4cores.%j.out            	 	# STDOUT (%j = JobId)
#SBATCH -e 4cores.%j.err             	# STDERR (%j = JobId)
#SBATCH --mail-type=ALL        			# Notifications for job done & fail
#SBATCH --mail-user=vddave@asu.edu   	# Send-to address

cd /home/vddave/cse330-project/
module load gcc/4.9x
make

echo "=============================== SERIAL CODE ==============================="
./search "and" Testing-Files/anne.txt Testing-Files/tale.txt Testing-Files/glass.txt
echo "==========================================================================="

echo "=============================== PARALLEL CODE ============================="
./psearch "and" Testing-Files/anne.txt Testing-Files/tale.txt Testing-Files/glass.txt
echo "==========================================================================="
