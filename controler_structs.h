/*
 * controler_parameters_structs.h
 *
 *  Created on: 23 sept. 2016
 *      Author: Maxime
 */

#ifndef PARAMETERS_DUALO_STRUCTS_CONTROLER_STRUCTS_H_
#define PARAMETERS_DUALO_STRUCTS_CONTROLER_STRUCTS_H_

typedef struct
{
	uint8_t	c_mainvolume;

	//uint8_t c_reverb_preset;
	uint8_t c_equalizer_onoff;;
	uint8_t	c_equalizer_preset;

	uint8_t	c_fine_tuning;
	//uint8_t	c_semi_tuning;
	//uint8_t	c_scale_tuning;
	uint8_t	c_diatonic;
	uint8_t c_accessibility;

	uint8_t c_midi_din;
	//uint8_t	c_keys_curve;
	uint8_t	c_gyro_curve;
	uint8_t	c_sliders_curve;

	uint8_t c_instr;
	uint8_t c_midiout_mode;
	uint8_t c_song;

	uint8_t c_sleep_time;
	uint8_t c_screen_mode;
	uint8_t c_luminosity;

	uint8_t c_keys_sensibility;
	uint8_t c_keys_smoothing;
	uint8_t c_keys_detection;

	uint8_t c_rf_freq;

	uint8_t c_led_display;
	//uint8_t	c_aftertouch;

	uint8_t c_midiout_channel;
	uint8_t c_midiout_preset;

	uint8_t gyro_dead_zone;
	uint8_t gyro_angle;
	//int8_t gyro_direction;
	uint8_t	c_pressure_curve;
	uint8_t	c_circle_time;
	uint8_t c_midiout_keysens;
	uint8_t dummy[5];

}param_controler;

enum{
	CONTROLER_MAINVOLUME,
	CONTROLER_EQ_ONOFF,
	CONTROLER_EQ,
	CONTROLER_FINETUNING,
	CONTROLER_DIATONIC, //CONTROLER_SEMITUNING,
	CONTROLER_ACCESSIBILITY, //CONTROLER_SCALETUNING,
	CONTROLER_MIDIDIN, //CONTROLER_KEYCURVE,
	CONTROLER_GYROCURVE,
	CONTROLER_SLIDERCURVE,
	CONTROLER_INSTRUMENT,
	CONTROLER_MIDIMODE,
	CONTROLER_SONG,
	CONTROLER_SLEEPTIME,
	CONTROLER_SCREENMODE,
	CONTROLER_LUMINOSITY,
	CONTROLER_KEYSENSIBILITY,
	CONTROLER_KEYSMOOTHING,
	CONTROLER_KEYDETECTION,
	CONTROLER_RFFREQ,
	CONTROLER_LEDDISPLAY,
	CONTROLER_MIDIOUTCHANNEL,
	CONTROLER_MIDIOUTPRESET,
	CONTROLER_IMU_DEAD_ZONE,
	CONTROLER_IMU_ANGLE,
	CONTROLER_PRESSURECURVE,
	CONTROLER_CIRCLETIME,
	CONTROLER_MIDIOUTKEYSENSIBILITY,
};

#define CONTROLER_PARAM_SIZE	32

/***** fx parameters general structure*****/
typedef struct
{
	param_controler	c_param;

    // FX
	FX_equalizer_gen	c_equalizer[FX_NUM_FX_INTR];
	FX_reverb		c_reverb[FX_NUM_FX_INTR];

}struct_controler;


#define CONTROLER_STRUCT_SIZE	(CONTROLER_PARAM_SIZE + (FX_NUM_FX_INTR*( FX_EQ_SIZE_GEN + FX_REVERB_SIZE)))


// defines
// main volume
#define	MAIN_VOL_MINVALUE				0
#define	MAIN_VOL_MAXVALUE				127
#define	MAIN_VOL_DEFAULTVALUE			64

#define FINE_TUNING_MINVALUE			0
#define FINE_TUNING_MAXVALUE			127
#define FINE_TUNING_DEFAULTVALUE		64

#define SEMI_TUNING_MINVALUE			0 //50
#define SEMI_TUNING_MAXVALUE			12 //50
#define SEMI_TUNING_DEFAULTVALUE		0

#define SCALE_TUNING_NUM_PARAM			12
#define SCALE_TUNING_MINVALUE			0
#define SCALE_TUNING_MAXVALUE			3
#define SCALE_TUNING_DEFAULTVALUE		0

#define KEYS_CURVES_MINVALUE			0
#define KEYS_CURVES_MAXVALUE			2
#define KEYS_CURVES_DEFAULTVALUE		1
#define KEYS_CURVES_POINT				4096

#define GYRO_CURVES_MINVALUE			0
#define GYRO_CURVES_MAXVALUE			1//2
#define GYRO_CURVES_DEFAULTVALUE		0
#define GYRO_CURVES_POINT				65536 //2048

#define GYRO_FULL_AXIS_MIN 				0
#define GYRO_FULL_AXIS_MAX				45
#define GYRO_FULL_AXIS_DEFAULT			10

#define GYRO_SEND_ANGLE_MIN 			10
#define GYRO_SEND_ANGLE_MAX				90
#define GYRO_SEND_ANGLE_DEFAULT			40

#define GYRO_ACTIV_TIME_MIN				0
#define GYRO_ACTIV_TIME_MAX				100
#define GYRO_ACTIV_TIME_DEFAULT			15

#define GYRO_FULL_AXIS_MIN 				0
#define GYRO_FULL_AXIS_MAX				45
#define GYRO_FULL_AXIS_DEFAULT			10

#define GYRO_SEND_ANGLE_MIN 			10
#define GYRO_SEND_ANGLE_MAX				90
#define GYRO_SEND_ANGLE_DEFAULT			40

#define SLIDERS_CURVES_MINVALUE			0
#define SLIDERS_CURVES_MAXVALUE			1
#define SLIDERS_CURVES_DEFAULTVALUE		0
#define SLIDERS_CURVES_POINT			2048

#define INSTRUMENT_MINVALUE				0
#define INSTRUMENT_MAXVALUE				(2*NUM_INSTRU_KEYBOARD)
#define INSTRUMENT_DEFAULTVALUE			68

#define MIDIMODE_MINVALUE				0
#define MIDIMODE_MAXVALUE				3
#define MIDIMODE_DEFAULTVALUE			0

#define SONG_MINVALUE					0
#define SONG_MAXVALUE					RECORD_MAXSONG
#define SONG_DEFAULTVALUE				0

#define SLEEP_TIME_MINVALUE				0
#define SLEEP_TIME_MAXVALUE				30
#define SLEEP_TIME_DEFAULTVALUE			SLEEP_TIME_MINVALUE

#define SCREEN_MODE_MINVALUE			0
#define SCREEN_MODE_MAXVALUE			(NUM_SCREEN_MODE - 1)
#define SCREEN_MODE_DEFAULTVALUE		DISPLAY_NOTE_LATIN

#define LUMINOSITY_MINVALUE				0
#define LUMINOSITY_MAXVALUE				0x3F
#define LUMINOSITY_DEFAULTVALUE			0x29

#define KEY_SENSIBILITY_MINVALUE		0
#define KEY_SENSIBILITY_MAXVALUE		255
#define KEY_SENSIBILITY_DEFAULTVALUE	50 //120

#define KEY_SMOOTHING_MINVALUE			0
#define KEY_SMOOTHING_MAXVALUE			255
#define KEY_SMOOTHING_DEFAULTVALUE		180

#define KEY_DETECTION_MINVALUE			0
#define KEY_DETECTION_MAXVALUE			255
#define KEY_DETECTION_DEFAULTVALUE		150

#define RF_FREQ_MINVALUE				0
#define RF_FREQ_MAXVALUE				100 //125
#define RF_FREQ_DEFAULTVALUE			100

#define LED_DISPLAY_MINVALUE			0
#define LED_DISPLAY_MAXVALUE			1
#define LED_DISPLAY_DEFAULTVALUE		1
#endif /* PARAMETERS_DUALO_STRUCTS_CONTROLER_STRUCTS_H_ */
