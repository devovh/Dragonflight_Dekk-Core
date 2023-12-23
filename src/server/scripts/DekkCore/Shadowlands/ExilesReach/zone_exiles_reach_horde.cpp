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
#include "zone_exiles_reach.h"

/*
TODO:

The Rescue of herbert quest:
- bloodbeak script with threat to player + spells
- 167182 herbert hoverheight
- helper shuja and bo power decrease
- herbert shouldn't dissappear on quest reward
- walk with nearly mob fight herbert shuja and bo after quest is rewarded

go_thick_cocoon_339568_350796 script rescued fellas

QUEST_H_WHO_LURKS_IN_THE_PIT need fix crenna hoverheight

killclaw spell cast scripting

adddelayedevent moving need to be replaced to waypoints

fix ritual beam scene bugs

ogre spell cast scripting + remove sit emote state and sleep aura etc

NEED REMOVE the REMOVE THIS part
*/

class exiles_reach_h : public PlayerScript
{
public:
    exiles_reach_h() : PlayerScript("exiles_reach_h") { }

    void OnLogin(Player* player, bool firstLogin) override
    {
        // Set zone as sanctuary
        if (player->GetMapId() == MAP_NPE)
        {
            player->SetPvpFlag(UNIT_BYTE2_FLAG_SANCTUARY);

            if (player->HasAura(298241) && player->GetQuestStatus(QUEST_H_RIGHT_BENEATH_THEIR_EYES) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* grimaxe = GetClosestCreatureWithEntry(player, 167596, 25.0f))
                    grimaxe->ForcedDespawn(3000);
                if (Creature* shuja = GetClosestCreatureWithEntry(player, 167597, 25.0f))
                    shuja->ForcedDespawn(3000);
                if (Creature* herbert = GetClosestCreatureWithEntry(player, 167598, 25.0f))
                    herbert->ForcedDespawn(3000);
                if (Creature* crenna = GetClosestCreatureWithEntry(player, 167599, 25.0f))
                    crenna->ForcedDespawn(3000);
            }

            /*
            // it makes ui not show cursor not show sometimes -- bugged
            if (player->GetQuestStatus(QUEST_H_THE_REDEATHER) == QUEST_STATUS_REWARDED && !player->HasAura(SPELL_RITUAL_SCENE_HARPY_BEAM))
            {
                if (player->HasAura(SPELL_RITUAL_SCENE_HARPY_BEAM)) player->RemoveAura(SPELL_RITUAL_SCENE_HARPY_BEAM);
                if (player->HasAura(SPELL_RITUAL_SCENE_HRUN_BEAM)) player->RemoveAura(SPELL_RITUAL_SCENE_HRUN_BEAM);
                if (player->HasAura(SPELL_RITUAL_SCENE_MAIN_BEAM)) player->RemoveAura(SPELL_RITUAL_SCENE_MAIN_BEAM);
                if (player->HasAura(SPELL_RITUAL_SCENE_OGRE_CITADEL)) player->RemoveAura(SPELL_RITUAL_SCENE_OGRE_CITADEL);
                player->CastSpell(player, SPELL_RITUAL_SCENE_HARPY_BEAM, true);
                player->CastSpell(player, SPELL_RITUAL_SCENE_HRUN_BEAM, true);
                player->CastSpell(player, SPELL_RITUAL_SCENE_MAIN_BEAM, true);
                player->CastSpell(player, SPELL_RITUAL_SCENE_OGRE_CITADEL, true);
                player->GetSceneMgr().CancelScene(SCENE_RITUAL_MAIN_BEAM, false);
                player->GetSceneMgr().CancelScene(SCENE_RITUAL_HARPY_BEAM, false);
                player->GetSceneMgr().CancelScene(SCENE_RITUAL_HRUN_BEAM, false);
                player->GetSceneMgr().CancelScene(SCENE_RITUAL_OGRE_CITADEL, false);
            }
            */
        }
    }

    // after the ship crash movie play the tiny murloc scene
    void OnMovieComplete(Player* player, uint32 movieId) override
    {
        if (movieId == 931)
        {
            player->TeleportTo(2175, -462.722f, -2620.544f, 0.472f, 0.760f);
            player->SetEmoteState(EMOTE_STATE_DEAD);
            player->RemoveAura(325131); //big black screen - means eyes closed //  remove - means opening eyes
            player->GetSceneMgr().PlaySceneByPackageId(SCENE_ALI_HORDE_CRASHED_ON_ISLAND, SceneFlag::None); // when opening eyes seeing murlocs xD
        }
    }

    void OnSceneComplete(Player* player, uint32 sceneInstanceID) override
    {
        // waking up from EMOTE_STATE_DEAD when player reach the shore
        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == SCENE_ALI_HORDE_CRASHED_ON_ISLAND)
        {
            player->SetEmoteState(EMOTE_STATE_STAND);
        }

        /*
        // we activate the necrotic ritual tunnel scene auras after torgok died scene completed
        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == SCENE_REUNION)
        {
            player->CastSpell(player, SPELL_RITUAL_SCENE_HARPY_BEAM, true);
            player->CastSpell(player, SPELL_RITUAL_SCENE_HRUN_BEAM, true);
            player->CastSpell(player, SPELL_RITUAL_SCENE_MAIN_BEAM, true);
            player->CastSpell(player, SPELL_RITUAL_SCENE_OGRE_CITADEL, true);
        }
        */

        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == 2491) //  QUEST_H_WHO_LURKS_IN_THE_PIT = 59949
        {
            player->NearTeleportTo(108.546f, -2271.868f, 97.172f, 5.64f, false);
        }

        if (player->GetSceneMgr().GetSceneTemplateFromInstanceId(sceneInstanceID)->SceneId == 2379) // QUEST_WHO_LURKS_IN_THE_PIT = 55639
        {
            player->NearTeleportTo(108.546f, -2271.868f, 97.172f, 5.64f, false);
            player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                {
                    player->ForceCompleteQuest(QUEST_WHO_LURKS_IN_THE_PIT);
                });
        }
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/) override
    {
        switch (newArea = player->GetAreaId())
        {
        case AREA_NORTH_SEA:
            if (Creature* npc = player->FindNearestCreature(NPC_GARRIC, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(5000), [npc,player] (TaskContext context)
                {
                    npc->AI()->Talk(GARRIC_TXT0,player);
                });
            }
            break;
        case AREA_NORTH_SEA_H:
            if (Creature* npc = player->FindNearestCreature(NPC_GRIMAXE, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(5000), [npc, player](TaskContext context)
                    {
                        npc->AI()->Talk(0, player);
                    });
            }
            break;
        case AREA_MURLOC_HIDEAWAY:
            if (Creature* npc = player->FindNearestCreature(166782, 20.0f, true))
                if (player->GetQuestStatus(QUEST_H_MURLOCK_MANIA) == QUEST_STATUS_NONE)
                {
                    player->GetScheduler().Schedule(Milliseconds(3000), [npc, player](TaskContext context)
                        {
                            npc->AI()->Talk(0, player);
                        });
                }
            break;
        default:
            break;
        }
    }

    // for dancing in the ogre cooking area
    void OnTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, ObjectGuid guid) override
    {
        /* this one is for getting correct emote id lol
        std::ostringstream checkemote;
        checkemote << "textEmote: " << textEmote << " emoteNum: " << emoteNum;
        player->Say(checkemote.str(), LANG_UNIVERSAL);
        */
        if (player->GetMapId() == MAP_NPE)
            if (emoteNum == 34) // dance
                if (player->HasQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES) || player->HasQuest(QUEST_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 85148) // dancing in the cooking area
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                    if (Creature* grunk = player->FindNearestCreature(154145, 40.0f, true))
                                    {
                                        grunk->AddDelayedEvent(2000, [grunk, player]()        // time_delay
                                            {
                                                grunk->Talk("This one got some moves!", CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, 25, player);
                                                grunk->SetEmoteState(EMOTE_STATE_DANCE);
                                                if (Creature* jugnug = grunk->FindNearestCreature(154147, 40.0f, true))
                                                    jugnug->SetEmoteState(EMOTE_STATE_DANCE);
                                                if (Creature* wug = grunk->FindNearestCreature(154146, 40.0f, true))
                                                    wug->SetEmoteState(EMOTE_STATE_DANCE);
                                            });
                                        grunk->AddDelayedEvent(8000, [grunk, player]()        // time_delay
                                            {
                                                grunk->GetMotionMaster()->MovePoint(1, 545.088f, -2047.535f, 151.438f, true);
                                                grunk->DespawnOrUnsummon(5s, 60s);

                                                if (Creature* jugnug = grunk->FindNearestCreature(154147, 40.0f, true))
                                                {
                                                    jugnug->GetMotionMaster()->MovePoint(1, 545.088f, -2047.535f, 151.438f, true);
                                                    jugnug->DespawnOrUnsummon(5s, 60s);
                                                }
                                                if (Creature* wug = grunk->FindNearestCreature(154146, 40.0f, true))
                                                {
                                                    wug->GetMotionMaster()->MovePoint(1, 633.747f, -2098.588f, 158.566f, true);
                                                    wug->DespawnOrUnsummon(5s, 60s);
                                                }
                                                if (Creature* breka = grunk->FindNearestCreature(167596, 40.0f, true))
                                                {
                                                    breka->Talk("I said act like an ogre, not dance like one!", CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, 25, player);
                                                    breka->Say(196532);
                                                    breka->PlayDirectSound(157178, player, 196532);
                                                }
                                            });
                                    }
                                }
                            }

                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 80052) // dancing in the cooking area
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                    if (Creature* grunk = player->FindNearestCreature(154145, 40.0f, true))
                                    {
                                        grunk->AddDelayedEvent(2000, [grunk, player]()        // time_delay
                                            {
                                                grunk->Talk("This one got some moves!", CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, 25, player);
                                                grunk->SetEmoteState(EMOTE_STATE_DANCE);
                                                if (Creature* jugnug = grunk->FindNearestCreature(154147, 40.0f, true))
                                                    jugnug->SetEmoteState(EMOTE_STATE_DANCE);
                                                if (Creature* wug = grunk->FindNearestCreature(154146, 40.0f, true))
                                                    wug->SetEmoteState(EMOTE_STATE_DANCE);
                                            });
                                        grunk->AddDelayedEvent(8000, [grunk, player]()        // time_delay
                                            {
                                                grunk->GetMotionMaster()->MovePoint(1, 545.088f, -2047.535f, 151.438f, true);
                                                grunk->DespawnOrUnsummon(5s, 60s);

                                                if (Creature* jugnug = grunk->FindNearestCreature(154147, 40.0f, true))
                                                {
                                                    jugnug->GetMotionMaster()->MovePoint(1, 545.088f, -2047.535f, 151.438f, true);
                                                    jugnug->DespawnOrUnsummon(5s, 60s);
                                                }
                                                if (Creature* wug = grunk->FindNearestCreature(154146, 40.0f, true))
                                                {
                                                    wug->GetMotionMaster()->MovePoint(1, 633.747f, -2098.588f, 158.566f, true);
                                                    wug->DespawnOrUnsummon(5s, 60s);
                                                }
                                                if (Creature* garrick = grunk->FindNearestCreature(156961, 70.0f, true))
                                                {
                                                    garrick->AI()->Talk(2, player);
                                                }
                                            });
                                    }
                                }
                            }
                }
    }
};

class zone_exiles_reach_h : public ZoneScript
{
public:
    zone_exiles_reach_h() : ZoneScript("zone_exiles_reach_h") { }

    void OnPlayerExitZone(Player* player) override
    {
        // Remove sanctuary flag when leaving exiles reach
        player->RemovePvpFlag(UNIT_BYTE2_FLAG_SANCTUARY);
        // removing ritual scene auras to unsee ritual effects very nice
        player->RemoveAura(SPELL_RITUAL_SCENE_HARPY_BEAM);
        player->RemoveAura(SPELL_RITUAL_SCENE_HRUN_BEAM);
        player->RemoveAura(SPELL_RITUAL_SCENE_MAIN_BEAM);
        player->RemoveAura(SPELL_RITUAL_SCENE_OGRE_CITADEL);
    }
};

//160737 npc_combat_dummy
struct npc_combat_dummy : public ScriptedAI
{
    npc_combat_dummy(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
        me->SetRegenerateHealth(false);
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            if (player->HasQuest(QUEST_WARMING_UP))
            {
                player->KilledMonsterCredit(174954);

                if (Creature* npc = player->FindNearestCreature(NPC_GARRIC, 20.0f, true))
                {
                    //npc->PlayDirectSound(152718, player); // TODO: find correct text and sound
                    me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_RAIN, 0.5f);
                    //npc->AI()->Talk(GARRIC_TXT2);
                    npc->AI()->Speak(184168,152718,player);
                }
            }

            if (player->HasQuest(QUEST_H_WARMING_UP))
            {
                player->KilledMonsterCredit(174954);

                if (Creature* npc = player->FindNearestCreature(NPC_GRIMAXE, 20.0f, true))
                {
                    me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_RAIN, 0.5f);
                    npc->AI()->Talk(3);
                }
            }
            me->ForcedDespawn(1000, 5s);
        }
    }
};

// warlord_breka_grimaxe_166573
struct warlord_breka_grimaxe_166573 : public ScriptedAI
{
    warlord_breka_grimaxe_166573(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_WARMING_UP)
        {
            Talk(0);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_WARMING_UP)
            Talk(1);

        if (quest->GetQuestId() == QUEST_H_BRACE_FOR_IMPACT)
        {
            Talk(4);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH, 0.5f);
            //player->GetSceneMgr().PlaySceneByPackageId(SCENE_HIDE_TRANSITION_TO_BEACH, SceneFlag::None);
            player->CastSpell(player, SPELL_H_SHIP_CRASH);
        }

    }
};

// warlord_breka_grimaxe_166782
class warlord_breka_grimaxe_166782 : public CreatureScript
{
public:
    warlord_breka_grimaxe_166782 () : CreatureScript("warlord_breka_grimaxe_166782") { }


    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*item*/) override
    {
        if (quest->GetQuestId() == QUEST_H_MURLOCK_MANIA)
        {
            creature->RemoveAura(SPELL_KNEEL);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new warlord_breka_grimaxe_166782AI(creature);
    }

    struct warlord_breka_grimaxe_166782AI : public EscortAI
    {
        warlord_breka_grimaxe_166782AI(Creature* creature) : EscortAI(creature) {}

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_H_MURLOCK_MANIA)
            {
                Talk(1);
            }

            if (quest->GetQuestId() == QUEST_H_EMERGENCY_FIRST_AID)
            {
                if (Creature* npc = me->FindNearestCreature(NPC_THROG, 5.0f, true))
                {
                    me->CastSpell(npc, SPELL_FIRST_AID);

                    player->GetScheduler().Schedule(Milliseconds(4000), [this] (TaskContext context)
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(1, GrimaxePositions[0]);
                        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    })
                    .Schedule(15s, [this] (TaskContext context)
                    {
                        if (Creature* npc = me->FindNearestCreature(NPC_JINHAKE, 5.0f, true))
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

            if (quest->GetQuestId() == QUEST_H_FINDING_THE_LOST_EXPEDITION)
            {
                Start(true);
                SetEscortPaused(true);
                player->GetScheduler().Schedule(2s, [this,player] (TaskContext /*context*/)
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
                if (Creature* npc = me->FindNearestCreature(166786, 5.0f, true))
                {
                    npc->GetMotionMaster()->MovePoint(1, BoPositions[1]);
                    npc->GetMotionMaster()->MovePoint(2, BoPositions[2]);
                    npc->DespawnOrUnsummon(5s, 30s);
                }

                if (Creature* npc = me->FindNearestCreature(166791, 5.0f, true))
                {
                    npc->GetMotionMaster()->MovePoint(1, MithdranRastrealbaPositions[1]);
                    npc->GetMotionMaster()->MovePoint(2, MithdranRastrealbaPositions[2]);
                    npc->DespawnOrUnsummon(5s, 30s);
                }

                if (Creature* npc = me->FindNearestCreature(166796, 5.0f, true))
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
                // despawn old version after 30 sec and 10 sec for respawning
                me->DespawnOrUnsummon(30s, 10s);
                if (Creature* npc = me->FindNearestCreature(166854, 10.0f, true))
                {
                    npc->AI()->Talk(2);
                    me->AddDelayedEvent(3000, [this]()
                        {
                            me->AI()->Talk(3);
                        });
                }
                break;
            default:
                break;
            }
        }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_H_MURLOCK_MANIA)
                me->RemoveAura(SPELL_KNEEL);
        }
    };
};


// warlord_breka_grimaxe_166906
struct warlord_breka_grimaxe_166906 : public ScriptedAI
{
    warlord_breka_grimaxe_166906(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        me->SetFullHealth();
        me->SetFaction(35); // 35 was their faction basically... need re-sniff -.-
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_COOKING_MEAT)

            if (Creature* wansa = me->FindNearestCreature(166854, 10.0f, true))
            {
                wansa->AI()->Talk(0);

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
            if (Creature* wansa = me->FindNearestCreature(166854, 20.0f, true))
                me->SetFacingToObject(wansa, true); //
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

// npc_Throg_166784
struct npc_Throg_166784 : public ScriptedAI
{
    npc_Throg_166784 (Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetWalk(true);

            caster->ToUnit()->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext /*task*/)
            {
                me->GetMotionMaster()->MovePoint(1, -389.445f, -2593.08f, 4.0625f);
                me->ForcedDespawn(15000);
            });
    }
};

// npc_Jinhake_16680
struct npc_Jinhake_16680 : public ScriptedAI
{
    npc_Jinhake_16680 (Creature* creature) : ScriptedAI(creature) { }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetWalk(true);

        caster->ToUnit()->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext /*task*/)
        {
            me->GetMotionMaster()->MovePoint(1, -389.445f, -2593.08f, 4.0625f);
            me->ForcedDespawn(10000);
        });
    }
};

// npc_Bo_166786
struct npc_Bo_166786 : public ScriptedAI
{
    npc_Bo_166786(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->SetEmoteState(EMOTE_STATE_SLEEP);
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_H_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
                Talk(0);
                //me->CastSpell(me, SPELL_FIRST_AID, true);
                me->SetStandState(UNIT_STAND_STATE_STAND);

                player->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext /*task*/)
                {
                        me->GetMotionMaster()->MovePoint(1, BoPositions[0]);
                });
        }
    }
};

// npc_Mithdran_Rastrealba_166791
struct npc_Mithdran_Rastrealba_166791 : public ScriptedAI
{
    npc_Mithdran_Rastrealba_166791(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->SetEmoteState(EMOTE_STATE_SLEEP);
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_FIRST_AID)
            return;

        Player* player = caster->ToPlayer();
        if (player && player->GetQuestStatus(QUEST_H_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
        {
            Talk(0);
            //me->CastSpell(me, SPELL_FIRST_AID, true);
            me->SetStandState(UNIT_STAND_STATE_STAND);

            player->GetScheduler().Schedule(Milliseconds(3000), [this] (TaskContext /*task*/)
            {
                me->GetMotionMaster()->MovePoint(1, MithdranRastrealbaPositions[0]);
            });
        }
    }
};

// npc_Lana_Jordan_166796
struct npc_Lana_Jordan_166796 : public ScriptedAI
{
     npc_Lana_Jordan_166796 (Creature* creature) : ScriptedAI(creature) { }

     void JustAppeared() override
     {
         me->SetEmoteState(EMOTE_STATE_SLEEP);
     }

     void SpellHit(WorldObject* caster, SpellInfo const* spell) override
     {
         if (spell->Id != SPELL_FIRST_AID)
             return;

         Player* player = caster->ToPlayer();
         if (player && player->GetQuestStatus(QUEST_H_EMERGENCY_FIRST_AID) != QUEST_STATUS_REWARDED)
         {
             Talk(0);
             //me->CastSpell(me, SPELL_FIRST_AID, true);
             me->SetStandState(UNIT_STAND_STATE_STAND);

             player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext /*task*/)
                 {
                     me->GetMotionMaster()->MovePoint(1, LanaJordanPositions[0]);
                 });
         }
     }
};

// 166583_grunt_throg
struct grunt_throg_166583 : public ScriptedAI
{
    grunt_throg_166583(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_STAND_YOUR_GROUND)
        {
            Talk(0);

            player->GetScheduler().Schedule(Milliseconds(5000), [this] (TaskContext context)
            {
                me->SetFaction(14);
                me->SetReactState(REACT_AGGRESSIVE);
                me->AI()->Talk(1);
            });
        }

        if (quest->GetQuestId() == QUEST_H_BRACE_FOR_IMPACT)
        {
            Talk(2);
            me->GetMap()->SetZoneWeather(10424, WEATHER_STATE_EXILE_REACH_HEAVY_RAIN, 0.5f);

            if (Creature* npc = player->FindNearestCreature(NPC_GRIMAXE, 20.0f, true))
            {
                player->GetScheduler().Schedule(Milliseconds(4000), [npc] (TaskContext context)
                {
                    npc->AI()->Talk(4);
                });
            }
        }

    }


void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if ((me->GetHealth() < me->CountPctFromMaxHealth(60)) && (me->GetHealth() >= me->CountPctFromMaxHealth(50)))
            {
                Talk(3);
            }
            if (me->HealthBelowPctDamaged(30, damage))
            {
                damage = 0;
                me->SetFullHealth();
                me->SetFaction(35);
                me->AttackStop();
                attacker->ToPlayer()->KilledMonsterCredit(155607, ObjectGuid::Empty);
                //me->SummonCreature(166827, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

                attacker->ToPlayer()->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext context)
                {
                    me->AI()->Talk(4);
                });
            }
        }
    }

    void JustDied(Unit* killer)
    {
        Player* player = killer->ToPlayer();
        if (player->HasQuest(QUEST_H_BRACE_FOR_IMPACT))
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_BRACE_FOR_IMPACT))
                for (QuestObjective const& obj : quest->GetObjectives())
                {
                    uint16 slot = player->FindQuestSlot(QUEST_H_BRACE_FOR_IMPACT);
                    // if breka got too much dmg but the quest is not completed breka got full hp just like on retail
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
                    }
                }
        }
    }
};


// npc_wonsa_166854
    struct npc_wonsa_166854 : public ScriptedAI
    {
        npc_wonsa_166854 (Creature* creature) : ScriptedAI(creature) { }

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
                    if (player->HasQuest(QUEST_H_FINDING_THE_LOST_EXPEDITION))
                    {
                        player->KilledMonsterCredit(166854, ObjectGuid::Empty);
                    }

                    if (player->HasItemCount(COOKED_MEAT, 2, false))
                    {
                        if (Creature* npc = me->FindNearestCreature(166906, 10.0f, true))
                        {
                            if (!say)
                            {
                                Talk(1);

                                player->GetScheduler().Schedule(5s, [npc] (TaskContext context)
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
            if (quest->GetQuestId() == QUEST_H_COOKING_MEAT)
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


    // npc_wonsa_175030
    struct npc_wonsa_175030 : public ScriptedAI
    {
        npc_wonsa_175030 (Creature* creature) : ScriptedAI(creature) { }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == (QUEST_H_NORTHBOUND))
            {
                if (Creature* npc = me->FindNearestCreature(166906, 10.0f, true))
                {
                    me->AddAura(SPELL_QUEST_INVISIBILITY, npc);
                    player->GetScheduler().Schedule(Milliseconds(60000), [npc](TaskContext context)
                        {
                            npc->RemoveAllAuras();
                        });
                }

                if (TempSummon* grimaxe = player->SummonCreature(166824, Position(-249.05f, -2492.35f, 17.957f, 0.466517f), TEMPSUMMON_TIMED_DESPAWN, 5min, 0U))
                {
                    grimaxe->AI()->Talk(0);
                    player->GetScheduler().Schedule(Milliseconds(10000), [this](TaskContext context)
                        {
                            me->AI()->Talk(0);
                        }).Schedule(Milliseconds(16000), [this, grimaxe](TaskContext context)
                            {
                                grimaxe->AI()->Talk(1);

                            }).Schedule(Milliseconds(20000), [this, grimaxe, player](TaskContext context)
                                {
                                    grimaxe->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                                    // running to mithdran
                                    grimaxe->AddDelayedEvent(3000, [grimaxe]() { grimaxe->GetMotionMaster()->MovePoint(1, -238.273f, -2467.617f, 15.990f, true); });
                                    grimaxe->AddDelayedEvent(7500, [grimaxe]() { grimaxe->GetMotionMaster()->MovePoint(1, -220.940f, -2470.998f, 16.520f, true); });
                                    grimaxe->AddDelayedEvent(10000, [grimaxe]() { grimaxe->GetMotionMaster()->MovePoint(1, -197.318f, -2510.768f, 21.758f, true); });
                                    grimaxe->AddDelayedEvent(17500, [grimaxe]() { grimaxe->GetMotionMaster()->MovePoint(1, -190.575f, -2555.820f, 27.304f, true); });
                                    grimaxe->AddDelayedEvent(25000, [grimaxe]() { grimaxe->GetMotionMaster()->MovePoint(1, -173.183f, -2584.720f, 33.041f, true); });
                                    grimaxe->AddDelayedEvent(30000, [grimaxe]() { grimaxe->GetMotionMaster()->MovePoint(1, -155.535f, -2596.575f, 36.171f, true); });
                                    grimaxe->AddDelayedEvent(33500, [grimaxe]() { grimaxe->GetMotionMaster()->MovePoint(1, -137.381f, -2625.058f, 44.287f, true); });
                                    grimaxe->AddDelayedEvent(43000, [grimaxe, player]()
                                        {
                                            grimaxe->GetMotionMaster()->MovePoint(1, -143.075f, -2640.596f, 48.804f, true);
                                        });
                                    grimaxe->AddDelayedEvent(48000, [grimaxe]() { grimaxe->SetFacingTo(6.2f, true); });
                                    grimaxe->AddDelayedEvent(50000, [grimaxe]() { grimaxe->SetStandState(UNIT_STAND_STATE_KNEEL); });

                                });
                }
            }
        }
    };

// npc_Mithdran_Dawntracker_166996
    struct  npc_Mithdran_Dawntracker_166996 : public ScriptedAI
    {
        npc_Mithdran_Dawntracker_166996 (Creature* creature) : ScriptedAI(creature) {}

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if ( player->HasQuest(QUEST_H_DOWN_WITH_THE_QUILBOAR) && player->HasQuest(QUEST_H_FORBIDDEN_QUILBOAR_NECROMANCY))
            {
                if (Creature* npc = me->FindNearestCreature(166997, 5.0f, true))
                {
                    npc->AI()->Talk(0);
                    player->GetScheduler().Schedule(Milliseconds(5500), [this] (TaskContext context)
                    {
                        me->AI()->Talk(0);

                    }).Schedule(Milliseconds(11000), [this,npc] (TaskContext context)
                    {
                        npc->AI()->Talk(1);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        npc->SetStandState(UNIT_STAND_STATE_STAND);
                        me->GetMotionMaster()->MovePoint(1, -128.411f, -2637.33f, 48.4554f, true);
                        npc->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, npc->GetFollowAngle());

                    }).Schedule(Milliseconds(14000), [this, npc] (TaskContext context)
                    {
                        me->GetMotionMaster()->MovePoint(2, -109.327f, -2646.74f, 52.5968f, true);

                    }).Schedule(Milliseconds(18000), [this, npc] (TaskContext context)
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
            if (quest->GetQuestId() == QUEST_H_NORTHBOUND)
            {
                Talk(1);
                if (Creature* npc = me->FindNearestCreature(166824, 20.0f, true))
                {
                    npc->DespawnOrUnsummon(1s, 30s);
                }
            }
        }
    };

//168410 item_first_aid_kit
class item_first_aid_kit : public ItemScript
{
public:
    item_first_aid_kit() : ItemScript("item_first_aid_kit") { }


    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        std::list<Creature*> spellTargetsList;
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 156609, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 156610, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 156612, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 166786, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 166791, 2.0f);
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 166796, 2.0f);
        for (auto& targets : spellTargetsList)
        {
            targets->RemoveUnitFlag3(UNIT_FLAG3_FAKE_DEAD);
            targets->RemoveUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
            targets->ClearUnitState(UNIT_STATE_CANNOT_TURN);

          switch (targets->GetEntry())
          {
            case 156609:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                        {
                            player->KilledMonsterCredit(156609);
                        });
                break;
            }
            case 156612:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(156612);
                    });
                break;
            }
            case 156610:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(156610);
                    });
                break;
            }
            case 166786:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(166786);
                    });
                break;
            }
            case 166791:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(166791);
                    });
                break;
            }
            case 166796:
            {
                player->CastSpell(targets, SPELL_FIRST_AID);
                player->AddDelayedEvent(1000, [player]() -> void
                    {
                        player->KilledMonsterCredit(166796);
                    });
                break;
            }
          }
        }
        return true;
    }
};


//go_campfire_339769
struct go_campfire_339769 : public GameObjectAI
{
    go_campfire_339769(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestObjectiveProgress(QUEST_COOKING_MEAT, 2) == 5 || player->GetQuestObjectiveProgress(QUEST_H_COOKING_MEAT, 2) == 5 )
        {
            if (player->HasQuest(QUEST_COOKING_MEAT) || player->HasQuest(QUEST_H_COOKING_MEAT))
                player->ForceCompleteQuest(QUEST_COOKING_MEAT);

            player->AddItem(174074, 1);

        }

        return true;

    }
};


//npc_Cork_Fizzlepop_167019
struct npc_Cork_Fizzlepop_167019 : public ScriptedAI
{
    npc_Cork_Fizzlepop_167019(Creature* c) : ScriptedAI(c) { Reset(); }
private:
    bool FIZZLEPOP_SAY1;
    bool FIZZLEPOP_SAY2;
    bool FIZZLEPOP_SAY3;

    void Reset()
    {
        FIZZLEPOP_SAY1 = false;
        FIZZLEPOP_SAY2 = false;
        FIZZLEPOP_SAY3 = false;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_CHOPPY_BOOSTER_MK5)
        {

            player->SummonCreature(167905, Position(106.282f, -2415.671f, 90.41f, 3.601f), TEMPSUMMON_MANUAL_DESPAWN);
            player->FindNearestCreature(167905, 10.0f, true)->SetHover(true);
            player->FindNearestCreature(167905, 10.0f, true)->SetHoverHeight(3);

            player->GetScheduler().Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                    me->SelectNearbyTarget(fake_choppy, 10.0f);
                    me->CastSpell(fake_choppy, SPELL_RESIZER_CHANNELING);

                }).Schedule(Milliseconds(5000), [this, player](TaskContext context)
                    {
                        Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                        fake_choppy->CastSpell(fake_choppy, SPELL_RESIZER_HIT_TOSMALL);
                        me->AI()->Talk(0);

                    }).Schedule(Milliseconds(11000), [this, player](TaskContext context)
                        {

                            Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                            fake_choppy->RemoveAura(SPELL_RESIZER_HIT_TOSMALL);
                            fake_choppy->CastSpell(fake_choppy, SPELL_INCREASE_SIZE);
                            me->AI()->Talk(1);

                        }).Schedule(Milliseconds(17000), [this, player](TaskContext context)
                            {
                                Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                                fake_choppy->RemoveAura(SPELL_INCREASE_SIZE);
                                me->AI()->Talk(2);

                            }).Schedule(Milliseconds(20000), [this](TaskContext context)
                                {
                                    Creature* fake_choppy = me->FindNearestCreature(167905, 10.0f, true);
                                    fake_choppy->DespawnOrUnsummon(2s);

                                }).Schedule(Milliseconds(23000), [this, player](TaskContext context)
                                    {
                                        player->SummonCreature(167027, Position(106.282f, -2415.671f, 90.41f, 3.601f), TEMPSUMMON_MANUAL_DESPAWN);
                                    });

        }

        if (quest->GetQuestId() == QUEST_H_RESIZING_THE_SITUATION)
        {
            FIZZLEPOP_SAY1 = false;
            FIZZLEPOP_SAY2 = false;
            FIZZLEPOP_SAY3 = false;
            player->CastSpell(player, SPELL_H_RESIZING_BACKPACK); // apply the magical resizer unit to the players back (bro i found this spell hhaha)
            me->AI()->Talk(3);
            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());
        }

        if (quest->GetQuestId() == QUEST_H_THE_REDEATHER)
        {
            me->AI()->Talk(4);
            me->SetWalk(false);
            me->GetMotionMaster()->MovePoint(1, 100.742f, -2417.84f, 90.3844f, true, 3.81584f);
            me->AddDelayedEvent(3000, [this, player]() -> void
                {
                    player->SummonCreature(167142, Position(103.419f, -2417.24f, 93.4075f, 0.55373f), TEMPSUMMON_TIMED_DESPAWN, 10min, 0);
                });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        // hide when old fizzlepop coming
        if (Creature* oldme = me->FindNearestCreature(167008, 100.0f))
            if (oldme->IsInDist(me, 100.0f))
                me->AddAura(SPELL_QUEST_INVISIBILITY, me);
        // show when old fizzlepop going to despawn
        if (Creature* oldme = me->FindNearestCreature(167008, 100.0f))
            if (oldme->IsInDist(me, 1.0f))
                me->RemoveAura(SPELL_QUEST_INVISIBILITY);
        // another fizzlepop due to backpack aura - removing
        if (Creature* fizzlepop_duplication = me->FindNearestCreature(167915, 100.0f))
            if (fizzlepop_duplication->IsInDist(me, 20.0f))
                fizzlepop_duplication->AddAura(SPELL_QUEST_INVISIBILITY, fizzlepop_duplication);

        if (Player* player = who->ToPlayer())
        {
           if (player->IsInDist(me, 10.0f))
          {
           if (player->HasQuest(QUEST_H_RESIZING_THE_SITUATION))
            {
                if (player->GetQuestObjectiveProgress(QUEST_H_RESIZING_THE_SITUATION, 0) == 1)
                {
                    if (!FIZZLEPOP_SAY1)
                    {
                        Talk(5);
                        FIZZLEPOP_SAY1 = true;
                    }
                }
                if (player->GetQuestObjectiveProgress(QUEST_H_RESIZING_THE_SITUATION, 0) == 2)
                {
                    if (!FIZZLEPOP_SAY2)
                    {
                        Talk(6);
                        FIZZLEPOP_SAY2 = true;
                    }
                }
                if (player->GetQuestObjectiveProgress(QUEST_H_RESIZING_THE_SITUATION, 0) == 3)
                {
                    if (!FIZZLEPOP_SAY3)
                    {
                        Talk(7);
                        FIZZLEPOP_SAY3 = true;
                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(1, 100.742f, -2417.84f, 90.3844f, true, 3.81584f);
                    }
                }
              }

           if (player->GetQuestStatus(QUEST_H_RESIZING_THE_SITUATION) == QUEST_STATUS_NONE)
           {
               me->GetMotionMaster()->MoveTargetedHome();
               player->RemoveAura(SPELL_H_RESIZING_BACKPACK);
           }

           if (player->GetQuestStatus(QUEST_H_RESIZING_THE_SITUATION) == QUEST_STATUS_COMPLETE)
           {
               me->GetMotionMaster()->MoveTargetedHome();
               player->RemoveAura(SPELL_H_RESIZING_BACKPACK);
           }
          }

        }
     }
};


// npc_Choppy_Booster_167027 (for observe zombies)
struct npc_Choppy_Booster_167027: public VehicleAI
{
    npc_Choppy_Booster_167027(Creature* creature) : VehicleAI(creature)
    {
        me->SetFlying(true);
        me->SetCanFly(true);
        me->SetHover(true);
        me->SetHoverHeight(10.f);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_H_THE_CHOPPY_BOOSTER_MK5))
            {
                summoner->ToUnit()->EnterVehicle(me);
                me->CastSpell(summoner, SPELL_ROPE);
                me->GetMotionMaster()->MovePoint(1, 264.0f, -2310.0f, 117.0f, true);
                me->AddDelayedEvent(2000, [this, player]() -> void
                    {
                        player->PlayConversation(14517);
                        player->AddAura(SPELL_TARGET_INVISIBILITY, player);
                    });
                me->AddDelayedEvent(25000, [this, player]() -> void
                    {
                        player->ExitVehicle();
                        player->CastSpell(player, SPELL_GORGROTH_SCENE);
                        player->EnterVehicle(me);
                    });
                me->AddDelayedEvent(50000, [this, player]() -> void
                    {
                        player->SetViewpoint(me, true);
                    });
                me->AddDelayedEvent(55000, [this, player]() -> void
                    {
                        me->GetMotionMaster()->MovePoint(2, 108.609f, -2413.905f, 95.530f, true);
                        player->PlayConversation(14520);
                    });
                me->AddDelayedEvent(83000, [this, player]() -> void
                    {
                        me->DespawnOrUnsummon(100ms);
                        player->SetViewpoint(me, false);
                        player->KilledMonsterCredit(167027);
                        player->RemoveAura(SPELL_TARGET_INVISIBILITY);
                    });
            }
        }

    }
};

// npc_Choppy_Booster_167142 (for killing zombies)
// update to the new version: UPDATE `creature_template` SET `npcflag`='1', `spell1`='0', `spell6`='0', `VehicleId`='0' WHERE  `entry`=167142;
struct npc_Choppy_Booster_167142 : public ScriptedAI
{
    npc_Choppy_Booster_167142(Creature* creature) : ScriptedAI(creature)
    {
        me->SetHoverHeight(3);
        me->SetHover(true);
    }

    bool OnGossipHello(Player* player) override
    {
        CloseGossipMenuFor(player);
        if (player->HasQuest(QUEST_H_THE_REDEATHER))
        {
            player->CastSpell(player, 325368);
            player->KilledMonsterCredit(167142);
            // not the best way but feels like more blizzlike we giving credit old version can kill manual but that's not the same vehicle
            player->GetScheduler().Schedule(Milliseconds(7000), [player](TaskContext context)
                {
                    player->KilledMonsterCredit(165150);
                }).Schedule(Milliseconds(10000), [player](TaskContext context)
                    {
                        player->KilledMonsterCredit(165150);
                    }).Schedule(Milliseconds(12000), [player](TaskContext context)
                        {
                            player->KilledMonsterCredit(165150);
                        }).Schedule(Milliseconds(15000), [player](TaskContext context)
                            {
                                player->KilledMonsterCredit(165150);
                            }).Schedule(Milliseconds(17000), [player](TaskContext context)
                                {
                                    player->KilledMonsterCredit(165150);
                                }).Schedule(Milliseconds(19000), [player](TaskContext context)
                                    {
                                        player->KilledMonsterCredit(165150);
                                    }).Schedule(Milliseconds(22000), [player](TaskContext context)
                                        {
                                            player->KilledMonsterCredit(165150);
                                        }).Schedule(Milliseconds(24000), [player](TaskContext context)
                                            {
                                                player->KilledMonsterCredit(165150);
                                            }).Schedule(Milliseconds(30000), [player](TaskContext context)
                                                {
                                                    player->RemoveAura(325368); // exiting the vehicle                     
                                                    player->NearTeleportTo(263.819f, -2290.56f, 80.5917f, 2.115129f, false);
                                                }).Schedule(Milliseconds(32000), [this, player](TaskContext context)
                                                    {
                                                        me->ForcedDespawn(1000);
                                                    });
        }
        return true;
    }
};

// npc_Shuja_Grimaxe_167126
struct npc_Shuja_Grimaxe_167126 : public ScriptedAI
{
    npc_Shuja_Grimaxe_167126(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 10.0f))
                if (player->HasQuest(QUEST_H_THE_REDEATHER))
                    if (player->GetQuestObjectiveProgress(QUEST_H_THE_REDEATHER, 1))
                    {
                        if (!say)
                        {
                            me->RemoveAllAuras();
                            me->NearTeleportTo(230.912f, -2297.61f, 80.7119f, 1.31069f, false);
                            if (Creature* grimaxe = player->FindNearestCreature(167145, 15.0f, true))
                            {
                                player->GetScheduler().Schedule(Milliseconds(3000), [this, grimaxe] (TaskContext context)
                                {
                                    grimaxe->AI()->Talk(0);
                                }).Schedule(Milliseconds(9000), [this, player] (TaskContext context)
                                {
                                    me->AI()->Talk(0);
                                }).Schedule(Milliseconds(18000), [player, grimaxe] (TaskContext context)
                                {
                                    grimaxe->AI()->Talk(1);
                                });
                            }
                            say = true;
                        }
                    }

        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 100.0f))
                if (player->HasQuest(QUEST_H_THE_REDEATHER))
                    if (player->GetQuestObjectiveProgress(QUEST_H_THE_REDEATHER, 2) == 8)
                    {
                        FizzPlayConversation(player);
                    }
    }


    void FizzPlayConversation(Player* player)
    {
        if (!say)
        {
            player->PlayConversation(14521);
            player->GetScheduler().Schedule(Milliseconds(6000), [this, player](TaskContext context)
                {
                    //player->NearTeleportTo(263.819f, -2290.56f, 80.5917f, 2.115129f, false);

                    if (TempSummon* grimaxe = player->SummonCreature(167145, Position(264.819f, -2291.56f, 80.5917f, 2.115129f), TEMPSUMMON_TIMED_DESPAWN, 30min, 0U))
                    {
                        //grimaxe->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, grimaxe->GetFollowAngle());            //follower_player
                        player->GetScheduler().Schedule(Milliseconds(3000), [player, grimaxe](TaskContext context)
                            {
                                if (Creature* Shuja = player->FindNearestCreature(167126, 50.0f, true))
                                {
                                    Shuja->AI()->Talk(1);

                                    player->GetScheduler().Schedule(Milliseconds(4000), [grimaxe](TaskContext context)
                                        {
                                            grimaxe->GetMotionMaster()->MovePoint(1, 235.297f, -2268.614f, 80.895f, true);
                                        });
                                }
                            });
                    }

                });
        }

        say = true;
    }

    bool say;

    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_REDEATHER)
        {
            if (Creature* grimaxe = player->FindNearestCreature(167145, 50.0f, true))
            {
                grimaxe->ForcedDespawn(100);
            }

            me->ForcedDespawn(100);

            // placing the player to the right location of the scene
            player->NearTeleportTo(229.736f, -2296.816f, 80.894f, 1.03f, false);

        }
    }
};

// npc_Shuja_Grimaxe_167219
struct npc_Shuja_Grimaxe_167219 : public ScriptedAI
{
    npc_Shuja_Grimaxe_167219(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_HARPY_PROBLEM)
        {
            Talk(0);

            me->GetMotionMaster()->MovePoint(1, 332.667f, -2358.745f, 96.335f, true);

            me->AddDelayedEvent(6000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 375.262f, -2399.416f, 112.083f, true);
                });
            me->AddDelayedEvent(12000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 392.667f, -2440.18f, 125.475f, true);
                });
            me->AddDelayedEvent(17000, [this]() -> void
                {
                    me->DespawnOrUnsummon(1s, 30s);
                });
        }
    }

};

enum MessagetoBaseEvents
{
    EVENT_WAYPOINT_REACHED,
};

// npc_Shuja_Grimaxe_167290_q59947
struct npc_Shuja_Grimaxe_167290 : public EscortAI
{
    npc_Shuja_Grimaxe_167290(Creature* creature) : EscortAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_MESSAGE_TO_BASE)
        {
            Talk(2);
            Start(true);
            BoRunBase(player);
            HerbertRunBase(player);
        }
    }

    void BoRunBase(Player* player)
    {
        if (Creature* bo = player->FindNearestCreature(167291, 20.0f, true))
        {
            bo->SetWalk(false);
            player->GetScheduler().Schedule(Milliseconds(3000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[0]);
            }).Schedule(Milliseconds(6000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[1]);
            }).Schedule(Milliseconds(10000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[2]);
            }).Schedule(Milliseconds(14000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[3]);
            }).Schedule(Milliseconds(20000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[4]);
            }).Schedule(Milliseconds(26000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[5]);
            }).Schedule(Milliseconds(32000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, MessagetoBase[6]);
            }).Schedule(Milliseconds(35000), [this, bo] (TaskContext context)
            {
                bo->GetMotionMaster()->MovePoint(1, 187.056f, -2279.94f, 81.8464f, false, 5.02938f);
            });
        }
    }

    void HerbertRunBase(Player* player)
    {
        if (Creature* herbert = player->FindNearestCreature(167182, 20.0f, true))
        {
            herbert->SetWalk(false);
            herbert->SetFlying(false);
            player->GetScheduler().Schedule(Milliseconds(4000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[0]);
            }).Schedule(Milliseconds(7000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[1]);
            }).Schedule(Milliseconds(11000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[2]);
            }).Schedule(Milliseconds(17000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[3]);
            }).Schedule(Milliseconds(23000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[4]);
            }).Schedule(Milliseconds(30000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[5]);
            }).Schedule(Milliseconds(36000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, MessagetoBase[6]);
            }).Schedule(Milliseconds(40000), [this, herbert] (TaskContext context)
            {
                herbert->GetMotionMaster()->MovePoint(2, 182.311f, -2281.61f, 81.8466f, false, 4.7545f);
            });
        }
    }
};

// npc_Bo_167291
struct npc_Bo_167291 : public ScriptedAI
{
    npc_Bo_167291(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 20.0f))
            {
                if (player->HasQuest(QUEST_H_THE_HARPY_PROBLEM))
                    if (!say)
                    {
                        Talk(0);

                        player->GetScheduler().Schedule(Milliseconds(4000), [player] (TaskContext context)
                        {
                            if (Creature* shuja = player->FindNearestCreature(167290, 20.0f, true))
                                shuja->AI()->Talk(1);
                        });
                        say = true;
                    }
            }

        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST) && player->HasQuest(QUEST_H_HARPY_CULLING) && player->HasQuest(QUEST_H_PURGE_THE_TOTEMS))
            if (Creature* shuja = player->FindNearestCreature(167290, 10.0f, true))
            {
                shuja->AI()->Talk(1);
                player->GetScheduler().Schedule(Milliseconds(4000), [this] (TaskContext context)
                {
                    me->AI()->Talk(1);
                });
            }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_THE_HARPY_PROBLEM)
            if (Creature* shuja = player->FindNearestCreature(167219, 20.0f, true))
            {
                shuja->DespawnOrUnsummon(1s);
                shuja->SetRespawnDelay(10);
            }
    }
    bool say;
};

// npc_Herbert_Gloomburst_167182
class npc_Herbert_Gloomburst_167182 : public CreatureScript
{
public:
    npc_Herbert_Gloomburst_167182() : CreatureScript("npc_Herbert_Gloomburst_167182") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_Herbert_Gloomburst_167182AI(creature);
    }

    struct npc_Herbert_Gloomburst_167182AI : public ScriptedAI
    {
        npc_Herbert_Gloomburst_167182AI(Creature* creature) : ScriptedAI(creature) { }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST)
            {
                me->RemoveAura(305513);
                if (me->IsHovering())
                {
                    me->SetHoverHeight(1);
                    me->SetHover(false);
                }
                me->GetMotionMaster()->MovePoint(1, 495.411f, -2354.56f, 160.3297f, true);
                me->Say(196516);
                me->PlayDirectSound(157190, player, 196516);
                me->ForcedDespawn(3000);
            }
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->IsActiveQuest(QUEST_H_THE_RESCUE_OF_HERBERT_GLOOMBURST) && triggered == false)
                AddGossipItemFor(player, GossipOptionNpc::None, "I'll fight the harpies that come. Can you use your magic to break free?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            else
                player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 0)
            {
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                me->Say(196511);
                me->PlayDirectSound(157187, player, 196511);
                FightingBloodBeak(player, me);
                triggered = true;
            }
            return true;
        }

        uint32 bloodbeak = 153964;
        uint32 harpy_ambusher = 155192;
        uint32 hunting_worg = 169162;
        bool triggered = false;

        void FightingBloodBeak(Player* player, Creature* creature)
        {
            creature->AddDelayedEvent(5000, [this, player]() -> void
                {
                    TempSummon* worg1 = player->SummonCreature(hunting_worg, Position(477.835f, -2381.51f, 156.771f, 1.1187f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    worg1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                    TempSummon* harpy1 = player->SummonCreature(harpy_ambusher, Position(500.832f, -2331.03f, 164.318f, 4.25644f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    harpy1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                });
            creature->AddDelayedEvent(20000, [this, player]() -> void
                {
                    TempSummon* worg2 = player->SummonCreature(hunting_worg, Position(472.838f, -2351.12f, 162.377f, 5.558f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    worg2->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                    TempSummon* harpy2 = player->SummonCreature(harpy_ambusher, Position(492.376f, -2397.87f, 154.462f, 1.52499f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    harpy2->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                });
            creature->AddDelayedEvent(30000, [this, player]() -> void
                {
                    if (TempSummon* bo1 = player->SummonCreature(167300, Position(498.74f, -2359.76f, 159.924f, 4.6364f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U))
                    {
                        bo1->AI()->Talk(0);
                        player->GetScheduler().Schedule(Milliseconds(30000), [this, bo1](TaskContext context)
                            {
                                bo1->AI()->Talk(1);
                            });
                    }
                    if (TempSummon* shuja1 = player->SummonCreature(167301, Position(490.437f, -2359.71f, 159.975f, 3.96887f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U))
                    {
                        player->GetScheduler().Schedule(Milliseconds(3000), [this, shuja1](TaskContext context)
                            {
                                shuja1->AI()->Talk(0);
                            }).Schedule(Milliseconds(33000), [this, shuja1](TaskContext context)
                                {
                                    shuja1->AI()->Talk(1);
                                });
                    }

                });

            creature->AddDelayedEvent(40000, [this, player]() -> void
                {
                    TempSummon* worg3 = player->SummonCreature(hunting_worg, Position(477.835f, -2381.51f, 156.771f, 1.1187f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    worg3->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                    TempSummon* harpy3 = player->SummonCreature(harpy_ambusher, Position(500.832f, -2331.03f, 164.318f, 4.25644f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    harpy3->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                });
            creature->AddDelayedEvent(55000, [this, player]() -> void
                {
                    TempSummon* worg4 = player->SummonCreature(hunting_worg, Position(472.838f, -2351.12f, 162.377f, 5.558f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    worg4->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                    TempSummon* harpy4 = player->SummonCreature(harpy_ambusher, Position(492.376f, -2397.87f, 154.462f, 1.52499f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    harpy4->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                });
            creature->AddDelayedEvent(70000, [this, player]() -> void
                {
                    TempSummon* bloodbeak1 = player->SummonCreature(bloodbeak, Position(491.975f, -2414.46f, 162.734f, 1.502485f), TEMPSUMMON_TIMED_DESPAWN, 15min, 0U);
                    bloodbeak1->GetMotionMaster()->MovePoint(1, 494.811f, -2359.18f, 159.993f, true);
                });
        }
    };
};

// npc_Lana_Jordan_167221
struct npc_Lana_Jordan_167221 : public EscortAI
{
    npc_Lana_Jordan_167221(Creature* creature) : EscortAI(creature) { }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_WESTWARD_BOUND)
        {
            if (Creature* wonsa = player->FindNearestCreature(167222, 10.0f, true))
            {
                wonsa->DespawnOrUnsummon(1s, 30s);
            }
            me->DespawnOrUnsummon(1s, 30s);
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_WESTWARD_BOUND)
        {
            Talk(0);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            if (Creature* wonsa = player->FindNearestCreature(167222, 10.0f, true))
            {
                wonsa->SetWalk(false);
                wonsa->GetMotionMaster()->MovePoint(1, 151.681f, -2305.85f, 86.9756f, true);
                player->GetScheduler().Schedule(Milliseconds(2000), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(2, 137.013f, -2297.01f, 91.3035f, true);

                })
                    .Schedule(Milliseconds(4500), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(3, 120.97f, -2284.04f, 96.3998f, true);

                })
                    .Schedule(Milliseconds(7000), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(4, 106.753f, -2267.51f, 96.8173f, true);

                })
                    .Schedule(Milliseconds(10500), [wonsa] (TaskContext context)
                {
                    wonsa->GetMotionMaster()->MovePoint(5, 89.954f, -2249.46f, 94.7153f, true);

                });

            }
						
			me->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));

            player->GetScheduler().Schedule(Milliseconds(2000), [this] (TaskContext context)
            {
                Start(true);
            });
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (waypointId == 5)
        {
            Talk(1);
            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

};

//npc_Lana_Jordan_167225
struct npc_Lana_Jordan_167225 : public EscortAI
{
    npc_Lana_Jordan_167225(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    bool event59949;

    void Reset() override
    {
        event59949 = false;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_WHO_LURKS_IN_THE_PIT)
        {
            Talk(1);
            player->GetScheduler().Schedule(Milliseconds(6000), [this, player] (TaskContext context)
            {
                if (Creature* wonsa = me->FindNearestCreature(167226, 10.0f, true))
                    wonsa->AI()->Talk(0);
            });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
                if (player->GetQuestStatus(QUEST_H_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_COMPLETE)
                {
                    if (!event59949)
                    {
                        if (Creature* wonsa = me->FindNearestCreature(167226, 30.0f, true))
                            player->GetScheduler().Schedule(Milliseconds(6000), [this, wonsa](TaskContext context)
                                {
                                    Start(true);
                                    wonsa->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, wonsa->GetFollowAngle());
                                    wonsa->AI()->Talk(1);

                                }).Schedule(Milliseconds(10000), [this](TaskContext context)
                                    {
                                        Talk(1);
                                    }).Schedule(Milliseconds(15000), [this, wonsa](TaskContext context)
                                        {
                                            wonsa->AI()->Talk(2);
                                        });
                                    event59949 = true;
                    }
                }
    }
    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        switch (pointId)
        {
        case 9:
        {
            if (Creature* wonsa = me->FindNearestCreature(167226, 30.f))
                wonsa->GetMotionMaster()->MovePoint(0, 178.796f, -2292.23f, 81.853f, true, 0.70315f);
        }
        break;
        }
    }
};

// npc_Crenna_Earth_Daughter_167254
struct npc_Crenna_Earth_Daughter_167254 : public ScriptedAI
{
    npc_Crenna_Earth_Daughter_167254(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool result) override
    {
        if (Player* player = clicker->ToPlayer())
            if (player->HasQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
            {
                player->KilledMonsterCredit(167254);
                player->GetScheduler().Schedule(Milliseconds(50000), [this, player](TaskContext context)
                    {
                        me->ForcedDespawn(1000);
                        player->NearTeleportTo(108.546f, -2271.868f, 97.172f, 5.64f, false);
                    });

            }
    }
};

// npc_Crenna_Earth_Daughter_166575
struct npc_Crenna_Earth_Daughter_166575 : public EscortAI
{
    npc_Crenna_Earth_Daughter_166575(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    bool event59949;

    void Reset() override
    {
        event59949 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
                if (player->GetQuestStatus(QUEST_H_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_COMPLETE)
                    if (!event59949)
                    {
                        Talk(0);
                        Start(true);
                        event59949 = true;
                    }
    }
};

//npc_Warlord_Breka_Grimaxe_167212
struct npc_Warlord_Breka_Grimaxe_167212 : public EscortAI
{
    npc_Warlord_Breka_Grimaxe_167212(Creature* creature) : EscortAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_STOCKING_UP_ON_SUPPLIES)
        {
            me->AddDelayedEvent(4000, [this, player]() -> void
                {
                    player->ForceCompleteQuest(QUEST_H_STOCKING_UP_ON_SUPPLIES);
                });
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (player->GetQuestStatus(QUEST_H_MESSAGE_TO_BASE) == QUEST_STATUS_REWARDED)
        {
            if (Creature* shuja = me->FindNearestCreature(167290, 30.0f, true))
                shuja->DespawnOrUnsummon(1s, 30s);
            if (Creature* bo = me->FindNearestCreature(167291, 30.0f, true))
                bo->DespawnOrUnsummon(1s, 30s);
            if (Creature* herbert = me->FindNearestCreature(167182, 30.0f, true))
                herbert->DespawnOrUnsummon(1s, 30s);
        }

        if (quest->GetQuestId() == QUEST_H_WHO_LURKS_IN_THE_PIT)
        {
            if (Creature* lana = me->FindNearestCreature(167225, 30.0f, true))
                if (Creature* wonsa = me->FindNearestCreature(167226, 30.0f, true))
                    if (Creature* crenna = me->FindNearestCreature(166575, 30.0f, true))
                    {
                        lana->DespawnOrUnsummon(1s, 30s);
                        wonsa->DespawnOrUnsummon(1s, 30s);
                        crenna->DespawnOrUnsummon(1s, 30s);
                    }
        }
    }

};

// npc_Monstrous_Cadaver_157091
struct npc_Monstrous_Cadaver_157091 : public ScriptedAI
{
    npc_Monstrous_Cadaver_157091 (Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* killer) override
    {
        if (Vehicle* vehicle = killer->GetVehicleKit())
            if (Unit* passenger = vehicle->GetPassenger(0))
                if (Player* player = passenger->ToPlayer())
                    player->KilledMonsterCredit(165150);       
    }

};

// item 178051 item_h_resizer_v901
class item_h_resizer_v901 : public ItemScript
{
public:
    item_h_resizer_v901() : ItemScript("item_h_resizer_v901") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        if (player->HasQuest(QUEST_H_RESIZING_THE_SITUATION))
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

//npc_coulston_nereus
struct npc_coulston_nereus : public ScriptedAI
{
    npc_coulston_nereus(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->GetQuestStatus(58917) == QUEST_STATUS_INCOMPLETE)
            {
                if (player->IsInDist(me, 30.0f))
                {
                    player->KilledMonsterCredit(162972, ObjectGuid::Empty);
                }
            }
        }
    }
};

// npc_gutgruck_the_tough
struct npc_gutgruck_the_tough : public ScriptedAI
{
    npc_gutgruck_the_tough(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->Yell("Me gonna get more sacrifices for ritual", LANG_UNIVERSAL);
        me->GetMotionMaster()->MoveRandom(20.0f);
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
        {
            player->KilledMonsterCredit(175015);
        }

        Talk(0);
        me->DespawnOrUnsummon(1000ms);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(1);
    }
};

// at_for_gutgruck_the_tough
struct at_for_gutgruck_the_tough : public AreaTriggerAI
{
    at_for_gutgruck_the_tough(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* unit) override
    {
        Player* player = unit->ToPlayer();

        if (!player)
            return;

        if (player->GetQuestStatus(58933) == QUEST_STATUS_INCOMPLETE)
            at->SummonCreature(163031, Position(282.236420f, -2148.987793f, 103.455276f, 3.317860f), TEMPSUMMON_MANUAL_DESPAWN);
    }
};

// hrun_the_exiled_156900
struct hrun_the_exiled_156900 : public ScriptedAI
{
    hrun_the_exiled_156900(Creature* creature) : ScriptedAI(creature) { }


    void Reset() override
    {
        ScriptedAI::Reset();
        theoreticallydead = false;
    }

    bool casted1, casted2 = false;

    void JustEngagedWith(Unit* who) override
    {
        Talk(0);
        me->AddDelayedEvent(3000, [this, who]()
            {
                me->CastSpell(who, SPELL_SPIRIT_BOLT);
            });
    }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();

        if ((hp < maxhp * 0.9) && !casted1)
        {
            me->CastSpell(attacker, SPELL_SPIRIT_BOLT);
            casted1 = true;
        }

        if (((hp < maxhp * 0.6) && !casted2) || damage > hp * 2)
        {
            me->CastSpell(attacker, SPELL_DRAIN_SPIRIT);
            casted2 = true;
        }

        if (hp < maxhp * 0.3) // Hrun doesn't die he just burns and hide + reset
        {
            damage = 0;
            me->CastSpell(me, SPELL_FINAL_SACRIFICE_FIRE);
            me->SetUnitFlag(UnitFlags::UNIT_FLAG_UNINTERACTIBLE);
            me->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);
            me->AddUnitState(UnitState::UNIT_STATE_UNATTACKABLE);
            me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            me->DespawnOrUnsummon(1min, 2min);

            me->AddDelayedEvent(10000, [this]()
                {
                    me->CombatStop();
                });

            //theoreticallydead = true;

            if (Player* player = attacker->ToPlayer())
            {
                player->KilledMonsterCredit(156900);

                if (player->IsInAlliance())
                {
                    if (player->HasQuest(QUEST_WHO_LURKS_IN_THE_PIT))
                    {
                        Talk(1);
                        if (Creature* ralia_noform = player->FindNearestCreature(156902, 100.0f, true))
                            if (Creature* ralia_travelform = player->SummonCreature(156929, Position(76.064f, -2137.493f, -30.173f, 5.0f), TEMPSUMMON_MANUAL_DESPAWN, 5min, 0U))
                            {
                                ralia_noform->DespawnOrUnsummon(100ms, 2min);

                                player->GetScheduler().Schedule(Milliseconds(4000), [player, ralia_travelform](TaskContext context)
                                    {
                                        ralia_travelform->Say(196467);
                                        ralia_travelform->PlayDirectSound(157167, player, 196467);
                                    });
                            }
                    }
                }
                if (player->IsInHorde())
                {
                    if (player->HasQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
                    {
                        Talk(1);
                        if (Creature* crenna_noform = player->FindNearestCreature(167250, 50.0f, true))
                            if (Creature* crenna_travelform = player->SummonCreature(167254, Position(76.064f, -2137.493f, -30.173f, 5.0f), TEMPSUMMON_MANUAL_DESPAWN, 5min, 0U))
                            {
                                crenna_noform->DespawnOrUnsummon(100ms, 2min);

                                player->GetScheduler().Schedule(Milliseconds(4000), [player, crenna_travelform](TaskContext context)
                                    {
                                        crenna_travelform->Say(196467);
                                        crenna_travelform->PlayDirectSound(157167, player, 196467);
                                    });
                            }
                    }
                }
            }
        }
    }

    // if the player don't have the quest or already completed or rewarded
    // hide hrun
    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 200.0f) && !theoreticallydead)
            {
                // if the quest is rewarded or completed (horde + alli) or killing hrun objective done
                if (player->GetQuestObjectiveProgress(QUEST_H_WHO_LURKS_IN_THE_PIT, 1) == 1
                    || player->GetQuestObjectiveProgress(QUEST_WHO_LURKS_IN_THE_PIT, 1) == 1
                    || player->GetQuestStatus(QUEST_H_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_COMPLETE
                    ||  player->GetQuestStatus(QUEST_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_COMPLETE
                    ||  player->GetQuestStatus(QUEST_H_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_REWARDED
                    ||  player->GetQuestStatus(QUEST_WHO_LURKS_IN_THE_PIT) == QUEST_STATUS_REWARDED)
                {
                    // remove for this player
                    if (!me->HasAura(SPELL_FINAL_SACRIFICE_FIRE))
                        me->CastSpell(me, SPELL_FINAL_SACRIFICE_FIRE);
                    if (me->GetFaction() == 14)
                        me->SetFaction(35);
                }
                else
                {
                    // player is on quest so we remove the flame+hide aura and let's begin
                    if (me->HasAura(SPELL_FINAL_SACRIFICE_FIRE))
                    {
                        me->RemoveAura(SPELL_FINAL_SACRIFICE_FIRE);
                        me->SetObjectScale(2.f);
                    }
                    if (me->GetFaction() == 35)
                        me->SetFaction(14);
                }
            }
        }
    }

private:
    bool theoreticallydead;
};

// npc_ogre_overseer_156676
struct npc_ogre_overseer_156676 : public ScriptedAI
{
    npc_ogre_overseer_156676(Creature* creature) : ScriptedAI(creature)
    {
        me->SetObjectScale(2.f); // i was lazy for sql query
    }

    void JustEngagedWith(Unit* who) override
    {
        me->Say(178950);
        me->PlayDirectSound(150269, who->ToPlayer(), 178950);
        me->AddDelayedEvent(3000, [this, who]()
            {
                me->CastSpell(who->ToPlayer(), SPELL_EARTHSHATTER);
            });
    }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.7) && (hp > maxhp * 0.4))
        {
            me->CastSpell(player, SPELL_BACKHAND);
        }
        if ((hp < maxhp * 0.3) && (hp > maxhp * 0.1))
        {
            me->CastSpell(attacker, SPELL_EARTHSHATTER);
        }

        if (damage > hp) // on last hit before die
            Talk(0);
    }
};

// npc_quilboars_warrior_geomancer_150237_150238
struct npc_quilboars_warrior_geomancer_150237_150238 : public ScriptedAI
{
    npc_quilboars_warrior_geomancer_150237_150238(Creature* creature) : ScriptedAI(creature) { }

    int8 random_of_5, chancetospeak;
    bool mid_talked;

    void JustEngagedWith(Unit* who) override
    {
        Player* player = who->ToPlayer();

        random_of_5 = rand() % 5 + 1; // randomize texts
        chancetospeak = rand() % 3 + 1; // chance to speak 33%

        if (me->HasAura(SPELL_QUILBOAR_SLEEP)) me->RemoveAura(SPELL_QUILBOAR_SLEEP);

        if (chancetospeak == 1)
        {
            if (random_of_5 == 1) { me->PlayDirectSound(150281, player, 176079); me->Say(176079); } // Death to the outsiders! 
            if (random_of_5 == 2) { me->PlayDirectSound(150283, player, 176082); me->Say(176082); } // More sacrifices for the ritual! 
            if (random_of_5 == 3) { me->PlayDirectSound(150282, player, 176082); me->Say(176080); } // Our thorns will strangle you!  
            if (random_of_5 == 4) { me->PlayDirectSound(150278, player, 176076); me->Say(176076); } // The geolord demands your demise!  
            if (random_of_5 == 5) { me->PlayDirectSound(150276, player, 176074); me->Say(176074); } // The ogres give us strength!
        }

        me->AddDelayedEvent(3000, [this, player]()
            {
                if (me->GetEntry() == 150237)
                    me->CastSpell(player, SPELL_BRUTAL_STRIKE);
                else
                    me->CastSpell(player, SPELL_EARTH_BOLT_2);
            });
    }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.3) && (hp > maxhp * 0.2))
        {
            if (me->GetEntry() == 150237)
                me->CastSpell(player, SPELL_BRUTAL_STRIKE);
            else
                me->CastSpell(player, SPELL_EARTH_BOLT_2);

            if (!mid_talked)
            {
                random_of_5;
                mid_talked = true;

                if (random_of_5 == 1) { me->PlayDirectSound(150279, player, 176077); me->Say(176077); } // We serve the geolord! 
                if (random_of_5 == 3) { me->PlayDirectSound(150277, player, 176075); me->Say(176075); } // Your blood will earn our reward!
                if (random_of_5 == 5) { me->PlayDirectSound(150280, player, 176078); me->Say(176078); } // Witness our power! 
            }
        }

        if (damage > hp) // on last hit before die
        {
            random_of_5;

            if (random_of_5 == 1) { me->PlayDirectSound(150284, player, 176083); me->Say(176083); } // I return... to... the earth... 
            if (random_of_5 == 2) { me->PlayDirectSound(150286, player, 176085); me->Say(176085); } // Ogres... will... kill you... 
            if (random_of_5 == 3) { me->PlayDirectSound(150285, player, 176084); me->Say(176084); } // Our tribe... will... survive... 
            if (random_of_5 == 4) { me->PlayDirectSound(150287, player, 176086); me->Say(176086); } // The geolord... will not... lose...   
            if (random_of_5 == 5) { me->PlayDirectSound(150288, player, 176088); me->Say(176088); } // Thorns... take... you... 
        }
    }
};

// npc_gorgroth_153580
struct npc_gorgroth_153580 : public ScriptedAI
{
    npc_gorgroth_153580(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 40.0f))
            {
                if (player->HasQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 17000) // find cooking area
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                    me->Say(174173);
                                    me->PlayDirectSound(153058, player, 174173);
                                }
                            }
                }

                if (player->HasQuest(QUEST_RIGHT_BENEATH_THEIR_EYES))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 17000) // find cooking area
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                    me->Say(174173);
                                    me->PlayDirectSound(153058, player, 174173);
                                }
                            }
                }
            }
        }
    }

    void ReceiveEmote(Player* player, uint32 emoteId) override
    {
        if (emoteId == 101) // wave (bro i had to make a script to get this id)
            if (player->HasQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES) || player->HasQuest(QUEST_RIGHT_BENEATH_THEIR_EYES))
            {
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                    for (QuestObjective const& obj : quest->GetObjectives())
                        if (obj.ObjectID == 85146) // wave to gorgroth
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                            {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                me->AddDelayedEvent(2000, [this, player]()        // time_delay
                                    {
                                        me->SetEmoteState(EMOTE_ONESHOT_POINT);
                                        me->Say(175499);
                                        me->PlayDirectSound(153059, player, 175499);
                                    });
                                me->AddDelayedEvent(7000, [this, player]()        // time_delay
                                    {
                                        me->Say(175501);
                                        me->PlayDirectSound(153060, player, 175501);
                                        // despawn old version after 60 sec and 5 sec for respawning
                                        me->DespawnOrUnsummon(5s, 60s);
                                    });
                            }
                        }

                if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_RIGHT_BENEATH_THEIR_EYES))
                    for (QuestObjective const& obj : quest->GetObjectives())
                        if (obj.ObjectID == 80050) // wave to gorgroth
                        {
                            uint16 slot = player->FindQuestSlot(QUEST_RIGHT_BENEATH_THEIR_EYES);
                            if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                            {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                me->AddDelayedEvent(2000, [this, player]()        // time_delay
                                    {
                                        me->SetEmoteState(EMOTE_STATE_TALK);
                                        me->Say(175499);
                                        me->PlayDirectSound(153059, player, 175499);
                                    });
                                me->AddDelayedEvent(7000, [this, player]()        // time_delay
                                    {
                                        me->Say(175501);
                                        me->PlayDirectSound(153060, player, 175501);
                                        // despawn old version after 60 sec and 5 sec for respawning
                                        me->DespawnOrUnsummon(5s, 60s);
                                    });
                            }
                        }
            }
    }

};

// npc_torgok_162817
struct npc_torgok_162817 : public ScriptedAI
{
    npc_torgok_162817(Creature* creature) : ScriptedAI(creature) { }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if (!attacker->IsPlayer() && me->IsThreatenedBy(attacker))
            attacker->GetThreatManager().RemoveMeFromThreatLists();

        if ((hp < maxhp) && (hp > maxhp * 0.95))
        {
            if (!attacker->HasAura(SPELL_SOUL_GRASP))
                if (attacker->IsWithinDist(me, 10.0f, true))
                    me->CastSpell(attacker, SPELL_SOUL_GRASP);
        }

        if ((hp < maxhp * 0.7) && (hp > maxhp * 0.6))
        {
            me->CastSpell(player, SPELL_SPIRIT_BOLT);
        }

        if (damage > hp && attacker->IsCreature())
            damage = 0;
    }

    void JustDied(Unit* killer)
    {
        if (killer->IsPlayer())
        {
            Player* player = killer->ToPlayer();

            // safeguard if the player don't meet the next quest level requirement
            if (player->GetLevel() < 6)
                player->GiveXP(player->GetXPForNextLevel(), player, 1);
            Talk(0);
            if (player->HasQuest(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR))
            {
                player->ForceCompleteQuest(QUEST_RIDE_OF_THE_SCIENTIFICALLY_ENHANCED_BOAR);
                if (Creature* garrick = player->FindNearestCreature(156280, 20.0f, true))
                    garrick->AI()->DoAction(ACTION_TALK_GARRICK_HENRY);
                if (Creature* henry = player->FindNearestCreature(156799, 100.0f, true))
                    henry->AI()->DoAction(ACTION_TALK_GARRICK_HENRY);
            }
        }
    }
};

//go_harpy_totem_350803
struct go_harpy_totem_350803 : public GameObjectAI
{
    go_harpy_totem_350803(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasQuest(QUEST_H_PURGE_THE_TOTEMS))
        {
            me->SummonCreature(167993, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 5s);
            if (Creature* harpy_totem_npc = me->FindNearestCreature(167993, 5.0f, true))
                me->CastSpell(harpy_totem_npc, SPELL_BURNING_TOP);
            player->KillCreditGO(350803);
            me->DestroyForPlayer(player);
        }
        return true;
    }
};

//go_thick_cocoon_339568_350796
struct go_thick_cocoon_339568_350796 : public GameObjectAI
{
    go_thick_cocoon_339568_350796(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        // horde
        if (player->HasQuest(QUEST_H_WHO_LURKS_IN_THE_PIT))
        {
            player->KillCreditGO(350796);
            player->CastSpell(player, 325524, true);
            me->DestroyForPlayer(player);
        }
        // alli
        if (player->HasQuest(QUEST_WHO_LURKS_IN_THE_PIT))
        {
            player->KillCreditGO(339568);
            player->CastSpell(player, 313324, true);
            me->DestroyForPlayer(player);
        }

        return true;
    }
};

//npc_Freed_Expedition_Member_167244_160964
struct npc_Freed_Expedition_Member_167244_160964 : public ScriptedAI
{
    npc_Freed_Expedition_Member_167244_160964(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            me->SelectNearestPlayers(10.0f, true);
            me->SetFacingToObject(player);
            me->AI()->Talk(0);
            me->AddDelayedEvent(2000, [this]() -> void
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    me->GetMotionMaster()->MoveRandom(10.0f);
                    me->ForcedDespawn(2000);
                });
            if (player->GetQuestObjectiveProgress(QUEST_H_WHO_LURKS_IN_THE_PIT, 0) == 5 && !player->GetQuestObjectiveProgress(QUEST_H_WHO_LURKS_IN_THE_PIT, 1))
            {
                player->GetScheduler().Schedule(Milliseconds(4000), [player](TaskContext context)
                    {
                        player->PlayConversation(14540);
                    });

            }
            if (player->GetQuestObjectiveProgress(QUEST_WHO_LURKS_IN_THE_PIT, 0) == 5 && !player->GetQuestObjectiveProgress(QUEST_WHO_LURKS_IN_THE_PIT, 1))
            {
                player->GetScheduler().Schedule(Milliseconds(4000), [player](TaskContext context)
                    {
                        player->PlayConversation(12120);
                    });

            }
        }
    }
};

// npc_harpie_boltcaller_152843
struct npc_harpie_boltcaller_152843 : public ScriptedAI
{
    npc_harpie_boltcaller_152843(Creature* creature) : ScriptedAI(creature) { }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.8) && (hp > maxhp * 0.7))
        {
            me->CastSpell(player, SPELL_HARPIE_LIGHTNINGBOLT);
        }
        if ((hp < maxhp * 0.4) && (hp > maxhp * 0.3))
        {
            me->CastSpell(player, SPELL_HARPIE_LIGHTNINGBOLT);
        }
    }

};

// npc_harpie_snatcher_152571
struct npc_harpie_snatcher_152571 : public ScriptedAI
{
    npc_harpie_snatcher_152571(Creature* creature) : ScriptedAI(creature) { }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.8) && (hp > maxhp * 0.7))
        {
            me->CastSpell(player, SPELL_HARPIE_TALONRAKE);
        }
        if ((hp < maxhp * 0.4) && (hp > maxhp * 0.3))
        {
            me->CastSpell(player, SPELL_HARPIE_TALONRAKE);
        }
    }

};

// npc_hunting_worg_152998
struct npc_hunting_worg_152998 : public ScriptedAI
{
    npc_hunting_worg_152998(Creature* creature) : ScriptedAI(creature) { }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.6) && (hp > maxhp * 0.3))
        {
            me->CastSpell(player, SPELL_WORG_CLAW);
        }
    }

};

//npc_breka_grimaxe_167145
struct npc_breka_grimaxe_167145 : public ScriptedAI
{
    npc_breka_grimaxe_167145(Creature* c) : ScriptedAI(c) { }

    void IsSummonedBy(WorldObject* unit) override
    {
        if (Player* player = unit->ToPlayer())
        {
            me->SetOwnerGUID(player->GetGUID());
            player->GetScheduler().Schedule(Milliseconds(6000), [this, player](TaskContext context)
                {
                    if (Creature* shuja = player->FindNearestCreature(167126, 20.0f, true))
                        shuja->AI()->Talk(1);
                }).Schedule(Milliseconds(11000), [this](TaskContext context)
                    {
                        Talk(2);
                    });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        if (player->IsInDist(me, 10.0f) && player->IsPlayer() == true && me->GetPositionX() == 235.297f) //player come close breka follows
        {
            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());
        }

        if (Creature* torgok = who->ToCreature())
        if (torgok->IsInDist(me, 10.0f) && torgok->GetEntry() == 162817) //torgok come close
        {
            me->SetTarget(torgok->GetGUID());
            me->SetFaction(FACTION_ESCORTEE_H_NEUTRAL_ACTIVE);
            me->SetBaseWeaponDamage(BASE_ATTACK, WeaponDamageRange(MAXDAMAGE), 0.1f);
            me->setAttackTimer(BASE_ATTACK, 2000);
            me->AI()->AttackStart(torgok);
        }
    }

};

// npc_barrow_spider_160394
struct npc_barrow_spider_160394 : public ScriptedAI
{
    npc_barrow_spider_160394(Creature* creature) : ScriptedAI(creature) { }

    bool casted = false;

    void JustEngagedWith(Unit* who) override
    {
        me->AddDelayedEvent(3000, [this, who]()
            {
                me->CastSpell(who, SPELL_POISONED_BITE);
            });
    }

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        uint32 maxhp = me->GetMaxHealth();
        uint32 hp = me->GetHealth();
        Player* player = attacker->ToPlayer();

        if ((hp < maxhp * 0.5) && !casted)
        {
            me->CastSpell(player, SPELL_POISONED_BITE);
            casted = true;
        }
    }
};

//npc_breka_grimaxe_167596
struct npc_breka_grimaxe_167596 : public EscortAI
{
    npc_breka_grimaxe_167596(Creature* creature) : EscortAI(creature) { }

    ObjectGuid   m_shujaGUID;
    ObjectGuid   m_herbertGUID;
    ObjectGuid   m_crennaGUID;
    ObjectGuid   m_playerGUID;

    void Initialize()
    {
        m_shujaGUID = ObjectGuid::Empty;
        m_herbertGUID = ObjectGuid::Empty;
        m_crennaGUID = ObjectGuid::Empty;
        m_playerGUID = ObjectGuid::Empty;
    }

    void GetFollowerMember()
    {
        if (!m_shujaGUID)
            if (Creature* shuja = me->FindNearestCreature(167290, 20.0f, true))
                m_shujaGUID = shuja->GetGUID();

        if (!m_herbertGUID)
            if (Creature* herbert = me->FindNearestCreature(167182, 20.0f, true))
                m_herbertGUID = herbert->GetGUID();

        if (!m_crennaGUID)
            if (Creature* crenna = me->FindNearestCreature(166575, 20.0f, true))
                m_crennaGUID = crenna->GetGUID();

    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_TO_DARKMAUL_CITADEL)
        {
            Talk(0); // Recruit, we push forward! The rest of you, hold this position.

            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);

            m_playerGUID = player->GetGUID();

            //Start(true, false);
            GetFollowerMember();

            Creature* shuja = ObjectAccessor::GetCreature(*me, m_shujaGUID);
            shuja->GetMotionMaster()->MoveFollow(me, -2.0f, shuja->GetFollowAngle());

            Creature* herbert = ObjectAccessor::GetCreature(*me, m_herbertGUID);
            herbert->GetMotionMaster()->MoveFollow(shuja, -2.0f, herbert->GetFollowAngle());

            Creature* crenna = ObjectAccessor::GetCreature(*me, m_crennaGUID);
            crenna->GetMotionMaster()->MoveFollow(herbert, -2.0f, crenna->GetFollowAngle());

            me->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
            shuja->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
            herbert->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
            crenna->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));

            me->AddDelayedEvent(1000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 210.077f, -2279.267f, 80.738f, true);
                });
            me->AddDelayedEvent(3500, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(2, 215.775f, -2263.325f, 80.455f, true);
                });
            me->AddDelayedEvent(7000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(3, 209.081f, -2243.268f, 83.129f, true);
                });
            me->AddDelayedEvent(11000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(4, 209.794f, -2207.457f, 89.874f, true);
                });
            me->AddDelayedEvent(17000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(5, 226.471f, -2182.809f, 92.941f, true);
                });
            me->AddDelayedEvent(22000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(6, 256.923f, -2178.689f, 96.801f, true);
                });
            me->AddDelayedEvent(26000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(7, 296.684f, -2168.243f, 103.682f, true);
                });
            me->AddDelayedEvent(33000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(8, 321.692f, -2173.496f, 106.295f, true);
                });
            me->AddDelayedEvent(36000, [this, player]() -> void
                {
                    if (Creature* shuja = ObjectAccessor::GetCreature(*me, m_shujaGUID))
                    {
                        shuja->SetRespawnDelay(10);
                        shuja->DespawnOrUnsummon(100ms);
                    }
                    if (Creature* herbert = ObjectAccessor::GetCreature(*me, m_herbertGUID))
                    {
                        herbert->SetRespawnDelay(10);
                        herbert->DespawnOrUnsummon(100ms);
                    }
                    if (Creature* crenna = ObjectAccessor::GetCreature(*me, m_crennaGUID))
                    {
                        crenna->SetRespawnDelay(10);
                        crenna->DespawnOrUnsummon(100ms);
                    }
                    me->SetRespawnDelay(10);
                    me->DespawnOrUnsummon(100ms);
                });
        }

        if (quest->GetQuestId() == QUEST_H_RIGHT_BENEATH_THEIR_EYES)
        {
            player->RemoveAura(298241);
        }
    }
};

//npc_warlord_mulgrin_thunderwalker_167646
struct npc_warlord_mulgrin_thunderwalker_167646 : public ScriptedAI
{
    npc_warlord_mulgrin_thunderwalker_167646(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 60.0f))
            {
                if (player->HasQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == 16893) // reach citadel entrance
                            {
                                uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                {
                                    player->RemoveAura(298241);
                                    player->SetObjectScale(1);
                                    player->SetQuestObjectiveData(obj, 1);
                                    player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);

                                    if (Creature* shuja = player->FindNearestCreature(167597, 60.0f, true))
                                    {
                                        me->AddDelayedEvent(3000, [shuja, player]()                    // time_delay
                                            {
                                                shuja->Say(196534);
                                                shuja->PlayDirectSound(157179, player, 196534);
                                            });
                                    }
                                    if (Creature* breka = player->FindNearestCreature(167596, 60.0f, true))
                                    {
                                        me->AddDelayedEvent(8000, [breka, player]()                    // time_delay
                                            {
                                                breka->Say(196535);
                                                breka->PlayDirectSound(157180, player, 196535);
                                            });
                                        me->AddDelayedEvent(13000, [breka, player]()                    // time_delay
                                            {
                                                player->ForceCompleteQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                                if (Creature* shuja = player->FindNearestCreature(167597, 30.0f, true))
                                                    shuja->ForcedDespawn(1000);
                                                if (Creature* herbert = player->FindNearestCreature(167598, 30.0f, true))
                                                    herbert->ForcedDespawn(1000);
                                                if (Creature* crenna = player->FindNearestCreature(167599, 30.0f, true))
                                                    crenna->ForcedDespawn(1000);
                                                breka->ForcedDespawn(1000);
                                            });
                                    }
                                }
                            }

                // QUEST_H_CONTROLLING_THEIR_STONES hidden objective (397378) if player got the ward stones complete the objective
                if (player->HasQuest(QUEST_H_CONTROLLING_THEIR_STONES) && !wardstonesready)
                    if (player->HasItemCount(168599) && player->HasItemCount(168600) && player->HasItemCount(168601))
                        if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_CONTROLLING_THEIR_STONES))
                            for (QuestObjective const& obj : quest->GetObjectives())
                                if (obj.StorageIndex == 3)
                                {
                                    uint16 slot = player->FindQuestSlot(quest->GetQuestId());
                                    if (!player->IsQuestObjectiveComplete(slot, quest, obj))
                                    {
                                        player->SetQuestObjectiveData(obj, 3);
                                        player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 3);
                                        wardstonesready = true;
                                    }
                                }

            }
    }

private:
    bool wardstonesready;
};

// npc_herbert_gloomburst_167598
struct npc_herbert_gloomburst_167598 : public ScriptedAI
{
    npc_herbert_gloomburst_167598(Creature* creature) : ScriptedAI(creature) { Reset(); }
     
private:

    ObjectGuid   shujaGUID;
    ObjectGuid   brekaGUID;
    ObjectGuid   crennaGUID;
    ObjectGuid   herbertGUID;

    void Reset() override
    {
        shujaGUID = ObjectGuid::Empty;
        brekaGUID = ObjectGuid::Empty;
        crennaGUID = ObjectGuid::Empty;
        herbertGUID = ObjectGuid::Empty;
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (!player->HasAura(298241) && player->HasQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready, transorm me into an ogre and I'll sneak into their citadel.", 24550, GOSSIP_ACTION_INFO_DEF + 0);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:
            if (Creature* shuja = player->FindNearestCreature(167597, 20.0f, true))
            {
                Position shujaposition = shuja->GetPosition();
                if (Creature* shuja2 = player->SummonCreature(167597, shujaposition, TEMPSUMMON_TIMED_DESPAWN, 10min))
                    shujaGUID = shuja2->GetGUID();
                shuja->DespawnOrUnsummon(1s, 60s);
            }
            if (Creature* breka = player->FindNearestCreature(167596, 20.0f, true))
            {
                Position brekaposition = breka->GetPosition();
                if (Creature* breka2 = player->SummonCreature(167596, brekaposition, TEMPSUMMON_TIMED_DESPAWN, 10min))
                    brekaGUID = breka2->GetGUID();
                breka->DespawnOrUnsummon(1s, 60s);
            }
            if (Creature* crenna = player->FindNearestCreature(167599, 10.0f, true))
            {
                Position crennaposition = crenna->GetPosition();
                if (Creature* crenna2 = player->SummonCreature(167599, crennaposition, TEMPSUMMON_TIMED_DESPAWN, 10min))
                    crennaGUID = crenna2->GetGUID();
                crenna->DespawnOrUnsummon(1s, 60s);
            }

            me->CastSpell(player, 313583); // ogre transformation herbert channeling

            Talk(0); // A willing test subject! Such a rare find.

            player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                {
                    Creature* crenna = ObjectAccessor::GetCreature(*me, crennaGUID);
                    {
                        crenna->CastSpell(player, 313598); // ogre transformation crenna channeling
                    }

                    if (Creature* herbert = player->SummonCreature(167598, *me, TEMPSUMMON_TIMED_DESPAWN, 10min))
                        herbertGUID = herbert->GetGUID();
                    me->AddAura(65050, me);          // add invisiable
                    me->DespawnOrUnsummon(60s, 60s);

            }).Schedule(Milliseconds(8000), [this, player](TaskContext context)
                {
                    player->CastSpell(player, 298241); // transforming to ogre
                    player->SetObjectScale(2.0f); // make it big like and ogre

                }).Schedule(Milliseconds(13000), [this, player](TaskContext context)
                    {
                        // give the credit
                        player->KilledMonsterCredit(167598);
                        Creature* herbert = ObjectAccessor::GetCreature(*me, herbertGUID);
                        {
                            herbert->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);

                            // chaining
                            herbert->CastSpell(player, 329760); // left hand chain
                            herbert->GetMotionMaster()->MoveFollow(player, -4.0f, me->GetFollowAngle());
                            herbert->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                        }

                        Creature* shuja = ObjectAccessor::GetCreature(*me, shujaGUID);
                        {
                            shuja->CastSpell(player, 329760); // left hand chain
                            shuja->GetMotionMaster()->MoveFollow(player, 2.0f, shuja->GetFollowAngle());
                            shuja->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                            shuja->AddDelayedEvent(10000, [shuja]() -> void
                                {
                                    shuja->AI()->Talk(2);
                                });
                        }

                        Creature* breka = ObjectAccessor::GetCreature(*me, brekaGUID);
                        {
                            breka->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                            breka->CastSpell(player, 329760); // left hand chain
                            breka->GetMotionMaster()->MoveFollow(player, -2.0f, breka->GetFollowAngle());
                            breka->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                            breka->Say(196529);
                            breka->PlayDirectSound(157175, player, 196529);
                        }

                        Creature* crenna = ObjectAccessor::GetCreature(*me, crennaGUID);
                        {
                            crenna->CastSpell(player, 329760); // left hand chain
                            crenna->GetMotionMaster()->MoveFollow(player, 4.0f, crenna->GetFollowAngle());
                            crenna->SetSpeed(MOVE_RUN, player->GetSpeed(MOVE_RUN));
                        }
                    });

                CloseGossipMenuFor(player);

                break;
        }
        return true;
    }
};

//npc_Ogre_Shadowcaster_153242
struct npc_Ogre_Shadowcaster_153242 : public ScriptedAI
{
    npc_Ogre_Shadowcaster_153242(Creature* creature) : ScriptedAI(creature) { Reset(); }
private:
    bool say_alliance;
    bool say_horde;

    void Reset() override
    {
        say_horde = false;
        say_alliance = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!say_horde)
            if (Player* player = who->ToPlayer())
                if (player->IsInDist(me, 20.0f))
                    if (player->HasQuest(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                    {
                        Talk(0);
                        if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_RIGHT_BENEATH_THEIR_EYES))
                            for (QuestObjective const& obj : quest->GetObjectives())
                                if (obj.ObjectID == 16996) // enter dark maul citadel
                                {
                                    uint16 slot = player->FindQuestSlot(QUEST_H_RIGHT_BENEATH_THEIR_EYES);
                                    if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                    {
                                        player->SetQuestObjectiveData(obj, 1);
                                        player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                    }
                                }
                        if (Creature* breka = player->FindNearestCreature(167596, 40.0f, true))
                        {
                            me->AddDelayedEvent(6000, [this,breka, player]()                    // time_delay
                                {
                                    breka->Say(196531);
                                    breka->PlayDirectSound(157177, player, 196531);
                                    me->DespawnOrUnsummon(30s, 10s);
                                });
                        }
                        say_horde = true;
                    }

        if (!say_alliance)
            if (Player* player = who->ToPlayer())
                if (player->IsInDist(me, 20.0f))
                    if (player->HasQuest(QUEST_RIGHT_BENEATH_THEIR_EYES))
                    {
                        say_alliance = true;
                        Talk(0);
                        if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_RIGHT_BENEATH_THEIR_EYES))
                            for (QuestObjective const& obj : quest->GetObjectives())
                                if (obj.ObjectID == 16996) // enter dark maul citadel
                                {
                                    uint16 slot = player->FindQuestSlot(QUEST_RIGHT_BENEATH_THEIR_EYES);
                                    if (!player->IsQuestObjectiveComplete(slot, quest, obj)) // just make it complete once
                                    {
                                        player->SetQuestObjectiveData(obj, 1);
                                        player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                                    }
                                }

                        if (Creature* garrick = player->FindNearestCreature(156961, 30.0f, true))
                        {
                            me->AddDelayedEvent(7000, [this, garrick]()                    // time_delay
                                {
                                    garrick->AI()->Talk(1);
                                    me->DespawnOrUnsummon(30s, 10s);
                                });
                        }
                    }
    }
};

// npc_Warlord_Breka_Grimaxe_167633
struct npc_Warlord_Breka_Grimaxe_167633 : public ScriptedAI
{
    npc_Warlord_Breka_Grimaxe_167633(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_CONTROLLING_THEIR_STONES)
        {
            Talk(0);
        }
    }
};

// npc_Warlord_Mulgrin_Thunderwalker_167183
struct npc_Warlord_Mulgrin_Thunderwalker_167183 : public ScriptedAI
{
    npc_Warlord_Mulgrin_Thunderwalker_167183(Creature* creature) : ScriptedAI(creature) { }

    // REMOVE THIS IF U SCRIPTED DARKMAUL CITADEL QUEST
    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_DUNGEON_DARKMAUL_CITADEL)
        {
            player->GetSession()->SendNotification("|cff00FF00We're sorry this quest is still in development!");
            ChatHandler(player->GetSession()).SendSysMessage("We're sorry this quest is still in development!");
            player->PlayDirectSound(44853, player, 84021); // Sorry, but we don't have that kind of time. xDDDD
            player->ForceCompleteQuest(QUEST_H_DUNGEON_DARKMAUL_CITADEL);
            Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_H_DUNGEON_DARKMAUL_CITADEL);
            player->RewardQuest(quest, LootItemType::Item, 0, player);
        }
    }
};

// npc_Warlord_Breka_Grimaxe_167675_q59984
struct npc_Warlord_Breka_Grimaxe_167675 : public ScriptedAI
{
    npc_Warlord_Breka_Grimaxe_167675(Creature* creature) : ScriptedAI(creature) { }

private:
    bool say;

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 100.0f))
                if (player->HasQuest(QUEST_H_DUNGEON_DARKMAUL_CITADEL))
                {
                    if (!say)
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(1, 723.528f, -1840.77f, 186.507f);
                        player->AddDelayedEvent(3000, [this, player]() -> void
                            {
                                if (Creature* shuja = player->FindNearestCreature(167676, 50.0f, true))
                                {
                                    shuja->SetWalk(true);
                                    shuja->GetMotionMaster()->MovePoint(1, 714.377f, -1865.16f, 186.753f);
                                }
                                me->GetMotionMaster()->MovePoint(2, 712.377f, -1859.51f, 186.966f);
                            });
                        say = true;
                    }
                }
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (point == 1)
        {
            Talk(0);

            if (Creature* bruktor = GetClosestCreatureWithEntry(me, 167670, 300.0f, true))
            {
                bruktor->GetMotionMaster()->MovePoint(1, 698.929f, -1889.12f, 192.311f);
                me->AddDelayedEvent(4000, [this, bruktor]() -> void
                    {
                        bruktor->GetMotionMaster()->MovePoint(2, 708.175f, -1869.58f, 188.727f);
                    });
                me->AddDelayedEvent(8000, [this, bruktor]() -> void
                    {
                        bruktor->AI()->Talk(0);
                    });
            }
        }

    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_H_AN_END_TO_BEGINNINGS)
            Talk(1);
    }

};

// npc_Horde_Wyvern_167669_q59985
struct npc_Horde_Wyvern_167669 : public ScriptedAI
{
    npc_Horde_Wyvern_167669(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool /*result*/)
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->HasQuest(QUEST_H_AN_END_TO_BEGINNINGS))
            {
                player->GetSceneMgr().PlaySceneByPackageId(SCENE_FLY_AWAY, SceneFlag::NotCancelable);
                player->AddDelayedEvent(60000, [this, player]() -> void
                    {
                        player->KilledMonsterCredit(167669);
                        player->TeleportTo(1, 1454.75f, -4421.74f, 25.4538f, 0.106463f);
                    });
            }
        }
    }
};

// go_catapult_351477_326651_q59980
struct go_catapult_351477_326651 : public GameObjectAI
{
    go_catapult_351477_326651(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        Position _positon = me->GetPosition();

        if (player->HasQuest(QUEST_H_CATAPULT_DESTRUCTION))
        {
            me->SetFlag(GO_FLAG_NOT_SELECTABLE);
            player->KillCreditGO(351477, me->GetGUID());
            player->SummonCreature(42046, _positon, TEMPSUMMON_TIMED_DESPAWN, 10min, 0);

            player->GetScheduler().Schedule(60s, [this, player](TaskContext context)
                {
                    me->DestroyForPlayer(player);
                });
        }

        if (player->HasQuest(QUEST_CATAPULT_DESTRUCTION))
        {
            me->SetFlag(GO_FLAG_NOT_SELECTABLE);
            player->KillCreditGO(326651, me->GetGUID());
            player->SummonCreature(42046, _positon, TEMPSUMMON_TIMED_DESPAWN, 10min, 0);

            player->GetScheduler().Schedule(60s, [this, player](TaskContext context)
                {
                    me->DestroyForPlayer(player);
                });
        }
        return true;
    }
};

// go_ogre_runestone_351476_339865
struct go_ogre_runestone_351476_339865 : public GameObjectAI
{
    go_ogre_runestone_351476_339865(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasQuest(QUEST_H_CONTROLLING_THEIR_STONES))
        {
            if (player->HasItemCount(168599) && player->HasItemCount(168600) && player->HasItemCount(168601))
            {
                me->SetFlag(GO_FLAG_NOT_SELECTABLE);
                player->KillCreditGO(351476, me->GetGUID());

                if (player->GetQuestObjectiveProgress(QUEST_H_CONTROLLING_THEIR_STONES, 4) == 3)
                {
                    player->ForceCompleteQuest(QUEST_H_CONTROLLING_THEIR_STONES);

                    player->GetScheduler().Schedule(5s, [player](TaskContext context)
                        {
                            if (Creature* thunderwalker = player->FindNearestCreature(167183, 30.0f, true))
                                thunderwalker->AI()->Talk(0);
                        });
                }
            }
        }

        if (player->HasQuest(QUEST_CONTROLLING_THEIR_STONES))
        {
            if (player->HasItemCount(168599) && player->HasItemCount(168600) && player->HasItemCount(168601))
            {
                me->SetFlag(GO_FLAG_NOT_SELECTABLE);
                player->KillCreditGO(339865, me->GetGUID());

                if (player->GetQuestObjectiveProgress(QUEST_CONTROLLING_THEIR_STONES, 4) == 3)
                {
                    if (Creature* kelra = player->FindNearestCreature(156954, 30.0f, true))
                    {
                        kelra->RemoveAura(305513);
                        if (kelra->IsHovering())
                        {
                            kelra->SetHoverHeight(1);
                            kelra->SetHover(false);
                        }
                        player->GetScheduler().Schedule(3s, [player, kelra](TaskContext context)
                            {
                                    kelra->AI()->Talk(0);
                            }).Schedule(9s, [player, kelra](TaskContext context)
                                {
                                    player->ForceCompleteQuest(QUEST_CONTROLLING_THEIR_STONES);
                                    kelra->DespawnOrUnsummon(1s, 30s);
                                });
                    }
                }
            }
        }

        return true;
    }
};

//npc_lightspawn_157114
class npc_lightspawn_157114 : public CreatureScript
{
public:
    npc_lightspawn_157114() : CreatureScript("npc_lightspawn_157114") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lightspawn_157114AI(creature);
    }

    struct npc_lightspawn_157114AI : public ScriptedAI
    {
        npc_lightspawn_157114AI(Creature* creature) : ScriptedAI(creature) { }

        void OnQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_FREEING_THE_LIGHT)
            {
                // Dispel the foul magic that binds me here. 
                me->Say(179206);
                me->PlayDirectSound(153065, player, 179206);

                player->CastSpell(player, SPELL_LIGHTSPAWN_LIGHTSBOON, true);
            }
        }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
        {
            if (quest->GetQuestId() == QUEST_FREEING_THE_LIGHT)
            {
                // Accept my blessing, and end the necromancy that plagues this island. 
                me->Say(179243);
                me->PlayDirectSound(153071, player, 179243);

                if (GameObject* pedestal1 = me->FindNearestGameObject(326716, 50.0f))
                    pedestal1->AI()->Reset();

                player->RemoveAura(SPELL_LIGHTSPAWN_LIGHTSBOON);
            }
        }

    };
};

struct go_ritual_predestal_326716_326717_326718_326719 : public GameObjectAI
{

    void Reset()
    {
        if (Creature* InvisBunny_OnPedestal = me->FindNearestCreature(NPC_LIGHTSPAWN_PEDESTAL_INVISBUNNY, 2.0f))
        {
            InvisBunny_OnPedestal->RemoveAllAuras();
            InvisBunny_OnPedestal->CastSpell(InvisBunny_OnPedestal, SPELL_LIGHTSPAWN_NECROTICBALL, true);
        }
    }

    go_ritual_predestal_326716_326717_326718_326719(GameObject* go) : GameObjectAI(go)
    {
        Reset();
    }

    bool OnGossipHello(Player* player) override
    {
        if (!player->IsActiveQuest(QUEST_FREEING_THE_LIGHT))
            return false;

        player->KillCreditGO(me->GetEntry());

        if (Creature* InvisBunny_OnPedestal = player->FindNearestCreature(NPC_LIGHTSPAWN_PEDESTAL_INVISBUNNY, 10.0f))
        {
            InvisBunny_OnPedestal->RemoveAllAuras();
            InvisBunny_OnPedestal->CastSpell(InvisBunny_OnPedestal, SPELL_LIGHTSPAWN_LIGHTBALL, true);

            InvisBunny_OnPedestal->DespawnOrUnsummon(120s, 5s); // despawn after 2min and respawn after 5s
        }

        if (Creature* LightSpawn = player->FindNearestCreature(157114, 50.0f))
        {
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 1) == 1)
            {
                // I attempted to bring an end to these rituals... 
                LightSpawn->Say(179202);
                LightSpawn->PlayDirectSound(153066, player, 179202);
            }
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 2) == 1)
            {
                // But the ogres ensnared me... drained my power... 
                LightSpawn->Say(179203);
                LightSpawn->PlayDirectSound(153067, player, 179203);
            }
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 3) == 1)
            {
                // Free me, so I might recover... and pass some of my power to you.  
                LightSpawn->Say(179204);
                LightSpawn->PlayDirectSound(153068, player, 179204);
            }
            if (player->GetQuestObjectiveProgress(QUEST_FREEING_THE_LIGHT, 4) == 1)
            {
                player->CastSpell(player, SPELL_LIGHTSPAWN_FREED, true);
                // The spell is weakening... the Light returns to me!  
                LightSpawn->Say(179205);
                LightSpawn->PlayDirectSound(153069, player, 179205);
            }
        }

        return true;
    }

};


//325364 - resizer blast
class spell_resizer_blast : public SpellScript
{
    PrepareSpellScript(spell_resizer_blast);

    void HandleDummy(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);

        Unit* caster = GetCaster();
        Vehicle* vehicle = caster->GetVehicleKit();
        Unit* passenger = vehicle->GetPassenger(0);
        Player* player = passenger->ToPlayer();

        if (WorldLocation* targetedloc = GetHitDest())
        {
            player->CastSpell(targetedloc->GetPosition(), 325365, true);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_resizer_blast::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//325365 - resizer blast explosion
class spell_resizer_blast_explosion : public SpellScript
{
    PrepareSpellScript(spell_resizer_blast_explosion);

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (WorldLocation* targetedloc = GetHitDest())
            if (TempSummon* tempSumm = caster->SummonCreature(WORLD_TRIGGER, targetedloc->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 3s))
            {
                tempSumm->SetFaction(caster->GetFaction());
                //PhasingHandler::AddPhase(tempSumm, 16752, true); // add zombie phase to explosion trigger for finding the zombies not sure if need

                std::list<Creature*> spellTargetCreatures = tempSumm->FindAllCreaturesInRange(15.0f);
                for (auto& target : spellTargetCreatures)
                {
                    if (target->IsAlive())
                        if (target->GetEntry() == 157091 || target->GetEntry() == 156532)
                        {
                            CastSpellExtraArgs args;
                            args.SetOriginalCaster(caster->GetGUID());
                            args.SetTriggerFlags(TRIGGERED_FULL_MASK);

                            tempSumm->CastSpell(target, 305721, args); // resizer blast explosion

                            if (caster->IsPlayer() && target->GetEntry() == 157091) // add kill credit for big zombie
                                caster->ToPlayer()->KilledMonsterCredit(165150);
                        }
                }
            }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_resizer_blast_explosion::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//344248 - exit vehicle
class spell_exit_vehicle_choppy : public SpellScript
{
    PrepareSpellScript(spell_exit_vehicle_choppy);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* choppy = GetCaster();

        choppy->ToCreature()->DespawnOrUnsummon(100ms);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_exit_vehicle_choppy::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};


void AddSC_zone_exiles_reach_horde()
{
    new exiles_reach_h();
    RegisterZoneScript(zone_exiles_reach_h);
    RegisterCreatureAI(warlord_breka_grimaxe_166573);
    new warlord_breka_grimaxe_166782();
    RegisterCreatureAI(warlord_breka_grimaxe_166906);
    RegisterCreatureAI(npc_Mithdran_Rastrealba_166791);
    RegisterCreatureAI(npc_Lana_Jordan_166796);
    RegisterCreatureAI(npc_Bo_166786);
    RegisterCreatureAI(npc_Jinhake_16680);
    RegisterCreatureAI(npc_Throg_166784);
    RegisterCreatureAI(grunt_throg_166583);
    RegisterCreatureAI(npc_wonsa_166854);
    RegisterCreatureAI(npc_wonsa_175030);
    RegisterCreatureAI(npc_Mithdran_Dawntracker_166996);
    RegisterCreatureAI(npc_combat_dummy);
    RegisterItemScript(item_first_aid_kit);
    RegisterGameObjectAI(go_campfire_339769);
    RegisterCreatureAI(npc_Cork_Fizzlepop_167019);
    RegisterCreatureAI(npc_Choppy_Booster_167027);
    RegisterCreatureAI(npc_Choppy_Booster_167142);
    RegisterCreatureAI(npc_Shuja_Grimaxe_167126);
    RegisterCreatureAI(npc_Shuja_Grimaxe_167219);
    RegisterCreatureAI(npc_Shuja_Grimaxe_167290);
    RegisterCreatureAI(npc_Bo_167291);
    new npc_Herbert_Gloomburst_167182();
    RegisterCreatureAI(npc_Lana_Jordan_167221);
    RegisterCreatureAI(npc_Lana_Jordan_167225);
    RegisterCreatureAI(npc_Crenna_Earth_Daughter_167254);
    RegisterCreatureAI(npc_Crenna_Earth_Daughter_166575);
    RegisterCreatureAI(npc_Warlord_Breka_Grimaxe_167212);
    RegisterCreatureAI(npc_Monstrous_Cadaver_157091);
    new item_h_resizer_v901();
    RegisterCreatureAI(npc_coulston_nereus);
    RegisterCreatureAI(npc_gutgruck_the_tough);
    RegisterAreaTriggerAI(at_for_gutgruck_the_tough);
    RegisterCreatureAI(hrun_the_exiled_156900);
    RegisterCreatureAI(npc_ogre_overseer_156676);
    RegisterCreatureAI(npc_quilboars_warrior_geomancer_150237_150238);
    RegisterCreatureAI(npc_gorgroth_153580);
    RegisterCreatureAI(npc_torgok_162817);
    RegisterGameObjectAI(go_thick_cocoon_339568_350796);
    RegisterGameObjectAI(go_harpy_totem_350803);
    RegisterCreatureAI(npc_Freed_Expedition_Member_167244_160964);
    RegisterCreatureAI(npc_harpie_snatcher_152571);
    RegisterCreatureAI(npc_harpie_boltcaller_152843);
    RegisterCreatureAI(npc_hunting_worg_152998);
    RegisterCreatureAI(npc_breka_grimaxe_167145);
    RegisterCreatureAI(npc_barrow_spider_160394);
    RegisterCreatureAI(npc_breka_grimaxe_167596);
    RegisterCreatureAI(npc_warlord_mulgrin_thunderwalker_167646);
    RegisterCreatureAI(npc_herbert_gloomburst_167598);
    RegisterCreatureAI(npc_Ogre_Shadowcaster_153242);
    RegisterCreatureAI(npc_Warlord_Breka_Grimaxe_167633);
    RegisterCreatureAI(npc_Warlord_Mulgrin_Thunderwalker_167183);
    RegisterCreatureAI(npc_Warlord_Breka_Grimaxe_167675);
    RegisterCreatureAI(npc_Horde_Wyvern_167669);
    RegisterGameObjectAI(go_catapult_351477_326651);
    RegisterGameObjectAI(go_ogre_runestone_351476_339865);
    new npc_lightspawn_157114();
    RegisterGameObjectAI(go_ritual_predestal_326716_326717_326718_326719);
    RegisterSpellScript(spell_resizer_blast);
    RegisterSpellScript(spell_resizer_blast_explosion);
    RegisterSpellScript(spell_exit_vehicle_choppy);
}
