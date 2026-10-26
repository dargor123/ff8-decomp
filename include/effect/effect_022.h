#ifndef EFFECT_EFFECT_022_H
#define EFFECT_EFFECT_022_H

#include "effect.h"

typedef struct {
    /** 0x00 */ u8 pad000[0x30];
    /** 0x30 */ VECTOR unk030;
    /** 0x40 */ u8 pad040[0x70 - 0x40];
} func_801A722C_arg0;

extern u8 D_801BA9F0;
extern u8 D_801BB308;
extern u8 D_801C7308;
extern u8 D_801E3D88[];
extern u8 D_801E3DA8[];
extern u8 *D_801E3DDC;
extern u8 *D_801E3DE0;
extern u8 *D_801E3DE4;
extern s32 D_801E3DE8;
extern s32 D_801E3EB8;
extern s32 D_801E3EC8;
extern s32 D_801E40F8;
extern s32 D_801E4108;
extern s32 D_801E4338;
extern s32 D_801E4348;
extern s32 D_801EC688;
extern s32 D_801EC698;
extern s32 D_801ED278;
extern s32 D_801ED288;
extern s32 D_801ED308;
extern EffectRenderPart D_801ED318;
extern s32 D_801ED348;
extern s32 D_801F3108;
extern EffectRender D_801F3118;

void func_801A1508(EffectRender *arg0, void *arg1, EffectRenderPart *arg2, void *arg3, BattleEffectSlot *arg4, s32 arg5);
void func_801A1E00(EffectRender *arg0);
void func_801A1F3C(EffectEntity *arg0);
void *func_801A1F64(void *arg0, void *arg1, s32 arg2, EffectEntity *arg3);
void func_801A22B4(EffectEntity *arg0);
s32 func_801A5644(EffectEntity *arg0);
s32 func_801A6AC0(EffectEntity *arg0);
s32 func_801A722C(EffectEntity *arg0);
s32 func_801A7580(EffectEntity *arg0);
void func_801A76FC(EffectEntity *arg0);
void func_801A776C(EffectEntity *arg0);
void func_801A813C(EffectEntity *arg0);
void func_801A8184(EffectEntity *arg0);
void func_801A81E0(EffectEntity *arg0);
void func_801A8210(EffectEntity *arg0);
void func_801A824C(EffectEntity *arg0);
void func_801A8C00(EffectEntity *arg0);
void func_801A8C14(EffectEntity *arg0);
void func_801A8C6C(EffectEntity *arg0);
void func_801A8CBC(EffectEntity *arg0);

#endif /* EFFECT_EFFECT_022_H */
