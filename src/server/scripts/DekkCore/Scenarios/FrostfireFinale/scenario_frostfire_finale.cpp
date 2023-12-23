/*
scenario_frostfire_finale
*/
#include "Scenario.h"
#include "GameObject.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "WorldStatePackets.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "QuestDef.h"
#include "TemporarySummon.h"

enum CriteriaTrees
{
    CRITERIA_TREE_SPEAK_WITH_DUROTAN = 34921, // 38980
    CRITERIA_TREE_SURVIVE_WAVE_1 = 34904, // 39002, 39003
    CRITERIA_TREE_KILL_TANK = 34906, // kill 76629; 39040
    CRITERIA_TREE_SURVIVE_WAVE_2 = 34908, // 39004, 39005
    CRITERIA_TREE_PROTECT_DREK_THAR = 34910, // 38944
    CRITERIA_TREE_SURVIVE_WAVE_3 = 34912, // 39006, 39007
    CRITERIA_TREE_KILL_THUNDERLORD_BOSS = 34914,// kill 76630;  39052
    CRITERIA_TREE_SURVIVE_WAVE_4 = 34916, // 39008, 39009
    CRITERIA_TREE_KILL_GRONN = 34882, // kill 76491

    DATA_WAVE_1_1 = 1,
    DATA_WAVE_1_2,
};

enum Npcs
{
    NPC_DUROTAN_76484 = 76484,
    NPC_DRAKA_76485 = 76485,
    NPC_FARSEER_DREK_THAR_76489 = 76489,
    NPC_GA_NAR_76486 = 76486,
    NPC_THRALL_76488 = 76488,

    NPC_IRON_CLUSTERPULT_76629 = 76629,
    NPC_MALGRIM_STORMHAND_76630 = 76630,
    NPC_MAGGOC_76491 = 76491,
    NPC_MAGGOC_88249 = 88249,
};

enum GameObjects
{
    GO_LANDSLIDE = 237557,
};

enum Spells
{
    SPELL_TURN_IN_SCENE = 154944,
    SPELL_FINALE_CINEMATIC = 175995, // frostfire - finale - cinematic

    SPELL_DAZED = 1604,
};

ObjectData const creatureData[] =
{
    {NPC_DUROTAN_76484,NPC_DUROTAN_76484},
    {NPC_DRAKA_76485,NPC_DRAKA_76485},
    {NPC_FARSEER_DREK_THAR_76489,NPC_FARSEER_DREK_THAR_76489},
    {NPC_GA_NAR_76486,NPC_GA_NAR_76486},
    {NPC_THRALL_76488, NPC_THRALL_76488},
    {NPC_IRON_CLUSTERPULT_76629,NPC_IRON_CLUSTERPULT_76629},
    {NPC_MALGRIM_STORMHAND_76630,NPC_MALGRIM_STORMHAND_76630},
    {NPC_MAGGOC_76491,NPC_MAGGOC_76491},
    {NPC_MAGGOC_88249,NPC_MAGGOC_88249},
    {0,                         0} // END
};

ObjectData const gobjectData[] =
{
    {GO_LANDSLIDE,GO_LANDSLIDE},
    {0,                     0 } // END
};

struct scenario_frostfire_finale : public InstanceScript
{
    scenario_frostfire_finale(InstanceMap* map) : InstanceScript(map) { }

    int32 wav_1, wav_2, wav_3, wav_4, wav_5, wav_6, wav_7, wav_8;

    void Initialize() 
    {
        LoadObjectData(creatureData, gobjectData);
    }

    void OnPlayerEnter(Player* player) override
    {
        SetCheckPointPos(Position(5975.919f, 2959.971f, 175.7909f, 5.505547f));
    }

    void OnCompletedCriteriaTree(CriteriaTree const* tree) //override
    {
        switch (tree->ID)
        {
        case  CRITERIA_TREE_SPEAK_WITH_DUROTAN:
            events.ScheduleEvent(CRITERIA_TREE_SPEAK_WITH_DUROTAN, 40s);
            if (Creature* iron = GetCreature(NPC_IRON_CLUSTERPULT_76629))
                iron->SetVisible(false);
            if (Creature* malgrim = GetCreature(NPC_MALGRIM_STORMHAND_76630))
            {
                malgrim->SetVisible(false);
                malgrim->SetReactState(ReactStates::REACT_PASSIVE);
            }
            if (Creature* maggoc = GetCreature(NPC_MAGGOC_76491))
                maggoc->SetVisible(false);
            if (Creature* maggoc = GetCreature(NPC_MAGGOC_88249))
                maggoc->SetVisible(false);
            if (GameObject* go = GetGameObject(GO_LANDSLIDE))
                go->setActive(false);
            break;
        case  CRITERIA_TREE_SURVIVE_WAVE_1:
            //2.28
            if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
            {
                durotan->AI()->Talk(2);//We can\'t fight with this cannon fire coming down. Thrall! $p! Take care of that tank!
            }
            if (Creature* thrall = GetCreature(NPC_THRALL_76488))
            {
                thrall->GetScheduler().Schedule(Milliseconds(7000), [=](TaskContext context)
                {
                    thrall->AI()->Talk(2);//I was taught by the best... Drek\'thar
                    thrall->SetHomePosition(Position(6047.64f, 2770.92f, 201.913f, 5.5635f));
                    thrall->GetMotionMaster()->MovePoint(1, Position(6047.64f, 2770.92f, 201.913f, 5.5635f));
                });
                thrall->GetScheduler().Schedule(Milliseconds(17000), [=](TaskContext context)
                {
                    thrall->AI()->Talk(3);//He would be pleased to hear you say that.
                });
            }
            break;
        case  CRITERIA_TREE_KILL_TANK:
            if (Creature* thrall = GetCreature(NPC_THRALL_76488))
            {
                thrall->SetHomePosition(Position(6023.34f, 2843.98f, 200.76f, 4.9f));
                thrall->GetMotionMaster()->MovePoint(1, Position(6023.34f, 2843.98f, 200.76f, 4.9f));
            }
            SummonWave1();
            break;
        case  CRITERIA_TREE_SURVIVE_WAVE_2:
            if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
            {
                durotan->AI()->Talk(3);//Drek\'Thar\'s in trouble. Ga\'nar, get out there and lend him a hand.        
            }
            SummonWaveToDerk();
            //154777/Bringing Down the Canyon
            if (Creature* drek = GetCreature(NPC_FARSEER_DREK_THAR_76489))
                drek->CastSpell(drek, 154777, true);
            break;
        case  CRITERIA_TREE_PROTECT_DREK_THAR:
            if (Creature* drek = GetCreature(NPC_FARSEER_DREK_THAR_76489))
                drek->AI()->Talk(5);//I owe you my life.
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
            {
                draka->GetScheduler().Schedule(Milliseconds(10000), [=](TaskContext context)
                {
                    draka->AI()->Talk(2);//I smell Thunderlords...
                });
            }
            if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
            {
                durotan->GetScheduler().Schedule(Milliseconds(14000), [=](TaskContext context)
                {
                    durotan->AI()->Talk(5);//Spears incoming! Hold your ground!
                });
            }
            events.ScheduleEvent(CRITERIA_TREE_SURVIVE_WAVE_3, 14s);
            break;
        case  CRITERIA_TREE_SURVIVE_WAVE_3:

            if (Creature* malgrim = GetCreature(NPC_MALGRIM_STORMHAND_76630))
            {
                malgrim->SetVisible(true);
                malgrim->setActive(true);
                malgrim->SetReactState(ReactStates::REACT_AGGRESSIVE);
                malgrim->GetMotionMaster()->MovePoint(1, Position(6056.82f, 2759.28f, 203.73f));              
            }


            //7.34
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
            {
                draka->AI()->Talk(3);//This one\'s mine.
            }
            if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
            {
                durotan->GetScheduler().Schedule(Milliseconds(5000), [=](TaskContext context)
                {
                    durotan->AI()->Talk(6);//I\'ll be waiting.
                });
            }
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
            {
                draka->GetScheduler().Schedule(Milliseconds(9000), [=](TaskContext context)
                {
                    draka->AI()->Talk(4);//Come back me up, $p
                    draka->GetMotionMaster()->MovePoint(1, Position(6047.64f, 2770.92f, 201.913f, 5.5635f));
                });
            }
            if (Creature* malgrim = GetCreature(NPC_MALGRIM_STORMHAND_76630))
            {
                malgrim->GetScheduler().Schedule(Milliseconds(15000), [=](TaskContext context)
                {
                    malgrim->AI()->Talk(0);//Look at this cute little puppy you've sent me, Durotan! Does she know any tricks?
                });
            }
            //7.58
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
            {
                draka->GetScheduler().Schedule(Milliseconds(23000), [=](TaskContext context)
                {
                    draka->AI()->Talk(5);//Yeah. A few.
                });
            }
            if (Creature* malgrim = GetCreature(NPC_MALGRIM_STORMHAND_76630))
            {
                malgrim->GetScheduler().Schedule(Milliseconds(28000), [=](TaskContext context)
                {
                    malgrim->AI()->Talk(1);//Come here, puppy. I want to play FETCH.
                });
            }
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
            {
                draka->GetScheduler().Schedule(Milliseconds(34000), [=](TaskContext context)
                {
                    draka->AI()->Talk(6);//Well... if it\'ll make you happy, then LET\'S PLAY!
                });
            }
            if (Creature* malgrim = GetCreature(NPC_MALGRIM_STORMHAND_76630))
            {
                malgrim->GetScheduler().Schedule(Milliseconds(40000), [=](TaskContext context)
                {
                    malgrim->AI()->Talk(2);//Draka... come join me in the Iron Horde.  Lose that armor, and settle down with a REAL MAN.
                });
            }
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
            {
                draka->GetScheduler().Schedule(Milliseconds(45000), [=](TaskContext context)
                {
                    draka->AI()->Talk(7);//A real man treats his mate with respect!
                });
                draka->GetScheduler().Schedule(Milliseconds(51000), [=](TaskContext context)
                {
                    draka->AI()->Talk(8);//...and he probably would have caught that axe.
                });
            }
            break;
        case  CRITERIA_TREE_KILL_THUNDERLORD_BOSS:
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
                draka->GetMotionMaster()->MoveTargetedHome();
            SummonWave1();
            //play visual
            break;
        case  CRITERIA_TREE_SURVIVE_WAVE_4:
            //final boss 
            if (Creature* maggoc = GetCreature(NPC_MAGGOC_76491))
            {
                maggoc->SetVisible(true);
                maggoc->setActive(true);
                maggoc->GetMotionMaster()->MovePoint(1, Position(6056.82f, 2759.28f, 203.73f));
            }
            if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
            {
                durotan->AI()->Talk(9);//GRONN!
                durotan->GetScheduler().Schedule(Milliseconds(2000), [=](TaskContext context)
                {
                    durotan->AI()->Talk(10);//All Frostwolves - charge ahead! Take that thing down!
                });
                durotan->SetInCombatWithZone();
                //all in
            }

            break;
        case  CRITERIA_TREE_KILL_GRONN:
            break;
        }
    }

    void Update(uint32 diff) override
    {
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case CRITERIA_TREE_SPEAK_WITH_DUROTAN:
        {
            if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
            {
                durotan->AI()->Talk(0);//We will hold the line here.           
            }
            if (Creature* draka = GetCreature(NPC_DRAKA_76485))
            {
                draka->GetScheduler().Schedule(Milliseconds(2000), [=](TaskContext context)
                {
                    draka->AI()->Talk(0);//Drek\'Thar, you can speak with the earth. Do you think you can collapse this canyon?
                });             
            }           
            if (Creature* drek = GetCreature(NPC_FARSEER_DREK_THAR_76489))
            {
                drek->GetScheduler().Schedule(Milliseconds(6000), [=](TaskContext context)
                {
                    drek->AI()->Talk(0);//It would be a momentous task... I cannot make any promises.
                    drek->SetHomePosition(Position(6025.78f, 2775.0f, 221.22f, 0.275f));
                    drek->GetMotionMaster()->MovePoint(1, Position(6025.78f, 2775.0f, 221.22f, 0.275f));
                });
            }
            if (Creature* ganar = GetCreature(NPC_GA_NAR_76486))
            {
                ganar->GetScheduler().Schedule(Milliseconds(14000), [=](TaskContext context)
                {
                    ganar->AI()->Talk(0);//I can. Shaman, if you bring that cliff wall down, I promise that our enemy will be waiting right underneath it.
                });
            }
            if (Creature* drek = GetCreature(NPC_FARSEER_DREK_THAR_76489))
            {
                drek->GetScheduler().Schedule(Milliseconds(24000), [=](TaskContext context)
                {
                    drek->AI()->Talk(1);//Then I will do my best.
                });
            }
            if (Creature* thrall = GetCreature(NPC_THRALL_76488))
            {
                thrall->GetScheduler().Schedule(Milliseconds(29000), [=](TaskContext context)
                {
                    thrall->AI()->Talk(0);//You\'d better hurry!  Here they come!
                    //wav 1
                });
            }
            events.ScheduleEvent(DATA_WAVE_1_1, 29s);
            //1.36
            if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
            {
                durotan->GetScheduler().Schedule(Milliseconds(34000), [=](TaskContext context)
                {
                    durotan->AI()->Talk(1);//Hold your position, Frostwolves! Let them come to you!
                });
            }
           
            break;
        }
        case CRITERIA_TREE_SURVIVE_WAVE_3:
        {
            SummonWaveStage6();
            break;
        }
        case DATA_WAVE_1_1:
        {
            SummonWave1();
            break;
        }
        default:
            break;
        }
    }

    void SummonWave1()
    {
        //5 * 76549
        Position pos = { 6045.31f, 2750.39f, 203.83f, 1.76f };
        Position targetpos = { 6027.73f, 2839.35f, 200.22f, 1.79f };
        for (uint8 i = 0; i < 5; i++)
            if (auto mob = instance->SummonCreature(76549, pos))
                mob->GetMotionMaster()->MovePoint(1, targetpos, true);
    }

    void SummonWaveToDerk()
    {
        //5 * 76549
        Position pos = { 6045.31f, 2750.39f, 203.83f, 1.76f };
        Position targetpos = { 6015.84f, 2769.24f, 222.0, 0.579f };

        for (uint8 i = 0; i < 5; i++)
            if (auto mob = instance->SummonCreature(76549, pos))
                mob->GetMotionMaster()->MovePoint(1, targetpos, true);
    }

    void SummonWaveStage6()
    {
        //1 * 76624
        Position pos = { 6042.31f, 2748.75f, 204.196f, 0.018f };
        Position targetpos = { 6027.73f, 2839.35f, 200.22f, 1.79f };
        if (auto mob = instance->SummonCreature(76624, pos))
            mob->GetMotionMaster()->MovePoint(1, targetpos, true);
        SummonWave1();
    }

    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);
        switch (creature->GetEntry())
        {
        case NPC_IRON_CLUSTERPULT_76629:
        case NPC_MALGRIM_STORMHAND_76630:
        case NPC_MAGGOC_76491:
        case NPC_MAGGOC_88249:
            creature->SetVisible(false);
            creature->setActive(false);
            creature->SetReactState(ReactStates::REACT_DEFENSIVE);
            break;
        default:
            break;
        }
    }

    void OnUnitDeath(Unit* unit)
    {
        switch (unit->GetEntry())
        {
        case 76549:
        case 76639:
        case 76960:
        case 76633:
        {      
            wav_1++;
            printf("count =%u\n", wav_1);
            if (wav_1 == 5)
            {
                //DoSendEventScenario(39002);
                if (Creature* ganar = GetCreature(NPC_GA_NAR_76486))
                    ganar->AI()->Talk(1);//Bring me more! Your Iron Horde can bleed on my axe!'
                SummonWave1();

                if (Creature* mob = GetCreature(NPC_IRON_CLUSTERPULT_76629))
                {
                    mob->SetVisible(true);
                    mob->GetMotionMaster()->MovePoint(1, Position(6056.82f, 2759.28f, 203.73f));
                }
            }
            if (wav_1 == 10)
            {
            //    DoSendEventScenario(39003);
            }
            if (wav_1 == 15)
            {
                //wav 2_1
              //  DoSendEventScenario(39004);
                if (Creature* mob = GetCreature(NPC_GA_NAR_76486))
                    mob->AI()->Talk(2);//Durotan, this isn\'t a choke point... it\'s a meat grinder!
                if (Creature* draka = GetCreature(NPC_DRAKA_76485))
                {
                    draka->GetScheduler().Schedule(Milliseconds(6000), [=](TaskContext context)
                    {
                        draka->AI()->Talk(1);//...and yet they still they keep coming.
                    });
                }
                if (Creature* mob = GetCreature(NPC_GA_NAR_76486))
                {
                    mob->GetScheduler().Schedule(Milliseconds(11000), [=](TaskContext context)
                    {
                        mob->AI()->Talk(3);//I know! That\'s the best part!
                    });
                }
                SummonWave1();
            }
            if (wav_1 == 20)
            {
             //   DoSendEventScenario(39005);
            }
            if (wav_1 == 25)
            {
          //      DoSendEventScenario(38944);
            }
            //stage 8
            if (wav_1 == 40)
            {
            //    DoSendEventScenario(39008);
                if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
                {
                    durotan->AI()->Talk(7);//Drek\'Thar! How much time do you need?    
                }
                if (Creature* drek = GetCreature(NPC_FARSEER_DREK_THAR_76489))
                {
                    drek->GetScheduler().Schedule(Milliseconds(2000), [=](TaskContext context)
                    {
                        drek->AI()->Talk(6);//Only a bit longer!
                    });
                }
                if (Creature* durotan = GetCreature(NPC_DUROTAN_76484))
                {
                    durotan->GetScheduler().Schedule(Milliseconds(6000), [=](TaskContext context)
                    {
                        durotan->AI()->Talk(8);//We are counting on you, shaman!
                    });
                }
                SummonWave1();
                SummonWave1();
            }
            if (wav_1 == 50)
            {
              //  DoSendEventScenario(39009);
            }
            break;
        }
        case 76624:
        {
            wav_5++;
            if (wav_5 == 1)
            {
             //   DoSendEventScenario(39006);
                //6.16
                if (Creature* thrall = GetCreature(NPC_THRALL_76488))
                {
                    thrall->AI()->Talk(4);//Here comes another elekk!
                }
                SummonWaveStage6();
            }
            if (wav_5 == 2)
            {
             //   DoSendEventScenario(39007);
            }
            break;
        }
        case NPC_IRON_CLUSTERPULT_76629:
        //    DoSendEventScenario(39040);
            break;
        case NPC_MALGRIM_STORMHAND_76630:
          //  DoSendEventScenario(39052);
            break;
        case NPC_MAGGOC_76491:
            DoCastSpellOnPlayers(SPELL_FINALE_CINEMATIC);

            if (Creature* maggoc = GetCreature(NPC_MAGGOC_76491))
                maggoc->SetVisible(false);
            if (Creature* maggoc = GetCreature(NPC_MAGGOC_88249))
                maggoc->SetVisible(true);
            if (GameObject* go = GetGameObject(GO_LANDSLIDE))
                go->setActive(true);

            DoOnPlayers([](Player* player)
            {
                player->TeleportTo(1116, Position(5962.268f, 2990.985f, 167.6576f, 1.902939f));
            });
            break;
        }
    }

private:
    EventMap events;
};
//
struct npc_durotan_76484 : public ScriptedAI
{
    npc_durotan_76484(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void Reset() override
    {
        me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
    }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
        //Q 34124
        player->KilledMonsterCredit(me->GetEntry());
        player->TalkedToCreature(me->GetEntry(), me->GetGUID());
        if (instance)
        {
            //if (instance->getScenarionStep() == 0)
            {
               // instance->DoSendEventScenario(38980);
                instance->DoCastSpellOnPlayers(SPELL_TURN_IN_SCENE);
            }
        }
    }
private:
    InstanceScript* instance;
};

struct npc_draka_76485 : public ScriptedAI
{
    npc_draka_76485(Creature* creature) : ScriptedAI(creature) {  }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        DoSpellAttackIfReady(38558);
    }
};

struct npc_farseer_drek_thar_76489 : public ScriptedAI
{
    npc_farseer_drek_thar_76489(Creature* creature) : ScriptedAI(creature) {  }

    //154777/bringing-down-the-canyon
    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        DoMeleeAttackIfReady();
    }
};

struct npc_thrall_76488 : public ScriptedAI
{
    npc_thrall_76488(Creature* creature) : ScriptedAI(creature) {  }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_DAZED,  5s);
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
            case SPELL_DAZED:
            {
                DoCast(SPELL_DAZED);
                events.Repeat(5s);
                break;
            }
        }
        DoMeleeAttackIfReady();
    }
    EventMap events;
};

struct npc_ga_nar_76486 : public ScriptedAI
{
    npc_ga_nar_76486(Creature* creature) : ScriptedAI(creature) {  }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_DAZED, 5s);
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
        case SPELL_DAZED:
        {
            DoCast(SPELL_DAZED);
            events.Repeat(5s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }
    EventMap events;
};

struct npc_malgrim_stormhand_76630 : public ScriptedAI
{
    npc_malgrim_stormhand_76630(Creature* creature) : ScriptedAI(creature) {  }

    enum malgrim_spells
    {
        SPELL_IMPALE = 148829,
        SPELL_MASSIVE_SHOCKWAVE = 148815,
        SPELL_DISENGAGE = 148842,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_MASSIVE_SHOCKWAVE,  10s);
        events.ScheduleEvent(SPELL_DISENGAGE, 15s);
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
            case SPELL_MASSIVE_SHOCKWAVE:
            {
                DoCast(SPELL_MASSIVE_SHOCKWAVE);
                events.Repeat(15s);
                break;
            }
            case SPELL_DISENGAGE:
            {
                DoCast(SPELL_DISENGAGE);
                events.Repeat(15s);
                break;
            }
        }

        DoSpellAttackIfReady(SPELL_IMPALE);
    }
    EventMap events;
};

struct npc_iron_clusterpult_76629 : public ScriptedAI
{
    npc_iron_clusterpult_76629(Creature* creature) : ScriptedAI(creature) {  }

    enum iron_spells
    {
        SPELL_CLUSTERPULT_SHOT = 155264,
        SPELL_LEFT_PISTON = 155243,
        SPELL_RIGHT_PISTON = 155255,
        SPELL_PUNCH = 155241,
    };
    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_CLUSTERPULT_SHOT, 5s);
        events.ScheduleEvent(SPELL_LEFT_PISTON, 10s);
        events.ScheduleEvent(SPELL_RIGHT_PISTON, 15s);
        events.ScheduleEvent(SPELL_PUNCH, 20s);
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
        case SPELL_CLUSTERPULT_SHOT:
        {
            DoCast(SPELL_CLUSTERPULT_SHOT);
            events.Repeat(5s);
            break;
        }
        case SPELL_LEFT_PISTON:
        {
            DoCast(SPELL_LEFT_PISTON);
            events.Repeat(15s);
            break;
        }
        case SPELL_RIGHT_PISTON:
        {
            DoCast(SPELL_RIGHT_PISTON);
            events.Repeat(15s);
            break;
        }
        case SPELL_PUNCH:
        {
            DoCast(SPELL_PUNCH);
            events.Repeat(15s);
            break;
        }
        }
    }
    EventMap events;
};

struct npc_maggoc_76491 : public ScriptedAI
{
    npc_maggoc_76491(Creature* creature) : ScriptedAI(creature) {  }

    enum maggoc_spells
    {
        SPELL_AVALANCHE_PALM = 155230,
        SPELL_WILD_THRASHING = 155290,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_AVALANCHE_PALM, 5s);
        events.ScheduleEvent(SPELL_WILD_THRASHING, 10s);
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
        case SPELL_AVALANCHE_PALM:
        {
            DoCast(SPELL_AVALANCHE_PALM);
            events.Repeat(15s);
            break;
        }
        case SPELL_WILD_THRASHING:
        {
            DoCast(SPELL_WILD_THRASHING);
            events.Repeat(15s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }
    EventMap events;
};

class q34124_teleport : public PlayerScript
{
public:
    q34124_teleport() : PlayerScript("q34124_teleport") { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 34124)
            player->TeleportTo(1203, 5975.919f, 2959.971f, 175.7909f, 5.505547f);
    }

};
void AddSC_scenario_frostfire_finale()
{
    RegisterInstanceScript(scenario_frostfire_finale, 1203);
    RegisterCreatureAI(npc_durotan_76484);
    RegisterCreatureAI(npc_draka_76485);
    RegisterCreatureAI(npc_farseer_drek_thar_76489);
    RegisterCreatureAI(npc_thrall_76488);
    RegisterCreatureAI(npc_ga_nar_76486);
    RegisterCreatureAI(npc_malgrim_stormhand_76630);
    RegisterCreatureAI(npc_iron_clusterpult_76629);
    RegisterCreatureAI(npc_maggoc_76491);
    new q34124_teleport();
}
