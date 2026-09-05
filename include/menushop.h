#ifndef MENUSHOP_H
#define MENUSHOP_H

#define SHOP_COUNT 20
#define SHOP_ITEM_COUNT 16
#define ITEM_PRICE_COUNT 200
#define ITEM_SLOT_COUNT 198 

typedef struct {
    u8 itemId;      /**< 0x0: item ID. */
    u8 rarity;      /**< 0x1: item rarity (0x00 = Rare, 0xFF = Common). */
} ShopItemRarity; /* 0x2 = 2 bytes */

typedef struct {
    u8 itemId;      /**< 0x0: item ID. */
    u8 visible;     /**< 0x1: item visibility flag. */
} ShopItemVisibility; /* 0x2 = 2 bytes */

typedef struct {
    u16 basePrice;  /**< 0x0: base price. */
    u8 sellRate;    /**< 0x2: sell rate. */
} ShopItemPrice; /* 0x3 = 3 bytes */

typedef struct {
    u8 unk0[0xC7];
} Struct_D_801EB088;

typedef struct {
    u8 pad000[0x20];
    s32 *unk20;
    u8 pad024[0x12];
    u16 unk36;
} Struct_801E8B60;

extern ShopData D_80077CC8[SHOP_COUNT]; /**< Shop data table. */
extern u8 D_80077EBC[ITEM_SLOT_COUNT]; /**< Item slot inventory. */
extern ShopItemPrice D_801EA3F0[ITEM_PRICE_COUNT]; /**< Item price table. */
extern u8 D_801EA70C[];
extern u8 D_801F7F98[];
extern u8 D_801E9B64[];
extern u8 D_801E9B6C[];
extern ShopItemRarity D_801EA170[SHOP_COUNT][SHOP_ITEM_COUNT]; /**< Shop item rarity tables. */
extern ShopItemVisibility D_801EAA28[SHOP_ITEM_COUNT]; /**< Shop item visibility table. */
extern s32 D_801EAA48[ITEM_PRICE_COUNT]; /**< Item sell prices. */
extern s32 D_801EAD68[ITEM_PRICE_COUNT]; /**< Item buy prices. */
extern Struct_D_801EB088 D_801EB088;
extern s32 g_menuColor;
extern s32 D_80077E70;
extern s32 func_801E8AB0;
extern void func_801E5E90(void*);
extern s32 func_801E79D4(void*, s32, s32);

#endif /* MENUSHOP_H */
