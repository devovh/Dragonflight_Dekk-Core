/*
 * Copyright 2023 DekkCore Team Devs
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

#include "CriteriaHandler.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "Scenario.h"
#include "Scenario_defending_the_primus.h"
#include "Vehicle.h"
#include "TemporarySummon.h"


ObjectData const creatureData[] =
{
    { NPC_PRIMUS,   DATA_PRIMUS },
    { 0,            0           }
};


class Scenario_defending_the_primus : public InstanceMapScript
{
public:
    Scenario_defending_the_primus() : InstanceMapScript("Scenario_defending_the_primus", 2471) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new Scenario_defending_the_primus_InstanceMapScript(map);
    }

    struct Scenario_defending_the_primus_InstanceMapScript : public InstanceScript
    {
         
        Scenario_defending_the_primus_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadObjectData(creatureData, nullptr);
        }

        void OnPlayerEnter(Player* player) override
        {
            if (!player)
                return;

            if (player->GetGUID() == ObjectGuid::Empty)
                return;


            if (player->GetAreaId() == 13547)
                if (player->GetQuestObjectiveProgress(QUEST_UNTANGLING_THE_SIGIL, 1) && !player->GetQuestObjectiveProgress(QUEST_UNTANGLING_THE_SIGIL, 0))
                {
                    if (Scenario* scenario = player->GetScenario())
                    {
                       if(!player->GetScenario()->IsCompletedStep(sScenarioStepStore.LookupEntry(SCENARIO_SPEAK_TO_THE_PRIMUS_STAGE_1)))
                        if (scenario->GetStep() == sScenarioStepStore.LookupEntry(SCENARIO_SPEAK_TO_THE_PRIMUS_STAGE_1))
                        {
                            player->KilledMonsterCredit(178505);
                        }
                    }
                }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (type)
            {
            case DATA_PRIMUS:
                if (state == DONE)
                {

                }
                break;
            default:
                break;
            }
            return true;
        }

            // Creatures
        ObjectGuid  heirmirGUID;
    };

};



class Scene_defending_the_primus : public SceneScript
{
public:
    Scene_defending_the_primus() : SceneScript("Scene_defending_the_primus") { }

    void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override {}

    // Called when a scene is canceled
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3313)
            DefendingThePrimusFinal(player);
    }

    // Called when a scene is completed
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3313)
            DefendingThePrimusFinal(player);
    }

    void DefendingThePrimusFinal(Player* player)
    {
        player->RemoveAura(SPELL_ROOT_SELF);
        player->RemoveAura(SPELL_CHAINED);
        if (Creature* primus = player->FindNearestCreature(NPC_PRIMUS, 100.f, true))
            primus->DespawnOrUnsummon(1s, 15s);
        if (Creature* jailer = player->FindNearestCreature(NPC_THE_JAILER, 50.f, true))
            jailer->ForcedDespawn();
        if (Creature* bh = player->FindNearestCreature(NPC_HEIRMIR, 50.f, true))
            bh->ForcedDespawn();
        if (Creature* bh = player->FindNearestCreature(NPC_ANDUIN, 50.f, true))
            bh->ForcedDespawn();
        player->KilledMonsterCredit(177570);
        player->ForceCompleteQuest(QUEST_UNTANGLING_THE_SIGIL);
        player->TeleportTo(2453, 2691.41f, 2099.32f, 318.11f, 0.084f);
    }
};


// 177570 The Primus
class npc_the_primus_177570 : public CreatureScript
{
public:
    npc_the_primus_177570() : CreatureScript("npc_the_primus_177570") { }

    struct npc_the_primus_177570AI : public  BossAI
    {
        npc_the_primus_177570AI(Creature* creature) : BossAI(creature, DATA_PRIMUS)
        {
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->SummonCreature(NPC_HEIRMIR, 2688.57f, 2104.67f, 317.48f, 6.05716f, TEMPSUMMON_MANUAL_DESPAWN);
            Initialize();
        }
    private:
        ObjectGuid m_playerGUID;
        ObjectGuid jailerGUID;
        ObjectGuid anduinGUID;
        ObjectGuid mawswornGUID[ENCOUNTER_MAWSWORN_NUMBER];
        EventMap _events;
        uint8 summon_num = 0;
        uint8 mawswornKilled;


        void Initialize()
        {
            mawswornKilled = 0;
        }

        void Reset() override
        {
            Initialize();
            m_playerGUID = ObjectGuid::Empty;
            jailerGUID = ObjectGuid::Empty;
            anduinGUID = ObjectGuid::Empty;
            _events.Reset();
        }

        void SetData(uint32 type, uint32 data) override
        {
            if (type != TYPE_MAWSWORN)
                return;

            if (data == DATA_MAWSWORN_KILLED)
            {
                ++mawswornKilled;
                if (mawswornKilled <= 20)
                    switch (mawswornKilled)
                    {
                    case 5:
                        SummonMawsworns(5);
                        Talk(3);
                        me->AddDelayedEvent(8000, [this]()->void
                            {
                                Talk(4);
                            });
                        me->AddDelayedEvent(13000, [this]()->void
                            {
                                Talk(5);
                            });
                        break;
                    case 10:
                        SummonMawsworns(5);
                        break;
                    case 15:
                        SummonMawsworns(4);
                        break;
                    case 19:
                        SummonAnduin();
                        break;
                    }
            }
        }

        bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
        {
            if (gossipListId == 0)
            {
                CloseGossipMenuFor(player);
                m_playerGUID = player-> GetGUID();
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                me->CastSpell(me, SPELL_PRIMUS_CASTING, false);
                SummonAnimaCells();
                Talk(0);
                me->AddDelayedEvent(5000, [this]()->void { Talk(1); });
                me->AddDelayedEvent(12000, [this]()->void
                    {
                        if (Creature* jailer = me->SummonCreature(NPC_THE_JAILER, 2745.68f, 2099.56f, 322.251f, 3.28394f, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            jailerGUID = jailer->GetGUID();
                            jailer->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                        }
                    });
            }
            return true;
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            switch (summon->GetEntry())
            {
            case 179191:
                me->AI()->SetData(TYPE_MAWSWORN, DATA_MAWSWORN_KILLED);
                break;
            case 179249:
                me->AI()->SetData(TYPE_MAWSWORN, DATA_MAWSWORN_KILLED);
                break;
            case 178445:
                me->AI()->SetData(TYPE_MAWSWORN, DATA_MAWSWORN_KILLED);
                break;
            }
        }
       
        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SPEAK_WITH_THE_PRIMUS:
                    SummonJailer();
                    break;
                }
            }
        }

        void SummonAnimaCells()         //Summons in multiple places with a time delay
        {
            me->GetScheduler().Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    if (summon_num < 8)
                    {
                        me->SummonCreature(NPC_ANIMA_CELL, anima_cell_pos[summon_num], TEMPSUMMON_TIMED_DESPAWN, 20s);
                        summon_num++;
                    }
                    else
                    {
                        _events.ScheduleEvent(EVENT_SPEAK_WITH_THE_PRIMUS, 6s);
                        return;
                    }
                    context.Repeat(2s);
                });
        }

        void SummonMawsworns(uint8 ENCOUNTER_MAWSWORN_NUMBER)
        {
            Position mawsworn = me->GetPosition();
            float o = (mawsworn.GetOrientation() - M_PI) + rand32() % 100;
           

            switch (urand(0, 1))
            {
            case 0:
                for (uint8 uiSummon_counter = 0; uiSummon_counter < ENCOUNTER_MAWSWORN_NUMBER; ++uiSummon_counter)
                {
                    uint8 num = urand(0, 2);
                    if (Creature* temp = me->SummonCreature(mawswornData[num], (mawsworn.GetPositionX() - 20) + rand32() % 15, (mawsworn.GetPositionY() - 20) + rand32() % 15, mawsworn.GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        if (temp->GetEntry() == NPC_MAWSWORN_DESTRUCTOR)
                        {
                            temp->CastSpell(me, SPELL_GORM_CHAINS);
                        }
                        mawswornGUID[uiSummon_counter] = temp->GetGUID();
                        
                    }
                }
                break;
            case 1:
                for (uint8 uiSummon_counter = 0; uiSummon_counter < ENCOUNTER_MAWSWORN_NUMBER; ++uiSummon_counter)
                {
                    uint8 num = urand(0, 2);
                    if (Creature* temp = me->SummonCreature(mawswornData[num], (mawsworn.GetPositionX() + 10) + rand32() % 10, (mawsworn.GetPositionY() + 10) + rand32() % 10, mawsworn.GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        if (temp->GetEntry() == NPC_MAWSWORN_DESTRUCTOR)
                        {
                            temp->CastSpell(me, SPELL_GORM_CHAINS);
                        }
                        mawswornGUID[uiSummon_counter] = temp->GetGUID();
                    }
                }
                break;
            }
        }

        void SummonJailer()        
        {
            me->RemoveAura(SPELL_PRIMUS_CASTING);
            if (Creature* jailer = ObjectAccessor::GetCreature(*me, jailerGUID))
            {
                me->AddDelayedEvent(3000, [this, jailer]()->void
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            player->PlayConversation(16869);
                        me->SetFacingToObject(jailer);
                    });
                me->AddDelayedEvent(6000, [this]()->void
                    {
                        Talk(2);
                    });
                me->AddDelayedEvent(9500, [jailer]()->void
                    {
                        jailer->AI()->Talk(1);
                    });
                me->AddDelayedEvent(13000, [this, jailer]()->void
                    {
                        if (Creature* bh = me->FindNearestCreature(NPC_HEIRMIR, 20.f, true))
                        {
                            bh->AI()->SetGUID(m_playerGUID, GUID_PLAYER);
                            bh->AI()->DoAction(1);
                        }
                        me->CastSpell(me, SPELL_PRIMUS_CASTING, false);
                        me->CastSpell(jailer, SPELL_BONDS_OF_BROTHERHOOD, false);
                        jailer->CastSpell(jailer, SPELL_PRIMUS_CASTING, false);
                        jailer->CastSpell(me, SPELL_BONDS_OF_BROTHERHOOD, false);
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            if (Scenario* scenario = player->GetScenario())
                            {
                                if (scenario->GetStep() == sScenarioStepStore.LookupEntry(SCENARIO_SPEAK_TO_THE_PRIMUS_STAGE_1))
                                {
                                    scenario->SetStepState(sScenarioStepStore.LookupEntry(SCENARIO_SPEAK_TO_THE_PRIMUS_STAGE_1), SCENARIO_STEP_IN_PROGRESS);
                                    scenario->CompleteStep(sScenarioStepStore.LookupEntry(SCENARIO_SPEAK_TO_THE_PRIMUS_STAGE_1));
                                    scenario->SetStep(sScenarioStepStore.LookupEntry(SCENARIO_DEFEAT_INCOMING_MAWSWORN_FORCES_STAGE_2));
                                }
                            }
                            me->AddAura(SPELL_CURIOUS_MIASMA, player);
                            SummonMawsworns(5);
                        }
                    });
            }
           
        }

        void SummonAnduin()
        {
            me->SummonCreature(NPC_MAW_PORTAL, 2737.69f, 2094.55f, 320.317f, 2.9333f, TEMPSUMMON_TIMED_DESPAWN, 10s);
            if (Creature* jailer = ObjectAccessor::GetCreature(*me, jailerGUID))
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (Creature* bh = player->FindNearestCreature(NPC_HEIRMIR, 50.f, true))
                        if (Creature* anduin = me->SummonCreature(NPC_ANDUIN, 2735.25f, 2098.54f, 320.136f, 3.12577f, TEMPSUMMON_TIMED_DESPAWN, 3min))
                        {
                            player->PlayConversation(16870);
                            player->CastSpell(player, SPELL_NECROTIC_RITUAL, true);
                            bh->AI()->DoAction(2);
                            player->RemoveAura(SPELL_CURIOUS_MIASMA);
                            player->GetMotionMaster()->MovePoint(0, 2733.92f, 2102.91f, 320.109f, true, 6.026f);
                            anduinGUID = anduin->GetGUID();
                            me->AddDelayedEvent(7000, [anduin, player, bh, jailer]()->void
                                {
                                    bh->AI()->DoAction(3);
                                    player->RemoveAura(SPELL_NECROTIC_RITUAL);
                                    player->CastSpell(player, SPELL_ROOT_SELF, false);
                                    player->CastSpell(player, SPELL_CHAINED,false);
                                    anduin->AI()->Talk(0);
                                    anduin->CastSpell(anduin, SPELL_MASS_RECLAIM, false);
                                });
                            me->AddDelayedEvent(17000, [player]()->void
                                {
                                    player->CastSpell(player, SCENE_EXIT_PRIMUS_SCENARIO, false);
                                });
                        }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_the_primus_177570AI(creature);
    }
};

// 178774 Bonesmith Heirmir
struct npc_bonesmith_heirmir_178774 : public  ScriptedAI
{
public:
    npc_bonesmith_heirmir_178774(Creature* creature) : ScriptedAI(creature) {}
private:
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        switch (id)
        {
        case GUID_PLAYER:
          m_playerGUID = guid;
            break;
        }
    }

    void DoAction(int32 action) override
    {
        if (action == 1)
        {
            Talk(1);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            me->AddAura(SPELL_CURIOUS_MIASMA, me);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                me->SetOwnerGUID(player->GetGUID());                   //set player onwer
                me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, 60.f, {}, MOTION_SLOT_ACTIVE);
                if (int32 playerFaction = player->GetFaction())
                    me->SetFaction(playerFaction);
            }
        }
        if (action == 2)
        {
            me->GetOwnerGUID().Clear();
            me->GetMotionMaster()->Clear();
            me->CastSpell(me, SPELL_NECROTIC_RITUAL, true);
            me->GetMotionMaster()->MovePoint(0, 2720.25f, 2101.83f, 317.48f, true, 0.12424f);
            me->RemoveAura(SPELL_CURIOUS_MIASMA);
        }
        if (action == 3)
        {
            me->RemoveAura(SPELL_NECROTIC_RITUAL);
            me->CastSpell(me, SPELL_ROOT_SELF, false);
            me->CastSpell(me, SPELL_CHAINED, false);
        }
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Creature* creature = target->ToCreature())
        {
            if (creature->IsInDist(me, 15.0f))
            {
                if (creature->IsAlive())
                    if (creature->IsHostileToPlayers())
                    {
                        me->GetThreatManager().AddThreat(creature, 1000.0f);
                        creature->GetThreatManager().AddThreat(me, 1000.0f);
                        me->Attack(creature, true);
                    }
            }
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (attacker->ToCreature())
            if (me->GetHealth() <= damage || me->GetHealthPct() <= 10.0f)
                damage = 0;
    }

    void KilledUnit(Unit* u) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            if (me->GetEntry() == NPC_MAWSWORN_RIPPER || me->GetEntry() == NPC_MAWSWORN_RAVAGER || me->GetEntry() == NPC_MAWSWORN_DESTRUCTOR)
                player->KilledMonsterCredit(NPC_MAWSWORN_RIPPER);
        }
    }
};

void AddSC_Scenario_defending_the_primus()
{
    new Scenario_defending_the_primus();
    new Scene_defending_the_primus();
    new npc_the_primus_177570();
    RegisterCreatureAI(npc_bonesmith_heirmir_178774);
}
