/*
 * Copyright 2021
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
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Conversation.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "PhasingHandler.h"
#include "TemporarySummon.h"
#include "Transport.h"
#include "Vehicle.h"
#include "Log.h"
#include "Chat.h"
#include "ZoneScript.h"
#include "ScriptedGossip.h"
#include "Weather.h"
#include "Spell.h"
#include "TaskScheduler.h"
#include "zone_exiles_reach.h"

/*
TODO:
- need sniff missing texts and sounds
- need to implement movepointandwaitforplayer function which moves the creature to waypoint but not continue waypoint till player is within range

*/

class exiles_reach_a : public PlayerScript
{
public:
    exiles_reach_a() : PlayerScript("exiles_reach_a") { }

    // after the ship crash movie play the tiny murloc scene
    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        if (movieId == 895)
        {
            player->TeleportTo(2175, -462.722f, -2620.544f, 0.472f, 0.760f);
            player->RemoveAura(325131); //big black screen - means eyes closed //  remove - means opening eyes
            player->GetSceneMgr().PlaySceneByPackageId(SCENE_ALI_HORDE_CRASHED_ON_ISLAND, SceneFlag::None); // when opening eyes seeing murlocs xD
            player->RemoveAllAuras();
        }
    }
};

// npc_private_cole
struct npc_private_cole_160664 : public ScriptedAI
{
    npc_private_cole_160664(Creature* creature) : ScriptedAI(creature) { }
    bool talked;

    void Reset() override
    {
        ScriptedAI::Reset();
        talked = false;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {

        if (quest->GetQuestId() == QUEST_STAND_YOUR_GROUND)
        {
            Talk(NPC_COLE_TXT0);

            player->GetScheduler().Schedule(Milliseconds(5000), [this] (TaskContext context)
            {
                me->SetFaction(14);
                me->SetReactState(REACT_AGGRESSIVE);
                Talk(NPC_COLE_TXT1);
            });
        }

        if (quest->GetQuestId() == QUEST_BRACE_FOR_IMPACT)
        {
            Talk(NPC_COLE_TXT4);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_HEAVY_RAIN, 0.5f);

            if (Creature* npc = player->FindNearestCreature(NPC_GARRIC, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(3000), [npc] (TaskContext context)
                {
                    npc->AI()->Talk(GARRIC_TXT4);
                });
            }
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if (!talked)
                if ((me->GetHealth() < me->CountPctFromMaxHealth(60)) && (me->GetHealth() >= me->CountPctFromMaxHealth(50)))
                {
                    Talk(NPC_COLE_TXT2);
                    talked = true;
                }

            if (me->GetHealth() <= damage )
            {
                damage = 0;
                me->SetFaction(35);
                me->AttackStop();
                me->SetFullHealth();
                attacker->ToPlayer()->KilledMonsterCredit(155607, ObjectGuid::Empty);


                me->AI()->Reset();
                attacker->ToPlayer()->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext context)
                {
                    Talk(NPC_COLE_TXT3);
                });
            }
        }

    }

};

//157051 npc_private_cole
struct npc_private_cole_dummy : public ScriptedAI
{
    npc_private_cole_dummy(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false); // tempfix for npc falling through the boat
    }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_STAND_YOUR_GROUND)
        {
            me->SetFaction(14);
            me->SetReactState(REACT_AGGRESSIVE);
            me->Say(177657);

        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            if (player->HasQuest(QUEST_STAND_YOUR_GROUND))
        {
            me->Say(177657);
        }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
            if (player->HasQuest(QUEST_STAND_YOUR_GROUND))
            {
                player->KilledMonsterCredit(155607);
                me->Say(177677);
                me->DespawnOrUnsummon(10ms);
                me->SummonCreature(160664, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
            }
    }
};

// npc_captain_garrick_156280
struct npc_captain_garrick_156280 : public ScriptedAI
{
    npc_captain_garrick_156280(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR)
        {
            player->SummonCreature(156595, player->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 10min); //10 minutes to despawn
            if (TempSummon* garrick = player->SummonCreature(156280, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 10min))
            {
                me->DespawnOrUnsummon(1s, 60s);
            }
        }

        if (quest->GetQuestId() == QUEST_WARMING_UP)
        {
            //Talk(GARRIC_TXT1);
            Speak(184321, 152732, player);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR))
            {
                me->AddDelayedEvent(2000, [this]() -> void
                    {
                        Talk(6);
                    });
                me->AddDelayedEvent(8000, [this, player]() -> void
                    {
                        me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());
                        if (Creature* boar = me->FindNearestCreature(156595, 20.0f, true))
                            me->EnterVehicle(boar, 1);
                    });
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_TALK_GARRICK_HENRY)
        {
            Talk(10);
            me->AddDelayedEvent(12000, [this]() -> void
                {
                    Talk(11);
                });
        }
    }


    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_WARMING_UP)
        {
            //me->AI()->Talk(GARRIC_TXT3, player);
            me->AI()->Speak(184106,152734,player);
        }

        if (quest->GetQuestId() == QUEST_BRACE_FOR_IMPACT)
        {
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
            player->CastSpell(player, SPELL_A_SHIP_CRASH);
        }
    }
};

//npc_alliance_gryphon_154155
struct npc_alliance_gryphon_154155 : public ScriptedAI
{
    npc_alliance_gryphon_154155(Creature* creature) : ScriptedAI(creature)
    {
        Vehicle* vehicle = me->GetVehicleKit();
    }

    void Reset() override
    {
        me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->HasQuest(QUEST_AN_END_TO_BEGINNINGS))
            {
                player->EnterVehicle(me);
                player->ForceCompleteQuest(QUEST_AN_END_TO_BEGINNINGS);

                player->GetSceneMgr().PlaySceneByPackageId(SCENE_FLY_AWAY, SceneFlag::None);
                player->GetScheduler().Schedule(Milliseconds(66000), [this, player](TaskContext context)
                    {
                        player->GetSceneMgr().CancelSceneByPackageId(SCENE_FLY_AWAY); // there will be black screen without this

                        switch (player->GetTeam())
                        {
                        case ALLIANCE:
                            player->GetScheduler().Schedule(Milliseconds(1000), [this, player](TaskContext context)
                                {
                                    player->GetSceneMgr().PlaySceneByPackageId(2972, SceneFlag::None);
                                });
                            player->TeleportTo(0, -9060.474f, 438.164f, 93.055f, 0.657f); // stormwind
                            break;

                        case HORDE:
                            player->TeleportTo(1, 1458.339f, -4420.480f, 25.453f, 0.161f); // orgrimmar
                            break;
                        }
                    });
            }
        }
    }
};

// npc_captain_garrick_156626
struct npc_captain_garrick_156626 : public EscortAI
{
    bool talked;
    npc_captain_garrick_156626(Creature* creature) : EscortAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        talked = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_BRACE_FOR_IMPACT) == QUEST_STATUS_REWARDED &&
                player->GetQuestStatus(QUEST_MURLOC_MANIA) == QUEST_STATUS_NONE)
            {
                if (player->IsInDist(me, 20.0f) && !talked)
                {
                    Talk(0, player);
                    talked = true;
                }
            }
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_MURLOC_MANIA)
        {
            Talk(1);
            me->AI()->Reset();
            talked = false;
        }

        if (quest->GetQuestId() == QUEST_EMERGENCY_FIRST_AID)
        {
            if (Creature* npc = me->FindNearestCreature(149917, 5.0f, true))
            {
                me->CastSpell(npc, SPELL_FIRST_AID);

                player->GetScheduler().Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(1, GrimaxePositions[0]);
                        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    })
                    .Schedule(15s, [this](TaskContext context)
                        {
                            if (Creature* npc = me->FindNearestCreature(156622, 5.0f, true))
                                me->CastSpell(npc, SPELL_FIRST_AID);
                            me->GetMotionMaster()->MovePoint(2, GrimaxePositions[1]);
                        })
                        .Schedule(28s, [this](TaskContext context)
                            {
                                me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                                me->SetFacingTo(3.4f, true);
                            });
            }
        }

        if (quest->GetQuestId() == QUEST_FINDING_THE_LOST_EXPEDITION)
        {
            // despawn old version after 60 sec and 30 sec for respawning
            me->DespawnOrUnsummon(60s, 30s);
            Start(true);
            SetEscortPaused(true);
            player->GetScheduler().Schedule(2s, [this, player](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                    Talk(2, player);
                });
        }
    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 3:
            if (Creature* npc = me->FindNearestCreature(151088, 10.0f, true)) // kee-la
            {
                npc->GetMotionMaster()->MovePoint(1, BoPositions[1]);
                npc->GetMotionMaster()->MovePoint(2, BoPositions[2]);
                npc->DespawnOrUnsummon(5s, 30s);
            }

            if (Creature* npc = me->FindNearestCreature(154170, 10.0f, true)) // Austin Huxworth
            {
                npc->GetMotionMaster()->MovePoint(1, MithdranRastrealbaPositions[1]);
                npc->GetMotionMaster()->MovePoint(2, MithdranRastrealbaPositions[2]);
                npc->DespawnOrUnsummon(5s, 30s);
            }

            if (Creature* npc = me->FindNearestCreature(151089, 10.0f, true)) // Bjorn Stouthands
            {
                npc->GetMotionMaster()->MovePoint(1, LanaJordanPositions[1]);
                npc->GetMotionMaster()->MovePoint(2, LanaJordanPositions[2]);
                npc->DespawnOrUnsummon(5s, 30s);
            }
            break;
        case 6:
            SetEscortPaused(true);
            {
                if (HasEscortState(STATE_ESCORT_PAUSED))
                {
                    SetPauseTimer(3s);
                    SetEscortPaused(false);
                }
            }
            break;
        case 10:
            SetEscortPaused(true);
            SetPauseTimer(3s);
            SetEscortPaused(false);
            break;
        case 16:
            SetEscortPaused(true);
            if (Creature* npc = me->FindNearestCreature(156607, 20.0f, true))
            {
                npc->AI()->Talk(0);
                me->AddDelayedEvent(3000, [this]()
                    {
                        if (Creature* npc = me->FindNearestCreature(156651, 10.0f, true))
                            npc->RemoveAura(65050);
                        me->DespawnOrUnsummon(1s, 10s);
                    });
            }
            break;
        default:
            break;
        }
    }
};

// npc_cole_149917
struct npc_cole_149917 : public ScriptedAI
{
    npc_cole_149917(Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetWalk(true);

        caster->ToUnit()->GetScheduler().Schedule(Milliseconds(2000), [this](TaskContext /*task*/)
            {
                me->GetMotionMaster()->MovePoint(1, -389.445f, -2593.08f, 4.0625f);
                me->ForcedDespawn(15000);
            });
    }
};

// npc_richter_156622
struct npc_richter_156622 : public ScriptedAI
{
    npc_richter_156622(Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetWalk(true);

        caster->ToUnit()->GetScheduler().Schedule(Milliseconds(2000), [this](TaskContext /*task*/)
            {
                me->GetMotionMaster()->MovePoint(1, -389.445f, -2593.08f, 4.0625f);
                me->ForcedDespawn(10000);
            });
    }
};

// npc_Bjorn_Stouthands_156609
struct npc_Bjorn_Stouthands_156609 : public ScriptedAI
{
    npc_Bjorn_Stouthands_156609(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->SetEmoteState(EMOTE_STATE_SLEEP);
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
            Talk(0);
            me->CastSpell(me, SPELL_FIRST_AID, true);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            player->KilledMonsterCredit(156609);

            player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext /*task*/)
                {
                    me->GetMotionMaster()->MovePoint(1, LanaJordanPositions[0], MOVE_WALK);
                });
        }
    }
};

// npc_Austin_Huxworth_156610
struct npc_Austin_Huxworth_156610 : public ScriptedAI
{
    npc_Austin_Huxworth_156610(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->SetEmoteState(EMOTE_STATE_SLEEP);
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
            Talk(0);
            me->CastSpell(me, SPELL_FIRST_AID, true);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            player->KilledMonsterCredit(156610);

            player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext /*task*/)
                {
                    me->GetMotionMaster()->MovePoint(1, MithdranRastrealbaPositions[0], MOVE_WALK, false);
                });
        }
    }
};

// npc_kee_la_156612
struct npc_kee_la_156612 : public ScriptedAI
{
    npc_kee_la_156612(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->SetEmoteState(EMOTE_STATE_SLEEP);
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
            Talk(0);
            me->CastSpell(me, SPELL_FIRST_AID, true);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            player->KilledMonsterCredit(156612);

            player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext /*task*/)
                {
                    me->GetMotionMaster()->MovePoint(1, BoPositions[0], MOVE_WALK, false);
                });
        }
    }
};

// npc 156651 - captain garrick sparring version
struct npc_captain_garrick_156651 : public ScriptedAI
{
    npc_captain_garrick_156651(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        me->SetFullHealth();
        me->SetFaction(35); // 35 was their faction basically... need re-sniff -.-
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_COOKING_MEAT)

            if (Creature* allaria = me->FindNearestCreature(156607, 10.0f, true))
            {
                allaria->AI()->Talk(0);

                me->AddDelayedEvent(5000, [this, player]()        // time_delay
                    {
                        me->AI()->Talk(0);
                        player->GetScheduler().Schedule(6s, [this, player](TaskContext context)
                            {
                                me->AI()->Talk(1);
                            });
                    });
            }

        if (quest->GetQuestId() == QUEST_ENHANCED_COMBAT_TACTICS)
        {
            Talk(3);

            player->GetScheduler().Schedule(2s, [this](TaskContext context)
                {
                    me->GetMotionMaster()->MovePoint(1, -245.244f, -2479.318f, 18.080f, true);
                }).Schedule(4s, [this](TaskContext context)
                    {
                        me->GetMotionMaster()->MovePoint(2, -216.699f, -2508.925f, 22.957f, true);
                    }).Schedule(12s, [this, player](TaskContext context)
                        {
                            me->AI()->Talk(4);
                            me->SetFaction(14);
                            me->SetLevel(player->GetLevel());
                            me->SetTarget(player->GetGUID());
                        });
        }
      }

        void SpellHit(WorldObject * caster, SpellInfo const* spell) override
        {
            int8 itwilldo = rand() % 2 + 1; // 50% chance to breka will accept the spell;

            if (caster->IsPlayer())
            {
                Player* player = caster->ToPlayer();
                if (player->HasQuest(QUEST_ENHANCED_COMBAT_TACTICS) && itwilldo == 1)
                {

                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_ENHANCED_COMBAT_TACTICS))
                        for (QuestObjective const& obj : quest->GetObjectives())
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_ENHANCED_COMBAT_TACTICS);
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj))
                            {
                                player->KilledMonsterCredit(164577);

                            }
                            else
                            {
                                me->AttackStop();
                                Reset();
                                player->GetScheduler().Schedule(2s, [this](TaskContext context)
                                    {
                                        me->GetMotionMaster()->MovePoint(2, -249.05f, -2492.35f, 17.957f, true);
                                    });
                            }

                        }
                }
            }
        }

        void JustReachedHome() override
        {
            if (Creature* allaria = me->FindNearestCreature(156607, 20.0f, true))
                me->SetFacingToObject(allaria, true); //
        }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
        {

            if ((me->GetHealth() <= damage) || (me->GetHealth() <= me->GetHealth() * 0.2))
            {
                Player* player = attacker->ToPlayer();
                if (player->HasQuest(QUEST_ENHANCED_COMBAT_TACTICS))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_ENHANCED_COMBAT_TACTICS))
                        for (QuestObjective const& obj : quest->GetObjectives())
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_ENHANCED_COMBAT_TACTICS);
                            // if garrick got too much dmg but the quest is not completed breka got full hp just like on retail
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                            {
                                damage = 0;
                                me->SetFullHealth();
                            }
                            else
                            {
                                damage = 0;
                                me->AttackStop();
                                Reset();
                                player->GetScheduler().Schedule(2s, [this](TaskContext context)
                                    {
                                        me->GetMotionMaster()->MovePoint(1, -245.244f, -2479.318f, 18.080f, true);
                                    }).Schedule(12s, [this](TaskContext context)
                                        {
                                            me->GetMotionMaster()->MovePoint(2, -249.05f, -2492.35f, 17.957f, true);
                                        });
                            }
                        }
                }

            }
        }

        void JustDied(Unit* killer /*killer*/)
        {
            Player* player = killer->ToPlayer();
            if (player->HasQuest(QUEST_ENHANCED_COMBAT_TACTICS))
            {
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_ENHANCED_COMBAT_TACTICS))
                    for (QuestObjective const& obj : quest->GetObjectives())
                    {
                        uint16 slot = player->FindQuestSlot(QUEST_ENHANCED_COMBAT_TACTICS);
                        // if garrick got too much dmg but the quest is not completed breka got full hp just like on retail
                        if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                        {
                            me->Respawn();
                            me->SetFullHealth();
                        }
                        else
                        {
                            me->Respawn();
                            me->AttackStop();
                            Reset();
                            me->GetMotionMaster()->MovePoint(3, -249.05f, -2492.35f, 17.957f, true);
                        }
                    }
            }
        }
};




//  npc_alaria_156607
struct npc_alaria_156607 : public ScriptedAI
{
    npc_alaria_156607(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 10.0f))
            {
                if (player->GetQuestStatus(QUEST_EMERGENCY_FIRST_AID) == QUEST_STATUS_REWARDED)
                {
                    if (Creature* npc = me->FindNearestCreature(156651, 15.0f, true))
                        npc->RemoveAura(65050);
                }

                if (player->HasItemCount(COOKED_MEAT, 2, false))
                {
                    if (Creature* npc = me->FindNearestCreature(156651, 10.0f, true))
                    {
                        if (!say)
                        {
                            Talk(2);

                            player->GetScheduler().Schedule(5s, [npc](TaskContext context)
                                {
                                    npc->AI()->Talk(2);
                                });
                            say = true;
                        }
                    }
                }
            }
        }
    }
private:
    bool say;


    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_COOKING_MEAT)
        {
            if (player->GetItemByEntry(174072))
            {
                uint32 count = player->GetItemCount(174072);
                player->DestroyItemCount(174072, count, true, false);
            }
        }
        me->SetStandState(UNIT_STAND_STATE_STAND);
        say = false;
    }

};

// npc_alaria_175031
struct npc_alaria_175031 : public ScriptedAI
{
    npc_alaria_175031(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_NORTHBOUND)
        {
            if (Creature* garrick = me->FindNearestCreature(156651, 10.0f, true))
                garrick->DespawnOrUnsummon(1s, 60s);

            if (TempSummon* garrick = player->SummonCreature(156651, Position(-249.05f, -2492.35f, 17.957f, 0.466517f), TEMPSUMMON_TIMED_DESPAWN, 1min))
            {
                garrick->AI()->Talk(6);
                player->GetScheduler().Schedule(Milliseconds(5000), [garrick, player](TaskContext context)
                    {
                        garrick->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                        // running to mithdran
                        garrick->AddDelayedEvent(3000, [garrick]() { garrick->GetMotionMaster()->MovePoint(1, -238.273f, -2467.617f, 15.990f, true); });
                        garrick->AddDelayedEvent(7500, [garrick]() { garrick->GetMotionMaster()->MovePoint(1, -220.940f, -2470.998f, 16.520f, true); });
                        garrick->AddDelayedEvent(10000, [garrick]()
                            {
                                garrick->GetMotionMaster()->MovePoint(1, -197.318f, -2510.768f, 21.758f, true);
                                garrick->AI()->Talk(7);
                            });
                        garrick->AddDelayedEvent(17500, [garrick]() { garrick->GetMotionMaster()->MovePoint(1, -190.575f, -2555.820f, 27.304f, true); });
                        garrick->AddDelayedEvent(25000, [garrick]() { garrick->GetMotionMaster()->MovePoint(1, -173.183f, -2584.720f, 33.041f, true); });
                        garrick->AddDelayedEvent(30000, [garrick]() { garrick->GetMotionMaster()->MovePoint(1, -155.535f, -2596.575f, 36.171f, true); });
                        garrick->AddDelayedEvent(33500, [garrick]() { garrick->GetMotionMaster()->MovePoint(1, -137.381f, -2625.058f, 44.287f, true); });
                        garrick->AddDelayedEvent(40000, [garrick, player]()
                            {
                                garrick->GetMotionMaster()->MovePoint(1, -142.622f, -2641.04f, 48.9143f, true);
                            });
                        garrick->AddDelayedEvent(45000, [garrick]() { garrick->SetFacingTo(6.2f, true); });
                        garrick->AddDelayedEvent(48000, [garrick, player]()
                            {
                                garrick->SetStandState(UNIT_STAND_STATE_KNEEL);
                                if (Creature* garrick2 = player->FindNearestCreature(156662, 50.0f, true))
                                    garrick2->RemoveAura(65050);
                            });
                    });
            }
        }
    }
};

//154327 Austin Huxworth
struct npc_austin_huxworth_154327 : public ScriptedAI
{
    npc_austin_huxworth_154327(Creature* c) : ScriptedAI(c) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_DOWN_WITH_THE_QUILBOAR) && player->HasQuest(QUEST_FORBIDDEN_QUILBOAR_NECROMANCY))
        {
            if (Creature* npc = me->FindNearestCreature(156662, 5.0f, true))
            {
                npc->AI()->Talk(0);
                player->GetScheduler().Schedule(Milliseconds(5500), [this](TaskContext context)
                    {
                        me->AI()->Talk(1);

                    }).Schedule(Milliseconds(11000), [this, npc](TaskContext context)
                        {
                            npc->AI()->Talk(1);
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            npc->SetStandState(UNIT_STAND_STATE_STAND);
                            me->GetMotionMaster()->MovePoint(1, -128.411f, -2637.33f, 48.4554f, true);
                            npc->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, npc->GetFollowAngle());

                        }).Schedule(Milliseconds(14000), [this, npc](TaskContext context)
                            {
                                me->GetMotionMaster()->MovePoint(2, -109.327f, -2646.74f, 52.5968f, true);

                            }).Schedule(Milliseconds(18000), [this, npc](TaskContext context)
                                {
                                    me->GetMotionMaster()->MovePoint(3, -81.9396f, -2644.04f, 57.433f, true);
                                    me->DespawnOrUnsummon(5s, 30s);
                                    npc->DespawnOrUnsummon(5s, 30s);
                                });
            }
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_NORTHBOUND)
        {
            Talk(0);
            if (Creature* garrick = me->FindNearestCreature(156651, 10.0f, true)) // maybe need to check if she is on same phase as me
                garrick->ForcedDespawn(500);
        }
    }
};

// npc_geolord_grekog_151091
struct npc_geolord_grekog_151091 : public ScriptedAI
{
    npc_geolord_grekog_151091(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void JustEngagedWith(Unit* attacker) override                  // attack_started
    {
        if (Player* player = attacker->ToPlayer())
        {
            Talk(0);

            if (Creature* npc = player->FindNearestCreature(167008, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(5000), [npc](TaskContext context)
                    {
                        npc->AI()->Talk(0);
                    });
            }

            if (Creature* Lindie = player->FindNearestCreature(154301, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(5000), [Lindie](TaskContext context)
                    {
                        Lindie->AI()->Talk(0);
                    });
            }


            me->AddDelayedEvent(3000, [this, player]()
                {
                    // cast upheaval
                    me->CastSpell(player, SPELL_UPHEAVAL);
                });
        }
    }

    void JustDied(Unit* attacker) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if (player->HasQuest(QUEST_H_DOWN_WITH_THE_QUILBOAR))
            {
                if (Creature* npc = player->FindNearestCreature(167008, 50.0f, true))
                {
                    npc->SetCanFly(false);
                    npc->RemoveAura(305513);
                    npc->GetMotionMaster()->MovePoint(1, 16.6666f, -2511.82f, 73.39143f, true);
                    npc->AI()->Talk(1);

                    // walking to breka after freed
                    npc->AddDelayedEvent(3000, [npc]() { npc->GetMotionMaster()->MovePoint(1, 30.750f, -2513.189f, 73.408f, true); });
                    npc->AddDelayedEvent(5500, [npc]() { npc->GetMotionMaster()->MovePoint(1, 52.428f, -2481.062f, 78.931f, true); });
                    npc->AddDelayedEvent(11000, [npc]() { npc->GetMotionMaster()->MovePoint(1, 74.708f, -2458.385f, 88.283f, true); });
                    npc->AddDelayedEvent(16500, [npc]() { npc->GetMotionMaster()->MovePoint(1, 100.124f, -2429.822f, 90.373f, true); });
                    npc->AddDelayedEvent(22000, [npc, player]()
                        {
                            npc->GetMotionMaster()->MovePoint(1, 100.666f, -2417.773f, 90.392f, true);
                            npc->PlayDirectSound(156984, player, 196270); npc->Say(196270); // You must be Warlord Breka! Shuja's mother! She always said you'd save our hides.
                        });
                    npc->AddDelayedEvent(28000, [npc, player]()
                        {
                            if (Creature* grimaxe = player->FindNearestCreature(167021, 50.0f, true))
                                grimaxe->AI()->Talk(0);
                            npc->SetFacingToObject(npc->FindNearestCreature(167021, 10.0f));
                        });
                    npc->AddDelayedEvent(32000, [npc]()
                        {
                            npc->AI()->Talk(2);
                            npc->AddAura(SPELL_SEE_QUEST_INVISIBILITY, npc);
                            npc->SetRespawnDelay(30); // 30 sec for respawning
                            npc->DespawnOrUnsummon(1s); // despawn immediately
                        });
                }
            }

            if (player->HasQuest(QUEST_DOWN_WITH_THE_QUILBOAR))
            {
                if (Creature* npc = player->FindNearestCreature(154301, 50.0f, true))
                {
                    npc->SetCanFly(false);
                    npc->RemoveAura(305513);
                    npc->GetMotionMaster()->MovePoint(1, 16.6666f, -2511.82f, 73.39143f, true);
                    npc->AI()->Talk(0);

                    // walking to breka after freed
                    npc->AddDelayedEvent(3000, [npc]() { npc->GetMotionMaster()->MovePoint(1, 30.750f, -2513.189f, 73.408f, true); });
                    npc->AddDelayedEvent(5500, [npc]() { npc->GetMotionMaster()->MovePoint(1, 52.428f, -2481.062f, 78.931f, true); });
                    npc->AddDelayedEvent(11000, [npc]() { npc->GetMotionMaster()->MovePoint(1, 74.708f, -2458.385f, 88.283f, true); });
                    npc->AddDelayedEvent(16500, [npc]() { npc->GetMotionMaster()->MovePoint(1, 100.124f, -2429.822f, 90.373f, true); });
                    npc->AddDelayedEvent(22000, [npc, player]()
                        {
                            npc->GetMotionMaster()->MovePoint(1, 100.666f, -2417.773f, 90.392f, true);
                            npc->AI()->Talk(1);
                        });
                    npc->AddDelayedEvent(28000, [npc, player]()
                        {
                            if (Creature* garrick = player->FindNearestCreature(156280, 50.0f, true))
                                garrick->AI()->Talk(5);
                            npc->SetFacingToObject(npc->FindNearestCreature(156280, 10.0f));
                        });
                    npc->AddDelayedEvent(32000, [npc]()
                        {
                            npc->AI()->Talk(2);
                        });
                }
            }
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();


        if ((hp < maxhp * 0.75) && (hp > maxhp * 0.6)) // if hp between 60-75%
        {
            // cast earthbolt
            me->CastSpell(player, SPELL_EARTHBOLT);
        }

        if ((hp < maxhp * 0.4) && (hp > maxhp * 0.3)) // if hp between 30-40%
        {
            // cast upheaval
            me->CastSpell(player, SPELL_UPHEAVAL);
        }
    }

};


//npc_lindie_springstock_149899
struct npc_lindie_springstock_149899 : public ScriptedAI
{
    npc_lindie_springstock_149899(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_SCOUT_O_MATIC_5000)
        {

            player->SummonCreature(156526, Position(106.282f, -2415.671f, 90.41f, 3.601f), TEMPSUMMON_MANUAL_DESPAWN);
            player->FindNearestCreature(156526, 10.0f, true)->SetHover(true);
            player->FindNearestCreature(156526, 10.0f, true)->SetHoverHeight(3);

            player->GetScheduler().Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    Creature* fake_choppy = me->FindNearestCreature(156526, 10.0f, true);
                    me->SelectNearbyTarget(fake_choppy, 10.0f);
                    me->CastSpell(fake_choppy, SPELL_RESIZER_CHANNELING);

                }).Schedule(Milliseconds(5000), [this, player](TaskContext context)
                    {
                        Creature* fake_choppy = me->FindNearestCreature(156526, 10.0f, true);
                        fake_choppy->CastSpell(fake_choppy, SPELL_RESIZER_HIT_TOSMALL);
                        me->AI()->Talk(3);

                    }).Schedule(Milliseconds(11000), [this, player](TaskContext context)
                        {

                            Creature* fake_choppy = me->FindNearestCreature(156526, 10.0f, true);
                            fake_choppy->RemoveAura(SPELL_RESIZER_HIT_TOSMALL);
                            fake_choppy->CastSpell(fake_choppy, SPELL_INCREASE_SIZE);
                            me->AI()->Talk(4);

                        }).Schedule(Milliseconds(17000), [this, player](TaskContext context)
                            {
                                Creature* fake_choppy = me->FindNearestCreature(156526, 10.0f, true);
                                fake_choppy->RemoveAura(SPELL_INCREASE_SIZE);
                                me->AI()->Talk(5);

                            }).Schedule(Milliseconds(20000), [this](TaskContext context)
                                {
                                    Creature* fake_choppy = me->FindNearestCreature(156526, 10.0f, true);
                                    fake_choppy->DespawnOrUnsummon(2s);

                                }).Schedule(Milliseconds(23000), [this, player](TaskContext context)
                                    {
                                        player->SummonCreature(156518, Position(103.419f, -2417.24f, 92.4075f, 0.55373f), TEMPSUMMON_MANUAL_DESPAWN);
                                    });
        }

        if (quest->GetQuestId() == QUEST_RE_SIZING_THE_SITUATION)
        {
            player->CastSpell(player, 305750);   //summon_lindie
            me->AddAura(65050, me);     //invisable_aura
        }
    }
};


// npc_lindie_springstock_156749
struct npc_lindie_springstock_156749 : public ScriptedAI
{

    npc_lindie_springstock_156749(Creature* c) : ScriptedAI(c) { Reset(); }

private:
    bool Lindie_say01;
    bool Lindie_say02;
    bool Lindie_say03;

    void Reset() override
    {
        Lindie_say01 = false;
        Lindie_say02 = false;
        Lindie_say03 = false;
    };

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_RE_SIZING_THE_SITUATION))
            {
                Talk(0);
                me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());
            }
        }

    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 15.0f))
            {
                if (player->HasQuest(QUEST_RE_SIZING_THE_SITUATION))
                {
                    if (player->GetQuestObjectiveProgress(QUEST_RE_SIZING_THE_SITUATION, 0) == 1)
                    {
                        if (!Lindie_say01)
                        {
                            Talk(1);
                            Lindie_say01 = true;
                        }
                    }
                    if (player->GetQuestObjectiveProgress(QUEST_RE_SIZING_THE_SITUATION, 0) == 2)
                    {
                        if (!Lindie_say02)
                        {
                            Talk(2);
                            Lindie_say02 = true;
                        }
                    }
                    if (player->GetQuestObjectiveProgress(QUEST_RE_SIZING_THE_SITUATION, 0) == 3)
                    {
                        if (!Lindie_say03)
                        {
                            Talk(3);
                            Lindie_say03 = true;
                            me->SetWalk(false);
                            me->GetMotionMaster()->MovePoint(1, 100.742f, -2417.84f, 90.3844f, true, 3.81584f);
                            me->AddDelayedEvent(5000, [this, player]() -> void
                                {
                                    if (auto* lindie2 = me->FindNearestCreature(149899, 100.0f, true))
                                        lindie2->AI()->DoAction(ACTION_REMOVE_INVISIABLE_AURA);
                                    player->CastSpell(player, 305756);   //Desummon_lindie
                                });
                        }
                    }
                }

                if (player->GetQuestStatus(QUEST_RE_SIZING_THE_SITUATION) == QUEST_STATUS_NONE)
                {
                    if (auto* lindie2 = me->FindNearestCreature(149899, 100.0f, true))
                        lindie2->AI()->DoAction(ACTION_REMOVE_INVISIABLE_AURA);
                    player->CastSpell(player,305756);
                }
            }

        }
    }
};

// npc_scout_o_matic_5000
struct npc_scout_o_matic_5000 : public ScriptedAI
{
    npc_scout_o_matic_5000(Creature* creature) : ScriptedAI(creature)
    {
        me->CanFly();
        me->SetFlying(true);
    }

private:
    Vehicle* vehicle;


    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_THE_SCOUT_O_MATIC_5000))
            {
                // TODO: change to correct vehicle where the passenger hanging
                vehicle = me->GetVehicleKit();
                summoner->ToUnit()->EnterVehicle(me);
                me->CastSpell(summoner, SPELL_ROPE);
                me->GetMotionMaster()->MovePoint(1, 264.0f, -2310.0f, 117.0f, true);

                player->AddDelayedEvent(2000, [player]() -> void
                    {
                        player->PlayConversation(12083);
                    });
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->ExitVehicle();
                        player->CastSpell(player, 321342);
                        player->KilledMonsterCredit(156518);
                        player->EnterVehicle(player);
                    });
                me->AddDelayedEvent(50000, [this]() -> void
                    {
                        me->GetMotionMaster()->MovePoint(2, 108.609f, -2413.905f, 95.530f, true);
                    });
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->PlayConversation(12084);
                    });
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->NearTeleportTo(102.3729f, -2418.46f, 91.2849f, 3.6666f, false);
                    });
                me->AddDelayedEvent(78000, [this, player]() -> void
                    {
                        me->ForcedDespawn(1000);
                    });
            }
        }

    }
};

// item 170557 item_resizer_v901
class item_resizer_v901 : public ItemScript
{
public:
    item_resizer_v901() : ItemScript("item_resizer_v901") { }

    int8 castcount = 1; // removed randomize to get all the experience of this magical awesome weapon

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        if (player->HasQuest(QUEST_RE_SIZING_THE_SITUATION))
        {

            int8 random_casting_mechanic = rand() % 3 + 1; // randomize number 1-3 to select a mechanic

            if (random_casting_mechanic == 1) // mechanic1 - channeling with aura on mob then resizing success
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(target, SPELL_RESIZER_CHANNELING); // channeling resizing til another action
                        player->GetScheduler().Schedule(Milliseconds(4000), [this, player](TaskContext context)
                            {
                                player->CastStop();
                                player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_HIT_TOSMALL); // making the boar small + credit
                                player->GetSelectedUnit()->ToCreature()->ForcedDespawn(1000, 15s); // despawn resized mob
                            });
                    }
            }
            if (random_casting_mechanic == 2) // channeling with aura
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(target, SPELL_RESIZER_CHANNELING); // channeling resizing til another action
                        player->GetScheduler().Schedule(Milliseconds(4000), [this, player](TaskContext context)
                            {
                                player->CastStop();
                                player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_HIT_EXPLODE); // explode + credit
                                player->GetSelectedUnit()->ToCreature()->ForcedDespawn(1000, 15s); // despawn old mob
                            });
                    }
            }
            if (random_casting_mechanic == 3)
            {
                if (Unit* target = player->GetSelectedUnit())
                    if (target->GetEntry() == 156716)
                    {
                        player->CastSpell(player->GetSelectedUnit(), SPELL_RESIZER_OVERCHARGE); // overcharged hit kills with electro balls + credit
                    }
            }
        }
        return true;
    }
};

//156595 npc_giant_boar
struct npc_giant_boar : public ScriptedAI
{
    npc_giant_boar(Creature* creature) : ScriptedAI(creature)
    {
        Vehicle* vehicle = me->GetVehicleKit();
    }

    void Reset() override
    {
        me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            // this first "if" will prevent players from cheating by using the huge pig again :) -Varjgard
            if ((player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_COMPLETE))
            {
                player->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                player->ExitVehicle();
                me->DespawnOrUnsummon();
            }
            else if (player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_INCOMPLETE)
            {
                me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                me->SetWalk(false);

                player->EnterVehicle(me);
                player->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                player->KilledMonsterCredit(156595); // ride the boar credit
            }
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        me->SetWalk(false);

        if (Player* player = summoner->ToPlayer())
        {
            player->EnterVehicle(me);
            player->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            player->KilledMonsterCredit(156595); // ride the boar credit

            //Pretty weird combination between vehicle and scene, anyway, this is used as damage
            //player->GetSceneMgr().PlaySceneByPackageId(SCENE_SKELETON_ARMY_DARKMAUL_PLAINS);

        }
    }
};

//263018
class spell_giant_boar_trample : public SpellScript
{
    PrepareSpellScript(spell_giant_boar_trample);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* boar = GetCaster())
            if (Vehicle* vehicle = boar->GetVehicleKit())
                if (Unit* caster = vehicle->GetPassenger(0))
                    if (Player* player = caster->ToPlayer())
                    {
                        if (player->GetQuestStatus(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR) == QUEST_STATUS_INCOMPLETE)
                        {
                            MoveForward(boar, 8.0f);

                            std::list<Creature*> spellTargetsCreatures;
                            player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 157091, 8.0f);
                            player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 156532, 8.0f);
                            for (auto& target : spellTargetsCreatures)
                            {
                                if (target->GetEntry() == 157091)
                                    if (target->IsAlive())
                                        player->KilledMonsterCredit(157091);

                                target->KillSelf();
                            }
                        }

                        if (player->GetQuestObjectiveProgress(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR, 2) == 2)
                        {
                            if (Creature* garrick = player->FindNearestCreature(156280, 5.0f, true))
                                garrick->AI()->Talk(7);
                            player->GetScheduler().Schedule(Milliseconds(4500), [this, player](TaskContext context)
                                {
                                    if (Creature* henry = player->FindNearestCreature(156799, 100.0f, true))
                                        henry->AI()->Talk(0);
                                });
                        }

                        if (player->GetQuestObjectiveProgress(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR, 2) == 8)
                        {
                            player->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                            player->ExitVehicle();
                            player->GetScheduler().Schedule(Milliseconds(2000), [this, player](TaskContext context)
                                {
                                    if (Creature* garrick = player->FindNearestCreature(156280, 20.0f, true))
                                    {
                                        garrick->AI()->Talk(8, player);
                                        garrick->ExitVehicle();
                                    }
                                    if (Creature* boar = player->FindNearestCreature(156595, 20.0f, true))
                                        boar->CastSpell(boar, SPELL_RESIZER_HIT_TOSMALL);

                                }).Schedule(Milliseconds(7000), [this, player](TaskContext context)
                                    {
                                        if (Creature* garrick = player->FindNearestCreature(156280, 20.0f, true))
                                        {
                                            garrick->AI()->Talk(9);
                                        }
                                        if (Creature* boar = player->FindNearestCreature(156595, 20.0f, true))
                                            boar->DespawnOrUnsummon(5ms);
                                    });
                        }
                    }
    }

    void MoveForward(Unit* boar, float distance)
    {
        Position movePos;
        float ori = boar->GetOrientation();
        float x = boar->GetPositionX() + distance * cos(ori);
        float y = boar->GetPositionY() + distance * sin(ori);
        float z = boar->GetPositionZ();
        boar->GetNearPoint2D(boar, x, y, distance, ori);
        movePos = { x, y, z, ori };
        boar->GetMotionMaster()->MovePoint(0, movePos, false);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_giant_boar_trample::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//156799, familiar script will be also for horde version, which is 167128, scene 2905
struct npc_henry_garrick_156799 : public ScriptedAI
{
    npc_henry_garrick_156799(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR)
        {
            player->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            if (Creature* garrick = player->FindNearestCreature(156280, 50.0f, true))
            {
                garrick->ForcedDespawn(100);
            }

            player->NearTeleportTo(229.736f, -2296.816f, 80.894f, 1.03f, false);
            me->ForcedDespawn(100);
        }

    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_TALK_GARRICK_HENRY)
        {
            me->RemoveAura(305513);
            me->RemoveAura(325408);
            me->GetMotionMaster()->MovePoint(0, 230.795f, -2297.09f, 80.9792f, true, 1.356f);
            me->AddDelayedEvent(5000, [this]() -> void
                {
                    Talk(1);
                });
        }
    }
};

struct npc_henry_garrick_156859 : public EscortAI
{
    npc_henry_garrick_156859(Creature* creature) : EscortAI(creature) { Reset(); }
private:
    bool say_keela;

    void Reset() override
    {
        say_keela = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 30.0f))
                if (player->HasQuest(QUEST_THE_HARPY_PROBLEM))
                    if (!say_keela)
                    {
                        say_keela = true;
                        if (Creature* keela = me->FindNearestCreature(156860, 10.0f, true))
                            keela->AI()->Talk(0);
                    }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_THE_RESCUE_OF_MEREDY_HUNTSWELL) && player->HasQuest(QUEST_HARPY_CULLING) && player->HasQuest(QUEST_PURGE_THE_TOTEMS))
        {
            me->AI()->DoAction(ACTION_TALK_HENRY_KEELA);
        }

        if (player->HasQuest(QUEST_MESSAGE_TO_BASE))
        {
            Talk(2);
            player->GetScheduler().Schedule(Milliseconds(4500), [this](TaskContext context)
                {
                    Start(true);
                    if (Creature* keela = me->FindNearestCreature(156860, 10.0f, true))
                        keela->AI()->DoAction(ACTION_MESSAGET_TO_BASE_WP_START);
                    if (Creature* meredy = me->FindNearestCreature(156882, 10.0f, true))
                        meredy->AI()->DoAction(ACTION_MESSAGET_TO_BASE_WP_START);
                });

        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_THE_HARPY_PROBLEM)
        {
            Talk(0);
            say_keela = false;
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_TALK_HENRY_KEELA)
        {
            if (Creature* keela = me->FindNearestCreature(156860, 10.0f, true))
            {
                Talk(1);
                me->AddDelayedEvent(4000, [this, keela]() -> void
                    {
                        keela->AI()->Talk(1);
                    });
            }
        }

    }
};

// npc_kee_la_156860_Q55764
struct npc_kee_la_156860 : public EscortAI
{
    npc_kee_la_156860(Creature* creature) : EscortAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_THE_RESCUE_OF_MEREDY_HUNTSWELL) && player->HasQuest(QUEST_HARPY_CULLING) && player->HasQuest(QUEST_PURGE_THE_TOTEMS))
            if (Creature* henry = me->FindNearestCreature(156859, 10.0f, true))
            {
                henry->AI()->DoAction(ACTION_TALK_HENRY_KEELA);
            }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_MESSAGET_TO_BASE_WP_START)
        {
            me->AddDelayedEvent(1000, [this]() -> void
                {
                    Start(true);
                });
        }
    }
};

// npc_meredy_huntswell_156882_Q55882
struct npc_meredy_huntswell_156882 : public EscortAI
{
    npc_meredy_huntswell_156882(Creature* creature) : EscortAI(creature) { }

    void DoAction(int32 action) override
    {
        if (action == ACTION_MESSAGET_TO_BASE_WP_START)
        {
            me->AddDelayedEvent(2000, [this]() -> void
                {
                    Start(true);
                });

        }
    }
};


//153211
struct npc_meredy_huntswell_153211 : public ScriptedAI
{
    npc_meredy_huntswell_153211(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    uint32 bloodbeak = 153964;
    uint32 harpy_ambusher = 155192;
    uint32 hunting_worg = 169162;

    void Reset() override
    {
        me->SetHover(true);
        me->SetHoverHeight(4);
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (player->GetQuestStatus(QUEST_THE_RESCUE_OF_MEREDY_HUNTSWELL) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GossipOptionNpc::None, "I'll fight the harpies that come. Can you use your magic to break free?", 24887, GOSSIP_ACTION_INFO_DEF + 0);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 0:
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            Talk(0);
            FightingBloodBeak(player);
            break;
        }
        return true;
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_THE_RESCUE_OF_MEREDY_HUNTSWELL)
        {
            me->RemoveAura(305513);
            if (me->IsHovering())
            {
                me->SetHoverHeight(1);
                me->SetHover(false);
            }
            me->GetMotionMaster()->MovePoint(1, 495.411f, -2354.56f, 160.3297f, true);
            Talk(1);
            me->ForcedDespawn(5000);
            if (Creature* keela = me->FindNearestCreature(155199, 30.0f, true))
            {
                keela->GetMotionMaster()->MovePoint(0, 493.148f, -2378.73f, 157.097f, true);
                keela->ForcedDespawn(3500);
            }
            me->AddDelayedEvent(2000, [this]() -> void
                {
                    if (Creature* henry = me->FindNearestCreature(155197, 30.0f, true))
                    {
                        henry->GetMotionMaster()->MovePoint(0, 493.148f, -2378.73f, 157.097f, true);
                        henry->ForcedDespawn(3500);
                    }
                });
        }
    }

    void FightingBloodBeak(Player* player)
    {
        me->AddDelayedEvent(5000, [this, player]() -> void
            {
                TempSummon* worg1 = player->SummonCreature(hunting_worg, Position(477.835f, -2381.51f, 156.771f, 1.1187f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                worg1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy1 = player->SummonCreature(harpy_ambusher, Position(500.832f, -2331.03f, 164.318f, 4.25644f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                harpy1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(20000, [this, player]() -> void
            {
                TempSummon* worg2 = player->SummonCreature(hunting_worg, Position(472.838f, -2351.12f, 162.377f, 5.558f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                worg2->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy2 = player->SummonCreature(harpy_ambusher, Position(492.376f, -2397.87f, 154.462f, 1.52499f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                harpy2->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(30000, [this, player]() -> void
            {
                if (TempSummon* keela = player->SummonCreature(155199, Position(493.148f, -2378.73f, 157.097f, 4.6364f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U))
                {
                    keela->GetMotionMaster()->MovePoint(0, 498.74f, -2359.76f, 159.924f, true);
                    keela->AI()->Talk(0);
                    keela->SetOwnerGUID(player->GetGUID());                   //set player onwer
                    if (int32 playerFaction = player->GetFaction())
                        keela->SetFaction(playerFaction);
                }
            });
        me->AddDelayedEvent(36000, [this, player]() -> void
            {
                if (TempSummon* henry = player->SummonCreature(155197, Position(493.148f, -2378.73f, 157.097f, 4.6364f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U))
                {
                    henry->GetMotionMaster()->MovePoint(0, 490.437f, -2359.71f, 159.975f, true);
                    henry->AI()->Talk(0);
                    henry->SetOwnerGUID(player->GetGUID());                   //set player onwer
                    if (int32 playerFaction = player->GetFaction())
                        henry->SetFaction(playerFaction);
                }
            });

        me->AddDelayedEvent(42000, [this, player]() -> void
            {
                TempSummon* worg3 = player->SummonCreature(hunting_worg, Position(477.835f, -2381.51f, 156.771f, 1.1187f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                worg3->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy3 = player->SummonCreature(harpy_ambusher, Position(500.832f, -2331.03f, 164.318f, 4.25644f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                harpy3->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(55000, [this, player]() -> void
            {
                TempSummon* worg4 = player->SummonCreature(hunting_worg, Position(472.838f, -2351.12f, 162.377f, 5.558f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                worg4->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                TempSummon* harpy4 = player->SummonCreature(harpy_ambusher, Position(492.376f, -2397.87f, 154.462f, 1.52499f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                harpy4->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
        me->AddDelayedEvent(70000, [this, player]() -> void
            {
                if (Creature* keela = me->FindNearestCreature(155199, 30.0f, true))
                    keela->AI()->Talk(1);
            });
        me->AddDelayedEvent(76000, [this, player]() -> void
            {
                if (Creature* henry = me->FindNearestCreature(155197, 30.0f, true))
                    henry->AI()->Talk(1);
            });
        me->AddDelayedEvent(81000, [this, player]() -> void
            {
                TempSummon* bloodbeak1 = player->SummonCreature(bloodbeak, Position(491.975f, -2414.46f, 162.734f, 1.502485f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                bloodbeak1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
            });
    }
};

//153964
struct npc_Bloodbeak_153964 : public BossAI
{
    npc_Bloodbeak_153964(Creature* creature) : BossAI(creature, DATA_BLOODBEAK) { }

private:
    ObjectGuid   m_playerGUID;
    EventMap _events;

    void Initialize()
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void Reset() override
    {
        BossAI::Reset();
        ObjectGuid   m_playerGUID;
    }

    void JustEngagedWith(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            m_playerGUID = player->GetGUID();
        _events.ScheduleEvent(SPELL_SWOOPING_LUNGE, 3s);
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();

        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            player->KilledMonsterCredit(153964);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case SPELL_SWOOPING_LUNGE:
                DoCastVictim(SPELL_SWOOPING_LUNGE, false);
                _events.Repeat(15s);
                break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_captain_garrick_156807 : public EscortAI
{
    npc_captain_garrick_156807(Creature* creature) : EscortAI(creature) { Reset(); }
private:
    bool say_henry;
    ObjectGuid   m_henryGUID;
    ObjectGuid   m_raliaGUID;
    ObjectGuid   m_meredyGUID;

    void Reset() override
    {
        say_henry = false;
        m_henryGUID = ObjectGuid::Empty;
        m_raliaGUID = ObjectGuid::Empty;
        m_meredyGUID = ObjectGuid::Empty;
        me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_TO_DARKMAUL_CITADEL)
        {
            Talk(1);
            Start(true);
            if (Creature* henry = me->FindNearestCreature(156887, 20.0f, true))
            {
                m_henryGUID = henry->GetGUID();
                henry->SetWalk(false);
                henry->GetMotionMaster()->MoveFollow(me, -2.0f, henry->GetFollowAngle());
            }
            if (Creature* ralia = me->FindNearestCreature(156932, 20.0f, true))
            {
                m_raliaGUID = ralia->GetGUID();
                ralia->SetWalk(false);
                if (Creature* henry = ObjectAccessor::GetCreature(*me, m_henryGUID))
                ralia->GetMotionMaster()->MoveFollow(henry, -2.0f, ralia->GetFollowAngle());
            }
            if (Creature* meredy = me->FindNearestCreature(156886, 20.0f, true))
            {
                m_meredyGUID = meredy->GetGUID();
                meredy->SetWalk(false);
                if (Creature* ralia = ObjectAccessor::GetCreature(*me, m_raliaGUID))
                meredy->GetMotionMaster()->MoveFollow(ralia, -2.0f, meredy->GetFollowAngle());
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 30.0f))
                if (player->HasQuest(QUEST_MESSAGE_TO_BASE))
                    if (!say_henry)
                    {
                        say_henry = true;
                        if (Creature* henry = me->FindNearestCreature(156887, 15.0f, true))
                            henry->AI()->Talk(0, player);
                        me->AddDelayedEvent(5000, [this]() -> void
                            {
                                Talk(0);
                            });
                    }
    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 13:
            if (Creature* henry = ObjectAccessor::GetCreature(*me, m_henryGUID))
                henry->DespawnOrUnsummon(1s, 30s);
            if (Creature* ralia = ObjectAccessor::GetCreature(*me, m_raliaGUID))
                ralia->DespawnOrUnsummon(1s, 30s);
            if (Creature* meredy = ObjectAccessor::GetCreature(*me, m_meredyGUID))
                meredy->DespawnOrUnsummon(1s, 30s);
            me->DespawnOrUnsummon(1s, 30s);
            break;
        default:
            break;
        }
    }


    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_MESSAGE_TO_BASE)
        {
            say_henry = false;
        }

        if (quest->GetQuestId() == QUEST_WHO_LURKS_IN_THE_PIT)
        {
            if (Creature* bjorn = me->FindNearestCreature(156891, 30.0f, true))
                if (Creature* alaria = me->FindNearestCreature(156803, 30.0f, true))
                    if (Creature* ralia = me->FindNearestCreature(156932, 30.0f, true))
                    {
                        bjorn->DespawnOrUnsummon(1s, 30s);
                        alaria->DespawnOrUnsummon(1s, 30s);
                        ralia->DespawnOrUnsummon(1s, 30s);
                    }
        }
    }
};

// npc_quartermaster_richter_156800_Q55194
struct npc_quartermaster_richter_156800 : public ScriptedAI
{
    npc_quartermaster_richter_156800(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 10.0f))
                if (player->HasQuest(QUEST_STOCKING_UP_ON_SUPPLIES))
                    if (player->HasItemCount(168100, 1, false))
                    {
                        player->ForceCompleteQuest(QUEST_STOCKING_UP_ON_SUPPLIES);
                    }

    }
};

// npc_Bjorn_Stouthands_154300_Q55965
struct npc_Bjorn_Stouthands_154300 : public EscortAI
{
    npc_Bjorn_Stouthands_154300(Creature* creature) : EscortAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_WESTWARD_BOUND)
        {
            Talk(0);
            if (auto* alaria = me->FindNearestCreature(156803, 20.0f, true))
                alaria->AI()->DoAction(ACTION_WESTWARD_BOUND_WP_START);
            Start(true);
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_WESTWARD_BOUND)
        {
            me->DespawnOrUnsummon(1s, 30s);
            if (auto* alaria = me->FindNearestCreature(156803, 10.0f, true))
                alaria->DespawnOrUnsummon(1s, 30s);
        }
    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 6:
            Talk(1);
            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            break;
        default:
            break;
        }
    }

};

// npc_alaria_156803_Q55882
struct npc_alaria_156803 : public EscortAI
{
    npc_alaria_156803(Creature* creature) : EscortAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_WHO_LURKS_IN_THE_PIT)
        {
            if (auto* alaria = me->FindNearestCreature(156891, 10.0f, true))
                alaria->AI()->Talk(0);
            me->AddDelayedEvent(4500, [this]() -> void
                {
                    Talk(0);
                });
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_WESTWARD_BOUND_WP_START)
        {
            me->AddDelayedEvent(1000, [this]() -> void
                {
                    Start(true);
                });

        }
    }
};

// npc_ralia_dreamchacer_156902_Q55882
struct npc_ralia_dreamchacer_156902 : public EscortAI
{
    npc_ralia_dreamchacer_156902(Creature* creature) : EscortAI(creature) { Reset(); }

    void Reset() override
    {
        me->SetHover(true);
        me->SetHoverHeight(5);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_WHOLURKS_IN_THEPIT_RALIA_SAY)
        {
            me->RemoveAura(305513);
            if (me->IsHovering())
            {
                me->SetHoverHeight(1);
                me->SetHover(false);
            }
            me->AddDelayedEvent(1500, [this]() -> void
                {
                    Talk(0);
                    me->GetMotionMaster()->MovePoint(0, 77.5401f, -2138.47f, -30.1296f);
                });
            me->AddDelayedEvent(5000, [this]() -> void
                {
                    me->DespawnOrUnsummon(1s, 60s);
                    me->SummonCreature(156929, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 10min, 0U);
                });

        }
    }
};

//156929 npc_ralia_dreamchacer_156929
struct npc_ralia_dreamchacer_156929 : public ScriptedAI
{
    npc_ralia_dreamchacer_156929(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
            if (player->HasQuest(QUEST_WHO_LURKS_IN_THE_PIT))
            {
                player->KilledMonsterCredit(156929);
                player->ForceCompleteQuest(QUEST_WHO_LURKS_IN_THE_PIT);
                me->ForcedDespawn(1000);
                player->NearTeleportTo(108.546f, -2271.868f, 97.172f, 5.64f, false);
            }
    }
};

// npc_ralia_dreamchacer_156932_q55639
struct npc_ralia_dreamchacer_156932 : public EscortAI
{
    npc_ralia_dreamchacer_156932(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    bool event55639;

    void Reset() override
    {
        event55639 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
                if (player->GetQuestStatus(QUEST_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_COMPLETE)
                    if (!event55639)
                    {
                        event55639 = true;
                        me->AddDelayedEvent(3000, [this]() -> void
                            {
                                if (auto* bjorn = me->FindNearestCreature(156891, 20.0f, true))
                                    bjorn->AI()->DoAction(ACTION_WHO_LURKS_IN_THE_PIT_WP_START);
                                Talk(0);
                                Start(true);
                            });
                    }
    }
};

// npc_Bjorn_Stouthands_156891_q55639
struct npc_Bjorn_Stouthands_156891 : public EscortAI
{
    npc_Bjorn_Stouthands_156891(Creature* creature) : EscortAI(creature) { }

private:
    ObjectGuid alaria_GUID;

    void Reset() override
    {
        alaria_GUID = ObjectGuid::Empty;
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_WHO_LURKS_IN_THE_PIT_WP_START)
        {
            if (auto* alaria = me->FindNearestCreature(156803, 20.0f, true))
            {
                alaria->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, me->GetFollowAngle());
                alaria_GUID = alaria->GetGUID();
            }
            Start(true);
        }
    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 1:
            Talk(3);
            break;
        case 3:
            if (Creature* alaria = ObjectAccessor::GetCreature(*me, alaria_GUID))
                alaria->AI()->Talk(1);
            break;
        case 4:
            Talk(4);
            break;
        case 5:
            if (Creature* alaria = ObjectAccessor::GetCreature(*me, alaria_GUID))
                alaria->AI()->Talk(2);
            break;
        case 7:
            if (Creature* alaria = ObjectAccessor::GetCreature(*me, alaria_GUID))
                alaria->GetMotionMaster()->MovePoint(0, 182.128f, -2291.25f, 81.987f);
            break;
        default:
            break;
        }
    }
};

//npc_meredy_huntswell_156943_q55981
struct npc_meredy_huntswell_156943 : public ScriptedAI
{
    npc_meredy_huntswell_156943(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    ObjectGuid   henryGUID;
    ObjectGuid   garrickGUID;
    ObjectGuid   meredyGUID;
    ObjectGuid   raliaGUID;
    TaskScheduler _scheduler;

    void Reset() override
    {
        henryGUID = ObjectGuid::Empty;
        garrickGUID = ObjectGuid::Empty;
        meredyGUID = ObjectGuid::Empty;
        raliaGUID = ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (!player->HasAura(298241) && player->HasQuest(QUEST_RIGHT_BENEATH_THEIR_EYES))
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready, transorm me into an ogre and I'll sneak into their citadel.", 24551, GOSSIP_ACTION_INFO_DEF + 0);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:
            if (Creature* henry = player->FindNearestCreature(156942, 20.0f, true))
            {
                Position henryposition = henry->GetPosition();
                if (Creature* henry2 = player->SummonCreature(156962, henryposition, TEMPSUMMON_TIMED_DESPAWN, 10min))
                    henryGUID = henry2->GetGUID();
                henry->DespawnOrUnsummon(1s, 60s);
            }
            if (Creature* garrick = player->FindNearestCreature(156941, 20.0f, true))
            {
                Position garrickposition = garrick->GetPosition();
                if (Creature* garrick2 = player->SummonCreature(156961, garrickposition, TEMPSUMMON_TIMED_DESPAWN, 10min))
                    garrickGUID = garrick2->GetGUID();
                garrick->DespawnOrUnsummon(1s, 60s);
            }
            if (Creature* ralia = player->FindNearestCreature(156944, 20.0f, true))
            {
                Position raliaposition = ralia->GetPosition();
                if (Creature* ralia2 = player->SummonCreature(156947, raliaposition, TEMPSUMMON_TIMED_DESPAWN, 10min))
                    raliaGUID = ralia2->GetGUID();
                ralia->DespawnOrUnsummon(1s, 60s);
            }

            Talk(0); // A willing test subject! Such a rare find.

            me->CastSpell(me, 313583); // ogre transformation herbert channeling

            _scheduler.Schedule(Milliseconds(3000), [this, player](TaskContext context)
                {
                    Creature* ralia = ObjectAccessor::GetCreature(*me, raliaGUID);
                    {
                        ralia->CastSpell(player, 313598); // ogre transformation crenna channeling
                    }

                    if (Creature* meredy = player->SummonCreature(156960, *me, TEMPSUMMON_TIMED_DESPAWN, 10min))
                        meredyGUID = meredy->GetGUID();
                    me->AddAura(65050, me);          // add invisiable
                    me->DespawnOrUnsummon(60s, 60s);

                }).Schedule(Milliseconds(8000), [this, player](TaskContext context)
                    {
                        player->CastSpell(player, 298241); // transforming to ogre
                        player->SetObjectScale(2.0f); // make it big like and ogre

                    }).Schedule(Milliseconds(13000), [this, player](TaskContext context)
                        {
                            // give the credit
                            player->KilledMonsterCredit(156943);
                            Creature* meredy = ObjectAccessor::GetCreature(*me, meredyGUID);
                            {
                                meredy->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                                // chaining
                                meredy->CastSpell(player, 329760); // left hand chain
                                meredy->GetMotionMaster()->MoveFollow(player, -4.0f, me->GetFollowAngle());
                                meredy->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                            }

                            Creature* henry = ObjectAccessor::GetCreature(*me, henryGUID);
                            {
                                henry->CastSpell(player, 329760); // left hand chain
                                henry->GetMotionMaster()->MoveFollow(player, 2.0f, henry->GetFollowAngle());
                                henry->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                            }

                            Creature* garrick = ObjectAccessor::GetCreature(*me, garrickGUID);
                            {
                                garrick->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                                garrick->CastSpell(player, 329760); // left hand chain
                                garrick->GetMotionMaster()->MoveFollow(player, -2.0f, garrick->GetFollowAngle());
                                garrick->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                                garrick->AI()->Talk(0, player);
                            }

                            Creature* ralia = ObjectAccessor::GetCreature(*me, raliaGUID);
                            {
                                ralia->CastSpell(player, 329760); // left hand chain
                                ralia->GetMotionMaster()->MoveFollow(player, 4.0f, ralia->GetFollowAngle());
                                ralia->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                            }
                        }).Schedule(Milliseconds(20000), [this, player](TaskContext context)
                            {
                                Creature* henry = ObjectAccessor::GetCreature(*me, henryGUID);
                                henry->AI()->Talk(0);
                            });

                        CloseGossipMenuFor(player);

                        break;
        }
        return true;
    }
};

// npc_captain_garrick_156941_Q55981
struct npc_captain_garrick_156941 : public EscortAI
{
    npc_captain_garrick_156941(Creature* creature) : EscortAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_RIGHT_BENEATH_THEIR_EYES)
        {
            player->RemoveAura(298241);
            player->SetObjectScale(1.0f);
        }

        if (quest->GetQuestId() == QUEST_CONTROLLING_THEIR_STONES)
        {
            Talk(0);
        }
    }

};

// npc_captain_garrick_156961_Q55981
struct npc_captain_garrick_156961 : public EscortAI
{
    npc_captain_garrick_156961(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    Unit* owner = nullptr;

    void IsSummonedBy(WorldObject* summonerWO) override
    {
        Unit* summoner = summonerWO->ToUnit();
        if (!summoner)
            return;
        owner = summoner;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!owner || !owner->IsInWorld() || !owner->IsAlive())
        {
            me->DespawnOrUnsummon();
            return;
        }
    }
};

// npc_henry_garrick_156962_Q55981
struct npc_henry_garrick_156962 : public EscortAI
{
    npc_henry_garrick_156962(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    Unit* owner = nullptr;

    void IsSummonedBy(WorldObject* summonerWO) override
    {
        Unit* summoner = summonerWO->ToUnit();
        if (!summoner)
            return;
        owner = summoner;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!owner || !owner->IsInWorld() || !owner->IsAlive())
        {

            me->DespawnOrUnsummon();
            return;
        }
    }
};

// npc_meredy_huntswell_156960_Q55981
struct npc_meredy_huntswell_156960 : public EscortAI
{
    npc_meredy_huntswell_156960(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    Unit* owner = nullptr;

    void IsSummonedBy(WorldObject* summonerWO) override
    {
        Unit* summoner = summonerWO->ToUnit();
        if (!summoner)
            return;
        owner = summoner;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!owner || !owner->IsInWorld() || !owner->IsAlive())
        {
            me->DespawnOrUnsummon();
            return;
        }
    }
};

// npc_ralia_dreamchaser_156947_Q55981
struct npc_ralia_dreamchaser_156947 : public EscortAI
{
    npc_ralia_dreamchaser_156947(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    Unit* owner = nullptr;

    void IsSummonedBy(WorldObject* summonerWO) override
    {
        Unit* summoner = summonerWO->ToUnit();
        if (!summoner)
            return;
        owner = summoner;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!owner || !owner->IsInWorld() || !owner->IsAlive())
        {
            me->DespawnOrUnsummon();
            return;
        }
    }
};

// npc_captain_kelra_156954_Q55981
struct npc_captain_kelra_156954 : public ScriptedAI
{
    npc_captain_kelra_156954(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override
    {
        me->SetHover(true);
        me->SetHoverHeight(4);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 60.0f))
            {
                if (player->HasQuest(QUEST_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 16893) // reach citadel entrance
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->RemoveAura(298241);
                                    player->SetObjectScale(1.0f);
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                    if (Creature* henry = player->FindNearestCreature(156962, 40.0f, true))
                                    {
                                        me->AddDelayedEvent(3000, [henry, player]()                    // time_delay
                                            {
                                                henry->AI()->Talk(1);
                                            });
                                    }
                                    if (Creature* garrick = player->FindNearestCreature(156961, 40.0f, true))
                                    {
                                        me->AddDelayedEvent(8000, [garrick, player]()                    // time_delay
                                            {
                                                garrick->AI()->Talk(3);

                                            });
                                        me->AddDelayedEvent(13000, [garrick, player]()                    // time_delay
                                            {
                                                player->ForceCompleteQuest(QUEST_RIGHT_BENEATH_THEIR_EYES);
                                                if (Creature* henry = player->FindNearestCreature(156962, 30.0f, true))
                                                    henry->ForcedDespawn(1000);
                                                if (Creature* meredy = player->FindNearestCreature(156960, 30.0f, true))
                                                    meredy->ForcedDespawn(1000);
                                                if (Creature* ralia = player->FindNearestCreature(156947, 30.0f, true))
                                                    ralia->ForcedDespawn(1000);
                                                garrick->ForcedDespawn(1000);
                                            });
                                    }
                                }
                            }
                }
            }
        }
    }
};

// npc_austin_hucksworth_149915
struct npc_austin_hucksworth_149915 : public ScriptedAI
{
    npc_austin_hucksworth_149915(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_FORBIDDEN_QUILBOAR_NECROMANCY)
        {
            if (Creature* npc = player->FindNearestCreature(154301, 50.0f, true))
            {
                npc->SetRespawnDelay(30); // 30 sec for respawning
                npc->DespawnOrUnsummon(1s); // despawn immediately
            }
        }
    }

};

//go_harpy_totem_327146
struct go_harpy_totem_327146 : public GameObjectAI
{
    go_harpy_totem_327146(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasQuest(QUEST_PURGE_THE_TOTEMS))
        {
            me->SummonCreature(167993, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 5s);
            if (Creature* harpy_totem_npc = me->FindNearestCreature(167993, 5.0f, true))
                me->CastSpell(harpy_totem_npc, SPELL_BURNING_TOP);
            player->KillCreditGO(327146);
            me->DestroyForPlayer(player);
        }
        return true;
    }
};
void AddSC_zone_exiles_reach_alliance()
{
    new exiles_reach_a();
    RegisterCreatureAI(npc_private_cole_160664);
    RegisterCreatureAI(npc_private_cole_dummy);
    RegisterCreatureAI(npc_captain_garrick_156280);
    RegisterCreatureAI(npc_alliance_gryphon_154155);
    RegisterCreatureAI(npc_captain_garrick_156626);
    RegisterCreatureAI(npc_cole_149917);
    RegisterCreatureAI(npc_richter_156622);
    RegisterCreatureAI(npc_Bjorn_Stouthands_156609);
    RegisterCreatureAI(npc_Austin_Huxworth_156610);
    RegisterCreatureAI(npc_kee_la_156612);
    RegisterCreatureAI(npc_captain_garrick_156651);
    RegisterCreatureAI(npc_alaria_156607);
    RegisterCreatureAI(npc_alaria_175031);
    RegisterCreatureAI(npc_austin_huxworth_154327);
    RegisterCreatureAI(npc_geolord_grekog_151091);
    RegisterCreatureAI(npc_lindie_springstock_149899);
    RegisterCreatureAI(npc_lindie_springstock_156749);
    RegisterCreatureAI(npc_scout_o_matic_5000);
    RegisterItemScript(item_resizer_v901);
    RegisterCreatureAI(npc_giant_boar);
    RegisterSpellScript(spell_giant_boar_trample);
    RegisterCreatureAI(npc_henry_garrick_156799);
    RegisterCreatureAI(npc_henry_garrick_156859);
    RegisterCreatureAI(npc_kee_la_156860);
    RegisterCreatureAI(npc_meredy_huntswell_156882);
    RegisterCreatureAI(npc_meredy_huntswell_153211);
    RegisterCreatureAI(npc_Bloodbeak_153964);
    RegisterCreatureAI(npc_captain_garrick_156807);
    RegisterCreatureAI(npc_quartermaster_richter_156800);
    RegisterCreatureAI(npc_Bjorn_Stouthands_154300);
    RegisterCreatureAI(npc_alaria_156803);
    RegisterCreatureAI(npc_ralia_dreamchacer_156902);
    RegisterCreatureAI(npc_ralia_dreamchacer_156929);
    RegisterCreatureAI(npc_ralia_dreamchacer_156932);
    RegisterCreatureAI(npc_Bjorn_Stouthands_156891);
    RegisterCreatureAI(npc_meredy_huntswell_156943);
    RegisterCreatureAI(npc_captain_garrick_156941);
    RegisterCreatureAI(npc_captain_garrick_156961);
    RegisterCreatureAI(npc_henry_garrick_156962);
    RegisterCreatureAI(npc_meredy_huntswell_156960);
    RegisterCreatureAI(npc_ralia_dreamchaser_156947);
    RegisterCreatureAI(npc_captain_kelra_156954);
    RegisterCreatureAI(npc_austin_hucksworth_149915);
    RegisterGameObjectAI(go_harpy_totem_327146);
}
