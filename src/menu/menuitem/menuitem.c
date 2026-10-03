#include "common.h"
#include "menu.h"
#include "menuitem.h"
#include "overlay.h"
#include "game.h"
#include "numstr.h"
#include "ui/dialog.h"
#include "ui/text.h"
#include "ui/font.h"
#include "ui/window.h"
#include "ui/icon.h"
#include "gamestate.h"
#include "kernel.h"
#include "ability_list.h"
#include "gf_anim.h"
#include "card.h"
#include "battle.h"
#include "gf.h"
#include "snd_sfx.h"
#include "psxsdk/libetc.h"
#include "battle_results/number.h"

typedef struct {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u8 unk6;
    u8 unk7;
} ST_0;

typedef struct {
    u8 pad00[0x10];
    u16 state;
    u8 pad12[0x4];
    u16 returnState;
    u8 pad18[0x8];
    ItemSlot *itemSlots;
    s32 unk24;
    u8 *itemDesc;
    u8 *previousItemDesc;  
    u8 *unk30;
    s32 unk34;
    s32 unk38;
    s16 menuColorIntensity;
    s16 unk3E;
    s16 unk40;
    u8 pad42[0x4];
    s16 unk46;
    s16 unk48;
    s8 unk4A;
    s8 unk4B;
    s16 unk4C;
    s8 unk4E;
    s8 unk4F;
    s16 unk50;
    u8 unk52;
    s8 unk53;
    s16 unk54;
    u8 pad56[0x2];
    s16 unk58;
    s16 unk5A;
    s16 unk5C;
    s16 unk5E;
    u8 pad60[0x1];
    s8 unk61;
    s8 unk62;
    s8 unk63;
    u8 unk64;
    u8 unk65;
    s8 unk66;
    s8 unk67;
    s8 unk68;
    s8 unk69;
    s8 unk6A;
    s8 unk6B;
    s16 unk6C;
} MenuItemState;

extern u8 D_801ECC20[];
extern ST_0 D_801EB1FC[];
extern CharacterData D_801ECE40;
extern u8 D_801EB234[]; /**< Weapon ID's, indexed by char ID. */
extern s32 D_801ECED8;
extern u8 D_801EB1E4[];
extern u8 D_8007752D;
extern u8 D_800776C5;
extern u16 D_80077E8C;

s32 func_801E80D0(s32, s32, s32, s32, s32);
s32 func_801E95C4(s32, s32, s32);
void func_801E4EA4(MenuItemState*);

/** @brief Store item menu state pointer. */
void func_801E2800(s32 a0) {
    D_801ECE20 = a0;
}

/**
 * @brief Read item menu state pointer.
 *
 * @return Value of D_801ECE20.
 */
s32 func_801E280C(void) {
    return D_801ECE20;
}

/** @brief Look up string @p a0 in menu text category 0xB. */
u8 *func_801E281C(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 0);
}

s32 func_801E2848(u8 *src, u8 *dst) {
    s32 ch;
    u8 *name;
    u8 *pName;
    u8 *tempGp;
    u8 *savedGp;
    u8 *buf;
    s32 ret;
    u8* pos;
    u8 pad[256];

    GP_SAVE_SCRATCH(tempGp);
    savedGp = tempGp;
    GP_ALLOC(buf, 128);

    if (src != NULL) {
        while (1) {
            if (src == NULL) {
                break;
            }

            decodeMessage(src, buf, -1);
            src = nextMessageLine(src);
            pos = buf;

            while (1) {
                ch = *pos++;

                if (ch == 1 || ch == 7 || ch == 0) {
                    goto end;
                }

                if (ch == 10) {
                    ch = *pos++;
                    name = pad;
                    switch (ch) {
                    case 39:
                        name = getCharName(g_gameState.chars[D_801ECE28].characterId);
                        break;
                    case 40:
                        name = func_801E281C(D_801ECE34 + 4);
                        break;
                    case 36:
                        name = getMagicNamePtr(D_801ECE30 + 64);
                        break;
                    case 37:
                        name = getAbilityName(D_801ECE2C);
                        break;
                    case 41:
                        name = getBlueMagicName(D_801ECE24);
                        break;
                    case 34:
                        intToDecStringShort(D_801ECE38, name, 1);
                        replaceLeadingZeros(name, 4, 1, 16);
                        while (*name == 16) {
                            name++;
                        }
                        for (pName = name; *pName != 0; pName++) {
                            u8 tmp = getMenuString(11)[1] + 255;
                            *pName += tmp;
                        }
                    }
                    while (*name != 0) {
                        *dst++ = *name++;
                    }
                    continue;
                }

                *dst++ = ch;
                if (ch >= 25) {
                    s32 val;
                    if (ch >= 32) {
                        val = ch - 32;
                    } else if (ch < 28) {
                        val = ch * 224;
                        *dst++ = *pos;
                        val += *pos++;
                        val -= 5408;
                    } else {
                        val = ch * 224;
                        *dst++ = *pos;
                        val += *pos++;
                        val -= 6304;
                        val |= 1024;
                    }
                    getNibbleValue(val);
                } else if (ch == 2) {
                    break;
                } else if (ch < 16) {
                    *dst++ = *pos;
                    if (ch == 5) {
                        ch = *pos++;
                        getIconWidth(func_8002C734(ch));
                    } else {
                        pos++;
                    }
                }
            }
        }
    }
end:
    *dst = 0;
    GP_FREE(128);
    GP_RESTORE_RET(savedGp, ret);
    return ret;
}

/** @brief Store a2 to D_801ECE24 and call func_801E2848. */
void func_801E2BA4(u8 *a0, u8 *a1, s32 a2) {
    D_801ECE24 = a2;
    func_801E2848(a0, a1);
}

/**
 * @brief Store 4 configuration values to globals and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE30
 * @param a3 Value stored to D_801ECE2C
 * @param arg5 Value stored to D_801ECE38
 * @param arg6 Value stored to D_801ECE28
 */
void func_801E2BC8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {

    D_801ECE30 = a2;
    D_801ECE2C = a3;
    D_801ECE28 = arg6;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

/**
 * @brief Store 3 configuration values and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE28
 * @param a3 Value stored to D_801ECE34
 * @param arg5 Value stored to D_801ECE38
 */
void func_801E2C0C(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {

    D_801ECE28 = a2;
    D_801ECE34 = a3;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

/**
 * @brief Search byte pair array for a matching entry.
 *
 * Iterates through up to 198 pairs of consecutive bytes starting at @p a0.
 * For each pair, checks if the first byte equals @p a1 and the second byte
 * is greater than or equal to @p a2. Returns 1 on the first match found.
 *
 * @param a0 Pointer to byte pair array.
 * @param a1 Value to match against the first byte of each pair.
 * @param a2 Minimum threshold for the second byte of the matching pair.
 * @return 1 if a matching pair is found, 0 otherwise.
 */
s32 func_801E2C44(u8 *a0, s32 a1, s32 a2) {
    s32 i = 0;
    do {
        s32 b0 = *a0++;
        s32 b1 = *a0++;
        if (b0 == a1 && b1 >= a2) {
            return 1;
        }
    } while (++i < 0xC6);
    return 0;
}

/**
 * @brief Look up an item name string by index.
 *
 * If @p a0 is within bounds (less than D_801ECC10), uses it to index
 * into D_801ECB60 to get an item ID, then calls getAbilityDesc
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Item list index.
 * @return Pointer to item name string, or NULL if index out of bounds.
 */
u8* func_801E2C80(s32 a0) {
    if (a0 < D_801ECC10) {
        return getAbilityDesc(D_801ECB60[a0].slotIndex);
    }
    return NULL;
}

void func_801E2CCC(s32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        arg1 += 28;
        break;
    case 1:
        arg1 += 20;
        break;
    case 2:
        arg1 += 24;
        break;
    case 3:
        arg1 += 44;
        break;
    case 4:
        arg1 += 48;
        break;
    case 5:
        arg1 += 71;
        break;
    case 6:
        arg1 += 180;
        break;
    }
    loadOverlayWithTimCallback(arg1, 0x801D1000); // FIXME: create var to 0x801D1000
}

void func_801E2D54(ItemSlot *itemSlots, u8 *arg1) {
    u8 buffer[32];
    s32 i;
    u8 *pBuffer;
    u8 *pItemSlots;

    for (i = 31, pBuffer = buffer + i; i >= 0; i--) {
        *pBuffer-- = 0;
    }

    pItemSlots = (u8 *)itemSlots;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        s32 id = *pItemSlots++;
        s32 quantity = *pItemSlots++;

        if (id <= 32 && id != 0) {
            buffer[id - 1] = quantity;
        }
    }

    for (i = 0; i < 32; i++) {
        D_801ECB20[arg1[i]].id = i + 1;
        D_801ECB20[arg1[i]].count = buffer[i];
    }
}

/**
 * @brief Initialize 32 entries using a lookup table of offsets.
 *
 * For each of 32 iterations, reads a byte offset from the table at D_801ECB20
 * (stepping by 2 each iteration), adds a0, and stores the iteration index
 * at (a0 + offset - 1).
 *
 * @param a0 Base address for storing iteration indices.
 */
void func_801E2E04(u8 *a0) {
    s32 i;
    for (i = 0; i < 32; i++) {
        a0[D_801ECB20[i].id - 1] = i;
    }
}

/**
 * @brief Read byte 0 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return First byte of the 4-byte entry.
 */
u8 func_801E2E38(s32 a0) {
    return D_801F889C[a0].b0;
}

/**
 * @brief Read byte 1 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Second byte of the 4-byte entry.
 */
s32 func_801E2E54(s32 a0) {
    return D_801F889C[a0].b1;
}

/**
 * @brief Read byte 2 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Third byte of the 4-byte entry.
 */
u8 func_801E2E70(s32 a0) {
    return D_801F889C[a0].b2;
}

/**
 * @brief Read byte 3 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Fourth byte of the 4-byte entry.
 */
u8 func_801E2E8C(s32 a0) {
    return D_801F889C[a0].b3;
}

u8 func_801E2EA8(s32 arg0) {
    u8 ret;
    switch (func_801E2E38(arg0)) {
    case 19:
        if (!(func_80036EC0() & 8) || (D_8007809A & 1)) {
            ret = func_801E2E54(arg0) & 0xFE;
        } else {
            ret = func_801E2E54(arg0);
        }
        break;
    case 0 ... 8:
    case 10 ... 11:
    case 16 ... 18:
    case 20 ... 21:
        ret = func_801E2E54(arg0);
        break;
    case 12:
        ret = 17;
        if (D_8007809A & 0x10) {
            ret = 16;
        }
        break;
    case 14:
        ret = 16;
        if (D_8008520B == 0) {
            ret = 17;
        }
        break;
    case 9:
    case 13:
    case 15:
        ret = 17;
        break;
    }
    return ret;
}

/**
 * @brief Check if a specific item type should trigger an ability menu update.
 *
 * Loads item type from D_801F889C[a0*4] and checks against specific type IDs.
 * First calls func_801F79F8(0x40) as a precondition. For types 1, 2, 4, 5
 * returns 0. For type 6, checks func_801EFFD4 bit 0. Otherwise calls
 * func_801E2EA8 and returns bit 0 of its result.
 *
 * @param a0 Item index.
 * @return 0 for certain types, or bit 0 of func_801E2EA8 result.
 */
s32 func_801E2F88(s32 a0) {
    s32 type = D_801F889C[a0].b0;

    if (func_801F79F8(0x40) != 0) {
        if (type == 1) {
            return 0;
        }
        if (type == 2) {
            return 0;
        }
        if (type == 4) {
            return 0;
        }
        if (type == 5) {
            return 0;
        }
    }
    if (type == 6) {
        if ((func_801EFFD4() & 1) == 0) {
            return 0;
        }
    }
    return func_801E2EA8(a0) & 1;
}

s32 func_801E302C(s32 arg0, s32 arg1, s32 arg2) {
    s32 ret;
    s32 val1;
    s32 val2;
    s32 val3;
    s32 tmp;

    ret = 0;
    val1 = func_801F57DC(arg0);
    val2 = func_801F58EC(arg0);
    val3 = func_801F57A4(arg0);
    arg1 *= 50;

    if ((val3 & 0x40) && !(arg2 & 0x40)) {
        return ret;
    }

    if (val3 & 1) {
        arg1 = 0;
    }
    arg1 += val1;

    if (arg1 >= 0) {
        tmp = val2;
        if (val2 >= arg1) {
            tmp = arg1;
        }
    } else {
        tmp = 0;
    }
    arg1 = tmp;

    val3 = func_801F5150(arg1, val2, val3);
    
    if (val3 & arg2) {
        val3 &= ~arg2;
        ret = 1;
    }

    func_801F5868(arg0, arg1);
    func_801F576C(arg0, val3);
    
    if (val1 != arg1) {
        ret = 1;
    }
    return ret;
}

s32 func_801E3158(s32 arg0, s32 arg1, s32 arg2) {
    s32 ret;
    s32 val1;
    s32 val2;
    s32 val3;
    s32 tmp;

    ret = 0;
    val1 = func_801F57DC(arg0);
    val2 = func_801F58EC(arg0);
    val3 = func_801F57A4(arg0);
    arg1 *= 50;

    if ((val3 & 0x40) && !(arg2 & 0x40)) {
        arg1 = 0;
    }
    if (val3 & 1) {
        arg1 = 0;
    }
    arg1 += val1;

    if (arg1 >= 0) {
        tmp = val2;
        if (val2 >= arg1) {
            tmp = arg1;
        }
    } else {
        tmp = 0;
    }
    arg1 = tmp;

    val3 = func_801F5150(arg1, val2, val3);
    
    if (val3 & arg2) {
        val3 &= ~arg2;
        ret = 1;
    }

    func_801F5868(arg0, arg1);
    func_801F576C(arg0, val3);
    
    if (val1 != arg1) {
        ret = 1;
    }
    return ret;
}

/**
 * @brief Process input and update state with computed page index.
 *
 * Calls func_801F58EC and func_801F57A4. If the action is odd (bit 0 set),
 * divides func_801F58EC's result by 8 (signed, rounding toward zero),
 * calls func_801F5150 with the quotient and original result, then updates
 * via func_801F576C and func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if processed, 0 otherwise.
 */
/**
 * @brief Process input and update state with computed page index.
 *
 * Calls func_801F58EC and func_801F57A4. If the action is odd (bit 0 set),
 * divides func_801F58EC's result by 8 (signed, rounding toward zero),
 * calls func_801F5150 with the quotient and original result, then updates
 * via func_801F576C and func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if processed, 0 otherwise.
 */
s32 func_801E3288(s32 arg0) {
    s32 ret;
    s32 val1;
    s32 val2;
    s32 val3;

    val1 = func_801F58EC(arg0);
    val2 = func_801F57A4(arg0);
    if (!(val2 & 1)) {
        return 0;
    }

    val3 = val1 / 8;
    func_801F576C(arg0, func_801F5150(val3, val1, val2));
    func_801F5868(arg0, val3);
    return 1;
}

/**
 * @brief Process input event and update state if odd-numbered action.
 *
 * Calls func_801F58EC then func_801F57A4 to get an action code.
 * If the action is odd (bit 0 set), calls func_801F5150 with the action,
 * passes the result to func_801F576C, and finally calls func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if the action was processed (odd action), 0 otherwise.
 */
/**
 * @brief Process input event and update state if odd-numbered action.
 *
 * Calls func_801F58EC then func_801F57A4 to get an action code.
 * If the action is odd (bit 0 set), calls func_801F5150, passes the result
 * to func_801F576C, and finally calls func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if the action was processed (odd action), 0 otherwise.
 */
s32 func_801E3314(s32 arg0) {
    s32 val1;
    s32 val2;

    val1 = func_801F58EC(arg0);
    val2 = func_801F57A4(arg0);
    if (!(val2 & 1)) {
        return 0;
    }

    func_801F576C(arg0, func_801F5150(val1, val1, val2));
    func_801F5868(arg0, val1);
    return 1;
}

s32 func_801E338C(s32 arg0, s32 arg1, s32 arg2) {
    s32 ret;
    s32 val1;
    s32 val2;
    s32 val3;
    s32 val4;

    ret = 0;
    if (g_gameState.chars[arg0].gfCompatibility[arg2] == 1000) {
        return ret;
    }

    val1 = func_801E2E8C(arg1);
    val2 = g_gameState.chars[arg0].gfCompatibility[arg2];
    val3 = val2 - val1;
    if (val3 < 1000) {
        val4 = 1000;
    } else if (val3 <= 6000) {
        val4 = val3;
    } else {
        val4 = 6000;
    }
    val3 = val4;
    g_gameState.chars[arg0].gfCompatibility[arg2] = val3;
    val3 &= 0xFFFF;
    if (val2 != val3) {
        ret = 1;
    }
    return ret;
}

s32 func_801E347C(s32 arg0, s32 arg1) {
    s32 i;
    s32 gfMask;
    s32 val1;
    s32 val2;

    gfMask = 0;
    val1 = func_801E2E8C(arg1);

    for (i = 0; i < GF_COUNT; i++) {
        if (g_gameState.gfs[i].exists & GF_EXISTS) {
            gfMask |= func_801E338C(arg0, arg1, i);
        }
    }

    if (gfMask == 0) {
        val2 = func_801F08D4(1, 9, 32, 0);
        func_801E2BC8(val2, D_801ECC20, 0, 0, 0, arg0);
        func_801E2800(D_801ECC20);
    } else {
        val2 = func_801F08D4(1, 9, 33, 0);
        func_801E2BC8(val2, D_801ECC20, 0, 0, val1 / 5, arg0);
        func_801E2800(D_801ECC20);
    }
    
    return gfMask;
}

s32 func_801E35B8(s32 arg0, s32 arg1) {
    s32 i;
    s32 val1;
    s32 val2;
    s32 val3;
    s32 val4;
    s32 val5;
    
    if (arg0 >= 0x10) {
        return 0;
    }
    
    val1 = func_801E2E54(arg1);
    val2 = func_801E2E70(arg1);
    val3 = func_801E2E8C(arg1);
    
    if (val2 == 0xFF) {
        return func_801E347C(arg0, arg1);
    }

    val4 = 0;
    if (val1 & 0x80) {
        val4 = (u32)val3 >> 1;
    }

    if (!(g_gameState.gfs[val2].exists & 1)) {
        return 0;
    }
     
    if (g_gameState.chars[arg0].gfCompatibility[val2] == 0x3E8) {
        val5 = func_801F08D4(1, 9, 0x1C, 0);
        func_801E2BC8(val5, D_801ECC20, val2, 0, 0, arg0);
        func_801E2800(D_801ECC20);
        return 0;
    }
        
    for (i = 0; i < 16; i++) {
        s32 tmp1;
        s32 tmp2;

        tmp1 = g_gameState.chars[arg0].gfCompatibility[i];
        if (i == val2) {
            tmp1 -= val3;
        } else {
            tmp1 += val4;
        }

        if (tmp1 < 1000) {
            tmp2 = 1000;
        } else if (tmp1 <= 6000) {
            tmp2 = tmp1;
        } else {
            tmp2 = 6000;
        }
        
        g_gameState.chars[arg0].gfCompatibility[i] = tmp2;
    }
    
    val5 = func_801F08D4(1, 9, 0x1B, 0);
    func_801E2BC8(val5, D_801ECC20, val2, 0, val3 / 5, arg0);
    func_801E2800(D_801ECC20);
    return 1;
}

/**
 * @brief Process scroll input and update item list view state.
 *
 * Reads current and previous scroll positions, then checks button state.
 * If the 0x40 flag is set in func_801F79F8 and the action is odd, returns 0.
 * Otherwise extracts the 0x80 flag from the action, updates the display
 * via func_801F576C and func_801F5868. Returns 1 if either the position
 * or the 0x80 flag changed, 0 otherwise.
 *
 * @param a0 Context pointer.
 * @return 1 if state changed, 0 otherwise.
 */
s32 func_801E37A4(s32 arg0) {
    s32 val1;
    s32 val2;
    s32 val3;
    s32 val4;

    val1 = func_801F58EC(arg0);
    val2 = func_801F57DC(arg0);
    val3 = func_801F57A4(arg0);


    if (func_801F79F8(0x40) && (val3 & 1)) {
        return 0;
    }

    val4 = val3;
    val3 &= 0x80;

    func_801F576C(arg0, val3);    
    func_801F5868(arg0, val1);

    if (val2 != val1 || val3 != val4) {
        return 1;
    }

    return 0;
}

/**
 * @brief Set ability bit flag in character's ability table.
 *
 * Searches character a0's ability list (at D_80079D78 + a0*132) for
 * ability a1. If found at index i, sets bit (1 << (i+8)) in the
 * corresponding word at D_80077408 + a0*68.
 *
 * @param a0 Character index.
 * @param a1 Ability ID to search for.
 */
/**
 * @brief Set ability bit flag in character's ability table.
 *
 * Searches character a0's ability list (at D_80079D78 + a0*132) for
 * ability a1. If found at index i, sets bit (1 << (i+8)) in the
 * corresponding word at D_80077408 + a0*68.
 *
 * @param a0 Character index.
 * @param a1 Ability ID to search for.
 */
void func_801E3854(s32 arg0, s32 arg1) {
    s32 val1;
    JunctionableGfEntry *junctionableGf;
    s32 i;

    val1 = *(u32 *)&g_gameState.gfs[arg0].learning;
    junctionableGf = &g_kernel.junctionableGfs[arg0];

    for (i = 0; i < 21; i++) {
        if (junctionableGf->abilities[i].abilityId == arg1) {
            val1 |= 1 << (i + 8);
            break;
        }
    }

    *(u32 *)&g_gameState.gfs[arg0].learning = val1;
}

/**
 * @brief Clear ability bit flag in character's ability table.
 *
 * Searches character a0's ability list (at D_80079D78 + a0*132) for
 * ability a1. If found at index i, clears bit (1 << (i+8)) in the
 * corresponding word at D_80077408 + a0*68.
 *
 * @param a0 Character index.
 * @param a1 Ability ID to search for.
 */
void func_801E38DC(s32 arg0, s32 arg1) {
    s32 val1;
    JunctionableGfEntry *junctionableGf;
    s32 i;

    val1 = *(u32 *)&g_gameState.gfs[arg0].learning;
    junctionableGf = &g_kernel.junctionableGfs[arg0];

    for (i = 0; i < 21; i++) {
        if (junctionableGf->abilities[i].abilityId == arg1) {
            val1 &= ~(1 << (i + 8));
            break;
        }
    }

    *(u32 *)&g_gameState.gfs[arg0].learning = val1;
}

void func_801E3968(s32 arg0, s32 arg1) {
    u8 buffer[4];
    u8 *pBufferEnd;
    u8 *pBufferStart;
    s32 i;
    s32 j;
    s32 k;
    u32 accum;
    s32 hasAbility;
    s32 availableJunctions;
    s32 junctionMask;
    s32 val1;
    s32 val2;
    s32 *completeAbilities;

    if (g_gameState.gfs[arg0].learning == arg1) {
        g_gameState.gfs[arg0].learning = 0;
    }

    completeAbilities = g_gameState.gfs[arg0].completeAbilities;
    completeAbilities[arg1 / 32] &= ~(1 << (arg1 & 0x1F));

    func_801E3854(arg0, arg1);

    for (i = 0; i < 8; i++) {
        accum = 0;
        hasAbility = 0;

        for (j = 0; j < 16; j++) {
            if ((g_gameState.chars[i].junctedGfs >> j) & 1) {
                accum |= g_gameState.gfs[j].completeAbilities[0];
                completeAbilities = g_gameState.gfs[j].completeAbilities;
                val1 = completeAbilities[arg1 / 32];
                val2 = 1 << (arg1 & 0x1F);
                if (val1 & val2) {
                    hasAbility = 1;
                }
            }
        }

        accum /= 2;
        accum &= 0x7FFFF;
        k = 2;
        if (accum & 0x20000) {
            k = 3;
        }
        if (accum & 0x40000) {
            k = 4;
        }
        
        availableJunctions = func_801F7C20(accum);
        
        for (j = 0; j < 19; j++) {
            junctionMask = 1 << j;
            if (!(availableJunctions & junctionMask)) {
                g_gameState.chars[i].junctions[j] = 0;
            }
        }

        pBufferEnd = &buffer[3];
        for (j = 3; j >= 0; j--) {
            *pBufferEnd-- = 0;
        }
        
        pBufferStart = &buffer[0];
        for (j = 0; j < 4; j++) {
            s32 ability = g_gameState.chars[i].abilities[j];
            if (ability != 0 && (ability != arg1 || hasAbility)) {
                *pBufferStart++ = ability;
            }
        }
        
        pBufferStart = &buffer[0];
        for (j = 0; j < 4; j++) {
            g_gameState.chars[i].abilities[j] = *pBufferStart++;
        }

        for (; k < 4; k++) {
            g_gameState.chars[i].abilities[k] = 0;
        }

        for (j = 0; j < 4; j++) {
            if (g_gameState.chars[i].commands[j] == arg1 && !hasAbility) {
                g_gameState.chars[i].commands[j] = 0;
            }
        }
        
        func_801F5400(i);
        func_801F1B4C(i);
    }
}

s32 func_801E3C1C(s32 arg0, s32 arg1) {
    s32 i;
    s32 val;
    s32 *completeAbilities;
    AbilityListEntry *abilities;

    if (arg0 < 16) {
        return 0;
    }

    arg0 -= 16;
    completeAbilities = g_gameState.gfs[arg0].completeAbilities;

    abilities = D_801ECB60;
    D_801ECC10 = func_800369CC(arg0, abilities, 1);

    for (i = 0; i < D_801ECC10; i++) {
        if (abilities[i].slotIndex == arg1) {
            switch (abilities[i].type) {
            case 1:
                completeAbilities[arg1 / 32] |= 1 << (arg1 & 0x1F);
                func_801E38DC(arg0, arg1);
                val = func_801F08D4(1, 9, 24, 0);
                func_801E2BC8(val, D_801ECC20, arg0, arg1, 0, 0);
                func_801E2800(D_801ECC20);
                recalcPartyStats();
                return 1;
            case 2:
                val = func_801F08D4(1, 9, 25, 0);
                func_801E2BC8(val, D_801ECC20, arg0, abilities[i].slotIndex, 0, 0);
                func_801E2800(D_801ECC20);
                return 0;
            default:
                return 0;
            }
            break;
        }
    }

    if (D_801ECC10 < 22) {
        completeAbilities[arg1 / 32] |= 1 << (arg1 & 0x1F);
        func_801E38DC(arg0, arg1);
        val = func_801F08D4(1, 9, 24, 0);
        func_801E2BC8(val, D_801ECC20, arg0, arg1, 0, 0);
        func_801E2800(D_801ECC20);
        recalcPartyStats();
        return 1;
    }

    val = func_801F08D4(1, 9, 26, 0);
    func_801E2BC8(val, D_801ECC20, arg0, 0, 0, 0);
    func_801E2800(D_801ECC20);
    return 0;
}

/**
 * @brief Reset four item menu state words to -1.
 *
 * Sets D_801ECEDC, D_801ECEE4, D_801ECEE8, and D_801ECEE0 all to -1.
 */
void func_801E3E94(void) {

    D_801ECEDC = -1;
    D_801ECEE4 = -1;
    D_801ECEE8 = -1;
    D_801ECEE0 = -1;
}

s32 func_801E3EBC(s32 arg0, s32 arg1, s32 arg2) {
    BattleCharData buf;
    s32 ret;
    ST_0 *st;
    s32 i;
    s16* p1_16;
    s16* p2_16;
    u8* p1_8;
    u8* p2_8;
    s32 diff;
    s32 value;
    s32 sum;
    s32 tmp;
    s32 msg;

    ret = 0;
    if (arg0 >= 16) {
        return ret;
    }

    if (D_801ECEDC == arg0 && D_801ECEE0 == arg1 && D_801ECEE4 == arg2) {
        return D_801ECEE8;
    }

    st = &D_801EB1FC[arg2];

    D_801ECEDC = arg0;
    D_801ECEE0 = arg1;
    D_801ECEE4 = arg2;

    D_801ECE40 = g_gameState.chars[arg0];

    g_gameState.chars[arg0].statusFlags = 0;

    for (i = 0; i < 20; i++) {
        g_gameState.chars[arg0].junctions[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        g_gameState.chars[arg0].commands[i] = 0;
        g_gameState.chars[arg0].abilities[i] = 0;
    }

    g_gameState.chars[arg0].weaponId = D_801EB234[g_gameState.chars[arg0].characterId];

    func_801F537C(arg0, &buf);

    if (msg) { msg++; msg--; }

    switch (st->unk6) {
    case 0:
        p1_16 = (s16 *)&buf.pad0[st->unk2];
        D_801ECED8 = *p1_16;
        p2_16 = (s16 *)&((u8 *)&D_801ECE40)[st->unk0];

        if (*p1_16 < st->unk4) {
            diff = *p2_16;
            sum = diff + arg1;

            if (sum >= 0) {
                tmp = sum;
                if (st->unk4 < sum) {
                    tmp = st->unk4;
                }
            } else {
                tmp = 0;
            }

            value = tmp;
            diff = value - diff;
            *p2_16 = value;
            
            ret = 1;

            if (arg1 != 0) {
                msg = func_801F08D4(1, 9, 0x1D, 0);
                func_801E2C0C(msg, D_801ECC20, arg0, st->unk7, diff);
                func_801E2800(D_801ECC20);
            }
        } else {
            if (arg1 != 0) {
                msg = func_801F08D4(1, 9, 0x1E, 0);
                func_801E2C0C(msg, D_801ECC20, arg0, st->unk7, 0);
                func_801E2800(D_801ECC20);
            }
        }
        break;
    case 1:
        p1_8 = &buf.pad0[st->unk2];
        D_801ECED8 = *p1_8;
        p2_8 = &((u8 *)&D_801ECE40)[st->unk0];

        if (*p1_8 < st->unk4) {
            diff = *p2_8;
            sum = diff + arg1;

            if (sum >= 0) {
                tmp = sum;
                if (st->unk4 < sum) {
                    tmp = st->unk4;
                }
            } else {
                tmp = 0;
            }

            value = tmp;
            diff = value - diff;
            *p2_8 = value;

            ret = 1;

            if (arg1 != 0) {
                msg = func_801F08D4(1, 9, 0x1D, 0);
                func_801E2C0C(msg, D_801ECC20, arg0, st->unk7, diff);
                func_801E2800(D_801ECC20);
            }
        } else {
            if (arg1 != 0) {
                msg = func_801F08D4(1, 9, 0x1E, 0);
                func_801E2C0C(msg, D_801ECC20, arg0, st->unk7, 0);
                func_801E2800(D_801ECC20);
            }
        }
        break;
    }

    g_gameState.chars[arg0] = D_801ECE40;

    func_801F5400(arg0);
    D_801ECEE8 = ret;
    return ret;
}

/**
 * @brief Process ability bits and accumulate results.
 *
 * Iterates through 8 bits of @p a2. For each set bit at position i,
 * calls func_801E3E94 to reset state, then func_801E3EBC(a0, a1, i)
 * and OR's the result into an accumulator. After the loop, calls
 * func_801E3E94 one final time and returns the accumulated result.
 *
 * @param a0 First parameter passed through to func_801E3EBC.
 * @param a1 Second parameter passed through to func_801E3EBC.
 * @param a2 Bitmask of abilities to process (low 8 bits).
 * @return OR'd result of all func_801E3EBC calls.
 */
s32 func_801E42F8(s32 arg0, s32 arg1, u8 arg2) {
    s32 accum;
    s32 i;

    accum = 0;
    i = 0;
    while (i < 8) {
        if ((arg2 >> i) & 1) {
            func_801E3E94();
            accum |= func_801E3EBC(arg0, arg1, i);
        }
        i++;
    }
    func_801E3E94();
    return accum;
}

s32 func_801E4394(s32 arg0, s32 arg1) {
    s32 b2;
    s32 b3;
    s32 ret;
    s32 i;
    s32 mask;
    s32 val;

    b2 = D_801F889C[arg0].b2;
    b3 = D_801F889C[arg0].b3;
    val = func_801E2E38(arg0);
    val = D_801EB1E4[val];
    ret = 0;

    for (i = 0; i < 32; i++) {
        mask = 1 << i;
        if (arg1 & mask) {
            switch (val) {
            case 1:
                ret |= func_801E302C(i, b2, b3);
                break;
            case 2:
                ret |= func_801E3288(i);
                break;
            case 3:
                ret |= func_801E3314(i);
                break;
            case 4:
                ret |= func_801E37A4(i);
                break;
            case 5:
                ret |= func_801E35B8(i, arg0);
                break;
            case 6:
                ret |= func_801E3C1C(i, b2);
                break;
            case 8:
                ret |= func_801E42F8(i, b2, b3);
                break;
            case 9:
                ret |= func_801E3158(i, b2, b3);
                break;
            case 0:
                break;
            }
        }
    }

    if (ret != 0) {
        switch (val) {
        case 1: 
        case 4: 
            playSoundEffect(0xA);
            break;
        case 2: 
        case 3: 
        case 5: 
        case 6: 
        case 8: 
        case 9: 
            playSoundEffect(0xB);
            break;
        default:
            sendSpuCommand(8);
            break;
        }
    } else {
        sendSpuCommand(5);
    }

    if (val) { val++; val--; }

    return ret;
}

/**
 * @brief Decrement item count at table entry and clear first byte if depleted.
 *
 * Given a base pointer a1 and index a2, accesses the 2-byte entry at
 * a1[a2*2]. If byte 1 (count) is positive, decrements it. If the count
 * reaches zero after decrement, also clears byte 0 (item ID).
 *
 * @param a0 Unused.
 * @param a1 Base pointer to item table.
 * @param a2 Entry index.
 * @return 1 if count was decremented but not depleted, 0 otherwise.
 */
/**
 * @brief Decrement item count at table entry and clear first byte if depleted.
 *
 * Given a base pointer a1 and index a2, accesses the 2-byte entry at
 * a1[a2*2]. If byte 1 (count) is positive, decrements it. If the count
 * reaches zero after decrement, also clears byte 0 (item ID).
 *
 * @param a0 Unused.
 * @param a1 Base pointer to item table.
 * @param a2 Entry index.
 * @return 1 if count was decremented but not depleted, 0 otherwise.
 */
s32 func_801E457C(s32 arg0, ItemSlot *arg1, s32 arg2) {
    ItemSlot *item;
    s32 count;
    s32 ret;

    ret = 0;
    item = &arg1[arg2];
    count = item->count;

    if (count > 0) {
        count--;
        item->count = count;
        if (count != 0) {
            ret = 1;
            return ret;
        }
        item->id = 0;
    }

    if (ret) { ret++; ret--; }

    return ret;
}

/**
 * @brief Find and consume an item from the byte-pair table.
 *
 * Scans 198 byte-pair entries at @p a0 for one whose first byte matches @p a1.
 * If found, subtracts @p a2 from the second byte (quantity). If the quantity
 * reaches zero, also clears the first byte (item ID).
 *
 * @param a0 Pointer to byte-pair table (198 entries, 2 bytes each).
 * @param a1 Item ID to search for.
 * @param a2 Quantity to subtract.
 */
void func_801E45B4(u8 *a0, s32 a1, s32 a2) {
    s32 i = 0;
    u8 *p = a0 + 1;
    do {
        s32 id = a0[0];
        s32 qty = p[0];
        if (id != 0 && qty != 0 && id == a1) {
            qty -= a2;
            p[0] = qty;
            if (qty == 0) {
                a0[0] = 0;
            }
            return;
        }
        i++;
        p += 2;
        a0 += 2;
    } while (i < 0xC6);
}

void func_801E4608(s32 arg0, MenuItemState *s) {
    s32 unk67;
    s32 unk6C;
    s32 index;
    s32 xOffset;
    s32 yOffset;
    s32 xBase;
    s32 x;
    s32 y;

    unk6C = s->unk6C;

    if (unk6C == 0) {
        return;
    }

    unk67 = s->unk67;
    index = (4096 - unk6C) / 64;

    xOffset = D_801FA3C8[index];
    xOffset = (xOffset * 192) / 4096;

    do {
        xBase = 65;
    } while (0);

    yOffset = (unk67 % 4) * 13;

    x = xBase - xOffset; 
    y = 68 + yOffset;

    func_801F0A34(arg0, 0, x, y);
}

/**
 * @brief Render item entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x8D for Y)
 */
void func_801E46B8(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC1, (a1 % 4) * 13 + 0x8D);
}

/**
 * @brief Render item at Y position from lookup table D_801EB1D8.
 *
 * Copies 9 entries from D_801EB1D8 to a local buffer, looks up
 * the entry at index a1, adds 0x32 to get the Y coordinate, then
 * calls func_801F0A34 to render.
 *
 * @param a0 X position parameter passed through.
 * @param a1 Index into the Y-offset table (0-8).
 */
void func_801E4708(s32 a0, s32 a1) {
    s16 buf[36];

    func_801F5984(D_801EB1D8, buf, 9);
    func_801F0A34(a0, 0, buf[a1] + 0x32, 0xD);
}

void func_801E476C(s32 arg0, MenuItemState *s) {
    s32 unk58;
    s32 x;
    s32 y;
    
    unk58 = s->unk58;

    y = unk58 % 11;
    y *= 13;
    y += 65;

    x = 205;
    
    func_801F0A34(arg0, 0, x, y);
}

/**
 * @brief Render a visual indicator at a column position derived from an index.
 *
 * Computes column = index % 11, then draws at y=0x41 offset by column * 13.
 * Uses func_801F0A34 for the actual rendering with a height of 0x27.
 *
 * @param a0 First argument passed through to func_801F0A34.
 * @param a1 Index value, divided by 11 to determine column.
 */
void func_801E47E0(s32 arg0, s32 arg1) {
    s32 x;
    s32 y;

    y = arg1 % 11;
    y *= 13;
    y += 65;

    x = 39;

    func_801F0A34(arg0, 0, x, y);
}

void func_801E4848(s32 arg0, MenuItemState *s) {
    s32 unk5E;
    s32 unk69;
    s32 index;
    s32 xOffset;
    s32 yOffset;
    s32 x;
    s32 y;

    unk5E = s->unk5E;
    unk69 = s->unk69;

    index = unk5E / 64;

    xOffset = D_801FA3C8[index];
    xOffset = (xOffset * 190) / 4096;

    yOffset = unk69 % 11;
    yOffset *= 13;
    
    x = 39;
    x -= xOffset;

    y = 65 + yOffset;

    func_801F0A34(arg0, 0, x, y);
}

/**
 * @brief Render item at computed Y position based on row index.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E4908(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC8, a1 * 13 + 0x42);
}

void func_801E4940(s32 arg0) {
    MenuDisplayConfig *cfg;
    s32 counter;
    s32 y;
    s32 i;
    s32 one;
    s32 width;
    s32 mask;

    cfg = &g_menuDisplayCfg;
    counter = 0;
    y = 200;
    i = 0;
    one = 1;
    width = 66;

    for (; i < 8; i++) {
        mask = one << i;
        if (arg0 & mask) {
            if ((counter + cfg->animCounter) & 1) {
                func_801F0994(0, y, width);
            }
            width += 13;
            counter++;
        }
    }
}

/**
 * @brief Render item entry at position derived from index parity and half-index
 *
 * Uses the low bit of a1 to select a column (multiplied by 82 + 0xC4 for width),
 * and a1/2 to select a row (multiplied by 13 + 0x40 for Y position).
 *
 * @param a0 X position parameter
 * @param a1 Linear index (bit 0 = column, upper bits / 2 = row)
 */
void func_801E49FC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, (a1 & 1) * 82 + 0xC4, (a1 / 2) * 13 + 0x40);
}

void func_801E4A58(u32 arg0) {
    MenuDisplayConfig *cfg;
    s32 counter;
    s32 yBase;
    s32 widthBase;
    s32 i;
    s32 mask;
    s32 y;
    s32 width;

    cfg = &g_menuDisplayCfg;
    counter = 0;
    yBase = 196;
    widthBase = 64;
    arg0 >>= 16;
    
    for (i = 0; i < 16; i++) {
        mask = 1 << i;
        if (arg0 & mask) {
            y = yBase + ((i & 1) * 82);
            width = widthBase + ((i / 2) * 13);
            if ((counter + cfg->animCounter) & 1) {
                func_801F0994(0, y, width);
            }
            counter++;
        }
    }
}

/**
 * @brief Dispatch based on upper/lower 16 bits of flags.
 *
 * If upper 16 bits are zero, calls func_801E4908 (normal render).
 * If upper bits non-zero but lower 16 bits are zero, calls func_801E49FC.
 *
 * @param a0 X position parameter
 * @param a1 Row index parameter
 * @param a2 Combined flags (upper 16 = type, lower 16 = subtype)
 */
void func_801E4B38(s32 a0, s32 a1, s32 a2) {
    if ((a2 & ~0xFFFF) == 0) {
        func_801E4908(a0, a1);
    } else if ((a2 & 0xFFFF) == 0) {
        func_801E49FC(a0, a1);
    }
}

/**
 * @brief Dispatch to different handler based on mode flag.
 * @param a0 Mode flag: non-zero calls func_801E4A58, zero calls func_801E4940
 * @param a1 Parameter passed to the selected handler
 */
void func_801E4B80(s32 a0, s32 a1) {
    if (a0 != 0) {
        func_801E4A58(a1);
    } else {
        func_801E4940(a1);
    }
}

/**
 * @brief Load item entry data for the primary list index.
 *
 * Reads the current list index from @p s[0x54], uses it to look up
 * a byte pair from the item table at @p s[0x20]. Stores the first
 * byte (item ID) at @p s[0x65]. If both bytes are nonzero, calls
 * getItemDesc to get the item description and stores it at @p s[0x28].
 *
 * @param s Pointer to item menu context.
 */
void func_801E4BB4(MenuItemState *s) {
    s32 id;
    s32 count;
    ItemSlot *item;
    
    item = &s->itemSlots[s->unk54];

    item++; item--;

    id = item->id;
    count = item->count;

    s->unk65 = id;

    if (id && count) {
        s->itemDesc = getItemDesc(id);
    } else {
        s->itemDesc = NULL;    
    }
}

/**
 * @brief Load item entry data for the secondary list index.
 *
 * Same as func_801E4BB4 but uses the secondary index at @p s[0x58].
 *
 * @param s Pointer to item menu context.
 */
void func_801E4C14(MenuItemState *s) {
    s32 id;
    s32 count;
    ItemSlot *item;

    item = &s->itemSlots[s->unk58];

    item++; item--;

    id = item->id;
    count = item->count;

    s->unk65 = id;

    if (id && count) {
        s->itemDesc = getItemDesc(id);
    } else {
        s->itemDesc = NULL;    
    }
}

void func_801E4C74(u8* src, u8* dst, s32 abilityId) {
    u8 buffer[64];
    s32 srcVal;
    u8* bufferPtr;
    
    while (1) {
        srcVal = *src++;
    
        if (srcVal == 0) {
            break;
        }
        
        bufferPtr = buffer;
        
        if (srcVal != 0xA) {
            *dst++ = srcVal;
        } else {
            srcVal = *src++;
            *buffer = 0;
            
            if (srcVal == 0x25) {
                copyString(bufferPtr, getAbilityName(abilityId));
            }
            
            while (*bufferPtr != 0) {
                *dst++ = *bufferPtr++;
            }
        }
    }
    
    *dst = 0;
}

void func_801E4D40(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3, s32 arg4, AbilityListEntry *arg5) {
    GfLearnData *gfLearnData;
    s32 i;
    s32 j;
    s32 k;
    s32 slot;
    s32 levelReq;

    gfLearnData = &D_80079D78[arg2];
    func_801E4C74(arg0, arg1, arg3);
    arg1 = &arg1[btlStrlen(arg1)];

    for (i = 0; i < 21; i++) {
        if (arg3 != gfLearnData->abilities[i].slot) {
            continue;
        }

        for (j = 0; j < 21; j++) {
            levelReq = gfLearnData->abilities[j].levelReq;
            slot = gfLearnData->abilities[j].slot;

            if (levelReq == 0xFF || levelReq < 0x65) {
                continue;
            }

            levelReq -= 0x65;
            if (levelReq != i) {
                continue;
            }

            for (k = 0; k < arg4; k++) {
                if (arg5[k].slotIndex != slot || arg5[k].type != 1) {
                    continue;
                }

                func_801E4C74(arg0, arg1, slot);
                arg1 = &arg1[btlStrlen(arg1)];
            } 
        }
    }
}

void func_801E4EA4(MenuItemState *s) {
    u8 buffer1[256];
    u8 buffer2[208];
    u16 btnFlags;
    u16 cfgFlags;
    u16 *statePtr;
    u16 state;

    statePtr = &s->state;
    btnFlags = g_menuDisplayCfg.inputRepeat;
    cfgFlags = g_menuDisplayCfg.inputNew;
    state = *statePtr;

restart:
    switch (state) {
    case 0:
        s->unk54 = 0;
        func_801E4BB4(s);
        *statePtr = 1;
        break;
        
    case 1:
        s->menuColorIntensity += 256;
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            *statePtr = 4;
        }
        func_801E4708(0, 0);
        func_801E47E0(1, 0);
        break;
        
    case 2:
        func_801E4708(1, s->unk61);
        *statePtr = 3;
        break;
        
    case 3:
        s->itemDesc = func_801F08D4(1, 9, D_801EB1D8[s->unk61], 1);
        s->unk61 = func_801F76E0(btnFlags, 15, s->unk61);
        func_801E4708(1, s->unk61);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            state = 112;
            goto restart;
        }
        if (btnFlags & PADRdown) {
            sendSpuCommand(2);
            switch (s->unk61) {
            case 1:
                *statePtr = 94;
                break;
            case 2:
                *statePtr = 79;
                break;
            case 3:
                *statePtr = 26;
                break;
            case 0:
                *statePtr = 4;
                break;
            }
        }
        break;
        
    case 4:
        *statePtr = 5;
        /* fallthrough */
        
    case 5: {
        s32 page;
        s32 count;
        u8 *item;
        func_801E2800(0);
        s->unk6B = -1;
        s->unk62 = -1;
        func_801E4708(0, s->unk61);
        page = s->unk54 / 11;
        s->unk54 = func_801F6768(btnFlags, 11, s->unk54 % 11) + (page * 11);
        func_801E47E0(1, s->unk54);
        func_801E4BB4(s);
        if (btnFlags & PADLleft) {
            s->returnState = 5;
            *statePtr = 6;
        }
        if (btnFlags & PADLright) {
            s->returnState = 5;
            *statePtr = 8;
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 2;
        }
        if (btnFlags & PADRdown) {
            item = s->itemSlots;
            count = s->unk54;
            count *= 2;
            item += count;
            count = ((ItemSlot *)item)->count;
            if ((func_801E2EA8(s->unk65) & 6) == 4) {
                if ((getGfAvailabilityMask() << 16) == 0) {
                    sendSpuCommand(5);
                    s->unk30 = func_801F08D4(1, 9, 3, 0);
                    *statePtr = 22;
                    break;
                }
            }
            if (count != 0) {
                s32 mask;
                switch (func_801E2E38(s->unk65)) {
                case 16:
                    mask = func_801E2E70(s->unk65);
                    if (mask != 0xFF) {
                        if (!((getGfAvailabilityMask() >> mask) & 1)) {
                            sendSpuCommand(5);
                            s->unk30 = func_801F6AFC(63);
                            *statePtr = 22;
                            setTextBrightness(s->unk40);
                            func_801F0948(s->menuColorIntensity);
                            if (D_8008520C != 0) {
                                func_801F1DB0(s->menuColorIntensity);
                            }
                            return;
                        }
                        break;
                    }
                    if (getGfAvailabilityMask() == 0) {
                        sendSpuCommand(5);
                        s->unk30 = func_801F6AFC(63);
                        *statePtr = 22;
                        setTextBrightness(s->unk40);
                        func_801F0948(s->menuColorIntensity);
                        if (D_8008520C != 0) {
                            func_801F1DB0(s->menuColorIntensity);
                        }
                        return;
                    }
                    break;
                case 12:
                    if (func_801E2F88(s->unk65) != 0) {
                        sendSpuCommand(2);
                        func_801F728C(func_801F08D4(1, 9, 8, 0), 100);
                        func_801F0000(130);
                        state = 24;
                        goto restart;
                    }
                    sendSpuCommand(5);
                    s->unk30 = func_801F6AFC(63);
                    *statePtr = 22;
                    setTextBrightness(s->unk40);
                    func_801F0948(s->menuColorIntensity);
                    if (D_8008520C != 0) {
                        func_801F1DB0(s->menuColorIntensity);
                    }
                    return;
                case 13:
                    if (*getChocoboWorldPtr() & 1) {
                        sendSpuCommand(2);
                        func_801F728C(func_801F08D4(1, 9, 7, 0), 100);
                        func_801F0000(147);
                        state = 24;
                        goto restart;
                    }
                    sendSpuCommand(5);
                    s->unk30 = func_801F6AFC(63);
                    *statePtr = 22;
                    setTextBrightness(s->unk40);
                    func_801F0948(s->menuColorIntensity);
                    if (D_8008520C != 0) {
                        func_801F1DB0(s->menuColorIntensity);
                    }
                    return;
                case 9:
                    sendSpuCommand(2);
                    state = 80;
                    goto restart;
                case 14:
                    state = 65;
                    if (!(D_8007752D & 1)) {
                        if (D_8008520B == 0) {
                            sendSpuCommand(2);
                            D_8008520C = 1;
                            func_801E457C(s->unk65, s->itemSlots, s->unk54);
                            setModeData(func_801E2E70(s->unk65) | (func_801E2E8C(s->unk65) << 8));
                            setGfExists(5);
                            state = 112;
                            goto restart;
                        }
                    }
                    goto restart;
                case 15:
                    state = 50;
                    goto restart;
                }
            }
            if (func_801E2F88(s->unk65) != 0 && count != 0) {
                sendSpuCommand(2);
                *statePtr = 10;
            } else {
                sendSpuCommand(5);
            }
        }
        break;
    }

    case 8: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->unk4B = s->unk4A;
        page = s->unk54 / 11;
        rowInPage = s->unk54 % 11;
        page++;
        if (page >= 18) {
            page = 0;
        }
        s->unk54 = rowInPage + page * 11;
        s->unk4A = page;
        s->unk48 = 3687;
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        s->previousItemDesc = s->itemDesc;
        func_801E4BB4(s);
        *statePtr = 9;
        break;
    }

    case 9:
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        s->unk48 -= 409;
        if (s->unk48 <= 0) {
            s->unk48 = 0;
            *statePtr = s->returnState;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 6;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 8;
        }
        break;

    case 6: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->unk4B = s->unk4A;
        page = s->unk54 / 11;
        rowInPage = s->unk54 % 11;
        page--;
        if (page < 0) {
            page = 17;
        }
        s->unk54 = rowInPage + page * 11;
        s->unk4A = page;
        s->unk48 = -3687;
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        s->previousItemDesc = s->itemDesc;
        func_801E4BB4(s);
        *statePtr = 7;
        break;
    }

    case 7:
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        s->unk48 += 409;
        if (s->unk48 >= 0) {
            s->unk48 = 0;
            *statePtr = s->returnState;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 6;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 8;
        }
        break;

    case 10:
        s->unk58 = 0;
        /* fallthrough */

    case 11: {
        s32 val1;
        s32 val2;
        s32 val3;
        s32 val4;
        s32 mask;
        s32 i;
        val1 = func_801E2EA8(s->unk65);
        if (func_801E2E38(s->unk65) == 20) {
            s->unk6B = findNthSetBit(func_801E2E8C(s->unk65), 0);
        } else {
            s->unk6B = -1;
        }
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        val2 = 0;
        if (val1 & 2) {
            val2 = func_80036EC0();
        }
        if (val1 & 4) {
            val2 |= getGfAvailabilityMask() << 0x10;
            if (val2 == 0) {
                sendSpuCommand(5);
                s->unk30 = func_801F08D4(1, 9, 3, 0);
                *statePtr = 22;
                break;
            }
        }
        if (val1 & 0x40) {
            val2 &= 8;
            if (D_8007809A & 1) {
                val2 = 0;
            }
        }
        if ((val2 & 0xFFFF0000) && !(val1 & 0x10)) {
            s->unk64 = 1;
        } else {
            s->unk64 = 0;
        }
        s->unk38 = val2;
        if (val1 & 0x20) {
            val3 = ~(func_801F22F4() | (func_801F2370() << 0x10)) & val2;
            if (val3 != 0) {
                if (s->unk64 != 0) {
                    s->unk58 = findNthSetBit((u32)val3 >> 0x10, 0);
                } else {
                    s->unk58 = getBitRank(val2, findNthSetBit(val3, 0));
                }
            }
        }
        val4 = 0;
        if (s->unk64 != 0) {
            for (i = 16; i < 32; i++) {
                mask = 1 << i;
                if (val2 & mask) {
                    val4 = i;
                }
            }
            s->unk63 = (s8)(val4 - 14) / 2;
        } else {
            s->unk63 = popcount(val2);
        }
        state = 15;
        if (!(val1 & 0x10)) {
            if (!(val1 & 8)) {
                state = 12;
            }
        }
        goto restart;
    }

    case 12:
        *statePtr = 14;
        /* fallthrough */

    case 14: {
        s32 mask;
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        if (s->unk64 != 0) {
            s32 val1;
            s32 val2;
            s32 val3;
            s32 val4;
            s32 val5;
            s32 val6;
            s32 val7;
            val1 = (u16)s->unk58;
            val2 = (s16)val1;
            val3 = val2 + ((u32)(val1 << 0x10) >> 0x1F);
            val3 >>= 1;
            val4 = (s16)(val2 - (val3 * 2));
            val5 = func_801F6768(btnFlags, s->unk63, val3);
            if ((btnFlags & PADLleft) && val4 != 0) {
                sendSpuCommand(1);
                val4 = 0;
            }
            if ((btnFlags & PADLright) && val4 == 0) {
                sendSpuCommand(1);
                val4 = 1;
            }
            s->unk58 = val4 + (val5 << 1);
            val7 = 1 << s->unk58;
            if (s->unk38 & val7) {
                s->unk62 = s->unk58;
            } else {
                s->unk6B = -1;
                s->unk62 = -1;
            }
        } else {
            s->unk58 = func_801F6768(btnFlags, s->unk63, s->unk58);
        }
        func_801E4B38(1, s->unk58, s->unk38);
        if (s->unk64 != 0) {
            s->unk62 = s->unk58 + 16;
            mask = 1 << s->unk62;
            if (!(s->unk38 & mask)) {
                s->unk62 = -1;
            }
        } else {
            s->unk62 = findNthSetBit(s->unk38, s->unk58);
        }
        if (btnFlags & PADRdown) {
            state = 17;
            if (s->unk62 >= 0) {
                s->unk34 = 1 << s->unk62;
                s->returnState = s->state;
                goto restart;
            }
            sendSpuCommand(5);
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 4;
        }
        break;
    }

    case 15:
        s->unk34 = s->unk38;
        *statePtr = 16;
        /* fallthrough */

    case 16:
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        func_801E4B80(s->unk64, s->unk38);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 4;
        }
        state = 17;
        if (btnFlags & PADRdown) {
            s->returnState = s->state;
            goto restart;
        }
        break;

    case 17: {
        s32 val1;
        func_801E2800(0);
        *statePtr = 11;
        cfgFlags = 0;
        switch (func_801E2E38(s->unk65)) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 16:
        case 17:
        case 20:
        case 21:
            val1 = func_801E4394(s->unk65, s->unk34);
            break;
        case 11: 
            sendSpuCommand(2);
            func_801F0000((findNthSetBit(s->unk34 & 0xFFFF0000, 0) - 13) | 0x80);
            state = 45;
            goto restart;
        case 18: 
            sendSpuCommand(2);
            state = 66;
            goto restart;
        case 19: 
            state = 107;
            goto restart;
        }
        if (val1 == 1 && func_801E457C(s->unk65, s->itemSlots, s->unk54) == 0) {
            if (func_801E280C() != 0) {
                s->returnState = 4;
                *statePtr = 18;
                break;
            }
            *statePtr = 4;
            break;
        }
        if (func_801E280C() != 0) {
            *statePtr = 18;
        }
        break;
    }

    case 18:
        s->unk5C = 600;
        s->unk30 = func_801E280C();
        *statePtr = 19;
        break;

    case 19:
        s->unk5C = s->unk5C - 1;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk5C = 0;
        }
        if (s->unk5C <= 0) {
            s->unk30 = NULL;
            *statePtr = s->returnState;
        }
        break;

    case 20:
    case 21:
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        break;

    case 22:
        s->unk5C = 600;
        *statePtr = 23;
        break;

    case 23:
        s->unk5C--;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk5C = 0;
            s->unk30 = NULL;
            *statePtr = 4;
        }
        if (s->unk5C <= 0) {
            s->unk5C = 0;
            s->unk30 = NULL;
            *statePtr = 4;
        }
        break;

    case 24:
        *statePtr = 25;
        s->unk66 = 1;
        break;

    case 25:
        s->unk66 = func_801F6768(btnFlags, 2, s->unk66);
        func_801F6F88(s->unk66);
        if (cfgFlags & PADRdown) {
            sendSpuCommand(2);
            if (s->unk66 != 0) {
                s->unk66 = -1;
                *statePtr = 5;
            } else {
                *statePtr = 45;
            }
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->unk66 = -1;
            *statePtr = 5;
        }
        break;

    case 26:
        s->unk4E = s->unk67 / 4;
        s->unk52 = s->unk68 / 4;
        s->unk4C = 0;
        s->unk50 = 0;
        func_801E2D54(s->itemSlots, s->unk24);
        *statePtr = 27;
        /* fallthrough */

    case 27:
        s->unk3E -= 256;
        if (s->unk3E <= 0) {
            s->unk3E = 0;
            *statePtr = 28;
        }
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        break;

    case 28:
        s->unk6C += 256;
        if (s->unk6C >= 4096) {
            s->unk6C = 4096;
            *statePtr = 29;
        }
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        break;

    case 29:
        *statePtr = 30;
        /* fallthrough */

    case 30: {
        s32 page;
        page = s->unk67 / 4;
        s->unk67 = (page * 4) + func_801F6768(btnFlags, 4, s->unk67 % 4);
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        if (!(btnFlags & PADLleft)) {
            if (btnFlags & PADLright) {
                *statePtr = 33;
            } else {
                if (D_801ECB20[s->unk67].count != 0) {
                    s->itemDesc = getItemDesc(D_801ECB20[s->unk67].id);
                } else {
                    s->itemDesc = NULL;
                }
                if (cfgFlags & PADRup) {
                    sendSpuCommand(3);
                    *statePtr = 42;
                }
                if (cfgFlags & PADRdown) {
                    sendSpuCommand(2);
                    *statePtr = 35;
                }
            }
        } else {
            *statePtr = 31;
        }
        break;
    }

    case 31: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousItemDesc = s->itemDesc;
        s->unk4F = s->unk4E;
        page = s->unk67 / 4;
        rowInPage = s->unk67 % 4;
        page--;
        if (page < 0) {
            page = 7;
        }
        s->unk67 = (page * 4) + rowInPage;
        s->unk4E = page;
        s->unk4C = -3687;
        if (D_801ECB20[s->unk67].count != 0) {
            s->itemDesc = getItemDesc(D_801ECB20[s->unk67].id);
        } else {
            s->itemDesc = NULL;
        }
        *statePtr = 32;
        /* fallthrough */
    }

    case 32:
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        s->unk4C += 409;
        if (s->unk4C >= 0) {
            s->unk4C = 0;
            *statePtr = 30;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 33;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 31;
        }
        break;

    case 33: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousItemDesc = s->itemDesc;
        s->unk4F = s->unk4E;
        page = s->unk67 / 4;
        rowInPage = s->unk67 % 4;
        page++;
        if (page >= 8) {
            page = 0;
        }
        s->unk67 = (page * 4) + rowInPage;
        s->unk4E = page;
        s->unk4C = 3687;
        if (D_801ECB20[s->unk67].count != 0) {
            s->itemDesc = getItemDesc(D_801ECB20[s->unk67].id);
        } else {
            s->itemDesc = NULL;
        }
        *statePtr = 34;
        /* fallthrough */
    }

    case 34:
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        s->unk4C -= 409;
        if (s->unk4C <= 0) {
            s->unk4C = 0;
            *statePtr = 30;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 33;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 31;
        }
        break;

    case 35:
        *statePtr = 36;
        /* fallthrough */

    case 36: {
        s32 val1;
        s32 count;
        val1 = s->unk68 / 4;
        s->unk68 = (val1 * 4) + func_801F6768(btnFlags, 4, s->unk68 % 4);
        func_801E4708(0, s->unk61);
        func_801E4608(0, s);
        func_801E46B8(1, s->unk68);
        if (!(btnFlags & PADLleft)) {
            if (btnFlags & PADLright) {
                *statePtr = 39;
            } else {
                if (D_801ECB20[s->unk68].count != 0) {
                    s->itemDesc = getItemDesc(D_801ECB20[s->unk68].id);
                } else {
                    s->itemDesc = NULL;
                }
                if (cfgFlags & PADRup) {
                    sendSpuCommand(3);
                    *statePtr = 30;
                }
                if (cfgFlags & PADRdown) {
                    count = D_801ECB20[s->unk67].count;
                    if (count != D_801ECB20[s->unk68].count || count != 0) {
                        sendSpuCommand(2);
                        *statePtr = 41;
                    } else {
                        sendSpuCommand(5);
                    }
                }
            }
        } else {
            *statePtr = 37;
        }
        break;
    }

    case 37: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousItemDesc = s->itemDesc;
        s->unk53 = s->unk52;
        page = s->unk68 / 4;
        rowInPage = s->unk68 % 4;
        page--;
        if (page < 0) {
            page = 7;
        }
        s->unk68 = (page * 4) + rowInPage;
        s->unk52 = page;
        s->unk50 = -3687;
        if (D_801ECB20[s->unk68].count != 0) {
            s->itemDesc = getItemDesc(D_801ECB20[s->unk68].id);
        } else {
            s->itemDesc = NULL;
        }
        *statePtr = 38;
        /* fallthrough */
    }

    case 38:
        func_801E4708(0, s->unk61);
        func_801E4608(0, s);
        func_801E46B8(1, s->unk68);
        s->unk50 += 409;
        if (s->unk50 >= 0) {
            s->unk50 = 0;
            *statePtr = 36;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 39;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 37;
        }
        break;

    case 39: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousItemDesc = s->itemDesc;
        s->unk53 = s->unk52;
        page = s->unk68 / 4;
        rowInPage = s->unk68 % 4;
        page++;
        if (page >= 8) {
            page = 0;
        }
        s->unk68 = (page * 4) + rowInPage;
        s->unk52 = page;
        s->unk50 = 3687;
        if (D_801ECB20[s->unk68].count != 0) {
            s->itemDesc = getItemDesc(D_801ECB20[s->unk68].id);
        } else {
            s->itemDesc = NULL;
        }
        *statePtr = 40;
        /* fallthrough */
    }

    case 40:
        func_801E4708(0, s->unk61);
        func_801E4608(0, s);
        func_801E46B8(1, s->unk68);
        s->unk50 -= 409;
        if (s->unk50 <= 0) {
            s->unk50 = 0;
            *statePtr = 36;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 39;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 37;
        }
        break;

    case 41: {
        s32 id1;
        s32 id2;
        s32 count1;
        s32 count2;
        id1 = D_801ECB20[s->unk67].id;
        count1 = D_801ECB20[s->unk67].count;
        id2 = D_801ECB20[s->unk68].id;
        count2 = D_801ECB20[s->unk68].count;
        D_801ECB20[s->unk68].id = id1;
        D_801ECB20[s->unk68].count = count1;
        D_801ECB20[s->unk67].id = id2;
        D_801ECB20[s->unk67].count = count2;
        state = 29;
        goto restart;
    }

    case 42:
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        func_801E2E04(s->unk24);
        *statePtr = 43;
        break;

    case 43:
        s->unk6C -= 256;
        if (s->unk6C <= 0) {
            s->unk6C = 0;
            *statePtr = 44;
        }
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        break;

    case 44:
        s->unk3E += 256;
        if (s->unk3E >= 4096) {
            s->unk3E = 4096;
            *statePtr = 3;
        }
        func_801E4708(0, s->unk61);
        func_801E4608(1, s);
        break;

    case 45:
        func_801E457C(s->unk65, s->itemSlots, s->unk54);
        loadOverlayWithTimCallback(9, 0x801CD000);
        loadOverlayWithTimCallback(10, 0x801D5000);
        *statePtr = 46;
        break;

    case 46:
        s->menuColorIntensity -= 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity <= 0) {
            s->menuColorIntensity = 0;
            func_801F1DB0(0);
            if (pollCdReadStatus() == 0) {
                func_801F0C5C(15, s);
                *statePtr = 47;
            }
        }
        break;

    case 47:
        if (func_801F0D84() == 2) {
            *statePtr = 48;
        }
        break;

    case 48:
        func_801F202C();
        func_801F1DBC(2);
        s->unk62 = -1;
        s->unk6B = -1;
        s->unk66 = -1;
        func_801E4BB4(s);
        loadOverlayWithTimCallback(12, 0x801D5000);
        *statePtr = 49;
        break;

    case 49:
        s->menuColorIntensity += 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            func_801F1DB0(4096);
            *statePtr = 5;
        }
        break;

    case 50: {
        u8 *ptr;
        s32 val1;
        s32 val2;
        state = 65;
        if (!(D_800776C5 & 1)) {
            ptr = D_801EC710;
            while (1) {
                val1 = ptr[0];
                val2 = ptr[1];
                if (val1 == 0) {
                    state = 51;
                    break;
                }
                if (func_801E2C44(s->itemSlots, val1, val2) == 0) {
                    state = 64;
                    break;
                }
                ptr += 4;
            }
        }
        goto restart;
    }

    case 51: {
        u8 *ptr;
        s32 val1;
        s32 val2;
        playSoundEffect(26);
        playSoundEffect(27);
        playSoundEffect(28);
        ptr = D_801EC710;
        while (1) {
            val1 = ptr[0];
            val2 = ptr[1];
            if (val1 == 0) {
                break;
            }
            ptr += 4;
            func_801E45B4(&s->itemSlots->id, val1, val2);
        }
        *statePtr = 52;
        /* fallthrough */
    }

    case 52: {
        u8 *msg;
        msg = func_801F08D4(1, 9, 10, 0);
        setDialogMessage(0, msg);
        func_801F23D0(0, 104, msg);
        setDialogCornerIcon(0, 80);
        setDialogTextSpeed(0, 1024);
        openDialogAnimated(0);
        *statePtr = 53;
        break;
    }

    case 53:
        if ((getDialogTypingDone(0) != 0) && (cfgFlags & PADRdown)) {
            sendSpuCommand(2);
            closeDialogAnimated(0);
            *statePtr = 54;
        }
        break;

    case 54:
        if (getOpenDialogScale(0) == 0) {
            *statePtr = 55;
        }
        break;

    case 55: {
        u8 *msg;
        msg = func_801F08D4(1, 9, 11, 0);
        setDialogMessage(0, msg);
        func_801F23D0(0, 104, msg);
        setDialogCornerIcon(0, 80);
        setDialogTextSpeed(0, 1024);
        openDialogAnimated(0);
        *statePtr = 56;
        break;
    }

    case 56:
        if ((getDialogTypingDone(0) != 0) && (cfgFlags & PADRdown)) {
            sendSpuCommand(2);
            *statePtr = 57;
        }
        break;

    case 57:
        s->menuColorIntensity -= 256;
        func_801F1DB0(s->menuColorIntensity);
        setDialogBrightness(0, s->menuColorIntensity);
        if (s->menuColorIntensity <= 0) {
            s->menuColorIntensity = 0;
            func_801F0000(14);
            func_801F0C5C(15, s);
            setDialogTextSpeed(0, 0);
            closeDialogInstant(0);
            setDialogBrightness(0, 4096);
            setGfExists(11);
            func_801E457C(s->unk65, s->itemSlots, s->unk54);
            *statePtr = 58;
        }
        break;

    case 58:
        if (func_801F0D84() == 2) {
            *statePtr = 59;
        }
        break;

    case 59:
        func_801F202C();
        func_801F1DBC(2);
        s->unk46 = getGfAvailabilityMask();
        s->unk66 = -1;
        func_801E4BB4(s);
        *statePtr = 60;
        break;

    case 60:
        s->menuColorIntensity += 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            func_801F1DB0(4096);
            *statePtr = 61;
        }
        break;

    case 61: {
        u8 *msg;
        msg = func_801F08D4(1, 9, 12, 0);
        setDialogMessage(0, msg);
        func_801F23D0(0, 104, msg);
        setDialogCornerIcon(0, 80);
        setDialogTextSpeed(0, 1024);
        openDialogAnimated(0);
        *statePtr = 62;
        break;
    }

    case 62:
        if ((getDialogTypingDone(0) != 0) && (cfgFlags & PADRdown)) {
            sendSpuCommand(8);
            closeDialogAnimated(0);
            *statePtr = 63;
        }
        break;

    case 63:
        if (getOpenDialogScale(0) == 0) {
            setDialogTextSpeed(0, 0);
            *statePtr = 4;
        }
        break;

    case 64:
        sendSpuCommand(5);
        s->unk30 = func_801F08D4(1, 9, 9, 0);
        *statePtr = 22;
        break;

    case 65:
        sendSpuCommand(5);
        s->unk30 = func_801F08D4(1, 9, 23, 0);
        *statePtr = 22;
        break;

    case 66:
        s->unk69 = 0;
        s->unk4F = 0;
        s->unk4E = 0;
        s->unk6A = findNthSetBit(s->unk34 & 0xFFFF0000, 0) - 16;
        D_801ECC10 = func_800369CC(s->unk6A, D_801ECB60, 1);
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        *statePtr = 67;
        /* fallthrough */

    case 67:
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        s->unk5E -= 256;
        if (s->unk5E <= 0) {
            s->unk5E = 0;
            *statePtr = 68;
        }
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        func_801E4848(1, s);
        break;

    case 68:
        *statePtr = 69;
        /* fallthrough */

    case 69: {
        s32 val1;
        val1 = s->unk69 / 11;
        s->unk69 = func_801F6768(btnFlags, 11, s->unk69 % 11) + (val1 * 11);
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        func_801E4848(1, s);
        s->itemDesc = func_801E2C80(s->unk69);
        if (D_801ECC10 >= 12) {
            if ((btnFlags & PADLleft) && val1 != 0) {
                *statePtr = 70;
            }
            if ((btnFlags & PADLright) && val1 == 0) {
                *statePtr = 72;
            }
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 77;
        }
        if (cfgFlags & PADRdown) {
            if (s->unk69 < D_801ECC10) {
                sendSpuCommand(2);
                *statePtr = 74;
            } else {
                sendSpuCommand(5);
            }
        }
        break;
    }

    case 70:
        sendSpuCommand(1);
        s->unk4E = 0;
        s->unk4F = 1;
        s->unk69 %= 11;
        s->unk4C = -3687;
        s->previousItemDesc = s->itemDesc;
        s->itemDesc = func_801E2C80(s->unk69);
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        func_801E4848(1, s);
        *statePtr = 71;
        break;

    case 71:
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        func_801E4848(1, s);
        s->unk4C += 409;
        if (s->unk4C >= 0) {
            s->unk4C = 0;
            *statePtr = 69;
        }
        break;

    case 72: {
        s32 val1;
        sendSpuCommand(1);
        s->unk4F = 0;
        s->unk4E = 1;
        s->previousItemDesc = s->itemDesc;
        val1 = (s->unk69 % 11) + 11;
        s->unk69 = val1;
        s->itemDesc = func_801E2C80(s->unk69);
        s->unk4C = 3687;
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        func_801E4848(1, s);
        *statePtr = 73;
        break;
    }

    case 73:
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        func_801E4848(1, s);
        s->unk4C -= 409;
        if (s->unk4C <= 0) {
            s->unk4C = 0;
            *statePtr = 69;
        }
        break;

    case 74: {
        u8 *msg;
        s32 slot;
        s32 width;
        msg = func_801F08D4(1, 9, 34, 0);
        slot = D_801ECB60[s->unk69].slotIndex;
        func_801E4D40(msg, buffer1, s->unk6A, slot, D_801ECC10, D_801ECB60);
        msg = func_801F08D4(1, 9, 14, 0);
        copyString(D_801ECC20, msg);
        btlStrcat2(D_801ECC20, buffer1);
        msg = func_801F08D4(1, 9, 17, 0);
        btlStrcat2(D_801ECC20, msg);
        width = measureMessage(D_801ECC20);
        func_801F728C(D_801ECC20, 100 - ((width >> 16) - 26) / 2);
        s->unk66 = 1;
        *statePtr = 75;
        break;
    }

    case 75: {
        s->unk66 = func_801F6768(btnFlags, 2, s->unk66);
        func_801F6F88(s->unk66);
        if (cfgFlags & PADRdown) {
            sendSpuCommand(2);
            if (s->unk66 != 0) {
                if (s->unk66 == 1) {
                    s->unk66 = -1;
                    *statePtr = 69;
                    break;
                }
                if (cfgFlags & PADRup) {
                    sendSpuCommand(3);
                    s->unk66 = -1;
                    *statePtr = 69;
                }
                break;
            }
            s->unk66 = -1;
            state = 76;
            goto restart;
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->unk66 = -1;
            *statePtr = 69;
        }
        break;
    }

    case 76: {
        s32 i;
        s32 exists;
        func_801E3968(s->unk6A, D_801ECB60[s->unk69].slotIndex);
        D_801ECC10 = func_800369CC(s->unk6A, D_801ECB60, 1);
        s->unk66 = -1;
        if (s->unk69 >= D_801ECC10) {
            s->unk69 = D_801ECC10 - 1;
            if (s->unk69 < 0) {
                s->unk69 = 0;
            }
            s->unk4E = s->unk69 / 11;
        }
        exists = 0;
        i = 0;
        for (; i < D_801ECC10; i++) {
            if (D_801ECB60[i].slotIndex == g_gameState.gfs[s->unk6A].learning) {
                exists = 1;
            }
        }
        if (exists == 0) {
            g_gameState.gfs[s->unk6A].learning = 0;
        }
        if (func_801E457C(s->unk65, s->itemSlots, s->unk54) != 0) {
            *statePtr = 68;
        } else {
            s->unk5E = 4096;
            *statePtr = 4;
        }
        break;
    }

    case 77:
        *statePtr = 78;
        /* fallthrough */

    case 78:
        s->unk66 = -1;
        s->unk5E += 256;
        if (s->unk5E >= 4096) {
            s->unk5E = 4096;
            *statePtr = 12;
        }
        func_801E4708(0, s->unk61);
        func_801E4B38(0, s->unk58, s->unk38);
        func_801E4848(1, s);
        break;

    case 79: {
        s32 i;
        u8 *item;
        s32 id;
        s32 count;
        for (i = 0; i < 202; i++) {
            buffer2[i] = 0;
        }
        item = (u8 *)s->itemSlots;
        for (i = 0; i < 198; i++) {
            id = *item++;
            count = *item++;
            if (id != 0 && count != 0) {
                buffer2[id] = count;
            }
        }
        item = (u8 *)s->itemSlots;
        for (i = 0; i < 198; i++) {
            *item++ = 0;
            *item++ = 0;
        }
        item = (u8 *)s->itemSlots;
        for (i = 1; i < 199; i++) {
            if (buffer2[i] != 0) {
                *item++ = i;
                *item++ = buffer2[i];
            }
        }
        func_801F7B60();
        func_801E4708(1, s->unk61);
        s->unk61 = 0;
        *statePtr = 3;
        break;
    }

    case 80: {
        GfSaveData *gf;
        gf = D_801EB4BC;
        gf += func_801E2E70(s->unk65);
        s->unk52 = func_801E2E70(s->unk65);
        func_801E2CCC(((u8 *)gf->completeAbilities)[2], ((u8 *)gf->completeAbilities)[3]);
        loadSubOverlay(7, 0x801CD000);
        loadSubOverlay(87, 0x801D1000);
        *statePtr = 81;
        break;
    }

    case 81:
        s->menuColorIntensity -= 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity <= 0) {
            s->menuColorIntensity = 0;
            func_801F1DB0(0);
            if (pollCdReadStatus() == 0) {
                *statePtr = 82;
            }
        }
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        break;

    case 82:
        *statePtr = 83;
        break;

    case 83:
        s->unk40 += 256;
        if (s->unk40 >= 4096) {
            s->unk40 = 4096;
            *statePtr = 84;
        }
        break;

    case 84:
        *statePtr = 85;
        /* fallthrough */

    case 85:
        if (func_801E2E70(s->unk65) != func_801E2E8C(s->unk65)) {
            if (btnFlags & PADLleft) {
                *statePtr = 86;
            } else {
                if (!(btnFlags & PADLright)) {
                    if (cfgFlags & PADRup) {
                        sendSpuCommand(3);
                        *statePtr = 90;
                    }
                    break;
                }
                *statePtr = 88;
            }
        } else {
            if (cfgFlags & PADRup) {
                sendSpuCommand(3);
                *statePtr = 90;
            }
        }
        break;

    case 86: {
        s32 val1;
        sendSpuCommand(1);
        val1 = s->unk52;
        s->unk53 = val1;
        val1--;
        if (val1 < func_801E2E70(s->unk65)) {
            val1 = func_801E2E8C(s->unk65);
        }
        s->unk50 = -3968;
        s->unk52 = val1;
        *statePtr = 87;
        break;
    }

    case 87:
        s->unk50 += 128;
        if (s->unk50 >= 0) {
            s->unk50 = 0;
            *statePtr = 85;
        }
        s++; s--;
        if (cfgFlags & PADLleft) {
            *statePtr = 86;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 88;
        }
        break;

    case 88: {
        s32 val1;
        sendSpuCommand(1);
        val1 = s->unk52;
        s->unk53 = val1;
        val1++;
        if (func_801E2E8C(s->unk65) < val1) {
            val1 = func_801E2E70(s->unk65);
        }
        s->unk50 = 3968;
        s->unk52 = val1;
        *statePtr = 89;
        break;
    }

    case 89:
        s->unk50 -= 128;
        if (s->unk50 <= 0) {
            s->unk50 = 0;
            *statePtr = 85;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 86;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 88;
        }
        break;
        
    case 90:
        *statePtr = 91;
        break;

    case 91:
        s->unk40 -= 256;
        if (s->unk40 <= 0) {
            s->unk40 = 0;
            *statePtr = 92;
        }
        break;

    case 92:
        *statePtr = 93;
        break;

    case 93:
        s->menuColorIntensity += 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            func_801F1DB0(4096);
            *statePtr = 5;
        }
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        break;

    case 94:
        s->unk52 = s->unk4A;
        s->unk58 = s->unk54;
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        *statePtr = 95;
        /* fallthrough */

    case 95:                        
        s->unk5A -= 256;
        if (s->unk5A <= 0) {
            s->unk5A = 0;
            *statePtr = 96;
        }
        func_801E4708(0, s->unk61);
        func_801E47E0(1, s->unk54);
        break;

    case 96:
        *statePtr = 97;
        /* fallthrough */

    case 97: {
        s32 val1;
        func_801E4708(0, s->unk61);
        val1 = s->unk54 / 11;
        s->unk54 = func_801F6768(btnFlags, 11, s->unk54 % 11) + (val1 * 11);
        func_801E47E0(1, s->unk54);
        func_801E4BB4(s);
        if (btnFlags & PADLleft) {
            s->returnState = 97;
            *statePtr = 6;
        }
        if (btnFlags & PADLright) {
            s->returnState = 97;
            *statePtr = 8;
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 105;
        }
        if (btnFlags & PADRdown) {
            sendSpuCommand(2);
            *statePtr = 98;
        }
        break;
    }

    case 98:
        *statePtr = 99;
        /* fallthrough */

    case 99: {
        s32 page;
        s32 rowInPage;
        page = s->unk58 / 11;
        rowInPage = s->unk58 % 11;
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        s->unk58 = func_801F6768(btnFlags, 11, rowInPage) + (page * 11);
        func_801E476C(1, s);
        func_801E4C14(s);
        if (btnFlags & PADLleft) {
            *statePtr = 101;
        }
        if (btnFlags & PADLright) {
            *statePtr = 103;
        }
        if (!(cfgFlags & PADRdown)) {
            if (cfgFlags & PADRup) {
                sendSpuCommand(3);
                *statePtr = 97;
            }
        } else {
            state = 100;
            goto restart;
        }
        break;
    }

    case 100: {
        s32 id1;
        s32 id2;
        s32 count1;
        s32 count2;
        ItemSlot *item1;
        ItemSlot *item2;
        id1 = s->itemSlots[s->unk54].id;
        count1 = s->itemSlots[s->unk54].count;
        id2 = s->itemSlots[s->unk58].id;
        count2 = s->itemSlots[s->unk58].count;
        if (id1 == id2) {
            sendSpuCommand(5);
            *statePtr = 99;
        } else {
            item1 = &s->itemSlots[s->unk54];
            item2 = &s->itemSlots[s->unk58];
            item1->id = id2;
            item1->count = count2;
            item2->id = id1;
            item2->count = count1;
            sendSpuCommand(2);
            *statePtr = 97;
        }
        break;
    }

    case 101: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->unk53 = s->unk52;
        page = s->unk58 / 11;
        rowInPage = s->unk58 % 11;
        page--;
        if (page < 0) {
            page = 17;
        }
        s->unk58 = rowInPage + (page * 11);
        s->unk52 = page;
        s->unk50 = -3687;
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        func_801E476C(1, s);
        s->previousItemDesc = s->itemDesc;
        func_801E4C14(s);
        *statePtr = 102;
        break;
    }

    case 102:
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        func_801E476C(1, s);
        s->unk50 += 409;
        if (s->unk50 >= 0) {
            s->unk50 = 0;
            *statePtr = 99;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 101;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 103;
        }
        break;

    case 103: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->unk53 = s->unk52;
        page = s->unk58 / 11;
        rowInPage = s->unk58 % 11;
        page++;
        if (page > 17) {
            page = 0;
        }
        s->unk58 = rowInPage + (page * 11);
        s->unk52 = page;
        s->unk50 = 3687;
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        func_801E476C(1, s);
        s->previousItemDesc = s->itemDesc;
        func_801E4C14(s);
        *statePtr = 104;
        break;
    }

    case 104:
        func_801E4708(0, s->unk61);
        func_801E47E0(0, s->unk54);
        func_801E476C(1, s);
        s->unk50 -= 409;
        if (s->unk50 <= 0) {
            s->unk50 = 0;
            *statePtr = 99;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 101;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 103;
        }
        break;

    case 105:
        *statePtr = 106;
        /* fallthrough */

    case 106:
        func_801E4708(0, s->unk61);
        s->unk5A += 256;
        if (s->unk5A >= 4096) {
            s->unk5A = 4096;
            *statePtr = 2;
        }
        break;

    case 107: {
        s32 val1;
        val1 = func_801E2E70(s->unk65);
        val1 = 1 << val1;
        state = 108;
        if (D_80077E8C & val1) {
            state = 109;
        }
        goto restart;
    }

    case 108: {
        u8 *msg;
        s32 val1;
        sendSpuCommand(8);
        func_801E457C(s->unk65, s->itemSlots, s->unk54);
        msg = func_801F08D4(1, 9, 15, 0);
        func_801E2BA4(msg, D_801ECC20, func_801E2E70(s->unk65));
        s->unk30 = D_801ECC20;
        val1 = func_801E2E70(s->unk65);
        val1 = 1 << val1;
        g_gameState.mainData.limitBreaks.quistisLimits |= val1;
        state = 110;
        goto restart;
    }

    case 109: {
        u8 *msg;
        sendSpuCommand(5);
        msg = func_801F08D4(1, 9, 16, 0);
        func_801E2BA4(msg, D_801ECC20, func_801E2E70(s->unk65));
        state = 110;
        s->unk30 = D_801ECC20;
        goto restart;
    }

    case 110:
        s->unk62 = findNthSetBit(s->unk38, s->unk58);
        s->unk5C = 600;
        *statePtr = 111;
        break;

    case 111:
        s->unk62 = findNthSetBit(s->unk38, s->unk58);
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk5C = 0;
            s->unk30 = NULL;
            *statePtr = 4;
        }
        if (s->unk5C < 0) {
            s->unk5C = 0;
            s->unk30 = NULL;
            *statePtr = 4;
        }
        break;

    case 112:
        loadOverlayWithTimCallback(9, 0x801CD000);
        loadOverlayWithTimCallback(10, 0x801D5000);
        *statePtr = 113;
        break;

    case 113:
        s->menuColorIntensity -= 256;
        if (s->menuColorIntensity < 0) {
            s->menuColorIntensity = 0;
            if (pollCdReadStatus() == 0) {
                func_801F1CAC();
                func_801F72C4();
                func_801F18FC(s);
                func_801F0BB0();
            }
        }
        if (D_8008520C != 0) {
            func_801E4708(0, s->unk61);
            func_801E47E0(1, s->unk54);
            break;
        }
        func_801E4708(1, s->unk61);
        break;
    }

    setTextBrightness(s->unk40);
    func_801F0948(s->menuColorIntensity);
    if (D_8008520C != 0) {
        func_801F1DB0(s->menuColorIntensity);
    }
}

void *func_801E7D18(void *arg0, void *arg1, s32 arg2, s32 arg3, s32 arg4) {
    ItemSlot *item;
    s32 id;
    s32 count;
    s32 color;
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 x3;
    s32 y3;
    s32 x4;
    s32 y4;
    s32 x5;
    s32 y5;
    s32 tmp;

    item = g_menuDisplayCfg.dataPtr;
    item += arg2 * 11 + arg3;

    id = item->id;
    count = item->count;
    if (id == 0 || count == 0) {
        return arg1;
    }

    x1 = arg4 + 0xD;
    x2 = g_menuDisplayCfg.x + x1;

    y1 = arg3 * 0xD + 9;
    y2 = g_menuDisplayCfg.y + y1;

    tmp++; tmp--;

    if (func_801E2F88(id) != 0) {
        color = 7;
    } else {
        color = 1;
    }

    x3 = x2;
    y3 = y2 - 2;
    arg1 = drawIcon(arg0, arg1, func_801F6B28(id) + 0xDF, x3, y3, g_menuTint[0]);

    tmp = y2;

    x4 = x2 + 0xD;
    y4 = tmp;
    arg1 = func_801F0FEC(arg0, arg1, x4, y4, getItemName(id), color);

    x5 = x2 + 0x91;
    y5 = tmp;
    arg1 = drawNumberMenuTint(arg0, arg1, (y5 << 0x10) | (x5 & 0xFFFF), count, color);

    return arg1;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7E74);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7F4C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8024);

/**
 * @brief Render an item description text for a specific table entry.
 *
 * Reads a data pointer from g_menuDisplayCfg[0x20] indexed by @p a2. If the pointer
 * is non-null, decodes the string via decodeMessage into a local buffer, then
 * renders it via func_801F0FEC at a position derived from g_menuDisplayCfg fields.
 *
 * @param a0 Rendering context pointer.
 * @param a1 Current rendering state (returned unchanged if entry is null).
 * @param a2 Table entry index.
 * @param arg5 Additional offset added to g_menuDisplayCfg X position.
 * @return Updated rendering state after text is rendered.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E80D0);

/**
 * @brief Configure item list display and register render callback.
 *
 * Sets up g_menuDisplayCfg display config with icon 0x55, dimensions 0x144 x 0x1A,
 * scroll enabled, and page mode. Computes total item count from three fields
 * at offsets 0x48, 0x4C, and 0x50 of the source struct. Registers
 * func_801E80D0 as the display callback via func_801EFBB4.
 *
 * @param a0 Source data structure (offset 0x28 stored as data pointer).
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 X position for display config.
 * @param arg4 Y position for display config.
 */
void func_801E8180(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x144;
    g_menuDisplayCfg.h = 0x1A;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.scrollOffset = *(u16 *)(a0 + 0x48) + *(u16 *)(a0 + 0x4C) + *(u16 *)(a0 + 0x50);
    g_menuDisplayCfg.dataPtr = (s32)(a0 + 0x28);
    func_801EFBB4(a1, a2, func_801E80D0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E820C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E82CC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E83B4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E84A4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E859C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8684);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8780);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E88AC);

/**
 * @brief Look up sprite data address from table at 0x801D1000.
 *
 * Reads halfword offset at index a0 from the table, adds to base.
 *
 * @param a0 Sprite index.
 * @return Base address + halfword offset from table entry.
 */
s32 func_801E89A4(s32 a0) {
    s32 base = 0x801D1000;
    return *(u16 *)(base + a0 * 2 + 2) + base;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E89C0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8AF0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8C88);

/** @brief Return base address of item sprite data (0x801CD000). */
s32 func_801E8DA4(void) {
    return 0x801CD000;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8DB0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8E98);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8FA8);

/**
 * @brief Render complete item sprite with palette, tiles, text and effects.
 *
 * Chains several rendering functions: func_801E8E98 (palette/icon setup),
 * func_801E89C0 (sprite tiles), func_801E8AF0 (additional graphics),
 * then marks an Angelo trick as known (the item's byte at +0x1B is the
 * trick index, 0xFF meaning the item teaches none). Finally calls func_801E8DB0 (text labels),
 * func_801E8C88 (color overlay), func_8002B898 (border), and
 * func_801E8FA8 (effects). Each chained function receives the return
 * value of the previous one as its second argument.
 *
 * @param a0 Rendering context pointer.
 * @param a1 Initial rendering state.
 * @param a2 Display configuration pointer.
 * @param a3 OT (ordering table) pointer.
 * @param arg5 Item data pointer.
 */
void func_801E90D8(s32 a0, s32 a1, MenuDisplayConfig *cfg, s32 a3, s32 arg5) {
    s32 ctx = a0;
    s32 ot = a3;
    s32 data = arg5;
    s32 result;
    s32 bit;

    result = func_801E8E98(a0, a1, cfg, a3, data);
    result = func_801E89C0(ctx, result, cfg, ot, data);
    result = func_801E8AF0(ctx, result, cfg, ot, data);

    if (*(u8 *)(data + 0x1B) != 0xFF) {
        s32 trick = *(u8 *)(data + 0x1B);
        s32 known = g_gameState.mainData.limitBreaks.angeloKnown;
        bit = 1 << trick;
        g_gameState.mainData.limitBreaks.angeloKnown = known | bit;
    }

    result = func_801E8DB0(ctx, result, cfg, ot, data);
    result = func_801E8C88(ctx, result, cfg, ot, data);
    result = func_8002B898(ctx, result, cfg, ot);
    func_801E8FA8(ctx, result, cfg, ot, data);
}

/**
 * @brief Configure display rect from template and render item sprite data.
 *
 * Copies a 4-halfword rectangle template from @p arg5, adding @p a2 to the
 * X position and @p a3 to the Y position. Stores the result in g_menuDisplayCfg,
 * then calls func_801E90D8 to render with the display configuration and the
 * menu tint g_menuTint.
 *
 * @param a0 First parameter passed through to func_801E90D8.
 * @param a1 Second parameter passed through to func_801E90D8.
 * @param a2 X offset to add to template X.
 * @param a3 Y offset to add to template Y.
 * @param arg5 Pointer to 4-halfword rectangle template.
 */
void func_801E91E4(s32 a0, s32 a1, s32 a2, s32 a3, u16 *src) {
    g_menuDisplayCfg.x = src[0] + a2;
    g_menuDisplayCfg.y = src[1] + a3;
    g_menuDisplayCfg.w = src[2];
    g_menuDisplayCfg.h = src[3];
    func_801E90D8(a0, a1, &g_menuDisplayCfg, g_menuTint[MENU_TINT_NORMAL], src);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9248);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E934C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E95C4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9AAC);

/**
 * @brief Initialize item menu system.
 *
 * Sets up the main item handler and three data tables, clears six global
 * state words, calls display init, resets menu state. If the handler setup
 * returned a context pointer, initializes it with item data and renders.
 */
void func_801E9B98(void) {
    MenuItemState *s;

    s = func_801F179C(func_801E4EA4, func_801E95C4);
    func_801F1D2C(0, D_801EB17C, D_801EB330);
    func_801F1D2C(0, D_801EB188, D_801EB4BC);
    func_801F1D2C(0, D_801EB194, D_801EC710);
    D_801ECE24 = 0;
    D_801ECE28 = 0;
    D_801ECE2C = 0;
    D_801ECE30 = 0;
    D_801ECE34 = 0;
    D_801ECE38 = 0;
    func_801F0948(0);
    func_801E3E94();
    if (s != NULL) {
        func_801E9AAC(s);
        loadOverlayWithTimCallback(0xC, 0x801D5000);
        s->unk40 = 0;
        func_801E4EA4(s);
        s->menuColorIntensity = 0;
    }
    func_801F7B60();
}

/**
 * @brief Count leading non-null bytes in a string, capped at limit-1.
 *
 * Scans up to a1 bytes from a0, counting non-null bytes. Returns the count
 * of consecutive non-null bytes found, or (a1 - 1) if the limit is reached.
 *
 * @param a0 Pointer to byte string.
 * @param a1 Maximum number of bytes to scan.
 * @return Count of leading non-null bytes, capped at a1-1.
 */
/**
 * @brief Count leading non-null bytes in a string, capped at limit-1.
 *
 * Scans up to a1 bytes from a0, counting non-null bytes. Returns the count
 * of consecutive non-null bytes found, or (a1 - 1) if the limit is reached.
 *
 * @param a0 Pointer to byte string.
 * @param a1 Maximum number of bytes to scan.
 * @return Count of leading non-null bytes, capped at a1-1.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9C90);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9CD4);

/**
 * @brief Find the last non-zero byte in a string and set it to zero.
 *
 * Scans forward through the byte string at a0 until the first zero byte,
 * then writes zero to the byte before the terminator (the last non-zero byte).
 * If the first byte is already zero, does nothing.
 *
 * @param a0 Pointer to a null-terminated byte string.
 */
void func_801E9DE4(u8 *a0) {
    if (*a0++ == 0) {
        return;
    }
    while (*a0++ != 0) {
    }
    a0[-2] = 0;
}

/**
 * @brief Check if all bytes in string match the first byte of entry 0xB.
 *
 * Calls btlStrlen first as a precondition. If it returns 0, returns 0.
 * Otherwise iterates through each byte at a0 until a zero terminator,
 * comparing against the first byte of the entry returned by getMenuString(0xB).
 * Returns 1 as soon as a mismatch is found, 0 if all match or string is empty.
 *
 * @param a0 Pointer to a null-terminated byte string.
 * @return 1 if a mismatch is found, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9E10);

/** @brief Look up string @p a0 in menu text category 5. */
u8 *func_801E9E7C(s32 a0) {
    return func_801F08D4(1, 5, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9EA8);

/**
 * @brief Initialize item sub-menu.
 *
 * Sets up the sub-menu handler via func_801F179C, initializes display state,
 * reads button input to determine item type. If the context pointer is valid,
 * sets up the data table pointer, string, and various byte fields, then
 * calls func_801E9F94 to render.
 */
