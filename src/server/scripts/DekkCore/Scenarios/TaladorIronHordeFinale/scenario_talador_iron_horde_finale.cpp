/*
scenario_talador_iron_horde_finale

*/
#include "Scenario.h"
#include "GameObject.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "WorldStatePackets.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "QuestDef.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "GameObjectAI.h"
#include <G3D/Vector3.h>
#include "TemporarySummon.h"

enum CriteriaTrees
{
    CRITERIA_TREE_STAGE_1 = 34944, // 42809
    CRITERIA_TREE_STAGE_2 = 39059, // 42033
    CRITERIA_TREE_STAGE_3 = 34946, // 42032, 42109
    CRITERIA_TREE_STAGE_4 = 34954, // 41623
    CRITERIA_TREE_STAGE_5 = 34956, // 41842, 40692, 39193
    CRITERIA_TREE_STAGE_6 = 36023, // 42038, 40688
};

enum Npcs
{
    //STAGE_4
    NPC_DUROTAN_77193 = 77193,
    NPC_DRAKA_77194 = 77194,
    NPC_ARCHMAGE_KHADGAR_77195 = 77195,
    NPC_VINDICATOR_MARAAD_77196 = 77196,
    NPC_YREL_77197 = 77197,
    //MOBS
    NPC_BURNING_BLADEMASTER_83807 = 83807,
    NPC_GROM_KAR_VANGUARD_83806 = 83806,
    NPC_GROM_KAR_GRIMSHOT_83805 = 83805,
    NPC_IRON_BRUTE_83492 = 83492,
    NPC_GROM_KAR_GRIMSHOT_83638 = 83638,
    NPC_GROM_KAR_VANGUARD_83637 = 83637,

    NPC_ORGRIM_DOOMHAMMER_77257 = 77257,
    NPC_MACHINIST_BRANDT_80962 = 80962,
    NPC_BLACKHAND_77256 = 77256,

    NPC_IRON_STAR_84170 = 84170,
    NPC_GENERIC_BUNNY_54638 = 54638,//center pos
    NPC_GENERIC_BUNNY_59113 = 59113,//random target
};

enum GameObjects
{
    GO_PORTAL_TO_TALADOR_ALLIANCE = 231620,
};

Position const portalPos = { 3598.398f, 3058.7058f, 1.94938f, 5.823f };

enum Spells
{
    SPELL_STAGE_4_01 = 167854,
    SPELL_STAGE_4_STOP_TIME_CAST = 167264,
    SPELL_STAGE_4_STOP_TIME = 167260,
    SPELL_STAGE_4_STOP_TIME_STUN = 167519,

    SPELL_MOVIE = 176320,
};

ObjectData const creatureData[] =
{
    {NPC_DUROTAN_77193,             NPC_DUROTAN_77193},
    {NPC_DRAKA_77194,               NPC_DRAKA_77194},
    {NPC_ARCHMAGE_KHADGAR_77195,    NPC_ARCHMAGE_KHADGAR_77195},
    {NPC_VINDICATOR_MARAAD_77196,   NPC_VINDICATOR_MARAAD_77196},
    {NPC_YREL_77197,                NPC_YREL_77197},
    {0,                             0} // END
};

ObjectData const gobjectData[] =
{
    {0,                     0 } // END
};
/*
    NPC_DUROTAN_77193 = 77193,
    NPC_DRAKA_77194 = 77194,
    NPC_ARCHMAGE_KHADGAR_77195 = 77195,
    NPC_VINDICATOR_MARAAD_77196 = 77196,
    NPC_YREL_77197 = 77197,
*/
Position const stage4CombatPos[6] =
{
    {3490.320f, 3130.2153f, 2.3598711f, 0.0f},
    {3491.2578f, 3128.197f, 2.2548304f, 0.0f},
    {3490.5999f, 3126.481f, 2.451581f, 0.0f},
    {3490.1035f, 3117.1875f, 1.839489f, 0.0f},
    {3491.1465f, 3120.3125f, 1.964489f, 0.0f},
    {3491.1465f, 3120.3125f, 1.964489f, 0.0f},
};


uint32 const stage4FriendNpc[5] =
{
    NPC_DUROTAN_77193,
    NPC_DRAKA_77194,
    NPC_ARCHMAGE_KHADGAR_77195,
    NPC_VINDICATOR_MARAAD_77196 ,
    NPC_YREL_77197
};
/*
NPC_BURNING_BLADEMASTER_83807 = 83807,
    NPC_GROM_KAR_VANGUARD_83806 = 83806,
    NPC_GROM_KAR_GRIMSHOT_83805 = 83805,
    NPC_IRON_BRUTE_83492 = 83492,
    NPC_GROM_KAR_GRIMSHOT_83638 = 83638,
    NPC_GROM_KAR_VANGUARD_83637 = 83637,
*/

uint32 const stage4AttackerNpc[6] =
{
    NPC_BURNING_BLADEMASTER_83807,
    NPC_GROM_KAR_VANGUARD_83806,
    NPC_GROM_KAR_GRIMSHOT_83805,
    NPC_IRON_BRUTE_83492 ,
    NPC_GROM_KAR_GRIMSHOT_83638,
    NPC_GROM_KAR_VANGUARD_83637
};

Position const stage4SummonPos[6] =
{
    {-12646.00f, -2292.10f, 2514.26f, 1.44f},
    {-12639.39f, -2293.25f, 2514.26f, 1.44f},
    {-12630.38f, -2293.25f, 2514.26f, 1.60f},
    {-12621.95f, -2293.79f, 2514.26f, 1.66f},
    {-12621.95f, -2293.79f, 2514.26f, 1.66f},
    {-12621.95f, -2293.79f, 2514.26f, 1.66f},
};

Position const stage5FirstPos[5] =
{
    {3606.54f, 3046.01f, 3.0f, 1.44f},
    {3606.54f, 3046.01f, 3.0f, 1.44f},
    {3602.5618f, 3042.7014f, 1.8254404f},
    {3606.54f, 3046.01f, 3.0f, 1.66f},
    {3606.54f, 3046.01f, 3.0f, 1.66f},
};
// 83515 cast 167468

Position const stage6KhadgarPos = { 3918.34f,2937.666f, 18.8634f, 4.9786f };

G3D::Vector3 stage5SencondPos[3] =
{
    {3617.12f, 3042.61f, 26.159f},
    {3866.236f, 2915.77f, 34.54f},
    {3929.7f, 2890.54f, 18.696f},
};
using Path01Size = std::extent<decltype(stage5SencondPos)>;

G3D::Vector3 stage6FirstPos[3] =
{
    {3882.08f, 2963.3f, 43.118f},
    {3762.3f, 3084.98f, 31.34f},
    {3735.797f, 3104.118f, 18.064f},
};
using Path02Size = std::extent<decltype(stage6FirstPos)>;

struct scenario_talador_iron_horde_finale : public InstanceScript
{
    scenario_talador_iron_horde_finale(InstanceMap* map) : InstanceScript(map) { }

    int32 wav_1, wav_2, wav_3, wav_4;
    std::vector<ObjectGuid> stageAttackers;

   /* void Initialize() override
    {
        LoadObjectData(creatureData, gobjectData);
    }*/

    void OnPlayerEnter(Player* player) override
    {
        SetCheckPointPos(Position(2998.3826f, 2666.6997f, 78.83f, 1.2955966f));
    }

    void OnCompletedCriteriaTree(CriteriaTree const* tree) //override
    {
        switch (tree->ID)
        {
        case CRITERIA_TREE_STAGE_1:  SetData(1, DONE);  break;
        case CRITERIA_TREE_STAGE_2:
            SetData(2, DONE);
            if (Creature* khadgar = GetCreature(NPC_ARCHMAGE_KHADGAR_77195))
                khadgar->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            break;
        case CRITERIA_TREE_STAGE_3:
            SetData(3, DONE);
            StartStage4();
            if (Creature* khadgar = GetCreature(NPC_ARCHMAGE_KHADGAR_77195))
                khadgar->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            break;
        case CRITERIA_TREE_STAGE_4:
            SetData(4, DONE);
            StartStage5();
            if (Creature* khadgar = GetCreature(NPC_ARCHMAGE_KHADGAR_77195))
            {
                khadgar->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                khadgar->AI()->Talk(0);
            }
            break;
        case CRITERIA_TREE_STAGE_5:
            SetData(5, DONE);
            if (Creature* khadgar = GetCreature(NPC_ARCHMAGE_KHADGAR_77195))
            {
                khadgar->SetHomePosition(stage6KhadgarPos);
                khadgar->NearTeleportTo(stage6KhadgarPos);
                khadgar->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                khadgar->AI()->Talk(1);
            }
            break;
        case CRITERIA_TREE_STAGE_6:  SetData(6, DONE); break;
        }
    }

    void StartStage4()
    {
        for (uint8 i = 0; i < 5; ++i)
            if (Creature* npc = GetCreature(stage4FriendNpc[i]))
            {
                npc->SetHomePosition(stage4CombatPos[i]);
                npc->GetMotionMaster()->MovePoint(1, stage4CombatPos[i]);
            }
        std::list<Creature*> list;

        if (Creature* khadgar = GetCreature(NPC_ARCHMAGE_KHADGAR_77195))
        {
            khadgar->CastSpell(khadgar, SPELL_STAGE_4_01, true);
            for (uint8 i = 0; i < 6; ++i)
                GetCreatureListWithEntryInGrid(list, khadgar, stage4AttackerNpc[i], 50.0f);

            if (!list.empty())
                for (Creature* attacker : list)
                    for (uint8 i = 0; i < 6; ++i)
                        if (attacker->IsAlive() && attacker->GetEntry() == stage4AttackerNpc[i])
                        {
                            attacker->GetMotionMaster()->Clear();
                            attacker->GetMotionMaster()->MovePoint(1, stage4CombatPos[i]);
                        }
        }

        //conversation 55
        if (Creature* khadgar = GetCreature(NPC_ARCHMAGE_KHADGAR_77195))
        {
            khadgar->GetScheduler().Schedule(Milliseconds(100000), [=](TaskContext context)
            {
                khadgar->CastStop();
                khadgar->CastSpell(khadgar, SPELL_STAGE_4_STOP_TIME_CAST, true);
               // DoSendEventScenario(41623);
            });
        }

        //start 
    }

    void StartStage5()
    {
        if (Creature* khadgar = GetCreature(NPC_ARCHMAGE_KHADGAR_77195))
        {
            //cast spell stop combat
            khadgar->GetScheduler().Schedule(Milliseconds(1000), [=](TaskContext context)
            {
                std::list<Creature*> list;
                for (uint8 i = 0; i < 6; ++i)
                    GetCreatureListWithEntryInGrid(list, khadgar, stage4AttackerNpc[i], 500.0f);
                GetCreatureListWithEntryInGrid(list, khadgar, 84359, 500.0f);
                GetCreatureListWithEntryInGrid(list, khadgar, 83811, 500.0f);

                if (!list.empty())
                    for (Creature* attacker : list)
                        if (attacker->IsAlive())
                            khadgar->AddAura(SPELL_STAGE_4_STOP_TIME_STUN, attacker);
            });
            //delay move to new pos;
        //stage5FirstPos
            khadgar->GetScheduler().Schedule(Milliseconds(4500), [=](TaskContext context)
            {
                for (uint8 i = 0; i < 5; ++i)
                    if (Creature* npc = GetCreature(stage4FriendNpc[i]))
                    {
                        npc->AttackStop();
                        npc->SetHomePosition(stage5FirstPos[i]);
                        npc->GetMotionMaster()->Clear();
                        npc->GetMotionMaster()->MovePoint(1, stage5FirstPos[i]);
                    }
            });
        }
    }

    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);
        switch (creature->GetEntry())
        {
        case NPC_BURNING_BLADEMASTER_83807:
        case NPC_GROM_KAR_VANGUARD_83806:
        case NPC_GROM_KAR_GRIMSHOT_83805:
        case NPC_IRON_BRUTE_83492:
        case NPC_GROM_KAR_GRIMSHOT_83638:
        case NPC_GROM_KAR_VANGUARD_83637:
            stageAttackers.push_back(creature->GetGUID());
            break;
        default:
            break;
        }
    }

    void OnUnitDeath(Unit* unit)
    {
        switch (unit->GetEntry())
        {
        case NPC_BURNING_BLADEMASTER_83807:
        case NPC_GROM_KAR_VANGUARD_83806:
        case NPC_GROM_KAR_GRIMSHOT_83805:
        case NPC_IRON_BRUTE_83492:
        case NPC_GROM_KAR_GRIMSHOT_83638:
        case NPC_GROM_KAR_VANGUARD_83637:
            wav_4++;
            if (wav_4 == 18)
            {
                //DoSendEventScenario(41623);
            }
            break;
        case NPC_MACHINIST_BRANDT_80962:
          //  DoSendEventScenario(39193);
            break;
        case NPC_BLACKHAND_77256:
           // DoSendEventScenario(40688);
            instance->SummonGameObject(GO_PORTAL_TO_TALADOR_ALLIANCE, portalPos, QuaternionData(0.0f, 0.0f, 0.0f, 0.0f), 0);
            DoCastSpellOnPlayers(SPELL_MOVIE);
            //Death 
            if (Creature* maraad = GetCreature(NPC_VINDICATOR_MARAAD_77196))
                maraad->AddAura(159474, maraad);
            break;
        default:
            break;
        }
    }

private:
    EventMap events;
};
// 77192, 77191
struct npc_scenario_talador_stage_1 : public ScriptedAI
{
    npc_scenario_talador_stage_1(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
      //  instance->DoSendEventScenario(42809);
        //86671
        if (Creature* npc = me->FindNearestCreature(86671, 50.0f))
        {
            npc->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
            npc->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        }
    }
private:
    InstanceScript* instance;
};

struct npc_iron_star_86671 : public ScriptedAI
{
    npc_iron_star_86671(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool /*spellClickHandled*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (InstanceScript* instance = me->GetInstanceScript())
             //   instance->DoSendEventScenario(42033);
            //173525 cast  173524   86712 ()
            if (auto npc = player->SummonCreature(86712, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s))
            {
                npc->AddAura(155693, npc);
                npc->AddAura(173436, npc);
                player->CastSpell(npc, 46598, true);
            }
            //at aura 155693/iron-star-area-trigger Area Trigger (3219) dmg 173428 Fiery Death
            //OBJECT_FIELD_GUID: Full: 0x3031E496E007B4C00061E20000401A9C AreaTrigger/0 R3193/S25058 Map: 1207 Entry: 7891 Low: 4201116
            //3219	155693	7891	0	0	6	6	7	7	2.5	2.5	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	???? ???????? ??????

            //173436 quick
        }
    }
};
//at 7891 155693/iron-star-area-trigger Area Trigger (3219)
//at_iron_star_area_trigger
struct at_iron_star_area_trigger : AreaTriggerAI
{
    at_iron_star_area_trigger(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) {}

    uint32 timerStart = 2000;
    UnitList list;

    void OnUpdate(uint32 diff) override
    {
        if (timerStart)
        {
            if (timerStart <= diff)
            {
                if (auto caster = at->GetCaster())
                {
                    list.clear();
                    caster->GetAttackableUnitListInRange(list, 6.0f);
                    if (!list.empty())
                        for (auto creature : list)
                            if (caster->IsValidAttackTarget(creature))
                                caster->CastSpell(creature, 173428, true);
                }

                timerStart = 1000;
            }
            else
                timerStart -= diff;
        }
    }
};

struct npc_archmage_khadgar_77195 : public ScriptedAI
{
    npc_archmage_khadgar_77195(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
        if (instance->GetData(3) != DONE)
        {
            //instance->DoSendEventScenario(42109);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        }
        else if (instance->GetData(5) != DONE)
        {
          //  instance->DoSendEventScenario(41842);
           // if (!IsLock)
            {
              //  IsLock = true;
                if (Creature* black = me->FindNearestCreature(NPC_BLACKHAND_77256, 50.0f))
                {
                    if (Creature* mob = me->FindNearestCreature(NPC_ORGRIM_DOOMHAMMER_77257, 50.0f))
                    {
                        black->CastSpell(mob, 177698, true);
                        mob->SetVisible(false);
                    }
                }
            }
          //  instance->DoSendEventScenario(40692);
           // player->GetMotionMaster()->MoveSmoothPath(2, stage5SencondPos, Path01Size::value, false);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        }
        else if (instance->GetData(6) != DONE)
        {
          //  instance->DoSendEventScenario(42038);
         //   player->GetMotionMaster()->MoveSmoothPath(2, stage6FirstPos, Path02Size::value, false);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (me->IsWithinDist(player, 15.0f, false))
            {
             //   if (instance->GetData(3) != DONE)
                 //   instance->DoSendEventScenario(42032);
            }
        }
    }
private:
    InstanceScript* instance;
};
//167264 TIME STOP -> AURA 167260
class spell_talador_stop_time : public SpellScript
{
    PrepareSpellScript(spell_talador_stop_time);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (!GetCaster())
            return;

        if (targets.empty())
            return;

        for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
        {
            if (WorldObject* object = (*itr))
            {
                if (ChectTargetEntry(object->GetEntry()))
                    continue;

                targets.remove(object);
            }
        }
    }

    bool ChectTargetEntry(uint32 entry)
    {
        bool result = false;
        switch (entry)
        {
        case NPC_BURNING_BLADEMASTER_83807:
        case NPC_GROM_KAR_VANGUARD_83806:
        case NPC_GROM_KAR_GRIMSHOT_83805:
        case NPC_IRON_BRUTE_83492:
        case NPC_GROM_KAR_GRIMSHOT_83638:
        case NPC_GROM_KAR_VANGUARD_83637:
        case 84359:
        case 83811:
            result = true;
            break;
        default:
            break;
        }

        return result;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (!GetCaster() || !GetHitUnit())
            return;

        GetCaster()->CastSpell(GetHitUnit(), SPELL_STAGE_4_STOP_TIME, true);
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_talador_stop_time::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_talador_stop_time::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};
// 167260
class aura_talador_stop_time : public AuraScript
{
    PrepareAuraScript(aura_talador_stop_time);

    void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
    {
        if (!GetUnitOwner())
            return;

        GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_STAGE_4_STOP_TIME_STUN, true);
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_talador_stop_time::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

struct npc_machinist_brandt_80962 : public ScriptedAI
{
    npc_machinist_brandt_80962(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    enum brandt_spells
    {
        SPELL_BLACKROCK_BOMB = 168102,
        SPELL_NAPALM_THROWER = 168127,
        SPELL_BOMB_FRENZY = 168367,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_BLACKROCK_BOMB, 5s);
        events.ScheduleEvent(SPELL_NAPALM_THROWER, 10s);
        events.ScheduleEvent(SPELL_BOMB_FRENZY, 15s);
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
        case SPELL_BLACKROCK_BOMB:
        {
            DoCast(SPELL_BLACKROCK_BOMB);
            events.Repeat(5s);
            break;
        }
        case SPELL_NAPALM_THROWER:
        {
            DoCast(SPELL_NAPALM_THROWER);
            events.Repeat(10s);
            break;
        }
        case SPELL_BOMB_FRENZY:
        {
            DoCast(SPELL_BOMB_FRENZY);
            events.Repeat(25s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (me->IsWithinDist(player, 15.0f, false))
            {
                me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                if (instance->GetData(5) != DONE /*&& !IsLock*/)
                {
                   // IsLock = true;
                    if (Creature* mob = me->FindNearestCreature(84229, 50.0f))
                        if (Creature* blackhand = me->FindNearestCreature(NPC_BLACKHAND_77256, 500.0f))
                            mob->SetFacingToObject(blackhand);
                }
            }
        }
    }
private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_blackhand_77256 : public ScriptedAI
{
    npc_blackhand_77256(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    enum blackhand_spells
    {
        SPELL_SHATTERING_SMASH = 168766,//Shattering Smash
        SPELL_BIG_BLACKROCK_BOOT = 169041,//Big Blackrock Boot
        SPELL_BRIMSTONE = 168630,//Brimstone
        SPELL_MARKED_FOR_DEATH = 168261,//Marked for Death
    };
    void Reset() override
    {
        me->SetReactState(REACT_DEFENSIVE);
        me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
      //  IsLock = false;
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        events.ScheduleEvent(SPELL_SHATTERING_SMASH, 5s);
        events.ScheduleEvent(SPELL_BIG_BLACKROCK_BOOT, 5s);
        events.ScheduleEvent(SPELL_BRIMSTONE, 5s);
        events.ScheduleEvent(SPELL_MARKED_FOR_DEATH, 5s);
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
        case SPELL_SHATTERING_SMASH:
        {
            if (roll_chance_i(10))
                Talk(0);
            DoCast(SPELL_SHATTERING_SMASH);
            events.Repeat(10s);
            break;
        }
        case SPELL_BIG_BLACKROCK_BOOT:
        {
            if (roll_chance_i(10))
                Talk(1);
            DoCast(SPELL_BIG_BLACKROCK_BOOT);
            events.Repeat(15s);
            break;
        }
        case SPELL_BRIMSTONE:
        {
            if (roll_chance_i(10))
                Talk(2);
            DoCast(SPELL_BRIMSTONE);
            events.Repeat(15s);
            break;
        }
        case SPELL_MARKED_FOR_DEATH:
        {
            Talk(3);
            DoCast(SPELL_MARKED_FOR_DEATH);
            events.Repeat(15s);
            break;
        }
        }
        DoMeleeAttackIfReady();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (me->IsWithinDist(player, 15.0f, false))
            {
                me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetReactState(REACT_AGGRESSIVE);
            }
        }
    }
private:
    InstanceScript* instance;
    EventMap events;
};
// 231620 go_portal_to_talador
struct go_portal_to_talador : public GameObjectAI
{
    go_portal_to_talador(GameObject* go) : GameObjectAI(go) {  }

    bool GossipHello(Player* player)
    {
        if (player->IsInAlliance())
        {
            player->ForceCompleteQuest(34099);
            player->TeleportTo(1116, 3594.35f, 1678.62f, 172.898f, 3.0143406f);
        }
        else
        {
            player->ForceCompleteQuest(33731);
            player->TeleportTo(1116, 3250.524170f, 1563.177734f, 162.102158f, 2.407810f);
        }

        return true;
    }
};

class q34099_33731_teleport : public PlayerScript
{
public:
    q34099_33731_teleport() : PlayerScript("q34099_33731_teleport") { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 34099 || quest->GetQuestId() == 33731)
            player->TeleportTo(1207, 2998.3826f, 2666.6997f, 78.83f, 1.2955966f);
    }

};
void AddSC_scenario_talador_iron_horde_finale()
{
   // RegisterInstanceScript(scenario_talador_iron_horde_finale, 1207);
    RegisterCreatureAI(npc_scenario_talador_stage_1);
    RegisterCreatureAI(npc_iron_star_86671);
    RegisterAreaTriggerAI(at_iron_star_area_trigger);
    RegisterCreatureAI(npc_archmage_khadgar_77195);
    RegisterSpellScript(spell_talador_stop_time);
    RegisterSpellScript(aura_talador_stop_time);

    RegisterCreatureAI(npc_machinist_brandt_80962);
    RegisterCreatureAI(npc_blackhand_77256);

    RegisterGameObjectAI(go_portal_to_talador);
    new q34099_33731_teleport();
}
