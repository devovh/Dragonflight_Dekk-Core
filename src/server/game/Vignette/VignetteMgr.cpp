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

#include "VignetteMgr.h"
#include "ObjectAccessor.h"
#include "MiscPackets.h"
#include "AreaTrigger.h"
#include "Corpse.h"
#include "Conversation.h"
#include "GameObject.h"

namespace Vignette
{

Manager::Manager(Player const* player)
{
    _owner = player;
}

Manager::~Manager()
{
    _owner = nullptr;

    for (auto itr : _vignettes)
        delete itr.second;
}

Entity* Manager::CreateAndAddVignette(VignetteEntry const* vignetteEntry, uint32 const mapID, Type const vignetteType, Position const position, uint32 zoneID, ObjectGuid const sourceGuid /*= ObjectGuid::Empty*/)
{
    for (auto v : _vignettes)
        if (v.second->GetVignetteEntry()->ID == vignetteEntry->ID && v.second->GeSourceGuid() == sourceGuid)
            return nullptr;

    auto vignette = new Entity(vignetteEntry, mapID);
    vignette->Create(vignetteType, position, zoneID, sourceGuid);

    _vignettes.insert(std::make_pair(vignette->GetGuid(), vignette));
    _addedVignette.insert(vignette->GetGuid());

    return vignette;
}

void Manager::DestroyAndRemoveVignetteByEntry(VignetteEntry const* vignetteEntry)
{
    if (!vignetteEntry)
        return;

    for (auto itr = _vignettes.begin(); itr != _vignettes.end();)
    {
        if (itr->second->GetVignetteEntry()->ID == vignetteEntry->ID)
        {
            delete itr->second;
            _removedVignette.insert(itr->first);
            itr = _vignettes.erase(itr);
            continue;
        }

        ++itr;
    }
}

void Manager::DestroyAndRemoveVignettes(std::function<bool(Entity*)> const& lambda)
{
    for (auto itr = _vignettes.begin(); itr != _vignettes.end();)
    {
        if (lambda(itr->second))
        {
            delete itr->second;
            _removedVignette.insert(itr->first);
            itr = _vignettes.erase(itr);
            continue;
        }

        ++itr;
    }
}

void Manager::SendVignetteUpdateToClient()
{
    WorldPackets::Misc::VignetteUpdate packet;
    packet.ForceUpdate = false;

    for (auto x : _removedVignette)
        packet.Removed.IDs.emplace_back(x);

    for (auto v : _updatedVignette)
    {
        auto x = _vignettes.find(v);
        if (x == _vignettes.end())
            continue;

        auto vignette = x->second;
        packet.Updated.Data.emplace_back(vignette->GetGuid(), vignette->GetPosition(), vignette->GetVignetteEntry()->ID, vignette->GetZoneID());
        packet.Updated.IdList.IDs.emplace_back(v);
    }

    for (auto v : _addedVignette)
    {
        auto x = _vignettes.find(v);
        if (x == _vignettes.end())
            continue;

        auto vignette = x->second;
        packet.Added.Data.emplace_back(vignette->GetGuid(), vignette->GetPosition(), vignette->GetVignetteEntry()->ID, vignette->GetZoneID());
        packet.Added.IdList.IDs.emplace_back(v);
    }

    _owner->SendDirectMessage(packet.Write());

    _updatedVignette.clear();
    _addedVignette.clear();
    _removedVignette.clear();
}

void Manager::Update()
{
    for (auto itr : _vignettes)
    {
        auto vignette = itr.second;

        if (vignette->GeSourceGuid().IsUnit())
            if (auto sourceCreature = ObjectAccessor::GetCreature(*_owner, vignette->GeSourceGuid()))
                vignette->UpdatePosition(sourceCreature->GetPosition());

        if (vignette->NeedClientUpdate())
        {
            _updatedVignette.insert(vignette->GetGuid());
            vignette->ResetNeedClientUpdate();
        }
    }

    if (!_addedVignette.empty() || !_updatedVignette.empty() || !_removedVignette.empty())
        SendVignetteUpdateToClient();
}

VignetteEntry const* GetVignetteEntryFromWorldObject(WorldObject const* target)
{
    uint32 vignetteId = 0;
    if (target->IsCreature())
        vignetteId = target->ToCreature()->GetCreatureTemplate()->VignetteID;

   if (target->IsGameObject())
        vignetteId = target->ToGameObject()->GetGOInfo()->GetVignetteId();

    if (!vignetteId)
        return nullptr;

    return sVignetteStore.LookupEntry(vignetteId);
}

uint32 GetTrackingQuestIdFromWorldObject(WorldObject const* target)
{
    uint32 trackingQuest = 0;
    if (target->IsCreature())
        trackingQuest = target->ToCreature()->GetCreatureTemplate()->TrackingQuestID;

    if (target->IsGameObject())
        trackingQuest = target->ToGameObject()->GetGOInfo()->GetTrackingQuestId();

    return trackingQuest;
}

Type GetDefaultVignetteTypeFromWorldObject(WorldObject const* target) 
{
    switch (target->GetTypeId())
    {
        case TYPEID_UNIT:
            return Type::SourceCreature;
        case TYPEID_GAMEOBJECT:
            return Type::SourceGameObject;
        default:
            return Type::SourceScript;
    }
}

Type GetTrackingVignetteTypeFromWorldObject(WorldObject const* target)
{
    switch (target->GetTypeId())
    {
        case TYPEID_UNIT:
            return Type::SourceRare;
        case TYPEID_GAMEOBJECT:
            return Type::SourceTreasure;
        default:
            return Type::SourceScript;
    }
}

void Manager::OnWorldObjectAppear(WorldObject const* target)
{
    auto vignetteEntry = GetVignetteEntryFromWorldObject(target);
    if (vignetteEntry == nullptr)
        return;

    auto type = GetDefaultVignetteTypeFromWorldObject(target);

    if (auto trackingQuest = GetTrackingQuestIdFromWorldObject(target))
        if (!_owner->IsQuestBitFlaged(sDB2Manager.GetQuestUniqueBitFlag(trackingQuest)))
            type = GetTrackingVignetteTypeFromWorldObject(target);

    if (CanSeeVignette(target, vignetteEntry->ID))
        CreateAndAddVignette(vignetteEntry, target->GetMapId(), type, target->GetPosition(), target->GetZoneId(), target->GetGUID());
}

void Manager::OnWorldObjectDisappear(WorldObject const* target)
{
    if (!GetVignetteEntryFromWorldObject(target))
        return;

    DestroyAndRemoveVignettes([target](Entity const* vignette) -> bool
    {
        return vignette->GeSourceGuid() == target->GetGUID() && vignette->GetVignetteType() != Type::SourceScript;
    });
}

bool Manager::CanSeeVignette(WorldObject const* obj, uint32 vignetteID)
{
    if (!vignetteID || (obj->IsUnit() && (!obj->ToUnit()->IsAlive() || obj->ToUnit()->IsPet())))
        return false;

    VignetteEntry const* vignette = sVignetteStore.LookupEntry(vignetteID);
    if (PlayerConditionEntry const* playerCondition = sPlayerConditionStore.LookupEntry(vignette->PlayerConditionID))
        if (!vignette || !sConditionMgr->IsPlayerMeetingCondition(const_cast<Player*>(_owner), playerCondition))
            return false;

    if (!sConditionMgr->HasConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_VIGNETTE, vignetteID))
        return false;

    auto guid = obj->GetGUID();
    uint32 trackingQuest = 0;

    if (guid.IsUnit())
        if (auto creatureSource = ObjectAccessor::GetCreature(*_owner, guid))
        {
            trackingQuest = creatureSource->GetTrackingQuestID();
            if (trackingQuest && _owner->IsQuestRewarded(trackingQuest))
                return false;
        }

    if (guid.IsGameObject())
        if (auto goSource = ObjectAccessor::FindGameObject(guid))
        {
            trackingQuest = goSource->GetGOInfo()->GetTrackingQuestId();
            if (trackingQuest && _owner->IsQuestRewarded(trackingQuest))
                return false;
        }

    return true;
}

}
