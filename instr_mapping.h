/*
 * instr_mapping.h
 *
 *  Created on: 15/04/2012
 *      Author: mma
 */

#ifndef INSTR_MAPPING_H_
#define INSTR_MAPPING_H_

// includes
#include "parameters_mng.h"

// define

#define FX_GUITARE	0 //1

#define INSTR_DRUM_CHANNEL	8 //9

#define INSTR_DRUM_MAPPINGOFFMIKE_ON3	0 //1

#define INSTR_NEWMAPPING	1 //1

#define NUM_NOTE_MAP		4

// structure
typedef struct
{
	uint8_t instr_name[NAME_CARACT];
	uint8_t instr_midi_pc;
	uint8_t instr_midi_C0;
	uint8_t instr_key_map;
	uint8_t instr_octave;
	uint32_t instr_id;
	uint32_t instr_size;
	uint32_t instr_address;
	uint8_t instr_noteoff;
	uint8_t instr_cat[NAME_CARACT];
	uint8_t instr_relvolume;
	uint8_t instr_type;
	uint8_t instr_dummy[21];
} s_instr;

#define INSTR_INFO_SIZE (NAME_CARACT + 16 + 36)


enum INSTRUMENT_TYPE {
	INSTR_HARMONIC,
	INSTR_PERCU,
	INSTR_SAMPLE,
	NUM_INSTR_TYPE
};



#define NOTE_NAME_CARACT    16

typedef struct
{
    uint8_t note_gmref;
	uint8_t note_excl;
	uint8_t note_off;
    uint8_t note_key;
    uint8_t note_name[NOTE_NAME_CARACT];
	uint8_t cat_name[NAME_CARACT];
} s_note;

/*
typedef struct
{
	uint8_t note_key;
	uint8_t note_off;
	uint8_t note_excl;
	uint8_t note_dummy;
	uint8_t note_name[NAME_CARACT];
	uint8_t cat_name[NAME_CARACT];
} s_note;
*/

typedef struct
{
	uint8_t CC0;
	uint8_t CC32;
	uint8_t PC;
	uint8_t instr_name[NAME_CARACT];
} s_soniccell;



#endif //INSTR_MAPPING_H_
