/*
 * Copyright 2021 Thordekk
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
#include "AreaTriggerAI.h"
#include "ScriptMgr.h"

class instance_de_other_side : public InstanceMapScript
{
public:
    instance_de_other_side() : InstanceMapScript("instance_de_other_side", 2291) { }

    struct instance_de_other_side_InstanceMapScript : public InstanceScript
    {
        instance_de_other_side_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_de_other_side_InstanceMapScript(map);
    }

};

// AT to open Blood-wrought Door gameobject 364528
struct at_deotherside_hakkardoor : AreaTriggerAI
{
    at_deotherside_hakkardoor(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        Player* player = unit->ToPlayer();
        if (!player)
            return;

        if (GameObject* go = GetClosestGameObjectWithEntry(player, 364528, 35.0f))
            if (go->getLootState() == GO_READY)
                go->UseDoorOrButton();
    }
};


void AddSC_instance_de_other_side()
{
    new instance_de_other_side();
    RegisterAreaTriggerAI(at_deotherside_hakkardoor);
}
