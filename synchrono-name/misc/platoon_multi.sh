#!/usr/bin/env bash
for i in {2..11}
do
    mpirun --oversubscribe -n $i bin/demo_MPI_platoon --mca btl ^uct > output_platoon.txt
    grep 'real time' output_platoon.txt | cut -d ' ' -f 3 >> times_platoon.txt
    echo "Finished platoon $i"
done

for i in {3..12}
do
    mpirun --oversubscribe -n $i bin/demo_MPI_parkst --mca btl ^uct > output_park.txt
    grep 'real time' output_park.txt | cut -d ' ' -f 3 >> times_park.txt
    echo "Finished park st $i"
done
echo "All done!!"