/*
 * This file is part of DEKKCORETEAM
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

#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "Zone_BrackenhideHollow.h"

//DungeonEncounterData const encounters[] =
//{
//   // { DATA_RIRA_HACKLAW, {{  }} },
//     // { DATA, {{  }} },
//     // { DATA, {{  }} },
//};

ObjectData const creatureData[] =
{
    { BOSS_RIRA_HACKLAW,    DATA_RIRA_HACKLAW          },
//     { BOSS_,    DATA_          },
//    { BOSS_,    DATA_          },
//    { BOSS_,    DATA_          },
    { 0,                            0                                  }  // END
};


class instance_brackenhide_hollow : public InstanceMapScript
{
public:
    instance_brackenhide_hollow() : InstanceMapScript(BRHScriptName, 2520) { }

    struct instance_brackenhide_hollow_InstanceMapScript : public InstanceScript
    {
        instance_brackenhide_hollow_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
          //  LoadObjectData(creatureData, nullptr);
           // LoadDoorData(doorData);
          //  LoadDungeonEncounterData(encounters);
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_brackenhide_hollow_InstanceMapScript(map);
    }
};

void AddSC_instance_brackenhide_hollow()
{
    new instance_brackenhide_hollow();
}
