#include "common.h"
#include "menu.h"
#include "menuitem2.h"
#include "ui/text.h"
#include "menuitem.h"

/* Second translation unit of the item menu. splat's jumptable-alignment
 * heuristic placed the file boundary at 0x801E9F94, and the jump tables from
 * 0x801EB1A0 onward are used only by the functions below, which confirms it.
 */

void func_801E9F94(AnotherItemMenuState *s) {
    UNKST_2 *st;
    u16 btnFlags;
    u16 cfgFlags;
    u16 *statePtr;

    func_801F0948(s->unk28);
    btnFlags = g_menuDisplayCfg.inputRepeat;
    cfgFlags = g_menuDisplayCfg.inputNew;
    st = &D_801EB320[s->unk2C];
    statePtr = &s->state;

    switch (*statePtr) {
    case 0:
        s->unk28 = 0;
        *statePtr = 1;
        break;

    case 1:
        s->unk28 += 256;
        if (s->unk28 >= 4096) {
            s->unk28 = 4096;
            *statePtr = 2;
        }
        func_801E9CD4(s);
        break;

    case 2:
        s->unk2F = 0;
        *statePtr = 3;
        func_801E9CD4(s);
        break;

    case 3: {
        s32 page;
        s32 row;
        row = s->unk2D % 14;
        page = s->unk2D / 14;
        if (btnFlags & 0x1000) {
            page--;
            if (page < 0) {
                page = 0;
            } else {
                sendSpuCommand(1);
            }
        }
        if (btnFlags & 0x4000) {
            if (page < st->unk7) {
                sendSpuCommand(1);
            }
            page++;
        }
        if (page > st->unk7) {
            page = st->unk7;
        }
        if (btnFlags & 0x2000) {
            row++;
            if (row > 13) {
                row = 13;
            } else {
                sendSpuCommand(1);
            }
        }
        if (btnFlags & 0x8000) {
            sendSpuCommand(1);
            row--;
            if (row < 0) {
                row = 0;
                *statePtr = 4;
            }
        }
        s->unk2D = row + page * 14;
        func_801E9CD4(s);
        if (cfgFlags & 0x10) {
            sendSpuCommand(3);
            func_801E9DE4(s->unk24);
        } else if (btnFlags & 0x40) {
            sendSpuCommand(2);
            func_801E9EA8(s, 1);
        } else {
            if (cfgFlags & 8) {
                sendSpuCommand(1);
                s->unk2C++;
                if (s) { s->unk2C++; s->unk2C--; }
                s->unk2C &= 1;
            }
            if (cfgFlags & 4) {
                s32 val1;
                s32 val2;
                sendSpuCommand(1);
                val1 = s->unk2C;
                val1--;
                if (val1 < 0) {
                    val1 += 2;
                }
                val2 = val1 + ((u32)val1 >> 0x1F);
                val1 -= (val2 >> 1) * 2;
                s->unk2C = val1;
            }
            if (cfgFlags & 0x800) {
                sendSpuCommand(1);
                s->unk2E = 3;
                *statePtr = 4;
            }
        }
        break;
    }

    case 4:
        s->unk2F = 1;
        *statePtr = 5;
        break;

    case 5: {
        s32 unk2E;
        unk2E = s->unk2E;
        if (cfgFlags & 8) {
            sendSpuCommand(1);
            s->unk2C++;
            if (s) { s->unk2C++; s->unk2C--; }
            s->unk2C &= 1;
        }
        if (cfgFlags & 4) {
            s32 val1;
            s32 val2;
            sendSpuCommand(1);
            val1 = s->unk2C;
            val1--;
            if (val1 < 0) {
                val1 += 2;
            }
            val2 = val1 + ((u32)val1 >> 0x1F);
            val1 -= (val2 >> 1) * 2;
            s->unk2C = val1;
        }
        if (btnFlags & 0x4000) {
            sendSpuCommand(1);
            unk2E++;
            if (unk2E > 5) {
                unk2E = 0;
            }
        }
        if (btnFlags & 0x1000) {
            sendSpuCommand(1);
            unk2E--;
            if (unk2E < 0) {
                unk2E = 5;
            }
        }
        if (cfgFlags & 0x40) {
            switch (unk2E) {
            case 1:
                sendSpuCommand(2);
                s->unk2C = 1;
                break;
            case 0:
                sendSpuCommand(2);
                s->unk2C = 0;
                break;
            case 2:
                sendSpuCommand(2);
                func_801E9EA8(s, 0);
                break;
            case 4:
                sendSpuCommand(3);
                func_801E9DE4(s->unk24);
                break;
            case 3:
                if (func_801E9E10(s->unk24) != 0) {
                    sendSpuCommand(2);
                    *statePtr = 6;
                    break;
                }
                sendSpuCommand(5);
                break;
            case 5:
                sendSpuCommand(2);
                copyString(s->unk24, func_801E9E7C(s->unk20->unk2));
                break;
            }
        }
        if (cfgFlags & 0x800) {
            sendSpuCommand(1);
            unk2E = 3;
        }
        s->unk2E = unk2E;
        func_801E9CD4(s);
        if (btnFlags & 0x2000) {
            sendSpuCommand(2);
            *statePtr = 1;
        }
        break;
    }

    case 6:
        recalcPartyStats();
        func_801E9CD4(s);
        *statePtr = 7;
        break;

    case 7:
        s->unk28 -= 256;
        if (s->unk28 < 0) {
            s->unk28 = 0;
            func_801F18FC(s);
            func_801F0BB0();
            *statePtr = 2;
        }
        func_801E9CD4(s);
        break;
    }

    s->unk30 = func_801E9C90(s->unk24, s->unk20->unk0);
    func_801F0948(s->unk28);
}

/**
 * @brief Call func_800375A0 with rearranged args and g_menuTint[MENU_TINT_NORMAL] as 6th arg.
 * @param a0 First parameter passed through
 * @param a1 Second parameter passed through
 * @param a2 Becomes 4th argument to callee
 * @param a3 Becomes 5th argument (on stack) to callee
 * @param arg5 Becomes 3rd argument to callee
 */
s32 func_801EA500(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {

    return func_800375A0(a0, a1, arg5, a2, a3, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem2", func_801EA538);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem2", func_801EA714);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem2", func_801EA7E0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem2", func_801EA8F0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem2", func_801EAA04);

/**
 * @brief Render a menu panel with text and display configuration.
 *
 * Calls func_801EAA04 with adjusted position args (a3+8 for width,
 * arg5+0xA pushed to stack). Configures g_menuDisplayCfg with panel position,
 * size, and display properties, then calls func_801EF9AC with the result
 * and the menu tint g_menuTint.
 *
 * @param a0 First parameter passed through.
 * @param a1 Text data parameter.
 * @param a2 Second parameter passed through.
 * @param a3 X position for panel.
 * @param arg5 Y position for panel.
 */
s32 func_801EAB00(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    s32 result;

    result = func_801EAA04(a0, a1, a2, a3 + 8, arg5 + 0xA);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x102;
    g_menuDisplayCfg.y = arg5;
    g_menuDisplayCfg.h = 0x7D;
    return func_801EF9AC(a1, result, 0x1000, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem2", func_801EAB8C);

/**
 * @brief Render item detail sub-menu with multiple panel sections.
 *
 * Switches the menu brightness to the context's value (setMenuBrightness)
 * and, after drawing, restores the previous one read from @c g_menuBrightness; the early
 * return below leaves the new value in place. If the display mode
 * returned by func_801F0D84 is 0xF, renders several sub-panels: item name
 * (func_801EA500), description (func_801EA538), icon (func_801EA714),
 * stats (func_801EA7E0), info (func_801EAB00), and list (func_801EAB8C).
 * Otherwise returns the current rendering state unchanged.
 *
 * @param a0 Item menu context pointer.
 * @param a1 Rendering context pointer.
 * @param a2 Current rendering state.
 * @return Updated rendering state after all panels are drawn.
 */
s32 func_801EAC54(s32 a0, s32 a1, s32 a2) {
    s32 ctx = a0;
    s32 render = a1;
    s32 state = a2;
    s32 saved = g_menuBrightness;
    s32 result;
    s32 qty;

    setMenuBrightness(*(s32 *)(ctx + 0x28));
    if (func_801F0D84() != 0xF) {
        return state;
    }
    qty = *(u8 *)(*(s32 *)(ctx + 0x20) + 1);
    result = func_801EA500(render, state, 0x30, 0x22, qty);
    state = 0x37;
    result = func_801EA538(ctx, render, result, 0x6A, state);
    result = func_801EA714(render, result, 0x6A, 0x1D);
    state = 0x58;
    result = func_801EA7E0(ctx, render, result, 0x18, state);
    result = func_801EAB00(ctx, render, result, 0x6A, state);
    result = func_801EAB8C(render, result, 0x10E, 0x6);
    state = result;
    setMenuBrightness(saved);
    return state;
}

/**
 * @brief Initialize item sub-menu.
 *
 * Sets up the sub-menu handler via func_801F179C, initializes display state,
 * reads button input to determine item type. If the context pointer is valid,
 * sets up the data table pointer, string, and various byte fields, then
 * calls func_801E9F94 to render.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem2", func_801EAD64);
