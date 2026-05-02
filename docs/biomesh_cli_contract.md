# Unified `biomesh` CLI Contract (Ticket 1)

This note captures the baseline behavior of the current legacy executables and defines the first CLI contract for the unified `biomesh` command.

## 1) Baseline Audit (Current State)

### Legacy executables and arguments
Both legacy executables use the same positional interface:

```text
<program> <pdb_file> <voxel_size> <output_file> [padding] [inflate_factor] [format]
```

- `occupied_voxel_to_gid`: generates mesh from **occupied** voxels.
- `empty_voxel_to_gid`: generates mesh from **empty** voxels.

Shared defaults and validation in both tools:
- `padding` default: `2.0`.
- `inflate_factor` default: `1.0`.
- `format` default: `gid`.
- `voxel_size` must be `> 0`.
- `padding` must be `>= 0`.
- `inflate_factor` must be `> 0`.

### Common processing pipeline
Both binaries perform the same preprocessing stages:
1. Parse PDB file.
2. Enrich atoms with `AtomBuilder(inflateFactor)`.
3. Build `VoxelGrid(enrichedAtoms, voxelSize, padding)`.
4. Generate mesh (occupied or empty branch).
5. Export through `MeshExporter::exportMesh(mesh, outputFile, format)`.

### Current CMake targets
Current `CMakeLists.txt` registers both legacy executables directly:
- `empty_voxel_to_gid` from `examples/empty_voxel_to_gid.cpp`.
- `occupied_voxel_to_gid` from `examples/occupied_voxel_to_gid.cpp`.

No unified `biomesh` target exists yet in the current baseline.

---

## 2) Unified `biomesh` CLI Contract (Initial)

## Command shape

```text
biomesh <pdb_file> <voxel_size> [options]
```

### Required positional arguments
- `pdb_file` : input PDB file path.
- `voxel_size` : voxel edge length in Å; must be a positive number.

### Options
- `--mesh <occupied|empty|both>`
  - Default: `occupied`.
- `--padding <number>`
  - Default: `2.0`.
  - Must be `>= 0`.
- `--inflate-factor <number>`
  - Default: `1.0`.
  - Must be `> 0`.
- `--format <gid|stl>`
  - Default: `gid`.
- `--output <file>`
  - Single-output path used when `--mesh occupied` or `--mesh empty`.
- `--occupied-output <file>`
  - Explicit output path for occupied mesh (used by `--mesh both`).
- `--empty-output <file>`
  - Explicit output path for empty mesh (used by `--mesh both`).

---

## 3) Output-path rules

### A) `--mesh occupied`
- Required: `--output <file>`.
- Forbidden: `--occupied-output`, `--empty-output`.

### B) `--mesh empty`
- Required: `--output <file>`.
- Forbidden: `--occupied-output`, `--empty-output`.

### C) `--mesh both`
- Required: both `--occupied-output <file>` and `--empty-output <file>`.
- Forbidden: `--output`.
- Additional rule: resolved occupied and empty output paths must be different.

This contract intentionally requires explicit dual-output paths for `both` mode to avoid ambiguity and accidental overwrite.

---

## 4) Validation rules and expected errors

### Numeric validation
- If `voxel_size <= 0`: fail with non-zero exit and message:
  - `Error: Voxel size must be positive`
- If `padding < 0`: fail with non-zero exit and message:
  - `Error: Padding cannot be negative`
- If `inflate_factor <= 0`: fail with non-zero exit and message:
  - `Error: Inflate factor must be positive`
- If numeric parse fails: fail with non-zero exit and clear invalid-value message naming the option.

### Enum/choice validation
- If `--mesh` value is not one of `occupied|empty|both`: fail with:
  - `Error: Invalid value for --mesh. Expected occupied, empty, or both`
- If `--format` value is not `gid|stl`: fail with:
  - `Error: Invalid value for --format. Expected gid or stl`

### Output argument validation
- `occupied|empty` mode without `--output`: fail with:
  - `Error: --output is required when --mesh is occupied or empty`
- `both` mode without one of dual outputs: fail with:
  - `Error: --occupied-output and --empty-output are required when --mesh is both`
- `both` mode with same resolved path for both outputs: fail with:
  - `Error: Occupied and empty output paths must be different`
- Any forbidden output option used for the selected mode: fail with actionable mode-specific message.

---

## 5) Legacy-to-unified command mapping

## Occupied (legacy → new)

```bash
# legacy
occupied_voxel_to_gid protein.pdb 1.0 occupied_mesh.msh 2.0 1.0 gid

# unified
biomesh protein.pdb 1.0 \
  --mesh occupied \
  --output occupied_mesh.msh \
  --padding 2.0 \
  --inflate-factor 1.0 \
  --format gid
```

## Empty (legacy → new)

```bash
# legacy
empty_voxel_to_gid protein.pdb 1.0 empty_mesh.stl 2.0 1.0 stl

# unified
biomesh protein.pdb 1.0 \
  --mesh empty \
  --output empty_mesh.stl \
  --padding 2.0 \
  --inflate-factor 1.0 \
  --format stl
```

## New both-mode example

```bash
biomesh protein.pdb 1.0 \
  --mesh both \
  --occupied-output occupied_mesh.msh \
  --empty-output empty_mesh.msh \
  --padding 2.0 \
  --inflate-factor 1.0 \
  --format gid
```

