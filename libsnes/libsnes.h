#ifndef LIBSNES_HPP
#define LIBSNES_HPP

#ifdef _MSC_VER
#include <stddef.h>
typedef unsigned char bool;
#endif

/* These constants represent the two controller ports on the front of the SNES,*/
/* for use with the snes_set_controller_port_device() function and the*/
/* snes_input_state_t callback.*/
#define SNES_PORT_1  0
#define SNES_PORT_2  1

/* These constants represent the different kinds of controllers that can be*/
/* connected to a controller port, for use with the*/
/* snes_set_controller_port_device() function and the snes_input_state_t*/
/* callback.*/
#define SNES_DEVICE_NONE         0
#define SNES_DEVICE_JOYPAD       1
#define SNES_DEVICE_MULTITAP     2
#define SNES_DEVICE_MOUSE        3
#define SNES_DEVICE_SUPER_SCOPE  4
#define SNES_DEVICE_JUSTIFIER    5
#define SNES_DEVICE_JUSTIFIERS   6

/* These constants represent the button and axis inputs on various controllers,*/
/* for use with the snes_input_state_t callback.*/
#define SNES_DEVICE_ID_JOYPAD_B        0
#define SNES_DEVICE_ID_JOYPAD_Y        1
#define SNES_DEVICE_ID_JOYPAD_SELECT   2
#define SNES_DEVICE_ID_JOYPAD_START    3
#define SNES_DEVICE_ID_JOYPAD_UP       4
#define SNES_DEVICE_ID_JOYPAD_DOWN     5
#define SNES_DEVICE_ID_JOYPAD_LEFT     6
#define SNES_DEVICE_ID_JOYPAD_RIGHT    7
#define SNES_DEVICE_ID_JOYPAD_A        8
#define SNES_DEVICE_ID_JOYPAD_X        9
#define SNES_DEVICE_ID_JOYPAD_L       10
#define SNES_DEVICE_ID_JOYPAD_R       11

#define SNES_DEVICE_ID_MOUSE_X      0
#define SNES_DEVICE_ID_MOUSE_Y      1
#define SNES_DEVICE_ID_MOUSE_LEFT   2
#define SNES_DEVICE_ID_MOUSE_RIGHT  3

#define SNES_DEVICE_ID_SUPER_SCOPE_X        0
#define SNES_DEVICE_ID_SUPER_SCOPE_Y        1
#define SNES_DEVICE_ID_SUPER_SCOPE_TRIGGER  2
#define SNES_DEVICE_ID_SUPER_SCOPE_CURSOR   3
#define SNES_DEVICE_ID_SUPER_SCOPE_TURBO    4
#define SNES_DEVICE_ID_SUPER_SCOPE_PAUSE    5

#define SNES_DEVICE_ID_JUSTIFIER_X        0
#define SNES_DEVICE_ID_JUSTIFIER_Y        1
#define SNES_DEVICE_ID_JUSTIFIER_TRIGGER  2
#define SNES_DEVICE_ID_JUSTIFIER_START    3

/* These constants will be returned by snes_get_region(), representing the*/
/* region of the last loaded cartridge.*/
#define SNES_REGION_NTSC  0
#define SNES_REGION_PAL   1

/* These constants represent the kinds of non-volatile memory a SNES cartridge*/
/* might have, for use with the snes_get_memory_* functions.*/
#define SNES_MEMORY_CARTRIDGE_RAM       0
#define SNES_MEMORY_CARTRIDGE_RTC       1
#define SNES_MEMORY_BSX_RAM             2
#define SNES_MEMORY_BSX_PRAM            3
#define SNES_MEMORY_SUFAMI_TURBO_A_RAM  4
#define SNES_MEMORY_SUFAMI_TURBO_B_RAM  5
#define SNES_MEMORY_GAME_BOY_RAM        6
#define SNES_MEMORY_GAME_BOY_RTC        7

/* These constants represent the various kinds of volatile storage the SNES*/
/* offers, to allow libsnes clients to implement things like cheat-searching*/
/* and certain kinds of debugging. They are for use with the snes_get_memory_**/
/* functions.*/
#define SNES_MEMORY_WRAM    100
#define SNES_MEMORY_APURAM  101
#define SNES_MEMORY_VRAM    102
#define SNES_MEMORY_OAM     103
#define SNES_MEMORY_CGRAM   104

/* SSNES extension. Not required to be implemented for a working implementation.*/
#define SNES_ENVIRONMENT_GET_FULLPATH 0	/* const char ** -- Full path of game loaded.*/
#define SNES_ENVIRONMENT_SET_GEOMETRY 1	/* const struct snes_geometry * -- Window geometry information for the system/game.*/
#define SNES_ENVIRONMENT_SET_PITCH 2	/* const unsigned * -- Pitch of game image.*/
#define SNES_ENVIRONMENT_GET_OVERSCAN 3	/* bool * -- Boolean value whether or not the implementation should use overscan.*/
#define SNES_ENVIRONMENT_SET_TIMING 4	/* const struct snes_system_timing * -- Set exact timings of the system.*/
					/* Used primarily for video recording.*/
#define SNES_ENVIRONMENT_GET_CAN_DUPE 5 // bool * --
                                                // Boolean value whether or not SSNES supports frame duping,
                                                // passing NULL to video frame callback.
                                                //
                                                //
#define SNES_ENVIRONMENT_SET_NEED_FULLPATH 6 // const bool * --
                                                // Boolean value telling if implementation needs a valid fullpath to be able to run.
                                                // If this is the case, SSNES will not open the rom directly,
                                                // and pass NULL to rom data.
                                                // Implementation must then use SNES_ENVIRONMENT_GET_FULLPATH.
                                                // This is useful for implementations with very large roms,
                                                // which are impractical to load fully into RAM.
#define SNES_ENVIRONMENT_GET_CAN_REWIND 7 // bool * --
                                                // Boolean value telling if SSNES is able to rewind.
                                                // Some implementations might need to take extra precautions
                                                // to allow this as smoothly as possible.
                                                //
#define SNES_ENVIRONMENT_GET_VARIABLE 8 // struct snes_variable * --
                                                // Interface to aquire user-defined information from environment
                                                // that cannot feasibly be supported in a multi-system way.
                                                // Mostly used for obscure,
                                                // specific features that the user can tap into when neseccary.
                                                //
#define SNES_ENVIRONMENT_SET_VARIABLES 9 // const struct snes_variable * --
                                                // Allows an implementation to signal the environment
                                                // which variables it might want to check for later using GET_VARIABLE.
                                                // 'data' points to an array of snes_variable structs terminated by a { NULL, NULL } element.
                                                // snes_variable::value should contain a human readable description of the key.
                                                //
#define SNES_ENVIRONMENT_SET_BATCH_LOAD 10 // const bool * --
                                                // If true, the implementation will load several roms in batch.
                                                // This means the rom must be provided exactly as is, i.e. it cannot be extracted.
                                                // If supported, this must be called directly when snes_set_environment() is called.
                                                // (Used on consoles).
                                                //
#define SNES_ENVIRONMENT_SET_ROM_FORMATS 11 // const char * --
                                                // Sets rom extensions the core generally supports.
                                                // If supported, this must be called directly when snes_set_environment() is called.
                                                // Formats are delimited with '|', i.e. "foo|bar|baz".
                                                // (Used on consoles).
                                                //
#define SNES_ENVIRONMENT_SET_MESSAGE 12 // const struct snes_message * --
                                                // Sets a message to be displayed in implementation-specific manner for a certain amount of 'frames'.

#define SNES_ENVIRONMENT_GET_AUDIO_BATCH_CB 13  // snes_audio_sample_batch_t * --
                                                // Retrieves callback to a more optimized audio callback.

struct snes_message
{
   const char *msg;
   unsigned frames;
};

struct snes_geometry
{
	unsigned base_width;	/* Nominal video width of system*/
	unsigned base_height;	/* Nominal video height of system.*/
	unsigned max_width;	/* Maximum possible width of system.*/
	unsigned max_height;	/* Maximum possible height of system.*/
};

struct snes_system_timing
{
	double fps;
	double sample_rate;
};

typedef bool (*snes_environment_t)(unsigned cmd, void *data);

/* Must be called before calling snes_init().*/
EXPORT void snes_set_environment(snes_environment_t);

typedef void (*snes_audio_sample_t)(uint16_t left, uint16_t right);

// Performance extension. Retrieved from environ callback.
typedef unsigned (*snes_audio_sample_batch_t)(const int16_t *data, unsigned frames);

typedef void (*snes_video_refresh_t)(const uint16_t *data, unsigned width,
        unsigned height);

typedef void (*snes_input_poll_t)(void);

typedef int16_t (*snes_input_state_t)(bool port, unsigned device,
        unsigned index, unsigned id);

EXPORT const char* snes_library_id(void);

EXPORT unsigned snes_library_revision_major(void);

EXPORT unsigned snes_library_revision_minor(void);

EXPORT void snes_init(void);

EXPORT void snes_term(void);

EXPORT void snes_set_video_refresh(snes_video_refresh_t);

EXPORT void snes_set_audio_sample(snes_audio_sample_t);

EXPORT void snes_set_input_poll(snes_input_poll_t);


EXPORT void snes_set_input_state(snes_input_state_t);

EXPORT void snes_set_controller_port_device(bool port, unsigned device);

EXPORT void snes_power(void);

EXPORT void snes_reset(void);

EXPORT void snes_run(void);

EXPORT bool snes_get_region(void);

EXPORT unsigned snes_serialize_size(void);

EXPORT bool snes_serialize(uint8_t *data, unsigned size);

EXPORT bool snes_unserialize(const uint8_t *data, unsigned size);

EXPORT void snes_cheat_reset(void);

EXPORT void snes_cheat_set(unsigned index, bool enabled, const char *code);

EXPORT bool snes_load_cartridge_normal(
  const char *rom_xml, const uint8_t *rom_data, unsigned rom_size
);

EXPORT bool snes_load_cartridge_bsx(
  const char *rom_xml, const uint8_t *rom_data, unsigned rom_size,
  const char *bsx_xml, const uint8_t *bsx_data, unsigned bsx_size
);

EXPORT bool snes_load_cartridge_bsx_slotted(
  const char *rom_xml, const uint8_t *rom_data, unsigned rom_size,
  const char *bsx_xml, const uint8_t *bsx_data, unsigned bsx_size
);

EXPORT bool snes_load_cartridge_sufami_turbo(
  const char *rom_xml, const uint8_t *rom_data, unsigned rom_size,
  const char *sta_xml, const uint8_t *sta_data, unsigned sta_size,
  const char *stb_xml, const uint8_t *stb_data, unsigned stb_size
);

EXPORT bool snes_load_cartridge_super_game_boy(
  const char *rom_xml, const uint8_t *rom_data, unsigned rom_size,
  const char *dmg_xml, const uint8_t *dmg_data, unsigned dmg_size
);

EXPORT void snes_set_cartridge_basename(const char *basename);

EXPORT void snes_unload_cartridge(void);

EXPORT uint8_t* snes_get_memory_data(unsigned id);

EXPORT unsigned snes_get_memory_size(unsigned id);

#endif
