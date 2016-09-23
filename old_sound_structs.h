/*
 * oldsound_parameters_mng.h
 *
 *  Created on: 07/08/2013
 *      Author: mma
 */


#ifndef OLDSOUND_STRUCTS_H_
#define OLDSOUND_STRUCTS_H_

#include "g_structs.h"
#include "sound_structs.h"

#include "instr_mapping.h"

/******** general defines ************/
#define FX_NUM_FX_INTR_OLD	8


/***** fx parameters specific structures*****/

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
}FX_distortion_old;

#define FX_DIST_SIZE_OLD ( 8 + NAME_CARACT )

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
}FX_compressor_old;

#define FX_COMP_SIZE_OLD ( 8 + NAME_CARACT )

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
}FX_chorus_old;

#define FX_CHORUS_SIZE_OLD ( 10 + NAME_CARACT )

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
}FX_delay_old;

#define FX_DELAY_SIZE_OLD ( 8 + NAME_CARACT )

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

}preset_instr_old;

/*
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
*/
#define PRESET_STRUCT_SIZE_OLD	(64 + INSTR_INFO_SIZE)

/***** fx parameters general structure*****/
typedef struct
{
    info_instr s_instrument;

    uint8_t s_presetnum;
    uint8_t s_displayled;
    // alignement
    uint8_t align[2];

    preset_instr_old	s_preset[FX_NUM_PRESET_INTR];

    // FX
    FX_mix			s_mix;
    FX_distortion_old	s_distortion[FX_NUM_FX_INTR_OLD];
    uint8_t			dummywah[128];
    FX_compressor_old	s_compressor[FX_NUM_FX_INTR_OLD];
    FX_equalizer_gen	s_equalizer[FX_NUM_FX_INTR_OLD];
    FX_delay_old		s_delay[FX_NUM_FX_INTR_OLD];
    FX_chorus_old		s_chorus[FX_NUM_FX_INTR_OLD];
    uint8_t			dummyvibrato[128];
    uint8_t			dummyadsr[144];

    uint8_t s_leds[NUM_LED_VALUE];

}struct_instr_old;

#define INSTRU_STRUCT_SIZE_OLD	(INSTR_INFO_SIZE + 4 + NUM_LED_VALUE +(FX_NUM_PRESET_INTR*PRESET_STRUCT_SIZE_OLD) + FX_MIX_SIZE + (FX_NUM_FX_INTR*(FX_DIST_SIZE_OLD + 16 + FX_COMP_SIZE_OLD + FX_EQ_SIZE_OLD + FX_CHORUS_SIZE_OLD + FX_DELAY_SIZE_OLD + 16 +18)))



typedef struct
{
    info_instr i_instrument;

    preset_instr_old	i_preset;

    // FX
    FX_mix			i_mix;
    FX_distortion_old	i_distortion;
    uint8_t			dummy0[16];
    FX_compressor_old	i_compressor;
    FX_equalizer_gen	i_equalizer;
    FX_delay_old		i_delay;
    FX_chorus_old		i_chorus;
    uint8_t			dummy1[16];
    uint8_t			dummy2[18];

}music_instr_old;

#define MUSIC_INSTRU_SIZE_OLD	(INSTR_INFO_SIZE + PRESET_STRUCT_SIZE_OLD + FX_MIX_SIZE + FX_DIST_SIZE_OLD + 16 + FX_COMP_SIZE_OLD + FX_EQ_SIZE_GEN + FX_DELAY_SIZE_OLD + FX_CHORUS_SIZE_OLD + 16 +18)


#endif // OLDSOUND_STRUCTS_H_
