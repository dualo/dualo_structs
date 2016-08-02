/*
 * sound_parameters_mng.h
 *
 *  Created on: 07/08/2013
 *      Author: mma
 */


#ifndef SOUND_PARAMETERS_MNG_H_
#define SOUND_PARAMETERS_MNG_H_

#include "parameters_mng.h"
#include "g_parameters_mng.h"

#include "instr_mapping.h"

/******** general defines ************/
#define FX_NUM_PRESET_INTR	4
#define FX_NUM_FX_INTR	8

#define FX_FACTORYPRESET	0 //0 //8
#define FX_USERPRESET		8 //16 //8
#define FX_NUMPRESET		(FX_FACTORYPRESET+FX_USERPRESET)
#define FX_DISTO_PRESET		6

#define FX_NUM_NAME			12


/***** fx parameters specific structures*****/
typedef struct
{
	uint8_t	m_inputgain;
	uint8_t	m_locutfilterfrequency;
	uint8_t	m_hicutfilterfrequency;
	uint8_t	m_ouputlevel;
	uint8_t	m_outputpanning;
	uint8_t	m_ouputfrontrear;
	uint8_t	m_sendtoreverb;
	uint8_t	m_sendtochorus;
}FX_mix;

#define FX_MIX_SIZE 8

typedef struct
{
	uint8_t	d_on_off;
	uint8_t	d_pre_gain;
	uint8_t	d_type;
	uint8_t	d_lowpassfilterfreq;
	uint8_t	d_lowpassfilterres;
	uint8_t	d_postgain;
	uint8_t	d_drive;
    uint8_t d_dummy;
    uint8_t d_name[NAME_CARACT];
}FX_distortion;

#define FX_DIST_SIZE ( 8 + NAME_CARACT )

//typedef struct
//{
//	uint8_t	w_filtertype;
//	uint8_t	w_filterfreq;
//	uint8_t	w_filterres;
//	uint8_t	w_autowahsensitivity;
//	uint8_t w_name[NAME_CARACT];
//}FX_wah;

//#define FX_WAH_SIZE ( 4 + NAME_CARACT )

typedef struct
{
	uint8_t	c_on_off;
	uint8_t	c_attacktime;
	uint8_t	c_releasetime;
	uint8_t	c_threshold;
	uint8_t	c_ratio;
	uint8_t	c_boost;
	uint8_t	c_kneetype;
    uint8_t c_dummy;
    uint8_t c_name[NAME_CARACT];
}FX_compressor;

#define FX_COMP_SIZE ( 8 + NAME_CARACT )

typedef struct
{
	uint8_t	e_on_off;
	uint8_t	e_lowbandgain;
	uint8_t	e_lowmidbandgain;
	uint8_t	e_highmidbandgain;
	uint8_t	e_highbandgain;
	uint8_t	e_lowbandfrequency;
	uint8_t	e_lowmidbandfrequency;
	uint8_t	e_highmidbandfrequency;
	uint8_t	e_highbandfrequency;
	uint8_t	e_lowmidbandQ;
	uint8_t	e_highmidbandQ;
    uint8_t e_dummy;
    uint8_t e_name[NAME_CARACT];
}FX_equalizer;

#define FX_EQ_SIZE ( 12 + NAME_CARACT )

typedef struct
{
	uint8_t	c_mode;
	uint8_t	c_effectlevel;
	uint8_t	c_delaytime;
	uint8_t	c_feedback;
	uint8_t	c_inputhighpassfilter;
	uint8_t	c_hdamp;
	uint8_t	c_modulationdepth;
	uint8_t	c_modulationrate;
	uint8_t	c_tremoloshape;
	uint8_t	c_rotaryspeed;
    uint8_t c_name[NAME_CARACT];
}FX_chorus;

#define FX_CHORUS_SIZE ( 10 + NAME_CARACT )

typedef struct
{
	uint8_t	d_on_off;
	uint8_t	d_mode;
	uint8_t	d_prelp;
	uint8_t	d_level;
	uint8_t	d_time;
	uint8_t	d_feedback;
	uint8_t	d_hdamp;
    uint8_t d_dummy;
    uint8_t d_name[NAME_CARACT];
}FX_delay;

#define FX_DELAY_SIZE ( 8 + NAME_CARACT )

typedef struct
{
	uint8_t r_level;
	uint8_t	r_directlevel;
	uint8_t	r_revsend;
	uint8_t	r_tonegain;
	uint8_t	r_tonefreq;
	uint8_t	r_prehp;
	uint8_t	r_time;
	uint8_t	r_echofeedback;
	uint8_t	r_hdamp;
	uint8_t	r_thresgate;
	uint8_t	r_predelaytime;
    uint8_t r_dummy;
    uint8_t r_name[NAME_CARACT];
    //uint8_t r_drywet;
}FX_reverb;

#define FX_REVERB_SIZE ( 12 + NAME_CARACT) // +1

//typedef struct
//{
//	uint8_t v_depth;
//	uint8_t v_delay;
//	uint8_t v_rate;
//	uint8_t v_dummy;
//	uint8_t v_name[NAME_CARACT];
//}FX_vibrato;

//#define FX_VIB_SIZE ( 4 + NAME_CARACT )

//typedef struct
//{
//	uint8_t a_env_attack_time;
//	uint8_t a_env_decay_time;
//	uint8_t a_tvf_cutoff_res;
//	uint8_t a_tvf_cutoff_freq;
//	uint8_t a_env_release_time;
//	uint8_t a_dummy;
//	uint8_t a_name[NAME_CARACT];
//}FX_adsr;

//#define FX_ADSR_SIZE ( 6 + NAME_CARACT )

/***** fx parameters general structure*****/
typedef struct
{
	uint8_t s_volume;
	uint8_t s_panning;
	uint8_t s_sendtorev;
	uint8_t	s_instr_octave;

	uint8_t s_portamento_on_off;
	uint8_t s_portamento_ctrl;
	uint8_t s_portamento_time;
	uint8_t s_expression;
	uint8_t s_pitch_bend_sensitivity;
	uint8_t s_key_curve;

	// FX structs
	uint8_t dummy0; // ex preset adsr
	uint8_t	s_compressor_preset;
	uint8_t s_delay_preset;
	uint8_t s_distortion_preset;
    uint8_t s_eq_preset;
	uint8_t	s_chorus_preset;
	uint8_t dummy1; // ex preset vibrato
	uint8_t dummy2; // ex preset wah
    

	// Controlers
//	uint8_t	s_aftertouch_dyn_fx;
//	uint8_t s_gyro_P_dyn_fx;
//	uint8_t s_gyro_R_dyn_fx;
//	uint8_t s_gyro_Y_dyn_fx;
//	uint8_t	s_slider_L_dyn_fx;
//	uint8_t	s_slider_R_dyn_fx;

	// multi-note
	uint8_t s_arpegiator_type;
	uint8_t s_arpegiator_beat;
	uint8_t align0;

	int8_t s_direction_gyro_P;
	int8_t s_direction_gyro_R;
	int8_t s_direction_gyro_Y;

	uint16_t s_activ_aftertouch;
	uint16_t s_activ_slider_L;
	uint16_t s_activ_slider_R;
	uint16_t s_activ_gyro_P;
	uint16_t s_activ_gyro_R;
	uint16_t s_activ_gyro_Y;

	uint8_t dummy3; //ex adrs on off
	uint8_t	s_compressor_onoff;
	uint8_t s_delay_onoff;
	uint8_t s_distortion_onoff;
	uint8_t s_eq_onoff;
	uint8_t	s_chorus_onoff;
	uint8_t s_reverb_onoff; //ex adrs on off
	uint8_t dummy5; //ex wah on off
	uint8_t align1[11];

	uint8_t s_multinote_act;
	int8_t s_multinote[4];

    uint8_t s_pitch;

	uint8_t s_autopitch_rate;
	uint8_t s_autopitch_range;

	uint8_t s_tremolo_rate;
	uint8_t s_tremolo_range;

	uint8_t s_autopan_rate;
	uint8_t s_autopan_range;

	uint8_t s_autowah_rate;
	uint8_t s_autowah_range;

	uint8_t s_adsr_attack;
	uint8_t s_adsr_release;
	uint8_t	s_wah_type;
	uint8_t	s_wah_freq;
	uint8_t	s_wah_res;
	uint8_t align2[54];

}preset_instr;

enum
{
	SOUND_PRESET_VOLUME,
	SOUND_PRESET_PANNING,
	SOUND_PRESET_SENDTOREVERB,
	SOUND_PRESET_OCTAVE,
	SOUND_PRESET_PORTON,
	SOUND_PRESET_PORTCTRL,
	SOUND_PRESET_PORTTIME,
	SOUND_PRESET_EXPRESSION,
	SOUND_PRESET_PITCHSEND,
	SOUND_PRESET_KEY_CURVE,
	SOUND_PRESET_ADSR,
	SOUND_PRESET_COMPRESSOR,
	SOUND_PRESET_DELAY,
	SOUND_PRESET_DISTORTION,
	SOUND_PRESET_EQ,
	SOUND_PRESET_CHORUS,
	SOUND_PRESET_VIBRATO,
	SOUND_PRESET_WAH,
	SOUND_PRESET_ARPEGGIATO_TYPE,
	SOUND_PRESET_ARPEGGIATO_BEAT,
	SOUND_PRESET_DUMMY1,
	SOUND_PRESET_DUMMY2,
	SOUND_PRESET_DUMMY3,
	SOUND_PRESET_DUMMY4,
	SOUND_PRESET_AFTERTOUCH,
	SOUND_PRESET_DUMMY6,
	SOUND_PRESET_SLIDERL,
	SOUND_PRESET_DUMMY7,
	SOUND_PRESET_SLIDERR,
	SOUND_PRESET_DUMMY8,
	SOUND_PRESET_GYROP,
	SOUND_PRESET_DUMMY9,
	SOUND_PRESET_GYROR,
	SOUND_PRESET_DUMMY10,
	SOUND_PRESET_GYROY,
	SOUND_PRESET_DUMMY11,
	SOUND_PRESET_ADSR_ONOFF,
	SOUND_PRESET_COMPRESSOR_ONOFF,
	SOUND_PRESET_DELAY_ONOFF,
	SOUND_PRESET_DISTORTION_ONOFF,
	SOUND_PRESET_EQ_ONOFF,
	SOUND_PRESET_CHORUS_ONOFF,
	SOUND_PRESET_VIBRATO_ONOFF,
	SOUND_PRESET_WHAH_ONOFF,
	SOUND_PRESET_DUMMY001,
	SOUND_PRESET_DUMMY002,
	SOUND_PRESET_DUMMY003,
	SOUND_PRESET_DUMMY004,
	SOUND_PRESET_DUMMY005,
	SOUND_PRESET_DUMMY006,
	SOUND_PRESET_DUMMY007,
	SOUND_PRESET_DUMMY008,
	SOUND_PRESET_DUMMY009,
	SOUND_PRESET_DUMMY010,
	SOUND_PRESET_DUMMY011,
	SOUND_PRESET_CHORDS,
	SOUND_PRESET_DUMMY013,
	SOUND_PRESET_DUMMY014,
	SOUND_PRESET_DUMMY015,
	SOUND_PRESET_DUMMY016,
	SOUND_PRESET_PITCHVALUE,
	SOUND_PRESET_AUTOPITCH_RATE,
	SOUND_PRESET_AUTOPITCH_RANGE,
	SOUND_PRESET_TREMOLO_RATE,
	SOUND_PRESET_TREMOLO_RANGE,
	SOUND_PRESET_AUTOPAN_RATE,
	SOUND_PRESET_AUTOPAN_RANGE,
	SOUND_PRESET_AUTOWAH_RATE,
	SOUND_PRESET_AUTOWAH_RANGE,
	SOUND_PRESET_ADSR_ATTACK,
	SOUND_PRESET_ADSR_RELEASE,
	SOUND_PRESET_WAH_TYPE,
	SOUND_PRESET_WAH_FREQ,
	SOUND_PRESET_WAH_RES,
};

#define PRESET_STRUCT_SIZE	(64 + INSTR_INFO_SIZE)

/***** fx parameters general structure*****/
typedef struct
{
	s_instr s_instrument;

	uint8_t s_presetnum;
	uint8_t s_displayled;
	// alignement
	uint8_t align[2];

	preset_instr	s_preset[FX_NUM_PRESET_INTR];
    
    // FX
    FX_mix			s_mix;
	FX_distortion	s_distortion[FX_NUM_FX_INTR];
	uint8_t			dummywah[128];
	FX_compressor	s_compressor[FX_NUM_FX_INTR];
	FX_equalizer	s_equalizer[FX_NUM_FX_INTR];
	FX_delay		s_delay[FX_NUM_FX_INTR];
	FX_chorus		s_chorus[FX_NUM_FX_INTR];
	uint8_t			dummyvibrato[128];
	uint8_t			dummyadsr[144];

	uint8_t s_leds[NUM_LED_VALUE];

}struct_instr;

#define INSTRU_STRUCT_SIZE	(INSTR_INFO_SIZE + 4 + NUM_LED_VALUE +(FX_NUM_PRESET_INTR*PRESET_STRUCT_SIZE) + FX_MIX_SIZE + (FX_NUM_FX_INTR*(FX_DIST_SIZE + 16 + FX_COMP_SIZE + FX_EQ_SIZE + FX_CHORUS_SIZE + FX_DELAY_SIZE + 16 +18)))


/***** general parameters defines ************/
#define MAIN_VOLUME_MIN			0
#define MAIN_VOLUME_MAX			127
#define MAIN_VOLUME_DEFAULT		100

#define MAIN_PANNING_MIN		0
#define MAIN_PANNING_MAX		127
#define MAIN_PANNING_DEFAULT	64

#define MAIN_EXPRESSION_MIN		0
#define MAIN_EXPRESSION_MAX		127
#define MAIN_EXPRESSION_DEFAULT	102 //127 80%

#define MAIN_OCTAVE_MIN		0
#define MAIN_OCTAVE_MAX		4
#define MAIN_OCTAVE_DEFAULT	2

#define MAIN_SENDTOREV_MIN		0
#define MAIN_SENDTOREV_MAX		127
#define MAIN_SENDTOREV_DEFAULT	64

#define MAIN_CANAL_MIN			0
#define MAIN_CANAL_MAX			15
#define MAIN_CANAL_DEFAULT		0

#define MAIN_PORT_ONOFF_MIN		0
#define MAIN_PORT_ONOFF_MAX		1
#define MAIN_PORT_ONOFF_DEFAULT	0

#define MAIN_PORT_CTRL_MIN		0
#define MAIN_PORT_CTRL_MAX		127
#define MAIN_PORT_CTRL_DEFAULT	0

#define MAIN_PORT_TIME_MIN		0
#define MAIN_PORT_TIME_MAX		(127 + 1) // poly mono management
#define MAIN_PORT_TIME_DEFAULT	0 //64

#define MAIN_MONO_POLY_MIN		0
#define MAIN_MONO_POLY_MAX		1
#define MAIN_MONO_POLY_DEFAULT	1

#define MAIN_PB_SENS_MIN		0
#define MAIN_PB_SENS_MAX		127 //127
#define MAIN_PB_SENS_DEFAULT	2 //2

/***** fx parameters defines ****/



// mix
#define FX_MIX_NUM_PARAMETERS			8
#define FX_MIX_NOP_PARAMETERS			0

#define	FX_MIX_LCFREQ_MINVALUE			0
#define	FX_MIX_LCFREQ_MAXVALUE			127

#define	FX_MIX_HCFREQ_MINVALUE			0
#define	FX_MIX_HCFREQ_MAXVALUE			127

#define	FX_MIX_INGAIN_MINVALUE			0
#define	FX_MIX_INGAIN_MAXVALUE			127

#define	FX_MIX_OUTLEVEL_MINVALUE		0
#define	FX_MIX_OUTLEVEL_MAXVALUE		127

#define	FX_MIX_OUTPAN_MINVALUE		0
#define	FX_MIX_OUTPAN_MAXVALUE		127
#define FX_MIX_OUTPAN_DEFAULTVALUE	64

#define	FX_MIX_OUTFR_MINVALUE			0
#define	FX_MIX_OUTFR_MAXVALUE			127

#define	FX_MIX_REVERB_MINVALUE			0
#define	FX_MIX_REVERB_MAXVALUE			127

#define	FX_MIX_CHORUS_MINVALUE			0
#define	FX_MIX_CHORUS_MAXVALUE			127

//distortion
#define	FX_DISTO_PRESET_DEFAULTVALUE	0

#define FX_DISTO_NUM_PARAMETERS			7
#define FX_DISTO_NOP_PARAMETERS			1

#define	FX_DISTO_ONOFF_MINVALUE			0
#define	FX_DISTO_ONOFF_MAXVALUE			1
#define	FX_DISTO_ONOFF_DEFAULTVALUE		0

#define	FX_DISTO_PREGAIN_MINVALUE		27 //1
#define	FX_DISTO_PREGAIN_MAXVALUE		127
#define	FX_DISTO_PREGAIN_DEFAULTVALUE	64

#define	FX_DISTO_TYPE_MINVALUE			0
#define	FX_DISTO_TYPE_MAXVALUE			2
#define	FX_DISTO_TYPE_DEFAULTVALUE		1

#define	FX_DISTO_LPFREQ_MINVALUE		0
#define	FX_DISTO_LPFREQ_MAXVALUE		127
#define	FX_DISTO_LPFREQ_DEFAULTVALUE	64

#define	FX_DISTO_LPRES_MINVALUE			0
#define	FX_DISTO_LPRES_MAXVALUE			100
#define	FX_DISTO_LPRES_DEFAULTVALUE		80

#define	FX_DISTO_POSTGAIN_MINVALUE		27 //1
#define	FX_DISTO_POSTGAIN_MAXVALUE		127
#define	FX_DISTO_POSTGAIN_DEFAULTVALUE	64

#define	FX_DISTO_DRIVE_MINVALUE			0
#define	FX_DISTO_DRIVE_MAXVALUE			7
#define	FX_DISTO_DRIVE_DEFAULTVALUE		0

#define	FX_DISTO_RCTYPE_MINVALUE		0
#define	FX_DISTO_RCTYPE_MAXVALUE		127
#define	FX_DISTO_RCTYPE_DEFAULTVALUE	0

//wah-wah
#define	FX_WAH_PRESET_DEFAULTVALUE		0

#define FX_WAH_NUM_PARAMETERS			4
#define FX_WAH_NOP_PARAMETERS			0

#define	FX_WAH_FILTERTYPE_MINVALUE		0
#define	FX_WAH_FILTERTYPE_MAXVALUE		1
#define	FX_WAH_FILTERTYPE_DEFAULTVALUE	0

#define	FX_WAH_FILTERFREQ_MINVALUE		0
#define	FX_WAH_FILTERFREQ_MAXVALUE		127
#define	FX_WAH_FILTERFREQ_DEFAULTVALUE	127

#define	FX_WAH_FILTERRES_MINVALUE		0
#define	FX_WAH_FILTERRES_MAXVALUE		100
#define	FX_WAH_FILTERRES_DEFAULTVALUE	0

#define	FX_WAH_SENSITIVITY_MINVALUE		0
#define	FX_WAH_SENSITIVITY_MAXVALUE		127
#define	FX_WAH_SENSITIVITY_DEFAULTVALUE	0

// compressor
#define	FX_COMP_PRESET_DEFAULTVALUE		0

#define FX_COMP_NUM_PARAMETERS			7
#define FX_COMP_NOP_PARAMETERS			1

#define	FX_COMP_ONOFF_MINVALUE			0
#define	FX_COMP_ONOFF_MAXVALUE			1
#define	FX_COMP_ONOFF_DEFAULTVALUE		0

#define	FX_COMP_ATTACK_MINVALUE			0
#define	FX_COMP_ATTACK_MAXVALUE			127
#define	FX_COMP_ATTACK_DEFAULTVALUE		0

#define	FX_COMP_RELEASE_MINVALUE		0
#define	FX_COMP_RELEASE_MAXVALUE		127
#define	FX_COMP_RELEASE_DEFAULTVALUE	0

#define	FX_COMP_THRESHOLD_MINVALUE		0
#define	FX_COMP_THRESHOLD_MAXVALUE		127
#define	FX_COMP_THRESHOLD_DEFAULTVALUE	0

#define	FX_COMP_RATIO_MINVALUE			0
#define	FX_COMP_RATIO_MAXVALUE			127
#define	FX_COMP_RATIO_DEFAULTVALUE		0

#define	FX_COMP_BOOST_MINVALUE			0
#define	FX_COMP_BOOST_MAXVALUE			127
#define	FX_COMP_BOOST_DEFAULTVALUE		0

#define	FX_COMP_KNEE_MINVALUE			0
#define	FX_COMP_KNEE_MAXVALUE			1
#define	FX_COMP_KNEE_DEFAULTVALUE		0

// equalizer
#define	FX_PEQ_PRESET_DEFAULTVALUE		0

#define FX_PEQ_NUM_PARAMETERS			11
#define FX_PEQ_NOP_PARAMETERS			1

#define	FX_PEQ_ONOFF_MINVALUE			0
#define	FX_PEQ_ONOFF_MAXVALUE			1
#define	FX_PEQ_ONOFF_DEFAULTVALUE		0

#define	FX_PEQ_LBGAIN_MINVALUE			0
#define	FX_PEQ_LBGAIN_MAXVALUE			127
#define	FX_PEQ_LBGAIN_DEFAULTVALUE		64

#define	FX_PEQ_LMBGAIN_MINVALUE			0
#define	FX_PEQ_LMBGAIN_MAXVALUE			127
#define	FX_PEQ_LMBGAIN_DEFAULTVALUE		64

#define	FX_PEQ_HMBGAIN_MINVALUE			0
#define	FX_PEQ_HMBGAIN_MAXVALUE			127
#define	FX_PEQ_HMBGAIN_DEFAULTVALUE		64

#define	FX_PEQ_HBGAIN_MINVALUE			0
#define	FX_PEQ_HBGAIN_MAXVALUE			127
#define	FX_PEQ_HBGAIN_DEFAULTVALUE		64

#define	FX_PEQ_LBFREQ_MINVALUE			0
#define	FX_PEQ_LBFREQ_MAXVALUE			127
#define	FX_PEQ_LBFREQ_DEFAULTVALUE		0

#define	FX_PEQ_LMBFREQ_MINVALUE			0
#define	FX_PEQ_LMBFREQ_MAXVALUE			127
#define	FX_PEQ_LMBFREQ_DEFAULTVALUE		0

#define	FX_PEQ_HMBFREQ_MINVALUE			0
#define	FX_PEQ_HMBFREQ_MAXVALUE			127
#define	FX_PEQ_HMBFREQ_DEFAULTVALUE		0

#define	FX_PEQ_HBFREQ_MINVALUE			0
#define	FX_PEQ_HBFREQ_MAXVALUE			127
#define	FX_PEQ_HBFREQ_DEFAULTVALUE		0

#define	FX_PEQ_LMBQ_MINVALUE			0
#define	FX_PEQ_LMBQ_MAXVALUE			127
#define	FX_PEQ_LMBQ_DEFAULTVALUE		0

#define	FX_PEQ_HMBQ_MINVALUE			0
#define	FX_PEQ_HMBQ_MAXVALUE			127
#define	FX_PEQ_HMBQ_DEFAULTVALUE		0

// others: chorus/flanger/phaser/tremolo/rotary
#define	FX_CHORUS_PRESET_DEFAULTVALUE		0

#define FX_CHORUS_NUM_PARAMETERS			10
#define FX_CHORUS_NOP_PARAMETERS			0

#define	FX_CHORUS_MODE_MINVALUE			0
#define	FX_CHORUS_MODE_MAXVALUE			4
#define	FX_CHORUS_MODE_DEFAULTVALUE		0

#define	FX_CHORUS_LEVEL_MINVALUE			0
#define	FX_CHORUS_LEVEL_MAXVALUE			127
#define	FX_CHORUS_LEVEL_DEFAULTVALUE		64

#define	FX_CHORUS_DELAY_MINVALUE			0
#define	FX_CHORUS_DELAY_MAXVALUE			127
#define	FX_CHORUS_DELAY_DEFAULTVALUE		0

#define	FX_CHORUS_FEEDBACK_MINVALUE			0
#define	FX_CHORUS_FEEDBACK_MAXVALUE			114
#define	FX_CHORUS_FEEDBACK_DEFAULTVALUE		64

#define	FX_CHORUS_INPUTHP_MINVALUE			0
#define	FX_CHORUS_INPUTHP_MAXVALUE			127
#define	FX_CHORUS_INPUTHP_DEFAULTVALUE		0

#define	FX_CHORUS_HDAMP_MINVALUE			0
#define	FX_CHORUS_HDAMP_MAXVALUE			127
#define	FX_CHORUS_HDAMP_DEFAULTVALUE		0

#define	FX_CHORUS_MODRATE_MINVALUE			0
#define	FX_CHORUS_MODRATE_MAXVALUE			127
#define	FX_CHORUS_MODRATE_DEFAULTVALUE		0

#define	FX_CHORUS_MODDEPTH_MINVALUE			0
#define	FX_CHORUS_MODDEPTH_MAXVALUE			127
#define	FX_CHORUS_MODDEPTH_DEFAULTVALUE		64

#define	FX_CHORUS_TREMOLO_MINVALUE			0
#define	FX_CHORUS_TREMOLO_MAXVALUE			127
#define	FX_CHORUS_TREMOLO_DEFAULTVALUE		0

#define	FX_CHORUS_ROTARY_MINVALUE			0
#define	FX_CHORUS_ROTARY_MAXVALUE			1
#define	FX_CHORUS_ROTARY_DEFAULTVALUE		0

// delay
#define	FX_DELAY_PRESET_DEFAULTVALUE		0

#define FX_DELAY_NUM_PARAMETERS			7
#define FX_DELAY_NOP_PARAMETERS			1

#define	FX_DELAY_ONOFF_MINVALUE			0
#define	FX_DELAY_ONOFF_MAXVALUE			1
#define	FX_DELAY_ONOFF_DEFAULTVALUE		0

#define	FX_DELAY_MODE_MINVALUE			0
#define	FX_DELAY_MODE_MAXVALUE			1
#define	FX_DELAY_MODE_DEFAULTVALUE		0

#define	FX_DELAY_PRELP_MINVALUE			0
#define	FX_DELAY_PRELP_MAXVALUE			127
#define	FX_DELAY_PRELP_DEFAULTVALUE		0

#define	FX_DELAY_LEVEL_MINVALUE			0
#define	FX_DELAY_LEVEL_MAXVALUE			127
#define	FX_DELAY_LEVEL_DEFAULTVALUE		0

#define	FX_DELAY_TIME_MINVALUE			0
#define	FX_DELAY_TIME_MAXVALUE			127
#define	FX_DELAY_TIME_DEFAULTVALUE		0

#define	FX_DELAY_BPM_MINVALUE			0
#define	FX_DELAY_BPM_MAXVALUE			255
#define	FX_DELAY_BPM_DEFAULTVALUE		81

#define	FX_DELAY_FEEDBACK_MINVALUE		0
#define	FX_DELAY_FEEDBACK_MAXVALUE		123
#define	FX_DELAY_FEEDBACK_DEFAULTVALUE	0

#define	FX_DELAY_HDAMP_MINVALUE			0
#define	FX_DELAY_HDAMP_MAXVALUE			127
#define	FX_DELAY_HDAMP_DEFAULTVALUE		0


// reverb
#define	FX_REVERB_PRESET_DEFAULTVALUE		0

#define FX_REVERB_NUM_PARAMETERS			11
#define FX_REVERB_NOP_PARAMETERS			1

#define	FX_REVERB_LEVEL_MINVALUE			0
#define	FX_REVERB_LEVEL_MAXVALUE			255
#define	FX_REVERB_LEVEL_DEFAULTVALUE		0

#define	FX_REVERB_DIRECTLEVEL_MINVALUE			0
#define	FX_REVERB_DIRECTLEVEL_MAXVALUE			127
#define	FX_REVERB_DIRECTLEVEL_DEFAULTVALUE		127

#define	FX_REVERB_REVSEND_MINVALUE			0
#define	FX_REVERB_REVSEND_MAXVALUE			230
#define	FX_REVERB_REVSEND_DEFAULTVALUE		0

#define	FX_REVERB_DRYWET_MINVALUE			0
#define	FX_REVERB_DRYWET_MAXVALUE			255
#define	FX_REVERB_DRYWET_DEFAULTVALUE		0

#define	FX_REVERB_TONEGAIN_MINVALUE				0
#define	FX_REVERB_TONEGAIN_MAXVALUE				127
#define	FX_REVERB_TONEGAIN_DEFAULTVALUE			0

#define	FX_REVERB_TONEFREQ_MINVALUE			0
#define	FX_REVERB_TONEFREQ_MAXVALUE			127
#define	FX_REVERB_TONEFREQ_DEFAULTVALUE		0

#define	FX_REVERB_PREHP_MINVALUE			0
#define	FX_REVERB_PREHP_MAXVALUE			127
#define	FX_REVERB_PREHP_DEFAULTVALUE		0

#define	FX_REVERB_TIME_MINVALUE			0
#define	FX_REVERB_TIME_MAXVALUE			127
#define	FX_REVERB_TIME_DEFAULTVALUE		0

#define	FX_REVERB_ECHOFEED_MINVALUE			0
#define	FX_REVERB_ECHOFEED_MAXVALUE			127
#define	FX_REVERB_ECHOFEED_DEFAULTVALUE		0

#define	FX_REVERB_HDAMP_MINVALUE			0
#define	FX_REVERB_HDAMP_MAXVALUE			127
#define	FX_REVERB_HDAMP_DEFAULTVALUE		0

#define	FX_REVERB_THRESGATE_MINVALUE			0
#define	FX_REVERB_THRESGATE_MAXVALUE			127
#define	FX_REVERB_THRESGATE_DEFAULTVALUE		0

#define	FX_REVERB_PREDELAYTIME_MINVALUE			0
#define	FX_REVERB_PREDELAYTIME_MAXVALUE			127
#define	FX_REVERB_PREDELAYTIME_DEFAULTVALUE		0

// vibrato
#define	FX_VIB_PRESET_DEFAULTVALUE		0

#define FX_VIB_NUM_PARAMETERS			3
#define FX_VIB_NOP_PARAMETERS			1

#define FX_VIB_DEPTH_MINVALUE		63
#define FX_VIB_DEPTH_MAXVALUE		127
#define FX_VIB_DEPTH_DEFAULTVALUE	64

#define FX_VIB_DELAY_MINVALUE		0
#define FX_VIB_DELAY_MAXVALUE		127
#define FX_VIB_DELAY_DEFAULTVALUE	0

#define FX_VIB_RATE_MINVALUE		50
#define FX_VIB_RATE_MAXVALUE		127
#define FX_VIB_RATE_DEFAULTVALUE	50

//#define FX_VEL_SLOPE_MIN		0
//#define FX_VEL_SLOPE_MAX		127

//#define FX_VEL_OFFSET_MIN		0
//#define FX_VEL_OFFSET_MAX		127


// adsr
#define	FX_ADSR_PRESET_DEFAULTVALUE	0

#define FX_ADSR_NUM_PARAMETERS			5
#define FX_ADSR_NOP_PARAMETERS			1

#define FX_ADSR_ATTACK_MINVALUE		64 //68
#define FX_ADSR_ATTACK_MAXVALUE		127
#define FX_ADSR_ATTACK_DEFAULTVALUE	64 //68

#define FX_ADSR_DECAY_MINVALUE		0
#define FX_ADSR_DECAY_MAXVALUE		127
#define FX_ADSR_DECAY_DEFAULTVALUE	64

#define FX_ADSR_RES_MINVALUE		0
#define FX_ADSR_RES_MAXVALUE		127
#define FX_ADSR_RES_DEFAULTVALUE	64

#define FX_ADSR_FREQ_MINVALUE		0
#define FX_ADSR_FREQ_MAXVALUE		127
#define FX_ADSR_FREQ_DEFAULTVALUE	64

#define FX_ADSR_RELEAS_MINVALUE		46
#define FX_ADSR_RELEAS_MAXVALUE		127
#define FX_ADSR_RELEAS_DEFAULTVALUE	64

/***** parameters enum *****/

enum{ // Mix parameters
	FX_MIX_INPUTGAIN,
	FX_MIX_LCFILTER,
	FX_MIX_HCFILTER,
	FX_MIX_OUTPUTLEVEL,
	FX_MIX_OUTPUTPANNING,
	FX_MIX_OUTPUTFRMIX,
	FX_MIX_SENDTOREVERB,
	FX_MIX_SENDTOCHORUS,
	FX_MIX_ALL
};

enum{ // Compressor parameters
	FX_COMP_ON_OFF,
	FX_COMP_ATTACKTIME,
	FX_COMP_RELEASETIME,
	FX_COMP_THRESHOLD,
	FX_COMP_RATIO,
	FX_COMP_BOOST,
	FX_COMP_KNEE,
	FX_COMP_DUMMY,
	FX_COMP_NAME,
	FX_COMP_ALL
};
#define FX_COMP_USER_SIZE 3


enum{ // Wah-Wah parameters
	FX_WAH_FILTERTYPE,
	FX_WAH_FILTERFREQ,
	FX_WAH_FILTERRES,
	FX_WAH_AUTOSENSITIVITY,
	FX_WAH_NAME,
	FX_WAH_ALL
};
#define FX_WAH_USER_SIZE 2

enum{ // Distortion parameters
	FX_DISTO_ON_OFF,
	FX_DISTO_PREGAIN,
	FX_DISTO_TYPE,
	FX_DISTO_LPFILTERFREQ,
	FX_DISTO_LPFILTERRES,
	FX_DISTO_POSTGAIN,
	FX_DISTO_DRIVE,
	FX_DISTO_DUMMY,
	FX_DISTO_NAME,
	FX_DISTO_ALL
};
#define FX_DISTO_USER_SIZE 2 // 3

enum{ // Equalizer parameters
	FX_PEQ_ON_OFF,
	FX_PEQ_LBGAIN,
	FX_PEQ_LMBGAIN,
	FX_PEQ_HMBGAIN,
	FX_PEQ_HBGAIN,
	FX_PEQ_LBFREQ,
	FX_PEQ_LMBFREQ,
	FX_PEQ_HMBFREQ,
	FX_PEQ_HBFREQ,
	FX_PEQ_LMBQ,
	FX_PEQ_HMBQ,
	FX_PEQ_DUMMY,
	FX_PEQ_NAME,
	FX_PEQ_ALL
};
#define FX_PEQ_USER_SIZE 4

enum{ // Others parameters
	FX_CHORUS_MODE,
	FX_CHORUS_EFFECTLEVEL,
	FX_CHORUS_DELAYTIME,
	FX_CHORUS_FEEDBACK,
	FX_CHORUS_HPFREQ,
	FX_CHORUS_HDAMP,
	FX_CHORUS_MODRATE,
	FX_CHORUS_MODDEPTH,
	FX_CHORUS_TREMOLO,
	FX_CHORUS_ROTARYSPEED,
	FX_CHORUS_NAME,
	FX_CHORUS_ALL
};
#define FX_CHORUS_USER_SIZE 4

enum{ // Delay parameters
	FX_DELAY_ON_OFF,
	FX_DELAY_MODE,
	FX_DELAY_PRELP,
	FX_DELAY_LEVEL,
	FX_DELAY_TIME,
	FX_DELAY_FEEDBACK,
	FX_DELAY_HDAMP,
	FX_DELAY_DUMMY,
	FX_DELAY_NAME,
	FX_DELAY_ALL
};
#define FX_DELAY_USER_SIZE 3

enum{ // reverb parameters
	FX_REVERB_LEVEL,
	FX_REVERB_DIRECTLEVEL,
	FX_REVERB_REVSEND,
	FX_REVERB_TONEGAIN,
	FX_REVERB_TONEFREQ,
	FX_REVERB_PREHP,
	FX_REVERB_TIME,
	FX_REVERB_ECHOFEEDBACK,
	FX_REVERB_HDAMP,
	FX_REVERB_THRESGATE,
	FX_REVERB_PREDELAYTIME,
	FX_REVERB_DUMMY,
	FX_REVERB_NAME,
	FX_REVERB_ALL,
	FX_REVERB_DRY_WET
};
#define FX_REVERB_USER_SIZE 3

enum{ // vibrato parameters
	FX_VIBRATO_DEPTH,
	FX_VIBRATO_DELAY,
	FX_VIBRATO_RATE,
	FX_VIBRATO_DUMMY,
	FX_VIBRATO_NAME,
	FX_VIBRATO_ALL
};
#define FX_VIBRATO_USER_SIZE 2

enum{ // adsr parameters
	FX_ADSR_ENV_ATTACK,
	FX_ADSR_ENV_DECAY,
	FX_ADSR_CUTOFF_RES,
	FX_ADSR_CUTOFF_FREQ,
	FX_ADSR_ENV_RELEASE,
	FX_ADSR_DUMMY,
	FX_ADSR_NAME,
	FX_ADSR_ALL
};
#define FX_ADSR_USER_SIZE 2


////////////////////////////////////////
//#ifdef __LPC177X_8X__
void init_instrmap(void);
void init_sound(struct_instr *sound_struct, uint32_t instr);
void erase_sound( uint32_t instr);
void save_sound(struct_instr *sound_struct, uint32_t instr);
void init_preset(preset_instr *struct_preset, uint32_t octave);

void get_instrbin(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length);
void save_instrbin(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length);

void get_instrmap(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length);
void save_instrmap(uint32_t keyboard, uint32_t instrument, uint8_t *buffer, uint32_t address, uint32_t length);
void erase_instrmap( uint32_t keyboard);

void set_mixpreset(FX_mix *fx_mix, uint32_t preset_ID);
void set_distortionpreset(FX_distortion *fx_disto, uint32_t preset_ID);
void set_compressorpreset(FX_compressor *fx_compressor, uint32_t preset_ID);
void set_equalizerpreset(FX_equalizer *fx_equalizer, uint32_t preset_ID);
void set_choruspreset(FX_chorus *fx_others, uint32_t preset_ID);
void set_delaypreset(FX_delay *fx_delay, uint32_t preset_ID);
void set_reverbpreset(FX_reverb *fx_reverb, uint32_t preset_ID);
void set_instrequalizerpreset(FX_equalizer *fx_equalizer, uint32_t preset_ID);

void setinstrdistortion(param_struct *param, int32_t value, uint32_t rec);
void setinstrcompressor(param_struct *param, int32_t value, uint32_t rec);
void setgeneraleq(param_struct *param, int32_t value, uint32_t rec);
void setinstreq(param_struct *param, int32_t value, uint32_t rec);
void setinstrchorus(param_struct *param, int32_t value, uint32_t rec);
void setinstrdelay(param_struct *param, int32_t value, uint32_t rec);
void setgeneralreverb(param_struct *param, int32_t value, uint32_t rec);

void setinstrvolume(param_struct *param, int32_t value, uint32_t rec);
void setinstroctave(param_struct *param, int32_t value, uint32_t rec);
void setinstrpanning(param_struct *param, int32_t value, uint32_t rec);
void setinstrsendtorev(param_struct *param, int32_t value, uint32_t rec);
void setinstrpitchbendsens(param_struct *param, int32_t value, uint32_t rec);
void setinstrpitchbend(param_struct *param, int32_t value, uint32_t rec);
void setinstrporttime(param_struct *param, int32_t value, uint32_t rec);
void setinstrexpression(param_struct *param, int32_t value, uint32_t rec);
void setinstrrelvolume(param_struct *param, int32_t value, uint32_t rec);

void setinstrwah_type(param_struct *param, int32_t value, uint32_t rec);
void setinstrwah_freq(param_struct *param, int32_t value, uint32_t rec);
void setinstrwah_res(param_struct *param, int32_t value, uint32_t rec);

void setinstradsr_attack(param_struct *param, int32_t value, uint32_t rec);
void setinstradsr_release(param_struct *param, int32_t value, uint32_t rec);

void setgyro_activ_time(param_struct *param, int32_t value, uint32_t rec);
void setgyro_desactiv_time(param_struct *param, int32_t value, uint32_t rec);
void setgyro_dead_zone(param_struct *param, int32_t value, uint32_t rec);
void setgyro_full_axis_zone(param_struct *param, int32_t value, uint32_t rec);
void setgyro_angle(param_struct *param, int32_t value, uint32_t rec);
void setgyro_nb_val_before_send(param_struct *param, int32_t value, uint32_t rec);

void switch_off_ctrl_with_preset_off(preset_instr *preset, uint32_t struct_p);
//#endif // __LPC177X_8X__

#endif // FX_PARAMETERS_MNG_H_
