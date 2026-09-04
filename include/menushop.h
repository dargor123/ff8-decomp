#ifndef MENUSHOP_H
#define MENUSHOP_H

typedef struct {
    u8 unk0;
    u8 unk1;
} Struct_801E5DBC;

typedef struct {
    u16 unk0;
    u8 unk2;
} Struct_801E6ACC;

typedef struct {
    u8 unk0[0xC7];
} Struct_D_801EB088;

typedef struct {
    u8 pad000[0x20];
    s32 *unk20;
    u8 pad024[0x12];
    u16 unk36;
} Struct_801E8B60;

extern u8 D_80077EBC[];
extern u8 D_801EA3F0[];
extern u8 D_801EA70C[];
extern u8 D_801F7F98[];
extern u8 D_801E9B64[];
extern u8 D_801E9B6C[];
extern u8 D_801EA170[];
extern u8 D_801EAA28[];
extern s32 D_801EAA48[];
extern s32 D_801EAD68[];
extern Struct_D_801EB088 D_801EB088;
extern s32 g_menuColor;
extern s32 D_80077E70;
extern s32 func_801E8AB0;
extern void func_801E5E90(void*);
extern s32 func_801E79D4(void*, s32, s32);

#endif /* MENUSHOP_H */
