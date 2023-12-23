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
#include "seat_of_triumvirate.h"

class instance_seat_of_triumvirate : public InstanceMapScript
{
public:
    instance_seat_of_triumvirate() : InstanceMapScript("instance_seat_of_triumvirate", 1753) { }

    struct instance_seat_of_triumvirate_InstanceMapScript : public InstanceScript
    {
        instance_seat_of_triumvirate_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_seat_of_triumvirate_InstanceMapScript(map);
    }

};

void AddSC_instance_seat_of_triumvirate()
{
    new instance_seat_of_triumvirate();
}
