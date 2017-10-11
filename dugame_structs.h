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
	uint32_t dg_currentevent;
	s_dugame_sound dg_sound[MAX_DUGAME_SOUND];
	s_arrangement_event *dg_eventlist;
} s_dugame;

#define DUGAME_HEADER		(4 + 4 + 4 + (DUGAME_SOUND_STRUCT_SIZE*MAX_DUGAME_SOUND))//52

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

#endif /* DUGAME_STRUCTS_H_ */
