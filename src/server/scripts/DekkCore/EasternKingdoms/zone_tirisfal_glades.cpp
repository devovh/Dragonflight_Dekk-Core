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

#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "InstanceScript.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "Vehicle.h"
#include "ScriptedGossip.h"

 /*######
 ## npc_calvin_montague
 ######*/

enum eCalvin
{
    SAY_COMPLETE = -1000431,
    SPELL_DRINK = 2639,                             // possibly not correct spell (but iconId is correct)
    QUEST_590 = 590,
    FACTION_HOSTILE = 168
};

class npc_calvin_montague : public CreatureScript
{
public:
    npc_calvin_montague() : CreatureScript("npc_calvin_montague") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_590)
        {
            creature->SetFaction(FACTION_HOSTILE);
            creature->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            CAST_AI(npc_calvin_montague::npc_calvin_montagueAI, creature->AI())->AttackStart(player);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_calvin_montagueAI(creature);
    }

    struct npc_calvin_montagueAI : public ScriptedAI
    {
        npc_calvin_montagueAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 m_uiPhase;
        uint32 m_uiPhaseTimer;
        uint64 m_uiPlayerGUID;

        void Reset()
        {
            m_uiPhase = 0;
            m_uiPhaseTimer = 5000;
            m_uiPlayerGUID = 0;

            me->RestoreFaction();

            if (!me->HasUnitFlag(UNIT_FLAG_IMMUNE_TO_PC))
                me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        }

        void JustEngagedWith(Unit* /*who*/) {}

        void AttackedBy(Unit* pAttacker)
        {
            if (me->GetVictim() || me->IsFriendlyTo(pAttacker))
                return;

            AttackStart(pAttacker);
        }

        void DamageTaken(Unit* pDoneBy, uint32& uiDamage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (uiDamage > me->GetHealth() || me->HealthBelowPctDamaged(15, uiDamage))
            {
                uiDamage = 0;

                me->RestoreFaction();
                me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                me->CombatStop(true);

                m_uiPhase = 1;

              //  if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
               //     m_uiPlayerGUID = pDoneBy->GetGUID();
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (m_uiPhase)
            {
                if (m_uiPhaseTimer <= uiDiff)
                    m_uiPhaseTimer = 7500;
                else
                {
                    m_uiPhaseTimer -= uiDiff;
                    return;
                }

                switch (m_uiPhase)
                {
                case 1:
                   // DoScriptText(SAY_COMPLETE, me);
                    ++m_uiPhase;
                    break;
                case 2:
                 //   if (Player* player = Unit::GetPlayer(*me, m_uiPlayerGUID))
                   //     player->AreaExploredOrEventHappens(QUEST_590);

                    DoCast(me, SPELL_DRINK, true);
                    ++m_uiPhase;
                    break;
                case 3:
                    EnterEvadeMode();
                    break;
                }

                return;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

};

/*######
## go_mausoleum_door
## go_mausoleum_trigger
######*/

enum eMausoleum
{
    QUEST_ULAG = 1819,
    NPC_ULAG = 6390,
    GO_TRIGGER = 104593,
    GO_DOOR = 176594
};

class go_mausoleum_door : public GameObjectScript
{
public:
    go_mausoleum_door() : GameObjectScript("go_mausoleum_door") { }

    struct go_mausoleum_doorAI : public GameObjectAI
    {
        go_mausoleum_doorAI(GameObject* go) : GameObjectAI(go) { }
    };
    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_mausoleum_doorAI(go);
    }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->GetQuestStatus(QUEST_ULAG) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (GameObject* pTrigger = player->FindNearestGameObject(GO_TRIGGER, 30.0f))
        {
            pTrigger->SetGoState(GO_STATE_READY);
            player->SummonCreature(NPC_ULAG, 2390.26f, 336.47f, 40.01f, 2.26f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 5min);
            return false;
        }

        return false;
    }

};

class go_mausoleum_trigger : public GameObjectScript
{
public:
    go_mausoleum_trigger() : GameObjectScript("go_mausoleum_trigger") { }

    struct go_mausoleum_triggerAI : public GameObjectAI
    {
        go_mausoleum_triggerAI(GameObject* go) : GameObjectAI(go) { }
    };
    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_mausoleum_triggerAI(go);
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_ULAG) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (GameObject* pDoor = player->FindNearestGameObject(GO_DOOR, 30.0f))
        {
            go->SetGoState(GO_STATE_ACTIVE);
            pDoor->RemoveFlag(GO_FLAG_INTERACT_COND);
            return true;
        }

        return false;
    }

};

// npc_darnell_49141
class npc_darnell_49141 : public CreatureScript
{
public:
    npc_darnell_49141() : CreatureScript("npc_darnell_49141") { }

    struct npc_darnell_49141AI : public ScriptedAI
    {
        npc_darnell_49141AI(Creature* creature) : ScriptedAI(creature) {}

        void JustAppeared() override
        {
            Player* player = me->GetOwner()->ToPlayer();

            me->AI()->Talk(0, player); // Greetings, $n.

            me->AddDelayedEvent(5000, [this, player]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1, 1679.657f, 1665.263f, 135.379f, true);
                    me->AI()->Talk(1, player); //The Shadow Grave is this way.  Follow me, $n.
                    me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                });
        }

        bool darnell_walking_down_the_stairs; bool darnell_searching_event; bool darnell_walking_up_the_stairs;

        void MoveInLineOfSight(Unit* who) override
        {
            //if the player want to follow darnell or done with the quest
            if (who->IsInDist(me, 10.0f) && who->IsPlayer() && me->GetPositionX() == 1679.657f)
            {
                me->GetMotionMaster()->MovePoint(1, 1668.624f, 1661.781f, 141.009f, true);

                Player* player = me->GetOwner()->ToPlayer();
            }

            //if the quest is complete
            if (who->IsInDist(me, 40.0f) && who->IsPlayer() && !darnell_walking_up_the_stairs)
            {
                Player* player = me->GetOwner()->ToPlayer();

                // if complete following player again
                if (player->GetQuestStatus(28608) == QUEST_STATUS_COMPLETE)
                {
                    me->AI()->ClearDelayedOperations();

                    darnell_walking_up_the_stairs = true;

                    me->AI()->Talk(9); // Nice work!  You've found them.  Let's bring these back to Mordo.

                    me->GetMotionMaster()->MovePoint(1, 1657.245f, 1678.168f, 120.791f, true);

                    // going up in the stairs before follow to not drop to the walls (looks bad)
                    me->AddDelayedEvent(4000, [this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(1, 1642.788f, 1678.248f, 126.932f, true);
                        });
                    me->AddDelayedEvent(8000, [this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(1, 1643.151f, 1663.210f, 132.479f, true);
                        });
                    me->AddDelayedEvent(12000, [this]() -> void
                        {
                            me->AI()->Talk(10); //I saw someone up there whose jaw fell off.I wonder if Mordo can fix him up ?
                            me->GetMotionMaster()->MovePoint(1, 1659.241f, 1662.052f, 141.850f, true);
                        });

                    me->AddDelayedEvent(16000, [this, player]() -> void
                        {
                            // then follow back
                            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());
                        });
                }
            }

            //if darnel got close to generic dummy spawn id 192770 - enters the grave
            if (who->IsInDist(me, 20.0f) && who->IsCreature() && !darnell_walking_down_the_stairs)
            {
                if (who->ToCreature()->GetSpawnId() == 192770)
                {
                    darnell_walking_down_the_stairs = true;

                    //walking down the stairs
                    me->AddDelayedEvent(5000, [this]() -> void
                        {
                            me->AI()->Talk(2); // This way!
                            me->GetMotionMaster()->MovePoint(1, 1659.241f, 1662.052f, 141.850f, true);
                        });
                    me->AddDelayedEvent(7000, [this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(1, 1643.151f, 1663.210f, 132.479f, true);
                        });
                    me->AddDelayedEvent(11000, [this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(1, 1642.788f, 1678.248f, 126.932f, true);
                        });
                    me->AddDelayedEvent(15000, [this]() -> void
                        {
                            me->GetMotionMaster()->MovePoint(1, 1656.928f, 1678.366f, 120.720f, true);
                        });
                }
            }

            //if darnel got close to generic dummy-guid 192772 - starts the searching event loop
            if (who->IsInDist(me, 10.0f) && who->IsCreature() && !darnell_searching_event)
            {
                if (who->ToCreature()->GetSpawnId() == 192772)
                {
                    Player* player = me->GetOwner()->ToPlayer();

                    darnell_searching_event = true;

                    me->AI()->Talk(3); // Now, where could those supplies be?


                    me->AddDelayedEvent(5000, [this, player]() -> void
                        {
                            if (player->GetQuestStatus(28608) == QUEST_STATUS_INCOMPLETE)
                            {
                                me->GetMotionMaster()->MovePoint(1, 1664.604f, 1662.552f, 120.719f, true);
                                me->AI()->Talk(4); // Maybe they're over here?
                            }
                        });

                    me->AddDelayedEvent(12000, [this, player]() -> void
                        {
                            if (player->GetQuestStatus(28608) == QUEST_STATUS_INCOMPLETE)
                            {
                                me->GetMotionMaster()->MovePoint(1, 1656.420f, 1686.860f, 120.719f, true);
                                me->AI()->Talk(5); // Hmm...
                            }
                        });

                    me->AddDelayedEvent(19000, [this, player]() -> void
                        {
                            if (player->GetQuestStatus(28608) == QUEST_STATUS_INCOMPLETE)
                            {
                                me->GetMotionMaster()->MovePoint(1, 1672.031f, 1668.890f, 120.719f, true);
                                me->AI()->Talk(6); // No, not over here.
                            }
                        });

                    me->AddDelayedEvent(26000, [this, player]() -> void
                        {
                            if (player->GetQuestStatus(28608) == QUEST_STATUS_INCOMPLETE)
                            {
                                me->GetMotionMaster()->MovePoint(1, 1664.243f, 1694.084f, 120.719f, true);
                                me->AI()->Talk(7, player); // Hey, give me a hand, $n!  I can't find the supplies that Mordo needed!
                            }
                        });

                    me->AddDelayedEvent(33000, [this, player]() -> void
                        {
                            if (player->GetQuestStatus(28608) == QUEST_STATUS_INCOMPLETE)
                            {
                                me->GetMotionMaster()->MovePoint(1, 1656.691f, 1668.233f, 120.719f, true);
                                me->AI()->Talk(8); // Let's see now... where could they be...
                            }
                        });

                    me->AddDelayedEvent(40000, [this, player]() -> void
                        {
                            if (player->GetQuestStatus(28608) == QUEST_STATUS_INCOMPLETE)
                            {
                                me->GetMotionMaster()->MovePoint(1, 1672.578f, 1686.981f, 120.719f, true);
                                me->AI()->Talk(3); // Now, where could those supplies be?
                            }
                        });
                }
            }

            // if the quest is rewarded darnell goes
            if (who->IsInDist(me, 40.0f) && who->IsPlayer())
            {
                Player* player = me->GetOwner()->ToPlayer();

                if (player->GetQuestStatus(28608) == QUEST_STATUS_REWARDED)
                    me->DespawnOrUnsummon(100ms);
            }

        }
    };


    CreatureAI* GetAI(Creature* pCreature) const  override
    {
        return new npc_darnell_49141AI(pCreature);
    }

};

// npc_darnell_49337
class npc_darnell_49337 : public CreatureScript
{
public:
    npc_darnell_49337() : CreatureScript("npc_darnell_49337") { }

    struct npc_darnell_49337AI : public ScriptedAI
    {
        npc_darnell_49337AI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void JustAppeared() override
        {
            Player* player = me->GetOwner()->ToPlayer();

            me->AI()->Talk(0, player); // Hello again.

            me->AddDelayedEvent(5000, [this, player]() -> void
                {
                    me->AI()->Talk(1); //I know the way to Deathknell. Come with me!
                });
        }

        bool talk2, talk3, talk5;

        void MoveInLineOfSight(Unit* who) override
        {
            // entering deathknell
            if (who->IsInDist(me, 10.0f) && who->IsCreature() && who->ToCreature()->GetSpawnId() == 192714 && !talk2)
            {
                me->AI()->Talk(2); //Good, you're still here. Now, where's Deathguard Saltain?
                talk2 = true;
            }
            // reaching saltain
            if (who->IsInDist(me, 20.0f) && who->IsCreature() && who->ToCreature()->GetEntry() == 1740 && !talk3)
            {
                talk3 = true;
                me->AI()->Talk(3); //Ah, here he is.

                Player* player = me->GetOwner()->ToPlayer();

                me->AddDelayedEvent(5000, [this, player]() -> void
                    {
                        me->AI()->Talk(4, player); //Let's get moving, $n. Saltain said that we'd fine some corpses up here.
                    });
            }
            // reaching a corpse nearly
            if (who->IsInDist(me, 10.0f) && who->IsCreature() && who->ToCreature()->GetEntry() == 49340 && !talk5)
            {
                talk5 = true;

                Player* player = me->GetOwner()->ToPlayer();

                me->AI()->Talk(5, player); //I think I see some corpses up ahead. Let's go, $n! You do the searching and fighting. I'll do the lifting.
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const  override
    {
        return new npc_darnell_49337AI(creature);
    }
};

// npc_scarlet_corpse_49340
class npc_scarlet_corpse_49340 : public CreatureScript
{
public:
    npc_scarlet_corpse_49340() : CreatureScript("npc_scarlet_corpse_49340") { }

    struct npc_scarlet_corpse_49340AI : public ScriptedAI
    {
        npc_scarlet_corpse_49340AI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);

            if (player->HasQuest(26800))
            {
                player->CastSpell(me, 91942); // select scarlet corpse

                if (Creature* darnell = player->FindNearestCreature(49337, 10.0f, true))
                {
                    // darnell goes to the corpse
                    darnell->GetMotionMaster()->MoveFollow(me, 0.1f, darnell->GetFollowAngle());
                    // darnell kicks up corpse to his shoulder
                    me->AddDelayedEvent(2000, [this, darnell]() -> void
                        {
                            darnell->CastSpell(me, 91945, true); // kicking up
                            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                            me->RemoveAura(92230);
                            me->RemoveAura(29266);
                            darnell->CastSpell(darnell, 91935, true); // bend over
                            //me->CastSpell(darnell, 46598, true); // NEEDTOCORRECT corpse don't want to be in darnells shoulder
                            darnell->CastSpell(me, 193710, true);
                        });
                    darnell->AddDelayedEvent(1000, [darnell, player]() -> void
                        {
                            darnell->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, darnell->GetFollowAngle());
                        });
                    me->DespawnOrUnsummon(120s); // safe option despawn after 2 min
                    me->SetRespawnDelay(120000); // respawn in 2 min for other players
                }
                else
                {
                    player->KilledMonsterCredit(49340);
                }
            }

            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const  override
    {
        return new npc_scarlet_corpse_49340AI(creature);
    }
};

// npc_deathguard_saltain_1740
class npc_deathguard_saltain_1740 : public CreatureScript
{
public:
    npc_deathguard_saltain_1740() : CreatureScript("npc_deathguard_saltain_49340") { }

    struct npc_deathguard_saltain_1740AI : public ScriptedAI
    {
        npc_deathguard_saltain_1740AI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void OnQuestReward(Player* player, Quest const* quest, LootItemType type, uint32 opt) override
        {
            if (quest->GetQuestId() == 26800)
            {
                if (Creature* darnell = player->FindNearestCreature(49337, 10.0f, true))
                {
                    darnell->DespawnOrUnsummon(100ms);
                    for (uint8 i = 1; i < 4; ++i)
                    {
                        if (auto pass = darnell->GetVehicleKit()->GetPassenger(i))
                            pass->ExitVehicle();
                    }
                }
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const  override
    {
        return new npc_deathguard_saltain_1740AI(creature);
    }
};

enum etargetnpc
{
    NPC_CAPTURED_PUDDLEJUMPER = 38923,
    NPC_CAPTURED_ORACLE = 39078,
    NPC_PUDDLEJUMPER = 1543,
    NPC_MINOR_ORACLE = 1544,
};

class spell_murloc_leash : public SpellScript
{
    PrepareSpellScript(spell_murloc_leash);

    SpellCastResult CheckRequirement()
    {
        std::list<uint32>targets;
        targets.push_back(NPC_CAPTURED_PUDDLEJUMPER);
        targets.push_back(NPC_CAPTURED_ORACLE);

        if (Player* player = GetCaster()->ToPlayer())
        {
            std::list<Creature*>npcs = player->FindNearestCreatures(targets, 10.0f);

            if (npcs.size() > 0)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        }

        if (Unit* unit = GetExplTargetUnit())
        {
            if (unit->GetEntry() == NPC_PUDDLEJUMPER || unit->GetEntry() == NPC_MINOR_ORACLE)
            {
                if (unit->GetHealthPct() < 80.0f)
                    return SPELL_CAST_OK;
            }
            else
                return SPELL_FAILED_BAD_TARGETS;
        }

        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
    }

    void CheckTarget(WorldObject*& target)
    {
        if (Creature* npc = target->ToCreature())
            if (npc->GetEntry() == NPC_PUDDLEJUMPER || npc->GetEntry() == NPC_MINOR_ORACLE)
                if (npc->GetHealthPct() < 80.0f)
                    return;

        target = NULL;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_murloc_leash::CheckRequirement);
        OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_murloc_leash::CheckTarget, EFFECT_0, TARGET_UNIT_TARGET_ANY);
    }
};

void AddSC_DekkCore_tirisfal_glades()
{
    new npc_calvin_montague();
    new go_mausoleum_door();
    new go_mausoleum_trigger();
    new npc_darnell_49141();
    new npc_darnell_49337();
    new npc_scarlet_corpse_49340();
    new npc_deathguard_saltain_1740();
    RegisterSpellScript(spell_murloc_leash); 
}
