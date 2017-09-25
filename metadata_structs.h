/*
 * metadata_structs.h
 *
 *  Created on: 25/02/2015
 *      Author: mma
 */


#ifndef METADATA_STRUCTS_H_
#define METADATA_STRUCTS_H_

#ifdef __LPC177X_8X__
#include "lpc_types.h"
#else
#include <stdint.h>
#endif // __LPC177X_8X__

#define MUSIC_METADATAHEADERSIZE	12

/***************** metadata type list **********************/

enum MUSICMETADATA_TYPE {
	MUSICMETADATA_TYPE_NONE,
	MUSICMETADATA_TYPE_ARRANGEMENT = 0x47525241,//ARRG
	MUSICMETADATA_TYPE_TAGS,
	NUM_MUSICMETADATA_TYPE
};

/***************** metadata entry list **********************/
typedef struct
{
	uint32_t meta_type;
	uint32_t meta_size;
	uint32_t meta_address;
} s_metadata;

#define MUSIC_METADATAENTRYSIZE	(3 * 4)



#endif // METADATA_STRUCTS_H_
