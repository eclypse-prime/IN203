#OBJS specifies which files to compile as part of the project
OBJS = src/galaxie.o src/parametres.o src/colonisation.o

OBJS2 = src/galaxie.o src/parametres.o src/colonisation_benchmark.o

OBJS3 = src/galaxie.o src/parametres.o src/colonisation_benchmark2.o

OBJS4 = src/galaxie.o src/parametres.o src/colonisation_mpi.o

OBJS5 = src/galaxie.o src/parametres.o src/colonisation_benchmark3.o

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I"C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\include"

INCLUDE_PATHS_MPI = -I"C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\include" -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include"

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L"C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\lib"

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -O3 -march=native -Wall

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -fopenmp -lmsmpi


#This is the target that compiles our executable
ALL= colonisation.exe colonisation_benchmark.exe colonisation_benchmark2.exe colonisation_mpi.exe colonisation_benchmark3.exe


all: $(ALL)

clean:
	@del /Q /S *.o *.exe 


src/galaxie.o:	src/galaxie.cpp src/galaxie.hpp
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -c src/galaxie.cpp -o src/galaxie.o

src/parametres.o: src/parametres.cpp src/parametres.hpp src/galaxie.hpp
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -c src/parametres.cpp -o src/parametres.o

src/colonisation.o: src/colonisation.cpp src/parametres.hpp src/galaxie.hpp
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -c src/colonisation.cpp -o src/colonisation.o

src/colonisation_benchmark.o: src/colonisation_benchmark.cpp src/parametres.hpp src/galaxie.hpp
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -c src/colonisation_benchmark.cpp -o src/colonisation_benchmark.o

src/colonisation_benchmark2.o: src/colonisation_benchmark2.cpp src/parametres.hpp src/galaxie.hpp
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -c src/colonisation_benchmark2.cpp -o src/colonisation_benchmark2.o

src/colonisation_mpi.o: src/colonisation_mpi.cpp src/parametres.hpp src/galaxie.hpp
	$(CC) $(INCLUDE_PATHS_MPI) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -c src/colonisation_mpi.cpp -o src/colonisation_mpi.o

src/colonisation_benchmark3.o: src/colonisation_benchmark3.cpp src/parametres.hpp src/galaxie.hpp
	$(CC) $(INCLUDE_PATHS_MPI) $(LIBRARY_PATHS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -c src/colonisation_benchmark3.cpp -o src/colonisation_benchmark3.o

colonisation.exe : $(OBJS)
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS)  $(OBJS) -o colonisation.exe $(LINKER_FLAGS)

colonisation_benchmark.exe : $(OBJS2)
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS)  $(OBJS2) -o colonisation_benchmark.exe $(LINKER_FLAGS)

colonisation_benchmark2.exe : $(OBJS3)
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS)  $(OBJS3) -o colonisation_benchmark2.exe $(LINKER_FLAGS)

colonisation_mpi.exe : $(OBJS4)
	$(CC) $(INCLUDE_PATHS_MPI) $(LIBRARY_PATHS) $(COMPILER_FLAGS)  $(OBJS4) -o colonisation_mpi.exe $(LINKER_FLAGS)
	
colonisation_benchmark2.exe : $(OBJS5)
	$(CC) $(INCLUDE_PATHS_MPI) $(LIBRARY_PATHS) $(COMPILER_FLAGS)  $(OBJS5) -o colonisation_benchmark3.exe $(LINKER_FLAGS)


