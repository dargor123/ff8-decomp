#include "common.h"
#include "psxsdk/libapi.h"
#include "psxsdk/libetc.h"
#include "psxsdk/kernel.h"
#include "sound.h"
#include "snd_note.h"
#include "snd_bank.h"
#include "snd_dma.h"
#include "snd_init.h"
#include "snd_voice.h"

void func_8001BAA8(SoundSeqTrack *track, u32 bit);

extern s16 D_8007507A;
extern s32 D_800516A0;
extern u16 D_800516A4;
extern s32 D_800516A8[];
extern u8 D_800516F8[];
extern u8 D_80051758[];
extern u16 D_800517D8[];
extern s32 D_800517F4[];
extern u8 D_80077286;
extern void (*D_80052698[])(SoundSeqTrack *track, u32 bit);
extern void (*D_80052818[])(SoundSeqTrack *track, u32 bit);

/**
 * @brief Sets the CD audio input volume on the SPU to the current global value.
 *
 * Reads the volume level from D_8007507A and writes it to both the left
 * (0x1F801DB0) and right (0x1F801DB2) CD audio volume registers, producing
 * equal stereo volume.
 */
void sndSetCdVolume(void) {
    s32 val = D_8007507A;
    *(s16 *)0x1F801DB0 = val;
    *(s16 *)0x1F801DB2 = val;
}

void func_8001A57C(u32 *src, u32 *dst, u32 size) {
    u32 w0;
    u32 w1;
    u32 w2;
    u32 w3;

    size >>= 2;
    while (size >> 2) {
        w0 = src[0];
        w1 = src[1];
        w2 = src[2];
        w3 = src[3];
        dst[0] = w0;
        dst[1] = w1;
        dst[2] = w2;
        dst[3] = w3;
        src += 4;
        dst += 4;
        size -= 4;
    }
    while (size != 0) {
        *dst++ = *src++;
        size--;
    }
}

void func_8001A5FC(u32 *a, u32 *b, u32 size) {
    u32 a0;
    u32 b0;
    u32 a1;
    u32 b1;

    size >>= 2;
    while (size >> 1) {
        a0 = a[0];
        b0 = b[0];
        a1 = a[1];
        b1 = b[1];
        b[0] = a0;
        b[1] = a1;
        a[0] = b0;
        a[1] = b1;
        a += 2;
        b += 2;
        size -= 2;
    }
    if (size != 0) {
        a0 = *a;
        b0 = *b;
        *b = a0;
        *a = b0;
    }
}

void func_8001A674(void) {
    SoundMusicConfig *config;
    SoundStream *stream;
    SoundSeqTrack *track;
    u32 active;
    u32 bit;
    s32 val;
    s32 vol;

    if (D_80074FE4 != 0) {
        D_80074FE4--;
        D_80075078 += D_80074FE0;
        sndSetCdVolume();
    }
    stream = &g_sndStream;
    if (stream->active != 0 && stream->panTarget != 0) {
        stream->panTarget--;
        vol = stream->panVolume;
        val = vol + stream->unk44;
        if ((val & 0xFF00) != (vol & 0xFF00)) {
            if (D_8007728C & 2) {
                vol = (vol * D_80051954) >> 16;
                spuSetVoiceVolume(stream->voiceIdx, vol, vol, 0);
                spuSetVoiceVolume(stream->voiceIdx + 1, vol, vol, 0);
            } else {
                vol = val << 15;
                vol >>= 16;
                spuSetVoiceVolume(stream->voiceIdx, vol, 0, 0);
                spuSetVoiceVolume(stream->voiceIdx + 1, 0, vol, 0);
            }
        }
        D_800772D8 = val & 0xFFFF;
    }
    if (D_80073E62 != 0) {
        D_80073E62--;
        D_80077284 += D_80073DEC;
    }
    if (D_80073E60 != 0) {
        D_80073E60--;
        val = D_80077280 + D_80073DE8;
        if ((val & 0xFF0000) != (D_80077280 & 0xFF0000)) {
            for (bit = 32, track = D_80070D60; bit != 0; bit--, track++) {
                track->unkF4.updateFlags |= 0x10;
            }
        }
        D_80077280 = val;
    }
    if (g_sndSeqState->unk04 != 0 && g_sndSeqState->unk58 != 0) {
        g_sndSeqState->unk58--;
        val = g_sndSeqState->unk50 + g_sndSeqState->unk54;
        if ((val & 0x7F0000) != (g_sndSeqState->unk50 & 0x7F0000)) {
            func_80017D14(g_sndSeqState, D_80070D60);
        }
        g_sndSeqState->unk50 = val;
    }
    config = D_80073CA8;
    if (config != NULL && config->unk04 != 0 && config->unk58 != 0) {
        config->unk58--;
        val = config->unk50 + config->unk54;
        if ((val & 0x7F0000) != (config->unk50 & 0x7F0000)) {
            func_80017D14(config, D_80073C34);
        }
        D_80073CA8->unk50 = val;
    }
    if (D_80075028.unk00 != 0) {
        active = D_80075028.unk00;
        track = D_80072F70;
        bit = 0x1000;
        do {
            if (active & bit) {
                if (track->exprFade != 0) {
                    track->exprFade--;
                    val = (s16)track->expression + track->unkE0;
                    if ((val & 0xFF00) != ((s16)track->expression & 0xFF00)) {
                        track->unkF4.updateFlags |= 3;
                    }
                    track->expression = val;
                }
                if (track->pitchModCounter != 0) {
                    track->pitchModCounter--;
                    val = track->pitchModDepth + track->unkDC;
                    if ((val & 0xFF00) != (track->pitchModDepth & 0xFF00)) {
                        track->unkF4.updateFlags |= 3;
                    }
                    track->pitchModDepth = val;
                }
                if (track->pitchFadeCounter != 0) {
                    track->pitchFadeCounter--;
                    val = track->loopLimit + track->panRaw;
                    if ((val & 0xFF00) != (track->loopLimit & 0xFF00)) {
                        track->unkF4.updateFlags |= 0x10;
                    }
                    track->loopLimit = val;
                }
                active ^= bit;
            }
            bit <<= 1;
            track++;
        } while (active != 0);
    }
}

u32 func_8001AA28(SoundSeqTrack *tracks, s32 isSecond) {
    SoundMusicConfig *config;
    SoundSeqTrack *track;
    u32 active;
    u32 bit;
    u32 tempo;
    u32 mul;
    u32 update;
    u32 flags;

    tempo = (u16)(g_sndSeqState->unk20 >> 16);
    mul = D_80077286;
    if (mul != 0) {
        if (mul < 0x80) {
            tempo += (tempo * mul) >> 7;
        } else {
            tempo = (tempo * mul) >> 8;
        }
    }
    config = g_sndSeqState;
    update = config->unk28 + tempo;
    config->unk28 = update;
    if ((update & 0xFFFF0000) != 0 || (D_800772F4 & 4)) {
        config->unk28 = update & 0xFFFF;
        do {
            track = tracks;
            bit = 1;
            active = g_sndSeqState->unk04;
            do {
                if (active & bit) {
                    track->volume--;
                    track->volumeBase--;
                    if (track->volume == 0) {
                        func_8001BAA8(track, bit);
                    } else if (track->volumeBase == 0) {
                        g_sndSeqState->unk18 |= bit;
                    }
                    func_800151C4(track, bit, 0);
                    active &= ~bit;
                }
                track++;
                bit <<= 1;
            } while (active != 0);
            config = g_sndSeqState;
            if (config->unk5C != 0) {
                config->unk5C--;
                config->unk20 += config->unk24;
            }
            config = g_sndSeqState;
            if (config->unk5A != 0) {
                config->unk5A--;
                config->unk48 += config->unk4C;
                flags = D_80077288[2];
                if (isSecond == 0) {
                    // matching hack
                    do { D_80077288[2] = flags | 0x80; } while (0);
                }
            }
            if (g_sndSeqState->unk68 != 0) {
                if (++g_sndSeqState->unk6A == g_sndSeqState->unk68) {
                    g_sndSeqState->unk6A = 0;
                    if (++g_sndSeqState->unk66 == g_sndSeqState->unk64) {
                        g_sndSeqState->unk66 = 0;
                        g_sndSeqState->unk6C++;
                        if (isSecond == 0 && D_80074EB0 != 0) {
                            D_80074EB0--;
                        }
                    }
                }
            }
        } while (isSecond == 0 && D_80074EB0 != 0);
    }
    return g_sndSeqState->unk04;
}

void func_8001ACCC(SoundMusicConfig *track, SoundSeqTrack *tracks) {
    s32 savedUnk04;
    s32 newVal;

    if (D_80074F10.unk08 != 0) {
        if (--D_80074F10.unk08 == 0) {
            savedUnk04 = track->unk04;
            track->unk5E = 0;
            track->unk04 = 0;
            track->unk10 = 0;
            track->unk14 = 0;
            track->unk18 = savedUnk04;
        } else {
            newVal = D_80074F10.unk00 + D_80074F10.unk04;

            if ((newVal & 0xFFFF0000) != (D_80074F10.unk00 & 0xFFFF0000)) {
                func_80017D14(track, tracks);
            }

            D_80074F10.unk00 = newVal;
        }
    }
}

s32 func_8001AD60(void) {
    SoundMusicConfig *config;
    SoundSeqTrack *track;
    u32 active;
    u32 bit;
    u32 update;
    s32 ticks;
    s32 rcnt;

    rcnt = GetRCnt(RCntCNT2);
    D_80072F60[1] = D_80072F60[0];
    D_80072F60[0] = VSync(-1);
    active = D_80072F60[0] - D_80072F60[1];
    ticks = 1;
    if (active != 0) {
        if (active < 9) {
            bit = active * 4;
            if ((u32)D_80072F68 < bit) {
                ticks += bit - D_80072F68;
            }
            D_80072F68 = (u32)D_80072F68 % bit;
        }
    }
    do {
        ticks--;
        D_80072F68++;
        D_800516A4++;
        if ((g_sndSeqState->unk18 | D_80075028.unk0C) != 0 || (D_80073CA8 != NULL && D_80073CA8->unk18 != 0)) {
            func_80016A7C();
        }
        if (D_80073CA8 != NULL && D_80073CA8->unk04 == 0) {
            D_80073CA8 = NULL;
        }
        if ((D_80077288[2] | g_sndSeqState->unk14 | D_80075028.unk08) != 0) {
            func_800164D8();
        } else if (D_80073CA8 != NULL && D_80073CA8->unk14 != 0) {
            func_800164D8();
        }
        if (g_sndSeqState->unk04 != 0) {
            func_8001AA28(D_80070D60, 0);
        }
        if (D_80073CA8 != NULL) {
            if (D_80073CA8->unk04 != 0) {
                g_sndSeqState = D_80073CA8;
                func_8001AA28(D_80073C34, 1);
                g_sndSeqState = &D_80073D70;
            }
            if ((D_80077288[1] & 0x100) && (D_800516A4 & 3) == 0) {
                func_8001ACCC(D_80073CA8, D_80073C34);
                if (D_80073CA8->unk04 == 0) {
                    D_80077288[1] &= ~0x100;
                }
            }
            if ((g_sndSeqState->unk04 | g_sndSeqState->unk1C) == 0 && D_80073CA8->unk04 != 0) {
                func_8001A57C((u32 *)D_80073CA8, (u32 *)g_sndSeqState, 0x70);
                func_8001A57C((u32 *)D_80073C34, (u32 *)D_80070D60, 0x2200);
                config = D_80073CA8;
                D_80073CA8 = NULL;
                config->unk5E = 0;
                config->unk04 = 0;
            }
        }
        if (D_80075028.unk00 != 0) {
            active = D_80075028.unk00;
            update = D_80075028.unk18 + (u16)(D_80075028.unk14 >> 16);
            D_80075028.unk18 = update;
            if ((update & 0xFFFF0000) != 0 || (D_800772F4 & 4)) {
                D_80075028.unk18 = update & 0xFFFF;
                bit = 0x1000;
                track = D_80072F70;
                do {
                    if (active & bit) {
                        if (!(D_800772F4 & 2) || (track->keyOnMask & 0x2000000)) {
                            track->unk54++;
                            track->volume--;
                            track->volumeBase--;
                            if (track->volume == 0) {
                                func_8001BAA8(track, bit);
                            } else if (track->volumeBase == 0) {
                                D_80075028.unk0C |= bit;
                                D_80075028.unk08 &= ~bit;
                            }
                            func_800151C4(track, bit, 1);
                        }
                        active ^= bit;
                    }
                    track++;
                    bit <<= 1;
                } while (active != 0);
            }
        }
        if ((D_800516A4 & 3) == 0) {
            func_8001A674();
        }
    } while (ticks != 0);
    rcnt = GetRCnt(RCntCNT2) - rcnt;
    if (rcnt <= 0) {
        rcnt += 0x44E8;
    }
    D_800516A8[0] = D_800516A8[1];
    D_800516A8[1] = D_800516A8[2];
    D_800516A8[2] = D_800516A8[3];
    D_800516A8[3] = rcnt;
    D_800516A0 = D_800516A8[0] + D_800516A8[1] + D_800516A8[2] + rcnt;
    return rcnt;
}

u8 func_8001B1F4(SoundSeqTrack *track) {
    u8 *p = track->unk00;
    u32 loopId = track->panEnvSpeed;
    u32 op;
    s32 sub;
    u32 len;

    op = *p;
    while (1) {
        if (op < 0x9A) {
            if (op >= 0x8F) {
                track->timerActive &= 0xFFFA;
            }
            return *p;
        }
        if (op < 0xA0) {
            return 0xA0;
        }
        len = D_800516F8[*p - 0xA0];
        if (len != 0) {
            p += len;
        } else {
            switch (*p) {
            case 0xF0:
            case 0xF1:
            case 0xF2:
            case 0xF3:
            case 0xF4:
            case 0xF5:
            case 0xF6:
            case 0xF7:
            case 0xF8:
            case 0xF9:
            case 0xFA:
            case 0xFB:
                return 0x83;
            case 0xFC:
                return 0x84;
            case 0xFD:
                return 0x8F;
            case 0xFE:
                p++;
                sub = *p;
                len = D_80051758[sub];
                if (len != 0) {
                    p += len;
                    break;
                }
                if (sub != 7) {
                    if (sub < 8) {
                        if (sub != 6) {
                            op = sub;
                            continue;
                        }
                    } else {
                        if (sub < 10) {
                            p++;
                            if (*p == track->unk70[loopId] + 1) {
                                p++;
                                loopId--;
                                loopId &= 3;
                                op = p[0] + (p[1] << 8);
                                p += (s16)op;
                            } else {
                                p += 3;
                            }
                        }
                        break;
                    }
                    p++;
                    op = p[0] + (p[1] << 8);
                    p += (s16)op;
                    break;
                }
                p++;
                op = *p;
                p++;
                if (op <= g_sndSeqState->unk60) {
                    op = p[0] + (p[1] << 8);
                    p += (s16)op;
                    break;
                }
                p += 2;
                break;
            case 0xC9:
                p++;
                if (*p == track->unk70[loopId] + 1) {
                    p++;
                    loopId--;
                    loopId &= 3;
                } else {
                    p = track->unk04[loopId];
                }
                break;
            case 0xCB:
            case 0xCD:
            case 0xD1:
            case 0xDB:
                track->timerActive &= 0xFFFA;
                p++;
                break;
            case 0xCA:
                if (!(track->flags & 0x200000)) {
                    p = track->unk04[loopId];
                    break;
                }
            default:
                track->timerActive &= 0xFFFA;
                return 0xA0;
            }
        }
        op = *p;
    }
}

/**
 * @brief Adjusts a note/instrument value based on a flag and range check.
 *
 * If bit 10 of @p a0 is set and @p a1 is in range [0x40, 0x80), adds 0x20
 * to @p a1 (shifting the value into a higher register/octave).
 *
 * @param a0 Flags value; bit 10 enables the adjustment.
 * @param a1 Note or instrument index to potentially adjust.
 * @return Adjusted value, or @p a1 unchanged if conditions are not met.
 */
s32 sndAdjustNoteOctave(s32 a0, s32 a1) {
    if ((a0 & 0x400) && (u32)a1 >= 0x40 && (u32)a1 < 0x80) {
        return a1 + 0x20;
    }
    return a1;
}

void func_8001B42C(SoundSeqTrack *track, u32 key) {
    func_8001B42C_entry *entry;
    SndInstrument *inst;
    s32 flags;
    s32 id;

    if ((u32)track->unkE8 < key || track->unkE8 == 0xFF) {
        entry = track->unk14;
        if (entry[1].unk05 != 0) {
            while (1) {
                if (!(entry->unk02 < key)) {
                    break;
                }
                entry++;
                if (entry[1].unk05 == 0) {
                    break;
                }
                // matching hack
                if (track == NULL) {
                }
            }
        }
    } else if (key < (u32)track->unkE8) {
        entry = track->unk14;
        if (entry[1].unk05 != 0) {
            while (1) {
                if (key < entry[1].unk01) {
                    break;
                }
                entry++;
                if (entry[1].unk05 == 0) {
                    break;
                }
            }
        }
    } else {
        return;
    }
    flags = track->flags;
    id = sndAdjustNoteOctave(g_sndSeqState->unk00, entry->unk00);
    track->instrument = id;
    inst = &D_80073E68[id];
    track->unkF4.startAddr = inst->sampleAddr;
    track->unkF4.repeatAddr = inst->loopAddr;
    if (!(flags & 0x1000000)) {
        track->unkF4.adsrLow = entry->unk03 << 8;
    } else {
        track->unkF4.adsrLow &= 0x7F00;
    }
    track->unkF4.adsrLow |= inst->adsrLow & 0x80FF;
    if (!(flags & 0x8000000)) {
        track->unkF4.adsrHigh &= 0x201F;
        track->unkF4.adsrHigh |= entry->unk04 << 6;
    } else {
        track->unkF4.adsrHigh &= 0x3FDF;
    }
    switch (entry->unk05) {
    case 3:
        track->unkF4.adsrHigh |= 0x4000;
        break;
    case 5:
        track->unkF4.adsrHigh |= 0x8000;
        break;
    case 7:
        track->unkF4.adsrHigh |= 0xC000;
        break;
    }
    if (!(flags & 0x10000000)) {
        track->unkF4.adsrHigh &= 0xFFE0;
        track->unkF4.adsrHigh |= entry->unk06;
    }
    track->unkF4.adsrHigh |= inst->adsrHigh & 0x20;
    track->unkF4.sweep = entry->unk07;
}

s32 func_8001B690(SndInstrument *inst, s32 key, u32 fineTuning, s32 *out) {
    s32 pitch;
    s32 note;
    s32 octave;
    s32 scale;
    note = key - inst->unk0A;
    while (note < 0) {
        note += 12;
    }
    octave = note / 12;
    note -= octave * 12;
    scale = inst->unk08;
    if (scale == 0) {
        pitch = D_800517F4[note] << 8;
    } else if (scale < 0) {
        pitch = ((u32)D_800517F4[note] * (u16)inst->unk08) >> 8;
    } else {
        pitch = ((u32)D_800517F4[note] * scale) >> 7;
        pitch += D_800517F4[note] << 8;
    }
    fineTuning &= 0xFF;
    if (fineTuning != 0) {
        if (fineTuning < 0x80) {
            *out = ((u32)pitch * fineTuning) >> 7;
        } else {
            *out = (((u32)pitch * fineTuning) >> 8) - pitch;
        }
    }
    if (key < inst->unk0A) {
        do {
            *out >>= 1;
            pitch >>= 1;
            key += 12;
        } while (key < inst->unk0A);
    } else {
        note = (key - inst->unk0A) / 12;
        if (note != 0) {
            *out <<= note;
            pitch <<= note;
        }
    }
    *out >>= 8;
    pitch >>= 8;
    return pitch & 0xFFFF;
}

s32 func_8001B820(SoundSeqTrack *track, u32 bit, s32 idx) {
    func_8001B820_entry *entry;
    SndInstrument *inst;
    s32 flags;
    s32 id;
    s32 pitch;

    entry = g_sndSeqState->unk34;
    entry += idx;
    g_sndSeqState->unk10 |= bit;
    if (g_sndSeqState->unk14 & bit) {
        g_sndSeqState->unk18 |= bit;
    }
    flags = track->flags;
    id = sndAdjustNoteOctave(g_sndSeqState->unk00, entry->unk00);
    track->instrument = id;
    inst = &D_80073E68[id];
    track->unkF4.startAddr = inst->sampleAddr;
    track->unkF4.repeatAddr = inst->loopAddr;
    if (!(flags & 0x1000000)) {
        track->unkF4.adsrLow = entry->unk02 << 8;
    } else {
        track->unkF4.adsrLow &= 0x7F00;
    }
    track->unkF4.adsrLow |= inst->adsrLow & 0x80FF;
    if (!(flags & 0x8000000)) {
        track->unkF4.adsrHigh &= 0x201F;
        track->unkF4.adsrHigh |= entry->unk03 << 6;
    } else {
        track->unkF4.adsrHigh &= 0x3FDF;
    }
    switch (entry->unk04) {
    case 3:
        track->unkF4.adsrHigh |= 0x4000;
        break;
    case 5:
        track->unkF4.adsrHigh |= 0x8000;
        break;
    case 7:
        track->unkF4.adsrHigh |= 0xC000;
        break;
    }
    if (!(flags & 0x10000000)) {
        track->unkF4.adsrHigh &= 0xFFE0;
        track->unkF4.adsrHigh |= entry->unk05;
    }
    track->unkF4.adsrHigh |= inst->adsrHigh & 0x20;
    pitch = func_8001B690(inst, entry->unk01, track->detuneTarget, &track->timing);
    track->unkF4.sweep = entry->unk06;
    track->notePitch = ((entry->unk07 & 0x7F) + 0x40) << 8;
    if ((s8)entry->unk07 < 0) {
        g_sndSeqState->unk40 |= bit;
    } else {
        g_sndSeqState->unk40 &= ~bit;
    }
    D_80077288[2] |= 0x100;
    return pitch;
}

void func_8001BAA8(SoundSeqTrack *track, u32 bit) {
    u32 op;
    u32 value;
    u32 depth;
    u32 base;
    u16 len;
    s32 target;
    s32 out;

    do {
        op = *track->unk00++;
        if (op >= 0xA0) {
            if (op == 0xFE) {
                value = *track->unk00++;
                D_80052818[value](track, bit);
            } else if (op - 0xF0 < 0xE) {
                op = (op - 0xF0) * 11;
                track->volume = *track->unk00++;
            } else {
                if (op == 0xFF) {
                    op = 0xA0;
                } else if (op == 0xCA && (track->flags & 0x200000)) {
                    D_80075028.unk0C |= bit;
                    op = 0xA0;
                }
                D_80052698[op - 0xA0](track, bit);
            }
        }
        track->unk6E++;
    } while (op > 0xA0);
    if (op == 0xA0) {
        if (track->voiceActive == 0) {
            g_sndSeqState->unk18 |= bit;
        }
        return;
    }
    value = func_8001B1F4(track);
    if ((s16)track->volumeAccum != 0) {
        track->volume = track->volumeBase = track->volumeAccum;
    }
    if (track->volume != 0) {
        if (value >= 0x8F || (value < 0x84 && !(track->timerActive & 5))) {
            track->volumeBase -= 2;
        }
    } else {
        track->volume = D_800517D8[op % 11];
        len = track->volume;
        if ((value < 0x84 || value >= 0x8F) && !(track->timerActive & 5)) {
            len -= 2;
        }
        track->volumeBase = len;
    }
    if (track->voiceActive == 0 && (track->flags & 0x40)) {
        track->volumeBase = track->volume;
    }
    track->volumeDelta = track->volume;
    track->unkF4.updateFlags |= 0x4000;
    if (op >= 0x8F) {
        if (track->voiceActive == 0) {
            g_sndSeqState->unk14 &= ~bit;
            if ((u32)track->unkF4.unk00 < 24) {
                g_sndSeqState->unk18 |= bit;
            }
        }
        track->duration = 0;
        track->portamento = 0;
        track->volumeLfoStop = 0;
        track->timerActive &= ~2;
        return;
    }
    if (op < 0x84) {
        op /= 11;
        op += track->panpot * 12;
        if (track->flags & 8) {
            value = func_8001B820(track, bit, op);
        } else {
            if (!(track->timerActive & 2)) {
                if (track->voiceActive == 0) {
                    if (D_8007728C & 0x10) {
                        // matching hack
                        do {
                            op += g_sndSeqState->unk38[op % 12];
                        } while (0);
                    }
                    if (track->flags & 0x1000) {
                        func_8001B42C(track, op);
                    }
                    g_sndSeqState->unk10 |= bit;
                    if (g_sndSeqState->unk14 & bit) {
                        g_sndSeqState->unk18 |= bit;
                    }
                    if (track->panFadeTarget != 0) {
                        track->panFade = track->panFadeTarget;
                        track->panShifted = track->pitchValue;
                        track->panTarget = track->unk5C;
                    }
                } else {
                    D_80075028.unk04 |= bit;
                }
                track->unk90 = 0;
            }
            if (track->duration != 0 && track->durationCounter != 0) {
                target = track->detune + op;
                op = track->durationCounter + (s16)track->ecField;
                track->durationDelta = track->duration;
                track->durationDelta2 = target - track->durationCounter - track->ecField;
                track->unkE8 = track->durationCounter - (track->detune - track->ecField);
            } else {
                track->unkE8 = op;
                op += (s16)track->detune;
            }
            value = func_8001B690(&D_80073E68[track->instrument], op, track->detuneTarget, &track->timing);
        }
        track->loopCounter = value;
        if (track->voiceActive == 0) {
            g_sndSeqState->unk14 |= bit;
        } else {
            D_80075028.unk08 |= bit;
        }
        track->unkF4.updateFlags |= 0x13;
        op = track->flags;
        if (op & 1) {
            depth = (track->delaySend & 0x7F00) >> 8;
            if (!(track->delaySend & 0x8000)) {
                base = (depth * ((value * 15) >> 8)) >> 7;
            } else {
                base = (depth * value) >> 7;
            }
            track->unkA8 = base;
            if (!(track->timerActive & 2)) {
                track->unk18 = D_80052258[track->unkA6];
                track->unkA0 = track->unk9E;
                track->unkA4 = 1;
            }
        }
        if (op & 2) {
            if (!(track->timerActive & 2)) {
                track->unk1C = D_80052258[track->unkBA];
                track->unkB4 = track->unkB2;
                track->unkB8 = 1;
            }
        }
        track->portamento = 0;
        track->volumeLfoStop = 0;
        track->cmdDataPtr = 0;
    }
    track->timerActive = (track->timerActive & ~2) | ((track->timerActive & 1) << 1);
    if ((s16)track->durationDelta2 != 0) {
        track->unkE8 += track->durationDelta2;
        value = func_8001B690(&D_80073E68[track->instrument], track->unkE8 + (s16)track->detune, track->detuneTarget, &out) << 16;
        track->unk90 = track->durationDelta;
        track->unk4C = (s32)(value - ((track->loopCounter << 16) + track->cmdDataPtr)) / track->unk90;
        track->durationDelta2 = 0;
    }
    track->durationCounter = track->unkE8;
    track->ecField = track->detune;
}
