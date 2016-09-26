/*
 * g_parameters_structs.h
 *
 *  Created on: 23 sept. 2016
 *      Author: Maxime
 */

#ifndef PARAMETERS_DUALO_STRUCTS_G_STRUCTS_H_
#define PARAMETERS_DUALO_STRUCTS_G_STRUCTS_H_

/***** parameters enum *****/

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


/***** parameters list *****/

// memory management

#define EXTFLASH_ADDR_CONTROLER		0x90100000
#define EXTFLASH_ADDR_SOUND			0x90200000
#define EXTFLASH_ADDR_MUSIC			0x90300000
#define EXTFLASH_ADDR_RECORD		0x90700000

#define EXTFLASH_SIZE_CONTROLER		8
#define EXTFLASH_SIZE_SOUND			8
#define EXTFLASH_SIZE_MUSIC			32
#define EXTFLASH_SIZE_RECORD		2000

#endif /* PARAMETERS_DUALO_STRUCTS_G_STRUCTS_H_ */