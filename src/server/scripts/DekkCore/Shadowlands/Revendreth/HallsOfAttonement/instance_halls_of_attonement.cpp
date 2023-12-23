/*
 * Copyright 2022 BaldazzarCore-Shadowlands
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
#include "InstanceScript.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "halls_of_attonement.h"

ObjectGuid echelonExitDoorGuid;
ObjectGuid echelonEntranceDoorGuid;
ObjectGuid aleezEntranceDoorLeftGuid;
ObjectGuid aleezEntranceDoorRightGuid;
ObjectGuid aleezExitDoorGuid;

uint32 HoAEncounters = 4;

DoorData const doorData[] =
{
    { GO_ECHELON_ENTRANCE_DOOR, DATA_ECHELON, DOOR_TYPE_PASSAGE },
    { GO_ECHELON_EXIT_DOOR, DATA_ECHELON, DOOR_TYPE_PASSAGE },
    { GO_ALEEZ_LEFT_DOOR, DATA_HIGH_ADJUDICATOR_ALEEZ, DOOR_TYPE_PASSAGE },
    { GO_ALEEZ_RIGHT_DOOR, DATA_HIGH_ADJUDICATOR_ALEEZ, DOOR_TYPE_PASSAGE },
};

struct instance_halls_of_attonement : public InstanceScript
{
    instance_halls_of_attonement(InstanceMap* map) : InstanceScript(map)
    {
        LoadDoorData(doorData);
        SetBossNumber(HoAEncounters);
    }

    void OnGameObjectCreate(GameObject* go) override
    {
        switch (go->GetEntry())
        {
        case GO_ECHELON_EXIT_DOOR:
            echelonExitDoorGuid = go->GetGUID();
            break;

        case GO_ECHELON_ENTRANCE_DOOR:
            echelonExitDoorGuid = go->GetGUID();
            break;

        case GO_ALEEZ_LEFT_DOOR:
            aleezEntranceDoorLeftGuid = go->GetGUID();
            break;

        case GO_ALEEZ_RIGHT_DOOR:
            aleezEntranceDoorRightGuid = go->GetGUID();
            break;
        }
    }

    bool SetBossState(uint32 type, EncounterState state) override
    {
        if (!InstanceScript::SetBossState(type, state))
            return false;

        switch(type)
        {
        case DATA_ECHELON:
            if (state == DONE)
            {               
                HandleGameObject(echelonEntranceDoorGuid, true);
                HandleGameObject(echelonExitDoorGuid, true);
            }
            if (state == IN_PROGRESS)
                HandleGameObject(echelonEntranceDoorGuid, false);
            break;

        case DATA_HIGH_ADJUDICATOR_ALEEZ:
            if (state == IN_PROGRESS)
            {
                HandleGameObject(aleezEntranceDoorLeftGuid, false);
                HandleGameObject(aleezEntranceDoorRightGuid, false);
            }
            if (state == DONE)
            {
                HandleGameObject(aleezEntranceDoorLeftGuid, true);
                HandleGameObject(aleezEntranceDoorRightGuid, true);
            }
            break;
        }

        return true;
    }
};

void AddSC_instance_halls_of_attonement()
{
    RegisterInstanceScript(instance_halls_of_attonement, 2287);
}
