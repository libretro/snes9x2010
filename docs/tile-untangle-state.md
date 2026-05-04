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
**Stage:** Stage 3.5 + Stage 4 complete (structural cleanup + fan-out unrolling).
**Last code commit:** Stage 3.5 + Stage 4 — tile.c: unroll fan-out macros + structural cleanup (this commit)
**Previous code commit:** `a40fa37` — tile.c: de-template DrawTile16 + remove dispatch scaffold (Stage 2.11 + Stage 3)
**In-flight work:** none
**Working tree:** clean

State file lives in-tree at `docs/tile-untangle-state.md`. Updates
ride along in the relevant commit.

**All renderer families are de-templated and the dispatch scaffold
is gone.** The Mode 7 group: HR4X (`0cbca9e`), BL4X (`69f75a1`),
BL1X (`0e147ff`), BL (`6fde805`), HR (`624492c`), native BG
(`f5894be`), mosaic + dead-array cleanup (`3c7ad4f`). The non-Mode-7
families: DrawBackdrop16 (`9ac00cb`), DrawMosaicPixel16
(`bb3cacf`), DrawClippedTile16 (`8906302`), DrawTile16 + scaffold
removal (`a40fa37`).

**Stage 3.5 (structural cleanup) and Stage 4 (fan-out unrolling)
landed together in this commit.** All 313 renderer function
definitions are now written explicitly with `static void NAME(args)
TILE_BODY_*(args)` — no more `DEFINE_*_FN` / `DEFINE_*_NAME2` /
`DEFINE_*_BG` / `DEFINE_*_FAMILY` macros generating function
definitions. Body macros (`TILE_BODY_*`), pixel plotters
(`DT_PIXEL_*`, `CT_PIXEL_*`, etc.), color-math ops (`NOMATH`,
`REGMATH`, `MATHF1_2`, `MATHS1_2`, `COLOR_ADD*` / `COLOR_SUB*`)
and helpers (`GET_CACHED_TILE`, `IS_BLANK_TILE`, `SELECT_PALETTE`,
`H_FLIP`, `V_FLIP`, `CLIP_10_BIT_SIGNED`, `M7HR_LOOKUP_4`,
`M7HR_BLEND_AND_WRITE`, `M7HR_SAMPLE_BILINEAR`) all stay as macros.

The dead `#ifndef NAME1` wrapper at line 996 (always true since
NAME1 is never defined) is also removed (Stage 3.5). The outer
`#ifndef _NEWTILE_CPP / #else` split and the line-660 self-include
remain; they're still load-bearing for separating "first include"
helpers from the recursive-include family code body.

`tile.c` no longer self-includes recursively at the level-2 or
level-3 boundaries. The `#ifndef NAME2` / `#else / Third-level`
blocks are gone, along with the per-NAME2 BPSTART / PITCH /
DRAW_PIXEL macros and the per-(NAME1, NAME2) 7-fold MAKENAME
fan-out. The only structural scaffold that remains is the outer
`#ifndef _NEWTILE_CPP / #else / #ifndef NAME1` wrapper at the
top of the file plus the line-660 `#include "tile.c"` that
enters the family code at compile time. That wrapper is now
structurally dead (NAME1 is never defined and the `#else` branch
is never reached), but removing it is a separate structural
cleanup not bundled here.

Optional follow-up (Stage 4) is profile-driven: cross-cutting
plotter consolidation (the `MP_PIXEL_*` / `CT_PIXEL_*` /
`DT_PIXEL_*` plotters are byte-identical and could share a
single `TILE_PIXEL_*` definition across the three sections).
Worth doing only if it pays off in maintainability without
compromising codegen.

The level-1 Mode 7 templating scaffold is gone: file-scope
`Z1`/`Z2`/`MASK`/`DCMODE`/`BG`/`NO_INTERLACE` `#define`s for Mode 7
removed. The `DRAW_TILE_NORMAL` and `DRAW_TILE_MOSAIC` macros are
deleted. The level-2 dispatch in `tile.c` is still alive because
DrawMosaicPixel16, DrawClippedTile16, and DrawTile16 still use it.

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

1. ~~`DrawMode7BG1HR4X` and `DrawMode7BG2HR4X`~~ — **done** (0cbca9e).
2. ~~`DrawMode7BG1BL4X` and `DrawMode7BG2BL4X`~~ — **done**
   (Stage 2.2a `54c9153` parameterize helpers; Stage 2.2b `69f75a1`
   de-template BL4X). The parameterization commit is shared with
   subsequent BL stages.
3. ~~`DrawMode7BG1BL1X` and `DrawMode7BG2BL1X`~~ — **done**
   (Stage 2.3, `0e147ff`). Same fan-out pattern as BL4X but
   without sub-pixel division. Single commit since helpers were
   already parameterized.
4. ~~`DrawMode7BG1BL` and `DrawMode7BG2BL` (the existing 2x BL)~~
   — **done** (Stage 2.4, `6fde805`). Same fan-out pattern as
   BL4X but with half-step (aa_h = aa/2) sub-pixel division.
   Removed the `DRAW_TILE_NORMAL_M7HIRES_BILINEAR` macro (no
   longer referenced) and the `ARGS / M7HIRES_ONLY` scope
   scaffolding for the BL block (BL was the last user).
5. ~~`DrawMode7BG1HR` and `DrawMode7BG2HR` (the existing 2x HR)~~
   — **done** (Stage 2.5, `624492c`). Same fan-out pattern as
   BL but with inline nearest-neighbour DRAW_PIXEL writes
   (assignment-and-test in single expression matching the
   original macro shape) instead of bilinear blending. Removed
   the `DRAW_TILE_NORMAL_M7HIRES` macro (no longer referenced).
   This was the last user of the level-1 Mode 7 hires templating
   scaffold.
6. ~~`DrawMode7BG1` and `DrawMode7BG2` (native Mode 7)~~ — **done**
   (Stage 2.6a, `f5894be`). Full NAME2 set emitted in 2.6a; dead
   variants dropped in 2.6b/2.7 below.

   Stage 2.6b folded into Stage 2.7 (`3c7ad4f`): drop the
   never-referenced NAME2 variants (Normal4x1, Interlace,
   HiresInterlace) for the de-templated families. Cleans up all
   the unused-Renderers warnings.

7. ~~`DrawMode7MosaicBG1` and `DrawMode7MosaicBG2` (Stage 2.7)~~ —
   **done** (`3c7ad4f`). Mosaic Mode 7 de-templated using a
   `TILE_BODY_MOSAIC_M7` body macro plus `DEFINE_M7_MOSAIC_*`
   fan-out, mirroring the native machinery. Combined with 2.6b
   (drop dead NAME2 variants for both native and mosaic) into
   one commit. The `DRAW_TILE_MOSAIC` macro is gone, and the
   file-scope `Z1`/`Z2`/`MASK`/`DCMODE`/`BG`/`NO_INTERLACE`
   `#define`s for Mode 7 are gone.
8. ~~`DrawBackdrop16` — full NAME2 set.~~ — **done** (Stage 2.8,
   `9ac00cb`). Four NAME2 variants emitted (Normal1x1, Normal2x1,
   Normal4x1, Hires) -- the Interlace and HiresInterlace variants
   the level-2 dispatch normally generates were already excluded
   by the templated form's `NO_INTERLACE` define and the
   de-templated form simply doesn't emit them. Backdrop-specific
   plotters (`BACKDROP_PIXEL_*`) bake in `Z = 1`, `M = 1`,
   `Pix = 0` as literals to match the original DRAW_PIXEL
   expansion exactly.
9. ~~`DrawMosaicPixel16` — full NAME2 set.~~ — **done** (Stage 2.9,
   `bb3cacf`). All six NAME2 variants emitted (Normal1x1,
   Normal2x1, Normal4x1, Hires, Interlace, HiresInterlace) -- this
   was the first family to need the interlace variants and
   therefore the first to bake the per-NAME2 BPSTART expression
   (`StartLine` for non-interlace, `StartLine * 2 + BG.InterlaceLine`
   for interlace) directly into the fan-out. Section-local
   `MP_PIXEL_*` plotters mirror the original DRAW_PIXEL_* shapes
   with `Z = GFX.Z1`/`GFX.Z2` and runtime `Pix`. PITCH is not
   used by mosaic-pixel and therefore not threaded through.
10. ~~`DrawClippedTile16` — full NAME2 set.~~ — **done** (Stage 2.10,
    `8906302`). All six NAME2 variants emitted. First family to
    thread both BPSTART and PITCH (as `BP_STEP_EXPR` = `8 * PITCH`,
    so `8` non-interlace, `16` interlace). Body has 4 flip-case
    branches each with an 8-case fall-through switch -- preserved
    verbatim in `TILE_BODY_CLIPPED`. Section-local `CT_PIXEL_*`
    plotters with the same shape as `MP_PIXEL_*`.
11. ~~`DrawTile16` — full NAME2 set.~~ — **done** (Stage 2.11,
    `a40fa37`). The hot-path renderer for unclipped 8-pixel
    BG tiles. All six NAME2 variants emitted. Same plotter shape
    as DrawMosaicPixel16 / DrawClippedTile16 (`DT_PIXEL_*`,
    Z = GFX.Z1/Z2, runtime Pix). Body simpler than ClippedTile
    (no per-pixel switch over StartPixel, no Width clamp): just
    4 flip cases each with a plain `for (n = 0; n < 8; n++)`
    loop. ARGS shape is the smallest of any tile family
    (Tile, Offset, StartLine, LineCount). Threads BPSTART and
    BP_STEP per NAME2 same as ClippedTile.

    Bundled with Stage 3 (`a40fa37`): once DrawTile16 was
    de-templated, no `#include "tile.c"` self-recursion remained
    inside the `#ifndef NAME1` branch. The level-2/3 dispatch
    scaffold became dead code and was removed.

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

**Done** (bundled into the Stage 2.11 commit). The level-2 dispatch
(`#ifndef NAME2`, with per-NAME2 BPSTART / PITCH / DRAW_PIXEL
macros) and the level-3 dispatch (`MAKENAME` 7-fold math fan-out)
have both been deleted. No `#include "tile.c"` self-recursion
remains inside the `#ifndef NAME1` branch. The outer
`#ifndef _NEWTILE_CPP / #else / #ifndef NAME1` wrapper plus the
line-660 self-include remains -- still load-bearing for compile
ordering, but `NAME1` is now never defined and the `#else` branch
of `#ifndef _NEWTILE_CPP` is only reached during the recursive
include. Removing the wrapper is a separate structural cleanup
not bundled here.

Status: **DONE** (`a40fa37`, bundled with Stage 2.11).

### Stage 3.5 — Structural cleanup

Goal: remove the now-dead `#ifndef NAME1` wrapper at `src/tile.c`
line 996. With Stage 3 complete, NAME1 is never defined; the
wrapper's body always runs and the `#else` branch is dead code.

**Done** (this commit). The `#ifndef NAME1 / #endif` pair has been
collapsed; the family-code block now runs unconditionally on the
recursive include. Replaced with an explanatory banner comment
describing the recursive-include landing-zone purpose. The outer
`#ifndef _NEWTILE_CPP / #else` split and the line-660 self-include
remain; they're still load-bearing for separating "first include"
code (helpers, ConvertTile_*, etc.) from "recursive include"
code (family bodies).

Bundled with Stage 4.

Status: **DONE** (this commit, bundled with Stage 4).

### Stage 4 — Unroll function-emitting fan-out macros

Goal: convert all `DEFINE_*_FN` / `DEFINE_*_NAME2` / `DEFINE_*_BG`
/ `DEFINE_*_FAMILY` macros (which emit function definitions through
token-pasting) into explicit `static void NAME(args) TILE_BODY_*(...)`
forms. The user's stated motivation: "It will allow us to optimize
the code later better for all the various modes" -- per-function
optimization opportunities (different inlining hints, different
attributes on hot vs cold variants, function-level diff diagnostics)
are easier to apply when each function is a separate textual unit
instead of one row in a fan-out macro.

**Done** (this commit). All 313 renderer functions are now written
explicitly. No macros emit function definitions. The 11 fan-out
macros that previously did so are gone:

  - `DEFINE_DT_FN` / `DEFINE_DT_NAME2` (DrawTile16)
  - `DEFINE_CT_FN` / `DEFINE_CT_NAME2` (DrawClippedTile16)
  - `DEFINE_MP_FN` / `DEFINE_MP_NAME2` (DrawMosaicPixel16)
  - `DEFINE_BACKDROP_FN` / `DEFINE_BACKDROP_NAME2` (DrawBackdrop16)
  - `DEFINE_M7_FN` / `DEFINE_M7_NAME2` / `DEFINE_M7_BG` (native M7)
  - `DEFINE_M7_MOSAIC_FN` / `DEFINE_M7_MOSAIC_NAME2`
    / `DEFINE_M7_MOSAIC_BG` (mosaic M7)
  - `DEFINE_M7_HR_FAMILY` (M7 HR2X)
  - `DEFINE_M7_HR4X_FAMILY` (M7 HR4X)
  - `DEFINE_M7_BL_FAMILY` (M7 BL2X)
  - `DEFINE_M7_BL4X_FAMILY` (M7 BL4X)
  - `DEFINE_M7_BL1X_FAMILY` (M7 BL1X)

The 5 M7 hires families (HR/HR4X/BL/BL4X/BL1X) had the function
definition inside their `DEFINE_*_FAMILY` macro body. For each
of those, the macro was converted from a function-emitter to a
body-only `TILE_BODY_M7_<X>` macro: the `static void
BG_NAME##suffix##Normal1x1 (...)` line was removed from the macro
body, the macro's parameter list was reduced (suffix and BG_NAME
moved to the call site), and each invocation was rewritten as
explicit `static void NAME(args) TILE_BODY_M7_<X>(...)`.

What stays as macros (per the user's instruction):
  - All body macros: `TILE_BODY_NORMAL_M7`, `TILE_BODY_MOSAIC_M7`,
    `TILE_BODY_CLIPPED`, `TILE_BODY_UNCLIPPED`, `TILE_BODY_BACKDROP`,
    `TILE_BODY_MOSAIC_PIXEL`, `TILE_BODY_M7_HR`, `TILE_BODY_M7_HR4X`,
    `TILE_BODY_M7_BL`, `TILE_BODY_M7_BL4X`, `TILE_BODY_M7_BL1X`.
  - All pixel plotters: `DT_PIXEL_*`, `CT_PIXEL_*`, `MP_PIXEL_*`,
    `BACKDROP_PIXEL_*`, `M7N_PIXEL_*`, `M7HR_LOOKUP_4`,
    `M7HR_BLEND_AND_WRITE`, `M7HR_SAMPLE_BILINEAR`.
  - All color-math: `NOMATH`, `REGMATH`, `MATHF1_2`, `MATHS1_2`,
    `COLOR_ADD*`, `COLOR_SUB*`.
  - All file-scope helpers: `GET_CACHED_TILE`, `IS_BLANK_TILE`,
    `SELECT_PALETTE`, `H_FLIP`, `V_FLIP`, `CLIP_10_BIT_SIGNED`.

Symbol verification (vs pristine `a40fa37`):
  - 313 Draw* functions; full per-function size diff is empty
    across all 313 (verified). True pure refactor.
  - 44 Renderers_ arrays; full diff empty.
  - Build clean, zero warnings.

Files changed: `src/tile.c` +1383 / -400 (+983 net). The unrolled
form is significantly larger than the macro-driven form -- 313
function definitions written by hand take more text than 11 fan-out
macros. The trade-off the user accepted: more lines now, but
per-function flexibility for future optimization.

Comments: each section has a banner comment describing what it
does, and each NAME2 group within a section has a banner like
`/* DrawTile16 NAME2 = Normal1x1: 7 math variants. */` to make
it easy to find the function block for a given variant. Section
banners and section-end markers preserve the original structural
documentation.

Status: **DONE** (this commit).

### Stage 5 — Folding optimizations (future)

Goal: with all functions explicit, look for per-mode optimizations
that the macro abstraction was hiding. Candidates (each its own
commit with profile data):

- Hoist `Settings.Mode7HiresBilinear == 2` from per-pixel to
  per-frame in M7BL renderers.
- Hoist Mode 7 matrix recompute that's invariant within a tile.
- Eliminate redundant Z-tests across sub-pixel positions in 4x
  / 2x plotters.
- Mosaic per-block hoist of pixel-replicate.
- Consolidate `MP_PIXEL_*` / `CT_PIXEL_*` / `DT_PIXEL_*`
  cross-cutting plotter shapes (they're nearly identical) into
  shared `TILE_PIXEL_*` macros.

Status: **TODO** (re-evaluate later, profile-driven)

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

### Stage 3.5 + Stage 4 — tile.c: unroll fan-out macros + structural cleanup (this commit)

Removes the now-dead `#ifndef NAME1` wrapper (Stage 3.5) and unrolls
all 11 function-emitting fan-out macros into explicit `static void
NAME(args) TILE_BODY_*(args)` definitions (Stage 4).

User-stated motivation for Stage 4: "It will allow us to optimize
the code later better for all the various modes." Per-function
optimization opportunities (different inlining hints, different
attributes on hot vs cold variants, function-level diff diagnostics)
are easier to apply when each function is a separate textual unit
instead of one row in a fan-out macro.

**Stage 3.5 part:** removes the `#ifndef NAME1 / #endif` wrapper
at `src/tile.c` line 996. With Stage 3 complete, NAME1 is never
defined; the wrapper's body always runs. The `#endif` at end-of-file
that closed it is also removed. Replaced with an explanatory banner
comment describing the recursive-include landing-zone.

**Stage 4 part:** all 313 renderer function definitions are now
written explicitly. The 11 fan-out macros are gone:
  - DrawTile16: `DEFINE_DT_FN`, `DEFINE_DT_NAME2`
  - DrawClippedTile16: `DEFINE_CT_FN`, `DEFINE_CT_NAME2`
  - DrawMosaicPixel16: `DEFINE_MP_FN`, `DEFINE_MP_NAME2`
  - DrawBackdrop16: `DEFINE_BACKDROP_FN`, `DEFINE_BACKDROP_NAME2`
  - native Mode 7: `DEFINE_M7_FN`, `DEFINE_M7_NAME2`, `DEFINE_M7_BG`
  - mosaic Mode 7: `DEFINE_M7_MOSAIC_FN`, `DEFINE_M7_MOSAIC_NAME2`,
    `DEFINE_M7_MOSAIC_BG`
  - M7 HR2X: `DEFINE_M7_HR_FAMILY`
  - M7 HR4X: `DEFINE_M7_HR4X_FAMILY`
  - M7 BL2X: `DEFINE_M7_BL_FAMILY`
  - M7 BL4X: `DEFINE_M7_BL4X_FAMILY`
  - M7 BL1X: `DEFINE_M7_BL1X_FAMILY`

The 5 M7 hires `DEFINE_M7_*_FAMILY` macros previously embedded the
function definition inside the macro body. For each of those, the
macro was converted from function-emitter to body-only `TILE_BODY_M7_<X>`:
the `static void BG_NAME##suffix##Normal1x1 (...)` line was removed,
the parameter list was reduced from 7 params to 5 (suffix and
BG_NAME moved to the call site), and each of the 14 invocations
(7 BG1 + 7 BG2) was rewritten as explicit
`static void NAME(args) TILE_BODY_M7_<X>(sel, op, z_expr, mask, dc)`.

Comments: per-NAME2 banner comments (e.g.
`/* DrawTile16 NAME2 = Normal1x1: 7 math variants. */`) added for
each NAME2 group across all sections. Section banners and
section-end markers preserve the original structural documentation.
Each unrolled section also has an "Outer fan-out: explicit functions
and dispatch arrays" preamble describing what changed.

What stays as macros (per the user's instruction):
  - All body macros: `TILE_BODY_NORMAL_M7`, `TILE_BODY_MOSAIC_M7`,
    `TILE_BODY_CLIPPED`, `TILE_BODY_UNCLIPPED`, `TILE_BODY_BACKDROP`,
    `TILE_BODY_MOSAIC_PIXEL`, `TILE_BODY_M7_HR`, `TILE_BODY_M7_HR4X`,
    `TILE_BODY_M7_BL`, `TILE_BODY_M7_BL4X`, `TILE_BODY_M7_BL1X`.
  - All pixel plotters: `DT_PIXEL_*`, `CT_PIXEL_*`, `MP_PIXEL_*`,
    `BACKDROP_PIXEL_*`, `M7N_PIXEL_*`, `M7HR_LOOKUP_4`,
    `M7HR_BLEND_AND_WRITE`, `M7HR_SAMPLE_BILINEAR`.
  - All color-math: `NOMATH`, `REGMATH`, `MATHF1_2`, `MATHS1_2`,
    `COLOR_ADD*`, `COLOR_SUB*`.
  - All file-scope helpers: `GET_CACHED_TILE`, `IS_BLANK_TILE`,
    `SELECT_PALETTE`, `H_FLIP`, `V_FLIP`, `CLIP_10_BIT_SIGNED`.

Symbol verification (vs pristine `a40fa37`):
  - 313 Draw* functions; full per-function size diff is empty
    across all 313 (verified). True pure refactor.
  - 44 Renderers_ arrays; full diff empty.
  - Build clean, zero warnings.

Files changed: `src/tile.c` +1383 / -400 (+983 net). The unrolled
form is significantly larger than the macro-driven form -- 313
function definitions written by hand take more text than 11 fan-out
macros. The trade-off the user accepted: more lines now, but
per-function flexibility for future optimization.

Behaviour invariants -- to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean.

### `a40fa37` — tile.c: de-template DrawTile16 + remove dispatch scaffold (Stage 2.11 + Stage 3)

**Final commit of the tile-untangle effort.** De-templates the
last templated family (DrawTile16) and removes the now-dead
level-2/level-3 dispatch scaffold from `tile.c`.

**Stage 2.11 part:** de-templates DrawTile16, the renderer for
unclipped 8-pixel-wide BG tiles. The hot path of the BG-tile
renderer.

Body shape: simpler than DrawClippedTile16. No per-pixel switch
over StartPixel, no Width clamp -- just 4 flip-case branches each
running `for (n = 0; n < 8; n++) DRAW_PIXEL(n, Pix = bp[idx])`.

ARGS shape: (uint32 Tile, uint32 Offset, uint32 StartLine,
uint32 LineCount). Four parameters -- the smallest of any tile
family.

Same plotter shape as DrawMosaicPixel16 / DrawClippedTile16
(Z = GFX.Z1/Z2, runtime Pix). Section-local DT_PIXEL_*
plotters. New TILE_BODY_UNCLIPPED body macro mirrors the
previously-templated DRAW_TILE() expansion -- 4 flip-case
branches, 8-pixel inner for-loop. Threads BPSTART_EXPR and
BP_STEP_EXPR per NAME2 same as ClippedTile.

42 functions emitted (6 NAME2 x 7 math) plus 6 dispatch arrays.

**Stage 3 part:** removes the level-2 and level-3 dispatch
scaffold from `tile.c`. With Stage 2.11 landed, no `#include
"tile.c"` self-recursion remains inside the `#ifndef NAME1`
branch -- so the entire `#else` block (lines that used to handle
NAME1-defined recursive includes for level-2 dispatch, and
NAME1+NAME2-defined recursive includes for level-3 dispatch)
is dead code.

Deleted:
  - `#define BPSTART StartLine` / `#define PITCH 1` and matching
    interlace redefines
  - per-NAME2 `DRAW_PIXEL_*` macro definitions (DRAW_PIXEL,
    DRAW_PIXEL_N2x1, DRAW_PIXEL_N4x1, DRAW_PIXEL_H2x1)
  - per-NAME2 `#define NAME2 / #include "tile.c" / #undef NAME2`
    machinery for Normal1x1, Normal2x1, Normal4x1, Hires,
    Interlace, HiresInterlace
  - `#ifndef M7HIRES_ONLY` and `#ifndef NO_INTERLACE` guards
  - level-3 `MAKENAME` / `CONCAT3` machinery
  - 7 `static void MAKENAME(NAME1, suffix, NAME2) (ARGS)`
    instantiations and the dispatcher array template

What stays:
  - line-660 outer `#include "tile.c"` -- load-bearing, triggers
    the recursive include that places family code in the
    compilation
  - `#ifndef _NEWTILE_CPP / #else` outer wrapper at the top of
    the file -- still load-bearing for separating "first include"
    code (helpers, ConvertTile_*, etc.) from "recursive include"
    code (family bodies)
  - `#ifndef NAME1` wrapper at line 996 -- structurally dead
    (NAME1 is never defined now), but kept for now; removing it
    is a separate structural cleanup

Removed:
  - templated DrawTile16 instantiation block
  - DRAW_TILE() macro definition for unclipped tile
  - `#define Z1 GFX.Z1` / `#define Z2 GFX.Z2` and matching
    `#undef`s
  - 264 lines of dispatch scaffold (level-2 plus level-3)

Added:
  - de-templated DrawTile16 section with banner comment
  - `DT_PIXEL_*` plotters (4 plotter shapes)
  - `TILE_BODY_UNCLIPPED` body macro
  - `DEFINE_DT_FN` / `DEFINE_DT_NAME2` fan-out
  - 42 explicit `DrawTile16*_*` functions
  - 6 explicit `Renderers_DrawTile16*` arrays

Symbol verification (vs pristine `8906302`):
  - 313 Draw* functions; full per-function size diff is empty
    across all 313 (verified). True pure refactor.
  - 44 Renderers_ arrays; full diff empty.
  - Build clean, zero warnings.

Files changed: `src/tile.c` +200/-336 (-136 net). The DrawTile16
de-templated section adds ~200 lines, but Stage 3 deletes ~264
lines of scaffold, for a net source-size reduction.

**The tile-untangle effort proper is done.** All 11 templated
families are de-templated, the level-2/3 dispatch scaffold is
gone, and the Mode 7 file-scope macros are gone. Optional Stage
4 (folding optimizations) and the structural cleanup of the
now-dead `#ifndef NAME1` wrapper are separate, profile-driven /
opportunistic concerns.

Lessons from earlier stages applied without trouble:
  - N parameter unparenthesized in plotter bodies (Stage 2.6b/2.7)
  - `&& (M)` clause preserved verbatim (Stage 2.5)
  - Per-NAME2 BPSTART threading (Stage 2.9)
  - PITCH as BP_STEP literal (Stage 2.10)

This commit also updates docs/tile-untangle-state.md to reflect
Stages 2.11 + 3 completion. Bundled SHA refresh from the previous
rebase.

Behaviour invariants -- to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on the 42 new DrawTile16 functions.

### `8906302` — tile.c: de-template DrawClippedTile16 (Stage 2.10)

Stage 2.10. De-templates DrawClippedTile16, the renderer that
draws a tile clipped to an arbitrary [StartPixel, StartPixel +
Width) horizontal range. Used by the BG-tile path when only part
of a tile is visible (clipped against a window or screen edge).

Body shape: 4 flip-case branches (no flip / H_FLIP only / V_FLIP
only / both flips), each running an 8-case switch on StartPixel
with fall-through and a per-case `--w` decrement to honour the
Width clamp. Each case calls DRAW_PIXEL with the assignment-and-
test `Pix = bp[idx]` -- if the source pixel is transparent (0),
the AND short-circuits and no write happens.

First family to thread both BPSTART and PITCH:
  - Non-interlace (Normal1x1, Normal2x1, Normal4x1, Hires):
    BPSTART = StartLine,                    BP_STEP = 8
  - Interlace (Interlace, HiresInterlace):
    BPSTART = (StartLine * 2 + BG.InterlaceLine), BP_STEP = 16

The original templated form used `8 * PITCH` for the per-line
cache pointer advance; we substitute the product directly into
the BP_STEP_EXPR parameter so the call site sees a literal (8 or
16). This keeps the pointer-arithmetic compiler-visible and
matches the templated codegen byte-for-byte.

ARGS shape: (uint32 Tile, uint32 Offset, uint32 StartPixel,
uint32 Width, uint32 StartLine, uint32 LineCount). Six parameters.
Note this differs from DrawMosaicPixel16's order
(StartLine/StartPixel/Width vs StartPixel/Width/StartLine).

Z values are runtime GFX.Z1 / GFX.Z2 (file-scope GFX struct
fields, set by the caller per BG/priority pass). Same as
DrawMosaicPixel16.

Section-local plotters CT_PIXEL_N1x1 / N2x1 / N4x1 / H2x1 mirror
the MP_PIXEL_* shapes from Stage 2.9 with section-local naming.
New TILE_BODY_CLIPPED body macro mirrors the previously-templated
DRAW_TILE() expansion -- 4 flip-case branches, 8-case switches
with fall-through, all 64 PIXEL_PLOT call sites preserved.

Removed:
  - templated DrawClippedTile16 instantiation block
  - #define Z1 GFX.Z1 / #define Z2 GFX.Z2 and matching #undefs
  - DRAW_TILE() macro definition for clipped-tile (~85 lines)

Added:
  - de-templated section with banner comment
  - CT_PIXEL_* plotters (4 plotter shapes)
  - TILE_BODY_CLIPPED body macro (~75 lines, mirrors original)
  - DEFINE_CT_FN / DEFINE_CT_NAME2 fan-out
  - 42 explicit DrawClippedTile16*_* functions (6 NAME2 x 7 math)
  - 6 explicit Renderers_DrawClippedTile16* arrays

Symbol verification (vs pristine `bb3cacf`):
  - 313 Draw* functions; full per-function size diff is empty
    across all 313 (verified). True pure refactor.
  - 44 Renderers_ arrays; full diff empty.
  - Build clean, zero warnings.

Files changed: `src/tile.c` +228/-105 (+123 net).

One non-Mode-7 family remains: DrawTile16 (Stage 2.11), the
hot-path renderer for unclipped 8-pixel tiles. After that, Stage
3 removes the level-2/level-3 templating scaffold (the
`#ifndef NAME1` / `#ifndef NAME2` recursion and the per-NAME2
BPSTART/PITCH/DRAW_PIXEL machinery in the `#else` branches).
The plan is to bundle the Stage 3 scaffold removal with the
Stage 2.11 commit, since Stage 2.11 makes the scaffold dead code
and removing dead code in the same commit that creates the
deadness follows the precedent set by earlier stages
(DRAW_TILE_NORMAL / DRAW_TILE_MOSAIC removal in Stage 2.6a /
Stage 2.6b/2.7).

Lessons from earlier stages applied without trouble:
  - N parameter unparenthesized in plotter bodies (Stage 2.6b/2.7)
  - `&& (M)` clause preserved verbatim (Stage 2.5)
  - Per-NAME2 BPSTART threading (Stage 2.9)

This commit also updates docs/tile-untangle-state.md to reflect
Stage 2.10 completion. Bundled SHA refresh from the previous
rebase.

Behaviour invariants -- to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on the 42 new DrawClippedTile16 functions.

### `bb3cacf` — tile.c: de-template DrawMosaicPixel16 (Stage 2.9)

Stage 2.9. De-templates DrawMosaicPixel16, the renderer that
replicates a single mosaic pixel (sourced from one cached tile)
across an HMosaic-by-LineCount block. Used by the BG-mosaic path
(non-Mode-7 mosaic).

First family in the de-templating effort to need **all six
NAME2 variants**, including Interlace and HiresInterlace -- and
therefore the first to thread per-NAME2 BPSTART expressions
through the fan-out:

  - Non-interlace (Normal1x1, Normal2x1, Normal4x1, Hires):
    BPSTART = StartLine
  - Interlace (Interlace, HiresInterlace):
    BPSTART = (StartLine * 2 + BG.InterlaceLine)

PITCH is not used by mosaic-pixel (per the original "PITCH is
not used" comment) so it isn't threaded. The Interlace variants
differ from Normal2x1 / Hires only in which row of the cached
tile gets sampled.

ARGS shape: `(uint32 Tile, uint32 Offset, uint32 StartLine,
uint32 StartPixel, uint32 Width, uint32 LineCount)` -- six
parameters.

Z values are runtime `GFX.Z1` / `GFX.Z2` (file-scope GFX struct
fields, set by the caller per BG/priority pass). The plotter's
"M" parameter is the literal 1 -- the source-pixel transparency
check (`if (Pix)`) happens once outside the replication loop.

Section-local plotters `MP_PIXEL_N1x1` / `N2x1` / `N4x1` / `H2x1`
mirror the original DRAW_PIXEL_* shapes with these conventions
baked in. New `TILE_BODY_MOSAIC_PIXEL` body macro mirrors the
previously-templated DRAW_TILE() expansion.

Removed:
  - templated DrawMosaicPixel16 instantiation block
  - `#define Z1 GFX.Z1` / `#define Z2 GFX.Z2` and matching `#undef`s
  - DRAW_TILE() macro definition for mosaic-pixel

Added:
  - de-templated section with banner comment
  - `MP_PIXEL_*` plotters (4 plotter shapes)
  - `TILE_BODY_MOSAIC_PIXEL` body macro
  - `DEFINE_MP_FN` / `DEFINE_MP_NAME2` fan-out
  - 42 explicit `DrawMosaicPixel16*_*` functions (6 NAME2 x 7 math)
  - 6 explicit `Renderers_DrawMosaicPixel16*` arrays

Symbol verification (vs pristine `9ac00cb`):
  - 313 Draw* functions; full per-function size diff is empty
    (verified). True pure refactor.
  - 44 Renderers_ arrays; full diff empty.
  - Build clean, zero warnings.

Files changed: `src/tile.c` +160/-43 (+117 net).

Two non-Mode-7 families remain: DrawClippedTile16 (Stage 2.10),
DrawTile16 (Stage 2.11). After those, Stage 3 removes the
level-2/level-3 templating scaffold (the `#ifndef NAME1` /
`#ifndef NAME2` recursion and the per-NAME2 BPSTART/PITCH/
DRAW_PIXEL machinery in the `#else` branches). The shared
helpers `GET_CACHED_TILE`, `IS_BLANK_TILE`, `SELECT_PALETTE`,
`H_FLIP`, `V_FLIP` stay -- they're used by all the tile-rendering
families and live at file scope.

Lessons from earlier stages applied without trouble:
  - N parameter unparenthesized in plotter bodies (Stage 2.6b/2.7)
  - `&& (M)` clause preserved verbatim (Stage 2.5)
  - 7-fold math fan-out matches the existing pattern

This commit also updates docs/tile-untangle-state.md to reflect
Stage 2.9 completion. Bundled SHA refresh from the previous
rebase.

Behaviour invariants -- to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on the 42 new DrawMosaicPixel16 (`bb3cacf`)
functions.

### `9ac00cb` — tile.c: de-template DrawBackdrop16 (Stage 2.8)

Stage 2.8. First non-Mode-7 family de-templated. DrawBackdrop16
is the renderer that fills a horizontal range of the scanline
buffer with the backdrop colour (palette index 0) at depth 1,
drawn after all BG/OBJ layers.

Structurally simpler than Mode 7:
  - No tile lookup, no per-pixel data
  - Z is constant 1 (depth 1, drawn last)
  - Pix is constant 0 (always palette index 0)
  - The DRAW_PIXEL "M" parameter (test) is constant 1
  - ARGS shape is `(uint32 Offset, uint32 Left, uint32 Right)` --
    Offset is a parameter, not a body-local

Different ARGS from Mode 7 (which uses `(uint32 Left, uint32 Right,
int D)`) means the body and fan-out can't share with Mode 7's
machinery. The de-templated form uses a separate set of
`BACKDROP_PIXEL_*` plotter macros and a `TILE_BODY_BACKDROP` body
macro.

The original templated form baked Z = 1, Pix = 0 as preprocessor
#defines and used NO_INTERLACE to skip Interlace/HiresInterlace
variants. The de-templated form bakes the same constants directly
into the plotter macros and simply doesn't emit Interlace
variants.

Per-NAME2 plotters mirror the original DRAW_PIXEL_* expansion
exactly, including the `&& (1)` clause from the templated `M = 1`
substitution. This preserves the expression tree shape the
compiler sees and gives byte-identical codegen.

Files changed: `src/tile.c`, +136/-39, +97 net. The de-templated
form is bigger than the templated source because backdrop's
templated form was tiny (40 lines, all the work was in the
level-2/3 dispatch); explicit form unrolls 4 plotter macros plus
body plus 4 fan-out invocations.

Removed:
  - templated DrawBackdrop16 instantiation block
  - `#define NO_INTERLACE` / `Z1` / `Z2` / `Pix` and matching
    `#undef`s
  - DRAW_TILE() macro definition for backdrop

Added:
  - de-templated section with banner comment
  - `BACKDROP_PIXEL_N1x1` / `N2x1` / `N4x1` / `H2x1` plotters
  - `TILE_BODY_BACKDROP` body macro
  - `DEFINE_BACKDROP_FN` / `DEFINE_BACKDROP_NAME2` fan-out
  - 28 explicit `DrawBackdrop16*_*` functions
  - 4 explicit `Renderers_DrawBackdrop16*` arrays

Symbol verification (vs pristine `3c7ad4f`):
  - 313 Draw* functions; full per-function size diff vs pristine
    is empty (verified). True pure refactor.
  - 44 Renderers_ arrays; full diff empty.
  - Build clean, zero warnings.

Lessons from earlier stages applied (no codegen surprises this
time):
  - N parameter unparenthesized in plotter bodies (Stage 2.6b/2.7
    lesson)
  - `&& (M)` clause preserved verbatim in plotter, even though
    M is the literal `1` (Stage 2.5 lesson about preserving
    expression-tree shape)

The non-Mode-7 families now have one done (DrawBackdrop16) and
three remaining (DrawMosaicPixel16, DrawClippedTile16,
DrawTile16). After those, Stage 3 removes the level-2/3
templating scaffold.

This commit also updates docs/tile-untangle-state.md to reflect
Stage 2.8 completion. Bundled SHA refresh from the previous
rebase.

Behaviour invariants -- to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on the 28 new backdrop functions.

### `3c7ad4f` — tile.c: de-template mosaic Mode 7 + drop dead NAME2 variants (Stage 2.6b/2.7)

Combines Stage 2.6b (drop unused NAME2 variants for de-templated
native Mode 7 BG families) and Stage 2.7 (de-template the mosaic
Mode 7 NAME1 families) into one commit.

Stage 2.7 part: de-templates DrawMode7MosaicBG1 and DrawMode7MosaicBG2.
Same approach as native: a `TILE_BODY_MOSAIC_M7` body macro
mirrors the structure of the previously-templated DRAW_TILE_MOSAIC,
parameterized for explicit MATH_SELECTOR/MATH_OP/Z_EXPR/MASK_VAL/
DC_EXPR/BG_INDEX/PIXEL_PLOT. The `BG_INDEX` parameter (0 or 1)
selects which `PPU.BGMosaic[]` entry to read for the per-BG
mosaic-enable bit; this is the only structural difference from
the native body. New fan-out macros `DEFINE_M7_MOSAIC_FN`,
`DEFINE_M7_MOSAIC_NAME2`, `DEFINE_M7_MOSAIC_BG` mirror the native
ones. 42 functions emitted (2 BGs x 3 NAME2 x 7 math) plus 6
arrays.

Stage 2.6b part: Drop Normal4x1, Interlace, HiresInterlace from
both native and mosaic Mode 7 BG families. The dispatcher only
selects Normal1x1, Normal2x1, and Hires (LTO had been stripping
the others all along). The `M7N_PIXEL_N4x1` macro is also
dropped since no surviving variant uses it.

Together: 84 native + 42 mosaic = 126 native+mosaic Mode 7
functions emitted explicitly. Down from 168+168 = 336 in the
templated form before LTO (LTO stripped half on both sides).

Pure refactor confirmed via byte-identical machine code:
  - 313 Draw* functions, full per-function size diff vs pristine
    `f5894be` is empty across all 313.
  - 44 Renderers_ arrays, full diff empty.

Codegen-correctness lesson learned: my first draft of
`M7N_PIXEL_*` plotter macros parenthesized the N parameter
(`Offset + 2 * (N)`). For native Mode 7 this was byte-identical
because the call site passes `N = x` (a simple loop variable)
and `2 * x` == `2 * (x)`. For mosaic the call site passes
`N = w + h * GFX.PPL` (a compound expression), and the original
templated `DRAW_PIXEL_*` macros used unparenthesized N, so the
substitution was `2 * w + h * GFX.PPL` not `2 * (w + h * GFX.PPL)`
-- semantically different memory locations. I changed the
plotter macros to use unparenthesized N to match the original;
this restored byte-identical mosaic codegen and (incidentally)
preserves whatever buffer-offset semantics the original assumed.

Bundled cleanups:
  - `DRAW_TILE_MOSAIC` macro definition deleted (~135 lines)
  - file-scope `Z1`/`Z2`/`MASK`/`DCMODE`/`BG`/`NO_INTERLACE`
    `#define`s for Mode 7 deleted
  - `M7N_PIXEL_N4x1` definition deleted
  - dangling `#undef DRAW_TILE_MOSAIC` and `#undef NO_INTERLACE`
    deleted

Removed:
  - templated DrawMode7MosaicBG{1,2} instantiation block
  - DRAW_TILE_MOSAIC macro definition
  - file-scope Mode 7 #defines (Z1, Z2, MASK, DCMODE, BG,
    NO_INTERLACE)
  - M7N_PIXEL_N4x1 macro
  - 3 dead NAME2 variants per BG (Normal4x1, Interlace,
    HiresInterlace) -- previously LTO-stripped but visible in
    the source

Added:
  - TILE_BODY_MOSAIC_M7 body macro
  - DEFINE_M7_MOSAIC_FN / DEFINE_M7_MOSAIC_NAME2 /
    DEFINE_M7_MOSAIC_BG fan-out macros
  - 42 explicit DrawMode7MosaicBG{1,2}* functions
  - 6 explicit Renderers_DrawMode7MosaicBG{1,2}* arrays

Files changed: `src/tile.c`, +251/-269, -18 net (the ~135-line
DRAW_TILE_MOSAIC definition and the dead-NAME2 boilerplate
together outweigh the new mosaic body and fan-out).

The Mode 7 group is now complete. Stage 2.8 (DrawBackdrop16)
starts the non-Mode-7 families.

This commit also updates docs/tile-untangle-state.md to reflect
Stage 2.6b/2.7 completion. Bundled SHA refresh from the previous
rebase (Stage 2.6a entry switching from "(this commit)"
placeholder to its real upstream SHA `f5894be`).

Behaviour invariants -- to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on the new mosaic functions.

### `f5894be` — tile.c: de-template native Mode 7 BG renderers (Stage 2.6a)

Stage 2.6a. De-templates the two non-mosaic native Mode 7 NAME1
families: `DrawMode7BG1` and `DrawMode7BG2`. Their mosaic
counterparts (`DrawMode7MosaicBG1`, `DrawMode7MosaicBG2`) remain
templated and are de-templated separately in Stage 2.7.

Native Mode 7 is the first family to use the **full NAME2 set**
in level-2 dispatch (Normal1x1, Normal2x1, Normal4x1, Hires,
Interlace, HiresInterlace), so the fan-out structure is two
levels deeper than the M7 hires families:

- `M7N_PIXEL_*` macros: per-NAME2 inline pixel plotters that
  mirror the original DRAW_PIXEL_N1x1 / N2x1 / N4x1 / H2x1 macro
  shapes. Take MATH_SELECTOR / MATH_OP / Z_EXPR as parameters
  instead of reading from enclosing scope. Preserve the
  assignment-and-test expression form
  `(Pix = (b & MASK_VAL))` so the compiler can short-circuit
  the AND when the Z-test fails (lesson from Stage 2.5).
- `TILE_BODY_NORMAL_M7`: per-line Mode 7 body, takes PIXEL_PLOT
  as a parameter. Identical structure and arithmetic to the
  former DRAW_TILE_NORMAL macro.
- `DEFINE_M7_FN`: emits one function with the chosen math op
  and pixel plotter.
- `DEFINE_M7_NAME2`: 7-fold math fan-out plus dispatch array.
- `DEFINE_M7_BG`: 6-fold NAME2 fan-out, used twice (BG1, BG2).

84 functions emitted (2 BGs x 6 NAME2 x 7 math) plus 12 dispatch
arrays. Of those, 36 functions and 6 arrays are LTO-stripped
because the dispatcher only references the Normal1x1, Normal2x1,
and Hires variants. The Normal4x1 / Interlace / HiresInterlace
variants are emitted to match the templated form byte-for-byte;
they're removed in Stage 2.6b.

Removed:
  - templated `DrawMode7BG1` / `DrawMode7BG2` instantiation blocks
  - `DRAW_TILE_NORMAL` macro definition (no longer referenced;
    last user was the templated BG1/BG2 instantiations -- the
    Mosaic blocks below this section use DRAW_TILE_MOSAIC, not
    DRAW_TILE_NORMAL)

Added:
  - banner section comment explaining the family
  - `M7N_PIXEL_N1x1` / `N2x1` / `N4x1` / `H2x1` per-NAME2 plotters
  - `TILE_BODY_NORMAL_M7` body macro
  - `DEFINE_M7_FN` / `DEFINE_M7_NAME2` / `DEFINE_M7_BG` fan-out
  - 84 explicit `DrawMode7BG{1,2}*` functions
  - 12 explicit `Renderers_DrawMode7BG{1,2}*` arrays

The mosaic variants (DrawMode7MosaicBG1, DrawMode7MosaicBG2) are
left templated for Stage 2.7. They still use the file-scope
Z1/Z2/MASK/DCMODE/BG/NO_INTERLACE #defines which therefore stay
alive across the Mosaic block.

Symbol verification (vs pristine `624492c`):
  - 313 Draw* functions; full per-function size diff vs pristine
    is empty across all 313 (verified, not just the 84 native
    Mode 7 ones). True pure refactor.
  - 44 Renderers_ arrays; full diff empty.
  - Build clean. 8 unused-Renderers warnings up from 4 -- the
    extra 4 are the BG1/BG2 Normal4x1/Interlace/HiresInterlace
    variants that were previously hidden inside the templated
    expansion and are now visible at file scope. Stage 2.6b
    removes them and cleans up the warnings.

Behaviour invariants -- to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on the new functions.

This commit also updates docs/tile-untangle-state.md (in-tree)
to reflect Stage 2.6a completion. Bundled SHA refresh from the
previous rebase (Stage 2.5 entry switching from "(this commit)"
placeholder to its real upstream SHA `624492c`).

### `624492c` — tile.c: de-template Mode 7 HR renderers (Stage 2.5)

Stage 2.5. Same fan-out pattern as BL (Stage 2.4) but with inline
nearest-neighbour DRAW_PIXEL writes instead of bilinear blending.
A local `DEFINE_M7_HR_FAMILY` macro materializes 14 functions (7
math variants x 2 BGs).

HR is the original 2x nearest-neighbour Mode 7 path: two samples
per native pixel using half-step (aa_h = aa/2), each written
through the standard Z-test + MATH pixel write. Same overall
structure as BL but inline of the per-pixel write instead of
helper invocation.

The per-pixel write inlines the original Normal1x1 DRAW_PIXEL
shape exactly: `if (Z_EXPR > GFX.DB[...] && (Pix = (b & MASK)))`
-- single assignment-and-test expression, no separate Pix
assignment, no idx local. This matches the original templated
codegen byte-for-byte. (An earlier draft separated the assignment
and used an idx local; that produced ~+58 bytes per BG2 function
and ~18 extra instructions because the compiler couldn't exploit
the Z-test short-circuit to skip the AND-with-MASK on
non-drawing pixels. Restoring the original shape brought the
codegen back to byte-identical.)

Files changed: `src/tile.c`, +280/-195, +85 net.

Removed:
  - templated `DrawMode7BG{1,2}HR` instantiation block
  - `DRAW_TILE_NORMAL_M7HIRES` macro definition (~140 lines, no
    longer referenced -- HR was the only user)
  - `ARGS / M7HIRES_ONLY` scope `#define`s and `#undef`s (HR was
    the last user inside that scope)
  - dangling `#undef DRAW_TILE_NORMAL_M7HIRES`

Added:
  - de-templated section with banner comment explaining the family
  - `DEFINE_M7_HR_FAMILY` fan-out macro with parameter docs
  - 14 explicit `DrawMode7BG{1,2}HR*_Normal1x1` functions
  - 2 explicit `Renderers_DrawMode7BG{1,2}HRNormal1x1` arrays

Bundled stale-comment fixes (replaced as part of the section
banner rewrite):
  - "These functions are scaffolded but not yet referenced by any
    dispatcher. The wiring lands in a follow-up patch." This was
    true at one point, but the dispatcher in `S9xSelectTileRenderers`
    references `Renderers_DrawMode7BG{1,2}HRNormal1x1` already.
    Banner now describes the actual wiring.
  - The cross-reference in the BL helper section header
    ("same output rate as DRAW_TILE_NORMAL_M7HIRES") was updated
    to point to the de-templated HR family below, since the
    macro it referenced no longer exists.

Symbol verification (vs pristine `6fde805`):
  - 14 HR functions present, byte-identical machine code.
    Per-function sizes verified for all 14 -- diff of size lists
    is empty:

      DrawMode7BG1HR_Normal1x1        0x5a9
      DrawMode7BG1HRAdd_Normal1x1     0x8a1
      DrawMode7BG1HRAddF1_2_Normal1x1 0x7b1
      DrawMode7BG2HR_Normal1x1        0x607
      DrawMode7BG2HRSubS1_2_Normal1x1 0xb60
      (all match pristine exactly)

  - 2 Renderers_ arrays present
  - Total Draw* count unchanged: 313
  - Total Renderers_* count unchanged: 45
  - No new warnings

Five M7 hires families are now fully de-templated: HR4X
(0cbca9e), BL4X (69f75a1), BL1X (0e147ff), BL (6fde805), HR
(`624492c`). The Mode 7 hires templating scaffold is gone:
the second `M7HIRES_ONLY` scope was the last consumer, and it's
now removed. Stage 2.6 (native Mode 7) is next.

This commit also updates docs/tile-untangle-state.md (in-tree)
to reflect Stage 2.5 completion.

### `6fde805` — tile.c: de-template Mode 7 BL renderers (Stage 2.4)

Stage 2.4. Same fan-out pattern as BL4X / BL1X: a local
`DEFINE_M7_BL_FAMILY` macro materializes 14 functions (7 math
variants x 2 BGs) using the parameterized helpers from `54c9153`.

BL is the original 2x bilinear-filtered Mode 7 path: two samples
per native pixel using half-step (aa_h = aa/2) sub-pixel division,
each blended through M7HR_SAMPLE_BILINEAR / M7HR_BLEND_AND_WRITE.
Differs from BL4X in step factor (1/2 instead of 1/4) and output
indexing (2*x and 2*x+1 instead of 4*x..4*x+3).

Files changed: `src/tile.c`, +250/-180, +70 net.

Removed:
  - templated `DrawMode7BG{1,2}BL` instantiation block
  - `DRAW_TILE_NORMAL_M7HIRES_BILINEAR` macro definition (~110
    lines, no longer referenced)
  - `ARGS / M7HIRES_ONLY` scope `#define`s (the BL block was the
    last user; HR4X / BL4X / BL1X / BL2X are now all de-templated)
  - dangling `#undef ARGS / #undef M7HIRES_ONLY / #undef
    DRAW_TILE_NORMAL_M7HIRES_BILINEAR`

Added:
  - de-templated section with banner comment explaining the family
  - `DEFINE_M7_BL_FAMILY` fan-out macro with parameter docs
  - 14 explicit `DrawMode7BG{1,2}BL*_Normal1x1` functions
  - 2 explicit `Renderers_DrawMode7BG{1,2}BLNormal1x1` arrays

Bundled stale-comment fixes (replaced as part of the section
banner rewrite):
  - The "BG2BL pins to low priority tier" claim at the head of
    the BL block was wrong post-`51b05ee` (the actual code uses
    the EXTBG per-pixel priority bit just like the other BG2
    paths). Banner now accurately describes the priority handling.
  - The "colour math is bypassed" claim was a duplicate of the
    one fixed in `54c9153`; deleted with the rest of the old
    header.

Symbol verification (vs pristine `0e147ff`):
  - 14 BL functions present, byte-identical machine code:

      DrawMode7BG1BLAdd_Normal1x1     0x32d1
      DrawMode7BG1BLAddF1_2_Normal1x1 0x2f81
      DrawMode7BG2BL_Normal1x1        0x2d0a
      DrawMode7BG2BLSubS1_2_Normal1x1 0x3db2
      (all match pristine exactly; same offsets too)

  - 2 Renderers_ arrays present
  - Total Draw* count unchanged: 313
  - Total Renderers_* count unchanged: 45
  - No new warnings

Behaviour invariants — to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on BL.

This commit also updates docs/tile-untangle-state.md (in-tree)
to reflect Stage 2.4 completion.

### `0e147ff` — tile.c: de-template Mode 7 BL1X renderers (Stage 2.3)

Stage 2.3. Same fan-out pattern as HR4X / BL4X: a local
`DEFINE_M7_BL1X_FAMILY` macro materializes 14 functions (7 math
variants x 2 BGs) using the parameterized helpers from `54c9153`.

BL1X is the bilinear-filtered Mode 7 path at native (1x) width --
one sample per native pixel, but each sample goes through bilinear
blending. Used when the user enables bilinear without enabling
hires.

Differs from BL2X / BL4X in that there's no sub-pixel division of
aa/cc (no aa_q/aa_h locals) and output goes to GFX.S[Offset + x]
(no 2*x or 4*x sub-pixel index). The wrap path uses
M7HR_SAMPLE_BILINEAR; the repeat path inlines the LOOKUP_4 /
LOOKUP_4_FILL split for per-sample range checking.

Files changed: `src/tile.c`, +211/-133, +78 net.

Removed:
  - templated `DrawMode7BG{1,2}BL1X` instantiation block (~38 lines)
  - `DRAW_TILE_NORMAL_M7BL_1X` macro definition (~90 lines, no
    longer referenced)
  - dangling `#undef DRAW_TILE_NORMAL_M7BL_1X`

Added:
  - de-templated section with banner + fan-out macro doc
  - 14 explicit `DrawMode7BG{1,2}BL1X*_Normal1x1` functions
  - 2 explicit `Renderers_DrawMode7BG{1,2}BL1XNormal1x1` arrays

Symbol verification (vs pristine `bc47509`):
  - 14 BL1X functions present, byte-identical machine code
    (verified by comparing function sizes via nm --print-size)
  - 2 Renderers_ arrays present
  - Total Draw* count unchanged: 313
  - Total Renderers_* count unchanged: 45
  - No new warnings

Behaviour invariants — to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on BL1X.

This commit also updates docs/tile-untangle-state.md (in-tree)
to reflect Stage 2.3 completion. Per the new convention, state
file updates ride along in the relevant commit instead of being
batched into separate "Update doc" commits.

### `69f75a1` — tile.c: de-template Mode 7 BL4X renderers (Stage 2.2)

Stage 2.2b. Same fan-out pattern as HR4X: a local
`DEFINE_M7_BL4X_FAMILY` macro materializes 14 functions (7 math
variants x 2 BGs) using the parameterized helpers from `54c9153`.

Files changed: `src/tile.c`, +217/-135, +82 net.

Removed:
  - templated `DrawMode7BG{1,2}BL4X` instantiation block
  - `DRAW_TILE_NORMAL_M7HIRES_4X_BILINEAR` macro definition (~100
    lines, no longer referenced)
  - dangling `#undef DRAW_TILE_NORMAL_M7HIRES_4X_BILINEAR`

Added:
  - de-templated section with banner + fan-out macro doc
  - 14 explicit `DrawMode7BG{1,2}BL4X*_Normal1x1` functions
  - 2 explicit `Renderers_DrawMode7BG{1,2}BL4XNormal1x1` arrays

Symbol verification (vs pristine `54c9153`):
  - 14 BL4X functions present, byte-identical machine code
  - 2 Renderers_ arrays present
  - Total Draw* count unchanged: 313
  - Total Renderers_* count unchanged: 45
  - No new warnings

The byte-identical machine code is the strongest possible signal
this is a pure refactor: the compiler produces the same .text
bytes from the explicit form as from the templated form because
the textual substitution chain produces the same expression tree.

Behaviour invariants — to be verified externally by Lib before
push: audio bit-identical, savestate compat, valgrind clean,
7-distinct-math-variants on BL4X.

### `54c9153` — tile.c: parameterize Mode 7 bilinear helper macros (Stage 2.2 prep)

Stage 2.2a. Pure refactor in preparation for BL4X de-templating.
Six helper macros that the bilinear renderer paths share
(M7HR_LOOKUP_PIX, M7HR_LOOKUP_PIX_RAW, M7HR_LOOKUP_4,
M7HR_LOOKUP_4_FILL, M7HR_BLEND_AND_WRITE, M7HR_SAMPLE_BILINEAR)
previously read VRAM1, MASK, Z1, Z2, Offset, and MATH(...) from
enclosing scope via the level-1 templating's #define machinery.
They now take those values as explicit parameters.

Files changed: `src/tile.c`, +187/-74, +113 net.

Why: de-templated renderers (HR4X done; BL4X next) cannot rely on
enclosing-scope values because they're not inside the level-1
#define / #include / #undef scaffold. Parameterizing the helpers
lets the same macros serve both templated and de-templated
callers.

Updated:
  - Helper macros take new params: vram1, mask (LOOKUP_*),
    math_selector, math_op, z1_expr, z2_expr, offset (BLEND_AND_WRITE
    and SAMPLE_BILINEAR forwards everything).
  - Level-3 expansion now also defines `MATH_SELECTOR` and
    `MATH_OP` alongside `MATH` for each math variant.
  - 24 templated call sites in BL2X / BL4X / BL1X DRAW_TILE
    macros forward `VRAM1, MASK, MATH_SELECTOR, MATH_OP, Z1, Z2,
    Offset` from their enclosing scope.

Bundled fix: stale doc comment that claimed BL bypasses MATH
and the 7 variants are functionally identical (true before
`6d214cf` color-math fix; false now). Bundled because the
refactor changed the truth-value of the comment.

Verified byte-identical machine code for all BL functions vs
pristine `0cbca9e`. Symbol set unchanged (313 functions, 45
arrays).

### `0cbca9e` — tile.c: de-template Mode 7 HR4X renderers

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

(none currently — the BL color-math stale comment was fixed as
part of `54c9153` since the macro refactor in that commit changed
the truth-value of the comment.)

---

*Last updated: Stage 3.5 + Stage 4 commit (structural cleanup + fan-out unrolling). Branch `tile-untangle` past upstream `a40fa37` by one commit. All 313 renderer functions now written explicitly; no macros emit function definitions. Body macros, plotters, color-math, and helpers all remain as macros. Optional Stage 5 (folding optimizations) is profile-driven future work.*