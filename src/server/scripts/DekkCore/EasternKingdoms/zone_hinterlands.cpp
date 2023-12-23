/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "MotionMaster.h"
#include "Player.h"
#include "ScriptedEscortAI.h"

enum TrainedRazorbeak
{
    QUEST_RAZORBEAKFRIENDS = 26546,
    SPELL_FEED_RAZORBEAK = 80782,
    NPC_RAZORBEAK_CREDIT = 43236
};

struct npc_trained_razorbeak : public ScriptedAI
{
    npc_trained_razorbeak(Creature* creature) : ScriptedAI(creature) {}

    void SpellHit(WorldObject* caster, const SpellInfo* spell) override
    {
        if (spell->Id != SPELL_FEED_RAZORBEAK)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_RAZORBEAKFRIENDS) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(NPC_RAZORBEAK_CREDIT, ObjectGuid::Empty);
    }

};

enum FacesEvil
{
    NPC_MASK_BURNT_CREDIT = 42704
};

class spell_tiki_torch : public SpellScript
{
    PrepareSpellScript(spell_tiki_torch);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Player* player = GetCaster()->ToPlayer())
        {
            if (Creature* target = GetHitCreature())
            {
                player->RewardPlayerAndGroupAtEvent(NPC_MASK_BURNT_CREDIT, GetCaster());
                target->DisappearAndDie();
            }
        }
    }

    void SelectTarget(WorldObject*& target)
    {
        target = GetCaster()->FindNearestCreature(NPC_MASK_BURNT_CREDIT, 15.0f, true);
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_tiki_torch::SelectTarget, EFFECT_0, TARGET_UNIT_NEARBY_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_tiki_torch::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum RitualShadra
{
    NPC_SHADRA_NW_ALTAR_BUNNY = 43067,
    NPC_SHADRA_SW_ALTAR_BUNNY = 43068,
    NPC_SHADRA_E_ALTAR_BUNNY = 43069
};

class spell_ritual_of_shadra : public SpellScript
{
    PrepareSpellScript(spell_ritual_of_shadra);

    void SelectTarget(WorldObject*& target)
    {
        target = GetCaster()->FindNearestCreature(NPC_SHADRA_NW_ALTAR_BUNNY, 15.0f, true);
        if (!target)
            target = GetCaster()->FindNearestCreature(NPC_SHADRA_SW_ALTAR_BUNNY, 15.0f, true);
        if (!target)
            target = GetCaster()->FindNearestCreature(NPC_SHADRA_E_ALTAR_BUNNY, 15.0f, true);
    }

    SpellCastResult CheckRequirement()
    {
        Creature* northwest = GetCaster()->FindNearestCreature(NPC_SHADRA_NW_ALTAR_BUNNY, 15.0f, true);
        Creature* southwest = GetCaster()->FindNearestCreature(NPC_SHADRA_SW_ALTAR_BUNNY, 15.0f, true);
        Creature* east = GetCaster()->FindNearestCreature(NPC_SHADRA_E_ALTAR_BUNNY, 15.0f, true);

        if (!northwest && !southwest && !east)
            return SPELL_FAILED_INCORRECT_AREA;
        return SPELL_CAST_OK;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Creature* hitCreature = GetHitCreature();
        Player* player = GetCaster()->ToPlayer();
        if (!hitCreature || !player)
            return;

        switch (hitCreature->GetEntry())
        {
        case NPC_SHADRA_NW_ALTAR_BUNNY:
            player->RewardPlayerAndGroupAtEvent(hitCreature->GetEntry(), GetCaster());
            break;
        case NPC_SHADRA_SW_ALTAR_BUNNY:
            player->RewardPlayerAndGroupAtEvent(hitCreature->GetEntry(), GetCaster());
            break;
        case NPC_SHADRA_E_ALTAR_BUNNY:
            player->RewardPlayerAndGroupAtEvent(hitCreature->GetEntry(), GetCaster());
            break;
        default:
            break;
        }
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_ritual_of_shadra::SelectTarget, EFFECT_0, TARGET_UNIT_NEARBY_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_ritual_of_shadra::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCheckCast += SpellCheckCastFn(spell_ritual_of_shadra::CheckRequirement);
    }
};

void AddSC_DekkCore_hinterlands()
{
    RegisterCreatureAI(npc_trained_razorbeak);
    RegisterSpellScript(spell_tiki_torch);
    RegisterSpellScript(spell_ritual_of_shadra);
}
