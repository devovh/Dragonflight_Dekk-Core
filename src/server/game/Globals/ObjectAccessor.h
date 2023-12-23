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

#ifndef TRINITY_OBJECTACCESSOR_H
#define TRINITY_OBJECTACCESSOR_H

#include "ObjectGuid.h"
#include <shared_mutex>
#include <unordered_map>

class AreaTrigger;
class Conversation;
class Corpse;
class Creature;
class DynamicObject;
class GameObject;
class Map;
class Object;
class Pet;
class Player;
class SceneObject;
class Transport;
class Unit;
class WorldObject;

template <class T>
class TC_GAME_API HashMapHolder
{
    //Non instanceable only static
    HashMapHolder() { }

public:

    typedef std::unordered_map<ObjectGuid, T*> MapType;

    static void Insert(T* o);

    static void Remove(T* o);

    static T* Find(ObjectGuid guid);

    static MapType& GetContainer();

    static std::shared_mutex* GetLock();
};

namespace ObjectAccessor
{
    // these functions return objects only if in map of specified object
    TC_GAME_API WorldObject* GetWorldObject(WorldObject const&, ObjectGuid const&);
    TC_GAME_API Object* GetObjectByTypeMask(WorldObject const&, ObjectGuid const&, uint32 typemask);
    TC_GAME_API Corpse* GetCorpse(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API GameObject* GetGameObject(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API Transport* GetTransport(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API DynamicObject* GetDynamicObject(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API AreaTrigger* GetAreaTrigger(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API SceneObject* GetSceneObject(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API Conversation* GetConversation(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API Unit* GetUnit(WorldObject const&, ObjectGuid const& guid);
    TC_GAME_API Creature* GetCreature(WorldObject const& u, ObjectGuid const& guid);
    TC_GAME_API Pet* GetPet(WorldObject const&, ObjectGuid const& guid);
    TC_GAME_API Player* GetPlayer(Map const*, ObjectGuid const& guid);
    TC_GAME_API Player* GetPlayer(WorldObject const&, ObjectGuid const& guid);
    TC_GAME_API Creature* GetCreatureOrPetOrVehicle(WorldObject const&, ObjectGuid const&);
    // DekkCore
    TC_GAME_API Unit* FindUnit(ObjectGuid const& g);
    TC_GAME_API GameObject* FindGameObject(ObjectGuid const& guid);
    TC_GAME_API Creature* FindCreature(ObjectGuid const& guid);
    TC_GAME_API Player* GetObjectInWorld(ObjectGuid guid, Player* /*typeSpecifier*/);
    template<class T> static T* GetObjectInOrOutOfWorld(ObjectGuid guid, T* /*typeSpecifier*/)
    {
        return HashMapHolder<T>::Find(guid);
    }
    TC_GAME_API Player* FindPlayer(Map* map, ObjectGuid guid);

    // returns object if is in map
    template<class T> static T* GetObjectInMap(ObjectGuid guid, Map* map, T* /*typeSpecifier*/)
    {
        // ASSERT(map);
        if (!map)
            return nullptr;

        if (T* obj = GetObjectInWorld(guid, static_cast<T*>(nullptr)))
            // if (obj->GetMap() == map && !obj->IsPreDelete())
            return obj;

        return nullptr;
    }

    template<class T> static T* GetObjectInWorld(uint32 mapid, float x, float y, ObjectGuid guid, T* /*fake*/)
    {
        T* obj = HashMapHolder<T>::Find(guid);
        if (!obj || obj->GetMapId() != mapid || obj->IsPreDelete())
            return nullptr;

        CellCoord p = Trinity::ComputeCellCoord(x, y);
        if (!p.IsCoordValid())
        {
            //TC_LOG_ERROR("LOG_FILTER_GENERAL", "ObjectAccessor::GetObjectInWorld: invalid coordinates supplied X:{} Y:{} grid cell [{}:{}]", x, y, p.x_coord, p.y_coord);
            return nullptr;
        }

        CellCoord q = Trinity::ComputeCellCoord(obj->GetPositionX(), obj->GetPositionY());
        if (!q.IsCoordValid())
        {
            //TC_LOG_ERROR("LOG_FILTER_GENERAL", "ObjectAccessor::GetObjecInWorld: object (GUID: {} TypeId: {}) has invalid coordinates X:{} Y:{} grid cell [{}:{}]", obj->GetGUIDLow(), obj->GetTypeId(), obj->GetPositionX(), obj->GetPositionY(), q.x_coord, q.y_coord);
            return nullptr;
        }

        int32 dx = int32(p.x_coord) - int32(q.x_coord);
        int32 dy = int32(p.y_coord) - int32(q.y_coord);

        if (dx > -2 && dx < 2 && dy > -2 && dy < 2)
            return obj;
        return nullptr;
    }

    // DekkCore
    // these functions return objects if found in whole world
    // ACCESS LIKE THAT IS NOT THREAD SAFE
    TC_GAME_API Player* FindPlayer(ObjectGuid const&);
    TC_GAME_API Player* FindPlayerByName(std::string_view name);
    TC_GAME_API Player* FindPlayerByLowGUID(ObjectGuid::LowType lowguid);

    // this returns Player even if he is not in world, for example teleporting
    TC_GAME_API Player* FindConnectedPlayer(ObjectGuid const&);
    TC_GAME_API Player* FindConnectedPlayerByName(std::string_view name);

    // when using this, you must use the hashmapholder's lock
    TC_GAME_API HashMapHolder<Player>::MapType const& GetPlayers();

    template<class T>
    void AddObject(T* object)
    {
        HashMapHolder<T>::Insert(object);
    }

    template<class T>
    void RemoveObject(T* object)
    {
        HashMapHolder<T>::Remove(object);
    }

    template<>
    void AddObject(Player* player);

    template<>
    void RemoveObject(Player* player);

    TC_GAME_API void SaveAllPlayers();
};

#endif
