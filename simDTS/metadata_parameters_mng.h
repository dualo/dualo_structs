/*
 * metadata_parameters_mng.h
 *
 *  Created on: 25/02/2015
 *      Author: mma
 */


#ifndef METADATA_PARAMETERS_MNG_H_
#define METADATA_PARAMETERS_MNG_H_


#define MUSIC_METADATAHEADERSIZE	128

/***************** metadata type list **********************/

enum MUSICMETADATA_TYPE {
	MUSICMETADATA_TYPE_NONE,
	MUSICMETADATA_TYPE_ARRANGEMENT,
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



#endif // METADATA_PARAMETERS_MNG_H_
