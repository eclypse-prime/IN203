@echo off
echo OMP+Threads > results/benchmark.csv
echo Threads, Temps, FPS >> results/benchmark.csv
FOR /L %%i IN (1,1,16) DO (
	SET OMP_NUM_THREADS=%%i
	colonisation_benchmark >> results/benchmark.csv
)

echo OMP >> results/benchmark.csv
echo Threads, Temps, FPS >> results/benchmark.csv
FOR /L %%i IN (1,1,16) DO (
	SET OMP_NUM_THREADS=%%i
	colonisation_benchmark2 >> results/benchmark.csv
)

echo MPI >> results/benchmark.csv
SET OMP_NUM_THREADS=1
echo Threads, Temps, FPS >> results/benchmark.csv
FOR /L %%i IN (2,1,16) DO (
	mpiexec -n %%i colonisation_benchmark3.exe >> results/benchmark.csv
)