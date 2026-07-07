# snes9x2010 (libretro)

A libretro port of a fast, portable Super Nintendo Entertainment System /
Super Famicom emulator descended from Snes9x 1.52 (the same lineage as the
former "snes9x-next"). It targets low-to-mid power hardware while keeping
broad game compatibility, and integrates with RetroArch and other libretro
frontends.

The emulator core is written in C; the SPC700 + S-DSP audio and the
tile renderer include hand-tuned SSE2 (x86) and NEON (ARM) paths with a
portable scalar fallback.

## Building

The core builds against the standard libretro Makefile:

```
make -f Makefile.libretro platform=<platform>
```

Common `platform` values include `unix`, `osx`, `win`, `android`, and the
various console targets. The result is `snes9x2010_libretro.<ext>`, which a
libretro frontend loads.

## Features

### CPU / PPU / APU

- Full 65c816 main CPU, SA-1 sub-CPU, and SPC700 sound CPU emulation.
- S-DSP audio with selectable interpolation (see the audio option below).
- Complete PPU: all background modes including Mode 7, offset-per-tile,
  hi-res and pseudo-hi-res, mosaic, windowing, and colour math (add /
  subtract, half, fixed-colour and sub-screen), with SSE2/NEON accelerated
  colour math and tile plotting.
- Save states (serialize / unserialize) and battery SRAM.
- NTSC and PAL region handling with auto-detection.

### Enhancement chips / coprocessors

- **SA-1** — accelerator CPU (Super Mario RPG, Kirby Super Star, etc.).
- **Super FX / GSU** (`fxemu.c`) — Star Fox, Yoshi's Island, Doom.
- **DSP-1 / DSP-2 / DSP-3 / DSP-4** (`dsp.c`) — the NEC uPD77C25 math
  coprocessors (Pilotwings, Super Mario Kart, and others).
- **CX4 / C4** (`c4emu.c`) — the Capcom Hitachi coprocessor (Mega Man X2/X3).
- **S-DD1** — graphics decompression (Star Ocean, Street Fighter Alpha 2).
- **SPC7110** — data decompression + real-time clock (Far East of Eden Zero,
  Momotarou Dentetsu Happy).
- **OBC1** (`obc1.c`) — Metal Combat: Falcon's Revenge.
- **Seta ST010 / ST011 / ST018** (`seta.c`) — F1 ROC II, Hayazashi Nidan
  Morita Shougi, and others.
- **S-RTC** — the SharpRTC real-time clock (Daikaijuu Monogatari II).
- **MSU-1** — Near's virtual expansion chip for CD-quality audio and large
  data ROMs used by MSU-1 game patches.

### Add-on hardware

- **Satellaview (BS-X)** — the BS-X base unit with an emulated BS Memory
  flash cassette and the Satellaview real-time clock. Loadable both as a
  single auto-detected dump and through a dedicated two-slot subsystem
  (base cartridge + BS Memory pack).
- **Sufami Turbo** — the Bandai base cassette plus one or two minicarts,
  loadable either as a pre-merged combined image or through a dedicated
  two-slot subsystem (base cassette + minicart file). Requires the Sufami
  Turbo base-cassette BIOS (`STBIOS.bin`), matching every other emulator.

### Input

- Standard controllers, multitap, the SNES Mouse, the Super Scope, and the
  Justifier lightgun.

### Core options

Exposed to the frontend (key prefix `snes9x_2010_`):

- `region` — Auto / NTSC / PAL.
- `aspect` — output aspect ratio.
- `audio_interpolation` — S-DSP interpolation: **Gaussian** (the default and
  the only hardware-accurate mode), **Cubic**, or **Sinc** (the latter two
  brighten/sharpen the sound but are not hardware-accurate).
- `overclock` / `overclock_cycles` — main-CPU / cycle overclock to reduce
  slowdown in demanding games (accuracy trade-off).
- `superfx_cycle_accuracy` — toggle cycle-accurate Super FX timing.
- `mode7_hires` / `mode7_hires_bilinear` — higher-resolution Mode 7 with
  optional bilinear filtering.
- `pseudo_hires_blend` — blend pseudo-hi-res (512-wide dithering) output.
- `reduce_sprite_flicker` — raise the per-scanline sprite limit.
- `block_invalid_vram_access`, `blargg` (NTSC filter), `frameskip` /
  `frameskip_threshold`, `turbodelay`, and MSU-1 enhanced audio (below).

## Changes in this fork

This fork carries a series of accuracy, correctness, and feature commits on
top of the upstream base. Each was verified against a reference (ares /
MiSTer RTL / Snes9x mainline) or on real hardware before landing.

### SA-1

- Made the SA-1 arithmetic unit (multiply / divide / cumulative-sum) bit-exact
  versus hardware.
- Fixed multiple-evaluation of a side-effecting address in `S9xSA1GetByte`.
- Implemented the type-1 character-conversion DMA (CC1) read path.
- Honoured BW-RAM write protection (`$2226`/`$2227`/`$2228`).
- Fixed `$2300` (SFR) to read the latched IRQ flags rather than the
  enable-gated line.

### Real-time clocks

- **SPC7110** and **S-RTC** now tick on the emulated clock instead of host
  wall-clock time, making clock-dependent games deterministic and
  savestate-safe.

### MSU-1

- Implemented the MSU-1 audio + data-ROM expansion.
- Obtain the ROM path via `GET_GAME_INFO_EXT` so MSU-1 works correctly under
  RetroArch.
- Stream audio through a RAM buffer instead of per-sample file I/O.
- Added an **"MSU-1 Enhanced Audio"** option for a 44.1 kHz output pipeline.

### Satellaview (BS-X)

- Added an ares-accurate BS Memory flash chip (Sharp LH28F800SU family).
- Drove the flash chip from the BS-X base-unit command path.
- Provided the full Satellaview RTC date (weekday / day / month / year).
- Added a **Satellaview (BS-X + Memory Pack)** two-slot subsystem.

### Sufami Turbo

- Load Sufami Turbo **combined images** (base-cassette BIOS with the minicart
  appended) that previously failed to load.
- Added a **Sufami Turbo** two-slot subsystem so a base cassette and a bare
  minicart dump can be loaded as separate files (base detected by signature,
  slot order does not matter).

### Compatibility fixes

- **Chou Aniki - Bakuretsu Rantou Hen (Japan)** — fixed a permanent black
  screen (the game's NMI-driven loop stalled) by adding it to the per-game
  APU-speedup timing list, matching mainline Snes9x's fix for this title.
- **Madden NFL 96** — fixed silent audio: the game was missing from the
  APU time-overflow list, so its SPC700 sound driver deadlocked after boot
  and stopped issuing DSP writes. Added it to the list.
- **MechWarrior 3050 / Battle Tech 3050** — fixed degraded sound: the
  existing APU time-overflow entry used an exact ROM-name match against the
  ROM ID ("A35"), so it never applied. Corrected to match by ROM ID, also
  covering the title's regional variants.

### Audited (verified accurate, no change required)

As part of this work the audio and video hot paths were audited against
independent references and found already correct, so they were left
unchanged:

- **`apu.c`** — the S-DSP and SPC700. Verified against ares' independent
  cores, including exhaustive differential tests of ADC/SBC, ADDW/SUBW, the
  timers, and PSW packing.
- **`tile.c`** — the tile renderer. The SSE2 and NEON colour-math paths were
  verified bit-exact against the scalar reference, and byte-identical
  framebuffers were confirmed at runtime.

## License

This core inherits the Snes9x license; see `LICENSE.txt` (and
`docs/snes9x-license.txt`) for the full terms. Snes9x is intended for
non-commercial use and may not be sold or distributed for profit. The
included NTSC filter carries its own license (`filter/snes_ntsc-license.txt`).
