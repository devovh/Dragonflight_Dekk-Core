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
#include "sanguine_depths.h"


DungeonEncounterData const encounters[] =
{
    { DATA_KRYXIS,          {{ 2360 }} },
    { DATA_EXECUTOR,        {{ 2361 }} },
    { DATA_BERYLLIA,        {{ 2362 }} },
    { DATA_KAAL,            {{ 2363 }} },
};

class instance_sanguine_depths : public InstanceMapScript
{
    public:
        instance_sanguine_depths() : InstanceMapScript(SDScriptName, 2284) { }

struct instance_sanguine_depths_InstanceMapScript : public InstanceScript
{
    instance_sanguine_depths_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
    {
        SetHeaders(DataHeader);
        SetBossNumber(EncounterCount);
        LoadDungeonEncounterData(encounters);
    }
};

InstanceScript* GetInstanceScript(InstanceMap* map) const override
{
    return new instance_sanguine_depths_InstanceMapScript(map);
}
};

void AddSC_instance_sanguine_depths()
{
    new instance_sanguine_depths();
}
