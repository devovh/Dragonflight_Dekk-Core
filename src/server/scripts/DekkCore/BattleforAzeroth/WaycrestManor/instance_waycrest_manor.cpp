/*
 * Copyright 2021 DEKKCORE
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

#include "ScriptMgr.h"
#include "Player.h"
#include "InstanceScript.h"
#include "waycrest_manor.h"

// ObjectData const creatureData[] =
// {
//     { NPC_LADY_WAYCREST_INTRO, DATA_INTRO     },
//     { 0,                          0                      } // END
// };

class instance_waycrest_manor : public InstanceMapScript
{
public:
    instance_waycrest_manor() : InstanceMapScript("instance_waycrest_manor", 1862) { }

    struct instance_waycrest_manor_InstanceMapScript : public InstanceScript
    {
        instance_waycrest_manor_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadObjectData(nullptr, nullptr); // TO BE ADDED (by someone that is not hoff please)
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_waycrest_manor_InstanceMapScript(map);
    }

};

void AddSC_instance_waycrest_manor()
{
    new instance_waycrest_manor();
}
