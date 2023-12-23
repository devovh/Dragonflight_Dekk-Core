/*
 * Copyright 2023 Fluxurion
 */

#include "Flux.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectMgr.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "ScriptMgr.h"

class spell_warboard_alliance_261654 : public SpellScriptLoader
{
public:
    spell_warboard_alliance_261654() : SpellScriptLoader("spell_warboard_alliance_261654") {}

    class spell_warboard_alliance_261654_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warboard_alliance_261654_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            WorldObject* caster = GetCaster();

            if (!GetHitUnit()->IsPlayer())
                return;

            Player* player = GetHitUnit()->ToPlayer();

            Fluxurion::SendPlayerChoice(player, caster->GetGUID(), 352);
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_warboard_alliance_261654_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };
};

class spell_warboard_horde_261655 : public SpellScriptLoader
{
public:
    spell_warboard_horde_261655() : SpellScriptLoader("spell_warboard_horde_261655") {}

    class spell_warboard_horde_261655_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warboard_horde_261655_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            WorldObject* caster = GetCaster();

            if (!GetHitUnit()->IsPlayer())
                return;

            Player* player = GetHitUnit()->ToPlayer();

            Fluxurion::SendPlayerChoice(player, caster->GetGUID(), 342);
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_warboard_horde_261655_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };
};

// Alliance gob: 281339  |  Horde gob: 281340
struct go_warboard : public GameObjectAI
{
    go_warboard(GameObject* go) : GameObjectAI(go) { }

    bool OnReportUse(Player* player)
    {
        if (player->GetLevel() < 10)
            return false;

        if (player->GetTeam() == ALLIANCE)
            //me->CastSpell(player, 261654, true);
            Fluxurion::SendPlayerChoice(player, me->GetGUID(), 352); // forced version

        if (player->GetTeam() == HORDE)
            //me->CastSpell(player, 261655, true);
            Fluxurion::SendPlayerChoice(player, me->GetGUID(), 342); // forced version

        if (Fluxurion::HasQuest(player, 58903))
        {
            Fluxurion::AddCreditForQuestObjective(player, 58903, 281340);
            player->RewardQuest(sObjectMgr->GetQuestTemplate(58903), LootItemType::Item, 0, player, true);
        }

        return true;
    }

};

void AddSC_Warboard_Scripts()
{
    RegisterSpellScript(spell_warboard_alliance_261654);
    RegisterSpellScript(spell_warboard_horde_261655);
    RegisterGameObjectAI(go_warboard);
}
