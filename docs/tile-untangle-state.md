# tile-untangle persistent state

**Read me first at the start of every session before doing anything else.**

This is the long-running checklist for the de-templating + folding work
on `src/tile.c` in libretro/snes9x2010. The work spans many sessions
and Claude's context window will compact between sessions; this file
is the source of truth for "where are we" and "what's next."

If anything in this file contradicts conversation history or memory,
**this file wins** because it's updated at every commit boundary.

---

## Current state

**Branch:** `tile-untangle` (origin/libretro/snes9x2010)
**Base:** `7fb5b58` — "Mode 7 hires: 4x horizontal mode, bilinear stable/smooth modes, BL at 1x"
**Stage:** Stage 2.1 complete (HR4X de-templated); ready to start Stage 2.2 (BL4X)
**Last commit on tile-untangle:** `ec6a6db` — tile.c: de-template Mode 7 HR4X renderers
**In-flight work:** none
**Working tree:** clean

## Invariants — must hold at every commit

These are non-negotiable. Verified externally by Lib (the user) before
each commit lands. Claude does *not* fabricate verification; if a hash
or harness wasn't actually run, say so.

- **Audio bit-identical** with all hires/BL options off:
  - `NTSC = 0xe2cacc592d6fc085`
  - `PAL  = 0x4274c83d71cb4b45`
- **Save state compatibility**: a savestate from `7fb5b58` loads
  cleanly on the current commit and produces byte-identical
  subsequent frames (verifies no SRAM/CPU/PPU state shape change).
- **Valgrind clean** on the 9 standing harnesses.
- **Symbol set preserved**: every public symbol that was in
  `7fb5b58` is still in the .so. Internal symbols may change shape
  (LTO will inline), but the dispatch arrays
  (`Renderers_DrawTile16Normal1x1` etc.) must still resolve and
  point at functions with the same observable behavior.
- **7 distinct math variants** per (NAME1, NAME2) combination. Lib
  has a checker for this; if any two collapse to the same code,
  that's a regression.

## Documentation rule — applies at every conversion

Every commit that de-templates a renderer family **also** adds C
comments explaining what the resulting code does. The macro
abstraction was carrying a lot of implicit knowledge (which of
NAME1 / NAME2 / MATH this code is specialized for, what the
fractional-step arithmetic means, why a particular dispatch i-index
maps to a particular math op, what the per-corner opacity bitmap
means, etc). When the macros go, that knowledge has to land
somewhere readable in the C source. It does not get to live only in
git history or in this state file.

Required comment shape per de-templated function:

  /* DrawMode7BG1HR4X_AddF1_2_Normal1x1
   *
   * Mode 7 BG1, 4x horizontal hires, no bilinear, color math is
   * "add fixed colour, half intensity" (CGADSUB add + ½ + math
   * outside window selects fixed colour). Output buffer is at 4x
   * native width; this renderer writes 4 sub-samples per native
   * pixel into GFX.S[Offset + 4*x .. + 4*x+3].
   *
   * Z1/Z2 are constants (D + 7) for BG1 because BG1 priority does
   * not depend on the EXTBG per-pixel priority bit.
   * ...
   */
  static void DrawMode7BG1HR4X_AddF1_2_Normal1x1 (uint32 Left, uint32 Right, int D)
  {
      ...
  }

Required comment shape at the module level (top of file or
section header):

  /* === Mode 7 BG1 hires renderers ===
   *
   * Six closely-related renderer families:
   *   HR  = 2x horizontal, nearest-neighbour
   *   BL  = 2x horizontal, bilinear (stable or smooth at runtime)
   *   HR4X = 4x horizontal, nearest-neighbour
   *   BL4X = 4x horizontal, bilinear
   *   BL1X = native (1x), bilinear (no upsample)
   *
   * Each is dispatched only when (BGMode == 7 && !sub) and the
   * appropriate (Mode7Hires, Mode7HiresBilinear) settings are on;
   * see S9xSelectTileRenderers.
   *
   * Sub-pixel sample positions for 2x/4x: stepping AA, CC by
   * (matrixA / factor), (matrixC / factor) per output column,
   * with the rounding leftover deposited on the final sub-sample
   * to keep the sum-per-native-pixel exactly equal to the
   * matrixA/matrixC the SNES would have advanced.
   *
   * For mid-frame promotion semantics, see ppu.c::S9xUpdateScreen.
   */

Both shapes get added at every conversion commit. If the
explanation is essentially the same as one already nearby (e.g.
BG1 vs BG2 just differ in the priority bit), the second function
can reference the first by name instead of repeating ("Same as
DrawMode7BG1HR4X_AddF1_2_Normal1x1 except Z1/Z2 reflect the
EXTBG per-pixel priority bit; see that function for details").

The whole-file goal at the end of Stage 3: a contributor who has
never read tile.c before should be able to read it top-to-bottom
and understand the dispatch model, the buffer-width tier system,
the math variants, and the Mode 7 sub-pixel arithmetic without
needing this state file or any external doc.

## Stage plan

### Stage 1 — Inventory (no code changes)

Goal: produce a complete, accurate map of the templating in
`src/tile.c` so subsequent stages have a target to convert against.

Single doc commit. The doc lives in `docs/tile-internals.md` (new
directory, new file). Covers:

- The three-level `#include "tile.c"` re-entry scheme.
- Every NAME1 family and what it produces.
- Every NAME2 expansion and what it produces.
- The 7 math variants and what they produce.
- The full table of generated symbols (NAME1 × NAME2 × MATH
  combinations actually produced, after `M7HIRES_ONLY` /
  `NO_INTERLACE` skips).
- The dispatch path: how `S9xSelectTileRenderers` picks an array,
  how the per-tile call site picks an entry.
- The math primitives (`NOMATH`, `REGMATH`, `MATHF1_2`,
  `MATHS1_2`) and `COLOR_ADD` / `COLOR_SUB` / `_1_2` cousins.
- A worked example: trace what
  `DrawMode7BG1BLAddF1_2_Normal1x1` actually expands to.

Stage 1 is the **external** doc. The in-source C comments start
at Stage 2 (per the Documentation rule above). Once Stage 3 is
done and the templating is gone, the external doc can be
shortened to just dispatch / module overview, with the per-renderer
explanations living in the source itself where future contributors
will actually read them.

Status: **TODO**

### Stage 2 — De-macroing renderer bodies

Goal: replace the templating-driven instantiation of each NAME1
family with explicit static (inline) functions.

Order (small/safe first, large/risky last):

1. ~~`DrawMode7BG1HR4X` and `DrawMode7BG2HR4X`~~ — **done** (ec6a6db).
2. `DrawMode7BG1BL4X` and `DrawMode7BG2BL4X` — **next**.
3. `DrawMode7BG1BL1X` and `DrawMode7BG2BL1X`.
4. `DrawMode7BG1BL` and `DrawMode7BG2BL` (the existing 2x BL).
5. `DrawMode7BG1HR` and `DrawMode7BG2HR` (the existing 2x HR).
6. `DrawMode7BG1` and `DrawMode7BG2` (native Mode 7) — full
   NAME2 set (Normal1x1, Normal2x1, Normal4x1, Hires, Interlace,
   HiresInterlace).
7. `DrawMode7MosaicBG1` and `DrawMode7MosaicBG2`.
8. `DrawBackdrop16` — full NAME2 set.
9. `DrawMosaicPixel16` — full NAME2 set.
10. `DrawClippedTile16` — full NAME2 set.
11. `DrawTile16` — full NAME2 set. Largest; convert last.

Each step is one commit. Each commit:

- Touches only the files for that family.
- Builds clean.
- Passes invariants.
- objdump -d shows functionally-equivalent code for the converted
  symbols (verified by Lib externally).
- The dispatch arrays (`Renderers_*`) still resolve correctly.

Status: **TODO** (waiting on Stage 1)

### Stage 3 — Remove the templating scaffold

Goal: delete the `#include "tile.c"` self-recursion, the level
dispatchers, and all the `#define NAME1` / `#define NAME2` /
`#define DRAW_TILE` machinery. With Stage 2 complete nothing uses
them.

Single cleanup commit.

Status: **TODO**

### Stage 4 — Folding optimizations

Goal: with explicit code, look for cross-cutting optimizations that
the macro abstraction was hiding. Candidates (each its own commit
with profile data):

- Hoist `Settings.Mode7HiresBilinear == 2` from per-pixel to
  per-frame in M7BL renderers.
- Hoist Mode 7 matrix recompute that's invariant within a tile.
- Collapse the 7 math variants to one function with a `MathOp`
  switch in the inner loop (LTO may re-specialize anyway, in
  which case this is a code-volume win without runtime cost).
- Eliminate redundant Z-tests across sub-pixel positions in 4x
  / 2x plotters.
- Mosaic per-block hoist of pixel-replicate.

Status: **TODO** (waiting on Stages 1–3)

### Stage 5 — Optional cross-cutting cleanups

Goal: with everything explicit, look for shared structure that
wasn't visible through the macros. May or may not be worth doing;
re-evaluate after Stage 4.

Status: **TODO** (re-evaluate later)

## What to do at the start of each session

1. Read this file top-to-bottom.
2. `git status` and `git log --oneline -5` on tile-untangle to
   confirm where the branch actually is.
3. If the branch is ahead of what this file says, **update this
   file** to reflect the actual state before doing any new work.
   The file lags reality if Lib pushed commits between sessions.
4. If a stage was in flight, look at the in-flight notes in this
   file and resume from there.
5. If no work is in flight, pick the next TODO from the stage
   plan above.

## What to do at the end of each session

1. Update **Current state** at the top of this file.
2. If a commit was made: add an entry to **Commit log** below
   with the SHA, the family/feature, and the invariant
   verification status.
3. If work is in flight (e.g. a partial conversion): add an
   **In-flight notes** section describing exactly what was
   done, what file is in what state, and what comes next.
4. Save the file via the create_file or str_replace tool.

## Commit log (newest first)

### `ec6a6db` — tile.c: de-template Mode 7 HR4X renderers

Stage 2.1 deliverable. Converts `DrawMode7BG{1,2}HR4X` from the
3-level `#include "tile.c"` templating to explicit hand-written
code with C-comment documentation per the documentation rule.

Approach: a local `DEFINE_M7_HR4X_FAMILY(...)` macro fans out the
7 MATH variants per BG. Defined and undef'd within the section;
no new global macros. The fan-out preserves compile-time
specialization — each of the 14 functions gets the math op
resolved at compile time, no per-pixel runtime branch.

C comments: section banner explains the family at module level;
fan-out macro has a comment block explaining each parameter and
the body structure; inline comments call out non-obvious
arithmetic (quarter-step rounding leftover, BG2 priority bit
handling, wrap vs repeat-mode paths).

Files changed: `src/tile.c`, +294/-156, +138 net.

Removed:
  - templated `DrawMode7BG{1,2}HR4X` instantiation block (#define
    NAME1 / #include "tile.c" / #undef NAME1 pattern, x2)
  - `DRAW_TILE_NORMAL_M7HIRES_4X` macro definition (~110 lines,
    no longer referenced)
  - dangling `#undef DRAW_TILE_NORMAL_M7HIRES_4X` at level-1 close

Added:
  - new de-templated section (~280 lines incl. comments)
  - 14 explicit `static void DrawMode7BG{1,2}HR4X*_Normal1x1`
    functions
  - 2 explicit `static void (*Renderers_DrawMode7BG{1,2}HR4X
    Normal1x1[7])(...)` dispatch arrays

Symbol verification (build comparison vs pristine `b388159`):
  - 14 HR4X functions present, same names, same arity
  - 2 Renderers arrays present, same names, same 7 entries each
  - Total Draw* function count unchanged: 313 in both builds
  - Total Renderers_* array count unchanged: 45 in both builds
  - No new warnings; pre-existing 4 'defined but not used'
    warnings on `DrawMode7BG{1,2}Normal4x1` /
    `DrawMode7MosaicBG{1,2}Normal4x1` are still there
    (LTO-eliminated dead arrays, pre-existing condition).

Behaviour invariants — to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on HR4X.

### `b388159` — Document tile.c templating model and dispatch path

Stage 1 deliverable. Adds `docs/tile-internals.md` (531 lines).
Descriptive walkthrough of the 3-level `#include "tile.c"` scheme,
every NAME1 family, every NAME2 expansion, the 7 MATH ops, the
runtime `i`-index dispatch, the full symbol-count table including
LTO-eliminated arrays, and a worked expansion of
`DrawMode7BG1BLAddF1_2_Normal1x1`.

No code changes. Build verified clean. Audio/savestate/valgrind
invariants unaffected by definition.

## In-flight notes

(none — clean state)

## Open questions for Lib

(none currently)

## Open followups (deferred from in-flight work, not bundled)

### Stale BL color-math comment

Lines ~2575-2594 of current `src/tile.c` (the BL2X header comment
block) claim that the bilinear path "bypasses MATH()" and that all
7 generated functions "are functionally identical because the
bilinear DRAW_TILE writes final RGB directly to GFX.S without
invoking MATH()." This was true before commit `6d214cf` (color-math
fix) but is now wrong: `M7HR_BLEND_AND_WRITE` does call
`MATH(blended, ...)`, and the 7 variants are now genuinely distinct.

This was found during Stage 2.1 design work but was kept out of
the de-templating commit to keep scope clean. Should be a tiny
standalone commit before or during Stage 2.4 (BL2X de-templating)
since the BL2X templated block is what the comment describes — and
de-templating BL2X will touch that comment anyway.

---

*Last updated: after Stage 2.1 commit `ec6a6db` (de-template Mode 7 HR4X). Branch `tile-untangle` is local at `ec6a6db`, two commits past upstream `b388159`. Awaiting Lib's external invariant verification + push. Ready to start Stage 2.2 (BL4X) once `ec6a6db` lands upstream.*
