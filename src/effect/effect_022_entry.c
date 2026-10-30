#include "common.h"
#include "effect.h"
#include "effect/lib/entity.h"
#include "effect/effect_022.h"
#include "effect/effect_022_entry.h"

void *func_801A0000(EffectAnimSet *arg0) {
    EffectEntity *entity;
    u8 slot;

    slot = arg0->anims->parts->unk000;
    func_800B2A00(&D_801E3EB8, &D_801E3DE8, 100, 2);
    entity = effectSpawnTask(&D_801E3EB8, func_801A8CF4, 100, NULL);
    entity->animSet = arg0;
    entity->unk02D = slot;
    entity->unk05A = arg0->anims->unk010;
    entity->unk058 = arg0->anims->unk011;
    if (!(arg0->flags & EFFECT_ANIMSET_FLAG_LOADED)) {
        func_800C3BE0(&D_801BA9F0);
        func_800BB084(&D_801BB308);
    }
    D_801E3DE0 = &D_801BB308;
    D_801E3DE4 = &D_801C7308;
    func_800B2A00(&D_801E40F8, &D_801E3EC8, 140, 4);
    func_800B2A00(&D_801E4338, &D_801E4108, 56, 10);
    func_800B2A00(&D_801EC688, &D_801E4348, 112, 300);
    func_800B2A00(&D_801ED278, &D_801EC698, 152, 20);
    func_800B2A00(&D_801ED308, &D_801ED288, 60, 2);
    return &D_801E3EB8;
}
