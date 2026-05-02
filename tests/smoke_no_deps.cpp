#include "biomesh/AtomBuilder.hpp"
#include "biomesh/EmptyVoxelMeshGenerator.hpp"
#include "biomesh/PDBParser.hpp"
#include "biomesh/VoxelGrid.hpp"
#include "biomesh/VoxelMeshGenerator.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

int main() {
    try {
        const std::filesystem::path pdbPath = std::filesystem::path(BIOMESH_SOURCE_DIR) / "data" / "test_peptide.pdb";
        if (!std::filesystem::exists(pdbPath)) {
            std::cerr << "Smoke test fixture missing: " << pdbPath << "\n";
            return 1;
        }

        auto basicAtoms = biomesh::PDBParser::parsePDBFile(pdbPath.string());
        if (basicAtoms.empty()) {
            std::cerr << "Smoke test failed: parser returned no atoms\n";
            return 1;
        }

        biomesh::AtomBuilder builder(1.0);
        auto enrichedAtoms = builder.buildAtoms(basicAtoms);
        biomesh::VoxelGrid voxelGrid(enrichedAtoms, 1.0, 2.0);

        auto occupiedMesh = biomesh::VoxelMeshGenerator::generateHexMesh(voxelGrid);
        auto emptyMesh = biomesh::EmptyVoxelMeshGenerator::generateHexMesh(voxelGrid);

        if (occupiedMesh.getNodeCount() == 0 || occupiedMesh.getElementCount() == 0) {
            std::cerr << "Smoke test failed: occupied mesh is empty\n";
            return 1;
        }
        if (emptyMesh.getNodeCount() == 0 || emptyMesh.getElementCount() == 0) {
            std::cerr << "Smoke test failed: empty mesh is empty\n";
            return 1;
        }

        std::cout << "Smoke test passed: occupied=" << occupiedMesh.getElementCount()
                  << " empty=" << emptyMesh.getElementCount() << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Smoke test exception: " << ex.what() << "\n";
        return 1;
    }
}
