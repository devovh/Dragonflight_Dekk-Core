/*
 * Copyright 2021 TrinityCore
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

#include "mists_of_tirna_scithe.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"

struct MazeLink
{
    MazeLink(int8 zoneID, bool canAccess, Position linkPosition)
    {
        ZoneID = zoneID;
        CanAccess = canAccess;
        LinkPosition = linkPosition;
    }

    int8 ZoneID;
    bool CanAccess;
    Position LinkPosition;
};

std::map<uint8, std::vector<MazeLink>> mazeLinks =
{
    {

        {
            1, {
                { -2, false, { -6865.729980f, 2258.629883f, 5478.620117f, 3.042210f } },
                { -1, false, { -6876.950195f, 2288.120117f, 5477.089844f, 3.998410f } },
                { 2,  false, { -6918.180176f, 2243.330078f, 5479.629883f, 0.532292f } },
                { 6,  true,  { -6900.270020f, 2279.149902f, 5477.529785f, 5.302830f } },
            }
        },
        {
            2, {
                { -1, false, { -6974.069824f, 2214.939941f, 5486.799805f, 1.550090f } },
                { 1,  true,  { -6950.839844f, 2225.120117f, 5486.509766f, 3.489840f } },
                { 3,  false, { -6999.839844f, 2233.840088f, 5485.100098f, 0.991181f } },
                { 7,  true,  { -6985.600098f, 2256.219971f, 5479.290039f, 4.674620f } },
            }
        },
        {
            3, {
                { 0,  false, { -7013.890137f, 2205.120117f, 5486.979980f, 2.216160F } },
                { 2,  true,  { -7011.850098f, 2221.500000f, 5486.299805f, 3.804940f } },
                { 4,  true,  { -7029.770020f, 2207.620117f, 5486.799805f, 0.118659f } },
                { 8,  true,  { -7026.470215f, 2224.510010f, 5486.359863f, 5.302830f } },
            }
        },
        {
            4, {
                { -1, false, { -7082.310059f, 2163.219971f, 5489.160156f, 1.875340f } },
                { 3,  false, { -7065.259766f, 2194.959961f, 5488.200195f, 3.500810f } },
                { 5,  true,  { -7105.830078f, 2168.580078f, 5489.350098f, 0.548284f } },
                { 9,  true,  { -7107.870117f, 2183.000000f, 5489.049805f, 5.722640f } },
            }
        },
        {
            5, {
                { -1, false, { -7170.609863f, 2147.020020f, 5487.540039f, 0.787082f } },
                { 4,  false, { -7134.470215f, 2151.280029f, 5489.810059f, 2.965590f } },
                { 10, true,  { -7173.479980f, 2181.350098f, 5487.520020f, 4.906050f } },
                { 11, true,  { -7183.819824f, 2167.050049f, 5487.560059f, 6.068160f } },
            }
        },
        {
            6, {
                { -1, false, { -6899.270020f, 2353.500000f, 5468.399902f, 3.338130f } },
                { 1,  false, { -6921.259766f, 2313.620117f, 5469.399902f, 2.299420f } },
                { 7,  true,  { -6959.830078f, 2310.959961f, 5471.759766f, 0.921302f } },
                { 17, true,  { -6941.879883f, 2354.120117f, 5468.149902f, 5.302830f } },
            }
        },
        {
            7, {
                { -1, false, { -6967.520020f, 2279.080078f, 5476.490234f, 2.688620f } },
                { 2,  false, { -6985.399902f, 2270.389893f, 5476.509766f, 1.670220f } },
                { 6,  true,  { -6972.430176f, 2295.689941f, 5475.529785f, 4.208850f } },
                { 13, true,  { -7007.169922f, 2303.840088f, 5474.810059f, 4.947850f } },
            }
        },
        {
            8, {
                { 3,  false, { -7044.580078f, 2248.500000f, 5482.140137f, 2.272850f } },
                { 9,  false, { -7078.180176f, 2248.679932f, 5480.160156f, 0.628820f } },
                { 14, true,  { -7085.379883f, 2276.139893f, 5477.379883f, 6.198620f } },
                { 13, true,  { -7058.149902f, 2296.699951f, 5476.299805f, 4.711300f } },
            }
        },
        {
            9, {
                { -1, false, { -7115.629883f, 2241.290039f, 5483.939941f, 4.721930f } },
                { 4,  false, { -7117.089844f, 2200.649902f, 5487.359863f, 1.899830f } },
                { 8,  true,  { -7093.189941f, 2235.760010f, 5483.750000f, 3.936090f } },
                { 10, true,  { -7139.589844f, 2223.989990f, 5483.790039f, 0.073853f } },
            }
        },
        {
            10, {
                { 5,  false, { -7177.490234f, 2197.010010f, 5483.819824f, 1.805420f } },
                { 9,  false, { -7154.740234f, 2224.090088f, 5481.979980f, 2.819390f } },
                { 14, true,  { -7165.370117f, 2242.030029f, 5481.740234f, 4.086020f } },
                { 15, true,  { -7192.959961f, 2229.760010f, 5481.600098f, 5.548930f } },
            }
        },
        {
            11, {
                { -2, false, { -7244.700195f, 2172.399902f, 5476.540039f, 0.383477f } },
                { -1, false, { -7254.990234f, 2215.709961f, 5472.819824f, 0.032025f } },
                { 5,  false, { -7215.209961f, 2178.070068f, 5476.660156f, 2.833870f } },
                { 16, true,  { -7247.549805f, 2226.280029f, 5471.049805f, 4.947340f } },
            }
        },
        {
            13, {
                { 7,  false, { -7016.520020f, 2319.290039f, 5470.600098f, 2.273530f } },
                { 8,  false, { -7052.080078f, 2317.489990f, 5470.229980f, 1.041200f } },
                { 17, true,  { -7039.120117f, 2363.379883f, 5466.979980f, 5.252790f } },
                { 18, true,  { -7054.490234f, 2346.449951f, 5466.810059f, 6.198620F } },
            }
        },
        {
            14, {
                { 10, false, { -7157.959961f, 2255.169922f, 5477.839844f, 1.253930f } },
                { 15, true,  { -7169.109863f, 2278.280029f, 5473.009766f, 0.153264f } },
                { 19, true,  { -7147.319824f, 2299.350098f, 5472.870117f, 5.542960f } },
                { 18, true,  { -7130.009766f, 2302.870117f, 5472.779785f, 4.603050f } },
            }
        },
        {
            15, {
                { -1, false, { -7225.120117f, 2268.199951f, 5470.180176f, 0.283928f } },
                { 10, false, { -7203.609863f, 2239.510010f, 5477.279785f, 2.017270f } },
                { 14, true,  { -7183.790039f, 2275.989990f, 5471.910156f, 3.226150f } },
                { 20, true,  { -7220.279785f, 2292.639893f, 5469.850098f, 5.038420f } },
            }
        },
        {
            16, {
                { -2, false, { -7280.919922f, 2249.149902F, 5465.810059f, 0.695036f } },
                { -1, false, { -7304.959961f, 2272.479980f, 5465.790039f, 6.034660f } },
                { 11, false, { -7256.390137f, 2243.570068f, 5466.839844f, 2.417340f } },
                { 20, true,  { -7268.149902f, 2298.729980f, 5465.819824f, 4.245270f } },
            }
        },
        {
            17, {
                { 6,  false, { -7030.240234f, 2411.760010f, 5460.839844f, 3.135540f } },
                { 13, false, { -7051.649902f, 2386.969971f, 5462.729980f, 2.331510f } },
                { 18, true,  { -7081.660156f, 2395.439941f, 5460.470215f, 0.792141f } },
                { 21, true,  { -7087.649902f, 2419.120117f, 5459.899902f, 6.265200f } },
            }
        },
        {
            18, {
                { 13, false, { -7076.600098f, 2356.530029f, 5465.490234f, 2.649080f } },
                { 17, true,  { -7092.939941f, 2382.810059f, 5462.589844f, 3.965270f } },
                { 19, true,  { -7145.290039f, 2350.020020f, 5465.000000f, 0.323602f } },
                { 21, true,  { -7129.350098f, 2376.719971f, 5463.850098f, 5.096950f } },
            }
        },
        {
            19, {
                { 14, false, { -7161.089844f, 2312.949951f, 5469.270020f, 2.318420f } },
                { 18, true,  { -7162.359863f, 2343.510010f, 5464.520020f, 3.462070f } },
                { 20, true,  { -7206.129883f, 2322.870117f, 5464.080078f, 0.371477f } },
                { 21, true,  { -7183.310059f, 2353.030029f, 5464.089844f, 4.662920f } },
            }
        },
        {
            20, {
                { -1, false, { -7273.970215f, 2358.689941f, 5458.899902f, 5.568840f } },
                { 16, false, { -7263.450195f, 2317.120117f, 5463.350098f, 1.296630f } },
                { 19, true,  { -7219.950195f, 2320.189941f, 5464.109863f, 3.215270f } },
                { 21, true,  { -7232.290039f, 2360.050049f, 5460.270020f, 3.865680f } },
            }
        }
    }
};

std::map<std::pair<uint8, int8>, uint32> mazeDoors =
{
    { { 1,  2  }, 352966 },
    { { 1,  6  }, 352975 },
    { { 2,  3  }, 352945 },
    { { 2,  7  }, 352963 },
    { { 3,  4  }, 352934 },
    { { 3,  8  }, 352957 },
    { { 4,  5  }, 352973 },
    { { 4,  9  }, 352955 },
    { { 5,  10 }, 352958 },
    { { 5,  11 }, 352959 },
    { { 6,  7  }, 352953 },
    { { 6,  17 }, 352944 },
    { { 7,  13 }, 352956 },
    { { 8,  9  }, 352961 },
    { { 8,  13 }, 352960 },
    { { 8,  14 }, 352948 },
    { { 9,  10 }, 352979 },
    { { 10, 14 }, 352981 },
    { { 10, 15 }, 352965 },
    { { 11, 16 }, 352962 },
    { { 13, 18 }, 352935 },
    { { 14, 15 }, 352936 },
    { { 14, 18 }, 352950 },
    { { 14, 19 }, 352980 },
    { { 16, 20 }, 352967 },
    { { 13, 17 }, 352972 },
    { { 15, 20 }, 352969 },
    { { 17, 18 }, 352970 },
    { { 17, 21 }, 352942 },
    { { 18, 19 }, 352971 },
    { { 18, 21 }, 352941 },
    { { 19, 20 }, 352982 },
    { { 19, 21 }, 352943 },
    { { 20, 21 }, 352939 },
};

std::map<uint8, uint32> MazeSymbolsVisualKits
{
    { 1, SPELL_MAZE_VISUAL_KIT_EMPTY_FLOWER         },
    { 2, SPELL_MAZE_VISUAL_KIT_EMPTY_FLOWER_CIRCLE  },
    { 3, SPELL_MAZE_VISUAL_KIT_FILLED_FLOWER        },
    { 4, SPELL_MAZE_VISUAL_KIT_FILLED_FLOWER_CIRCLE },
    { 5, SPELL_MAZE_VISUAL_KIT_EMPTY_LEAF           },
    { 6, SPELL_MAZE_VISUAL_KIT_EMPTY_LEAF_CIRCLE    },
    { 7, SPELL_MAZE_VISUAL_KIT_FILLED_LEAF          },
    { 8, SPELL_MAZE_VISUAL_KIT_FILLED_LEAF_CIRCLE   },
};

DoorData const doorData[] =
{
    { GO_INGRA_MALOCK_EXIT_DOOR,    DATA_INGRA_MALOCH,  DOOR_TYPE_PASSAGE },
    { GO_MISTCALLER_EXIT_DOOR_1,    DATA_MISTCALLER,    DOOR_TYPE_PASSAGE },
    { GO_MISTCALLER_EXIT_DOOR_2,    DATA_MISTCALLER,    DOOR_TYPE_PASSAGE },
    { GO_MISTCALLER_EXIT_DOOR_3,    DATA_MISTCALLER,    DOOR_TYPE_PASSAGE },
    { GO_MISTCALLER_EXIT_DOOR_4,    DATA_MISTCALLER,    DOOR_TYPE_PASSAGE },
    { GO_MISTCALLER_EXIT_DOOR_5,    DATA_MISTCALLER,    DOOR_TYPE_PASSAGE },
    { GO_TREDOVA_EXIT_DOOR,         DATA_TREDOVA,       DOOR_TYPE_PASSAGE },
};

struct instance_mists_of_tirna_scithe : public InstanceScript
{
    instance_mists_of_tirna_scithe(InstanceMap* map) : InstanceScript(map)
    {
        SetHeaders(DataHeader);
        SetBossNumber(EncounterCount);
        LoadDoorData(doorData);
    }

    void OnPlayerEnter(Player* player) override
    {
        if (_nextMazeZone == 0)
        {
            InitMazeHelpers();
            GenerateNextMazeZone();
        }
    }

    void SetData(uint32 index, uint32 data) override
    {
        if (index == DATA_MAZE_DOOR_REACHED)
        {
            if (data == CLUE_STALKER_NO_ACTION)
                return;

            if (data == CLUE_STALKER_WRONG_CHOICE)
            {
                DoCastSpellOnPlayers(SPELL_TELEPORT_TO_MAZE_ENTRANCE);
                DoPlayConversation(SPELL_CONVERSATION_MAZE_TAUNT_1 + urand(0, 2));
                return;
            }

            uint8 minZoneId = std::min(_currentMazeZone, _nextMazeZone);
            uint8 maxZoneId = std::max(_currentMazeZone, _nextMazeZone);

            DoPlayConversation(SPELL_CONVERSATION_MAZE_SUCCESS_1 + urand(0, 2));

            if (GameObject* door = GetGameObject(mazeDoors[std::make_pair(minZoneId, maxZoneId)]))
                HandleGameObject(door->GetGUID(), true, door);

            for (MazeLink link : mazeLinks[_currentMazeZone])
                if (Creature* clueStalker = instance->GetCreature(mazeHelperMap[std::make_pair(_currentMazeZone, link.ZoneID)]))
                    clueStalker->AI()->SetData(0, CLUE_STALKER_NO_ACTION);

            _currentMazeZone = _nextMazeZone;

            if (_currentMazeZone != 21)
                GenerateNextMazeZone();
        }

        InstanceScript::SetData(index, data);
    }

    uint32 GetData(uint32 DataId) const override
    {
        if (DataId == DATA_MAZE_NEXT_DOOR)
            return _nextMazeZone;

        return InstanceScript::GetData(DataId);
    }

private:

    void InitMazeHelpers()
    {
        for (auto linkItr : mazeLinks)
            for (auto link : linkItr.second)
                if (Creature* clueStalker = instance->SummonCreature(NPC_CLUE_STALKER, link.LinkPosition))
                    mazeHelperMap[std::make_pair(linkItr.first, link.ZoneID)] = clueStalker->GetGUID();
    }

    void GenerateNextMazeZone()
    {
        _alreadyDoneMazeZone.insert(_currentMazeZone);

        std::vector<MazeLink> availableLinks;

        for (MazeLink link : mazeLinks[_currentMazeZone])
            if (link.CanAccess)
                if (_alreadyDoneMazeZone.find(link.ZoneID) == _alreadyDoneMazeZone.end())
                    availableLinks.push_back(link);

        _nextMazeZone = Trinity::Containers::SelectRandomContainerElement(availableLinks).ZoneID;

        std::vector<uint8> symbols = GenerateSymbols();

        uint8 symbolIndex = 0;
        for (MazeLink link : mazeLinks[_currentMazeZone])
        {
            if (Creature* clueStalker = instance->GetCreature(mazeHelperMap[std::make_pair(_currentMazeZone, link.ZoneID)]))
            {
                if (link.ZoneID == _nextMazeZone)
                    clueStalker->AI()->SetData(MazeSymbolsVisualKits[symbols[0]], CLUE_STALKER_SELECTED_CLUE);
                else
                {
                    uint32 behaviour = CLUE_STALKER_WRONG_CHOICE;
                    if (_alreadyDoneMazeZone.find(link.ZoneID) != _alreadyDoneMazeZone.end())
                        behaviour = CLUE_STALKER_NO_ACTION;

                    clueStalker->AI()->SetData(MazeSymbolsVisualKits[symbols[++symbolIndex]], behaviour);
                }
            }
        }
    }

    uint32 _nextMazeZone = 0;
    uint32 _currentMazeZone = 3;

    std::map<std::pair<uint8, int8>, ObjectGuid> mazeHelperMap;
    std::set<uint8> _alreadyDoneMazeZone = { 0 };
};

void AddSC_instance_mists_of_tirna_scithe()
{
    RegisterInstanceScript(instance_mists_of_tirna_scithe, 2290);
}
