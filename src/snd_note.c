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
extern u8 D_80077286;

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

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B1F4);

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

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B42C);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B690);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B820);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001BAA8);
