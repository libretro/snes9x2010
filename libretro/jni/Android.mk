LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

APP_DIR := ../../src

LOCAL_MODULE    := retro

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS += -DANDROID_ARM
endif

ifeq ($(TARGET_ARCH),x86)
LOCAL_CFLAGS +=  -DANDROID_X86
endif

ifeq ($(TARGET_ARCH),mips)
LOCAL_CFLAGS += -DANDROID_MIPS -D__mips__ -D__MIPSEL__
endif

LOCAL_SRC_FILES    += $(APP_DIR)/apu.c $(APP_DIR)/bsx.c $(APP_DIR)/c4emu.c $(APP_DIR)/cheats.c $(APP_DIR)/controls.c $(APP_DIR)/cpu.c $(APP_DIR)/cpuexec.c $(APP_DIR)/dsp.c $(APP_DIR)/fxemu.c $(APP_DIR)/globals.c $(APP_DIR)/memmap.c $(APP_DIR)/obc1.c $(APP_DIR)/ppu.c $(APP_DIR)/sa1.c $(APP_DIR)/sdd1.c $(APP_DIR)/seta.c $(APP_DIR)/snapshot.c $(APP_DIR)/spc7110.c $(APP_DIR)/srtc.c $(APP_DIR)/tile.c ../libretro.c ../memstream.c
LOCAL_CFLAGS += -O3 -std=gnu99 -ffast-math -funroll-loops -DINLINE=inline -DRIGHTSHIFT_IS_SAR -D__LIBRETRO__ -DFRONTEND_SUPPORTS_RGB565

include $(BUILD_SHARED_LIBRARY)
