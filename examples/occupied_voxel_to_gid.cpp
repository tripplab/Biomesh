#include "biomesh/VoxelMeshGenerator.hpp"
#include "biomesh/MeshExporter.hpp"
#include "biomesh/VoxelGrid.hpp"
#include "biomesh/PDBParser.hpp"
#include "biomesh/AtomBuilder.hpp"
#include <iostream>
#include <iomanip>
#include <string>

using namespace biomesh;

void printUsage(const char* programName) {
    std::cout << "\nUsage: " << programName << " <pdb_file> <voxel_size> <output_file> [padding] [inflate_factor] [format]\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  pdb_file     : Path to input PDB file\n";
    std::cout << "  voxel_size   : Edge length of voxels in Angstroms (e.g., 1.0)\n";
    std::cout << "  output_file  : Output mesh file path (e.g., occupied_mesh.msh or occupied_mesh.stl)\n";
    std::cout << "  padding      : Optional padding around bounding box in Angstroms (default: 2.0)\n";
    std::cout << "  inflate_factor : Optional radius scale factor (default: 1.0)\n";
    std::cout << "  format       : Optional export format gid|stl (default: gid)\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << programName << " protein.pdb 1.0 occupied_mesh.msh 2.0 1.0 gid\n\n";
    std::cout << "Note: Occupied voxel meshes represent the molecule volume.\n";
    std::cout << "      Use larger voxel sizes for initial testing.\n\n";
}

int main(int argc, char* argv[]) {
    std::cout << "BioMesh - Occupied Voxel Mesh Generator with Mesh Export\n";
    std::cout << "========================================================\n\n";
    
    // Check arguments
    if (argc < 4) {
        std::cerr << "Error: Insufficient arguments\n";
        printUsage(argv[0]);
        return 1;
    }
    
    std::string pdbFile = argv[1];
    double voxelSize = 0.0;
    std::string outputFile = argv[3];
    double padding = 2.0; // Default padding
    double inflateFactor = 1.0; // Default atom radius scale
    std::string outputFormat = "gid"; // Default format
    
    // Parse voxel size
    try {
        voxelSize = std::stod(argv[2]);
        if (voxelSize <= 0.0) {
            std::cerr << "Error: Voxel size must be positive\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid voxel size: " << argv[2] << "\n";
        return 1;
    }
    
    // Parse optional padding
    if (argc > 4) {
        try {
            padding = std::stod(argv[4]);
            if (padding < 0.0) {
                std::cerr << "Error: Padding cannot be negative\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid padding value: " << argv[4] << "\n";
            return 1;
        }
    }

    // Parse optional inflate factor
    if (argc > 5) {
        try {
            inflateFactor = std::stod(argv[5]);
            if (inflateFactor <= 0.0) {
                std::cerr << "Error: Inflate factor must be positive\n";
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid inflate factor value: " << argv[5] << "\n";
            return 1;
        }
    }
    
    // Parse optional output format
    if (argc > 6) {
        outputFormat = argv[6];
    }
    
    try {
        // Step 1: Parse PDB file
        std::cout << "Loading PDB file: " << pdbFile << "\n";
        auto basicAtoms = PDBParser::parsePDBFile(pdbFile);
        std::cout << "  Loaded " << basicAtoms.size() << " atoms\n\n";
        
        // Step 2: Enrich atoms with physical properties
        std::cout << "Enriching atoms with physical properties...\n";
        AtomBuilder builder(inflateFactor);
        auto enrichedAtoms = builder.buildAtoms(basicAtoms);
        std::cout << "  Enriched " << enrichedAtoms.size() << " atoms\n\n";
        
        // Step 3: Create voxel grid
        std::cout << "Creating voxel grid...\n";
        std::cout << "  Voxel size: " << voxelSize << " Å\n";
        std::cout << "  Padding: " << padding << " Å\n";
        std::cout << "  Inflate factor: " << inflateFactor << "\n";
        VoxelGrid voxelGrid(enrichedAtoms, voxelSize, padding);
        
        std::cout << "\n";
        voxelGrid.printStatistics();
        std::cout << "\n";
        
        // Step 4: Generate hexahedral mesh from occupied voxels
        // Note: VoxelMeshGenerator::generateHexMesh() creates mesh elements only for
        // voxels that intersect with atom spheres (occupied voxels), representing
        // the molecule volume. For void space mesh, use EmptyVoxelMeshGenerator.
        std::cout << "Generating hexahedral mesh from occupied voxels...\n";
        HexMesh mesh = VoxelMeshGenerator::generateHexMesh(voxelGrid);
        
        std::cout << "  Generated mesh:\n";
        std::cout << "    Nodes: " << mesh.getNodeCount() << "\n";
        std::cout << "    Elements: " << mesh.getElementCount() << "\n";
        
        // Calculate mesh statistics
        int occupiedVoxelCount = voxelGrid.getOccupiedVoxelCount();
        int theoreticalNodes = occupiedVoxelCount * 8;
        double efficiency = 0.0;
        if (theoreticalNodes > 0) {
            efficiency = (1.0 - (double)mesh.getNodeCount() / theoreticalNodes) * 100.0;
        }
        std::cout << "    Node sharing efficiency: " << std::fixed << std::setprecision(1) 
                  << efficiency << "%\n\n";
        
        // Warn about large meshes
        if (mesh.getElementCount() > 100000) {
            std::cout << "WARNING: Large mesh detected (" << mesh.getElementCount() 
                      << " elements). File may be large.\n\n";
        }
        
        // Step 5: Export to selected format
        std::cout << "Exporting to format [" << outputFormat << "]: " << outputFile << "\n";
        bool success = MeshExporter::exportMesh(mesh, outputFile, outputFormat);
        
        if (success) {
            std::cout << "  Export successful!\n";
            std::cout << "\nMesh file written to: " << outputFile << "\n";
            std::cout << "You can now open this file in tools compatible with the selected output format.\n";
            return 0;
        } else {
            std::cerr << "  Export failed!\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
        return 1;
    }
}
