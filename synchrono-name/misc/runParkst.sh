#!/usr/bin/env bash
#SBATCH -t 0-00:10:00
#SBATCH -J Synchrono-parkst
#SBATCH -o syn-%j-%n.out -e syn-%j-%n.err
#SBATCH -c 2
#SBATCH -n 6 
#SBATCH --gres=gpu:1
source ~/synchrono/misc/loadsynmods.sh
nvidia-smi
mpirun -n 6 ~/synchrono/build/bin/demo_MPI_parkst
