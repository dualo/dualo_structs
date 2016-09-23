#ifndef MUSIC_MIGRATION_H
#define MUSIC_MIGRATION_H

#include "music_structs.h"

/*
 * WARNING :
 * This file is used in the firmware and in du-station code.
 * Keep that in mind when you modify it.
 */

enum MUSIC_MIGRATION_ERROR {
    MUSIC_MIGRATION_SUCCESS = 0,

    MUSIC_MIGRATION_FAIL                = -1,
    MUSIC_MIGRATION_NULL_PTR            = -2,
    MUSIC_MIGRATION_INVALID_VERSION     = -3,
    MUSIC_MIGRATION_TOO_RECENT          = -4,
    MUSIC_MIGRATION_BAD_LADRESS         = -5,
    MUSIC_MIGRATION_BAD_SAMPLE_ADRESS   = -6
};

int migrate_music(s_total_buffer *du_music);

#endif // MUSIC_MIGRATION_H
