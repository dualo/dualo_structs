/*
 * sound_parameters_mng.c
 *
 *  Created on: 07/08/2013
 *      Author: mma
 */

// includes
#include "sound_parameters_mng.h"
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
#include "eeprom/eeprom_map.h"
#include "dutouch_config.h"

// external parameters
extern uint8_t fx_distortion_preset[FX_NUMPRESET][FX_DISTO_NUM_PARAMETERS+FX_DISTO_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_wah_preset[FX_NUMPRESET][FX_WAH_NUM_PARAMETERS+FX_WAH_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_compressor_preset[FX_NUMPRESET][FX_COMP_NUM_PARAMETERS+FX_COMP_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_mix_preset[FX_NUMPRESET][FX_MIX_NUM_PARAMETERS+FX_MIX_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_equalizer_preset[FX_NUMPRESET][FX_PEQ_NUM_PARAMETERS+FX_PEQ_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_instr_equalizer_preset[FX_NUMPRESET][FX_PEQ_NUM_PARAMETERS+FX_PEQ_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_chorus_preset[FX_NUMPRESET][FX_CHORUS_NUM_PARAMETERS+FX_CHORUS_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_delay_preset[FX_NUMPRESET][FX_DELAY_NUM_PARAMETERS+FX_DELAY_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_adsr_preset[FX_NUMPRESET][FX_ADSR_NUM_PARAMETERS+FX_ADSR_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_vibrato_preset[FX_NUMPRESET][FX_VIB_NUM_PARAMETERS+FX_VIB_NOP_PARAMETERS+FX_NUM_NAME];
extern uint8_t fx_reverb_preset[FX_NUMPRESET][FX_REVERB_NUM_PARAMETERS+FX_REVERB_NOP_PARAMETERS+FX_NUM_NAME];

extern s_instr keyboardL_instr_map[];
extern s_instr keyboardR_instr_map[];
extern s_instr keyboardL_instr_map_default[];
extern s_instr keyboardR_instr_map_default[];

extern struct_controler controler_tab;
extern struct_instr instr_tab[];
extern music_song music_tab[];

uint32_t quart_pitch = 0;
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

/******* fx struct initialisation ************/
void init_instrmap(void)
{
	uint32_t i, j;
	int32_t instr_file_L = -1;
	int32_t instr_file_R = -1;
	uint32_t ret;
#ifndef YAFFS_OFF
	instr_file_R = yaffs_open("/p0/instruments/KR_map", O_RDWR, 0);
	instr_file_L = yaffs_open("/p0/instruments/KL_map", O_RDWR, 0);
#endif
	if(instr_file_L == -1)
	{
		for(i=0; i<dt_config.nb_key; i++)
		{
			for(j = 0; j<NAME_CARACT; j++)
			{
				keyboardL_instr_map[i].instr_name[j] = keyboardL_instr_map_default[i].instr_name[j];
				keyboardL_instr_map[i].instr_cat[j] = keyboardL_instr_map_default[i].instr_cat[j];
			}
			keyboardL_instr_map[i].instr_midi_pc  = keyboardL_instr_map_default[i].instr_midi_pc;
			keyboardL_instr_map[i].instr_midi_C0  = keyboardL_instr_map_default[i].instr_midi_C0;
			keyboardL_instr_map[i].instr_key_map = keyboardL_instr_map_default[i].instr_key_map;
			keyboardL_instr_map[i].instr_octave = keyboardL_instr_map_default[i].instr_octave;
			keyboardL_instr_map[i].instr_id = keyboardL_instr_map_default[i].instr_id;
			keyboardL_instr_map[i].instr_size = keyboardL_instr_map_default[i].instr_size;
			keyboardL_instr_map[i].instr_address = keyboardL_instr_map_default[i].instr_address;
			keyboardL_instr_map[i].instr_noteoff = keyboardL_instr_map_default[i].instr_noteoff;
			keyboardL_instr_map[i].instr_relvolume = keyboardL_instr_map_default[i].instr_relvolume;
			keyboardL_instr_map[i].instr_type = keyboardL_instr_map_default[i].instr_type;
		}
		//num_instruments_L = dt_config.nb_key;
	}
	else
	{
#ifndef YAFFS_OFF
		ret = yaffs_read(instr_file_L,(uint8_t *)keyboardL_instr_map, INSTR_INFO_SIZE * dt_config.nb_key);
		yaffs_close(instr_file_L);
#endif

		//num_instruments_L = ret / INSTR_INFO_SIZE;
	}

	if(instr_file_R == -1)
	{
		for(i=0; i<dt_config.nb_key; i++)
		{
			for(j = 0; j<NAME_CARACT; j++)
			{
				keyboardR_instr_map[i].instr_name[j] = keyboardR_instr_map_default[i].instr_name[j];
				keyboardR_instr_map[i].instr_cat[j] = keyboardR_instr_map_default[i].instr_cat[j];
			}
			keyboardR_instr_map[i].instr_midi_pc  = keyboardR_instr_map_default[i].instr_midi_pc;
			keyboardR_instr_map[i].instr_midi_C0  = keyboardR_instr_map_default[i].instr_midi_C0;
			keyboardR_instr_map[i].instr_key_map = keyboardR_instr_map_default[i].instr_key_map;
			keyboardR_instr_map[i].instr_octave = keyboardR_instr_map_default[i].instr_octave;
			keyboardR_instr_map[i].instr_id = keyboardR_instr_map_default[i].instr_id;
			keyboardR_instr_map[i].instr_size = keyboardR_instr_map_default[i].instr_size;
			keyboardR_instr_map[i].instr_address = keyboardR_instr_map_default[i].instr_address;
			keyboardR_instr_map[i].instr_noteoff = keyboardR_instr_map_default[i].instr_noteoff;
			keyboardR_instr_map[i].instr_relvolume = keyboardR_instr_map_default[i].instr_relvolume;
			keyboardR_instr_map[i].instr_type = keyboardR_instr_map_default[i].instr_type;
		}
		//num_instruments_R = dt_config.nb_key;
	}
	else
	{
#ifndef YAFFS_OFF
		ret = yaffs_read(instr_file_R,(uint8_t *)keyboardR_instr_map, INSTR_INFO_SIZE * dt_config.nb_key);
		yaffs_close(instr_file_R);
#endif

		//num_instruments_R = ret / INSTR_INFO_SIZE;
	}
}


/******* fx struct parameters initialisation ************/
void migrate_sound(struct_instr *sound_struct)
{
	uint32_t version = 0xFFFFFFFF;
	uint32_t i;
#ifndef EEPROM_OFF
	EEPROM_Read8(eeprom_map[EE_INFO_LASTFIRMWARE].addr,(void *)&version, eeprom_map[EE_INFO_LASTFIRMWARE].size);
#endif

	if(version<0x0002000E) // V2.0.14
	{
		set_instrequalizerpreset(&(sound_struct->s_equalizer[0]), 0); // for getting "off" displayed

		for(i = 0; i<FX_NUM_PRESET_INTR; i++)
		{
			sound_struct->s_preset[i].s_adsr_attack = FX_ADSR_ATTACK_DEFAULTVALUE;
			sound_struct->s_preset[i].s_adsr_release = FX_ADSR_RELEAS_DEFAULTVALUE;
			sound_struct->s_preset[i].s_wah_type = FX_WAH_FILTERTYPE_DEFAULTVALUE;
			sound_struct->s_preset[i].s_wah_freq = FX_WAH_FILTERFREQ_DEFAULTVALUE;
			sound_struct->s_preset[i].s_wah_res = FX_WAH_FILTERRES_DEFAULTVALUE;
		}
	}

	if(version<0x00020010) // V2.0.14
	{
		for(i = 0; i<FX_NUM_PRESET_INTR; i++)
			sound_struct->s_preset[i].s_reverb_onoff = 0;
	}

	if(version<0x00020017) // V2.0.23
	{ // invert filter freq
		for(i = 0; i<FX_NUM_PRESET_INTR; i++)
			sound_struct->s_preset[i].s_wah_freq = 127 - sound_struct->s_preset[i].s_wah_freq;
	}
}

void init_sound(struct_instr *sound_struct, uint32_t instr)
{
	int32_t j;
	char str[80];
	int32_t sound_file = -1;

	//trace( info, "init sound %d\r\n", instr);

	strcpy(str, "/p0/sound/");

	if(instr<dt_config.nb_key)
		strcat(str, (char *)keyboardL_instr_map[instr].instr_name);
	else
		strcat(str, (char *)keyboardR_instr_map[instr - dt_config.nb_key].instr_name);
#ifndef YAFFS_OFF
	sound_file = yaffs_open(str, O_RDWR, 0);
#endif

	if(sound_file == -1)
	{
		if(instr<dt_config.nb_key)
		{
			for(j = 0; j<NAME_CARACT; j++)
			{
				sound_struct->s_instrument.instr_name[j] = keyboardL_instr_map[instr].instr_name[j];
				sound_struct->s_instrument.instr_cat[j] = keyboardL_instr_map[instr].instr_cat[j];
			}
			sound_struct->s_instrument.instr_midi_pc  = keyboardL_instr_map[instr].instr_midi_pc;
			sound_struct->s_instrument.instr_midi_C0  = keyboardL_instr_map[instr].instr_midi_C0;
			sound_struct->s_instrument.instr_key_map = keyboardL_instr_map[instr].instr_key_map;
			sound_struct->s_instrument.instr_octave = keyboardL_instr_map[instr].instr_octave;
			sound_struct->s_instrument.instr_id = keyboardL_instr_map[instr].instr_id;
			sound_struct->s_instrument.instr_noteoff = keyboardL_instr_map[instr].instr_noteoff;
			sound_struct->s_instrument.instr_relvolume = keyboardL_instr_map[instr].instr_relvolume;
			sound_struct->s_instrument.instr_type = keyboardL_instr_map[instr].instr_type;
		}
		else
		{
			for(j = 0; j<NAME_CARACT; j++)
			{
				sound_struct->s_instrument.instr_name[j] = keyboardR_instr_map[instr-dt_config.nb_key].instr_name[j];
				sound_struct->s_instrument.instr_cat[j] = keyboardR_instr_map[instr-dt_config.nb_key].instr_cat[j];
			}
			sound_struct->s_instrument.instr_midi_pc  = keyboardR_instr_map[instr-dt_config.nb_key].instr_midi_pc;
			sound_struct->s_instrument.instr_midi_C0  = keyboardR_instr_map[instr-dt_config.nb_key].instr_midi_C0;
			sound_struct->s_instrument.instr_key_map = keyboardR_instr_map[instr-dt_config.nb_key].instr_key_map;
			sound_struct->s_instrument.instr_octave = keyboardR_instr_map[instr-dt_config.nb_key].instr_octave;
			sound_struct->s_instrument.instr_id = keyboardR_instr_map[instr-dt_config.nb_key].instr_id;
			sound_struct->s_instrument.instr_noteoff = keyboardR_instr_map[instr-dt_config.nb_key].instr_noteoff;
			sound_struct->s_instrument.instr_relvolume = keyboardR_instr_map[instr-dt_config.nb_key].instr_relvolume;
			sound_struct->s_instrument.instr_type = keyboardR_instr_map[instr-dt_config.nb_key].instr_type;
		}

		if((sound_struct->s_instrument.instr_key_map)&&(sound_struct->s_instrument.instr_type == INSTR_HARMONIC))
			sound_struct->s_instrument.instr_type = INSTR_PERCU;
		else
			sound_struct->s_instrument.instr_type = INSTR_HARMONIC;

		sound_struct->s_presetnum = 0;
		sound_struct->s_displayled = 0;

		for(j = 0; j<FX_NUM_PRESET_INTR; j++)
		{
			 init_preset(&(sound_struct->s_preset[j]), sound_struct->s_instrument.instr_octave);
		}

		set_mixpreset(&(sound_struct->s_mix), 0);

		for(j = 0; j<FX_NUM_FX_INTR; j++)
		{
			set_distortionpreset(&(sound_struct->s_distortion[j]), j);
			set_compressorpreset(&(sound_struct->s_compressor[j]), j);
			set_instrequalizerpreset(&(sound_struct->s_equalizer[j]), j);
			set_choruspreset(&(sound_struct->s_chorus[j]), j);
			set_delaypreset(&(sound_struct->s_delay[j]), j);
		}

		for(j = 0; j<NUM_LED_VALUE; j++)
		{
			sound_struct->s_leds[j] = 0;
		}

		save_sound( sound_struct, instr);
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_read(sound_file,(uint8_t *)sound_struct, INSTRU_STRUCT_SIZE);
		yaffs_close(sound_file);
#endif

		if(instr<dt_config.nb_key)
		{
			for(j = 0; j<NAME_CARACT; j++)
			{
				sound_struct->s_instrument.instr_name[j] = keyboardL_instr_map[instr].instr_name[j];
				sound_struct->s_instrument.instr_cat[j] = keyboardL_instr_map[instr].instr_cat[j];
			}
			sound_struct->s_instrument.instr_midi_pc  = keyboardL_instr_map[instr].instr_midi_pc;
			sound_struct->s_instrument.instr_midi_C0  = keyboardL_instr_map[instr].instr_midi_C0;
			sound_struct->s_instrument.instr_key_map = keyboardL_instr_map[instr].instr_key_map;
			sound_struct->s_instrument.instr_octave = keyboardL_instr_map[instr].instr_octave;
			sound_struct->s_instrument.instr_id = keyboardL_instr_map[instr].instr_id;
			sound_struct->s_instrument.instr_noteoff = keyboardL_instr_map[instr].instr_noteoff;
			sound_struct->s_instrument.instr_relvolume = keyboardL_instr_map[instr].instr_relvolume;
			sound_struct->s_instrument.instr_type = keyboardL_instr_map[instr].instr_type;

			if((sound_struct->s_instrument.instr_key_map)&&(sound_struct->s_instrument.instr_type == INSTR_HARMONIC))
				sound_struct->s_instrument.instr_type = INSTR_PERCU;
			else
				sound_struct->s_instrument.instr_type = INSTR_HARMONIC;

			if(sound_struct->s_instrument.instr_relvolume<40)
				sound_struct->s_instrument.instr_relvolume = 0x7F;
		}
		else
		{
			for(j = 0; j<NAME_CARACT; j++)
			{
				sound_struct->s_instrument.instr_name[j] = keyboardR_instr_map[instr-dt_config.nb_key].instr_name[j];
				sound_struct->s_instrument.instr_cat[j] = keyboardR_instr_map[instr-dt_config.nb_key].instr_cat[j];
			}
			sound_struct->s_instrument.instr_midi_pc  = keyboardR_instr_map[instr-dt_config.nb_key].instr_midi_pc;
			sound_struct->s_instrument.instr_midi_C0  = keyboardR_instr_map[instr-dt_config.nb_key].instr_midi_C0;
			sound_struct->s_instrument.instr_key_map = keyboardR_instr_map[instr-dt_config.nb_key].instr_key_map;
			sound_struct->s_instrument.instr_octave = keyboardR_instr_map[instr-dt_config.nb_key].instr_octave;
			sound_struct->s_instrument.instr_id = keyboardR_instr_map[instr-dt_config.nb_key].instr_id;
			sound_struct->s_instrument.instr_noteoff = keyboardR_instr_map[instr-dt_config.nb_key].instr_noteoff;
			sound_struct->s_instrument.instr_relvolume = keyboardR_instr_map[instr-dt_config.nb_key].instr_relvolume;
			sound_struct->s_instrument.instr_type = keyboardR_instr_map[instr-dt_config.nb_key].instr_type;

			if((sound_struct->s_instrument.instr_key_map)&&(sound_struct->s_instrument.instr_type == INSTR_HARMONIC))
				sound_struct->s_instrument.instr_type = INSTR_PERCU;
			else
				sound_struct->s_instrument.instr_type = INSTR_HARMONIC;

			if(sound_struct->s_instrument.instr_relvolume<40)
				sound_struct->s_instrument.instr_relvolume = 0x7F;
		}

		migrate_sound(sound_struct);

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
		}

		if(sound_struct->s_presetnum >= FX_NUM_PRESET_INTR)
		{
			sound_struct->s_presetnum = 0;
		}
	}

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

void save_sound(struct_instr *sound_struct, uint32_t instr)
{
#ifndef CONFIG_TEST
	int32_t sound_file = -1;
	char str[80];
	strcpy(str, "/p0/sound/");

	if(instr<dt_config.nb_key)
		strcat(str, (char *)keyboardL_instr_map[instr].instr_name);
	else
		strcat(str, (char *)keyboardR_instr_map[instr - dt_config.nb_key].instr_name);
#ifndef YAFFS_OFF
	sound_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#endif

	if(sound_file < 0)
	{
		trace( error, "Fail! %d\r\n", instr);
		//while(1);
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_write(sound_file,(uint8_t *)sound_struct,INSTRU_STRUCT_SIZE);
		yaffs_close(sound_file);
#endif
	}
#endif
}

void get_instrbin(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length)
{
	char str[80];
	int32_t instr_file = -1;

	strcpy(str, "/p0/sound/");

	if(!keyboard)
		strcat(str, (char *)keyboardL_instr_map[instrument].instr_name);
	else
		strcat(str, (char *)keyboardR_instr_map[instrument].instr_name);
#ifndef YAFFS_OFF
	instr_file = yaffs_open(str, O_RDWR, 0);
#endif
	if(instr_file == -1)
	{

	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_pread( instr_file, buffer, length, address);
		yaffs_close( instr_file);
#endif
	}
}

void save_instrbin(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length)
{
	char str[80];
	int32_t instr_file = -1;

	strcpy(str, "/p0/sound/");

	if(!keyboard)
		strcat(str, (char *)keyboardL_instr_map[instrument].instr_name);
	else
		strcat(str, (char *)keyboardR_instr_map[instrument].instr_name);
#ifndef YAFFS_OFF
	instr_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#endif
	if(instr_file == -1)
	{

	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_pwrite( instr_file, buffer, length, address);
		yaffs_close( instr_file);
#endif
	}
}

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

void init_preset(preset_instr *struct_preset, uint32_t octave)
{

	memset( (void *)struct_preset, 0, PRESET_STRUCT_SIZE);

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

	struct_preset->s_compressor_preset = FX_COMP_PRESET_DEFAULTVALUE;
	struct_preset->s_delay_preset = FX_DELAY_PRESET_DEFAULTVALUE;
	struct_preset->s_distortion_preset = FX_DISTO_PRESET_DEFAULTVALUE;
	struct_preset->s_eq_preset = FX_PEQ_PRESET_DEFAULTVALUE;
	struct_preset->s_chorus_preset = FX_CHORUS_PRESET_DEFAULTVALUE;
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
}

/******* fill an fx mix structure with preset parameters ************/
void set_mixpreset(FX_mix *fx_mix, uint32_t preset_ID)
{
	if(preset_ID>FX_NUMPRESET)
		return;

	fx_mix->m_locutfilterfrequency = fx_mix_preset[preset_ID][0];
	fx_mix->m_hicutfilterfrequency = fx_mix_preset[preset_ID][1];
	fx_mix->m_inputgain = fx_mix_preset[preset_ID][2];
	fx_mix->m_ouputlevel = fx_mix_preset[preset_ID][3];
	fx_mix->m_outputpanning = fx_mix_preset[preset_ID][4];
	fx_mix->m_ouputfrontrear = fx_mix_preset[preset_ID][5];
	fx_mix->m_sendtoreverb = fx_mix_preset[preset_ID][6];
	fx_mix->m_sendtochorus = fx_mix_preset[preset_ID][7];
}

/******* fill an fx distortion structure with preset parameters ************/
void set_distortionpreset(FX_distortion *fx_disto, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUMPRESET)
		return;

	for(i=0; i<NAME_CARACT; i++)
		fx_disto->d_name[i] = fx_distortion_preset[preset_ID][FX_DISTO_NUM_PARAMETERS+FX_DISTO_NOP_PARAMETERS+i];

	fx_disto->d_on_off = fx_distortion_preset[preset_ID][0];
	fx_disto->d_pre_gain = fx_distortion_preset[preset_ID][1];
	fx_disto->d_type = fx_distortion_preset[preset_ID][2];
	fx_disto->d_lowpassfilterfreq = fx_distortion_preset[preset_ID][3];
	fx_disto->d_lowpassfilterres = fx_distortion_preset[preset_ID][4];
	fx_disto->d_postgain = fx_distortion_preset[preset_ID][5];
	fx_disto->d_drive = fx_distortion_preset[preset_ID][6];
}

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
				pregain = (double) instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_pre_gain;
				instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset].d_postgain = (uint32_t) (0.010434723 * pregain * pregain -2.522869547 * pregain + 186.4921302);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), FX_DISTO_POSTGAIN, 1,	0);
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
			set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), param->p_param, 1,	0);
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
				drive = music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion.d_drive;
				if (drive == 0)
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion.d_postgain =  127;
				else if (drive == 7)
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion.d_postgain =  5;
				else
					music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion.d_postgain = drive * (-18) + 127;
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion), FX_DISTO_POSTGAIN, 1,	 music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
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

#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_disto(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_DISTORTION);

#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_disto(&(instr_tab[temp].s_distortion[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion_preset]), FX_DISTO_ALL, 1,	0);
#endif
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

			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_distortion[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_distortion_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion), FX_REVERB_SIZE);
#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_disto(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_distortion), FX_DISTO_ALL, 1,	music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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


/******* fill an fx compressor structure with preset parameters ************/
void set_compressorpreset(FX_compressor *fx_compressor, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUMPRESET)
		return;

	for(i=0; i<NAME_CARACT; i++)
		fx_compressor->c_name[i] = fx_compressor_preset[preset_ID][FX_COMP_NUM_PARAMETERS+FX_COMP_NOP_PARAMETERS+i];

	fx_compressor->c_on_off = fx_compressor_preset[preset_ID][0];
	fx_compressor->c_attacktime = fx_compressor_preset[preset_ID][1];
	fx_compressor->c_releasetime = fx_compressor_preset[preset_ID][2];
	fx_compressor->c_threshold = fx_compressor_preset[preset_ID][3];
	fx_compressor->c_ratio = fx_compressor_preset[preset_ID][4];
	fx_compressor->c_boost = fx_compressor_preset[preset_ID][5];
	fx_compressor->c_kneetype = fx_compressor_preset[preset_ID][6];
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
			set_fx_compressor(&(instr_tab[temp].s_compressor[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_compressor_preset]), param->p_param, 1, 0);
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
			set_fx_compressor(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_compressor), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_compressor(&(instr_tab[temp].s_compressor[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_compressor_preset]), FX_PEQ_ALL, 1, 0);
#endif
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

			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_compressor[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_compressor_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_compressor), FX_COMP_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_compressor(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_compressor), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
}


/******* fill an fx equalizer structure with preset parameters ************/
void set_equalizerpreset(FX_equalizer *fx_equalizer, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUMPRESET)
		return;

	for(i=0; i<NAME_CARACT; i++)
		fx_equalizer->e_name[i] = fx_equalizer_preset[preset_ID][FX_PEQ_NUM_PARAMETERS+FX_PEQ_NOP_PARAMETERS+i];

	fx_equalizer->e_on_off = fx_equalizer_preset[preset_ID][0];
	fx_equalizer->e_lowbandgain = fx_equalizer_preset[preset_ID][1];
	fx_equalizer->e_lowmidbandgain = fx_equalizer_preset[preset_ID][2];
	fx_equalizer->e_highmidbandgain = fx_equalizer_preset[preset_ID][3];
	fx_equalizer->e_highbandgain = fx_equalizer_preset[preset_ID][4];
	fx_equalizer->e_lowbandfrequency = fx_equalizer_preset[preset_ID][5];
	fx_equalizer->e_lowmidbandfrequency = fx_equalizer_preset[preset_ID][6];
	fx_equalizer->e_highmidbandfrequency = fx_equalizer_preset[preset_ID][7];
	fx_equalizer->e_highbandfrequency = fx_equalizer_preset[preset_ID][8];
	fx_equalizer->e_lowmidbandQ = fx_equalizer_preset[preset_ID][9];
	fx_equalizer->e_highmidbandQ = fx_equalizer_preset[preset_ID][10];
}

void set_instrequalizerpreset(FX_equalizer *fx_equalizer, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUMPRESET)
		return;

	for(i=0; i<NAME_CARACT; i++)
		fx_equalizer->e_name[i] = fx_instr_equalizer_preset[preset_ID][FX_PEQ_NUM_PARAMETERS+FX_PEQ_NOP_PARAMETERS+i];

	fx_equalizer->e_on_off = fx_instr_equalizer_preset[preset_ID][0];
	fx_equalizer->e_lowbandgain = fx_instr_equalizer_preset[preset_ID][1];
	fx_equalizer->e_lowmidbandgain = fx_instr_equalizer_preset[preset_ID][2];
	fx_equalizer->e_highmidbandgain = fx_instr_equalizer_preset[preset_ID][3];
	fx_equalizer->e_highbandgain = fx_instr_equalizer_preset[preset_ID][4];
	fx_equalizer->e_lowbandfrequency = fx_instr_equalizer_preset[preset_ID][5];
	fx_equalizer->e_lowmidbandfrequency = fx_instr_equalizer_preset[preset_ID][6];
	fx_equalizer->e_highmidbandfrequency = fx_instr_equalizer_preset[preset_ID][7];
	fx_equalizer->e_highbandfrequency = fx_instr_equalizer_preset[preset_ID][8];
	fx_equalizer->e_lowmidbandQ = fx_instr_equalizer_preset[preset_ID][9];
	fx_equalizer->e_highmidbandQ = fx_instr_equalizer_preset[preset_ID][10];
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
			set_fx_equalizer(&(instr_tab[temp].s_equalizer[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_eq_preset]), param->p_param, 1, 0);
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
			set_fx_equalizer(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_equalizer), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_EQUALIZER);

#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_equalizer(&(instr_tab[temp].s_equalizer[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_eq_preset]), FX_PEQ_ALL, 1, 0);
#endif
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

			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_equalizer[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_eq_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_equalizer), FX_EQ_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_equalizer(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_equalizer), FX_PEQ_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
}

/******* fill an fx others structure with preset parameters ************/
void set_choruspreset(FX_chorus *fx_chorus, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUMPRESET)
		return;

	for(i=0; i<NAME_CARACT; i++)
		fx_chorus->c_name[i] = fx_chorus_preset[preset_ID][FX_CHORUS_NUM_PARAMETERS+FX_CHORUS_NOP_PARAMETERS+i];

	fx_chorus->c_mode = fx_chorus_preset[preset_ID][0];
	fx_chorus->c_effectlevel = fx_chorus_preset[preset_ID][1];
	fx_chorus->c_delaytime = fx_chorus_preset[preset_ID][2];
	fx_chorus->c_feedback = fx_chorus_preset[preset_ID][3];
	fx_chorus->c_inputhighpassfilter = fx_chorus_preset[preset_ID][4];
	fx_chorus->c_hdamp = fx_chorus_preset[preset_ID][5];
	fx_chorus->c_modulationrate = fx_chorus_preset[preset_ID][6];
	fx_chorus->c_modulationdepth = fx_chorus_preset[preset_ID][7];
	fx_chorus->c_tremoloshape = fx_chorus_preset[preset_ID][8];
	fx_chorus->c_rotaryspeed = fx_chorus_preset[preset_ID][9];
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
			set_fx_chorus(&(instr_tab[temp].s_chorus[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus_preset]), param->p_param, 1, 0);
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
			set_fx_chorus(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_chorus), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_CHORUS);

#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_chorus(&(instr_tab[temp].s_chorus[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus_preset]), FX_CHORUS_ALL, 1, 0);
#endif
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

			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_chorus[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_chorus_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_chorus), FX_CHORUS_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_chorus(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_chorus), FX_CHORUS_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
}

/******* fill an fx delay structure with preset parameters ************/
void set_delaypreset(FX_delay *fx_delay, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUMPRESET)
		return;

	for(i=0; i<NAME_CARACT; i++)
		fx_delay->d_name[i] = fx_delay_preset[preset_ID][FX_DELAY_NUM_PARAMETERS+FX_DELAY_NOP_PARAMETERS+i];

	fx_delay->d_on_off = fx_delay_preset[preset_ID][0];
	fx_delay->d_mode = fx_delay_preset[preset_ID][1];
	fx_delay->d_prelp = fx_delay_preset[preset_ID][2];
	fx_delay->d_level = fx_delay_preset[preset_ID][3];
	fx_delay->d_time = fx_delay_preset[preset_ID][4];
	fx_delay->d_feedback = fx_delay_preset[preset_ID][5];
	fx_delay->d_hdamp = fx_delay_preset[preset_ID][6];
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
				time = instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_time;
				feedback = instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_feedback;

				if ((time == 0) || (feedback == 0))
					instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_on_off = 0;
				else
					instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_on_off = 1;

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset]), FX_DELAY_ON_OFF, 1,	0);
#endif
			}
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_delay(&(instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset]), param->p_param, 1, 0);
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
				time = instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_time;
				feedback = instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_feedback;

				if ((time == 0) || (feedback == 0))
					instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_on_off = 0;
				else
					instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset].d_on_off = 1;

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_delay), FX_DELAY_ON_OFF, 1,	 music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
#endif
			}
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_delay(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_delay), param->p_param, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
				switch_off_ctrl_with_preset_off(&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]), INSTR_DELAY);

#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_delay(&(instr_tab[temp].s_delay[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay_preset]), FX_DELAY_ALL, 1, 0);
#endif
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

			temp2 = layer_sound_findandchangeinstrument(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_pc, music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_instrument.instr_midi_C0, 0);

			if(temp2 != 0xFFFF)
			{
				mem_cpy((uint8_t *)&(instr_tab[temp2].s_delay[music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_preset.s_delay_preset]), (uint8_t *)&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_delay), FX_DELAY_SIZE);

#if CONFIG_MIDI_TASK_ENABLED == 1
				set_fx_delay(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_delay), FX_DELAY_ALL, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
}

/******* fill an fx reverb structure with preset parameters ************/
void set_reverbpreset(FX_reverb *fx_reverb, uint32_t preset_ID)
{
	uint32_t i;

	if(preset_ID>FX_NUMPRESET)
		return;

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
	set_panic(0);
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
			instr_tab[temp].s_mix.m_outputpanning = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(instr_tab[temp].s_mix), FX_MIX_OUTPUTPANNING, 1, 0);
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
			music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_mix.m_outputpanning = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_mix), FX_MIX_OUTPUTPANNING, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
			instr_tab[temp].s_mix.m_sendtoreverb = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(instr_tab[temp].s_mix), FX_MIX_SENDTOREVERB, 1, 0);
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
			music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_mix.m_sendtoreverb = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(music_tab[temp].s_track[music_ctrack()].t_loop[music_cloop()].l_instr.i_mix), FX_MIX_SENDTOREVERB, 1, music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_midichannel);
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
	instr_tab[temp].s_mix.m_inputgain = value;
#if CONFIG_MIDI_TASK_ENABLED == 1
	set_fx_mix(&(instr_tab[temp].s_mix), FX_MIX_INPUTGAIN, 1, 0);
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

