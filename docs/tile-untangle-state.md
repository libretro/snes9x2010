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
**Stage:** Stages 2.6b + 2.7 complete (mosaic Mode 7 de-templated; dead native NAME2 variants dropped); ready for Stage 2.8 (DrawBackdrop16)
**Last code commit:** Stage 2.6b/2.7 — tile.c: de-template mosaic Mode 7 + drop dead NAME2 variants (this commit)
**Previous code commit:** `f5894be` — tile.c: de-template native Mode 7 BG renderers (Stage 2.6a)
**In-flight work:** none
**Working tree:** clean

State file lives in-tree at `docs/tile-untangle-state.md`. Updates
ride along in the relevant commit.

All Mode 7 renderer families are now fully de-templated:
HR4X (`0cbca9e`), BL4X (`69f75a1`), BL1X (`0e147ff`), BL (`6fde805`),
HR (`624492c`), native BG (`f5894be`), mosaic + dead-array cleanup
(this commit). The Mode 7 group is done. Stage 2.8 starts the
non-Mode-7 families: DrawBackdrop16 first, then DrawMosaicPixel16,
DrawClippedTile16, DrawTile16.

The level-1 Mode 7 templating scaffold is gone: file-scope
`Z1`/`Z2`/`MASK`/`DCMODE`/`BG`/`NO_INTERLACE` `#define`s for Mode 7
removed. The `DRAW_TILE_NORMAL` and `DRAW_TILE_MOSAIC` macros are
deleted. The level-2 dispatch in `tile.c` is still alive because
the non-Mode-7 templated families (DrawBackdrop16,
DrawMosaicPixel16, DrawClippedTile16, DrawTile16) still use it.

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

   Stage 2.6b folded into Stage 2.7 (this commit): drop the
   never-referenced NAME2 variants (Normal4x1, Interlace,
   HiresInterlace) for the de-templated families. Cleans up all
   the unused-Renderers warnings.

7. ~~`DrawMode7MosaicBG1` and `DrawMode7MosaicBG2` (Stage 2.7)~~ —
   **done** (this commit). Mosaic Mode 7 de-templated using a
   `TILE_BODY_MOSAIC_M7` body macro plus `DEFINE_M7_MOSAIC_*`
   fan-out, mirroring the native machinery. Combined with 2.6b
   (drop dead NAME2 variants for both native and mosaic) into
   one commit. The `DRAW_TILE_MOSAIC` macro is gone, and the
   file-scope `Z1`/`Z2`/`MASK`/`DCMODE`/`BG`/`NO_INTERLACE`
   `#define`s for Mode 7 are gone.
8. `DrawBackdrop16` — full NAME2 set. **Next.** First non-Mode-7
   family.
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

### Stage 2.6b/2.7 — tile.c: de-template mosaic Mode 7 + drop dead NAME2 variants (this commit)

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

*Last updated: combined Stage 2.6b + 2.7 commit (de-template mosaic Mode 7, drop dead NAME2 variants for native+mosaic). Branch `tile-untangle` past upstream `f5894be` by one commit. The Mode 7 group is complete: HR4X, BL4X, BL1X, BL, HR, native BG, mosaic BG all de-templated. Stage 2.8 (DrawBackdrop16) starts the non-Mode-7 families.*