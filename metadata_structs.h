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

#define METADATA_CURRENT_VERSION 1
#define METADATA_SIGNATURE 0x4154454D // META

#define MUSICMETADATA_ARRANGEMENT_CURRENT_VERSION 1
#define MUSICMETADATA_ARRANGEMENT_SIGNATURE 0x47525241 // ARRG

#define MUSICMETADATA_GAME_CURRENT_VERSION 3
#define MUSICMETADATA_GAME_SIGNATURE 0x454D4147 // GAME

typedef struct
{
    uint32_t meta_signature;
    uint32_t meta_version;
    uint32_t meta_size;
} s_metadata_header;

#define METADATA_HEADER_SIZE (3 * 4)


#endif // METADATA_STRUCTS_H_
