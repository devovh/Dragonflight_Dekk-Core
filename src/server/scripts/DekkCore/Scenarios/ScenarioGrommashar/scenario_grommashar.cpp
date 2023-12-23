/*
scenario_grommashar
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


//508 509

enum CriteriaTrees
{
    //DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, eventId);
    CRITERIA_TREE_SPEAK_WITH_VINDICATOR_YREL = 37446, // 41156
    CRITERIA_TREE_SPEAK_WITH_RANGARI_DKAAN = 37444, // 41895
    CRITERIA_TREE_DEFEAT_PACKMASTER_A = 36375, // 82131
    CRITERIA_TREE_SPEAK_WITH_VINDICATOR_YREL_2 = 37403, // 41827
    CRITERIA_TREE_DEFEAT_DURN_THE_HUNGERER_A = 36377, // 82132
    CRITERIA_TREE_SPEAK_WITH_VINDICATOR_YREL_3 = 37452, // 41829
    CRITERIA_TREE_DEFEAT_GARROSH_A = 36379, // 82133

    CRITERIA_TREE_SPEAK_WITH_DUROTAN = 37448, // 41162
    CRITERIA_TREE_SPEAK_WITH_AGGRA = 37450, // 41896
    CRITERIA_TREE_DEFEAT_PACKMASTER_H = 36381, // 82131
    CRITERIA_TREE_SPEAK_WITH_DUROTAN_2 = 37405, // 41828
    CRITERIA_TREE_DEFEAT_DURN_THE_HUNGERER_H = 36383, // 82132
    CRITERIA_TREE_SPEAK_WITH_DUROTAN_3 = 37454, // 41897
    CRITERIA_TREE_DEFEAT_GARROSH_H = 36385, // 82133

    DATA_AFTER_PLAY_MOVIE = 9,
};

enum Npcs
{
    NPC_VINDICATOR_YREL_81404 = 81404,
    NPC_RANGARI_DKAAN_81405 = 81405,
    NPC_RANGARI_DKAAN_85061 = 85061,
    NPC_VINDICATOR_YREL_83407 = 83407,

    NPC_DUROTAN_81414 = 81414,
    NPC_AGGRA_81416 = 81416,
    NPC_DUROTAN_84443 = 84443,
    NPC_AGGRA_84444 = 84444,

    NPC_PACKMASTER_GROKGLOK_82131 = 82131,
    NPC_GARROSH_HELLSCREAM_82133 = 82133,
};

enum GameObjects
{
    GO_GATES_01 = 233652,
    GO_GATES_02 = 233942,
    GO_GATES_03 = 233943,
    GO_PORTAL_TO_TALADOR_ALLIANCE = 231620,
};

//Position const portalPos = { 3598.398f, 3058.7058f, 1.94938f, 5.823f };

enum Spells
{
    SPELL_MOVIE = 164204, //And Justice for Thrall: Play Cinematic in Scenario w / Teleport

};

ObjectData const creatureData[] =
{
    {NPC_VINDICATOR_YREL_81404,NPC_VINDICATOR_YREL_81404},
    {NPC_RANGARI_DKAAN_81405,NPC_RANGARI_DKAAN_81405},
    {NPC_RANGARI_DKAAN_85061,NPC_RANGARI_DKAAN_85061},
    {NPC_VINDICATOR_YREL_83407,NPC_VINDICATOR_YREL_83407},

    {NPC_DUROTAN_81414,NPC_DUROTAN_81414},
    {NPC_AGGRA_81416,NPC_AGGRA_81416},
    {NPC_DUROTAN_84443,NPC_DUROTAN_84443},
    {NPC_AGGRA_84444,NPC_AGGRA_84444},

    {NPC_PACKMASTER_GROKGLOK_82131,NPC_PACKMASTER_GROKGLOK_82131},
    {NPC_GARROSH_HELLSCREAM_82133,NPC_GARROSH_HELLSCREAM_82133},
    {0,                             0} // END
};

ObjectData const gobjectData[] =
{
    {GO_GATES_01,GO_GATES_01},
    {GO_GATES_02,GO_GATES_02},
    {GO_GATES_03,GO_GATES_03},
    {0,                     0 } // END
};

Position const stageMovePos[6] =
{
    {2042.6f, 5066.8f, 192.175f, 3.0f},//stage 1 
    {1938.28f, 5208.89f, 183.867f, 2.26f},//stage 2
    {2042.6f, 5066.8f, 192.175f, 3.0f},//stage 3
    {1861.22f, 5014.02f, 220.5f, 3.0f},//stage 4
    {1824.75f, 5162.09f, 204.51f, 2.0f},//stage 5
    {1743.47f, 5236.78f, 222.38f, 2.4f},//stage 6
};

struct NpcInfo
{
    NpcInfo(uint32 allianceentry, uint32 hordeentry, float x, float y, float z, float o) :
        allianceEntry(allianceentry), hordeEntry(hordeentry), pos_x(x), pos_y(y), pos_z(z), pos_o(o) { }
    uint32      allianceEntry;
    uint32      hordeEntry;
    float    pos_x;
    float    pos_y;
    float    pos_z;
    float    pos_o;
};

const NpcInfo npcInfos[] =
{
{81404, 81414, 2291.44f, 5076.99f, 207.438f, 2.56683f},
{81405, 81416, 2033.8f, 5067.69f, 192.102f, 0.137295f},
{81833, 80999, 2265.78f, 5083.9f, 206.174f, 2.38831f},
{81833, 80999, 2282.14f, 5084.69f, 206.614f, 2.41956f},
{81833, 80999, 2283.72f, 5075.93f, 207.236f, 2.33243f},
{81833, 80999, 2260.92f, 5098.56f, 205.322f, 3.45466f},
{81833, 80999, 2288.88f, 5083.01f, 207.125f, 2.43812f},
{81833, 80999, 2265.5f, 5090.13f, 205.645f, 3.38046f},
{81833, 80999, 2266.25f, 5096.6f, 205.413f, 2.04056f},
{81833, 80999, 2278.66f, 5080.02f, 206.724f, 2.41171f},
{81833, 80999, 2271.79f, 5091.32f, 205.758f, 2.5248f},
{81833, 80999, 2283.94f, 5083.27f, 206.788f, 2.41956f},
{81833, 80999, 2280.35f, 5082.32f, 206.635f, 2.53797f},
{81833, 80999, 2283.93f, 5086.97f, 206.651f, 2.54758f},
{81833, 80999, 2282.16f, 5080.89f, 206.822f, 2.53797f},
{81833, 80999, 2280.46f, 5078.6f, 206.915f, 2.41171f},
{81833, 80999, 2276.96f, 5095.77f, 206.035f, 3.08106f},
{81833, 80999, 2285.74f, 5085.55f, 206.83f, 2.54758f},
{81833, 80999, 2283.74f, 5079.53f, 206.989f, 2.40636f},
{81833, 80999, 2287.25f, 5080.74f, 207.102f, 2.42489f},
{81833, 80999, 2287.23f, 5084.17f, 206.977f, 2.32788f},
{81833, 80999, 2281.88f, 5077.26f, 207.082f, 2.37099f},
{81833, 80999, 2285.39f, 5078.24f, 207.134f, 2.51087f},
{81833, 80999, 2285.69f, 5082.0f, 206.962f, 2.37053f},
{81835, 81856, 2028.7f, 5110.31f, 182.149f, 0.497307f},
{81835, 81856, 2026.59f, 5110.41f, 181.984f, 1.92855f},
{81835, 81856, 1969.29f, 5175.27f, 185.083f, 3.02207f},
{81835, 81856, 2150.28f, 5104.9f, 203.274f, 2.96495f},
{81835, 81856, 2142.16f, 5099.15f, 201.609f, 4.32826f},
{81835, 81856, 2200.23f, 5107.17f, 205.155f, 2.5248f},
{81835, 81856, 2100.56f, 5097.46f, 194.98f, 4.22761f},
{81835, 81856, 2224.04f, 5100.63f, 202.977f, 2.5248f},
{81835, 81856, 2172.65f, 5101.95f, 205.873f, 3.66621f},
{81835, 81856, 2175.9f, 5098.78f, 206.42f, 3.09431f},
{81835, 81856, 2107.75f, 5090.37f, 196.035f, 3.13431f},
{81835, 81856, 2094.39f, 5071.44f, 192.294f, 3.64125f},
{81835, 81856, 2089.29f, 5073.48f, 192.021f, 3.37979f},
{81835, 81856, 2030.45f, 5070.98f, 191.956f, 1.55422f},
{81835, 81856, 2035.22f, 5076.85f, 191.701f, 2.20465f},
{81835, 81856, 2035.02f, 5144.67f, 186.192f, 1.73571f},
{81835, 81856, 2027.42f, 5073.27f, 191.704f, 1.54785f},
{81835, 81856, 2039.55f, 5137.73f, 186.557f, 0.497307f},
{81835, 81856, 1927.69f, 5068.11f, 198.663f, 4.8526f},
{81835, 81856, 1891.5f, 5071.91f, 199.848f, 6.07487f},
{81835, 81856, 1948.39f, 5206.4f, 184.455f, 1.50108f},
{81835, 81856, 1872.82f, 5101.38f, 199.761f, 6.12333f},
{81835, 81856, 1861.58f, 5126.86f, 200.318f, 4.32638f},
{81835, 81856, 1966.39f, 5194.24f, 185.324f, 3.07762f},
{81835, 81856, 1967.17f, 5199.29f, 185.535f, 3.07762f},
{81835, 81856, 1837.67f, 5287.12f, 196.824f, 5.20285f},
{81835, 81856, 1867.49f, 5336.98f, 177.767f, 3.64125f},
{81835, 81856, 1954.8f, 5060.88f, 196.266f, 1.92855f},
{81835, 81856, 1849.2f, 5258.48f, 200.432f, 1.92855f},
{81835, 81856, 1995.45f, 5058.64f, 193.707f, 3.22274f},
{81835, 81856, 1847.25f, 5318.23f, 185.984f, 3.68253f},
{81835, 81856, 1833.09f, 5160.16f, 202.997f, 4.1708f},
{81839, 81860, 2285.25f, 5073.56f, 207.453f, 2.63826f},
{81839, 81860, 2291.5f, 5081.58f, 207.328f, 1.67065f},
{81839, 81860, 2304.4f, 5068.86f, 208.033f, 5.76239f},
{81844, 81874, 2290.49f, 5070.71f, 207.815f, 1.87689f},
{81844, 81874, 2295.45f, 5076.79f, 207.619f, 2.35105f},
{83506, 83507, 2277.83f, 5076.98f, 207.004f, 2.37099f},
{83506, 83507, 2276.02f, 5080.71f, 206.608f, 0.913518f},
{83506, 83507, 2314.58f, 5070.94f, 208.612f, 5.70516f},
{83506, 83507, 2287.21f, 5089.31f, 206.927f, 2.33451f},
{83506, 83507, 2305.98f, 5058.75f, 208.892f, 5.16477f},
{83506, 83507, 2295.57f, 5081.24f, 207.715f, 2.24422f},
{85061, 84444, 1990.11f, 5038.95f, 196.112f, 3.13719f},
{83498, 83502, 2282.62f, 5068.94f, 207.988f, 2.28336f},
{83498, 83502, 2304.1f, 5076.2f, 207.826f, 2.64274f},
{83498, 83502, 1997.53f, 5054.0f, 193.84f, 4.22686f},
{83498, 83502, 1832.72f, 5166.79f, 203.487f, 2.33058f},
{81836, 81862, 2274.5f, 5070.42f, 208.2f, 1.32536f},
{81836, 81862, 2272.17f, 5070.21f, 208.274f, 1.45979f},
{81836, 81862, 2292.4f, 5086.98f, 207.493f, 2.75722f},
{81836, 81862, 2291.85f, 5064.47f, 209.063f, 2.08311f},
{81836, 81862, 2292.87f, 5089.87f, 207.977f, 3.00463f},
{83407, 84443, 1990.55f, 5042.211f, 195.4139f, 2.860446f}
};

struct scenario_grommashar : public InstanceScript
{
    scenario_grommashar(InstanceMap* map) : InstanceScript(map) { }

    int32 wav_1, wav_2, wav_3, wav_4;
    std::vector<ObjectGuid> stageAttackers;
    bool isSpawned;

   /* void Initialize() override
    {
        LoadObjectData(creatureData, gobjectData);
        isSpawned = false;
    }*/

    void LoadNpcData(NpcInfo const* data, Player* player)
    {
        while (data->allianceEntry)
        {
            player->SummonCreature(player->IsInAlliance() ? data->allianceEntry : data->hordeEntry, data->pos_x, data->pos_y, data->pos_z, data->pos_o);
            ++data;
        }
    }

    void OnPlayerEnter(Player* player) override
    {
        SetCheckPointPos(Position(2329.153f, 5151.324f, 232.8326f, 4.344268f));//a 1329

        if (!isSpawned)
        {
            isSpawned = true;
            LoadNpcData(npcInfos, player); 
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

    void OnCompletedCriteriaTree(CriteriaTree const* tree) //override
    {
        switch (tree->ID)
        {
        case CRITERIA_TREE_SPEAK_WITH_VINDICATOR_YREL:
            if (GameObject* gate = GetGameObject(GO_GATES_01))
                gate->SetGoState(GO_STATE_ACTIVE);
            if (Creature* yrel = GetCreature(NPC_VINDICATOR_YREL_81404))
                DoCreatureMoveTo(yrel, stageMovePos[0]);
            break;
        case CRITERIA_TREE_SPEAK_WITH_RANGARI_DKAAN:
            if (Creature* dkaan = GetCreature(NPC_RANGARI_DKAAN_81405))
                DoCreatureMoveTo(dkaan, stageMovePos[1]);
            if (Creature* yrel = GetCreature(NPC_VINDICATOR_YREL_81404))
                DoCreatureMoveTo(yrel, stageMovePos[1]);
            break;
        case CRITERIA_TREE_DEFEAT_PACKMASTER_A:
            if (GameObject* gate = GetGameObject(GO_GATES_02))
                gate->SetGoState(GO_STATE_ACTIVE);
            if (Creature* dkaan = GetCreature(NPC_RANGARI_DKAAN_81405))
            {
                DoCreatureMoveTo(dkaan, stageMovePos[2]);

                dkaan->GetScheduler().Schedule(Milliseconds(18000), [=](TaskContext context)
                {
                    dkaan->SetVisible(false);
                });
            }
            if (Creature* yrel = GetCreature(NPC_VINDICATOR_YREL_81404))
            {
                DoCreatureMoveTo(yrel, stageMovePos[2]);

                yrel->GetScheduler().Schedule(Milliseconds(18000), [=](TaskContext context)
                {
                    yrel->SetVisible(false);
                });
            }
            break;
        case CRITERIA_TREE_SPEAK_WITH_VINDICATOR_YREL_2:
            SetData(4, DONE);
            if (Creature* dkaan = GetCreature(NPC_RANGARI_DKAAN_85061))
            {
                dkaan->SetWalk(false);
                DoCreatureMoveTo(dkaan, stageMovePos[3]);
                if (Creature* yrel = GetCreature(NPC_VINDICATOR_YREL_83407))
                {
                    DoCreatureMoveTo(yrel, stageMovePos[3]);
                    dkaan->GetMotionMaster()->MoveFollow(yrel, 5.0f, dkaan->GetOrientation());
                }
            }
            break;
        case CRITERIA_TREE_DEFEAT_DURN_THE_HUNGERER_A:
            if (Creature* dkaan = GetCreature(NPC_RANGARI_DKAAN_85061))
                DoCreatureMoveTo(dkaan, stageMovePos[4]);
            if (Creature* yrel = GetCreature(NPC_VINDICATOR_YREL_83407))
                DoCreatureMoveTo(yrel, stageMovePos[4]);
            break;
        case CRITERIA_TREE_SPEAK_WITH_VINDICATOR_YREL_3:
        {
            if (Creature* garrosh = GetCreature(NPC_GARROSH_HELLSCREAM_82133))
                garrosh->Yell(84350);//Come, draenei witch. I'm waiting. 7.02
            if (Creature* yrel = GetCreature(NPC_VINDICATOR_YREL_83407))
            {
                yrel->GetScheduler().Schedule(Milliseconds(2100), [=](TaskContext context)
                {
                    yrel->Say(84351);
                });
                yrel->GetScheduler().Schedule(Milliseconds(13100), [=](TaskContext context)
                {
                    yrel->Say(84352);
                });    
            }
            if (Creature* dkaan = GetCreature(NPC_RANGARI_DKAAN_85061))
            {
                dkaan->GetScheduler().Schedule(Milliseconds(10100), [=](TaskContext context)
                {
                    dkaan->Say(84353);//Whatever happens, Maraad would be proud of you. 7.12
                });
                dkaan->GetScheduler().Schedule(Milliseconds(13100), [=](TaskContext context)
                {
                    dkaan->Say(84354);//Blow open these gates!      
                });           
            }

            if (Creature* yrel = GetCreature(NPC_VINDICATOR_YREL_83407))
            {
                yrel->GetScheduler().Schedule(Milliseconds(14000), [=](TaskContext context)
                {
                    if (GameObject* gate = GetGameObject(GO_GATES_03))
                        gate->SetGoState(GO_STATE_ACTIVE);
                });
                yrel->GetScheduler().Schedule(Milliseconds(15000), [=](TaskContext context)
                {
                    DoCreatureMoveTo(yrel, stageMovePos[5]);
                });
            }
            break;
        }
        case CRITERIA_TREE_DEFEAT_GARROSH_A: break;

        case CRITERIA_TREE_SPEAK_WITH_DUROTAN:
            if (GameObject* gate = GetGameObject(GO_GATES_01))
                gate->SetGoState(GO_STATE_ACTIVE);
            if (Creature* durotan = GetCreature(NPC_DUROTAN_81414))
                DoCreatureMoveTo(durotan, stageMovePos[0]);
            break;
        case CRITERIA_TREE_SPEAK_WITH_AGGRA:
            if (Creature* durotan = GetCreature(NPC_DUROTAN_81414))
                DoCreatureMoveTo(durotan, stageMovePos[1]);
            if (Creature* aggra = GetCreature(NPC_AGGRA_81416))
                DoCreatureMoveTo(aggra, stageMovePos[1]);
            break;
        case CRITERIA_TREE_DEFEAT_PACKMASTER_H:
            if (GameObject* gate = GetGameObject(GO_GATES_02))
                gate->SetGoState(GO_STATE_ACTIVE);
            if (Creature* durotan = GetCreature(NPC_DUROTAN_81414))
            {
                DoCreatureMoveTo(durotan, stageMovePos[2]);
                durotan->GetScheduler().Schedule(Milliseconds(18000), [=](TaskContext context)
                {
                    durotan->SetVisible(false);
                });
            }
            if (Creature* aggra = GetCreature(NPC_AGGRA_81416))
            {
                DoCreatureMoveTo(aggra, stageMovePos[2]);
                aggra->GetScheduler().Schedule(Milliseconds(18000), [=](TaskContext context)
                {
                    aggra->SetVisible(false);
                });
            }
            break;
        case CRITERIA_TREE_SPEAK_WITH_DUROTAN_2:
            SetData(4, DONE);
            if (Creature* aggra = GetCreature(NPC_AGGRA_84444))
            {
                aggra->SetWalk(false);
                DoCreatureMoveTo(aggra, stageMovePos[3]);
                if (Creature* durotan = GetCreature(NPC_DUROTAN_84443))
                {
                    DoCreatureMoveTo(durotan, stageMovePos[3]);
                    aggra->GetMotionMaster()->MoveFollow(durotan, 5.0f, aggra->GetOrientation());
                }
            }
            break;
        case CRITERIA_TREE_DEFEAT_DURN_THE_HUNGERER_H:
            if (Creature* durotan = GetCreature(NPC_DUROTAN_84443))
                DoCreatureMoveTo(durotan, stageMovePos[4]);
            if (Creature* aggra = GetCreature(NPC_AGGRA_84444))
                DoCreatureMoveTo(aggra, stageMovePos[4]);
            break;
        case CRITERIA_TREE_SPEAK_WITH_DUROTAN_3:
            if (Creature* garrosh = GetCreature(NPC_GARROSH_HELLSCREAM_82133))
                garrosh->Yell(84350);//Come, draenei witch. I'm waiting. 7.02
            if (Creature* durotan = GetCreature(NPC_DUROTAN_84443))
            {
                durotan->GetScheduler().Schedule(Milliseconds(2100), [=](TaskContext context)
                {
                    durotan->Say(84405); //D'kaan, I need you and our forces to stay here. horde 84405
                });
                durotan->GetScheduler().Schedule(Milliseconds(13100), [=](TaskContext context)
                {
                    durotan->Say(84406); //Buy us the time we need to deal with Garrosh. 7.09 
                });
                durotan->GetScheduler().Schedule(Milliseconds(14000), [=](TaskContext context)
                {
                    if (GameObject* gate = GetGameObject(GO_GATES_03))
                        gate->SetGoState(GO_STATE_ACTIVE);
                });
                durotan->GetScheduler().Schedule(Milliseconds(15000), [=](TaskContext context)
                {
                    DoCreatureMoveTo(durotan, stageMovePos[5]);
                });
            }
            if (Creature* aggra = GetCreature(NPC_AGGRA_84444))
            {
                aggra->GetScheduler().Schedule(Milliseconds(10100), [=](TaskContext context)
                {
                    aggra->Say(84407); //Please know, Durotan, you have been like a father to Thrall.
                });
                aggra->GetScheduler().Schedule(Milliseconds(13100), [=](TaskContext context)
                {
                    aggra->Say(84408); //Blow open these gates!      
                });
            }
            break;
        case CRITERIA_TREE_DEFEAT_GARROSH_H: break;
        }
    }

    void OnGameObjectCreate(GameObject* go) override
    {
        InstanceScript::OnGameObjectCreate(go);
        switch (go->GetEntry())
        {
        case GO_GATES_01:
        case GO_GATES_02:
        case GO_GATES_03:
            go->SetGoState(GO_STATE_READY);
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

        default:
            break;
        }
    }

    void OnUnitDeath(Unit* unit)
    {
        switch (unit->GetEntry())
        {
        case NPC_GARROSH_HELLSCREAM_82133:
        {
            DoOnPlayers([](Player* player)
            {
                player->AddMovieDelayedAction(177, [player]
                {
                    player->RemoveAurasDueToSpell(SPELL_MOVIE);
                    player->TeleportTo(1116, 3639.39f, 5512.71f, 52.8115f, 5.916666f);
                });
            });
            DoCastSpellOnPlayers(SPELL_MOVIE);
            break;
        }
        default:
            break;
        }
    }
};

class q35169_35171_teleport : public PlayerScript
{
public:
    q35169_35171_teleport() : PlayerScript("q35169_35171_teleport") { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 35169)
        {
            //player->SetScenarioId(508);
            player->TeleportTo(1329, 2329.153f, 5151.324f, 208.8341f, 4.344268f);
        }

        if (quest->GetQuestId() == 35171)
        {
            //player->SetScenarioId(509);
            player->TeleportTo(1329, 2329.153f, 5151.324f, 208.8341f, 4.344268f);
        }
    }
};

struct npc_vindicator_yrel_81404 : public ScriptedAI
{
    npc_vindicator_yrel_81404(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
        player->KilledMonsterCredit(me->GetEntry());
      //  instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41156, 1);
    }
private:
    InstanceScript* instance;
};
//81405
struct npc_rangari_dkaan_81405 : public ScriptedAI
{
    npc_rangari_dkaan_81405(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
       // instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41895, 1);
        //summon
    }
private:
    InstanceScript* instance;
};
//84022 fire arrow
//89054 fir


//85061
struct npc_rangari_dkaan_85061 : public ScriptedAI
{
    npc_rangari_dkaan_85061(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
    }
private:
    InstanceScript* instance;
};
//83000 wolf- 82131

struct npc_packmaster_grokglok_82131 : public ScriptedAI
{
    npc_packmaster_grokglok_82131(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    enum grokglok_spells
    {
        SPELL_SUNDER = 166997,
        SPELL_RAIN_OF_FIRE = 167011,
        SPELL_TASTE_OF_BLOOD = 213738,
    };

    void JustDied(Unit* /*killer*/) override
    {
        Talk(2);
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        Talk(0);
        me->GetScheduler().Schedule(Milliseconds(10000), [=](TaskContext context)
        {
            Talk(1);
        });
        events.ScheduleEvent(SPELL_SUNDER, 5s);
        events.ScheduleEvent(SPELL_RAIN_OF_FIRE, 5s);
        events.ScheduleEvent(SPELL_TASTE_OF_BLOOD, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        ScriptedAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
            case SPELL_SUNDER:
            {
                DoCast(SPELL_SUNDER);
                events.Repeat(15s);
                break;
            }
            case SPELL_RAIN_OF_FIRE:
            {
                DoCast(SPELL_RAIN_OF_FIRE);
                events.Repeat(15s);
                break;
            }
            case SPELL_TASTE_OF_BLOOD:
            {
                DoCast(SPELL_TASTE_OF_BLOOD);
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

//83407 yerl
struct npc_vindicator_yrel_83407 : public ScriptedAI
{
    npc_vindicator_yrel_83407(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void Reset() override
    {
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void sGossipHello(Player* player)
    {
        //player->PlayerTalkClass->ClearMenus();
        //stage 4
        if (instance->GetData(4) != DONE)
        {
            AddGossipItemFor(player, 16825, 0, GOSSIP_SENDER_MAIN, 0);
            player->PlayerTalkClass->SendGossipMenu(16825, me->GetGUID());
        }
        //stage 6
        else //if (instance->getScenarionStep() == 5)
        {
            AddGossipItemFor(player, 16829, 0, GOSSIP_SENDER_MAIN, 0);
            player->PlayerTalkClass->SendGossipMenu(16825, me->GetGUID());
        }
    }

    void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);

        //stage 4
        //if (instance->GetData(4) != DONE)
        //    instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41827, 1);
        ////stage 6
        //else//if (instance->getScenarionStep() == 5)
        //    instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41829, 1);
    }
private:
    InstanceScript* instance;
};

struct npc_garrosh_hellscream_82133 : public ScriptedAI
{
    npc_garrosh_hellscream_82133(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    enum garrosh_spells
    {
        SPELL_SUNDER = 167544,
        SPELL_NO_ESCAPE = 167492,
        SPELL_GORE_SEEKER = 167767,
        SPELL_BACK_HAND = 167811,
        SPELL_IRON_BARRAGE = 167856,
    };

    void JustDied(Unit* /*killer*/) override
    {
        Talk(4);
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        Talk(0);
        events.ScheduleEvent(SPELL_SUNDER, 5s);
        events.ScheduleEvent(SPELL_NO_ESCAPE, 5s);
        events.ScheduleEvent(SPELL_GORE_SEEKER, 5s);
        events.ScheduleEvent(SPELL_BACK_HAND, 5s);
        events.ScheduleEvent(SPELL_IRON_BARRAGE, 5s);
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
        case SPELL_SUNDER:
        {
            DoCast(SPELL_SUNDER);
            events.Repeat(15s);
            break;
        }
        case SPELL_NO_ESCAPE:
        {
            DoCast(SPELL_NO_ESCAPE);
            events.Repeat(15s);
            break;
        }
        case SPELL_GORE_SEEKER:
        {
            Talk(2);
            DoCast(SPELL_GORE_SEEKER);
            events.Repeat(15s);
            break;
        }
        case SPELL_BACK_HAND:
        {
            DoCast(SPELL_BACK_HAND);
            events.Repeat(15s);
            break;
        }
        case SPELL_IRON_BARRAGE:
        {
            DoCast(SPELL_IRON_BARRAGE);
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


struct npc_durotan_81414 : public ScriptedAI
{
    npc_durotan_81414(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
        player->KilledMonsterCredit(me->GetEntry());
       // instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41162, 1);
    }
private:
    InstanceScript* instance;
};

struct npc_aggra_81416 : public ScriptedAI
{
    npc_aggra_81416(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void sGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);
      //  instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41896, 1);
        //summon
    }
private:
    InstanceScript* instance;
};

struct npc_durotan_84443 : public ScriptedAI
{
    npc_durotan_84443(Creature* creature) : ScriptedAI(creature) { instance = creature->GetInstanceScript(); }

    void Reset() override
    {
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void sGossipHello(Player* player)
    {
        //player->PlayerTalkClass->ClearMenus();
        //stage 4
        if (instance->GetData(4) != DONE)
        {
            AddGossipItemFor(player, 16828, 0, GOSSIP_SENDER_MAIN, 0);
            player->PlayerTalkClass->SendGossipMenu(16828, me->GetGUID());
        }
        //stage 6
        else //if (instance->getScenarionStep() == 5)
        {
            AddGossipItemFor(player, 16830, 0, GOSSIP_SENDER_MAIN, 0);
            player->PlayerTalkClass->SendGossipMenu(16830, me->GetGUID());
        }
    }

    void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
    {
        CloseGossipMenuFor(player);

        //stage 4
        //if (instance->GetData(4) != DONE)
        //    instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41828, 1);
        ////stage 6
        //else//if (instance->getScenarionStep() == 5)
        //    instance->DoUpdateAchievementCriteria(CRITERIA_TYPE_SEND_EVENT, 41897, 1);
    }
private:
    InstanceScript* instance;
};
void AddSC_scenario_grommashar()
{
    //RegisterInstanceScript(scenario_grommashar, 1329);
    new q35169_35171_teleport();
    RegisterCreatureAI(npc_vindicator_yrel_81404);
    RegisterCreatureAI(npc_rangari_dkaan_81405);
    RegisterCreatureAI(npc_rangari_dkaan_85061);
    RegisterCreatureAI(npc_packmaster_grokglok_82131);
    RegisterCreatureAI(npc_vindicator_yrel_83407);
    RegisterCreatureAI(npc_garrosh_hellscream_82133);

    RegisterCreatureAI(npc_durotan_81414);
    RegisterCreatureAI(npc_aggra_81416);
    RegisterCreatureAI(npc_durotan_84443);
}
