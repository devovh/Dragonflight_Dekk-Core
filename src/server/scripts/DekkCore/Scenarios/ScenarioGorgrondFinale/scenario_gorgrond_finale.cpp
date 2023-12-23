/*
scenario_gorgrond_finale_1
*/
#include "Scenario.h"
#include "GameObject.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "WorldStatePackets.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "QuestDef.h"
#include "AreaTriggerAI.h"
#include "GameObjectAI.h"
#include "TemporarySummon.h"


//723
/*
QUEST 
?? 36575/???? 36576/???? kill 81074
?? 36573/???? 36574/???? kill 81055
723 ????????    38087   0   42412   92  38088   ??????? 1   0   0   25388   38087   0
723 ??????????? 38089   1   42413   92  38090   ??????????  1   0   0   25389   38089   0
723 ??????????????  38091   2   42414   92  38092   ??????? 1   0   0   25390   38091   0
723 ?????   38093   3   42415   92  38094   ????    1   0   0   25391   38093   0
723 ????????    38095   4   42416   92  38096   ??????? 1   0   0   25392   38095   0
*/
enum CriteriaTrees
{
    CRITERIA_TREE_1 = 38087, // 42412
    CRITERIA_TREE_2 = 38089, // 42413
    CRITERIA_TREE_3 = 38091, // 42414
    CRITERIA_TREE_4 = 38093, // 42415
    CRITERIA_TREE_5 = 38095, // 42416

    DATA_AFTER_PLAY_MOVIE = 9,
};

enum Npcs
{
    NPC_YREL_85619 = 85619,

    NPC_DRAKA_86401 = 86401,

    NPC_YKISH_86210 = 86210,
    NPC_BEATFACE_86208 = 86208,
    NPC_PROWLER_SASHA_86209 = 86209,
    NPC_REXXAR_85620 = 85620,
    NPC_NISHA_85623 = 85623,

    NPC_COMMANDER_GAR_85571 = 85571,
    NPC_GOC_85580 = 85580,

    //mobs
    NPC_GROM_KAR_GRUNT_85641 = 85641,
    NPC_GROM_KAR_DEADEYE_85642 = 85642,

    NPC_SHATTERED_CHAINMASTER_85643 = 85643,
    NPC_SIEGE_CANNON_85587 = 85587,

    NPC_GRONNLING_SLAVE_85646 = 85646,
    NPC_IRON_STAR_85579 = 85579,

    NPC_BECKONED_EARTHTWISTER_85637 = 85637,
    NPC_BECKONED_EARTHSHAKER_85636 = 85636,

};

enum GameObjects
{
};

enum Spells
{
    AURA_PRIMAL_ARTIFACT = 172875, //  / primal - artifact
    SPELL_ACCESS_ARTIFACT = 172674, //access-artifact 
};

ObjectData const creatureData[] =
{
    {NPC_YREL_85619,NPC_YREL_85619},
    {NPC_DRAKA_86401,NPC_DRAKA_86401},
    {NPC_YKISH_86210,NPC_YKISH_86210},
    {NPC_BEATFACE_86208,NPC_BEATFACE_86208},
    {NPC_PROWLER_SASHA_86209,NPC_PROWLER_SASHA_86209},
    {NPC_REXXAR_85620,NPC_REXXAR_85620},
    {NPC_NISHA_85623,NPC_NISHA_85623},
    {NPC_COMMANDER_GAR_85571,NPC_COMMANDER_GAR_85571},
    {NPC_GOC_85580,NPC_GOC_85580},

    {0,                             0} // END
};

ObjectData const gobjectData[] =
{
    {0,                     0 } // END
};

struct NpcInfo
{
    NpcInfo(uint32 stepentry, uint32 entry, float x, float y, float z, float o) :
        stepEntry(stepentry), Entry(entry), pos_x(x), pos_y(y), pos_z(z), pos_o(o) { }
    uint32      stepEntry;
    uint32      Entry;
    float    pos_x;
    float    pos_y;
    float    pos_z;
    float    pos_o;
};

const NpcInfo npcInfos[] =
{
{2, NPC_GROM_KAR_GRUNT_85641, 8621.54f, 1462.37f, 67.5524f, 1.07683f},
{2, NPC_GROM_KAR_GRUNT_85641, 8801.48f, 1438.85f, 82.1513f, 2.34405f},
{2, NPC_GROM_KAR_GRUNT_85641, 8815.65f, 1437.18f, 81.6593f, 2.57957f},
{2, NPC_GROM_KAR_GRUNT_85641, 8812.92f, 1438.62f, 81.5864f, 1.85703f},
{2, NPC_GROM_KAR_GRUNT_85641, 8800.43f, 1433.72f, 82.9236f, 1.13116f},
{2, NPC_GROM_KAR_GRUNT_85641, 8803.97f, 1434.03f, 82.8113f, 1.08049f},
{2, NPC_GROM_KAR_GRUNT_85641, 8809.44f, 1444.56f, 81.1513f, 2.2981f},
{2, NPC_GROM_KAR_GRUNT_85641, 8797.95f, 1438.54f, 82.8709f, 1.18028f},
{2, NPC_GROM_KAR_GRUNT_85641, 8676.62f, 1351.29f, 67.0486f, 5.88139f},
{2, NPC_GROM_KAR_GRUNT_85641, 8680.33f, 1474.2f, 67.5524f, 1.85376f},
{2, NPC_GROM_KAR_GRUNT_85641, 8652.09f, 1338.6f, 63.8573f, 1.2196f},
{2, NPC_GROM_KAR_GRUNT_85641, 8707.66f, 1402.96f, 67.401f, 0.576867f},
{2, NPC_GROM_KAR_GRUNT_85641, 8742.76f, 1440.5f, 73.0873f, 3.54276f},
{2, NPC_GROM_KAR_GRUNT_85641, 8636.58f, 1465.81f, 67.5524f, 5.10922f},
{2, NPC_GROM_KAR_GRUNT_85641, 8724.57f, 1467.14f, 69.4686f, 3.54276f},
{2, NPC_GROM_KAR_GRUNT_85641, 8695.34f, 1381.02f, 66.5387f, 5.4548f},
{2, NPC_GROM_KAR_GRUNT_85641, 8716.41f, 1411.88f, 67.1252f, 1.5422f},
{2, NPC_GROM_KAR_GRUNT_85641, 8716.0f, 1479.24f, 67.8947f, 3.99167f},
{2, NPC_GROM_KAR_GRUNT_85641, 8648.78f, 1456.99f, 67.5524f, 2.82518f},
{2, NPC_GROM_KAR_GRUNT_85641, 8597.42f, 1457.88f, 67.5524f, 1.11568f},
{2, NPC_GROM_KAR_GRUNT_85641, 8766.64f, 1470.78f, 78.1736f, 3.54768f},
{2, NPC_GROM_KAR_GRUNT_85641, 8733.69f, 1453.79f, 69.1013f, 3.54276f},
{2, NPC_GROM_KAR_GRUNT_85641, 8768.99f, 1473.15f, 78.826f, 2.91308f},
{2, NPC_GROM_KAR_GRUNT_85641, 8768.55f, 1469.63f, 78.6974f, 2.53386f},
{2, NPC_GROM_KAR_GRUNT_85641, 8766.23f, 1482.78f, 78.4349f, 3.66736f},
{2, NPC_GROM_KAR_GRUNT_85641, 8774.22f, 1474.57f, 79.9717f, 2.44117f},
{2, NPC_GROM_KAR_GRUNT_85641, 8773.78f, 1471.05f, 79.9027f, 2.48281f},
{2, NPC_GROM_KAR_GRUNT_85641, 8771.63f, 1484.28f, 79.4439f, 3.21887f},
{2, NPC_GROM_KAR_GRUNT_85641, 8773.6f, 1486.66f, 80.0837f, 3.94709f},
{2, NPC_GROM_KAR_GRUNT_85641, 8755.9f, 1496.82f, 77.0886f, 5.10041f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8716.21f, 1484.53f, 69.2542f, 4.18955f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8765.31f, 1476.38f, 78.1046f, 3.83427f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8766.94f, 1463.69f, 81.8544f, 0.640571f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8820.28f, 1449.75f, 82.1202f, 2.40036f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8811.17f, 1434.78f, 82.0267f, 1.96671f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8819.01f, 1441.06f, 81.8613f, 1.97934f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8807.76f, 1435.37f, 82.112f, 2.63736f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8822.77f, 1444.92f, 82.3431f, 3.07804f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8816.53f, 1445.88f, 81.8434f, 3.20708f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8805.27f, 1440.19f, 81.1015f, 2.21491f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8740.11f, 1447.3f, 72.7794f, 3.33106f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8705.57f, 1382.27f, 77.8012f, 2.0271f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8640.34f, 1472.34f, 88.7133f, 5.14438f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8726.15f, 1410.91f, 88.1088f, 1.02827f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8669.28f, 1479.94f, 77.6781f, 5.25601f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8697.35f, 1372.29f, 77.4826f, 2.12917f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8717.14f, 1396.48f, 88.1113f, 2.84988f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8722.39f, 1478.79f, 71.3091f, 3.93772f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8756.04f, 1496.84f, 77.0908f, 5.01676f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8762.28f, 1493.82f, 78.9545f, 3.76873f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8768.47f, 1477.13f, 78.8302f, 3.58249f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8765.27f, 1489.33f, 78.9638f, 4.2802f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8767.52f, 1495.24f, 80.4047f, 4.44161f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8773.71f, 1478.56f, 79.7855f, 4.00713f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8770.5f, 1490.76f, 80.2355f, 3.34338f},
{2, NPC_GROM_KAR_DEADEYE_85642, 8775.0f, 1481.77f, 79.9321f, 3.32826f},
{3, NPC_SHATTERED_CHAINMASTER_85643, 8758.82f, 1481.26f, 77.0943f, 3.74152f},
{3, NPC_SHATTERED_CHAINMASTER_85643, 8758.9f, 1489.66f, 77.7233f, 4.01202f},
{3, NPC_SHATTERED_CHAINMASTER_85643, 8760.47f, 1482.48f, 77.4896f, 3.72966f},
{3, NPC_SIEGE_CANNON_85587, 8765.52f, 1473.35f, 78.0879f, 3.50394f},
{3, NPC_SIEGE_CANNON_85587, 8755.05f, 1484.3f, 76.692f, 3.97009f},
{3, NPC_SIEGE_CANNON_85587, 8756.04f, 1475.86f, 76.1698f, 3.46539f},
{3, NPC_SIEGE_CANNON_85587, 8755.82f, 1485.14f, 76.9277f, 3.97009f},
{3, NPC_SIEGE_CANNON_85587, 8746.56f, 1470.99f, 73.8709f, 3.40746f},
{3, NPC_SIEGE_CANNON_85587, 8761.94f, 1487.77f, 78.1581f, 4.67526f},
{3, NPC_SIEGE_CANNON_85587, 8764.79f, 1480.58f, 78.168f, 4.0169f},
};

struct scenario_gorgrond_finale_1 : public InstanceScript
{
    scenario_gorgrond_finale_1(InstanceMap* map) : InstanceScript(map) { }

   /* void Initialize() override
    {
        LoadObjectData(creatureData, gobjectData);
    }*/

    void LoadNpcData(NpcInfo const* data, uint32 step)
    {
        while (data->stepEntry)
        {
            if (data->stepEntry == step)
            {
                if (auto creature = instance->SummonCreature(data->Entry, Position(data->pos_x, data->pos_y, data->pos_z, data->pos_o)))
                {
                    creature->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    creature->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                    creature->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                    creature->GetMotionMaster()->MovePoint(1, Position(8706.05f + urand(1, 10), 1447.38f + urand(1, 10), 65.406f, 3.8601f));
                }
            }
            ++data;
        }
    }

    void OnPlayerEnter(Player* player) override
    {
        SetCheckPointPos(Position(8589.5f, 1389.43f, 65.098015f, 0.5219f));//a 1402
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

    void OnCompletedCriteriaTree(CriteriaTree const* tree) //override
    {
        switch (tree->ID)
        {
        case CRITERIA_TREE_1:
            if (Creature* gar = GetCreature(NPC_COMMANDER_GAR_85571))
            {
                gar->SetReactState(ReactStates::REACT_PASSIVE);
                gar->SetHomePosition(8731.33f, 1473.89f, 84.384f, 3.8f);
                gar->NearTeleportTo(8731.33f, 1473.89f, 84.384f, 3.8f);
            }
            DoAddAuraOnPlayers(AURA_PRIMAL_ARTIFACT);
            //AddDelayedEvent(5000), [=](TaskContext context)
            //{
                LoadNpcData(npcInfos, 2);
            //});
            break;
        case CRITERIA_TREE_2:
            //spawn
            //AddDelayedEvent(5000), [=](TaskContext context)
            //{
                LoadNpcData(npcInfos, 3);
            //});
            break;
        case CRITERIA_TREE_3:
            if (Creature* gar = GetCreature(NPC_COMMANDER_GAR_85571))
                gar->AI()->Talk(2);
            if (Creature* goc = GetCreature(NPC_GOC_85580))
            {
                goc->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                goc->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                goc->SetReactState(ReactStates::REACT_AGGRESSIVE);
                DoCreatureMoveTo(goc, Position(8730.967f, 1461.993f, 69.93983f, 3.8601f));
            }    
            break;
        case CRITERIA_TREE_4:
            DoRemoveAurasDueToSpellOnPlayers(AURA_PRIMAL_ARTIFACT);
            if (Creature* gar = GetCreature(NPC_COMMANDER_GAR_85571))
            {
                gar->SetReactState(ReactStates::REACT_AGGRESSIVE);
                gar->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                gar->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                gar->SetHomePosition(8730.967f, 1461.993f, 69.93983f, 3.8601f);
                gar->GetMotionMaster()->MoveJump(Position(8730.967f, 1461.993f, 69.93983f, 3.8601f), 10.0f, 10.0f);
            }
            break;
        case CRITERIA_TREE_5:         
            break;
        }
    }

    void OnGameObjectCreate(GameObject* go) override
    {
        InstanceScript::OnGameObjectCreate(go);
        switch (go->GetEntry())
        {
        default:
            break;
        }
    }

    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);
        switch (creature->GetEntry())
        {

        default:
            break;
        }
    }

    void OnUnitDeath(Unit* unit)
    {
        switch (unit->GetEntry())
        {
        case NPC_GOC_85580:
        {           
           // DoSendEventScenario(42415);//stage 4
            //DoCastSpellOnPlayers(SPELL_MOVIE);
            //events.ScheduleEvent(DATA_AFTER_PLAY_MOVIE, 5000);
            break;
        }
        case NPC_COMMANDER_GAR_85571:
        {
           // DoSendEventScenario(42416);//stage 5
            //It is over, their defenses are shattered. We have won this battle, but without the artifact we have far to go. Let us rally back at your outpost, commander. We must plan our next course of attack.
            if (Creature* yrel = GetCreature(NPC_YREL_85619))
                yrel->Say(87467);
            if (Creature* draka = GetCreature(NPC_DRAKA_86401))
                draka->Say(87467);

            DoOnPlayers([](Player* player)
            {
                player->GetScheduler().Schedule(5s, [=](TaskContext /*context*/)
                {
                    player->ForceCompleteQuest(36575);
                    player->ForceCompleteQuest(36576);
                    player->ForceCompleteQuest(36573);
                    player->ForceCompleteQuest(36574);
                    player->TeleportTo(1116, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
                });
            });
            break;
        }
        default:
            break;
        }
    }

private:
    EventMap events;
};

class q36575_36576_36573_36574_teleport : public PlayerScript
{
public:
    q36575_36576_36573_36574_teleport() : PlayerScript("q36575_36576_36573_36574_teleport") { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        /*
        ?? 36575/???? 36576/???? kill 81074
        ?? 36573/???? 36574/???? kill 81055
        */
        if (quest->GetQuestId() == 36575 || quest->GetQuestId() == 36576 || quest->GetQuestId() == 36573 || quest->GetQuestId() == 36574)
        {
            //player->SetScenarioId(723);
            player->KilledMonsterCredit(81074);
            player->KilledMonsterCredit(81055);
            player->TeleportTo(1402, 8589.5f, 1389.43f, 65.098015f, 0.5219f);
        }
    }

    void OnSuccessfulSpellCast(Player* player, Spell* spell) override
    {
        if (player->GetMapId() == 1402 && spell->GetSpellInfo()->Id == SPELL_ACCESS_ARTIFACT)
        {
            //NPC_BECKONED_EARTHTWISTER_85637 = 85637,
            //NPC_BECKONED_EARTHSHAKER_85636 = 85636,
            if (auto npc = player->SummonCreature(NPC_BECKONED_EARTHSHAKER_85636, player->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 50s))
            {
                npc->CastSpell(npc, 172812, true);

                npc->GetScheduler().Schedule(Milliseconds(10000), [=](TaskContext context)
                {
                    npc->CastSpell(npc, 172812, true);
                });
            }
            //if (InstanceScript* instance = player->GetInstanceScript())
            //{
            //    instance->DoSendEventScenario(42414);//stage 3

            //    instance->DoSendEventScenario(42413);//stage 2                   
            //}
        }
    }
};
//85619 86401
struct npc_draka_86401 : public ScriptedAI
{
    npc_draka_86401(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
        player->KilledMonsterCredit(me->GetEntry());

        me->SetHomePosition(8686.537f, 1445.717f, 65.02763f, 0.569f);
        me->GetMotionMaster()->MovePoint(1, Position(8686.537f, 1445.717f, 65.02763f, 0.569f));

        if (Creature* npc = me->FindNearestCreature(NPC_YKISH_86210, 50.0f))
        {
            npc->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            npc->SetHomePosition(8696.484f, 1444.593f, 65.0825f, 0.569f);
            npc->GetMotionMaster()->MovePoint(1, Position(8696.484f, 1444.593f, 65.0825f, 0.569f));
        }
        if (Creature* npc = me->FindNearestCreature(NPC_BEATFACE_86208, 50.0f))
        {
            npc->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            npc->SetHomePosition(8698.396f, 1426.189f, 64.89952f, 0.569f);
            npc->GetMotionMaster()->MovePoint(1, Position(8698.396f, 1426.189f, 64.89952f, 0.569f));
        }
        if (Creature* npc = me->FindNearestCreature(NPC_PROWLER_SASHA_86209, 50.0f))
        {
            npc->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            npc->SetHomePosition(8697.638f, 1432.023f, 65.02452f, 0.569f);
            npc->GetMotionMaster()->MovePoint(1, Position(8697.638f, 1432.023f, 65.02452f, 0.569f));
        }
        if (Creature* npc = me->FindNearestCreature(NPC_REXXAR_85620, 50.0f))
        {
            npc->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            npc->SetHomePosition(8669.899f, 1450.779f, 67.06866f, 0.569f);
            npc->GetMotionMaster()->MovePoint(1, Position(8669.899f, 1450.779f, 67.06866f, 0.569f));
        }
        if (Creature* npc = me->FindNearestCreature(NPC_NISHA_85623, 50.0f))
        {
            npc->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            npc->SetHomePosition(8674.912f, 1444.127f, 64.97562f, 0.569f);
            npc->GetMotionMaster()->MovePoint(1, Position(8674.912f, 1444.127f, 64.97562f, 0.569f));
        }
    //    instance->DoSendEventScenario(42412);
        //88965 We will never surrender to you! Champion, now is the time! Use the artifact!

        Talk(0);//The Iron Horde have no idea how much potential lies within the artifact. Let us give them a proper demonstration, shall we?
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (me->IsWithinDist(player, 100.0f, false))
            {
                if (player->IsInAlliance())
                    me->UpdateEntry(NPC_YREL_85619);
                else
                    me->UpdateEntry(NPC_DRAKA_86401);
            }
        }
    }
private:
    InstanceScript* instance;
};
//172812 
class spell_rune_of_earth_172812 : public SpellScript
{
    PrepareSpellScript(spell_rune_of_earth_172812);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (!GetCaster())
            return;

        targets.remove_if([this](WorldObject* object) -> bool
        {
            if (object == nullptr)
                return true;

            if (object->IsPlayer() || object->GetEntry() == GetCaster()->GetEntry())
                return true;

            if(!GetCaster()->IsValidAttackTarget(object->ToUnit()))
                return true;

            return false;
        });
    }

    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rune_of_earth_172812::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rune_of_earth_172812::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};
void AddSC_scenario_gorgrond_finale_1()
{
  //  RegisterInstanceScript(scenario_gorgrond_finale_1, 1402);
    new q36575_36576_36573_36574_teleport();
    RegisterCreatureAI(npc_draka_86401);
    RegisterSpellScript(spell_rune_of_earth_172812);
}
