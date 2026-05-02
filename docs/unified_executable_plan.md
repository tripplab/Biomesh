# Task Breakdown: Consolidate `empty_voxel_to_gid` + `occupied_voxel_to_gid` into One `biomesh` Executable

## Objective
Deliver a single executable (`biomesh`) that supports exporting:
- occupied mesh only,
- empty mesh only,
- or both meshes to separate files,
while centralizing shared logic into one maintainable pipeline.

---

## Ticket 1 — Baseline Audit + CLI Contract Draft

### Scope
- Confirm current behavior of:
  - `examples/occupied_voxel_to_gid.cpp`
  - `examples/empty_voxel_to_gid.cpp`
  - `CMakeLists.txt` executable targets.
- Define the initial CLI contract for unified `biomesh` (arguments, options, defaults, validation rules).
- Record migration mapping from old commands to new commands.

### Deliverables
- Short design note (in repo docs) containing:
  - finalized option names,
  - output-path rules for single/both mesh modes,
  - error cases and expected messages.

### Acceptance Criteria
- [ ] CLI contract explicitly defines `--mesh occupied|empty|both`.
- [ ] Output behavior is unambiguous for all mesh modes.
- [ ] Validation rules for numeric inputs and output collisions are documented.
- [ ] Old-to-new command mapping examples are documented.

---

## Ticket 2 — Shared Pipeline Skeleton in New `examples/biomesh.cpp`

### Scope
- Add new executable entrypoint source (`examples/biomesh.cpp`) with no behavior removed yet from legacy binaries.
- Implement shared preprocessing skeleton only:
  - parse/validate shared args,
  - PDB parse,
  - atom enrichment,
  - voxel grid construction,
  - unified logging scaffold.

### Deliverables
- Compileable `examples/biomesh.cpp` invoking shared preprocessing path.
- Internal structures/enums for mesh mode selection (`Occupied`, `Empty`, `Both`).

### Acceptance Criteria
- [ ] `biomesh` target compiles and runs to preprocessing completion for valid inputs.
- [ ] Invalid numeric parameters return non-zero with clear error output.
- [ ] Mesh mode parsing works for all three values and rejects invalid values.

---

## Ticket 3 — Occupied Mesh Branch Integration

### Scope
- Implement occupied branch in unified executable:
  - `VoxelMeshGenerator::generateHexMesh(voxelGrid)`
  - occupied stats and efficiency report
  - export using selected format and output path.

### Deliverables
- `--mesh occupied` fully functional in `biomesh`.

### Acceptance Criteria
- [ ] For identical input settings, occupied mesh node/element counts match legacy occupied executable.
- [ ] Occupied export works in GiD and STL.
- [ ] Large-mesh warning behavior is preserved.

---

## Ticket 4 — Empty Mesh Branch Integration

### Scope
- Implement empty branch in unified executable:
  - `EmptyVoxelMeshGenerator::generateHexMesh(voxelGrid)`
  - empty stats and efficiency report
  - export using selected format and output path.

### Deliverables
- `--mesh empty` fully functional in `biomesh`.

### Acceptance Criteria
- [ ] For identical input settings, empty mesh node/element counts match legacy empty executable.
- [ ] Empty export works in GiD and STL.
- [ ] Large-mesh warning behavior is preserved.

---

## Ticket 5 — Both-Mode Orchestration + Output Routing

### Scope
- Implement `--mesh both` orchestration in unified executable.
- Support explicit separate outputs:
  - `--occupied-output <file>`
  - `--empty-output <file>`
- Optional basename expansion if included in contract (e.g., `<base>_occupied`, `<base>_empty`).
- Add collision checks (same resolved path for both outputs).

### Deliverables
- Reliable dual-export flow that reuses one preprocessing pass.

### Acceptance Criteria
- [ ] `--mesh both` creates two mesh files in one run.
- [ ] Occupied and empty outputs are never silently overwritten by one another.
- [ ] If output args are incomplete/invalid, command fails fast with actionable error.
- [ ] Preprocessing executes once per invocation in both mode.

---

## Ticket 6 — CMake Target Migration + Legacy Strategy

### Scope
- Update `CMakeLists.txt` to add unified `biomesh` executable target.
- Decide and implement one migration path:
  1. Keep legacy executables temporarily as wrappers/compat targets, or
  2. Remove legacy targets immediately after parity confirmation.

### Deliverables
- Build config reflecting chosen migration path.

### Acceptance Criteria
- [ ] `cmake` configure + build succeeds with new target.
- [ ] No broken target references in docs/scripts/tests.
- [ ] Migration behavior for old executable names is explicit (supported wrapper or documented removal).

---

## Ticket 7 — Documentation & Usage Migration

### Scope
- Update README/examples/docs to reference unified `biomesh` usage.
- Add direct replacements for previous commands.
- Document single-mode vs both-mode output argument rules.

### Deliverables
- Updated user-facing docs and examples.

### Acceptance Criteria
- [x] README no longer instructs new users to run two separate mesh executables.
- [x] At least one example each for occupied-only, empty-only, and both-mode is present.
- [x] Error/validation expectations are documented for common misuse cases.

---

## Ticket 8 — Parity & Regression Validation

### Scope
- Execute verification matrix for new unified CLI.
- Run existing tests and sanity checks.
- Compare new outputs against legacy behavior for representative inputs.

### Deliverables
- Validation report (in PR description and/or test notes) covering:
  - mode coverage,
  - format coverage,
  - parity checks,
  - regression outcomes.

### Acceptance Criteria
- [ ] Existing automated tests pass (or failures documented as pre-existing/unrelated).
- [ ] Occupied-only and empty-only parity confirmed on representative sample inputs.
- [ ] Both-mode verified to produce two valid outputs from one preprocessing run.
- [ ] No critical regressions introduced in core library APIs.

---

## Recommended PR Sequence (6–8 PR-Sized Steps)
1. PR-1: Ticket 1 (contract/design).
2. PR-2: Ticket 2 (skeleton executable).
3. PR-3: Ticket 3 (occupied branch).
4. PR-4: Ticket 4 (empty branch).
5. PR-5: Ticket 5 (both-mode + outputs).
6. PR-6: Ticket 6 (CMake migration).
7. PR-7: Ticket 7 (docs migration).
8. PR-8: Ticket 8 (final parity/regression validation).

## Definition of Done
- One executable (`biomesh`) handles occupied, empty, and both workflows.
- Shared preprocessing logic is implemented once and reused across modes.
- Dual export is deterministic with explicit output controls.
- Legacy executable transition is complete and documented.
- Build and tests are green, with parity evidence captured.


### Ticket 7 addendum for constrained CI
- Add a minimal, no-dependency `SmokeNoDeps` CTest target so core mesh generation is validated even when GoogleTest is unavailable.
