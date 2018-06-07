/*
 * instr_mapping.h
 *
 *  Created on: 15/04/2012
 *      Author: mma
 */

#ifndef INSTR_MAPPING_H_
#define INSTR_MAPPING_H_

// includes
#include "g_structs.h"

// define

#ifdef __LPC177X_8X__
#include "lpc_types.h"
#else
#include <stdint.h>
#endif // __LPC177X_8X__

#define FX_GUITARE	0 //1

#define INSTR_DRUM_CHANNEL	8 //9

#define INSTR_DRUM_MAPPINGOFFMIKE_ON3	0 //1

#define INSTR_NEWMAPPING	1 //1

#define NUM_NOTE_MAP_DTS_L		254//32 //8//4
#define NUM_NOTE_MAP_DTPRO		32//32 //8//4

#define DUSOUND_HW_VERSION	0
#define DUSOUND_SW_VERSION	3 //0
#define MAX_MIGRATABLE_DUSOUND_VERSION_BY_DT	2

#define INSTRUMENT_MAPPING_MAX  31

// structure
enum INSTRUMENT_TYPE {
	INSTR_HARMONIC,
	INSTR_PERCU,
	INSTR_SAMPLE,
	NUM_INSTR_TYPE
};

#if 0
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
} info_instr;

#define INSTR_INFO_SIZE (NAME_CARACT + 16 + 36) // 64

#else


#define INSTR_INFO_DUMMY 0//4

typedef struct
{
    uint8_t instr_name[NAME_CARACT];

    uint8_t instr_midi_pc;
    uint8_t instr_midi_C0;
    uint8_t instr_key_map;
    uint8_t instr_octave;

    uint32_t instr_user_id; // =1 pour du-sound Dualo
    uint32_t instr_id;
    uint32_t sample_address;

    uint8_t instr_noteoff;  // 0 or 1

    uint8_t instr_cat[NAME_CARACT];

    uint8_t instr_relvolume;

    uint8_t format_id;
    uint8_t nb_layer;

    uint16_t ip_size;
    uint16_t sp_size;
    uint32_t sample_size;

    uint8_t instr_type; // INSTRUMENT_TYPE
    uint8_t instr_preset; // Gestion First Fit des PRESETs
    uint8_t instr_mapping;  // Gestion First Fit des MAPPINGs
	uint8_t align;

    uint16_t HW_instr_version;
    uint16_t SW_instr_version;

    uint32_t instr_version;
    //uint8_t instr_dummy[INSTR_INFO_DUMMY];
} info_instr;

#define INSTR_INFO_SIZE 64 //(NAME_CARACT*2 + 4*4 + 2*2 + 8*1 + INSTR_INFO_DUMMY)

#define INSTR_PC_OFFSET         NAME_CARACT
#define INSTR_PC_SIZE           1

#define INSTR_KEY_MAP_OFFSET	NAME_CARACT + 2
#define INSTR_KEYMAP        	1

#define INSTR_USER_ID_OFFSET    INSTR_PC_OFFSET + 4
#define INSTR_USER_ID_SIZE      4

#define INSTR_ID_OFFSET         INSTR_USER_ID_OFFSET + 4
#define INSTR_ID_SIZE           4

#define INSTR_FORMAT_ID_OFFSET  INSTR_ID_OFFSET + 2*4 + 1 + NAME_CARACT + 1
#define INSTR_FORMAT_ID_SIZE    1

#define INSTR_TYPE_OFFSET       INSTR_FORMAT_ID_OFFSET + 2 + 2*2 + 4
#define INSTR_TYPE_SIZE         1

#define INSTR_VERSION_OFFSET    INSTR_TYPE_OFFSET + 4 + 2*2
#define INSTR_VERSION_SIZE      4

#endif



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

//#define S_NOTE_SIZE (NAME_CARACT*2 + 4*1)
#define S_NOTE_SIZE (NOTE_NAME_CARACT + NAME_CARACT + 4*1)// = 32
#define MAPPING_L_SIZE (NUM_BUTTON_KEYBOARD_L * 2 * S_NOTE_SIZE)// = 3712
#define MAPPING_S_SIZE (NUM_BUTTON_KEYBOARD_S * 2 * S_NOTE_SIZE)// = 1664
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
