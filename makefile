CCOMP=g++
CFLAGS=-std=c++11 -Wall
LFLAGS=-lm
EJECUTABLE=./ejecutable

all: $(EJECUTABLE)
$(EJECUTABLE): ./main.o
	$(CCOMP) $(LFLAGS) ./main.o -o $(EJECUTABLE)
./obj/main.o: ./main.cc
	$(CCOMP) $(CFLAGS) -c ./main.cc -o ./main.o
clean:
	rm -f ./ejecutable
	rm -f ./main.o
run:
	./ejecutable  ./input/1cwp_full.vdb T3_1cwp_full.vdb
