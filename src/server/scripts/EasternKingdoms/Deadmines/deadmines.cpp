/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
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

#include "GameObject.h"
#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "deadmines.h"

class go_defias_cannon : public GameObjectScript
{
public:
    go_defias_cannon() : GameObjectScript("go_defias_cannon") { }

    struct go_defias_cannonAI : public GameObjectAI
    {
        go_defias_cannonAI(GameObject* go) : GameObjectAI(go)
        {
        }

    bool OnGossipHello(Player* pPlayer) override
    {
        InstanceScript* instance = me->GetInstanceScript();

        if (!instance)
            return false;

        instance->SetData(DATA_CANNON_EVENT, CANNON_BLAST_INITIATED);
        return false;

        if (GameObject* ironCladDoor = me->FindNearestGameObject(GO_IRONCLAD_DOOR, 30.0f))
        {
            me->SetGoState(GO_STATE_ACTIVE);
        }
        return true;   
    }
};
    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_defias_cannonAI(go);
    }

};

void AddSC_deadmines()
{
    new go_defias_cannon();
}
