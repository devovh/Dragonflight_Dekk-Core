/*
scenario_draenor_shadowmoon_valley
*/
#include "Scenario.h"
#include "GameObject.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "WorldStatePackets.h"
#include "PhasingHandler.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"

enum DataTypes
{
    DATA_BROKENSHORE = 0,
    DATA_STAGE_1 = 1,
    DATA_STAGE_2 = 2,
    DATA_STAGE_3 = 3,
    DATA_STAGE_4 = 4,

    DATA_FOLLOW_PROPHET_VELEN,
    DATA_DEFEAT_KRULL,
    DATA_WITNESS_COMMANDER_VORKA_S_ARRIVAL,
    DATA_SPEAK_TO_PROPHET_VELEN,

    DATA_EFEAT_ARNOKK_THE_BURNER,
    DATA_FIGHT_YOUR_WAY_TO_THE_KARABOR_HARBOR,
    DATA_PROTECT_KARA,
    DATA_STOP_THE_BOMBARDMENT_OF_KARABOR,

    DATA_MAX_ENCOUNTERS,

    QUEST_THE_FATE_OF_KARABOR = 33059,
    SPELL_PLAY_SCENE_END_CAMERA = 160931,
    SPELL_PLAY_SCENE_EPILOGUE = 161144,

    SCENARIO_ID_THE_FATE_OF_KARABOR = 450,//449
    PHASE_THE_FATE_OF_KARABOR = 3420,

    QUEST_THE_RIGHTEOUS_MARCH = 33255,
    QUEST_THE_DEFENSE_OF_KARABOR = 33256,

    SCENARIO_ID_THE_RIGHTEOUS_MARCH = 449,
    PHASE_THE_RIGHTEOUS_MARCH = 3460,

    SPELL_IRON_MARCH_BOMBING_RUN_SCENE = 149625,
    SPELL_WARM_UP_DEFENSE_OF_KARABOR_SCENARIO = 165490,
    SPELL_KARAS_TRIUMPH_VISUAL = 161115,
    SPELL_DEFENSE_OF_KARABOR_END_SCENE = 173681,
    SPELL_DEFENSE_OF_KARABOR_CELEBRATION_SCENE = 173694,

    IS_ALLIANCE = 0,
    IS_HORDE = 1,

    GO_IRON_HORDE_CAPSULE = 230565,
    GO_HARBOR_GATE = 232394
};

enum CriteriaTrees
{
    CRITERIA_TREE_FOLLOW_PROPHET_VELEN = 35613,//40063
    CRITERIA_TREE_DEFEAT_KRULL = 35628,//79560
    CRITERIA_TREE_WITNESS_COMMANDER_VORKA_S_ARRIVAL = 35615,//40093
    CRITERIA_TREE_SPEAK_TO_PROPHET_VELEN = 35631,//40075

    CRITERIA_TREE_EFEAT_ARNOKK_THE_BURNER = 35587,//75358
    CRITERIA_TREE_FIGHT_YOUR_WAY_TO_THE_KARABOR_HARBOR = 35589,//40077
    CRITERIA_TREE_PROTECT_KARA = 35591,//40209
    CRITERIA_TREE_STOP_THE_BOMBARDMENT_OF_KARABOR = 35704,//40049
};

enum Npcs
{
    NPC_PROPHET_VELEN_79522 = 79522,
    NPC_YREL_79521 = 79521,
    NPC_KRULL_79560 = 79560,
    NPC_COMMANDER_VORKA_74023 = 74023,
    NPC_ARNOKK_THE_BURNER_75358 = 75358,
    NPC_COMMANDER_VORKA_74715 = 74715,
    NPC_YREL_73395 = 73395,
    NPC_RAINDASH_80300 = 80300,
};


ObjectData const creatureData[] =
{
    {NPC_PROPHET_VELEN_79522,      NPC_PROPHET_VELEN_79522},
    {NPC_YREL_79521,               NPC_YREL_79521},
    {NPC_KRULL_79560,              NPC_KRULL_79560},
    {NPC_COMMANDER_VORKA_74023,    NPC_COMMANDER_VORKA_74023},
    {NPC_ARNOKK_THE_BURNER_75358,  NPC_ARNOKK_THE_BURNER_75358},
    {NPC_COMMANDER_VORKA_74715,    NPC_COMMANDER_VORKA_74715},
    {NPC_YREL_73395,               NPC_YREL_73395},
    {NPC_RAINDASH_80300,           NPC_RAINDASH_80300},
    {0,                         0} // END
};

ObjectData const gobjectData[] =
{
    {GO_HARBOR_GATE,        GO_HARBOR_GATE},
    {0,                     0 } // END
};

class scenario_draenor_shadowmoon_valley : public InstanceMapScript
{
public:
    scenario_draenor_shadowmoon_valley() : InstanceMapScript("scenario_draenor_shadowmoon_valley", 1277) { }

struct scenario_draenor_shadowmoon_valley_InstanceMapScript : public InstanceScript
{
    scenario_draenor_shadowmoon_valley_InstanceMapScript(InstanceMap* map) : InstanceScript(map) {}


    void Initialize() 
    {
        SetBossNumber(DATA_MAX_ENCOUNTERS);
        for (uint8 i = 1; i < DATA_MAX_ENCOUNTERS; ++i)
            SetData(i, NOT_STARTED);
        isIntr = false;
        KillCount = 0;
        LoadObjectData(creatureData, gobjectData);
    }

    //579.53 -3020.64 69.4958 1277 4.699142
    void OnPlayerEnter(Player* player) override
    {
        SetCheckPointPos(Position(579.53f, -3020.64f, 69.4958f, 4.699142f));
        if (player->HasQuest(QUEST_THE_RIGHTEOUS_MARCH))
            SetCheckPointPos(Position(1002.85f, -2930.59f, 99.3366f, 2.701752f));
        ClassType = player->IsInAlliance() ? IS_ALLIANCE : IS_HORDE;

        if (player->HasQuest(QUEST_THE_FATE_OF_KARABOR) || player->IsInAlliance())
            Phase = PHASE_THE_FATE_OF_KARABOR;

        if (player->HasQuest(QUEST_THE_RIGHTEOUS_MARCH) || player->GetQuestStatus(QUEST_THE_RIGHTEOUS_MARCH) == QUEST_STATUS_REWARDED)
            Phase = PHASE_THE_RIGHTEOUS_MARCH;

        player->GetScheduler().Schedule(3s, [=](TaskContext /*context*/)
        {
            PhasingHandler::RemovePhase(player, PHASE_THE_FATE_OF_KARABOR, true);
            PhasingHandler::RemovePhase(player, PHASE_THE_RIGHTEOUS_MARCH, true);
        });
        events.ScheduleEvent(DATA_FOLLOW_PROPHET_VELEN, 5s);
    }

    void Update(uint32 diff) override
    {
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case DATA_FOLLOW_PROPHET_VELEN:
        {
            if (Creature* velen = GetCreature(NPC_PROPHET_VELEN_79522))
            {
                if (GetData(DATA_FOLLOW_PROPHET_VELEN) == NOT_STARTED && ClassType == IS_ALLIANCE)
                {
                    SetData(DATA_FOLLOW_PROPHET_VELEN, IN_PROGRESS);
                    velen->AI()->Talk(0);
                    velen->GetMotionMaster()->MovePoint(1, Position(575.7344f, -3046.632f, 69.38943f), true);

                    velen->GetScheduler().Schedule(5s, [=](TaskContext /*context*/)
                    {
                        velen->GetMotionMaster()->MovePoint(2, Position(577.6111f, -3081.415f, 59.60399f), true);
                    });

                    if (Creature* yrel = GetCreature(NPC_YREL_79521))
                    {
                        yrel->GetScheduler().Schedule(5s, [=](TaskContext /*context*/)
                        {
                            yrel->AI()->Talk(0);
                        });
                        yrel->GetScheduler().Schedule(10s, [=](TaskContext /*context*/)
                        {
                            yrel->AI()->Talk(1);
                        });
                        yrel->GetScheduler().Schedule(15s, [=](TaskContext /*context*/)
                        {
                            yrel->AI()->Talk(2);
                          //  DoSendEventScenario(40063);
                        });
                    }

                }
            }

        }
        break;
        }
    }

    void DoCreatureMoveTo(Creature* npc, Position pos)
    {
        if (npc)
        {
            npc->GetMotionMaster()->Clear();
            npc->SetHomePosition(pos);
            npc->GetMotionMaster()->MovePoint(1, pos, true);
        }
    }

    void OnCompletedCriteriaTree(CriteriaTree const* tree) //override todo
    {
        switch (tree->ID)
        {
        case CRITERIA_TREE_FOLLOW_PROPHET_VELEN:
            SetData(DATA_FOLLOW_PROPHET_VELEN, DONE);
            if (Creature* krull = GetCreature(NPC_KRULL_79560))
            {
                krull->SetVisible(true);
              //  krull->GetMotionMaster()->MovePoint(1, krull->GetPositionWithDistInFront(15.0f), true);
                if (GameObject* door = GetGameObject(GO_IRON_HORDE_CAPSULE))
                    door->Delete();
            }
            break;
        case CRITERIA_TREE_DEFEAT_KRULL:
            break;
        case CRITERIA_TREE_WITNESS_COMMANDER_VORKA_S_ARRIVAL:
            break;
        case CRITERIA_TREE_SPEAK_TO_PROPHET_VELEN:
            break;
        case CRITERIA_TREE_EFEAT_ARNOKK_THE_BURNER:
            SetCheckPointPos(Position(1002.85f, -2930.59f, 99.3366f, 2.701752f));
            if (GameObject* door = GetGameObject(GO_HARBOR_GATE))
                door->Delete();
            break;
        case CRITERIA_TREE_FIGHT_YOUR_WAY_TO_THE_KARABOR_HARBOR:
            DoOnPlayers([=](Player* player)
            {
                player->GetScheduler().Schedule(15s, [=](TaskContext /*context*/)
                {
                //    DoSendEventScenario(40209);
                });
            });
            break;
        case CRITERIA_TREE_PROTECT_KARA:
            if (Creature* vorka = GetCreature(NPC_COMMANDER_VORKA_74715))
                vorka->AI()->DoAction(1);
            break;
        case CRITERIA_TREE_STOP_THE_BOMBARDMENT_OF_KARABOR:
            break;
        default:
            break;
        }
    }

    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);
        switch (creature->GetEntry())
        {
        case NPC_KRULL_79560:
            if (GetData(DATA_FOLLOW_PROPHET_VELEN) == DONE)
                creature->SetVisible(true);
            else
                creature->SetVisible(false);
            break;
        default:
            break;
        }
    }

    void OnUnitDeath(Unit* unit)
    {
        switch (unit->GetEntry())
        {
        case NPC_KRULL_79560:
            if (Creature* yrel = GetCreature(NPC_YREL_79521))
            {
                yrel->AI()->Talk(3);
            }
            DoUpdateCriteria(CriteriaType::KillCreature, NPC_KRULL_79560);
            //play visual            

            //delay

            //AddDelayedEvent(5000, [this](TaskContext /*context*/)
            //{             
         //   DoSendEventScenario(40093);
            if (Creature* yrel = GetCreature(NPC_YREL_79521))
            {
                yrel->AI()->Talk(4);
                yrel->GetScheduler().Schedule(5s, [=](TaskContext /*context*/)
                {
                    yrel->AI()->Talk(5);
                });
                yrel->GetScheduler().Schedule(15s, [this, yrel](TaskContext /*context*/)
                {
                    yrel->AI()->Talk(6);
                    DoCreatureMoveTo(yrel, Position(581.9635f, -3030.925f, 69.3974f));
                });
            }
            if (Creature* velen = GetCreature(NPC_PROPHET_VELEN_79522))
            {
                velen->GetScheduler().Schedule(20s, [this, velen](TaskContext /*context*/)
                {
                    velen->AI()->Talk(1);
                    DoCreatureMoveTo(velen, Position(576.354f, -3031.42f, 69.4728f));
                    velen->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                });
            }

            if (Creature* vorka = GetCreature(NPC_COMMANDER_VORKA_74023))
            {
                DoCreatureMoveTo(vorka, Position(553.9844f, -3177.292f, 82.88578f));

                vorka->GetScheduler().Schedule(5s, [this, vorka](TaskContext /*context*/)
                {
                    DoCreatureMoveTo(vorka, Position(559.533f, -3141.853f, 74.71755f));
                });

                vorka->GetScheduler().Schedule(10s, [this, vorka](TaskContext /*context*/)
                {
                    vorka->CastSpell(vorka, 160743, true);
                });
                vorka->GetScheduler().Schedule(20s, [this, vorka](TaskContext /*context*/)
                {
                    DoCreatureMoveTo(vorka, Position(579.149f, -3128.947f, 58.947f));
                });
            }
            //});
            break;
        case NPC_COMMANDER_VORKA_74715:
        {
            //AddDelayedEvent(10000, [this](TaskContext /*context*/)
            //
            if (Creature* npc = GetCreature(NPC_YREL_73395))
            {
                npc->GetMotionMaster()->MovePoint(1, Position(568.9647f, -3129.5573f, 58.292f, 4.7033f), true);

                npc->GetScheduler().Schedule(10s, [=](TaskContext /*context*/)
                {
                    npc->AI()->Talk(5);
                });
            }
            if (Creature* yudian = GetCreature(NPC_RAINDASH_80300))
            {
                yudian->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                yudian->GetMotionMaster()->MovePoint(1, Position(562.7f, -3118.629f, 58.295f, 4.781f), true);
            }
            //});
            break;
        }
        case 79843:
        case 79824:
        case 79827:
        case 79875:
            //case 79534:
        case 79855:
        case 79869:
        case 79879:
        default:
            DoUpdateCriteria(CriteriaType::KillCreature, 79534);
           // DoKilledMonsterCredit(QUEST_THE_DEFENSE_OF_KARABOR, 79534);
            break;
        }
    }
    
private:
    EventMap events;
    bool isIntr;
    std::set<ObjectGuid> m_guardMobsA;
    std::set<ObjectGuid> m_guardMobsH;
    std::set<ObjectGuid> m_targetMobs;

    uint32 KillCount;
    uint8 ClassMode;
    uint8 ClassType = 0;
    uint32 Phase;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new scenario_draenor_shadowmoon_valley_InstanceMapScript(map);
    }
  
};

struct npc_prophet_velen_79522 : public ScriptedAI
{
    npc_prophet_velen_79522(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
    }

    enum velen_Spells
    {
        SPELL_HOLY_SMITE = 59703,
    };

void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        damage = 0;
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_HOLY_SMITE, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case SPELL_HOLY_SMITE:
        {
            DoCast(SPELL_HOLY_SMITE);
            events.Repeat(4s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);

        Talk(2);
      //  instance->DoSendEventScenario(40075);

        if (Creature* vorka = instance->GetCreature(NPC_COMMANDER_VORKA_74023))
        {
            vorka->AI()->Talk(0);

            vorka->GetScheduler().Schedule(5s, [this, vorka](TaskContext /*context*/)
            {
                vorka->AI()->Talk(1);
            });
        }
        if (Creature* velen = instance->GetCreature(NPC_PROPHET_VELEN_79522))
        {
            velen->GetScheduler().Schedule(10s, [this, velen](TaskContext /*context*/)
            {
                velen->AI()->Talk(3);
            });
            velen->GetScheduler().Schedule(15s, [this, velen](TaskContext /*context*/)
            {
                instance->DoCastSpellOnPlayers(SPELL_PLAY_SCENE_END_CAMERA);
               // instance->DoKilledMonsterCredit(QUEST_THE_FATE_OF_KARABOR, 79803, velen->GetGUID());
            });
            velen->GetScheduler().Schedule(25s, [this, velen](TaskContext /*context*/)
            {
                instance->DoOnPlayers([](Player* player)
                {
                    player->TeleportTo(1116, Position(459.013f, -99.948f, 92.2936f, 0.6479203f));
                });
            });
        }
    }
private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_yrel_79521 : public ScriptedAI
{
    npc_yrel_79521(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
    }

    enum yrelSpells
    {
        SPELL_SHIELD_OF_VENGEANCE = 160387,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_SHIELD_OF_VENGEANCE, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case SPELL_SHIELD_OF_VENGEANCE:
        {
            DoCast(SPELL_SHIELD_OF_VENGEANCE);
            events.Repeat(5s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }
private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_krull_79560 : public ScriptedAI
{
    npc_krull_79560(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void Reset() override
    {
        if (instance->GetData(DATA_FOLLOW_PROPHET_VELEN) == DONE)
            me->SetVisible(true);
        else
            me->SetVisible(false);
    }

    enum Spells
    {
        SPELL_BRUTAL_SMASH = 176947,
        SPELL_LEAP = 177016,
    };

    void JustDied(Unit* /*killer*/) override
    {

    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_BRUTAL_SMASH, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        switch (events.ExecuteEvent())
        {
        case SPELL_BRUTAL_SMASH:
        {
            events.ScheduleEvent(SPELL_LEAP, 3s);
            DoCast(SPELL_BRUTAL_SMASH);
            events.Repeat(15s);
            break;
        }
        case SPELL_LEAP:
        {
            DoCast(SPELL_LEAP);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_commander_vorka_74023 : public ScriptedAI
{
    npc_commander_vorka_74023(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    enum vorka_spells
    {
        SPELL_FIRE_SUPPORT = 160743,
    };
    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_FIRE_SUPPORT, 10s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case SPELL_FIRE_SUPPORT:
            DoCast(SPELL_FIRE_SUPPORT);
            events.ScheduleEvent(SPELL_FIRE_SUPPORT, 20s);
            break;
        }
        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_prophet_velen_79043 : public ScriptedAI
{
    npc_prophet_velen_79043(Creature* creature) : ScriptedAI(creature) { }

      void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_FATE_OF_KARABOR)
        {
            Talk(0);
            me->GetScheduler().Schedule(10s, [player](TaskContext /*context*/)
            {
                player->KilledMonsterCredit(79802);
                player->SetScenarioId(SCENARIO_ID_THE_FATE_OF_KARABOR);
                player->TeleportTo(1277, 579.53f, -3020.64f, 69.4958f, 4.699142f);
            });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (me->IsWithinDist(player, 15.0f, false))
            {
                if (player->HasQuest(QUEST_THE_FATE_OF_KARABOR) && player->GetQuestStatus(QUEST_THE_FATE_OF_KARABOR) == QUEST_STATE_COMPLETE)
                {
                    player->CastSpell(player, SPELL_PLAY_SCENE_EPILOGUE, true);
                }
            }
        }
    }
};

/*QUEST_THE_RIGHTEOUS_MARCH start*/
struct npc_vindicator_maraad_75145 : public ScriptedAI
{
    npc_vindicator_maraad_75145(Creature* creature) : ScriptedAI(creature) { }

      void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_RIGHTEOUS_MARCH)
        {
            if (Creature* yral = me->FindNearestCreature(80514, 50.0f, true))
                yral->AI()->Talk(4);//broadcastid 82567
            player->GetScheduler().Schedule(5s, [this, player](TaskContext /*context*/)
            {
                player->CastSpell(player, SPELL_IRON_MARCH_BOMBING_RUN_SCENE, true);
            });
        }
    }
};

//class scene_iron_march_bombing_run : public PlayerScript
//{
//public:
//    scene_iron_march_bombing_run() : PlayerScript("scene_iron_march_bombing_run") {}
//
//    void OnSceneTriggerEvent(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/, std::string const& triggerName)
//    {
//        if (triggerName == "Credit")
//            player->KilledMonsterCredit(72458);
//        if (triggerName == "Teleport")
//        {
//            player->SetScenarioId(SCENARIO_ID_THE_RIGHTEOUS_MARCH);
//            player->CastSpell(player, 165484, true);
//            player->TeleportTo(1277, 1002.85f, -2930.59f, 99.3366f, 2.701752f);
//            player->GetScheduler().Schedule(10s, [this, player](TaskContext /*context*/)
//            {
//                if (player->HasQuest(QUEST_THE_RIGHTEOUS_MARCH) || player->GetQuestStatus(QUEST_THE_RIGHTEOUS_MARCH) == QUEST_STATUS_REWARDED)
//                    PhasingHandler::AddPhase(player, PHASE_THE_RIGHTEOUS_MARCH, true);
//            });
//        }
//    }
//};

struct npc_arnokk_the_burner_75358 : public ScriptedAI
{
    npc_arnokk_the_burner_75358(Creature* creature) : ScriptedAI(creature) {  }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(2);
    }

    enum arnokkSpells
    {
        SPELL_BURN = 149378,
        SPELL_FIERY_BLAST = 151732,
        SPELL_SCORCHED_EARTH = 151766,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        Talk(0);
        events.ScheduleEvent(SPELL_BURN, 5s);
        events.ScheduleEvent(SPELL_FIERY_BLAST, 10s);
        events.ScheduleEvent(SPELL_SCORCHED_EARTH, 15s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        events.Update(diff);
        switch (events.ExecuteEvent())
        {

        case SPELL_BURN:
        {
            DoCast(SPELL_BURN);
            events.Repeat(5s);
            break;
        }
        case SPELL_FIERY_BLAST:
        {
            Talk(1);
            DoCast(SPELL_FIERY_BLAST);
            events.Repeat(15s);
            break;
        }
        case SPELL_SCORCHED_EARTH:
        {
            Talk(3);
            DoCast(SPELL_SCORCHED_EARTH);
            events.Repeat(15s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }
    EventMap events;
};

struct npc_exarch_akama_72413 : public ScriptedAI
{
    npc_exarch_akama_72413(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_DEFENSE_OF_KARABOR)
        {
            Talk(1);
            if (player->GetMapId() == 1116)
            {
                player->SetScenarioId(SCENARIO_ID_THE_RIGHTEOUS_MARCH);
                player->TeleportTo(1277, 1002.85f, -2930.59f, 99.3366f, 2.701752f);
            }
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_THE_RIGHTEOUS_MARCH)
            Talk(0);
    }
};

struct npc_yrel_73395 : public ScriptedAI
{
    npc_yrel_73395(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
        //IsLock = false;
    }

    enum yrel_spells
    {
        SPELL_CONSECRATION = 107100,
        SPELL_DIVINE_STORM = 150674,
        SPELL_HAMMER_OF_JUSTICE = 13005,
        SPELL_HAMMER_OF_RIGHTEOUS = 150627,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_CONSECRATION, 5s);
        events.ScheduleEvent(SPELL_DIVINE_STORM, 5s);
        events.ScheduleEvent(SPELL_HAMMER_OF_JUSTICE, 5s);
        events.ScheduleEvent(SPELL_HAMMER_OF_RIGHTEOUS, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case SPELL_CONSECRATION:
            DoCast(SPELL_CONSECRATION);
            events.Repeat(10s);
        case SPELL_DIVINE_STORM:
            DoCast(SPELL_DIVINE_STORM);
            events.Repeat(10s);
        case SPELL_HAMMER_OF_JUSTICE:
            DoCast(SPELL_HAMMER_OF_JUSTICE);
            events.Repeat(15s);
        case SPELL_HAMMER_OF_RIGHTEOUS:
            DoCast(SPELL_HAMMER_OF_RIGHTEOUS);
            events.Repeat(10s);
            break;
        }
        DoMeleeAttackIfReady();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (me->IsWithinDist(player, 15.0f, false))
            {
                if (instance)
                {
                    if (/*!IsLock &&*/ player->HasQuest(QUEST_THE_DEFENSE_OF_KARABOR))
                    {
                     //   IsLock = true;
                        Talk(1);//78040
                      //  instance->DoSendEventScenario(40077);
                        player->AchieveCriteriaCredit(CRITERIA_TREE_FIGHT_YOUR_WAY_TO_THE_KARABOR_HARBOR);
                        player->GetScheduler().Schedule(15s, [this, player](TaskContext /*context*/)
                        {
                            Talk(4);//82584
                           // instance->DoSendEventScenario(40209);
                        });
                    }
                }
                else
                {
                    if (player->HasQuest(QUEST_THE_DEFENSE_OF_KARABOR))
                    {
                        player->CastSpell(player, SPELL_DEFENSE_OF_KARABOR_CELEBRATION_SCENE, true);
                    }
                }
            }
        }
    }
private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_commander_vorka_74715 : public ScriptedAI
{
    npc_commander_vorka_74715(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    enum commander_vorka_Spells
    {
        SPELL_BLAZING_SHOT = 150278,
        SPELL_FIRE_SUPPORT_BLACK_SKIES = 150273,
        SPELL_FIRE_SUPPORT_BLACKEST_SKIES = 151659,
        SPELL_GRAPESHOT_BLAST = 152247,
    };
    void DoAction(int32 param)
    {
        switch (param)
        {
        case 1:
            me->SetHomePosition(Position(579.4179f, -3126.022f, 58.9744f, 1.553f));
            me->GetMotionMaster()->MovePoint(1, Position(579.4179f, -3126.022f, 58.9744f, 1.553f), true);
            me->GetScheduler().Schedule(5s, [this](TaskContext /*context*/)
            {
                Talk(1);//82585
            });
            break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (instance)
        {
          //  instance->DoSendEventScenario(40049);
         //   instance->DoKilledMonsterCredit(QUEST_THE_DEFENSE_OF_KARABOR, 73772);
            if (Creature* kala = me->FindNearestCreature(79664, 200.0f, true))
                kala->GetMotionMaster()->MovePoint(1, Position(580.1214f, -3148.4792f, 59.524f, 4.7339f), true);
            instance->DoCastSpellOnPlayers(SPELL_KARAS_TRIUMPH_VISUAL);
        }
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        Talk(4);
        Talk(2);
        events.ScheduleEvent(SPELL_BLAZING_SHOT, 5s);
        events.ScheduleEvent(SPELL_FIRE_SUPPORT_BLACK_SKIES, 10s);
        events.ScheduleEvent(SPELL_FIRE_SUPPORT_BLACKEST_SKIES, 25s);
        events.ScheduleEvent(SPELL_GRAPESHOT_BLAST, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        events.Update(diff);
        switch (events.ExecuteEvent())
        {

        case SPELL_BLAZING_SHOT:
        {
            DoCast(SPELL_BLAZING_SHOT);
            events.Repeat(5s);
            break;
        }
        case SPELL_FIRE_SUPPORT_BLACK_SKIES:
        {
            DoCast(SPELL_FIRE_SUPPORT_BLACK_SKIES);
            events.Repeat(15s);
            break;
        }
        case SPELL_FIRE_SUPPORT_BLACKEST_SKIES:
        {
            DoCast(SPELL_FIRE_SUPPORT_BLACKEST_SKIES);
            events.Repeat(25s);
            break;
        }
        case SPELL_GRAPESHOT_BLAST:
        {
            DoCast(SPELL_GRAPESHOT_BLAST);
            events.Repeat(15s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_raindash_80300 : public ScriptedAI
{
    npc_raindash_80300(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* Clicker, bool /*spellClickHandled*/) override
    {
        if (Player* player = Clicker->ToPlayer())
        {
            player->CastSpell(player, SPELL_DEFENSE_OF_KARABOR_END_SCENE, true);

            player->GetScheduler().Schedule(10s, [this, player](TaskContext /*context*/)
            {
                player->TeleportTo(1116, 938.949f, -830.656f, -28.8338f, 3.961478f);
            });
            //938.949 -830.656 -28.8338 1116
        }
    }
};

void AddSC_scenario_draenor_shadowmoon_valley()
{
    new(scenario_draenor_shadowmoon_valley);
    RegisterCreatureAI(npc_prophet_velen_79522);
    RegisterCreatureAI(npc_yrel_79521);
    RegisterCreatureAI(npc_krull_79560);
    RegisterCreatureAI(npc_commander_vorka_74023);
    RegisterCreatureAI(npc_prophet_velen_79043);

    RegisterCreatureAI(npc_vindicator_maraad_75145);
    //new scene_iron_march_bombing_run();
    RegisterCreatureAI(npc_arnokk_the_burner_75358);
    RegisterCreatureAI(npc_exarch_akama_72413);
    RegisterCreatureAI(npc_yrel_73395);
    RegisterCreatureAI(npc_commander_vorka_74715);
    RegisterCreatureAI(npc_raindash_80300);
}
