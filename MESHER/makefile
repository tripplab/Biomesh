CCOMP=g++
CFLAGS=-std=c++11 -Wall -fopenmp
LFLAGS=-fopenmp -lm
EJECUTABLE=./output/mesher

all: $(EJECUTABLE)
$(EJECUTABLE): ./obj/file/file.o ./obj/geometry/geometry.o ./obj/boundary/boundary.o ./obj/mesh/mesh.o ./obj/capside/capside.o ./obj/octree/octree_driver.o ./obj/mesher/mesher.o ./obj/main.o
	$(CCOMP) $(LFLAGS) ./obj/file/file.o ./obj/geometry/geometry.o ./obj/boundary/boundary.o ./obj/mesh/mesh.o ./obj/capside/capside.o ./obj/octree/octree_driver.o ./obj/mesher/mesher.o ./obj/main.o -o $(EJECUTABLE)
./obj/file/file.o: ./src/file/file.cc
	$(CCOMP) $(CFLAGS) -c ./src/file/file.cc -o ./obj/file/file.o
./obj/geometry/geometry.o: ./src/geometry/geometry.cc
	$(CCOMP) $(CFLAGS) -c ./src/geometry/geometry.cc -o ./obj/geometry/geometry.o
./obj/boundary/boundary.o: ./src/boundary/boundary.cc
	$(CCOMP) $(CFLAGS) -c ./src/boundary/boundary.cc -o ./obj/boundary/boundary.o
./obj/mesh/mesh.o: ./src/mesh/mesh.cc
	$(CCOMP) $(CFLAGS) -c ./src/mesh/mesh.cc -o ./obj/mesh/mesh.o
./obj/capside/capside.o: ./src/capside/capside.cc
	$(CCOMP) $(CFLAGS) -c ./src/capside/capside.cc -o ./obj/capside/capside.o
./obj/octree/octree_driver.o: ./src/octree/octree_driver.cc
	$(CCOMP) $(CFLAGS) -c ./src/octree/octree_driver.cc -o ./obj/octree/octree_driver.o
./obj/mesher/mesher.o: ./src/mesher/mesher.cc
	$(CCOMP) $(CFLAGS) -c ./src/mesher/mesher.cc -o ./obj/mesher/mesher.o
./obj/main.o: ./src/main.cc
	$(CCOMP) $(CFLAGS) -c ./src/main.cc -o ./obj/main.o
clean:
	rm -f ./*.msh
	rm -f ./*.dat
	rm -f ./*.post.*
	rm -f ./*.txt
	rm -f ./*.log
	rm -f ./output/mesher
	rm -f ./obj/main.o
	rm -f ./obj/file/file.o
	rm -f ./obj/geometry/geometry.o
	rm -f ./obj/boundary/boundary.o
	rm -f ./obj/mesh/mesh.o
	rm -f ./obj/octree/octree_driver.o
	rm -f ./obj/mesher/mesher.o
	rm -f ./obj/capside/capside.o
run:
	./output/mesher ./input/T3_1cwp_full.vdb 3 1 16.00 5 0 1CWP 15.00 0.000 0.0200


