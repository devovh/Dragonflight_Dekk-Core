/*
 * Copyright (C) DekkCore 
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

#include "CovenantCampaign.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "GameObject.h"
#include "GarrisonMgr.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectMgr.h"


CovenantCampaign::CovenantCampaign(Player* owner) : Garrison(owner)
{
    _garrisonType = GARRISON_TYPE_COVENANT;
}

bool CovenantCampaign::LoadFromDB()
{
    if (!Garrison::LoadFromDB())
        return false;

    return true;
}

void CovenantCampaign::SaveToDB(CharacterDatabaseTransaction& trans)
{
    Garrison::SaveToDB(trans);
}

bool CovenantCampaign::Create(uint32 garrSiteId)
{
    if (!Garrison::Create(garrSiteId))
        return false;

    return true;
}

void CovenantCampaign::Delete()
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    DeleteFromDB(trans);
    CharacterDatabase.CommitTransaction(trans);

    Garrison::Delete();
}

bool CovenantCampaign::IsAllowedArea(AreaTableEntry const* area) const
{
    return area->GetFlags2().HasFlag(AreaFlags2::IsGarrison);
}
