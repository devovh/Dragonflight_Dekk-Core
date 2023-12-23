/*
 * Copyright 2023 Fluxurion
 */

#include "Containers.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Player.h"

enum FluxurionSpellGeneric
{
    // Spells
    SPELL_DRACTHYR_LOGIN = 369728, // teleports to random room, plays scene for the room, binds the home position
    SPELL_STASIS_1 = 369735, // triggers 366620
    SPELL_STASIS_2 = 366620, // triggers 366636
    SPELL_STASIS_3 = 366636, // removes 365560, sends first quest (64864)
    SPELL_STASIS_4 = 365560, // freeze the target
    SPELL_DRACTHYR_MOVIE_ROOM_01 = 394245, // scene for room 1
    SPELL_DRACTHYR_MOVIE_ROOM_02 = 394279, // scene for room 2
    SPELL_DRACTHYR_MOVIE_ROOM_03 = 394281, // scene for room 3
    SPELL_DRACTHYR_MOVIE_ROOM_04 = 394282, // scene for room 4
    //SPELL_DRACTHYR_MOVIE_ROOM_05	= 394283, // scene for room 5 (only plays sound)
};

// ID - 323977 In Between Tunnel/Weather
class fluxurion_spell_generic_inbetweentunnelweather : public SpellScriptLoader
{
public:
    fluxurion_spell_generic_inbetweentunnelweather() : SpellScriptLoader("fluxurion_spell_generic_inbetweentunnelweather") { }

    class fluxurion_spell_generic_inbetweentunnelweather_AuraScript : public AuraScript
    {
        PrepareAuraScript(fluxurion_spell_generic_inbetweentunnelweather_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            TC_LOG_INFO("server.Fluxurion", "Debug 2");
            if (!GetCaster()->HasAura(332323))
                GetCaster()->RemoveAura(332323); // In Between LightParam (Oribos to Bastion)
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(fluxurion_spell_generic_inbetweentunnelweather_AuraScript::OnRemove, EFFECT_0, SPELL_EFFECT_APPLY_AURA, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new fluxurion_spell_generic_inbetweentunnelweather_AuraScript();
    }
};

// 348162 - Wandering Ancient (Mount)
class spell_wandering_ancient : public SpellScriptLoader
{
public:
    spell_wandering_ancient() : SpellScriptLoader("spell_wandering_ancient") { }

    class spell_wandering_ancient_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_wandering_ancient_AuraScript);

        void AfterAuraApplied(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            uint32 WanderingAncient_DisplayIDs[4] = { 100463, 100464, 100465, 100466 };
            Trinity::Containers::RandomShuffle(WanderingAncient_DisplayIDs);

            GetTarget()->Mount(WanderingAncient_DisplayIDs[0]);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_wandering_ancient_AuraScript::AfterAuraApplied, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_wandering_ancient_AuraScript();
    }
};

/*######
## 366636 - Stasis (SPELL_STASIS_3)
######*/
class spell_stasis : public SpellScript
{
    PrepareSpellScript(spell_stasis);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_STASIS_3 });
    }

    void HandleQuestStart(SpellEffIndex effIndex)
    {
        Player* player = GetHitPlayer();
        if (!player)
            return;

        PreventHitDefaultEffect(effIndex);

        if (Quest const* quest = sObjectMgr->GetQuestTemplate(GetEffectInfo().MiscValue))
        {
            if (!player->CanTakeQuest(quest, false))
                return;

            if (player->CanAddQuest(quest, false))
            {
                player->AddQuestAndCheckCompletion(quest, player);
                player->PlayerTalkClass->SendQuestGiverQuestDetails(quest, player->GetGUID(), false, true);
            }
            else
                player->PlayerTalkClass->SendQuestGiverQuestDetails(quest, player->GetGUID(), true, false);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_stasis::HandleQuestStart, EFFECT_1, SPELL_EFFECT_QUEST_START);
    }
};

void AddSC_Fluxurion_SpellScripts_Generic()
{
    RegisterSpellScript(fluxurion_spell_generic_inbetweentunnelweather);
    new spell_wandering_ancient();
    RegisterSpellScript(spell_stasis);
}
