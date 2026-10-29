#ifndef SND_NOTE_H
#define SND_NOTE_H

#include "sound.h"

extern void sndSetCdVolume(void);
extern void func_8001A57C(u32 *src, u32 *dst, u32 size);
extern void func_8001A5FC(u32 *a, u32 *b, u32 size);
extern s32 sndAdjustNoteOctave(s32 a0, s32 a1);
extern s32 func_8001AD60(void);

#endif /* SND_NOTE_H */
