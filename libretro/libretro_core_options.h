#ifndef LIBRETRO_CORE_OPTIONS_H__
#define LIBRETRO_CORE_OPTIONS_H__

#include <stdlib.h>
#include <string.h>

#include <libretro.h>
#include <retro_inline.h>

#ifndef HAVE_NO_LANGEXTRA
#include "libretro_core_options_intl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_ENGLISH */

/* Default language:
 * - All other languages must include the same keys and values
 * - Will be used as a fallback in the event that frontend language
 *   is not available
 * - Will be used as a fallback for any missing entries in
 *   frontend language definition */

struct retro_core_option_definition option_defs_us[] = {
	/* These variable names and possible values constitute an ABI with ZMZ (ZSNES Libretro player).
	 * Changing "Show layer 1" is fine, but don't change "layer_1"/etc or the possible values ("Yes|No").
	 * Adding more variables and rearranging them is safe. */
   {
      "snes9x_2010_overclock",
      "SuperFX Overclock",
      "Overclock or underclock the SuperFX chip. This may improve the framerate and playability of games that use SuperFX.",
      {
         { "5 MHz (Underclock)", NULL },
         { "8 MHz (Underclock)", NULL },
         { "9 MHz (Underclock)", NULL },
         { "10 MHz (Default)",  NULL },
         { "11 MHz", NULL },
         { "12 MHz", NULL },
         { "13 MHz", NULL },
         { "14 MHz", NULL },
         { "15 MHz", NULL },
         { "20 MHz", NULL },
         { "30 MHz", NULL },
         { "40 MHz", NULL },
         { NULL, NULL},
      },
      "10 MHz (Default)"
   },
   {
      "snes9x_2010_overclock_cycles",
      "Reduce Slowdown (Hack, Unsafe)",
      "Overclock SNES CPU. May cause games to crash! Use 'Light' for shorter loading times, 'Compatible' for most games exhibiting slowdown and 'Max' only if absolutely required (Gradius 3, Super R-type...).",
      {
         { "disabled",   NULL },
         { "light",      "Light" },
         { "compatible", "Compatible" },
         { "max",        "Max" },
         { NULL, NULL},
      },
      "disabled"
   },
   {
      "snes9x_2010_reduce_sprite_flicker",
      "Reduce Flickering (Hack, Unsafe)",
      "Increases number of sprites that can be drawn simultaneously on screen.",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL},
      },
      "disabled"
   },
   {
      "snes9x_2010_region",
      "Console Region (Restart)",
      "Specify which region the system is from. 'PAL' is 50hz, 'NTSC' is 60hz. Games will run faster or slower than normal if the incorrect region is selected.",
      {
         { "auto", "Auto" },
         { "ntsc", "NTSC" },
         { "pal",  "PAL" },
         { NULL, NULL},
      },
      "auto"
   },
   {
      "snes9x_2010_aspect",
      "Preferred Aspect Ratio",
      "Choose the preferred content aspect ratio. This will only apply when RetroArch's aspect ratio is set to 'Core provided' in the Video settings.",
      {
         { "4:3",         NULL },
         { "uncorrected", "Uncorrected" },
         { "auto",        "Auto" },
         { "ntsc",        "NTSC" },
         { "pal",         "PAL" },
         { NULL, NULL},
      },
      "auto"
   },
   {
      "snes9x_2010_turbodelay",
      "Set Autofire Pulse",
      "Fire interval: medium - 6 frames, fast - 4 frames, slow - 8 frames.",
      {
         { "medium", "Medium" },
         { "fast", "Fast" },
         { "slow", "Slow "},
         { NULL, NULL },
      },
      "medium"
   },
   {
      "snes9x_2010_blargg",
      "Blargg NTSC Filter",
      "Apply a video filter to mimic various NTSC TV signals.",
      {
         { "disabled",   NULL },
         { "monochrome", "Monochrome" },
         { "rf",         "RF" },
         { "composite",  "Composite" },
         { "s-video",    "S-Video" },
         { "rgb",        "RGB" },
         { NULL, NULL},
      },
      "disabled"
   },
   {
      "snes9x_2010_block_invalid_vram_access",
      "Block Invalid VRAM Access (Restart)",
      "Some homebrew/ROM hacks require this option to be disabled for correct operation.",
      {
         { "enabled",  NULL },
         { "disabled", NULL },
         { NULL, NULL},
      },
      "enabled"
   },

   { NULL, NULL, NULL, {{0}}, NULL },
};

/*
 ********************************
 * Language Mapping
 ********************************
*/

struct retro_core_option_definition *option_defs_intl[RETRO_LANGUAGE_LAST] = {
   option_defs_us, /* RETRO_LANGUAGE_ENGLISH */
   NULL,           /* RETRO_LANGUAGE_JAPANESE */
   NULL,           /* RETRO_LANGUAGE_FRENCH */
   NULL,           /* RETRO_LANGUAGE_SPANISH */
   NULL,           /* RETRO_LANGUAGE_GERMAN */
   NULL,           /* RETRO_LANGUAGE_ITALIAN */
   NULL,           /* RETRO_LANGUAGE_DUTCH */
   NULL,           /* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */
   NULL,           /* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */
   NULL,           /* RETRO_LANGUAGE_RUSSIAN */
   NULL,           /* RETRO_LANGUAGE_KOREAN */
   NULL,           /* RETRO_LANGUAGE_CHINESE_TRADITIONAL */
   NULL,           /* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
   NULL,           /* RETRO_LANGUAGE_ESPERANTO */
   NULL,           /* RETRO_LANGUAGE_POLISH */
   NULL,           /* RETRO_LANGUAGE_VIETNAMESE */
   NULL,           /* RETRO_LANGUAGE_ARABIC */
   NULL,           /* RETRO_LANGUAGE_GREEK */
#ifdef HAVE_NO_LANGEXTRA
   NULL,	   /* RETRO_LANGUAGE_TURKISH */
#else
   option_defs_tr, /* RETRO_LANGUAGE_TURKISH */
#endif
};

/*
 ********************************
 * Functions
 ********************************
*/

/* Handles configuration/setting of core options.
 * Should be called as early as possible - ideally inside
 * retro_set_environment(), and no later than retro_load_game()
 * > We place the function body in the header to avoid the
 *   necessity of adding more .c files (i.e. want this to
 *   be as painless as possible for core devs)
 */

static INLINE void libretro_set_core_options(retro_environment_t environ_cb)
{
   unsigned version = 0;

   if (!environ_cb)
      return;

   if (environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version) && (version >= 1))
   {
#ifndef HAVE_NO_LANGEXTRA
      struct retro_core_options_intl core_options_intl;
      unsigned language = 0;

      core_options_intl.us    = option_defs_us;
      core_options_intl.local = NULL;

      if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
          (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH))
         core_options_intl.local = option_defs_intl[language];

      environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL, &core_options_intl);
#else
      environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, &option_defs_us);
#endif
   }
   else
   {
      size_t i;
      size_t option_index              = 0;
      size_t num_options               = 0;
      struct retro_variable *variables = NULL;
      char **values_buf                = NULL;

      /* Determine number of options
       * > Note: We are going to skip a number of irrelevant
       *   core options when building the retro_variable array,
       *   but we'll allocate space for all of them. The difference
       *   in resource usage is negligible, and this allows us to
       *   keep the code 'cleaner' */
      while (true)
      {
         if (option_defs_us[num_options].key)
            num_options++;
         else
            break;
      }

      /* Allocate arrays */
      variables  = (struct retro_variable *)calloc(num_options + 1, sizeof(struct retro_variable));
      values_buf = (char **)calloc(num_options, sizeof(char *));

      if (!variables || !values_buf)
         goto error;

      /* Copy parameters from option_defs_us array */
      for (i = 0; i < num_options; i++)
      {
         const char *key                        = option_defs_us[i].key;
         const char *desc                       = option_defs_us[i].desc;
         const char *default_value              = option_defs_us[i].default_value;
         struct retro_core_option_value *values = option_defs_us[i].values;
         size_t buf_len                         = 3;
         size_t default_index                   = 0;

         values_buf[i] = NULL;

         if (desc)
         {
            size_t num_values = 0;

            /* Determine number of values */
            while (true)
            {
               if (values[num_values].value)
               {
                  /* Check if this is the default value */
                  if (default_value)
                     if (strcmp(values[num_values].value, default_value) == 0)
                        default_index = num_values;

                  buf_len += strlen(values[num_values].value);
                  num_values++;
               }
               else
                  break;
            }

            /* Build values string */
            if (num_values > 0)
            {
               size_t j;

               buf_len += num_values - 1;
               buf_len += strlen(desc);

               values_buf[i] = (char *)calloc(buf_len, sizeof(char));
               if (!values_buf[i])
                  goto error;

               strcpy(values_buf[i], desc);
               strcat(values_buf[i], "; ");

               /* Default value goes first */
               strcat(values_buf[i], values[default_index].value);

               /* Add remaining values */
               for (j = 0; j < num_values; j++)
               {
                  if (j != default_index)
                  {
                     strcat(values_buf[i], "|");
                     strcat(values_buf[i], values[j].value);
                  }
               }
            }
         }

         variables[option_index].key   = key;
         variables[option_index].value = values_buf[i];
         option_index++;
      }

      /* Set variables */
      environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);

error:

      /* Clean up */
      if (values_buf)
      {
         for (i = 0; i < num_options; i++)
         {
            if (values_buf[i])
            {
               free(values_buf[i]);
               values_buf[i] = NULL;
            }
         }

         free(values_buf);
         values_buf = NULL;
      }

      if (variables)
      {
         free(variables);
         variables = NULL;
      }
   }
}

#ifdef __cplusplus
}
#endif

#endif
