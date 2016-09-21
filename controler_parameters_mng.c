/*
 * controler_parameters_mng.h
 *
 *  Created on: 07/08/2013
 *      Author: mma
 */

// includes
#include "g_parameters_mng.h"
#include "controler_parameters_mng.h"
#include "midi_parameters_mng.h"
#include "midiout_mapping.h"
#include "mem/memory_manager.h"
//#include "norflash.h"
//#include "nand.h"
#include "yaffsfs.h"
//#include "bsp.h"
#include "config.h"
#include "midi_task.h"
#include "screen_task.h"
#include "record_task.h"
#ifdef __LPC18XX__
#include "screen/screen.h"
#else
#ifndef USB_OFF
#include "usb/usbdesc.h"
#endif
#include "screen/DT018ATFT.h"
#endif

#include "main_task.h"
#include "coprocessor/coprocessor.h"
#include "led_interface.h"
#include "dutouch_config.h"
//#include "rf/nrf24l01p.h"
#include "eeprom/eeprom_map.h"

//extern int32_t gyro_direction;
//extern uint32_t gyro_angle;
//extern uint32_t gyro_dead_zone;
//extern uint32_t gyro_activation_zone;

extern struct_controler controler_tab;
extern sound_instr instr_tab[];
extern music_song music_tab[];


extern FX_equalizer	temp_eqmain;
extern FX_reverb	temp_reverb;

__attribute__ ((section(".extflash_controler")))
struct_controler saved_controler_tab;

void migrate_controler(struct_controler *control_struct)
{
	uint32_t version = 0xFFFFFFFF;
#ifndef EEPROM_OFF
	EEPROM_Read8(eeprom_map[EE_INFO_LASTFIRMWARE].addr,(void *)&version, eeprom_map[EE_INFO_LASTFIRMWARE].size);
#endif

	if(version<0x0002000E) // V2.14
	{
		control_struct->c_param.c_accessibility = 0;
		set_equalizerpreset(&(control_struct->c_equalizer[0]), 0); // for getting "off" displayed
	}

	if(version<0x00020017) // V2.23
	{
		control_struct->c_param.c_circle_time = 1;
	}
}

/******* controler struct initialisation ************/
void init_controler(struct_controler *control_struct)
{
	uint32_t i;
	int32_t controler_file = -1;

	//controler_file = yaffs_open("/p0/controler/controler_tab", O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
#ifndef YAFFS_OFF
	controler_file = yaffs_open("/p0/controler/controler_tab", O_RDWR, 0);
#endif

	if(controler_file == -1)
	{
		trace( info, "New controler file\r\n");
		control_struct->c_param.c_mainvolume = MAIN_VOL_DEFAULTVALUE;

		control_struct->c_param.c_equalizer_preset = FX_PEQ_PRESET_DEFAULTVALUE;
		control_struct->c_param.c_equalizer_onoff = 0;

		control_struct->c_param.c_fine_tuning = FINE_TUNING_DEFAULTVALUE;
		control_struct->c_param.c_diatonic = 0;
		control_struct->c_param.c_midi_din = 0;
		control_struct->c_param.c_accessibility = 0;

		//control_struct->c_param.c_keys_curve = KEYS_CURVES_DEFAULTVALUE;
		control_struct->c_param.c_gyro_curve = GYRO_CURVES_DEFAULTVALUE;
		control_struct->c_param.c_sliders_curve = GYRO_CURVES_DEFAULTVALUE;

		control_struct->c_param.c_instr = INSTRUMENT_DEFAULTVALUE;
		control_struct->c_param.c_midiout_mode = MIDIOUTMODE_NONE;
		control_struct->c_param.c_song = SONG_DEFAULTVALUE;

		control_struct->c_param.c_sleep_time = SLEEP_TIME_DEFAULTVALUE;
		control_struct->c_param.c_screen_mode = SCREEN_MODE_DEFAULTVALUE;
		control_struct->c_param.c_luminosity = LUMINOSITY_DEFAULTVALUE;

		control_struct->c_param.c_keys_sensibility = KEY_SENSIBILITY_DEFAULTVALUE;
		control_struct->c_param.c_keys_smoothing = KEY_SMOOTHING_DEFAULTVALUE;
		control_struct->c_param.c_keys_detection = KEY_DETECTION_DEFAULTVALUE;

		control_struct->c_param.c_rf_freq = RF_FREQ_DEFAULTVALUE;

		control_struct->c_param.c_led_display = LED_DISPLAY_DEFAULTVALUE;
		//control_struct->c_param.c_note_display = DEFAULT_KEY_LED_MODE;

		control_struct->c_param.gyro_dead_zone = GYRO_FULL_AXIS_DEFAULT;
		control_struct->c_param.gyro_angle = GYRO_SEND_ANGLE_DEFAULT;
		control_struct->c_param.c_pressure_curve = GYRO_CURVES_DEFAULTVALUE;

		control_struct->c_param.c_circle_time = 0; //1;

		control_struct->c_param.c_midiout_channel = 0;
		control_struct->c_param.c_midiout_keysens = KEYS_CURVES_DEFAULTVALUE;

		for(i = 0; i<FX_NUM_FX_INTR; i++)
		{
			set_equalizergenpreset(&(control_struct->c_equalizer[i]), i);
			set_reverbpreset(&(control_struct->c_reverb[i]), i);
		}

#ifdef DUMMYSATURATION_TEST
		control_struct->c_param.c_gyro_curve = 1;
		control_struct->c_param.c_sliders_curve = 1;
		control_struct->c_param.c_pressure_curve = 1;
		control_struct->c_param.c_led_display = 1;
#endif
		//controler_file = yaffs_open("/p0/controler/controler_tab", O_CREAT | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE);
	}
	else
	{
		trace( info, "Open previous controler file\r\n");
#ifndef YAFFS_OFF
		yaffs_read(controler_file,(uint8_t *)control_struct, CONTROLER_STRUCT_SIZE);
		yaffs_close(controler_file);
#endif

		migrate_controler(control_struct);

		if(control_struct->c_param.c_midiout_channel > 15)
			control_struct->c_param.c_midiout_channel = 0;
	}

	if(control_struct->c_param.c_instr>(2*dt_config.nb_key))
		control_struct->c_param.c_instr = 0;

	if(control_struct->c_param.c_song>(dt_config.nb_key))
		control_struct->c_param.c_song = 0;

	memcpy((uint8_t *)&(temp_eqmain), (uint8_t *)&(control_struct->c_equalizer[control_struct->c_param.c_equalizer_preset]), FX_EQ_SIZE);
	//mem_cpy((uint8_t *)&(control_struct->c_reverb[control_struct->c_param.c_reverb_preset]), (uint8_t *)&(temp_reverb), FX_REVERB_SIZE);

	setdiatonic(NULL, control_struct->c_param.c_diatonic, 0);

#ifdef TEST_KEYBOARD_S
	control_struct->c_param.c_led_display = 1;
#endif

#if CONFIG_FOR_FACTORYTEST == 1
	control_struct->c_param.c_screen_mode = DISPLAY_NOTE_MODE;
#endif

	control_struct->c_param.c_led_display = 1;
}

/******* general fx struct parameters saving ************/
void erase_controler(void)
{
#ifndef YAFFS_OFF
	yaffs_unlink("/p0/controler/controler_tab");
#endif
}

void save_controler(struct_controler *control_struct)
{
#ifndef CONFIG_TEST
#ifndef YAFFS_OFF
	int32_t controler_file, ret;

	controler_file = yaffs_open("/p0/controler/controler_tab", O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);

	if(controler_file < 0)
    {
    	trace( error, "Fail!");
        //while(1);
    }
	else
	{
		yaffs_write(controler_file,(uint8_t *)control_struct,CONTROLER_STRUCT_SIZE);
		ret = yaffs_close(controler_file);

		while(ret < 0)
		{// bad writing
			trace( error, "Controler bad writing\r\n");
			erase_controler();

			controler_file = yaffs_open("/p0/controler/controler_tab", O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
			yaffs_write(controler_file,(uint8_t *)control_struct,CONTROLER_STRUCT_SIZE);
			ret = yaffs_close(controler_file);

		}
	}
#endif
#endif
}


void setmainvolume(param_struct *param, int32_t value, uint32_t rec)
{
#if CONFIG_MIDI_TASK_ENABLED == 1
#if INPUT_GAIN == 0
		set_main_vol(value);
#else
		set_fx_mix(&(instr_tab[controler_tab.c_param.c_mainvolume].s_mix), FX_MIX_INPUTGAIN, 1, 0);
		set_fx_mix(&(instr_tab[controler_tab.c_param.c_mainvolume].s_mix), FX_MIX_INPUTGAIN, 1, MAX_MIDI_CANAL);
#endif

#endif
}

void setaccesibility(param_struct *param, int32_t value, uint32_t rec)
{
	//init channel to not keep last param for accesibility
	uint32_t channel = recordTask_getfreechannel();
	if(channel)
		init_metronome(channel);

}

void setgyro(param_struct *param, int32_t value, uint32_t rec)
{
	if (value == 0) // OFF
	{
#ifndef COPRO_OFF
		coprocessor_set_IMU(0);
#endif
	}
	else// if (value == 1) // ON
	{

		coprocessor_set_IMU(1); //coprocessor_reinit_IMU();
	//	controler_tab.c_param.gyro_direction = -1;
	}
	//else if (value == 2) // INVERT
	//{
	//	coprocessor_set_IMU(1); //coprocessor_reinit_IMU();
	//	controler_tab.c_param.gyro_direction = 1;
//	}
}

void setgyro_x_th(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_X_TH(value);
#endif
}

void setgyro_y_th(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_Y_TH(value);
#endif
}

void setgyro_z_th(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_Z_TH(value);
#endif
}

void setsliders(param_struct *param, int32_t value, uint32_t rec)
{
	if (value == 0) // OFF
	{
		extcontrolTask_enablesliders(0);
	}
	else// if (value == 1) // ON
	{
		extcontrolTask_enablesliders(1);
	}
}

void setgyro_x_acc(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_X_ACC(value);
#endif
}

void setgyro_y_acc(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_Y_ACC(value);
#endif
}

void setgyro_z_acc(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_Z_ACC(value);
#endif
}

void setgyro_P_angle(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_P_angle(value);
#endif
}

void setgyro_Y_angle(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_Y_angle(value);
#endif
}

void setgyro_R_angle(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_R_angle(value);
#endif
}


void setgyro_dead_zone(param_struct *param, int32_t value, uint32_t rec)
{
	//gyro_dead_zone = value;
}

void setgyro_activation_zone(param_struct *param, int32_t value, uint32_t rec)
{
	//gyro_activation_zone = value;
}


void setgyro_full_axis_zone(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_P_angle(value);
	coprocessor_send_R_angle(value);
#endif
}

void setgyro_nb_val_before_send(param_struct *param, int32_t value, uint32_t rec)
{
#ifndef COPRO_OFF
	coprocessor_send_nb_val_BS(value);
#endif
}

void setgyro_activ_time(param_struct *param, int32_t value, uint32_t rec)
{
	//time_to_enabled = value;
}

void setgyro_desactiv_time(param_struct *param, int32_t value, uint32_t rec)
{
	//time_to_disabled = value;
}


void setgyro_angle(param_struct *param, int32_t value, uint32_t rec)
{
	//gyro_angle = value;
}

void setdisplaynote(param_struct *param, int32_t value, uint32_t rec)
{
	if(value)
	{
		if(recordTask_getsongstate(controler_tab.c_param.c_song)!=SONG_CURRENTEMPTY)
			screenTask_add_msg( PLAYSCREEN_FULL, SCREEN_PRIORITY_WAKEUP, SCREEN_DISPLAY_TIMECIRCLE, 1);
	}
}


void setfinetuning(param_struct *param, int32_t value, uint32_t rec)
{
	uint32_t i;

	for(i = 0; i<(2*MAX_MIDI_CANAL);i++)
		set_fine_tuning( i, value);
}

void setdiatonic(param_struct *param, int32_t value, uint32_t rec)
{
	if(value < (NUM_KEY_LAYOUT-1))
	{
		current_key_map = value;
		current_key_offset = DEFAULT_KEY_OFFSET;
	}
	else
	{
		if((music_tab[controler_tab.c_param.c_song].s_displaynote!=STATIC_LED_MODE)&&(music_tab[controler_tab.c_param.c_song].s_displaynote!=NONE_LED_MODE))
		{
			current_key_map = value + (music_tab[controler_tab.c_param.c_song].s_displaynote - 1);
			current_key_offset = music_tab[controler_tab.c_param.c_song].s_scaletonality - 1;
		}
		else
		{
			current_key_map = 0;
			current_key_offset = DEFAULT_KEY_OFFSET;
		}
	}

	if(instr_tab[controler_tab.c_param.c_instr].s_instrument.instr_key_map) // is a drum
	{
		Set_LedPlaySong( instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_leds, instr_tab[controler_tab.c_param.c_instr].s_instrument.instr_key_map, 0, music_tab[controler_tab.c_param.c_song].s_displaynote, instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_displayled, music_tab[controler_tab.c_param.c_song].s_scaletonality);
	}
	else
	{
		Set_LedPlaySong( music_tab[controler_tab.c_param.c_song].s_leds, 0, 0, music_tab[controler_tab.c_param.c_song].s_displaynote, instr_tab[controler_tab.c_param.c_instr].s_preset[instr_tab[controler_tab.c_param.c_instr].s_presetnum].s_displayled, music_tab[controler_tab.c_param.c_song].s_scaletonality);
	}
}

void setmidioutmode(param_struct *param, int32_t value, uint32_t rec)
{
	// stop current music
	recordTask_stopsong(0);
}

void setrffreq(param_struct *param, int32_t value, uint32_t rec)
{
#if ACTIV_RFPLUG == 1
	NRF24L01P_disable();
	NRF24L01P_setRfFrequency(value + NRF24L01P_RF_FREQ_MIN);
	NRF24L01P_enable();
#endif
}

void setrfmode(param_struct *param, int32_t value, uint32_t rec)
{
#ifdef CONFIG_TEST
	uint32_t i;
#ifdef FREE_RTOS
	taskENTER_CRITICAL();
#endif
	if(rf_config == RF_CARRIER)
	{
		NRF24L01P_disable();
		//while(1)
		NRF24L01P_powerDown();

		for(i=0;i<200000;i++)
			asm("nop");

		NRF24L01P_write_register(NRF24L01P_CONFIG, 0x2);

		for(i=0;i<2000000;i++)
			asm("nop");

		NRF24L01P_write_register(NRF24L01P_RF_SETUP, 0x90 | NRF24L01P_RF_PWR_0DBM);

		NRF24L01P_setRfFrequency(controler_tab.c_param.c_rf_freq + NRF24L01P_RF_FREQ_MIN);//(2500);
		for(i=0;i<2000000;i++)
			asm("nop");

		NRF24L01P_powerUp();
		NRF24L01P_enable();

	}
	else if(rf_config == RF_MSG)
	{
		NRF24L01P_disable();
		NRF24L01P_powerDown();

		for(i=0;i<20000;i++)
			asm("nop");


		NRF24L01P_write_register(NRF24L01P_RF_SETUP, NRF24L01P_RF_PWR_0DBM);

		NRF24L01P_write_register(NRF24L01P_CONFIG, 0x70);

		NRF24L01P_disableRxPipes(NRF24L01P_P_ALL);
		NRF24L01P_setRfFrequency(controler_tab.c_param.c_rf_freq + NRF24L01P_RF_FREQ_MIN);
		NRF24L01P_setRfOutputPWR(NRF24L01P_RF_PWR_0DBM);
		NRF24L01P_setAirDataRate(NRF24L01P_DATARATE_250_KBPS);
		NRF24L01P_setCRCWidth(NRF24L01P_CRC_16BITS);
		NRF24L01P_setAddrWidth(0x3);
		uint8_t buf[5] = {0xDE, 0xAD, 0xBE, 0xEF,0xDD};
		NRF24L01P_setTxAddress(buf);
		NRF24L01P_setRxAddress(buf,0);
		NRF24L01P_disableAutoAcknowledge(NRF24L01P_P_ALL);
		NRF24L01P_enableAutoAcknowledge(NRF24L01P_P0);
		NRF24L01P_AutoRetransmit(NRF24L01P_WAIT_500US,0);
		NRF24L01P_setTransferSize(4,0);

		NRF24L01P_powerUp();

		NRF24L01P_enableRxPipes(NRF24L01P_P0);
		NRF24L01P_setRfFrequency(controler_tab.c_param.c_rf_freq + NRF24L01P_RF_FREQ_MIN);

		NRF24L01P_enable();
	}
	else
	{
		NRF24L01P_disable();
		NRF24L01P_powerDown();
	}
#ifdef FREE_RTOS
	taskEXIT_CRITICAL();
#endif
#endif
}
