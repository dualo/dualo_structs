/*
 * g_parameters_mng.c
 *
 *  Created on: 18/05/2010
 *      Author: mma
 */


// includes
#include "g_parameters_mng.h"

//#include "fx_parameters_mng.h"
#include "record_task.h"

// external parameters
extern int8_t standart_key_map[];
extern int8_t piano_key_map_R[];
extern int8_t piano_key_map_L[];
extern int8_t accordion_key_map_R[];
extern int8_t accordion_key_map_L[];
extern int8_t universal_key_map_R[];
extern int8_t universal_key_map_L[];
extern int8_t major_key_map[];
extern int8_t minor_nat_key_map[];
extern int8_t dorien_key_map[];
extern int8_t mixolidien_key_map[];
extern int8_t minor_harm_key_map[];
extern int8_t oriental_key_map[];
extern int8_t penta_maj_key_map[];
extern int8_t penta_min_key_map[];

/******* global parameters initialisation ************/
void init_globalparameters(void)
{
	// general
	//current_sleep_time = DEFAULT_SLEEP_TIME;
	//current_midiout_mode = NONE_MIDIOUT_MODE;

	// battery
	current_fuel_gauge = 0;

	// controls
	//current_audio_ctrl = DEFAULT_AUDIO_CTRL;
	last_preset_instr = 0;
	last_midi_instr = DEFAULT_MIDI_INSTR;
	//current_slidergyro_act = NONE_CTRL_ACT;

	//num_instruments_L = NUM_INSTRU_KEYBOARD;
	//num_instruments_R = NUM_INSTRU_KEYBOARD;

	current_rec_track = 0xFF;
	current_rec_track_ctrl = 0xFF;

	//v_transpose = RECORD_TRANSPOSEDEFAULT;
	//v_quantification = RECORD_QUANTLEVELDEFAULT;
	v_metronome = 0;

	// general volume
//	param_main_vol = DEFAULT_GENERAL_VOLUME;
	attach_main_vol = 0;

	// keyboards
	key_map[0][0] = standart_key_map;
	key_map[0][1] = standart_key_map;
	key_map[0][2] = 0;
	key_map[1][0] = piano_key_map_L;
	key_map[1][1] = piano_key_map_R;
	key_map[1][2] = 1;
	key_map[1][3] = 1;
	key_map[2][0] = accordion_key_map_L;
	key_map[2][1] = accordion_key_map_R;
	key_map[2][2] = 1;
	key_map[2][3] = 0;
	key_map[3][0] = universal_key_map_L;
	key_map[3][1] = universal_key_map_R;
	key_map[3][2] = 1;
	key_map[3][3] = 0;
	key_map[4][0] = major_key_map;
	key_map[4][1] = major_key_map;
	key_map[4][2] = 0;
	key_map[5][0] = minor_nat_key_map;
	key_map[5][1] = minor_nat_key_map;
	key_map[5][2] = 0;
	key_map[6][0] = dorien_key_map;
	key_map[6][1] = dorien_key_map;
	key_map[6][2] = 0;
	key_map[7][0] = mixolidien_key_map;
	key_map[7][1] = mixolidien_key_map;
	key_map[7][2] = 0;
	key_map[8][0] = minor_harm_key_map;
	key_map[8][1] = minor_harm_key_map;
	key_map[8][2] = 0;
	key_map[9][0] = oriental_key_map;
	key_map[9][1] = oriental_key_map;
	key_map[9][2] = 0;
	key_map[10][0] = penta_maj_key_map;
	key_map[10][1] = penta_maj_key_map;
	key_map[10][2] = 0;
	key_map[11][0] = penta_min_key_map;
	key_map[11][1] = penta_min_key_map;
	key_map[11][2] = 0;
	current_key_map = DEFAULT_KEY_MAP;
	current_key_offset = DEFAULT_KEY_OFFSET;

	// keyboard leds
	//current_led_mode = STATIC_LED_MODE; //DEFAULT_KEY_LED_MODE;

	// screen mode
	current_screen_mode = DEFAULT_SCREEN_MODE;

	//Leds
	//current_leds_luminosity = (DEFAULT_LEDS_LUM<<6)|DEFAULT_LEDS_LUM;

	// FX Controls
	//FX_Control_State = 0;

	// midi
	//midi_canal = DEFAULT_MIDI_CANAL;

	// record
	rec_state = 0;

#ifdef CONFIG_TEST
	rf_config = RF_MSG;
#endif

	// metronome
	//metronome_type = METRONOME_4_4;
	//metronome_mode = METRONOME_SOUND_MASK;
	//metronome_tempo = METRONOME_DEFAULT_TEMPO;
	//metronome_vol = 0;

}

