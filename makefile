CLEANER=./CLEANER/cleaner
MESHER=./MESHER/output/mesher
ROTATOR=./ROTATOR/rotator
SOLVER=./SOLVER/gid/problemtypes/Solid.gid/Solid
PDB=./CREATES_PDB/output/pdb

all: $(CLEANER) $(MESHER) $(ROTATOR) $(SOLVER) $(PDB) folder
$(CLEANER): 
	make -C ./CLEANER/
$(MESHER): 
	make -C ./MESHER/
$(ROTATOR): 
	make -C ./ROTATOR/
$(SOLVER): 
	make -C ./SOLVER/build/gcc/
$(PDB): 
	make -C ./CREATES_PDB/
folder: ./bin ./bin/cleaner ./bin/mesher ./bin/rotator ./bin/Solid ./bin/pdb
./bin:
	mkdir ./bin
./bin/cleaner:
	cp $(CLEANER) ./bin/cleaner
./bin/mesher:
	cp $(MESHER) ./bin/mesher
./bin/rotator:
	cp $(ROTATOR) ./bin/rotator
./bin/Solid:
	cp $(SOLVER) ./bin/Solid
./bin/pdb:
	cp $(PDB) ./bin/pdb
clean:
	rm -f *.pdb
	rm -f *.log
	rm -f *.dat
	rm -f *.msh
	rm -f *.res
	rm -f *.vdb
	rm -f ./bin/*
	make -C ./CLEANER/ clean
	make -C ./MESHER/ clean
	make -C ./ROTATOR/ clean
	make -C ./SOLVER/build/gcc/ clean
	make -C ./CREATES_PDB/ clean
shear:
	./bin/cleaner ./CLEANER/input/1cwp_full.vdb ./T3_1cwp_full.vdb
	./bin/mesher ./T3_1cwp_full.vdb 3 1 16.00 2 0 1CWP 15.00 0.000 0.0200
	./bin/Solid ./2_T3_1CWP 2
	./bin/pdb ./T3_1cwp_full.vdb ./2_T3_1CWP.post.msh ./2_T3_1CWP.post.res ./2_T3_1CWP.pdb 5
nanoindentation:
	./bin/cleaner ./CLEANER/input/1cwp_full.vdb ./T3_1cwp_full.vdb
	./bin/mesher ./T3_1cwp_full.vdb 3 1 16.00 2 0 1CWP 15.00 0.000 0.0200
	./bin/rotator ./2_T3_1CWP.geometry.dat ./2_T3_1CWP.geometry.dat
	./bin/Solid ./2_T3_1CWP 2
	./bin/pdb ./T3_1cwp_full.vdb ./2_T3_1CWP.post.msh ./2_T3_1CWP.post.res ./2_T3_1CWP.pdb 5

