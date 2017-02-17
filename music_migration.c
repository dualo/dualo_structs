#include "old_sound_structs.h"
#include "music_migration.h"


/*
 * WARNING :
 * This file is used in the firmware and in du-station code.
 * Keep that in mind when you modify it.
 */
#include <string.h>
#if !defined(__LPC177X_8X__) && !defined(__LPC18XX__)//TODO : add a du station or C++ flag
//Only to be use on computer apps
#include <stdlib.h>
#define lpc_new malloc
#define lpc_free free
#else
#include "debug/trace.h"
#endif

typedef struct
{
    uint8_t old_PC;
    uint8_t old_CC0;
    uint8_t new_ID;
    uint8_t new_PC;
} migrate_dusound;

migrate_dusound list_dusound[] =
{
    {45,	0,	1,   1}, // percu
    { 9,	0,	2,   1}, // jazz set
    { 8,	0,	3,   1}, // rock set
    {114,	0,	7,   2}, // steeldrum
    {13,	0,	4,   2}, // balafon
    {12,	0,	5,   2}, // marimba
    {11,	0,	6,   2}, // vibraphone
    {14,	0,	8,   3}, // celesta
    { 1,	0,	9,   3}, // jazz piano
    { 2,	0,	10,  3}, // classical piano
    { 3,	0,	11,  4}, // el piano 1
    { 4,	0,	12,  4}, // el piano 2
    {46,	0,	13,  4}, // wurlie
    {98,	0,	14,  4}, // clavi
    {48,	0,	15,  5}, // fretless bass
    {49,	0,	16,  5}, // jazz bass
    {50,	0,	17,  5}, // double bass
    {15,	0,	18,  6}, // cymbalum
    {25,	0,	19,  6}, // banjo
    {100,	0,	20,  6}, // classical guitar
    {24,	0,	21,  6}, // folk guitar
    {55,	0,	23,  7}, // jazz guitar
    {26,	0,	24,  7}, // clean guitar
    {27,	0,	25,  8}, // blues guitar
    {30,	0,	27,  8}, // rock guitar
    {97,	0,	29,  9}, // harp
    { 6,	0,	30,  9}, // harpsichord
    {104,	0,	31,  9}, // sitar
    {22,	0,	36, 11}, // harmonica
    {21,	0,	37, 11}, // musette
    {21,	8,	38, 11}, // jazz accordion
    {20,	0,	39, 12}, // reed organ
    {19,	8,	40, 12}, // church organ
    {19,	0,	41, 12}, // cathedral
    {17,	0,	42, 13}, // perc organ
    {16,	0,	43, 13}, // drawbar organ
    {18,	0,	44, 13}, // rock organ
    {99,	0,	45, 13}, // brige organ
    {113,	0,	48, 14}, // clarinet
    {111,	0,	46, 14}, // bassoon oboe
    {115,	0,	51, 15}, // flute
    {117,	0,	49, 15}, // pan flute
    {120,	0,	52, 15}, // ocarina
    {102,	0,	56, 16}, // tuba
    {107,	0,	57, 16}, // trombone
    {106,	0,	58, 16}, // trumpet

    { 7,	0,	59, 17}, // electro set
    {31,	0,	66, 19}, // square bass
    {32,	0,	67, 19}, // mod square bass
    {33,	0,	68, 19}, // sub bass
    {34,	0,	72, 21}, // fuzz bass
    {37,	0,	69, 20}, // funk bass
    {39,	0,	70, 20}, // bow wow bass
    {40,	0,	71, 20}, // saw bass
    {95,	0,	77, 22}, // sweep pad
    {89,	0,	78, 22}, // warm pad
    {56,	0,	80, 22}, // synth string
    {57,	0,	81, 22}, // smotth string
    {59,	0,	84, 23}, // sweetness
    {60,	0,	83, 23}, // travel pad
    {62,	0,	85, 23}, // crypte pad
    {63,	0,	86, 23}, // hyper lead
    {64,	0,	91, 23}, // growlin
    {66,	0,	89, 23}, // june
    {68,	0,	91, 24}, // funk lead
    {69,	0,	92, 24}, // snow lead
    {127,	0,	106,26}, // sinus
    {72,	0,	95, 24}, // kraft
    {73,	0,	64, 18}, // tech stab
    {74,	0,	65, 18}, // synth bell
    {75,	0,	98, 25}, // blumble
    {76,	0,	99, 25}, // funky lead
    {77,	0,	100,25}, // cat lead
    {79,	0,	102,25}, // glide
    {83,	0,	63, 18}, // morodot
    {91,	0,	112,29}, // synth brass 1
    {92,	0,	113,29}, // synth brass 2
};

void find_instrument(info_instr* new_instr, info_instr* old_instr)
{
    uint32_t i;

    uint32_t nb_instr = sizeof(list_dusound)/sizeof(migrate_dusound);

    memcpy(new_instr, old_instr, INSTR_INFO_SIZE);
    new_instr->instr_midi_pc = 0xFF;

    for(i=0; i<nb_instr; i++)
    {
        if((old_instr->instr_midi_pc == list_dusound[i].old_PC)&&(old_instr->instr_midi_C0 == list_dusound[i].old_CC0))
        {
            new_instr->instr_id = list_dusound[i].new_ID;
            new_instr->instr_midi_pc = list_dusound[i].new_PC;
            new_instr->instr_user_id = 1; // dualo
            new_instr->instr_version = 0; // in order to migrate on next du-music load
        }
    }
}

int migrate_music(s_total_buffer *du_music)
{
    if (du_music == NULL)
    {
        return MUSIC_MIGRATION_NULL_PTR;
    }

    music_song* song = &du_music->local_song;
    if (song == NULL)
    {
        return MUSIC_MIGRATION_NULL_PTR;
    }

    uint32_t version = song->s_version_music;

    if (version <= 0)
    {
        return MUSIC_MIGRATION_INVALID_VERSION;
    }
    else if (version > VERSION_MUSIC)
    {
        return MUSIC_MIGRATION_TOO_RECENT;
    }
    else if (version == VERSION_MUSIC)
    {
        return MUSIC_MIGRATION_SUCCESS;
    }

    uint32_t i, j, k;

    if (version <= 1)
    {
        song->s_size = MUSIC_SONG_SIZE + (song->s_totalsample * MUSIC_SAMPLE_SIZE);
        song->s_metadata = 0;
        song->s_playhead = 0;
        song->s_state = 0;
        song->s_transpose = RECORD_TRANSPOSEDEFAULT;

        song->s_reverb_preset = FX_REVERB_PRESET_DEFAULTVALUE;

        song->s_direction_gyro_P = -1;
        song->s_direction_gyro_R = -1;
        song->s_direction_gyro_Y = -1;
        song->s_activ_aftertouch = 0;
        song->s_activ_slider_L = 0;
        song->s_activ_slider_R = 0;
        song->s_activ_gyro_P = 0;
        song->s_activ_gyro_R = 0;
        song->s_activ_gyro_Y = 0;

        FX_reverb* reverb = &song->s_reverb;
        if (reverb == NULL)
        {
            return MUSIC_MIGRATION_NULL_PTR;
        }

        reverb->r_level         = FX_REVERB_LEVEL_DEFAULTVALUE;
        reverb->r_directlevel   = FX_REVERB_DIRECTLEVEL_DEFAULTVALUE;
        reverb->r_revsend       = FX_REVERB_REVSEND_DEFAULTVALUE;
        reverb->r_tonegain      = FX_REVERB_TONEGAIN_DEFAULTVALUE;
        reverb->r_tonefreq      = FX_REVERB_TONEFREQ_DEFAULTVALUE;
        reverb->r_prehp         = FX_REVERB_PREHP_DEFAULTVALUE;
        reverb->r_time          = FX_REVERB_TIME_DEFAULTVALUE;
        reverb->r_echofeedback  = FX_REVERB_ECHOFEED_DEFAULTVALUE;
        reverb->r_hdamp         = FX_REVERB_HDAMP_DEFAULTVALUE;
        reverb->r_thresgate     = FX_REVERB_THRESGATE_DEFAULTVALUE;
        reverb->r_predelaytime  = FX_REVERB_PREDELAYTIME_DEFAULTVALUE;


        for (i = 0; i < MUSIC_MAXTRACK; ++i)
        {
            for (j = 0; j < MUSIC_MAXLAYER; ++j)
            {
                music_loop* loop = &song->s_track[i].t_loop[j];
                if (loop == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                music_instr_old* instr = (music_instr_old*) &loop->l_instr;
                if (instr == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                // PRESETS
                preset_instr_old* preset = &instr->i_preset;
                if (preset == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                preset->s_arpegiator_type = 0;
                preset->s_arpegiator_beat = 0;

                preset->s_direction_gyro_P = -1;
                preset->s_direction_gyro_R = -1;
                preset->s_direction_gyro_Y = -1;

                preset->s_compressor_onoff = 0;
                preset->s_delay_onoff = 0;
                preset->s_distortion_onoff = 0;
                preset->s_eq_onoff = 0;
                preset->s_chorus_onoff = 0;

                preset->s_autopitch_rate = 0;
                preset->s_autopitch_range = 127;

                preset->s_tremolo_rate = 0;
                preset->s_tremolo_range = 127;

                preset->s_autopan_rate = 0;
                preset->s_autopan_range = 127;

                preset->s_autowah_rate = 0;
                preset->s_autowah_range = 127;

                preset->s_multinote_act = 0;
                preset->s_multinote[0] = 0;
                preset->s_multinote[1] = 0;
                preset->s_multinote[2] = 0;
                preset->s_multinote[3] = 0;

                preset->s_adsr_attack    = FX_ADSR_ATTACK_DEFAULTVALUE;
                preset->s_adsr_release   = FX_ADSR_RELEAS_DEFAULTVALUE;
                preset->s_wah_type       = FX_WAH_FILTERTYPE_DEFAULTVALUE;
                preset->s_wah_freq       = 0;
                preset->s_wah_res        = FX_WAH_FILTERRES_DEFAULTVALUE;

                if(preset->s_sendtorev)
                	preset->s_sendtorev = (preset->s_sendtorev*MAIN_SENDTOREV_DEFAULT)/32; //32 is old default value for reverb
                else
                	preset->s_sendtorev = MAIN_SENDTOREV_MIN;


                // INSTR
                info_instr* info = &instr->i_instrument;
                if (info == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                if (info->instr_key_map)
                    info->instr_type = INSTR_PERCU;
                else
                    info->instr_type = INSTR_HARMONIC;

                if (loop->l_state == REC_EMPTY)
                {
                    // Loop empty
                    continue;
                }

#ifdef __LPC177X_8X__
                trace(error,"cvt sample %d %d %d\r\n", i, j, loop->l_numsample);
#endif

                if (((uint32_t)loop->l_adress) % MUSIC_SAMPLE_SIZE != 0)
                {
                    // l_adress is not a multiple of MUSIC_SAMPLE_SIZE
                    return MUSIC_MIGRATION_BAD_LADRESS;
                }

                music_sample_p firstSampleIndex = (music_sample_p)(((uint32_t)loop->l_adress) / MUSIC_SAMPLE_SIZE);
                if (((uint32_t)firstSampleIndex) + loop->l_numsample > RECORD_SAMPLEBUFFERSIZE)
                {
                    // sample adresses point outside sample array
                    return MUSIC_MIGRATION_BAD_SAMPLE_ADRESS;
                }

                // SAMPLES
                music_sample *played_buffer = &du_music->local_buffer[((uint32_t)firstSampleIndex)];
                if (played_buffer == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                for (k = 0; k < loop->l_numsample; ++k)
                {
                    // remove canals 8 -> 15 not used (and initialized anymore)
                    played_buffer[k].canal = played_buffer[k].canal & 0xF7;
                    // harmonic instrument -> remove octave from sample
                    if (info->instr_type == INSTR_HARMONIC)
                    {
                        if (played_buffer[k].control == 0 || played_buffer[k].control == 1)
                        {
                            played_buffer[k].note -= (12 * preset->s_instr_octave)
                                    + (song->s_transpose - RECORD_TRANSPOSEMAX);
                        }
                    }
                }
            }
        }
    }

    if (version <= 2)
    {
        // add swing
        song->s_swing = 0;

        // migration here
        music_instr_old* instr_old = (music_instr_old*)lpc_new(MUSIC_INSTRU_SIZE_OLD);

        for (i = 0; i < MUSIC_MAXTRACK; ++i)
        {
            for (j = 0; j < MUSIC_MAXLAYER; ++j)
            {
                music_instr* instr = &song->s_track[i].t_loop[j].l_instr;
                if (instr == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                memcpy(instr_old, instr, MUSIC_INSTRU_SIZE_OLD);
                memset(instr, 0, MUSIC_INSTRU_SIZE);

                // Info
                find_instrument(&instr->i_instrument, &instr_old->i_instrument);
                //memcpy(&instr->i_instrument, &instr_old->i_instrument, INSTR_INFO_SIZE);

                // Preset
                preset_instr* preset = &instr->i_preset;
                if (preset == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                preset_instr_old* preset_old = &instr_old->i_preset;
                if (preset_old == NULL)
                {
                    return MUSIC_MIGRATION_NULL_PTR;
                }

                preset->s_name[0] = '0';

                preset->s_volume = preset_old->s_volume;
                preset->s_panning = preset_old->s_panning;
                preset->s_sendtorev = preset_old->s_sendtorev;
                preset->s_instr_octave = preset_old->s_instr_octave;
                preset->s_key_curve = preset_old->s_key_curve;
                preset->s_expression = preset_old->s_expression;

                preset->s_activ_aftertouch = preset_old->s_activ_aftertouch;
                preset->s_activ_slider_L = preset_old->s_activ_slider_L;
                preset->s_activ_slider_R = preset_old->s_activ_slider_R;
                preset->s_activ_gyro_P = preset_old->s_activ_gyro_P;
                preset->s_activ_gyro_R = preset_old->s_activ_gyro_R;
                preset->s_activ_gyro_Y = preset_old->s_activ_gyro_Y;

                preset->s_direction_aftertouch = -1;
                preset->s_direction_slider_L = -1;
                preset->s_direction_slider_R = -1;
                preset->s_direction_gyro_P = preset_old->s_direction_gyro_P;
                preset->s_direction_gyro_R = preset_old->s_direction_gyro_R;
                preset->s_direction_gyro_Y = preset_old->s_direction_gyro_Y;

                preset->s_portamento_on_off = preset_old->s_portamento_on_off;
                preset->s_portamento_ctrl = preset_old->s_portamento_ctrl;
                preset->s_portamento_time = preset_old->s_portamento_time;

                preset->s_displayled = 0;
                memset(preset->s_leds, 0, NUM_LED_VALUE);

                preset->s_pitch = 64;
                preset->s_pitch_bend_sensitivity = preset_old->s_pitch_bend_sensitivity;

                preset->s_adsr_attack = preset_old->s_adsr_attack;
                preset->s_adsr_release = preset_old->s_adsr_release;

                preset->s_wah_type = preset_old->s_wah_type;
                preset->s_wah_freq = 127 - preset_old->s_wah_freq; // invert filter freq
                preset->s_wah_res = preset_old->s_wah_res;

                preset->s_multinote_act = preset_old->s_multinote_act;
                memcpy(&preset->s_multinote, &preset_old->s_multinote, 4);

                preset->s_arpegiator_type = preset_old->s_arpegiator_type;
                preset->s_arpegiator_beat = preset_old->s_arpegiator_beat;

                preset->s_autopitch_rate = preset_old->s_autopitch_rate;
                preset->s_autopitch_range = preset_old->s_autopitch_range;

                preset->s_tremolo_rate = preset_old->s_tremolo_rate;
                preset->s_tremolo_range = preset_old->s_tremolo_range;

                preset->s_autopan_rate = preset_old->s_autopan_rate;
                preset->s_autopan_range = preset_old->s_autopan_range;

                preset->s_autowah_rate = preset_old->s_autowah_rate;
                preset->s_autowah_range = preset_old->s_autowah_range;

                preset->s_reverb_onoff = preset_old->s_reverb_onoff;

                if (preset_old->s_compressor_preset != 0)
                {
                    preset->s_compressor_onoff = 1;
                    memcpy(&preset->s_compressor, &instr_old->i_compressor, FX_COMP_SIZE);
                }
                if (preset_old->s_delay_preset != 0)
                {
                    preset->s_delay_onoff = 1;
                    memcpy(&preset->s_delay, &instr_old->i_delay, FX_DELAY_SIZE);
                }
                if (preset_old->s_distortion_preset != 0)
                {
                    preset->s_distortion_onoff = 1;
                    memcpy(&preset->s_distortion, &instr_old->i_distortion, FX_DIST_SIZE);
                }
                if (preset_old->s_eq_preset != 0)
                {
                    preset->s_eq_onoff = 1;
                    memcpy(&preset->s_equalizer, &instr_old->i_equalizer, FX_EQ_SIZE);
                }

                if (preset_old->s_chorus_preset != 0)
                {
                    preset->s_chorus_preset = 1;
                    preset->s_chorus_onoff = 1;
                    memcpy(&preset->s_chorus[0], &instr_old->i_chorus, FX_CHORUS_SIZE);
                }
            }
        }

        lpc_free(instr_old);
    }

    song->s_version_music = VERSION_MUSIC;

    return MUSIC_MIGRATION_SUCCESS;
}
