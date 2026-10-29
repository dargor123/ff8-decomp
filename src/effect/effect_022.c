/**
 * @file effect_022.c
 * @brief Bio
 */
#include "common.h"
#include "battle.h"
#include "effect.h"
#include "battle/bc_object8.h"
#include "battle/bc_object11.h"
#include "battle/bc_object12.h"
#include "battle/bc_object13.h"
#include "effect/lib/common.h"
#include "effect/lib/entity.h"
#include "effect/lib/step.h"
#include "effect/effect_022_0D00.h"
#include "effect/effect_022.h"

static void func_801A74CC(EffectEntity *arg0);
static void func_801A764C(EffectEntity *arg0);
static void func_801A77C8(EffectEntity *arg0);
static void func_801A7870(EffectEntity *arg0);
static s32 func_801A7878(EffectEntity *arg0);
static void func_801A7DA8(EffectEntity *arg0);
static void func_801A7F90(EffectEntity *arg0);
static void func_801A82A8(EffectEntity *arg0);
static s32 func_801A82B0(EffectEntity *arg0);
static void func_801A8750(EffectEntity *arg0);
static void func_801A87B8(EffectEntity *arg0);
static void func_801A8820(EffectEntity *arg0);
static void func_801A8888(EffectEntity *arg0);
static void func_801A8A50(EffectEntity *arg0);
static void func_801A8CEC(EffectEntity *arg0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A568C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5724);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5820);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5A40);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5B2C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5C14);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5DB8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5DF4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5E30);

void func_801A5E4C(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5E54);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A60B8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6234);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A66D4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A67E8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6934);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6A7C);

void func_801A6AB8(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6AC0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6BC0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6C48);

void func_801A6CFC(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6D04);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6DD0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6E50);

void func_801A6F18(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6F20);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6FCC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A700C);

void func_801A7098(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A70A0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7148);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A71DC);

void func_801A7224(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A722C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A72F8);

static void func_801A74CC(EffectEntity *arg0) {
    // 0x60..0x65 holds an SVECTOR on this path, overlaying unk060 and unk063.
    arg0->unk058 += ((SVECTOR *)arg0->unk060)->vx;
    arg0->unk05A += ((SVECTOR *)arg0->unk060)->vy;
    arg0->unk05C += ((SVECTOR *)arg0->unk060)->vz;
    arg0->pos.vx += arg0->unk058;
    arg0->pos.vy += arg0->unk05A;
    arg0->pos.vz += arg0->unk05C;
    if (effectStepCounter(arg0)) {
        arg0->flags |= EFFECT_FLAG_STOP;
        arg0->pc++;
    }
}

void func_801A7578(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7580);

static void func_801A764C(EffectEntity *arg0) {
    BattleEffectSlot *slot = &D_800EF2D0[arg0->unk02D];

    func_801A1508(&D_801F3118, &D_801F3108, &D_801ED318, &D_801ED348, slot, arg0->unk02D);
    arg0->flags |= EFFECT_FLAG_UNK08;
    slot->flags |= BATTLE_SLOT_FLAG_UNK04;
    arg0->pc++;
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A76FC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A776C);

static void func_801A77C8(EffectEntity *arg0) {
    BattleEffectSlot *slot;
    EffectModel *model;

    D_801F3118.unk0F2 -= 64;
    if (D_801F3118.unk0F2 < 0) {
        D_801F3118.unk0F2 = 0;
    }
    D_801F3118.unk0CE -= 256;
    if (D_801F3118.unk0CE <= 0) {
        arg0->flags &= ~EFFECT_FLAG_UNK08;
        slot = &D_800EF2D0[arg0->unk02D];
        model = arg0->unk010;
        slot->flags &= ~BATTLE_SLOT_FLAG_UNK04;
        model->unk063 = 0;
        arg0->flags |= EFFECT_FLAG_STOP;
        arg0->pc++;
    }
}

static void func_801A7870(EffectEntity *arg0) {
}

static s32 func_801A7878(EffectEntity *arg0) {
    EffectHandler handlers[5] = {
        func_801A764C, func_801A76FC, func_801A776C, func_801A77C8,
        func_801A7870
    };
    EffectRender *render = &D_801F3118;

    handlers[arg0->pc](arg0);
    render->unk0CC = (render->unk0CC + 160) & 0xFFF;
    if (arg0->flags & EFFECT_FLAG_UNK08) {
        func_801A1E00(render);
    }
    arg0->unk024++;
    if (arg0->flags & EFFECT_FLAG_STOP) {
        if (arg0->wait != 0) {
            return 0;
        }
        effectReleaseWait(arg0);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7964);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A79A0);

void func_801A7CC4(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7CCC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7D6C);

static void func_801A7DA8(EffectEntity *arg0) {
    func_801A722C_arg0 *child;
    EffectModel *model;
    s32 count;
    s32 i;

    if (arg0->unk024 < 25) {
        count = D_801E3D88[arg0->unk024];
        model = arg0->unk014;
        for (i = 0; i < count; i++) {
            child = effectSpawnTask(&D_801EC688, func_801A722C, 112, arg0);
            if (rand() & 0x1) {
                child->unk030.vx = child->unk030.vy = child->unk030.vz = model->unk086;
            } else {
                child->unk030.vx = child->unk030.vy = child->unk030.vz = model->unk086 / 2;
            }
        }
    } else {
        arg0->flags |= EFFECT_FLAG_STOP;
        arg0->pc++;
    }
}

void func_801A7EAC(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7EB4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7F54);

static void func_801A7F90(EffectEntity *arg0) {
    func_801A722C_arg0 *child;
    EffectModel *model;
    s32 count;
    s32 i;

    if (arg0->unk024 < 21) {
        count = D_801E3DA8[arg0->unk024];
        model = arg0->unk014;
        for (i = 0; i < count; i++) {
            child = effectSpawnTask(&D_801EC688, func_801A7580, 112, arg0);
            if (rand() & 0x1) {
                child->unk030.vx = child->unk030.vy = child->unk030.vz = model->unk086;
            } else {
                child->unk030.vx = child->unk030.vy = child->unk030.vz = model->unk086 / 2;
            }
        }
    } else {
        arg0->flags |= EFFECT_FLAG_STOP;
        arg0->pc++;
    }
}

void func_801A8094(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A809C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A813C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8184);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A81E0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8210);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A824C);

static void func_801A82A8(EffectEntity *arg0) {
}

static s32 func_801A82B0(EffectEntity *arg0) {
    EffectHandler handlers[6] = {
        func_801A813C, func_801A8184, func_801A81E0, func_801A8210,
        func_801A824C, func_801A82A8
    };

    handlers[arg0->pc](arg0);
    arg0->unk024++;
    if (arg0->flags & EFFECT_FLAG_STOP) {
        if (arg0->wait != 0) {
            return 0;
        }
        effectReleaseWait(arg0);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8368);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8384);

void func_801A84AC(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A84B4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8554);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A86BC);

static void func_801A8750(EffectEntity *arg0) {
    EffectEntity *child;

    if (arg0->unk024 >= 4) {
        child = effectSpawnTask(&D_801ED278, func_801A6AC0, 152, arg0);
        child->unk08C = 2;
        arg0->pc++;
    }
}

static void func_801A87B8(EffectEntity *arg0) {
    EffectEntity *child;

    if (arg0->unk024 >= 6) {
        child = effectSpawnTask(&D_801ED278, func_801A6AC0, 152, arg0);
        child->unk08C = 3;
        arg0->pc++;
    }
}

static void func_801A8820(EffectEntity *arg0) {
    EffectEntity *child;

    if (arg0->unk024 >= 8) {
        child = effectSpawnTask(&D_801ED278, func_801A6AC0, 152, arg0);
        child->unk08C = 4;
        arg0->pc++;
    }
}

static void func_801A8888(EffectEntity *arg0) {
    EffectEntity *child;

    if (arg0->unk024 >= 10) {
        child = effectSpawnTask(&D_801ED278, func_801A6AC0, 152, arg0);
        child->unk08C = 5;
        arg0->pc++;
    }
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A88F0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A896C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A89F0);

static void func_801A8A50(EffectEntity *arg0) {
    if (arg0->unk080 == 1) {
        func_800BFE1C(arg0->animSet->anims[arg0->unk02A].parts);
        arg0->flags |= EFFECT_FLAG_STOP;
        arg0->pc++;
    }
}

void func_801A8AC4(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8ACC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8C00);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8C14);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8C6C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8CBC);

static void func_801A8CEC(EffectEntity *arg0) {
}

s32 func_801A8CF4(EffectEntity *arg0) {
    EffectHandler handlers[5] = {
        func_801A8C00, func_801A8C14, func_801A8C6C, func_801A8CBC,
        func_801A8CEC
    };

    if (arg0->unk05C & 0x1) {
        g_effectPrimCursor = D_801E3DE0;
    } else {
        g_effectPrimCursor = D_801E3DE4;
    }
    effectCacheAnimSlotAnchors(arg0);
    handlers[arg0->pc](arg0);
    arg0->unk05E = func_800B2B68(&D_801E40F8);
    arg0->unk05E += func_800B2B68(&D_801E4338);
    arg0->unk05E += func_800B2B68(&D_801EC688);
    arg0->unk05E += func_800B2B68(&D_801ED278);
    arg0->unk05E += func_800B2B68(&D_801ED308);
    arg0->unk05C++;
    arg0->unk024++;
    if (arg0->flags & EFFECT_FLAG_STOP) {
        if (arg0->wait != 0) {
            return 0;
        }
        effectReleaseWait(arg0);
        return 2;
    }
    return 0;
}
