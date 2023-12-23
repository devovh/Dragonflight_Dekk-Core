/*
 * Copyright (C) DekkCore
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

#include "shadowfang_keep.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "TemporarySummon.h"
#include "LFGMgr.h"
#include "Player.h"
#include "Group.h"

/*######
## apothecary hummel
######*/

enum Hummel
{
    FACTION_HOSTILE                  = 14,
    NPC_CRAZED_APOTHECARY            = 36568,
    NPC_VIAL_BUNNY                   = 36530,

    SAY_AGGRO_1                      = 0, // Did they bother to tell you who I am and why Iam doing this?
    SAY_AGGRO_2                      = 1, // ...or are they just using you like they do everybody else?
    SAY_AGGRO_3                      = 2, // But what does it matter. It is time for this to end.
    SAY_CALL_BAXTER                  = 3, // Baxter! Get in here and help! NOW!
    SAY_CALL_FRYE                    = 4, // It is time, Frye! Attack!
    SAY_SUMMON_ADDS                  = 5, // Apothecaries! Give your life for The Crown!
    SAY_DEATH_BAXTER                 = 0, // It has been the greatest honor of my life to server with you, Hummel.
    SAY_DEATH_FRYE                   = 0, // Great. We're nto gutless, we're incompetent.

    SPELL_ALLURING_PERFUME           = 68589,
    SPELL_ALLURING_PERFUME_SPRAY     = 68607,
    SPELL_IRRESISTIBLE_COLOGNE       = 68946,
    SPELL_IRRESISTIBLE_COLOGNE_SPRAY = 68948,

    SPELL_TABLE_APPEAR               = 69216,
    SPELL_SUMMON_TABLE               = 69218,
    SPELL_CHAIN_REACTION             = 68821,
    SPELL_UNSTABLE_REACTION          = 68957,

    // frye
    SPELL_THROW_PERFUME              = 68799,
    SPELL_THROW_COLOGNE              = 68841,
    SPELL_ALLURING_PERFUME_SPILL     = 68798,
    SPELL_IRRESISTIBLE_COLOGNE_SPILL = 68614,
};

enum Action
{
    START_INTRO,
    START_FIGHT,
    APOTHECARY_DIED,
    SPAWN_CRAZED
};

enum Phase
{
    PHASE_NORMAL,
    PHASE_INTRO
};

static Position Loc[]=
{
    // spawn points
    {-215.776443f, 2242.365479f, 79.769257f, 0.0f},
    {-169.500702f, 2219.286377f, 80.613045f, 0.0f},
    {-200.056641f, 2152.635010f, 79.763107f, 0.0f},
    {-238.448242f, 2165.165283f, 89.582985f, 0.0f},
    // moveto points
    {-210.784164f, 2219.004150f, 79.761803f, 0.0f},
    {-198.453400f, 2208.410889f, 79.762474f, 0.0f},
    {-208.469910f, 2167.971924f, 79.764969f, 0.0f},
    {-228.251511f, 2187.282471f, 79.762840f, 0.0f}
};

#define GOSSIP_ITEM_START "Begin the battle."

void SetInCombat(Creature* self)
{
    self->AI()->DoZoneInCombat(self);

    if (!self->IsInCombat())
        self->AI()->EnterEvadeMode();
}

class npc_apothecary_hummel : public CreatureScript
{
    public:
        npc_apothecary_hummel() : CreatureScript("npc_apothecary_hummel") { }

      /*  bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            // player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }*/

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF)
            {
               // player->CLOSE_GOSSIP_MENU();
                creature->AI()->DoAction(START_INTRO);
            }

            return true;
        }

        struct npc_apothecary_hummelAI : public ScriptedAI
        {
            npc_apothecary_hummelAI(Creature* creature) : ScriptedAI(creature), _summons(me)
            {
                _instance = creature->GetInstanceScript();
            }

            InstanceScript* _instance;
            SummonList _summons;
            uint8 _deadCount;
            uint8 _phase;
            uint8 _step;
            uint32 _aggroTimer;
            uint32 _stepTimer;
            uint32 _sprayTimer;
            uint32 _chainReactionTimer;
            bool _firstCrazed;

            void Reset()
            {
                me->RestoreFaction();
                me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                _phase = PHASE_NORMAL;
                _step = 0;
                _deadCount = 0;
                _stepTimer = 1500;
                _aggroTimer = 5000;
                _sprayTimer = urand(4000, 7000);
                _chainReactionTimer = urand(10000, 25000);
                _firstCrazed = false;

                me->SetCorpseDelay(900); 
                _summons.DespawnAll();

                if (!_instance)
                    return;

                /*if (Creature* baxter = ObjectAccessor::GetCreature(*me, _instance->GetData(DATA_BAXTER)))
                {
                    if (baxter->IsAlive())
                        baxter->AI()->EnterEvadeMode();
                    else
                        baxter->Respawn();
                }*/

              /*  if (Creature* frye = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_FRYE)))
                {
                    if (frye->IsAlive())
                        frye->AI()->EnterEvadeMode();
                    else
                        frye->Respawn();
                }

                if (GameObject* door = _instance->instance->GetGameObject(_instance->GetData64(DATA_COURTYARD_DOOR)))
                    _instance->HandleGameObject(0, true, door);*/
            }

            void DoAction(int32 action)
            {
                switch (action)
                {
                    case START_INTRO:
                    {
                      /*  if (Creature* baxter = ObjectAccessor::GetCreature(*me, _instance? _instance->GetData64(DATA_BAXTER) : 0))
                            baxter->AI()->DoAction(START_INTRO);
                        if (Creature* frye = ObjectAccessor::GetCreature(*me, _instance ? _instance->GetData64(DATA_FRYE) : 0))
                            frye->AI()->DoAction(START_INTRO);*/

                        _phase = PHASE_INTRO;
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                        SetInCombat(me);
                        break;
                    }
                    case START_FIGHT:
                    {
                        _phase = PHASE_NORMAL;
                        me->SetFaction(FACTION_HOSTILE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                        SetInCombat(me);
                        break;
                    }
                    case APOTHECARY_DIED:
                    {
                        ++_deadCount;
                        if (_deadCount > 2) // all 3 apothecarys dead, set lootable
                        {
                            _summons.DespawnAll();
                            me->SetCorpseDelay(90); // set delay
                            me->SetDynamicFlag(UNIT_DYNFLAG_LOOTABLE);

                        }
                        else
                        {
                            if (me->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE))
                                me->RemoveDynamicFlag(UNIT_DYNFLAG_LOOTABLE);
                        }
                        break;
                    }
                    case SPAWN_CRAZED:
                    {
                        uint8 i = urand(0, 3);
                    /*    if (Creature* crazed = me->SummonCreature(NPC_CRAZED_APOTHECARY, Loc[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3s))
                        {
                            crazed->setFaction(FACTION_HOSTILE);
                            crazed->SetReactState(REACT_PASSIVE);
                            crazed->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            crazed->GetMotionMaster()->MovePoint(1, Loc[i + 4]);
                        }*/

                        if (!_firstCrazed)
                        {
                            Talk(SAY_SUMMON_ADDS);
                            _firstCrazed = true;
                        }
                        break;
                    }
                }
            }

            void UpdateAI(uint32 diff)
            {
                if (_phase == PHASE_INTRO)
                {
                    if (_stepTimer <= diff)
                    {
                        ++_step;
                        switch (_step)
                        {
                            case 1:
                            {
                                Talk(SAY_AGGRO_1);
                                _stepTimer = 4000;
                                break;
                            }
                            case 2:
                            {
                                Talk(SAY_AGGRO_2);
                                _stepTimer = 5500;
                                break;
                            }
                            case 3:
                            {
                                Talk(SAY_AGGRO_3);
                                _stepTimer = 1000;
                                break;
                            }
                            case 4:
                            {
                                DoAction(START_FIGHT);
                                break;
                            }
                        }
                    }
                    else
                        _stepTimer -= diff;
                }
                else // PHASE_NORMAL
                {
                    if (!UpdateVictim())
                        return;

                    if (_aggroTimer <= diff)
                    {
                        SetInCombat(me);
                        _aggroTimer = 5000;
                    }
                    else
                        _aggroTimer -= diff;

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    if (_chainReactionTimer <= diff)
                    {
                        DoCast(me, SPELL_TABLE_APPEAR, true);
                        DoCast(me, SPELL_SUMMON_TABLE, true);
                        DoCast(SPELL_CHAIN_REACTION);
                        _chainReactionTimer = urand(15000, 25000);
                    }
                    else
                        _chainReactionTimer -= diff;

                    if (_sprayTimer <= diff)
                    {
                        DoCastVictim(SPELL_ALLURING_PERFUME_SPRAY);
                        _sprayTimer = urand(8000, 15000);
                    }
                    else
                        _sprayTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            }

            void JustSummoned(Creature* summon)
            {
                _summons.Summon(summon);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoAction(APOTHECARY_DIED);

              //  _instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, 36296, NULL);

              /*  Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                {
                    if (Group* group = players.begin()->GetSource()->GetGroup())
                        if (group->isLFGGroup())
                            sLFGMgr->FinishDungeon(group->GetGUID(), 288);
                } */
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_apothecary_hummelAI(creature);
        }
};

/*######
## npc_crazed_apothecary
######*/

class npc_crazed_apothecary : public CreatureScript
{
    public:
        npc_crazed_apothecary() : CreatureScript("npc_crazed_apothecary") { }

        struct npc_crazed_apothecaryAI : public ScriptedAI
        {
            npc_crazed_apothecaryAI(Creature* creature) : ScriptedAI(creature) { }

            void MovementInform(uint32 type, uint32 /*id*/)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                DoZoneInCombat(me);

                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 150.0f, true))
                {
                    _explodeTimer = urand (2500, 5000);
                    me->GetMotionMaster()->MoveFollow(target, 0.0f, float(2*M_PI*rand_norm()));
                }
                else
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (_explodeTimer <= diff)
                {
                    DoCast(me, SPELL_UNSTABLE_REACTION);
                }
                else
                    _explodeTimer -= diff;
            }

        private:
            uint32 _explodeTimer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_crazed_apothecaryAI(creature);
        }
};

void AddSC_boss_apothecary_trio()
{
    new npc_apothecary_hummel();
    new npc_crazed_apothecary();
}
