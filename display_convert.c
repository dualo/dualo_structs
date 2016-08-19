/*
 * display_convert.c
 *
 *  Created on: 18 juin 2014
 *      Author: LAdolphe
 */

// includes
#include "display_convert.h"
#include "controler_parameters_mng.h"
#include "music_parameters_mng.h"
#include "midi_parameters_mng.h"
#include "g_parameters_mng.h"
#include "yaffsfs.h"
#include "config.h"
#include "midi_task.h"
#include "main_task.h"
#include "mem/memory_manager.h"
#include "string.h"
#include "record_task.h"

#ifdef __LPC18XX__
#include "screen/screen.h"
#else
#include "screen/DT018ATFT.h"
#include "lpc177x_8x_eeprom.h"
#endif

#include "eeprom/eeprom_map.h"
#include "copy/copyandpast.h"


extern struct_controler controler_tab;
extern sound_instr instr_tab[];
extern music_song music_tab[];
extern struct_miditable midi_tab[MIDITABLE_MAX];

extern param_struct	controler_param[NUM_PARAM_CONTROLER_MAX];
extern param_struct	music_param[NUM_PARAM_MUSIC_MAX];
extern param_struct dutouch_info[];
extern uint8_t displayled_mode[NUM_LED_MODE][15];
extern uint8_t licenceOS_name[];
extern uint8_t licenceFT_name[];
extern uint8_t copyandpast_screenmsg[3][17];
extern uint8_t midiout_mode[MIDIOUTMODE_NUM][SCREEN_DISPLAY_CTRL_NAME_NUM];
extern uint8_t diatonic_mode[NUM_KEY_LAYOUT][10];
extern uint8_t ble_mode[8][10];
extern  uint8_t defaultsong_screenmsg[SCREEN_DISPLAY_CTRL_NAME_NUM];

//#include "debug/trace.h"

void negvalue (int32_t* val) {
	if (val == NULL)
		return;
	if ((*val) < 0) {
		(*val) = -(*val); // On met la valeur en positif
		(*val) &= 0x00007FFF; // Masque sur les 15 premiers bits
		(*val) |= 0x00008000; // Le 16ième bit devient le bit de parité
	}
	else if ((*val) >= 0) {
		(*val) &= 0x00007FFF;
	}
}

static void display_value(uint32_t value, uint32_t screen_windown, uint32_t rec)
{
	if(rec == 2)
	{
#if CONFIG_SCREEN_TASK_ENABLED == 1
		screenTask_add_msg( POPUPSCREEN_LINE3, SCREEN_PRIORITY_POPUP, screen_windown, value);
#endif
	}
	else if((rec == 1)||(rec == 3))
	{
#if CONFIG_SCREEN_TASK_ENABLED == 1
		screenTask_add_msg( MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, screen_windown, value);
#endif
	}
	else
	{
#if CONFIG_SCREEN_TASK_ENABLED == 1
		screenTask_add_msg( PLAYSCREEN_LINE3, SCREEN_PRIORITY_POPUP, screen_windown, value);
#endif
	}
}

// Gen Reverb : Dry Level, Wet Level ( sauf si on regroupe le Dry/Wet)
// Wah-Wah : Auto-Sensitivity

void displaypercent(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 100]%
	val_display = (value*100)/127;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

void displaypercent_255(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 255] => [0 ; 100]%
	val_display = (value*100)/255;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

void displaypercent_230(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 230] => [0 ; 100]%
	val_display = (value*100)/230;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}



// EQ (Gen + Instr) : Low Band, Mid-L Band, Mid-H Band, High Gain
void displaygain(param_struct *param, int32_t value, uint32_t rec)
{

	int32_t val_display;

	// Linear Conversion [0 ; 127] => [-12 ; 12]dB
	val_display = (value*24)/127 - 12;
	negvalue(&val_display);
	//trace(info,"val_display = %.8x\n", res);

	display_value(val_display, param->p_screen, rec);
}


// Gen EQ (Gen + Instr) : Low Band Freq
void displayeq_low_freq(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 800]Hz
	val_display = (value*800)/127;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Gen EQ (Gen + Instr) : Mid-Low Band Freq
void displayeq_midl_freq(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 2500]Hz
	val_display = (value*2500)/127;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Gen EQ (Gen + Instr) : Mid-High Band Freq
void displayeq_midh_freq(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [60 ; 10700]Hz
	val_display = (value*10640)/127 + 60;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Gen EQ (Gen + Instr) : High Band Freq
void displayeq_high_freq(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [1000 ; 5800]Hz
	val_display = (value*4800)/127 + 1000;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}



// Reverb : Tone Gain
void displayrevtonegain(param_struct *param, int32_t value, uint32_t rec)
{

	int32_t val_display;

	// Linear Conversion [0 ; 127] => [-12 ; 6]dB
	val_display = (value*18)/127 + 5;
	negvalue(&val_display);

	display_value(val_display, param->p_screen, rec);
}

// Reverb : Time
/*--------------- Fixed range from 500ms to 20sec (linear) ---------------*/
void displayrevtime(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Exponential Linearized Conversion [0 ; 127] => [500 ; 20000]ms
	if ((value >= 0) && (value < 50)) {
		val_display = (value*900) / 50 + 500;
	}
	else if ((value >= 50) && (value < 90)) {
		val_display = (value*6600) / 40 - 6850;
	}
	else if ((value >= 90) && (value <= 127)){
		val_display = (value*12000) / 37 - 21189;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Reverb : Pre-Delay Time
void displayrevpredeltime(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 250]ms
	val_display = (value*250)/127;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}



// Compressor : Attack Time
void displaycompressorattack(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Exponential Linearized Conversion [0 ; 127] => [0,1 ; 100]ms
	if ((value >= 0) && (value < 100)) {
		val_display = value/10;
	}
	else if ((value >= 100) && (value <= 127)) {
		val_display = (value*10)/3 - 323;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Compressor : Release Time
void displaycompressorrelease(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Exponential Linearized Conversion [0 ; 127] => [10 ; 5000]ms
	if ((value >= 0) && (value < 76)) {
		val_display = value*2;
	}
	else if ((value >= 76) && (value < 114)) {
		val_display = value*50 - 3631;
	}
	else if ((value >= 114) && (value < 127)) {
		val_display = value*219 - 22818;
	}
	else if (value == 127) {
		val_display = 5000;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}


// Compressor : Threshold
// Distortion : Pre-Gain, Post-Gain
void displaygain_42(param_struct *param, int32_t value, uint32_t rec)
{

	int32_t val_display;

	// Linear Conversion [0 ; 127] => [-42.3 ; 0]dB
	val_display = (value*42)/127 - 42;
	negvalue(&val_display);

	display_value(val_display, param->p_screen, rec);
}


// Compressor : Ratio
void displaycompressorratio(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Non-Linear Conversion [0 ; 127] => [1 ; 128]
	val_display = 128 / (127 - value + 1);
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Compressor : Boost
void displaycompressorboost(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Non-Linear Conversion [0 ; 127] => [0 ; 21]dB
	if ((value >= 0) && (value < 16)) {
		val_display = 0;
	}
	else if ((value >= 16) && (value < 127)) {
		val_display = (value*3)/16 - 3;
	}
	else if (value == 127) {
		val_display = 21;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Wah-wah : Filter Resonance
void displaywahres(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [20 ; 300]%
	val_display = (value * 280) / 127 + 20;
	val_display &= 0x0000FFFF;


	display_value(val_display, param->p_screen, rec);
}

// Distortion : Low Pass Filter Frequency
void display8khzfreq(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 8000]Hz
	val_display = (value * 8000) / 127;
	val_display &= 0x0000FFFF;


	display_value(val_display, param->p_screen, rec);
}

// Wah-wah : Frequency = Pedal Position
void display8khzfreqwah(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 8000]Hz
	val_display = (value * 8000) / 127;
	val_display &= 0x0000FFFF;


	display_value(val_display, param->p_screen, rec);
}

void displaymixlp(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [1500 ; 8000]Hz
	val_display = ((value * 6500) / 127) + 1500;
	val_display &= 0x0000FFFF;


	display_value(val_display, param->p_screen, rec);
}

// Distortion : Drive
void displaydrive(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Linear Conversion [0 ; 7] => [0 ; 42]dB
	if ((value >= 0) && (value <= 7))
		val_display = value * 6;
	else if ((value > 7) && (value <= 127))
		val_display = 42; // = max : 6*7
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}


// Chorus : Delay Time
void displaychorusdelay(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [1 ; 30]ms
	val_display = (value * 29) / 127 + 1;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}


// Chorus : High Pass Filter on Input
void display1200Hz(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 1200]Hz
	val_display = (value * 1200) / 127;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Chorus : Rate
void displaychorusrate(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	uint32_t val_display = 0;
	uint32_t mode;

	temp = controler_tab.c_param.c_instr;
	mode = instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus[instr_tab[temp].s_preset[instr_tab[temp].s_presetnum].s_chorus_preset-1].c_mode;


	if (mode == 2) {
	// Mode 2 = Tremolo ==> 3 Linearized Conversion [0 ; 127] => [1000 ; 20000]mHz
		if ((value >= 0) && (value <= 64))
			val_display = (value * 3000) / 64;
		else if ((value > 64) && (value <= 100))
			val_display = (value * 7000) / 36 - 9444;
		else if ((value > 100) && (value <= 127))
			val_display = (value * 10000) / 27 - 27037;
	}
	else {
	// Others Mode = Chorus, Flanger, Phaser or Rotary ==> 2-Linear Conversion [0 ; 127] => [23 ; 5800]mHz
		if ((value >= 0) && (value <= 64))
			val_display = (value * 66) / 64 + 23;
		else if ((value > 64) && (value < 127))
			val_display = (value * 5711) / 63 - 5713;
		else if ((value == 127))
			val_display = 5800;

	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}


// Delay Time
void displaydelaytime(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	double a, b, c, d;
	// Polynomial Conversion [255 ; 0] => [255 ; 234] = [0; 107] ms | [234 ; 0] = [40; 268] BPM

#if MIDI_127 == 1
	value = 127 - value;
	if (value < 117) {
		a = -0.000125258 * value * value * value;
		b = 0.042463356 * value * value;
		c = -5.171194072 * value;
		d = 268.5614004;
#else
	value = 255 - value;
	if (value < 234) {
		a = -0.00001565723 * value * value * value;
		b = 0.010615839 * value * value;
		c = -2.585 * value;
		d = 268.561;
#endif
		val_display = (uint32_t) (a + b + c + d);

		// 16ième bit est à 0 si on doit afficher des BPM
		val_display &= 0x00007FFF;
	}
	else {
#if MIDI_127 == 1
		// Polynomial Conversion [0 ; 40] => [111; 243] ms
		/*a = -0.000223333 * value * value * value;
		b = 0.054470733 * value * value;
		c = 1.638043989 * value;
		d = 110.7770147;
		val_display = (uint32_t) a + b + c + d;*/

		// Linear Conversion [0 ; 127] => [0 ; 1365]ms
		val_display = (value * 1365) / 127;
#else
		val_display = value * 5;
#endif
		// 16ième bit est à 1 si on doit afficher des ms
		val_display &= 0x00007FFF;
		val_display |= 0x00008000;
	}

	//trace(info,"t_delay = %d\r\n", val_display);

	display_value(val_display, param->p_screen, rec);
}


// Delay BPM
void displaydelaybpm(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	double a, b, c, d;
	// Polynomial Conversion [40 ; 127] => [537; 80] BPM


#if MIDI_127
	value = 127 - value;
	a = -0.000125258 * value * value * value;
	b = 0.042463356 * value * value;
	c = -5.171194072 * value;
	d = 268.5614004;
#else
	value = 255 - value;
	a = -0.0000156572373832607 * value * value * value;
	b = 0.0106158390292934 * value * value;
	c = -2.58559703592344 * value;
	d = 268.561400359114;
#endif
	val_display = (uint32_t) (a + b + c + d);

	// 16ième bit est à 0 si on doit afficher des BPM
	val_display &= 0x00007FFF;

	//trace(info,"t_delay = %d\r\n", val_display);

	display_value(val_display, param->p_screen, rec);
}

// ADSR : Attack Time
/*--------------- Fixed range from 10ms to 5sec (non-linear) ---------------*/
void displayADSRattack(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Exponential Linearized Conversion [68 ; 127] => [0 ; 5000]ms
	if ((value >= 0) && (value <= FX_ADSR_ATTACK_MINVALUE)) {
		val_display = 0;
	}
	else if ((value > FX_ADSR_ATTACK_MINVALUE) && (value < 110)) {
		val_display = (value*1414) / 40 - 2190;
	}
	else if ((value >= 110) && (value < 127)){
		val_display = (value*3300) / 17 - 19653;
	}
	else if (value == 127) {
		val_display = 5000;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// ADSR : Decay Time
/*--------------- Fixed range from 10ms to 20sec (non-linear) ---------------*/
void displayADSRdecay(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Exponential Linearized Conversion [0 ; 127] => [10 ; 20000]ms
	if ((value >= 0) && (value < 58)) {
		val_display = (value*2990) / 58 + 10;
	}
	else if ((value >= 58) && (value < 85)) {
		val_display = (value*13000) / 27 - 24926;
	}
	else if ((value >= 85) && (value <= 127)) {
		val_display = (value*4000) / 42 + 7905;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}



// ADSR : Release Time
/*--------------- Fixed range from 10ms to 10sec (non-linear) ---------------*/
void displayADSRrelease(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Exponential Linearized Conversion [46 ; 127] => [0 ; 10000]ms
	if ((value >= 0) && (value < 47)) {
		val_display = 0;
	}
	if ((value >= 47) && (value < 64)) {
		val_display = (value*500) / (64-47) - 1382;
	}
	else if ((value >= 64) && (value < 100)) {
		val_display = (value*2700) / 36 - 4300;
	}
	else if ((value >= 100) && (value <= 127)) {
		val_display = (value*6800) / 27 - 21985;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Vibrato : Rate
/*--------------- Fixed range from 0 to 20000mHz (non-linear) ---------------*/
void displayratevib(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	// Exponential Linearized Conversion [0 ; 127] => [0 ; 20]mHz
	if ((value >= 0) && (value < 64)) {
		val_display = (value*3000) / 64;
	}
	else if ((value >= 64) && (value < 100)) {
		val_display = (value*7000) / 36 - 9444;
	}
	else if ((value >= 100) && (value < 127)) {
		val_display = (value*10000) / 27 - 27037;
	}
	else if (value == 127) {
		val_display = 20000;
	}
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Vibrato : Depth
/*--------------- Fixed range from 0 to ±50ct (linear) ---------------*/
void displaydepthvib(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	// Linear Conversion [64 ; 127] => [0 ; 50]ct
	//val_display = (value*50) / 127;
	if (value >= 64)
		val_display = (value*100) / 127 - 50;
	else
		val_display = 0;

	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}


// Tone Tuning
/*--------------- Fixed range from -50ct to +50ct (linear) ---------------*/
void displayhundredthtone(param_struct *param, int32_t value, uint32_t rec)
{

	int32_t val_display;

	// Linear Conversion [0 ; 127] => [-50 ; +50]ct
	val_display = (value*100) / 127 - 50;
	negvalue(&val_display);

	display_value(val_display, param->p_screen, rec);
}

// Luminosity
void displayluminositypercent(param_struct *param, int32_t value, uint32_t rec)
{

	int32_t val_display;

	// Linear Conversion [0 ; 63] => [0 ; 100]%
	val_display = (value*100) / 63;
	val_display &= 0x0000FFFF;

	display_value(val_display, param->p_screen, rec);
}

// Panning
void displaypanning(param_struct *param, int32_t value, uint32_t rec)
{

	int32_t val_display;

	// Linear Conversion [0 ; 127] => [0 ; 100]%
	val_display = value - 64;
	negvalue(&val_display);

	display_value(val_display, param->p_screen, rec);
}

// Pitch
void displaypitch(param_struct *param, int32_t value, uint32_t rec)
{
	/*
	uint32_t temp;
	if(rec)
	{
	#if CONFIG_SCREEN_TASK_ENABLED == 1
		temp = (SCREEN_PART_3<<(SCREEN_DISPLAY_CMD_NUM + SCREEN_DISPLAY_VALUE_NUM)) | (SCREEN_DISPLAY_PITCH << SCREEN_DISPLAY_VALUE_NUM) | (value);
		screenTask_add_cmd(temp);
	#endif
	}
	else
	{
	#if CONFIG_SCREEN_TASK_ENABLED == 1
		temp = ((SCREEN_DISPLAY_TURN_ON | SCREEN_DISPLAY_WITH_TIMER_MASK | SCREEN_PART_3)<<(SCREEN_DISPLAY_CMD_NUM + SCREEN_DISPLAY_VALUE_NUM)) | (SCREEN_DISPLAY_VALUE << SCREEN_DISPLAY_VALUE_NUM) | (value);
		screenTask_add_cmd(temp);
	#endif
	}
	*/
}

// Portamento
void displayportamento(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display = 0;

	if(value>1)
	{
		value--;
		// Exponential 3-Linearized Conversion [0 ; 127] => [0 ; 25000]mHz/oct
		if ((value >= 0) && (value < 64)) {
			val_display = (value*360) / 64;
		}
		else if ((value >= 64) && (value < 114)) {
			val_display = (value*4165) / 57 - 4280;
		}
		else if ((value >= 114) && (value < 127)) {
			val_display = (value*20475) / 13 - 175498;
		}
		else if ((value == 127)) {
			val_display = 25000;
		}
		val_display &= 0x0000FFFF;
		negvalue((int32_t *)&val_display);
	}
	else
		val_display = value;

	display_value(val_display, param->p_screen, rec);
}

// Octave
void displayoctave(param_struct *param, int32_t value, uint32_t rec)
{

	uint32_t val_display;

	if(value==0)
	{
		val_display = 0x8002;
	}
	else if(value==1)
	{
		val_display = 0x8001;
	}
	else
	{
		val_display =  (value - 2);
	}


	display_value(val_display, param->p_screen, rec);
}

// Transpose
void displaytranspose(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t val_display;

	if(value<RECORD_TRANSPOSEDEFAULT)
	{
		val_display = 0x8000;
		val_display |= RECORD_TRANSPOSEDEFAULT - value;
	}
	else
	{
		val_display =  value - RECORD_TRANSPOSEDEFAULT;
	}


	display_value(val_display, param->p_screen, rec);
}


// info
void display_info(param_struct *param, int32_t value, uint32_t rec)
{

	uint8_t buffer_tmp[16];

	screenTask_add_string(MENUSCREEN_LINE1, SCREEN_PRIORITY_NONE, (uint8_t *)controler_param[15].p_name, 0);
	screenTask_add_string(MENUSCREEN_LINE2, SCREEN_PRIORITY_NONE, (uint8_t *)dutouch_info[value].p_name, 0);

	if(value == 0)
	{
		screenTask_add_msg( MENUSCREEN_LINE3_BATT, SCREEN_PRIORITY_WAKEUP, SCREEN_DISPLAY_BIGBATT, 0);
	}
	else if(value == 1)
	{

#ifndef EEPROM_OFF
		EEPROM_Read8(eeprom_map[EE_INFO_NAME].addr,(void *)buffer_tmp, eeprom_map[EE_INFO_NAME].size);
		screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, buffer_tmp, 1);
#endif
	}
	else if(value == 2)
	{
#ifndef EEPROM_OFF
		EEPROM_Read8(eeprom_map[EE_INFO_USER].addr,(void *)buffer_tmp, eeprom_map[EE_INFO_USER].size);
		screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, buffer_tmp, 1);
#endif
	}
	else if(value == 3)
	{
#ifndef EEPROM_OFF
		EEPROM_Read8(eeprom_map[EE_INFO_SN].addr,(void *)buffer_tmp, eeprom_map[EE_INFO_SN].size);
		screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, buffer_tmp, 1);
#endif
	}
	else if(value == 4)
	{
		screenTask_add_msg( MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, SCREEN_DISPLAY_DUTOUCH_COREFIRMWARE, 1);
	}
	else if(value == 5)
	{
#ifndef EEPROM_OFF
		EEPROM_Read8(eeprom_map[EE_INFO_VERSION_SOUND].addr,(void *)buffer_tmp, eeprom_map[EE_INFO_VERSION_SOUND].size);
		screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, buffer_tmp, 1);
#endif
	}
	else if(value == 6)
	{
		screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, (uint8_t *)licenceOS_name, 0);
	}
	else if(value == 7)
	{
		screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, (uint8_t *)licenceFT_name, 0);
	}
}

// scale
void displayscale(param_struct *param, int32_t value, uint32_t rec)
{
	if((music_tab[controler_tab.c_param.c_song].s_displaynote < STATIC_LED_MODE)&&(music_tab[controler_tab.c_param.c_song].s_displaynote != NONE_LED_MODE))
	{
		screenTask_add_string(MENUSCREEN_LINE2, SCREEN_PRIORITY_NONE, (uint8_t *)displayled_mode[music_tab[controler_tab.c_param.c_song].s_displaynote], 0);
		screenTask_add_msg( MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, SCREEN_DISPLAY_NOTE, ((music_tab[controler_tab.c_param.c_song].s_scaletonality + MIDI_DECALAGE_R) | 0x8000));
	}
	else
	{
		//screenTask_add_string(PLAYSCREEN_LINE2, SCREEN_PRIORITY_NONE, (uint8_t *)music_param[2].p_name, 0);
		screenTask_add_string(MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, (uint8_t *)displayled_mode[value], 0);
	}
}

// marks
void displaymarks(param_struct *param, int32_t value, uint32_t rec)
{
	if(value)
		screenTask_add_string(MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, (uint8_t *)displayled_mode[STATIC_LED_MODE], 0);
	else
		screenTask_add_string(MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, (uint8_t *)displayled_mode[NONE_LED_MODE], 0);
}

// fx preset name
void displaypreset(param_struct *param, int32_t value, uint32_t rec)
{
	uint8_t *p_name;

	p_name = getparamtoset(((param_struct *)param->extras)->p_struct, param->p_screen);

	if(rec%2)
	{
#if CONFIG_SCREEN_TASK_ENABLED == 1
		screenTask_add_string(MENUSCREEN_LINE3, SCREEN_PRIORITY_WAKEUP, p_name, 0);
#endif
	}
	else
	{
#if CONFIG_SCREEN_TASK_ENABLED == 1
		screenTask_add_string(PLAYSCREEN_LINE3, SCREEN_PRIORITY_POPUP, p_name, 0);
#endif
	}


}

// tempo
void displaytempo(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t temp;
	uint32_t val_display = value;

	if(rec)
	{
		//if (temp_tempo != 0)
		//{
		//	val_display = ((music_tab[controler_tab.c_param.c_song].s_tempo << 8) & 0xFF00) | (ctrl_val_to_reset[GYRO_P] & 0xFF);
		//}
		//else
		//{
			val_display = (music_tab[controler_tab.c_param.c_song].s_tempo & 0xFF);
		//}
#if CONFIG_SCREEN_TASK_ENABLED == 1
		temp = (SCREEN_PART_3<<(SCREEN_DISPLAY_CMD_NUM + SCREEN_DISPLAY_VALUE_NUM)) | (param->p_screen << SCREEN_DISPLAY_VALUE_NUM) | (val_display);
		screenTask_add_cmd(temp);
#endif
	}
	else
	{
#if CONFIG_SCREEN_TASK_ENABLED == 1
		temp = ((SCREEN_DISPLAY_TURN_ON | SCREEN_DISPLAY_WITH_TIMER_MASK | SCREEN_PART_3)<<(SCREEN_DISPLAY_CMD_NUM + SCREEN_DISPLAY_VALUE_NUM)) | (param->p_screen << SCREEN_DISPLAY_VALUE_NUM) | (val_display);
		screenTask_add_cmd(temp);
#endif
	}
}

void displaypreset_onoff(param_struct *param, int32_t value, uint32_t rec)
{
	if(rec > 2) // coming from displayparam -> key event
	{
		if(param->p_struct == CONTROLER)
		{
			if(param->p_param == CONTROLER_EQ)
			{
				if(value != 0)
				{
					controler_tab.c_param.c_equalizer_onoff = value;
					controler_tab.c_param.c_equalizer_preset = 0;
				}
				else
				{
					if(controler_tab.c_param.c_equalizer_onoff)
						controler_tab.c_param.c_equalizer_preset = controler_tab.c_param.c_equalizer_onoff;
					else
						controler_tab.c_param.c_equalizer_preset = 1;
				}
				setgeneraleq( param, controler_tab.c_param.c_equalizer_preset, rec);
				displaypreset( param, controler_tab.c_param.c_equalizer_preset, rec);
			}
		}
		if(param->p_struct == INSTR_PRESET)
		{
			if(param->p_param == SOUND_PRESET_CHORUS)
			{
				if(value != 0)
				{
					instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_onoff = value;
					instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_preset = 0;
				}
				else
				{
					if(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_onoff)
						instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_preset = instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_onoff;
					else
						instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_preset = 1;
				}
				setinstrchorus( param, instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_preset, rec);
				display_value(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_preset, param->p_screen, rec);
			}
			else if(param->p_param == SOUND_PRESET_SENDTOREVERB)
			{
				if(value != 0)
				{
					instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_reverb_onoff = value;
					instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_sendtorev = 0;
				}
				else
				{
					if(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_reverb_onoff)
						instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_sendtorev = instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_reverb_onoff;
					else
						instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_sendtorev = 1;
				}
				value = instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_sendtorev;
				setinstrsendtorev( param, instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_sendtorev, rec);
				displaypercent( param, instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_sendtorev, rec);
			}
		}
		else if(param->p_struct == MUSIC_INSTR_PRESET)
		{
			if(param->p_param == SOUND_PRESET_CHORUS)
			{
				if(value != 0)
				{
					music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_onoff = value;
					music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_preset = 0;
				}
				else
				{
					if(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_onoff)
						music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_preset = music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_onoff;
					else
						music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_preset = 1;
				}
				setinstrchorus( param, music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_preset, rec);
				display_value(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_preset, param->p_screen, rec);
			}
			else if(param->p_param == SOUND_PRESET_SENDTOREVERB)
			{
				if(value != 0)
				{
					music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_reverb_onoff = value;
					music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_sendtorev = 0;
				}
				else
				{
					if(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_reverb_onoff)
						music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_sendtorev = music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_reverb_onoff;
					else
						music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_sendtorev = 1;
				}
				value = music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_sendtorev;
				setinstrsendtorev( param, music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_sendtorev, rec);
				displaypercent( param, music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_sendtorev, rec);
			}
		}
	}

//	if(param->p_param == SOUND_PRESET_SENDTOREVERB)
//		displaypercent( param, value, rec);
	else if ((param->p_param == SOUND_PRESET_CHORUS)&&(param->p_struct == MUSIC_INSTR_PRESET))
		display_value(music_tab[controler_tab.c_param.c_song].s_track[music_strack()].t_loop[music_sloop()].l_instr.i_preset.s_chorus_preset, param->p_screen, rec);
	else if ((param->p_param == SOUND_PRESET_CHORUS)&&(param->p_struct == INSTR_PRESET))
		display_value(instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_chorus_preset, param->p_screen, rec);
	else
		displaypreset( param, controler_tab.c_param.c_equalizer_preset, rec);
}


// copy and past
void displaycopyandpast(param_struct *param, int32_t value, uint32_t rec)
{
	screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, copyandpast_screenmsg[copyandpastindex], 0);
}

// midi preset name
void displaymiditable(param_struct *param, int32_t value, uint32_t rec)
{
	screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, midiout_mode[controler_tab.c_param.c_midiout_mode], 0);
}

// chromatic / diatoic
void displaychromadiato(param_struct *param, int32_t value, uint32_t rec)
{
	screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, diatonic_mode[value], 0);
}

// chromatic / diatoic
void displayblepower(param_struct *param, int32_t value, uint32_t rec)
{
	screenTask_add_string(MENUSCREEN_LINE3_LOWFONT, SCREEN_PRIORITY_WAKEUP, ble_mode[value], 0);
}
// time circle
void displaytimecircle(param_struct *param, int32_t value, uint32_t rec)
{
	screenTask_add_msg( PLAYSCREEN_FULL, SCREEN_PRIORITY_REFRESH, SCREEN_DISPLAY_TIMECIRCLEPOSITION, value>>1);
}
