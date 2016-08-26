/*
 * music_parameters_mng.h
 *
 *  Created on: 07/08/2013
 *      Author: mma
 */


#ifndef MUSIC_PARAMETERS_MNG_H_
#define MUSIC_PARAMETERS_MNG_H_

#include "parameters_mng.h"
#include "sound_parameters_mng.h"
#include "metadata_parameters_mng.h"
#include "arrangement_parameters_mng.h"

/******** general defines ************/
//#define RECORD_MAXSONG		58 //3 //58
#define MUSIC_MAXTRACK		7
#define MUSIC_MAXLAYER		4

#define MUSIC_SONG_NAME_SIZE			32 //12
#define MUSIC_SONG_OWNER_STR_SIZE		0x010

#define MUSIC_DEFAULTREVERB	1

#define VERSION_MUSIC	3 //1

/***** controler parameters structure*****/
typedef struct
{
    uint32_t	time;
    uint8_t	control;
    uint8_t	canal;
    uint8_t	note;
    uint8_t	value;
}music_sample;

#ifdef __LPC177X_8X__
typedef music_sample* music_sample_p;
#else
typedef uint32_t music_sample_p;
#endif

#define MUSIC_SAMPLE_SIZE	8

typedef struct
{
	info_instr i_instrument;

	preset_instr	i_preset;
}music_instr;

#define MUSIC_INSTRU_SIZE	(INSTR_INFO_SIZE + PRESET_STRUCT_SIZE)


typedef struct
{
    uint8_t	l_state;
    uint8_t l_loopmod;
    uint8_t l_learn;
    uint8_t l_midioutchannel;
    uint32_t l_savelooptimer; //uint8_t dummy[4];
    music_instr l_instr;
    music_sample_p l_adress;
    uint16_t l_numsample;
    uint8_t dummy2[2];
}music_loop;

#define MUSIC_LOOP_SIZE	(4 + 4 + MUSIC_INSTRU_SIZE + 4 + 2 + 2)

enum {
    LOOP_STATE,
    LOOP_MOD,
    LOOP_LEARN,
    LOOP_MIDIOUTCHANNEL,
    NUM_INFOLOOP
};

typedef struct
{
    uint8_t	t_midichannel;
    uint8_t t_currentloop;
    uint8_t	dummy[2];
    music_loop t_loop[MUSIC_MAXLAYER];
}music_track;

#define MUSIC_TRACK_SIZE	(2 + 2 + (MUSIC_LOOP_SIZE * MUSIC_MAXLAYER))

enum TRACK_INFO {
    TRACK_MIDICHANNEL,
    TRACK_CURRENTLOOP,
    NUM_INFOTRACK
};

typedef struct
{
	uint32_t s_version_music; // music struct version
    uint8_t s_original_sn[MUSIC_SONG_OWNER_STR_SIZE];
    uint8_t s_original_name[MUSIC_SONG_OWNER_STR_SIZE];
    uint8_t s_original_user[MUSIC_SONG_OWNER_STR_SIZE];
    uint8_t s_original_userid[MUSIC_SONG_OWNER_STR_SIZE];
    uint8_t s_modif_sn[MUSIC_SONG_OWNER_STR_SIZE];
    uint8_t s_modif_name[MUSIC_SONG_OWNER_STR_SIZE];
    uint8_t s_modif_user[MUSIC_SONG_OWNER_STR_SIZE];
    uint8_t s_modif_userid[MUSIC_SONG_OWNER_STR_SIZE];

	uint32_t s_size;
	uint32_t s_metadata;

	uint8_t s_playhead;
	uint8_t s_transpose;
	uint8_t s_state;

	uint8_t dummyheader[2];// remember to keep some space for id growing
	int8_t s_direction_gyro_P;
	int8_t s_direction_gyro_R;
	int8_t s_direction_gyro_Y;
	uint16_t s_activ_aftertouch;
	uint16_t s_activ_slider_L;
	uint16_t s_activ_slider_R;
	uint16_t s_activ_gyro_P;
	uint16_t s_activ_gyro_R;
	uint16_t s_activ_gyro_Y;

	uint32_t s_id;
	uint8_t	s_name[MUSIC_SONG_NAME_SIZE];
	uint32_t s_version_song; // file version

	uint8_t s_volume;
	uint8_t s_tempo;
	uint8_t s_voltempo;
	uint8_t s_reftrack;
	uint8_t s_currenttrack;
	uint8_t s_decaltempo;
	uint16_t s_totalsample;
	uint32_t s_looptimer;
	uint8_t s_leds[NUM_LED_VALUE];
	FX_mix	s_mix;
	uint8_t dummy[2];
	uint8_t s_swing;
	uint8_t s_quantification;
	uint8_t s_displaynote;
	uint8_t s_scaletonality;
	uint8_t s_timesignature;

	uint8_t s_reverb_preset;
	FX_reverb	s_reverb;

	music_track	s_track[MUSIC_MAXTRACK];

}music_song;

#define MUSIC_SONG_SIZE    (4 + (8*(MUSIC_SONG_OWNER_STR_SIZE)) + 32 + MUSIC_SONG_NAME_SIZE + 4 + 12 + NUM_LED_VALUE + FX_MIX_SIZE + 8 + FX_REVERB_SIZE + (MUSIC_TRACK_SIZE * MUSIC_MAXTRACK))

#define RECORD_SAMPLEBUFFERSIZE	0x10000

typedef struct
{
    music_song local_song;
    music_sample local_buffer[RECORD_SAMPLEBUFFERSIZE];
} s_total_buffer;

enum SONG_INFO {
	SONG_VERSION_MUSIC,
	SONG_ORIGINAL_SN = 0x04,
	SONG_ORIGINAL_NAME = 0x14,
	SONG_ORIGINAL_USER = 0x24,
	SONG_ORIGINAL_USERID = 0x34,
	SONG_MODIF_SN = 0x44,
	SONG_MODIF_NAME = 0x54,
	SONG_MODIF_USER = 0x64,
	SONG_MODIF_USERID = 0x74,

	SONG_SIZE = 0x84,

	SONG_PLAYHEAD = 0x8C,
	SONG_TRANSPOSE,

	SONG_ID = 0xA0,
	SONG_NAME = 0xA4,
	SONG_VERSION_SONG = 0xC4,


	SONG_VOLUME = 0xC8,
	SONG_TEMPO,
	SONG_VOLTEMPO,
	SONG_REFTRACK,
	SONG_CURRENTTRACK,
	SONG_DECALTEMPO,
	SONG_TOTALSAMPLE,
	SONG_DUMMY,
	SONG_TIMER,

	SONG_LEDS = 0xD4,
	SONG_MIX = 0xF4,

	SONG_SWING = 0xFE,
	SONG_QUANTIZE = 0xFF,
	SONG_DISPLAYNOTE = 0x100,
	SONG_SCALTONALITY = 0x101,
	SONG_TIMESIGNATURE = 0x102,
	SONG_REVERBPRESET = 0x103,

	NUM_INFOSONG
};



// defines




enum{ // CHANNEL CONTROL
    REC_EMPTY,
    REC_RECORD,
    REC_PLAY,
    REC_STOP,
    REC_PAUSE,
    REC_RECORDWAIT,
    REC_STATE_NUM
};

enum{ // LEARN MODE
    LEARN_OFF,
    LEARN_ON,
    LEARN_TWOBEAT,
    LEARN_ONEBEAT,
    LEARN_HALFBEAT,
    LEARN_QUARTERBEAT,
    LEARN_STEPBYSTEP,
    NUM_LEARNMODE
};

enum{ // TIME SIGNATURE
    TIME_OFF,
    TIME_2_4,
    TIME_3_4,
    TIME_4_4,
    TIME_5_4,
    NUM_TIMESIGNATURE
};


#define MAX_BOUCLETIMER_MAXMODULO	64

#define MUSIC_INIT_BOUCLETIMER 	(0xFFFFFFFF / MAX_BOUCLETIMER_MAXMODULO)

#define	MUSIC_VOL_MINVALUE			0
#define	MUSIC_VOL_MAXVALUE			127
#define	MUSIC_VOL_DEFAULTVALUE		127

#define MUSIC_TEMPO_MINVALUE		40
#define MUSIC_TEMPO_MAXVALUE		208
#define MUSIC_TEMPO_DEFAULTVALUE	120

#define	MUSIC_TEMPOVOL_MINVALUE			0
#define	MUSIC_TEMPOVOL_MAXVALUE			127
#define	MUSIC_TEMPOVOL_DEFAULTVALUE		0 //0

#define	MUSIC_LOOPMOD_MINVALUE			0
#define	MUSIC_LOOPMOD_MAXVALUE			127
#define	MUSIC_LOOPMOD_DEFAULTVALUE		0

#define RECORD_TRANSPOSEMAX		12 //6
#define RECORD_TRANSPOSEDEFAULT	(RECORD_TRANSPOSEMAX) // 6 - 6 = 0

////////////////////////////////////////
//#ifdef __LPC177X_8X__
void init_musicmemory(void);
void init_music(music_song *song_struct, uint32_t song);
int32_t get_musicsamples(s_total_buffer *music_struct, uint32_t song);
int32_t save_musicsamples(s_total_buffer *music_struct, uint32_t song, uint32_t size);
void get_musicdata(uint32_t song, uint8_t *buffer, uint32_t address, uint32_t length);
void save_musicdata(uint32_t song, uint8_t *buffer, uint32_t address, uint32_t length);
void init_music_instr(music_instr *instr_struct, uint32_t instr);
void erase_music_instr_name(music_instr *instr_struct);
void cp_music_instr(music_instr *music_instr_struct, sound_instr* instr_struct, uint32_t preset);
void check_music_instr(music_instr *instr_struct);
void save_music(music_song *song_struct, uint32_t song);
void erase_music( uint32_t song);
int32_t load_defaultmusic(music_song *song_struct);
int32_t save_defaultmusic(music_song *song_struct);

uint32_t get_musicmetadatasize(void);
int32_t get_musicarrangement(s_arrangement *arrangement_struct, uint32_t metadataaddress, uint32_t metadatasize, uint32_t song);
int32_t save_musicarrangement(uint32_t metadataaddress, uint32_t metadatasize, uint32_t song);

void setmusiclearn(param_struct *param, int32_t value, uint32_t rec);
void setmusicvolume(param_struct *param, int32_t value, uint32_t rec);
void setmusicmix(param_struct *param, int32_t value, uint32_t rec);
void setmusictempo(param_struct *param, int32_t value, uint32_t rec);
void setmusictempovolume(param_struct *param, int32_t value, uint32_t rec);
void setmusictempomode(param_struct *param, int32_t value, uint32_t rec);
void setmusicplayhead(param_struct *param, int32_t value, uint32_t rec);
void switchledmode(param_struct *param, int32_t value, uint32_t rec);
void switchledmode_ps(param_struct *param, int32_t value, uint32_t rec);
void transpose(param_struct *param, int32_t value, uint32_t rec);
void setarrangementstep(param_struct *param, int32_t value, uint32_t rec);
void setmusicbeatrepeat(param_struct *param, int32_t value, uint32_t rec);
void setloopbeatrepeat(param_struct *param, int32_t value, uint32_t rec);
void change_scale(uint32_t key, uint32_t keyboard);

void get_musicname(uint32_t song, char *string);

uint32_t music_recovery(music_song *song_struct);
void convertmusic_V1toV2(music_song *song_struct, uint32_t song_num);

extern inline uint32_t music_strack(void);
extern inline uint32_t music_sloop(void);

extern inline uint32_t music_cloop(void);

void process_cpparam_iflinkedloop(uint32_t track, uint32_t loop);

// USB control//////////////////////////////////
int32_t music_getinfosong_param(uint32_t num_song, uint32_t num_track, uint32_t param, uint8_t *buffer);
int32_t music_setinfosong_param(uint32_t num_song, uint32_t num_track, uint32_t param, uint8_t *buffer);
void music_getmemsong(uint32_t num_song, uint32_t address, uint8_t *buf);
void music_clearmemsong(uint32_t num_song);
void music_setmemsong(uint32_t num_song, uint32_t address, uint8_t *buf);
//#endif // __LPC177X_8X__

#endif // MUSIC_PARAMETERS_MNG_H_
