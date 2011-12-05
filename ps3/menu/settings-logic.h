#define toggle_settings_items() \
	for(int i = 0; i < MAX_NO_OF_CONTROLS_SETTINGS; i++) \
			items_generalsettings[i].enabled = 1;	 \
	\
	menu_generalvideosettings.items[SETTING_SHADER_2].enabled = Settings.ScaleEnabled; \
	menu_generalvideosettings.items[SETTING_SHADER_PRESETS].enabled = Settings.ScaleEnabled; \
	menu_generalvideosettings.items[SETTING_HW_TEXTURE_FILTER_2].enabled = Settings.ScaleEnabled; \
	menu_generalvideosettings.items[SETTING_SCALE_FACTOR].enabled = Settings.ScaleEnabled; \
	menu_generalvideosettings.items[SETTING_GAME_AWARE_SHADER].enabled = Settings.ScaleEnabled; \
	update_item_colors = false;

static void apply_scaling(void)
{
	ps3graphics_set_fbo_scale(Settings.ScaleEnabled, Settings.ScaleFactor);
	ps3graphics_set_smooth(Settings.PS3Smooth, 0);
	ps3graphics_set_smooth(Settings.PS3Smooth2, 1);
}

static void producesettingentry(uint64_t switchvalue)
{
	uint64_t state = cell_pad_input_poll_device(0);
	//static uint64_t old_state = 0;
	//uint64_t diff_state = old_state ^ state;

	switch(switchvalue)
	{
		case SETTING_EMU_CURRENT_SAVE_STATE_SLOT:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_CROSS(state))
			{
				if(Settings.CurrentSaveStateSlot != 0)
					Settings.CurrentSaveStateSlot--;

				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_RIGHT(state)  || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				Settings.CurrentSaveStateSlot++;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}

			if(CTRL_START(state))
				Settings.CurrentSaveStateSlot = 0;

			break;
		case SETTING_CHANGE_RESOLUTION:
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) )
			{
				ps3graphics_next_resolution();
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) )
			{
				ps3graphics_previous_resolution();
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_CROSS(state))
			{
				if (ps3graphics_get_current_resolution() == CELL_VIDEO_OUT_RESOLUTION_576)
				{
					if(ps3graphics_check_resolution(CELL_VIDEO_OUT_RESOLUTION_576))
					{
						ps3graphics_set_pal60hz(Settings.PS3PALTemporalMode60Hz);
						ps3graphics_switch_resolution(ps3graphics_get_current_resolution(), Settings.PS3PALTemporalMode60Hz, Settings.TripleBuffering, Settings.ScaleEnabled, Settings.ScaleFactor);
						ps3graphics_set_vsync(Settings.Throttled);
						apply_scaling();
					}
				}
				else
				{
					ps3graphics_set_pal60hz(0);
					ps3graphics_switch_resolution(ps3graphics_get_current_resolution(), 0, Settings.TripleBuffering, Settings.ScaleEnabled, Settings.ScaleFactor);
					ps3graphics_set_vsync(Settings.Throttled);
					apply_scaling();
					emulator_implementation_set_texture(Settings.PS3CurrentBorder);
				}
			}
			break;
			/*
			   case SETTING_PAL60_MODE:
			   if(CTRL_RIGHT(state) || CTRL_LSTICK_LEFT(state) || CTRL_CROSS(state) || CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state))
			   {
			   if (Graphics->GetCurrentResolution() == CELL_VIDEO_OUT_RESOLUTION_576)
			   {
			   if(Graphics->CheckResolution(CELL_VIDEO_OUT_RESOLUTION_576))
			   {
			   Settings.PS3PALTemporalMode60Hz = !Settings.PS3PALTemporalMode60Hz;
			   Graphics->SetPAL60Hz(Settings.PS3PALTemporalMode60Hz);
			   Graphics->SwitchResolution(Graphics->GetCurrentResolution(), Settings.PS3PALTemporalMode60Hz, Settings.TripleBuffering);
			   }
			   }

			   }
			   break;
			 */
		case SETTING_GAME_AWARE_SHADER:
			if((CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_CROSS(state)) && Settings.ScaleEnabled)
			{
				menuStackindex++;
				menuStack[menuStackindex] = menu_filebrowser;
				menuStack[menuStackindex].enum_id = GAME_AWARE_SHADER_CHOICE;
				set_initial_dir_tmpbrowser = true;
			}
			if(CTRL_START(state) && Settings.ScaleEnabled)
			{
				strcpy(Settings.GameAwareShaderPath,"");
				strcpy(Settings.PS3CurrentShader, DEFAULT_SHADER_FILE);
				strcpy(Settings.PS3CurrentShader2, DEFAULT_SHADER_FILE);
			}
			break;
		case SETTING_SHADER_PRESETS:
			if(Settings.ScaleEnabled)
			{
				if((CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_CROSS(state)) && Settings.ScaleEnabled)
				{
					menuStackindex++;
					menuStack[menuStackindex] = menu_filebrowser;
					menuStack[menuStackindex].enum_id = PRESET_CHOICE;
					set_initial_dir_tmpbrowser = true;
				}
			}
			if(CTRL_START(state) && Settings.ScaleEnabled)
			{
				strcpy(Settings.ShaderPresetPath, "");
				strcpy(Settings.ShaderPresetTitle, "None");
			}
			break;
		case SETTING_BORDER:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				menuStackindex++;
				menuStack[menuStackindex] = menu_filebrowser;
				menuStack[menuStackindex].enum_id = BORDER_CHOICE;
				set_initial_dir_tmpbrowser = true;
			}
			if(CTRL_START(state))
			{
				strcpy(Settings.PS3CurrentBorder, DEFAULT_BORDER_FILE);
				emulator_implementation_set_texture(Settings.PS3CurrentBorder);
			}
			break;
		case SETTING_SHADER:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				menuStackindex++;
				menuStack[menuStackindex] = menu_filebrowser;
				menuStack[menuStackindex].enum_id = SHADER_CHOICE;
				set_shader = 0;
				set_initial_dir_tmpbrowser = true;
			}
			if(CTRL_START(state))
			{
				ps3graphics_load_fragment_shader(DEFAULT_SHADER_FILE, 0);
			}
			break;
		case SETTING_SHADER_2:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				if(Settings.ScaleEnabled)
				{
					set_shader = 1;
					menuStackindex++;
					menuStack[menuStackindex] = menu_filebrowser;
					menuStack[menuStackindex].enum_id = SHADER_CHOICE;
					set_initial_dir_tmpbrowser = true;
				}
			}
			if(CTRL_START(state))
			{
				if(Settings.ScaleEnabled)
					ps3graphics_load_fragment_shader(DEFAULT_SHADER_FILE, 1);
			}
			break;
		case SETTING_FONT_SIZE:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state) || CTRL_CROSS(state))
			{
				if(Settings.PS3FontSize > 0) 
				{
					Settings.PS3FontSize--;
					sys_timer_usleep(FILEBROWSER_DELAY);
				}
			}
			if(CTRL_RIGHT(state)  || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				if((Settings.PS3FontSize < 200))
				{
					Settings.PS3FontSize++;
					sys_timer_usleep(FILEBROWSER_DELAY);
				}
			}
			if(CTRL_START(state))
				Settings.PS3FontSize = 100;
			break;
		case SETTING_KEEP_ASPECT_RATIO:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state))
			{
				if(Settings.PS3KeepAspect > 0)
				{
					Settings.PS3KeepAspect--;
					ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, SCREEN_RENDER_TEXTURE_WIDTH, SCREEN_RENDER_TEXTURE_HEIGHT, 1);
					sys_timer_usleep(SETTINGS_DELAY);
				}
			}
			if(CTRL_RIGHT(state)  || CTRL_LSTICK_RIGHT(state))
			{
				if(Settings.PS3KeepAspect < LAST_ASPECT_RATIO)
				{
					Settings.PS3KeepAspect++;
					ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, SCREEN_RENDER_TEXTURE_WIDTH, SCREEN_RENDER_TEXTURE_HEIGHT, 1);
					sys_timer_usleep(SETTINGS_DELAY);
				}
			}
			if(CTRL_START(state))
			{
				Settings.PS3KeepAspect = ASPECT_RATIO_4_3;
				ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, SCREEN_RENDER_TEXTURE_WIDTH, SCREEN_RENDER_TEXTURE_HEIGHT, 1);
				sys_timer_usleep(SETTINGS_DELAY);
			}
			break;
		case SETTING_HW_TEXTURE_FILTER:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				Settings.PS3Smooth = !Settings.PS3Smooth;
				ps3graphics_set_smooth(Settings.PS3Smooth, 0);
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
			{
				Settings.PS3Smooth = 1;
				ps3graphics_set_smooth(Settings.PS3Smooth, 0);
			}
			break;
		case SETTING_HW_TEXTURE_FILTER_2:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				Settings.PS3Smooth2 = !Settings.PS3Smooth2;
				ps3graphics_set_smooth(Settings.PS3Smooth2, 1);
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
			{
				Settings.PS3Smooth2 = 1;
				ps3graphics_set_smooth(Settings.PS3Smooth2, 1);
			}
			break;
		case SETTING_SCALE_ENABLED:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				Settings.ScaleEnabled = !Settings.ScaleEnabled;
				if(Settings.ScaleEnabled)
					ps3graphics_set_fbo_scale(1, Settings.ScaleFactor);
				else
				{
					ps3graphics_load_fragment_shader(DEFAULT_SHADER_FILE, 1);
					Settings.ScaleFactor = 1;
					ps3graphics_set_fbo_scale(0, 2);
				}
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
			{
				Settings.ScaleEnabled = 1;
				ps3graphics_set_fbo_scale(0, 2);
			}
			update_item_colors = 1;
			break;
		case SETTING_SCALE_FACTOR:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state))
			{
				if((Settings.ScaleFactor > 1) && Settings.ScaleEnabled)
				{
					Settings.ScaleFactor -= 1;
					apply_scaling();
				}
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				if((Settings.ScaleFactor < 5) && Settings.ScaleEnabled)
				{
					Settings.ScaleFactor += 1;
					apply_scaling();
				}
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
			{
				Settings.ScaleFactor = 2;
				apply_scaling();
			}
			break;
		case SETTING_HW_OVERSCAN_AMOUNT:
			if(CTRL_LEFT(state)  ||  CTRL_LSTICK_LEFT(state) || CTRL_CROSS(state))
			{
				Settings.PS3OverscanAmount--;
				Settings.PS3OverscanEnabled = 1;

				if(Settings.PS3OverscanAmount == 0)
					Settings.PS3OverscanEnabled = 0;

				ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				Settings.PS3OverscanAmount++;
				Settings.PS3OverscanEnabled = 1;

				if(Settings.PS3OverscanAmount == 0)
					Settings.PS3OverscanEnabled = 0;

				ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);
			}
			if(CTRL_START(state))
			{
				Settings.PS3OverscanAmount = 0;
				Settings.PS3OverscanEnabled = 0;
				ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);
			}
			break;
		case SETTING_SOUND_MODE:
			if(CTRL_LEFT(state) ||  CTRL_LSTICK_LEFT(state))
			{
				if(Settings.SoundMode != SOUND_MODE_NORMAL)
				{
					Settings.SoundMode--;
					emulator_toggle_sound(Settings.SoundMode);
					sys_timer_usleep(FILEBROWSER_DELAY);
				}
			}
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				if(Settings.SoundMode < SOUND_MODE_RSOUND)
				{
					Settings.SoundMode++;
					emulator_toggle_sound(Settings.SoundMode);
					sys_timer_usleep(FILEBROWSER_DELAY);
				}
			}
			if(CTRL_START(state))
			{
				Settings.SoundMode = SOUND_MODE_NORMAL;
				emulator_toggle_sound(Settings.SoundMode);
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			break;
		case SETTING_RSOUND_SERVER_IP_ADDRESS:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_CROSS(state) | CTRL_LSTICK_RIGHT(state) )
			{
				oskutil_write_initial_message(&oskutil_handle, L"192.168.1.1");
				oskutil_write_message(&oskutil_handle, L"Enter IP address for the RSound Server.");
				oskutil_start(&oskutil_handle);
				while(OSK_IS_RUNNING(oskutil_handle))
				{
					glClear(GL_COLOR_BUFFER_BIT);
					ps3graphics_draw_menu();
					psglSwap(); 
					cell_console_poll();
					cellSysutilCheckCallback();
				}

				if(oskutil_handle.text_can_be_fetched)
					strcpy(Settings.RSoundServerIPAddress, OUTPUT_TEXT_STRING(oskutil_handle));
			}
			if(CTRL_START(state))
				strcpy(Settings.RSoundServerIPAddress, "0.0.0.0");
			break;
		case SETTING_THROTTLE_MODE:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_CROSS(state) || CTRL_LSTICK_RIGHT(state))
			{
				Settings.Throttled = !Settings.Throttled;
				ps3graphics_set_vsync(Settings.Throttled);
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
			{
				Settings.Throttled = 1;
				ps3graphics_set_vsync(Settings.Throttled);
			}
			break;
		case SETTING_TRIPLE_BUFFERING:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state))
			{
				Settings.TripleBuffering = !Settings.TripleBuffering;
				ps3graphics_set_triple_buffering(Settings.TripleBuffering);
				ps3graphics_switch_resolution(ps3graphics_get_current_resolution(), Settings.PS3PALTemporalMode60Hz, Settings.TripleBuffering, Settings.ScaleEnabled, Settings.ScaleFactor);
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
			{
				if(Settings.TripleBuffering == false)
				{
					Settings.TripleBuffering = 1;
					ps3graphics_set_triple_buffering(Settings.TripleBuffering);
					ps3graphics_switch_resolution(ps3graphics_get_current_resolution(), Settings.PS3PALTemporalMode60Hz, Settings.TripleBuffering, Settings.ScaleEnabled, Settings.ScaleFactor);
				}
			}
			break;
		case SETTING_ENABLE_SCREENSHOTS:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state))
			{
#if(CELL_SDK_VERSION > 0x340000)
				Settings.ScreenshotsEnabled = !Settings.ScreenshotsEnabled;
				if(Settings.ScreenshotsEnabled)
				{
					cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
					CellScreenShotSetParam screenshot_param = {0, 0, 0, 0};

					screenshot_param.photo_title = "SNES9x PS3";
					screenshot_param.game_title = "SNES9x PS3";
					cellScreenShotSetParameter (&screenshot_param);
					cellScreenShotEnable();
				}
				else
				{
					cellScreenShotDisable();
					cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
				}

				sys_timer_usleep(FILEBROWSER_DELAY);
#endif
			}
			if(CTRL_START(state))
			{
#if(CELL_SDK_VERSION > 0x340000)
				Settings.ScreenshotsEnabled = false;
#endif
			}
			break;
		case SETTING_SAVE_SHADER_PRESET:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state)  || CTRL_RIGHT(state) | CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				emulator_save_settings(SHADER_PRESET_FILE);
			}
			break;
		case SETTING_APPLY_SHADER_PRESET_ON_STARTUP:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state)  || CTRL_RIGHT(state) | CTRL_LSTICK_RIGHT(state) || CTRL_START(state) || CTRL_CROSS(state))
			{
				if(Settings.ApplyShaderPresetOnStartup)
					Settings.ApplyShaderPresetOnStartup = 0;
				else
					Settings.ApplyShaderPresetOnStartup = 1;
			}
			break;
		case SETTING_DEFAULT_VIDEO_ALL:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state)  || CTRL_RIGHT(state) | CTRL_LSTICK_RIGHT(state) || CTRL_START(state) || CTRL_CROSS(state))
			{
				Settings.PS3FontSize = 100;
				strcpy(Settings.ShaderPresetPath, "");
				strcpy(Settings.ShaderPresetTitle, "None");

				Settings.PS3KeepAspect = 0;
				ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, SCREEN_RENDER_TEXTURE_WIDTH, SCREEN_RENDER_TEXTURE_HEIGHT, 1);

				Settings.PS3Smooth = 1;
				Settings.PS3Smooth2 = 1;
				ps3graphics_set_smooth(Settings.PS3Smooth, 0);
				ps3graphics_set_smooth(Settings.PS3Smooth2, 1);

				Settings.PS3OverscanEnabled = 0;
				Settings.PS3OverscanAmount = 0;
				ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);

				Settings.PS3PALTemporalMode60Hz = 0;
				ps3graphics_set_pal60hz(Settings.PS3PALTemporalMode60Hz);

				if(!Settings.TripleBuffering)
				{
					Settings.TripleBuffering = 1;
					ps3graphics_set_triple_buffering(Settings.TripleBuffering);

					ps3graphics_switch_resolution(ps3graphics_get_current_resolution(), Settings.PS3PALTemporalMode60Hz, Settings.TripleBuffering, Settings.ScaleEnabled, Settings.ScaleFactor);

					Settings.ScaleEnabled = 0;
					Settings.ScaleFactor = 2;
					ps3graphics_set_fbo_scale(Settings.ScaleEnabled, Settings.ScaleFactor);
				}
				Settings.Throttled = 1;
				ps3graphics_set_vsync(Settings.Throttled);

				Settings.ScreenshotsEnabled = false;
				emulator_implementation_set_texture(DEFAULT_BORDER_FILE);
				Settings.ApplyShaderPresetOnStartup = 0;
				update_item_colors = 1;
			}
			break;
		case SETTING_DEFAULT_AUDIO_ALL:
			if(CTRL_LEFT(state)  || CTRL_LSTICK_LEFT(state)  || CTRL_RIGHT(state) | CTRL_LSTICK_RIGHT(state) || CTRL_START(state) || CTRL_CROSS(state))
			{
				strcpy(Settings.RSoundServerIPAddress, "0.0.0.0");
				if(Settings.SoundMode == SOUND_MODE_RSOUND)
				{
					Settings.SoundMode = SOUND_MODE_NORMAL;
					emulator_toggle_sound(Settings.SoundMode);
				}
				Settings.SoundMode = SOUND_MODE_NORMAL;
				emulator_toggle_sound(Settings.SoundMode);
			}
			break;
			// display framerate on/off
		case SETTING_SNES9X_SOUND_INPUT_RATE:
			if(CTRL_LEFT(state) | CTRL_LSTICK_LEFT(state) )
			{
				if(Settings.SoundInputRate > 30000)
					Settings.SoundInputRate--;

				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state))
			{
				if(Settings.SoundInputRate < 32000) // SNES APU has an input rate of 32000, no higher
					Settings.SoundInputRate++;

				sys_timer_usleep(FILEBROWSER_DELAY);
			}

			if(CTRL_START(state))
				Settings.SoundInputRate = 31950;

			break;
		case SETTING_SNES9X_MUTE_SOUND:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state))
			{
				*(menu_emu_audiosettings.items[switchvalue].setting_ptr) = !(*(menu_emu_audiosettings.items[switchvalue].setting_ptr));
				sys_timer_usleep(FILEBROWSER_DELAY);
			}

			if(CTRL_START(state))
				*(menu_emu_audiosettings.items[switchvalue].setting_ptr) = 1;
			break;
		case SETTING_SNES9X_AUTO_APPLY_CHEATS:
		case SETTING_SNES9X_AUTO_APPLY_PATCH:
		case SETTING_SNES9X_SRAM_WRITEPROTECT:
			if(CTRL_RIGHT(state) || CTRL_LSTICK_LEFT(state) || CTRL_LEFT(state) || CTRL_CROSS(state))
			{
				*(menu_emu_settings.items[switchvalue].setting_ptr) = !(*(menu_emu_settings.items[switchvalue].setting_ptr));
				sys_timer_usleep(FILEBROWSER_DELAY);
			}

			if(CTRL_START(state))
				*(menu_emu_settings.items[switchvalue].setting_ptr) = menu_emu_settings.items[switchvalue].default_value;
			break;
		case SETTING_SNES9X_FORCE_PAL:
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LSTICK_LEFT(state) || CTRL_LEFT(state) || CTRL_CROSS(state))
			{
				Settings.ForcePAL = !Settings.ForcePAL;
				if(Settings.ForceNTSC)
					Settings.ForceNTSC = 0;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}

			if(CTRL_START(state))
				Settings.ForcePAL = 0;
			break;
		case SETTING_SNES9X_FORCE_NTSC:
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LSTICK_LEFT(state) || CTRL_LEFT(state) || CTRL_CROSS(state))
			{
				Settings.ForceNTSC = !Settings.ForceNTSC;
				if(Settings.ForcePAL)
					Settings.ForcePAL = 0;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}

			if(CTRL_START(state))
				Settings.ForceNTSC = 0;

			break;
		case SETTING_SNES9X_PAL_TIMING:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) )
			{
				Settings.FrameTimePAL = 16667;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) )
			{
				Settings.FrameTimePAL = 20000;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
				Settings.FrameTimePAL = 20000;

			break;
		case SETTING_SNES9X_ACCESSORY_AUTODETECTION:
			if(CTRL_LEFT(state) | CTRL_LSTICK_LEFT(state))
			{
				if(Settings.AccessoryAutoDetection != ACCESSORY_AUTODETECTION_CONFIRM)
					Settings.AccessoryAutoDetection--;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				if(Settings.AccessoryAutoDetection != ACCESSORY_AUTODETECTION_NONE)
					Settings.AccessoryAutoDetection++;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
				Settings.AccessoryAutoDetection = ACCESSORY_AUTODETECTION_CONFIRM;
			break;
		case SETTING_SNES9X_ACCESSORY_TYPE:
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state))
			{
				Settings.AccessoryType == 1 ? Settings.AccessoryType = 0 : Settings.AccessoryType = 1;
				sys_timer_usleep(FILEBROWSER_DELAY);
			}
			if(CTRL_START(state))
				Settings.AccessoryType = 0;

			break;
		case SETTING_EMU_DEFAULT_ALL:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_START(state) || CTRL_CROSS(state))
			{
				Settings.ForceNTSC = 0;
				Settings.ForcePAL = 0;
				Settings.FrameTimePAL = 20000;
				Settings.NoPatch = 0;
				Settings.ApplyCheats = 1;
				Settings.ResetBeforeRecordingMovie = 0;
				Settings.AccessoryAutoDetection = ACCESSORY_AUTODETECTION_CONFIRM;
				Settings.AccessoryType = 0;
				Settings.SRAMWriteProtect = 0;
			}
			break;
		case SETTING_EMU_AUDIO_DEFAULT_ALL:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_START(state) || CTRL_CROSS(state))
			{
				Settings.SoundInputRate = 31950;
			}
			break;
		case SETTING_PATH_DEFAULT_ROM_DIRECTORY:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				menuStackindex++;
				menuStack[menuStackindex] = menu_filebrowser;
				menuStack[menuStackindex].enum_id = PATH_DEFAULT_ROM_DIR_CHOICE;
				set_initial_dir_tmpbrowser = true;
			}

			if(CTRL_START(state))
				strcpy(Settings.PS3PathROMDirectory, "/");

			break;
		case SETTING_PATH_SAVESTATES_DIRECTORY:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				menuStackindex++;
				menuStack[menuStackindex] = menu_filebrowser;
				menuStack[menuStackindex].enum_id = PATH_SAVESTATES_DIR_CHOICE;
				set_initial_dir_tmpbrowser = true;
			}

			if(CTRL_START(state))
				strcpy(Settings.PS3PathSaveStates, usrDirPath);

			break;
		case SETTING_PATH_SRAM_DIRECTORY:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) ||  CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				menuStackindex++;
				menuStack[menuStackindex] = menu_filebrowser;
				menuStack[menuStackindex].enum_id = PATH_SRAM_DIR_CHOICE;
				set_initial_dir_tmpbrowser = true;
			}

			if(CTRL_START(state))
				strcpy(Settings.PS3PathSRAM, "");

			break;
		case SETTING_PATH_CHEATS:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) ||  CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state))
			{
				menuStackindex++;
				menuStack[menuStackindex] = menu_filebrowser;
				menuStack[menuStackindex].enum_id = PATH_CHEATS_DIR_CHOICE;
				set_initial_dir_tmpbrowser = true;
			}

			if(CTRL_START(state))
				strcpy(Settings.PS3PathCheats, usrDirPath);

			break;
		case SETTING_PATH_DEFAULT_ALL:
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) ||  CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state) || CTRL_START(state))
			{
				strcpy(Settings.PS3PathROMDirectory, "/");
				strcpy(Settings.PS3PathSaveStates, usrDirPath);
				strcpy(Settings.PS3PathCheats, usrDirPath);
				strcpy(Settings.PS3PathSRAM, "");
			}
			break;
	} // end of switch
}
