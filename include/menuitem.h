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

/**
 * @brief Picture/text overlay drawn in the magazine page.
 */
typedef struct {
    u16 x; /**< 0x01: X position. */
    u8 y;/**< 0x03: Y position. */
    u8 id; /**< 0x04: ID. */
} MagazinePageOverlay;

/**
 * @brief Weapon magazine page. Each entry repesents one page view (mmag.bin content).
 */
typedef struct {
    u16 windowX; /**< 0x00: Window X. */
    u16 windowY; /**< 0x02: Window Y. */
    u16 windowWidth; /**< 0x04: Window width. */
    u16 windowHeight; /**< 0x06: Window height. */
    u16 paperMatX; /**< 0x08: Paper mat X. */
    u16 paperMatY; /**< 0x0A: Paper mat Y. */
    u16 paperMatWidth; /**< 0x0C: Paper mat width. */
    u16 paperMatHeight; /**< 0x0E: Paper mat height. */
    u8 paperMatRed; /**< 0x10: Paper mat red. */
    u8 paperMatGreen; /**< 0x11: Paper mat green. */
    u8 paperMatBlue; /**< 0x12: Paper mat blue. */
    u8 paperBgParamA; /**< 0x13: Paper Background parameter A. */
    u8 paperBgParamB; /**< 0x14: Paper Background parameter B. */
    u8 textFileIndex; /**< 0x15: Text file index. */
    u8 pageTextureCategory; /**< 0x16: Page texture category. */
    u8 pageTexture; /**< 0x17: Page texture page number. */
    u8 weaponId; /**< 0x18: Weapon ID. */
    u8 weaponLineSpacing; /**< 0x19: Weapon line spacing. */
    u8 duelId; /**< 0x1A: Zell limit break duel ID. */
    u8 angeloId; /**< 0x1B: Angelo ID. */
    u16 weaponListX; /**< 0x1C: Weapon list X. */
    u8 weaponListY; /**< 0x1E: Weapon list Y. */
    u8 weaponXOffset; /**< 0x1F: Weapon quantity column X offset. */
    u16 duelComboX; /**< 0x20: Duel combo X. */
    u8 duelComboY; /**< 0x22: Duel combo Y. */
    u8 footerFlag; /**< 0x23: Footer flag. */
    MagazinePageOverlay pictureOverlays[4]; /**< 0x24: Picture overlays in the page. */
    MagazinePageOverlay textOverlays[4]; /**< 0x34: Text overlays in the page. */
} MagazinePageEntry; /** 0x44 bytes */

extern s32 D_801ECC10;
extern s32 D_801ECE20;
extern s32 D_801ECE24;
extern s32 D_801ECE28;
extern s32 D_801ECE2C;
extern s32 D_801ECE30;
extern s32 D_801ECE34;
extern s32 D_801ECE38;
extern s32 D_801ECEDC;
extern s32 D_801ECEE0;
extern s32 D_801ECEE4;
extern s32 D_801ECEE8;
extern u8 D_801EB17C[];
extern u8 D_801EB188[];
extern u8 D_801EB194[];
extern s16 D_801EB1D8[];
extern WeaponRecipe D_801EB330[WEAPON_RECIPE_COUNT]; /**< Weapon recipes (mwepon.bin content). */
extern MagazinePageEntry D_801EB4BC[]; /**< Magazine pages (mmag.bin content). */
extern u8 D_801EC710[]; /**< Items required to unlock Doomtrain GF (mthomas.bin content). */
extern ItemSlot D_801ECB20[];
extern AbilityListEntry D_801ECB60[];
extern u8 func_801E2EA8(s32);
extern s32 func_801EFFD4(void);

#endif /* MENUITEM_H */
