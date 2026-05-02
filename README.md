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
git clone -b dev01 --single-branch --depth 1 https://github.com/tripplab/Biomesh.git
cd BioMesh
git branch
bash -lc 'mkdir -p build && cd build && cmake .. && cmake --build . -j'
```

### Build with OpenMP multithreading

OpenMP is enabled automatically when CMake can find an OpenMP-capable compiler/runtime.

```bash
# from repo root
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
```

During `cmake ..`, look for one of these status lines:

- `OpenMP found - parallel mesh generation enabled`
- `OpenMP not found - mesh generation will run serially`

If OpenMP is not found, install/update your compiler toolchain and OpenMP runtime, then re-run CMake in a clean build directory.

#### Installing OpenMP in a micromamba environment

If you are building inside micromamba, install compiler + OpenMP runtime packages from `conda-forge` in your active environment:

```bash
# create/activate env (example)
micromamba create -n biomesh -c conda-forge cmake ninja cxx-compiler
micromamba activate biomesh

# Linux: GNU OpenMP runtime
micromamba install -n biomesh -c conda-forge libgomp

# macOS (clang): LLVM OpenMP runtime
micromamba install -n biomesh -c conda-forge llvm-openmp
```

Then configure/build from a fresh build directory:

```bash
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
```

Re-check CMake output for `OpenMP found - parallel mesh generation enabled`.

#### Runtime thread count (OpenMP)

Yes — OpenMP-enabled builds may still run with one thread unless the runtime decides otherwise.
Set the thread count explicitly before running `biomesh`:

```bash
# use all visible cores
export OMP_NUM_THREADS=$(nproc)      # Linux
# export OMP_NUM_THREADS=$(sysctl -n hw.ncpu)   # macOS

# or set a fixed count
export OMP_NUM_THREADS=8

./biomesh ../data/test_peptide.pdb 1.0 --mesh occupied --output occupied.msh
```

Useful diagnostics:

```bash
# bind OpenMP threads and print runtime settings
export OMP_PROC_BIND=true
export OMP_PLACES=cores
export OMP_DISPLAY_ENV=VERBOSE
```

Note: in BioMesh, OpenMP currently parallelizes voxel corner-node generation loops; other stages remain serial, so speedup may be limited for small inputs.

##### Linux/macOS troubleshooting (micromamba)

If CMake still cannot detect OpenMP in micromamba, point CMake to the active environment and explicitly choose compilers from that environment:

```bash
# make sure env is active first
micromamba activate biomesh

# Linux/macOS: use env prefix + env compilers
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" \
  -DCMAKE_C_COMPILER="$CONDA_PREFIX/bin/x86_64-conda-linux-gnu-cc" \
  -DCMAKE_CXX_COMPILER="$CONDA_PREFIX/bin/x86_64-conda-linux-gnu-c++"
```

On macOS with clang-based toolchains, this variant is often sufficient:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++
```

Tip: if compiler paths differ in your environment, inspect `$CONDA_PREFIX/bin` and substitute the exact compiler executable names.

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
