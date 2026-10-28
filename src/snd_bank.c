#include "common.h"
#include "sound.h"
#include "snd_init.h"
#include "snd_bank.h"
#include "snd_note.h"
#include "snd_track.h"

s32 func_80016DB4(s32 a0, SoundSeqTrack *a1);
s32 func_80016E08(s32 a0, SoundSeqTrack *a1);
void func_80016E5C(void);
s32 sndGetBankGroup(s32 a0);
void sndInitTrack(SoundSeqTrack *track, s32 bankId);
s32 sndBuildVoiceMask(SoundSeqTrack *tracks, s32 a1);
void func_8001708C(u8 *data, u32 mask);
void func_80017880(SoundSeqTrack *track, s32 *params, u32 bit, s32 bankId);
void sndReleaseVoiceFromTracks(s32 voiceId);
void func_80017C9C(s32 *a0, s32 *a1, s32 a2);
void func_80017DB0(u8 *seq, s32 swap);

extern s32 D_80073DE4;
extern u8 D_800517F0[];
extern s8 D_8005182F[];
extern SoundSeqTrack D_80075080[];

/**
 * @brief Adjusts instrument index upward if flag 0x400 is set and instrument is in range.
 *
 * If bit 10 of @p a0 is set and the track's instrument index is in [0x40, 0x80),
 * offsets the sample addresses by +0x20000 and the instrument by +0x20.
 *
 * @param a0 Flags bitfield.
 * @param a1 Pointer to the sequence track structure.
 * @return The (possibly adjusted) instrument index.
 */
s32 func_80016DB4(s32 a0, SoundSeqTrack *a1) {
    if (a0 & 0x400) {
        if ((u32)(a1->instrument - 0x40) < 0x40) {
            s32 adj = 0x20000;
            a1->unkF4.startAddr = a1->unkF4.startAddr + adj;
            a1->unkF4.repeatAddr = a1->unkF4.repeatAddr + adj;
            a1->instrument = a1->instrument + 0x20;
        }
    }
    return a1->instrument;
}

/**
 * @brief Adjusts instrument index downward if flag 0x400 is set and instrument is in range.
 *
 * If bit 10 of @p a0 is set and the track's instrument index is in [0x60, 0x80),
 * offsets the sample addresses by -0x20000 and the instrument by -0x20.
 *
 * @param a0 Flags bitfield.
 * @param a1 Pointer to the sequence track structure.
 * @return The (possibly adjusted) instrument index.
 */
s32 func_80016E08(s32 a0, SoundSeqTrack *a1) {
    if (a0 & 0x400) {
        if ((u32)(a1->instrument - 0x60) < 0x20) {
            s32 adj = (s32)0xFFFE0000;
            a1->unkF4.startAddr = a1->unkF4.startAddr + adj;
            a1->unkF4.repeatAddr = a1->unkF4.repeatAddr + adj;
            a1->instrument = a1->instrument - 0x20;
        }
    }
    return a1->instrument;
}

void func_80016E5C(void) {
    SoundMusicConfig *second;
    SoundSeqTrack *track;
    u32 freed;
    u32 mask;
    s32 voice;
    s32 n;
    u32 mainBits;
    u32 secondBits;

    second = D_80073CA8;
    if (second == NULL) {
        return;
    }
    mainBits = g_sndSeqState->unk08 & g_sndSeqState->unk0C;
    secondBits = second->unk08 & second->unk0C;
    voice = ~mainBits | secondBits;
    mask = ~(second->unk0C & voice);
    mask &= g_sndSeqState->unk0C;
    mask &= 0xFFFFFF;
    freed = 0;
    voice = 0;
    if (mask != 0) {
        do {
            if (mask & (1 << voice)) {
                track = D_80073C34;
                for (n = 32; n != 0; n--, track++) {
                    if (track->unkF4.unk00 == voice) {
                        track->unkF4.unk00 = 0x18;
                        freed |= 1 << voice;
                    }
                }
                mask &= ~(1 << voice);
            }
            voice++;
        } while (mask != 0);
    }
    D_80075028.unk0C = freed | D_80075028.unk0C;
}

/**
 * @brief Determines which sound bank group a given identifier belongs to.
 *
 * Compares @p a0 against entries in D_80074EB8 (a table of sound bank IDs).
 * Indices 1 and 4 map to group 1; indices 2 and 5 map to group 2.
 *
 * @param a0 Sound bank or instrument identifier to look up.
 * @return 1 if in group 1, 2 if in group 2, 0 if not found or @p a0 is 0.
 */
s32 sndGetBankGroup(s32 a0) {
    s32 ret = 0;
    if (a0 == 0) return ret;
    if (a0 == D_80074EB8[1] || a0 == D_80074EB8[4]) {
        ret = 1;
    } else if (a0 == D_80074EB8[2] || a0 == D_80074EB8[5]) {
        ret = 2;
    }
    return ret;
}

/**
 * @brief Initializes a sequence track structure to default values.
 *
 * Sets the track's instrument bank ID at offset +0x00, initializes
 * default pitch bend (0x6E00 at +0x80), default tempo (0x32000000 at +0x44),
 * sentinel voice value (0xFFFF at +0x6E), and zeroes all modulation,
 * panning, volume, and effect fields. Finally applies instrument table
 * entry 0 via sndTrackApplyInstrument.
 *
 * @param a0 Pointer to the track structure (stride 0x110).
 * @param a1 Instrument bank ID to assign to this track.
 */
void sndInitTrack(SoundSeqTrack *track, s32 bankId) {
    track->pitchBend = 0x6E00;
    track->panShifted = 0x32000000;
    *(s32 *)track = bankId;
    track->detune = 0;
    track->detuneTarget = 0;
    track->duration = 0;
    track->cmdDataPtr = 0;
    track->durationDelta2 = 0;
    track->unk90 = 0;
    track->volumeAccum = 0;
    track->volumeDelta = 0;
    track->panFade = 0;
    track->timing = 0;
    track->panEnvSpeed = 0;
    track->flags = 0;
    track->panLfoStop = 0;
    track->timerActive = 0;
    track->unk6E = 0xFFFF;
    track->unkF4.sweep = 0;
    track->panLfo = 0;
    track->volumeLfo = 0;
    track->delaySend = 0;
    track->panLfoTarget = 0;
    track->volumeLfoTarget = 0;
    track->delaySendTarget = 0;
    track->reverbDuration = 0;
    track->noiseDuration = 0;
    sndTrackApplyInstrument(track, 0);
}

/**
 * @brief Builds a voice bitmask from a track bitmask.
 *
 * For each of the 32 tracks whose bit is set in @p a1, reads the voice the
 * track is assigned to. If the voice index is valid (< 24), sets the
 * corresponding bit in the result.
 *
 * @param tracks The sequence track array.
 * @param a1 Bitmask of tracks to include (bit N = track N).
 * @return Bitmask of SPU voices used by the selected tracks.
 */
s32 sndBuildVoiceMask(SoundSeqTrack *tracks, s32 a1) {
    u32 i = 0;
    s32 result = 0;
    s32 bit = 1;
    do {
        if (a1 & (bit << i)) {
            s32 val = tracks->unkF4.unk00;
            if ((u32)val < 0x18) {
                result |= (bit << val);
            }
        }
        i++;
        tracks++;
    } while (i < 0x20);
    return result;
}

void func_8001708C(u8 *data, u32 mask) {
    SoundMusicConfig *config;
    SoundSeqTrack *track;
    func_8001A1E8_data *seq;
    u32 tracks;
    u32 bit;
    u32 i;
    s32 flags;
    u32 allVoices;
    u8 *p;

    seq = (func_8001A1E8_data *)data;
    g_sndSeqState->unk2C = seq;
    tracks = seq->unk20;
    bit = (D_80073CA8 != NULL) ? sndBuildVoiceMask(D_80073C34, D_80073CA8->unk04) : 0;
    allVoices = 0xFFFFFF;
    D_80075028.unk0C |= ~bit & (~(D_80075028.unk00 | D_800772A4) & allVoices);
    g_sndSeqState->unk18 = 0;
    if (D_800772F4 & 1) {
        g_sndSeqState->unk04 = 0;
        g_sndSeqState->unk1C |= tracks & mask;
    } else {
        g_sndSeqState->unk1C = 0;
        g_sndSeqState->unk04 |= tracks & mask;
    }
    g_sndSeqState->unk08 = seq->unk24;
    g_sndSeqState->unk0C = seq->unk28;
    flags = g_sndSeqState->unk00 & ~0x603;
    g_sndSeqState->unk00 = flags;
    g_sndSeqState->unk00 = (seq->unk14 == D_80073DE4) ? flags | 0x400 : flags | 0x200;
    p = (u8 *)&seq->unk30 + seq->unk30;
    g_sndSeqState->unk30 = seq->unk30 ? (s16 *)p : NULL;
    p = (u8 *)&seq->unk34 + seq->unk34;
    g_sndSeqState->unk34 = seq->unk34 ? (func_8001B820_entry *)p : NULL;
    bit = 1;
    i = 0;
    track = D_80070D60;
    data += 0x40;
    for (; i < 32; track++, i++, bit <<= 1) {
        if (tracks & bit & mask) {
            track->unk00 = data + *(u16 *)data;
            track->volume = 4;
            track->volumeBase = 2;
            track->pitchBend = 0x7F00;
            track->panShifted = 0x3FFF0000;
            track->expression = 0x4000;
            track->detuneTarget = 0;
            track->detune = 0;
            track->duration = 0;
            track->cmdDataPtr = 0;
            track->durationDelta2 = 0;
            track->unk90 = 0;
            track->volumeAccum = 0;
            track->volumeDelta = 0;
            track->notePitch = 0x8000;
            track->notePitchFade = 0;
            track->duration = 0;
            track->exprFade = 0;
            track->panFade = 0;
            track->timing = 0;
            track->panFadeTarget = 0;
            track->timerActive = 0;
            track->panLfoStop = 0;
            track->panEnvSpeed = 0;
            data += 2;
            track->flags = ((g_sndSeqState->unk0C & bit) == 0) << 6;
            track->panLfo = 0;
            track->volumeLfo = 0;
            track->delaySend = 0;
            track->panLfoTarget = 0;
            track->volumeLfoTarget = 0;
            track->delaySendTarget = 0;
            track->reverbDuration = 0;
            track->noiseDuration = 0;
            sndTrackApplyInstrument(track, 0);
        } else {
            if (tracks & bit) {
                if (!(bit & mask)) {
                    data += 2;
                }
            }
            track->volume = 3;
            track->volumeBase = 1;
            track->unk00 = D_800517F0;
            track->unkF4.updateFlags |= 0x4400;
            track->unkF4.adsrHigh = (track->unkF4.adsrHigh & 0xFFE0) | 5;
        }
        track->unkF4.unk00 = 0x18;
        tracks &= ~bit;
    }
    config = g_sndSeqState;
    config->unk20 = 0xFFFF0000;
    config->unk28 = 1;
    config->unk5C = 0;
    D_80077288[2] = 0;
    config->unk6A = 0;
    config->unk68 = 0;
    config->unk66 = 0;
    config->unk6C = 0;
    config->unk3C = 0;
    config->unk40 = 0;
    config->unk44 = 0;
    config->unk60 = 0;
    config->unk14 = 0;
    config->unk10 = 0;
    config->unk38 = D_8005182F;
    D_80077288[2] |= 0x100;
}

void func_80017410(SoundMusicConfig *config, SoundSeqTrack *tracks, s32 id) {
    SoundMusicConfig **owner;
    u32 i;

    if (config->unk04 == 0) {
        return;
    }
    if (id == 0 || id == config->unk5E) {
        config->unk18 = -1;
        // matching hack
        for (i = 32; i != 0; i--, tracks++) {
            tracks->volume = 3;
            tracks->volumeBase = 1;
            tracks->unk00 = D_800517F0;
        }
        config->unk5E = 0;
        config->unk14 = 0;
        config->unk10 = 0;
        owner = D_800772F8;
        for (i = 0; i < 24; i++, owner++) {
            if (*owner == config) {
                *owner = NULL;
                spuSetVoiceReleaseMode(i, 5, 3);
            }
        }
    }
}

void func_800174E4(s32 id, u32 mode) {
    SoundSeqTrack *track;
    u32 active;
    u32 bit;
    u32 i;
    s32 age;
    s32 flags;

    bit = 0x1000;
    track = D_80072F70;
    active = D_80075028.unk00 | D_80075028.unk10;
    if (mode & 0x0FFFFFFF) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if (active & bit) {
                if (id == -2) {
                    if ((track->keyOnMask & mode) && track->keyOffMask < 0) {
                        flags = track->flags;
                        if (flags & 0x100000) {
                            track->flags = flags | 0x200000;
                        } else {
                            D_80075028.unk0C |= bit;
                            sndTrackClearVoiceBits(track, bit);
                            track->flags = 0;
                        }
                    }
                } else if (track->keyOnMask & mode) {
                    flags = track->flags;
                    if (flags & 0x100000) {
                        track->flags = flags | 0x200000;
                    } else {
                        D_80075028.unk0C |= bit;
                        sndTrackClearVoiceBits(track, bit);
                        track->flags = 0;
                    }
                }
            }
        }
    } else if (mode & 0x80000000) {
        track = &D_80072F70[id];
        bit <<= id;
        if (active & bit) {
            func_800174E4(track->keyOffMask, 0);
        }
        bit <<= 1;
        track++;
        if (active & bit) {
            func_800174E4(track->keyOffMask, 0);
        }
        return;
    } else if (mode & 0x40000000) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if (track->keyOnMask != 0) {
                active &= ~bit;
            }
        }
        track = D_80072F70;
        bit = 0x1000;
        age = 0;
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && age < track->unk54) {
                age = track->unk54;
            }
        }
        track = D_80072F70;
        bit = 0x1000;
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && age == track->unk54) {
                flags = track->flags;
                if (flags & 0x100000) {
                    track->flags = flags | 0x200000;
                } else {
                    D_80075028.unk0C |= bit;
                    sndTrackClearVoiceBits(track, bit);
                    track->flags = 0;
                }
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if (active & bit) {
                if (id == -1) {
                    if (track->keyOffMask < 0) {
                        flags = track->flags;
                        if (flags & 0x100000) {
                            track->flags = flags | 0x200000;
                        } else {
                            D_80075028.unk0C |= bit;
                            sndTrackClearVoiceBits(track, bit);
                            track->flags = 0;
                        }
                    }
                } else if (track->keyOffMask == id) {
                    flags = track->flags;
                    if (flags & 0x100000) {
                        track->flags = flags | 0x200000;
                    } else {
                        D_80075028.unk0C |= bit;
                        sndTrackClearVoiceBits(track, bit);
                        track->flags = 0;
                    }
                }
            }
        }
    }
    D_80077288[2] |= 0x110;
}

void func_80017880(SoundSeqTrack *track, s32 *params, u32 bit, s32 bankId) {
    u32 i;

    track->keyOffMask = params[0];
    track->keyOnMask = params[1];
    track->pitchModDepth = (u8)params[2] << 8;
    track->pitchModCounter = 0;
    track->notePitch = 0x8000;
    track->notePitchFade = 0;
    track->expression = ((u16)params[3] & 0x7F) << 8;
    track->exprFade = 0;
    track->bankPtr = 1 << params[4];
    track->volume = 2;
    track->volumeBase = 1;
    track->voiceActive = 1;
    track->unk54 = -2;
    track->loopLimit = 0;
    track->pitchFadeCounter = 0;
    sndInitTrack(track, bankId);
    D_800772F8[track->unkF4.unk00] = 0;
    spuSetVoiceReleaseMode(track->unkF4.unk00, 5, 3);
    D_80075028.unk00 |= bit;
    D_80075028.unk0C |= bit;
    bit = ~bit;
    D_80075028.unk04 &= bit;
    D_80075028.unk08 &= bit;
    D_80075028.unk1C &= bit;
    D_80075028.unk20 &= bit;
    D_80075028.unk24 &= bit;
    if (D_800772F4 & 2) {
        bit = 0x1000;
        track = D_80072F70;
        // matching hack
        for (i = 12; i != 0; i--, track++, bit <<= 1) {
            if ((D_80075028.unk00 & bit) && !(track->keyOnMask & 0x2000000)) {
                D_80075028.unk00 &= ~bit;
                D_80075028.unk10 |= bit;
            }
        }
    }
}

/**
 * @brief Releases a voice from all active sequence tracks (duplicate of sndReleaseVoice).
 *
 * Iterates over 32 track entries in D_80070D60 (stride 0x110, voice at +0xF4).
 * Any track assigned to voice @p a0 is reassigned to 24 (none). Also checks
 * the secondary sequence array D_80073C34 if D_80073CA8 is set.
 *
 * @param a0 SPU voice index to release (0-23).
 */
void sndReleaseVoiceFromTracks(s32 voiceId) {
    s32 i;
    SoundSeqTrack *track;

    track = D_80070D60;
    for (i = 32; i != 0; i--) {
        if (track->unkF4.unk00 == voiceId)
            track->unkF4.unk00 = 0x18;
        track++;
    }

    if (D_80073CA8 != NULL) {
        track = D_80073C34;
        for (i = 32; i != 0; i--) {
            if (track->unkF4.unk00 == voiceId)
                track->unkF4.unk00 = 0x18;
            track++;
        }
    }
}

void func_80017AAC(s32 *cmd, s32 sfx1, s32 sfx2, s32 noStop) {
    SoundSeqTrack *track;
    u32 bit;
    u32 busy;
    s32 i;

    if (sfx1 == 0 && sfx2 == 0) {
        return;
    }
    if (noStop == 0 && cmd[1] != 0) {
        func_800174E4(0, cmd[1]);
    }
    do {
        track = &D_80072F70[11];
        bit = 0x800000;
        busy = D_80075028.unk00 | D_80075028.unk10 | g_sndStream.active;
        if (sfx1 != 0 && sfx2 != 0) {
            i = 11;
            track--;
            bit = 0x400000;
            while (1) {
                if (!(busy & (bit | (bit << 1)))) {
                    break;
                }
                i--;
                track--;
                bit >>= 1;
                if (i == 0) {
                    break;
                }
                // matching hack
                continue;
            }
        } else {
            for (i = 12; i != 0; i--, track--, bit >>= 1) {
                if (!(busy & bit)) {
                    break;
                }
            }
        }
        if (i != 0) {
            break;
        }
        func_800174E4(0, 0x40000000);
        if (busy == (D_80075028.unk00 | D_80075028.unk10 | g_sndStream.active)) {
            return;
        }
    } while (i == 0);
    if (sfx1 != 0) {
        func_80017880(track, cmd, bit, sfx1);
        sndReleaseVoiceFromTracks(track->unkF4.unk00);
    }
    if (sfx2 != 0) {
        if (sfx1 != 0) {
            track++;
            bit <<= 1;
        }
        func_80017880(track, cmd, bit, sfx2);
        sndReleaseVoiceFromTracks(track->unkF4.unk00);
        if (sfx1 != 0) {
            track->flags |= 0x10000;
        }
    }
    D_80077288[2] |= 0x110;
}

/**
 * @brief Resolves two consecutive sample addresses from the instrument table.
 *
 * Looks up the halfword at D_80074ED0[a2*2] (first entry) and
 * D_80074ED0[a2*2+1] (second entry). For each, if the value is not
 * 0xFFFF, adds D_80074EDC as a base to produce a sample pointer;
 * otherwise returns 0. Stores results at *a0 and *a1.
 *
 * @param a0 Output pointer for first sample address.
 * @param a1 Output pointer for second sample address.
 * @param a2 Instrument index (low 10 bits used).
 */
void func_80017C9C(s32 *a0, s32 *a1, s32 a2) {
    s32 val;

    a2 &= 0x3FF;
    a2 <<= 1;
    if (D_80074ED0[a2] != 0xFFFF) {
        val = (s32)(D_80074EDC + D_80074ED0[a2]);
    } else {
        val = 0;
    }
    a2++;
    *a0 = val;
    if (D_80074ED0[a2] != 0xFFFF) {
        val = (s32)(D_80074EDC + D_80074ED0[a2]);
    } else {
        val = 0;
    }
    *a1 = val;
}

void func_80017D14(SoundMusicConfig *seq, SoundSeqTrack *tracks) {
    s32 value = seq->unk04;
    s32 mask;
    s32 bit;
    s32 *flags;

    if (value != 0) {
        mask = value;
        bit = 1;
        flags = (s32 *)((s32)tracks + 0xF8);
        do {
            if (mask & bit) {
                mask ^= bit;
                *flags |= 3;
            }
            flags = (s32 *)((s32)flags + 0x110);
            bit <<= 1;
        } while (mask != 0);
    }
}

void func_80017D5C(void) {
    s32 value = D_80075028.unk00;
    s32 mask;
    s32 bit;
    SoundSeqTrack *tracks;
    s32 *flags;

    if (value != 0) {
        mask = value;
        bit = 0x1000;
        tracks = D_80072F70;
        flags = (s32 *)((s32)tracks + 0xF8);
        do {
            if (mask & bit) {
                mask ^= bit;
                *flags |= 3;
            }
            flags = (s32 *)((s32)flags + 0x110);
            bit <<= 1;
        } while (mask != 0);
    }
}

void func_80017DB0(u8 *seq, s32 swap) {
    SoundSeqTrack *track;
    SoundMusicConfig *config;
    func_8001A1E8_data *old;
    u32 active;
    u32 bit;
    u32 allVoices;
    s32 flags;
    s32 delta;
    s32 i;
    u32 stored;

    if (swap == 0) {
        func_8001A57C((u32 *)&D_80073DF0, (u32 *)g_sndSeqState, 0x70);
        func_8001A57C((u32 *)D_80075080, (u32 *)D_80070D60, 0x2200);
    } else {
        func_8001A5FC((u32 *)&D_80073DF0, (u32 *)g_sndSeqState, 0x70);
        func_8001A5FC((u32 *)D_80075080, (u32 *)D_80070D60, 0x2200);
    }
    flags = g_sndSeqState->unk00 & ~0x600;
    g_sndSeqState->unk00 = flags;
    g_sndSeqState->unk00 = (((func_8001A1E8_data *)seq)->unk14 == D_80073DE4) ? flags | 0x400 : flags | 0x200;
    track = D_80070D60;
    i = 32;
    bit = 1;
    old = g_sndSeqState->unk2C;
    g_sndSeqState->unk2C = (func_8001A1E8_data *)seq;
    D_80077288[2] |= 0x90;
    delta = seq - (u8 *)old;
    g_sndSeqState->unk30 = (s16 *)((u8 *)g_sndSeqState->unk30 + delta);
    active = g_sndSeqState->unk04;
    g_sndSeqState->unk34 = (func_8001B820_entry *)((u8 *)g_sndSeqState->unk34 + delta);
    g_sndSeqState->unk10 = g_sndSeqState->unk14;
    do {
        if (active & bit) {
            track->unk00 += delta;
            track->unk14 = (func_8001B42C_entry *)((u8 *)track->unk14 + delta);
            track->unk04[0] += delta;
            track->unk04[1] += delta;
            track->unk04[2] += delta;
            track->unk04[3] += delta;
            track->volume += 2;
            track->volumeBase += 2;
            track->unkF4.updateFlags |= 0x1FF93;
            func_80016DB4(g_sndSeqState->unk00, track);
        } else {
            track->volume = 4;
            track->volumeBase = 2;
            track->unk00 = D_800517F0;
        }
        track->unkF4.unk00 = 0x18;
        i--;
        track++;
        bit <<= 1;
    } while (i != 0);
    active = (D_80073CA8 != NULL) ? sndBuildVoiceMask(D_80073C34, D_80073CA8->unk04) : 0;
    allVoices = 0xFFFFFF;
    g_sndSeqState->unk18 = 0;
    D_80073DF0.unk5E = 0;
    D_80075028.unk0C |= ~active & (~(D_80075028.unk00 | D_800772A4) & allVoices);
    D_80077288[2] |= 0x100;
    if (swap != 0) {
        bit = 0x30;
        delta = g_sndSeqState->unk48 + g_sndSeqState->unk4C * g_sndSeqState->unk5A;
        delta &= ~0xFFF;
        if (g_sndSeqState->unk5A != 0) {
            bit = g_sndSeqState->unk5A;
        }
        if (D_80073CA8 != NULL) {
            g_sndSeqState->unk48 = D_80073CA8->unk48;
        }
        g_sndSeqState->unk48 &= ~0xFFF;
        delta -= g_sndSeqState->unk48;
        if (delta != 0) {
            g_sndSeqState->unk5A = bit;
            g_sndSeqState->unk4C = delta / (s32)bit;
        } else {
            g_sndSeqState->unk5A = 0;
        }
    }
    if (D_800772F4 & 1) {
        config = g_sndSeqState;
        stored = config->unk04;
        config->unk04 = 0;
        config->unk1C = stored;
    }
}

/**
 * @brief Plays or stops a sound depending on the active sequence ID.
 *
 * Checks the halfword at D_80073DF0+0x5E (active sequence ID). If non-zero
 * and matches a0[8], calls func_80017DB0(a0[0], 0) to stop. Otherwise calls
 * func_8001708C(a0[0], -1) and stores a0[8] into the structure pointed to
 * by g_sndSeqState at offset 0x5E.
 *
 * @param a0 Pointer to a sound config structure (word 0 = track ID, halfword 8 = sequence ID).
 */
void func_80018158(SndVoice *voice) {
    if (D_80073DF0.unk5E != 0 && D_80073DF0.unk5E == voice->params[2]) {
        func_80017DB0((u8 *)voice->params[0], 0);
    } else {
        func_8001708C((u8 *)voice->params[0], -1);
        g_sndSeqState->unk5E = voice->params[2];
    }
}

/**
 * @brief Configure audio track and update playback globals.
 *
 * Calls func_8001708C with the first and fourth fields of the input struct,
 * copies field8 to g_sndSeqState->0x5E, and sets D_80074EB0 to field10-1
 * (or 0 if field10 is 0).
 *
 * @param a0 Pointer to audio config struct.
 */
void sndConfigureTrackPlayback(s32 *a0) {
    s32 val, result;
    func_8001708C((u8 *)a0[0], a0[3]);
    g_sndSeqState->unk5E = *(u16 *)((u8 *)a0 + 8);
    result = 0;
    /* The val load is block-extracted so result binds to $a0: a plain
       `val = a0[4];` here schedules result into $v1 and mismatches. */
    do {
        val = a0[4];
    } while (0);
    if (val != 0) {
        result = val - 1;
    }
    D_80074EB0 = result;
}

void func_80018234(void) {
    SoundSeqTrack *track;
    u32 mask;
    u32 i;

    if (g_sndSeqState->unk04 == 0) {
        return;
    }
    if (D_80073CA8 != NULL && D_80073CA8->unk04 != 0) {
        track = D_80073C34;
        i = 0;
        mask = D_80073CA8->unk04;
        while (i < 32) {
            if ((mask & (1 << i)) && track->unkF4.unk00 != 0x18) {
                D_80075028.unk0C |= 1 << track->unkF4.unk00;
            }
            i++;
            track++;
        }
        D_80073CA8 = NULL;
    }
    func_8001A57C((u32 *)g_sndSeqState, (u32 *)&D_80073DF0, 0x70);
    func_8001A57C((u32 *)D_80070D60, (u32 *)D_80075080, 0x2200);
    track = D_80075080;
    for (i = 0; i < 32; i++, track++) {
        func_80016E08(D_80073DF0.unk00, track);
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018358);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018438);

/**
 * @brief Process a sound voice and compute the remaining play count.
 *
 * Calls func_80018158 on the voice structure, then reads the play count
 * at offset +0x0C. If non-zero, decrements by 1 and stores to D_80074EB0;
 * otherwise stores 0.
 *
 * @param a0 Pointer to the sound voice structure.
 */
void sndPlayVoiceWithCount(SndVoice *voice) {
    s32 val, result;
    func_80018158(voice);
    val = voice->playCount;
    result = 0;
    if (val != 0) {
        result = val - 1;
    }
    D_80074EB0 = result;
}

/**
 * @brief Saves current sound parameters, replaces with defaults, and triggers playback.
 *
 * Saves the first two words of the structure at @p a0, then overwrites
 * with default volume/pan parameters (0x400, 0x1000000, 0x80, 0x7F, 0)
 * and calls func_80017AAC with the saved values.
 *
 * @param a0 Pointer to a sound parameter structure (5 words).
 */
void sndPlayWithDefaults(s32 *a0) {
    s32 a1 = a0[0];
    s32 a2 = a0[1];
    a0[0] = 0x400;
    a0[1] = 0x1000000;
    a0[2] = 0x80;
    a0[3] = 0x7F;
    a0[4] = 0;
    func_80017AAC(a0, a1, a2, 0);
}

/**
 * @brief Set default sound parameters, look up voice, and play.
 *
 * Resolves the SPU voice address via func_80017C9C, sets default volume
 * (0x2000000), pan (0x80), and envelope (0x7F) on the parameter structure,
 * looks up the instrument group via D_80074ED8, and plays via func_80017AAC.
 *
 * @param a0 Pointer to the sound parameter structure.
 */
void sndPlaySoundEffect(s32 *a0) {
    s32 local10, local14;

    func_80017C9C(&local10, &local14, a0[0]);
    a0[1] = 0x2000000;
    a0[2] = 0x80;
    a0[3] = 0x7F;
    a0[4] = sndGetBankGroup(D_80074ED8[a0[0]]);
    func_80017AAC(a0, local10, local14, 0);
}

/**
 * @brief Look up and play a sound voice by index.
 *
 * Resolves the SPU voice address via func_80017C9C, reads the sample pitch
 * from D_80074ED8 table, processes it with sndGetBankGroup, stores the result
 * at offset +0x10 of the parameter structure, and plays via func_80017AAC.
 *
 * @param a0 Pointer to the sound parameter structure.
 */
void sndPlaySoundByIndex(s32 *a0) {
    s32 local10, local14;

    func_80017C9C(&local10, &local14, a0[0]);
    a0[4] = sndGetBankGroup(D_80074ED8[a0[0]]);
    func_80017AAC(a0, local10, local14, 0);
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018784);

/** @brief Calls func_800174E4 with two words loaded from the input pointer. */
void sndStopTrack(s32 *a0) {
    func_800174E4(a0[0], a0[1]);
}

/**
 * @brief Clears 12 entries (stride 16) in the D_80074F20 array.
 * Writes zero to offset 0 of each entry, iterating backward.
 */
void sndClearVoicePool(void) {
    s32 i;
    for (i = 12; i != 0; i--) {
        D_80074F20[i - 1].cmd = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018908);

/**
 * @brief Sets volume/pan on the active sequence and marks dirty tracks.
 *
 * Checks if a0[0] matches the active sequence ID. If so, writes the
 * volume (a0[1] & 0x7F shifted) and clears the pan fade counter on the
 * appropriate bank. Then calls func_80017D14 with the track array to
 * mark matching tracks dirty.
 *
 * @param a0 Pointer to a 2-word config struct: [seqId, volumeParam].
 */
INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018A74);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018B28);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018C48);

/**
 * @brief Clears the sound fade counter and sets SPU transfer address.
 *
 * Sets D_80074FE4 to 0, loads the halfword at @p a0, shifts it left 16,
 * stores to D_80075078, and calls sndSetCdVolume.
 *
 * @param a0 Pointer to a halfword containing the SPU address high bits.
 */
void sndSetFadeAndTransfer(u16 *a0) {
    D_80074FE4 = 0;
    D_80075078 = *a0 << 16;
    sndSetCdVolume();
}

/**
 * @brief Computes fade step from input parameters and stores to globals.
 *
 * Reads a duration from a0[0] (default 1 if zero), computes the per-tick
 * fade delta as ((a0[4] << 16) - D_80075078) / duration, stores duration
 * to D_80074FE4 and delta to D_80074FE0.
 *
 * @param a0 Pointer to a fade config structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018D74);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018DDC);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018E4C);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018F34);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800190B4);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80019130);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800191F8);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800192D8);

/**
 * @brief Applies expression value to all active tracks matching a bitmask.
 *
 * Iterates 12 tracks in D_80072F70 (stride 0x110). For each track whose
 * bit is set in D_80075028.unk00 and whose flags do NOT have bit 0x2000000,
 * reads the expression byte from a0, clears the halfword at +0x8A,
 * stores (byte & 0x7F) << 8 at +0x6A, and ORs 0x3 into updateFlags.
 *
 * @param a0 Pointer to the expression value byte.
 */
INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80019450);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800194C8);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_8001958C);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_8001966C);

/**
 * @brief Applies a note byte to all active tracks without flag 0x2000000.
 *
 * Iterates 12 tracks in D_80072F70 (stride 0x110). For each track whose
 * flags at +0x24 do NOT have bit 0x2000000 set, reads the note byte from
 * a0, clears the halfword at +0x84, shifts the byte left 8 and stores
 * at +0x3C, and ORs 0x10 into updateFlags at +0xF8.
 *
 * @param a0 Pointer to a byte containing the note value.
 */
void func_800197F4(u8 *param) {
    s32 i;
    SoundSeqTrack *track = (SoundSeqTrack *)D_80072F70;

    for (i = 12; i != 0; i--) {
        if (!(track->keyOnMask & 0x2000000)) {
            track->loopLimit = *param << 8;
            track->pitchFadeCounter = 0;
            track->unkF4.updateFlags |= 0x10;
        }
        track++;
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_8001984C);

