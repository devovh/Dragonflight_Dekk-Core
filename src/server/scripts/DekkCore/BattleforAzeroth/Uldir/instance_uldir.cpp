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
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectMgr.h"
#include "uldir.h"

DoorData const doorData[] =
{
    { GO_MYTHRAX_DOOR, DATA_MYTHRAX, DOOR_TYPE_PASSAGE },
};

class instance_uldir : public InstanceMapScript
{
public:
    instance_uldir() : InstanceMapScript("instance_uldir", 1861) {}

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_uldir_InstanceMapScript(map);
    }

    struct instance_uldir_InstanceMapScript : public InstanceScript
    {
        instance_uldir_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {

            SetBossNumber(EncounterCount);
        }

        void Initialize()
        {
            LoadDoorData(doorData);
        }

        void OnCreatureCreate(Creature* cre) override
        {
            if (CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(cre->GetEntry()))
            {
                if (cInfo->MechanicImmuneMask == 0)
                {
                    //if (instance->IsMythic())
                      //  if (cre->GetFaction() == 16)
                      //      cre->SetBaseHealth(cre->GetMaxHealth() * 3.0f);

                  //  if (instance->IsHeroic())
                      //  if (cre->GetFaction() == 16)
                          //  cre->SetBaseHealth(cre->GetMaxHealth() * 6.0f);

                   //  else
                     //   if (cre->GetFaction() == 16)
                       //     cre->SetBaseHealth(cre->GetMaxHealth() * 9.8f);
                }
                // if (cInfo->rank == 3)
                // {
                //     cre->SetBaseHealth(cre->GetMaxHealth() * 4.5f);
                // }
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
            case GO_MYTHRAX_DOOR:
                go->SetGoState(GO_STATE_READY);
                break;
            }
        }
    };
};


void AddSC_instance_uldir()
{
    new (instance_uldir);
}
