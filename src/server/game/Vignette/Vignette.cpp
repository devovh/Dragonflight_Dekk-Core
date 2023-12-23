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

#include "Vignette.h"
#include "ObjectMgr.h"
#include "Object.h"
#include "ObjectGuid.h"

namespace Vignette
{
    Entity::Entity(VignetteEntry const* vignetteEntry, uint32 mapID) : _zoneID{ 0 }
    {
        ASSERT(vignetteEntry != nullptr);
        _needClientUpdate = false;
        _map = mapID;
        _vignetteEntry = vignetteEntry;
        _type = Type::SourceCreature;
    }

    Entity::~Entity() = default;

    void Entity::Create(Type type, Position const& position, uint32 zoneID, ObjectGuid sourceGuid)
    {
        ObjectGuid::LowType guidlow = sObjectMgr->GetGenerator<HighGuid::Vignette>().Generate();
        _guid = ObjectGuid::Create<HighGuid::Vignette>(_map, _vignetteEntry->ID, guidlow);

        _type = type;
        _position = position;
        _zoneID = zoneID;
        _sourceGuid = sourceGuid;
    }

    void Entity::UpdatePosition(Position newPosition)
    {
        if (static_cast<int32>(_position.GetPositionX()) == static_cast<int32>(newPosition.GetPositionX()) && static_cast<int32>(_position.GetPositionY()) == static_cast<int32>(newPosition.GetPositionY()))
            return;

        _position = newPosition;
        _needClientUpdate = true;
    }

    void Entity::ResetNeedClientUpdate()
    {
        _needClientUpdate = false;
    }
}
