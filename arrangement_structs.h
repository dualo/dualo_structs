/*
 * arrangement_structs.h
 *
 *  Created on: 18/02/2015
 *      Author: mma
 */


#ifndef ARRANGEMENT_STRUCTS_H_
#define ARRANGEMENT_STRUCTS_H_

#include "g_structs.h"


#ifdef __LPC177X_8X__
#include "lpc_types.h"
#else
#include <stdint.h>
#endif // __LPC177X_8X__


#ifdef __cplusplus
#include <QMetaObject>
#include <QtGlobal>

// inline namespaces not handled before MSVC 14.0 (Visual Studio 2015)
#if !defined(_MSC_VER) || _MSC_VER >= 1900
inline
#endif // _MSC_VER
namespace ArrangementStructs {

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
Q_NAMESPACE
#endif // QT_VERSION

#endif // __cplusplus


/***************** arrangement event action **********************/
#define ARRANGEMENT_MAXEVENTACTION	64

typedef struct
{
	uint8_t aea_type;
	uint8_t aea_item;
	uint8_t aea_cmd;
	uint8_t aea_value;
} s_arrangement_event_action;

#define ARRANGEMENT_EVENTACTION_SIZE	4

enum ARRANGEMENT_EVENTACTION {
    ARRANGEMENTEVENTACTION_NONE,
	ARRANGEMENTEVENTACTION_DULOOP,
	ARRANGEMENTEVENTACTION_TEMPO,
	ARRANGEMENTEVENTACTION_SCALE,
	ARRANGEMENTEVENTACTION_CLICVOL,
	ARRANGEMENTEVENTACTION_DISPLAY,
	ARRANGEMENTEVENTACTION_PLAYHEAD,
	ARRANGEMENTEVENTACTION_PLAYSAMPLE,
	ARRANGEMENTEVENTACTION_PARAMETERS,
	ARRANGEMENTEVENTACTION_LOADDUSOUND,
	NUM_ARRANGEMENTEVENTACTION
};

enum
{
	AEA_CMD_STOP,
	AEA_CMD_PLAY,
	AEA_CMD_TEST_STOP,
	AEA_CMD_TEST_PLAY
};

/***************** arrangement exit condition **********************/
typedef struct
{
	uint32_t aec_type;
	uint16_t aec_value_1;
	uint16_t aec_value_2;
} s_arrangement_exit_condition;

#define ARRANGEMENT_EXITCONDITION_SIZE	8

enum ARRANGEMENT_EXITCONDITION {
	ARRANGEMENTEXITCONDITION_NONE, // to do only action and jump to the next event
	ARRANGEMENTEXITCONDITION_TEST,
	ARRANGEMENTEXITCONDITION_REPEATBEAT,
	ARRANGEMENTEXITCONDITION_REPEATLOOP,
	NUM_ARRANGEMENTEXITCONDITION
};

#define DUGAME_SAMPLE_MIDI_CHANNEL 1

/***************** arrangement messages **********************/
#define ARRANGEMENT_MSG_MAXSIZE	32

typedef struct
{
	uint8_t am_line1[ARRANGEMENT_MSG_MAXSIZE];
	uint8_t am_line2[ARRANGEMENT_MSG_MAXSIZE];
	uint8_t am_line3[ARRANGEMENT_MSG_MAXSIZE];
} s_arrangement_msg;

#define ARRANGEMENT_MSG_SIZE	(3*ARRANGEMENT_MSG_MAXSIZE)//96

/***************** arrangement event **********************/

typedef struct
{
	s_arrangement_msg ae_intro_msg;

	uint8_t ae_wait_for_loop_start;

	uint8_t ae_short_dummy[31];
	uint32_t ae_long_dummy[16];

    uint32_t ae_nb_actions;
	s_arrangement_event_action ae_actionlist[ARRANGEMENT_MAXEVENTACTION];

	s_arrangement_exit_condition ae_exit_condition; // num of loop player or result of test or none if we add it to the next event...

    uint8_t ae_led[NUM_LED_VALUE];
} s_arrangement_event;

#define ARRANGEMENT_EVENT_SIZE	((2*ARRANGEMENT_MSG_SIZE) + 4 + (ARRANGEMENT_MAXEVENTACTION*ARRANGEMENT_EVENTACTION_SIZE) + ARRANGEMENT_EXITCONDITION_SIZE + NUM_LED_VALUE) //456

/***************** arrangement struct **********************/
#define ARRANGEMENT_MAXEVENT	256 //64

typedef struct
{
	uint32_t as_grade;
	uint32_t as_numevent;
	uint32_t as_currentevent;
	s_arrangement_event *as_eventlist;
} s_arrangement;

#define ARRANGEMENT_MAXSIZE		(4 + 4 + 4 + (ARRANGEMENT_MAXEVENT*ARRANGEMENT_EVENT_SIZE))
#define ARRANGEMENT_HEADER		(4 + 4 + 4)
/****************** example ***********************************/
/*
s_arrangement_event	arrangement_eventlist[] =
{
		{ // event 1
				{ // intro msg
						"Step 1",
						"It's the start",
						"So let's play"
				},
				{ // exit msg
						"Step 1 Done",
						"Finally",
						"you did it"
				},
				{ // comment msg
						"Step 1 Going",
						"and on...",
						"and on..."
				},
				{ // action list
						{ ARRANGEMENTEVENTACTION_TEMPO, 0, 0, 130}, //action 1
						{ ARRANGEMENTEVENTACTION_DULOOP, 0, 0, REC_PLAY}, //action 2
						{ ARRANGEMENTEVENTACTION_DULOOP, 0, 1, REC_STOP}, //action 3
						{ ARRANGEMENTEVENTACTION_DULOOP, 0, 1, REC_STOP}, //action 4
						{ ARRANGEMENTEVENTACTION_NONE, 0, 0, 0},
				},
				{ ARRANGEMENTEXITCONDITION_REPEAT, 	16}// exit condition
		}
};
*/


#ifdef __cplusplus

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
Q_ENUM_NS(ARRANGEMENT_EVENTACTION)
Q_ENUM_NS(ARRANGEMENT_EXITCONDITION)
#endif // QT_VERSION

} // namespace ArrangementStructs

// inline namespaces not handled before MSVC 14.0 (Visual Studio 2015)
#if defined(_MSC_VER) && _MSC_VER < 1900
using namespace ArrangementStructs;
#endif // _MSC_VER

#endif // __cplusplus

#endif // ARRANGEMENT_STRUCTS_H_
