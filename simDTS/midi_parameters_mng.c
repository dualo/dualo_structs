/*
 * midi_parameters_mng.c
 *
 *  Created on: 30/03/2015
 *      Author: mma
 */

// includes
#include "midi_parameters_mng.h"
#include "controler_parameters_mng.h"
#include "g_parameters_mng.h"
#include "yaffsfs.h"
#include "config.h"
#include "layer.h"

uint32_t miditable_maxindex;

extern struct_controler controler_tab;
extern struct_miditable midi_tab[MIDITABLE_MAX];

extern struct_miditable midimap_map_harmonic;
extern struct_miditable midimap_map_percu;
extern struct_miditable midimap_map_sample;

void get_miditablefilename(uint32_t numtable, char *string)
{
	char tmp[3];

	strcpy(string, "/p0/midi/table_");

	if(numtable>9)
		tmp[0] = 48 + (numtable/10);
	else
		tmp[0] = 48;
	tmp[1] = 48 + (numtable%10);
	tmp[2] = 0;

	strcat(string, tmp);
}

/******* midi struct parameters initialisation ************/
void init_miditable(void)
{
	uint32_t table;
	char str[80];
	int32_t midi_file = -1;

	//trace( info, "init midi %d\r\n", instr);


	// static table
	memcpy(&midi_tab[0], &midimap_map_harmonic, MIDITABLE_STRUCT_SIZE);
	memcpy(&midi_tab[1], &midimap_map_percu, MIDITABLE_STRUCT_SIZE);
	memcpy(&midi_tab[2], &midimap_map_sample, MIDITABLE_STRUCT_SIZE);
	miditable_maxindex = 3;


	// dynamic table
	for(table = miditable_maxindex; table <MIDITABLE_MAX; table++)
	{
		get_miditablefilename(table, str);
#ifndef YAFFS_OFF
		midi_file = yaffs_open(str, O_RDWR, 0);
#endif

		if(midi_file == -1)
		{

		}
		else
		{
#ifndef YAFFS_OFF
			yaffs_read(midi_file,(uint8_t *)&midi_tab[miditable_maxindex], MIDITABLE_STRUCT_SIZE);
			yaffs_close(midi_file);
			miditable_maxindex++;
#endif
		}
	}
}

/******* fx struct parameters saving ************/
void save_miditable(void)
{
	uint32_t table;
	char str[80];
	int32_t midi_file = -1;

	//trace( info, "save midi %d\r\n", instr);


	// dynamic table
	for(table = 3; table <miditable_maxindex; table++)
	{
		get_miditablefilename(table, str);
#ifndef YAFFS_OFF
		midi_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#endif
		if(midi_file == -1)
		{

		}
		else
		{
#ifndef YAFFS_OFF
			yaffs_write(midi_file,(uint8_t *)&midi_tab[table], MIDITABLE_STRUCT_SIZE);
			yaffs_close(midi_file);
#endif
		}
	}
}

/********** get song memory ********************/
void miditable_gettable(uint32_t num_table, uint32_t address, uint8_t *buf)
{
	int32_t i;
	uint8_t *temp_table;

	if(num_table>=MIDITABLE_MAX)
		return;

	if(address>=MIDITABLE_STRUCT_SIZE)
		return;

	temp_table = (uint8_t *)&midi_tab[num_table];

	for(i = 0; i<32; i++)
	{
		buf[i] = temp_table[address + i];
	}
}

/********** clear song info ********************/
void miditable_clearmemtable(uint32_t num_table)
{
	char str[80];

	if(num_table>=MIDITABLE_MAX)
		return;

	get_miditablefilename(num_table, str);
#ifndef YAFFS_OFF
	yaffs_unlink(str);
#endif

	init_miditable();

}

/********** write song info ********************/
void miditable_settable(uint32_t num_table, uint32_t address, uint8_t *buf)
{
	int32_t i;
	uint8_t *temp_table;
	char str[80];
	int32_t midi_file = -1;

	if(num_table>=MIDITABLE_MAX)
		return;

	if(address<MIDITABLE_STRUCT_SIZE)
	{

		temp_table = (uint8_t *)&midi_tab[num_table];

		for(i = 0; i<32; i++)
		{
			temp_table[address + i] = buf[i];
		}
	}
	else if(address == 0xFFFFFFFF)
	{
		get_miditablefilename(num_table, str);
#ifndef YAFFS_OFF
		midi_file = yaffs_open(str, O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE);
#endif
		if(midi_file == -1)
		{

		}
		else
		{
#ifndef YAFFS_OFF
			yaffs_write(midi_file,(uint8_t *)&midi_tab[num_table], MIDITABLE_STRUCT_SIZE);
			yaffs_close(midi_file);
#endif
		}

		init_miditable();
	}
}
