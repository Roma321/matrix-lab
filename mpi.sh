echo 'Введите количество потоков'
read n
mpic++ mpi.cpp -o mpi
mpiexec -n $n ./mpi 