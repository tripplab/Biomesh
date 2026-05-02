# BioMesh

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.14+-064F8C.svg)](https://cmake.org/)

BioMesh is a C++17 toolkit for generating **hexahedral voxel meshes** from molecular structures in PDB format. It supports:

- **Occupied meshes** (voxels intersecting molecule volume)
- **Empty meshes** (void/fluid-domain voxels around the molecule)
- Export to **GiD (`.msh`)** and **STL (`.stl`)**

The repository currently provides a unified executable (`biomesh`), compatibility wrappers for legacy commands, a Bash workflow runner (`biomesh.sh`), and tests.

---

## Features

### Core pipeline
- Parse ATOM/HETATM records from PDB input.
- Enrich atoms with atomic radii/masses.
- Build padded 3D voxel grid.
- Generate occupied and/or empty hexahedral meshes.
- Export meshes to GiD or STL.

### Developer-focused capabilities
- Unified CLI with strict argument validation for occupied/empty/both workflows.
- Legacy compatibility wrappers:
  - `occupied_voxel_to_gid`
  - `empty_voxel_to_gid`
- Optional OpenMP acceleration when detected by CMake.
- Smoke test target with no external test dependency requirements.
- Extended test suite via GoogleTest (if available).

---

## Quick Start

### Build

```bash
git clone https://github.com/pausalinas/BioMesh.git
cd BioMesh
git checkout work
bash -lc 'mkdir -p build && cd build && cmake .. && cmake --build . -j'
```

### Run unified executable

```bash
# occupied only
./biomesh ../data/test_peptide.pdb 1.0 --mesh occupied --output occupied.msh

# empty only
./biomesh ../data/test_peptide.pdb 1.0 --mesh empty --output empty.msh

# both in one run
./biomesh ../data/test_peptide.pdb 1.0 --mesh both \
  --occupied-output occ.msh \
  --empty-output emp.msh
```

### Run tests

```bash
ctest --output-on-failure
```

---

## Unified CLI (`biomesh`)

### Usage

```bash
./biomesh <pdb_file> <voxel_size> [options]
```

### Options

- `--mesh <occupied|empty|both>` (default: `occupied`)
- `--padding <number>` (default: `2.0`)
- `--inflate-factor <number>` (default: `1.0`)
- `--format <gid|stl>` (default: `gid`)
- `--output <file>` (required for `occupied` or `empty`)
- `--occupied-output <file>` (required for `both`)
- `--empty-output <file>` (required for `both`)

### Output argument rules

- `--mesh occupied|empty` requires `--output`.
- `--mesh both` requires `--occupied-output` and `--empty-output`.
- `--mesh both` rejects `--output`.
- In `both` mode, occupied and empty output paths must be different.

---

## Bash Runner (`biomesh.sh`)

`biomesh.sh` orchestrates dual-mesh workflows, optional config loading, and batch processing while invoking build outputs.

### Usage

```bash
./biomesh.sh [options] <input.pdb>
```

### Frequently used options

- `-i, --input FILE`
- `-o, --output BASENAME`
- `-v, --voxel-size VALUE`
- `--inflate-factor VALUE`
- `-p, --padding VALUE`
- `-f, --format gid|stl`
- `--filter TYPE` (`none|all|protein-only|no-water|custom`)
- `--occupied true|false`
- `--empty true|false`
- `-c, --config FILE`
- `--generate-config`
- `--batch LIST`
- `-V, --verbose`

### Examples

```bash
# Generate both meshes
./biomesh.sh data/test_peptide.pdb -o peptide

# Generate occupied mesh only
./biomesh.sh data/test_peptide.pdb --empty false

# Batch
./biomesh.sh --batch data/test_peptide.pdb,data/mixed_molecules.pdb -v 1.0
```

---

## Project Structure

```text
BioMesh/
├── CMakeLists.txt
├── README.md
├── biomesh.sh
├── config/
│   └── default.conf
├── data/
│   ├── mixed_molecules.pdb
│   └── test_peptide.pdb
├── docs/
│   ├── EmptyVoxelMeshGeneration.md
│   ├── MoleculeFilter.md
│   ├── VoxelizationMeshGeneration.md
│   ├── biomesh_cli_contract.md
│   ├── ticket8_validation.md
│   └── unified_executable_plan.md
├── examples/
│   ├── empty_voxel_to_gid_wrapper.cpp
│   ├── occupied_voxel_to_gid_wrapper.cpp
│   ├── voxel_demo.cpp
│   ├── filter_demo.cpp
│   └── filter_workflow_example.cpp
├── include/biomesh/
├── src/
│   ├── biomesh.cpp
│   ├── main.cpp
│   └── ...
└── tests/
```

---

## Build and Test Matrix

### Requirements
- CMake 3.14+
- C++17 compiler (GCC/Clang/MSVC)
- Optional: OpenMP
- Optional: GoogleTest

### CMake targets
- `biomesh_lib` (core library)
- `biomesh` (unified CLI executable)
- `biomesh_example`, `voxel_demo`, `filter_demo`, `filter_workflow_example`
- `occupied_voxel_to_gid`, `empty_voxel_to_gid` (legacy wrappers)
- `biomesh_smoke_test`
- `biomesh_tests` (only when GoogleTest is found)

### Test behavior
- `SmokeNoDeps` is always added.
- `AllTests` is added only if GoogleTest is available.

---

## C++ API Snapshot

```cpp
#include "biomesh/PDBParser.hpp"
#include "biomesh/AtomBuilder.hpp"
#include "biomesh/VoxelGrid.hpp"
#include "biomesh/VoxelMeshGenerator.hpp"
#include "biomesh/EmptyVoxelMeshGenerator.hpp"
#include "biomesh/MeshExporter.hpp"

auto basicAtoms = biomesh::PDBParser::parsePDBFile("protein.pdb");
biomesh::AtomBuilder builder(1.0);
auto atoms = builder.buildAtoms(basicAtoms);

biomesh::VoxelGrid grid(atoms, 1.0, 2.0);
auto occupied = biomesh::VoxelMeshGenerator::generateHexMesh(grid);
auto empty = biomesh::EmptyVoxelMeshGenerator::generateHexMesh(grid);

biomesh::MeshExporter::exportMesh(occupied, "occupied.msh", "gid");
biomesh::MeshExporter::exportMesh(empty, "empty.stl", "stl");
```

---

## Documentation

- `docs/biomesh_cli_contract.md`
- `docs/unified_executable_plan.md`
- `docs/ticket8_validation.md`
- `docs/VoxelizationMeshGeneration.md`
- `docs/EmptyVoxelMeshGeneration.md`
- `docs/MoleculeFilter.md`
- `IMPLEMENTATION_SUMMARY.md`

---

## Contributing

1. Create a feature branch.
2. Make changes + tests.
3. Run local validation (`ctest --output-on-failure`).
4. Open a pull request.

---

## License

MIT License. See `LICENSE`.
