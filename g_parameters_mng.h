/*
 * g_parameters_mng.h
 *
 *  Created on: 18/05/2010
 *      Author: mma
 */


#ifndef G_PARAMETERS_MNG_H_
#define G_PARAMETERS_MNG_H_


//#include "fx_parameters_mng.h"

/***** parameters enum *****/

// controls
/*
enum{
	PLAY,
	EDITION_PLAY_CTRL,
	AUDIO_CTRL,
	EDITION_INSTR_CTRL,
	ARR_CTRL,
	EDITION_ARR_CTRL,
	EDITION_ARR_INSTR_CTRL,
	UPDATE,
	REFRESH
};
*/
/*
// Editions Ctrl
enum{
	NONE_ED_CTRL,

	ED_DUMMY,
	ED_SCREEN_MODE,
	ED_LEDS_LUM,
	ED_LED_MODE,
	ED_MIDIOUT_MODE,
	ED_MIDIOUT_CHANNEL,
	ED_BATTERYLEVEL,
	ED_SLEEP_MODE,
	ED_KEYS_SENSIBILITY,
	ED_KEYS_SMOOTHING,
	ED_KEYS_DETECTION,
	ED_SLIDERGYRO_ACTIVATION,

	ED_KEY_CURVE,
	ED_GYRO_CURVE,
	ED_SLIDER_CURVE,
	ED_INSTR_FINETUNE,
	ED_INSTR_SEMITONETUNE,
	ED_INSTR_OCTAVETUNE,
	ED_INSTR_TEMPEREDTUNE,


	ED_INSTR_VOL,
	ED_INSTR_PANNING,
	ED_INSTR_SENDTOREV,
	ED_INSTR_KEYSFX,
	ED_INSTR_GYROFX,
	ED_INSTR_SOUND,
	ED_INSTR_DRUMKEYS,
	ED_INSTR_MULTINOTES,
	ED_INSTR_PITCHBEND,
	ED_INSTR_POLY_MONO,
	ED_INSTR_PORTAMENTO_TIME,
	ED_INSTR_PORTAMENTO_ONOFF,

	ED_INSTR_PRESET,
	ED_GENERAL_REVERB,
	ED_GENERAL_EQ,

	ED_INSTR_VIBRATO,
	ED_INSTR_COMPRESSION,
	ED_INSTR_WAH,
	ED_INSTR_DISTORTION,
	ED_INSTR_CHORUS,
	ED_INSTR_DELAY,

	ED_LOOPER_1STLOOPMODE,
	ED_LOOPER_QUANTIFICATION,
	ED_LOOPER_TRANSPOSE,
	ED_LOOPER_LEARNADVANCE,

	ED_METRO_METER,
	ED_METRO_TEMPO,
	ED_METRO_VOL,
	ED_METRO_MODE,

	NUM_ED_CTRL
};
*/

// Audio controls
enum{
	KEY_AFTERTOUCH,
	SLIDER_L_DYN,
	SLIDER_R_DYN,
	GYROSCOPE_PITCH,
	GYROSCOPE_ROLL,
	GYROSCOPE_YAW,
	NUM_AUDIO_CTRL
};

// keyboard led mode states
enum{
	NONE_LED_MODE,
	MAJOR_LED_MODE,
	MINOR_NAT_LED_MODE,
	DORIEN_MODE,
	MIXOLIDIEN_MODE,
	MINOR_HARM_LED_MODE,
	ORIENTAL_LED_MODE,
	PENTA_MAJ_MODE,
	PENTA_MIN_MODE,
	STATIC_LED_MODE,
	NUM_LED_MODE
};

// Screen mode states
enum{
	NONE_SCREEN_MODE,
	DISPLAY_NOTE_LATIN,
	DISPLAY_NOTE_ENGLISH,
	DISPLAY_NOTE_GERMAN,
	NUM_SCREEN_MODE,
};

// Midi Out mode states
/*enum{
	NONE_MIDIOUT_MODE,
	MIDI_DREAM,
	MIDI_GM,
	MIDI_GM_LOWPOWER,
	MIDI_TABLE,
	MIDI_SONICCELL,
	NUM_MIDIOUT_MODE,
};
*/

// Sliders/Gyro states
enum{
	NONE_CTRL_ACT,
	SLIDERS_ACT,
	GYRO_ACT,
	SLIDER_GYRO_ACT,
	NUM_CTRL_ACT_MODE,
};

enum{
	KEYLAYOUT_DUALO,
	KEYLAYOUT_PIANO,
	KEYLAYOUT_ACCORDION,
	KEYLAYOUT_UNIVERSAL,
	KEYLAYOUT_DIATONIC,
	NUM_KEY_LAYOUT,
};

#if 0
// parameters
enum{
	NOTE_OFF,
	NOTE_ON,
	NOTE_AFTERTOUCH,
	NOTE_DETECTED
};

enum{
	KEYBOARD_L,
	KEYBOARD_R
};

typedef struct
{
	uint32_t	keyboard;
	uint32_t	key;
	uint32_t	cmd;
	uint32_t	value;
}key_control_t;
#endif

/***** parameters default/bounds *****/

#define FX_NUM_CONTROL_SLIDERS_PARAMETERS	10 //6
#define FX_NUM_CONTROL_GYRO_PARAMETERS		7 //6
#define FX_NUM_CONTROL_KEYS_PARAMETERS	5 //7 //6
#define FX_NUM_SLIDER_STATIC_ZONE	6

// general

#define NAME_CARACT	12


// controls
#define DEFAULT_CONTROL_STATE	PLAY
#define NUM_ED_PLAY_MAX			16 //15 //39
#define NUM_ED_INSTR_MAX		17 //39
#define NUM_ED_ARR_MAX			11 //39
//#define DEFAULT_ED_CTRL			NONE_ED_CTRL
//#define DEFAULT_ED_PARAM		NONE_ED_PARAM
#define DEFAULT_AUDIO_CTRL		NONE_AUDIO_CTRL

// volume level
#define MIN_GENERAL_VOLUME		0
#define MAX_GENERAL_VOLUME		0x7F
#define DEFAULT_GENERAL_VOLUME	MIN_GENERAL_VOLUME

// buttons reaction

// keyboards
#define NEW_INSTRU_MAP	2
#if NEW_INSTRU_MAP == 2
#define NUM_INSTRU_KEYBOARD	58
#elif NEW_INSTRU_MAP == 1
#define NUM_INSTRU_KEYBOARD	58
#else
#define NUM_INSTRU_KEYBOARD	4
#endif
#define NUM_BUTTON_KEYBOARD	58
#define NUM_LED_VALUE	32
//#define NUM_KEY_LAYOUT		5 //9
#define NUM_KEY_MAP			12 //9
#define DEFAULT_KEY_MAP		0
#define DEFAULT_KEY_OFFSET	0 // do

#define SECONDINSTR_SEUIL_ON	3500
#define NOTE_SEUIL_ON	150 //50
#define NOTE_SEUIL_OFF	30 //30

#define KEYS_SENSIBILITY_MAX	0x200 //64

#define NOTE_SEUIL_CTRL	40 // Carefull range 0->127

#define KEYBOARD_CONFIG 2 //2 //0
#define KEYBOARD_AFTERTOUCH  1//0
#define KEYBOARD_AFTERTOUCH_COMPT  16 // 64 //128 //64//0
#define KEY_SAVE	2 // 2^KEY_SAVE
#define KEY_DIFF_RATE	20 // 20 //10
#define KEY_NUM_MAX_ANALYSE	20
#define KEY_DEFAULT_VALUE (0x40<<5)

#define KEY_FX_MANDOLINE_THRESHOLD	0x10 //20
#define KEY_FX_LATCH_THRESHOLD	0x70

// keyboard leds
#define DEFAULT_KEY_LED_MODE	MAJOR_LED_MODE //STATIC_LED_MODE //NONE_LED_MODE
#define TIMER_KEY_LED	150 //100

#define KEY_DETECTION_MAX	255

// midi
#define DEFAULT_MIDI_INSTR	10 //20
#define MIDI_INSTRUMENTS_NUM	128
#define DEFAULT_MIDI_CANAL	0
#define MAX_MIDI_CANAL	8 //16
#define MIDI_MAX_INSTR	58 //58

// sliders (IQS222)
#define SLIDER_VALUE_CHANGED_THRESHOLD	25//25 //5 //40  //3
#define SLIDER_VALUE_MIN_THRESHOLD		0 //200 //50
#define SLIDER_VALUE_MAX_THRESHOLD		(0xE00 - SLIDER_VALUE_MIN_THRESHOLD)
#define SLIDER_G_MIN_THRESHOLD	0x100 //0x6FF //30
#define SLIDER_G_MAX_THRESHOLD	0xcb0 //0xd00 //(0x7FF - SLIDER_G_MIN_THRESHOLD) //0x6FF //30

// gyroscope
#define GYRO_VALUE_CHANGED_THRESHOLD	60 //30 //20 //10 //20
#define GYRO_VALUE_CHANGED_THRESHOLD_ERROR	0x800
#define GYRO_VALUE_MAX_ZONE		0xFF00
#define GYRO_VALUE_MIN_ZONE		0x00FF
#define GYRO_FX_THRESHOLD		1400 //700
#define GYRO_COMPENSATION_THRESHOLD	2 //5

// Screen
#define DEFAULT_SCREEN_MODE	NONE_SCREEN_MODE

// Leds
#define MAX_LEDS_LUM		0x3F
#define DEFAULT_LEDS_LUM	MAX_LEDS_LUM

// FX Controls
#define FX_CONTROL_SLIDER_L	0x01
#define FX_CONTROL_SLIDER_R	0x02
#define FX_CONTROL_GIRO_X	0x04
#define FX_CONTROL_GIRO_Y	0x08
#define FX_CONTROL_GIRO_Z	0x10


// decalage slider
#define DECAL_MIN	5 //5 // value 0->127
#define DECAL_NONE	0
#define DECAL_MED	8 //7 // value 0->15
#define DECAL_MAX	9 //8 // value 0->3


#if 0
// Keyboards
#define KEY_FIFO_ELEMENT_SIZE	(sizeof(key_control_t)/sizeof(uint32_t))
#define KEY_FIFO_LENGTH_CTRL (16 * KEY_FIFO_ELEMENT_SIZE)
#endif
/***** parameters list *****/

// memory management

/*
#define EXTFLASH_PART_DATA		0x80000000
#define EXTFLASH_PART_FXPARAM	0x80200000
#define EXTFLASH_PART_GPARAM	0x802FE000
#define EXTFLASH_PART_PRESET	0x802FF000
#define EXTFLASH_PART_RECORD	0x80300000
#define EXTFLASH_END			0x80400000
*/
#define EXTFLASH_ADDR_CONTROLER		0x90100000
#define EXTFLASH_ADDR_SOUND			0x90200000
#define EXTFLASH_ADDR_MUSIC			0x90300000
#define EXTFLASH_ADDR_RECORD		0x90700000

#define EXTFLASH_SIZE_CONTROLER		8
#define EXTFLASH_SIZE_SOUND			8
#define EXTFLASH_SIZE_MUSIC			32
#define EXTFLASH_SIZE_RECORD		2000


#ifdef __LPC177X_8X__
#include "lpc_types.h"
// general
//uint32_t current_sleep_time;
//uint32_t current_midiout_mode;

// fuel gauge
uint32_t current_fuel_gauge;

// controls

//uint32_t v_transpose;
//uint32_t v_quantification;
uint32_t v_metronome;

uint32_t loop_edit_sound;
uint32_t loop_beat_repeat;
uint32_t music_beat_repeat;

uint32_t current_rec_track;
uint32_t current_rec_track_ctrl;

uint32_t current_audio_ctrl;
uint32_t last_preset_instr;
uint32_t last_midi_instr;
//uint32_t current_slidergyro_act;

//uint32_t num_instruments_L;
//uint32_t num_instruments_R;


// general volume
//uint32_t param_main_vol;
uint32_t attach_main_vol;

// keyboards
int8_t *key_map[NUM_KEY_MAP][4];
uint32_t current_key_map;
int32_t current_key_offset;

// keyboards led
//uint32_t current_led_mode;

// midi
//uint32_t midi_canal;

// screen
uint32_t current_screen_mode;

// leds
//uint32_t current_leds_luminosity;

// FX Controls
//uint32_t FX_Control_State;
//uint32_t instr_preset;

// record
uint32_t rec_state;

// metronome
uint32_t metronome_type;
uint32_t metronome_mode;
uint32_t metronome_tempo;
uint32_t metronome_vol;

#ifdef CONFIG_TEST

enum {
	RF_NONE,
	RF_CARRIER,
	RF_MSG
};

uint32_t rf_config;
#endif

//extern uint8_t Edition_Play_table[NUM_ED_PLAY_MAX][2];
//extern uint8_t Edition_Instr_table[NUM_ED_INSTR_MAX][2];
//extern uint8_t Edition_Arr_table[NUM_ED_ARR_MAX][2];
////////////////////////////////////////
void init_globalparameters(void);

#endif

#ifdef __LPC18XX__
#include "lpc_types.h"
// general
//uint32_t current_sleep_time;
//uint32_t current_midiout_mode;

// fuel gauge
uint32_t current_fuel_gauge;

// controls

//uint32_t v_transpose;
//uint32_t v_quantification;
uint32_t v_metronome;

uint32_t loop_edit_sound;
uint32_t loop_beat_repeat;
uint32_t music_beat_repeat;

uint32_t current_rec_track;
uint32_t current_rec_track_ctrl;

uint32_t current_audio_ctrl;
uint32_t last_preset_instr;
uint32_t last_midi_instr;
//uint32_t current_slidergyro_act;

//uint32_t num_instruments_L;
//uint32_t num_instruments_R;


// general volume
//uint32_t param_main_vol;
uint32_t attach_main_vol;

// keyboards
int8_t *key_map[NUM_KEY_MAP][4];
uint32_t current_key_map;
int32_t current_key_offset;

// keyboards led
//uint32_t current_led_mode;

// midi
//uint32_t midi_canal;

// screen
uint32_t current_screen_mode;

// leds
//uint32_t current_leds_luminosity;

// FX Controls
//uint32_t FX_Control_State;
//uint32_t instr_preset;

// record
uint32_t rec_state;

// metronome
uint32_t metronome_type;
uint32_t metronome_mode;
uint32_t metronome_tempo;
uint32_t metronome_vol;

#ifdef CONFIG_TEST

enum {
	RF_NONE,
	RF_CARRIER,
	RF_MSG
};

uint32_t rf_config;
#endif

//extern uint8_t Edition_Play_table[NUM_ED_PLAY_MAX][2];
//extern uint8_t Edition_Instr_table[NUM_ED_INSTR_MAX][2];
//extern uint8_t Edition_Arr_table[NUM_ED_ARR_MAX][2];
////////////////////////////////////////
void init_globalparameters(void);

#endif


#endif // G_PARAMETERS_MNG_H_
