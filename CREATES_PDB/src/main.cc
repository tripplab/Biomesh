#include "./interpolator/interpolator.h"

/**
 *This program reads the results of a simulation with a capside and calculates the
 *magnitude of the deformation vector from each atom and also it von misses and stores it
 *int he PDB format using columns Occupancy(55-60) and Temperature factor(61-66), the
 *values to be set on the PDB file are calculated using the average of all the elements 
 *that are intersected by the atoms and then is calculated its natural logarithm
 *The parameters received by the program are:
 *     1-Name of the vdb input file
 *		 2-Name of the mesh file (using GiD format)
 *     3-Name of the results file (using GiD format)
 *     4-Name of the output file with pdb extension
 *		 5-Refinement level 
 */
int main( int argc , char** argv ){
	std::cout << " 0/100 READING DATA " << std::endl;
	Interpolator* com = new Interpolator( argc , argv );
	std::cout << " 17/100 TRANSLATING RESULTS TO NODES " << std::endl;
	com->TranslateAllResultsToNodes(  );
	std::cout << " 34/100 ASSIGNING ATOMS ON LOCAL ROOT " << std::endl;
	com->AssignAtomsOnLocalRoot( );
	std::cout << " 51/100 ASSIGNING RESULTS ON LOCAL ROOT " << std::endl;
	com->AssignHexahedralsOnLocalRoot( );
	std::cout << " 68/100 REFINNNING LOCAL ROOT " << std::endl;
	com->RefineLocalRoot();
	std::cout << " 85/100 INTERPOLATING RESULTS TO ATOMS " << std::endl;
	com->InterpolateResultsToAtomsAndSavePdb();
	std::cout << " 100/100 MESH SAVED " << std::endl;
	delete com;
  return 0;
}
