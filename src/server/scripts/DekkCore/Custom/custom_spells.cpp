/*
* Copyright 2021 DekkCore
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

#include "AreaTriggerAI.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "Garrison.h"
#include "SpellScript.h"
#include "ScriptMgr.h"
#include "ChatPackets.h"
#include "MiscPackets.h"
#include "GameObjectAI.h"
#include "CreatureTextMgr.h"
#include "DBCEnums.h"
#include <Grids/Notifiers/GridNotifiers.h>
#include "Challenge.h"

//FluxurionsTestSpell = 263427 // i use this to test spell mechanics

enum Cool_SpellVisuals
{
    Deaths_Due = 104151, // like death and decay
    Elysian_Decree = 102131,
    Convoke_the_Spirits = 96283,
    FleshcraftChannel = 95876,
    FleshcraftOrb = 96659,
    Radiant_Spark = 90436,
    Faeline_Stomp = 97433,
    Decimating_Bolt_Casting = 97003,
    Decimating_Bolt_Shoot = 97416,
    Decimating_Bolt_Explosion = 97417,
    FaeTransfusionChannel = 98147,
    ChainHarvestChannel = 95227,
    FaeTransfusionBlueMagicSmoke = 99373,
    FaeTransfusionBlueTerrainEffect = 98147,
};

enum Cool_Spells
{
    Death_Grasp_NecroticStyle = 323831,
    SPELL_CRUSHING_ASSAULT_AMOUNT = 278751,
};


class spell_fluxurion_test_spell : public SpellScriptLoader
{
public:
    spell_fluxurion_test_spell() : SpellScriptLoader("spell_fluxurion_test_spell") {}

    class spell_fluxurion_test_spell_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_fluxurion_test_spell_SpellScript);

        void HandleOnCast()
        {
            if (Unit* caster = GetCaster())
            {
                caster->SendPlaySpellVisual(caster->GetPosition(), Cool_SpellVisuals::Decimating_Bolt_Casting, SPELL_MISS_NONE, SPELL_MISS_NONE, 20);
                caster->AddDelayedEvent(10, [caster]()        // time_delay
                    {
                        caster->SendPlaySpellVisual(caster->GetPosition(), Cool_SpellVisuals::Convoke_the_Spirits, SPELL_MISS_NONE, SPELL_MISS_NONE, 20);
                    });
                caster->AddDelayedEvent(500, [caster]()        // time_delay
                    {
                        caster->SendPlaySpellVisual(caster->GetPosition(), Cool_SpellVisuals::FleshcraftChannel, SPELL_MISS_NONE, SPELL_MISS_NONE, 20);
                    });
                caster->AddDelayedEvent(1000, [caster]()        // time_delay
                    {
                        caster->SendPlaySpellVisual(caster->GetPosition(), Cool_SpellVisuals::Radiant_Spark, SPELL_MISS_NONE, SPELL_MISS_NONE, 20);
                    });
            }
        }

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            Unit* target = GetHitUnit();
            Unit* caster = GetCaster();
            if (!target || !caster)
                return;

            target->CastSpell(target, Death_Grasp_NecroticStyle);
            target->AddDelayedEvent(2000, [target, caster]()        // time_delay
                {
                    target->SendPlaySpellVisual(target->GetPosition(), Cool_SpellVisuals::Deaths_Due, SPELL_MISS_NONE, SPELL_MISS_NONE, 50);
                });
            target->AddDelayedEvent(4000, [target, caster]()        // time_delay
                {
                    target->SendPlaySpellVisual(target->GetPosition(), Cool_SpellVisuals::Elysian_Decree, SPELL_MISS_NONE, SPELL_MISS_NONE, 50);
                });
            target->AddDelayedEvent(6000, [target, caster]()        // time_delay
                {
                    target->SendPlaySpellVisual(target->GetPosition(), Cool_SpellVisuals::Faeline_Stomp, SPELL_MISS_NONE, SPELL_MISS_NONE, 50);
                });
            target->AddDelayedEvent(8000, [target, caster]()        // time_delay
                {
                    target->SendPlaySpellVisual(target->GetPosition(), Cool_SpellVisuals::Decimating_Bolt_Explosion, SPELL_MISS_NONE, SPELL_MISS_NONE, 20);
                    caster->Kill(caster, target);
                });
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_fluxurion_test_spell_SpellScript::HandleOnCast);
            OnEffectHitTarget += SpellEffectFn(spell_fluxurion_test_spell_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }

    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_fluxurion_test_spell_SpellScript();
    }
};

//162714
class spell_q34824 : public SpellScriptLoader
{
public:
    spell_q34824() : SpellScriptLoader("spell_q34824") { }

    class spell_q34824_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q34824_SpellScript);


        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit* caster = GetCaster())
            {
                std::list<Player*> playerList;
                caster->GetPlayerListInGrid(playerList, 10.0f);
                for (auto player : playerList)
                    player->KilledMonsterCredit(caster->GetEntry(), ObjectGuid::Empty);

                if (Creature* c = caster->ToCreature())
                    sCreatureTextMgr->SendChat(c, 0);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_q34824_SpellScript::HandleScriptEffect, 2, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_q34824_SpellScript();
    }
};

//161033 for the gord cast 160767 too
//! HORDE Q: 34378, ALLIANCE Q: 34586
class spell_161033 : public SpellScriptLoader
{
public:
    spell_161033() : SpellScriptLoader("spell_161033") { }

    class spell_161033_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_161033_SpellScript);

#define __spell 160767
        void Handle(SpellEffIndex effIndex)
        {
            Unit* caster = GetCaster();
            if (!caster || !caster->ToPlayer())
                return;

            if (caster->ToPlayer()->GetTeam() == ALLIANCE)
                return;

            caster->CastSpell(caster, __spell, true);
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(spell_161033_SpellScript::Handle, 0, SPELL_EFFECT_KILL_CREDIT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_161033_SpellScript();
    }
};

//- 155071
/*
UPDATE `npc_spellclick_spells` set cast_flags = 1 WHERE`spell_id` = 155071; -- need plr caster to creature
*/

class spell_interract : public SpellScriptLoader
{
public:
    spell_interract() : SpellScriptLoader("spell_interract") { }

    class spell_interract_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_interract_SpellScript);

        enum data
        {
            NPC__ = 79446,

            SPELL_CREDIT_Q34646 = 177492,

        };

        void HandleEffect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit* caster = GetCaster())
            {
                Player* plr = caster->ToPlayer();
                if (!plr)
                    return;

                Unit* target = caster->GetVictim();
                if (!target)
                    return;

                volatile uint32 entry = target->GetEntry();
                switch (target->GetEntry())
                {
                case NPC__:
                    //cast 164640
                    //cast 164649
                    plr->TalkedToCreature(target->GetEntry(), target->GetGUID());
                    caster->CastSpell(caster, SPELL_CREDIT_Q34646, true);
                    break;
                default:
                    break;
                }

            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_interract_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_interract_SpellScript();
    }
};

// Lumber mill
// Summon Lumberjack Faction Picker
//! Spell: 168641 -> 167962 -> H: 167961 A: 167902 (NPC: H:83985, A: 83950)
//! Q: A: 36189, H: 36137
//! Go: 234021
class spell_garr_lumberjack : public SpellScriptLoader
{
public:
    spell_garr_lumberjack() : SpellScriptLoader("spell_garr_lumberjack") { }

    class spell_garr_lumberjack_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_garr_lumberjack_SpellScript);

        void HandleEffect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit* caster = GetCaster())
            {
                Player* plr = caster->ToPlayer();
                if (!plr)
                    return;

                if (plr->GetTeam() == ALLIANCE)
                    plr->CastSpell(plr, 167902, false);
                else
                    plr->CastSpell(plr, 167961, false);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_garr_lumberjack_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_garr_lumberjack_SpellScript();
    }
};

// 83985 83950
class mob_garr_lumberjack : public CreatureScript
{
public:
    mob_garr_lumberjack() : CreatureScript("mob_garr_lumberjack") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_garr_lumberjackAI(creature);
    }

    struct mob_garr_lumberjackAI : public ScriptedAI
    {

        mob_garr_lumberjackAI(Creature* creature) : ScriptedAI(creature), isQuest(false)
        {
        }

        enum data
        {
            SOUNDID = 7514,
            GO_TREE_H = 233922,
            GO_TREE_A = 234021,
            SPELL_PEON_AXE = 167957,
            SPELL_CARRY_LUMBER = 167958,//Carry Lumber
            SPELL_CREATE_TIMBER = 168647,

            SPELL_STUMP = 170079,

            SPELL_TIMBER_5 = 168523,
        };

        ObjectGuid treeGUID;
        ObjectGuid plrGUID;
        EventMap _events;
        bool isQuest;

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                if (GameObject* tree = me->GetMap()->GetGameObject(treeGUID))

                    //me->SetFacingToObject(tree);

                me->CastSpell(me, SPELL_PEON_AXE, true);
                me->HandleEmoteCommand(EMOTE_STATE_WORK_CHOPWOOD_2);
              //  me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, EMOTE_STATE_WORK_CHOPWOOD_2);
              //  _events.ScheduleEvent(EVENT_1, 5000);
            }
            else
            {
                sCreatureTextMgr->SendChat(me, 0);
                if (Player* player = ObjectAccessor::GetPlayer(*me, plrGUID))
                {
                    if (!isQuest)
                    {
                        float count = urand(5, 7);
                       // WorldPackets::Chat::WorldText packet;
                     //   packet.Guid = ObjectGuid::Empty;
                     //   packet.Arg1 = count;
                      //  packet.Arg2 = 0;
                     //   packet.Text = sObjectMgr->GetTrinityStringForDBCLocale(LANG_LUMBER_RESULT);
                      //  player->SendDirectMessage(packet.Write());
                       // me->CastCustomSpell(player, SPELL_TIMBER_5, &count, NULL, NULL, false);
                    }
                    else
                        me->CastSpell(player, SPELL_CREATE_TIMBER, true);
                }
                me->DespawnOrUnsummon(1s);
            }
        }

        void IsSummonedBy(WorldObject* summoner) override
        {
            Player* player = summoner->ToPlayer();
            if (!player)
            {
                me->Yell("SCRIPT::mob_garr_lumberjack summoner is not player", LANG_UNIVERSAL, NULL);
                return;
            }
            plrGUID = summoner->GetGUID();
            me->PlayDirectSound(SOUNDID, player);

            QuestStatus status = player->GetQuestStatus(player->GetTeam() == ALLIANCE ? 36189 : 36137);
            isQuest = !(status == QUEST_STATUS_REWARDED);

            std::list<GameObject*> gameobjectList;
          //  CellCoord pair(Trinity::ComputeCellCoord(summoner->GetPositionX(), summoner->GetPositionY()));
          //  Cell cell(pair);
           // cell.SetNoCreate();

            Trinity::GameObjectInRangeCheck check(summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ(), 10.0f);
            Trinity::GameObjectListSearcher<Trinity::GameObjectInRangeCheck> searcher(summoner, gameobjectList, check);

          /*  cell.Visit(pair, Trinity::makeGridVisitor(searcher), *(summoner->GetMap()), *summoner, 10.0f);
            for (auto tree : gameobjectList)
            {
                if (tree->GetUInt32Value(GAMEOBJECT_FIELD_FLAGS) & GO_FLAG_LUMBER)
                {
                    Position pos;
                    tree->GetRandomNearPosition(pos, 5.0f);
                    me->GetMotionMaster()->MovePoint(1, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
                    treeGUID = tree->GetGUID();

                    break;
                }
            }*/
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, plrGUID))
                {
                    switch (eventId)
                    {
                    case 1:
                        if (GameObject* tree = me->GetMap()->GetGameObject(treeGUID))
                        {
                            me->CastSpell(me, SPELL_STUMP, true);
                         //   tree->setVisibilityCDForPlayer(plrGUID, 300);
                            tree->DestroyForPlayer(player);
                        }
                        me->CastSpell(me, SPELL_CARRY_LUMBER, true);
                        _events.ScheduleEvent(2, 5s);
                        me->GetMotionMaster()->MovePoint(2, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
                        break;
                    case 2:
                        sCreatureTextMgr->SendChat(me, 0);
                        me->CastSpell(player, SPELL_CREATE_TIMBER, true);
                        break;
                    }
                }
            }
        }
    };
};

//! A: 36194 H: 36142
// Go: 234000
// Spell: 167970 Summon Lumberjack Faction Picker
class spell_garr_lumberjack_lvl2 : public SpellScriptLoader
{
public:
    spell_garr_lumberjack_lvl2() : SpellScriptLoader("spell_garr_lumberjack_lvl2") { }

    class spell_garr_lumberjack_lvl2_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_garr_lumberjack_lvl2_SpellScript);

        void HandleEffect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit* caster = GetCaster())
            {
                Player* plr = caster->ToPlayer();
                if (!plr)
                    return;

                //AiID: 6784
                if (plr->GetTeam() == ALLIANCE)
                    plr->CastSpell(plr, 167969, false);
                else
                    plr->CastSpell(plr, 168043, false);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_garr_lumberjack_lvl2_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_garr_lumberjack_lvl2_SpellScript();
    }
};

// 84003 84004
// 170069->167972(animKIt)->167974->168028->167958
class mob_garr_lumberjack2 : public CreatureScript
{
public:
    mob_garr_lumberjack2() : CreatureScript("mob_garr_lumberjack2") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_garr_lumberjack2AI(creature);
    }

    struct mob_garr_lumberjack2AI : public ScriptedAI
    {

        mob_garr_lumberjack2AI(Creature* creature) : ScriptedAI(creature), isQuest(false)
        {
            me->UpdatePosition(creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ() + 6.0f, true);
        }

        enum data
        {
            SPELL_PEON_AXE = 167957,
            SPELL_CARRY_LUMBER = 167958, //Carry Lumber
            SPELL_CREATE_TIMBER2 = 168942,

            SPELL_STUMP = 170079,

            SPELL_TIMBER_10 = 168524,
        };

        ObjectGuid treeGUID;
        ObjectGuid plrGUID;
        EventMap _events;
        bool isQuest;

        void IsSummonedBy(WorldObject* summoner) override
        {
            Player* player = summoner->ToPlayer();
            if (!player)
            {
                me->Yell("SCRIPT::mob_garr_lumberjack summoner is not player", LANG_UNIVERSAL, NULL);
                return;
            }
            plrGUID = summoner->GetGUID();

            QuestStatus status = player->GetQuestStatus(player->GetTeam() == ALLIANCE ? 36194 : 36142);
            isQuest = !(status == QUEST_STATUS_REWARDED);
            me->UpdatePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 6.0f, true);
            me->CastSpell(me, 170069, true);

            //167972
            me->SetAIAnimKitId(6784);
            me->SetDisableGravity(true, false);
            me->SetPlayHoverAnim(true);
            me->CastSpell(me, 167972, true);

            _events.ScheduleEvent(1, 1s);

            std::list<GameObject*> gameobjectList;
          //  CellCoord pair(Trinity::ComputeCellCoord(summoner->GetPositionX(), summoner->GetPositionY()));
         //   Cell cell(pair);
         //   cell.SetNoCreate();

            Trinity::GameObjectInRangeCheck check(summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ(), 10.0f);
            Trinity::GameObjectListSearcher<Trinity::GameObjectInRangeCheck> searcher(summoner, gameobjectList, check);

            /*cell.Visit(pair, Trinity::makeGridVisitor(searcher), *(summoner->GetMap()), *summoner, 10.0f);
            for (auto tree : gameobjectList)
            {
                if (tree->GetUInt32Value(GAMEOBJECT_FIELD_FLAGS) & GO_FLAG_LUMBER)
                {
                    Position pos;
                    tree->GetNearPoint(tree, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 1.0f, 1.0f, 360.0f);
                    treeGUID = tree->GetGUID();
                    me->UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() + 6.0f, true);
                    me->SetFacingToObject(tree);
                    break;
                }
            }*/
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                me->CastSpell(me, 167974, true);
                me->RemoveAurasDueToSpell(167972);
              //  me->SetAnimKitId(0);
                me->SetDisableGravity(false);
                me->SetPlayHoverAnim(false);
                _events.ScheduleEvent(2, 4s);
                me->GetMotionMaster()->MoveFall();
                if (GameObject* tree = me->GetMap()->GetGameObject(treeGUID))
                    me->SetFacingToObject(tree);
            }
            else if (id == 2)
            {
                sCreatureTextMgr->SendChat(me, 0);
                if (Player* player = ObjectAccessor::GetPlayer(*me, plrGUID))
                {
                    if (!isQuest)
                    {
                        float count = urand(10, 15);
                       /* WorldPackets::Chat::WorldText packet;
                        packet.Guid = ObjectGuid::Empty;
                        packet.Arg1 = count;
                        packet.Arg2 = 0;
                        packet.Text = sObjectMgr->GetTrinityStringForDBCLocale(LANG_LUMBER_RESULT);
                        player->SendDirectMessage(packet.Write());
                        me->CastCustomSpell(player, SPELL_TIMBER_10, &count, NULL, NULL, false);*/
                    }
                    else
                        me->CastSpell(player, SPELL_CREATE_TIMBER2, true);
                }
                me->DespawnOrUnsummon(1s);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, plrGUID))
                {
                    switch (eventId)
                    {
                    case 1:
                        me->GetMotionMaster()->MoveTakeoff(me->GetPositionX(), me->GetPositionY()- 6.0f);
                        break;
                    case 2:
                        me->CastSpell(me, 168028, true);
                        sCreatureTextMgr->SendChat(me, 0);
                        _events.ScheduleEvent(3, 1s);
                        break;
                    case 3:
                        me->CastSpell(me, 159560, true);
                        _events.ScheduleEvent(4, 2s);

                        /*if (GameObject * tree = me->GetMap()->GetGameObject(treeGUID))
                        {
                            Position pos;
                            tree->GetRandomNearPosition(pos, 5.0f);
                            me->GetMotionMaster()->MovePoint(0, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
                        }*/
                        break;
                    case 4:
                        me->RemoveAurasDueToSpell(167974);
                       // me->SetAnimKitId(6778);
                        me->HandleEmoteCommand(EMOTE_STATE_WORK_CHOPWOOD_2);
                       // me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, EMOTE_STATE_WORK_CHOPWOOD_2);
                        _events.ScheduleEvent(5, 5s);
                        break;
                    case 5:
                      //  me->SetAnimKitId(0);
                        me->RemoveAurasDueToSpell(168028);
                        if (GameObject* tree = me->GetMap()->GetGameObject(treeGUID))
                        {
                            me->CastSpell(me, SPELL_STUMP, true);
                            me->CastSpell(me, 167958, true);
                          //  tree->setVisibilityCDForPlayer(plrGUID, 300);
                            tree->DestroyForPlayer(player);
                        }
                        me->GetMotionMaster()->MovePoint(2, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
                        break;
                    }
                }
            }
        }
    };
};

// Q: A : 37088 H : 37062
// spell: 174569
class spell_q37088_q37062 : public SpellScriptLoader
{
public:
    spell_q37088_q37062() : SpellScriptLoader("spell_q37088_q37062") { }

    class spell_q37088_q37062_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q37088_q37062_SpellScript);

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            Unit* caster = GetCaster();
            if (!caster)
                return;

            Player* player = caster->ToPlayer();
            if (!player)
                return;

            player->KilledMonsterCredit(87254, ObjectGuid::Empty);

            if (Unit* target = GetExplTargetUnit())
                sCreatureTextMgr->SendChat(target->ToCreature(), 0);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_q37088_q37062_SpellScript::HandleScriptEffect, 0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_q37088_q37062_SpellScript();
    }
};

//m+
//206150/challengers_might
class spell_challengers_might : public SpellScriptLoader
{
public:
    spell_challengers_might() : SpellScriptLoader("spell_challengers_might") { }

    enum eSpell
    {
        SPELL_ENRAGE = 228318,
    };

    class spell_challengers_might_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_challengers_might_AuraScript);

        uint8 volcanicTimer = 0;
        uint32 felExplosivesTimer = 0;
        uint32 necroticProcDelay = 0;

        void OnProc(AuraEffect* aurEff, ProcEventInfo& p_EventInfo)
        {
            PreventDefaultAction();
            ;
            if (!GetUnitOwner())
                return;
            //??
            if (AuraEffect* aurEff1 = aurEff->GetBase()->GetEffect(EFFECT_2))
            {
                if (aurEff1->GetBaseAmount())
                {
                    if (!GetUnitOwner()->ToCreature()->IsDungeonBoss())
                    {
                        if (GetUnitOwner()->GetHealthPct() <= 30.f)
                            GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_ENRAGE, true);
                        else
                            GetUnitOwner()->RemoveAurasDueToSpell(SPELL_ENRAGE);
                    }
                }
            }
        }

        // 3 Volcanic
        void OnTick(AuraEffect const* aurEff)
        {
            if (!aurEff->GetAmount() || !GetCaster()->IsInCombat())
                return;

            if (volcanicTimer == 7)
                volcanicTimer = 0;
            else
            {
                ++volcanicTimer;
                return;
            }

            auto caster = GetCaster()->ToCreature();
            if (!caster || caster->IsAffixDisabled() || (caster->AI() && (caster->AI() && (caster->HasAuraType(SPELL_AURA_MOD_STUN) || caster->HasAuraType(SPELL_AURA_MOD_FEAR) ||
                caster->HasAuraType(SPELL_AURA_MOD_CHARM) || caster->HasAuraType(SPELL_AURA_MOD_CONFUSE) ||
                caster->HasAuraType(SPELL_AURA_MOD_ROOT) || caster->HasAuraType(SPELL_AURA_MOD_FEAR_2)))) || !caster->IsHostileToPlayers())
                return;

            if (auto owner = caster->GetOwner())
            {
                if (owner->IsPlayer())
                    return;

                if (owner->IsCreature() && !owner->ToCreature()->IsDungeonBoss())
                    return;
            }

            auto _map = caster->GetMap();
            if (!_map)
                return;

            Map::PlayerList const& players = _map->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (auto player = itr->GetSource())
                {
                    if (player->IsInCombat() && player->IsValidAttackTarget(caster))
                        if (caster->GetDistance(player) > 15.0f && caster->GetDistance(player) < 60.0f) // Offlike 10m
                            caster->CastSpell(player, ChallengerSummonVolcanicPlume, true);
                }
            }
        }

        // 4 Necrotic
        void OnProc2(AuraEffect* /*auraEffect*/, ProcEventInfo& eventInfo)
        {
            if (necroticProcDelay)
                PreventDefaultAction();
            else
                necroticProcDelay = 1000;
        }

        // Fel Explosives 13 Explosive
        void OnUpdate(const uint32 diff)
        {
            if (necroticProcDelay)
            {
                if (necroticProcDelay <= diff)
                    necroticProcDelay = 0;
                else
                    necroticProcDelay -= diff;
            }

            if (!GetCaster()->IsInCombat())
                return;

            if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                if (!instance->HasAffix(Affixes::Explosive))
                    return;

            if (felExplosivesTimer <= diff)
                felExplosivesTimer = urandms(8, 16);
            else
            {
                felExplosivesTimer -= diff;
                return;
            }

            auto caster = GetCaster()->ToCreature();
            if (!caster || caster->IsAffixDisabled() || (caster->AI() && (caster->HasAuraType(SPELL_AURA_MOD_STUN) || caster->HasAuraType(SPELL_AURA_MOD_FEAR) ||
                caster->HasAuraType(SPELL_AURA_MOD_CHARM) || caster->HasAuraType(SPELL_AURA_MOD_CONFUSE) ||
                caster->HasAuraType(SPELL_AURA_MOD_ROOT) || caster->HasAuraType(SPELL_AURA_MOD_FEAR_2))) || !caster->IsHostileToPlayers())
                return;

            if (auto owner = caster->GetOwner())
            {
                if (owner->IsPlayer())
                    return;

                if (owner->IsCreature() && !owner->ToCreature()->IsDungeonBoss())
                    return;
            }

            auto _map = caster->GetMap();
            if (!_map)
                return;

            Map::PlayerList const& players = _map->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (auto player = itr->GetSource())
                {
                    if (player->IsInCombat() && player->IsValidAttackTarget(caster))
                        if (caster->GetDistance(player) < 60.0f)
                        {
                            caster->CastSpell(caster, SPELL_FEL_EXPLOSIVES_SUMMON_2, true);
                            return;
                        }
                }
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_challengers_might_AuraScript::OnProc, EFFECT_2, SPELL_AURA_DUMMY);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_challengers_might_AuraScript::OnTick, EFFECT_7, SPELL_AURA_PERIODIC_DUMMY);
            OnEffectProc += AuraEffectProcFn(spell_challengers_might_AuraScript::OnProc2, EFFECT_8, SPELL_AURA_DUMMY);
            OnAuraUpdate += AuraUpdateFn(spell_challengers_might_AuraScript::OnUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_challengers_might_AuraScript();
    }
};

struct npc_volcanic_plume_105877 : public ScriptedAI
{
    npc_volcanic_plume_105877(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
        me->AddUnitState(UNIT_STATE_CANNOT_TURN);
        me->SetDisableGravity(true);
        me->SetFaction(16);
        me->SetDisplayId(16925);
        me->DespawnOrUnsummon(3s);
        events.Reset();
        events.ScheduleEvent(1, 2s);
    }

    void UpdateAI(uint32 diff) override
    {
      //  events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case 1:
                me->CastSpell(me, ChallengerVolcanicPlume, false);
                break;
            default:
                break;
            }
        }
    }
};

// 120651 - Fel Explosives
struct npc_challenger_fel_explosives : ScriptedAI
{
    npc_challenger_fel_explosives(Creature* creature) : ScriptedAI(creature)
    {
        me->AddUnitTypeMask(UNIT_MASK_TOTEM);
        me->SetReactState(REACT_PASSIVE);
    }

    EventMap events;

    void Reset() override {}

    void IsSummonedBy(WorldObject* summoner) override
    {
        DoCast(me, SPELL_FEL_EXPLOSIVES_VISUAL, true);
        events.ScheduleEvent(1, 5s);
    }

    void OnSpellCasted(SpellInfo const* spellInfo) //override NEED ADD ONSPELLCASTED
    {
        if (spellInfo->Id == SPELL_FEL_EXPLOSIVES_DMG)
            me->DespawnOrUnsummon(1s);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case 1:
                DoCast(SPELL_FEL_EXPLOSIVES_DMG);
                break;
            }
        }
    }
};

// 209859 - Bolster
class spell_challengers_bolster : public SpellScriptLoader
{
public:
    spell_challengers_bolster() : SpellScriptLoader("spell_challengers_bolster") { }

    class spell_challengers_bolster_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_challengers_bolster_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.empty())
                return;

            std::list<WorldObject*> temp;
            for (auto object : targets)
            {
                if (auto unit = object->ToUnit())
                {
                    if (!unit->IsInCombat() || unit->IsPlayer())
                        continue;

                    auto owner = unit->GetOwner();
                    if (owner && owner->IsPlayer())
                        continue;

                    if (Creature* creature = unit->ToCreature())
                        if (creature->IsDungeonBoss())
                            continue;

                    temp.push_back(object);
                }
            }
            targets = temp;
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challengers_bolster_SpellScript::FilterTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_challengers_bolster_SpellScript();
    }
};

//240443
class spell_challengers_burst : public SpellScriptLoader
{
public:
    spell_challengers_burst() : SpellScriptLoader("spell_challengers_burst") {}

    class spell_challengers_burst_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_challengers_burst_AuraScript);

        void OnTick(AuraEffect const* /*aurEff*/)
        {
            Unit* target = GetTarget();
            if (!target)
                return;

            /*if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(243237))
            { TODO DETECTIVE FLUX
                int32 bp = spellInfo->GetEffect(EFFECT_0)->BasePoints * target->GetAuraCount(GetId());
                target->CastCustomSpell(target, 243237, &bp, NULL, NULL, true);
            }*/

        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_challengers_burst_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_challengers_burst_AuraScript();
    }
};

class challange_player_instance_handler : public PlayerScript
{
public:
    challange_player_instance_handler() : PlayerScript("challange_player_instance_handler") {  }


    void OnSuccessfulSpellCast(Player* player, Spell* spell)
    {
      //  if (spell->GetSpellInfo()->Id == 227334)
          //  player->TaxiFlightNearestNode();
    }

    void OnStartChallengeModeshadowlands(Player* player, uint8 level, uint8 affix1, uint8 affix2, uint8 affix3, uint8 affix4)
    {
        isstarted = true;
        _affix1 = affix1;
        _affix2 = affix2;
        _affix3 = affix3;
        _affix4 = affix4;
    }

    bool CheckChallengeModeStarted(Player* player)
    {
        if (InstanceScript* instance = player->GetInstanceScript())
            isstarted = instance->IsChallengeModeStarted();
        else
            isstarted = false;
        return isstarted;
    }

    void OnTakeDamage(Player* player, uint32 /*damage*/, SpellSchoolMask schoolMask)
    {
        if (isstarted)
        {
            isstarted = CheckChallengeModeStarted(player);
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->HasAffix(Affixes::Necrotic) && schoolMask == SPELL_SCHOOL_MASK_NORMAL)//necrotic
                {
                    player->AddAura(209858, player);
                }

                if (instance->HasAffix(Affixes::Grievous))//grievous
                {
                    if (player->GetHealthPct() < 90.f && !player->HasAura(240559) && player->IsInCombat())
                        player->CastSpell(player, 240559, true);

                    if (player->GetHealthPct() > 90.f || player->GetHealthPct() < 60.f)
                        player->RemoveAurasDueToSpell(240559);
                }
            }

        }
    }

    void OnUpdate(Player* player, uint32 diff)
    {
        if (checkTimer <= diff)
        {
            if (isstarted)
            {
                isstarted = CheckChallengeModeStarted(player);

                if (InstanceScript* instance = player->GetInstanceScript())
                {
                    if (instance->HasAffix(Affixes::Quaking) && player->IsInCombat())//quaking
                    {
                        player->AddAura(240447, player);
                    }
                }

            }
            checkTimer = 20000;
        }
        else checkTimer -= diff;

    }
    uint32 checkTimer = 10000;
    bool isstarted = false;
    uint8 _affix1;
    uint8 _affix2;
    uint8 _affix3;
    uint8 _affix4;
};

//226489 / sanguine_ichor AT 12765
struct at_challenge_sanguine_ichor : AreaTriggerAI
{
    at_challenge_sanguine_ichor(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (!unit)
            return;
        if (unit->IsPlayer())
            unit->CastSpell(unit, 226512, true);
        else
            unit->CastSpell(unit, 226510, true);
    }

    void OnUnitExit(Unit* unit) override
    {
        if (!unit)
            return;

        if (unit->HasAura(226512))
            unit->RemoveAura(226512);
        if (unit->HasAura(226510))
            unit->RemoveAura(226510);
    }

    void OnRemove() override
    {
        Unit* caster = at->GetCaster();

        if (!caster)
            return;

        for (auto guid : at->GetInsideUnits())
            if (Unit* unit = ObjectAccessor::GetUnit(*caster, guid))
            {
                if (unit->HasAura(226512))
                    unit->RemoveAura(226512);
                if (unit->HasAura(226510))
                    unit->RemoveAura(226510);
            }
    }
};

//226512
class spell_challenge_sanguine_ichor : public SpellScriptLoader
{
public:
    spell_challenge_sanguine_ichor() : SpellScriptLoader("spell_challenge_sanguine_ichor") { }

    class spell_challenge_sanguine_ichor_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_challenge_sanguine_ichor_AuraScript);

        void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (aurEff->GetCaster()->IsPlayer())
                amount = 5;
            else
                amount = 1;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challenge_sanguine_ichor_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_challenge_sanguine_ichor_AuraScript();
    }
};

// 280713 Champion of Azeroth
class spell_champion_of_azeroth : public SpellScriptLoader
{
public:
    spell_champion_of_azeroth() : SpellScriptLoader("spell_champion_of_azeroth") { }

    class spell_champion_of_azeroth_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_champion_of_azeroth_AuraScript);

        bool Validate(SpellInfo const* /*spell*/) override
        {
            return ValidateSpellInfo({ 280710 });
        }

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (AuraEffect* aurEf = caster->GetAuraEffect(280710, EFFECT_0)) // Champion of Azeroth
                {
                    amount = aurEf->GetAmount();
                }
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_champion_of_azeroth_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_champion_of_azeroth_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_RATING);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_champion_of_azeroth_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_RATING);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_champion_of_azeroth_AuraScript::CalculateAmount, EFFECT_3, SPELL_AURA_MOD_RATING);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_champion_of_azeroth_AuraScript();
    }
};

// 269239 Vampiric Speed
class spell_vampiric_speed_speed : public SpellScriptLoader
{
public:
    spell_vampiric_speed_speed() : SpellScriptLoader("spell_vampiric_speed_speed") { }

    class spell_vampiric_speed_speed_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_vampiric_speed_speed_AuraScript);

        bool Validate(SpellInfo const* /*spell*/) override
        {
            return ValidateSpellInfo({ 268599 });
        }

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (AuraEffect* aurEf = caster->GetAuraEffect(268599, EFFECT_1)) // Vampiric Speed
                {
                    amount = aurEf->GetAmount();
                }
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_vampiric_speed_speed_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_vampiric_speed_speed_AuraScript();
    }
};

// 269238 Vampiric Speed
class spell_vampiric_speed_heal : public SpellScriptLoader
{
public:
    spell_vampiric_speed_heal() : SpellScriptLoader("spell_vampiric_speed_heal") { }

    class spell_vampiric_speed_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_vampiric_speed_heal_SpellScript);

        bool Validate(SpellInfo const* /*spell*/) override
        {
            return ValidateSpellInfo({ 268599 });
        }

        void HandleHeal(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (AuraEffect* aurEf = caster->GetAuraEffect(268599, EFFECT_0)) // Vampiric Speed
                {
                    SetHitHeal(aurEf->GetAmount());
                }
            }
        }
        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_vampiric_speed_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }

    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_vampiric_speed_heal_SpellScript();
    }
};

// 278826 Crushing Assault (Level 120)
class spell_crushing_assault : public AuraScript
{
    PrepareAuraScript(spell_crushing_assault);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (AuraEffect* aurEf = caster->GetAuraEffect(SPELL_CRUSHING_ASSAULT_AMOUNT, EFFECT_0))
            {
                amount = aurEf->GetAmount();
            }
        }
    }

    void CalculateAmount2(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (AuraEffect* aurEf = caster->GetAuraEffect(SPELL_CRUSHING_ASSAULT_AMOUNT, EFFECT_1))
            {
                amount = -(aurEf->GetAmount());
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_crushing_assault::CalculateAmount, EFFECT_0, SPELL_AURA_DUMMY);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_crushing_assault::CalculateAmount2, EFFECT_1, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// Arcane Pulse (Nightborne racial) - 260364
class spell_arcane_pulse : public SpellScript
{
    PrepareSpellScript(spell_arcane_pulse);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        float damage = GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) * 2.f;

        if (!damage)
            damage = float(GetCaster()->GetTotalSpellPowerValue(SPELL_SCHOOL_MASK_ALL, false)) * 0.75f;

        SetHitDamage(damage);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_arcane_pulse::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

enum SpatialRiftSpells
{
    SPELL_SPATIAL_RIFT_AT = 256948,
    SPELL_SPATIAL_RIFT_TELEPORT = 257034,
    SPELL_SPATIAL_RIFT_DESPAWN_AT = 257040
};

// Spatial Rift teleport (Void Elf racial) - 257040
class spell_spatial_rift_despawn : public SpellScript
{
    PrepareSpellScript(spell_spatial_rift_despawn);

    void OnDespawnAreaTrigger(SpellEffIndex /*effIndex*/)
    {
        if (AreaTrigger* at = GetCaster()->GetAreaTrigger(SPELL_SPATIAL_RIFT_AT))
        {
            GetCaster()->CastSpell(at->GetPosition(), SPELL_SPATIAL_RIFT_TELEPORT, true);
            at->SetDuration(0);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spatial_rift_despawn::OnDespawnAreaTrigger, EFFECT_0, SPELL_EFFECT_DESPAWN_AREATRIGGER);
    }
};

// Light's Judgement - 256893  (Lightforged Draenei Racial)
class spell_light_judgement : public SpellScript
{
    PrepareSpellScript(spell_light_judgement);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            SetHitDamage(6.25f * caster->m_unitData->AttackPower);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_light_judgement::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

//312372
// 9.0.5
class spell_back_camp : public SpellScript
{
    PrepareSpellScript(spell_back_camp);

    SpellCastResult CheckRequirement()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (!player->GetStoredAuraTeleportLocation(313055))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_YOU_CANNOT_MAKE_YOUR_CAMP_HERE);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

        return SPELL_CAST_OK;
    }

    void HandleTeleport()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Player* player = caster->ToPlayer();
        if (!player)
            return;
        player->RemoveMovementImpairingAuras(false);

        // Tente: 292769
        // Sac: 276247
        // campfire: 301125
     //   if (WorldLocation const* dest = player->GetStoredAuraTeleportLocation(GetEffectInfo()->MiscValue))
        {
            uint32 spawntm = 300;
            if (GameObject* tempGob = player->SummonGameObject(292769, *player, QuaternionData::fromEulerAnglesZYX(player->GetOrientation(), 0.0f, 0.0f), Seconds(spawntm)))
                player->SetLastTargetedGO(tempGob->GetGUID().GetCounter());

            if (GameObject* tempGob = player->SummonGameObject(276247, Position(player->GetPositionX() + 2.0f, player->GetPositionY() + 2.0f, player->GetPositionZ(), player->GetOrientation()), QuaternionData::fromEulerAnglesZYX(player->GetOrientation(), 0.0f, 0.0f), Seconds(spawntm)))
                player->SetLastTargetedGO(tempGob->GetGUID().GetCounter());

            if (GameObject* tempGob = player->SummonGameObject(301125, Position(player->GetPositionX() + -2.0f, player->GetPositionY() + -2.0f, player->GetPositionZ(), player->GetOrientation()), QuaternionData::fromEulerAnglesZYX(player->GetOrientation(), 0.0f, 0.0f), Seconds(spawntm)))
                player->SetLastTargetedGO(tempGob->GetGUID().GetCounter());
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_back_camp::CheckRequirement);
        AfterCast += SpellCastFn(spell_back_camp::HandleTeleport);
    }
};

//312370
class spell_make_camp : public SpellScript
{
    PrepareSpellScript(spell_make_camp);

    void Oncast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        float x = caster->GetPositionX();
        float y = caster->GetPositionY();
        float z = caster->GetPositionZ();
        float o = caster->GetOrientation();
        uint32 m = caster->GetMapId();

        // Tente: 292769
        // Sac: 276247
        // campfire: 301125
        Player* player = caster->ToPlayer();
        if (!player)
            return;

        uint32 spawntm = 300;
        if (GameObject* tempGob = player->SummonGameObject(292769, *player, QuaternionData::fromEulerAnglesZYX(player->GetOrientation(), 0.0f, 0.0f), Seconds(spawntm)))
            player->SetLastTargetedGO(tempGob->GetGUID().GetCounter());

        if (GameObject* tempGob = player->SummonGameObject(276247, Position(x + 2.0f, y + 2.0f, z, o), QuaternionData::fromEulerAnglesZYX(player->GetOrientation(), 0.0f, 0.0f), Seconds(spawntm)))
            player->SetLastTargetedGO(tempGob->GetGUID().GetCounter());

        if (GameObject* tempGob = player->SummonGameObject(301125, Position(x + -2.0f, y + -2.0f, z, o), QuaternionData::fromEulerAnglesZYX(player->GetOrientation(), 0.0f, 0.0f), Seconds(spawntm)))
            player->SetLastTargetedGO(tempGob->GetGUID().GetCounter());
    }

    void Register()
    {
        OnCast += SpellCastFn(spell_make_camp::Oncast);
    }
};

//274738
class spell_maghar_orc_racial_ancestors_call : public SpellScript
{
    PrepareSpellScript(spell_maghar_orc_racial_ancestors_call);

    void Oncast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        uint32 RandomStats = urand(0, 3);

        switch (RandomStats)
        {
        case 0:
            //mastery
            caster->CastSpell(nullptr, 274741, true);
            break;

        case 1:

            //versatility
            caster->CastSpell(nullptr, 274742, true);
            break;

        case 2:
            //haste
            caster->CastSpell(nullptr, 274740, true);
            break;

        case 3:
            //crit
            caster->CastSpell(nullptr, 274739, true);
            break;
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_maghar_orc_racial_ancestors_call::Oncast);
    }
};

//161775
class spell_item_goren_gas_extractor : public SpellScriptLoader
{
public:
    spell_item_goren_gas_extractor() : SpellScriptLoader("spell_item_goren_gas_extractor") {}

    class spell_item_goren_gas_extractor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_item_goren_gas_extractor_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (Player* player = GetCaster()->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (!target || target->IsAlive() || target->GetEntry() != 79190 && target->GetEntry() != 80345)
                        return SPELL_FAILED_BAD_TARGETS;

            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_item_goren_gas_extractor_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_item_goren_gas_extractor_SpellScript();
    }
};

//500500
class item_level_boost_50 : public ItemScript
{
public:
    item_level_boost_50() : ItemScript("item_level_boost_50") { }

    bool OnRemove(Player* player, Item* /*item*/) override
    {
        if (player->GetLevel() < 60)
        {
            player->SetLevel(60);
            player->SetFullHealth();
            player->SetFullPower(player->GetPowerType());
            player->AddItem(182716, 4); //BAGS 30 slots
            player->AddItem(191380, 10); //healing potion
            player->LearnSpell(34092, true);// riding skill
            player->AddItem(194034, 1); //dragon riding mount
            player->ModifyMoney(5000000, false);
            switch (player->GetClass())
            {
            case CLASS_WARRIOR: //plate set ilvl 250
                player->AddItem(204948, 1); //Helm 
                player->AddItem(204950, 1); //Shoulder
                player->AddItem(204945, 1); //Chest
                player->AddItem(204951, 1); //Waist
                player->AddItem(204949, 1); //Legs
                player->AddItem(204946, 1); //Feet
                player->AddItem(204952, 1); //Wrist
                player->AddItem(204947, 1); //Hands
                player->AddItem(193779, 2); //2 hand weapon
                player->AddItem(193778, 1); //Shield
                player->AddItem(193785, 1); //ONE hand weapon
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_PALADIN:
                player->AddItem(204948, 1); //head
                player->AddItem(204950, 1); //shoulder
                player->AddItem(204945, 1); //chest
                player->AddItem(204934, 1); //waist
                player->AddItem(204949, 1); //legs
                player->AddItem(204946, 1); //feet
                player->AddItem(204952, 1); //wrist
                player->AddItem(204947, 1); //hands
                player->AddItem(193779, 1); //2 hand weapon
                player->AddItem(193778, 1); //shield
                player->AddItem(204956, 1); // one hand weapon
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_EVOKER:
                player->AddItem(204940, 1); //head
                player->AddItem(204942, 1); //Shoulder
                player->AddItem(204936, 1); //chest
                player->AddItem(204805, 1); //trinket
                player->AddItem(204943, 1); //waist
                player->AddItem(204941, 1); //legs
                player->AddItem(204937, 1); //feet
                player->AddItem(193727, 1); //wrist
                player->AddItem(204938, 1); //hands
                player->AddItem(193803, 1); //staff
                break;
            case CLASS_DEATH_KNIGHT:
                player->AddItem(204948, 1); //head
                player->AddItem(204950, 1); //shoulder
                player->AddItem(204945, 1); //chest
                player->AddItem(204951, 1); //waist
                player->AddItem(204949, 1); //legs
                player->AddItem(204946, 1); //feet
                player->AddItem(204952, 1); //wrist
                player->AddItem(204947, 1); //2 hands
                player->AddItem(204956, 1); //one hand 
                player->AddItem(204805, 1); //trinket
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_HUNTER:
                player->AddItem(204940, 1); //head
                player->AddItem(204942, 1); //Shoulder
                player->AddItem(204936, 1); //chest
                player->AddItem(204943, 1); //waist
                player->AddItem(204941, 1); //legs
                player->AddItem(204937, 1); //feet
                player->AddItem(193727, 1); //wrist
                player->AddItem(200139, 1); //Bow
                player->AddItem(204938, 1); //hands
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_SHAMAN:
                player->AddItem(204940, 1); //head
                player->AddItem(204942, 1); //Shoulder
                player->AddItem(204936, 1); //chest
                player->AddItem(204943, 1); //waist
                player->AddItem(204941, 1); //legs
                player->AddItem(204937, 1); //feet
                player->AddItem(193727, 1); //wrist
                player->AddItem(204938, 1); //hands
                player->AddItem(164282, 1);
                player->AddItem(164320, 2);
                player->AddItem(193803, 1); //staff
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_MAGE:
                player->AddItem(166828, 1);
                player->AddItem(164825, 1);
                player->AddItem(164824, 1);
                player->AddItem(164829, 1);
                player->AddItem(164827, 1);
                player->AddItem(164822, 1);
                player->AddItem(166826, 1);
                player->AddItem(166823, 1);
                player->AddItem(193803, 1); //staff
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_WARLOCK:
                player->AddItem(166828, 1);
                player->AddItem(164825, 1);
                player->AddItem(164824, 1);
                player->AddItem(164829, 1);
                player->AddItem(164827, 1);
                player->AddItem(164822, 1);
                player->AddItem(166826, 1);
                player->AddItem(166823, 1);
                player->AddItem(193803, 1); //staff
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_PRIEST:
                player->AddItem(166828, 1);
                player->AddItem(164825, 1);
                player->AddItem(164824, 1);
                player->AddItem(164829, 1);
                player->AddItem(164827, 1);
                player->AddItem(164822, 1);
                player->AddItem(166826, 1);
                player->AddItem(166823, 1);
                player->AddItem(193803, 1); //staff
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_ROGUE:
                player->AddItem(164559, 1);
                player->AddItem(164561, 1);
                player->AddItem(164560, 1);
                player->AddItem(204934, 1); //waist
                player->AddItem(164557, 1);
                player->AddItem(164556, 1);
                player->AddItem(164555, 1);
                player->AddItem(164558, 1);
                player->AddItem(164320, 2);
                player->AddItem(164291, 2);
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_DEMON_HUNTER:
                player->AddItem(164559, 1);
                player->AddItem(164561, 1);
                player->AddItem(164560, 1);
                player->AddItem(204934, 1); //waist
                player->AddItem(164557, 1);
                player->AddItem(164556, 1);
                player->AddItem(164555, 1);
                player->AddItem(164558, 1);
                player->AddItem(164320, 2);
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_MONK:
                player->AddItem(164559, 1);
                player->AddItem(164561, 1);
                player->AddItem(164560, 1);
                player->AddItem(204934, 1); //waist
                player->AddItem(164557, 1);
                player->AddItem(164556, 1);
                player->AddItem(164555, 1);
                player->AddItem(164558, 1);
                player->AddItem(164359, 1);
                player->AddItem(193803, 1); //staff
                player->AddItem(204805, 1); //trinket
                break;
            case CLASS_DRUID:
                player->AddItem(164559, 1);
                player->AddItem(164561, 1);
                player->AddItem(164560, 1);
                player->AddItem(204934, 1); //waist
                player->AddItem(164557, 1);
                player->AddItem(164556, 1);
                player->AddItem(164555, 1);
                player->AddItem(164558, 1);
                player->AddItem(164359, 1);
                player->AddItem(193803, 1); //staff
                player->AddItem(204805, 1); //trinket
                break;
            }
            player->SaveToDB();
        }

        return true;
    }
};

//// 220124 - Ratstallion Harness
//class spell_gen_ratstallion_harness : public SpellScriptLoader
//{
//public:
//    spell_gen_ratstallion_harness() : SpellScriptLoader("spell_gen_ratstallion_harness") { }
//
//    class spell_gen_ratstallion_harness_AuraScript : public AuraScript
//    {
//        PrepareAuraScript(spell_gen_ratstallion_harness_AuraScript);
//
//        uint32 update = 0;
//
//        void OnUpdate(const uint32 diff)
//        {
//            update += diff;
//
//            if (update >= 250)
//            {
//                if (!GetCaster())
//                    return;
//
//                if (Unit* vehicle = GetCaster()->GetVehicleBase())
//                {
//                    if (vehicle->IsInCombat())
//                        GetAura()->Remove();
//                    else if (GetCaster()->IsInCombat())
//                        GetAura()->Remove();
//                }
//                update = 0;
//            }
//        }
//
//        void Register() override
//        {
//             OnAuraUpdate += AuraUpdateFn(spell_gen_ratstallion_harness_AuraScript::OnUpdate, SPELL_AURA_CONTROL_VEHICLE);
//        }
//    };
//
//    AuraScript* GetAuraScript() const override
//    {
//        return new spell_gen_ratstallion_harness_AuraScript();
//    }
//};

enum racialspell
 {
    SPELL_DRAENEI_LIGHT_JUDGEMENT = 255647,
 };

//255647
class spell_Draenei_light_judgment : public SpellScriptLoader
{
public:
    spell_Draenei_light_judgment() : SpellScriptLoader("spell_Draenei_light_judgment") { }

    class spell_Draenei_light_judgment_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_Draenei_light_judgment_SpellScript);

        void HandleOnHit()
        {
            if (Player* player = GetCaster()->ToPlayer())
            {
                int32 damage = player->GetTotalAttackPowerValue(BASE_ATTACK) * 3 * 1 * (1);
                GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DRAENEI_LIGHT_JUDGEMENT, &damage);
            }
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_Draenei_light_judgment_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_Draenei_light_judgment_SpellScript();
    }
};

//159224 zuldazarhearthstone
class item_sack_hearthstone : public ItemScript
{
public:
    item_sack_hearthstone() : ItemScript("item_sack_hearthstone") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/, ObjectGuid /*castId*/) override
    {
        
        player->TeleportTo(2444, 238.304520f, -1028.009644f, 1437.286987f, 5.670575f);
            
        return true;
    }
};

void AddSC_Custom_Spells()
{
    new spell_fluxurion_test_spell();
    new spell_q34824();
    new spell_161033();
    new spell_interract();
    new spell_garr_lumberjack();
    new mob_garr_lumberjack();
    new spell_garr_lumberjack_lvl2();
    new mob_garr_lumberjack2();
    new spell_q37088_q37062();
    new spell_challengers_might();
    RegisterCreatureAI(npc_volcanic_plume_105877);
    RegisterCreatureAI(npc_challenger_fel_explosives);
    new spell_challengers_bolster();
    new spell_challengers_burst();
    new challange_player_instance_handler();
    RegisterAreaTriggerAI(at_challenge_sanguine_ichor);
    new spell_champion_of_azeroth();
    new spell_vampiric_speed_speed();
    new spell_vampiric_speed_heal();
    RegisterAuraScript(spell_crushing_assault);
    RegisterSpellScript(spell_arcane_pulse);
    RegisterSpellScript(spell_spatial_rift_despawn);
    RegisterSpellScript(spell_light_judgement);
    RegisterSpellScript(spell_make_camp);
    RegisterSpellScript(spell_back_camp);
    RegisterSpellScript(spell_maghar_orc_racial_ancestors_call);
    new spell_item_goren_gas_extractor();
    RegisterItemScript(item_level_boost_50);
   // new spell_gen_ratstallion_harness();
    RegisterSpellScript(spell_Draenei_light_judgment);
    new item_sack_hearthstone();
}
