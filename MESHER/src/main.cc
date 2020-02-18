#include "./mesher/mesher.h"

/**
 *This program generates a capside mesh by using a vdb input file
 *The main idea is generate a mesh composed by spheres representing different kind of 
 *atoms and then determine the octree hexahedral elements intersecting the spherical mesh.
 *The parameters receied by the program are:
 *     1-Name of the vdb input file
 *     2-Virus type (T3 , T4 , T7,etc)
 *     3-Type of solution ( 1 = VDW , 2 = SAS )
 *     4-Resolution of the mesh in Amstrongs
 *     5-Number of fold to align with the Y axis (5,3 or 2)
 *     6-Number of id to align with Y axis (0-11)
 *     7-Virus name (1CWP,4G93,3IZG,3J4U)
 *     8-Cone amplitude in degrees (it is converted to radians by the programm)
 *     9-Variation in the proportion of loaded volume (positive or negative)
 *     10-Young modulus used in the simulation
 */
int main( int argc , char** argv ){
	double t_ini = clock();
	std::cout << " 0/100 START MESHING " << std::endl;
	Mesher* com = new Mesher( argc , argv );
	com->ReadVdbFile(  );
	com->CalculateRefinementAndPercentage();
	com->SaveVdbOnGiDMesh(  );
  com->ScaleCapside(  );
	std::cout << " 20/100 VDB READED " << std::endl;
	com->AssignAtomsOnLocalRoot( );
	std::cout << " 40/100 ATOMS ASSIGNED " << std::endl;
	com->RefineLocalRoot();
	com->SetAllOctreeNodes();
	std::cout << " 60/100 OCTREE REFINED " << std::endl;
	com->SetLoadedAndFixedElements();	
	std::cout << " 80/100 BOUNDARY CONDITIONS SET  " << std::endl;
	com->PrintDataFilesForFEMT(  );
	std::cout << " 100/100 PROBLEMA GUARDADO " << std::endl;
	std::cout << "  Time " << (clock() - t_ini)/((double)CLOCKS_PER_SEC) << std::endl;
	delete com;
  return 0;
}
