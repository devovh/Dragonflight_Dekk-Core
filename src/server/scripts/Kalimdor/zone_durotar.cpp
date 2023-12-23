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

#include "CreatureAIImpl.h"
#include "GameObject.h"
#include "MotionMaster.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "ScriptedGossip.h"

//dekkcore
#include "MoveSplineInit.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
//dekkcore

enum Durotar
{
    //orc quest
    QUEST_LOST_IN_THE_FLOODS = 25187,
    QUEST_THE_WOLF_AND_THE_KODO = 25205,

    //trolls start quest
    QUEST_A_TROLLS_TRUEST_COMPANION = 24622,
    QUEST_SAVING_THE_YOUNG = 24623,
    QUEST_NO_MORE_MERCY = 24812,
    QUEST_TERRITORIAL_FETISH = 24813,

    //Pandarian
    QUEST_JOINING_THE_HORDE = 31012,

    SPELL_LIGHTING_BOLT = 73254,
};


/*######
## Quest 37446: Lazy Peons
## npc_lazy_peon
######*/

enum LazyPeonYells
{
    SAY_SPELL_HIT                                 = 0
};

enum LazyPeon
{
    QUEST_LAZY_PEONS    = 37446,
    GO_LUMBERPILE       = 175784,
    SPELL_BUFF_SLEEP    = 17743,
    SPELL_AWAKEN_PEON   = 19938
};

class npc_lazy_peon : public CreatureScript
{
public:
    npc_lazy_peon() : CreatureScript("npc_lazy_peon") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lazy_peonAI(creature);
    }

    struct npc_lazy_peonAI : public ScriptedAI
    {
        npc_lazy_peonAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            RebuffTimer = 0;
            work = false;
        }

        uint32 RebuffTimer;
        bool work;

        void Reset() override
        {
            Initialize();
        }

        void MovementInform(uint32 /*type*/, uint32 id) override
        {
            if (id == 1)
                work = true;
        }

        void SpellHit(WorldObject* caster, SpellInfo const* spell) override
        {
            if (spell->Id != SPELL_AWAKEN_PEON)
                return;

            Player* player = caster->ToPlayer();
            if (player && player->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                Talk(SAY_SPELL_HIT, caster);
                me->RemoveAllAuras();
                if (GameObject* Lumberpile = me->FindNearestGameObject(GO_LUMBERPILE, 20))
                    me->GetMotionMaster()->MovePoint(1, Lumberpile->GetPositionX()-1, Lumberpile->GetPositionY(), Lumberpile->GetPositionZ());
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (work == true)
                me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);
            if (RebuffTimer <= diff)
            {
                DoCast(me, SPELL_BUFF_SLEEP);
                RebuffTimer = 300000; //Rebuff agian in 5 minutes
            }
            else
                RebuffTimer -= diff;
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    };
};

enum VoodooSpells
{
    SPELL_BREW      = 16712, // Special Brew
    SPELL_GHOSTLY   = 16713, // Ghostly
    SPELL_HEX1      = 16707, // Hex
    SPELL_HEX2      = 16708, // Hex
    SPELL_HEX3      = 16709, // Hex
    SPELL_GROW      = 16711, // Grow
    SPELL_LAUNCH    = 16716, // Launch (Whee!)
};

// 17009 - Voodoo
class spell_voodoo : public SpellScript
{
    PrepareSpellScript(spell_voodoo);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BREW, SPELL_GHOSTLY, SPELL_HEX1, SPELL_HEX2, SPELL_HEX3, SPELL_GROW, SPELL_LAUNCH });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        uint32 spellid = RAND(SPELL_BREW, SPELL_GHOSTLY, RAND(SPELL_HEX1, SPELL_HEX2, SPELL_HEX3), SPELL_GROW, SPELL_LAUNCH);
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, spellid, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_voodoo::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum Mithaka
{
    DATA_SHIP_DOCKED    = 1,
    GOSSIP_MENU_MITHAKA = 23225,
    GOSSIP_TEXT_MITHAKA = 35969
};

struct npc_mithaka : ScriptedAI
{
    npc_mithaka(Creature* creature) : ScriptedAI(creature), _shipInPort(false) { }

    void SetData(uint32 /*type*/, uint32 data) override
    {
        if (data == DATA_SHIP_DOCKED)
            _shipInPort = true;
        else
            _shipInPort = false;
    }

    bool OnGossipHello(Player* player) override
    {
        InitGossipMenuFor(player, GOSSIP_MENU_MITHAKA);
        if (!_shipInPort)
            AddGossipItemFor(player, GOSSIP_MENU_MITHAKA, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        SendGossipMenuFor(player, GOSSIP_TEXT_MITHAKA, me->GetGUID());
        return true;
    }
private:
    bool _shipInPort;
};

//39365 The Kodo q25205
struct npc_the_kodo_39365 : public ScriptedAI
{
public:
    npc_the_kodo_39365(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool kodo1;

    void Reset() override
    {
        kodo1 = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Vehicle* vehicle = target->GetVehicleKit())
            if (Unit* passenger = vehicle->GetPassenger(0))
                if (Player* player = passenger->ToPlayer())
                {
                    if (player->IsInDist(me, 30.0f))
                    {
                        if (player->GetQuestStatus(QUEST_THE_WOLF_AND_THE_KODO) == QUEST_STATUS_INCOMPLETE)
                            if (!kodo1)
                            {
                                kodo1 = true;
                                Position guardPos = me->GetPosition();
                                GetPositionWithDistInFront(me, 3.0f, guardPos);

                                float z = me->GetMap()->GetHeight(me->GetPhaseShift(), guardPos.GetPositionX(), guardPos.GetPositionY(), guardPos.GetPositionZ());
                                guardPos.m_positionZ = z;
                                if (Creature* kodo = me->SummonCreature(393650, guardPos))
                                {
                                    me->AI()->Talk(0);
                                    me->SetFacingToObject(kodo);
                                    kodo->SetFacingToObject(me);
                                    me->SetEmoteState(EMOTE_STATE_ATTACK1H);
                                    kodo->SetEmoteState(EMOTE_STATE_ATTACK1H);
                                    me->AddDelayedEvent(6000, [this]()
                                        {
                                            me->AI()->Talk(1);
                                        });
                                    me->AddDelayedEvent(8000, [this, kodo]()
                                        {
                                            kodo->AddAura(29266, kodo);
                                            kodo->DespawnOrUnsummon(60s);
                                            me->SetEmoteState(EMOTE_STATE_NONE);
                                            me->AI()->Talk(2);
                                        });
                                }
                            }
                    }
                }
    }
};

//73841 Hunting
class spell_kodo_hunting : public SpellScript
{
    PrepareSpellScript(spell_kodo_hunting);

   void HandleDummy(SpellEffIndex /*effIndex*/)
   {
        if (Unit* wolf = GetCaster())
            if (Vehicle* vehicle = wolf->GetVehicleKit())
                if (Unit* caster = vehicle->GetPassenger(0))
                    if (Player* player = caster->ToPlayer())
                        if (player->GetQuestStatus(QUEST_THE_WOLF_AND_THE_KODO) == QUEST_STATUS_INCOMPLETE)
                        {
                            std::list<Creature*> cList = player->FindNearestCreatures(39365, 15.0f);
                            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                            {
                                if (Creature* kodo = *itr)
                                    if (!kodo->HasAura(29266))
                                    {
                                        kodo->AI()->Talk(3);
                                        player->GetScheduler().Schedule(4s, [this, kodo, player](TaskContext context)
                                            {
                                                if (Creature* wolf = player->FindNearestCreature(39364, 10.0f))
                                                {
                                                    kodo->CastSpell(wolf, 73868);
                                                    wolf->AddAura(163091, wolf);    //stun_aura
                                                    player->KilledMonsterCredit(39365);
                                                }
                                                kodo->AI()->Talk(4);

                                            }).Schedule(10s, [this, kodo, player](TaskContext context)
                                                {
                                                    player->RemoveAura(73868);
                                                    player->NearTeleportTo(1289.54f, -4337.85f, 34.032f, 3.752f, false);
                                                    kodo->DespawnOrUnsummon(20s, 30s);
                                                });
                                    }
                            }
                        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_kodo_hunting::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

struct npc_ji_firepaw_60570 : public ScriptedAI
{
    npc_ji_firepaw_60570(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool say_grunt;
    bool say_shang;
    bool say_board;
    bool say_hellscream;
    ObjectGuid m_playerGUID;
    ObjectGuid gruntGUID;
    ObjectGuid shangGUID;
    ObjectGuid boardGUID;
    ObjectGuid hellscreamGUID;
    TaskScheduler _scheduler;

    void Reset() override
    {
        say_grunt = false;
        say_shang = false;
        say_board = false;
        say_hellscream = false;
        m_playerGUID.Clear();
        gruntGUID.Clear();
        shangGUID.Clear();
        boardGUID.Clear();
        hellscreamGUID.Clear();
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            m_playerGUID = player->GetGUID();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_JOINING_THE_HORDE)
        {
            if (TempSummon* firepaw = player->SummonCreature(60570, Position(1363.75f, -4372.51f, 26.1639f, 0.09552f), TEMPSUMMON_TIMED_DESPAWN, 60s))
            {
                firepaw->GetMotionMaster()->MoveFollow(player, 1.f, 210);
                me->DespawnOrUnsummon(1s, 60s);
            }
        }
    }


    void MoveInLineOfSight(Unit* who) override
    {
        if (Creature* creature = who->ToCreature())
        {
            if (creature->IsInDist2d(me, 15.0f) && creature->GetEntry() == 3296 && creature->GetSpawnId() == 250865)
            {
                if (!say_grunt)
                {
                    say_grunt = true;
                    if (Creature* grunt = me->FindNearestCreature(3296, 15.0f))
                    {
                        gruntGUID = grunt->GetGUID();
                        grunt->AI()->Talk(6);
                        _scheduler.Schedule(Milliseconds(6000), [this, grunt](TaskContext context)
                            {
                                grunt->AI()->Talk(7);
                            });
                    }
                }
            }
            if (creature->IsInDist2d(me, 20.0f) && creature->GetEntry() == 62195)
            {
                if (!say_shang)
                {
                    say_shang = true;
                    if (Creature* shang = me->FindNearestCreature(62195, 20.0f))
                    {
                        shangGUID = shang->GetGUID();
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            shang->AI()->Talk(0, player);
                        _scheduler.Schedule(Milliseconds(4000), [this, shang](TaskContext context)
                            {
                                shang->AI()->Talk(1);
                            });
                    }
                }
            }
            if (creature->IsInDist2d(me, 35.0f) && creature->GetEntry() == 173281 && creature->GetSpawnId() == 345967)
            {
                if (!say_board)
                {
                    say_board = true;
                    if (Creature* board = me->FindNearestCreature(173281, 35.0f))
                    {
                        boardGUID = board->GetGUID();
                        me->GetMotionMaster()->MovePoint(0, 1607.55f, -4387.67f, 20.4002f, true, 0.44012f);
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            _scheduler.Schedule(Milliseconds(4000), [this, player](TaskContext context)
                                {
                                    Talk(0);
                                }).Schedule(Milliseconds(10000), [this, player](TaskContext context)
                                    {
                                        Talk(1);
                                        me->GetMotionMaster()->MoveFollow(player, 1.f, 210);
                                    });
                        }
                    }
                }
            }
            if (creature->IsInDist2d(me, 30.0f) && creature->GetEntry() == 39605)
            {
                if (!say_hellscream)
                {
                    say_hellscream = true;
                    if (Creature* hellscream = me->FindNearestCreature(39605, 30.0f))
                    {
                        hellscreamGUID = hellscream->GetGUID();
                        hellscream->AI()->Talk(0);
                        me->GetMotionMaster()->MovePoint(0, 1664.59f, -4355.05f, 26.3569f, true, 5.9762f);
                        _scheduler.Schedule(Milliseconds(5000), [this](TaskContext context)
                            {
                                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                                Talk(2);
                            }).Schedule(Milliseconds(8000), [this, hellscream](TaskContext context)
                                {
                                    hellscream->AI()->Talk(1);
                                }).Schedule(Milliseconds(11000), [this, hellscream](TaskContext context)
                                    {
                                        hellscream->AI()->Talk(2);
                                    }).Schedule(Milliseconds(13500), [this, hellscream](TaskContext context)
                                        {
                                            me->SetStandState(UNIT_STAND_STATE_STAND);
                                            hellscream->AI()->Talk(3);
                                        });
                    }
                }
            }
            if (Creature* grunt = ObjectAccessor::GetCreature(*me, gruntGUID))
                if (!me->IsInDist2d(grunt, 50.0f))
                    say_grunt = false;
            if (Creature* shang = ObjectAccessor::GetCreature(*me, shangGUID))
                if (!me->IsInDist2d(shang, 50.0f))
                    say_shang = false;
            if (Creature* board = ObjectAccessor::GetCreature(*me, boardGUID))
                if (!me->IsInDist2d(board, 100.0f))
                    say_board = false;
            if (Creature* hellscream = ObjectAccessor::GetCreature(*me, hellscreamGUID))
                if (!me->IsInDist2d(hellscream, 100.0f))
                    say_hellscream = false;
        }
    }
};

void AddSC_durotar()
{
    new npc_lazy_peon();
    RegisterSpellScript(spell_voodoo);
    RegisterCreatureAI(npc_mithaka);
    RegisterCreatureAI(npc_the_kodo_39365);
    RegisterSpellScript(spell_kodo_hunting);
    RegisterCreatureAI(npc_ji_firepaw_60570);
}
