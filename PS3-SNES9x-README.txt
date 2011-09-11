================================================================================
			SNES9x PS3
			Version 4.5.0
================================================================================

This is an application for the PlayStation3 that makes it possible to play Super
Nintendo Entertainment System / Super Famicom games on your jailbroken
PlayStation3. It is based on an up-to-date version of the popular PC emulator,
SNES9x.

================================================================================
			Table of Contents
================================================================================
0. Requirements........................................................... [A00]
1. Currently Implemented SNES9x functions................................. [A01]
2. How to Load ROMs....................................................... [B01]
     2.1.1 - Example ROM (Maister Pong)................................... [B02]
3. Controls............................................................... [C01]
     3.1.1 - Controls in ROM menu......................................... [C02]
     3.1.2 - Controls in Settings menu.................................... [C03]
     3.1.3 - Controls in-game............................................. [C04]
     3.1.4 - Controls in-game (SNES joypad)............................... [C05]
     3.1.5 - Controls in-game (SNES Mouse)................................ [C06]
     3.1.6 - Controls in-game (Super Scope)............................... [C07]
     3.1.7 - Controls in-game (miscellaneous)............................. [C08]
     3.1.8 - Controls in-game (miscellaneous - default)................... [C09]
     3.1.9 - Controls in-game (miscellaneous - new)....................... [C10]
4. Settings............................................................... [D01]
     4.1 - General Settings............................................... [D02]
     4.2 - SNES9x Settings................................................ [D03]
     4.3 - Path Settings.................................................. [D04]
     4.4 - Controls Settings.............................................. [D05]
     4.5 - Controls Settings - Actions.................................... [D06]
5. Cheat code files....................................................... [E01]
6. IPS/UPS translation patches............................................ [F01]
7. Currently known issues (as of 4.5.0)................................... [G01]
     7.1 - Resolution switching - error prone on FW 1.92 build............ [G02]
     7.2 - Custom Soundtrack support - doesn't work on FW 1.92 build...... [G03]
8. Notes.................................................................. [H01]
     8.1 - General notes.................................................. [H02]
     8.2 - Performance notes.............................................. [H03]
     8.3 - Performance w/ pixel shaders................................... [H04]
     8.4 - Custom Soundtrack support...................................... [H05]
9. Planned improvements................................................... [I01]
10. For Developers........................................................ [J01]
     10.1 - Source Code Repository........................................ [J02]
          10.1.1 - Getting the latest SNES9x revision..................... [J03]
     10.2  - Compilation Instructions..................................... [J04]
          10.2.1 - GCC.................................................... [J05]
          10.2.2 - SNC.................................................... [J06]
	  10.2.3 - To make a debugging-friendly compile........................ [J07]
     10.3 - CellFramework................................................. [J08]
          10.3.1 - Getting the latest Cellframework revision.............. [J09]
11. Credits............................................................... [K01]
     

================================================================================
[A00]			0. Requirements
================================================================================
To play this on your PlayStation3 system, you have one of two options:

a) Jailbreak your PS3 using a USB exploit - this works up to firmware 3.41. 

or, 

b) You must update your firmware with a custom firmware. (required if you 
have firmware 3.55)

You must install the version of SNES9x PS3 that corresponds to the firmware
you're running. Go to 'Game' in the XMB, go to 'Install Packages', and select
the package you want to install.

================================================================================
			PS3 has firmware 1.92+ installed (w/ jailbreak dongle)
================================================================================ 
Install snes9x-ps3-v4.4.9-fw1.92.pkg'.

================================================================================
			PS3 has firmware 3.41 installed (w/ jailbreak dongle)
================================================================================
Install snes9x-ps3-v4.4.9-fw3.41.pkg'.

NOTE: Some CFWs out there might require that you install this PKG instead of the
3.55 CFW. If the CFW 3.55 PKG fails to install on your specific CFW flavor,
then try installing the 3.41 PKG instead.

================================================================================
			PS3 has firmware Geohot 3.55 CFW installed 
================================================================================
Install 'snes9x-ps3-v4.4.9-geohot-cfw3.55.pkg'.

================================================================================
[A01]			1. CURRENTLY IMPLEMENTED SNES9X FUNCTIONS
================================================================================
* Based on the latest version (SNES9x 1.52 WIP)
* Savestate loading/saving support
  - Savestate slot selectable in-game
  - Unlimited saveslots
  - Compression of savestates with gzip
* Supports IPS/UPS translation patches
* Saving/loading of SRAM
  - Added SRAM Write Protect feature
    - Can be toggled in-game - by setting it to 'On', loading of 
    savestates can't overwrite SRAM. Setting it to 'Off' will enable
    SRAM saving again.
* Controls
  - Two player support
  - Multitap support (up to five players)
  - Super Scope
      - Ability to use a USB/Bluetooth mouse
      - Mapped to the left analog stick)
  - Mouse
      - Ability to use a USB/Bluetooth mouse
      - Mapped to the left analog stick)
  - Ability to reconfigure each controller
* Audio
   - Supports stereo output
   - Supports RSound - networked audio client built-in
   - Supports USB/Bluetooth headsets - sound can be listened to via headset
   - Supports Custom soundtracks - user can select music from the XMB and
   play it while the emulator is running
* ROM support
  - SFC/FIG/SMC/GD3/SWC ROM support
  - Supports ZIP/JMA archives
* Supports cheatcode files (cht)
  - Ability to enter cheat codes in-game with onscreen keyboard
    - Ability to enter labels for cheatcodes with onscreen keyboard
* Supports movie files (.smv)
  - Ability to record, stop and play movies in-game
* Fastforwarding, Pause

================================================================================
[B01]			2. HOW TO LOAD ROMS 
================================================================================
ROMs can be selected from the ROM menu. Simply browse to a folder that contains
the ROMs, and press 'X' when highlighting the ROM.

The ROM must have one of the following extensions for it to work on SNES9x PS3 -

* SMC/smc
* FIG/fig
* SFC/sfc
* GD3/gd3
* GD7/gd7
* DX2/dx2
* BSX/bsx
* SWC/swc
* JMA/jma

ROMS stored in ZIP archives can also be loaded with SNES9x PS3.

SNES9x PS3 is only compatible with the ROM extensions listed above. Any other
ROM will not work on it.

================================================================================
[B02]			2. EXAMPLE ROM - MAISTER PONG
================================================================================
An example ROM has been included in the following directory:

/dev_hdd0/game/SNES90000/USRDIR/roms

pong.sfc is a SNES homebrew ROM - a clone of 'Pong' made from scratch by 
Themaister. The game logic is currently not hooked up - so you can move 
the paddle up or down - the ball will bounce off it - but scoring is not
currently done.

A playable version of this homebrew clone will be bundled with SNES9x PS3
in a later release.

================================================================================
[C01]			3. CONTROLS
================================================================================

================================================================================
[C02]			3.1.1 CONTROLS IN ROM MENU
================================================================================
Up				   - Go up
Down				- Go down
Left				- Go back five file entries
Right				- Go forward five file entries
L1				   - Go back one page
R1				   - Go forward one page

Cross				- (If directory selected) enter directory/ 
				     (if ROM selected) start ROM)
Triangle			- (If ROM selected) start ROM with multitap
				     support (*)
Circle		   - (If not in root directory) Go back to
				     previous directory
L2 + R2			- (If you previously exited a ROM) return to
				     game
Select			- Go to settings menu
				     (see 'CONTROLS IN SETTINGS MENU' section)

* - This option might be removed in the future but is still available as a
fallback in case the automatic multitap capability detection fails to work
on a game.

================================================================================
[C03]			3.1.2 CONTROLS IN SETTINGS MENU
================================================================================
Up				   - Go up one setting.
Down				- Go down one setting.
Left				- Change setting to the left.
Right				- Change setting to the right.

Circle			- Go back to ROM menu/Go back to previous
				     Settings screen
Start				- Reset the setting back to the default value.
R1				   - Go to the next Settings screen
L1				   - Go to the previous Settings screen

================================================================================
[C04]			3.1.3 CONTROLS IN-GAME
================================================================================

================================================================================
[C05]			3.1.4 CONTROLS IN-GAME - SNES JOYPAD
================================================================================
The standard button mappings for the SNES joypad are:

D-pad/Left analog stick		- SNES D-pad
Circle button			      - Button A
Cross button			      - Button B
Triangle button			   - Button X
Square button			      - Button Y
Select button			      - Select button
Start button			      - Start button
L1 button			         - L button
R1 button			         - R button

These controls can be reconfigured on the fly.

================================================================================
[C06]			3.1.5 CONTROLS IN-GAME - SNES MOUSE
================================================================================
The standard button mappings for the SNES mouse are:

Left analog stick		      - Mouse movement
Triangle button			   - Mouse R button
Square button			      - Mouse L button

================================================================================
[C07]			3.1.6 CONTROLS IN-GAME - SUPER SCOPE
================================================================================
The standard button mappings for the Super Scope are:

Left analog stick		      - Super Scope movement
Circle button			      - Scope Toggle Turbo
Triangle button			   - Scope Aim Off Screen
Square button			      - Scope Fire Button
Start button			      - Scope Pause button

================================================================================
[C08]			3.1.7 CONTROLS IN-GAME - MISCELLANEOUS
================================================================================
There are two control schemes to choose from - 'Default' and 'New'. Depending
on which control scheme you have selected in the Settings menu, the button
mappings in-game will be as described in Section [2.1.8] or Section [2.1.9].

TIP: You can use any USB controller you like and it will map the buttons
on the fly - it is even possible to hotswap joysticks - remove one from the
USB port, replace it with another one - it should all work fine. even
SNES-to-USB converters should work.

TIP: To play a game with a USB controller as Player 1, start up your PS3 and 
rather than using the Sixaxis/DualShock3, plug in an USB port before connecting 

NOTE: You can only use controller 1 for the UI. If your controller lacks
the necessary buttons to enable you to use the UI to the fullest extent possible
(such as - L2 and R2 buttons missing), then you can work around this by doing
the following - press and hold the PS button on your DualShock3/Sixaxis - select
'Controller Settings' and 'Reassign Controller' to Controller 1. Then do the UI
stuff that you want to do, and after returning back to the game, press and hold
the PS button again, select 'Controller Settings' and 'Reassign Controller' 
back to a different controller port.

================================================================================
[C09]			3.1.8 CONTROLS IN-GAME - MISCELLANEOUS - DEFAULT
================================================================================

R3 + L3			         	- [Exit to Menu] - go back to the ROM Browser
				                 menu.

R3 + R2				         - Save to currently selected save state slot.
R3 + L2				         - Load from currently selected save state slot.

Right analog stick - Left	- Decrement save state slot by one (-1)
Right analog stick - Right	- Increment save state slot by one (+1)
Right analog stick - Up		- Enable the currently selected cheat.
Right analog stick - Down	- Disable the currently selected cheat.

Right analog stick - Left + L2	- Move cheat position back by one (-1).
Right analog stick - Right + L2	- Move cheat position forwards by one (+1).

================================================================================
[B10]			3.1.9 CONTROLS IN-GAME - MISCELLANEOUS - NEW
================================================================================

R3 + L3				         - [Exit to Menu] - go back to the ROM Browser
				               menu.

Right analog stick - Left	- Decrement fastforward speed by one (-1).
Right analog stick - Right	- Increment fastforward speed by one (+1).
Right analog stick - Down	- Toggle [Fastforwarding] on/off.

Right analog stick - Left + L2	- Decrement cheat position by one (-1).
Right analog stick - Right + L2	- Increment cheat position by one (+1).
Right analog stick - Up	+ L2     - Toggle the currently selected cheat on/off.
Right analog stick - Down + L2	- Input a cheat with an on-screen keyboard.
Right analog stick - Down + L2 + R2 - Input a label for the cheat with an
                                 onscreen keyboard.

Right analog stick - Left + R2	- Decrement save state slot by one (-1).
Right analog stick - Right + R2	- Increment save state slot by one (+1).
Right analog stick - Up	+ R2	   - Load from currently selected save state slot. 
Right analog stick - Down + R2	- Save to currently selected save state slot.
R2 + R3				               - Toggle [SRAM Write Protection] on/off.

L2 + R3				               - Toggle [Record Movie] on/off.
L2 + L3				               - Load Movie

================================================================================
[D01]			4. SETTINGS
================================================================================

================================================================================
[D02]			4.1 GENERAL SETTINGS
================================================================================

================================================================================
			Current save state slot
================================================================================
Set the save state slot - this way, you can save multiple states and switch 
inbetween save states.

================================================================================
			Resolution
================================================================================
Switch between resolutions (depending on your TV/monitor's supported 
resolutions)

================================================================================
			Shader Presets
================================================================================
Seelct a shader preset - shader presets are configuration files that will
automatically configure a number of options to give you a certain graphical
look.

The following will be configured for you by selecting a shader preset:
* Shader 1
* Shader 2
* Hardware Filtering Shader #1
* Hardware Filtering Shader #2
* Custom Scaling Factor
* Overscan

NOTE: You can create your own shader presets by making plain-text files with
the same syntax as the pre-existing files in USRDIR/presets.

================================================================================
			Selected Shader #1
================================================================================
Select the first shader - we call this Shader #1.

The default shader is '/stock.cg'. Choose between 2XSaI, Bloom shader, curved
CRT shader, HQ2x, Super2xSaI, SuperEagle, and more.

================================================================================
			Selected Shader #1
================================================================================
Select the second shader for the back buffer - we call this Shader #2.

The default shader is '/stock.cg'. Choose between 2XSaI, Bloom shader, curved
CRT shader, HQ2x, Super2xSaI, SuperEagle, and more.

NOTE: This option will only work if 'Custom Scaling / Dual Shaders' is set to
'ON'.

================================================================================
			Font Size		
================================================================================
The font size in menus. Set this to a value higher than 1.000 to enlarge the
font size, or set it to a value lower than 1.000 to shrink the font size.

================================================================================
			Aspect Ratio			
================================================================================
Switch between aspect ratios 'Scaled' (recommended for 4:3 TVs/monitors) and
'Stretched' (recommended for 16:9 TVs/monitors).

================================================================================
			Hardware Filtering shader #1
================================================================================
Switch between Linear interpolation (Bilinear filtering) (DEFAULT) and 
Point filtering for Shader #1.

Linear interpolation - A hardware bilinear filter is applied to the image for
                       shader #1.

Point filtering	   - No filters are applied for shader #1. Most shaders look 
                     much better on this setting.

================================================================================
			Hardware Filtering shader #2
================================================================================
Switch between Linear interpolation (Bilinear filtering) (DEFAULT) and 
Point filtering for Shader #2.

Linear interpolation - A hardware bilinear filter is applied to the image for
                       shader #2.

Point filtering	   - No filters are applied for shader #2. Most shaders look 
                     much better on this setting.

NOTE: This option will only work if 'Custom Scaling / Dual Shaders' is set to
'ON'.

================================================================================
			Custom Scaling / Dual Shaders
================================================================================
Set this option to 'ON' if you want to be able to set a scaling factor for
the game image and select multiple shaders. It is set to 'ON' by default.

================================================================================
			Custom Scaling Factor
================================================================================
Set a custom scaling factor (1 to 5x) for the game image.

NOTE: This option will only work if 'Custom Scaling / Dual Shaders' is set to
'ON'.

================================================================================
			Overscan
================================================================================
Set this to a certain value so that no part of the screen is cutoff on your 
TV/monitor.

Which value to set varies depending on your monitor/TV.

NOTE: This option will only work if 'Custom Scaling / Dual Shaders' is set to
'ON'.

================================================================================
			Sound Output
================================================================================
Switch between 'Normal' mode, 'USB/Bluetooth Headset' mode, and 'RSound' mode.

Normal Mode - The normal audio output will be used by the PS3. (DEFAULT)

USB/Bluetooth Headset - Audio will be output through the USB/Bluetooth headset.

Rsound Mode - RSound basically lets you redirect the audio from the PS3 over the
network to a PC - so, using this, the audio from SNES9x can be outputted on a 
PC/laptop/netbook/HTPC's speakers/audio installation instead of going through 
the television speakers or the audio receiver connected to the television.

For more info, see - http://www.rsound.org

================================================================================
			Rsound Server IP Address
================================================================================
Set the IP address for the RSound server. This Will pop up an onscreen keyboard 
where you must input a valid IP address that points to the server that will be 
running an RSound server application.

================================================================================
			Throttle Mode
================================================================================
ON  - Regular mode - VSync is enabled and sound is enabled.

OFF - Performance testing mode - VSync is disabled and sound is disabled.
      This mode allows you to measure raw FPS (when Throttle Mode is set
      to ON, FPS is locked at 60fps). There seems to be a FPS cap of 125
      that appears to be active everywhere - including the menu.

================================================================================
			Triple Buffering
================================================================================
ON  - Triple Buffering is turned on - faster graphics and better shader
     performance at the expense of possibly increased control lag.

OFF - Triple Buffering turned off - default to Double Buffering. Slower
    graphics and worse shader performance with the benefit of possibly
    decreased control lag. 

================================================================================
			DEFAULT
================================================================================
Set all of the general settings back to their default values

================================================================================
[D03]			SNES9x SETTINGS
================================================================================

================================================================================
			Display Framerate
================================================================================
ON  - This will show an FPS (Frames Per Second) counter onscreen.

OFF - FPS counter is disabled. (DEFAULT)

NOTE: This option no longer works, and is not planned to work in the future.
If you want an FPS counter - compile SNES9x PS3 with CELL_DEBUG_FPS set to 1
in the Makefile.

================================================================================
			Sound Input Rate
================================================================================
Set the sound input rate of the SNES' audio processor. The default value (31942)
has been found to be ideal for A/V synchronization. Your mileage may vary - 
hence the inclusion of this option.

================================================================================
			Transparency effects
================================================================================
Enable or disable transparency effects.

ON  - Transparency effects are enabled. (DEFAULT)
OFF - Transparency effects are disabled. Certain transparency features used by 
the SNES in commercial games will not be displayed. (*)

================================================================================
			Frame skip
================================================================================
Set frame skipping - normally set to AUTO. (*)

NOTE: This feature currently does not work.

================================================================================
			Disable graphic windows
================================================================================
ON  - Graphic windows is enabled. (DEFAULT)
OFF - Graphic windows is disabled. Certain graphical features used by the SNES
in commercial games will be disabled.

================================================================================
			Display button input
================================================================================
ON  - Button input will be displayed on-screen - similar to fighting games in 
training mode that show you button input on-screen.
OFF - Button input will not be displayed on-screen. (DEFAULT)

================================================================================
			Force NTSC ROM to PAL
================================================================================
Set this to ON everytime you want to play a NTSC ROM at 50Hz (for PAL 576p). 
Beware though that certain games might have region protection so that you could 
not play an NTSC cart on an PAL machines. This is definitely not the way it was 
intended to run PAL ROMs - it is simply a hack, and it works well on most 
games. You have to set this back to ON for every game you try to load; it will 
automatically turn itself off after having loaded the ROM.

ON  - Try to run an NTSC ROM at 50Hz PAL.
OFF - Run NTSC ROMs normally. (DEFAULT)

================================================================================
			Force PAL ROM to NTSC
================================================================================
Set this to ON everytime you want to play a PAL ROM at 60Hz. Beware though that 
certain games had region protection so that you could not play a PAL cart on an 
NTSC machine - this started with Super Mario All-Stars. Other games will simply 
crash when you attempt this - so this is definitely not the way it was intended 
to run PAL ROMs - it is simply a hack, and it works well on most games. You 
have to set this back to ON for every game you try to load; it will 
automatically turn itself off after having loaded the ROM.

ON  - Try to run a PAL ROM at 60Hz NTSC.
OFF - Run PAL ROMs normally. (DEFAULT)

================================================================================
			PAL Timing
================================================================================
Change PAL timing - press Left to set it to 16667 (same as NTSC), and Right to 
set it to 2000 (PAL). 

20000 - Use PAL timing frequency for PAL. (DEFAULT)
16667 - Use NTSC timing frequency for PAL.

================================================================================
			Auto-apply Cheats
================================================================================
Automatically apply cheat files if they have the same name as the ROM file.

ON  - Cheat files will be automatically applied. (DEFAULT)
OFF - Cheat files will not be applied.

================================================================================
			Auto-apply IPS/UPS Patch
================================================================================
Automatically apply IPS/UPS patches if they have the same name as the ROM file
and are located in the same directory as the ROM.

ON  - IPS/UPS patches will be automatically applied. (DEFAULT)
OFF - IPS/UPS patches will not be applied.

NOTE: IPS/UPS patches are mostly referred to as translation patches that often
translate a Japanese game's text/dialogue into English. You can load either
pre-patched ROMs with SNES9x PS3, or you can use this option to automatically
apply IPS/UPS patches to a ROM. The latter approach doesn't require manual
editing of the ROM.

================================================================================
			Reset Before Recording Movie
================================================================================
Normally, when SNES9x is about to record a movie, it will first reset the ROM
before doing so.

ON  - SNES9x will reset the game before recording the movie.
OFF - SNES9x will record at the current position you're at in the game.

================================================================================
			Savestate SRAM Overwrite Protect
================================================================================
Normally, when SNES9x loads a savestate, the SRAM contents are automatically
overwritten. The ugly side-effect of this is that this can unknowingly
delete your normal saved game progress.

ON  - SNES9x will write-protect SRAM - this means that if you load a save-
      state, it will not overwrite the SRAM save(s). However, SRAM is not
      saved at all in this mode.
OFF - SRAM is writable - this means that if you load a savestate, it will
      overwrite the SRAM save(s).

NOTE: This option can be toggled in-game with the 'New' control scheme (see
Section 3.4 and beyond).

================================================================================
			Fast-forwarding - frameskip
================================================================================
Set here the amount of frames to skip while fast-forwarding. Setting this to a
value higher than 15 will translate to faster fast-forwarding.

================================================================================
			Accessory Autodetection	
================================================================================
SNES9x supports a number of SNES peripherals:
- Multitap (for multi-player gameplay - up to five players) 
- Super Scope (lightgun)
- SNES Mouse
- Justifier (lightgun)

SNES9x PS3 auto-detects most games that feature support for these peripherals.
This setting will control how SNES9x PS3 will deal with these games when it
detects them.

CONFIRM	  -  When detecting a Mouse/Scope/Multitap-compatible game, a dialog
	     screen is presented to the user asking him/her if the accessory 
	     should be enabled. If you select 'No', normal 2 joypad controls
             will be used instead.
ON        -  Games which support Mouse/Scope/Multitap will be automatically
             detected and the accessory will be automatically enabled without
             any input from the user.
OFF       -  Accessories will not be detected or enabled - standard 2 joypad
             controls for all games.

================================================================================
         Accessory Type
================================================================================

USB/Bluetooth Mouse - Mouse and Super Scope games can be controlled with a
                    USB/Bluetooth mouse that has been connected to the PS3.
                    Bluetooth mouses are configured using the PS3's XMB 
                    (Settings -> Accessory Settings). Only the first USB
                    mouse connected to the PS3 is used for gameplay.

Left analog stick   - Mouse and Super Scope games use the left analog stick
                    for directional controls.

================================================================================
			Default
================================================================================
Set all of the SNES9x settings back to their default values

================================================================================
[D04]			4.3 PATH SETTINGS
================================================================================

================================================================================
			Startup ROM Directory
================================================================================
Set the default ROM startup directory. You will have to restart the emulator
after changing the path for this change to have any effect.

DEFAULT - is set to the root of the PS3's filesystem. (/)

================================================================================
			Savestate Directory
================================================================================
Set the default savestate directory where your savestates will be saved and
loaded from.

DEFAULT - is set to the USRDIR directory of SNES9x PS3. 
(/dev_hdd0/game/SNES90000/USRDIR)

================================================================================
			SRAM Directory
================================================================================
Set the default SRAM (SaveRAM) directory where all your SRAM files will be 
saved and loaded from.

DEFAULT - Normally, the .SRM file of a game that supports SRAM is saved in the
same directory where the ROM is found.

================================================================================
			Cheatfile Directory
================================================================================
Set the default cheatfile directory - all your cheatfiles will be saved and 
loaded from here.

DEFAULT - is set to the USRDIR directory of SNES9x PS3. 
(/dev_hdd0/game/SNES90000/USRDIR)

================================================================================
			Default
================================================================================
Set all of the path settings back to their default values

================================================================================
[D05]			4.4 CONTROLS SETTINGS
================================================================================

================================================================================
			Control Scheme	
================================================================================
SNES9x PS3 features three control schemes:
- Default
- New
- Custom

As the name implies, the custom control scheme can be entirely reconfigured
to suit the user's own preferences. The other two (Default/New) are predefined
and therefore not customisable. 

================================================================================
			Controller No	
================================================================================
Select the controller that you want to configure. Range goes from Controller #1
to Controller #7. Every controller can be individually configured.

================================================================================
			Save Custom Controls
================================================================================
The current state of the 'Custom' control scheme will be saved to the
configuration file. Useful for when you want to switch back and forth between
'Custom' and one of the other control schemes while still wanting to retain
your current Custom control scheme instead of having to reconfigure it all 
over again.

NOTE: All the controls for every controller port will be saved with this
option.

================================================================================
			Default
================================================================================
Set all of the control settings back to their default values.

NOTE: This will only work if the [Control Scheme] selected is 'Custom'.

================================================================================
[D06]			4.5 CONTROLS SETTINGS - ACTIONS
================================================================================
When you set the [Control Scheme] to 'Custom', you have the option of 
reconfiguring all of the buttons/button combos. Here is a general description
of all the actions that you can map buttons to (the non-obvious ones will be 
covered):

================================================================================
			Save State
================================================================================
Saves a [save state] to the currently selected [save state slot].

================================================================================
			Load State
================================================================================
Loads a [save state] at the currently selected [save state slot].

================================================================================
			Decrement cheat position
================================================================================
Decrement [cheat position] by one and show the label of the cheat.

================================================================================
			Increment cheat position
================================================================================
Increment [cheat position] by one and show the label of the cheat.


================================================================================
			Exit to menu
================================================================================
Pause the game and return back to the ROM browser menu. Game can be resumed
from the ROM browser menu by pressing button combo [L2 + R2].

================================================================================
			Enable selected cheat
================================================================================
Enable the cheat at the currently selected cheat position.

NOTE: This action has been superseded by 'Toggle selected cheat'.

================================================================================
			Disabled selected cheat
================================================================================
Disable the cheat at the currently selected cheat position.

NOTE: This action has been superseded by 'Toggle selected cheat'.

================================================================================
			Toggle selected cheat
================================================================================
Turn cheat at currently selected cheat position on/off.

================================================================================
			Input cheat
================================================================================
Input a cheatcode with an onscreen keyboard (OSK).

================================================================================
			Decrement savestate slot
================================================================================
Decrement [save state] slot by one.

================================================================================
			Increment savestate slot
================================================================================
Increment [save state] slot by one.

================================================================================
			Fast-forward
================================================================================
Toggle [Fast-forwarding] mode on/off.

================================================================================
			Decrement Fast-forward speed
================================================================================
Decrease the amount of frames by which [Fast-forwarding] skips.

================================================================================
			Increment Fast-forward speed
================================================================================
Increase the amount of frames by which [Fast-forwarding] skips.

================================================================================
			Reset
================================================================================
Hard-reset the game. This is equivalent to flicking the 'On'/'Off' switch on 
the SNES console.

================================================================================
			Software Reset
================================================================================
Soft-reset the game. This is equivalent to pressing 'Reset' on the SNES
console.

================================================================================
			Begin Recording Movie
================================================================================
Some SNES games did not feature a Pause function (Mortal Kombat 1, for one).
SNES9x provides a built-in 'Pause' feature that is independent from the 
regular Pause functionality provided by SNES games.

================================================================================
			Begin Recording Movie
================================================================================
Begin the recording of the movie.

NOTE: This action has been superseded by 'Toggle Recording Movie'.

================================================================================
			End Recording Movie
================================================================================
End recording of movie.

NOTE: This action has been superseded by 'Toggle Recording Movie'.

================================================================================
			Toggle Recording Movie
================================================================================
The first time this action is triggered, recording of the movie will begin.
The second time it is triggered, the movie recording will stop. A ZMV file
will have been written to disk containing the movie code.

This movie file can now be played back with the 'Load Movie' action.

================================================================================
			Load Movie
================================================================================
A movie file that has been saved to disk will be played back from start to 
end.

================================================================================
			Swap Joypads
================================================================================
Joypad #1 will be placed in controller port 2 while Joypad #2 will be
placed in controller port 1, and vice versa.

================================================================================
			SRAM Write Protect
================================================================================
See 'Savestate SRAM Overwrite Protect' for more details.

================================================================================
[E01]			5. CHEAT CODE FILES
================================================================================
Cheat code files need to be have the same filename as the ROM you want to apply 
it to.

For instance:

Seiken Densetsu 3 (Japan).sfc

The cheatfile would have the following name:

Seiken Densetsu 3 (Japan).cht

Cheatcode files are saved either in the default directory (USRDIR), or at the 
directory that you have set as the 'Cheatfile directory' (see 'PATH SETTINGS')

NOTE: See '2.1.2 - CONTROLS IN-GAME' for an explanation of settings 
activation/deactivation and selecting ingame.

================================================================================
[F01]			6. IPS/UPS TRANSLATION PATCHES
================================================================================

Put a IPS or UPS translation patch with the same filename as the ROM you want 
it applied to in the same directory, and load up the ROM.

For instance,

Seiken Densetsu 3 (Japan).sfc

should have an UPS file in the same folder with the following name:

Seiken Densetsu 3 (Japan).ups

================================================================================
[G01]			7. CURRENTLY KNOWN ISSUES (AS OF BUILD 4.4.9)
================================================================================

================================================================================
[G02]			7.1 Resolution switching - error prone on FW 1.92 build
================================================================================
There are known problems with resolution switching on firmware 1.92. Be warned
that while it might work up until a certain couple of times, it is known to 
crash the emulator after changing resolutions too many times. When this 
happens, you will have to restart the emulator and load the jailbreak payload
again.

================================================================================
[G03]			5.2 Custom Soundtrack support - doesn't work on FW 1.92 build
================================================================================
This feature was not supported back then. Nothing that can be done about it.

================================================================================
[H01]			8. NOTES
================================================================================

================================================================================
[H02]			8.1 GENERAL NOTES
================================================================================
For people running this on HDTVs complaining about input lag:

Turn off all post-processing filters you may have running - on Sony Bravia 
HDTVs, display Motion Flow (this also causes input lags with most games in 
general, not just this SNES emu. If your HDTV has a 'Game' mode or something 
of the sort, select that as well.

================================================================================
[H03]			8.2 PERFORMANCE NOTES
================================================================================
All games will run at 60fps constant (NTSC) / 50fps constant (PAL) at any 
resolution, except for two notable exceptions:

* Star Fox 2
* Vortex (USA - NTSC)

There is a way to get both of these games to run at fullspeed though. In both 
cases this will reduce the framerate to 50fps, but the game performance 
overall will be much better with no slowdown and audio pops.

Performance issues with these two games will be looked into.

================================================================================
			8.2.1 Star Fox 2
================================================================================
Set 'Force NTSC ROM to PAL' to 'ON' in the Settings menu (SNES9x section), and
then load the ROM.

================================================================================
			8.2.2 Vortex
================================================================================
Load the PAL ROM instead of the NTSC ROM.


================================================================================
[H04]			8.3 PERFORMANCE NOTES - SHADERS
================================================================================
Nearly every shader will run at fullspeed at the highest resolution
setting - 1920x1080 - in either FBO mode or non-FBO mode.

Combinations of shaders might or might not run at fullspeed. The same goes for 
any type of shader combination and a given scaling factor - the higher the 
scaling factor, the more strain it puts on the RSX.


================================================================================
[H05]			8.4 CUSTOM SOUNDTRACK SUPPORT
================================================================================
It is possible to play custom soundtracks/music while simultaneously 
running the emulator.

Go to the XMB Music category tab, and select an album or playlist.

NOTE: If you select a song and it shows 'Content cannot be played back during
this game' - try to re-rip the album/song from USB to PS3 HDD. For some reason,
trying to rip/copy albums from UPNP servers will result in this message being
shown when trying to playback this content from within the emulator. MP3 albums
ripped from a USB stick/external USB HDD seem to work fine - other content might
also work.

Do note that this app is just making use of the system utilities of the PS3 -
the same limitations and rules apply.

================================================================================
[I01]			9. PLANNED IMPROVEMENTS
================================================================================
* Include keyboard support and map it by default to the following buttons:
- b,a,y,x,l (for face and trigger buttons)
- arrow keys for D-pad
- start = enter
- select = shift
* Migrate to libsnes9x (https://github.com/Themaister/snes9x-libsnes)
* Netplay
* A better frontend

================================================================================
[J01]			10. FOR DEVELOPERS
================================================================================

================================================================================
[J02]			11.1 SOURCE CODE REPOSITORY
================================================================================
Link : https://code.google.com/p/snes9x-ps3/

This release corresponds with revision = ?  

================================================================================
[J03]			11.1.1 GETTING LATEST SNES9X REVISION
================================================================================
To be able to check out the latest revision, you must have Mercurial installed 
on your system.

To check out the latest revision in the trunk, type in the following from the
command-line:

hg clone https://snes9x-ps3.googlecode.com/hg/ snes9x-ps3  

================================================================================
[J04]			11.2 COMPILATION INSTRUCTIONS
================================================================================
SNES9x PS3 can be compiled with either GCC or the SN C/C++ compiler.

================================================================================
[J05]       11.2.1 GCC
================================================================================
1 - Set CELL_BUILD_TOOLS in the Makefile like this:

CELL_BUILD_TOOLS = GCC

2 - Do 'make clean && make && make pkg'

================================================================================
[J06]       11.2.2 SNC
================================================================================
1 - Set CELL_BUILD_TOOLS in the Makefile like this:

CELL_BUILD_TOOLS = SNC

2 - Do 'make clean && make && make pkg'

================================================================================
[J07]			11.2.3 TO MAKE A DEBUGGING-FRIENDLY COMPILE
================================================================================
NOTE: This currently does not work with the current build.

We provide a netlogger in case you want to have some way of debugging 
SNES9x PS3 remotely.

1 - Edit the Makefile and set 'CELL_DEBUG_LOGGER to '1' - like this

CELL_DEBUG_LOGGER = 1

The IP address needs to be changed to the IP address of the host machine that 
will be running netcat - do a search for 192.168.1.7 and replace that with
the IP address of the host machine.

2 - Do 'make clean && make compile'.

3 - Install SNES9 PS3 on your PS3, start up netcat on your PC with the 
following command:

netcat -l -p 9002

4 - Start up SNES9x PS3. If all went well, you will see debugging messages on 
your PC with the netcat application.

================================================================================
[J08]			11.3 CELLFRAMEWORK
================================================================================
Link : https://code.google.com/p/cellframework/)

SNES9x is an implementation of 'Cellframework', which is written by the same 
developers as a rudimentary framework for PS3 app development. It provides 
more-or-less complete classes for graphics, input, audio, and network.

Cellframework is a subrepository of SNES9x. To update the subrepository, from 
the commandline, go to the folder and type 'hg up'.

================================================================================
[J09]			11.3.1 GETTING LATEST CELLFRAMEWORK REVISION
================================================================================
To be able to check out the latest revision, you must have Mercurial 
installed on your system. Type in the following:

hg clone https://cellframework.googlecode.com/hg/ cellframework

================================================================================
[K01]			12. CREDITS
================================================================================
Orioto	-	PIC1.PNG based on 'Milky Sky' 
		(http://orioto.deviantart.com/gallery/?6943610#/d1k3qg7)
