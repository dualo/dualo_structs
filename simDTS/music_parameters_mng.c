/*
 * music_parameters_mng.c
 *
 *  Created on: 07/08/2010
 *      Author: mma
 */

// includes
#include "music_parameters_mng.h"
#include "sound_parameters_mng.h"
#include "controler_parameters_mng.h"
#include "arrangement_parameters_mng.h"
#include "g_parameters_mng.h"
#include "yaffsfs.h"
#include "mem/memory_manager.h"
#include "main_task.h"
#include "midi_task.h"
#include "com_task.h"
#include "record_task.h"
#include "looper_timer/looptimer_interface.h"
#include "led_interface.h"

#include "config.h"
#ifndef __LPC18XX__
#include "lpc_heap.h"
#include "lpc177x_8x_eeprom.h"
#include "leds/ledsmng.h"
#else
#include "led/ledsmng.h"
#include "memory_dispach.h"
#endif
#include "display_convert.h"
#include "dutouch_config.h"
#include "eeprom/eeprom_map.h"

volatile s_total_buffer *exchange_buffer;

extern s_instr keyboardL_instr_map[];
extern s_instr keyboardR_instr_map[];

// external parameters

//__attribute__ ((section(".recextflash")))
//music_song *record_buffers[dt_config.nb_key];

extern struct_controler controler_tab;
extern struct_instr instr_tab[];
extern music_song music_tab[];

extern s_arrangement arrangement_buffer;

extern uint32_t record_currentnumsong;

extern eeprom_region_t eeprom_map[];

extern uint32_t record_compt;
extern uint32_t record_beatrepeat[MUSIC_MAXTRACK][MUSIC_MAXLAYER][2];

extern uint32_t record_compt_loop[MUSIC_MAXTRACK][MUSIC_MAXLAYER];
extern uint32_t record_compt_tracker[MUSIC_MAXTRACK][MUSIC_MAXLAYER];
extern uint8_t record_forcelecture[MUSIC_MAXTRACK][MUSIC_MAXLAYER];
extern uint32_t compt_mult;

uint32_t last_ledstate;

uint8_t *metadata_buf = NULL;
uint32_t metadata_size = 0;

SECTION_INTFLASH
const uint8_t ref_led[NUM_LED_VALUE] = {
	0xE0,
	0x00,
	0x82,
	0x00,
	0x0A,
	0x30,
	0x08,
	0xA0,
	0x00,
	0x0A,
	0x00,
	0x0A,
	0xE0,
	0x00,
	0x82,
	0x00,
	0x0A,
	0x30,
	0x08,
	0xA0,
	0x00,
	0x0A,
	0x00,
	0x0A,
	0xE0,
	0x00,
	0x82,
	0x00,
	0x0A,
	0x00,
	0x00,
	0x00
};

/******* fx struct initialisation ************/
void init_musicmemory(void)
{
	exchange_buffer = (s_total_buffer *) lpc_new(RECORD_SAMPLEBUFFERSIZE + MUSIC_SONG_SIZE);
}

void get_musicfilename(uint32_t song, char *string)
{
	char tmp[3];

	strcpy(string, "/p0/music/song_");

	if(song>9)
		tmp[0] = 48 + (song/10);
	else
		tmp[0] = 48;
	tmp[1] = 48 + (song%10);
	tmp[2] = 0;

	strcat(string, tmp);
}

void get_musictrashname(uint32_t song, char *string)
{
	char tmp[3];

	strcpy(string, "/p0/trash/song_");

	if(song>9)
		tmp[0] = 48 + (song/10);
	else
		tmp[0] = 48;
	tmp[1] = 48 + (song%10);
	tmp[2] = 0;

	strcat(string, tmp);
}

void get_musicname(uint32_t song, char *string)
{
	char tmp[3];

	strcpy(string, "du-music ");

	if(song>9)
		tmp[0] = 48 + (song/10);
	else
		tmp[0] = 48;
	tmp[1] = 48 + (song%10);
	tmp[2] = 0;

	strcat(string, tmp);
}

extern inline uint32_t music_strack(void)
{
	return music_tab[controler_tab.c_param.c_song].s_currenttrack;
}

extern inline uint32_t music_sloop(void)
{
	return music_tab[controler_tab.c_param.c_song].s_track[music_tab[controler_tab.c_param.c_song].s_currenttrack].t_currentloop;
}

extern inline uint32_t music_ctrack(void)
{
	return music_tab[record_currentnumsong].s_currenttrack;
}

extern inline uint32_t music_cloop(void)
{
	return music_tab[record_currentnumsong].s_track[music_tab[record_currentnumsong].s_currenttrack].t_currentloop;
}

/******* fx struct parameters initialisation ************/
void migrate_music(music_song *song_struct)
{
	uint32_t version = 0xFFFFFFFF;
	uint32_t i, j;
#ifndef EEPROM_OFF
	EEPROM_Read8(eeprom_map[EE_INFO_LASTFIRMWARE].addr,(void *)&version, eeprom_map[EE_INFO_LASTFIRMWARE].size);
#endif

	if(version<0x00020015) // V2.22
	{
		song_struct->s_swing = 0; // add swing
	}

	if(version<0x00020017) // V2.0.23
	{ // invert filter freq
		for(i = 0; i<MUSIC_MAXTRACK; i++)
		{
			for(j = 0; j<MUSIC_MAXLAYER; j++)
			{
				song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_wah_freq = 127 - song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_wah_freq;
			}
		}
	}
}

void init_music_struct(music_song *song_struct, uint32_t song)
{
	int32_t i, j;

	if(load_defaultmusic(song_struct)>0)
	{ // there is a default file
		// name
		for(i=0; i<MUSIC_SONG_NAME_SIZE; i++)
			song_struct->s_name[i] = 0;
		get_musicname(song + 1, (char *)song_struct->s_name);

		song_struct->s_playhead = 0;
		song_struct->s_state = 0;
		song_struct->s_transpose = RECORD_TRANSPOSEDEFAULT;

		song_struct->s_size = MUSIC_SONG_SIZE;
		song_struct->s_metadata = 0;
		song_struct->s_id = 0;
		song_struct->s_looptimer = MUSIC_INIT_BOUCLETIMER;
		song_struct->s_reftrack = 0;
		song_struct->s_currenttrack = 0;
		song_struct->s_totalsample = 0;
		song_struct->s_decaltempo = 0;
		song_struct->s_version_song = 0;
#ifndef EEPROM_OFF
		EEPROM_Read8(eeprom_map[EE_INFO_NAME].addr,(void *)song_struct->s_original_name, eeprom_map[EE_INFO_NAME].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USER].addr,(void *)song_struct->s_original_user, eeprom_map[EE_INFO_USER].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USERID].addr,(void *)song_struct->s_original_userid, eeprom_map[EE_INFO_USERID].size);
		EEPROM_Read8(eeprom_map[EE_INFO_SN].addr,(void *)song_struct->s_original_sn, eeprom_map[EE_INFO_SN].size);
		EEPROM_Read8(eeprom_map[EE_INFO_NAME].addr,(void *)song_struct->s_modif_name, eeprom_map[EE_INFO_NAME].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USER].addr,(void *)song_struct->s_modif_user, eeprom_map[EE_INFO_USER].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USERID].addr,(void *)song_struct->s_modif_userid, eeprom_map[EE_INFO_USERID].size);
		EEPROM_Read8(eeprom_map[EE_INFO_SN].addr,(void *)song_struct->s_modif_sn, eeprom_map[EE_INFO_SN].size);
#endif

		for(i = 0; i<MUSIC_MAXTRACK; i++)
		{
			for(j = 0; j<MUSIC_MAXLAYER; j++)
			{
				song_struct->s_track[i].t_loop[j].l_adress = 0;
				song_struct->s_track[i].t_loop[j].l_numsample = 0;
				song_struct->s_track[i].t_loop[j].l_state = REC_EMPTY;
				song_struct->s_track[i].t_loop[j].l_loopmod = MUSIC_LOOPMOD_DEFAULTVALUE;
				song_struct->s_track[i].t_loop[j].l_learn = 0;
				song_struct->s_track[i].t_loop[j].l_midioutchannel = 0;
				init_music_instr(&(song_struct->s_track[i].t_loop[j].l_instr), 0);
			}

			song_struct->s_track[i].t_midichannel = MUSIC_MAXTRACK;
			song_struct->s_track[i].t_currentloop = 0;
		}
	}
	else
	{
		// name
		for(i=0; i<MUSIC_SONG_NAME_SIZE; i++)
			song_struct->s_name[i] = 0;
		get_musicname(song + 1, (char *)song_struct->s_name);

		song_struct->s_state = 0;
		song_struct->s_size = MUSIC_SONG_SIZE;
		song_struct->s_metadata = 0;
		song_struct->s_id = 0;
		song_struct->s_looptimer = MUSIC_INIT_BOUCLETIMER;
		song_struct->s_volume = MUSIC_VOL_DEFAULTVALUE;
		song_struct->s_tempo = MUSIC_TEMPO_DEFAULTVALUE;
		song_struct->s_voltempo = MUSIC_TEMPOVOL_DEFAULTVALUE;
		song_struct->s_reftrack = 0;
		song_struct->s_currenttrack = 0;
		song_struct->s_swing = 0;
		song_struct->s_quantification = RECORD_QUANTLEVELDEFAULT;
		song_struct->s_totalsample = 0;
		//song_struct->s_1stloopmode = 0;
		song_struct->s_decaltempo = 0;
		song_struct->s_version_song = 0;
		song_struct->s_version_music = VERSION_MUSIC;
		song_struct->s_displaynote = DEFAULT_KEY_LED_MODE;
		song_struct->s_scaletonality = 1; // C
		song_struct->s_timesignature = TIME_4_4;
		song_struct->s_reverb_preset = FX_REVERB_PRESET_DEFAULTVALUE;

		song_struct->s_direction_gyro_P = -1;
		song_struct->s_direction_gyro_R = -1;
		song_struct->s_direction_gyro_Y = -1;
		song_struct->s_activ_aftertouch = 0;
		song_struct->s_activ_slider_L = 0;
		song_struct->s_activ_slider_R = 0;
		song_struct->s_activ_gyro_P = 0;
		song_struct->s_activ_gyro_R = 0;
		song_struct->s_activ_gyro_Y = 0;

		song_struct->s_playhead = 0;
		song_struct->s_transpose = RECORD_TRANSPOSEDEFAULT;

#ifndef EEPROM_OFF
		EEPROM_Read8(eeprom_map[EE_INFO_NAME].addr,(void *)song_struct->s_original_name, eeprom_map[EE_INFO_NAME].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USER].addr,(void *)song_struct->s_original_user, eeprom_map[EE_INFO_USER].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USERID].addr,(void *)song_struct->s_original_userid, eeprom_map[EE_INFO_USERID].size);
		EEPROM_Read8(eeprom_map[EE_INFO_SN].addr,(void *)song_struct->s_original_sn, eeprom_map[EE_INFO_SN].size);
		EEPROM_Read8(eeprom_map[EE_INFO_NAME].addr,(void *)song_struct->s_modif_name, eeprom_map[EE_INFO_NAME].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USER].addr,(void *)song_struct->s_modif_user, eeprom_map[EE_INFO_USER].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USERID].addr,(void *)song_struct->s_modif_userid, eeprom_map[EE_INFO_USERID].size);
		EEPROM_Read8(eeprom_map[EE_INFO_SN].addr,(void *)song_struct->s_modif_sn, eeprom_map[EE_INFO_SN].size);
#endif

		set_mixpreset(&(song_struct->s_mix), 0);
		set_reverbpreset(&(song_struct->s_reverb), 0);

		for(i = 0; i<MUSIC_MAXTRACK; i++)
		{
			for(j = 0; j<MUSIC_MAXLAYER; j++)
			{
				song_struct->s_track[i].t_loop[j].l_adress = 0;
				song_struct->s_track[i].t_loop[j].l_numsample = 0;
				song_struct->s_track[i].t_loop[j].l_state = REC_EMPTY;
				song_struct->s_track[i].t_loop[j].l_loopmod = MUSIC_LOOPMOD_DEFAULTVALUE;
				song_struct->s_track[i].t_loop[j].l_learn = 0;
				song_struct->s_track[i].t_loop[j].l_midioutchannel = 0;
				init_music_instr(&(song_struct->s_track[i].t_loop[j].l_instr), 0);
			}

			song_struct->s_track[i].t_midichannel = MUSIC_MAXTRACK;
			song_struct->s_track[i].t_currentloop = 0;
		}

		for(i = 0; i<NUM_LED_VALUE; i++)
		{

		song_struct->s_leds[i] = 0x00; //ref_led[i];
		}
	}
}

void convertmusic_V1toV2(music_song *song_struct, uint32_t song_num)
{
	int32_t i, j, k;

	trace(error,"convert V1 to V2 song %d\r\n", song_num);

	song_struct->s_size = MUSIC_SONG_SIZE + (song_struct->s_totalsample * MUSIC_SAMPLE_SIZE);
	song_struct->s_metadata = 0;
	song_struct->s_playhead = 0;
	song_struct->s_state = 0;
	song_struct->s_swing = 0;
	song_struct->s_transpose = RECORD_TRANSPOSEDEFAULT;

	song_struct->s_reverb_preset = FX_REVERB_PRESET_DEFAULTVALUE;

	song_struct->s_direction_gyro_P = -1;
	song_struct->s_direction_gyro_R = -1;
	song_struct->s_direction_gyro_Y = -1;
	song_struct->s_activ_aftertouch = 0;
	song_struct->s_activ_slider_L = 0;
	song_struct->s_activ_slider_R = 0;
	song_struct->s_activ_gyro_P = 0;
	song_struct->s_activ_gyro_R = 0;
	song_struct->s_activ_gyro_Y = 0;
#ifndef EEPROM_OFF
	EEPROM_Read8(eeprom_map[EE_INFO_NAME].addr,(void *)song_struct->s_modif_name, eeprom_map[EE_INFO_NAME].size);
	EEPROM_Read8(eeprom_map[EE_INFO_USER].addr,(void *)song_struct->s_modif_user, eeprom_map[EE_INFO_USER].size);
	EEPROM_Read8(eeprom_map[EE_INFO_USERID].addr,(void *)song_struct->s_modif_userid, eeprom_map[EE_INFO_USERID].size);
	EEPROM_Read8(eeprom_map[EE_INFO_SN].addr,(void *)song_struct->s_modif_sn, eeprom_map[EE_INFO_SN].size);
#endif

	set_reverbpreset(&(song_struct->s_reverb), 0);

	get_musicsamples( (s_total_buffer *)exchange_buffer, song_num);

	for(i = 0; i<MUSIC_MAXTRACK; i++)
	{
		for(j = 0; j<MUSIC_MAXLAYER; j++)
		{
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_arpegiator_type = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_arpegiator_beat = 0;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_direction_gyro_P = -1;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_direction_gyro_R = -1;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_direction_gyro_Y = -1;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_compressor_onoff = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_delay_onoff = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_distortion_onoff = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_eq_onoff = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_chorus_onoff = 0;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_autopitch_rate = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_autopitch_range = 127;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_tremolo_rate = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_tremolo_range = 127;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_autopan_rate = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_autopan_range = 127;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_autowah_rate = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_autowah_range = 127;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_multinote_act = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_multinote[0] = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_multinote[1] = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_multinote[2] = 0;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_multinote[3] = 0;

			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_adsr_attack = FX_ADSR_ATTACK_DEFAULTVALUE;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_adsr_release = FX_ADSR_RELEAS_DEFAULTVALUE;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_wah_type = FX_WAH_FILTERTYPE_DEFAULTVALUE;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_wah_freq = FX_WAH_FILTERFREQ_DEFAULTVALUE;
			song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_wah_res = FX_WAH_FILTERRES_DEFAULTVALUE;

			if(song_struct->s_track[i].t_loop[j].l_instr.i_instrument.instr_key_map)
				song_struct->s_track[i].t_loop[j].l_instr.i_instrument.instr_type = INSTR_PERCU;
			else
				song_struct->s_track[i].t_loop[j].l_instr.i_instrument.instr_type = INSTR_HARMONIC;

			if(song_struct->s_track[i].t_loop[j].l_state != REC_EMPTY)
			{
				trace(error,"cvt sample %d %d %d\r\n", i, j, song_struct->s_track[i].t_loop[j].l_numsample);

				music_sample *played_buffer = (music_sample *)((uint32_t)song_struct->s_track[i].t_loop[j].l_adress + (uint32_t)&(exchange_buffer->local_buffer[0]));
				for(k = 0; k<song_struct->s_track[i].t_loop[j].l_numsample; k++)
				{
					played_buffer[k].canal = played_buffer[k].canal & 0xF7; // remove canals 8 -> 15 not used (and initialized anymore)
					if(song_struct->s_track[i].t_loop[j].l_instr.i_instrument.instr_type == INSTR_HARMONIC) // harmonic instrument -> remove octave from sample
						if((played_buffer[k].control == MIDI_NOTE_ON)||(played_buffer[k].control == MIDI_NOTE_OFF)||(played_buffer[k].control == MIDI_POLY_AFTERTOUCH))
							played_buffer[k].note -= (12*song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_instr_octave) +(song_struct->s_transpose - RECORD_TRANSPOSEMAX);
				}

			}
		}
	}

	song_struct->s_version_music = VERSION_MUSIC;

	mem_cpy((uint8_t *)song_struct, (uint8_t *)&(exchange_buffer->local_song), MUSIC_SONG_SIZE);
	save_musicsamples( (s_total_buffer *)exchange_buffer, song_num, song_struct->s_totalsample * MUSIC_SAMPLE_SIZE);
}


void init_music(music_song *song_struct, uint32_t song)
{
	int32_t i, j, tmp;
	char str[80];
	int32_t music_file = -1;

	get_musicfilename(song, str);
#ifndef YAFFS_OFF
	music_file = yaffs_open(str, O_RDONLY, 0);
#endif

	if(music_file == -1)
	{
		init_music_struct( song_struct, song);
	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_read(music_file,(uint8_t *)song_struct, MUSIC_SONG_SIZE);
		yaffs_close(music_file);
#endif
		
		if(song_struct->s_version_music == 1)
		{ // old file that cannot be read
			// do nothing here because we need to convert after the corruption detection
			//convertmusic_V1toV2( song_struct, song);
		}
		else if(song_struct->s_version_music != VERSION_MUSIC)
		{ // old file that cannot be read
			erase_music(song);
			init_music_struct( song_struct, song);
		}
		
		tmp = 0;
		for(i = 0; i<MUSIC_MAXTRACK; i++)
		{
			for(j = 0; j<MUSIC_MAXLAYER; j++)
			{
				if(((song_struct->s_track[i].t_loop[j].l_state != REC_RECORD)&&(song_struct->s_track[i].t_loop[j].l_state != REC_EMPTY))&&(((uint32_t) song_struct->s_track[i].t_loop[j].l_adress)>RECORD_SAMPLEBUFFERSIZE))
				{
					trace(error,"pb s%d.%d.%d state %d add %d\r\n", song, i, j, song_struct->s_track[i].t_loop[j].l_state, song_struct->s_track[i].t_loop[j].l_adress);
					tmp = 1;
				}
			}
		}

		if(tmp)
		{ // corrupt file
			trace(error,"corrupt song %d\r\n", song);
			if(music_recovery(song_struct))
			{
				trace(error,"song %d recovered\r\n", song);
			}
			else
			{
				trace(error,"erase song %d\r\n", song);
				erase_music(song);
				init_music_struct( song_struct, song);
			}
		}

		if(song_struct->s_version_music == 1)
		{ // old file that cannot be read
			convertmusic_V1toV2( song_struct, song);
		}

		migrate_music(song_struct);

		for(i = 0; i<MUSIC_MAXTRACK; i++)
		{
			for(j = 0; j<MUSIC_MAXLAYER; j++)
			{
				if(song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_expression<64)
					song_struct->s_track[i].t_loop[j].l_instr.i_preset.s_expression = MAIN_EXPRESSION_DEFAULT;

				if(song_struct->s_track[i].t_loop[j].l_instr.i_instrument.instr_relvolume<40)
					song_struct->s_track[i].t_loop[j].l_instr.i_instrument.instr_relvolume = 0x7F;
			}
		}

	}

}



int32_t get_musicsamples(s_total_buffer *music_struct, uint32_t song)
{
	int32_t ret = -1;
	char str[80];
	int32_t music_file = -1;

	get_musicfilename(song, str);

#ifndef YAFFS_OFF
	music_file = yaffs_open(str, O_RDWR, 0);
#endif

	if(music_file == -1)
	{

	}
	else
	{
#ifndef YAFFS_OFF
		ret = yaffs_read(music_file,(uint8_t *)music_struct, RECORD_SAMPLEBUFFERSIZE + MUSIC_SONG_SIZE);
		yaffs_close(music_file);
#endif
	}

	return ret;
}

int32_t save_musicsamples(s_total_buffer *music_struct, uint32_t song, uint32_t size)
{
#ifndef CONFIG_TEST
	int32_t ret = -1;
	char str[80];
	int32_t music_file = -1;

	get_musicfilename(song, str);

	if(size > RECORD_SAMPLEBUFFERSIZE)
	{// error
#ifndef YAFFS_OFF
		yaffs_unlink(str);
#endif
		return 0;
	}
#ifndef YAFFS_OFF
	music_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#endif

	if(music_file == -1)
	{
#ifndef YAFFS_OFF
		yaffs_unlink(str);
#endif
	}
	else
	{
#ifndef EEPROM_OFF
		EEPROM_Read8(eeprom_map[EE_INFO_NAME].addr,(void *)music_struct->local_song.s_modif_name, eeprom_map[EE_INFO_NAME].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USER].addr,(void *)music_struct->local_song.s_modif_user, eeprom_map[EE_INFO_USER].size);
		EEPROM_Read8(eeprom_map[EE_INFO_USERID].addr,(void *)music_struct->local_song.s_modif_userid, eeprom_map[EE_INFO_USERID].size);
		EEPROM_Read8(eeprom_map[EE_INFO_SN].addr,(void *)music_struct->local_song.s_modif_sn, eeprom_map[EE_INFO_SN].size);
#endif
#ifndef YAFFS_OFF
		ret = yaffs_write(music_file,(uint8_t *)music_struct, size + MUSIC_SONG_SIZE);
		yaffs_close(music_file);
#endif
	}
	return ret;
#endif
}

uint32_t get_musicmetadatasize(void)
{
	return metadata_size;
}

int32_t get_musicarrangement(s_arrangement *arrangement_struct, uint32_t metadataaddress, uint32_t metadatasize, uint32_t song)
{
	int32_t ret = -1;
	int32_t music_file = -1;
	uint32_t i;
	s_metadata *metadata_entry;
	char str[40];



	get_musicfilename(song, str);
#ifndef YAFFS_OFF
	music_file = yaffs_open(str, O_RDWR, 0);
#endif

	if(music_file == -1)
	{

	}
	else
	{
		metadata_size = 0;
		metadata_buf = (uint8_t *) lpc_realloc( metadata_buf, metadatasize);
#ifndef YAFFS_OFF
		ret = yaffs_pread(music_file,(uint8_t *)metadata_buf, metadatasize, metadataaddress);
#endif
		if(ret < metadatasize)
		{
			ret = -2;
		}
		else
		{
			// is it really metadata?
			ret = strncmp((char *)metadata_buf,"METADATA",8);

			if(ret)
			{
				ret = -3;
			}
			else
			{
				metadata_size = metadatasize;
				i = 0;
				metadata_entry = (s_metadata *)&metadata_buf[8];
				while((metadata_entry->meta_type != MUSICMETADATA_TYPE_ARRANGEMENT)&&(i<MUSIC_METADATAHEADERSIZE))
				{
					i+=MUSIC_METADATAENTRYSIZE;
					metadata_entry = (s_metadata *)&metadata_buf[i+8];
				}

				if(metadata_entry->meta_type != MUSICMETADATA_TYPE_ARRANGEMENT)
				{
					ret = -4;
				}
				else
				{
					if(metadata_entry->meta_size > ARRANGEMENT_MAXSIZE)
						metadata_entry->meta_size = ARRANGEMENT_MAXSIZE;

					memcpy((uint8_t *)arrangement_struct, (uint8_t *)&metadata_buf[metadata_entry->meta_address + (MUSIC_METADATAHEADERSIZE * MUSIC_METADATAENTRYSIZE) + 8], metadata_entry->meta_size);
					ret = 0;
				}
			}
		}
#ifndef YAFFS_OFF
		yaffs_close(music_file);
#endif
	}

	return ret;
}


int32_t save_musicarrangement(uint32_t metadataaddress, uint32_t metadatasize, uint32_t song)
{
	int32_t ret = -1;
	int32_t music_file = -1;
	//uint32_t i;
	//s_metadata *metadata_entry;
	char str[40];



	get_musicfilename(song, str);
#ifndef YAFFS_OFF
	music_file = yaffs_open(str, O_RDWR, 0);
#endif

	if(music_file == -1)
	{

	}
	else
	{
#ifndef YAFFS_OFF
		ret = yaffs_pwrite(music_file,(uint8_t *)metadata_buf, metadatasize, metadataaddress);
#endif

		if(ret < metadatasize)
		{
			ret = -2;
		}
		else
		{
			ret = 0;
		}
#ifndef YAFFS_OFF
		yaffs_close(music_file);
#endif
	}

	return ret;
}

void get_musicdata(uint32_t song, uint8_t *buffer, uint32_t address, uint32_t length)
{
	char str[80];
	int32_t music_file = -1;

	get_musicfilename(song, str);
#ifndef YAFFS_OFF
	music_file = yaffs_open(str, O_RDWR, 0);
#endif
	if(music_file == -1)
	{

	}
	else
	{
#ifndef YAFFS_OFF
		yaffs_pread( music_file, buffer, length, address);
		yaffs_close( music_file);
#endif
	}
}

void save_musicdata(uint32_t song, uint8_t *buffer, uint32_t address, uint32_t length)
{
#ifndef CONFIG_TEST
	char str[80];
	int32_t music_file = -1;

	get_musicfilename(song, str);
#ifndef YAFFS_OFF
	music_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#endif
	if(music_file == -1)
	{

	}
	else
	{
#ifndef YAFFS_OFF
		//yaffs_write( music_file, buffer, length);
		yaffs_pwrite( music_file, buffer, length, address);
		yaffs_close( music_file);
#endif
	}
#endif
}

void erase_music_instr_name(music_instr *instr_struct)
{
	int32_t j;

	for(j = 0; j<NAME_CARACT; j++)
	{
		instr_struct->i_instrument.instr_name[j] = 0;
		instr_struct->i_instrument.instr_cat[j] = 0;
	}
}

void init_music_instr(music_instr *instr_struct, uint32_t instr)
{
	memcpy(&(instr_struct->i_instrument), &(instr_tab[controler_tab.c_param.c_instr].s_instrument), INSTR_INFO_SIZE);
	memcpy(&(instr_struct->i_preset), &(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum]), PRESET_STRUCT_SIZE);

	memcpy(&(instr_struct->i_mix), &(instr_tab[controler_tab.c_param.c_instr].s_mix), FX_MIX_SIZE);
	memcpy(&(instr_struct->i_distortion), &(instr_tab[controler_tab.c_param.c_instr].s_distortion[instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_distortion_preset]), FX_DIST_SIZE);
	memcpy(&(instr_struct->i_compressor), &(instr_tab[controler_tab.c_param.c_instr].s_compressor[instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_compressor_preset]), FX_COMP_SIZE);
	memcpy(&(instr_struct->i_equalizer), &(instr_tab[controler_tab.c_param.c_instr].s_equalizer[instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_eq_preset]), FX_EQ_SIZE);
	memcpy(&(instr_struct->i_chorus), &(instr_tab[controler_tab.c_param.c_instr].s_chorus[instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_preset]), FX_CHORUS_SIZE);
	memcpy(&(instr_struct->i_delay), &(instr_tab[controler_tab.c_param.c_instr].s_delay[instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_delay_preset]), FX_DELAY_SIZE);
}

void cp_music_instr(music_instr *music_instr_struct, struct_instr* instr_struct, uint32_t preset)
{
	memcpy(&(music_instr_struct->i_instrument), &(instr_struct->s_instrument), INSTR_INFO_SIZE);
	memcpy(&(music_instr_struct->i_preset), &(instr_struct->s_preset[preset]), PRESET_STRUCT_SIZE);

	memcpy(&(music_instr_struct->i_mix), &(instr_struct->s_mix), FX_MIX_SIZE);
	memcpy(&(music_instr_struct->i_distortion), &(instr_struct->s_distortion[instr_struct->s_preset[preset].s_distortion_preset]), FX_DIST_SIZE);
	memcpy(&(music_instr_struct->i_compressor), &(instr_struct->s_compressor[instr_struct->s_preset[preset].s_compressor_preset]), FX_COMP_SIZE);
	memcpy(&(music_instr_struct->i_equalizer), &(instr_struct->s_equalizer[instr_struct->s_preset[preset].s_eq_preset]), FX_EQ_SIZE);
	memcpy(&(music_instr_struct->i_chorus), &(instr_struct->s_chorus[instr_struct->s_preset[preset].s_chorus_preset]), FX_CHORUS_SIZE);
	memcpy(&(music_instr_struct->i_delay), &(instr_struct->s_delay[instr_struct->s_preset[preset].s_delay_preset]), FX_DELAY_SIZE);
}

/******* fx struct parameters saving ************/
void erase_music( uint32_t song)
{
	char str[80];
	char str2[80];
	get_musicfilename(song, str);
	get_musictrashname(song, str2);
	//yaffs_unlink(str);
#ifndef YAFFS_OFF
	yaffs_rename( str, str2) ;
#endif
}

/******* load default song struct ************/
int32_t load_defaultmusic(music_song *song_struct)
{
	int32_t music_file = -1;
	int32_t ret = -1;
#ifndef YAFFS_OFF
	music_file = yaffs_open("/p0/music/default", O_RDONLY, 0);
#endif

	if(music_file == -1)
	{
		// there is no default file -> do nothing
	}
	else
	{
#ifndef YAFFS_OFF
		ret = yaffs_read(music_file,(uint8_t *)song_struct, MUSIC_SONG_SIZE);
		yaffs_close(music_file);
#endif
	}

	return ret;
}

int32_t save_defaultmusic(music_song *song_struct)
{
	int32_t music_file = -1;
	int32_t ret = -1;
#ifndef YAFFS_OFF
	music_file = yaffs_open("/p0/music/default", O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#endif
	if(music_file == -1)
	{
	}
	else
	{
#ifndef YAFFS_OFF
		ret = yaffs_write(music_file,(uint8_t *)song_struct, MUSIC_SONG_SIZE);
		yaffs_close(music_file);
#endif
	}

	return ret;
}

/******* set learn mode ************/
void setmusiclearn(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t i,j;

	if(rec)
	{

		if(value) // on
		{
			for(i=0; i< MUSIC_MAXTRACK; i++)
			{
				for(j=0; j<MUSIC_MAXLAYER; j++)
					music_tab[controler_tab.c_param.c_song].s_track[i].t_loop[j].l_learn = 0;
			}
			music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_learn = value;

//			if((music_tab[controler_tab.c_param.c_song].s_track[music_tab[controler_tab.c_param.c_song].s_currenttrack].t_instr.i_instrument.instr_key_map))//&&(controler_tab.c_param.c_midiout_mode < MIDI_8TRACKS)) // is a drum
//			{
//				Set_LedPlaySong( music_tab[controler_tab.c_param.c_song].s_leds, 0, 0);
//				//Set_LedPlayDrum(music_tab[controler_tab.c_param.c_song].s_track[music_tab[controler_tab.c_param.c_song].s_currenttrack].t_instr.i_instrument.instr_key_map);
//			}
//			else
//			{
//				Set_LedPlaySong( music_tab[controler_tab.c_param.c_song].s_leds, 0, 0);
//				//Set_LedPlayDrum(0);
//			}

			if(value == LEARN_STEPBYSTEP)
			{
				for(i=0; i< MUSIC_MAXTRACK; i++)
				{
					for(j=0; j<MUSIC_MAXLAYER; j++)
					{
						if((music_tab[controler_tab.c_param.c_song].s_track[i].t_loop[j].l_state == REC_PLAY)&&(!((i==music_strack())&&(j==music_sloop()))))
							music_tab[controler_tab.c_param.c_song].s_track[i].t_loop[j].l_state = REC_STOP;
					}
				}
				set_panic(0);
			}

			if(controler_tab.c_param.c_led_display)
				controler_tab.c_param.c_led_display = 0;

			//music_instr_fx_send( 0, &(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr));
		}
		else
		{
			music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_learn = 0;
		}

		if(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_learn != LEARN_STEPBYSTEP)
			recordTask_resetnotetoget();
	}
}

void setmusicvolume(param_struct *param, int32_t value, uint32_t rec)
{
	int32_t i;

	for(i=0;i<MUSIC_MAXTRACK;i++)
	{
		if(music_tab[controler_tab.c_param.c_song].s_track[i].t_loop[music_sloop()].l_state != REC_EMPTY)
		{
			//mainsend_midi( MIDI_NORECORD | MIDI_CONTROL_CHANGE, current_song.track_midichannel[ref], MIDI_CTRL_VOLUME, current_song.track_volume[ref]);
			//mainsend_midi( MIDI_NORECORD | MIDI_CONTROL_CHANGE, current_song.track_midichannel[ref] + RECORD_MIDI_MAXLAYER, MIDI_CTRL_VOLUME, current_song.track_volume[ref]);
			music_tab[controler_tab.c_param.c_song].s_mix.m_ouputlevel = (value&0x7F);
#if CONFIG_MIDI_TASK_ENABLED == 1
			set_fx_mix(&(music_tab[controler_tab.c_param.c_song].s_mix), FX_MIX_OUTPUTLEVEL, 1,  music_tab[controler_tab.c_param.c_song].s_track[i].t_midichannel);
			//set_fx_mix(&(music_tab[controler_tab.c_param.c_song].s_mix), FX_MIX_OUTPUTLEVEL, 1,  music_tab[controler_tab.c_param.c_song].s_track[i].t_midichannel + RECORD_MIDI_MAXLAYER);
#endif
		}
	}
	//set_metronome_volume( metronome_vol);
}

void setmusicmix(param_struct *param, int32_t value, uint32_t rec)
{
	int32_t i;

	if(param->p_param == FX_MIX_OUTPUTPANNING)
	{
		//if(rec)
		{
			music_tab[controler_tab.c_param.c_song].s_mix.m_outputpanning = (value&0x7F);

			for(i=0;i<MUSIC_MAXTRACK;i++)
			{
				if(music_tab[controler_tab.c_param.c_song].s_track[i].t_loop[music_sloop()].l_state != REC_EMPTY)
				{
#if CONFIG_MIDI_TASK_ENABLED == 1
					set_fx_mix(&(music_tab[controler_tab.c_param.c_song].s_mix), FX_MIX_OUTPUTPANNING, 1,  music_tab[controler_tab.c_param.c_song].s_track[i].t_midichannel);
					//set_fx_mix(&(music_tab[controler_tab.c_param.c_song].s_mix), FX_MIX_OUTPUTPANNING, 1,  music_tab[controler_tab.c_param.c_song].s_track[i].t_midichannel + RECORD_MIDI_MAXLAYER);
#endif
				}
			}
		}
	}
	else if(param->p_param == FX_MIX_HCFILTER)
	{
		//if(rec)
		{
			music_tab[controler_tab.c_param.c_song].s_mix.m_hicutfilterfrequency = (value&0x7F);

			for(i=0;i<MUSIC_MAXTRACK;i++)
			{
				if(music_tab[controler_tab.c_param.c_song].s_track[i].t_loop[music_sloop()].l_state != REC_EMPTY)
				{
#if CONFIG_MIDI_TASK_ENABLED == 1
					set_fx_mix(&(music_tab[controler_tab.c_param.c_song].s_mix), FX_MIX_HCFILTER, 1,  music_tab[controler_tab.c_param.c_song].s_track[i].t_midichannel);
					//set_fx_mix(&(music_tab[controler_tab.c_param.c_song].s_mix), FX_MIX_HCFILTER, 1,  music_tab[controler_tab.c_param.c_song].s_track[i].t_midichannel + RECORD_MIDI_MAXLAYER);
#endif
				}
			}
		}
	}
}

void setmusictempo(param_struct *param, int32_t value, uint32_t rec)
{
	if(rec) // TODO: is it really necessary
		music_tab[controler_tab.c_param.c_song].s_tempo = value;

#if CONFIG_RECORD_TASK_ENABLED == 1
	looptimer_settempo( value);
#endif
}

void setmusictempovolume(param_struct *param, int32_t value, uint32_t rec)
{
#if CONFIG_RECORD_TASK_ENABLED == 1
	set_metronome_vol( value);
#endif
	if(value)
	{ // active metronome if not activated
		if(!v_metronome)
		{
			//recordTask_synchtempo();
			v_metronome = 1;
		}
	}
}

void setmusicplayhead(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t i;
	uint32_t j;

#if CONFIG_RECORD_TASK_ENABLED == 1
	record_compt = (music_tab[record_currentnumsong].s_playhead * (compt_mult*music_tab[record_currentnumsong].s_looptimer))/255;

	for(i=0; i<MUSIC_MAXTRACK; i++)
	{
		for(j=0; j<MUSIC_MAXLAYER; j++)
		{
			record_forcelecture[i][j] = 1;
#if CONFIG_COM_TASK_ENABLED == 1
			add_cmd_usbmidi( MIDI_CONTROL_CHANGE, music_tab[record_currentnumsong].s_track[i].t_loop[j].l_midioutchannel, 0x78, 0); // panic mode en midi out
#endif
			record_compt_loop[i][j] = record_compt;
			record_compt_tracker[i][j] = record_compt;

		}
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[record_currentnumsong].s_track[i].t_midichannel, 0x78, 0); // panic mode sur le canal
		midiTask_add_cmd( MIDI_CONTROL_CHANGE, music_tab[record_currentnumsong].s_track[i].t_midichannel + 8, 0x78, 0); // panic mode sur le canal
	}

#endif
}

void setmusictempomode(param_struct *param, int32_t value, uint32_t rec)
{
//	if(last_metronome_mode != mode)
//	{
//		last_metronome_mode = mode;
//#if CONFIG_RECORD_TASK_ENABLED == 1
//		recordTask_set_metronome( mode);
//#endif
	//recordTask_synchtempo();

		if(!(v_metronome&METRONOME_LIGTH_MASK))
			LedMng_LogoBrightness(LEDSMNG_LOGOBRIGHTNESS);
//	}
}

void switchledmode(param_struct *param, int32_t value, uint32_t rec)
{
	if(value == STATIC_LED_MODE)
	{
		Set_LedPlaySong( music_tab[controler_tab.c_param.c_song].s_leds, 0, 1, music_tab[controler_tab.c_param.c_song].s_displaynote, instr_tab[controler_tab.c_param.c_instr].s_displayled, music_tab[controler_tab.c_param.c_song].s_scaletonality);
	}
	else
	{
		if(last_ledstate == STATIC_LED_MODE)
		{
			music_tab[controler_tab.c_param.c_song].s_displaynote = STATIC_LED_MODE;
			Set_LedPlaySong( music_tab[controler_tab.c_param.c_song].s_leds, 0, 2, music_tab[controler_tab.c_param.c_song].s_displaynote, instr_tab[controler_tab.c_param.c_instr].s_displayled, record_currentnumsong);
			music_tab[controler_tab.c_param.c_song].s_displaynote = value;
		}
		if(value == NONE_LED_MODE)
			Clear_LedPlay();
		else
			Draw_LedGamme(value - 1, music_tab[controler_tab.c_param.c_song].s_scaletonality);
	}
	last_ledstate = value;

	if(controler_tab.c_param.c_diatonic < (NUM_KEY_LAYOUT-1))
	{
		current_key_map = controler_tab.c_param.c_diatonic;
		current_key_offset = DEFAULT_KEY_OFFSET;
	}
	else
	{
		if((value!=STATIC_LED_MODE)&&(value!=NONE_LED_MODE))
		{
			current_key_map = controler_tab.c_param.c_diatonic + (value - 1);
			current_key_offset = music_tab[controler_tab.c_param.c_song].s_scaletonality - 1;
		}
		else
		{
			current_key_map = 0;
			current_key_offset = DEFAULT_KEY_OFFSET;
		}
	}
}

void switchledmode_ps(param_struct *param, int32_t value, uint32_t rec)
{
	if(value)
	{
		Set_LedPlaySong( instr_tab[controler_tab.c_param.c_instr].s_leds, instr_tab[controler_tab.c_param.c_instr].s_instrument.instr_key_map, 1, STATIC_LED_MODE, instr_tab[controler_tab.c_param.c_instr].s_displayled, 0);
	}
	else
	{
		Set_LedPlaySong( instr_tab[controler_tab.c_param.c_instr].s_leds, 0, 2, STATIC_LED_MODE, instr_tab[controler_tab.c_param.c_instr].s_displayled, 0);

		Clear_LedPlay();
	}
	last_ledstate = STATIC_LED_MODE;
}

void transpose(param_struct *param, int32_t value, uint32_t rec)
{
	set_panic(0);
}

void setmusicbeatrepeat(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t i;
	uint32_t j;
	uint32_t temp;
	static uint32_t record_tmp = 0;
	static uint32_t last_state = 0;

	set_panic(0);

#ifdef FREE_RTOS
	taskENTER_CRITICAL();
#endif

	if(value)
	{
		temp = (1<<((METRONOME_MOD_TIMER_P2 + 2)-value));

		if(!last_state)
		{
			record_tmp = record_compt;
			last_state = 1;
		}

		for(i=0; i<MUSIC_MAXTRACK; i++)
		{
			for(j=0; j<MUSIC_MAXLAYER; j++)
			{
				record_beatrepeat[i][j][0] = temp;// length
				record_beatrepeat[i][j][1] = record_tmp; // start time
			}
		}
	}
	else
	{
		last_state = 0;
		//record_compt = record_beatrepeat[0][0][0] + record_beatrepeat[0][0][1];
		for(i=0; i<MUSIC_MAXTRACK; i++)
		{
			for(j=0; j<MUSIC_MAXLAYER; j++)
			{
				record_beatrepeat[i][j][0] = 0;// length
				//record_beatrepeat[i][j][1] = 0; // start time
			}
		}
	}
#ifdef FREE_RTOS
	taskEXIT_CRITICAL();
#endif

}

void setloopbeatrepeat(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t i;
	uint32_t j;
	uint32_t temp;

#ifdef FREE_RTOS
	taskENTER_CRITICAL();
#endif

	i = music_strack();
	j =  music_sloop();

	if(value)
	{
		temp = (1<<(METRONOME_MOD_TIMER_P2 + 2 -value));
		record_beatrepeat[i][j][0] = temp;// length
		record_beatrepeat[i][j][1] = record_compt; // start time
	}
	else
	{
		record_beatrepeat[i][j][0] = 0;// length
		record_beatrepeat[i][j][1] = 0; // start time
	}
#ifdef FREE_RTOS
	taskEXIT_CRITICAL();
#endif
}

void setarrangementstep(param_struct *param, int32_t value, uint32_t rec)
{
	if(value < arrangement_buffer.as_grade)
	{
		if(arrangement_buffer.as_currentevent < arrangement_buffer.as_numevent)
			recordTask_arrangement_loadstep(arrangement_buffer.as_currentevent);
	}

}


void change_scale(uint32_t key, uint32_t keyboard)
{
	if((music_tab[controler_tab.c_param.c_song].s_displaynote < STATIC_LED_MODE)&&(music_tab[controler_tab.c_param.c_song].s_displaynote != NONE_LED_MODE))
	{
		if(keyboard) // R
		{
			music_tab[controler_tab.c_param.c_song].s_scaletonality = key % 24;
		}
		else
		{
			if(key < 10)
			{
				music_tab[controler_tab.c_param.c_song].s_scaletonality = 14 + key;
			}
			else
			{
				music_tab[controler_tab.c_param.c_song].s_scaletonality = (key - 10) % 24;
			}
		}
		Draw_LedGamme( music_tab[controler_tab.c_param.c_song].s_displaynote - 1, music_tab[controler_tab.c_param.c_song].s_scaletonality);

		if(controler_tab.c_param.c_diatonic < (NUM_KEY_LAYOUT-1))
		{
			current_key_map = controler_tab.c_param.c_diatonic;
			current_key_offset = DEFAULT_KEY_OFFSET;
		}
		else
		{
			if((music_tab[controler_tab.c_param.c_song].s_displaynote!=STATIC_LED_MODE)&&(music_tab[controler_tab.c_param.c_song].s_displaynote!=NONE_LED_MODE))
			{
				current_key_map = controler_tab.c_param.c_diatonic + (music_tab[controler_tab.c_param.c_song].s_displaynote - 1);
				current_key_offset = music_tab[controler_tab.c_param.c_song].s_scaletonality - 1;
			}
			else
			{
				current_key_map = 0;
				current_key_offset = DEFAULT_KEY_OFFSET;
			}
		}

		displayscale( NULL, 0, 0);
	}
}

uint32_t music_recovery(music_song *song_struct)
{
	uint32_t offset = 0xFFFFFFFF;
	uint32_t temp;
	uint32_t i, j;
	// search for buffer start
	for(i=0; i<MUSIC_MAXTRACK; i++)
	{
		for(j=0; j<MUSIC_MAXLAYER; j++)
		{
			if(song_struct->s_track[i].t_loop[j].l_numsample)
			{
				if(offset == (uint32_t)song_struct->s_track[i].t_loop[j].l_adress)
				{// error
					return 0;
				}
				else if(offset > (uint32_t)song_struct->s_track[i].t_loop[j].l_adress)
				{
					offset = (uint32_t)song_struct->s_track[i].t_loop[j].l_adress;
				}

			}
		}
	}

	// do the supposed offset
	for(i=0; i<MUSIC_MAXTRACK; i++)
	{
		for(j=0; j<MUSIC_MAXLAYER; j++)
		{
			if(song_struct->s_track[i].t_loop[j].l_numsample)
			{
				temp = (uint32_t)song_struct->s_track[i].t_loop[j].l_adress;
				temp -= offset;
				song_struct->s_track[i].t_loop[j].l_adress = (music_sample *) temp;

				if((song_struct->s_track[i].t_loop[j].l_state != REC_RECORD)&&(((uint32_t)(song_struct->s_track[i].t_loop[j].l_adress))>RECORD_SAMPLEBUFFERSIZE))
				{// error
					return 0;
				}
			}
		}
	}

	return 1;
}



/********** get song info ********************/
int32_t music_getinfosong_param(uint32_t num_song, uint32_t num_track, uint32_t param, uint8_t *buffer)
{
	//record_song_t tmp_song;
	int32_t ret = 0;
	uint32_t i;

	if(num_song>=dt_config.nb_key)
		return -1;

	if(num_track<MUSIC_MAXTRACK)
	{
		switch (param){
/*			case TRACK_STATE:
				buffer[0] = ((music_tab[num_song].s_track[num_track].t_state>>24) & 0xFF);
				buffer[1] = ((music_tab[num_song].s_track[num_track].t_state>>16) & 0xFF);
				buffer[2] = ((music_tab[num_song].s_track[num_track].t_state>>8) & 0xFF);
				buffer[3] = ((music_tab[num_song].s_track[num_track].t_state>>0) & 0xFF);
				ret = 4;
			break;
*/
			case TRACK_MIDICHANNEL:
				buffer[0] = ((music_tab[num_song].s_track[num_track].t_midichannel>>24) & 0xFF);
				buffer[1] = ((music_tab[num_song].s_track[num_track].t_midichannel>>16) & 0xFF);
				buffer[2] = ((music_tab[num_song].s_track[num_track].t_midichannel>>8) & 0xFF);
				buffer[3] = ((music_tab[num_song].s_track[num_track].t_midichannel>>0) & 0xFF);
				ret = 4;
			break;
/*
			case TRACK_LOOPMOD:
				buffer[0] = ((music_tab[num_song].s_track[num_track].t_loopmod>>24) & 0xFF);
				buffer[1] = ((music_tab[num_song].s_track[num_track].t_loopmod>>16) & 0xFF);
				buffer[2] = ((music_tab[num_song].s_track[num_track].t_loopmod>>8) & 0xFF);
				buffer[3] = ((music_tab[num_song].s_track[num_track].t_loopmod>>0) & 0xFF);
				ret = 4;
			break;
*/
			case TRACK_CURRENTLOOP:
				buffer[0] = ((music_tab[num_song].s_track[num_track].t_currentloop>>24) & 0xFF);
				buffer[1] = ((music_tab[num_song].s_track[num_track].t_currentloop>>16) & 0xFF);
				buffer[2] = ((music_tab[num_song].s_track[num_track].t_currentloop>>8) & 0xFF);
				buffer[3] = ((music_tab[num_song].s_track[num_track].t_currentloop>>0) & 0xFF);
				ret = 4;
			break;
/*
			case TRACK_LEARN:
				buffer[0] = ((music_tab[num_song].s_track[num_track].t_learn>>24) & 0xFF);
				buffer[1] = ((music_tab[num_song].s_track[num_track].t_learn>>16) & 0xFF);
				buffer[2] = ((music_tab[num_song].s_track[num_track].t_learn>>8) & 0xFF);
				buffer[3] = ((music_tab[num_song].s_track[num_track].t_learn>>0) & 0xFF);
				ret = 4;
			break;
*/
			default:
				ret = -1;
			break;
		}
	}
	else
	{
		switch (param){

			case SONG_VOLUME:
					buffer[0] = ((music_tab[num_song].s_volume>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_volume>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_volume>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_volume>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_TEMPO:
					buffer[0] = ((music_tab[num_song].s_tempo>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_tempo>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_tempo>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_tempo>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_VOLTEMPO:
					buffer[0] = ((music_tab[num_song].s_voltempo>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_voltempo>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_voltempo>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_voltempo>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_REFTRACK:
				buffer[0] = ((music_tab[num_song].s_reftrack>>24) & 0xFF);
				buffer[1] = ((music_tab[num_song].s_reftrack>>16) & 0xFF);
				buffer[2] = ((music_tab[num_song].s_reftrack>>8) & 0xFF);
				buffer[3] = ((music_tab[num_song].s_reftrack>>0) & 0xFF);
				ret = 4;
			break;

			case SONG_CURRENTTRACK:
					buffer[0] = ((music_tab[num_song].s_currenttrack>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_currenttrack>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_currenttrack>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_currenttrack>>0) & 0xFF);
					ret = 4;
				break;
/*
			case SONG_1STLOOPMODE:
					buffer[0] = ((music_tab[num_song].s_1stloopmode>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_1stloopmode>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_1stloopmode>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_1stloopmode>>0) & 0xFF);
					ret = 4;
				break;
*/
			case SONG_TOTALSAMPLE:
					buffer[0] = ((music_tab[num_song].s_totalsample>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_totalsample>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_totalsample>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_totalsample>>0) & 0xFF);
					ret = 4;
				break;


			case SONG_TIMER:
					buffer[0] = ((music_tab[num_song].s_looptimer>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_looptimer>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_looptimer>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_looptimer>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_ID:
					buffer[0] = ((music_tab[num_song].s_id>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_id>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_id>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_id>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_NAME:
					for(i=0; i<MUSIC_SONG_NAME_SIZE; i++)
						buffer[i] = music_tab[num_song].s_name[i];
					ret = MUSIC_SONG_NAME_SIZE;
				break;

			case SONG_VERSION_SONG:
					buffer[0] = ((music_tab[num_song].s_version_song>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_version_song>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_version_song>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_version_song>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_VERSION_MUSIC:
					buffer[0] = ((music_tab[num_song].s_version_music>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_version_music>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_version_music>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_version_music>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_SIZE:
					buffer[0] = ((music_tab[num_song].s_size>>24) & 0xFF);
					buffer[1] = ((music_tab[num_song].s_size>>16) & 0xFF);
					buffer[2] = ((music_tab[num_song].s_size>>8) & 0xFF);
					buffer[3] = ((music_tab[num_song].s_size>>0) & 0xFF);
					ret = 4;
				break;

			case SONG_ORIGINAL_SN:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_original_sn[i];
					ret = 0x010;
				break;

			case SONG_ORIGINAL_NAME:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_original_name[i];
					ret = 0x010;
				break;

			case SONG_ORIGINAL_USER:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_original_user[i];
					ret = 0x010;
				break;

			case SONG_ORIGINAL_USERID:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_original_userid[i];
					ret = 0x010;
				break;

			case SONG_MODIF_SN:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_modif_sn[i];
					ret = 0x010;
				break;

			case SONG_MODIF_NAME:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_modif_name[i];
					ret = 0x010;
				break;

			case SONG_MODIF_USER:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_modif_user[i];
					ret = 0x010;
				break;

			case SONG_MODIF_USERID:
					for(i=0; i<0x010; i++)
						buffer[i] = music_tab[num_song].s_modif_userid[i];
					ret = 0x010;
				break;

			default:
				ret = -1;
			break;
		}
	}
	return ret;
}

/********** get song info ********************/
int32_t music_setinfosong_param(uint32_t num_song, uint32_t num_track, uint32_t param, uint8_t *buffer)
{
	int32_t ret = 0;
	uint32_t i;
	uint32_t value = (((short)buffer[0]<<24) + ((short)buffer[1]<<16) + ((short)buffer[2]<<8) + (short)buffer[3]);

	if(num_song>=dt_config.nb_key)
		return -1;

	//hasbeenchanged[num_song] = 1;

	if(num_track<MUSIC_MAXTRACK)
	{
		switch (param){
/*			case TRACK_STATE:
				music_tab[num_song].s_track[num_track].t_state = (value & 0xFF);
			break;
*/
			case TRACK_MIDICHANNEL:
				music_tab[num_song].s_track[num_track].t_midichannel = (value & 0xFF);
			break;
/*
			case TRACK_LOOPMOD:
				music_tab[num_song].s_track[num_track].t_loopmod = (value & 0xFF);
			break;
*/
			case TRACK_CURRENTLOOP:
				music_tab[num_song].s_track[num_track].t_currentloop = (value & 0xFF);
			break;
/*
			case TRACK_LEARN:
				music_tab[num_song].s_track[num_track].t_learn = (value & 0xFF);
			break;
*/
			default:
				ret = -1;
			break;
		}
	}
	else
	{
		switch (param){
			case SONG_VOLUME:
				music_tab[num_song].s_volume = (value & 0xFF);
			break;

			case SONG_TEMPO:
				music_tab[num_song].s_tempo = (value & 0xFF);
			break;

			case SONG_VOLTEMPO:
				music_tab[num_song].s_voltempo = (value & 0xFF);
			break;

			case SONG_REFTRACK:
				music_tab[num_song].s_reftrack = (value & 0xFF);
			break;

			case SONG_CURRENTTRACK:
				music_tab[num_song].s_currenttrack = (value & 0xFF);
			break;
/*
			case SONG_1STLOOPMODE:
				music_tab[num_song].s_1stloopmode = (value & 0xFF);
			break;
*/
			case SONG_TOTALSAMPLE:
				music_tab[num_song].s_totalsample = (value & 0xFFFF);
			break;

			case SONG_TIMER:
				music_tab[num_song].s_looptimer = value;
			break;

			case SONG_ID:
				music_tab[num_song].s_id = value;
			break;

			case SONG_NAME:
				for(i=0; i<MUSIC_SONG_NAME_SIZE; i++)
					music_tab[num_song].s_name[i] = buffer[i];
			break;

			case SONG_VERSION_SONG:
				music_tab[num_song].s_version_song = value;
			break;

			default:
				ret = -1;
			break;
		}
	}

	get_musicsamples( (s_total_buffer *)exchange_buffer, num_song);
	mem_cpy((uint8_t *)&(music_tab[num_song]), (uint8_t *)&(exchange_buffer->local_song), MUSIC_SONG_SIZE);
	save_musicsamples( (s_total_buffer *)exchange_buffer, num_song, exchange_buffer->local_song.s_totalsample * MUSIC_SAMPLE_SIZE);

	return ret;
}

/********** get song memory ********************/
void music_getmemsong(uint32_t num_song, uint32_t address, uint8_t *buf)
{
	int32_t i;
	uint32_t tmp;
	uint8_t * temp_ech;

	if(num_song>=dt_config.nb_key)
		return;

	if((address>=(music_tab[num_song].s_totalsample * MUSIC_SAMPLE_SIZE)+MUSIC_SONG_SIZE))
		return;

	if(address == 0)
	{
		music_tab[num_song].s_version_song |= 0x80000000;
	}

	if(address<(MUSIC_SONG_SIZE - 32))
	{
		temp_ech = (uint8_t *)&music_tab[num_song];

		for(i = 0; i<32; i++)
		{
			buf[i] = temp_ech[address + i];
		}
	}
	else if(address<MUSIC_SONG_SIZE)
	{
		temp_ech = (uint8_t *)&music_tab[num_song];

		tmp = address%32;

		for(i = 0; i<tmp; i++)
		{
			buf[i] = temp_ech[address + i];
		}

		get_musicdata( num_song, &buf[tmp], address + tmp, 32 - tmp);
	}
	else
	{
		get_musicdata( num_song, buf, address, 32);
	}
}

/********** clear song info ********************/
void music_clearmemsong(uint32_t num_song)
{
	if(num_song>=dt_config.nb_key)
		return;

	erase_music(num_song);

	if(num_song == record_currentnumsong)
		set_panic(0);

	init_music(&(music_tab[num_song]), num_song);

	if(num_song == record_currentnumsong)
		recordTask_loadsong(num_song);

}

/********** write song info ********************/
void music_setmemsong(uint32_t num_song, uint32_t address, uint8_t *buf)
{
	uint32_t i;
	uint8_t *p_exchange_buffer = (uint8_t *)exchange_buffer;
	static uint32_t stat_address = 0;

	if(num_song>=dt_config.nb_key)
		return;

	if(address == 0xFFFFFFFF)
	{
		music_clearmemsong(num_song);
		save_musicsamples( (s_total_buffer *)exchange_buffer, num_song, stat_address /*exchange_buffer->local_song.s_totalsample * MUSIC_SAMPLE_SIZE*/);
		init_music(&(music_tab[num_song]), num_song);
	}
	else
	{
		for(i=0; i<32; i++)
			p_exchange_buffer[i + address] = buf[i];

		stat_address = address+i;
	}
}


/*************** mechanismes for linked loop parameters copy ********************/
void cp_linkedloop_parameters(music_instr *to_music_instr, music_instr *from_music_instr)
{
	to_music_instr->i_preset.s_volume = from_music_instr->i_preset.s_volume;
	to_music_instr->i_preset.s_panning = from_music_instr->i_preset.s_panning;
	to_music_instr->i_preset.s_sendtorev = from_music_instr->i_preset.s_sendtorev;
	to_music_instr->i_preset.s_portamento_on_off = from_music_instr->i_preset.s_portamento_on_off;
	to_music_instr->i_preset.s_portamento_ctrl = from_music_instr->i_preset.s_portamento_ctrl;
	to_music_instr->i_preset.s_portamento_time = from_music_instr->i_preset.s_portamento_time;
	to_music_instr->i_preset.s_expression = from_music_instr->i_preset.s_expression;
	to_music_instr->i_preset.s_pitch_bend_sensitivity = from_music_instr->i_preset.s_pitch_bend_sensitivity;
	to_music_instr->i_preset.s_adsr_attack = from_music_instr->i_preset.s_adsr_attack;
	to_music_instr->i_preset.s_adsr_release = from_music_instr->i_preset.s_adsr_release;
	to_music_instr->i_preset.s_wah_type = from_music_instr->i_preset.s_wah_type;
	to_music_instr->i_preset.s_wah_freq = from_music_instr->i_preset.s_wah_freq;
	to_music_instr->i_preset.s_wah_res = from_music_instr->i_preset.s_wah_res;

	memcpy(&(to_music_instr->i_mix), &(from_music_instr->i_mix), FX_MIX_SIZE);
	memcpy(&(to_music_instr->i_distortion), &(from_music_instr->i_distortion), FX_DIST_SIZE);
	memcpy(&(to_music_instr->i_compressor), &(from_music_instr->i_compressor), FX_COMP_SIZE);
	memcpy(&(to_music_instr->i_equalizer), &(from_music_instr->i_equalizer), FX_EQ_SIZE);
	memcpy(&(to_music_instr->i_chorus), &(from_music_instr->i_chorus), FX_CHORUS_SIZE);
	memcpy(&(to_music_instr->i_delay), &(from_music_instr->i_delay), FX_DELAY_SIZE);
}

void process_cpparam_iflinkedloop(uint32_t track, uint32_t loop)
{
	uint32_t i;

	// does not apply for harmonic instruments
	if(music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[loop].l_instr.i_instrument.instr_type == INSTR_HARMONIC)
		return;

	for(i=0; i<MUSIC_MAXLAYER; i++)
	{ // check if it's the same instrument
		if((music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[i].l_instr.i_instrument.instr_type != INSTR_HARMONIC) && (i != loop))
		{
			if(music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[i].l_instr.i_instrument.instr_midi_pc == music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[loop].l_instr.i_instrument.instr_midi_pc)
			{
				if(music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[i].l_instr.i_instrument.instr_midi_C0 == music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[loop].l_instr.i_instrument.instr_midi_C0)
				{ // if it's the case copy parameters
					cp_linkedloop_parameters(&(music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[i].l_instr), &(music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[loop].l_instr));
				}
			}
		}
	}
}
