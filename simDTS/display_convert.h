/*
 * display_convert.h
 *
 *  Created on: 18 juin 2014
 *      Author: LAdolphe
 */

#ifndef DISPLAY_CONVERT_H_
#define DISPLAY_CONVERT_H_

#include "parameters_mng.h"
#include "g_parameters_mng.h"

#include "screen_task.h"


void negvalue (int32_t* val);

void displaypercent(param_struct *param, int32_t value, uint32_t rec);
void displaypercent_255(param_struct *param, int32_t value, uint32_t rec);
void displaypercent_230(param_struct *param, int32_t value, uint32_t rec);

// Equalizer
void displaygain(param_struct *param, int32_t value, uint32_t rec);

void displayeq_low_freq(param_struct *param, int32_t value, uint32_t rec);

void displayeq_midl_freq(param_struct *param, int32_t value, uint32_t rec);

void displayeq_midh_freq(param_struct *param, int32_t value, uint32_t rec);

void displayeq_high_freq(param_struct *param, int32_t value, uint32_t rec);


// Reverb
void displayrevtonegain(param_struct *param, int32_t value, uint32_t rec);

void displayrevtime(param_struct *param, int32_t value, uint32_t rec);

void displayrevpredeltime(param_struct *param, int32_t value, uint32_t rec);

// Compressor
void displaycompressorattack(param_struct *param, int32_t value, uint32_t rec);

void displaycompressorrelease(param_struct *param, int32_t value, uint32_t rec);

void displaygain_42(param_struct *param, int32_t value, uint32_t rec);

void displaycompressorratio(param_struct *param, int32_t value, uint32_t rec);

void displaycompressorboost(param_struct *param, int32_t value, uint32_t rec);

// Wah-Wah
void displaywahres(param_struct *param, int32_t value, uint32_t rec);

// Distortion
void display8khzfreq(param_struct *param, int32_t value, uint32_t rec);

// Wah-wah : Frequency = Pedal Position
void display8khzfreqwah(param_struct *param, int32_t value, uint32_t rec);

void displaymixlp(param_struct *param, int32_t value, uint32_t rec);

// Distortion
void displaydrive(param_struct *param, int32_t value, uint32_t rec);

// Chorus
void displaychorusdelay(param_struct *param, int32_t value, uint32_t rec);

void display1200Hz(param_struct *param, int32_t value, uint32_t rec);

void displaychorusrate(param_struct *param, int32_t value, uint32_t rec);

// Delay
void displaydelaytime(param_struct *param, int32_t value, uint32_t rec);

void displaydelaybpm(param_struct *param, int32_t value, uint32_t rec);

// ADSR
void displayADSRattack(param_struct *param, int32_t value, uint32_t rec);

void displayADSRdecay(param_struct *param, int32_t value, uint32_t rec);

void displayADSRrelease(param_struct *param, int32_t value, uint32_t rec);

// Vibrato
void displayratevib(param_struct *param, int32_t value, uint32_t rec);

void displaydepthvib(param_struct *param, int32_t value, uint32_t rec);


// Tone Tuning
void displayhundredthtone(param_struct *param, int32_t value, uint32_t rec);

// Luminosity
void displayluminositypercent(param_struct *param, int32_t value, uint32_t rec);

// Panning
void displaypanning(param_struct *param, int32_t value, uint32_t rec);

// Pitch
void displaypitch(param_struct *param, int32_t value, uint32_t rec);

// Portamento
void displayportamento(param_struct *param, int32_t value, uint32_t rec);

// Octave
void displayoctave(param_struct *param, int32_t value, uint32_t rec);

// Transpose
void displaytranspose(param_struct *param, int32_t value, uint32_t rec);

// Info
void display_info(param_struct *param, int32_t value, uint32_t rec);

// Scale
void displayscale(param_struct *param, int32_t value, uint32_t rec);

// Tempo
void displaytempo(param_struct *param, int32_t value, uint32_t rec);

// marks
void displaymarks(param_struct *param, int32_t value, uint32_t rec);

// fx preset name
void displaypreset(param_struct *param, int32_t value, uint32_t rec);
void displaypreset_onoff(param_struct *param, int32_t value, uint32_t rec);

// copy and past
void displaycopyandpast(param_struct *param, int32_t value, uint32_t rec);

// midi preset name
void displaymiditable(param_struct *param, int32_t value, uint32_t rec);

// chromatic / diatoic
void displaychromadiato(param_struct *param, int32_t value, uint32_t rec);

// time circle
void displaytimecircle(param_struct *param, int32_t value, uint32_t rec);

//carrier BLE
void displayblepower(param_struct *param, int32_t value, uint32_t rec);

#endif /* DISPLAY_CONVERT_H_ */
