/*
 * instr_mapping.c
 *
 *  Created on: 15/04/2012
 *      Author: mma
 */

// includes
#include "instr_mapping.h"
#include "g_parameters_mng.h"
#include "sound_parameters_mng.h"
#ifdef __LPC18XX__
#include "memory_dispach.h"
#endif
//#include "config.h"


#ifdef __LPC177X_8X__
__attribute__ ((section(".extram")))
#else
SECTION_EXTRAM
#endif
//info_instr keyboardL_instr_map[NUM_BUTTON_KEYBOARD];
info_instr *keyboardL_instr_map;

#ifdef __LPC177X_8X__
__attribute__ ((section(".extram")))
#else
SECTION_EXTRAM
#endif
//info_instr keyboardR_instr_map[NUM_BUTTON_KEYBOARD];
info_instr *keyboardR_instr_map;

/*
#ifdef __LPC177X_8X__
//__attribute__ ((section(".intflash")))
__attribute__ ((section(".extram")))
#else
SECTION_INTFLASH
#endif
const s_note keyboard_note_map[NUM_NOTE_MAP][2][NUM_BUTTON_KEYBOARD];

const s_note keyboard_note_mapdefault[2][NUM_BUTTON_KEYBOARD]=
//{
	{ // Map 1: jazz
		{ // Left
			//gmref, excl,	 note off, 	key
            {35, 	0xFF,		0,		35, "Soul Kick   ", "PT"},// 0
            {35, 	0xFF,		0,		36, "Trip Kick   ", "PT"},// 1
            {36, 	0xFF,		0,		37, "Hood Kick   ", "PT"},// 2
            {38, 	0xFF,		0,		38, "Clappppp    ", "PT"},// 3
            {40, 	0xFF,		0,		39, "Pete Snare  ", "PT"},// 4
            {40, 	0xFF,		0,		40, "Pete Snare 2", "PT"},// 5
            {37, 	0xFF,		0,		41, "Vinyle Snare", "PT"},// 6
            {44, 	0xFF,		0,		42, "Ride        ", "PT"},// 7
            {46, 	0xFF,		0,		43, "Cymbals     ", "PT"},// 8
            {42, 	0xFF,		0,		44, "Closed hi-ha", "PT"},// 9
            {41, 	0xFF,		0,		49, "Sub Kick    ", "PL"},// 10
            {43, 	0xFF,		0,		50, "Analog Kick ", "PL"},// 11
            {50, 	0xFF,		0,		51, "Bass Drum   ", "PL"},// 12
            {55, 	0xFF,		0,		52, "Clap        ", "PL"},// 13
            {0xFF, 	0xFF,		0,		53, "Snap        ", "PL"},// 14
            {52, 	0xFF,		0,		54, "Snare       ", "PL"},// 15
            {53, 	0xFF,		0,		55, "Snare 2     ", "PL"},// 16
            {59, 	0xFF,		0,		56, "RimShot     ", "PL"},// 17
            {51, 	58,         0,		57, "Open hi-hat ", "PL"},// 18
            {53, 	57,         0,		58, "Closed hi-ha", "PL"},// 19
            {69, 	0xFF,		0,		45, "Glam Tambour", "TS"},// 20
            {70, 	0xFF,		0,		46, "Deep Tambour", "TS"},// 21
            {81, 	0xFF,		0,		47, "Shaker 1    ", "TS"},// 22
            {80, 	0xFF,		0,		48, "Shaker 2    ", "TS"},// 23
            {35, 	0xFF,		0,		59, "Boom Kick   ", "WS"},// 24
            {36, 	0xFF,		0,		60, "Kick 2      ", "WS"},// 25
            {36, 	0xFF,		0,		61, "Kick 1      ", "WS"},// 26
            {38, 	0xFF,		0,		62, "Clap        ", "WS"},// 27
            {40, 	0xFF,		0,		63, "Snare 2     ", "WS"},// 28
            {40, 	0xFF,		0,		64, "Snare 1     ", "WS"},// 29
            {37, 	0xFF,		0,		65, "Junk clap   ", "WS"},// 30
            {44, 	0xFF,		0,		66, "None        ", "WS"},// 31
            {46, 	68,         0,		67, "Open hi-hat ", "WS"},// 32
            {42, 	67,         0,		68, "Closed hi-ha", "WS"},// 33
            {45, 	0xFF,		0,		73, "Long Kick   ", "DP"},// 34
            {47, 	0xFF,		0,		74, "Kick        ", "DP"},// 35
            {50, 	0xFF,		0,		75, "Sub Kick    ", "DP"},// 36
            {0xFF, 	0xFF,		0,		76, "Junk Snare  ", "DP"},// 37
            {0xFF, 	0xFF,		0,		77, "Clap        ", "DP"},// 38
            {0xFF, 	0xFF,		0,		78, "Snare 2     ", "DP"},// 39
            {0xFF, 	0xFF,		0,		79, "Snare       ", "DP"},// 40
            {0xFF, 	0xFF,		0,		80, "Church Bell ", "DP"},// 41
            {0xFF, 	0xFF,		0,		81, "None        ", "DP"},// 42
            {0xFF, 	0xFF,		0,		82, "Closed hi-ha", "DP"},// 43
            {84, 	0xFF,		0,		69, "Low Tom     ", "TO"},// 44
            {58, 	0xFF,		0,		70, "Mid Low Tom ", "TO"},// 45
            {0xFF, 	72,         0,		71, "Closed Trian", "TO"},// 46
            {82, 	71,         0,		72, "Open Triangl", "TO"},// 47
            {0xFF, 	0xFF,		0,		83, "Lazer Drop  ", "FX"},// 48
            {0xFF, 	0xFF,		0,		84, "Drop        ", "FX"},// 49
            {0xFF, 	0xFF,		0,		85, "Siren Drop  ", "FX"},// 50
            {0xFF, 	0xFF,		0,		86, "Game Over   ", "FX"},// 51
            {0xFF, 	0xFF,		0,		87, "Goofy jump  ", "FX"},// 52
            {0xFF, 	0xFF,		0,		88, "Lazer       ", "FX"},// 53
            {0xFF, 	0xFF,		0,		89, "R2D2        ", "FX"},// 54
            {0xFF, 	0xFF,		0,		90, "Starter     ", "FX"},// 55
            {0xFF, 	0xFF,		0,		91, "Storm       ", "FX"},// 56
            {0xFF, 	0xFF,		0,		92, "White Wobble", "FX"} // 57
        },
		{ // Right
			//PC,	C0,	channel, 	key
            {35, 	0xFF,		0,		35, "Soul Kick   ", "PT"},// 0
            {35, 	0xFF,		0,		36, "Trip Kick   ", "PT"},// 1
            {36, 	0xFF,		0,		37, "Hood Kick   ", "PT"},// 2
            {38, 	0xFF,		0,		38, "Clappppp    ", "PT"},// 3
            {40, 	0xFF,		0,		39, "Pete Snare  ", "PT"},// 4
            {40, 	0xFF,		0,		40, "Pete Snare 2", "PT"},// 5
            {37, 	0xFF,		0,		41, "Vinyle Snare", "PT"},// 6
            {44, 	0xFF,		0,		42, "Ride        ", "PT"},// 7
            {46, 	0xFF,		0,		43, "Cymbals     ", "PT"},// 8
            {42, 	0xFF,		0,		44, "Closed hi-ha", "PT"},// 9
            {69, 	0xFF,		0,		45, "Glam Tambour", "TS"},// 10
            {70, 	0xFF,		0,		46, "Deep Tambour", "TS"},// 11
            {81, 	0xFF,		0,		47, "Shaker 1    ", "TS"},// 12
            {80, 	0xFF,		0,		48, "Shaker 2    ", "TS"},// 13
            {41, 	0xFF,		0,		49, "Sub Kick    ", "PL"},// 14
            {43, 	0xFF,		0,		50, "Analog Kick ", "PL"},// 15
            {50, 	0xFF,		0,		51, "Bass Drum   ", "PL"},// 16
            {55, 	0xFF,		0,		52, "Clap        ", "PL"},// 17
            {0xFF, 	0xFF,		0,		53, "Snap        ", "PL"},// 18
            {52, 	0xFF,		0,		54, "Snare       ", "PL"},// 19
            {53, 	0xFF,		0,		55, "Snare 2     ", "PL"},// 20
            {59, 	0xFF,		0,		56, "RimShot     ", "PL"},// 21
            {51, 	58,         0,		57, "Open hi-hat ", "PL"},// 22
            {53, 	57,         0,		58, "Closed hi-ha", "PL"},// 23
            {35, 	0xFF,		0,		59, "Boom Kick   ", "WS"},// 24
            {36, 	0xFF,		0,		60, "Kick 2      ", "WS"},// 25
            {36, 	0xFF,		0,		61, "Kick 1      ", "WS"},// 26
            {38, 	0xFF,		0,		62, "Clap        ", "WS"},// 27
            {40, 	0xFF,		0,		63, "Snare 2     ", "WS"},// 28
            {40, 	0xFF,		0,		64, "Snare 1     ", "WS"},// 29
            {37, 	0xFF,		0,		65, "Junk clap   ", "WS"},// 30
            {44, 	0xFF,		0,		66, "None        ", "WS"},// 31
            {46, 	68,         0,		67, "Open hi-hat ", "WS"},// 32
            {42, 	67,         0,		68, "Closed hi-ha", "WS"},// 33
            {84, 	0xFF,		0,		69, "Low Tom     ", "TO"},// 34
            {58, 	0xFF,		0,		70, "Mid Low Tom ", "TO"},// 35
            {0xFF, 	72,         0,		71, "Closed Trian", "TO"},// 36
            {82, 	71,         0,		72, "Open Triangl", "TO"},// 37
            {45, 	0xFF,		0,		73, "Long Kick   ", "DP"},// 38
            {47, 	0xFF,		0,		74, "Kick        ", "DP"},// 39
            {50, 	0xFF,		0,		75, "Sub Kick    ", "DP"},// 40
            {0xFF, 	0xFF,		0,		76, "Junk Snare  ", "DP"},// 41
            {0xFF, 	0xFF,		0,		77, "Clap        ", "DP"},// 42
            {0xFF, 	0xFF,		0,		78, "Snare 2     ", "DP"},// 43
            {0xFF, 	0xFF,		0,		79, "Snare       ", "DP"},// 44
            {0xFF, 	0xFF,		0,		80, "Church Bell ", "DP"},// 45
            {0xFF, 	0xFF,		0,		81, "None        ", "DP"},// 46
            {0xFF, 	0xFF,		0,		82, "Closed hi-ha", "DP"},// 47
            {0xFF, 	0xFF,		0,		83, "Lazer Drop  ", "FX"},// 48
            {0xFF, 	0xFF,		0,		84, "Drop        ", "FX"},// 49
            {0xFF, 	0xFF,		0,		85, "Siren Drop  ", "FX"},// 50
            {0xFF, 	0xFF,		0,		86, "Game Over   ", "FX"},// 51
            {0xFF, 	0xFF,		0,		87, "Goofy jump  ", "FX"},// 52
            {0xFF, 	0xFF,		0,		88, "Lazer       ", "FX"},// 53
            {0xFF, 	0xFF,		0,		89, "R2D2        ", "FX"},// 54
            {0xFF, 	0xFF,		0,		90, "Starter     ", "FX"},// 55
            {0xFF, 	0xFF,		0,		91, "Storm       ", "FX"},// 56
            {0xFF, 	0xFF,		0,		92, "White Wobble", "FX"} // 57
		}
	};
	{ // Map 2: rock
		{ // Left
			//PC,	C0,	channel, 	key
            {35, 	0xFF,		0,		35, "Soul Kick   ", "PT"},// 0
            {35, 	0xFF,		0,		36, "Trip Kick   ", "PT"},// 1
            {36, 	0xFF,		0,		37, "Hood Kick   ", "PT"},// 2
            {38, 	0xFF,		0,		38, "Clappppp    ", "PT"},// 3
            {40, 	0xFF,		0,		39, "Pete Snare  ", "PT"},// 4
            {40, 	0xFF,		0,		40, "Pete Snare 2", "PT"},// 5
            {37, 	0xFF,		0,		41, "Vinyle Snare", "PT"},// 6
            {44, 	0xFF,		0,		42, "Ride        ", "PT"},// 7
            {46, 	0xFF,		0,		43, "Cymbals     ", "PT"},// 8
            {42, 	0xFF,		0,		44, "Closed hi-ha", "PT"},// 9
            {41,	0xFF,		0,		49, "Sub Kick    ", "PL"},// 10
            {45,	0xFF,		0,		50, "Analog Kick ", "PL"},// 11
            {47, 	0xFF,		0,		51, "Bass Drum   ", "PL"},// 12
            {49, 	0xFF,		0,		52, "Clap        ", "PL"},// 13
            {57, 	0xFF,		0,		53, "Snap        ", "PL"},// 14
            {55, 	0xFF,		0,		54, "Snare       ", "PL"},// 15
            {51, 	0xFF,		0,		55, "Snare 2     ", "PL"},// 16
            {77, 	0xFF,		0,		56, "RimShot     ", "PL"},// 17
            {76, 	58,         0,		57, "Open hi-hat ", "PL"},// 18
            {34, 	57,         0,		58, "Closed hi-ha", "PL"},// 19
            {69, 	0xFF,		0,		45, "Glam Tambour", "TS"},// 20
            {70, 	0xFF,		0,		46, "Deep Tambour", "TS"},// 21
            {81, 	0xFF,		0,		47, "Shaker 1    ", "TS"},// 22
            {80, 	0xFF,		0,		48, "Shaker 2    ", "TS"},// 23
            {35, 	0xFF,		0,		59, "Boom Kick   ", "WS"},// 24
            {36, 	0xFF,		0,		60, "Kick 2      ", "WS"},// 25
            {35, 	0xFF,		0,		61, "Kick 1      ", "WS"},// 26
            {38, 	0xFF,		0,		62, "Clap        ", "WS"},// 27
            {40, 	0xFF,		0,		63, "Snare 2     ", "WS"},// 28
            {40, 	0xFF,		0,		64, "Snare 1     ", "WS"},// 29
            {37, 	0xFF,		0,		65, "Junk clap   ", "WS"},// 30
            {42, 	0xFF,		0,		66, "None        ", "WS"},// 31
            {46, 	68,         0,		67, "Open hi-hat ", "WS"},// 32
            {44, 	67,         0,		68, "Closed hi-ha", "WS"},// 33
            {41, 	0xFF,		0,		73, "Long Kick   ", "DP"},// 34
            {43, 	0xFF,		0,		74, "Kick        ", "DP"},// 35
            {47, 	0xFF,		0,		75, "Sub Kick    ", "DP"},// 36
            {55, 	0xFF,		0,		76, "Junk Snare  ", "DP"},// 37
            {52, 	0xFF,		0,		77, "Clap        ", "DP"},// 38
            {57, 	0xFF,		0,		78, "Snare 2     ", "DP"},// 39
            {49, 	0xFF,		0,		79, "Snare       ", "DP"},// 40
            {53, 	0xFF,		0,		80, "Church Bell ", "DP"},// 41
            {52, 	0xFF,		0,		81, "None        ", "DP"},// 42
            {57, 	0xFF,		0,		82, "Closed hi-ha", "DP"},// 43
            {84, 	0xFF,		0,		69, "Low Tom     ", "TO"},// 44
            {58, 	0xFF,		0,		70, "Mid Low Tom ", "TO"},// 45
            {75, 	72,         0,		71, "Closed Trian", "TO"},// 46
            {82, 	71,         0,		72, "Open Triangl", "TO"},// 47
            {0xFF, 	0xFF,		0,		83, "Lazer Drop  ", "FX"},// 48
            {0xFF, 	0xFF,		0,		84, "Drop        ", "FX"},// 49
            {0xFF, 	0xFF,		0,		85, "Siren Drop  ", "FX"},// 50
            {0xFF, 	0xFF,		0,		86, "Game Over   ", "FX"},// 51
            {0xFF, 	0xFF,		0,		87, "Goofy jump  ", "FX"},// 52
            {0xFF, 	0xFF,		0,		88, "Lazer       ", "FX"},// 53
            {0xFF, 	0xFF,		0,		89, "R2D2        ", "FX"},// 54
            {0xFF, 	0xFF,		0,		90, "Starter     ", "FX"},// 55
            {0xFF, 	0xFF,		0,		91, "Storm       ", "FX"},// 56
            {0xFF, 	0xFF,		0,		92, "White Wobble", "FX"} // 57
        },
        { // Right
			//PC,	C0,	channel, 	key
            {35, 	0xFF,		0,		35, "Soul Kick   ", "PT"},// 0
            {35, 	0xFF,		0,		36, "Trip Kick   ", "PT"},// 1
            {36, 	0xFF,		0,		37, "Hood Kick   ", "PT"},// 2
            {38, 	0xFF,		0,		38, "Clappppp    ", "PT"},// 3
            {40, 	0xFF,		0,		39, "Pete Snare  ", "PT"},// 4
            {40, 	0xFF,		0,		40, "Pete Snare 2", "PT"},// 5
            {37, 	0xFF,		0,		41, "Vinyle Snare", "PT"},// 6
            {44, 	0xFF,		0,		42, "Ride        ", "PT"},// 7
            {46, 	0xFF,		0,		43, "Cymbals     ", "PT"},// 8
            {42, 	0xFF,		0,		44, "Closed hi-ha", "PT"},// 9
            {69, 	0xFF,		0,		45, "Glam Tambour", "TS"},// 10
            {70, 	0xFF,		0,		46, "Deep Tambour", "TS"},// 11
            {81, 	0xFF,		0,		47, "Shaker 1    ", "TS"},// 12
            {80, 	0xFF,		0,		48, "Shaker 2    ", "TS"},// 13
            {41,	0xFF,		0,		49, "Sub Kick    ", "PL"},// 14
            {45,	0xFF,		0,		50, "Analog Kick ", "PL"},// 15
            {47, 	0xFF,		0,		51, "Bass Drum   ", "PL"},// 16
            {49, 	0xFF,		0,		52, "Clap        ", "PL"},// 17
            {57, 	0xFF,		0,		53, "Snap        ", "PL"},// 18
            {55, 	0xFF,		0,		54, "Snare       ", "PL"},// 19
            {51, 	0xFF,		0,		55, "Snare 2     ", "PL"},// 20
            {77, 	0xFF,		0,		56, "RimShot     ", "PL"},// 21
            {76, 	58,         0,		57, "Open hi-hat ", "PL"},// 22
            {34, 	57,         0,		58, "Closed hi-ha", "PL"},// 23
            {35, 	0xFF,		0,		59, "Boom Kick   ", "WS"},// 24
            {36, 	0xFF,		0,		60, "Kick 2      ", "WS"},// 25
            {35, 	0xFF,		0,		61, "Kick 1      ", "WS"},// 26
            {38, 	0xFF,		0,		62, "Clap        ", "WS"},// 27
            {40, 	0xFF,		0,		63, "Snare 2     ", "WS"},// 28
            {40, 	0xFF,		0,		64, "Snare 1     ", "WS"},// 29
            {37, 	0xFF,		0,		65, "Junk clap   ", "WS"},// 30
            {42, 	0xFF,		0,		66, "None        ", "WS"},// 31
            {46, 	68,         0,		67, "Open hi-hat ", "WS"},// 32
            {44, 	67,         0,		68, "Closed hi-ha", "WS"},// 33
            {84, 	0xFF,		0,		69, "Low Tom     ", "TO"},// 34
            {58, 	0xFF,		0,		70, "Mid Low Tom ", "TO"},// 35
            {75, 	72,         0,		71, "Closed Trian", "TO"},// 36
            {82, 	71,         0,		72, "Open Triangl", "TO"},// 37
            {41, 	0xFF,		0,		73, "Long Kick   ", "DP"},// 38
            {43, 	0xFF,		0,		74, "Kick        ", "DP"},// 39
            {47, 	0xFF,		0,		75, "Sub Kick    ", "DP"},// 40
            {55, 	0xFF,		0,		76, "Junk Snare  ", "DP"},// 41
            {52, 	0xFF,		0,		77, "Clap        ", "DP"},// 42
            {57, 	0xFF,		0,		78, "Snare 2     ", "DP"},// 43
            {49, 	0xFF,		0,		79, "Snare       ", "DP"},// 44
            {53, 	0xFF,		0,		80, "Church Bell ", "DP"},// 45
            {52, 	0xFF,		0,		81, "None        ", "DP"},// 46
            {57, 	0xFF,		0,		82, "Closed hi-ha", "DP"},// 47
            {0xFF, 	0xFF,		0,		83, "Lazer Drop  ", "FX"},// 48
            {0xFF, 	0xFF,		0,		84, "Drop        ", "FX"},// 49
            {0xFF, 	0xFF,		0,		85, "Siren Drop  ", "FX"},// 50
            {0xFF, 	0xFF,		0,		86, "Game Over   ", "FX"},// 51
            {0xFF, 	0xFF,		0,		87, "Goofy jump  ", "FX"},// 52
            {0xFF, 	0xFF,		0,		88, "Lazer       ", "FX"},// 53
            {0xFF, 	0xFF,		0,		89, "R2D2        ", "FX"},// 54
            {0xFF, 	0xFF,		0,		90, "Starter     ", "FX"},// 55
            {0xFF, 	0xFF,		0,		91, "Storm       ", "FX"},// 56
            {0xFF, 	0xFF,		0,		92, "White Wobble", "FX"} // 57
        }
    },
	{ // Map 3: electro
        { // Left
			//PC,	C0,	channel, 	key
            {36, 	0xFF,		0,		35, "Soul Kick", 	"Pete"},// 0
            {35, 	0xFF,		0,		36, "Trip Kick", 	"Pete"},// 1
            {35, 	0xFF,		0,		37, "Hood Kick", 	"Pete"},// 2
            {28, 	0xFF,		0,		38, "Clappppp",	 	"Pete"},// 3
            {38, 	0xFF,		0,		39, "Pete Snare",	"Pete"},// 4
            {40, 	0xFF,		0,		40, "Pete Snare 2", "Pete"},// 5
            {39, 	0xFF,		0,		41, "Vinyle Snare", "Pete"},// 6
            {55, 	0xFF,		0,		42, "Ride", 		"Pete"},// 7
            {46, 	0xFF,		0,		43, "Cymbals", 		"Pete"},// 8
            {42, 	0xFF,		0,		44, "Closed hi-ha", "Pete"},// 9
            {0xFF,	0xFF,		0,		49, "Sub Kick", 	"Pluton"},// 10
            {0xFF,	0xFF,		0,		50, "Analog Kick", 	"Pluton"},// 11
            {0xFF, 	0xFF,		0,		51, "Bass Drum", 	"Pluton"},// 12
            {39, 	0xFF,		0,		52, "Clap", 		"Pluton"},// 13
            {28, 	0xFF,		0,		53, "Snap",         "Pluton"},// 14
            {38, 	0xFF,		0,		54, "Snare", 		"Pluton"},// 15
            {40, 	0xFF,		0,		55, "Snare 2", 		"Pluton"},// 16
            {37, 	0xFF,		0,		56, "RimShot", 		"Pluton"},// 17
            {46, 	58,         0,		57, "Open hi-hat", 	"Pluton"},// 18
            {42, 	57,         0,		58, "Closed hi-ha", "Pluton"},// 19
            {54, 	0xFF,		0,		45, "Glam Tambour", "Percu"},// 20
            {54, 	0xFF,		0,		46, "Deep Tambour", "Percu"},// 21
            {82, 	0xFF,		0,		47, "Shaker 1", 	"Percu"},// 22
            {82, 	0xFF,		0,		48, "Shaker 2", 	"Percu"},// 23
            {35, 	0xFF,		0,		59, "Boom Kick", 	"WestSide"},// 24
            {36, 	0xFF,		0,		60, "Kick 2", 		"WestSide"},// 25
            {36, 	0xFF,		0,		61, "Kick 1", 		"WestSide"},// 26
            {40, 	0xFF,		0,		62, "Clap", 		"WestSide"},// 27
            {39, 	0xFF,		0,		63, "Snare 2", 		"WestSide"},// 28
            {38, 	0xFF,		0,		64, "Snare 1", 		"WestSide"},// 29
            {28, 	0xFF,		0,		65, "Junk clap", 	"WestSide"},// 30
            {46, 	0xFF,		0,		66, "None", 		"WestSide"},// 31
            {44, 	68,         0,		67, "Open hi-hat", 	"WestSide"},// 32
            {42, 	67,         0,		68, "Closed hi-ha", "WestSide"},// 33
            {35, 	0xFF,		0,		73, "Long Kick", 	"Deep"},// 34
            {36, 	0xFF,		0,		74, "Kick", 		"Deep"},// 35
            {36, 	0xFF,		1,		75, "Sub Kick", 	"Deep"},// 36
            {28, 	0xFF,		0,		76, "Junk Snare", 	"Deep"},// 37
            {39, 	0xFF,		0,		77, "Clap", 		"Deep"},// 38
            {40, 	0xFF,		0,		78, "Snare 2", 		"Deep"},// 39
            {38, 	0xFF,		0,		79, "Snare", 		"Deep"},// 40
            {0xFF, 	0xFF,		0,		80, "Church Bell", 	"Deep"},// 41
            {46, 	0xFF,		0,		81, "None", 		"Deep"},// 42
            {42, 	0xFF,		0,		82, "Closed hi-ha", "Deep"},// 43
            {43, 	0xFF,		0,		69, "Low Tom", 		"Percu"},// 44
            {45, 	0xFF,		0,		70, "Mid Low Tom", 	"Percu"},// 45
            {81, 	72,         0,		71, "Closed Trian", "Percu"},// 46
            {80, 	71,         0,		72, "Open Triangl", "Percu"},// 47
            {0xFF, 	0xFF,		1,		83, "Lazer Drop", 	"SFX"},// 48
            {0xFF, 	0xFF,		1,		84, "Drop", 		"SFX"},// 49
            {0xFF, 	0xFF,		1,		85, "Siren Drop", 	"SFX"},// 50
            {0xFF, 	0xFF,		1,		86, "Game Over", 	"SFX"},// 51
            {0xFF, 	0xFF,		1,		87, "Goofy jump", 	"SFX"},// 52
            {0xFF, 	0xFF,		1,		88, "Lazer", 		"SFX"},// 53
            {0xFF, 	0xFF,		1,		89, "R2D2", 		"SFX"},// 54
            {0xFF, 	0xFF,		1,		90, "Starter", 		"SFX"},// 55
            {0xFF, 	0xFF,		1,		91, "Storm", 		"SFX"},// 56
            {0xFF, 	0xFF,		1,		92, "White Wobble", "SFX"} // 57
		},
		{ // Right
			//PC,	C0,	channel, 	key
            {36, 	0xFF,		0,		35, "Soul Kick", 	"Pete"},// 0
            {35, 	0xFF,		0,		36, "Trip Kick", 	"Pete"},// 1
            {35, 	0xFF,		0,		37, "Hood Kick", 	"Pete"},// 2
            {28, 	0xFF,		0,		38, "Clappppp", 	"Pete"},// 3
            {38, 	0xFF,		0,		39, "Pete Snare", 	"Pete"},// 4
            {40, 	0xFF,		0,		40, "Pete Snare 2", "Pete"},// 5
            {39, 	0xFF,		0,		41, "Vinyle Snare", "Pete"},// 6
            {55, 	0xFF,		0,		42, "Ride", 		"Pete"},// 7
            {46, 	0xFF,		0,		43, "Cymbals", 		"Pete"},// 8
            {42, 	0xFF,		0,		44, "Closed hi-ha", "Pete"},// 9
            {54, 	0xFF,		0,		45, "Glam Tambour", "Percu"},// 10
            {54, 	0xFF,		0,		46, "Deep Tambour", "Percu"},// 11
            {82, 	0xFF,		0,		47, "Shaker 1", 	"Percu"},// 12
            {82, 	0xFF,		0,		48, "Shaker 2", 	"Percu"},// 13
            {0xFF,	0xFF,		0,		49, "Sub Kick", 	"Pluton"},// 14
            {0xFF,	0xFF,		0,		50, "Analog Kick", 	"Pluton"},// 15
            {0xFF, 	0xFF,		0,		51, "Bass Drum", 	"Pluton"},// 16
            {39, 	0xFF,		0,		52, "Clap", 		"Pluton"},// 17
            {28, 	0xFF,		0,		53, "Snap", 		"Pluton"},// 18
            {38, 	0xFF,		0,		54, "Snare", 		"Pluton"},// 19
            {40, 	0xFF,		0,		55, "Snare 2", 		"Pluton"},// 20
            {37, 	0xFF,		0,		56, "RimShot", 		"Pluton"},// 21
            {46, 	58,         0,		57, "Open hi-hat", 	"Pluton"},// 22
            {42, 	57,         0,		58, "Closed hi-ha", "Pluton"},// 23
            {35, 	0xFF,		0,		59, "Boom Kick", 	"WestSide"},// 24
            {36, 	0xFF,		0,		60, "Kick 2", 		"WestSide"},// 25
            {36, 	0xFF,		0,		61, "Kick 1", 		"WestSide"},// 26
            {40, 	0xFF,		0,		62, "Clap", 		"WestSide"},// 27
            {39, 	0xFF,		0,		63, "Snare 2", 		"WestSide"},// 28
            {38, 	0xFF,		0,		64, "Snare 1", 		"WestSide"},// 29
            {28, 	0xFF,		0,		65, "Junk clap", 	"WestSide"},// 30
            {46, 	0xFF,		0,		66, "None", 		"WestSide"},// 31
            {44, 	68,         0,		67, "Open hi-hat", 	"WestSide"},// 32
            {42, 	67,         0,		68, "Closed hi-ha", "WestSide"},// 33
            {43, 	0xFF,		0,		69, "Low Tom", 		"Percu"},// 34
            {45, 	0xFF,		0,		70, "Mid Low Tom", 	"Percu"},// 35
            {81, 	72,         0,		71, "Closed Trian", "Percu"},// 36
            {80, 	71,         0,		72, "Open Triangl", "Percu"},// 37
            {35, 	0xFF,		0,		73, "Long Kick", 	"Deep"},// 38
            {36, 	0xFF,		0,		74, "Kick", 		"Deep"},// 39
            {36, 	0xFF,		1,		75, "Sub Kick", 	"Deep"},// 40
            {28, 	0xFF,		0,		76, "Junk Snare", 	"Deep"},// 41
            {39, 	0xFF,		0,		77, "Clap", 		"Deep"},// 42
            {40, 	0xFF,		0,		78, "Snare 2", 		"Deep"},// 43
            {38, 	0xFF,		0,		79, "Snare", 		"Deep"},// 44
            {0xFF, 	0xFF,		0,		80, "Church Bell", 	"Deep"},// 45
            {46, 	0xFF,		0,		81, "None", 		"Deep"},// 46
            {42, 	0xFF,		0,		82, "Closed hi-ha", "Deep"},// 47
            {0xFF, 	0xFF,		1,		83, "Lazer Drop", 	"SFX"},// 48
            {0xFF, 	0xFF,		1,		84, "Drop", 		"SFX"},// 49
            {0xFF, 	0xFF,		1,		85, "Siren Drop", 	"SFX"},// 50
            {0xFF, 	0xFF,		1,		86, "Game Over", 	"SFX"},// 51
            {0xFF, 	0xFF,		1,		87, "Goofy jump", 	"SFX"},// 52
            {0xFF, 	0xFF,		1,		88, "Lazer", 		"SFX"},// 53
            {0xFF, 	0xFF,		1,		89, "R2D2", 		"SFX"},// 54
            {0xFF, 	0xFF,		1,		90, "Starter", 		"SFX"},// 55
            {0xFF, 	0xFF,		1,		91, "Storm", 		"SFX"},// 56
            {0xFF, 	0xFF,		1,		92, "White Wobble", "SFX"} // 57
		}
	},
	{ // Map 4: percu
		{ // Left
			//PC,	C0,	channel, 	key
            {66, 	0xFF,		0,		35, "Timbales 1", 	"Classical"},// 0
            {66, 	0xFF,		0,		36, "Timbales 2", 	"Classical"},// 1
            {65, 	0xFF,		0,		37, "Castagnet", 	"Classical"},// 2
            {52, 	0xFF,		0,		38, "Timbales rs", 	"Classical"},// 3
            {52, 	0xFF,		0,		39, "Bell", 		"Classical"},// 4
            {38, 	0xFF,		1,		40, "Ride 1", 		"Classical"},// 5
            {85, 	0xFF,		1,		41, "Ride 2", 		"Classical"},// 6
            {81, 	0xFF,		0,		42, "Triangle op.", "Classical"},// 7
            {80, 	0xFF,		0,		43, "Triangle cl.", "Classical"},// 8
            {56, 	0xFF,		0,		44, "Sifflet", 		"Classical"},// 9
            {64, 	0xFF,		0,		49, "Conga slap", 	"Latina"},// 10
            {63, 	0xFF,		0,		50, "Conga low", 	"Latina"},// 11
            {62, 	0xFF,		0,		51, "Conga hi", 	"Latina"},// 12
            {0xFF, 	0xFF,		0,		52, "Cajon muffl.", "Latina"},// 13
            {0xFF, 	0xFF,		0,		53, "Cajon open", 	"Latina"},// 14
            {61, 	0xFF,		0,		54, "Bongo low", 	"Latina"},// 15
            {60, 	0xFF,		0,		55, "Bongo hi", 	"Latina"},// 16
            {82, 	0xFF,		0,		56, "Shaker 1", 	"Latina"},// 17
            {82, 	0xFF,		0,		57, "Shaker 2", 	"Latina"},// 18
            {69, 	0xFF,		0,		58, "Casaba", 		"Latina"},// 19
            {0xFF, 	0xFF,		0,		45, "Chimes", 		"Misc."},// 20
            {58, 	0xFF,		0,		46, "Vibraslap",	"Misc."},// 21
            {0xFF, 	0xFF,		0,		47, "Fouet", 		"Misc."},// 22
            {0xFF, 	0xFF,		0,		48, "Vibratone",	"Misc."},// 23
            {87, 	0xFF,		0,		59, "Surdo Muffl.", "Brazil"},// 24
            {0xFF, 	0xFF,		0,		60, "Repinique", 	"Brazil"},// 25
            {54, 	0xFF,		0,		61, "Tambourine", 	"Brazil"},// 26
            {79, 	0xFF,		0,		62, "Caixa", 		"Brazil"},// 27
            {78, 	0xFF,		0,		63, "Cuica low", 	"Brazil"},// 28
            {0xFF, 	0xFF,		0,		64, "Cuica hi", 	"Brazil"},// 29
            {0xFF, 	0xFF,		0,		65, "Berimbau 1", 	"Brazil"},// 30
            {40, 	0xFF,		0,		66, "Berimbau 2", 	"Brazil"},// 31
            {72, 	0xFF,		0,		67, "Whistle sh.", 	"Brazil"},// 32
            {71, 	0xFF,		0,		68, "Whistle long", "Brazil"},// 33
            {0xFF, 	0xFF,		0,		73, "Udu low", 		"Africa"},// 34
            {0xFF, 	0xFF,		0,		74, "Udu hi", 		"Africa"},// 35
            {0xFF, 	0xFF,		0,		75, "Udu slap", 	"Africa"},// 36
            {0xFF, 	0xFF,		0,		76, "Dum Dum low", 	"Africa"},// 37
            {0xFF, 	0xFF,		0,		77, "Dum Dum hi", 	"Africa"},// 38
            {0xFF, 	0xFF,		0,		78, "Djembe", 		"Africa"},// 39
            {0xFF, 	0xFF,		0,		79, "Djembe slap", 	"Africa"},// 40
            {68, 	0xFF,		0,		80, "Agogo low", 	"Africa"},// 41
            {67, 	0xFF,		0,		81, "Agogo hi", 	"Africa"},// 42
            {70, 	0xFF,		0,		82, "Maracas", 		"Africa"},// 43
            {74, 	0xFF,		0,		69, "Guiro long", 	"Wood"},// 44
            {73, 	0xFF,		0,		70, "Guiro short", 	"Wood"},// 45
            {75, 	0xFF,		0,		71, "Clave", 		"Wood"},// 46
            {76, 	0xFF,		0,		72, "Wood block", 	"Wood"},// 47
            {0xFF, 	0xFF,		1,		83, "Gong", 		"Oriental"},// 48
            {0xFF, 	0xFF,		0,		84, "Flexatone", 	"Oriental"},// 49
            {0xFF, 	0xFF,		0,		85, "Karkabou", 	"Oriental"},// 50
            {0xFF, 	0xFF,		0,		86, "Tabla 1", 		"Oriental"},// 51
            {0xFF, 	0xFF,		0,		87, "Tabla 2", 		"Oriental"},// 52
            {0xFF, 	0xFF,		0,		88, "Tabla slap", 	"Oriental"},// 53
            {0xFF, 	0xFF,		0,		89, "Table rs", 	"Oriental"},// 54
            {0xFF, 	0xFF,		0,		90, "Bendir", 		"Oriental"},// 55
            {0xFF, 	0xFF,		0,		91, "Derbouka", 	"Oriental"},// 56
            {0xFF, 	0xFF,		0,		92, "None", 		"Oriental"} // 57
                },
                { // Right
                        //PC,	C0,	channel, 	key
            {66,   	0xFF,		0,		35, "Timbales 1", 	"Classical"},// 0
            {66,   	0xFF,		0,		36, "Timbales 2", 	"Classical"},// 1
            {65, 	0xFF,		0,		37, "Timbales rs", 	"Classical"},// 2
            {52,   	0xFF,		0,		38, "Castagnet", 	"Classical"},// 3
            {52,   	0xFF,		0,		39, "Bell", 		"Classical"},// 4
            {38,   	0xFF,		1,		40, "Ride 1", 		"Classical"},// 5
            {85,    0xFF,		1,		41, "Ride 2", 		"Classical"},// 6
            {81,   	0xFF,		0,		42, "Triangle op.", "Classical"},// 7
            {80,   	0xFF,		0,		43, "Triangle cl.", "Classical"},// 8
            {56, 	0xFF,		0,		44, "Sifflet", 		"Classical"},// 9
            {0xFF, 	0xFF,		0,		45, "Chimes", 		"Misc."},// 10
            {58, 	0xFF,		0,		46, "Vibraslap",	"Misc."},// 11
            {0xFF, 	0xFF,		0,		47, "Fouet", 		"Misc."},// 12
            {0xFF, 	0xFF,		0,		48, "Vibratone",	"Misc."},// 13
            {64, 	0xFF,		0,		49, "Conga slap", 	"Latina"},// 14
            {63, 	0xFF,		0,		50, "Conga low", 	"Latina"},// 15
            {62, 	0xFF,		0,		51, "Conga hi", 	"Latina"},// 16
            {0xFF, 	0xFF,		0,		52, "Cajon muffl.", "Latina"},// 17
            {0xFF, 	0xFF,		0,		53, "Cajon open", 	"Latina"},// 18
            {61, 	0xFF,		0,		54, "Bongo low", 	"Latina"},// 19
            {60, 	0xFF,		0,		55, "Bongo hi", 	"Latina"},// 20
            {82, 	0xFF,		0,		56, "Shaker 1", 	"Latina"},// 21
            {82, 	0xFF,		0,		57, "Shaker 2", 	"Latina"},// 22
            {69, 	0xFF,		0,		58, "Casaba", 		"Latina"},// 23
            {87, 	0xFF,		0,		59, "Surdo Muffl.", "Brazil"},// 24
            {0xFF, 	0xFF,		0,		60, "Repinique", 	"Brazil"},// 25
            {54, 	0xFF,		0,		61, "Tambourine", 	"Brazil"},// 26
            {79, 	0xFF,		0,		62, "Caixa", 		"Brazil"},// 27
            {78, 	0xFF,		0,		63, "Cuica low", 	"Brazil"},// 28
            {0xFF, 	0xFF,		0,		64, "Cuica hi", 	"Brazil"},// 29
            {0xFF, 	0xFF,		0,		65, "Berimbau 1", 	"Brazil"},// 30
            {40, 	0xFF,		0,		66, "Berimbau 2", 	"Brazil"},// 31
            {72, 	0xFF,		0,		67, "Whistle sh.", 	"Brazil"},// 32
            {71, 	0xFF,		0,		68, "Whistle long", "Brazil"},// 33
            {74, 	0xFF,		0,		69, "Guiro long", 	"Wood"},// 34
            {73, 	0xFF,		0,		70, "Guiro short", 	"Wood"},// 35
            {75, 	0xFF,		0,		71, "Clave", 		"Wood"},// 36
            {76, 	0xFF,		0,		72, "Wood block", 	"Wood"},// 37
            {0xFF, 	0xFF,		0,		73, "Udu low", 		"Africa"},// 38
            {0xFF, 	0xFF,		0,		74, "Udu hi", 		"Africa"},// 39
            {0xFF, 	0xFF,		0,		75, "Udu slap", 	"Africa"},// 40
            {0xFF, 	0xFF,		0,		76, "Dum Dum low", 	"Africa"},// 41
            {0xFF, 	0xFF,		0,		77, "Dum Dum hi", 	"Africa"},// 42
            {0xFF, 	0xFF,		0,		78, "Djembe", 		"Africa"},// 43
            {0xFF, 	0xFF,		0,		79, "Djembe slap", 	"Africa"},// 44
            {68, 	0xFF,		0,		80, "Agogo low", 	"Africa"},// 45
            {67, 	0xFF,		0,		81, "Agogo hi", 	"Africa"},// 46
            {70, 	0xFF,		0,		82, "Maracas", 		"Africa"},// 47
            {0xFF, 	0xFF,		1,		83, "Gong", 		"Oriental"},// 48
            {0xFF, 	0xFF,		0,		84, "Flexatone", 	"Oriental"},// 49
            {0xFF, 	0xFF,		0,		85, "Karkabou", 	"Oriental"},// 50
            {0xFF, 	0xFF,		0,		86, "Tabla 1", 		"Oriental"},// 51
            {0xFF, 	0xFF,		0,		87, "Tabla 2", 		"Oriental"},// 52
            {0xFF, 	0xFF,		0,		88, "Tabla slap", 	"Oriental"},// 53
            {0xFF, 	0xFF,		0,		89, "Table rs", 	"Oriental"},// 54
            {0xFF, 	0xFF,		0,		90, "Bendir", 		"Oriental"},// 55
            {0xFF, 	0xFF,		0,		91, "Derbouka", 	"Oriental"},// 56
            {0xFF, 	0xFF,		0,		92, "None", 		"Oriental"} // 57
		}
	}
};
*/

#if ADD_SONICCELL_INTERFACE == 1
#ifdef __LPC177X_8X__
__attribute__ ((section(".intflash")))
#endif
const s_soniccell soniccell_instr_map[2][NUM_BUTTON_KEYBOARD] =
{
	{ // Left
		//CC0,	CC32,	PC, name
		{86, 	0,		10,		"Brush Jz Kit"},// 0
		{86, 	0,		8,		"Rock Kit 1  "},// 1
		{86, 	0,		19,		"House Kit   "},// 2
		{87, 	65,		14,		"Simply Basic"},// 3
		{87, 	65,		16,		"MC-TB Bass  "},// 4
		{87, 	65,		100,	"SC Rubber Bs"},// 5
		{87, 	65,		32,		"Alpha Reso B"},// 6
		{87, 	65,		48,		"Dino Jazz Gt"},// 7
		{87, 	65,		49,		"Clean Gtr   "},// 8
		{87, 	65,		63,		"Punker 1    "},// 9
		{87, 	64,		34,		"SC Pure LP  "},// 10
		{87, 	64,		35,		"SC Trem EP  "},// 11
		{87, 	64,		38,		"SC E. Piano "},// 12
		{87, 	65,		49,		"Crystal EP  "},// 13
		{87, 	65,		52,		"Psycho EP   "},// 14
		{87, 	65,		53,		"Mk2 Stg phsr"},// 15
		{87, 	65,		42,		"Stage Phazer"},// 16
		{87, 	64,		57,		"Vibe EP     "},// 17
		{87, 	64,		61,		"Super Wurly "},// 18
		{87, 	64,		65,		"FM EP Mix   "},// 19
		{87, 	64,		72,		"BrillClav DB"},// 20
		{87, 	64,		74,		"Vintage Clav"},// 21
		{87, 	64,		76,		"Funky D     "},// 22
		{87, 	64,		86,		"Over-D6     "},// 23
		{87, 	64,		91,		"Himalaya Ice"},// 24
		{87, 	64,		94,		"Wine Glass  "},// 25
		{87, 	64,		106,	"SC Shime    "},// 26
		{87, 	69,		93,		"Cosmic Rays "},// 27
		{87, 	69,		102,	"Mod Dare    "},// 28
		{87, 	69,		103,	"Cell Space  "},// 29
		{87, 	69,		110,	"Saturn Ring "},// 30
		{87, 	70,		2,		"Soft OB Pad "},// 31
		{87, 	70,		23,		"Mystic Str  "},// 32
		{87, 	70,		46,		"Nu Epic Pad "},// 33
		{87, 	68,		111,	"Passing By  "},// 34
		{87, 	68,		115,	"SC Try This!"},// 35
		{87, 	68,		121,	"Scatter     "},// 36
		{87, 	68,		119,	"Control Room"},// 37
		{87, 	69,		1,		"Reso Sweep D"},// 38
		{87, 	69,		3,		"Poly Sweep N"},// 39
		{87, 	69,		6,		"12th Planet "},// 40
		{87, 	69,		63,		"TB Booster  "},// 41
		{87, 	69,		60,		"Fary Factor "},// 42
		{87, 	69,		59,		"Digi-vox Syn"},// 43
		{87, 	68,		16,		"Flazzy Lead "},// 44
		{87, 	68,		32,		"Teethy Grit "},// 45
		{87, 	68,		37,		"Anadroid    "},// 46
		{87, 	68,		38,		"Shroomy     "},// 47
		{87, 	68,		75,		"Panning Frmn"},// 48
		{87, 	68,		90,		"Mr Fourier  "},// 49
		{87, 	68,		92,		"Auto 5th Saw"},// 50
		{87, 	67,		9,		"80s Brass 2 "},// 51
		{87, 	67,		11,		"Soft Brass  "},// 52
		{87, 	67,		13,		"Sonic Brass "},// 53
		{87, 	67,		24,		"Cheezy Brass"},// 54
		{87, 	70,		74,		"3D Vox      "},// 55
		{87, 	70,		79,		"Jazz Doos   "},// 56
		{87, 	70,		83,		"Let's Talk! "} // 57
	},
	{ // Right
		//CC0,	CC32,	PC, name
		{87, 	70,		111,	"Timpani Low "},// 0
		{86, 	0,		32,		"Scrh&Voi&Wld"},// 1
		{86, 	0,		31,		"Percussion  "},// 2
		{87, 	65,		69,		"Ulti Acc Bas"},// 3
		{87, 	65,		75,		"Finger Maste"},// 4
		{87, 	65,		88,		"SC Fretnot 2"},// 5
		{87, 	65,		96,		"X Slap Bass "},// 6
		{87, 	65,		34,		"SC Brt Nylon"},// 7
		{87, 	65,		39,		"Thick Steel "},// 8
		{87, 	65,		44,		"SC 12str Gtr"},// 9
		{87, 	70,		110,	"Banjo       "},// 10
		{87, 	70,		101,	"Jamisen     "},// 11
		{87, 	70,		102,	"Koto        "},// 12
		{87, 	70,		93,		"Neo Sitar   "},// 13
		{87, 	70,		84,		"Nice Kalimba"},// 14
		{87, 	70,		98,		"Harpiness   "},// 15
		{87, 	64,		88,		"SC Harpsi   "},// 16
		{87, 	64,		1,		"Rich Grand  "},// 17
		{87, 	64,		6,		"Concert Pian"},// 18
		{87, 	64,		9,		"Hall Concert"},// 19
		{87, 	64,		18,		"Warm PadPian"},// 20
		{87, 	64,		90,		"SC Celesta  "},// 21
		{87, 	64,		112,	"Vibration   "},// 22
		{87, 	64,		122,	"Steel Drums "},// 23
		{87, 	66,		38,		"Viola       "},// 24
		{87, 	66,		36,		"SC Violin   "},// 25
		{87, 	66,		68,		"Delicate Piz"},// 26
		{87, 	66,		60,		"Sahara Str. "},// 27
		{87, 	66,		81,		"Henry IX    "},// 28
		{87, 	66,		84,		"Mix Hit 2   "},// 29
		{87, 	66,		88,		"Smear Hit 2 "},// 30
		{87, 	65,		23,		"Chapel Organ"},// 31
		{87, 	65,		25,		"Pipe Org/Mod"},// 32
		{87, 	65,		27,		"Mid Pipe Org"},// 33
		{87, 	65,		6,		"SC Perc Org "},// 34
		{87, 	65,		21,		"SC Soap Oper"},// 35
		{87, 	64,		127,	"HardRock Org"},// 36
		{87, 	65,		20,		"Smoky Organ "},// 37
		{87, 	65,		33,		"Green Bullet"},// 38
		{87, 	65,		30,		"Guiguette   "},// 39
		{87, 	65,		29,		"Squeeze Me !"},// 40
		{87, 	66,		100,	"SC Oboe     "},// 41
		{87, 	67,		31,		"Alto MP     "},// 42
		{87, 	67,		32,		"Alto Sax    "},// 43
		{87, 	66,		99,		"Clarence net"},// 44
		{87, 	66,		106,	"Andes Mood  "},// 45
		{87, 	66,		107,	"Himalaya Pip"},// 46
		{87, 	66,		105,	"Piccolo     "},// 47
		{87, 	67,		93,		"Sanetific   "},// 48
		{87, 	67,		108,	"Theramax    "},// 49
		{87, 	68,		2,		"Evangelized "},// 50
		{87, 	67,		44,		"Porta Lead  "},// 51
		{87, 	67,		71,		"Sonic Vampir"},// 52
		{87, 	67,		73,		"SC Dist Lead"},// 53
		{87, 	67,		84,		"Vintagolizer"},// 54
		{87, 	66,		114,	"Soft Tb     "},// 55
		{87, 	66,		108,	"Solo Tp     "},// 56
		{87, 	66,		112,	"Mute Tp/Mod "} // 57
	}
};
#endif
