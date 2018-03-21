/*
 * dugame_structs.h
 *
 *  Created on: 5 oct. 2017
 *      Author: charly
 */

#ifndef DUGAME_STRUCTS_H_
#define DUGAME_STRUCTS_H_

#include "arrangement_structs.h"

#ifdef __LPC177X_8X__
#include "lpc_types.h"
#else
#include <stdint.h>
#endif // __LPC177X_8X__


#define DUGAME_SAMPLE_MIDI_CHANNEL 1

typedef struct
{
	uint32_t dg_sound_id;
	uint32_t dg_sound_user_id;
} s_dugame_sound;

#define DUGAME_SOUND_STRUCT_SIZE 	(4+4)//8
#define MAX_DUGAME_SOUND	5

/***************** du-game struct **********************/
typedef struct
{
	uint32_t dg_grade;
	uint32_t dg_numevent;

    uint16_t dg_currentevent;
    uint16_t dg_first_star_event;
    uint16_t dg_second_star_event;
    uint16_t dg_third_star_event;

	uint32_t dg_version;
	uint32_t dg_id;
	uint32_t dummy[3];
	s_dugame_sound dg_sound[MAX_DUGAME_SOUND];
	s_arrangement_event *dg_eventlist;
} s_dugame;

#define DUGAME_HEADER		((9*4) + (DUGAME_SOUND_STRUCT_SIZE*MAX_DUGAME_SOUND))//52


enum DUGAME_MIGRATION_ERROR {
	DUGAME_MIGRATION_SUCCESS = 0,

	DUGAME_MIGRATION_FAIL                = -1,
	DUGAME_MIGRATION_NULL_PTR            = -2,
	DUGAME_MIGRATION_INVALID_VERSION     = -3,
	DUGAME_MIGRATION_TOO_RECENT          = -4,
};



#endif /* DUGAME_STRUCTS_H_ */
