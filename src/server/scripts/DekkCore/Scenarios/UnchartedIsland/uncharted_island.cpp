/*
 * Copyright (C) 2020 LatinCoreTeam
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

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "CombatAI.h"
#include "GameObjectAI.h"
#include "GameObject.h"
#include "Scenario.h"
#include "ScriptMgr.h"
#include "uncharted_island.h"
#include "LFGMgr.h"
#include "WorldSession.h"
#include "World.h"
#include "AzeriteItem.h"
#include "Object.h"

class go_pouch_of_azerite : public GameObjectScript
{
public:
    go_pouch_of_azerite() : GameObjectScript("go_pouch_of_azerite") { }

    struct go_pouch_of_azeriteAI : public GameObjectAI
    {
        go_pouch_of_azeriteAI(GameObject* go) : GameObjectAI(go) { }

        bool OnGossipHello(Player* player) override
        {
            if (Item* item = player->GetItemByEntry(158075)) // Heart of Azeroth
                item->ToAzeriteItem()->GiveXP(50);
            return false;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_pouch_of_azeriteAI(go);
    }
};

class go_azerite_crystal : public GameObjectScript
{
public:
    go_azerite_crystal() : GameObjectScript("go_azerite_crystal") { }

    struct go_azerite_crystalAI : public GameObjectAI
    {
        go_azerite_crystalAI(GameObject* go) : GameObjectAI(go) { }

        bool OnGossipHello(Player* player) override
        {
            if (Item* item = player->GetItemByEntry(158075)) // Heart of Azeroth
                item->ToAzeriteItem()->GiveXP(75);

            if (Scenario* scenario = player->GetScenario())
            {
                if (scenario->GetStep() == sScenarioStepStore.LookupEntry(STEP_INVESTIGATE))
                {
                    scenario->CompleteStep(sScenarioStepStore.LookupEntry(STEP_INVESTIGATE));
                    scenario->SetStep(sScenarioStepStore.LookupEntry(STEP_MINE));
                }
            }

            return false;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_azerite_crystalAI(go);
    }
};

void AddSC_uncharted_island_tutorial()
{
    new go_azerite_crystal();
    new go_pouch_of_azerite();
}
