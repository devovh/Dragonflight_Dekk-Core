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

#ifndef VIGNETTE_H
#define VIGNETTE_H

#include "Common.h"
#include "DB2Stores.h"
#include "Object.h"

namespace Vignette
{
enum class Type : uint8
{
    SourceCreature,           ///< Used for vignette linked to creature, player show the vignette if the creature is in the view-range
    SourceGameObject,         ///< Used for vignette linked to gameobject, player show the vignette if the gameobject is in the view-range
    SourceRare,               ///< Used for vignette linked to rare creature, player show the vignette if the creature is in the view-range and if the player doesn't have the tracking quest
    SourceTreasure,           ///< Used for vignette linked to treasure gameobject, player show the vignette if the treasure is in the view-range and if the player doesn't have the tracking quest
    SourceScript              ///< Used for script, that kind of vignette are never remove automaticaly
};

class Entity
{
    friend class Manager;

public:
    void UpdatePosition(Position newPosition);
    void ResetNeedClientUpdate();

    ObjectGuid GetGuid() const { return _guid; }
    ObjectGuid GeSourceGuid() const { return _sourceGuid; }
    Type GetVignetteType() const { return _type; }
    bool NeedClientUpdate() const { return _needClientUpdate; }
    Position const& GetPosition() const { return _position; }
    uint32 GetZoneID() const { return _zoneID; }
    VignetteEntry const* GetVignetteEntry() const { return _vignetteEntry; }

private:
    Entity(VignetteEntry const* vignetteEntry, uint32 mapID);
    ~Entity();

    void Create(Type type, Position const& position, uint32 zoneID, ObjectGuid sourceGuid);

    VignetteEntry const* _vignetteEntry;
    ObjectGuid _guid;
    ObjectGuid _sourceGuid;
    Position _position;
    uint32 _map;
    uint32 _zoneID;
    Type _type;

protected:
    bool _needClientUpdate;
};

}
#endif
