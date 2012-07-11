LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libretro
LOCAL_SRC_FILES    = ../src/apu.c ../src/bsx.c ../src/c4emu.c ../src/cheats.c ../src/controls.c ../src/cpu.c ../src/cpuexec.c ../src/dsp.c ../src/fxemu.c ../src/globals.c ../src/memmap.c ../src/obc1.c ../src/ppu.c ../src/sa1.c ../src/sdd1.c ../src/seta.c ../src/snapshot.c ../src/spc7110.c ../src/srtc.c ../src/tile.c ../libretro/libretro.c ../libretro/memstream.c
LOCAL_CFLAGS = -DINLINE=inline -DRIGHTSHIFT_IS_SAR -DLSB_FIRST -D__LIBRETRO__

include $(BUILD_SHARED_LIBRARY)
