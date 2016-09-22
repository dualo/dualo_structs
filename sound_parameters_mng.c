/*
 * sound_parameters_mng.c
 *
 *  Created on: 07/08/2013
 *      Author: mma
 */

// includes
#include "sound_parameters_mng.h"
#include "old_sound_parameters_mng.h"
#include "controler_parameters_mng.h"
#include "music_parameters_mng.h"
#include "g_parameters_mng.h"
#include "yaffsfs.h"
#include "config.h"
#include "midi_task.h"
#include "main_task.h"
#include "mem/memory_manager.h"
#include "string.h"
#include "layer_play.h"
#include "arpeggiator/arpeggiator_module.h"
#include "layer_sound.h"
#include "dream/dream.h"
#include "usb_vendor_interface/memoire.h"
#include "eeprom/eeprom_map.h"
#include "dutouch_config.h"

// external parameters
extern uint8_t fx_distortion_preset[2][FX_DISTO_NUM_PARAMETERS+FX_DISTO_NOP_PARAMETERS];
extern uint8_t fx_compressor_preset[2][FX_COMP_NUM_PARAMETERS+FX_COMP_NOP_PARAMETERS];
extern uint8_t fx_mix_preset[FX_MIX_NUM_PARAMETERS+FX_MIX_NOP_PARAMETERS];
extern uint8_t fx_equalizer_preset[FX_NUM_FX_INTR][FX_PEQ_NUM_PARAMETERS+FX_PEQ_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_instr_equalizer_preset[2][FX_PEQ_NUM_PARAMETERS+FX_PEQ_NOP_PARAMETERS];
extern uint8_t fx_chorus_preset[4][FX_CHORUS_NUM_PARAMETERS+FX_CHORUS_NOP_PARAMETERS];
extern uint8_t fx_delay_preset[2][FX_DELAY_NUM_PARAMETERS+FX_DELAY_NOP_PARAMETERS];
extern uint8_t fx_reverb_preset[FX_NUM_FX_INTR][FX_REVERB_NUM_PARAMETERS+FX_REVERB_NOP_PARAMETERS+FX_NUM_NAME];

extern info_instr *keyboardL_instr_map;
extern info_instr *keyboardR_instr_map;

extern struct_controler controler_tab;
extern sound_instr instr_tab[];
extern music_song music_tab[];

extern liste_occupe tete_occupe;
extern uint32_t audio_status;

uint32_t quart_pitch = 0;

extern uint8_t headerSNDBK[];
//__attribute__ ((section(".fxextflash")))
//struct_instr *saved_sound_tab[2*dt_config.nb_key];

/*
FX_distortion	*current_instr_distortion;
FX_wah			*current_instr_wah;
FX_compressor	*current_instr_compressor;
FX_equalizer	*current_instr_equalizer;
FX_chorus		*current_instr_chorus;
FX_delay		*current_instr_delay;
FX_vibrato		*current_instr_vibrato;
FX_adsr			*current_instr_adsr;
*/
extern FX_mix			temp_mix;
extern FX_distortion	temp_distortion;
extern FX_compressor	temp_compressor;
extern FX_equalizer		temp_equalizer;
extern FX_equalizer		temp_eqmain;
extern FX_chorus		temp_chorus;
extern FX_delay			temp_delay;
extern FX_reverb		temp_reverb;
//extern preset_instr	temp_preset_instr;


info_instr tmp_instr;

/****** check if current instrument is available ************/
uint32_t checksoundavailable(uint32_t num_sound)
{
	uint32_t ret = 0xFF;
	uint32_t i;

	if(num_sound>=(2*NUM_INSTRU_KEYBOARD))
		num_sound = 0; // we never know

	if(instr_tab[num_sound].s_instrument.instr_midi_pc != 0xFF)
	{
		ret = num_sound;
	}
	else
	{
		for(i=0; i<(2*NUM_INSTRU_KEYBOARD), ret == 0xFF; i++)
		{
			if(instr_tab[i].s_instrument.instr_midi_pc != 0xFF)
			{
				ret = i;
			}
		}
	}

	if(ret == 0xFF)
	{ // no du-sound available
		get_dummysound(&instr_tab[0]);
		ret = 0;
	}

	return ret;
}

void get_soundfilename(uint32_t sound, char *string)
{
	char tmp[4];

	strcpy(string, "/p0/sound/sound_");

	if(sound>99)
		tmp[0] = 48 + (sound/100);
	else
		tmp[0] = 48;

	if((sound%100)>9)
		tmp[1] = 48 + ((sound%100)/10);
	else
		tmp[1] = 48;
	tmp[2] = 48 + (sound%10);
	tmp[3] = 0;

	strcat(string, tmp);
}

/******* fx struct initialisation ************/
void init_instrmap(void)
{
	uint32_t i, j;
	int32_t instr_file_L = -1;
	int32_t instr_file_R = -1;
	uint32_t ret;

#if 1
		keyboardL_instr_map = (info_instr *)&headerSNDBK[0xF00];
		keyboardR_instr_map = (info_instr *)&headerSNDBK[0xF00 + (INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD)];
	#else
	#if 0
		synchronise_mapping(tete_occupe);
		memcpy( (uint8_t *)keyboardL_instr_map, &headerSNDBK[0xF00], INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD);
		memcpy( (uint8_t *)keyboardR_instr_map, &headerSNDBK[0xF00 + (INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD)], INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD);
	#else
		//synchronise_mapping(tete_occupe);
		instr_file_R = yaffs_open("/p0/instruments/KR_map", O_RDWR, 0);
		instr_file_L = yaffs_open("/p0/instruments/KL_map", O_RDWR, 0);
		if(instr_file_L == -1)
		{
			//synchronise_mapping(tete_occupe);
			memcpy( (uint8_t *)keyboardL_instr_map, &headerSNDBK[0xF00], INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD);
		}
		else
		{
			synchronise_mapping(tete_occupe);
			ret = yaffs_read(instr_file_L,(uint8_t *)keyboardL_instr_map, INSTR_INFO_SIZE * dt_config.nb_key);
			yaffs_close(instr_file_L);
		}

		if(instr_file_R == -1)
		{
			//synchronise_mapping(tete_occupe);
			memcpy( (uint8_t *)keyboardR_instr_map, &headerSNDBK[0xF00 + (INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD)], INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD);
		}
		else
		{
			synchronise_mapping(tete_occupe);
			ret = yaffs_read(instr_file_R,(uint8_t *)keyboardR_instr_map, INSTR_INFO_SIZE * dt_config.nb_key);
			yaffs_close(instr_file_R);
		}
	#endif
	#endif
}


#if 0
void load_instrmap(void)
{
	uint32_t i, j;
	int32_t instr_file_L;
	int32_t instr_file_R;
	uint32_t ret;

	instr_file_R = yaffs_open("/p0/instruments/KR_map", O_RDWR, 0);
	instr_file_L = yaffs_open("/p0/instruments/KL_map", O_RDWR, 0);
	if(instr_file_L == -1)
	{
		//synchronise_mapping(tete_occupe);
		memcpy( (uint8_t *)keyboardL_instr_map, &headerSNDBK[0xF00], INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD);
	}
	else
	{
		ret = yaffs_read(instr_file_L,(uint8_t *)keyboardL_instr_map, INSTR_INFO_SIZE * dt_config.nb_key);
		yaffs_close(instr_file_L);
	}

	if(instr_file_R == -1)
	{
		//synchronise_mapping(tete_occupe);
		memcpy( (uint8_t *)keyboardR_instr_map, &headerSNDBK[0xF00 + (INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD)], INSTR_INFO_SIZE * NUM_BUTTON_KEYBOARD);
	}
	else
	{
#ifndef YAFFS_OFF
		ret = yaffs_read(instr_file_R,(uint8_t *)keyboardR_instr_map, INSTR_INFO_SIZE * dt_config.nb_key);
		yaffs_close(instr_file_R);
#endif
	}
}
#endif
void get_dummysound(sound_instr *sound_struct)
{
	int32_t j;
	char str[16];
	int32_t sound_file;

	memset((void *)&tmp_instr, 0, INSTR_INFO_SIZE);
	strcpy(tmp_instr.instr_name, "empty");
	tmp_instr.instr_noteoff = 0xA5;

	strcpy(str, "/p0/sound/dummy");

	sound_file = yaffs_open(str, O_RDWR, 0);

	open_soundfile( sound_file, sound_struct, &tmp_instr, 0);

	memset((void *)&sound_struct->s_instrument, 0, INSTR_INFO_SIZE);
	strcpy(sound_struct->s_instrument.instr_name, "empty");
	sound_struct->s_instrument.instr_noteoff = 0xA5;

}


/******* fx struct parameters initialisation ************/
uint32_t migrate_sound(sound_instr *sound_struct)
{
	uint32_t version = 0xFFFFFFFF;
	uint32_t i, j;
	uint32_t ret = 0;

	EEPROM_Read8(eeprom_map[EE_INFO_LASTFIRMWARE].addr,(void *)&version, eeprom_map[EE_INFO_LASTFIRMWARE].size);

	if(sound_struct->s_instrument.SW_instr_version < DUSOUND_SW_VERSION)
	{ // adapte sw version to firmware version
		version = 0;
	}

	if(version<0x00020018) // V2.0.24
	{
		ret = 1;
		struct_instr_old* sound_struct_old = (struct_instr_old*)sound_struct;

		if(version<0x00020000) // V2.0.00
		{
			sound_struct_old->s_displayled = 0;

			for(i = 0; i<FX_NUM_PRESET_INTR; i++)
			{
				sound_struct_old->s_preset[i].s_key_curve = KEYS_CURVES_DEFAULTVALUE;

				sound_struct_old->s_preset[i].s_arpegiator_type = 0;
				sound_struct_old->s_preset[i].s_arpegiator_beat = 0;

				sound_struct_old->s_preset[i].s_direction_gyro_P = 0;
				sound_struct_old->s_preset[i].s_direction_gyro_R = 0;
				sound_struct_old->s_preset[i].s_direction_gyro_Y = 0;

				sound_struct_old->s_preset[i].s_compressor_onoff = 0;
				sound_struct_old->s_preset[i].s_delay_onoff = 0;
				sound_struct_old->s_preset[i].s_distortion_onoff = 0;
				sound_struct_old->s_preset[i].s_eq_onoff = 0;
				sound_struct_old->s_preset[i].s_chorus_onoff = 0;

				sound_struct_old->s_preset[i].s_autopitch_rate = 0;
				sound_struct_old->s_preset[i].s_autopitch_range = 0;
				sound_struct_old->s_preset[i].s_tremolo_rate = 0;
				sound_struct_old->s_preset[i].s_tremolo_range = 0;
				sound_struct_old->s_preset[i].s_autopan_rate = 0;
				sound_struct_old->s_preset[i].s_autopan_range = 0;
				sound_struct_old->s_preset[i].s_autowah_rate = 0;
				sound_struct_old->s_preset[i].s_autowah_range = 0;

				sound_struct_old->s_preset[i].s_adsr_attack = FX_ADSR_ATTACK_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_adsr_release = FX_ADSR_RELEAS_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_wah_type = FX_WAH_FILTERTYPE_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_wah_freq = 0; //FX_WAH_FILTERFREQ_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_wah_res = FX_WAH_FILTERRES_DEFAULTVALUE;
			}
		}

		if(version<0x0002000E) // V2.0.14
		{
			set_equalizerpreset(&(sound_struct_old->s_equalizer[0]), 0); // for getting "off" displayed

			for(i = 0; i<FX_NUM_PRESET_INTR; i++)
			{
				sound_struct_old->s_preset[i].s_adsr_attack = FX_ADSR_ATTACK_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_adsr_release = FX_ADSR_RELEAS_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_wah_type = FX_WAH_FILTERTYPE_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_wah_freq = 0; //FX_WAH_FILTERFREQ_DEFAULTVALUE;
				sound_struct_old->s_preset[i].s_wah_res = FX_WAH_FILTERRES_DEFAULTVALUE;
			}
		}

		if(version<0x00020010) // V2.0.14
		{
			for(i = 0; i<FX_NUM_PRESET_INTR; i++)
				sound_struct_old->s_preset[i].s_reverb_onoff = 0;
		}

		if(version<0x00020017) // V2.0.23
		{ // invert filter freq
			for(i = 0; i<FX_NUM_PRESET_INTR; i++)
				sound_struct_old->s_preset[i].s_wah_freq = 127 - sound_struct_old->s_preset[i].s_wah_freq;
		}

		if(version<0x00020018) // V2.0.24
		{
			// migration here
			sound_struct_old = (struct_instr_old*)lpc_new(INSTRU_STRUCT_SIZE);
			memcpy(sound_struct_old, sound_struct, INSTRU_STRUCT_SIZE);
			memset(sound_struct, 0, INSTRU_STRUCT_SIZE);

			memcpy(&sound_struct->s_instrument, &sound_struct_old->s_instrument, INSTR_INFO_SIZE);

			sound_struct->s_presetnum = sound_struct_old->s_presetnum;

			memcpy(sound_struct->s_complete_name, sound_struct->s_instrument.instr_name, NAME_CARACT);

			for(i = 0; i<FX_NUM_PRESET_INTR; i++)
			{
				init_preset( &(sound_struct->s_preset[i]), sound_struct->s_instrument.instr_octave, i);

				sound_struct->s_preset[i].s_volume = sound_struct_old->s_preset[i].s_volume;
				sound_struct->s_preset[i].s_panning = sound_struct_old->s_preset[i].s_panning;
				sound_struct->s_preset[i].s_sendtorev = sound_struct_old->s_preset[i].s_sendtorev;
				sound_struct->s_preset[i].s_instr_octave = sound_struct_old->s_preset[i].s_instr_octave;
				sound_struct->s_preset[i].s_key_curve = sound_struct_old->s_preset[i].s_key_curve;
				sound_struct->s_preset[i].s_expression = sound_struct_old->s_preset[i].s_expression;

				sound_struct->s_preset[i].s_activ_aftertouch = sound_struct_old->s_preset[i].s_activ_aftertouch;
				sound_struct->s_preset[i].s_activ_slider_L = sound_struct_old->s_preset[i].s_activ_slider_L;
				sound_struct->s_preset[i].s_activ_slider_R = sound_struct_old->s_preset[i].s_activ_slider_R;
				sound_struct->s_preset[i].s_activ_gyro_P = sound_struct_old->s_preset[i].s_activ_gyro_P;
				sound_struct->s_preset[i].s_activ_gyro_R = sound_struct_old->s_preset[i].s_activ_gyro_R;
				sound_struct->s_preset[i].s_activ_gyro_Y = sound_struct_old->s_preset[i].s_activ_gyro_Y;

				sound_struct->s_preset[i].s_direction_gyro_P = sound_struct_old->s_preset[i].s_direction_gyro_P;
				sound_struct->s_preset[i].s_direction_gyro_R = sound_struct_old->s_preset[i].s_activ_gyro_R;
				sound_struct->s_preset[i].s_direction_gyro_Y = sound_struct_old->s_preset[i].s_direction_gyro_Y;

				sound_struct->s_preset[i].s_portamento_on_off = sound_struct_old->s_preset[i].s_portamento_on_off;
				sound_struct->s_preset[i].s_portamento_ctrl = sound_struct_old->s_preset[i].s_portamento_ctrl;
				sound_struct->s_preset[i].s_portamento_time = sound_struct_old->s_preset[i].s_portamento_time;

				sound_struct->s_preset[i].s_displayled = sound_struct_old->s_displayled;
				memcpy(&sound_struct->s_preset[i].s_leds[0], &sound_struct_old->s_leds[0], NUM_LED_VALUE);

				sound_struct->s_preset[i].s_pitch = 64;
				sound_struct->s_preset[i].s_pitch_bend_sensitivity = sound_struct_old->s_preset[i].s_pitch_bend_sensitivity;

				sound_struct->s_preset[i].s_adsr_attack = sound_struct_old->s_preset[i].s_adsr_attack;
				sound_struct->s_preset[i].s_adsr_release = sound_struct_old->s_preset[i].s_adsr_release;

				sound_struct->s_preset[i].s_wah_type = sound_struct_old->s_preset[i].s_wah_type;
				sound_struct->s_preset[i].s_wah_freq = sound_struct_old->s_preset[i].s_wah_freq;
				sound_struct->s_preset[i].s_wah_res = sound_struct_old->s_preset[i].s_wah_res;

				sound_struct->s_preset[i].s_multinote_act = sound_struct_old->s_preset[i].s_multinote_act;
				memcpy(&sound_struct->s_preset[i].s_multinote[0], &sound_struct_old->s_preset[i].s_multinote[0], 4);

				sound_struct->s_preset[i].s_arpegiator_type = sound_struct_old->s_preset[i].s_arpegiator_type;
				sound_struct->s_preset[i].s_arpegiator_beat = sound_struct_old->s_preset[i].s_arpegiator_beat;

				sound_struct->s_preset[i].s_autopitch_rate = sound_struct_old->s_preset[i].s_autopitch_rate;
				sound_struct->s_preset[i].s_autopitch_range = sound_struct_old->s_preset[i].s_autopitch_range;

				sound_struct->s_preset[i].s_tremolo_rate = sound_struct_old->s_preset[i].s_tremolo_rate;
				sound_struct->s_preset[i].s_tremolo_range = sound_struct_old->s_preset[i].s_tremolo_range;

				sound_struct->s_preset[i].s_autopan_rate = sound_struct_old->s_preset[i].s_autopan_rate;
				sound_struct->s_preset[i].s_autopan_range = sound_struct_old->s_preset[i].s_autopan_range;

				sound_struct->s_preset[i].s_autowah_rate = sound_struct_old->s_preset[i].s_autowah_rate;
				sound_struct->s_preset[i].s_autowah_range = sound_struct_old->s_preset[i].s_autowah_range;

				sound_struct->s_preset[i].s_reverb_onoff = sound_struct_old->s_preset[i].s_reverb_onoff;

				if(sound_struct_old->s_preset[i].s_compressor_preset != 0)
				{
					sound_struct->s_preset[i].s_compressor_onoff = 1;
					memcpy(&sound_struct->s_preset[i].s_compressor, &(sound_struct_old->s_compressor[sound_struct_old->s_preset[i].s_compressor_preset]), FX_COMP_SIZE);
				}
				if(sound_struct_old->s_preset[i].s_delay_preset != 0)
				{
					sound_struct->s_preset[i].s_delay_onoff = 1;
					memcpy(&sound_struct->s_preset[i].s_delay, &(sound_struct_old->s_delay[sound_struct_old->s_preset[i].s_delay_preset]), FX_DELAY_SIZE);
				}
				if(sound_struct_old->s_preset[i].s_distortion_preset != 0)
				{
					sound_struct->s_preset[i].s_distortion_onoff = 1;
					memcpy(&sound_struct->s_preset[i].s_distortion, &(sound_struct_old->s_distortion[sound_struct_old->s_preset[i].s_distortion_preset]), FX_DIST_SIZE);
				}
				if(sound_struct_old->s_preset[i].s_eq_preset != 0)
				{
					sound_struct->s_preset[i].s_eq_onoff = 1;
					memcpy(&sound_struct->s_preset[i].s_equalizer, &(sound_struct_old->s_equalizer[sound_struct_old->s_preset[i].s_eq_preset]), FX_EQ_SIZE);
				}

				if(sound_struct_old->s_preset[i].s_chorus_preset != 0)
				{
					sound_struct->s_preset[i].s_chorus_preset = 1;
					sound_struct->s_preset[i].s_chorus_onoff = 1;
					memcpy(&sound_struct->s_preset[i].s_chorus[0], &(sound_struct_old->s_chorus[sound_struct_old->s_preset[i].s_chorus_preset]), FX_CHORUS_SIZE);
				}
			}

			lpc_free(sound_struct_old);
		}
	}
}

void init_sound(sound_instr *sound_struct, uint32_t instr)
{
	int32_t ret;
	char str[80];
	int32_t sound_file = -1;

	//trace( info, "init sound %d\r\n", instr);

	get_soundfilename( instr, str);

	sound_file = yaffs_open(str, O_RDWR, 0);

	if(instr < NUM_INSTRU_KEYBOARD)
		ret = open_soundfile( sound_file, sound_struct, &(keyboardL_instr_map[instr]), instr);
	else
		ret = open_soundfile( sound_file, sound_struct, &(keyboardR_instr_map[instr-NUM_INSTRU_KEYBOARD]), instr);

	if(ret) // need to save file
		save_sound( sound_struct, instr);
}

void init_soundsystem(sound_instr *sound_struct)
{
	int32_t sound_file;

	sound_file = yaffs_open("/p0/sound/sound_system", O_RDWR, 0);

	open_soundfile( sound_file, sound_struct, NULL, 116);
}

uint32_t open_soundfile(uint32_t sound_file, sound_instr *sound_struct, info_instr *instr_struct, uint32_t key)
{
	uint32_t j;
	uint32_t ret = 0;

	if(sound_file == -1)
	{
		ret = 1;
		memcpy( (uint8_t *)sound_struct, &headerSNDBK[PRESET_STARTADRESS + (key * INSTRU_STRUCT_SIZE)], INSTRU_STRUCT_SIZE);
//		if(key<NUM_INSTRU_KEYBOARD)
//			memcpy((uint8_t *)&(sound_struct->s_instrument), &keyboardL_instr_map[key], INSTR_INFO_SIZE);
//		else
//			memcpy((uint8_t *)&(sound_struct->s_instrument), &keyboardR_instr_map[key - NUM_INSTRU_KEYBOARD], INSTR_INFO_SIZE);
/*		if (sound_struct->s_instrument.instr_midi_pc == 0xFF)
		{
			if(instr_struct!=NULL)
			{
				for(j = 0; j<NAME_CARACT; j++)
				{
					sound_struct->s_instrument.instr_name[j] = instr_struct->instr_name[j];
					sound_struct->s_instrument.instr_cat[j] = instr_struct->instr_cat[j];
				}
				sound_struct->s_instrument.instr_midi_pc  = instr_struct->instr_midi_pc;
				sound_struct->s_instrument.instr_midi_C0  = instr_struct->instr_midi_C0;
				sound_struct->s_instrument.instr_key_map = instr_struct->instr_key_map;
				sound_struct->s_instrument.instr_octave = instr_struct->instr_octave;
				sound_struct->s_instrument.instr_id = instr_struct->instr_id;
				sound_struct->s_instrument.instr_noteoff = instr_struct->instr_noteoff;
				sound_struct->s_instrument.instr_relvolume = instr_struct->instr_relvolume;
				sound_struct->s_instrument.instr_type = instr_struct->instr_type;
			}

			if((sound_struct->s_instrument.instr_key_map)&&(sound_struct->s_instrument.instr_type == INSTR_HARMONIC))
				sound_struct->s_instrument.instr_type = INSTR_PERCU;
			else if((sound_struct->s_instrument.instr_key_map == 0)&&(sound_struct->s_instrument.instr_type == INSTR_PERCU))
				sound_struct->s_instrument.instr_type = INSTR_HARMONIC;

			sound_struct->s_presetnum = 0;

			for(j = 0; j<FX_NUM_PRESET_INTR; j++)
			{
				 init_preset(&(sound_struct->s_preset[j]), sound_struct->s_instrument.instr_octave, j);
			}

			memcpy(sound_struct->s_complete_name, sound_struct->s_instrument.instr_name, NAME_CARACT);

		}
*/
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_read(sound_file,(uint8_t *)sound_struct, INSTRU_STRUCT_SIZE);
		yaffs_close(sound_file);
#endif
	}

	if(instr_struct!=NULL)
	{
		memcpy((uint8_t *)&(sound_struct->s_instrument), instr_struct, INSTR_INFO_SIZE);
//		for(j = 0; j<NAME_CARACT; j++)
//		{
//			sound_struct->s_instrument.instr_name[j] = instr_struct->instr_name[j];
//			sound_struct->s_instrument.instr_cat[j] = instr_struct->instr_cat[j];
//		}
//		sound_struct->s_instrument.instr_midi_pc  = instr_struct->instr_midi_pc;
//		sound_struct->s_instrument.instr_midi_C0  = instr_struct->instr_midi_C0;
//		sound_struct->s_instrument.instr_key_map = instr_struct->instr_key_map;
//		sound_struct->s_instrument.instr_octave = instr_struct->instr_octave;
//		sound_struct->s_instrument.instr_id = instr_struct->instr_id;
//		sound_struct->s_instrument.instr_noteoff = instr_struct->instr_noteoff;
//		sound_struct->s_instrument.instr_relvolume = instr_struct->instr_relvolume;
//		sound_struct->s_instrument.instr_type = instr_struct->instr_type;
	}


	//Resolved problem of migration, absurd settings values
	if((sound_struct->s_instrument.instr_key_map)&&(sound_struct->s_instrument.instr_type == INSTR_HARMONIC))
		sound_struct->s_instrument.instr_type = INSTR_PERCU;
	else if((sound_struct->s_instrument.instr_key_map == 0)&&(sound_struct->s_instrument.instr_type == INSTR_PERCU))
		sound_struct->s_instrument.instr_type = INSTR_HARMONIC;

	if(sound_struct->s_instrument.instr_relvolume<40)
		sound_struct->s_instrument.instr_relvolume = 0x7F;

	ret |= migrate_sound(sound_struct);


	for(j = 0; j<FX_NUM_PRESET_INTR; j++)
	{
		//sound_struct->s_preset[j].s_activ_aftertouch = check_chainedfx(sound_struct->s_preset[j].s_aftertouch_chainedfx, sound_struct->s_preset[j].s_activ_aftertouch);
		//sound_struct->s_preset[j].s_activ_slider_L = check_chainedfx(sound_struct->s_preset[j].s_slider_L_chainedfx, sound_struct->s_preset[j].s_activ_slider_L);
		//sound_struct->s_preset[j].s_activ_slider_R = check_chainedfx(sound_struct->s_preset[j].s_slider_R_chainedfx, sound_struct->s_preset[j].s_activ_slider_R);
		//sound_struct->s_preset[j].s_activ_gyro_P = check_chainedfx(sound_struct->s_preset[j].s_gyro_P_chainedfx, sound_struct->s_preset[j].s_activ_gyro_P);
		//sound_struct->s_preset[j].s_activ_gyro_R = check_chainedfx(sound_struct->s_preset[j].s_gyro_R_chainedfx, sound_struct->s_preset[j].s_activ_gyro_R);
		//sound_struct->s_preset[j].s_activ_gyro_Y = check_chainedfx(sound_struct->s_preset[j].s_gyro_Y_chainedfx, sound_struct->s_preset[j].s_activ_gyro_Y);

		if(sound_struct->s_preset[j].s_expression<64)
			sound_struct->s_preset[j].s_expression = MAIN_EXPRESSION_DEFAULT;

		if(sound_struct->s_preset[j].s_key_curve>KEYS_CURVES_MAXVALUE)
			sound_struct->s_preset[j].s_key_curve = KEYS_CURVES_DEFAULTVALUE;

		//if(sound_struct->s_instrument.SW_instr_version < DUSOUND_SW_VERSION)
		//{
		//	update_sw_version(&(sound_struct->s_preset[j]));
		//}

	}

	if(sound_struct->s_presetnum >= FX_NUM_PRESET_INTR)
	{
		sound_struct->s_presetnum = 0;
	}

	if(sound_struct->s_instrument.SW_instr_version < DUSOUND_SW_VERSION)
	{
		sound_struct->s_instrument.SW_instr_version = DUSOUND_SW_VERSION;
	}
	//}

	return ret;
}

/******* fx struct parameters saving ************/
void erase_sound( uint32_t instr)
{
	char str[80];
	strcpy(str, "/p0/sound/");

	if(instr<dt_config.nb_key)
		strcat(str, (char *)keyboardL_instr_map[instr].instr_name);
	else
		strcat(str, (char *)keyboardR_instr_map[instr - dt_config.nb_key].instr_name);
#ifndef YAFFS_OFF
	yaffs_unlink(str);
#endif
}

void save_sound(sound_instr *sound_struct, uint32_t instr)
{
#ifndef CONFIG_TEST
	int32_t sound_file;
	char str[80];
	get_soundfilename( instr, str);

	sound_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);

	if(sound_file < 0)
	{
		trace( error, "Fail! %d\r\n", instr);
		//while(1);
	}
	else
	{
		yaffs_write(sound_file,(uint8_t *)sound_struct,INSTRU_STRUCT_SIZE);
		yaffs_close(sound_file);
	}
#endif
}

void get_instrbin(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length)
{
	char str[80];
	int32_t instr_file = -1;

	get_soundfilename( instrument + (keyboard*NUM_INSTRU_KEYBOARD), str);

	instr_file = yaffs_open(str, O_RDWR, 0);

	if(instr_file == -1)
	{
		trace(info, "can't open %s\r\n", str);
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_pread( instr_file, buffer, length, address);
		yaffs_close( instr_file);
#endif
	}
}

int32_t exist_instrbin(uint32_t instrument)
{
	char str[80];
	int32_t instr_file = -1;

	get_soundfilename( instrument, str);

	instr_file = yaffs_open(str, O_RDWR, 0);
	if(instr_file >= 0)
		yaffs_close( instr_file);
	return instr_file;
}
/*
void rename_instrbin(uint32_t keyboard, uint32_t instrument, uint8_t *name)
{
	char str[80];
	char rename[80];
	int32_t instr_file;

	strcpy(str, "/p0/sound/");

	strcpy(rename, "/p0/sound/");
	strcat(rename, (char *)name);

	if(!keyboard)
		strcat(str, (char *)keyboardL_instr_map[instrument].instr_name);
	else
		strcat(str, (char *)keyboardR_instr_map[instrument].instr_name);
#ifndef YAFFS_OFF
	instr_file = yaffs_open(str, O_RDWR, 0);

	if(instr_file == -1)
	{
		trace(info, "can't open %s\r\n", str);
	}
	else
	{
		yaffs_close( instr_file);
		yaffs_rename(str, rename);
	}
}
*/
void save_instrbin(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length)
{
	char str[80];
	int32_t instr_file;

	get_soundfilename( instrument + (keyboard*NUM_INSTRU_KEYBOARD), str);

	instr_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
	if(instr_file == -1)
	{
		trace(info, "can't open %s\r\n", str);
	}
	else
	{
		yaffs_pwrite( instr_file, buffer, length, address);
		yaffs_close( instr_file);
	}
}

void save_soundsystem(uint8_t *buffer, uint32_t address, uint32_t length)
{
	char str[80];
	int32_t instr_file;

	instr_file = yaffs_open("/p0/sound/sound_system", O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);

	if(instr_file == -1)
	{
		trace(info, "can't open %s\r\n", str);
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_pwrite( instr_file, buffer, length, address);
		yaffs_close( instr_file);
#endif
	}
}

void erase_soundsystem(void)
{
	yaffs_unlink("/p0/sound/sound_system");
}

#if 0
void get_instrmap(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length)
{
	//int32_t i, j;
	int32_t instr_file = -1;

	if(keyboard) // keyboard R
	{
#ifndef YAFFS_OFF
		instr_file = yaffs_open("/p0/instruments/KR_map", O_RDWR, 0);
#endif
	}
	else
	{
#ifndef YAFFS_OFF
		instr_file = yaffs_open("/p0/instruments/KL_map", O_RDWR, 0);
#endif
	}

	if(instr_file == -1)
	{
		trace(info, "can't open mapping\r\n");
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_pread( instr_file, buffer, length, (instrument * INSTR_INFO_SIZE) + address);
		yaffs_close( instr_file);
#endif
	}
}

void save_instrmap(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length)
{
	int32_t instr_file = -1;
	uint32_t i;
	uint8_t *tmp_map;

	if(keyboard) // keyboard R
	{
#ifndef YAFFS_OFF
		instr_file = yaffs_open("/p0/instruments/KR_map", O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
		tmp_map = (uint8_t *)keyboardR_instr_map;
#endif
	}
	else
	{
#ifndef YAFFS_OFF
		instr_file = yaffs_open("/p0/instruments/KL_map", O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
		tmp_map = (uint8_t *)keyboardL_instr_map;
#endif
	}

	if(instr_file == -1)
	{
		trace(info, "can't open mapping\r\n");
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_read( instr_file, tmp_map, INSTR_INFO_SIZE * dt_config.nb_key);


		for(i=0; i<length; i++)
			tmp_map[(instrument * INSTR_INFO_SIZE) + address + i] = buffer[i];

		yaffs_write( instr_file, tmp_map, INSTR_INFO_SIZE * dt_config.nb_key);
		yaffs_close( instr_file);
#endif
	}
}

void erase_instrmap( uint32_t keyboard)
{
	if(keyboard) // keyboard R
	{
#ifndef YAFFS_OFF
		yaffs_unlink("/p0/instruments/KR_map");
#endif
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_unlink("/p0/instruments/KL_map");
#endif
	}
}
#endif
void init_preset(preset_instr *struct_preset, uint32_t octave, uint32_t preset_num)
{

	memset( (void *)struct_preset, 0, PRESET_STRUCT_SIZE);

	struct_preset->s_name[0] = '1' + preset_num;

	struct_preset->s_instr_octave = octave;

	struct_preset->s_volume = MAIN_VOLUME_DEFAULT;
	struct_preset->s_panning = MAIN_PANNING_DEFAULT;
	struct_preset->s_sendtorev = MAIN_SENDTOREV_DEFAULT;

	struct_preset->s_portamento_on_off = MAIN_PORT_ONOFF_DEFAULT;
	struct_preset->s_portamento_ctrl = MAIN_PORT_CTRL_DEFAULT;
	struct_preset->s_portamento_time = MAIN_PORT_TIME_DEFAULT;
	struct_preset->s_expression = MAIN_EXPRESSION_DEFAULT;
	struct_preset->s_pitch_bend_sensitivity = MAIN_PB_SENS_DEFAULT;
	struct_preset->s_pitch = 64;
	struct_preset->s_key_curve = KEYS_CURVES_DEFAULTVALUE;

	struct_preset->s_chorus_preset = 0;
	struct_preset->s_compressor_onoff = 0;
	struct_preset->s_delay_onoff = 0;
	struct_preset->s_distortion_onoff = 0;
	struct_preset->s_eq_onoff = 0;
	struct_preset->s_chorus_onoff = 0;
	struct_preset->s_reverb_onoff = 0;

	// Controlers
	//struct_preset->s_aftertouch_chainedfx = 0;
	//struct_preset->s_slider_L_chainedfx = 0;
	//struct_preset->s_slider_R_chainedfx = 0;
	//struct_preset->s_gyro_P_chainedfx = 0;
	//struct_preset->s_gyro_R_chainedfx = 0;
	//struct_preset->s_gyro_Y_chainedfx = 0;

	struct_preset->s_activ_aftertouch = 0;
	struct_preset->s_activ_slider_L = 0;
	struct_preset->s_activ_slider_R = 0;
	struct_preset->s_activ_gyro_P = 0;
	struct_preset->s_activ_gyro_R = 0;
	struct_preset->s_activ_gyro_Y = 0;

	struct_preset->s_direction_aftertouch = -1;
	struct_preset->s_direction_slider_L = -1;
	struct_preset->s_direction_slider_R = -1;
	struct_preset->s_direction_gyro_P = -1;
	struct_preset->s_direction_gyro_R = -1;
	struct_preset->s_direction_gyro_Y = -1;

	// multinote
	struct_preset->s_arpegiator_type = ARPEGGIATOR_1B;
	struct_preset->s_arpegiator_beat = 0;

	struct_preset->s_autopitch_rate = 0;
	struct_preset->s_autopitch_range = 127;
	struct_preset->s_tremolo_rate = 0;
	struct_preset->s_tremolo_range = 127;
	struct_preset->s_autopan_rate = 0;
	struct_preset->s_autopan_range = 127;
	struct_preset->s_autowah_rate = 0;
	struct_preset->s_autowah_range = 127;

	struct_preset->s_multinote_act = 0;
	struct_preset->s_multinote[0] = 0;
	struct_preset->s_multinote[1] = 0;
	struct_preset->s_multinote[2] = 0;
	struct_preset->s_multinote[3] = 0;

	struct_preset->s_adsr_attack = FX_ADSR_ATTACK_DEFAULTVALUE;
	struct_preset->s_adsr_release = FX_ADSR_RELEAS_DEFAULTVALUE;
	struct_preset->s_wah_type = FX_WAH_FILTERTYPE_DEFAULTVALUE;
	struct_preset->s_wah_freq = FX_WAH_FILTERFREQ_DEFAULTVALUE;
	struct_preset->s_wah_res = FX_WAH_FILTERRES_DEFAULTVALUE;

	memcpy(&(struct_preset->s_mix), &fx_mix_preset, FX_MIX_SIZE);
	memcpy(&(struct_preset->s_distortion), &(fx_distortion_preset[1]), FX_DIST_SIZE);
	memcpy(&(struct_preset->s_compressor), &(fx_compressor_preset[1]), FX_COMP_SIZE);
	memcpy(&(struct_preset->s_equalizer), &(fx_equalizer_preset[1]), FX_EQ_SIZE);
	memcpy(&(struct_preset->s_delay), &(fx_delay_preset[1]), FX_DELAY_SIZE);
	memcpy(&(struct_preset->s_chorus[0]), &(fx_chorus_preset[1]), FX_CHORUS_SIZE);
	memcpy(&(struct_preset->s_chorus[1]), &(fx_chorus_preset[2]), FX_CHORUS_SIZE);
	memcpy(&(struct_preset->s_chorus[2]), &(fx_chorus_preset[3]), FX_CHORUS_SIZE);

	struct_preset->s_displayled = 0;
	memset(struct_preset->s_leds, 0, NUM_LED_VALUE);

}

//void init_presetfromdream(preset_instr *struct_preset, uint32_t octave, uint32_t key, uint32_t presetnum)
//{
//	memcpy( (uint8_t *)struct_preset, &headerSNDBK[PRESET_STARTADRESS + (key * INSTRU_STRUCT_SIZE) + INSTR_INFO_SIZE + 4 + (PRESET_STRUCT_SIZE * presetnum)], PRESET_STRUCT_SIZE);
//	if (struct_preset->s_volume == 0xFF)
//	{
//		init_preset( struct_preset, octave, 0);
//	}
//
//	if(((info_instr *)&(headerSNDBK[PRESET_STARTADRESS + (key * INSTRU_STRUCT_SIZE)]))->SW_instr_version < DUSOUND_SW_VERSION) // check sw version
//	{
//		update_sw_version(struct_preset);
//	}
//
//}


void setinstrdistortion(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	uint32_t temp2;
	uint32_t drive, filter_res;
	double pregain;

	if(param->p_struct == INSTR_DISTORTION)
	{
		if(rec)
		{
			temp = controler_tab.c_param.c_instr;
			if (param->p_param == FX_DISTO_PREGAIN)
			{
				pregain = (double) instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion.d_pre_gain;
				instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion.d_postgain = (uint32_t) (0.010434723 * pregain * pregain -2.522869547 * pregain + 186.4921302);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion), FX_DISTO_POSTGAIN, 1,	0);
				//set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), FX_DISTO_PREGAIN, 1,	0);
#endif
			}
#if 0
			if (param->p_param == FX_DISTO_LPFILTERRES)
			{
				filter_res = instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_lowpassfilterres;
				if (filter_res == 0)
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 0;
				else
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 1;
				set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), FX_DISTO_ON_OFF, 1,	0);
			}
			if (param->p_param == FX_DISTO_LPFILTERFREQ)
			{
				filter_res = instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_lowpassfilterfreq;
				if (filter_res == 0)
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 0;
				else
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 1;
				set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), FX_DISTO_ON_OFF, 1,	0);
			}
#endif
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_disto(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion), param->p_param, 1,	0);
#endif
		}
		else
		{
			((uint8_t *)&temp_distortion)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_disto(&(temp_distortion), param->p_param, 1,	0);
#endif
		}

	}
	else if(param->p_struct == MUSIC_INSTR_DISTORTION)
	{
		temp = controler_tab.c_param.c_song;
		if(rec)
		{
			if (param->p_param == FX_DISTO_DRIVE)
			{
				drive = music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion.d_drive;
				if (drive == 0)
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion.d_postgain =  127;
				else if (drive == 7)
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion.d_postgain =  5;
				else
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion.d_postgain = drive * (-18) + 127;
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion), FX_DISTO_POSTGAIN, 1,	 music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#if 0
			if (param->p_param == FX_DISTO_LPFILTERRES)
			{
				filter_res = instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_lowpassfilterres;
				if (filter_res == 0)
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 0;
				else
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 1;
				set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), FX_DISTO_ON_OFF, 1,	0);
			}
			if (param->p_param == FX_DISTO_LPFILTERFREQ)
			{
				filter_res = instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_lowpassfilterfreq;
				if (filter_res == 0)
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 0;
				else
					instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_on_off = 1;
				set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), FX_DISTO_ON_OFF, 1,	0);
			}
#endif
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_disto(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
		else
		{
			((uint8_t *)&temp_distortion)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_disto(&(temp_distortion), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
	}
	else if(param->p_struct == INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_instr;

			if (value == 0)
			{//off
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_DISTORTION);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto((FX_distortion *)&(fx_distortion_preset[0]), FX_DISTO_ALL, 1,	0);
#endif
			}
			else
			{//on
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion), FX_DISTO_ALL, 1,	0);
#endif
			}
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_disto(&(temp_distortion), FX_DISTO_ALL, 1,	0);
#endif
		//}
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_song;

			if(value == 0)
			{// off
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto((FX_distortion *)&(fx_distortion_preset[0]), FX_DISTO_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
			else
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion), FX_DISTO_ALL, 1,	music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}

#if 0
			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_distortion[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion), FX_REVERB_SIZE);
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion), FX_DISTO_ALL, 1,	music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#endif
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_disto(&(temp_distortion), FX_DISTO_ALL, 1,	0);
#endif
		//}
	}
}

/******* fill an fx wah-wah structure with preset parameters ************/
void setinstrwah_type(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_HIGH, MIDI_FX_MXF);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_LOW, MIDI_FX_WAH_STARTADDRESS+0);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_HIGH, MIDI_FX_MXF);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_LOW, MIDI_FX_WAH_STARTADDRESS+0);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}
}

void setinstrwah_freq(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
#if CONFIG_MIDI_TASK_ENABLED == 1
		layer_play_CC( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_HIGH, MIDI_FX_MXF);
		layer_play_CC( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_LOW, MIDI_FX_WAH_STARTADDRESS+1);
		layer_play_CC( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_HIGH, MIDI_FX_MXF);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_LOW, MIDI_FX_WAH_STARTADDRESS+1);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}
}

void setinstrwah_res(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_HIGH, MIDI_FX_MXF);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_LOW, MIDI_FX_WAH_STARTADDRESS+2);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_HIGH, MIDI_FX_MXF);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_LOW, MIDI_FX_WAH_STARTADDRESS+2);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}
}




void setinstrcompressor(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	uint32_t temp2;

	if(param->p_struct == INSTR_COMPRESSOR)
	{
		if(rec)
		{
			temp = controler_tab.c_param.c_instr;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_compressor(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_compressor), param->p_param, 1, 0);
#endif
		}
		else
		{
			((uint8_t *)&temp_compressor)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_compressor(&(temp_compressor), param->p_param, 1, 0);
#endif
		}
	}
	else if(param->p_struct == MUSIC_INSTR_COMPRESSOR)
	{
		temp = controler_tab.c_param.c_song;
		if(rec)
		{
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_compressor(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_compressor), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
		else
		{
			((uint8_t *)&temp_compressor)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_compressor(&(temp_compressor), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
	}
	else if(param->p_struct == INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_instr;
			if(value == 0)
			{ // off
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_compressor((FX_compressor *)&(fx_compressor_preset[0]), FX_PEQ_ALL, 1, 0);
#endif
			}
			else
			{ // on
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_compressor(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_compressor), FX_PEQ_ALL, 1, 0);
#endif
			}
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_compressor(&(temp_compressor), FX_COMP_ALL, 1, 0);
#endif
		//}
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_song;

			if(value == 0)
			{ // off
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_compressor((FX_compressor *)&(fx_compressor_preset[0]), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
			else
			{ // on
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_compressor(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_compressor), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#if 0
			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_compressor[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_compressor_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_compressor), FX_COMP_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_compressor(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_compressor), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#endif
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_compressor(&(temp_compressor), FX_COMP_ALL, 1, 0);
#endif
		//}
	}
}

void set_equalizerpreset(FX_equalizer *fx_eq, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUM_FX_INTR)
		return;

	memcpy(fx_eq, &(fx_equalizer_preset[preset_ID]), FX_EQ_SIZE);
}

void set_equalizergenpreset(FX_equalizer_gen *fx_eq, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUM_FX_INTR)
		return;

	memcpy(fx_eq, &(fx_equalizer_preset[preset_ID]), FX_EQ_SIZE_GEN);
}

void setgeneraleq(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == GENERAL_EQ)
	{
		if(rec)
		{
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_main_equalizer(&(controler_tab.c_equalizer[controler_tab.c_param.c_equalizer_preset]), param->p_param, 1);
#endif
		}
		else
		{
			((uint8_t *)&temp_eqmain)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_main_equalizer(&(temp_eqmain), param->p_param, 1);
#endif
		}
	}
	else // PRESET
	{
		//if(rec)
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
			temp = controler_tab.c_param.c_instr;

			if (value == 0)
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), GENERAL_EQ);

			set_main_equalizer(&(controler_tab.c_equalizer[controler_tab.c_param.c_equalizer_preset]), FX_PEQ_ALL, 1);
#endif
		//}
		//else
		//{

#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_main_equalizer(&(temp_eqmain), FX_PEQ_ALL, 1);
#endif
		//}
	}
}

void setinstreq(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	uint32_t temp2;

	if(param->p_struct == INSTR_EQUALIZER)
	{
		if(rec)
		{
			temp = controler_tab.c_param.c_instr;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_equalizer(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_equalizer), param->p_param, 1, 0);
#endif
		}
		else
		{
			((uint8_t *)&temp_equalizer)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_equalizer(&(temp_equalizer), param->p_param, 1, 0);
#endif
		}
	}
	else if(param->p_struct == MUSIC_INSTR_EQUALIZER)
	{
		temp = controler_tab.c_param.c_song;
		if(rec)
		{
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_equalizer(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_equalizer), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
		else
		{
			((uint8_t *)&temp_equalizer)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_equalizer(&(temp_equalizer), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
	}
	else if(param->p_struct == INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_instr;

			if (value == 0)
			{
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_EQUALIZER);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_equalizer((FX_equalizer *)&(fx_equalizer_preset[0]), FX_PEQ_ALL, 1, 0);
#endif
			}
			else
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_equalizer(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_equalizer), FX_PEQ_ALL, 1, 0);
#endif
			}
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_equalizer(&(temp_equalizer), FX_PEQ_ALL, 1, 0);
#endif
		//}
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_song;

			if (value == 0)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_equalizer((FX_equalizer *)&(fx_equalizer_preset[0]), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
			else
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_equalizer(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_equalizer), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#if 0
			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_equalizer[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_eq_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_equalizer), FX_EQ_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_equalizer(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_equalizer), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#endif
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_equalizer(&(temp_equalizer), FX_PEQ_ALL, 1, 0);
#endif
		//}
	}
}

void setinstrchorus(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	uint32_t temp2;

	if(param->p_struct == INSTR_CHORUS)
	{
		if(rec)
		{
			temp = controler_tab.c_param.c_instr;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_chorus(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus_preset-1]), param->p_param, 1, 0);
#endif
		}
		else
		{
			((uint8_t *)&temp_chorus)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_chorus(&(temp_chorus), param->p_param, 1, 0);
#endif
		}
	}
	else if(param->p_struct == MUSIC_INSTR_CHORUS)
	{
		temp = controler_tab.c_param.c_song;
		if(rec)
		{
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_chorus(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_chorus[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_chorus_preset - 1]), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
		else
		{
			((uint8_t *)&temp_chorus)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_chorus(&(temp_chorus), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
	}
	else if(param->p_struct == INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_instr;

			if (value == 0)
			{
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_CHORUS);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_chorus((FX_chorus *)&(fx_chorus_preset[0]), FX_CHORUS_ALL, 1, 0);
#endif
			}
			else
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_chorus(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus_preset-1]), FX_CHORUS_ALL, 1, 0);
#endif
			}
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_chorus(&(temp_chorus), FX_CHORUS_ALL, 1, 0);
#endif
		//}
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_song;

			if (value == 0)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_chorus((FX_chorus *)&(fx_chorus_preset[0]), FX_CHORUS_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
			else
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_chorus(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_chorus[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_chorus_preset-1]), FX_CHORUS_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#if 0
			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_chorus[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_chorus_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_chorus), FX_CHORUS_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_chorus(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_chorus), FX_CHORUS_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#endif
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_chorus(&(temp_chorus), FX_CHORUS_ALL, 1, 0);
#endif
		//}
	}
}


void setinstrdelay(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	uint32_t temp2;
	uint32_t time, feedback;

	if (param->p_struct == INSTR_DELAY)
	{
		if(rec)
		{
			temp = controler_tab.c_param.c_instr;
			if (param->p_param == FX_DELAY_TIME)
			{
				time = instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay.d_time;
				feedback = instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay.d_feedback;

				if ((time == 0) || (feedback == 0))
					instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay.d_on_off = 0;
				else
					instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay.d_on_off = 1;

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay), FX_DELAY_ON_OFF, 1,	0);
#endif
			}
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_delay(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay), param->p_param, 1, 0);
#endif
		}
		else
		{
			((uint8_t *)&temp_delay)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_delay(&(temp_delay), param->p_param, 1, 0);
#endif
		}
	}
	else if(param->p_struct == MUSIC_INSTR_DELAY)
	{
		temp = controler_tab.c_param.c_song;
		if(rec)
		{
			if (param->p_param == FX_DELAY_TIME)
			{
				time = music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay.d_time;
				feedback = music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay.d_feedback;

				if ((time == 0) || (feedback == 0))
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay.d_on_off = 0;
				else
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay.d_on_off = 1;

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay), FX_DELAY_ON_OFF, 1,	 music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_delay(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
		else
		{
			((uint8_t *)&temp_delay)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_delay(&(temp_delay), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
	}
	else if(param->p_struct == INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_instr;

			if (value == 0)
			{ // off
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_DELAY);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay((FX_delay *)&(fx_delay_preset[0]), FX_DELAY_ALL, 1, 0);
#endif
			}
			else
			{ // on
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay), FX_DELAY_ALL, 1, 0);
#endif
			}
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_delay(&(temp_delay), FX_DELAY_ALL, 1, 0);
#endif
		//}
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		//if(rec)
		//{
			temp = controler_tab.c_param.c_song;

			if (value == 0)
			{ // off
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay((FX_delay *)&(fx_delay_preset[0]), FX_DELAY_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
			else
			{ // on
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay), FX_DELAY_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#if 0
			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_delay[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_delay), FX_DELAY_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_delay), FX_DELAY_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#endif
		//}
		//else
		//{
#if CONFIG_MIDI_TASK_ENABLED == 1
		//	set_fx_delay(&(temp_delay), FX_DELAY_ALL, 1, 0);
#endif
		//}
	}
}

/******* fill an fx reverb structure with preset parameters ************/
void set_reverbpreset(FX_reverb *fx_reverb, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUM_FX_INTR)
		return;

#if 1
	memcpy((fx_reverb), &(fx_reverb_preset[preset_ID]), FX_REVERB_SIZE);
#else
	for(i=0; i<NAME_CARACT; i++)
		fx_reverb->r_name[i] = fx_reverb_preset[preset_ID][FX_REVERB_NUM_PARAMETERS+FX_REVERB_NOP_PARAMETERS+i];

	fx_reverb->r_level = fx_reverb_preset[preset_ID][0];
	fx_reverb->r_directlevel = fx_reverb_preset[preset_ID][1];
	fx_reverb->r_revsend = fx_reverb_preset[preset_ID][2];
	fx_reverb->r_tonegain = fx_reverb_preset[preset_ID][3];
	fx_reverb->r_tonefreq = fx_reverb_preset[preset_ID][4];
	fx_reverb->r_prehp = fx_reverb_preset[preset_ID][5];
	fx_reverb->r_time = fx_reverb_preset[preset_ID][6];
	fx_reverb->r_echofeedback = fx_reverb_preset[preset_ID][7];
	fx_reverb->r_hdamp = fx_reverb_preset[preset_ID][8];
	fx_reverb->r_thresgate = fx_reverb_preset[preset_ID][9];
	fx_reverb->r_predelaytime = fx_reverb_preset[preset_ID][10];
#endif
}

void setgeneralreverb(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t level;
#if 1
	if(param->p_struct == MUSIC_REVERB)
	{
		if(rec)
		{
			if (param->p_param == FX_REVERB_REVSEND)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				level = music_tab[controler_tab.c_param.c_song].s_reverb.r_revsend;

				if (level < 128) {
					music_tab[controler_tab.c_param.c_song].s_reverb.r_directlevel = 127;
					music_tab[controler_tab.c_param.c_song].s_reverb.r_level = (uint8_t) (level);
				}
				else {
					music_tab[controler_tab.c_param.c_song].s_reverb.r_directlevel = (uint8_t) (255 - level);
					music_tab[controler_tab.c_param.c_song].s_reverb.r_level = 127;
				}

				set_main_reverb(&(music_tab[controler_tab.c_param.c_song].s_reverb), FX_REVERB_DIRECTLEVEL, 1);
				set_main_reverb(&(music_tab[controler_tab.c_param.c_song].s_reverb), FX_REVERB_REVSEND, 1);
				set_main_reverb(&(music_tab[controler_tab.c_param.c_song].s_reverb), FX_REVERB_LEVEL, 1);
#endif
			}
			else //if (param->p_param != FX_REVERB_LEVEL)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_main_reverb(&(music_tab[controler_tab.c_param.c_song].s_reverb), param->p_param, 1);
#endif
			}
			mem_cpy( (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_reverb), (uint8_t *)&(controler_tab.c_reverb[music_tab[controler_tab.c_param.c_song].s_reverb_preset]), FX_REVERB_SIZE);
		}
		else
		{
			if (param->p_param == FX_REVERB_LEVEL)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				level = value;
				temp_reverb.r_level = value;

				if (level < 128) {
					temp_reverb.r_directlevel = 127;
					temp_reverb.r_revsend = (uint8_t) (level);
				}
				else {
					temp_reverb.r_directlevel = (uint8_t) (FX_REVERB_LEVEL_MAXVALUE - level);
					temp_reverb.r_revsend = 127;
				}

				set_main_reverb(&(temp_reverb), FX_REVERB_DIRECTLEVEL, 1);
				set_main_reverb(&(temp_reverb), FX_REVERB_REVSEND, 1);
				set_main_reverb(&(temp_reverb), FX_REVERB_LEVEL, 1);
#endif
			}
			else //if (param->p_param != FX_REVERB_LEVEL)
			{
				((uint8_t *)&temp_reverb)[param->p_param] = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_main_reverb(&(temp_reverb), param->p_param, 1);
#endif
			}
		}
	}
	else
	{
		//if(rec)
		//{
			mem_cpy((uint8_t *)&(controler_tab.c_reverb[music_tab[controler_tab.c_param.c_song].s_reverb_preset]), (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_reverb), FX_REVERB_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
			level = music_tab[controler_tab.c_param.c_song].s_reverb.r_level;
			if (level < 128) {
				music_tab[controler_tab.c_param.c_song].s_reverb.r_directlevel = 127;
				music_tab[controler_tab.c_param.c_song].s_reverb.r_revsend = (uint8_t) (level);
			}
			else {
				music_tab[controler_tab.c_param.c_song].s_reverb.r_directlevel = (uint8_t) (FX_REVERB_DRYWET_MAXVALUE - level);
				music_tab[controler_tab.c_param.c_song].s_reverb.r_revsend = 127;
			}

			set_main_reverb(&(music_tab[controler_tab.c_param.c_song].s_reverb), FX_REVERB_ALL, 1);
#endif
		/*}
		else
		{
			mem_cpy((uint8_t *)&(controler_tab.c_reverb[music_tab[controler_tab.c_param.c_song].s_reverb_preset]), (uint8_t *)&(temp_reverb), FX_REVERB_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1

			level = temp_reverb.r_level;

			if (level < 128) {
				temp_reverb.r_directlevel = 127;
				temp_reverb.r_revsend = (uint8_t) (level);
			}
			else {
				temp_reverb.r_directlevel = (uint8_t) (FX_REVERB_DRYWET_MAXVALUE - level);
				temp_reverb.r_revsend = 127;
			}

			set_main_reverb(&(temp_reverb), FX_REVERB_ALL, 1);
#endif
		}*/
	}
#else
	if(param->p_struct == GENERAL_REVERB)
	{
		if(rec)
		{
			if (param->p_param == FX_REVERB_REVSEND)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				level = controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_revsend;

				if (level < 128) {
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_directlevel = 127;
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_level = (uint8_t) (level);
				}
				else {
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_directlevel = (uint8_t) (255 - level);
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_level = 127;
				}

				set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), FX_REVERB_DIRECTLEVEL, 1);
				set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), FX_REVERB_REVSEND, 1);
				set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), FX_REVERB_LEVEL, 1);
#endif
			}
			else //if (param->p_param != FX_REVERB_LEVEL)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), param->p_param, 1);
#endif
			}

		}
		else
		{
			if (param->p_param == FX_REVERB_LEVEL)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				level = controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_level;

				if (level < 128) {
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_directlevel = 127;
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_revsend = (uint8_t) (level);
				}
				else {
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_directlevel = (uint8_t) (FX_REVERB_LEVEL_MAXVALUE - level);
					controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_revsend = 127;
				}

				set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), FX_REVERB_DIRECTLEVEL, 1);
				set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), FX_REVERB_REVSEND, 1);
				set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), FX_REVERB_LEVEL, 1);
#endif
			}
			else //if (param->p_param != FX_REVERB_LEVEL)
			{
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_main_reverb(&(temp_reverb), param->p_param, 1);
#endif
			}
		}
	}
	else
	{
		if(rec)
		{
#if CONFIG_MIDI_TASK_ENABLED == 1
			level = controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_level;
			if (level < 128) {
				controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_directlevel = 127;
				controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_revsend = (uint8_t) (level);
			}
			else {
				controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_directlevel = (uint8_t) (FX_REVERB_DRYWET_MAXVALUE - level);
				controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset].r_revsend = 127;
			}

			temp = controler_tab.c_param.c_instr;
			if (value == 0)
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), GENERAL_REVERB);

			set_main_reverb(&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]), FX_REVERB_ALL, 1);
#endif
		}
		else
		{
#if CONFIG_MIDI_TASK_ENABLED == 1
			level = temp_reverb.r_level;

			if (level < 128) {
				temp_reverb.r_directlevel = 127;
				temp_reverb.r_revsend = (uint8_t) (level);
			}
			else {
				temp_reverb.r_directlevel = (uint8_t) (FX_REVERB_DRYWET_MAXVALUE - level);
				temp_reverb.r_revsend = 127;
			}

			set_main_reverb(&(temp_reverb), FX_REVERB_ALL, 1);
#endif
		}
	}
#endif
}

/******* fill an fx adsr structure with preset parameters ************/
void setinstradsr_attack(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_ENV_ATTACK, value);
#endif
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_CTRL_ENV_ATTACK, value);
#endif
	}
}

void setinstradsr_release(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_ENV_RELEASE, value);
#endif
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_CTRL_ENV_RELEASE, value);
#endif
	}
}

/******************** parameters***********************/

/**    set instrument volume **/
void setinstrvolume(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
#if CONFIG_MIDI_TASK_ENABLED == 1
	#if INPUT_GAIN == 0
		layer_play_CC( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_VOLUME, value);
		//midiTask_add_cmd( MIDI_CONTROL_CHANGE, MAX_MIDI_CANAL, MIDI_CTRL_VOLUME, value);
	#else
		set_fx_mix(&(instr_tab[controler_tab.c_param.c_instr].s_mix), FX_MIX_INPUTGAIN, 1, 0);
		set_fx_mix(&(instr_tab[controler_tab.c_param.c_instr].s_mix), FX_MIX_INPUTGAIN, 1, MAX_MIDI_CANAL);
		//set_fx_mix(&(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_volume), FX_MIX_INPUTGAIN, 1, 0);
		//set_fx_mix(&(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_volume), FX_MIX_INPUTGAIN, 1, MAX_MIDI_CANAL);
	#endif
#endif
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_CTRL_VOLUME, value);
		//midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel + MAX_MIDI_CANAL, MIDI_CTRL_VOLUME, value);
#endif
	}
}

/**    set instrument octave **/
void setinstroctave(param_struct *param, int32_t value, uint32_t rec)
{
	//set_panic(0);
	//add_cmd_usbmidi( MIDI_CONTROL_CHANGE, music_tab[controler_tab.c_param.c_song].s_track[music_ctrack()].t_midichannel, 0x78, 0);
	//add_cmd_usbmidi( MIDI_CONTROL_CHANGE, music_tab[controler_tab.c_param.c_song].s_track[music_ctrack()].t_midichannel, 0x7B, 0);
	midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[controler_tab.c_param.c_song].s_track[music_ctrack()].t_midichannel, 0x78, 0);
}

/**    set instrument panning **/
void setinstrpanning(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
		if(rec)
		{
			temp = controler_tab.c_param.c_instr;
			instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_mix.m_outputpanning = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_mix), FX_MIX_OUTPUTPANNING, 1, 0);
#endif
		}
		else
		{
			temp_mix.m_outputpanning = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(temp_mix), FX_MIX_OUTPUTPANNING, 1, 0);

#endif
		}
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;

		if(rec)
		{
			music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_mix.m_outputpanning = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_mix), FX_MIX_OUTPUTPANNING, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
		else
		{
			temp_mix.m_outputpanning = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(temp_mix), FX_MIX_OUTPUTPANNING, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
	}
}

/**    set instrument send to reverb **/
void setinstrsendtorev(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
		if(rec)
		{
			temp = controler_tab.c_param.c_instr;
			instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_mix.m_sendtoreverb = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_mix), FX_MIX_SENDTOREVERB, 1, 0);
#endif
		}
		else
		{
			temp_mix.m_sendtoreverb = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(temp_mix), FX_MIX_SENDTOREVERB, 1, 0);
#endif
		}
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;

		if(rec)
		{
			music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_mix.m_sendtoreverb = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_mix), FX_MIX_SENDTOREVERB, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
		else
		{
			temp_mix.m_sendtoreverb = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(temp_mix), FX_MIX_SENDTOREVERB, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
		}
	}
}

void setinstrpitchbendsens(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	if(param->p_struct == INSTR_PRESET)
	{
		if(value>1) // for 1/2
			value--;

#if CONFIG_MIDI_TASK_ENABLED == 1
		layer_play_midi( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_RPN_HIGH, 0x00);
		layer_play_midi( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_RPN_LOW, MIDI_CTRL_PB_SENS);
		layer_play_midi( MIDI_CONTROL_CHANGE, 0, MIDI_ADDRESS_NRPN_DATA, value);

		//layer_play_midi( MIDI_CONTROL_CHANGE, MAX_MIDI_CANAL, MIDI_ADDRESS_RPN_HIGH, 0x00);
		//layer_play_midi( MIDI_CONTROL_CHANGE, MAX_MIDI_CANAL, MIDI_ADDRESS_RPN_LOW, MIDI_CTRL_PB_SENS);
		//layer_play_midi( MIDI_CONTROL_CHANGE, MAX_MIDI_CANAL, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		if(value>1) // for 1/2
			value--;

		temp = controler_tab.c_param.c_song;
#if CONFIG_MIDI_TASK_ENABLED == 1
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_RPN_HIGH, 0x00);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_RPN_LOW, MIDI_CTRL_PB_SENS);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel, MIDI_ADDRESS_NRPN_DATA, value);

		//midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel + MAX_MIDI_CANAL, MIDI_ADDRESS_RPN_HIGH, 0x00);
		//midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel + MAX_MIDI_CANAL, MIDI_ADDRESS_RPN_LOW, MIDI_CTRL_PB_SENS);
		//midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel + MAX_MIDI_CANAL, MIDI_ADDRESS_NRPN_DATA, value);
#endif
	}


}

void setinstrpitchbend(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
		if(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_pitch_bend_sensitivity == 1)
		{
			value = (64 * value) / 127 + 32;
		}

		layer_play_CC( MIDI_PITCHBEND, 0, (value&0x7F), (value&0x7F));
		//layer_play_midi( MIDI_PITCHBEND, MAX_MIDI_CANAL, (value&0x7F), (value&0x7F));
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;

		if(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_pitch_bend_sensitivity == 1)
		{
			value = (64 * value) / 127 + 32;
		}

		midiTask_add_cmd( MIDI_PITCHBEND, music_tab[temp].s_track[music_ctrack()].t_midichannel, (value&0x7F), (value&0x7F));
		//midiTask_add_cmd( MIDI_PITCHBEND, music_tab[temp].s_track[music_ctrack()].t_midichannel + MAX_MIDI_CANAL, (value&0x7F), (value&0x7F));
	}
}

/**    set instrument portamento time **/
void setinstrporttime(param_struct *param, int32_t value, uint32_t rec)
{
#if CONFIG_MIDI_TASK_ENABLED == 1
	if(value>1)
	{
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_PORT_TIME, value - 1);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_PORT_ON_OFF, 1);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_POLY, 0x00);
	}
	else if(value==1)
	{
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_PORT_TIME, 0);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_PORT_ON_OFF, 0);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_MONO, 0x00);
	}
	else
	{
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_PORT_TIME, 0);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_PORT_ON_OFF, 0);
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_POLY, 0x00);
	}
#endif

	//layer_play_midi( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_PORT_TIME, value);
	//layer_play_midi( MIDI_CONTROL_CHANGE, MAX_MIDI_CANAL, MIDI_CTRL_PORT_TIME, value);
}

/**    set instrument expression **/
void setinstrexpression(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	if(param->p_struct == INSTR_PRESET)
	{
		layer_play_CC( MIDI_CONTROL_CHANGE, 0, MIDI_CTRL_EXPRESSION, value);
		//layer_play_midi( MIDI_CONTROL_CHANGE, MAX_MIDI_CANAL, MIDI_CTRL_EXPRESSION, value);
	}
	else if(param->p_struct == MUSIC_INSTR_PRESET)
	{
		temp = controler_tab.c_param.c_song;

		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_ctrack()].t_midichannel, MIDI_CTRL_EXPRESSION, value);
		//midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[temp].s_track[music_ctrack()].t_midichannel + MAX_MIDI_CANAL, MIDI_CTRL_EXPRESSION, value);
	}

}


/* set instrument relativ volume */
void setinstrrelvolume(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;

	temp = controler_tab.c_param.c_instr;
	instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_mix.m_inputgain = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
	set_fx_mix(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_mix), FX_MIX_INPUTGAIN, 1, 0);
#endif

}

void switch_off_ctrl_with_preset_off(preset_instr *preset, uint32_t struct_p)
{
#if 0
	if ((preset->s_activ_aftertouch != 0) && ((preset->s_aftertouch_chainedfx)->p_struct == struct_p))
		preset->s_activ_aftertouch = 0;
	if ((preset->s_activ_slider_L != 0) && ((preset->s_slider_L_chainedfx)->p_struct == struct_p))
		preset->s_activ_slider_L = 0;
	if ((preset->s_activ_slider_R != 0) && ((preset->s_slider_R_chainedfx)->p_struct == struct_p))
		preset->s_activ_slider_R = 0;
	if ((preset->s_activ_gyro_P != 0) && ((preset->s_gyro_P_chainedfx)->p_struct == struct_p))
       preset->s_activ_gyro_P = 0;
	if ((preset->s_activ_gyro_R != 0) && ((preset->s_gyro_R_chainedfx)->p_struct == struct_p))
		preset->s_activ_gyro_R = 0;
	if ((preset->s_activ_gyro_Y != 0) && ((preset->s_gyro_Y_chainedfx)->p_struct == struct_p))
		preset->s_activ_gyro_Y = 0;
#endif
}

void init_dream_sp(dream_sp* sp_struct)
{
    sp_struct->align1 = 0x0000;
   // sp_struct->address1;
    //sp_struct->loopType;
    //sp_struct->address2;
    //sp_struct->address3;

    sp_struct->unknown1 = 0xB5F0;
    sp_struct->unknown2 = 0x0814;
    sp_struct->unknown3 = 0xFF00;
    sp_struct->unknown4 = 0x1032;
    sp_struct->unknown5 = 0x04;

    //sp_struct->volume_amplifier;

    sp_struct->unknown6 = 0x0081;

    //sp_struct->fine_tune;
//    sp_struct->unity_note;
    //sp_struct->coarse_tune;

    sp_struct->align2 = 0x0000;

    //sp_struct->loop_start;

    sp_struct->align3 = 0x0000;

    //sp_struct->wav_address;

    //sp_struct->volume_mixer1;

    //sp_struct->loop_end;

    sp_struct->unknown7 = 0x7F0E;
    sp_struct->unknown8 = 0x06;

    //sp_struct->amplitude_osc_amp;
    //sp_struct->volume_mixer2;

    sp_struct->init = 0x7F00;
    sp_struct->attack = 0x1FE3;
    sp_struct->decay = 0x5FE3;
    sp_struct->release = 0x605A;
    sp_struct->unknown13 = 0x0401;
    sp_struct->unknown14 = 0x0F00;
    sp_struct->unknown15 = 0x0015;
    sp_struct->unknown16 = 0x0403;
    sp_struct->unknown17 = 0x0010;
    sp_struct->unknown18 = 0x0201;

    sp_struct->unknown19 = 0x00000000;
    sp_struct->unknown20 = 0x00000000;
    sp_struct->unknown21 = 0x0000;

    sp_struct->unknown22 = 0x0002;
    sp_struct->size_wav = 0x00000000;

}

