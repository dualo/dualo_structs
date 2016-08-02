/*
 * midi_parameters_mng.h
 *
 *  Created on: 17/03/2015
 *      Author: mma
 */


#ifndef MIDI_PARAMETERS_MNG_H_
#define MIDI_PARAMETERS_MNG_H_

#include "parameters_mng.h"
#include "g_parameters_mng.h"
#include "controller_middleware/controller_middleware.h"

#define MIDITABLE_MAX	6

enum{
	MIDIOUTMODE_NONE,
	MIDIOUTMODE_SEQ,
	MIDIOUTMODE_CTRL,
	MIDIOUTMODE_NUM
};

////////////////////////////////////////////////////////////
#define MIDICMD_DATASIZE	15

enum {
	MIDICMD_CMD,
	MIDICMD_NOTE,
	MIDICMD_VALUE
};

typedef struct
{
	uint8_t mcmd_size; // if 0 -> MIDI message standard, otherwise send direclty the message of this size
	uint8_t mcmd_data[MIDICMD_DATASIZE];
}struct_midicmd;

#define MIDICMD_STRUCT_SIZE	(MIDICMD_DATASIZE + 1)

//////////////////////////////////////////////////////////////
typedef struct
{
	struct_midicmd mc_noteon;
	struct_midicmd mc_noteoff;
	struct_midicmd mc_dynamic;
	struct_midicmd mc_clic;
	struct_midicmd mc_dclic;
	struct_midicmd mc_presslong;

	uint32_t mc_dynamic_type;
	uint32_t mc_dummy[3];
}struct_midictrl;

#define MIDICTRL_STRUCT_SIZE ((6 * MIDICMD_STRUCT_SIZE) + (4 * 4))

////////////////////////////////////////////////////////////////

#define MIDITABLE_NAMESIZE	64
typedef struct
{
	// header
	uint32_t mt_version;
	uint8_t mt_name[MIDITABLE_NAMESIZE];

	// table
	struct_midictrl mt_keys[2][NUM_BUTTON_KEYBOARD];
	struct_midictrl	mt_controller[CONTROLLER_NUM];

}struct_miditable;

#define MIDITABLE_HEADER_SIZE	(MIDITABLE_NAMESIZE + 4)

#define MIDITABLE_STRUCT_SIZE ( MIDITABLE_HEADER_SIZE + (((2 * NUM_BUTTON_KEYBOARD) + CONTROLLER_NUM) * MIDICTRL_STRUCT_SIZE))


////////////////////////////////////////
void init_miditable(void);
//void erase_miditable( uint32_t instr);
void save_miditable(void);

void miditable_gettable(uint32_t num_table, uint32_t address, uint8_t *buf);
void miditable_clearmemtable(uint32_t num_table);
void miditable_settable(uint32_t num_table, uint32_t address, uint8_t *buf);
#endif // MIDI_PARAMETERS_MNG_H_
