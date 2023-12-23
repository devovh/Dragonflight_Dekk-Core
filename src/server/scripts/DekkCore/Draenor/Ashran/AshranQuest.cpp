////////////////////////////////////////////////////////////////////////////////
//
//  DEKK CORE 
//  Copyright 2022
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

//#include "AshranMgr.hpp"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraDefines.h"
#include "SpellAuraEffects.h"
#include <OutdoorPvP/OutdoorPvPMgr.h>
#include <DekkCore/Draenor/Ashran/AshranDatas.h>

enum eQuest
{
    WelcomeToAshran = 36119
};

//Commander Anne Dunworthy - 84173
struct  npc_ashran_commander_anne_dunworthy : public ScriptedAI
{
    npc_ashran_commander_anne_dunworthy(Creature* creature) : ScriptedAI(creature) { }
   
    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(WelcomeToAshran) == QUEST_STATUS_INCOMPLETE)
            player->CompleteQuest(WelcomeToAshran);

        return false;
    }
};

// General Ushet Wolfbarger - 84473
struct npc_ashran_general_ushet_wolfbarger : public ScriptedAI
{
    npc_ashran_general_ushet_wolfbarger(Creature* creature) : ScriptedAI(creature) { }

    enum eDatas
    {
        WelcomeToAshran = 36196
    };

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(WelcomeToAshran) == QUEST_STATUS_INCOMPLETE)
            player->CompleteQuest(WelcomeToAshran);

        return false;
    }
};

// Farseer Kylanda <Stormshield Shaman Leader> - 82901
struct npc_ashran_farseer_kylanda : public ScriptedAI
{
    npc_ashran_farseer_kylanda(Creature* creature) : ScriptedAI(creature) { }

    enum eDatas
    {
        ReportingForDuty = 36130,
        FarseerKylanda = 84256
    };

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(ReportingForDuty) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(FarseerKylanda);

        return false;
    }
};

// Chris Clarkie <SI:7 Operative> - 82909
struct npc_ashran_chris_clarkie : public ScriptedAI
{
    npc_ashran_chris_clarkie(Creature* creature) : ScriptedAI(creature) { }

    enum eDatas
    {
        ReportingForDuty = 36130,
        ChrisClarkie = 84257
    };

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(ReportingForDuty) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(ChrisClarkie);

        return false;
    }
};

///// Belloc Brightblade <The Reliquary> - 88448
//class npc_ashran_belloc_brightblade : public CreatureScript
//{
//public:
//    npc_ashran_belloc_brightblade() : CreatureScript("npc_ashran_belloc_brightblade") { }
//
//    enum eDatas
//    {
//        ReportingForDuty = 36197,
//        BellocBrightblade = 84474
//    };
//
//    bool OnGossipHello(Player* player, Creature* /*creature*/) override
//    {
//        if (player->GetQuestStatus(ReportingForDuty) == QUEST_STATUS_INCOMPLETE)
//            player->KilledMonsterCredit(BellocBrightblade);
//
//        return false;
//    }
//
//    struct npc_ashran_belloc_brightbladeAI : public ScriptedAI
//    {
//        npc_ashran_belloc_brightbladeAI(Creature* creature) : ScriptedAI(creature) { }
//
//        void sGossipSelect(Player* player, uint32 /*p_Sender*/, uint32 p_Action) override
//        {
//            /// "Take all of my Artifact Fragments" is always 0
//            if (p_Action)
//                return;
//
//            ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(me->GetZoneId());
//            if (l_ZoneScript == nullptr)
//                return;
//
//            uint32 artifactCount = player->GetCurrency(CURRENCY_TYPE_ARTIFACT_FRAGMENT);
//            player->ModifyCurrency(CURRENCY_TYPE_ARTIFACT_FRAGMENT, -int32(artifactCount * CURRENCY_PRECISION), false);
//
//            uint32 l_Modulo = artifactCount % MaxArtifactCounts;
//            uint32 l_Count = artifactCount / MaxArtifactCounts;
//            uint8 l_Rand = urand(0, MaxArtifactCounts - 1);
//
//            if (OutdoorPvPAshran* l_Ashran = static_cast<OutdoorPvPAshran*>(l_ZoneScript))
//            {
//                for (uint8 l_I = 0; l_I < MaxArtifactCounts; ++l_I)
//                {
//                    if (l_I == l_Rand)
//                        l_Ashran->AddCollectedArtifacts(TEAM_HORDE, l_I, l_Count + l_Modulo);
//                    else
//                        l_Ashran->AddCollectedArtifacts(TEAM_HORDE, l_I, l_Count);
//                }
//
//                l_Ashran->RewardHonorAndReputation(artifactCount, player);
//            }
//        }
//    };
//
//    CreatureAI* GetAI(Creature* creature) const override
//    {
//        return new npc_ashran_belloc_brightbladeAI(creature);
//    }
//};
//
///// Angry Zurge - 83869
//class npc_ashran_angry_zurge : public CreatureScript
//{
//public:
//    npc_ashran_angry_zurge() : CreatureScript("npc_ashran_angry_zurge") { }
//
//    enum eDatas
//    {
//        ReportingForDuty = 36197,
//        AngryZurge = 84659
//    };
//
//    bool OnGossipHello(Player* player, Creature* /*creature*/) override
//    {
//        if (player->GetQuestStatus(ReportingForDuty) == QUEST_STATUS_INCOMPLETE)
//            player->KilledMonsterCredit(AngryZurge);
//
//        return false;
//    }
//};
//
///// Mare Wildrunner <Warspear Farseer> - 84660
//class npc_ashran_mare_wildrunner : public CreatureScript
//{
//public:
//    npc_ashran_mare_wildrunner() : CreatureScript("npc_ashran_mare_wildrunner") { }
//
//    enum eDatas
//    {
//        ReportingForDuty = 36197,
//        MareWildrunner = 84661
//    };
//
//    bool OnGossipHello(Player* player, Creature* /*creature*/) override
//    {
//        if (player->GetQuestStatus(ReportingForDuty) == QUEST_STATUS_INCOMPLETE)
//            player->KilledMonsterCredit(MareWildrunner);
//
//        return false;
//    }
//};

/// Blade Twister - 178795
class spell_ashran_blade_twister : public SpellScriptLoader
{
public:
    spell_ashran_blade_twister() : SpellScriptLoader("spell_ashran_blade_twister") { }

    class spell_ashran_blade_twister_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ashran_blade_twister_AuraScript);

        enum eSpells
        {
            BladeTwisterDamage = 177167
        };

        void OnTick(AuraEffect const* p_AurEff)
        {
            if (Unit* l_Target = GetTarget())
            {
                std::list<Creature*> l_Blades;
                l_Target->GetCreatureListWithEntryInGrid(l_Blades, BladeTwisterTrigger, 50.0f);

                if (l_Blades.empty())
                {
                    //p_AurEff->GetBase()->Remove();
                    return;
                }

                l_Blades.remove_if([this, l_Target](Creature* creature) -> bool
                    {
                        if (!creature->GetOwner())
                            return true;

                        if (creature->GetOwner() != l_Target)
                            return true;

                        return false;
                    });

                for (Creature* l_Creature : l_Blades)
                    l_Target->CastSpell(l_Creature, BladeTwisterDamage, true);
            }
        }

        void Register() override
        {
            OnEffectPeriodic += static_cast<EffectPeriodicHandler>(AuraEffectPeriodicFn(spell_ashran_blade_twister_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY));
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ashran_blade_twister_AuraScript();
    }
};

/// Living Bomb - 176670
class spell_ashran_emberfall_living_bomb : public SpellScriptLoader
{
public:
    spell_ashran_emberfall_living_bomb() : SpellScriptLoader("spell_ashran_emberfall_living_bomb") { }

    class spell_ashran_emberfall_living_bomb_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ashran_emberfall_living_bomb_AuraScript);

        enum eSpells
        {
            LivingBombTriggered = 176673
        };

        void AfterRemove(AuraEffect const*, AuraEffectHandleModes)
        {
            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode != AURA_REMOVE_BY_DEATH && removeMode != AURA_REMOVE_BY_EXPIRE)
                return;

            if (Unit* l_Caster = GetCaster())
            {
                if (Unit* l_Target = GetTarget())
                    l_Caster->CastSpell(l_Target, LivingBombTriggered, true);
            }
        }

        void Register() override
        {
            AfterEffectRemove += static_cast<EffectApplyHandler>(AuraEffectRemoveFn(spell_ashran_emberfall_living_bomb_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL));
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ashran_emberfall_living_bomb_AuraScript();
    }
};

/// Alliance Reward - 178531
/// Horde Reward - 178533
class spell_ashran_faction_rewards : public SpellScriptLoader
{
public:
    spell_ashran_faction_rewards() : SpellScriptLoader("spell_ashran_faction_rewards") { }

    class spell_ashran_faction_rewards_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ashran_faction_rewards_SpellScript);

        enum Items
        {
            StrongboxHorde = 120151,
            StrongboxAlliance = 118065
        };

        void CorrectTargets(std::list<WorldObject*>& p_Targets)
        {
            if (p_Targets.empty())
                return;

            p_Targets.remove_if([this](WorldObject* object) -> bool
                {
                    if (object == nullptr || object->GetTypeId() != TYPEID_PLAYER)
                        return true;

                    Player* l_Player = object->ToPlayer();
                    if (l_Player == nullptr)
                        return true;

                    /// Only one strongbox per day
                    //if (!l_Player->CanHaveDailyLootForItem(Items::StrongboxAlliance) || !l_Player->CanHaveDailyLootForItem(Items::StrongboxHorde))
                    //    return true;

                    return false;
                });
        }

        void HandleOnHit()
        {
            if (GetHitUnit() == nullptr)
                return;

            //if (Player* l_Player = GetHitUnit()->ToPlayer())
            //{
            //    if (GetSpellInfo()->Id == eAshranSpells::SpellAllianceReward)
            //        l_Player->AddDailyLootCooldown(Items::StrongboxHorde);
            //    else
            //        l_Player->AddDailyLootCooldown(Items::StrongboxAlliance);
            //}
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += static_cast<ObjectAreaTargetSelectHandler>(SpellObjectAreaTargetSelectFn(spell_ashran_faction_rewards_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY));
            OnHit += SpellHitFn(spell_ashran_faction_rewards_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_ashran_faction_rewards_SpellScript();
    }
};

/// Booming Shout - 177150
class spell_ashran_booming_shout : public SpellScriptLoader
{
public:
    spell_ashran_booming_shout() : SpellScriptLoader("spell_ashran_booming_shout") { }

    class spell_ashran_booming_shout_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ashran_booming_shout_SpellScript);

        void CorrectTargets(std::list<WorldObject*>& p_Targets)
        {
            if (p_Targets.empty())
                return;

            p_Targets.remove_if([this](WorldObject* object) -> bool
                {
                    if (object == nullptr)
                        return true;

                    if (object->GetEntry() == GaulDunFirok || object->GetEntry() == MukmarRaz || object->GetEntry() == KorlokTheOgreKing)
                        return true;

                    return false;
                });
        }

        void HandleDamage(SpellEffIndex p_EffIndex)
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    float l_MaxRadius = GetSpellInfo()->GetEffect(EFFECT_0).CalcRadius(GetCaster());
                    float l_Dist = l_Caster->GetDistance(l_Target);
                    float l_Pct = l_Dist / l_MaxRadius;
                    SetHitDamage(int32(GetHitDamage() * l_Pct));
                }
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += static_cast<ObjectAreaTargetSelectHandler>(SpellObjectAreaTargetSelectFn(spell_ashran_booming_shout_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY));
            OnObjectAreaTargetSelect += static_cast<ObjectAreaTargetSelectHandler>(SpellObjectAreaTargetSelectFn(spell_ashran_booming_shout_SpellScript::CorrectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY));
            OnEffectHitTarget += static_cast<EffectHandler>(SpellEffectFn(spell_ashran_booming_shout_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE));
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_ashran_booming_shout_SpellScript();
    }
};

/// Horde Racer - 166819
/// Alliance Racer - 166784
class spell_ashran_horde_and_alliance_racer : public SpellScriptLoader
{
public:
    spell_ashran_horde_and_alliance_racer() : SpellScriptLoader("spell_ashran_horde_and_alliance_racer") { }

    class spell_ashran_horde_and_alliance_racer_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ashran_horde_and_alliance_racer_AuraScript);

            enum eSpells
        {
            HordeRider = 166819,
            AllianceRider = 166784
        };

        enum eDatas
        {
            SpeedPCTPerPlayer = 30,
            MaxSpeedPCT = 500
        };

        uint32 m_CheckTimer = 0;

        bool Load() override
        {
            m_CheckTimer = 200;
            return true;
        }

        void OnUpdate(uint32 p_Diff)
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (m_CheckTimer)
                {
                    if (m_CheckTimer <= p_Diff)
                    {
                        m_CheckTimer = 200;

                        std::list<Player*> l_PlayerList;
                        l_Caster->GetPlayerListInGrid(l_PlayerList, 10.0f);

                        if (l_PlayerList.empty())
                        {
                            if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                                l_AurEff->ChangeAmount(0);

                            return;
                        }

                        l_PlayerList.remove_if([this](Player* player) -> bool
                            {
                                if (player == nullptr)
                                    return true;

                                if (GetSpellInfo()->Id == AllianceRider && player->GetTeamId() != TEAM_ALLIANCE)
                                    return true;

                                if (GetSpellInfo()->Id == HordeRider && player->GetTeamId() != TEAM_HORDE)
                                    return true;

                                return false;
                            });

                        if (l_PlayerList.empty())
                        {
                            if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                                l_AurEff->ChangeAmount(0);

                            return;
                        }

                        /// The riders move very slowly, but for each player of their faction within their vicinity their speed is increased by 30%,
                        /// allowing them to outstrip their competitor.
                        if (AuraEffect* l_AurEff = l_Caster->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                            l_AurEff->ChangeAmount(std::min(static_cast<int32>(l_PlayerList.size()) * SpeedPCTPerPlayer, static_cast<int32>(MaxSpeedPCT)));
                    }
                    else
                        m_CheckTimer -= p_Diff;
                }
            }
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_ashran_horde_and_alliance_racer_AuraScript::OnUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ashran_horde_and_alliance_racer_AuraScript();
    }
};

void AddSC_AshranQuest()
{
    RegisterCreatureAI(npc_ashran_commander_anne_dunworthy);
    RegisterCreatureAI(npc_ashran_general_ushet_wolfbarger);
    RegisterCreatureAI(npc_ashran_farseer_kylanda);
   /* new npc_ashran_chris_clarkie();
    new npc_ashran_harrison_jones();
    new npc_ashran_belloc_brightblade();
    new npc_ashran_angry_zurge();
    new npc_ashran_mare_wildrunner();*/
    new spell_ashran_blade_twister();
    new spell_ashran_emberfall_living_bomb();
    new spell_ashran_faction_rewards();
    new spell_ashran_booming_shout();
    new spell_ashran_horde_and_alliance_racer();
}
