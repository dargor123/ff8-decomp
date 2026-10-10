/**
 * @file menuitem.h
 * @brief Symbols and types owned by the menuitem overlay.
 *
 * The overlay is split into two translation units, menuitem.c and menuitem2.c
 * (the file boundary is at 0x801E9F94), so these declarations are shared.
 */
#ifndef MENUITEM_H
#define MENUITEM_H

#include "common.h"
#include "gamestate.h"
#include "menumain.h"
#include "ability_list.h"

typedef struct {
    u16 *a0;
    u16 unk4;
    u8 unk6;
    u8 unk7;
} UNKST_2; /* 0x8 */

typedef struct {
    u8 unk0;
    u8 unk1;
    u16 unk2;
    u16 unk4;
    u8 pad6[2];
} UNKST_3; /* 0x8 */

typedef struct {
    u8 pad00[0x10];
    u16 state; 
    u8 pad[0xE];
    UNKST_3 *unk20;
    u8 *unk24;
    s32 unk28;
    u8 unk2C;
    u8 unk2D;
    u8 unk2E;
    u8 unk2F;
    u8 unk30;
} AnotherItemMenuState;

extern UNKST_2 D_801EB320[];

s32 func_801E9C90(u8 *, s32);
void func_801E9CD4(AnotherItemMenuState *);
void func_801E9DE4(u8 *);
s32 func_801E9E10(u8 *);
u8 *func_801E9E7C(s32);
void func_801E9EA8(AnotherItemMenuState *, s32);

#endif /* MENUITEM_H */
