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

#include "WorldSession.h"
#include "BattlePetMgr.h"
#include "BattlePetPackets.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "TemporarySummon.h"
#include "BattlePetSystem.h"
#include "Hoff.h" // < Hoff

void WorldSession::HandleBattlePetRequestJournal(WorldPackets::BattlePet::NullCmsg& /*battlePetRequestJournal*/)
{
    SendBattlePetJournal();
}


void WorldSession::HandleBattlePetSetBattleSlot(WorldPackets::BattlePet::BattlePetSetBattleSlot& battlePetSetBattleSlot)
{
    if (m_isPetBattleJournalLocked)
        return;

    if (battlePetSetBattleSlot.Slot >= MAX_PET_BATTLE_SLOTS)
        return;

    if (auto battlePet = _player->GetBattlePet(battlePetSetBattleSlot.PetGuid))
    {
        auto petSlots = _player->GetBattlePetCombatTeam();

        for (uint8 i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
        {
            auto& slot = petSlots[i];
            if (slot && slot->Slot == battlePetSetBattleSlot.Slot)
            {
                slot->Slot = battlePet->Slot;
                slot->needSave = true;
            }
        }

        battlePet->Slot = battlePetSetBattleSlot.Slot;
        battlePet->needSave = true;
    }

    _player->UpdateBattlePetCombatTeam();
    //SendPetBattleSlotUpdates();
}

void WorldSession::HandleBattlePetModifyName(WorldPackets::BattlePet::BattlePetModifyName& battlePetModifyName)
{
    uint32 timeStamp = battlePetModifyName.Name.empty() ? 0 : time(nullptr);

    if (auto battlePet = _player->GetBattlePet(battlePetModifyName.PetGuid))
    {
        battlePet->Name = battlePetModifyName.Name;
        battlePet->NameTimeStamp = timeStamp;
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i) 
            battlePet->DeclinedNames[i] = battlePetModifyName.DeclinedNames.name[i];
        battlePet->needSave = true;
    }

    _player->SetBattlePetCompanionNameTimestamp(timeStamp);

    Creature* creature = _player->GetSummonedBattlePet();
    if (!creature)
        return;

    if (creature->GetBattlePetCompanionGUID() == battlePetModifyName.PetGuid)
    {
        creature->SetName(battlePetModifyName.Name);
        creature->SetBattlePetCompanionNameTimestamp(timeStamp);
    }
}

void WorldSession::HandleQueryBattlePetName(WorldPackets::BattlePet::QueryBattlePetName& queryBattlePetName)
{
    Creature* creature = Unit::GetCreature(*_player, queryBattlePetName.UnitGUID);
    if (!creature)
        return;

    std::shared_ptr<BattlePet> battlePet = nullptr;
    if (creature->GetOwner() && creature->GetOwner()->IsPlayer())
        battlePet = creature->GetOwner()->ToPlayer()->GetBattlePet(queryBattlePetName.BattlePetID);

    if (!battlePet)
        return;

    bool haveDeclinedNames = false;

    for (auto const& name : battlePet->DeclinedNames)
        if (!name.empty())
        {
            haveDeclinedNames = true;
            break;
        }

    WorldPackets::BattlePet::QueryBattlePetNameResponse response;
    response.BattlePetID = queryBattlePetName.BattlePetID;
    response.CreatureID = creature->GetEntry();
    response.Timestamp = creature->GetBattlePetCompanionNameTimestamp();
    if (creature->GetBattlePetCompanionNameTimestamp() != 0)
    {
        response.Allow = true;
        response.Name = creature->GetName();
        if (haveDeclinedNames)
        {
            response.HasDeclined = true;
            for (uint32 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                response.DeclinedNames[i] = battlePet->DeclinedNames[i];
        }
    }
    SendPacket(response.Write());
}

void WorldSession::HandleBattlePetDeletePet(WorldPackets::BattlePet::BattlePetDeletePet& battlePetDeletePet)
{
    auto battlePet = _player->GetBattlePet(battlePetDeletePet.PetGuid);
   /* if (!battlePet)
    {
        TC_LOG_ERROR("network", "CMSG_BATTLE_PET_DELETE - Player %s tryed to release Battle Pet %s which it doesn't own!",
            _player->GetGUID().ToString().c_str(), packet.BattlePetGUID.ToString().c_str());
        return;
    }*/
    //@TODO
    /*if (sDB2Manager.HasBattlePetSpeciesFlag(battlePet->Species, BATTLE_PET_SPECIES_FLAG_RELEASABLE))
    {
        TC_LOG_ERROR("network", "CMSG_BATTLE_PET_DELETE - Player %s tryed to release Battle Pet %s which isn't releasable!",
            _player->GetGUID().ToString().c_str(), packet.BattlePetGUID.ToString().c_str());
        return;
    }*/

    SendBattlePetDeleted(battlePetDeletePet.PetGuid);
    battlePet->Remove(nullptr);
    _player->_battlePets.erase(battlePetDeletePet.PetGuid);
}

void WorldSession::HandleBattlePetSetFlags(WorldPackets::BattlePet::BattlePetSetFlags& battlePetSetFlags)
{
    if (auto battlePet = _player->GetBattlePet(battlePetSetFlags.PetGuid))
    {
        if (battlePet->Flags & battlePetSetFlags.Flags)
            battlePet->Flags = battlePet->Flags & ~battlePetSetFlags.Flags;
        else
            battlePet->Flags |= battlePetSetFlags.Flags;
        battlePet->needSave = true;
    }
}

void WorldSession::HandleBattlePetClearFanfare(WorldPackets::BattlePet::BattlePetClearFanfare& battlePetClearFanfare)
{
    //GetBattlePetMgr()->ClearFanfare(battlePetClearFanfare.PetGuid);
}

void WorldSession::HandleCageBattlePet(WorldPackets::BattlePet::CageBattlePet& cageBattlePet)
{
    // ReSharper disable once CppUnreachableCode
    auto const& battlePet = _player->GetBattlePet(cageBattlePet.PetGuid);
    if (!battlePet)
        return;

  //  if (sDB2Manager.HasBattlePetSpeciesFlag(battlePet->Species, BATTLE_PET_SPECIES_FLAG_CAGEABLE))
  //      return;

    ItemPosCountVec dest;
    if (_player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, BATTLE_PET_CAGE_ITEM_ID, 1) != EQUIP_ERR_OK)
        return;

    Item* item = _player->StoreNewItem(dest, BATTLE_PET_CAGE_ITEM_ID, true);
    if (!item)
        return;

    item->SetModifier(ITEM_MODIFIER_BATTLE_PET_SPECIES_ID, battlePet->Species);
    item->SetModifier(ITEM_MODIFIER_BATTLE_PET_BREED_DATA, battlePet->Breed | battlePet->Quality << 24);
    item->SetModifier(ITEM_MODIFIER_BATTLE_PET_LEVEL, battlePet->Level);
    item->SetModifier(ITEM_MODIFIER_BATTLE_PET_DISPLAY_ID, battlePet->DisplayModelID);

    _player->SendNewItem(item, 1, true, true); // FIXME: "You create: ." - item name missing in chat

    SendBattlePetDeleted(cageBattlePet.PetGuid);
    battlePet->Remove(nullptr);
    _player->_battlePets.erase(cageBattlePet.PetGuid);
}

void WorldSession::SendBattlePetDeleted(ObjectGuid battlePetGUID)
{
    SendPacket(WorldPackets::BattlePet::BattlePetDeleted(battlePetGUID).Write());
}

void WorldSession::HandleBattlePetDeletePetCheat(WorldPackets::BattlePet::BattlePetDeletePetCheat& battlePetDeletePet)
{
   // GetBattlePetMgr()->RemovePet(battlePetDeletePet.PetGuid);
}

void WorldSession::HandleBattlePetUpdateDisplayNotify(WorldPackets::BattlePet::BattlePetUpdateDisplayNotify& packet)
{
}

void WorldSession::SendPetBattleRequestFailed(uint8 reason)
{
    SendPacket(WorldPackets::BattlePet::RequestFailed(reason).Write());
}

void WorldSession::HandlePetBattleRequestWild(WorldPackets::BattlePet::RequestWild& packet)
{

    if (m_isPetBattleJournalLocked)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NO_ACCOUNT_LOCK);
        return;
    }

    auto battleRequest = sPetBattleSystem->CreateRequest(_player->GetGUID());
    battleRequest->LocationResult = packet.Battle.Location.LocationResult;
    battleRequest->PetBattleCenterPosition = packet.Battle.Location.BattleOrigin;

    for (auto i = 0; i < MAX_PET_BATTLE_TEAM; i++)
        battleRequest->TeamPosition[i] = packet.Battle.Location.PlayerPositions[i];

    battleRequest->RequestType = PET_BATTLE_TYPE_PVE;
    battleRequest->OpponentGuid = packet.Battle.TargetGUID;

    auto canEnterResult = sPetBattleSystem->CanPlayerEnterInPetBattle(_player, battleRequest);
    if (canEnterResult != BATTLE_PET_REQUEST_OK)
    {
        SendPetBattleRequestFailed(canEnterResult);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    auto wildBattlePetCreature = _player->GetNPCIfCanInteractWith(battleRequest->OpponentGuid, UNIT_NPC_FLAG_WILD_BATTLE_PET, UNIT_NPC_FLAG_2_NONE);
    if (!wildBattlePetCreature)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_TARGET_NOT_CAPTURABLE);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    Creature* wildBattlePet = ObjectAccessor::GetCreature(*_player, battleRequest->OpponentGuid);
    if (!wildBattlePet)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_TARGET_NOT_CAPTURABLE);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    if (!sWildBattlePetMgr->IsWildPet(wildBattlePet))
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_TARGET_NOT_CAPTURABLE);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    std::shared_ptr<BattlePetInstance> playerPets[MAX_PET_BATTLE_SLOTS];
    std::shared_ptr<BattlePetInstance> wildBattlePets[MAX_PET_BATTLE_SLOTS];
    size_t playerPetCount = 0;

    for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
    {
        playerPets[i] = nullptr;
        wildBattlePets[i] = nullptr;
    }

    auto l_WildBattlePet = sWildBattlePetMgr->GetWildBattlePet(wildBattlePet);
    if (!l_WildBattlePet)
    {
        l_WildBattlePet = nullptr;

        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_TARGET_NOT_CAPTURABLE);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    wildBattlePets[0] = l_WildBattlePet;

    std::list<Unit*> targets;
    wildBattlePet->GetFriendlyUnitListInRange(targets, 40.f);

    uint32 wildsPetCount = 1;
    for (Unit* current : targets)
    {
        Creature* WildPet = current->ToCreature();
        if (!WildPet)
            continue;

        if (wildsPetCount >= MAX_PET_BATTLE_SLOTS)
            break;

        if (!WildPet->IsAlive() || WildPet->GetGUID() == wildBattlePet->GetGUID() || !sWildBattlePetMgr->IsWildPet(WildPet))
            continue;

        std::shared_ptr<BattlePetInstance> NewPet = sWildBattlePetMgr->GetWildBattlePet(WildPet);
        if (NewPet != nullptr && roll_chance_i(80))
        {
            NewPet->OriginalCreature = WildPet->GetGUID();
            wildBattlePets[wildsPetCount] = NewPet;
            wildsPetCount++;
        }
    }

    auto petSlots = _player->GetBattlePetCombatTeam();

    for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
    {
        if (!petSlots[i])
            continue;

        if (playerPetCount >= MAX_PET_BATTLE_SLOTS || playerPetCount >= _player->GetUnlockedPetBattleSlot())
            break;

        playerPets[playerPetCount] = std::make_shared<BattlePetInstance>();
        playerPets[playerPetCount]->CloneFrom(petSlots[i]);
        playerPets[playerPetCount]->Slot = playerPetCount;
        playerPets[playerPetCount]->OriginalBattlePet = petSlots[i];

        ++playerPetCount;
    }

    _player->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC); ///< Immuned only to NPC
    _player->SetTarget(wildBattlePetCreature->GetGUID());

    SendPetBattleFinalizeLocation(battleRequest);

    _player->SetFacingTo(_player->GetAbsoluteAngle(&battleRequest->TeamPosition[PET_BATTLE_TEAM_2]));
    _player->SetRooted(true);

    _player->SendClearTarget();

    auto battle = sPetBattleSystem->CreateBattle();

    battle->Teams[PET_BATTLE_TEAM_1]->OwnerGuid = _player->GetGUID();
    battle->Teams[PET_BATTLE_TEAM_1]->PlayerGuid = _player->GetGUID();

    battle->Teams[PET_BATTLE_TEAM_2]->OwnerGuid = wildBattlePet->GetGUID();

    for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
    {
        if (playerPets[i])
            battle->AddPet(PET_BATTLE_TEAM_1, playerPets[i]);

        if (wildBattlePets[i])
        {
            battle->AddPet(PET_BATTLE_TEAM_2, wildBattlePets[i]);

            if (Creature* currrentCreature = Hoff::FindMapCreature(_player->GetMap(), wildBattlePets[i]->OriginalCreature))
            {
                currrentCreature->_petBattleId = battle->ID;
                sWildBattlePetMgr->EnterInBattle(currrentCreature);
            }
        }
    }

    battle->BattleType = battleRequest->RequestType;
    battle->PveBattleType = PVE_BATTLE_PET_WILD;

    _player->_petBattleId = battle->ID;
    battle->Begin();

    sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);

    for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
    {
        if (playerPets[i])
            playerPets[i] = nullptr;

        if (wildBattlePets[i])
            wildBattlePets[i] = nullptr;
    }

    l_WildBattlePet = nullptr;
}

void WorldSession::HandlePetBattleRequestUpdate(WorldPackets::BattlePet::RequestUpdate& packet)
{
    auto battleRequest = sPetBattleSystem->GetRequest(packet.TargetGUID);
    auto opposant = ObjectAccessor::FindPlayer(packet.TargetGUID);

    if (!packet.Canceled && battleRequest && opposant)
    {
        _player->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC); // Immuned only to NPC
        opposant->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC); // Immuned only to NPC

        std::shared_ptr<BattlePetInstance> playerPets[MAX_PET_BATTLE_SLOTS];
        std::shared_ptr<BattlePetInstance> playerOpposantPets[MAX_PET_BATTLE_SLOTS];
        size_t playerPetCount = 0;
        size_t playerOpposantPetCount = 0;

        for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
        {
            playerPets[i] = nullptr;
            playerOpposantPets[i] = nullptr;
        }

        auto petSlots = _player->GetBattlePetCombatTeam();

        for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
        {
            if (!petSlots[i])
                continue;

            if (playerPetCount >= MAX_PET_BATTLE_SLOTS || playerPetCount >= _player->GetUnlockedPetBattleSlot())
                break;

            playerPets[playerPetCount] = std::make_shared<BattlePetInstance>();
            playerPets[playerPetCount]->CloneFrom(petSlots[i]);
            playerPets[playerPetCount]->Slot = playerPetCount;
            playerPets[playerPetCount]->OriginalBattlePet = petSlots[i];

            ++playerPetCount;
        }

        auto petOpposantSlots = opposant->GetBattlePetCombatTeam();

        for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
        {
            if (!petOpposantSlots[i])
                continue;

            if (playerOpposantPetCount >= MAX_PET_BATTLE_SLOTS || playerOpposantPetCount >= _player->GetUnlockedPetBattleSlot())
                break;

            playerOpposantPets[playerOpposantPetCount] = std::make_shared<BattlePetInstance>();
            playerOpposantPets[playerOpposantPetCount]->CloneFrom(petOpposantSlots[i]);
            playerOpposantPets[playerOpposantPetCount]->Slot = playerOpposantPetCount;
            playerOpposantPets[playerOpposantPetCount]->OriginalBattlePet = petOpposantSlots[i];

            ++playerOpposantPetCount;
        }

        if (!playerOpposantPetCount || !playerPetCount)
        {
            _player->GetSession()->SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NO_PETS_IN_SLOT);
            opposant->GetSession()->SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NO_PETS_IN_SLOT);
            sPetBattleSystem->RemoveRequest(packet.TargetGUID);
            return;
        }

        _player->GetSession()->SendPetBattleFinalizeLocation(battleRequest);
        opposant->GetSession()->SendPetBattleFinalizeLocation(battleRequest);

        _player->SetFacingTo(_player->GetAbsoluteAngle(&battleRequest->TeamPosition[PET_BATTLE_TEAM_1]));
        opposant->SetFacingTo(_player->GetAbsoluteAngle(&battleRequest->TeamPosition[PET_BATTLE_TEAM_2]));
        _player->SetRooted(true);
        opposant->SetRooted(true);

        auto battle = sPetBattleSystem->CreateBattle();

        battle->Teams[PET_BATTLE_TEAM_1]->OwnerGuid = opposant->GetGUID();
        battle->Teams[PET_BATTLE_TEAM_1]->PlayerGuid = opposant->GetGUID();
        battle->Teams[PET_BATTLE_TEAM_2]->OwnerGuid = _player->GetGUID();
        battle->Teams[PET_BATTLE_TEAM_2]->PlayerGuid = _player->GetGUID();

        for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
        {
            if (playerOpposantPets[i])
                battle->AddPet(PET_BATTLE_TEAM_1, playerOpposantPets[i]);

            if (playerPets[i])
                battle->AddPet(PET_BATTLE_TEAM_2, playerPets[i]);
        }

        battle->BattleType = battleRequest->RequestType;

        // Launch battle
        _player->_petBattleId = battle->ID;
        opposant->_petBattleId = battle->ID;
        battle->Begin();

        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);

        for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
        {
            if (playerPets[i])
                playerPets[i] = std::shared_ptr<BattlePetInstance>();

            if (playerOpposantPets[i])
                playerOpposantPets[i] = std::shared_ptr<BattlePetInstance>();
        }
    }
    else
    {
        if (opposant)
            opposant->GetSession()->SendPetBattleRequestFailed(BATTLE_PET_REQUEST_DECLINED);
        sPetBattleSystem->RemoveRequest(packet.TargetGUID);
    }
}

void WorldSession::HandleBattlePetSummon(WorldPackets::BattlePet::BattlePetSummon& battlePetSummon)
{
    if (_player->IsOnVehicle(_player) || _player->IsSitState())
        return;

    if (!_player->GetSummonedBattlePetGUID().IsEmpty() && _player->GetSummonedBattlePetGUID() == battlePetSummon.PetGuid)
    {
        _player->UnsummonCurrentBattlePetIfAny(false);
        return;
    }
    _player->UnsummonCurrentBattlePetIfAny(false);
    if (!_player->GetSummonedBattlePet())
        _player->SummonBattlePet(battlePetSummon.PetGuid);
}

void WorldSession::HandleBattlePetUpdateNotify(WorldPackets::BattlePet::BattlePetUpdateNotify& battlePetUpdateNotify)
{
   // GetBattlePetMgr()->UpdateBattlePetData(battlePetUpdateNotify.PetGuid);
}

void WorldSession::HandleBattlePetJournalLock(WorldPackets::BattlePet::NullCmsg& /*packet*/)
{
    /*if (m_isPetBattleJournalLocked)
        SendBattlePetJournalLockAcquired();
    else
        SendBattlePetJournalLockDenied();*/
}

void WorldSession::SendPetBattleFinalizeLocation(PetBattleRequest* petBattleRequest)
{
    WorldPackets::BattlePet::FinalizeLocation locationUpdate;
    locationUpdate.Location.BattleOrigin = petBattleRequest->PetBattleCenterPosition;
    locationUpdate.Location.LocationResult = petBattleRequest->LocationResult;
    for (uint8 i = 0; i < MAX_PET_BATTLE_TEAM; i++)
        locationUpdate.Location.PlayerPositions[i] = petBattleRequest->TeamPosition[i];
    SendPacket(locationUpdate.Write());
}

void WorldSession::SendBattlePetUpdates(BattlePet* pet2 /*= nullptr*/, bool add)
{
    BattlePetMap* pets = _player->GetBattlePets();
    WorldPackets::BattlePet::BattlePetUpdates update;
    update.PetAdded = add;

    if (pet2)
    {
        WorldPackets::BattlePet::BattlePet pet2Update;
        pet2Update.Guid = pet2->JournalID;
        pet2Update.Species = pet2->Species;
        if (auto const& speciesInfo = sBattlePetSpeciesStore.LookupEntry(pet2->Species))
            pet2Update.CreatureID = speciesInfo->CreatureID;
        pet2Update.DisplayID = pet2->DisplayModelID;
        pet2Update.Breed = pet2->Breed;
        pet2Update.Level = pet2->Level;
        pet2Update.Exp = pet2->XP;
        pet2Update.Flags = pet2->Flags;
        pet2Update.Power = pet2->InfoPower;
        pet2Update.Health = pet2->Health > pet2->InfoMaxHealth ? pet2->InfoMaxHealth : pet2->Health;
        pet2Update.MaxHealth = pet2->InfoMaxHealth;
        pet2Update.Speed = pet2->InfoSpeed;
        pet2Update.Quality = pet2->Quality;
        pet2Update.Name = pet2->Name;

        update.Pets.emplace_back(pet2Update);
    }
    else
    {
        for (const std::pair<const ObjectGuid, std::shared_ptr<BattlePet>>& pair : *pets)
        {
            const std::shared_ptr<BattlePet> pet = pair.second;

            WorldPackets::BattlePet::BattlePet petUpdate;
            petUpdate.Guid = pet->JournalID;
            petUpdate.Species = pet->Species;
            if (const BattlePetSpeciesEntry* speciesInfo = sBattlePetSpeciesStore.LookupEntry(pet->Species))
                petUpdate.CreatureID = speciesInfo->CreatureID;
            petUpdate.DisplayID = pet->DisplayModelID;
            petUpdate.Breed = pet->Breed;
            petUpdate.Level = pet->Level;
            petUpdate.Exp = pet->XP;
            petUpdate.Flags = pet->Flags;
            petUpdate.Power = pet->InfoPower;
            petUpdate.Health = pet->Health > pet->InfoMaxHealth ? pet->InfoMaxHealth : pet->Health;
            petUpdate.MaxHealth = pet->InfoMaxHealth;
            petUpdate.Speed = pet->InfoSpeed;
            petUpdate.Quality = pet->Quality;
            petUpdate.Name = pet->Name;

            update.Pets.emplace_back(petUpdate);
        }
    }
    SendPacket(update.Write());
}

void WorldSession::SendBattlePetJournal()
{
    BattlePetMap* pets = _player->GetBattlePets();
    uint32 unlockedSlotCount = _player->GetUnlockedPetBattleSlot();
    std::shared_ptr<BattlePet>* petSlots = _player->GetBattlePetCombatTeam();

    if (unlockedSlotCount)
        _player->SetPlayerFlag(PLAYER_FLAGS_PET_BATTLES_UNLOCKED);

    WorldPackets::BattlePet::BattlePetJournal responce;
    //responce.NumMaxPets = BATTLE_PET_MAX_JOURNAL_PETS;
    responce.Trap = _player->GetBattlePetTrapLevel();
    responce.HasJournalLock = true;

    for (uint32 i = 0; i < unlockedSlotCount; i++)
    {
        WorldPackets::BattlePet::BattlePetSlot slot;
        slot.CollarID = 0;
        slot.Index = i;
        slot.Locked = !(i + 1 <= unlockedSlotCount);
        if (petSlots[i])
            slot.Pet.Guid = petSlots[i]->JournalID;
        responce.Slots.emplace_back(slot);
    }

    for (const std::pair<const ObjectGuid, std::shared_ptr<BattlePet>>& pair : *pets)
    {
        const std::shared_ptr<BattlePet> pet = pair.second;

        WorldPackets::BattlePet::BattlePet petUpdate;
        petUpdate.Guid = pet->JournalID;
        petUpdate.Species = pet->Species;
        if (const BattlePetSpeciesEntry* speciesInfo = sBattlePetSpeciesStore.LookupEntry(pet->Species))
            petUpdate.CreatureID = speciesInfo->CreatureID;
        petUpdate.DisplayID = pet->DisplayModelID;
        petUpdate.Breed = pet->Breed;
        petUpdate.Level = pet->Level;
        petUpdate.Exp = pet->XP;
        petUpdate.Flags = pet->Flags;
        petUpdate.Power = pet->InfoPower;
        petUpdate.Health = pet->Health > pet->InfoMaxHealth ? pet->InfoMaxHealth : pet->Health;
        petUpdate.MaxHealth = pet->InfoMaxHealth;
        petUpdate.Speed = pet->InfoSpeed;
        petUpdate.Quality = pet->Quality;
        petUpdate.Name = pet->Name;

        responce.Pets.emplace_back(petUpdate);
    }

    SendPacket(responce.Write());
}

void WorldSession::SendPetBattleReplacementMade(PetBattle* petBattle)
{
    auto isPVP = petBattle->BattleType != PET_BATTLE_TYPE_PVE;
    uint16 pvpMaxRoundTime = isPVP ? 30 : 0;

    WorldPackets::BattlePet::BattleRound replacementMade(SMSG_PET_BATTLE_REPLACEMENTS_MADE);
    replacementMade.MsgData.CurRound = petBattle->Turn;
    replacementMade.MsgData.NextPetBattleState = petBattle->RoundResult;

    for (uint8 i = 0; i < MAX_PET_BATTLE_TEAM; i++)
    {
        replacementMade.MsgData.NextInputFlags[i] = petBattle->Teams[i]->GetTeamInputFlags();
        replacementMade.MsgData.NextTrapStatus[i] = petBattle->Teams[i]->GetTeamTrapStatus();
        replacementMade.MsgData.RoundTimeSecs[i] = pvpMaxRoundTime;
    }

    replacementMade.MsgData.PetXDied = petBattle->PetXDied;

    for (auto const& pet : petBattle->Pets)
    {
        if (!pet)
            continue;

        for (uint8 s = 0; s < MAX_PET_BATTLE_ABILITIES; s++)
        {
            if (pet->Cooldowns[s] != -1 || pet->Lockdowns[s] != 0)
            {
                WorldPackets::BattlePet::BattlePetAbility abilityUpdate;
                abilityUpdate.AbilityID = pet->Abilities[s];
                abilityUpdate.CooldownRemaining = pet->Cooldowns[s];
                abilityUpdate.LockdownRemaining = pet->Lockdowns[s];
                abilityUpdate.AbilityIndex = s;
                abilityUpdate.Pboid = pet->ID;
                replacementMade.MsgData.Ability.emplace_back(abilityUpdate);
            }
        }
    }

    for (auto const& roundEvent : petBattle->RoundEvents)
    {
        WorldPackets::BattlePet::Effect effectUpdate;
        effectUpdate.AbilityEffectID = roundEvent.AbilityEffectID;
        effectUpdate.Flags = roundEvent.Flags;
        effectUpdate.SourceAuraInstanceID = roundEvent.BuffTurn; ///< Can be swap down
        effectUpdate.TurnInstanceID = roundEvent.RoundTurn; ///< Can be swap up
        effectUpdate.EffectType = roundEvent.EventType;
        effectUpdate.CasterPBOID = roundEvent.SourcePetID;
        effectUpdate.StackDepth = roundEvent.StackDepth;

        for (auto const& update : roundEvent.Updates)
        {
            WorldPackets::BattlePet::PetBattleEffectTarget effectTargetUpdate;
            effectTargetUpdate.Type = update.UpdateType;
            effectTargetUpdate.Petx = update.TargetPetID;

            effectTargetUpdate.Params.Aura.AuraInstanceID = update.Buff.ID;
            effectTargetUpdate.Params.Aura.AuraAbilityID = update.Buff.AbilityID;
            effectTargetUpdate.Params.Aura.RoundsRemaining = update.Buff.Duration;
            effectTargetUpdate.Params.Aura.CurrentRound = update.Buff.Turn;
            effectTargetUpdate.Params.State.StateID = update.State.ID;
            effectTargetUpdate.Params.State.StateValue = update.State.Value;
            effectTargetUpdate.Params.Health = update.Health;
            effectTargetUpdate.Params.NewStatValue = update.Speed;
            effectTargetUpdate.Params.TriggerAbilityID = update.TriggerAbilityId;
            effectTargetUpdate.Params.AbilityChange.ChangedAbilityID = 0;
            effectTargetUpdate.Params.AbilityChange.CooldownRemaining = 0;
            effectTargetUpdate.Params.AbilityChange.LockdownRemaining = 0;
            effectTargetUpdate.Params.BroadcastTextID = update.NpcEmote.BroadcastTextID;

            effectUpdate.EffectTargetData.emplace_back(effectTargetUpdate);
        }

        replacementMade.MsgData.EffectData.emplace_back(effectUpdate);
    }

    SendPacket(replacementMade.Write());
}

void WorldSession::SendPetBattleInitialUpdate(PetBattle* petBattle)
{
    if (petBattle->BattleType == PET_BATTLE_TYPE_PVE)
        petBattle->InitialWildPetGUID = petBattle->Teams[PET_BATTLE_PVE_TEAM_ID]->OwnerGuid;

    WorldPackets::BattlePet::PetBattleInitialUpdate update;
    uint16 waitingForFrontPetsMaxSecs = 30;
    uint16 pvpMaxRoundTime = 30;
    uint8 curPetBattleState = 1;
    bool isPVP = petBattle->BattleType != PET_BATTLE_TYPE_PVE;

    if (petBattle->BattleType == PET_BATTLE_TYPE_PVE && petBattle->PveBattleType == PVE_BATTLE_PET_TRAINER)
    {
        if (Player* player = ObjectAccessor::FindPlayer(petBattle->Teams[PET_BATTLE_TEAM_1]->OwnerGuid))
        {
            if (Creature* trainer = Hoff::FindMapCreature(player->GetMap(), petBattle->InitialWildPetGUID))
            {
                update.MsgData.NpcCreatureID = 247; // some random npc i found, not a quest giver or gossip
                update.MsgData.NpcDisplayID = trainer->GetDisplayId();
            }
        }
    }

    //PetBattleEnviroUpdate Enviros[3] = {};
    update.MsgData.InitialWildPetGUID = petBattle->InitialWildPetGUID;
    update.MsgData.CurRound = petBattle->Turn;
    update.MsgData.WaitingForFrontPetsMaxSecs = waitingForFrontPetsMaxSecs;
    update.MsgData.PvpMaxRoundTime = pvpMaxRoundTime;
    update.MsgData.ForfeitPenalty = petBattle->GetForfeitHealthPenalityPct();
    update.MsgData.CurPetBattleState = curPetBattleState;
    update.MsgData.IsPVP = isPVP;
    update.MsgData.CanAwardXP = petBattle->BattleType != PET_BATTLE_TYPE_PVP_DUEL;

    for (uint8 i = 0; i < MAX_PET_BATTLE_TEAM; i++)
    {
        auto ownerGuid = petBattle->Teams[i]->OwnerGuid;
        if (petBattle->BattleType == PET_BATTLE_TYPE_PVE && i == PET_BATTLE_PVE_TEAM_ID)
            ownerGuid.Clear();

        WorldPackets::BattlePet::PetBattlePlayerUpdate playerUpdate;
        playerUpdate.CharacterID = ownerGuid;
        playerUpdate.TrapAbilityID = petBattle->Teams[i]->GetCatchAbilityID();
        playerUpdate.TrapStatus = i == PET_BATTLE_TEAM_1 ? 5 : 2;
        playerUpdate.RoundTimeSecs = isPVP ? pvpMaxRoundTime : 0;
        playerUpdate.InputFlags = PETBATTLE_TEAM_INPUT_FLAG_LOCK_PET_SWAP | PETBATTLE_TEAM_INPUT_FLAG_LOCK_ABILITIES_2;

        if (i == PET_BATTLE_TEAM_1 || petBattle->Teams[i]->ActivePetID == PET_BATTLE_NULL_ID)
            playerUpdate.FrontPet = int8(petBattle->Teams[i]->ActivePetID);
        else
            playerUpdate.FrontPet = int8(petBattle->Teams[i]->ActivePetID - (i == PET_BATTLE_TEAM_2 ? MAX_PET_BATTLE_SLOTS : 0));

        for (uint8 v = 0; v < petBattle->Teams[i]->TeamPetCount; v++)
        {
            auto pet = petBattle->Teams[i]->TeamPets[v];

            WorldPackets::BattlePet::PetBattlePetUpdate petUpdate;
            petUpdate.NpcTeamMemberID = 0;
             if (i == PET_BATTLE_TEAM_1 && !isPVP)
                 if (CreatureTemplate const* npc = sObjectMgr->GetCreatureTemplate(petBattle->InitialWildPetGUID.GetEntry()))
                     petUpdate.NpcTeamMemberID = npc->Entry;

            petUpdate.StatusFlags = 0;
            petUpdate.Slot = v;
            //std::vector<BattlePetAura> Auras;

            petUpdate.JournalInfo.Guid = petBattle->BattleType == PET_BATTLE_TYPE_PVE && i == PET_BATTLE_PVE_TEAM_ID ? ObjectGuid::Empty : pet->JournalID;
            petUpdate.JournalInfo.Species = pet->Species;
            if (auto const& speciesInfo = sBattlePetSpeciesStore.LookupEntry(pet->Species))
                petUpdate.JournalInfo.CreatureID = speciesInfo->CreatureID;
            petUpdate.JournalInfo.DisplayID = pet->DisplayModelID;
            petUpdate.JournalInfo.Breed = pet->Breed;
            petUpdate.JournalInfo.Level = pet->Level;
            petUpdate.JournalInfo.Exp = pet->XP;
            petUpdate.JournalInfo.Flags = pet->Flags & ~BATTLE_PET_FLAG_CAPTURED;
            petUpdate.JournalInfo.Power = pet->InfoPower;

            petUpdate.JournalInfo.Health = pet->Health;
            petUpdate.JournalInfo.MaxHealth = pet->InfoMaxHealth;
            petUpdate.JournalInfo.Speed = pet->InfoSpeed;
            petUpdate.JournalInfo.Quality = pet->Quality;
            petUpdate.JournalInfo.Name = pet->Name;

            for (uint8 slot = 0; slot < MAX_PET_BATTLE_ABILITIES; slot++)
            {
                if (pet->Abilities[slot])
                {
                    WorldPackets::BattlePet::BattlePetAbility abilityUpdate;
                    abilityUpdate.AbilityID = pet->Abilities[slot];
                    abilityUpdate.CooldownRemaining = pet->Cooldowns[slot] != -1 ? pet->Cooldowns[slot] : 0; ///< Sending cooldown at -1 make client disable it
                    abilityUpdate.LockdownRemaining = pet->Lockdowns[slot];
                    abilityUpdate.AbilityIndex = slot;
                    abilityUpdate.Pboid = pet->ID;
                    petUpdate.Abilities.emplace_back(abilityUpdate);
                }
            }

            for (uint32 stateIdx = 0; stateIdx < NUM_BATTLE_PET_STATES; ++stateIdx)
            {
                switch (stateIdx)
                {
                case BATTLE_PET_STATE_Stat_Power:
                case BATTLE_PET_STATE_Stat_Stamina:
                case BATTLE_PET_STATE_Stat_Speed:
                case BATTLE_PET_STATE_Stat_CritChance:
                case BATTLE_PET_STATE_Stat_Accuracy:
                    petUpdate.States[stateIdx] = pet->States[stateIdx];
                    break;
                default:
                    break;
                }
            }

            playerUpdate.Pets.emplace_back(petUpdate);
        }


        update.MsgData.Players[i] = playerUpdate;
    }

    SendPacket(update.Write());
}

void WorldSession::SendPetBattleFirstRound(PetBattle* petBattle)
{
    // TC_LOG_DEBUG(LOG_FILTER_BATTLEPET, "SendPetBattleFirstRound");

    auto isPVP = petBattle->BattleType != PET_BATTLE_TYPE_PVE;
    uint16 pvpMaxRoundTime = isPVP ? 30 : 0;

    WorldPackets::BattlePet::BattleRound firstRound(SMSG_PET_BATTLE_FIRST_ROUND);
    firstRound.MsgData.CurRound = petBattle->Turn;
    firstRound.MsgData.NextPetBattleState = petBattle->RoundResult;

    for (uint8 i = 0; i < MAX_PET_BATTLE_TEAM; i++)
    {
        firstRound.MsgData.NextInputFlags[i] = petBattle->Teams[i]->GetTeamInputFlags();
        firstRound.MsgData.NextTrapStatus[i] = petBattle->Teams[i]->GetTeamTrapStatus();
        firstRound.MsgData.RoundTimeSecs[i] = pvpMaxRoundTime;
    }

    firstRound.MsgData.PetXDied = petBattle->PetXDied;

    for (auto const& pet : petBattle->Pets)
    {
        if (!pet)
            continue;

        for (uint8 s = 0; s < MAX_PET_BATTLE_ABILITIES; s++)
        {
            if (pet->Cooldowns[s] != -1 || pet->Lockdowns[s] != 0)
            {
                WorldPackets::BattlePet::BattlePetAbility abilityUpdate;
                abilityUpdate.AbilityID = pet->Abilities[s];
                abilityUpdate.CooldownRemaining = pet->Cooldowns[s];
                abilityUpdate.LockdownRemaining = pet->Lockdowns[s];
                abilityUpdate.AbilityIndex = s;
                abilityUpdate.Pboid = pet->ID;
                firstRound.MsgData.Ability.emplace_back(abilityUpdate);
            }
        }
    }

    for (auto const& eventIntr : petBattle->RoundEvents)
    {
        WorldPackets::BattlePet::Effect effectUpdate;
        effectUpdate.AbilityEffectID = eventIntr.AbilityEffectID;
        effectUpdate.Flags = eventIntr.Flags;
        effectUpdate.SourceAuraInstanceID = eventIntr.BuffTurn; ///< Can be swap down
        effectUpdate.TurnInstanceID = eventIntr.RoundTurn; ///< Can be swap up
        effectUpdate.EffectType = eventIntr.EventType;
        effectUpdate.CasterPBOID = eventIntr.SourcePetID;
        effectUpdate.StackDepth = eventIntr.StackDepth;

        for (auto const& update : eventIntr.Updates)
        {
            WorldPackets::BattlePet::PetBattleEffectTarget effectTargetUpdate;
            effectTargetUpdate.Type = update.UpdateType;
            effectTargetUpdate.Petx = update.TargetPetID;

            effectTargetUpdate.Params.Aura.AuraInstanceID = update.Buff.ID;
            effectTargetUpdate.Params.Aura.AuraAbilityID = update.Buff.AbilityID;
            effectTargetUpdate.Params.Aura.RoundsRemaining = update.Buff.Duration;
            effectTargetUpdate.Params.Aura.CurrentRound = update.Buff.Turn;
            effectTargetUpdate.Params.State.StateID = update.State.ID;
            effectTargetUpdate.Params.State.StateValue = update.State.Value;
            effectTargetUpdate.Params.Health = update.Health;
            effectTargetUpdate.Params.NewStatValue = update.Speed;
            effectTargetUpdate.Params.TriggerAbilityID = update.TriggerAbilityId;
            effectTargetUpdate.Params.AbilityChange.ChangedAbilityID = 0;
            effectTargetUpdate.Params.AbilityChange.CooldownRemaining = 0;
            effectTargetUpdate.Params.AbilityChange.LockdownRemaining = 0;
            effectTargetUpdate.Params.BroadcastTextID = update.NpcEmote.BroadcastTextID;

            effectUpdate.EffectTargetData.emplace_back(effectTargetUpdate);
        }

        firstRound.MsgData.EffectData.emplace_back(effectUpdate);
    }

    SendPacket(firstRound.Write());
}

void WorldSession::SendPetBattleRoundResult(PetBattle* petBattle)
{
    // TC_LOG_DEBUG(LOG_FILTER_BATTLEPET, "SendPetBattleRoundResult");

    auto isPVP = petBattle->BattleType != PET_BATTLE_TYPE_PVE;
    uint16 pvpMaxRoundTime = isPVP ? 30 : 0;

    WorldPackets::BattlePet::BattleRound roundResult(SMSG_PET_BATTLE_ROUND_RESULT);
    roundResult.MsgData.CurRound = petBattle->Turn;
    roundResult.MsgData.NextPetBattleState = petBattle->RoundResult;

    for (uint8 i = 0; i < MAX_PET_BATTLE_TEAM; i++)
    {
        roundResult.MsgData.NextInputFlags[i] = petBattle->Teams[i]->GetTeamInputFlags();
        roundResult.MsgData.NextTrapStatus[i] = petBattle->Teams[i]->GetTeamTrapStatus();
        roundResult.MsgData.RoundTimeSecs[i] = pvpMaxRoundTime;
    }

    roundResult.MsgData.PetXDied = petBattle->PetXDied;

    for (auto const& pet : petBattle->Pets)
    {
        if (!pet)
            continue;

        for (uint8 s = 0; s < MAX_PET_BATTLE_ABILITIES; s++)
        {
            if (pet->Cooldowns[s] != -1 || pet->Lockdowns[s] != 0)
            {
                WorldPackets::BattlePet::BattlePetAbility abilityUpdate;
                abilityUpdate.AbilityID = pet->Abilities[s];
                abilityUpdate.CooldownRemaining = pet->Cooldowns[s];
                abilityUpdate.LockdownRemaining = pet->Lockdowns[s];
                abilityUpdate.AbilityIndex = s;
                abilityUpdate.Pboid = pet->ID;
                roundResult.MsgData.Ability.emplace_back(abilityUpdate);
            }
        }
    }

    for (auto const& roundEvent : petBattle->RoundEvents)
    {
        WorldPackets::BattlePet::Effect effectUpdate;

        bool isDead = false;
        for (const auto& update : roundEvent.Updates)
        {
            isDead = update.State.ID == BATTLE_PET_STATE_Is_Dead;
            WorldPackets::BattlePet::PetBattleEffectTarget effectTargetUpdate;
            effectTargetUpdate.Type = update.UpdateType;
            effectTargetUpdate.Petx = update.TargetPetID;
            effectTargetUpdate.Params.Aura.AuraInstanceID = update.Buff.ID;
            effectTargetUpdate.Params.Aura.AuraAbilityID = isDead ? 0 : update.Buff.AbilityID;
            effectTargetUpdate.Params.Aura.RoundsRemaining = update.Buff.Duration;
            effectTargetUpdate.Params.Aura.CurrentRound = update.Buff.Turn;
            effectTargetUpdate.Params.State.StateID = update.State.ID;
            effectTargetUpdate.Params.State.StateValue = update.State.Value;
            effectTargetUpdate.Params.Health = update.Health;
            effectTargetUpdate.Params.NewStatValue = update.Speed;
            effectTargetUpdate.Params.TriggerAbilityID = update.TriggerAbilityId;
            effectTargetUpdate.Params.AbilityChange.ChangedAbilityID = 0;
            effectTargetUpdate.Params.AbilityChange.CooldownRemaining = 0;
            effectTargetUpdate.Params.AbilityChange.LockdownRemaining = 0;
            effectTargetUpdate.Params.BroadcastTextID = update.NpcEmote.BroadcastTextID;
            effectUpdate.EffectTargetData.emplace_back(effectTargetUpdate);
        }

        effectUpdate.AbilityEffectID = roundEvent.AbilityEffectID;
        effectUpdate.Flags = isDead ? 0 : roundEvent.Flags;
        effectUpdate.SourceAuraInstanceID = roundEvent.BuffTurn; ///< Can be swap down
        effectUpdate.TurnInstanceID = isDead ? 0 : roundEvent.RoundTurn; ///< Can be swap up
        effectUpdate.EffectType = roundEvent.EventType;
        effectUpdate.CasterPBOID = roundEvent.SourcePetID;
        effectUpdate.StackDepth = roundEvent.StackDepth;

        roundResult.MsgData.EffectData.emplace_back(effectUpdate);
    }

    SendPacket(roundResult.Write());
}

void WorldSession::SendPetBattleFinalRound(PetBattle* petBattle)
{
    // TC_LOG_DEBUG(LOG_FILTER_BATTLEPET, "SendPetBattleFinalRound");

    WorldPackets::BattlePet::PetBattleFinalRound roundUpdate;
    roundUpdate.MsgData.Abandoned = petBattle->CombatResult == PET_BATTLE_RESULT_ABANDON;
    roundUpdate.MsgData.PvpBattle = petBattle->BattleType != PET_BATTLE_TYPE_PVE;
    for (uint8 teamID = 0; teamID < MAX_PET_BATTLE_TEAM; ++teamID)
    {
        roundUpdate.MsgData.Winner[teamID] = petBattle->WinnerTeamId == teamID;
        roundUpdate.MsgData.NpcCreatureID[teamID] = 0;
    }

    for (auto const& pet : petBattle->Pets)
    {
        if (!pet)
            continue;

        WorldPackets::BattlePet::FinalPet petUpdate;
        petUpdate.Guid = pet->JournalID;
        petUpdate.Level = pet->Level;
        petUpdate.Xp = pet->XP;
        petUpdate.Health = pet->Health;
        petUpdate.MaxHealth = pet->InfoMaxHealth;
        petUpdate.InitialLevel = pet->OldLevel;
        petUpdate.Pboid = pet->ID;
        petUpdate.Captured = pet->Captured;
        petUpdate.Caged = pet->Caged;
        petUpdate.AwardedXP = pet->OldXP != pet->XP;
        petUpdate.SeenAction = false /*bool(petUpdate.Guid)*/;
        roundUpdate.MsgData.Pets.emplace_back(petUpdate);
    }

    SendPacket(roundUpdate.Write());
}

void WorldSession::HandleJoinPetBattleQueue(WorldPackets::BattlePet::NullCmsg& /*packet*/)
{
    //@TODO
    /*if (_player->_petBattleId)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_IN_BATTLE);
        return;
    }*/

    if (_player->IsInCombat())
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NOT_WHILE_IN_COMBAT);
        return;
    }

    std::shared_ptr<BattlePetInstance> playerPets[MAX_PET_BATTLE_SLOTS];
    size_t playerPetCount = 0;

    // Temporary pet buffer
    for (auto& playerPet : playerPets)
        playerPet = std::shared_ptr<BattlePetInstance>();

    // Load player pets
    auto petSlots = _player->GetBattlePetCombatTeam();
    uint32 deadPetCount = 0;

    for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
    {
        if (!petSlots[i])
            continue;

        if (playerPetCount >= MAX_PET_BATTLE_SLOTS || playerPetCount >= _player->GetUnlockedPetBattleSlot())
            break;

        if (petSlots[i]->Health == 0)
            deadPetCount++;

        playerPets[playerPetCount] = std::make_shared<BattlePetInstance>();
        playerPets[playerPetCount]->CloneFrom(petSlots[i]);
        playerPets[playerPetCount]->Slot = playerPetCount;
        playerPets[playerPetCount]->OriginalBattlePet = petSlots[i];

        ++playerPetCount;
    }

    if (deadPetCount && deadPetCount == playerPetCount)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_ALL_PETS_DEAD);
        return;
    }

    if (!playerPetCount)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NO_PETS_IN_SLOT);
        return;
    }

    sPetBattleSystem->JoinQueue(_player);
}

void WorldSession::HandleBattlePetLeaveQueue(WorldPackets::BattlePet::LeaveQueue& /*packet*/)
{
    sPetBattleSystem->LeaveQueue(_player);
}

void WorldSession::SendPetBattleQueueStatus(uint32 ticketTime, uint32 tcketID, uint32 status, uint32 avgWaitTime)
{
    WorldPackets::BattlePet::PetBattleQueueStatus statusUpdate;
    statusUpdate.Msg.Ticket.RequesterGuid = GetBattlenetAccountGUID();
    statusUpdate.Msg.Ticket.Id = tcketID;
    statusUpdate.Msg.Ticket.Type = WorldPackets::LFG::RideType::PvPPetBattle;
    statusUpdate.Msg.Ticket.Time = ticketTime;
    statusUpdate.Msg.Status = status;

    if (status != LFB_LEAVE_QUEUE)
    {
        statusUpdate.Msg.ClientWaitTime = avgWaitTime;
        statusUpdate.Msg.AverageWaitTime = time(nullptr) - ticketTime;
    }

    //statusUpdate.Msg.SlotResult; std::vector<uint32> was commented

    SendPacket(statusUpdate.Write());
}

void WorldSession::SendBattlePetTrapLevel()
{
    SendPacket(WorldPackets::BattlePet::BattlePetTrapLevel(_player->GetBattlePetTrapLevel()).Write());
}

void WorldSession::SendBattlePetJournalLockAcquired()
{
    m_isPetBattleJournalLocked = true;
    SendPacket(WorldPackets::BattlePet::NullSMsg(SMSG_BATTLE_PET_JOURNAL_LOCK_ACQUIRED).Write());
}

void WorldSession::SendBattlePetJournalLockDenied()
{
    m_isPetBattleJournalLocked = false;
    SendPacket(WorldPackets::BattlePet::NullSMsg(SMSG_BATTLE_PET_JOURNAL_LOCK_DENIED).Write());
}

void WorldSession::SendPetBattleQueueProposeMatch()
{
    SendPacket(WorldPackets::BattlePet::NullSMsg(SMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH).Write());
}

void WorldSession::HandlePetBattleFinalNotify(WorldPackets::BattlePet::NullCmsg& /*packet*/)
{ }

void WorldSession::SendPetBattleFinished()
{
    SendPacket(WorldPackets::BattlePet::NullSMsg(SMSG_PET_BATTLE_FINISHED).Write());
}

enum ePetBattleActions
{
    PETBATTLE_ACTION_REQUEST_LEAVE = 0,
    PETBATTLE_ACTION_CAST = 1,
    PETBATTLE_ACTION_SWAP_OR_PASS = 2,
    PETBATTLE_ACTION_CATCH = 3,
    PETBATTLE_ACTION_LEAVE_PETBATTLE = 4
};

void WorldSession::HandlePetBattleInput(WorldPackets::BattlePet::PetBattleInput& packet)
{
    if (packet.MoveType == PETBATTLE_ACTION_LEAVE_PETBATTLE)
    {
        SendPetBattleFinished();
        return;
    }

    if (!_player->_petBattleId)
    {
        SendPetBattleFinished();
        return;
    }

    auto petBattle = sPetBattleSystem->GetBattle(_player->_petBattleId);
    if (!petBattle || petBattle->BattleStatus == PET_BATTLE_STATUS_FINISHED)
    {
        SendPetBattleFinished();
        return;
    }

    if (packet.Round + 1 != petBattle->Turn)
    {
        sPetBattleSystem->ForfeitBattle(petBattle->ID, _player->GetGUID(), packet.IgnoreAbandonPenalty);
        return;
    }

    if (!packet.MoveType)
        return;

    uint32 playerTeamID = 0;
    if (petBattle->Teams[PET_BATTLE_TEAM_2]->PlayerGuid == _player->GetGUID())
        playerTeamID = PET_BATTLE_TEAM_2;

    auto& battleTeam = petBattle->Teams[playerTeamID];

    if (petBattle->BattleType == PET_BATTLE_TYPE_PVE)
    {
        petBattle->Teams[PET_BATTLE_TEAM_1]->isRun = true;
        petBattle->Teams[PET_BATTLE_TEAM_2]->isRun = true;
    }
    else
        battleTeam->isRun = true;

    if (battleTeam->Ready)
        return;

    switch (packet.MoveType)
    {
    case PETBATTLE_ACTION_REQUEST_LEAVE:
        sPetBattleSystem->ForfeitBattle(petBattle->ID, _player->GetGUID(), packet.IgnoreAbandonPenalty);
        break;
    case PETBATTLE_ACTION_CAST:
        if (petBattle->CanCast(playerTeamID, packet.AbilityID))
        {
            petBattle->PrepareCast(playerTeamID, packet.AbilityID);
            break;
        }
    case PETBATTLE_ACTION_CATCH:
        if (battleTeam->CanCatchOpponentTeamFrontPet() == PETBATTLE_TEAM_CATCH_FLAG_ENABLE_TRAP)
            petBattle->PrepareCast(playerTeamID, battleTeam->GetCatchAbilityID());
        break;
    case PETBATTLE_ACTION_SWAP_OR_PASS:
    {
        packet.NewFrontPet = (playerTeamID == PET_BATTLE_TEAM_2 ? MAX_PET_BATTLE_SLOTS : 0) + packet.NewFrontPet;

        if (!battleTeam->CanSwap(packet.NewFrontPet))
            return;

        petBattle->SwapPet(playerTeamID, packet.NewFrontPet);
        break;
    }
    default:
        break;
    }
}

void WorldSession::HanldeQueueProposeMatchResult(WorldPackets::BattlePet::QueueProposeMatchResult& packet)
{
    sPetBattleSystem->ProposalResponse(_player, packet.Accepted);
}

void WorldSession::HandlePetBattleQuitNotify(WorldPackets::BattlePet::NullCmsg& /*packet*/)
{
    auto petBattle = sPetBattleSystem->GetBattle(_player->_petBattleId);
    if (petBattle)
    {
        petBattle->BattleStatus = PET_BATTLE_STATUS_FINISHED;
        sPetBattleSystem->ForfeitBattle(petBattle->ID, _player->GetGUID(), true);
        SendPetBattleFinished();
    }
}

void WorldSession::HandleReplaceFrontPet(WorldPackets::BattlePet::ReplaceFrontPet& packet)
{
    if (!_player->_petBattleId)
    {
        SendPetBattleFinished();
        return;
    }

    PetBattle* petBattle = sPetBattleSystem->GetBattle(_player->_petBattleId);
    if (!petBattle || petBattle->BattleStatus == PET_BATTLE_STATUS_FINISHED)
    {
        SendPetBattleFinished();
        return;
    }

    uint32 playerTeamID = 0;
    if (petBattle->Teams[PET_BATTLE_TEAM_2]->PlayerGuid == _player->GetGUID())
        playerTeamID = PET_BATTLE_TEAM_2;

    if (petBattle->Teams[playerTeamID]->Ready)
        return;

    packet.FrontPet = (playerTeamID == PET_BATTLE_TEAM_2 ? MAX_PET_BATTLE_SLOTS : 0) + packet.FrontPet;

    if (!petBattle->Teams[playerTeamID]->CanSwap(packet.FrontPet))
        return;

    petBattle->SwapPet(playerTeamID, packet.FrontPet);
    petBattle->SwapPet(!playerTeamID, petBattle->Teams[!playerTeamID]->ActivePetID);
}

void WorldSession::HandlePetBattleRequestPVP(WorldPackets::BattlePet::RequestPVP& packet)
{
    auto battleRequest = sPetBattleSystem->CreateRequest(_player->GetGUID());

    battleRequest->LocationResult = packet.Battle.Location.LocationResult;
    battleRequest->PetBattleCenterPosition = packet.Battle.Location.BattleOrigin;

    for (auto i = 0; i < MAX_PET_BATTLE_TEAM; i++)
        battleRequest->TeamPosition[i] = packet.Battle.Location.PlayerPositions[i];

    battleRequest->RequestType = PET_BATTLE_TYPE_PVP_DUEL;
    battleRequest->OpponentGuid = packet.Battle.TargetGUID;

    //@TODO
    /*if (_player->_petBattleId)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_IN_BATTLE);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }*/

    if (_player->IsInCombat())
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NOT_WHILE_IN_COMBAT);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    /*for (auto const& teamPosition : battleRequest->TeamPosition)
    {
        if (_player->GetMap()->getObjectHitPos(_player->GetPhaseShift(), battleRequest->PetBattleCenterPosition, teamPosition, 0.0f))
        {
            SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NOT_HERE_UNEVEN_GROUND);
            sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
            return;
        }
    }
    @TODO get position team compatible with getObjectHitPos() function, or create new function*/

    auto opposant = ObjectAccessor::FindPlayer(packet.Battle.TargetGUID);
    if (!opposant)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_TARGET_INVALID);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    /*if (opposant->_petBattleId)
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_IN_BATTLE);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }*/

    if (opposant->IsInCombat())
    {
        SendPetBattleRequestFailed(BATTLE_PET_REQUEST_NOT_WHILE_IN_COMBAT);
        sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
        return;
    }

    battleRequest->IsPvPReady[PET_BATTLE_TEAM_1] = true;
    opposant->GetSession()->SendPetBattlePvPChallenge(battleRequest);
}

void WorldSession::SendPetBattlePvPChallenge(PetBattleRequest* petBattleRequest)
{
    WorldPackets::BattlePet::PVPChallenge challengeUpdate;
    challengeUpdate.ChallengerGUID = petBattleRequest->RequesterGuid;
    challengeUpdate.Location.BattleOrigin = petBattleRequest->PetBattleCenterPosition;
    challengeUpdate.Location.LocationResult = petBattleRequest->LocationResult;
    for (uint8 i = 0; i < MAX_PET_BATTLE_TEAM; i++)
        challengeUpdate.Location.PlayerPositions[i] = petBattleRequest->TeamPosition[i];

    SendPacket(challengeUpdate.Write());
}

void WorldSession::HandlePetBattleScriptErrorNotify(WorldPackets::BattlePet::NullCmsg& /*packet*/)
{ }

void WorldSession::SendBattlePetsHealed()
{
    SendPacket(WorldPackets::BattlePet::NullSMsg(SMSG_BATTLE_PETS_HEALED).Write());
}


void WorldSession::SendPetBattleSlotUpdates(bool newSlotUnlocked /*= false*/)
{
    // TC_LOG_DEBUG(LOG_FILTER_BATTLEPET, "SendPetBattleSlotUpdates");

    auto unlockedSlotCount = _player->GetUnlockedPetBattleSlot();
    auto petSlots = _player->GetBattlePetCombatTeam();

    if (unlockedSlotCount)
        _player->SetPlayerFlag(PLAYER_FLAGS_PET_BATTLES_UNLOCKED);

    WorldPackets::BattlePet::PetBattleSlotUpdates updates;
    updates.AutoSlotted = true;
    updates.NewSlot = newSlotUnlocked;

    for (uint32 i = 0; i < MAX_PET_BATTLE_SLOTS; i++)
    {
        WorldPackets::BattlePet::BattlePetSlot slot;
        if (petSlots[i])
            slot.Pet.Guid = petSlots[i]->JournalID;
        slot.CollarID = 0;
        slot.Index = i;
        slot.Locked = !(i + 1 <= unlockedSlotCount);
        updates.Slots.emplace_back(slot);
    }

    SendPacket(updates.Write());
}
