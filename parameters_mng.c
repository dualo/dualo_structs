/*
 * parameters_mng.c
 *
 *  Created on: 08/08/2013
 *      Author: mma
 */

#include "parameters_mng.h"
#include "controler_parameters_mng.h"
#include "sound_parameters_mng.h"
#include "music_parameters_mng.h"
#include "midi_parameters_mng.h"
#include "arrangement_parameters_mng.h"
#include "g_parameters_mng.h"
#include "display_convert.h"
#include "controller_middleware/controller_middleware.h"
#include "looper_timer/looptimer_interface.h"
#include "debug/trace.h"
#include "config.h"
#include "layer.h"

#include "screen_task.h"
#include "record_task.h"

#include "copy/copyandpast.h"

#ifdef __LPC18XX__
#include "screen/screen.h"
#include "memory_dispach.h"
#include "nrf/nrf.h"

SECTION_EXTRAM
struct_controler controler_tab;
SECTION_EXTRAM
sound_instr instr_tab[2*NUM_BUTTON_KEYBOARD];
SECTION_EXTRAM
sound_instr instr_sys;
SECTION_EXTRAM
music_song music_tab[NUM_BUTTON_KEYBOARD];
SECTION_EXTRAM
struct_miditable midi_tab[MIDITABLE_MAX];

#else
#include "screen/DT018ATFT.h"

__attribute__ ((section(".extram")))
struct_controler controler_tab;
__attribute__ ((section(".extram")))
sound_instr instr_tab[2*NUM_BUTTON_KEYBOARD];
__attribute__ ((section(".extram")))
sound_instr instr_sys;
__attribute__ ((section(".extram")))
music_song music_tab[NUM_BUTTON_KEYBOARD];
__attribute__ ((section(".extram")))
struct_miditable midi_tab[MIDITABLE_MAX];

#endif
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_mix			temp_mix;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_distortion	temp_distortion;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_compressor	temp_compressor;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_equalizer	temp_equalizer;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_equalizer	temp_eqmain;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_chorus		temp_chorus;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_delay		temp_delay;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
FX_reverb		temp_reverb;
#ifdef __LPC18XX__
SECTION_EXTRAM
#endif
preset_instr	temp_preset_instr;

extern music_instr tmp_loop_instr[MUSIC_MAXTRACK][MUSIC_MAXLAYER];

extern layer_t layer_play;

static uint32_t local_loop;
static uint32_t local_track;

uint8_t dummyparam;

//extern uint32_t v_transpose;
//extern uint32_t v_quantification;
extern uint32_t v_metronome;
extern uint32_t layer_play_record;

extern s_arrangement arrangement_buffer;

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	dummy_paramstruct =
// 			p_struct,		p_param,				p_name,			p_min,				p_max,				p_default,			p_sticky,			p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0000};


#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	controler_vol_param =
// 			p_struct,		p_param,				p_name,			p_min,				p_max,				p_default,			p_sticky,			p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	CONTROLER,		CONTROLER_MAINVOLUME,	"volume",		MAIN_VOLUME_MIN,	MAIN_VOLUME_MAX,	MAIN_VOLUME_MIN,	MAIN_VOLUME_DEFAULT,SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setmainvolume, 	(uint32_t)displaypercent,	0,			0,			0,				NULL,			NULL,			0x00,	0x0010};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_vol_param =
// 			p_struct,		p_param,				p_name,			p_min,				p_max,				p_default,			p_sticky,			p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	INSTR_PRESET,	SOUND_PRESET_VOLUME,	"sound volume",	MAIN_VOLUME_MIN,	MAIN_VOLUME_MAX,	MAIN_VOLUME_MIN,	MAIN_VOLUME_DEFAULT,SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setinstrvolume,	(uint32_t)displaypercent,	0,			0,			0,				NULL,			NULL,			0x00,	0x0011};

//__attribute__ ((section(".intflash")))
//const param_struct	music_vol_param =
//// 			p_struct,		p_param,				p_name,			p_min,				p_max,				p_default,			p_sticky,			p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
//		{	MUSIC_SONG,		SONG_VOLUME,			"d-m volume",	MAIN_VOLUME_MIN,   MAIN_VOLUME_MAX,		MAIN_VOLUME_MIN,		MAIN_VOLUME_MIN,	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setmusicvolume, 	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0012};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_head_param =
// 			p_struct,		p_param,				p_name,			p_min,				p_max,				p_default,			p_sticky,			p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
	{	MUSIC_SONG,		SONG_PLAYHEAD,				"play-head",	0,        			255,				0,					0,					SCREEN_DISPLAY_TIMECIRCLEPOSITION,	0,					CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)NULL,				(uint32_t)displaytimecircle,NULL,							0,					0,				NULL,			NULL,			0x00,	0x0012};


//__attribute__ ((section(".intflash")))
param_struct	music_dugame_param =
// 			p_struct,		p_param,				p_name,			p_min,				p_max,				p_default,			p_sticky,			p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	OTHERS,			PARAM_DUGAME,			"du-game step",	0,   		(ARRANGEMENT_MAXEVENT - 1),	0,					0,					SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MAX,		0,				0,					(uint32_t)setarrangementstep,(uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0013};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	controler_eq_sensor[FX_PEQ_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,			p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	GENERAL_EQ,		FX_PEQ_LBGAIN,			"low gain",		FX_PEQ_LBGAIN_MINVALUE,     		FX_PEQ_LBGAIN_MAXVALUE,     	FX_PEQ_LBGAIN_DEFAULTVALUE,			FX_PEQ_LBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setgeneraleq,	(uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0530},
		{	GENERAL_EQ,		FX_PEQ_LMBGAIN,			"mid-l gain",	FX_PEQ_LMBGAIN_MINVALUE,    		FX_PEQ_LMBGAIN_MAXVALUE,    	FX_PEQ_LMBGAIN_DEFAULTVALUE,		FX_PEQ_LMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setgeneraleq,	(uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0531},
		{	GENERAL_EQ,		FX_PEQ_HMBGAIN,			"mid-h gain",	FX_PEQ_HMBGAIN_MINVALUE,    		FX_PEQ_HMBGAIN_MAXVALUE,    	FX_PEQ_HMBGAIN_DEFAULTVALUE,		FX_PEQ_HMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setgeneraleq,	(uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0532},
		{	GENERAL_EQ,		FX_PEQ_HBGAIN,			"high gain",	FX_PEQ_HBGAIN_MINVALUE,     		FX_PEQ_HBGAIN_MAXVALUE,     	FX_PEQ_HBGAIN_DEFAULTVALUE,			FX_PEQ_HBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setgeneraleq,	(uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0533},
		//{	GENERAL_EQ,		FX_PEQ_LBFREQ,			"low freq",		FX_PEQ_LBFREQ_MINVALUE,     		FX_PEQ_LBFREQ_MAXVALUE,     	FX_PEQ_LBFREQ_DEFAULTVALUE,			FX_PEQ_LBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,						0,					(uint32_t)setgeneraleq,		(uint32_t)displayeq_low_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_LMBFREQ,			"mid-l freq",	FX_PEQ_LMBFREQ_MINVALUE,    		FX_PEQ_LMBFREQ_MAXVALUE,    	FX_PEQ_LMBFREQ_DEFAULTVALUE,		FX_PEQ_LMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,						0,					(uint32_t)setgeneraleq,		(uint32_t)displayeq_midl_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_HMBFREQ,			"mid-h freq",	FX_PEQ_HMBFREQ_MINVALUE,    		FX_PEQ_HMBFREQ_MAXVALUE,    	FX_PEQ_HMBFREQ_DEFAULTVALUE,		FX_PEQ_HMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,						0,					(uint32_t)setgeneraleq,		(uint32_t)displayeq_midh_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_HBFREQ,			"high freq",	FX_PEQ_HBFREQ_MINVALUE,     		FX_PEQ_HBFREQ_MAXVALUE,     	FX_PEQ_HBFREQ_DEFAULTVALUE,			FX_PEQ_HBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,						0,					(uint32_t)setgeneraleq,	 	(uint32_t)displayeq_high_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_LMBQ, 			"mid-l q.",		FX_PEQ_LMBQ_MINVALUE,       		FX_PEQ_LMBQ_MAXVALUE,       	FX_PEQ_LMBQ_DEFAULTVALUE,			FX_PEQ_LMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,						0,					(uint32_t)setgeneraleq,	 	(uint32_t)NULL,				NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_HMBQ,			"mid-h q.",		FX_PEQ_HMBQ_MINVALUE,       		FX_PEQ_HMBQ_MAXVALUE,       	FX_PEQ_HMBQ_DEFAULTVALUE,			FX_PEQ_HMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,						0,					(uint32_t)setgeneraleq,	 	(uint32_t)NULL,				NULL,		0,			0}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	controler_eq_param[FX_PEQ_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	GENERAL_EQ,		FX_PEQ_LBGAIN,			"low gain",		FX_PEQ_LBGAIN_MINVALUE,     		FX_PEQ_LBGAIN_MAXVALUE,     	FX_PEQ_LBGAIN_DEFAULTVALUE,			FX_PEQ_LBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,	 	(uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&controler_eq_sensor[0],			NULL,			0x00,	0x0030},
		{	GENERAL_EQ,		FX_PEQ_LMBGAIN,			"mid-l gain",	FX_PEQ_LMBGAIN_MINVALUE,    		FX_PEQ_LMBGAIN_MAXVALUE,    	FX_PEQ_LMBGAIN_DEFAULTVALUE,		FX_PEQ_LMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,	 	(uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&controler_eq_sensor[1],			NULL,			0x00,	0x0031},
		{	GENERAL_EQ,		FX_PEQ_HMBGAIN,			"mid-h gain",	FX_PEQ_HMBGAIN_MINVALUE,    		FX_PEQ_HMBGAIN_MAXVALUE,    	FX_PEQ_HMBGAIN_DEFAULTVALUE,		FX_PEQ_HMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,	 	(uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&controler_eq_sensor[2],			NULL,			0x00,	0x0032},
		{	GENERAL_EQ,		FX_PEQ_HBGAIN,			"high gain",	FX_PEQ_HBGAIN_MINVALUE,     		FX_PEQ_HBGAIN_MAXVALUE,     	FX_PEQ_HBGAIN_DEFAULTVALUE,			FX_PEQ_HBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,	 	(uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&controler_eq_sensor[3],			NULL,			0x00,	0x0033},
		//{	GENERAL_EQ,		FX_PEQ_LBFREQ,			"low freq",		FX_PEQ_LBFREQ_MINVALUE,     		FX_PEQ_LBFREQ_MAXVALUE,     	FX_PEQ_LBFREQ_DEFAULTVALUE,			FX_PEQ_LBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,		(uint32_t)displayeq_low_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_LMBFREQ,			"mid-l freq",	FX_PEQ_LMBFREQ_MINVALUE,    		FX_PEQ_LMBFREQ_MAXVALUE,    	FX_PEQ_LMBFREQ_DEFAULTVALUE,		FX_PEQ_LMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,		(uint32_t)displayeq_midl_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_HMBFREQ,			"mid-h freq",	FX_PEQ_HMBFREQ_MINVALUE,    		FX_PEQ_HMBFREQ_MAXVALUE,    	FX_PEQ_HMBFREQ_DEFAULTVALUE,		FX_PEQ_HMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,		(uint32_t)displayeq_midh_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_HBFREQ,			"high freq",	FX_PEQ_HBFREQ_MINVALUE,     		FX_PEQ_HBFREQ_MAXVALUE,     	FX_PEQ_HBFREQ_DEFAULTVALUE,			FX_PEQ_HBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,	 	(uint32_t)displayeq_high_freq,NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_LMBQ, 			"mid-l q.",		FX_PEQ_LMBQ_MINVALUE,       		FX_PEQ_LMBQ_MAXVALUE,       	FX_PEQ_LMBQ_DEFAULTVALUE,			FX_PEQ_LMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,	 	(uint32_t)NULL,				NULL,		0,			0},
		//{	GENERAL_EQ,		FX_PEQ_HMBQ,			"mid-h q.",		FX_PEQ_HMBQ_MINVALUE,       		FX_PEQ_HMBQ_MAXVALUE,       	FX_PEQ_HMBQ_DEFAULTVALUE,			FX_PEQ_HMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setgeneraleq,	 	(uint32_t)NULL,				NULL,		0,			0}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	controler_reverb_sensor[FX_REVERB_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	GENERAL_REVERB,	FX_REVERB_LEVEL,		"rev level",	FX_REVERB_LEVEL_MINVALUE,			FX_REVERB_LEVEL_MAXVALUE,		FX_REVERB_LEVEL_MAXVALUE,			FX_REVERB_LEVEL_MINVALUE,			SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0540},
		//{	GENERAL_REVERB,	FX_REVERB_DIRECTLEVEL, 	"dry level",	FX_REVERB_DIRECTLEVEL_MINVALUE,		FX_REVERB_DIRECTLEVEL_MAXVALUE,	FX_REVERB_DIRECTLEVEL_MAXVALUE,     FX_REVERB_DIRECTLEVEL_MINVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0541},
		{	GENERAL_REVERB,	FX_REVERB_REVSEND,		"level",		FX_REVERB_REVSEND_MINVALUE,			FX_REVERB_REVSEND_MAXVALUE,		FX_REVERB_REVSEND_MINVALUE,    		FX_REVERB_REVSEND_MAXVALUE,			SCREEN_DISPLAY_PERCENTAGE,	0,							0,					0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_230,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0542},
		//{	GENERAL_REVERB,	FX_REVERB_TONEGAIN,		"tone gain",	FX_REVERB_TONEGAIN_MINVALUE,    	FX_REVERB_TONEGAIN_MAXVALUE,	FX_REVERB_TONEGAIN_MAXVALUE,    	FX_REVERB_TONEGAIN_MINVALUE, 		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevtonegain,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0543},
		//{	GENERAL_REVERB,	FX_REVERB_TONEFREQ, 	"tone freq",	FX_REVERB_TONEFREQ_MINVALUE,    	FX_REVERB_TONEFREQ_MAXVALUE,	FX_REVERB_TONEFREQ_MAXVALUE,     	FX_REVERB_TONEFREQ_MINVALUE,     	SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0544},
		//{	GENERAL_REVERB,	FX_REVERB_PREHP, 		"h-pass freq",	FX_REVERB_PREHP_MINVALUE,     		FX_REVERB_PREHP_MAXVALUE,		FX_REVERB_PREHP_MAXVALUE,     		FX_REVERB_PREHP_MINVALUE,     		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0545},
		{	GENERAL_REVERB,	FX_REVERB_TIME,			"time",			FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,		FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,    		SCREEN_DISPLAY_TIME,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setgeneralreverb,	(uint32_t)displayrevtime,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0546},
		//{	GENERAL_REVERB,	FX_REVERB_ECHOFEEDBACK,	"echofeedback",	FX_REVERB_ECHOFEED_MINVALUE,    	FX_REVERB_ECHOFEED_MAXVALUE,	FX_REVERB_ECHOFEED_MAXVALUE,    	FX_REVERB_ECHOFEED_MINVALUE,    	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0547},
		//{	GENERAL_REVERB,	FX_REVERB_HDAMP,		"h-damp",		FX_REVERB_HDAMP_MINVALUE,     		FX_REVERB_HDAMP_MAXVALUE,		FX_REVERB_HDAMP_MAXVALUE,     		FX_REVERB_HDAMP_MINVALUE,     		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0548},
		//{	GENERAL_REVERB,	FX_REVERB_THRESGATE,	"thresgate",	FX_REVERB_THRESGATE_MINVALUE,   	FX_REVERB_THRESGATE_MAXVALUE,	FX_REVERB_THRESGATE_MAXVALUE,       FX_REVERB_THRESGATE_MINVALUE,      	SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0549},
		{	GENERAL_REVERB,	FX_REVERB_PREDELAYTIME,	"predelaytime",	FX_REVERB_PREDELAYTIME_MINVALUE,	FX_REVERB_PREDELAYTIME_MAXVALUE,FX_REVERB_PREDELAYTIME_MINVALUE,    FX_REVERB_PREDELAYTIME_MAXVALUE,    SCREEN_DISPLAY_TIME,		0,							0,					0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevpredeltime,	NULL,	0,			0,				NULL,			NULL,			0x00,	0x054a},
		//{	GENERAL_REVERB,	FX_REVERB_DRY_WET,		"dry / wet",	FX_REVERB_DRYWET_MINVALUE,			FX_REVERB_DRYWET_MAXVALUE,		FX_REVERB_DRYWET_MAXVALUE,    		FX_REVERB_DRYWET_MINVALUE,    		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			00x00,	x054b}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	controler_reverb_param[FX_REVERB_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	GENERAL_REVERB,	FX_REVERB_LEVEL,		"rev level",	FX_REVERB_LEVEL_MINVALUE,			FX_REVERB_LEVEL_MAXVALUE,		FX_REVERB_LEVEL_MAXVALUE,			FX_REVERB_LEVEL_MINVALUE,			SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0040},
		//{	GENERAL_REVERB,	FX_REVERB_DIRECTLEVEL, 	"dry level",	FX_REVERB_DIRECTLEVEL_MINVALUE,		FX_REVERB_DIRECTLEVEL_MAXVALUE,	FX_REVERB_DIRECTLEVEL_MAXVALUE,     FX_REVERB_DIRECTLEVEL_MINVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0041},
		{	GENERAL_REVERB,	FX_REVERB_REVSEND,		"level",		FX_REVERB_REVSEND_MINVALUE,			FX_REVERB_REVSEND_MAXVALUE,		FX_REVERB_REVSEND_MINVALUE,    		FX_REVERB_REVSEND_MAXVALUE,			SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_230,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0042},
		//{	GENERAL_REVERB,	FX_REVERB_TONEGAIN,		"tone gain",	FX_REVERB_TONEGAIN_MINVALUE,    	FX_REVERB_TONEGAIN_MAXVALUE,	FX_REVERB_TONEGAIN_MAXVALUE,    	FX_REVERB_TONEGAIN_MINVALUE, 		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevtonegain,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0043},
		//{	GENERAL_REVERB,	FX_REVERB_TONEFREQ, 	"tone freq",	FX_REVERB_TONEFREQ_MINVALUE,    	FX_REVERB_TONEFREQ_MAXVALUE,	FX_REVERB_TONEFREQ_MAXVALUE,     	FX_REVERB_PREHP_MINVALUE,     		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0044},
		//{	GENERAL_REVERB,	FX_REVERB_PREHP, 		"h-pass freq",	FX_REVERB_PREHP_MINVALUE,     		FX_REVERB_PREHP_MAXVALUE,		FX_REVERB_PREHP_MAXVALUE,     		FX_REVERB_PREHP_MINVALUE,			0,							CTRL_EVENT_REL_MIN,	0,				(uint32_t)setgeneralreverb,	0,					(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x0045},
		{	GENERAL_REVERB,	FX_REVERB_TIME,			"time",			FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,		FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,    		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevtime,	NULL,		0,			0,(uint32_t *)&controler_reverb_sensor[1],		NULL,			0x00,	0x0046},
		//{	GENERAL_REVERB,	FX_REVERB_ECHOFEEDBACK,	"echofeedback",	FX_REVERB_ECHOFEED_MINVALUE,    	FX_REVERB_ECHOFEED_MAXVALUE,	FX_REVERB_ECHOFEED_MAXVALUE,    	FX_REVERB_ECHOFEED_MINVALUE,    	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0047},
		//{	GENERAL_REVERB,	FX_REVERB_HDAMP,		"h-damp",		FX_REVERB_HDAMP_MINVALUE,     		FX_REVERB_HDAMP_MAXVALUE,		FX_REVERB_HDAMP_MAXVALUE,     		FX_REVERB_HDAMP_MINVALUE,     		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0048},
		//{	GENERAL_REVERB,	FX_REVERB_THRESGATE,	"thresgate",	FX_REVERB_THRESGATE_MINVALUE,   	FX_REVERB_THRESGATE_MAXVALUE,	FX_REVERB_THRESGATE_MAXVALUE,       FX_REVERB_THRESGATE_MINVALUE,      	SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0049},
		{	GENERAL_REVERB,	FX_REVERB_PREDELAYTIME,	"predelaytime",	FX_REVERB_PREDELAYTIME_MINVALUE,	FX_REVERB_PREDELAYTIME_MAXVALUE,FX_REVERB_PREDELAYTIME_MINVALUE,    FX_REVERB_PREDELAYTIME_MAXVALUE,    SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevpredeltime,	NULL,	0,			0,				NULL,			NULL,			0x00,	0x004a},
		//{	GENERAL_REVERB,	FX_REVERB_DRY_WET,		"dry / wet",	FX_REVERB_DRYWET_MINVALUE,			FX_REVERB_DRYWET_MAXVALUE,		FX_REVERB_DRYWET_MAXVALUE,    		FX_REVERB_DRYWET_MINVALUE,    		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			0x00,	0x004b}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_compressor_param[FX_COMP_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,							extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_COMPRESSOR,	FX_COMP_ON_OFF,			"state",		FX_COMP_ONOFF_MINVALUE,				FX_COMP_ONOFF_MAXVALUE,			FX_COMP_ONOFF_DEFAULTVALUE,			FX_COMP_ONOFF_DEFAULTVALUE,			SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0050},
		//{	INSTR_COMPRESSOR,	FX_COMP_ATTACKTIME,		"attack time",	FX_COMP_ATTACK_MINVALUE,			FX_COMP_ATTACK_MAXVALUE,     	FX_COMP_ATTACK_DEFAULTVALUE,		FX_COMP_ATTACK_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorattack,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0051},
		//{	INSTR_COMPRESSOR,	FX_COMP_RELEASETIME,	"release time",	FX_COMP_RELEASE_MINVALUE,			FX_COMP_RELEASE_MAXVALUE,    	FX_COMP_RELEASE_DEFAULTVALUE,		FX_COMP_RELEASE_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorrelease,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0052},
		{	INSTR_COMPRESSOR,	FX_COMP_THRESHOLD,		"threshold",	FX_COMP_THRESHOLD_MINVALUE,    		FX_COMP_THRESHOLD_MAXVALUE,    	FX_COMP_THRESHOLD_DEFAULTVALUE,		FX_COMP_THRESHOLD_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaygain_42,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0053},
		{	INSTR_COMPRESSOR,	FX_COMP_RATIO,			"ratio",		FX_COMP_RATIO_MINVALUE,    			FX_COMP_RATIO_MAXVALUE,     	FX_COMP_RATIO_DEFAULTVALUE,			FX_COMP_RATIO_DEFAULTVALUE,			SCREEN_DISPLAY_RATIO,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorratio,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0054},
		{	INSTR_COMPRESSOR,	FX_COMP_BOOST,			"boost",		FX_COMP_BOOST_MINVALUE,     		FX_COMP_BOOST_MAXVALUE,     	FX_COMP_BOOST_DEFAULTVALUE,			FX_COMP_BOOST_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorboost,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0055},
		//{	INSTR_COMPRESSOR,	FX_COMP_KNEE,			"knee",			FX_COMP_KNEE_MINVALUE,    			FX_COMP_KNEE_MAXVALUE,    		FX_COMP_KNEE_DEFAULTVALUE,			FX_COMP_KNEE_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0056},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_wah_sensor[FX_WAH_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_WAH,		FX_WAH_FILTERTYPE,		"filter type",	FX_WAH_FILTERTYPE_MINVALUE,			FX_WAH_FILTERTYPE_MAXVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_FILTERTYPE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah_type,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0560},
		{	INSTR_PRESET,	SOUND_PRESET_WAH_FREQ,	"filter freq",	FX_WAH_FILTERFREQ_MINVALUE,			FX_WAH_FILTERFREQ_MAXVALUE,     FX_WAH_FILTERFREQ_DEFAULTVALUE,		FX_WAH_FILTERFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setinstrwah_freq,	(uint32_t)display8khzfreqwah,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0561},
		{	INSTR_PRESET,	SOUND_PRESET_WAH_RES,	"filter res.",	FX_WAH_FILTERRES_MINVALUE,			FX_WAH_FILTERRES_MAXVALUE,    	FX_WAH_FILTERRES_DEFAULTVALUE,		FX_WAH_FILTERRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah_res,	(uint32_t)displaywahres,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0562},
		//{	INSTR_WAH,		FX_WAH_AUTOSENSITIVITY,	"auto-sens",	FX_WAH_SENSITIVITY_MINVALUE,    	FX_WAH_SENSITIVITY_MAXVALUE,    FX_WAH_SENSITIVITY_DEFAULTVALUE,	FX_WAH_SENSITIVITY_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah,		(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0563},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_wah_param[FX_WAH_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	INSTR_PRESET,	SOUND_PRESET_WAH_TYPE,	"filter type",	FX_WAH_FILTERTYPE_MINVALUE,			FX_WAH_FILTERTYPE_MAXVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_FILTERTYPE,	0,							CTRL_EVENT_REL_MIN,	0,				1,					(uint32_t)setinstrwah_type,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0060},
		//{	INSTR_WAH,		FX_WAH_FILTERFREQ,		"filter freq",	FX_WAH_FILTERFREQ_MINVALUE,			FX_WAH_FILTERFREQ_MAXVALUE,     FX_WAH_FILTERFREQ_DEFAULTVALUE,		FX_WAH_FILTERFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah_freq,	(uint32_t)display8khzfreq,	NULL,		0,			0,(uint32_t *)&sound_wah_sensor[0],			NULL,			0x00,	0x0061},
		{	INSTR_PRESET,	SOUND_PRESET_WAH_RES,	"filter res.",	FX_WAH_FILTERRES_MINVALUE,			FX_WAH_FILTERRES_MAXVALUE,    	FX_WAH_FILTERRES_DEFAULTVALUE,		FX_WAH_FILTERRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah_res,	(uint32_t)displaywahres,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0062},
		//{	INSTR_WAH,		FX_WAH_AUTOSENSITIVITY,	"auto-sens",	FX_WAH_SENSITIVITY_MINVALUE,    	FX_WAH_SENSITIVITY_MAXVALUE,    FX_WAH_SENSITIVITY_DEFAULTVALUE,	FX_WAH_SENSITIVITY_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah,		(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0063},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_distortion_sensor[FX_DISTO_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_DISTORTION,	FX_DISTO_ON_OFF,		"state",		FX_DISTO_ONOFF_MINVALUE,			FX_DISTO_ONOFF_MAXVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0570},
		//{	INSTR_DISTORTION,	FX_DISTO_PREGAIN,		"gain",			FX_DISTO_PREGAIN_MINVALUE,			FX_DISTO_PREGAIN_MAXVALUE,     	FX_DISTO_PREGAIN_DEFAULTVALUE,		FX_DISTO_PREGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0571},
		//{	INSTR_DISTORTION,	FX_DISTO_TYPE,			"type",			FX_DISTO_TYPE_MINVALUE,				FX_DISTO_TYPE_MAXVALUE,    		FX_DISTO_TYPE_DEFAULTVALUE,			FX_DISTO_TYPE_DEFAULTVALUE,			SCREEN_DISPLAY_DISTOTYPE,	0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0572},
		{	INSTR_DISTORTION,	FX_DISTO_LPFILTERFREQ,	"filter freq",	FX_DISTO_LPFREQ_MINVALUE,    		FX_DISTO_LPFREQ_MAXVALUE,    	FX_DISTO_LPFREQ_DEFAULTVALUE,		FX_DISTO_LPFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setinstrdistortion,	(uint32_t)display8khzfreq,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0573},
		{	INSTR_DISTORTION,	FX_DISTO_LPFILTERRES,	"filter res.",	FX_DISTO_LPRES_MINVALUE,    		FX_DISTO_LPRES_MAXVALUE,     	FX_DISTO_LPRES_DEFAULTVALUE,		FX_DISTO_LPRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0574},
		//{	INSTR_DISTORTION,	FX_DISTO_DRIVE,			"drive",		FX_DISTO_DRIVE_MINVALUE,    		FX_DISTO_DRIVE_MAXVALUE,    	FX_DISTO_DRIVE_DEFAULTVALUE,		FX_DISTO_DRIVE_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaydrive,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0575},
		//{	INSTR_DISTORTION,	FX_DISTO_POSTGAIN,		"post gain",	FX_DISTO_POSTGAIN_MINVALUE,     	FX_DISTO_POSTGAIN_MAXVALUE,     FX_DISTO_POSTGAIN_DEFAULTVALUE,		FX_DISTO_POSTGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0576},
		///{INSTR_DISTORTION,	FX_DISTO_RCTYPEFREQ,	"rc type freq",	FX_DISTO_RCTYPE_MINVALUE,    		FX_DISTO_RCTYPE_MAXVALUE,    	FX_DISTO_RCTYPE_DEFAULTVALUE,		FX_DISTO_RCTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0577},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_distortion_param[FX_DISTO_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_DISTORTION,	FX_DISTO_ON_OFF,		"state",		FX_DISTO_ONOFF_MINVALUE,			FX_DISTO_ONOFF_MAXVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0070},
		//{	INSTR_DISTORTION,	FX_DISTO_PREGAIN,		"gain",			FX_DISTO_PREGAIN_MINVALUE,			FX_DISTO_PREGAIN_MAXVALUE,     	FX_DISTO_PREGAIN_DEFAULTVALUE,		FX_DISTO_PREGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0071},
		//{	INSTR_DISTORTION,	FX_DISTO_TYPE,			"type",			FX_DISTO_TYPE_MINVALUE,				FX_DISTO_TYPE_MAXVALUE,    		FX_DISTO_TYPE_DEFAULTVALUE,			FX_DISTO_TYPE_DEFAULTVALUE,			SCREEN_DISPLAY_DISTOTYPE,	0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0072},
		{	INSTR_DISTORTION,	FX_DISTO_LPFILTERFREQ,	"filter freq",	FX_DISTO_LPFREQ_MINVALUE,    		FX_DISTO_LPFREQ_MAXVALUE,    	FX_DISTO_LPFREQ_DEFAULTVALUE,		FX_DISTO_LPFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)display8khzfreq,	NULL,		0,			0,(uint32_t *)&sound_distortion_sensor[0],			NULL,			0x00,	0x0073},
		{	INSTR_DISTORTION,	FX_DISTO_LPFILTERRES,	"filter res.",	FX_DISTO_LPRES_MINVALUE,    		FX_DISTO_LPRES_MAXVALUE,     	FX_DISTO_LPRES_DEFAULTVALUE,		FX_DISTO_LPRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0074},
		//{	INSTR_DISTORTION,	FX_DISTO_DRIVE,			"drive",		FX_DISTO_DRIVE_MINVALUE,    		FX_DISTO_DRIVE_MAXVALUE,    	FX_DISTO_DRIVE_DEFAULTVALUE,		FX_DISTO_DRIVE_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaydrive,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0075},
		//{	INSTR_DISTORTION,	FX_DISTO_POSTGAIN,		"post gain",	FX_DISTO_POSTGAIN_MINVALUE,     	FX_DISTO_POSTGAIN_MAXVALUE,     FX_DISTO_POSTGAIN_DEFAULTVALUE,		FX_DISTO_POSTGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0076},
		///{INSTR_DISTORTION,	FX_DISTO_RCTYPEFREQ,	"rc type freq",	FX_DISTO_RCTYPE_MINVALUE,    		FX_DISTO_RCTYPE_MAXVALUE,    	FX_DISTO_RCTYPE_DEFAULTVALUE,		FX_DISTO_RCTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0077},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_eq_sensor[FX_PEQ_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_EQUALIZER,	FX_PEQ_ON_OFF,			"state",		FX_PEQ_ONOFF_MINVALUE,				FX_PEQ_ONOFF_MAXVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,	 	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0580},
		{	INSTR_EQUALIZER,	FX_PEQ_LBGAIN,			"low gain",		FX_PEQ_LBGAIN_MINVALUE,     		FX_PEQ_LBGAIN_MAXVALUE,     	FX_PEQ_LBGAIN_DEFAULTVALUE,			FX_PEQ_LBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0581},
		{	INSTR_EQUALIZER,	FX_PEQ_LMBGAIN, 		"mid-l gain",	FX_PEQ_LMBGAIN_MINVALUE,    		FX_PEQ_LMBGAIN_MAXVALUE,    	FX_PEQ_LMBGAIN_DEFAULTVALUE,		FX_PEQ_LMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0582},
		{	INSTR_EQUALIZER,	FX_PEQ_HMBGAIN,			"mid-h gain",	FX_PEQ_HMBGAIN_MINVALUE,    		FX_PEQ_HMBGAIN_MAXVALUE,    	FX_PEQ_HMBGAIN_DEFAULTVALUE,		FX_PEQ_HMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0583},
		{	INSTR_EQUALIZER,	FX_PEQ_HBGAIN,			"high gain",	FX_PEQ_HBGAIN_MINVALUE,     		FX_PEQ_HBGAIN_MAXVALUE,     	FX_PEQ_HBGAIN_DEFAULTVALUE,			FX_PEQ_HBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0584},
		//{	INSTR_EQUALIZER,	FX_PEQ_LBFREQ,			"low freq",		FX_PEQ_LBFREQ_MINVALUE,   		  	FX_PEQ_LBFREQ_MAXVALUE,     	FX_PEQ_LBFREQ_DEFAULTVALUE,			FX_PEQ_LBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_low_freq,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0585},
		//{	INSTR_EQUALIZER,	FX_PEQ_LMBFREQ, 		"mid-l freq",	FX_PEQ_LMBFREQ_MINVALUE,   			FX_PEQ_LMBFREQ_MAXVALUE,    	FX_PEQ_LMBFREQ_DEFAULTVALUE,		FX_PEQ_LMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midl_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0586},
		//{	INSTR_EQUALIZER,	FX_PEQ_HMBFREQ,			"mid-h freq",	FX_PEQ_HMBFREQ_MINVALUE,    		FX_PEQ_HMBFREQ_MAXVALUE,    	FX_PEQ_HMBFREQ_DEFAULTVALUE,		FX_PEQ_HMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midh_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0587},
		//{	INSTR_EQUALIZER,	FX_PEQ_HBFREQ,			"high freq",	FX_PEQ_HBFREQ_MINVALUE,     		FX_PEQ_HBFREQ_MAXVALUE,     	FX_PEQ_HBFREQ_DEFAULTVALUE,			FX_PEQ_HBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_high_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0588},
		//{	INSTR_EQUALIZER,	FX_PEQ_LMBQ,			"mid-l q.",		FX_PEQ_LMBQ_MINVALUE,       		FX_PEQ_LMBQ_MAXVALUE,       	FX_PEQ_LMBQ_DEFAULTVALUE,			FX_PEQ_LMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0589},
		//{	INSTR_EQUALIZER,	FX_PEQ_HMBQ,			"mid-h q.",		FX_PEQ_HMBQ_MINVALUE,       		FX_PEQ_HMBQ_MAXVALUE,       	FX_PEQ_HMBQ_DEFAULTVALUE,			FX_PEQ_HMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x058A}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_eq_param[FX_PEQ_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_EQUALIZER,	FX_PEQ_ON_OFF,			"state",		FX_PEQ_ONOFF_MINVALUE,				FX_PEQ_ONOFF_MAXVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,	 	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0080},
		{	INSTR_EQUALIZER,	FX_PEQ_LBGAIN,			"low gain",		FX_PEQ_LBGAIN_MINVALUE,     		FX_PEQ_LBGAIN_MAXVALUE,     	FX_PEQ_LBGAIN_DEFAULTVALUE,			FX_PEQ_LBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&sound_eq_sensor[0],			NULL,			0x00,	0x0081},
		{	INSTR_EQUALIZER,	FX_PEQ_LMBGAIN, 		"mid-l gain",	FX_PEQ_LMBGAIN_MINVALUE,    		FX_PEQ_LMBGAIN_MAXVALUE,    	FX_PEQ_LMBGAIN_DEFAULTVALUE,		FX_PEQ_LMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&sound_eq_sensor[1],			NULL,			0x00,	0x0082},
		{	INSTR_EQUALIZER,	FX_PEQ_HMBGAIN,			"mid-h gain",	FX_PEQ_HMBGAIN_MINVALUE,    		FX_PEQ_HMBGAIN_MAXVALUE,    	FX_PEQ_HMBGAIN_DEFAULTVALUE,		FX_PEQ_HMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&sound_eq_sensor[2],			NULL,			0x00,	0x0083},
		{	INSTR_EQUALIZER,	FX_PEQ_HBGAIN,			"high gain",	FX_PEQ_HBGAIN_MINVALUE,     		FX_PEQ_HBGAIN_MAXVALUE,     	FX_PEQ_HBGAIN_DEFAULTVALUE,			FX_PEQ_HBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&sound_eq_sensor[3],			NULL,			0x00,	0x0084},
		//{	INSTR_EQUALIZER,	FX_PEQ_LBFREQ,			"low freq",		FX_PEQ_LBFREQ_MINVALUE,   		  	FX_PEQ_LBFREQ_MAXVALUE,     	FX_PEQ_LBFREQ_DEFAULTVALUE,			FX_PEQ_LBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_low_freq,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0085},
		//{	INSTR_EQUALIZER,	FX_PEQ_LMBFREQ, 		"mid-l freq",	FX_PEQ_LMBFREQ_MINVALUE,   			FX_PEQ_LMBFREQ_MAXVALUE,    	FX_PEQ_LMBFREQ_DEFAULTVALUE,		FX_PEQ_LMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midl_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0086},
		//{	INSTR_EQUALIZER,	FX_PEQ_HMBFREQ,			"mid-h freq",	FX_PEQ_HMBFREQ_MINVALUE,    		FX_PEQ_HMBFREQ_MAXVALUE,    	FX_PEQ_HMBFREQ_DEFAULTVALUE,		FX_PEQ_HMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midh_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0087},
		//{	INSTR_EQUALIZER,	FX_PEQ_HBFREQ,			"high freq",	FX_PEQ_HBFREQ_MINVALUE,     		FX_PEQ_HBFREQ_MAXVALUE,     	FX_PEQ_HBFREQ_DEFAULTVALUE,			FX_PEQ_HBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_high_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0088},
		//{	INSTR_EQUALIZER,	FX_PEQ_LMBQ,			"mid-l q.",		FX_PEQ_LMBQ_MINVALUE,       		FX_PEQ_LMBQ_MAXVALUE,       	FX_PEQ_LMBQ_DEFAULTVALUE,			FX_PEQ_LMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0089},
		//{	INSTR_EQUALIZER,	FX_PEQ_HMBQ,			"mid-h q.",		FX_PEQ_HMBQ_MINVALUE,       		FX_PEQ_HMBQ_MAXVALUE,       	FX_PEQ_HMBQ_DEFAULTVALUE,			FX_PEQ_HMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x008A}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_chorus_param[FX_CHORUS_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_CHORUS,	FX_CHORUS_MODE,			"mode",			FX_CHORUS_MODE_MINVALUE,			FX_CHORUS_MODE_MAXVALUE,		FX_CHORUS_MODE_DEFAULTVALUE,		FX_CHORUS_MODE_DEFAULTVALUE,		SCREEN_DISPLAY_CHORUSMODE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0090},
		//{	INSTR_CHORUS,	FX_CHORUS_EFFECTLEVEL,	"effect level",	FX_CHORUS_LEVEL_MINVALUE,			FX_CHORUS_LEVEL_MAXVALUE,   	FX_CHORUS_LEVEL_DEFAULTVALUE,		FX_CHORUS_LEVEL_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0091},
		//{	INSTR_CHORUS,	FX_CHORUS_DELAYTIME,	"delay time",	FX_CHORUS_DELAY_MINVALUE,			FX_CHORUS_DELAY_MAXVALUE,   	FX_CHORUS_DELAY_DEFAULTVALUE,		FX_CHORUS_DELAY_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,			0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaychorusdelay,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0092},
		{	INSTR_CHORUS,	FX_CHORUS_FEEDBACK,		"feedback",		FX_CHORUS_FEEDBACK_MINVALUE,    	FX_CHORUS_FEEDBACK_MAXVALUE,	FX_CHORUS_FEEDBACK_DEFAULTVALUE,	FX_CHORUS_FEEDBACK_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0093},
		//{	INSTR_CHORUS,	FX_CHORUS_HPFREQ,		"hp freq",		FX_CHORUS_INPUTHP_MINVALUE,    		FX_CHORUS_INPUTHP_MAXVALUE, 	FX_CHORUS_INPUTHP_DEFAULTVALUE,		FX_CHORUS_INPUTHP_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)display1200Hz,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0094},
		//{	INSTR_CHORUS,	FX_CHORUS_HDAMP, 		"hd amp",		FX_CHORUS_HDAMP_MINVALUE,     		FX_CHORUS_HDAMP_MAXVALUE,   	FX_CHORUS_HDAMP_DEFAULTVALUE,		FX_CHORUS_HDAMP_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0095},
		{	INSTR_CHORUS,	FX_CHORUS_MODRATE,		"rate",			FX_CHORUS_MODRATE_MINVALUE,    		FX_CHORUS_MODRATE_MAXVALUE, 	FX_CHORUS_MODRATE_DEFAULTVALUE,		FX_CHORUS_MODRATE_DEFAULTVALUE,		SCREEN_DISPLAY_MILI_HZ,			0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaychorusrate,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0096},
		{	INSTR_CHORUS,	FX_CHORUS_MODDEPTH,		"depth",		FX_CHORUS_MODDEPTH_MINVALUE,    	FX_CHORUS_MODDEPTH_MAXVALUE,	FX_CHORUS_MODDEPTH_DEFAULTVALUE,	FX_CHORUS_MODDEPTH_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0097},
		//{	INSTR_CHORUS,	FX_CHORUS_TREMOLO,		"trem. shape",	FX_CHORUS_TREMOLO_MINVALUE,     	FX_CHORUS_TREMOLO_MAXVALUE, 	FX_CHORUS_TREMOLO_DEFAULTVALUE,		FX_CHORUS_TREMOLO_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0098},
		//{	INSTR_CHORUS,	FX_CHORUS_ROTARYSPEED,	"rotary speed",	FX_CHORUS_ROTARY_MINVALUE,   		FX_CHORUS_ROTARY_MAXVALUE,  	FX_CHORUS_ROTARY_DEFAULTVALUE,		FX_CHORUS_ROTARY_DEFAULTVALUE,	SCREEN_DISPLAY_ROTARYSPEED,			0,						CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0099},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_delay_sensor[FX_DELAY_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_DELAY,	FX_DELAY_ON_OFF,		"state",		FX_DELAY_ONOFF_MINVALUE,			FX_DELAY_ONOFF_MAXVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05A0},
		{	INSTR_DELAY,	FX_DELAY_MODE,			"mode",			FX_DELAY_MODE_MINVALUE,				FX_DELAY_MODE_MAXVALUE,     	FX_DELAY_MODE_DEFAULTVALUE,			FX_DELAY_MODE_DEFAULTVALUE,			SCREEN_DISPLAY_PANMODE,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05A1},
		//{	INSTR_DELAY,	FX_DELAY_PRELP,			"lowpass freq",	FX_DELAY_PRELP_MINVALUE,			FX_DELAY_PRELP_MAXVALUE,    	FX_DELAY_PRELP_DEFAULTVALUE,		FX_DELAY_PRELP_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05A2},
		//{	INSTR_DELAY,	FX_DELAY_LEVEL,			"level",		FX_DELAY_LEVEL_MINVALUE,    		FX_DELAY_LEVEL_MAXVALUE,    	FX_DELAY_LEVEL_DEFAULTVALUE,		FX_DELAY_LEVEL_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05A3},
		{	INSTR_DELAY,	FX_DELAY_TIME,			"time",			FX_DELAY_BPM_MINVALUE,    			FX_DELAY_BPM_MAXVALUE,     		FX_DELAY_BPM_DEFAULTVALUE,			FX_DELAY_BPM_DEFAULTVALUE,			SCREEN_DISPLAY_BPM_TIME,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05A4},
		{	INSTR_DELAY,	FX_DELAY_FEEDBACK,		"feedback",		FX_DELAY_FEEDBACK_MINVALUE, 		FX_DELAY_FEEDBACK_MAXVALUE, 	FX_DELAY_FEEDBACK_DEFAULTVALUE,		FX_DELAY_FEEDBACK_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,			(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05A5},
		//{	INSTR_DELAY,	FX_DELAY_HDAMP,			"h-damp",		FX_DELAY_HDAMP_MINVALUE,   			FX_DELAY_HDAMP_MAXVALUE,    	FX_DELAY_HDAMP_DEFAULTVALUE,		FX_DELAY_HDAMP_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05A6},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_delay_param[FX_DELAY_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	INSTR_DELAY,	FX_DELAY_ON_OFF,		"state",		FX_DELAY_ONOFF_MINVALUE,			FX_DELAY_ONOFF_MAXVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00A0},
		{	INSTR_DELAY,	FX_DELAY_MODE,			"mode",			FX_DELAY_MODE_MINVALUE,				FX_DELAY_MODE_MAXVALUE,     	FX_DELAY_MODE_DEFAULTVALUE,			FX_DELAY_MODE_DEFAULTVALUE,			SCREEN_DISPLAY_PANMODE,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00A1},
		//{	INSTR_DELAY,	FX_DELAY_PRELP,			"lowpass freq",	FX_DELAY_PRELP_MINVALUE,			FX_DELAY_PRELP_MAXVALUE,    	FX_DELAY_PRELP_DEFAULTVALUE,		FX_DELAY_PRELP_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00A2},
		//{	INSTR_DELAY,	FX_DELAY_LEVEL,			"level",		FX_DELAY_LEVEL_MINVALUE,    		FX_DELAY_LEVEL_MAXVALUE,    	FX_DELAY_LEVEL_DEFAULTVALUE,		FX_DELAY_LEVEL_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00A3},
		{	INSTR_DELAY,	FX_DELAY_TIME,			"time",			FX_DELAY_BPM_MINVALUE,    			FX_DELAY_BPM_MAXVALUE,     		FX_DELAY_BPM_DEFAULTVALUE,			FX_DELAY_BPM_DEFAULTVALUE,			SCREEN_DISPLAY_BPM_TIME,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00A4},
		{	INSTR_DELAY,	FX_DELAY_FEEDBACK,		"feedback",		FX_DELAY_FEEDBACK_MINVALUE, 		FX_DELAY_FEEDBACK_MAXVALUE, 	FX_DELAY_FEEDBACK_DEFAULTVALUE,		FX_DELAY_FEEDBACK_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,(uint32_t *)&sound_delay_sensor[2],			NULL,			0x00,	0x00A5},
		//{	INSTR_DELAY,	FX_DELAY_HDAMP,			"h-damp",		FX_DELAY_HDAMP_MINVALUE,   			FX_DELAY_HDAMP_MAXVALUE,    	FX_DELAY_HDAMP_DEFAULTVALUE,		FX_DELAY_HDAMP_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00A6},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_adsr_sensor[FX_ADSR_USER_SIZE] = {
// 			p_struct,		p_param,					p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	INSTR_PRESET,	SOUND_PRESET_ADSR_ATTACK,	"attack time",	FX_ADSR_ATTACK_MINVALUE,			FX_ADSR_ATTACK_MAXVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,					CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setinstradsr_attack,	(uint32_t)displayADSRattack,NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C0},
		//{	INSTR_ADSR,		FX_ADSR_ENV_DECAY,			"decay time",	FX_ADSR_DECAY_MINVALUE,				FX_ADSR_DECAY_MAXVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)displayADSRdecay,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C1},
		//{	INSTR_ADSR,		FX_ADSR_CUTOFF_RES,			"filter res.",	FX_ADSR_RES_MINVALUE,				FX_ADSR_RES_MAXVALUE,			FX_ADSR_RES_DEFAULTVALUE,			FX_ADSR_RES_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C2},
		//{	INSTR_ADSR,		FX_ADSR_CUTOFF_FREQ,		"filter freq.",	FX_ADSR_FREQ_MINVALUE, 				FX_ADSR_FREQ_MAXVALUE,   		FX_ADSR_FREQ_DEFAULTVALUE,			FX_ADSR_FREQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C3},
		{	INSTR_PRESET,	SOUND_PRESET_ADSR_RELEASE,	"release time",	FX_ADSR_RELEAS_MINVALUE,  			FX_ADSR_RELEAS_MAXVALUE,  		FX_ADSR_RELEAS_DEFAULTVALUE,		FX_ADSR_RELEAS_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,					CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C4},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	sound_adsr_param[FX_ADSR_USER_SIZE] = {
// 			p_struct,		p_param,					p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	INSTR_PRESET,	SOUND_PRESET_ADSR_ATTACK,	"attack time",	FX_ADSR_ATTACK_MINVALUE,			FX_ADSR_ATTACK_MAXVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstradsr_attack,	(uint32_t)displayADSRattack,NULL,		0,			0,				NULL,			(uint32_t *)&sound_adsr_param[1],			0x00,	0x05C0},
		//{	INSTR_ADSR,		FX_ADSR_ENV_DECAY,			"decay time",	FX_ADSR_DECAY_MINVALUE,				FX_ADSR_DECAY_MAXVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)displayADSRdecay,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C1},
		//{	INSTR_ADSR,		FX_ADSR_CUTOFF_RES,			"filter res.",	FX_ADSR_RES_MINVALUE,				FX_ADSR_RES_MAXVALUE,			FX_ADSR_RES_DEFAULTVALUE,			FX_ADSR_RES_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C2},
		//{	INSTR_ADSR,		FX_ADSR_CUTOFF_FREQ,		"filter freq.",	FX_ADSR_FREQ_MINVALUE, 				FX_ADSR_FREQ_MAXVALUE,   		FX_ADSR_FREQ_DEFAULTVALUE,			FX_ADSR_FREQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00C3},
		{	INSTR_PRESET,	SOUND_PRESET_ADSR_RELEASE,	"release time",	FX_ADSR_RELEAS_MINVALUE,  			FX_ADSR_RELEAS_MAXVALUE,  		FX_ADSR_RELEAS_DEFAULTVALUE,		FX_ADSR_RELEAS_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,NULL,		0,			0,	            NULL,			(uint32_t *)&sound_adsr_param[0],			0x00,	0x05C4},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_compressor_param[FX_COMP_USER_SIZE] = {
// 			p_struct,				p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,							extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_COMPRESSOR,	FX_COMP_ON_OFF,			"state",		FX_COMP_ONOFF_MINVALUE,				FX_COMP_ONOFF_MAXVALUE,			FX_COMP_ONOFF_DEFAULTVALUE,			FX_COMP_ONOFF_DEFAULTVALUE,			SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x00D0},
		//{	MUSIC_INSTR_COMPRESSOR,	FX_COMP_ATTACKTIME,		"attack time",	FX_COMP_ATTACK_MINVALUE,			FX_COMP_ATTACK_MAXVALUE,     	FX_COMP_ATTACK_DEFAULTVALUE,		FX_COMP_ATTACK_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorattack,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00D1},
		//{	MUSIC_INSTR_COMPRESSOR,	FX_COMP_RELEASETIME,	"release time",	FX_COMP_RELEASE_MINVALUE,			FX_COMP_RELEASE_MAXVALUE,    	FX_COMP_RELEASE_DEFAULTVALUE,		FX_COMP_RELEASE_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorrelease,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00D2},
		{	MUSIC_INSTR_COMPRESSOR,	FX_COMP_THRESHOLD,		"threshold",	FX_COMP_THRESHOLD_MINVALUE,    		FX_COMP_THRESHOLD_MAXVALUE,    	FX_COMP_THRESHOLD_DEFAULTVALUE,		FX_COMP_THRESHOLD_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaygain_42,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x00D3},
		{	MUSIC_INSTR_COMPRESSOR,	FX_COMP_RATIO,			"ratio",		FX_COMP_RATIO_MINVALUE,    			FX_COMP_RATIO_MAXVALUE,     	FX_COMP_RATIO_DEFAULTVALUE,			FX_COMP_RATIO_DEFAULTVALUE,			SCREEN_DISPLAY_RATIO,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorratio,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00D4},
		{	MUSIC_INSTR_COMPRESSOR,	FX_COMP_BOOST,			"boost",		FX_COMP_BOOST_MINVALUE,     		FX_COMP_BOOST_MAXVALUE,     	FX_COMP_BOOST_DEFAULTVALUE,			FX_COMP_BOOST_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)displaycompressorboost,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00D5},
		//{	MUSIC_INSTR_COMPRESSOR,	FX_COMP_KNEE,			"knee",			FX_COMP_KNEE_MINVALUE,    			FX_COMP_KNEE_MAXVALUE,    		FX_COMP_KNEE_DEFAULTVALUE,			FX_COMP_KNEE_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrcompressor,	(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x00D6},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_wah_sensor[FX_WAH_USER_SIZE] = {
// 			p_struct,				p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_WAH,		FX_WAH_FILTERTYPE,		"filter type",	FX_WAH_FILTERTYPE_MINVALUE,			FX_WAH_FILTERTYPE_MAXVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_FILTERTYPE,	0,							CTRL_EVENT_REL_MIN,	0,					0,					(uint32_t)setinstrwah,		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05E0},
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_WAH_FREQ,	"filter freq",	FX_WAH_FILTERFREQ_MINVALUE,			FX_WAH_FILTERFREQ_MAXVALUE,     FX_WAH_FILTERFREQ_DEFAULTVALUE,		FX_WAH_FILTERFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,				(uint32_t)setinstrwah_freq,	(uint32_t)display8khzfreqwah,NULL,		0,			0,				NULL,			NULL,			0x00,	0x05E1},
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_WAH_RES,	"filter res.",	FX_WAH_FILTERRES_MINVALUE,			FX_WAH_FILTERRES_MAXVALUE,    	FX_WAH_FILTERRES_DEFAULTVALUE,		FX_WAH_FILTERRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,				(uint32_t)setinstrwah_res,	(uint32_t)displaywahres,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05E2},
		//{	MUSIC_INSTR_WAH,		FX_WAH_AUTOSENSITIVITY,	"auto-sens",	FX_WAH_SENSITIVITY_MINVALUE,    	FX_WAH_SENSITIVITY_MAXVALUE,    FX_WAH_SENSITIVITY_DEFAULTVALUE,	FX_WAH_SENSITIVITY_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,					0,					(uint32_t)setinstrwah,		(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05E3},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_wah_param[FX_WAH_USER_SIZE] = {
// 			p_struct,				p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_WAH_TYPE,	"filter type",	FX_WAH_FILTERTYPE_MINVALUE,			FX_WAH_FILTERTYPE_MAXVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		FX_WAH_FILTERTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_FILTERTYPE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah_type,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00E0},
		//{	MUSIC_INSTR_WAH,		FX_WAH_FILTERFREQ,		"filter freq",	FX_WAH_FILTERFREQ_MINVALUE,			FX_WAH_FILTERFREQ_MAXVALUE,     FX_WAH_FILTERFREQ_DEFAULTVALUE,		FX_WAH_FILTERFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah,		(uint32_t)display8khzfreq,	NULL,		0,			0,(uint32_t *)&music_wah_sensor[0],			NULL,			0x00,	0x00E1},
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_WAH_RES,	"filter res.",	FX_WAH_FILTERRES_MINVALUE,			FX_WAH_FILTERRES_MAXVALUE,    	FX_WAH_FILTERRES_DEFAULTVALUE,		FX_WAH_FILTERRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah_res,	(uint32_t)displaywahres,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00E2},
		//{	MUSIC_INSTR_WAH,		FX_WAH_AUTOSENSITIVITY,	"auto-sens",	FX_WAH_SENSITIVITY_MINVALUE,    	FX_WAH_SENSITIVITY_MAXVALUE,    FX_WAH_SENSITIVITY_DEFAULTVALUE,	FX_WAH_SENSITIVITY_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrwah,		(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00E3},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_distortion_sensor[FX_DISTO_USER_SIZE] = {
// 			p_struct,				p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_ON_OFF,		"state",		FX_DISTO_ONOFF_MINVALUE,			FX_DISTO_ONOFF_MAXVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F0},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_PREGAIN,		"gain",			FX_DISTO_PREGAIN_MINVALUE,			FX_DISTO_PREGAIN_MAXVALUE,     	FX_DISTO_PREGAIN_DEFAULTVALUE,		FX_DISTO_PREGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F1},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_TYPE,			"type",			FX_DISTO_TYPE_MINVALUE,				FX_DISTO_TYPE_MAXVALUE,    		FX_DISTO_TYPE_DEFAULTVALUE,			FX_DISTO_TYPE_DEFAULTVALUE,			SCREEN_DISPLAY_DISTOTYPE,	0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F2},
		{	MUSIC_INSTR_DISTORTION,	FX_DISTO_LPFILTERFREQ,	"filter freq",	FX_DISTO_LPFREQ_MINVALUE,    		FX_DISTO_LPFREQ_MAXVALUE,    	FX_DISTO_LPFREQ_DEFAULTVALUE,		FX_DISTO_LPFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,			(uint32_t)setinstrdistortion,	(uint32_t)display8khzfreq,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F3},
		{	MUSIC_INSTR_DISTORTION,	FX_DISTO_LPFILTERRES,	"filter res.",	FX_DISTO_LPRES_MINVALUE,    		FX_DISTO_LPRES_MAXVALUE,     	FX_DISTO_LPRES_DEFAULTVALUE,		FX_DISTO_LPRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,			(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F4},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_DRIVE,			"drive",		FX_DISTO_DRIVE_MINVALUE,    		FX_DISTO_DRIVE_MAXVALUE,    	FX_DISTO_DRIVE_DEFAULTVALUE,		FX_DISTO_DRIVE_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaydrive,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F5},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_POSTGAIN,		"post gain",	FX_DISTO_POSTGAIN_MINVALUE,     	FX_DISTO_POSTGAIN_MAXVALUE,     FX_DISTO_POSTGAIN_DEFAULTVALUE,		FX_DISTO_POSTGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F6},
		///{MUSIC_INSTR_DISTORTION,	FX_DISTO_RCTYPEFREQ,	"rc type freq",	FX_DISTO_RCTYPE_MINVALUE,    		FX_DISTO_RCTYPE_MAXVALUE,    	FX_DISTO_RCTYPE_DEFAULTVALUE,		FX_DISTO_RCTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x05F7},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_distortion_param[FX_DISTO_USER_SIZE] = {
// 			p_struct,				p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_ON_OFF,		"state",		FX_DISTO_ONOFF_MINVALUE,			FX_DISTO_ONOFF_MAXVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		FX_DISTO_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00F0},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_PREGAIN,		"gain",			FX_DISTO_PREGAIN_MINVALUE,			FX_DISTO_PREGAIN_MAXVALUE,     	FX_DISTO_PREGAIN_DEFAULTVALUE,		FX_DISTO_PREGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00F1},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_TYPE,			"type",			FX_DISTO_TYPE_MINVALUE,				FX_DISTO_TYPE_MAXVALUE,    		FX_DISTO_TYPE_DEFAULTVALUE,			FX_DISTO_TYPE_DEFAULTVALUE,			SCREEN_DISPLAY_DISTOTYPE,	0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00F2},
		{	MUSIC_INSTR_DISTORTION,	FX_DISTO_LPFILTERFREQ,	"filter freq",	FX_DISTO_LPFREQ_MINVALUE,    		FX_DISTO_LPFREQ_MAXVALUE,    	FX_DISTO_LPFREQ_DEFAULTVALUE,		FX_DISTO_LPFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)display8khzfreq,	NULL,		0,			0,(uint32_t *)&music_distortion_sensor[0],			NULL,			0x00,	0x00F3},
		{	MUSIC_INSTR_DISTORTION,	FX_DISTO_LPFILTERRES,	"filter res.",	FX_DISTO_LPRES_MINVALUE,    		FX_DISTO_LPRES_MAXVALUE,     	FX_DISTO_LPRES_DEFAULTVALUE,		FX_DISTO_LPRES_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00F4},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_DRIVE,			"drive",		FX_DISTO_DRIVE_MINVALUE,    		FX_DISTO_DRIVE_MAXVALUE,    	FX_DISTO_DRIVE_DEFAULTVALUE,		FX_DISTO_DRIVE_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaydrive,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x00F5},
		//{	MUSIC_INSTR_DISTORTION,	FX_DISTO_POSTGAIN,		"post gain",	FX_DISTO_POSTGAIN_MINVALUE,     	FX_DISTO_POSTGAIN_MAXVALUE,     FX_DISTO_POSTGAIN_DEFAULTVALUE,		FX_DISTO_POSTGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAINOFF,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)displaygain_42,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x00F6},
		///{MUSIC_INSTR_DISTORTION,	FX_DISTO_RCTYPEFREQ,	"rc type freq",	FX_DISTO_RCTYPE_MINVALUE,    		FX_DISTO_RCTYPE_MAXVALUE,    	FX_DISTO_RCTYPE_DEFAULTVALUE,		FX_DISTO_RCTYPE_DEFAULTVALUE,		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdistortion,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x00F7},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_eq_sensor[FX_PEQ_USER_SIZE] = {
// 			p_struct,				p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_ON_OFF,			"state",		FX_PEQ_ONOFF_MINVALUE,				FX_PEQ_ONOFF_MAXVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,	 	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0600},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LBGAIN,			"low gain",		FX_PEQ_LBGAIN_MINVALUE,     		FX_PEQ_LBGAIN_MAXVALUE,     	FX_PEQ_LBGAIN_DEFAULTVALUE,			FX_PEQ_LBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0601},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LMBGAIN, 		"mid-l gain",	FX_PEQ_LMBGAIN_MINVALUE,    		FX_PEQ_LMBGAIN_MAXVALUE,    	FX_PEQ_LMBGAIN_DEFAULTVALUE,		FX_PEQ_LMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0602},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HMBGAIN,			"mid-h gain",	FX_PEQ_HMBGAIN_MINVALUE,    		FX_PEQ_HMBGAIN_MAXVALUE,    	FX_PEQ_HMBGAIN_DEFAULTVALUE,		FX_PEQ_HMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0603},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HBGAIN,			"high gain",	FX_PEQ_HBGAIN_MINVALUE,     		FX_PEQ_HBGAIN_MAXVALUE,     	FX_PEQ_HBGAIN_DEFAULTVALUE,			FX_PEQ_HBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,	0,			(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0604},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LBFREQ,			"low freq",		FX_PEQ_LBFREQ_MINVALUE,   		  	FX_PEQ_LBFREQ_MAXVALUE,     	FX_PEQ_LBFREQ_DEFAULTVALUE,			FX_PEQ_LBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_low_freq,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0605},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LMBFREQ, 		"mid-l freq",	FX_PEQ_LMBFREQ_MINVALUE,   			FX_PEQ_LMBFREQ_MAXVALUE,    	FX_PEQ_LMBFREQ_DEFAULTVALUE,		FX_PEQ_LMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midl_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0606},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HMBFREQ,			"mid-h freq",	FX_PEQ_HMBFREQ_MINVALUE,    		FX_PEQ_HMBFREQ_MAXVALUE,    	FX_PEQ_HMBFREQ_DEFAULTVALUE,		FX_PEQ_HMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midh_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0607},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HBFREQ,			"high freq",	FX_PEQ_HBFREQ_MINVALUE,     		FX_PEQ_HBFREQ_MAXVALUE,     	FX_PEQ_HBFREQ_DEFAULTVALUE,			FX_PEQ_HBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_high_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0608},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LMBQ,			"mid-l q.",		FX_PEQ_LMBQ_MINVALUE,       		FX_PEQ_LMBQ_MAXVALUE,       	FX_PEQ_LMBQ_DEFAULTVALUE,			FX_PEQ_LMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0609},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HMBQ,			"mid-h q.",		FX_PEQ_HMBQ_MINVALUE,       		FX_PEQ_HMBQ_MAXVALUE,       	FX_PEQ_HMBQ_DEFAULTVALUE,			FX_PEQ_HMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x060A}
};


#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_eq_param[FX_PEQ_USER_SIZE] = {
// 			p_struct,				p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_ON_OFF,			"state",		FX_PEQ_ONOFF_MINVALUE,				FX_PEQ_ONOFF_MAXVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			FX_PEQ_ONOFF_DEFAULTVALUE,			SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,	 	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0100},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LBGAIN,			"low gain",		FX_PEQ_LBGAIN_MINVALUE,     		FX_PEQ_LBGAIN_MAXVALUE,     	FX_PEQ_LBGAIN_DEFAULTVALUE,			FX_PEQ_LBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&music_eq_sensor[0],			NULL,			0x00,	0x0101},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LMBGAIN, 		"mid-l gain",	FX_PEQ_LMBGAIN_MINVALUE,    		FX_PEQ_LMBGAIN_MAXVALUE,    	FX_PEQ_LMBGAIN_DEFAULTVALUE,		FX_PEQ_LMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&music_eq_sensor[1],			NULL,			0x00,	0x0102},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HMBGAIN,			"mid-h gain",	FX_PEQ_HMBGAIN_MINVALUE,    		FX_PEQ_HMBGAIN_MAXVALUE,    	FX_PEQ_HMBGAIN_DEFAULTVALUE,		FX_PEQ_HMBGAIN_DEFAULTVALUE,		SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&music_eq_sensor[2],			NULL,			0x00,	0x0103},
		{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HBGAIN,			"high gain",	FX_PEQ_HBGAIN_MINVALUE,     		FX_PEQ_HBGAIN_MAXVALUE,     	FX_PEQ_HBGAIN_DEFAULTVALUE,			FX_PEQ_HBGAIN_DEFAULTVALUE,			SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displaygain,		NULL,		0,			0,(uint32_t *)&music_eq_sensor[3],			NULL,			0x00,	0x0104},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LBFREQ,			"low freq",		FX_PEQ_LBFREQ_MINVALUE,   		  	FX_PEQ_LBFREQ_MAXVALUE,     	FX_PEQ_LBFREQ_DEFAULTVALUE,			FX_PEQ_LBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_low_freq,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0105},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LMBFREQ, 		"mid-l freq",	FX_PEQ_LMBFREQ_MINVALUE,   			FX_PEQ_LMBFREQ_MAXVALUE,    	FX_PEQ_LMBFREQ_DEFAULTVALUE,		FX_PEQ_LMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midl_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0106},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HMBFREQ,			"mid-h freq",	FX_PEQ_HMBFREQ_MINVALUE,    		FX_PEQ_HMBFREQ_MAXVALUE,    	FX_PEQ_HMBFREQ_DEFAULTVALUE,		FX_PEQ_HMBFREQ_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_midh_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0107},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HBFREQ,			"high freq",	FX_PEQ_HBFREQ_MINVALUE,     		FX_PEQ_HBFREQ_MAXVALUE,     	FX_PEQ_HBFREQ_DEFAULTVALUE,			FX_PEQ_HBFREQ_DEFAULTVALUE,			SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)displayeq_high_freq,NULL,	0,			0,				NULL,			NULL,			0x00,	0x0108},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_LMBQ,			"mid-l q.",		FX_PEQ_LMBQ_MINVALUE,       		FX_PEQ_LMBQ_MAXVALUE,       	FX_PEQ_LMBQ_DEFAULTVALUE,			FX_PEQ_LMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0109},
		//{	MUSIC_INSTR_EQUALIZER,	FX_PEQ_HMBQ,			"mid-h q.",		FX_PEQ_HMBQ_MINVALUE,       		FX_PEQ_HMBQ_MAXVALUE,       	FX_PEQ_HMBQ_DEFAULTVALUE,			FX_PEQ_HMBQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstreq,		 (uint32_t)NULL,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x010A}
};


#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_chorus_param[FX_CHORUS_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_CHORUS,	FX_CHORUS_MODE,			"mode",			FX_CHORUS_MODE_MINVALUE,			FX_CHORUS_MODE_MAXVALUE,		FX_CHORUS_MODE_DEFAULTVALUE,		FX_CHORUS_MODE_DEFAULTVALUE,		SCREEN_DISPLAY_CHORUSMODE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0110},
		//{	MUSIC_INSTR_CHORUS,	FX_CHORUS_EFFECTLEVEL,	"effect level",	FX_CHORUS_LEVEL_MINVALUE,			FX_CHORUS_LEVEL_MAXVALUE,   	FX_CHORUS_LEVEL_DEFAULTVALUE,		FX_CHORUS_LEVEL_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0111},
		//{	MUSIC_INSTR_CHORUS,	FX_CHORUS_DELAYTIME,	"delay time",	FX_CHORUS_DELAY_MINVALUE,			FX_CHORUS_DELAY_MAXVALUE,   	FX_CHORUS_DELAY_DEFAULTVALUE,		FX_CHORUS_DELAY_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,			0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaychorusdelay,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0112},
		{	MUSIC_INSTR_CHORUS,	FX_CHORUS_FEEDBACK,		"feedback",		FX_CHORUS_FEEDBACK_MINVALUE,    	FX_CHORUS_FEEDBACK_MAXVALUE,	FX_CHORUS_FEEDBACK_DEFAULTVALUE,	FX_CHORUS_FEEDBACK_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0113},
		//{	MUSIC_INSTR_CHORUS,	FX_CHORUS_HPFREQ,		"hp freq",		FX_CHORUS_INPUTHP_MINVALUE,    		FX_CHORUS_INPUTHP_MAXVALUE, 	FX_CHORUS_INPUTHP_DEFAULTVALUE,		FX_CHORUS_INPUTHP_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)display1200Hz,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0114},
		//{	MUSIC_INSTR_CHORUS,	FX_CHORUS_HDAMP, 		"hd amp",		FX_CHORUS_HDAMP_MINVALUE,     		FX_CHORUS_HDAMP_MAXVALUE,   	FX_CHORUS_HDAMP_DEFAULTVALUE,		FX_CHORUS_HDAMP_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0115},
		{	MUSIC_INSTR_CHORUS,	FX_CHORUS_MODRATE,		"rate",			FX_CHORUS_MODRATE_MINVALUE,    		FX_CHORUS_MODRATE_MAXVALUE, 	FX_CHORUS_MODRATE_DEFAULTVALUE,		FX_CHORUS_MODRATE_DEFAULTVALUE,		SCREEN_DISPLAY_MILI_HZ,			0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaychorusrate,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0116},
		{	MUSIC_INSTR_CHORUS,	FX_CHORUS_MODDEPTH,		"depth",		FX_CHORUS_MODDEPTH_MINVALUE,    	FX_CHORUS_MODDEPTH_MAXVALUE,	FX_CHORUS_MODDEPTH_DEFAULTVALUE,	FX_CHORUS_MODDEPTH_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0117},
		//{	MUSIC_INSTR_CHORUS,	FX_CHORUS_TREMOLO,		"trem. shape",	FX_CHORUS_TREMOLO_MINVALUE,     	FX_CHORUS_TREMOLO_MAXVALUE, 	FX_CHORUS_TREMOLO_DEFAULTVALUE,		FX_CHORUS_TREMOLO_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0118},
		{	MUSIC_INSTR_CHORUS,	FX_CHORUS_ROTARYSPEED,	"rotary speed",	FX_CHORUS_ROTARY_MINVALUE,   		FX_CHORUS_ROTARY_MAXVALUE,  	FX_CHORUS_ROTARY_DEFAULTVALUE,		FX_CHORUS_ROTARY_DEFAULTVALUE,	SCREEN_DISPLAY_ROTARYSPEED,			0,						CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0119},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_delay_sensor[FX_DELAY_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_ON_OFF,		"state",		FX_DELAY_ONOFF_MINVALUE,			FX_DELAY_ONOFF_MAXVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0620},
		{	MUSIC_INSTR_DELAY,	FX_DELAY_MODE,			"mode",			FX_DELAY_MODE_MINVALUE,				FX_DELAY_MODE_MAXVALUE,     	FX_DELAY_MODE_DEFAULTVALUE,			FX_DELAY_MODE_DEFAULTVALUE,			SCREEN_DISPLAY_PANMODE,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0621},
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_PRELP,			"lowpass freq",	FX_DELAY_PRELP_MINVALUE,			FX_DELAY_PRELP_MAXVALUE,    	FX_DELAY_PRELP_DEFAULTVALUE,		FX_DELAY_PRELP_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0622},
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_LEVEL,			"level",		FX_DELAY_LEVEL_MINVALUE,    		FX_DELAY_LEVEL_MAXVALUE,    	FX_DELAY_LEVEL_DEFAULTVALUE,		FX_DELAY_LEVEL_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0623},
		{	MUSIC_INSTR_DELAY,	FX_DELAY_TIME,			"time",			FX_DELAY_BPM_MINVALUE,    			FX_DELAY_BPM_MAXVALUE,     		FX_DELAY_BPM_DEFAULTVALUE,			FX_DELAY_BPM_DEFAULTVALUE,			SCREEN_DISPLAY_BPM_TIME,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0624},
		{	MUSIC_INSTR_DELAY,	FX_DELAY_FEEDBACK,		"feedback",		FX_DELAY_FEEDBACK_MINVALUE, 		FX_DELAY_FEEDBACK_MAXVALUE, 	FX_DELAY_FEEDBACK_DEFAULTVALUE,		FX_DELAY_FEEDBACK_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0625},
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_HDAMP,			"h-damp",		FX_DELAY_HDAMP_MINVALUE,   			FX_DELAY_HDAMP_MAXVALUE,    	FX_DELAY_HDAMP_DEFAULTVALUE,		FX_DELAY_HDAMP_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0626},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_delay_param[FX_DELAY_USER_SIZE] = {
// 			p_struct,			p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_ON_OFF,		"state",		FX_DELAY_ONOFF_MINVALUE,			FX_DELAY_ONOFF_MAXVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		FX_DELAY_ONOFF_DEFAULTVALUE,		SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0120},
		{	MUSIC_INSTR_DELAY,	FX_DELAY_MODE,			"mode",			FX_DELAY_MODE_MINVALUE,				FX_DELAY_MODE_MAXVALUE,     	FX_DELAY_MODE_DEFAULTVALUE,			FX_DELAY_MODE_DEFAULTVALUE,			SCREEN_DISPLAY_PANMODE,		0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0121},
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_PRELP,			"lowpass freq",	FX_DELAY_PRELP_MINVALUE,			FX_DELAY_PRELP_MAXVALUE,    	FX_DELAY_PRELP_DEFAULTVALUE,		FX_DELAY_PRELP_DEFAULTVALUE,		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0122},
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_LEVEL,			"level",		FX_DELAY_LEVEL_MINVALUE,    		FX_DELAY_LEVEL_MAXVALUE,    	FX_DELAY_LEVEL_DEFAULTVALUE,		FX_DELAY_LEVEL_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0123},
		{	MUSIC_INSTR_DELAY,	FX_DELAY_TIME,			"time",			FX_DELAY_BPM_MINVALUE,    			FX_DELAY_BPM_MAXVALUE,     		FX_DELAY_BPM_DEFAULTVALUE,			FX_DELAY_BPM_DEFAULTVALUE,			SCREEN_DISPLAY_BPM_TIME,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0124},
		{	MUSIC_INSTR_DELAY,	FX_DELAY_FEEDBACK,		"feedback",		FX_DELAY_FEEDBACK_MINVALUE, 		FX_DELAY_FEEDBACK_MAXVALUE, 	FX_DELAY_FEEDBACK_DEFAULTVALUE,		FX_DELAY_FEEDBACK_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,(uint32_t *)&music_delay_sensor[2],			NULL,			0x00,	0x0125},
		//{	MUSIC_INSTR_DELAY,	FX_DELAY_HDAMP,			"h-damp",		FX_DELAY_HDAMP_MINVALUE,   			FX_DELAY_HDAMP_MAXVALUE,    	FX_DELAY_HDAMP_DEFAULTVALUE,		FX_DELAY_HDAMP_DEFAULTVALUE,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0126},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_adsr_sensor[FX_ADSR_USER_SIZE] = {
// 			p_struct,				p_param,					p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_ADSR_ATTACK,	"attack time",	FX_ADSR_ATTACK_MINVALUE,			FX_ADSR_ATTACK_MAXVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,				CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,		0,					(uint32_t)setinstradsr_attack,	(uint32_t)displayADSRattack,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0640},
		//{	MUSIC_INSTR_ADSR,		FX_ADSR_ENV_DECAY,			"decay time",	FX_ADSR_DECAY_MINVALUE,				FX_ADSR_DECAY_MAXVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)displayADSRdecay,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0141},
		//{	MUSIC_INSTR_ADSR,		FX_ADSR_CUTOFF_RES,			"filter res.",	FX_ADSR_RES_MINVALUE,				FX_ADSR_RES_MAXVALUE,			FX_ADSR_RES_DEFAULTVALUE,			FX_ADSR_RES_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0142},
		//{	MUSIC_INSTR_ADSR,		FX_ADSR_CUTOFF_FREQ,		"filter freq.",	FX_ADSR_FREQ_MINVALUE, 				FX_ADSR_FREQ_MAXVALUE,   		FX_ADSR_FREQ_DEFAULTVALUE,			FX_ADSR_FREQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0143},
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_ADSR_RELEASE,	"release time",	FX_ADSR_RELEAS_MINVALUE,  			FX_ADSR_RELEAS_MAXVALUE,  		FX_ADSR_RELEAS_DEFAULTVALUE,		FX_ADSR_RELEAS_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,				CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,		0,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0644},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_adsr_param[FX_ADSR_USER_SIZE] = {
// 			p_struct,				p_param,					p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,						p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_ADSR_ATTACK,	"attack time",	FX_ADSR_ATTACK_MINVALUE,			FX_ADSR_ATTACK_MAXVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		FX_ADSR_ATTACK_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr_attack,	(uint32_t)displayADSRattack,NULL,		0,			0,				NULL,			(uint32_t *)&music_adsr_param[1],			0x00,	0x0140},
		//{	MUSIC_INSTR_ADSR,		FX_ADSR_ENV_DECAY,			"decay time",	FX_ADSR_DECAY_MINVALUE,				FX_ADSR_DECAY_MAXVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			FX_ADSR_DECAY_DEFAULTVALUE,			SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)displayADSRdecay,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0141},
		//{	MUSIC_INSTR_ADSR,		FX_ADSR_CUTOFF_RES,			"filter res.",	FX_ADSR_RES_MINVALUE,				FX_ADSR_RES_MAXVALUE,			FX_ADSR_RES_DEFAULTVALUE,			FX_ADSR_RES_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0142},
		//{	MUSIC_INSTR_ADSR,		FX_ADSR_CUTOFF_FREQ,		"filter freq.",	FX_ADSR_FREQ_MINVALUE, 				FX_ADSR_FREQ_MAXVALUE,   		FX_ADSR_FREQ_DEFAULTVALUE,			FX_ADSR_FREQ_DEFAULTVALUE,			SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr,			(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0143},
		{	MUSIC_INSTR_PRESET,		SOUND_PRESET_ADSR_RELEASE,	"release time",	FX_ADSR_RELEAS_MINVALUE,  			FX_ADSR_RELEAS_MAXVALUE,  		FX_ADSR_RELEAS_DEFAULTVALUE,		FX_ADSR_RELEAS_DEFAULTVALUE,		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,NULL,		0,			0,	            NULL,	(uint32_t *)&music_adsr_param[0],			0x00,	0x0144},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_reverb_sensor[FX_REVERB_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_REVERB,	FX_REVERB_LEVEL,		"rev level",	FX_REVERB_LEVEL_MINVALUE,			FX_REVERB_LEVEL_MAXVALUE,		FX_REVERB_LEVEL_MAXVALUE,			FX_REVERB_LEVEL_MAXVALUE,			SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0640},
		//{	MUSIC_REVERB,	FX_REVERB_DIRECTLEVEL, 	"dry level",	FX_REVERB_DIRECTLEVEL_MINVALUE,		FX_REVERB_DIRECTLEVEL_MAXVALUE,	FX_REVERB_DIRECTLEVEL_MAXVALUE,     FX_REVERB_DIRECTLEVEL_MAXVALUE,     SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0641},
		{	MUSIC_REVERB,	FX_REVERB_REVSEND,		"level",		FX_REVERB_REVSEND_MINVALUE,			FX_REVERB_REVSEND_MAXVALUE,		FX_REVERB_REVSEND_MINVALUE,    		FX_REVERB_REVSEND_MAXVALUE,    		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_230,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0642},
		//{	MUSIC_REVERB,	FX_REVERB_TONEGAIN,		"tone gain",	FX_REVERB_TONEGAIN_MINVALUE,    	FX_REVERB_TONEGAIN_MAXVALUE,	FX_REVERB_TONEGAIN_MAXVALUE,    	FX_REVERB_TONEGAIN_MAXVALUE,    	SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevtonegain,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0643},
		//{	MUSIC_REVERB,	FX_REVERB_TONEFREQ, 	"tone freq",	FX_REVERB_TONEFREQ_MINVALUE,    	FX_REVERB_TONEFREQ_MAXVALUE,	FX_REVERB_TONEFREQ_MAXVALUE,     	FX_REVERB_TONEFREQ_MAXVALUE,     	SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0644},
		//{	MUSIC_REVERB,	FX_REVERB_PREHP, 		"h-pass freq",	FX_REVERB_PREHP_MINVALUE,     		FX_REVERB_PREHP_MAXVALUE,		FX_REVERB_PREHP_MAXVALUE,     		FX_REVERB_PREHP_MAXVALUE,     		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0645},
		{	MUSIC_REVERB,	FX_REVERB_TIME,			"time",			FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,		FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,    		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,			(uint32_t)setgeneralreverb,	(uint32_t)displayrevtime,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0646},
		//{	MUSIC_REVERB,	FX_REVERB_ECHOFEEDBACK,	"echofeedback",	FX_REVERB_ECHOFEED_MINVALUE,    	FX_REVERB_ECHOFEED_MAXVALUE,	FX_REVERB_ECHOFEED_MAXVALUE,    	FX_REVERB_ECHOFEED_MAXVALUE,    	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0647},
		//{	MUSIC_REVERB,	FX_REVERB_HDAMP,		"h-damp",		FX_REVERB_HDAMP_MINVALUE,     		FX_REVERB_HDAMP_MAXVALUE,		FX_REVERB_HDAMP_MAXVALUE,     		FX_REVERB_HDAMP_MAXVALUE,     		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0648},
		//{	MUSIC_REVERB,	FX_REVERB_THRESGATE,	"thresgate",	FX_REVERB_THRESGATE_MINVALUE,   	FX_REVERB_THRESGATE_MAXVALUE,	FX_REVERB_THRESGATE_MAXVALUE,       FX_REVERB_THRESGATE_MAXVALUE,       SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0649},
		{	MUSIC_REVERB,	FX_REVERB_PREDELAYTIME,	"predelaytime",	FX_REVERB_PREDELAYTIME_MINVALUE,	FX_REVERB_PREDELAYTIME_MAXVALUE,FX_REVERB_PREDELAYTIME_MINVALUE,    FX_REVERB_PREDELAYTIME_MAXVALUE,    SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevpredeltime,	NULL,	0,			0,				NULL,			NULL,			0x00,	0x064a},
		//{	MUSIC_REVERB,	FX_REVERB_DRY_WET,		"dry / wet",	FX_REVERB_DRYWET_MINVALUE,			FX_REVERB_DRYWET_MAXVALUE,		FX_REVERB_DRYWET_MAXVALUE,    		FX_REVERB_DRYWET_MAXVALUE,    		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			0x00,	0x064b}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	music_reverb_param[FX_REVERB_USER_SIZE] = {
// 			p_struct,		p_param,				p_name,			p_min,								p_max,							p_default,							p_sticky,							p_screen,					p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		//{	MUSIC_REVERB,	FX_REVERB_LEVEL,		"rev level",	FX_REVERB_LEVEL_MINVALUE,			FX_REVERB_LEVEL_MAXVALUE,		FX_REVERB_LEVEL_MAXVALUE,			FX_REVERB_LEVEL_MAXVALUE,			SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0240},
		//{	MUSIC_REVERB,	FX_REVERB_DIRECTLEVEL, 	"dry level",	FX_REVERB_DIRECTLEVEL_MINVALUE,		FX_REVERB_DIRECTLEVEL_MAXVALUE,	FX_REVERB_DIRECTLEVEL_MAXVALUE,     FX_REVERB_DIRECTLEVEL_MAXVALUE,     SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0241},
		{	MUSIC_REVERB,	FX_REVERB_REVSEND,		"level",		FX_REVERB_REVSEND_MINVALUE,			FX_REVERB_REVSEND_MAXVALUE,		FX_REVERB_REVSEND_MINVALUE,    		FX_REVERB_REVSEND_MAXVALUE,    		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_230,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0242},
		//{	MUSIC_REVERB,	FX_REVERB_TONEGAIN,		"tone gain",	FX_REVERB_TONEGAIN_MINVALUE,    	FX_REVERB_TONEGAIN_MAXVALUE,	FX_REVERB_TONEGAIN_MAXVALUE,    	FX_REVERB_TONEGAIN_MAXVALUE,    	SCREEN_DISPLAY_GAIN,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevtonegain,NULL,		0,			0,				NULL,			NULL,			0x00,	0x0243},
		//{	MUSIC_REVERB,	FX_REVERB_TONEFREQ, 	"tone freq",	FX_REVERB_TONEFREQ_MINVALUE,    	FX_REVERB_TONEFREQ_MAXVALUE,	FX_REVERB_TONEFREQ_MAXVALUE,     	FX_REVERB_TONEFREQ_MAXVALUE,     	SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0244},
		//{	MUSIC_REVERB,	FX_REVERB_PREHP, 		"h-pass freq",	FX_REVERB_PREHP_MINVALUE,     		FX_REVERB_PREHP_MAXVALUE,		FX_REVERB_PREHP_MAXVALUE,     		FX_REVERB_PREHP_MAXVALUE,     		SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0245},
		{	MUSIC_REVERB,	FX_REVERB_TIME,			"time",			FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,		FX_REVERB_TIME_MINVALUE,    		FX_REVERB_TIME_MAXVALUE,    		SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevtime,	NULL,		0,			0,(uint32_t *)&music_reverb_sensor[1],			NULL,			0x00,	0x0246},
		//{	MUSIC_REVERB,	FX_REVERB_ECHOFEEDBACK,	"echofeedback",	FX_REVERB_ECHOFEED_MINVALUE,    	FX_REVERB_ECHOFEED_MAXVALUE,	FX_REVERB_ECHOFEED_MAXVALUE,    	FX_REVERB_ECHOFEED_MAXVALUE,    	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0247},
		//{	MUSIC_REVERB,	FX_REVERB_HDAMP,		"h-damp",		FX_REVERB_HDAMP_MINVALUE,     		FX_REVERB_HDAMP_MAXVALUE,		FX_REVERB_HDAMP_MAXVALUE,     		FX_REVERB_HDAMP_MAXVALUE,     		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0248},
		//{	MUSIC_REVERB,	FX_REVERB_THRESGATE,	"thresgate",	FX_REVERB_THRESGATE_MINVALUE,   	FX_REVERB_THRESGATE_MAXVALUE,	FX_REVERB_THRESGATE_MAXVALUE,       FX_REVERB_THRESGATE_MAXVALUE,       SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0249},
		{	MUSIC_REVERB,	FX_REVERB_PREDELAYTIME,	"predelaytime",	FX_REVERB_PREDELAYTIME_MINVALUE,	FX_REVERB_PREDELAYTIME_MAXVALUE,FX_REVERB_PREDELAYTIME_MINVALUE,    FX_REVERB_PREDELAYTIME_MAXVALUE,    SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displayrevpredeltime,	NULL,	0,			0,				NULL,			NULL,			0x00,	0x024a},
		//{	MUSIC_REVERB,	FX_REVERB_DRY_WET,		"dry / wet",	FX_REVERB_DRYWET_MINVALUE,			FX_REVERB_DRYWET_MAXVALUE,		FX_REVERB_DRYWET_MAXVALUE,    		FX_REVERB_DRYWET_MAXVALUE,    		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setgeneralreverb,	(uint32_t)displaypercent_255,NULL,		0,			0,				NULL,			NULL,			0x00,	0x024b}
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	dutouch_info[NUM_DUTOUCHINFO] = {
// 			p_struct,		p_param,				p_name,			p_min,				p_max,				p_default,			p_sticky,			p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,					extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	DUTOUCHINFO,	DUTOUCH_BATT,			"battery",		0,  				0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,	 	 		(uint32_t)NULL,	(uint32_t *)NULL,		0,			0,				NULL,			NULL,			0x00,	0x0150},
		{	DUTOUCHINFO,	DUTOUCH_NAME,			"name",			0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0151},
		{	DUTOUCHINFO,	DUTOUCH_USERID,			"user id",		0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0152},
		{	DUTOUCHINFO,	DUTOUCH_FACTORYREF,		"fact. ref",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0153},
		{	DUTOUCHINFO,	DUTOUCH_COREFIRMWARE,	"firmware",		0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0154},
		{	DUTOUCHINFO,	DUTOUCH_SOUND,			"soundbank",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0155},
		{	DUTOUCHINFO,	DUTOUCH_LICENCEOS,		"OS licence",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0156},
		{	DUTOUCHINFO,	DUTOUCH_LICENCEFS,		"FS licence",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0157},
		//{	DUTOUCHINFO,	DUTOUCH_MEMFREESPACE,	"free memory",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0158},
		//{	DUTOUCHINFO,	DUTOUCH_MEMTOTALSPACE,	"total memory",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0x0159},
#ifdef CONFIG_TEST
		{	DUTOUCHINFO,	COPRO_STATUS_CONTROL,	"CP status ct",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0xFFFF},
		{	DUTOUCHINFO,	COPRO_BATTERY_SUPPLY,	"CP batt supp",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0xFFFF},
		{	DUTOUCHINFO,	COPRO_CONTROL,			"CP control",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0xFFFF},
		{	DUTOUCHINFO,	COPRO_BATTERY_VOLTAGE,	"CP batt volt",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0xFFFF},
		{	DUTOUCHINFO,	COPRO_BATTERY_TERM,		"CP batt term",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0xFFFF},
		{	DUTOUCHINFO,	COPRO_VIN_VOLTAGE,		"CP Vin volt",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0xFFFF},
		{	DUTOUCHINFO,	COPRO_SAFETY,			"CP safety",	0,					0,					0,					0,					SCREEN_CLEAR,				0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,				NULL,		0,			0,				NULL,			NULL,			0x00,	0xFFFF},
#endif
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#endif
const param_struct	imu_param[2] = {
// 			p_struct,		p_param,				p_name,				p_min,					p_max,					p_default,					p_sticky,					p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	CONTROLER,		CONTROLER_IMU_ANGLE,	"motion angle",		GYRO_SEND_ANGLE_MIN,	GYRO_SEND_ANGLE_MAX,	GYRO_SEND_ANGLE_DEFAULT,	GYRO_SEND_ANGLE_DEFAULT,	SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x026B},
		{	CONTROLER,		CONTROLER_IMU_DEAD_ZONE,"motion deadzone",	GYRO_FULL_AXIS_MIN,		GYRO_FULL_AXIS_MAX,		GYRO_FULL_AXIS_DEFAULT,		GYRO_FULL_AXIS_DEFAULT,		SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x026C},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	fx_onoff_ctrl =
		{	CONTROLER,		CONTROLER_EQ, 			"equalizer",		0,						FX_NUM_FX_INTR-1,		FX_PEQ_PRESET_DEFAULTVALUE,	FX_PEQ_PRESET_DEFAULTVALUE,	FX_PEQ_NAME,				0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setgeneraleq,	(uint32_t)displaypreset_onoff,	(uint32_t *)controler_eq_param,	FX_PEQ_USER_SIZE,		0,NULL,	NULL,			0x00,	0x0271};

#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	controler_param[NUM_PARAM_CONTROLER_MAX] = {
// 			p_struct,		p_param,				p_name,				p_min,					p_max,					p_default,					p_sticky,					p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	CONTROLER,		CONTROLER_LEDDISPLAY,	"lighted keys",		LED_DISPLAY_MINVALUE,	LED_DISPLAY_MAXVALUE,	LED_DISPLAY_DEFAULTVALUE,	LED_DISPLAY_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0161},
		{	CONTROLER,		CONTROLER_SCREENMODE,	"display note",		SCREEN_MODE_MINVALUE,	SCREEN_MODE_MAXVALUE,	SCREEN_MODE_DEFAULTVALUE,	SCREEN_MODE_DEFAULTVALUE,	SCREEN_DISPLAY_SCREEN_MODE,	0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0160},
		{	CONTROLER,		CONTROLER_LUMINOSITY,	"keys brightness",	LUMINOSITY_MINVALUE,	LUMINOSITY_MAXVALUE,	LUMINOSITY_DEFAULTVALUE,	LUMINOSITY_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)NULL,				(uint32_t)displayluminositypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0169},

		{	CONTROLER,		CONTROLER_MIDIMODE,		"midi preset",		0,       				(MIDIOUTMODE_NUM-1),	0,							0,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MID,		0,				2,					(uint32_t)setmidioutmode,	(uint32_t)displaymiditable,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0165},
		{	CONTROLER,		CONTROLER_MIDIDIN,		"midi din",			0,       				1,						0,							0,							SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0166},
		//{	CONTROLER,		CONTROLER_CIRCLETIME,	"time circle",		0,						1,						0,							1,							SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0162},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	OTHERS,			PARAM_METRO,			"clic mode",		0,        				(METRONOME_MODE_NUM-1),	0,							0,							SCREEN_DISPLAY_METRO_MODE,	0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)setmusictempomode,(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0163},

		{	CONTROLER,		CONTROLER_PRESSURECURVE,"aftertouch",		GYRO_CURVES_MINVALUE,   GYRO_CURVES_MAXVALUE,	GYRO_CURVES_DEFAULTVALUE,	GYRO_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016D},
		{	CONTROLER,		CONTROLER_SLIDERCURVE,	"sliders",			GYRO_CURVES_MINVALUE,   GYRO_CURVES_MAXVALUE,	GYRO_CURVES_DEFAULTVALUE,	GYRO_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,				(uint32_t)setsliders,			(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016C},
		{	CONTROLER,		CONTROLER_GYROCURVE,	"motion",			GYRO_CURVES_MINVALUE,	GYRO_CURVES_MAXVALUE,	GYRO_CURVES_DEFAULTVALUE,	GYRO_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,				(uint32_t)setgyro,				(uint32_t)NULL,				(uint32_t *)imu_param,2,		0,				NULL,			NULL,			0x00,	0x016B},

		{	CONTROLER,		CONTROLER_ACCESSIBILITY,"accessibility",	0,       				1,						0,							0,							SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)setaccesibility,	(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0164},
		{	CONTROLER,		CONTROLER_DIATONIC,		"key layout",		0,       				(NUM_KEY_LAYOUT-1),		0,							0,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MID,		0,				0x83,				(uint32_t)setdiatonic,		(uint32_t)displaychromadiato,		NULL,		0,			0,				NULL,			NULL,			0x00,	0x0167},
		{	CONTROLER,		CONTROLER_FINETUNING,	"fine tuning",		FINE_TUNING_MINVALUE,	FINE_TUNING_MAXVALUE,	FINE_TUNING_MINVALUE,	    FINE_TUNING_DEFAULTVALUE,	SCREEN_DISPLAY_FINETUNNING,	0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setfinetuning,	(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0168},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},

		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	CONTROLER,		CONTROLER_EQ, 			"equalizer",		0,						FX_NUM_FX_INTR-1,		FX_PEQ_PRESET_DEFAULTVALUE, FX_PEQ_PRESET_DEFAULTVALUE, FX_PEQ_NAME,				0,							CTRL_EVENT_REL_MID,		0,				1,					(uint32_t)setgeneraleq,		(uint32_t)displaypreset,(uint32_t *)controler_eq_param,	FX_PEQ_USER_SIZE,		0,	NULL,			NULL,			0x00,	0x0171},
		{	OTHERS,			PARAM_INFODUTOUCH,		"du-touch",			0,						(NUM_DUTOUCHINFO-1),	0,							0,							SCREEN_CLEAR,				0,							CTRL_EVENT_REL_MID,		0,				1,					(uint32_t)NULL,		 		(uint32_t)display_info,				(uint32_t *)NULL,0,		0,				NULL,			NULL,			0x00,	0x016F},

		//{	CONTROLER,		CONTROLER_KEYCURVE,		"keys sensibility",	KEYS_CURVES_MINVALUE,	KEYS_CURVES_MAXVALUE,	KEYS_CURVES_DEFAULTVALUE,	KEYS_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_KEYCURVES,	0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016E},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0170},

//		{	OTHERS,			PARAM_NONE,				"un-set ctrl",		0,       				0,						0,							0,							0,							0,							0,						0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0172},
};
#ifdef BRUNO_MIDIOUT
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	controler_param_bruno[NUM_PARAM_CONTROLER_MAX] = {
// 			p_struct,		p_param,				p_name,				p_min,					p_max,					p_default,					p_sticky,					p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,		extra_size,	p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	CONTROLER,		CONTROLER_LEDDISPLAY,	"lighted keys",		LED_DISPLAY_MINVALUE,	LED_DISPLAY_MAXVALUE,	LED_DISPLAY_DEFAULTVALUE,	LED_DISPLAY_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0161},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},

		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	CONTROLER,		CONTROLER_MIDIMODE,		"midi preset",		0,       				(MIDIOUTMODE_NUM-1),	0,							0,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MID,		0,				2,					(uint32_t)setmidioutmode,	(uint32_t)displaymiditable,			NULL,		0,			0,				NULL,			NULL,			0x00,	0x0165},
		{	CONTROLER,		CONTROLER_MIDIDIN,		"midi din",			0,       				1,						0,							0,							SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0166},

		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	CONTROLER,		CONTROLER_LUMINOSITY,	"keys brightness",	LUMINOSITY_MINVALUE,	LUMINOSITY_MAXVALUE,	LUMINOSITY_DEFAULTVALUE,	LUMINOSITY_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)NULL,				(uint32_t)displayluminositypercent,	NULL,		0,			0,				NULL,			NULL,			0x00,	0x0169},

		{	CONTROLER,CONTROLER_MIDIOUTKEYSENSIBILITY,"keys sensibility",KEYS_CURVES_MINVALUE,	KEYS_CURVES_MAXVALUE,	KEYS_CURVES_DEFAULTVALUE,	KEYS_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_KEYCURVES,	0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	CONTROLER,		CONTROLER_GYROCURVE,	"motion",			GYRO_CURVES_MINVALUE,	GYRO_CURVES_MAXVALUE,	GYRO_CURVES_DEFAULTVALUE,	GYRO_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,				(uint32_t)setgyro,				(uint32_t)NULL,				(uint32_t *)imu_param,2,		0,				NULL,			NULL,			0x00,	0x016B},
		{	CONTROLER,		CONTROLER_SLIDERCURVE,	"sliders",			GYRO_CURVES_MINVALUE,   GYRO_CURVES_MAXVALUE,	GYRO_CURVES_DEFAULTVALUE,	GYRO_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,				(uint32_t)setsliders,			(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016C},
		{	CONTROLER,		CONTROLER_PRESSURECURVE,"aftertouch",		GYRO_CURVES_MINVALUE,   GYRO_CURVES_MAXVALUE,	GYRO_CURVES_DEFAULTVALUE,	GYRO_CURVES_DEFAULTVALUE,	SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016D},

		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016E},
		{	OTHERS,			PARAM_INFODUTOUCH,		"du-touch",			0,						(NUM_DUTOUCHINFO-1),	0,							0,							SCREEN_CLEAR,				0,							CTRL_EVENT_REL_MID,		0,				1,					(uint32_t)NULL,		 		(uint32_t)display_info,				(uint32_t *)NULL,0,		0,				NULL,			NULL,			0x00,	0x016F},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x0170},

		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
		{	NONE,			0,						"",					0,       				0,						0,							0,							0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,		0,			0,				NULL,			NULL,			0x00,	0x016A},
};
#endif

extern const param_struct	sound_param[];
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	sound_sensor[7] = {
		{	INSTR_PRESET,	SOUND_PRESET_PITCHVALUE,	"pitch",			MAIN_PB_SENS_MIN,   	MAIN_PB_SENS_MAX,   	0x40,							0x40,							SCREEN_DISPLAY_VALUE,		CTRL_EVENT_LINEAR_UP,		CTRL_EVENT_REL_MIN,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setinstrpitchbend,(uint32_t)NULL, 					NULL,							0,					0,				NULL,			NULL,			0x00,	0x0681},
		{	INSTR_PRESET,	SOUND_PRESET_EXPRESSION,	"expression",		MAIN_EXPRESSION_MIN,	MAIN_EXPRESSION_MAX,	MAIN_EXPRESSION_DEFAULT,		MAIN_EXPRESSION_DEFAULT,		SCREEN_DISPLAY_PERCENTAGE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_REL_MIN,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setinstrexpression,(uint32_t)displaypercent, 			NULL,							0,					0,				NULL,			NULL,			0x00,	0x068A},
		{	INSTR_PRESET,	SOUND_PRESET_PANNING,		"panning",			MAIN_PANNING_MIN,   	MAIN_PANNING_MAX,		MAIN_PANNING_MIN,				MAIN_PANNING_DEFAULT,			SCREEN_DISPLAY_PAN,			CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_REL_MIN,	CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setinstrpanning,	(uint32_t)displaypanning,			NULL,							0,					0,				NULL,			NULL,			0x00,	0x068B},
		{	INSTR_PRESET,	SOUND_PRESET_OCTAVE,		"octave",			MAIN_OCTAVE_MIN,		MAIN_OCTAVE_MAX,		MAIN_OCTAVE_DEFAULT,			MAIN_OCTAVE_DEFAULT,			SCREEN_DISPLAY_SIGNEDVALUE,	0,							CTRL_EVENT_REL_MAX,	0,						0,					(uint32_t)setinstroctave,	(uint32_t)displayoctave,			NULL,							0,					0,				NULL,			NULL,			CC_OCTAVE,	0x0683},
		{	INSTR_PRESET,SOUND_PRESET_ARPEGGIATO_BEAT,	"arpeggiator",		0,       				RECORD_BEATLEVELMAX,	0,								0,								SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0686},
		{	INSTR_PRESET,SOUND_PRESET_ARPEGGIATO_BEAT,	"beat repeat",		0,       				RECORD_BEATLEVELMAX,	0,								0,								SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0A86},
		{	INSTR_PRESET,	SOUND_PRESET_VOLUME,		"volume",			MAIN_VOLUME_MIN,		MAIN_VOLUME_MAX,		MAIN_VOLUME_MIN,				MAIN_VOLUME_MIN,				SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,0,					(uint32_t)setinstrvolume,	(uint32_t)displaypercent,			0,								0,					0,				NULL,			NULL,			0x00,	0x0A80}
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	onoff_param =
		{	NONE,			SOUND_PRESET_CHORUS,		"fx rack",			0,       				0,						0,								0,								0,							0,							CTRL_EVENT_DUMMY,		0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,								0,					0,				NULL,			NULL,			0x00,	0x0180}
;
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	fx_onoff_param[2] = {
		{	INSTR_PRESET,	SOUND_PRESET_CHORUS,		"chorus",			0,                  	3,   					0,								0,								SCREEN_DISPLAY_CHORUS,		0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypreset_onoff,		(uint32_t *)sound_chorus_param,		FX_CHORUS_USER_SIZE,0,(uint32_t *)&onoff_param,			NULL,			0x00,	0x0287},
		{	INSTR_PRESET,	SOUND_PRESET_SENDTOREVERB,	"reverb level",		MAIN_SENDTOREV_MIN, 	MAIN_SENDTOREV_MAX, 	MAIN_SENDTOREV_DEFAULT,			MAIN_SENDTOREV_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,  0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setinstrsendtorev,(uint32_t)displaypreset_onoff,		NULL,								0,					0,					NULL,			NULL,			0x00,	0x0289},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	auto_range_param[4] = {
		{	INSTR_PRESET,SOUND_PRESET_AUTOPITCH_RANGE,	"vibrato range",	0,       				127,					127,							127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x0384},
		{	INSTR_PRESET,SOUND_PRESET_TREMOLO_RANGE,	"tremolo range",	0,       				127,					127,							127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x038A},
		{	INSTR_PRESET,SOUND_PRESET_AUTOPAN_RANGE,	"auto-pan range",	0,       				127,					127,							127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x038B},
		{	INSTR_PRESET,SOUND_PRESET_AUTOWAH_RANGE,	"auto-fltr range",	0,       				127,					127,							127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x038D},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	ctrl_rate_param[4] = {
		{	INSTR_PRESET,SOUND_PRESET_AUTOPITCH_RATE,	"vibrato",			0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[0],			1,					0,				NULL,	NULL,	CC_VIBRATO_RATE,	0x0484},
		{	INSTR_PRESET,SOUND_PRESET_TREMOLO_RATE,		"tremolo",			0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[1],			1,					0,				NULL,	NULL,	CC_TREMOLO_RATE,	0x048A},
		{	INSTR_PRESET,SOUND_PRESET_AUTOPAN_RATE,		"auto-pan",			0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[2],			1,					0,				NULL,	NULL,	CC_AUTOPAN_RATE,	0x048B},
		{	INSTR_PRESET,SOUND_PRESET_AUTOWAH_RATE,		"auto-filter",		0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[3],			1,					0,				NULL,	NULL,	CC_AUTOFILTER_RATE,	0x048D},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	auto_rate_param[4] = {
		{	INSTR_PRESET,SOUND_PRESET_AUTOPITCH_RATE,	"vibrato",			0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[0],			(1|0x80),					0,	(uint32_t *)&ctrl_rate_param[0],	(uint32_t *)&sound_param[12],	0x00,	0x0284},
		{	INSTR_PRESET,SOUND_PRESET_TREMOLO_RATE,		"tremolo",			0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[1],			(1|0x80),					0,	(uint32_t *)&ctrl_rate_param[1],	(uint32_t *)&sound_param[10],	0x00,	0x028A},
		{	INSTR_PRESET,SOUND_PRESET_AUTOPAN_RATE,		"auto-pan",			0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[2],			(1|0x80),					0,	(uint32_t *)&ctrl_rate_param[2],	(uint32_t *)&sound_param[11],	0x00,	0x028B},
		{	INSTR_PRESET,SOUND_PRESET_AUTOWAH_RATE,		"auto-filter",		0,       				RECORD_BEATLEVELMAX,	0,							0,								SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&auto_range_param[3],			(1|0x80),					0,	(uint32_t *)&ctrl_rate_param[3],	(uint32_t *)&sound_param[13],	0x00,	0x028D},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	sound_param[NUM_PARAM_SOUND_MAX] = {
// 			p_struct,		p_param,					p_name,				p_min,					p_max,					p_default,						p_sticky,						p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	INSTR_PRESET,	SOUND_PRESET_VOLUME,		"volume",			MAIN_VOLUME_MIN,		MAIN_VOLUME_MAX,		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setinstrvolume,	(uint32_t)displaypercent,			0,								0,					0,(uint32_t *)&sound_sensor[6],			NULL,			0x00,	0x0180},
		{	INSTR_PRESET,	SOUND_PRESET_OCTAVE,		"octave",			MAIN_OCTAVE_MIN,		MAIN_OCTAVE_MAX,		MAIN_OCTAVE_DEFAULT,			MAIN_OCTAVE_DEFAULT,			SCREEN_DISPLAY_SIGNEDVALUE,	0,							CTRL_EVENT_REL_MAX,		0,				2,					(uint32_t)setinstroctave,	(uint32_t)displayoctave,			NULL,							0,					0,(uint32_t *)&sound_sensor[3],	NULL,			0x00,	0x0183},
		{	COPYANDPAST,	COPY_SOUNDPRESET,			"copy & past",		0,        				1,						0,								0,								SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MAX,		0,				9,				(uint32_t)checkcopyandpastparam,(uint32_t)displaycopyandpast,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x0182},

		{	INSTR_PRESET,	SOUND_PRESET_CHORDS,		"chords",			0,       				1,						0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				1,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0184},
		{	INSTR_PRESET,	SOUND_PRESET_ADSR_RELEASE,	"release time",		FX_ADSR_RELEAS_MINVALUE,FX_ADSR_RELEAS_MAXVALUE,FX_ADSR_RELEAS_DEFAULTVALUE,	FX_ADSR_RELEAS_DEFAULTVALUE,	SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,	NULL,							0,					0,				NULL,			(uint32_t *)&sound_adsr_param[0],		0x00,	0x05C4},
		{	INSTR_PRESET,	SOUND_PRESET_PORTTIME,		"portamento",		MAIN_PORT_TIME_MIN, 	MAIN_PORT_TIME_MAX, 	MAIN_PORT_TIME_DEFAULT,			MAIN_PORT_TIME_DEFAULT,			SCREEN_DISPLAY_TIME_OCT,	0,							CTRL_EVENT_REL_MID,		0,				2,					(uint32_t)setinstrporttime,	(uint32_t)displayportamento,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x0185},
		{	INSTR_PRESET,SOUND_PRESET_ARPEGGIATO_BEAT,	"arpeggiator",		0,       				RECORD_BEATLEVELMAX,	0,								0,								SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,(uint32_t *)&sound_sensor[4],			NULL,			0x00,	0x0186},

		{	INSTR_PRESET,	SOUND_PRESET_SENDTOREVERB,	"reverb level",		MAIN_SENDTOREV_MIN, 	MAIN_SENDTOREV_MAX, 	MAIN_SENDTOREV_DEFAULT,			MAIN_SENDTOREV_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,  0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setinstrsendtorev,(uint32_t)displaypercent,			NULL,							0,					0,				NULL,			(uint32_t *)&fx_onoff_param[1],			0x00,	0x0189},
		{	INSTR_PRESET,	SOUND_PRESET_CHORUS,		"chorus",			0,                  	3,   					0,								0,								SCREEN_DISPLAY_CHORUS,		0,							CTRL_EVENT_REL_MID,		0,				2,					(uint32_t)setinstrchorus,	(uint32_t)NULL,						(uint32_t *)sound_chorus_param,	FX_CHORUS_USER_SIZE,0,	(uint32_t *)&onoff_param,(uint32_t *)&fx_onoff_param[0],0x00,	0x0187},
		{	INSTR_PRESET,	SOUND_PRESET_DELAY_ONOFF,	"delay",			0,                  	1,   					0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)setinstrdelay,	(uint32_t)NULL,						(uint32_t *)sound_delay_param,	FX_DELAY_USER_SIZE,	0,	(uint32_t *)&onoff_param,	NULL,			0x00,	0x0188},

		{	INSTR_PRESET,	SOUND_PRESET_WAH_FREQ,		"filter",		FX_WAH_FILTERFREQ_MINVALUE,	FX_WAH_FILTERFREQ_MAXVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,		0,				0xA,				(uint32_t)setinstrwah_freq,	(uint32_t)display8khzfreqwah,		(uint32_t *)sound_wah_param,	(FX_WAH_USER_SIZE|0x80),0,(uint32_t *)&sound_wah_sensor[0],(uint32_t *)&auto_rate_param[3],			0x00,	0x018D},
		{	INSTR_PRESET,	SOUND_PRESET_PANNING,		"panning",			MAIN_PANNING_MIN,   	MAIN_PANNING_MAX,		MAIN_PANNING_DEFAULT,			MAIN_PANNING_DEFAULT,			SCREEN_DISPLAY_PAN,			0,							CTRL_EVENT_REL_MIN,		0,				0xA,				(uint32_t)setinstrpanning,	(uint32_t)displaypanning,			NULL,							0,					0,(uint32_t *)&sound_sensor[2],(uint32_t *)&auto_rate_param[2],			0x00,	0x018B},
		{	INSTR_PRESET,	SOUND_PRESET_PITCHSEND,		"pitch",			MAIN_PB_SENS_MIN,   	MAIN_PB_SENS_MAX,   	MAIN_PB_SENS_DEFAULT,			MAIN_PB_SENS_DEFAULT,			SCREEN_DISPLAY_PITCH,		0,							CTRL_EVENT_REL_MID,		0,				0xA,			(uint32_t)setinstrpitchbendsens,(uint32_t)NULL, 					NULL,							0,					0,(uint32_t *)&sound_sensor[0],(uint32_t *)&auto_rate_param[0],0x00,	0x018C},
		{	INSTR_PRESET,	SOUND_PRESET_EXPRESSION,	"expression",		MAIN_EXPRESSION_MIN,	MAIN_EXPRESSION_MAX,	MAIN_EXPRESSION_DEFAULT,		MAIN_EXPRESSION_DEFAULT,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				0xA,				(uint32_t)setinstrexpression,(uint32_t)displaypercent, 			NULL,							0,					0,(uint32_t *)&sound_sensor[1],(uint32_t *)&auto_rate_param[1],			0x00,	0x018A},
		//{	NONE,			0,							"",					0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x018C},
		//{	INSTR_PRESET,	SOUND_PRESET_VIBRATO,		"vibrato",			0,                  	FX_NUM_FX_INTR-1,   	FX_VIB_PRESET_DEFAULTVALUE,		FX_VIB_PRESET_DEFAULTVALUE,		FX_VIBRATO_NAME,			0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstrvibrato,	(uint32_t)displaypreset,			(uint32_t *)sound_vibrato_param,FX_VIBRATO_USER_SIZE,0,	(uint32_t *)&onoff_param[1],(uint32_t *)&fx_onoff_param[6],			0x00,	0x018C},
		//{	INSTR_PRESET,	SOUND_PRESET_WAH,			"wah-wah",			0,                  	FX_NUM_FX_INTR-1,   	FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_NAME,				0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstrwah,		(uint32_t)displaypreset,			(uint32_t *)sound_wah_param,	FX_WAH_USER_SIZE,	0,	(uint32_t *)&onoff_param[1],(uint32_t *)&auto_rate_param[3],			0x00,	0x018D},

		//{	INSTR_PRESET,	SOUND_PRESET_COMPRESSOR,	"compressor",		0,                  	FX_NUM_FX_INTR-1,   	FX_COMP_PRESET_DEFAULTVALUE,	FX_COMP_PRESET_DEFAULTVALUE,	FX_COMP_NAME,				0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstrcompressor,(uint32_t)displaypreset,			(uint32_t *)sound_compressor_param,	FX_COMP_USER_SIZE,0,(uint32_t *)&onoff_param[0],(uint32_t *)&fx_onoff_param[1],			0x00,	0x018E},
		{	INSTR_PRESET,	SOUND_PRESET_DISTORTION_ONOFF,"distortion",		0,		            	1,						0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)setinstrdistortion,(uint32_t)NULL,					(uint32_t *)sound_distortion_param,FX_DISTO_USER_SIZE,0,(uint32_t *)&onoff_param,	NULL,			0x00,	0x018F},
		//{	INSTR_PRESET,	SOUND_PRESET_ADSR,			"adsr",				0,                  	FX_NUM_FX_INTR-1,   	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_NAME,				0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstradsr,	 	(uint32_t)displaypreset,			(uint32_t *)sound_adsr_param,	FX_ADSR_USER_SIZE,	0,	(uint32_t *)&onoff_param[0],(uint32_t *)&fx_onoff_param[0],			0x00,	0x0190},
		{	INSTR_PRESET,	SOUND_PRESET_EQ_ONOFF,		"equalizer",		0,                  	1,   					0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				0xB,				(uint32_t)setinstreq,		(uint32_t)NULL,						(uint32_t *)sound_eq_param,		FX_PEQ_USER_SIZE,	0,	(uint32_t *)&onoff_param,	NULL,			0x00,	0x0191},
		{	NONE,			0,							"",					0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x018E},

		{	OTHERS,			PARAM_NONE,					"un-set ctrl",		0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0192},
		{	INSTR_PRESET,	SOUND_PRESET_KEY_CURVE,		"keys sensitivity",	KEYS_CURVES_MINVALUE,	KEYS_CURVES_MAXVALUE,	KEYS_CURVES_DEFAULTVALUE,		KEYS_CURVES_DEFAULTVALUE,		SCREEN_DISPLAY_KEYCURVES,	0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0181},


			//
		//{	OTHERS,			PARAM_INSTRUVOL,			"relativ vol",		MAIN_EXPRESSION_MIN,	MAIN_EXPRESSION_MAX,	MAIN_EXPRESSION_DEFAULT,		MAIN_EXPRESSION_DEFAULT,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,	0,				0,					(uint32_t)setinstrrelvolume,(uint32_t)displaypercent, 			NULL,							0,					0,				NULL,			NULL,			0x00,	0x018C},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	sound_param_ps[NUM_PARAM_SOUND_MAX] = {
// 			p_struct,		p_param,					p_name,				p_min,					p_max,					p_default,						p_sticky,						p_screen,					p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	INSTR_PRESET,	SOUND_PRESET_VOLUME,		"volume",			MAIN_VOLUME_MIN,		MAIN_VOLUME_MAX,		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setinstrvolume,	(uint32_t)displaypercent,			0,								0,					0,(uint32_t *)&sound_sensor[6],			NULL,			0x00,	0x0180},
		{	OTHERS,			PARAM_INSTRLED,				"marks",			0,        				1,						0,								0,								SCREEN_DISPLAY_LED_MODE,	0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)switchledmode_ps,	(uint32_t)displaymarks,				NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A1},
		{	COPYANDPAST,	COPY_SOUNDPRESET,			"copy & past",		0,        				1,						0,								0,								SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MAX,		0,				9,				(uint32_t)checkcopyandpastparam,(uint32_t)displaycopyandpast,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x0182},

		//{	NONE,			0,							"",					0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0183},
		//{	INSTR_PRESET,	SOUND_PRESET_CHORDS,		"chords",			0,       				1,						0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0184},
		{	NONE,			0,							"",					0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0184},
		{	INSTR_PRESET,	SOUND_PRESET_ADSR_RELEASE,	"release time",		FX_ADSR_RELEAS_MINVALUE,FX_ADSR_RELEAS_MAXVALUE,FX_ADSR_RELEAS_DEFAULTVALUE,	FX_ADSR_RELEAS_DEFAULTVALUE,	SCREEN_DISPLAY_TIME,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,	NULL,							0,					0,				NULL,(uint32_t *)&sound_adsr_param[0],		0x00,	0x05C4},
		{	NONE,			0,							"",					0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0185},
		{	INSTR_PRESET,SOUND_PRESET_ARPEGGIATO_BEAT,	"beat repeat",		0,       				RECORD_BEATLEVELMAX,	0,								0,								SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,(uint32_t *)&sound_sensor[5],	NULL,			0x00,	0x0186},

		{	INSTR_PRESET,	SOUND_PRESET_SENDTOREVERB,	"reverb level",		MAIN_SENDTOREV_MIN, 	MAIN_SENDTOREV_MAX, 	MAIN_SENDTOREV_DEFAULT,			MAIN_SENDTOREV_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,  0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setinstrsendtorev,(uint32_t)displaypercent,			NULL,							0,					0,				NULL,			(uint32_t *)&fx_onoff_param[1],			0x00,	0x0189},
		{	INSTR_PRESET,	SOUND_PRESET_CHORUS,		"chorus",			0,                  	3,   					0,								0,								SCREEN_DISPLAY_CHORUS,		0,							CTRL_EVENT_REL_MID,		0,				2,					(uint32_t)setinstrchorus,	(uint32_t)NULL,						(uint32_t *)sound_chorus_param,	FX_CHORUS_USER_SIZE,0,				NULL,(uint32_t *)&fx_onoff_param[0],0x00,	0x0187},
		{	INSTR_PRESET,	SOUND_PRESET_DELAY_ONOFF,	"delay",			0,                  	1,   					0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				2,					(uint32_t)setinstrdelay,	(uint32_t)NULL,						(uint32_t *)sound_delay_param,	FX_DELAY_USER_SIZE,	0,				NULL,			NULL,			0x00,	0x0188},

		{	INSTR_PRESET,	SOUND_PRESET_WAH_FREQ,		"filter",		FX_WAH_FILTERFREQ_MINVALUE,	FX_WAH_FILTERFREQ_MAXVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,		0,				0xA,				(uint32_t)setinstrwah_freq,	(uint32_t)display8khzfreqwah,		(uint32_t *)sound_wah_param,	(FX_WAH_USER_SIZE|0x80),0,(uint32_t *)&sound_wah_sensor[0],(uint32_t *)&auto_rate_param[3],			0x00,	0x018D},
		{	INSTR_PRESET,	SOUND_PRESET_PANNING,		"panning",			MAIN_PANNING_MIN,   	MAIN_PANNING_MAX,		MAIN_PANNING_DEFAULT,			MAIN_PANNING_DEFAULT,			SCREEN_DISPLAY_PAN,			0,							CTRL_EVENT_REL_MIN,		0,				0xA,				(uint32_t)setinstrpanning,	(uint32_t)displaypanning,			NULL,							0,					0,(uint32_t *)&sound_sensor[2],(uint32_t *)&auto_rate_param[2],			0x00,	0x018B},
		{	INSTR_PRESET,	SOUND_PRESET_PITCHSEND,		"pitch",			MAIN_PB_SENS_MIN,   	MAIN_PB_SENS_MAX,   	MAIN_PB_SENS_DEFAULT,			MAIN_PB_SENS_DEFAULT,			SCREEN_DISPLAY_PITCH,		0,							CTRL_EVENT_REL_MID,		0,				0xA,			(uint32_t)setinstrpitchbendsens,(uint32_t)NULL, 					NULL,							0,					0,(uint32_t *)&sound_sensor[0],(uint32_t *)&auto_rate_param[0],0x00,	0x018C},
		{	INSTR_PRESET,	SOUND_PRESET_EXPRESSION,	"expression",		MAIN_EXPRESSION_MIN,	MAIN_EXPRESSION_MAX,	MAIN_EXPRESSION_DEFAULT,		MAIN_EXPRESSION_DEFAULT,		SCREEN_DISPLAY_PERCENTAGE,	0,							CTRL_EVENT_REL_MIN,		0,				0xA,				(uint32_t)setinstrexpression,(uint32_t)displaypercent, 			NULL,							0,					0,(uint32_t *)&sound_sensor[1],(uint32_t *)&auto_rate_param[1],			0x00,	0x018A},
		//{	NONE,			0,							"",					0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x018C},
		//{	INSTR_PRESET,	SOUND_PRESET_VIBRATO,		"vibrato",			0,                  	FX_NUM_FX_INTR-1,   	FX_VIB_PRESET_DEFAULTVALUE,		FX_VIB_PRESET_DEFAULTVALUE,		FX_VIBRATO_NAME,			0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstrvibrato,	(uint32_t)displaypreset,			(uint32_t *)sound_vibrato_param,FX_VIBRATO_USER_SIZE,0,				NULL,(uint32_t *)&fx_onoff_param[6],			0x00,	0x018C},
		//{	INSTR_PRESET,	SOUND_PRESET_WAH,			"wah-wah",			0,                  	FX_NUM_FX_INTR-1,   	FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_NAME,				0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstrwah,		(uint32_t)displaypreset,			(uint32_t *)sound_wah_param,	FX_WAH_USER_SIZE,	0,				NULL,(uint32_t *)&auto_rate_param[3],			0x00,	0x018D},

		{	INSTR_PRESET,	SOUND_PRESET_DISTORTION_ONOFF,"distortion",		0,		            	1,						0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				2,					(uint32_t)setinstrdistortion,(uint32_t)NULL,					(uint32_t *)sound_distortion_param,FX_DISTO_USER_SIZE,0,			NULL,			NULL,			0x00,	0x018F},
		//{	INSTR_PRESET,	SOUND_PRESET_COMPRESSOR,	"compressor",		0,                  	FX_NUM_FX_INTR-1,   	FX_COMP_PRESET_DEFAULTVALUE,	FX_COMP_PRESET_DEFAULTVALUE,	FX_COMP_NAME,				0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstrcompressor,(uint32_t)displaypreset,			(uint32_t *)sound_compressor_param,	FX_COMP_USER_SIZE,0,			NULL,(uint32_t *)&fx_onoff_param[1],			0x00,	0x018E},
		//{	INSTR_PRESET,	SOUND_PRESET_ADSR,			"adsr",				0,                  	FX_NUM_FX_INTR-1,   	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_NAME,				0,							CTRL_EVENT_REL_MID,		0,				0,					(uint32_t)setinstradsr,	 	(uint32_t)displaypreset,			(uint32_t *)sound_adsr_param,	FX_ADSR_USER_SIZE,	0,				NULL,(uint32_t *)&fx_onoff_param[0],			0x00,	0x0190},
		{	INSTR_PRESET,	SOUND_PRESET_EQ_ONOFF,		"equalizer",		0,                  	1,   					0,								0,								SCREEN_DISPLAY_BOOL,		0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)setinstreq,		(uint32_t)NULL,						(uint32_t *)sound_eq_param,		FX_PEQ_USER_SIZE,	0,				NULL,			NULL,			0x00,	0x0191},
		{	NONE,			0,							"",					0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x018E},

		{	OTHERS,			PARAM_NONE,					"un-set ctrl",		0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0192},
		{	INSTR_PRESET,	SOUND_PRESET_KEY_CURVE,		"keys sensibility",	KEYS_CURVES_MINVALUE,	KEYS_CURVES_MAXVALUE,	KEYS_CURVES_DEFAULTVALUE,		KEYS_CURVES_DEFAULTVALUE,		SCREEN_DISPLAY_KEYCURVES,	0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0181},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	music_sensor[6] = {
// 			p_struct,		p_param,				p_name,			p_min,						p_max,					p_default,						p_sticky,						p_screen,						p_linktoaftertouch,			p_linktoslider,			p_linktogyro,		p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_SONG,		SONG_VOLUME,			"volume",			MAIN_VOLUME_MIN,   		MAIN_VOLUME_MAX,		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_LINEAR,		CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setmusicvolume, 	(uint32_t)displaypercent,			NULL,							0,					0,				NULL,			NULL,			0x00,	0x06A0},
		{	MUSIC_SONG,		SONG_TEMPO,				"tempo",			MUSIC_TEMPO_MINVALUE,	MUSIC_TEMPO_MAXVALUE,   MUSIC_TEMPO_DEFAULTVALUE,		MUSIC_TEMPO_DEFAULTVALUE,		SCREEN_DISPLAY_VALUE,			0,						CTRL_EVENT_LINEAR,		CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setmusictempo,	(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x06A4},
		{	MUSIC_SONG,		SONG_VOLTEMPO,			"clic level",		MUSIC_TEMPOVOL_MINVALUE,MUSIC_TEMPOVOL_MAXVALUE,MUSIC_TEMPOVOL_DEFAULTVALUE,	MUSIC_TEMPOVOL_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,		0,						CTRL_EVENT_LINEAR,		0,						0,					(uint32_t)setmusictempovolume,(uint32_t)displaypercent,			NULL,							0,					0,				NULL,			NULL,			0x00,	0x06A5},
		{	OTHERS,			PARAM_MUSICBEATREPEAT,	"beat repeat",		0,						RECORD_BEATLEVELMAX,	RECORD_BEATLEVELDEFAULT,		RECORD_BEATLEVELDEFAULT,		SCREEN_DISPLAY_BEAT_STATE,		0,						CTRL_EVENT_LINEAR_RET,	0,						0,					(uint32_t)setmusicbeatrepeat,(uint32_t)NULL,					0,								0,					0,				NULL,			NULL,			0x00,	0x06A6},
		{	MUSIC_SONG,		SONG_TRANSPOSE,			"transpose",		0,        				((2*RECORD_TRANSPOSEDEFAULT)-1),RECORD_TRANSPOSEDEFAULT,RECORD_TRANSPOSEDEFAULT,		SCREEN_DISPLAY_SIGNEDVALUE,		0,						CTRL_EVENT_LINEAR,		0,						0,					(uint32_t)transpose,		(uint32_t)displaytranspose,			NULL,							0,					0,				NULL,			NULL,			0x00,	0x06A7},
		{	MUSIC_MIX,		FX_MIX_HCFILTER,		"lowpass",			FX_MIX_HCFREQ_MINVALUE,	FX_MIX_HCFREQ_MAXVALUE,	FX_MIX_HCFREQ_MINVALUE,			FX_MIX_HCFREQ_MINVALUE,			SCREEN_DISPLAY_FREQUENCY,		CTRL_EVENT_LINEAR_REF,	CTRL_EVENT_LINEAR,		CTRL_EVENT_LINEAR_GYRO,	0,					(uint32_t)setmusicmix,		(uint32_t)displaymixlp,				NULL,							0,					0,				NULL,			NULL,			0x00,	0x06AA},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	music_swing_param =
// 			p_struct,		p_param,					p_name,				p_min,					p_max,					p_default,			p_sticky,						p_screen,						p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_SONG,		SONG_SWING,					"swing",			0,        				127,					0,					0,								SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,	0,				6,					(uint32_t)NULL,	 			(uint32_t)displaypercent,			NULL,							0,					0,				NULL,		(uint32_t *)NULL,	0x00,	0x01A6};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	music_param[NUM_PARAM_MUSIC_MAX] = {
// 			p_struct,		p_param,					p_name,				p_min,					p_max,					p_default,						p_sticky,						p_screen,						p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_SONG,		SONG_VOLUME,				"volume",			MAIN_VOLUME_MIN,   		MAIN_VOLUME_MAX,		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setmusicvolume, 	(uint32_t)displaypercent,			NULL,							0,					0,(uint32_t *)&music_sensor[0],	NULL,			0x00,	0x01A0},
		{	MUSIC_SONG,		SONG_DISPLAYNOTE,			"scale",			0,        				NUM_LED_MODE-1,			NONE_LED_MODE,					NONE_LED_MODE,					SCREEN_DISPLAY_LED_MODE,		0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)switchledmode,	(uint32_t)displayscale,				NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A1},
		{	COPYANDPAST,	COPY_MUSIC,					"copy & past",		0,        				2,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MAX,		0,				9,				(uint32_t)checkcopyandpastparam,(uint32_t)displaycopyandpast,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A2},

		{	MUSIC_SONG,		SONG_TIMESIGNATURE,			"signature",		0,        				(NUM_TIMESIGNATURE - 1),0,								0,								SCREEN_DISPLAY_TIME_SIGNATURE,	0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A3},
		{	MUSIC_SONG,		SONG_TEMPO,					"tempo",			MUSIC_TEMPO_MINVALUE,	MUSIC_TEMPO_MAXVALUE,   MUSIC_TEMPO_MINVALUE,			MUSIC_TEMPO_DEFAULTVALUE,		SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setmusictempo,	(uint32_t)NULL,						NULL,							0,					0,(uint32_t *)&music_sensor[1],	NULL,			0x00,	0x01A4},
		{	MUSIC_SONG,		SONG_VOLTEMPO,				"clic level",		MUSIC_TEMPOVOL_MINVALUE,MUSIC_TEMPOVOL_MAXVALUE,MUSIC_TEMPOVOL_DEFAULTVALUE,	MUSIC_TEMPOVOL_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,		0,				6,					(uint32_t)setmusictempovolume,(uint32_t)displaypercent,			NULL,							0,					0,(uint32_t *)&music_sensor[2],	NULL,			0x00,	0x01A5},
		{	MUSIC_SONG,		SONG_QUANTIZE,				"quantizer",		0,        				RECORD_QUANTLEVELMAX,	RECORD_QUANTLEVELDEFAULT,		RECORD_QUANTLEVELDEFAULT,		SCREEN_DISPLAY_REC_QUANT_STATE,	0,							CTRL_EVENT_REL_MAX,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,			(uint32_t *)&music_swing_param,				1,					0,(uint32_t *)&music_sensor[3],	NULL,			0x00,	0x01A6},

		{	MUSIC_SONG,		SONG_TRANSPOSE,				"transpose",			0,        	((2*RECORD_TRANSPOSEDEFAULT)-1),RECORD_TRANSPOSEDEFAULT,		RECORD_TRANSPOSEDEFAULT,		SCREEN_DISPLAY_SIGNEDVALUE,		0,							CTRL_EVENT_REL_MID,		0,				2,					(uint32_t)transpose,		(uint32_t)displaytranspose,			NULL,							0,					0,(uint32_t *)&music_sensor[4],	NULL,			0x00,	0x01A7},
		{	NONE,			PARAM_NONE,					"",					0,        				0,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_NONE,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A8},
		{	NONE,			PARAM_NONE,					"",					0,        				0,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_NONE,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01AA},

		{	NONE,			PARAM_NONE,					"",					0,        				0,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_NONE,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01AA},
		//{	OTHERS,			PARAM_NONE,					"default du-music",	0,        				0,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_NONE,		0,				0,					(uint32_t)NULL,				(uint32_t)displaydefaultsong,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A8},
		//{	MUSIC_SONG,		SONG_REVERBPRESET,			"reverb",			0,						FX_NUM_FX_INTR-1,		FX_REVERB_PRESET_DEFAULTVALUE, 	FX_REVERB_PRESET_DEFAULTVALUE, 	FX_REVERB_NAME,					0,							CTRL_EVENT_REL_MAX,		0,				2,					(uint32_t)setgeneralreverb,	(uint32_t)displaypreset,		(uint32_t *)music_reverb_param,		FX_REVERB_USER_SIZE,0,				NULL,			NULL,			0x00,	0x01A9},
		//{	MUSIC_MIX,		FX_MIX_HCFILTER,			"lowpass",			FX_MIX_HCFREQ_MINVALUE,	FX_MIX_HCFREQ_MAXVALUE,	FX_MIX_HCFREQ_MAXVALUE,			FX_MIX_HCFREQ_MAXVALUE,			SCREEN_DISPLAY_FREQUENCY,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setmusicmix,		(uint32_t)displaymixlp,				NULL,							0,					0,(uint32_t *)&music_sensor[5],	NULL,			0x00,	0x01AA},

		{	OTHERS,			PARAM_NONE,					"un-set ctrl",		0,       				0,						0,								0,								0,								0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0192},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	music_param_ps[NUM_PARAM_MUSIC_MAX] = {
// 			p_struct,		p_param,					p_name,				p_min,					p_max,					p_default,						p_sticky,						p_screen,						p_linktoaftertouch,			p_linktoslider,			p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_SONG,		SONG_VOLUME,				"volume",			MAIN_VOLUME_MIN,   		MAIN_VOLUME_MAX,		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setmusicvolume, 	(uint32_t)displaypercent,			NULL,							0,					0,(uint32_t *)&music_sensor[0],	NULL,			0x00,	0x01A0},
		{	OTHERS,			PARAM_INSTRLED,				"marks",			0,        				1,						0,								0,								SCREEN_DISPLAY_LED_MODE,		0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)switchledmode_ps,	(uint32_t)displaymarks,				NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A1},
		{	COPYANDPAST,	COPY_MUSIC,					"copy & past",		0,        				2,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MAX,		0,				9,				(uint32_t)checkcopyandpastparam,(uint32_t)displaycopyandpast,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A2},

		{	MUSIC_SONG,		SONG_TIMESIGNATURE,			"signature",		0,        				(NUM_TIMESIGNATURE - 1),0,								0,								SCREEN_DISPLAY_TIME_SIGNATURE,	0,							CTRL_EVENT_REL_MAX,		0,				3,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A3},
		{	MUSIC_SONG,		SONG_TEMPO,					"tempo",			MUSIC_TEMPO_MINVALUE,	MUSIC_TEMPO_MAXVALUE,   MUSIC_TEMPO_MINVALUE,			MUSIC_TEMPO_DEFAULTVALUE,		SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)setmusictempo,	(uint32_t)NULL,						NULL,							0,					0,(uint32_t *)&music_sensor[1],	NULL,			0x00,	0x01A4},
		{	MUSIC_SONG,		SONG_VOLTEMPO,				"clic level",		MUSIC_TEMPOVOL_MINVALUE,MUSIC_TEMPOVOL_MAXVALUE,MUSIC_TEMPOVOL_DEFAULTVALUE,	MUSIC_TEMPOVOL_DEFAULTVALUE,	SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,		0,				6,					(uint32_t)setmusictempovolume,(uint32_t)displaypercent,			NULL,							0,					0,(uint32_t *)&music_sensor[2],	NULL,			0x00,	0x01A5},
		{	MUSIC_SONG,		SONG_QUANTIZE,				"quantizer",		0,        				RECORD_QUANTLEVELMAX,	RECORD_QUANTLEVELDEFAULT,		RECORD_QUANTLEVELDEFAULT,		SCREEN_DISPLAY_REC_QUANT_STATE,	0,							CTRL_EVENT_REL_MAX,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,			(uint32_t *)&music_swing_param,				1,					0,(uint32_t *)&music_sensor[3],	NULL,			0x00,	0x01A6},

		{	MUSIC_SONG,		SONG_TRANSPOSE,				"transpose",			0,        	((2*RECORD_TRANSPOSEDEFAULT)-1),RECORD_TRANSPOSEDEFAULT,		RECORD_TRANSPOSEDEFAULT,		SCREEN_DISPLAY_SIGNEDVALUE,		0,							CTRL_EVENT_REL_MID,		0,				2,					(uint32_t)transpose,		(uint32_t)displaytranspose,			NULL,							0,					0,(uint32_t *)&music_sensor[4],	NULL,			0x00,	0x01A7},
		{	NONE,			PARAM_NONE,					"",					0,        				0,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_NONE,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01A8},
		{	NONE,			PARAM_NONE,					"",					0,        				0,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_NONE,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01AA},

		{	NONE,			PARAM_NONE,					"",					0,        				0,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_NONE,		0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01AA},
		//{	MUSIC_SONG,		SONG_REVERBPRESET,			"reverb",			0,						FX_NUM_FX_INTR-1,		FX_REVERB_PRESET_DEFAULTVALUE, 	FX_REVERB_PRESET_DEFAULTVALUE, 	FX_REVERB_NAME,					0,							CTRL_EVENT_REL_MAX,		0,				2,					(uint32_t)setgeneralreverb,	(uint32_t)displaypreset,		(uint32_t *)music_reverb_param,		FX_REVERB_USER_SIZE,0,				NULL,			NULL,			0x00,	0x01A9},
		//{	MUSIC_MIX,		FX_MIX_HCFILTER,			"lowpass",			FX_MIX_HCFREQ_MINVALUE,	FX_MIX_HCFREQ_MAXVALUE,	FX_MIX_HCFREQ_MAXVALUE,			FX_MIX_HCFREQ_MAXVALUE,			SCREEN_DISPLAY_FREQUENCY,		0,							CTRL_EVENT_REL_MIN,		0,				0,					(uint32_t)setmusicmix,		(uint32_t)displaymixlp,				NULL,							0,					0,(uint32_t *)&music_sensor[5],	NULL,			0x00,	0x01AA},
		{	OTHERS,			PARAM_NONE,					"un-set ctrl",		0,       				0,						0,								0,								0,								0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0192},
};

extern const param_struct	music_track_param[];
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	loop_changesound_param =
// 			p_struct,			p_param,					p_name,				p_min,					p_max,					p_default,			p_sticky,						p_screen,						p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	OTHERS,				PARAM_LOOPSOUND,			"change sound",		0,        				0,						0,					0,								SCREEN_CLEAR,			0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)NULL,	 			(uint32_t)NULL,						NULL,							0,					0,				NULL,(uint32_t *)&music_track_param[0],0x00,	0x02C0};

#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	loop_sensor[6] = {
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_VOLUME,		"volume",			MAIN_VOLUME_MIN,   		MAIN_VOLUME_MAX,   		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,							0,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)setinstrvolume,	(uint32_t)displaypercent,			NULL,							0,					0,				NULL,			NULL,			0x00,	0x06C0},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_OCTAVE,		"octave",			MAIN_OCTAVE_MIN,		MAIN_OCTAVE_MAX,		MAIN_OCTAVE_DEFAULT,			MAIN_OCTAVE_DEFAULT,			SCREEN_DISPLAY_SIGNEDVALUE, 						0,		CTRL_EVENT_LINEAR,	CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)setinstroctave,	(uint32_t)displayoctave,			NULL,							0,					0,				NULL,			NULL,			0x00,	0x06C3},
		{	OTHERS,				PARAM_LOOPBEATREPEAT,		"beat repeat",		0,						RECORD_BEATLEVELMAX,	RECORD_BEATLEVELDEFAULT,		RECORD_BEATLEVELDEFAULT,		SCREEN_DISPLAY_BEAT_STATE,							0,		CTRL_EVENT_LINEAR_RET,	0,								0,					(uint32_t)setloopbeatrepeat,(uint32_t)NULL,						0,								0,					0,				NULL,			NULL,			0x00,	0x06C6},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_EXPRESSION,	"expression",		MAIN_EXPRESSION_MIN,	MAIN_EXPRESSION_MAX,	MAIN_EXPRESSION_DEFAULT,		MAIN_EXPRESSION_DEFAULT,		SCREEN_DISPLAY_PERCENTAGE,		CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_REL_MIN,	CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)setinstrexpression,(uint32_t)displaypercent, 			NULL,							0,					0,				NULL,			NULL,			0x00,	0x06CA},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_PANNING,		"panning",			MAIN_PANNING_MIN,   	MAIN_PANNING_MAX,		MAIN_PANNING_MIN,				MAIN_PANNING_DEFAULT,			SCREEN_DISPLAY_PAN,				CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_REL_MIN,	CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)setinstrpanning,	(uint32_t)displaypanning,			NULL,							0,					0,				NULL,			NULL,			0x00,	0x06CB},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_PITCHVALUE,	"pitch",			MAIN_PB_SENS_MIN,   	MAIN_PB_SENS_MAX,   	0x40,							0x40,							SCREEN_DISPLAY_VALUE,			CTRL_EVENT_LINEAR_UP,		CTRL_EVENT_REL_MIN,	CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)setinstrpitchbend,(uint32_t)NULL, 					NULL,							0,					0,				NULL,			NULL,			0x00,	0x06C1},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	musicfx_onoff_param[2] = {
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_CHORUS,		"chorus",			0,                  	3,   					0,								0,								SCREEN_DISPLAY_CHORUS,			0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)displaypreset_onoff,		(uint32_t *)music_chorus_param,		FX_CHORUS_USER_SIZE,0,				NULL,			NULL,			0x00,	0x02C7},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_SENDTOREVERB,	"reverb level",		MAIN_SENDTOREV_MIN, 	MAIN_SENDTOREV_MAX, 	MAIN_SENDTOREV_DEFAULT,			MAIN_SENDTOREV_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,   	0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstrsendtorev,(uint32_t)displaypreset_onoff,		NULL,								0,					0,				NULL,			NULL,			0x00,	0x02C9},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	musicauto_range_param[4] = {
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOPITCH_RANGE,	"vibrato range",	0,       				127,					127,					127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x03C4},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_TREMOLO_RANGE,		"tremolo range",	0,       				127,					127,					127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x03CA},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOPAN_RANGE,		"auto-pan range",	0,       				127,					127,					127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x03CB},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOWAH_RANGE,		"auto-fltr range",	0,       				127,					127,					127,							SCREEN_DISPLAY_VALUE,		0,							CTRL_EVENT_REL_MIN,		0,				2,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,	0x00,	0x03CD},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	musicctrl_rate_param[4] = {
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOPITCH_RATE,		"vibrato",			0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[0],			1,					0,				NULL,	NULL,	0x00,	0x04C4},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_TREMOLO_RATE,		"tremolo",			0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[1],			1,					0,				NULL,	NULL,	0x00,	0x04CA},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOPAN_RATE,		"auto-pan",			0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[2],			1,					0,				NULL,	NULL,	0x00,	0x04CB},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOWAH_RATE,		"auto-filter",		0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	CTRL_EVENT_LINEAR_REF,		CTRL_EVENT_LINEAR_RET,CTRL_EVENT_LINEAR_GYRO,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[3],			1,					0,				NULL,	NULL,	0x00,	0x04CD},
};
#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	musicauto_rate_param[4] = {
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOPITCH_RATE,		"vibrato",			0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[0],			(1|0x80),					0,				(uint32_t *)&musicctrl_rate_param[0],	(uint32_t *)&music_track_param[12],	0x00,	0x02C4},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_TREMOLO_RATE,		"tremolo",			0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[1],			(1|0x80),					0,				(uint32_t *)&musicctrl_rate_param[1],	(uint32_t *)&music_track_param[10],	0x00,	0x02CA},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOPAN_RATE,		"auto-pan",			0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[2],			(1|0x80),					0,				(uint32_t *)&musicctrl_rate_param[2],	(uint32_t *)&music_track_param[11],	0x00,	0x02CB},
		{	MUSIC_INSTR_PRESET,SOUND_PRESET_AUTOWAH_RATE,		"auto-filter",		0,       				RECORD_BEATLEVELMAX,	0,							0,							SCREEN_DISPLAY_BEAT_STATE,	0,							CTRL_EVENT_REL_MID,		0,				8,					(uint32_t)NULL,		 		(uint32_t)NULL,			(uint32_t *)&musicauto_range_param[3],			(1|0x80),					0,				(uint32_t *)&musicctrl_rate_param[3],	(uint32_t *)&music_track_param[13],	0x00,	0x02CD},
};

#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	music_track_param[NUM_PARAM_MUSIC_TRACK_MAX] = {
// 			p_struct,			p_param,					p_name,				p_min,					p_max,					p_default,						p_sticky,						p_screen,						p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_VOLUME,		"volume",			MAIN_VOLUME_MIN,   		MAIN_VOLUME_MAX,   		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstrvolume,	(uint32_t)displaypercent,			NULL,							0,					0,	(uint32_t *)&loop_sensor[0],(uint32_t *)&loop_changesound_param,	0x00,	0x01C0},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_OCTAVE,		"octave",			MAIN_OCTAVE_MIN,		MAIN_OCTAVE_MAX,		MAIN_OCTAVE_DEFAULT,			MAIN_OCTAVE_DEFAULT,			SCREEN_DISPLAY_SIGNEDVALUE, 	0,							CTRL_EVENT_REL_MAX,	0,				2,					(uint32_t)setinstroctave,	(uint32_t)displayoctave,			NULL,							0,					0,	(uint32_t *)&loop_sensor[1],			NULL,			0x00,	0x01C3},
		{	COPYANDPAST,		COPY_LOOP,					"copy & past",		0,        				1,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MAX,	0,				9,				(uint32_t)checkcopyandpastparam,(uint32_t)displaycopyandpast,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C2},

		{	MUSIC_LOOP,			LOOP_LEARN,					"score",			0,        				(NUM_LEARNMODE - 2),	0,								0,								SCREEN_DISPLAY_LEARN,			0,							CTRL_EVENT_REL_MAX,	0,				1,					(uint32_t)setmusiclearn,	(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C1},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_ADSR_RELEASE,	"release time",		FX_ADSR_RELEAS_MINVALUE,FX_ADSR_RELEAS_MAXVALUE,FX_ADSR_RELEAS_DEFAULTVALUE,	FX_ADSR_RELEAS_DEFAULTVALUE,	SCREEN_DISPLAY_TIME,			0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,	NULL,							0,					0,				NULL,(uint32_t *)&music_adsr_param[0],				0x00,	0x0144},
		{	NONE,				0,							"",					0,       				0,						0,								0,								0,								0,							0,					0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C4},
		{	OTHERS,				PARAM_LOOPBEATREPEAT,		"beat repeat",		0,						RECORD_BEATLEVELMAX,	RECORD_BEATLEVELDEFAULT,		RECORD_BEATLEVELDEFAULT,		SCREEN_DISPLAY_BEAT_STATE,		0,							CTRL_EVENT_REL_MAX,	0,				1,					(uint32_t)setloopbeatrepeat,(uint32_t)NULL,						0,								0,					0,	(uint32_t *)&loop_sensor[2],			NULL,0x00,	0x01C6},

		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_SENDTOREVERB,	"reverb level",		MAIN_SENDTOREV_MIN, 	MAIN_SENDTOREV_MAX, 	MAIN_SENDTOREV_DEFAULT,			MAIN_SENDTOREV_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,   	0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstrsendtorev,(uint32_t)displaypercent,						NULL,							0,					0,				NULL,			(uint32_t *)&musicfx_onoff_param[1],			0x00,	0x01C9},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_CHORUS,		"chorus",			0,                  	3,  					0,								0,								SCREEN_DISPLAY_CHORUS,			0,							CTRL_EVENT_REL_MID,	0,				2,					(uint32_t)setinstrchorus,	(uint32_t)NULL,						(uint32_t *)music_chorus_param,	FX_CHORUS_USER_SIZE,0,				NULL,(uint32_t *)&musicfx_onoff_param[0],			0x00,	0x01C7},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_DELAY_ONOFF,	"delay",			0,                  	1,  					0,								0,								SCREEN_DISPLAY_BOOL,			0,							CTRL_EVENT_REL_MAX,	0,				3,					(uint32_t)setinstrdelay,	(uint32_t)NULL,						(uint32_t *)music_delay_param,	FX_DELAY_USER_SIZE,	0,				NULL,			NULL,			0x00,	0x01C8},

		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_WAH_FREQ,		"filter",			FX_WAH_FILTERFREQ_MINVALUE,	FX_WAH_FILTERFREQ_MAXVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,	0,				0xA,				(uint32_t)setinstrwah_freq,	(uint32_t)display8khzfreqwah,		(uint32_t *)sound_wah_param,	(FX_WAH_USER_SIZE|0x80),0,(uint32_t *)&music_wah_sensor[0],	(uint32_t *)&musicauto_rate_param[3],			0x00,	0x01CD},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_PANNING,		"panning",			MAIN_PANNING_MIN,   	MAIN_PANNING_MAX,		MAIN_PANNING_DEFAULT,			MAIN_PANNING_DEFAULT,			SCREEN_DISPLAY_PAN,				0,							CTRL_EVENT_REL_MIN,	0,				0xA,				(uint32_t)setinstrpanning,	(uint32_t)displaypanning,			NULL,							0,					0,	(uint32_t *)&loop_sensor[4],(uint32_t *)&musicauto_rate_param[2],			0x00,	0x01CB},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_PITCHSEND,		"pitch",			MAIN_PB_SENS_MIN,   	MAIN_PB_SENS_MAX,   	MAIN_PB_SENS_DEFAULT,			MAIN_PB_SENS_DEFAULT,			SCREEN_DISPLAY_PITCH,			0,							CTRL_EVENT_REL_MID,	0,				0xA,			(uint32_t)setinstrpitchbendsens,(uint32_t)NULL, 					NULL,							0,					0,	(uint32_t *)&loop_sensor[5],(uint32_t *)&musicauto_rate_param[0],0x00,	0x01CC},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_EXPRESSION,	"expression",		MAIN_EXPRESSION_MIN,	MAIN_EXPRESSION_MAX,	MAIN_EXPRESSION_DEFAULT,		MAIN_EXPRESSION_DEFAULT,		SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,	0,				0xA,				(uint32_t)setinstrexpression,(uint32_t)displaypercent, 			NULL,							0,					0,	(uint32_t *)&loop_sensor[3],(uint32_t *)&musicauto_rate_param[1],			0x00,	0x01CA},
		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_VIBRATO,		"vibrato",			0,                  	FX_NUM_FX_INTR-1,   	FX_VIB_PRESET_DEFAULTVALUE,		FX_VIB_PRESET_DEFAULTVALUE,		FX_VIBRATO_NAME,				0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrvibrato,	(uint32_t)displaypreset,			(uint32_t *)music_vibrato_param,FX_VIBRATO_USER_SIZE,0,				NULL,(uint32_t *)&musicfx_onoff_param[6],			0x00,	0x01CC},
		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_WAH,			"wah-wah",			0,                  	FX_NUM_FX_INTR-1,   	FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_NAME,				   	0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrwah,		(uint32_t)displaypreset,			(uint32_t *)music_wah_param,	FX_WAH_USER_SIZE,	0,				NULL,(uint32_t *)&musicauto_rate_param[3],			0x00,	0x01CD},

		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_COMPRESSOR,	"compressor",		0,                  	FX_NUM_FX_INTR-1,   	FX_COMP_PRESET_DEFAULTVALUE,	FX_COMP_PRESET_DEFAULTVALUE,	SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrcompressor,(uint32_t)NULL,					(uint32_t *)music_compressor_param,FX_COMP_USER_SIZE,0,				NULL,(uint32_t *)&musicfx_onoff_param[1],			0x00,	0x01CE},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_DISTORTION_ONOFF,"distortion",		0,		            	1,						0,								0,								SCREEN_DISPLAY_BOOL,			0,							CTRL_EVENT_REL_MAX,	0,				3,					(uint32_t)setinstrdistortion,(uint32_t)NULL,					(uint32_t *)music_distortion_param,FX_DISTO_USER_SIZE,0,			NULL,			NULL,			0x00,	0x01CF},
		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_ADSR,			"adsr",				0,                  	FX_NUM_FX_INTR-1,   	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_NAME,					0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstradsr,	 	(uint32_t)displaypreset,			(uint32_t *)music_adsr_param,	FX_ADSR_USER_SIZE,	0,				NULL,(uint32_t *)&musicfx_onoff_param[0],			0x00,	0x01D0},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_EQ_ONOFF,		"equalizer",		0,                  	1,  					0,								0,								SCREEN_DISPLAY_BOOL, 			0,							CTRL_EVENT_REL_MAX,	0,				2,					(uint32_t)setinstreq,		(uint32_t)NULL,						(uint32_t *)music_eq_param,		FX_PEQ_USER_SIZE,	0,				NULL,			NULL,			0x00,	0x01D1},
		{	NONE,				0,							"",					0,       				0,						0,								0,								0,								0,							0,					0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01CE},

		{	OTHERS,				PARAM_NONE,					"un-set ctrl",		0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0192},

};

#ifdef __LPC18XX__
SECTION_INTFLASH
#else
__attribute__ ((section(".intflash")))
#endif
const param_struct	music_track_param_ps[NUM_PARAM_MUSIC_TRACK_MAX] = {
// 			p_struct,			p_param,					p_name,				p_min,					p_max,					p_default,						p_sticky,						p_screen,						p_linktoaftertouch,			p_linktoslider,		p_linktogyro,	p_incrementonclic,	p_callback,					p_display,							extras,							extra_size,			p_screen_extra,	linked_dynamic,	linked_next,	p_CC,	p_id
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_VOLUME,		"volume",			MAIN_VOLUME_MIN,   		MAIN_VOLUME_MAX,   		MAIN_VOLUME_MIN,				MAIN_VOLUME_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstrvolume,	(uint32_t)displaypercent,			NULL,							0,					0,		(uint32_t *)&loop_sensor[0],(uint32_t *)&loop_changesound_param,	0x00,	0x01C0},
		{	NONE,				0,							"",					0,       				0,						0,								0,								0,								0,							0,					0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C3},
		{	COPYANDPAST,		COPY_LOOP,					"copy & past",		0,        				1,						0,								0,								SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MAX,	0,				9,				(uint32_t)checkcopyandpastparam,(uint32_t)displaycopyandpast,		NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C2},

		{	MUSIC_LOOP,			LOOP_LEARN,					"score",			0,        				(NUM_LEARNMODE - 2),	0,								0,								SCREEN_DISPLAY_LEARN,			0,							CTRL_EVENT_REL_MAX,	0,				1,					(uint32_t)setmusiclearn,	(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C1},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_ADSR_RELEASE,	"release time",		FX_ADSR_RELEAS_MINVALUE,FX_ADSR_RELEAS_MAXVALUE,FX_ADSR_RELEAS_DEFAULTVALUE,	FX_ADSR_RELEAS_DEFAULTVALUE,	SCREEN_DISPLAY_TIME,			0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstradsr_release,	(uint32_t)displayADSRrelease,	NULL,							0,					0,				NULL,(uint32_t *)&music_adsr_param[0],				0x00,	0x0144},
		{	NONE,				0,							"",					0,       				0,						0,								0,								0,								0,							0,					0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C4},
		{	OTHERS,				PARAM_LOOPBEATREPEAT,		"beat repeat",		0,						RECORD_BEATLEVELMAX,	RECORD_BEATLEVELDEFAULT,		RECORD_BEATLEVELDEFAULT,		SCREEN_DISPLAY_BEAT_STATE,		0,							CTRL_EVENT_REL_MAX,	0,				1,					(uint32_t)setloopbeatrepeat,(uint32_t)NULL,						0,								0,					0,		(uint32_t *)&loop_sensor[2],			NULL,0x00,	0x01C6},

		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_SENDTOREVERB,	"reverb level",		MAIN_SENDTOREV_MIN, 	MAIN_SENDTOREV_MAX, 	MAIN_SENDTOREV_DEFAULT,			MAIN_SENDTOREV_DEFAULT,			SCREEN_DISPLAY_PERCENTAGE,   	0,							CTRL_EVENT_REL_MIN,	0,				2,					(uint32_t)setinstrsendtorev,(uint32_t)displaypercent,			NULL,							0,					0,				NULL,			(uint32_t *)&musicfx_onoff_param[1],			0x00,	0x01C9},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_CHORUS,		"chorus",			0,                  	3,  					0,								0,								SCREEN_DISPLAY_CHORUS,			0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrchorus,	(uint32_t)NULL,						(uint32_t *)music_chorus_param,	FX_CHORUS_USER_SIZE,0,				NULL,(uint32_t *)&musicfx_onoff_param[0],			0x00,	0x01C7},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_DELAY_ONOFF,	"delay",			0,                  	1,  					0,								0,								SCREEN_DISPLAY_BOOL,			0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdelay,	(uint32_t)NULL,						(uint32_t *)music_delay_param,	FX_DELAY_USER_SIZE,	0,				NULL,			NULL,			0x00,	0x01C8},

		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_WAH_FREQ,		"filter",		FX_WAH_FILTERFREQ_MINVALUE,	FX_WAH_FILTERFREQ_MAXVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,FX_WAH_FILTERFREQ_DEFAULTVALUE,SCREEN_DISPLAY_FREQUENCY,	0,							CTRL_EVENT_REL_MIN,		0,				0xA,				(uint32_t)setinstrwah_freq,	(uint32_t)display8khzfreqwah,		(uint32_t *)sound_wah_param,	(FX_WAH_USER_SIZE|0x80),0,(uint32_t *)&music_wah_sensor[0],(uint32_t *)&musicauto_rate_param[3],			0x00,	0x01CD},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_PANNING,		"panning",			MAIN_PANNING_MIN,   	MAIN_PANNING_MAX,		MAIN_PANNING_DEFAULT,			MAIN_PANNING_DEFAULT,			SCREEN_DISPLAY_PAN,				0,							CTRL_EVENT_REL_MIN,	0,				0xA,				(uint32_t)setinstrpanning,	(uint32_t)displaypanning,			NULL,							0,					0,		(uint32_t *)&loop_sensor[4],(uint32_t *)&musicauto_rate_param[2],			0x00,	0x01CB},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_PITCHSEND,		"pitch",			MAIN_PB_SENS_MIN,   	MAIN_PB_SENS_MAX,   	MAIN_PB_SENS_DEFAULT,			MAIN_PB_SENS_DEFAULT,			SCREEN_DISPLAY_PITCH,			0,							CTRL_EVENT_REL_MID,	0,				0xA,			(uint32_t)setinstrpitchbendsens,(uint32_t)NULL, 					NULL,							0,					0,	(uint32_t *)&loop_sensor[5],(uint32_t *)&musicauto_rate_param[0],0x00,	0x01CC},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_EXPRESSION,	"expression",		MAIN_EXPRESSION_MIN,	MAIN_EXPRESSION_MAX,	MAIN_EXPRESSION_DEFAULT,		MAIN_EXPRESSION_DEFAULT,		SCREEN_DISPLAY_PERCENTAGE,		0,							CTRL_EVENT_REL_MIN,	0,				0xA,				(uint32_t)setinstrexpression,(uint32_t)displaypercent, 			NULL,							0,					0,		(uint32_t *)&loop_sensor[3],(uint32_t *)&musicauto_rate_param[1],			0x00,	0x01CA},
		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_VIBRATO,		"vibrato",			0,                  	FX_NUM_FX_INTR-1,   	FX_VIB_PRESET_DEFAULTVALUE,		FX_VIB_PRESET_DEFAULTVALUE,		FX_VIBRATO_NAME,				0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrvibrato,	(uint32_t)displaypreset,			(uint32_t *)music_vibrato_param,FX_VIBRATO_USER_SIZE,0,				NULL,(uint32_t *)&musicfx_onoff_param[6],			0x00,	0x01CC},
		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_WAH,			"wah-wah",			0,                  	FX_NUM_FX_INTR-1,   	FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_PRESET_DEFAULTVALUE,		FX_WAH_NAME,				   	0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrwah,		(uint32_t)displaypreset,			(uint32_t *)music_wah_param,	FX_WAH_USER_SIZE,	0,				NULL,(uint32_t *)&musicauto_rate_param[3],			0x00,	0x01CD},

		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_COMPRESSOR,	"compressor",		0,                  	FX_NUM_FX_INTR-1,   	FX_COMP_PRESET_DEFAULTVALUE,	FX_COMP_PRESET_DEFAULTVALUE,	SCREEN_DISPLAY_VALUE,			0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstrcompressor,(uint32_t)NULL,					(uint32_t *)music_compressor_param,FX_COMP_USER_SIZE,0,				NULL,(uint32_t *)&musicfx_onoff_param[1],			0x00,	0x01CE},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_DISTORTION_ONOFF,"distortion",		0,		           		1,						0,								0,								SCREEN_DISPLAY_BOOL,			0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstrdistortion,(uint32_t)NULL,					(uint32_t *)music_distortion_param,FX_DISTO_USER_SIZE,0,			NULL,			NULL,			0x00,	0x01CF},
		//{	MUSIC_INSTR_PRESET,	SOUND_PRESET_ADSR,			"adsr",				0,                  	FX_NUM_FX_INTR-1,   	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_PRESET_DEFAULTVALUE,	FX_ADSR_NAME,					0,							CTRL_EVENT_REL_MID,	0,				0,					(uint32_t)setinstradsr,	 	(uint32_t)displaypreset,			(uint32_t *)music_adsr_param,	FX_ADSR_USER_SIZE,	0,				NULL,(uint32_t *)&musicfx_onoff_param[0],			0x00,	0x01D0},
		{	MUSIC_INSTR_PRESET,	SOUND_PRESET_EQ_ONOFF,		"equalizer",		0,                  	1,  					0,								0,								SCREEN_DISPLAY_BOOL, 			0,							CTRL_EVENT_REL_MAX,	0,				0,					(uint32_t)setinstreq,		(uint32_t)NULL,						(uint32_t *)music_eq_param,		FX_PEQ_USER_SIZE,	0,				NULL,			NULL,			0x00,	0x01D1},
		{	NONE,				0,							"",					0,       				0,						0,								0,								0,								0,							0,					0,				0,					(uint32_t)NULL,				(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x01C4},

		{	OTHERS,				PARAM_NONE,					"un-set ctrl",		0,       				0,						0,								0,								0,							0,							0,						0,				0,					(uint32_t)NULL,		 		(uint32_t)NULL,						NULL,							0,					0,				NULL,			NULL,			0x00,	0x0192},

};

tab_param tab_struct_param[NUM_PARAM_TAB] =
{
	{&controler_vol_param, 	1}, // 1
	{&sound_vol_param, 		1}, // 2
	//{music_vol_param, 	1}, // 3
	{&music_head_param, 	1}, // 4
	{&music_dugame_param, 	1}, // 5
	{controler_eq_sensor, 	FX_PEQ_USER_SIZE}, // 6
	{controler_eq_param,	FX_PEQ_USER_SIZE}, // 7
	{controler_reverb_sensor,FX_REVERB_USER_SIZE}, // 8
	{controler_reverb_param,FX_REVERB_USER_SIZE}, // 9
	{sound_compressor_param,FX_COMP_USER_SIZE}, // 10
	{sound_wah_sensor,		FX_WAH_USER_SIZE}, // 11
	{sound_wah_param,		FX_WAH_USER_SIZE}, // 12
	{sound_distortion_sensor,FX_DISTO_USER_SIZE}, // 13
	{sound_distortion_param,FX_DISTO_USER_SIZE}, // 14
	{sound_eq_sensor,		FX_PEQ_USER_SIZE}, // 15
	{sound_eq_param,		FX_PEQ_USER_SIZE}, // 16
	{sound_chorus_param,	FX_CHORUS_USER_SIZE}, // 17
	{sound_delay_sensor,	FX_DELAY_USER_SIZE}, // 18
	{sound_delay_param,		FX_DELAY_USER_SIZE}, // 19
	{sound_adsr_param,		FX_ADSR_USER_SIZE},
	{sound_adsr_param,		FX_ADSR_USER_SIZE}, // 22
	{music_compressor_param,FX_COMP_USER_SIZE}, // 23
	{music_wah_sensor,		FX_WAH_USER_SIZE}, // 24
	{music_wah_param,		FX_WAH_USER_SIZE}, // 25
	{music_distortion_sensor,FX_DISTO_USER_SIZE}, // 26
	{music_distortion_param,FX_DISTO_USER_SIZE}, // 27
	{music_eq_sensor,		FX_PEQ_USER_SIZE}, // 28
	{music_eq_param,		FX_PEQ_USER_SIZE}, // 29
	{music_chorus_param,	FX_CHORUS_USER_SIZE}, // 30
	{music_delay_sensor,	FX_DELAY_USER_SIZE}, // 31
	{music_delay_param,		FX_DELAY_USER_SIZE}, // 32
	{music_adsr_sensor,		FX_ADSR_USER_SIZE}, //
	{music_adsr_param,		FX_ADSR_USER_SIZE}, // 35
	{music_reverb_sensor,	FX_REVERB_USER_SIZE}, // 36
	{music_reverb_param,	FX_REVERB_USER_SIZE}, // 37
	{dutouch_info,			NUM_DUTOUCHINFO}, // 38
	{imu_param,				2}, // 39
	{controler_param,		NUM_PARAM_CONTROLER_MAX}, // 40
	{sound_sensor,			7}, // 41
	{&onoff_param,			1}, // 42
	{fx_onoff_param,		2}, // 43
	{auto_range_param,		4}, // 44
	{auto_rate_param,		4}, // 45
	{sound_param,			NUM_PARAM_SOUND_MAX}, // 46
	{sound_param_ps,		NUM_PARAM_SOUND_MAX}, // 47
	{music_sensor,			5}, // 48
	{music_param,			NUM_PARAM_MUSIC_MAX}, // 49
	{music_param_ps,		NUM_PARAM_MUSIC_MAX}, // 50
	{&loop_changesound_param,1}, // 51
	{loop_sensor,			6}, // 52
	{musicfx_onoff_param,	2}, // 53
	{musicauto_range_param,	4}, // 54
	{musicauto_rate_param,	4}, // 55
	{music_track_param,		NUM_PARAM_MUSIC_TRACK_MAX}, // 56
	{music_track_param_ps,	NUM_PARAM_MUSIC_TRACK_MAX}, // 57
	{ctrl_rate_param,		4}, // 58
	{musicctrl_rate_param,	4} // 59
};

const param_struct	*internalCCtab[NUM_INTERNALCC] =
{
		&dummy_paramstruct,
		&music_track_param[3], //sound_param[3],
		&musicctrl_rate_param[2],
		&musicctrl_rate_param[0],
		&musicctrl_rate_param[1],
		&musicctrl_rate_param[3],
};

void setparamcontext(uint32_t loop, uint32_t track)
{
	local_loop = loop;
	local_track = track;
}

uint8_t *getparamtoset(uint32_t cat, uint32_t param)
{
	uint8_t *tab;
	uint8_t *ret;
	uint32_t temp;

	ret = 0;

    switch(cat)
    {
        case CONTROLER:
        {
        	tab = (uint8_t *)&(controler_tab.c_param);
        	ret = &(tab[param]);
        }
        break;
        case GENERAL_REVERB:
        {
        	//tab = (uint8_t *)&(controler_tab.c_reverb[controler_tab.c_param.c_reverb_preset]);
			//ret = &(tab[param]);
        }
		break;
        case GENERAL_EQ:
        {
        	tab = (uint8_t *)&(controler_tab.c_equalizer[controler_tab.c_param.c_equalizer_preset]);
			ret = &(tab[param]);
        }
		break;
        case INSTR_MIX:
        {
        	temp = controler_tab.c_param.c_instr;
        	tab = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_mix);
			ret = &(tab[param]);
        }
		break;
        case INSTR_DISTORTION:
        {
        	temp = controler_tab.c_param.c_instr;
        	tab = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_distortion);
			ret = &(tab[param]);
        }
		break;
        case INSTR_COMPRESSOR:
        {
        	temp = controler_tab.c_param.c_instr;
			tab = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_compressor);
			ret = &(tab[param]);
        }
		break;
        case INSTR_EQUALIZER:
        {
        	temp = controler_tab.c_param.c_instr;
			tab = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_equalizer);
			ret = &(tab[param]);
        }
		break;
        case INSTR_CHORUS:
        {
        	temp = controler_tab.c_param.c_instr;
			tab = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus_preset-1]);
			ret = &(tab[param]);
        }
		break;
        case INSTR_DELAY:
        {
        	temp = controler_tab.c_param.c_instr;
			tab = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_delay);
			ret = &(tab[param]);
        }
		break;
        case INSTR_PRESET:
        {
        	temp = controler_tab.c_param.c_instr;
			tab = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum]);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_MIX:
        {
        	tab = (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_mix);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_DISTORTION:
        {
			tab = (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_distortion);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_COMPRESSOR:
        {
			tab = (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_compressor);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_EQUALIZER:
        {
			tab = (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_equalizer);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_CHORUS:
        {
			tab = (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus[music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_preset-1]);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_DELAY:
        {
			tab = (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_delay);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_PRESET:
        {
        	//if((param!=SOUND_PRESET_ADSR)&&(param!=SOUND_PRESET_COMPRESSOR)&&(param!=SOUND_PRESET_DELAY)&&(param!=SOUND_PRESET_DISTORTION)&&(param!=SOUND_PRESET_EQ)&&(param!=SOUND_PRESET_CHORUS)&&(param!=SOUND_PRESET_VIBRATO)&&(param!=SOUND_PRESET_WAH))
        	//{
        		tab = (uint8_t *)&(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset);
        		ret = &(tab[param]);
        	//}
        	//else
        	//{
        	//	ret = &dummyparam;
        	//}
        }
		break;
        case MUSIC_TRACK:
        {
        	temp = controler_tab.c_param.c_song;
			tab = (uint8_t *)&(music_tab[temp].s_track[music_tab[temp].s_currenttrack]);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_LOOP:
		{
			temp = controler_tab.c_param.c_song;
			tab = (uint8_t *)&(music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_loop[music_tab[temp].s_track[music_tab[temp].s_currenttrack].t_currentloop]);
			ret = &(tab[param]);
		}
		break;
        case MUSIC_MIX:
		{
			temp = controler_tab.c_param.c_song;
			tab = (uint8_t *)&(music_tab[temp].s_mix);
			ret = &(tab[param]);
		}
		break;
        case MUSIC_REVERB:
		{
			temp = controler_tab.c_param.c_song;
			tab = (uint8_t *)&(music_tab[temp].s_reverb);
			ret = &(tab[param]);
		}
		break;
        case MUSIC_SONG:
        {
			temp = controler_tab.c_param.c_song;
			tab = (uint8_t *)&(music_tab[temp]);
			ret = &(tab[param]);
        }
		break;
        case DUTOUCHINFO:
        { // no real interest on the variable
        	ret = &dummyparam;
        }
        break;
        case COPYANDPAST:
        {
        	switch(param)
			{
				case COPY_SOUNDPRESET:
				{
					ret = (uint8_t *)&copyandpastindex;
				}
				break;

				case COPY_MUSIC:
				{
					ret = (uint8_t *)&copyandpastindex;
				}
				break;

				case COPY_LOOP:
				{
					ret = (uint8_t *)&copyandpastindex;
				}
				break;
			}
        }
        break;
        case OTHERS:
        {
        	switch(param)
			{
				//case PARAM_TRANSPOSE:
				//{
				//	ret = (uint8_t *)&(v_transpose);
				//}
				//break;
				case PARAM_DUGAME:
				{
					ret = (uint8_t *)&(arrangement_buffer.as_currentevent);
				}
				break;
				case PARAM_METRO:
				{
					ret = (uint8_t *)&(v_metronome);
				}
				break;
				case PARAM_INSTRUVOL:
				{
					ret = (uint8_t *)&(instr_tab[controler_tab.c_param.c_instr].s_instrument.instr_relvolume);
				}
				break;
				case PARAM_LOOPSOUND:
				{
					ret = (uint8_t *)&loop_edit_sound;
				}
				break;
				case PARAM_LOOPBEATREPEAT:
				{
					ret = (uint8_t *)&loop_beat_repeat;
				}
				break;
				case PARAM_MUSICBEATREPEAT:
				{
					ret = (uint8_t *)&music_beat_repeat;
				}
				break;

				case PARAM_INSTRLED:
				{
					temp = controler_tab.c_param.c_instr;
					ret = (uint8_t *)&(instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_displayled);
				}
				break;

#ifdef CONFIG_TEST
				case PARAM_RF:
				{
					ret = (uint8_t *)&(rf_config);
				}
				break;
#endif
				default:
				{
					ret = &dummyparam;
				}
				break;
			}
        }
        break;
    }
    return ret;
}

uint8_t *getparamtoset_internalCC(uint32_t cat, uint32_t param)
{
	uint8_t *tab;
	uint8_t *ret;
//	uint32_t temp;

	ret = 0;

    switch(cat)
    {
        case MUSIC_INSTR_MIX:
        {
        	tab = (uint8_t *)&(tmp_loop_instr[music_strack()][music_sloop()].i_preset.s_mix);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_DISTORTION:
        {
			tab = (uint8_t *)&(tmp_loop_instr[music_strack()][music_sloop()].i_preset.s_distortion);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_COMPRESSOR:
        {
			tab = (uint8_t *)&(tmp_loop_instr[music_strack()][music_sloop()].i_preset.s_compressor);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_EQUALIZER:
        {
			tab = (uint8_t *)&(tmp_loop_instr[music_strack()][music_sloop()].i_preset.s_equalizer);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_CHORUS:
        {
			tab = (uint8_t *)&(tmp_loop_instr[music_strack()][music_sloop()].i_preset.s_chorus[tmp_loop_instr[music_strack()][music_sloop()].i_preset.s_chorus_onoff]);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_DELAY:
        {
			tab = (uint8_t *)&(tmp_loop_instr[music_strack()][music_sloop()].i_preset.s_delay);
			ret = &(tab[param]);
        }
		break;
        case MUSIC_INSTR_PRESET:
        {
        	//if((param!=SOUND_PRESET_ADSR)&&(param!=SOUND_PRESET_COMPRESSOR)&&(param!=SOUND_PRESET_DELAY)&&(param!=SOUND_PRESET_DISTORTION)&&(param!=SOUND_PRESET_EQ)&&(param!=SOUND_PRESET_CHORUS)&&(param!=SOUND_PRESET_VIBRATO)&&(param!=SOUND_PRESET_WAH))
        	//{
        		tab = (uint8_t *)&(tmp_loop_instr[music_strack()][music_sloop()].i_preset);
        		ret = &(tab[param]);
        	//}
        	//else
        	//{
        	//	ret = &dummyparam;
        	//}
        }
		break;

    }
    return ret;
}

uint32_t processparam(param_struct *param_s, int32_t rel_value, uint32_t fix_value, uint32_t rec)
{
	//uint32_t temp;
	uint32_t ref;
	int32_t compute = 0;
	//int32_t decal = 0;
    uint8_t *param = 0;
    uint32_t only_accessibility = rec & 0x80;
    rec &= 0x7f;
	//static int32_t mem;
	static param_struct *lastparam;

/*
	if(rec & PARAM_FROMAFTERTOUCH)
	{
		if(!param_s->p_linktoaftertouch)
			return 0;
	}
	else if(rec & PARAM_FROMSLIDER)
	{
		if(!param_s->p_linktoslider)
			return 0;
	}
	else if(rec & PARAM_FROMGYRO)
	{
		if(!param_s->p_linktogyro)
			return 0;
	}
*/
	rec = rec & 0xFFFF;

	//if(rec)
        param = getparamtoset(param_s->p_struct, param_s->p_param);
	//else
    //    param = getparamtoprocess(param_s->p_struct, param_s->p_param_dyn);


	if(!param)
	{
		return 0;
	}

	if(param_s!=lastparam)
	{
		//mem = 0;
		lastparam = param_s;
	}

	if(rel_value == FX_INTERFACE_FORCE_0)
	{
		if((*param) == param_s->p_default)
			return 0;
		else// if(rec)
			compute = param_s->p_default;
		//else
		//	compute = param_s->p_default_dyn;
	}
	else if(rel_value == FX_INTERFACE_FORCE_VAL)
	{
		compute = fix_value;
	}
	else if(fix_value)
	{
		if((*param) == fix_value)
			return fix_value;

		//compute = (int32_t)fix_value;
		compute = (int32_t) ( param_s->p_min + ((fix_value * param_s->p_max) / 127));

		if(compute < (int32_t)param_s->p_min)
			compute = param_s->p_min;
		else if(compute > (int32_t)param_s->p_max)
			compute = param_s->p_max;
	}
	else
	{
		//if(rel_value<0)
		//	decal = 1;

		ref = (*param);

		compute = ref + rel_value; // + decal;


		if(compute < (int32_t)param_s->p_min)
			compute = param_s->p_min;
		else if(compute > (int32_t)param_s->p_max)
			compute = param_s->p_max;
	}

	if((*param) == compute)
		return compute;

	//if(rec)
		(*param) = compute;


	// Callback function for parameter setting
    if(param_s->p_callback)
    	((FX_Callback)param_s->p_callback)(param_s, compute, rec);

    // Callback function for parameter display
    if(!(rec & 0x40))
    {
		if(param_s->p_display)
		{
			if(rec == 2)
			{
#if CONFIG_SCREEN_TASK_ENABLED == 1
				//screenTask_add_string( MENUSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
				screenTask_add_string( POPUPSCREEN_LINE2, SCREEN_PRIORITY_NONE, param_s->p_name, 0);
#endif
				((FX_Callback)param_s->p_display)(param_s, compute, rec);
			}
			else if(rec == 1)
			{
#if CONFIG_SCREEN_TASK_ENABLED == 1
				//screenTask_add_string( MENUSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
				screenTask_add_string( MENUSCREEN_LINE2, SCREEN_PRIORITY_NONE, param_s->p_name, 0);
#endif
				((FX_Callback)param_s->p_display)(param_s, compute, rec);
			}
			else if(!layer_play_record)
			{
#if CONFIG_SCREEN_TASK_ENABLED == 1
				//screenTask_add_string( POPUPSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
				screenTask_add_string( PLAYSCREEN_LINE2, SCREEN_PRIORITY_NONE, param_s->p_name, 0);
#endif
				((FX_Callback)param_s->p_display)(param_s, compute, rec);
			}
		}
		else if(rec == 2)
		{
#if CONFIG_SCREEN_TASK_ENABLED == 1
			//screenTask_add_msg( MENUSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
			screenTask_add_string( POPUPSCREEN_LINE2, SCREEN_PRIORITY_NONE, param_s->p_name, 0);
			screenTask_add_msg( POPUPSCREEN_LINE3, SCREEN_PRIORITY_POPUP, param_s->p_screen, compute);
#endif
		}
		else if(rec == 1)
		{
#if CONFIG_SCREEN_TASK_ENABLED == 1
			//screenTask_add_msg( MENUSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
			screenTask_add_string( MENUSCREEN_LINE2, SCREEN_PRIORITY_NONE, param_s->p_name, 0);
			screenTask_add_msg( MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, param_s->p_screen, compute);
#endif
		}
		else if(!layer_play_record)
		{
#if CONFIG_SCREEN_TASK_ENABLED == 1
			//screenTask_add_msg( POPUPSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
			screenTask_add_string( PLAYSCREEN_LINE2, SCREEN_PRIORITY_NONE, param_s->p_name, 0);
			screenTask_add_msg( PLAYSCREEN_LINE3, SCREEN_PRIORITY_POPUP, param_s->p_screen, compute);
#endif
		}
    }

    if(param_s->p_CC)
    { // send to record
#if CONFIG_RECORD_TASK_ENABLED == 1
    	recordTask_add_cmd( 0x80, 0, param_s->p_CC, (*param));
#endif
    }

    if((rec || only_accessibility) && controler_tab.c_param.c_accessibility)
    {
    	send_accessibility_sound();
    }

    if(rec) // update a potential linked controler
    {
    	if((((param_struct *)param_s->linked_dynamic)->p_struct == param_s->p_struct) && (((param_struct *)param_s->linked_dynamic)->p_param == param_s->p_param))
    		controller_updaterefvalue(((param_struct *)param_s->linked_dynamic)->p_id , (*param));
    }

    return compute;
}

uint32_t incrementparam(param_struct *param_s)
{
	uint32_t ref;
	int32_t compute = 0;
    uint8_t *param = 0;

    param = getparamtoset(param_s->p_struct, param_s->p_param);


	if(!param)
	{
		return 0;
	}


	ref = (*param);

	compute = ref + 1;


	if(compute > (int32_t)param_s->p_max)
		compute = param_s->p_min;

	if((*param) == compute)
		return 0;

	(*param) = compute;


	// Callback function for parameter setting
    if(param_s->p_callback)
    	((FX_Callback)param_s->p_callback)(param_s, compute, 1);

    if(controler_tab.c_param.c_accessibility)
	{
		send_accessibility_sound();
	}

    return 1;
}

uint32_t invertparam(param_struct *param_s)
{
	uint32_t ref;
	int32_t compute = 0;
    uint8_t *param = 0;

    param = getparamtoset(param_s->p_struct, param_s->p_param);


	if(!param)
	{
		return 0;
	}


	ref = (*param);

	if(ref> ((param_s->p_min + param_s->p_max)/2))
		compute = param_s->p_min;
	else
		compute = param_s->p_max;

	if((*param) == compute)
		return 0;

	(*param) = compute;


	// Callback function for parameter setting
    if(param_s->p_callback)
    	((FX_Callback)param_s->p_callback)(param_s, compute, 1);

    if(controler_tab.c_param.c_accessibility)
	{
		send_accessibility_sound();
	}

    return 1;
}

// Display function for the bottom of the Screen (SCREEN_PART_3)
void displayparam(param_struct *param_s)
{
	//uint32_t temp;
	int32_t compute;
    uint8_t *param = getparamtoset(param_s->p_struct, param_s->p_param);

	if(!param)
	{
		// if param is NULL ==> the screen is CLEARED
		//screenTask_add_msg( MENUSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
		screenTask_add_msg( MENUSCREEN_LINE2, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
		screenTask_add_msg( MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, SCREEN_CLEAR, 0);
		screenTask_add_msg( MENUSCREEN_PICTO1, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_KEY, 0);
		screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERL, 0);
		screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERR, 0);
		screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROP, 0);
		screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROR, 0);
		return;
	}

#if CONFIG_SCREEN_TASK_ENABLED == 1
	compute = (int32_t) ((*param) & 0xFFFF);

	if(compute < (int32_t)param_s->p_min)
	{
		processparam( param_s, FX_INTERFACE_FORCE_VAL, param_s->p_min, 1);
	}
	else if(compute > (int32_t)param_s->p_max)
	{
		processparam( param_s, FX_INTERFACE_FORCE_VAL, param_s->p_max, 1);
	}
	else
	{
		if(param_s->linked_dynamic)
		{
			if(layer_play.s_aftertouch_chainedfx == ((param_struct *)param_s->linked_dynamic))
				screenTask_add_msg( MENUSCREEN_PICTO1, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_KEY, ((param_struct *)param_s->linked_dynamic)->p_linktoaftertouch);
			else
				screenTask_add_msg( MENUSCREEN_PICTO1, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_KEY_LOW, ((param_struct *)param_s->linked_dynamic)->p_linktoaftertouch);

			if(layer_play.s_slider_L_chainedfx == ((param_struct *)param_s->linked_dynamic))
				screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERL, ((param_struct *)param_s->linked_dynamic)->p_linktoslider);
			else
				screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERL_LOW, ((param_struct *)param_s->linked_dynamic)->p_linktoslider);

			if(layer_play.s_slider_R_chainedfx == ((param_struct *)param_s->linked_dynamic))
				screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERR, ((param_struct *)param_s->linked_dynamic)->p_linktoslider);
			else
				screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERR_LOW, ((param_struct *)param_s->linked_dynamic)->p_linktoslider);

			if(layer_play.s_gyro_P_chainedfx == ((param_struct *)param_s->linked_dynamic))
				screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROP, ((param_struct *)param_s->linked_dynamic)->p_linktogyro);
			else
				screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROP_LOW, ((param_struct *)param_s->linked_dynamic)->p_linktogyro);

			if(layer_play.s_gyro_R_chainedfx == ((param_struct *)param_s->linked_dynamic))
				screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROR, ((param_struct *)param_s->linked_dynamic)->p_linktogyro);
			else
				screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROR_LOW, ((param_struct *)param_s->linked_dynamic)->p_linktogyro);
		}
		else
		{
			screenTask_add_msg( MENUSCREEN_PICTO1, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_KEY, 0);
			screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERL, 0);
			screenTask_add_msg( MENUSCREEN_PICTO2, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_SLIDERR, 0);
			screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROP, 0);
			screenTask_add_msg( MENUSCREEN_PICTO3, SCREEN_PRIORITY_NONE, SCREEN_DISPLAY_SENSOR_GYROR, 0);
		}

		//screenTask_add_string( MENUSCREEN_LINE1, SCREEN_PRIORITY_NONE, SCREEN_CLEAR, 0);
		screenTask_add_string( MENUSCREEN_LINE2, SCREEN_PRIORITY_NONE, param_s->p_name, 0);
		if (param_s->p_display) {
			((FX_Callback)param_s->p_display)(param_s, compute, 3);
		}
		else {
			 screenTask_add_msg( MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, param_s->p_screen, compute);
		}
	}

#endif
}


// check if we display and allow to use extra param
uint32_t useextra(param_struct *param_s)
{
    uint8_t *param = getparamtoset(param_s->p_struct, param_s->p_param);

	if(!param)
	{
		return 0;
	}
	else if(!param_s->extra_size)
	{
		return 0;
	}
	else if(param_s->p_max == 0)
	{ // if there are no presets
		return param_s->extra_size;
	}
	else if(param_s->extra_size & 0x80)
	{// force display extra
		return (param_s->extra_size & 0x7F);
	}
	else if(*param)
	{// if this is not the first preset
		return (param_s->extra_size & 0x7F);
	}

	return 0;
}

uint32_t getparamvalue(param_struct *param_s)
{
	//uint32_t temp;
	int32_t compute;
    uint8_t *param = getparamtoset(param_s->p_struct, param_s->p_param);

	if(!param)
	{
		return 0;
	}

	compute = (int32_t) ((*param) & 0xFFFF);

	return compute;

}

uint32_t check_chainedfx(const param_struct ** param, uint32_t id)
{
	uint32_t i, j;
	uint32_t ret_id = 0;

	*param = 0; // just in case we don't find the parameter

	if(!id)
		return 0;

	for(i=0; i<NUM_PARAM_TAB; i++)
	{
		for(j=0; j<tab_struct_param[i].size; j++)
		{
			if(tab_struct_param[i].param[j].p_id == id)
			{
				//trace(info,"add fx %d %d %d %x\r\n", id, i, j, (uint32_t)&(tab_struct_param[i].param[j]));
				*param = &(tab_struct_param[i].param[j]);
				ret_id = id;
			}
		}
	}

	return ret_id;

}

void manageinternalCC(uint32_t CC, uint32_t value, uint32_t track, uint32_t loop)
{
	uint8_t *param = 0;

	if(music_tab[controler_tab.c_param.c_song].s_track[track].t_currentloop != loop)
		return;
	else
	{
		if(music_tab[controler_tab.c_param.c_song].s_track[track].t_loop[loop].l_state == REC_EMPTY)
			return;
		else
			music_tab[controler_tab.c_param.c_song].s_currenttrack = track;
	}

	if(CC<NUM_INTERNALCC)
	{
		param = getparamtoset_internalCC(internalCCtab[CC]->p_struct, internalCCtab[CC]->p_param);


		if(!param)
		{
			return;
		}

		if((*param) == value)
			return;

		(*param) = value;


		// Callback function for parameter setting
		if(internalCCtab[CC]->p_callback)
			((FX_Callback)internalCCtab[CC]->p_callback)(internalCCtab[CC], value, 0);
	}
}
