#include "biomesh/VoxelMeshGenerator.hpp"
#include "biomesh/MeshExporter.hpp"
#include "biomesh/PDBParser.hpp"
#include "biomesh/AtomBuilder.hpp"
#include "biomesh/VoxelGrid.hpp"

#include <exception>
#include <iomanip>
#include <iostream>
#include <string>

using namespace biomesh;

namespace {

enum class MeshMode {
    Occupied,
    Empty,
    Both
};

struct CliOptions {
    std::string pdbFile;
    double voxelSize = 0.0;
    MeshMode meshMode = MeshMode::Occupied;
    double padding = 2.0;
    double inflateFactor = 1.0;
    std::string outputFormat = "gid";
    std::string output;
    std::string occupiedOutput;
    std::string emptyOutput;
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <pdb_file> <voxel_size> [options]\n\n";
    std::cout << "Required arguments:\n";
    std::cout << "  pdb_file                         Path to input PDB file\n";
    std::cout << "  voxel_size                       Positive voxel edge length in Angstroms\n\n";
    std::cout << "Options:\n";
    std::cout << "  --mesh <occupied|empty|both>     Mesh mode (default: occupied)\n";
    std::cout << "  --padding <number>               Padding around bounds in Angstroms (default: 2.0)\n";
    std::cout << "  --inflate-factor <number>        Atom radius scale factor (default: 1.0)\n";
    std::cout << "  --format <gid|stl>               Mesh export format (default: gid)\n";
    std::cout << "  --output <file>                  Required for occupied/empty modes\n";
    std::cout << "  --occupied-output <file>         Required for both mode\n";
    std::cout << "  --empty-output <file>            Required for both mode\n";
}

MeshMode parseMeshMode(const std::string& value) {
    if (value == "occupied") return MeshMode::Occupied;
    if (value == "empty") return MeshMode::Empty;
    if (value == "both") return MeshMode::Both;
    throw std::runtime_error("Error: Invalid value for --mesh. Expected occupied, empty, or both");
}

double parseDouble(const std::string& raw, const std::string& optionName) {
    try {
        return std::stod(raw);
    } catch (const std::exception&) {
        throw std::runtime_error("Error: Invalid numeric value for " + optionName + ": " + raw);
    }
}

CliOptions parseArgs(int argc, char* argv[]) {
    if (argc < 3) {
        throw std::runtime_error("Error: Missing required arguments");
    }

    CliOptions options;
    options.pdbFile = argv[1];
    options.voxelSize = parseDouble(argv[2], "voxel_size");

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--mesh") {
            if (++i >= argc) throw std::runtime_error("Error: Missing value for --mesh");
            options.meshMode = parseMeshMode(argv[i]);
        } else if (arg == "--padding") {
            if (++i >= argc) throw std::runtime_error("Error: Missing value for --padding");
            options.padding = parseDouble(argv[i], "--padding");
        } else if (arg == "--inflate-factor") {
            if (++i >= argc) throw std::runtime_error("Error: Missing value for --inflate-factor");
            options.inflateFactor = parseDouble(argv[i], "--inflate-factor");
        } else if (arg == "--format") {
            if (++i >= argc) throw std::runtime_error("Error: Missing value for --format");
            options.outputFormat = argv[i];
        } else if (arg == "--output") {
            if (++i >= argc) throw std::runtime_error("Error: Missing value for --output");
            options.output = argv[i];
        } else if (arg == "--occupied-output") {
            if (++i >= argc) throw std::runtime_error("Error: Missing value for --occupied-output");
            options.occupiedOutput = argv[i];
        } else if (arg == "--empty-output") {
            if (++i >= argc) throw std::runtime_error("Error: Missing value for --empty-output");
            options.emptyOutput = argv[i];
        } else {
            throw std::runtime_error("Error: Unknown option: " + arg);
        }
    }

    if (options.voxelSize <= 0.0) {
        throw std::runtime_error("Error: Voxel size must be positive");
    }
    if (options.padding < 0.0) {
        throw std::runtime_error("Error: Padding cannot be negative");
    }
    if (options.inflateFactor <= 0.0) {
        throw std::runtime_error("Error: Inflate factor must be positive");
    }

    if (options.outputFormat != "gid" && options.outputFormat != "stl") {
        throw std::runtime_error("Error: Invalid value for --format. Expected gid or stl");
    }

    if (options.meshMode == MeshMode::Both) {
        if (!options.output.empty()) {
            throw std::runtime_error("Error: --output cannot be used when --mesh is both");
        }
        if (options.occupiedOutput.empty() || options.emptyOutput.empty()) {
            throw std::runtime_error("Error: --occupied-output and --empty-output are required when --mesh is both");
        }
        if (options.occupiedOutput == options.emptyOutput) {
            throw std::runtime_error("Error: Occupied and empty output paths must be different");
        }
    } else {
        if (options.output.empty()) {
            throw std::runtime_error("Error: --output is required when --mesh is occupied or empty");
        }
        if (!options.occupiedOutput.empty() || !options.emptyOutput.empty()) {
            throw std::runtime_error("Error: --occupied-output and --empty-output are only valid when --mesh is both");
        }
    }

    return options;
}

const char* modeName(MeshMode mode) {
    switch (mode) {
        case MeshMode::Occupied: return "occupied";
        case MeshMode::Empty: return "empty";
        case MeshMode::Both: return "both";
    }
    return "unknown";
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        CliOptions options = parseArgs(argc, argv);

        std::cout << "BioMesh - Unified Preprocessing Pipeline\n";
        std::cout << "=======================================\n\n";
        std::cout << "Configuration:\n";
        std::cout << "  Mesh mode: " << modeName(options.meshMode) << "\n";
        std::cout << "  Voxel size: " << options.voxelSize << " Å\n";
        std::cout << "  Padding: " << options.padding << " Å\n";
        std::cout << "  Inflate factor: " << options.inflateFactor << "\n";
        std::cout << "  Format: " << options.outputFormat << "\n\n";

        std::cout << "[1/3] Loading PDB file: " << options.pdbFile << "\n";
        auto basicAtoms = PDBParser::parsePDBFile(options.pdbFile);
        std::cout << "      Loaded " << basicAtoms.size() << " atoms\n\n";

        std::cout << "[2/3] Enriching atoms with physical properties\n";
        AtomBuilder builder(options.inflateFactor);
        auto enrichedAtoms = builder.buildAtoms(basicAtoms);
        std::cout << "      Enriched " << enrichedAtoms.size() << " atoms\n\n";

        std::cout << "[3/3] Building voxel grid\n";
        VoxelGrid voxelGrid(enrichedAtoms, options.voxelSize, options.padding);
        voxelGrid.printStatistics();

        if (options.meshMode == MeshMode::Occupied) {
            std::cout << "\nGenerating hexahedral mesh from occupied voxels...\n";
            HexMesh mesh = VoxelMeshGenerator::generateHexMesh(voxelGrid);

            std::cout << "  Generated mesh:\n";
            std::cout << "    Nodes: " << mesh.getNodeCount() << "\n";
            std::cout << "    Elements: " << mesh.getElementCount() << "\n";

            int occupiedVoxelCount = voxelGrid.getOccupiedVoxelCount();
            int theoreticalNodes = occupiedVoxelCount * 8;
            double efficiency = 0.0;
            if (theoreticalNodes > 0) {
                efficiency = (1.0 - static_cast<double>(mesh.getNodeCount()) / theoreticalNodes) * 100.0;
            }
            std::cout << "    Node sharing efficiency: " << std::fixed << std::setprecision(1)
                      << efficiency << "%\n\n";

            if (mesh.getElementCount() > 100000) {
                std::cout << "WARNING: Large mesh detected (" << mesh.getElementCount()
                          << " elements). File may be large.\n\n";
            }

            std::cout << "Exporting to format [" << options.outputFormat << "]: " << options.output << "\n";
            bool success = MeshExporter::exportMesh(mesh, options.output, options.outputFormat);
            if (!success) {
                std::cerr << "  Export failed!\n";
                return 1;
            }

            std::cout << "  Export successful!\n";
            std::cout << "\nMesh file written to: " << options.output << "\n";
            return 0;
        }

        throw std::runtime_error("Error: mesh mode not implemented yet in this ticket: " + std::string(modeName(options.meshMode)));
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n\n";
        printUsage(argv[0]);
        return 1;
    }
}
