/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  (c) Copyright 1996 - 2002  Gary Henderson (gary.henderson@ntlworld.com),
                             Jerremy Koot (jkoot@snes9x.com)

  (c) Copyright 2002 - 2004  Matthew Kendora

  (c) Copyright 2002 - 2005  Peter Bortas (peter@bortas.org)

  (c) Copyright 2004 - 2005  Joel Yliluoma (http://iki.fi/bisqwit/)

  (c) Copyright 2001 - 2006  John Weidman (jweidman@slip.net)

  (c) Copyright 2002 - 2006  funkyass (funkyass@spam.shaw.ca),
                             Kris Bleakley (codeviolation@hotmail.com)

  (c) Copyright 2002 - 2010  Brad Jorsch (anomie@users.sourceforge.net),
                             Nach (n-a-c-h@users.sourceforge.net),
                             zones (kasumitokoduck@yahoo.com)

  (c) Copyright 2006 - 2007  nitsuja

  (c) Copyright 2009 - 2010  BearOso,
                             OV2


  BS-X C emulator code
  (c) Copyright 2005 - 2006  Dreamer Nom,
                             zones

  C4 x86 assembler and some C emulation code
  (c) Copyright 2000 - 2003  _Demo_ (_demo_@zsnes.com),
                             Nach,
                             zsKnight (zsknight@zsnes.com)

  C4 C++ code
  (c) Copyright 2003 - 2006  Brad Jorsch,
                             Nach

  DSP-1 emulator code
  (c) Copyright 1998 - 2006  _Demo_,
                             Andreas Naive (andreasnaive@gmail.com),
                             Gary Henderson,
                             Ivar (ivar@snes9x.com),
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora,
                             Nach,
                             neviksti (neviksti@hotmail.com)

  DSP-2 emulator code
  (c) Copyright 2003         John Weidman,
                             Kris Bleakley,
                             Lord Nightmare (lord_nightmare@users.sourceforge.net),
                             Matthew Kendora,
                             neviksti

  DSP-3 emulator code
  (c) Copyright 2003 - 2006  John Weidman,
                             Kris Bleakley,
                             Lancer,
                             z80 gaiden

  DSP-4 emulator code
  (c) Copyright 2004 - 2006  Dreamer Nom,
                             John Weidman,
                             Kris Bleakley,
                             Nach,
                             z80 gaiden

  OBC1 emulator code
  (c) Copyright 2001 - 2004  zsKnight,
                             pagefault (pagefault@zsnes.com),
                             Kris Bleakley
                             Ported from x86 assembler to C by sanmaiwashi

  SPC7110 and RTC C++ emulator code used in 1.39-1.51
  (c) Copyright 2002         Matthew Kendora with research by
                             zsKnight,
                             John Weidman,
                             Dark Force

  SPC7110 and RTC C++ emulator code used in 1.52+
  (c) Copyright 2009         byuu,
                             neviksti

  S-DD1 C emulator code
  (c) Copyright 2003         Brad Jorsch with research by
                             Andreas Naive,
                             John Weidman

  S-RTC C emulator code
  (c) Copyright 2001 - 2006  byuu,
                             John Weidman

  ST010 C++ emulator code
  (c) Copyright 2003         Feather,
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora

  Super FX x86 assembler emulator code
  (c) Copyright 1998 - 2003  _Demo_,
                             pagefault,
                             zsKnight

  Super FX C emulator code
  (c) Copyright 1997 - 1999  Ivar,
                             Gary Henderson,
                             John Weidman

  Sound emulator code used in 1.5-1.51
  (c) Copyright 1998 - 2003  Brad Martin
  (c) Copyright 1998 - 2006  Charles Bilyue'

  Sound emulator code used in 1.52+
  (c) Copyright 2004 - 2007  Shay Green (gblargg@gmail.com)

  SH assembler code partly based on x86 assembler code
  (c) Copyright 2002 - 2004  Marcus Comstedt (marcus@mc.pp.se)

  2xSaI filter
  (c) Copyright 1999 - 2001  Derek Liauw Kie Fa

  HQ2x, HQ3x, HQ4x filters
  (c) Copyright 2003         Maxim Stepin (maxim@hiend3d.com)

  NTSC filter
  (c) Copyright 2006 - 2007  Shay Green

  GTK+ GUI code
  (c) Copyright 2004 - 2010  BearOso

  Win32 GUI code
  (c) Copyright 2003 - 2006  blip,
                             funkyass,
                             Matthew Kendora,
                             Nach,
                             nitsuja
  (c) Copyright 2009 - 2010  OV2

  Mac OS GUI code
  (c) Copyright 1998 - 2001  John Stiles
  (c) Copyright 2001 - 2010  zones


  Specific ports contains the works of other authors. See headers in
  individual files.


  Snes9x homepage: http://www.snes9x.com/

  Permission to use, copy, modify and/or distribute Snes9x in both binary
  and source form, for non-commercial purposes, is hereby granted without
  fee, providing that this license information and copyright notice appear
  with all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software or it's derivatives.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes,
  but is not limited to, charging money for Snes9x or software derived from
  Snes9x, including Snes9x or derivatives in commercial game bundles, and/or
  using Snes9x as a promotion for your commercial product.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
 ***********************************************************************************/


#include <ctype.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "snes9x.h"
#include "memmap.h"
#include "controls.h"
#include "crosshairs.h"
#include "cheats.h"
#include "display.h"
#ifdef NETPLAY_SUPPORT
#include "netplay.h"
#endif

//BEGINNING OF SNES9x PS3
#include <stdlib.h>
//END OF SNES9x PS3

#ifdef DEBUGGER
extern FILE	*trace;
#endif

#define S9X_CONF_FILE_NAME	"snes9x.conf"

static char	*rom_filename = NULL;

static bool parse_controller_spec (int, const char *);
static void parse_crosshair_spec (enum crosscontrols, const char *);
//static bool try_load_config_file (const char *, ConfigFile &);


static bool parse_controller_spec (int port, const char *arg)
{
	if (!strcasecmp(arg, "none"))
		S9xSetController(port, CTL_NONE,       0, 0, 0, 0);
	else
		if (!strncasecmp(arg, "pad",   3) && arg[3] >= '1' && arg[3] <= '8' && arg[4] == '\0')
			S9xSetController(port, CTL_JOYPAD, arg[3] - '1', 0, 0, 0);
		else
			if (!strncasecmp(arg, "mouse", 5) && arg[5] >= '1' && arg[5] <= '2' && arg[6] == '\0')
				S9xSetController(port, CTL_MOUSE,  arg[5] - '1', 0, 0, 0);
			else
				if (!strcasecmp(arg, "superscope"))
					S9xSetController(port, CTL_SUPERSCOPE, 0, 0, 0, 0);
				else
					if (!strcasecmp(arg, "justifier"))
						S9xSetController(port, CTL_JUSTIFIER,  0, 0, 0, 0);
					else
						if (!strcasecmp(arg, "two-justifiers"))
							S9xSetController(port, CTL_JUSTIFIER,  1, 0, 0, 0);
						else
							if (!strncasecmp(arg, "mp5:", 4) && ((arg[4] >= '1' && arg[4] <= '8') || arg[4] == 'n') &&
									((arg[5] >= '1' && arg[5] <= '8') || arg[5] == 'n') &&
									((arg[6] >= '1' && arg[6] <= '8') || arg[6] == 'n') &&
									((arg[7] >= '1' && arg[7] <= '8') || arg[7] == 'n') && arg[8] == '\0')
								S9xSetController(port, CTL_MP5, (arg[4] == 'n') ? -1 : arg[4] - '1',
										(arg[5] == 'n') ? -1 : arg[5] - '1',
										(arg[6] == 'n') ? -1 : arg[6] - '1',
										(arg[7] == 'n') ? -1 : arg[7] - '1');
							else
								return (false);

	return (true);
}

static void parse_crosshair_spec (enum crosscontrols ctl, const char *spec)
{
	int			idx = -1, i;
	const char	*fg = NULL, *bg = NULL, *s = spec;

	if (s[0] == '"')
	{
		s++;
		for (i = 0; s[i] != '\0'; i++)
			if (s[i] == '"' && s[i - 1] != '\\')
				break;

		idx = 31 - ctl;

		std::string	fname(s, i);
		if (!S9xLoadCrosshairFile(idx, fname.c_str()))
			return;

		s += i + 1;
	}
	else
	{
		if (isdigit(*s))
		{
			idx = *s - '0';
			s++;
		}

		if (isdigit(*s))
		{
			idx = idx * 10 + *s - '0';
			s++;
		}

		if (idx > 31)
		{
			fprintf(stderr, "Invalid crosshair spec '%s'.\n", spec);
			return;
		}
	}

	while (*s != '\0' && isspace(*s))
		s++;

	if (*s != '\0')
	{
		fg = s;

		while (isalnum(*s))
			s++;

		if (*s != '/' || !isalnum(s[1]))
		{
			fprintf(stderr, "Invalid crosshair spec '%s.'\n", spec);
			return;
		}

		bg = ++s;

		while (isalnum(*s))
			s++;

		if (*s != '\0')
		{
			fprintf(stderr, "Invalid crosshair spec '%s'.\n", spec);
			return;
		}
	}

	S9xSetControllerCrosshair(ctl, idx, fg, bg);
}

void S9xUsage (void)
{
	/*                               12345678901234567890123456789012345678901234567890123456789012345678901234567890 */

	S9xMessage(S9X_INFO, S9X_USAGE, "");
	S9xMessage(S9X_INFO, S9X_USAGE, "Snes9x " VERSION);
	S9xMessage(S9X_INFO, S9X_USAGE, "");
	S9xMessage(S9X_INFO, S9X_USAGE, "usage: snes9x [options] <ROM image filename>");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

	// SOUND OPTIONS
	S9xMessage(S9X_INFO, S9X_USAGE, "-soundsync                      Synchronize sound as far as possible");
	S9xMessage(S9X_INFO, S9X_USAGE, "-playbackrate <Hz>              Set sound playback rate");
	S9xMessage(S9X_INFO, S9X_USAGE, "-inputrate <Hz>                 Set sound input rate");
	S9xMessage(S9X_INFO, S9X_USAGE, "-reversestereo                  Reverse stereo sound output");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nostereo                       Disable stereo sound output");
	S9xMessage(S9X_INFO, S9X_USAGE, "-eightbit                       Use 8bit sound instead of 16bit");
	S9xMessage(S9X_INFO, S9X_USAGE, "-mute                           Mute sound");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

	// DISPLAY OPTIONS
	S9xMessage(S9X_INFO, S9X_USAGE, "-displayframerate               Display the frame rate counter");
	S9xMessage(S9X_INFO, S9X_USAGE, "-displaykeypress                Display input of all controllers and peripherals");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nohires                        (Not recommended) Disable support for hi-res and");
	S9xMessage(S9X_INFO, S9X_USAGE, "                                interlace modes");
	S9xMessage(S9X_INFO, S9X_USAGE, "-notransparency                 (Not recommended) Disable transparency effects");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nowindows                      (Not recommended) Disable graphic window effects");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

	// CONTROLLER OPTIONS
	S9xMessage(S9X_INFO, S9X_USAGE, "-nomp5                          Disable emulation of the Multiplayer 5 adapter");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nomouse                        Disable emulation of the SNES mouse");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nosuperscope                   Disable emulation of the Superscope");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nojustifier                    Disable emulation of the Konami Justifier");
	S9xMessage(S9X_INFO, S9X_USAGE, "-port# <control>                Specify which controller to emulate in port 1/2");
	S9xMessage(S9X_INFO, S9X_USAGE, "    Controllers: none              No controller");
	S9xMessage(S9X_INFO, S9X_USAGE, "                 pad#              Joypad number 1-8");
	S9xMessage(S9X_INFO, S9X_USAGE, "                 mouse#            Mouse number 1-2");
	S9xMessage(S9X_INFO, S9X_USAGE, "                 superscope        Superscope (not useful with -port1)");
	S9xMessage(S9X_INFO, S9X_USAGE, "                 justifier         Blue Justifier (not useful with -port1)");
	S9xMessage(S9X_INFO, S9X_USAGE, "                 two-justifiers    Blue & Pink Justifiers");
	S9xMessage(S9X_INFO, S9X_USAGE, "                 mp5:####          MP5 with the 4 named pads (1-8 or n)");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

	// ROM OPTIONS
	S9xMessage(S9X_INFO, S9X_USAGE, "-hirom                          Force Hi-ROM memory map");
	S9xMessage(S9X_INFO, S9X_USAGE, "-lorom                          Force Lo-ROM memory map");
	S9xMessage(S9X_INFO, S9X_USAGE, "-ntsc                           Force NTSC timing (60 frames/sec)");
	S9xMessage(S9X_INFO, S9X_USAGE, "-pal                            Force PAL timing (50 frames/sec)");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nointerleave                   Assume the ROM image is not in interleaved");
	S9xMessage(S9X_INFO, S9X_USAGE, "                                format");
	S9xMessage(S9X_INFO, S9X_USAGE, "-interleaved                    Assume the ROM image is in interleaved format");
	S9xMessage(S9X_INFO, S9X_USAGE, "-interleaved2                   Assume the ROM image is in interleaved 2 format");
	S9xMessage(S9X_INFO, S9X_USAGE, "-interleavedgd24                Assume the ROM image is in interleaved gd24");
	S9xMessage(S9X_INFO, S9X_USAGE, "                                format");
	S9xMessage(S9X_INFO, S9X_USAGE, "-noheader                       Assume the ROM image doesn't have a header of a");
	S9xMessage(S9X_INFO, S9X_USAGE, "                                copier");
	S9xMessage(S9X_INFO, S9X_USAGE, "-header                         Assume the ROM image has a header of a copier");
	S9xMessage(S9X_INFO, S9X_USAGE, "-bsxbootup                      Boot up BS games from BS-X");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

	// PATCH/CHEAT OPTIONS
	S9xMessage(S9X_INFO, S9X_USAGE, "-nopatch                        Do not apply any available IPS/UPS patches");
	S9xMessage(S9X_INFO, S9X_USAGE, "-cheat                          Apply saved cheats");
	S9xMessage(S9X_INFO, S9X_USAGE, "-gamegenie <code>               Supply a Game Genie code");
	S9xMessage(S9X_INFO, S9X_USAGE, "-actionreplay <code>            Supply a Pro-Action Reply code");
	S9xMessage(S9X_INFO, S9X_USAGE, "-goldfinger <code>              Supply a Gold Finger code");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

#ifdef NETPLAY_SUPPORT
	// NETPLAY OPTIONS
	S9xMessage(S9X_INFO, S9X_USAGE, "-net                            Enable netplay");
	S9xMessage(S9X_INFO, S9X_USAGE, "-port <num>                     Use port <num> for netplay (use with -net)");
	S9xMessage(S9X_INFO, S9X_USAGE, "-server <string>                Use the specified server for netplay");
	S9xMessage(S9X_INFO, S9X_USAGE, "                                (use with -net)");
	S9xMessage(S9X_INFO, S9X_USAGE, "");
#endif

	// HACKING OR DEBUGGING OPTIONS
#ifdef DEBUGGER
	S9xMessage(S9X_INFO, S9X_USAGE, "-debug                          Set the Debugger flag");
	S9xMessage(S9X_INFO, S9X_USAGE, "-trace                          Begin CPU instruction tracing");
#endif
	S9xMessage(S9X_INFO, S9X_USAGE, "-noirq                          (Not recommended) Disable IRQ emulation");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nohdma                         (Not recommended) Disable HDMA emulation");
	S9xMessage(S9X_INFO, S9X_USAGE, "-hdmatiming <1-199>             (Not recommended) Changes HDMA transfer timings");
	S9xMessage(S9X_INFO, S9X_USAGE, "-cpushutdown                    (Not recommended) Skip emulation until the next");
	S9xMessage(S9X_INFO, S9X_USAGE, "                                event comes");
	S9xMessage(S9X_INFO, S9X_USAGE, "-invalidvramaccess              (Not recommended) Allow invalid VRAM access");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

	// OTHER OPTIONS
	S9xMessage(S9X_INFO, S9X_USAGE, "-frameskip <num>                Screen update frame skip rate");
	S9xMessage(S9X_INFO, S9X_USAGE, "-frametime <num>                Milliseconds per frame for frameskip auto-adjust");
	S9xMessage(S9X_INFO, S9X_USAGE, "-upanddown                      Override protection from pressing left+right or");
	S9xMessage(S9X_INFO, S9X_USAGE, "                                up+down together");
	S9xMessage(S9X_INFO, S9X_USAGE, "-conf <filename>                Use specified conf file (after standard files)");
	S9xMessage(S9X_INFO, S9X_USAGE, "-nostdconf                      Do not load the standard config files");
	S9xMessage(S9X_INFO, S9X_USAGE, "");

	//S9xExtraUsage();

	S9xMessage(S9X_INFO, S9X_USAGE, "");
	S9xMessage(S9X_INFO, S9X_USAGE, "ROM image can be compressed with zip, gzip, JMA, or compress.");

	exit(1);
}
