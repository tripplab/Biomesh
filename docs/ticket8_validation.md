# Ticket 8 Validation Report — Unified `biomesh` Parity & Regression

Date: 2026-05-02 (UTC)

## Scope covered
- Mode coverage: `occupied`, `empty`, `both`.
- Format coverage: GiD (`gid`) and STL (`stl`).
- Parity checks against legacy compatibility entry points:
  - `occupied_voxel_to_gid`
  - `empty_voxel_to_gid`
- Regression checks:
  - Build/configure succeeds.
  - CTest smoke test target succeeds.

## Environment
- Source tree: `/workspace/Biomesh`
- Build directory: `build/`
- Representative input: `data/test_peptide.pdb`

## Commands executed

### 1) Configure, build, and run automated tests
```bash
cmake -S . -B build
cmake --build build -j4
ctest --test-dir build --output-on-failure
```

Outcome:
- Configure/build succeeded.
- `SmokeNoDeps` passed.
- GoogleTest suite was not built in this environment because `GTest` was not found (already handled by CMake as optional).

### 2) Occupied parity (GiD)
```bash
./build/biomesh data/test_peptide.pdb 1.0 --mesh occupied --output build/ticket8/new_occ.gid --format gid
./build/occupied_voxel_to_gid data/test_peptide.pdb 1.0 build/ticket8/legacy_occ.gid 2.0 1.0 gid
```

Comparison:
```bash
wc -c build/ticket8/new_occ.gid build/ticket8/legacy_occ.gid
sha256sum build/ticket8/new_occ.gid build/ticket8/legacy_occ.gid
```

Result:
- Byte size identical: `17686` bytes each.
- SHA-256 identical.

### 3) Empty parity (STL)
```bash
./build/biomesh data/test_peptide.pdb 1.0 --mesh empty --output build/ticket8/new_empty.stl --format stl
./build/empty_voxel_to_gid data/test_peptide.pdb 1.0 build/ticket8/legacy_empty.stl 2.0 1.0 stl
```

Comparison:
```bash
wc -c build/ticket8/new_empty.stl build/ticket8/legacy_empty.stl
sha256sum build/ticket8/new_empty.stl build/ticket8/legacy_empty.stl
```

Result:
- Byte size identical: `573245` bytes each.
- SHA-256 identical.

### 4) Both-mode dual-output validation (GiD)
```bash
./build/biomesh data/test_peptide.pdb 1.0 --mesh both \
  --occupied-output build/ticket8/both_occ.gid \
  --empty-output build/ticket8/both_empty.gid \
  --format gid
```

Checks:
- Both outputs created in one invocation.
- Output sizes:
  - `build/ticket8/both_occ.gid`: `17686` bytes
  - `build/ticket8/both_empty.gid`: `187465` bytes
- Log shows a single preprocessing sequence (`[1/3]`, `[2/3]`, `[3/3]`) followed by both mesh-generation/export branches.

## Acceptance criteria status
- Existing automated tests pass (or documented limitations): **Met**.
  - `SmokeNoDeps` passed.
  - Optional GTest targets unavailable in this environment (`find_package(GTest QUIET)` not found).
- Occupied-only and empty-only parity on representative sample inputs: **Met**.
  - Output artifact size and digest parity confirmed.
- Both-mode produces two valid outputs from one preprocessing run: **Met**.
  - Two files generated with expected distinct sizes.
  - Single preprocessing phase confirmed in run log.
- No critical regressions introduced in core library APIs: **Met (sanity level)**.
  - Core library builds and links all existing executable targets successfully.

