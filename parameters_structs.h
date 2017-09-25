/*
 * parameters_structs.h
 *
 *  Created on: 25 sept. 2017
 *      Author: charly
 */

#ifndef PARAMETERS_STRUCTS_H_
#define PARAMETERS_STRUCTS_H_

// includes
#ifdef __LPC177X_8X__
#include "lpc_types.h"
#else
#include <stdint.h>
#endif // __LPC177X_8X__


#define NUM_PARAM_CONTROLER_MAX		20//19 //18
#define NUM_PARAM_SOUND_MAX			19 //18
#define NUM_PARAM_SOUND_MIDI_MAX	8 //18
#define NUM_PARAM_MUSIC_MAX			12 //11
#define NUM_PARAM_MUSIC_TRACK_MAX	18 //18
#define NUM_PARAM_MUSIC_TRACK_MIDI_MAX	7 //18

#define NUM_PARAM_TAB	65

//#define NUM_BUTTON_KEYBOARD	58

//#define NUM_INSTRU_KEYBOARD	4

#define PARAM_NAME_SIZE	18 //16 //12

#define NAME_CARACT	12

//#define NUM_LED_VALUE	32

#define FX_INTERFACE_FORCE_0 0x7FFFFFFF
#define FX_INTERFACE_FORCE_LAST 0x8FFFFFFF
#define FX_INTERFACE_FORCE_VAL 0x9FFFFFFF

#define FX_PARAM_NONE	0xFFFFFFFF

#define PARAM_FROMAFTERTOUCH	0x000F0000
#define PARAM_FROMSLIDER		0x00F00000
#define PARAM_FROMGYRO			0x0F000000
#define PARAM_FROMLFO			0xF0000000

#define PARAM_INCREMENTONCLIC	0x1
#define PARAM_PRESSLONGINIT		0x2
#define PARAM_INVERTONCLIC		0x4
#define PARAM_INCREMENTON2CLIC	0x8

typedef struct
{
	uint16_t	p_struct;
    uint16_t    p_param;

    uint8_t		p_name[PARAM_NAME_SIZE];

	uint8_t	p_min;
	uint8_t	p_max;
	uint8_t	p_default;
	uint8_t	p_sticky;

	uint16_t	p_screen;

	uint8_t 	p_linktoaftertouch;
	uint8_t 	p_linktoslider;
	uint8_t 	p_linktogyro;

	uint8_t 	p_incrementonclic;

	// Callback function for parameter setting
	uint32_t 	p_callback;
	// Callback function for parameter display
	uint32_t 	p_display;

	// pointer to extra parameters
	uint32_t 	*extras;
    uint16_t 	extra_size;
    uint16_t	p_screen_extra;

    // pointer to other parameters struct
    uint32_t 	*linked_dynamic;
    uint32_t 	*linked_next;

    uint16_t	p_CC;
    uint16_t	p_id;

}param_struct;

#ifdef __LPC177X_8X__
typedef param_struct* param_struct_p;
#else
typedef uint32_t param_struct_p;
#endif

// Size definition of param_struct
#define PARAM_STRUCT_SIZE	 (11 + (PARAM_NAME_SIZE/4)) // int32

// param struct
enum{
	CONTROLER,
	GENERAL_REVERB,
	GENERAL_EQ,
	INSTR_MIX,
	INSTR_DISTORTION,
	INSTR_WAH,
	INSTR_COMPRESSOR,
	INSTR_CHORUS,
	INSTR_EQUALIZER,
	INSTR_DELAY,
	INSTR_VIB,
	INSTR_ADSR,
	INSTR_PRESET,
	MUSIC_INSTR_MIX,
	MUSIC_INSTR_DISTORTION,
	MUSIC_INSTR_WAH,
	MUSIC_INSTR_COMPRESSOR,
	MUSIC_INSTR_EQUALIZER,
	MUSIC_INSTR_CHORUS,
	MUSIC_INSTR_DELAY,
	MUSIC_INSTR_PRESET,
	MUSIC_INSTR_VIB,
	MUSIC_INSTR_ADSR,
	MUSIC_TRACK,
	MUSIC_LOOP,
	MUSIC_MIX,
	MUSIC_REVERB,
	MUSIC_SONG,
	DUTOUCHINFO,
	COPYANDPAST,
	OTHERS,
	NONE,
	INSTR_ARPEGIATOR,
	NUM_PARAMETERSTRUCT,
};

// param struct
enum{
	CC_DUMMY,
	CC_OCTAVE,
	CC_AUTOPAN_RATE,
	CC_VIBRATO_RATE,
	CC_TREMOLO_RATE,
	CC_AUTOFILTER_RATE,
	NUM_INTERNALCC,
};

typedef struct
{
	const param_struct *param;
	uint32_t size;
}tab_param;

// others param
enum{
	PARAM_NONE,
	PARAM_DUGAME,
	PARAM_METRO,
	PARAM_INFODUTOUCH,
	PARAM_BATTERY,
	PARAM_INSTRUVOL,
	PARAM_LOOPSOUND,
	PARAM_LOOPBEATREPEAT,
	PARAM_MUSICBEATREPEAT,
	PARAM_COPYANDPAST_DUSOUND,
	PARAM_COPYANDPAST_DUMUSIC,
	PARAM_COPYANDPAST_DULOOP,
	PARAM_INSTRLED,
	PARAM_BLE_CANAL,
	PARAM_BLE_POWER,
	PARAM_SOUND_PRESET,
	PARAM_SOUND_TYPE,
	PARAM_LOOPCHANNEL,
	//CONTROLER_GYRO_Z_ACC,
	//CONTROLER_GYRO_P_ANGLE,
	//CONTROLER_GYRO_Y_ANGLE,
	//CONTROLER_GYRO_R_ANGLE,
	//CONTROLER_IMU_ACTIV_TIME,
	//CONTROLER_IMU_DESACTIV_TIME,
	//CONTROLER_IMU_DEAD_ZONE,
	//CONTROLER_IMU_FULL_AXIS_ZONE,
	//CONTROLER_IMU_ANGLE,
	//CONTROLER_IMU_NB_VAL_BEFORE_SEND,
	//CONTROLER_IMU_ACTIVATION_ZONE,
#ifdef CONFIG_TEST
	PARAM_RF
#endif
};

// dutouch infos
enum{
	DUTOUCH_BATT,
	DUTOUCH_NAME,
	DUTOUCH_USERID,
	DUTOUCH_FACTORYREF,
	DUTOUCH_COREFIRMWARE,
	DUTOUCH_SOUND,
	DUTOUCH_LICENCEOS,
	DUTOUCH_LICENCEFS,
	//DUTOUCH_MEMFREESPACE,
	//DUTOUCH_MEMTOTALSPACE,
#ifdef CONFIG_TEST
	COPRO_STATUS_CONTROL,
	COPRO_BATTERY_SUPPLY,
	COPRO_CONTROL,
	COPRO_BATTERY_VOLTAGE,
	COPRO_BATTERY_TERM,
	COPRO_VIN_VOLTAGE,
	COPRO_SAFETY,
#endif
	NUM_DUTOUCHINFO
};

#endif /* PARAMETERS_STRUCTS_H_ */
