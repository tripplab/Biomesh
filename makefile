CLEANER=./CLEANER/ejecutable
MESHER=./MESHER/output/mesher
ROTATOR=./ROTATOR/rotator
SOLVER=./SOLVER/gid/problemtypes/Solid.gid/Solid
PDB=./CREATES_PDB/output/ejecutable

all: $(CLEANER) $(MESHER) $(ROTATOR) $(SOLVER) $(PDB)
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
clean:
	rm -f *.pdb
	rm -f *.log
	rm -f *.dat
	rm -f *.msh
	rm -f *.res
	rm -f *.vdb
	make -C ./CLEANER/ clean
	make -C ./MESHER/ clean
	make -C ./ROTATOR/ clean
	make -C ./SOLVER/build/gcc/ clean
	make -C ./CREATES_PDB/ clean
shear:
	$(CLEANER) ./CLEANER/input/1cwp_full.vdb ./T3_1cwp_full.vdb
	$(MESHER) ./T3_1cwp_full.vdb 3 1 16.00 2 0 1CWP 15.00 0.000 0.0200
	$(SOLVER) ./2_T3_1CWP 2
	$(PDB) ./T3_1cwp_full.vdb ./2_T3_1CWP.post.msh ./2_T3_1CWP.post.res ./2_T3_1CWP.pdb 5
nanoindentation:
	$(CLEANER) ./CLEANER/input/1cwp_full.vdb ./T3_1cwp_full.vdb
	$(MESHER) ./T3_1cwp_full.vdb 3 1 16.00 2 0 1CWP 15.00 0.000 0.0200
	$(ROTATOR) ./2_T3_1CWP.geometry.dat ./2_T3_1CWP.geometry.dat
	$(SOLVER) ./2_T3_1CWP 2
	$(PDB) ./T3_1cwp_full.vdb ./2_T3_1CWP.post.msh ./2_T3_1CWP.post.res ./2_T3_1CWP.pdb 5

