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

#include "BattlePetPackets.h"

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePetSlot const& slot)
{
    data << (slot.Pet.Guid.IsEmpty() ? ObjectGuid::Create<HighGuid::BattlePet>(0) : slot.Pet.Guid);
    data << uint32(slot.CollarID);
    data << uint8(slot.Index);
    data.WriteBit(slot.Locked);
    data.FlushBits();

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePet const& pet)
{
    data << pet.Guid;
    data << uint32(pet.Species);
    data << uint32(pet.CreatureID);
    data << uint32(pet.DisplayID);
    data << uint16(pet.Breed);
    data << uint16(pet.Level);
    data << uint16(pet.Exp);
    data << uint16(pet.Flags);
    data << uint32(pet.Power);
    data << uint32(pet.Health);
    data << uint32(pet.MaxHealth);
    data << uint32(pet.Speed);
    data << uint8(pet.Quality);
    data.WriteBits(pet.Name.size(), 7);
    data.WriteBit(pet.OwnerInfo.has_value());
    data.WriteBit(false); // NoRename
    data.FlushBits();

    data.WriteString(pet.Name);

    if (pet.OwnerInfo)
    {
        data << pet.OwnerInfo->Guid;
        data << uint32(pet.OwnerInfo->PlayerVirtualRealm);
        data << uint32(pet.OwnerInfo->PlayerNativeRealm);
    }

    return data;
}

WorldPacket const* WorldPackets::BattlePet::BattlePetJournal::Write()
{
    _worldPacket << Trap;
    _worldPacket << uint32(Slots.size());
    _worldPacket << uint32(Pets.size());
    //_worldPacket << NumMaxPets;
    _worldPacket.WriteBit(HasJournalLock);
    _worldPacket.FlushBits();

    for (BattlePetSlot const& slots : Slots)
        _worldPacket << slots;

    for (BattlePet const& pet : Pets)
        _worldPacket << pet;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::BattlePetUpdates::Write()
{
    _worldPacket << uint32(Pets.size());
    _worldPacket.WriteBit(PetAdded);
    _worldPacket.FlushBits();

    for (BattlePet const& pet : Pets)
        _worldPacket << pet;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::PetBattleSlotUpdates::Write()
{
    _worldPacket << uint32(Slots.size());
    _worldPacket.WriteBit(NewSlot);
    _worldPacket.WriteBit(AutoSlotted);
    _worldPacket.FlushBits();

    for (auto const& slot : Slots)
        _worldPacket << slot;

    return &_worldPacket;
}

void WorldPackets::BattlePet::BattlePetSetBattleSlot::Read()
{
    _worldPacket >> PetGuid;
    _worldPacket >> Slot;
}

void WorldPackets::BattlePet::BattlePetModifyName::Read()
{
    _worldPacket >> PetGuid;
    uint32 nameLength = _worldPacket.ReadBits(7);
    bool hasDeclinedNames = _worldPacket.ReadBit();

    if (hasDeclinedNames)
    {
        int32 count[MAX_DECLINED_NAME_CASES] = {};
        for (int& var : count)
            var = _worldPacket.ReadBits(7);

        for (int32 i = 0; i < MAX_DECLINED_NAME_CASES; i++)
            DeclinedNames.name[i] = _worldPacket.ReadString(count[i]);
    }

    Name = _worldPacket.ReadString(nameLength);
}

void WorldPackets::BattlePet::QueryBattlePetName::Read()
{
    _worldPacket >> BattlePetID;
    _worldPacket >> UnitGUID;
}

WorldPacket const* WorldPackets::BattlePet::QueryBattlePetNameResponse::Write()
{
    _worldPacket << BattlePetID;
    _worldPacket << int32(CreatureID);
    _worldPacket << Timestamp;

    if (!_worldPacket.WriteBit(Allow))
        return &_worldPacket;

    _worldPacket.WriteBits(Name.size(), 8);
    _worldPacket.WriteBit(HasDeclined);
    _worldPacket.FlushBits();

    for (auto const& v : DeclinedNames)
        _worldPacket.WriteBits(v.size(), 7);

    for (auto const& v : DeclinedNames)
        _worldPacket.WriteString(v);

    _worldPacket.WriteString(Name);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::GuidData::Write()
{
    _worldPacket << BattlePetGUID;

    return &_worldPacket;
}

void WorldPackets::BattlePet::BattlePetDeletePet::Read()
{
    _worldPacket >> PetGuid;
}

void WorldPackets::BattlePet::BattlePetSetFlags::Read()
{
    _worldPacket >> PetGuid;
    _worldPacket >> Flags;
    ControlType = _worldPacket.ReadBits(2);
}

void WorldPackets::BattlePet::BattlePetClearFanfare::Read()
{
    _worldPacket >> PetGuid;
}

void WorldPackets::BattlePet::CageBattlePet::Read()
{
    _worldPacket >> PetGuid;
}

WorldPacket const* WorldPackets::BattlePet::BattlePetDeleted::Write()
{
    _worldPacket << PetGuid;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::BattlePetError::Write()
{
    _worldPacket.WriteBits(Result, 4);
    _worldPacket << int32(CreatureID);

    return &_worldPacket;
}

void WorldPackets::BattlePet::BattlePetSummon::Read()
{
    _worldPacket >> PetGuid;
}

void WorldPackets::BattlePet::BattlePetUpdateNotify::Read()
{
    _worldPacket >> PetGuid;
}

WorldPacket const* WorldPackets::BattlePet::BattlePetCageDateError::Write()
{
    _worldPacket << SecondsUntilCanCage;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::BattlePetTrapLevel::Write()
{
    _worldPacket << TrapLevel;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::FinalizeLocation::Write()
{
    _worldPacket << Location;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleLocation& locations)
{
    data << locations.LocationResult;
    data << locations.BattleOrigin;
    for (auto const& playerPosition : locations.PlayerPositions)
        data << playerPosition;

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::BattlePet::PetBattleLocation& locations)
{
    data >> locations.LocationResult;
    data >> locations.BattleOrigin;
    for (auto& playerPosition : locations.PlayerPositions)
        data >> playerPosition;

    return data;
}

void WorldPackets::BattlePet::BattlePetDeletePetCheat::Read()
{
    _worldPacket >> PetGuid;
}

void WorldPackets::BattlePet::RequestWild::Read()
{
    _worldPacket >> Battle.TargetGUID;
    _worldPacket >> Battle.Location;
}

void WorldPackets::BattlePet::RequestUpdate::Read()
{
    _worldPacket >> TargetGUID;
    Canceled = _worldPacket.ReadBit();
}

WorldPacket const* WorldPackets::BattlePet::RequestFailed::Write()
{
    _worldPacket << Reason;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattlePetUpdate const& update)
{
    data << update.JournalInfo.Guid;

    data << update.JournalInfo.Species;
    data << update.JournalInfo.DisplayID;
    data << update.JournalInfo.CreatureID;

    data << update.JournalInfo.Level;
    data << update.JournalInfo.Exp;

    data << update.JournalInfo.Health;
    data << update.JournalInfo.MaxHealth;
    data << update.JournalInfo.Power;
    data << update.JournalInfo.Speed;
    data << update.NpcTeamMemberID;

    data << uint16(update.JournalInfo.Quality);
    data << update.StatusFlags;

    data << update.Slot;

    data << uint32(update.Abilities.size());
    data << uint32(update.Auras.size());
    data << uint32(update.States.size());

    for (auto const& x : update.Abilities)
        data << x;

    for (auto const& v : update.Auras)
        data << v;

    for (auto const& c : update.States)
    {
        data << c.first;
        data << c.second;
    }

    data.WriteBits(update.JournalInfo.Name.size(), 7);
    data.WriteString(update.JournalInfo.Name);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePetAbility const& ability)
{
    data << ability.AbilityID;
    data << ability.CooldownRemaining;
    data << ability.LockdownRemaining;
    data << ability.AbilityIndex;
    data << ability.Pboid;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattlePlayerUpdate const& update)
{
    data << update.CharacterID;

    data << update.TrapAbilityID;
    data << update.TrapStatus;

    data << update.RoundTimeSecs;

    data << update.FrontPet;
    data << update.InputFlags;

    data.WriteBits(update.Pets.size(), 2);
    data.FlushBits();

    for (auto const& z : update.Pets)
        data << z;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePetAura const& aura)
{
    data << aura.AbilityID;
    data << aura.InstanceID;
    data << aura.RoundsRemaining;
    data << aura.CurrentRound;
    data << aura.CasterPBOID;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleEnviroUpdate const& update)
{
    data << uint32(update.Auras.size());
    data << uint32(update.States.size());
    for (auto const& x : update.Auras)
        data << x;

    for (auto const& v : update.States)
    {
        data << v.first;
        data << v.second;
    }

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleFullUpdate const& update)
{
    for (auto const& player : update.Players)
        data << player;

    for (auto const& enviro : update.Enviros)
        data << enviro;

    data << update.WaitingForFrontPetsMaxSecs;
    data << update.PvpMaxRoundTime;

    data << update.CurRound;
    data << update.NpcCreatureID;
    data << update.NpcDisplayID;

    data << update.CurPetBattleState;
    data << update.ForfeitPenalty;

    data << update.InitialWildPetGUID;

    data.WriteBit(update.IsPVP);
    data.WriteBit(update.CanAwardXP);
    data.FlushBits();

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::FinalRound const& finalRound)
{
    data.FlushBits();
    data.WriteBit(finalRound.Abandoned);
    data.WriteBit(finalRound.PvpBattle);
    for (auto winner : finalRound.Winner)
        data.WriteBit(winner);

    for (auto npcCreatureID : finalRound.NpcCreatureID)
        data << npcCreatureID;

    data << uint32(finalRound.Pets.size());

    for (auto const& pet : finalRound.Pets)
        data << pet;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleEffectTarget const& effectTarget)
{
    data.FlushBits();
    data.WriteBits(effectTarget.Type, 4);
    data << effectTarget.Petx;

    switch (effectTarget.Type)
    {
    case PET_BATTLE_EFFECT_TARGET_EX_NPC_EMOTE:
        data << int32(effectTarget.Params.BroadcastTextID);
        break;
    case PET_BATTLE_EFFECT_TARGET_EX_AURA:
        data << int32(effectTarget.Params.Aura.AuraInstanceID);
        data << int32(effectTarget.Params.Aura.AuraAbilityID);
        data << int32(effectTarget.Params.Aura.RoundsRemaining);
        data << int32(effectTarget.Params.Aura.CurrentRound);
        break;
    case PET_BATTLE_EFFECT_TARGET_EX_STAT_CHANGE:
        data << int32(effectTarget.Params.NewStatValue);
        break;
    case PET_BATTLE_EFFECT_TARGET_EX_PET:
        data << int32(effectTarget.Params.Health);
        break;
    case PET_BATTLE_EFFECT_TARGET_EX_ABILITY_CHANGE:
        data << int32(effectTarget.Params.AbilityChange.ChangedAbilityID);
        data << int32(effectTarget.Params.AbilityChange.CooldownRemaining);
        data << int32(effectTarget.Params.AbilityChange.LockdownRemaining);
        break;
    case PET_BATTLE_EFFECT_TARGET_EX_TRIGGER_ABILITY:
        data << int32(effectTarget.Params.TriggerAbilityID);
        break;
    case PET_BATTLE_EFFECT_TARGET_EX_STATE:
        data << int32(effectTarget.Params.State.StateID);
        data << int32(effectTarget.Params.State.StateValue);
        break;
    default:
        break;
    }

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::Effect const& effect)
{
    data << effect.AbilityEffectID;
    data << effect.Flags;
    data << effect.SourceAuraInstanceID;
    data << effect.TurnInstanceID;
    data << effect.EffectType;
    data << effect.CasterPBOID;
    data << effect.StackDepth;
    data << uint32(effect.EffectTargetData.size());
    for (auto const& map : effect.EffectTargetData)
        data << map;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::RoundResult const& roundResult)
{
    data << roundResult.CurRound;
    data << roundResult.NextPetBattleState;
    data << uint32(roundResult.EffectData.size());

    for (uint8 i = 0; i < PARTICIPANTS_COUNT; ++i)
    {
        data << roundResult.NextInputFlags[i];
        data << roundResult.NextTrapStatus[i];
        data << roundResult.RoundTimeSecs[i];
    }

    data << uint32(roundResult.Ability.size());
    for (auto const& map : roundResult.Ability)
        data << map;

    data.FlushBits();
    data.WriteBits(roundResult.PetXDied.size(), 3);

    for (auto const& map : roundResult.EffectData)
        data << map;

    for (uint8 const& map : roundResult.PetXDied)
        data << map;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::FinalPet const& finalPet)
{
    data << finalPet.Guid;
    data << finalPet.Level;
    data << finalPet.Xp;
    data << finalPet.Health;
    data << finalPet.MaxHealth;
    data << finalPet.InitialLevel;
    data << finalPet.Pboid;

    data.WriteBit(finalPet.Captured);
    data.WriteBit(finalPet.SeenAction);
    data.WriteBit(finalPet.Caged);
    data.WriteBit(finalPet.AwardedXP);
    data.FlushBits();

    return data;
}

WorldPacket const* WorldPackets::BattlePet::BattleRound::Write()
{
    _worldPacket << MsgData; 

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::PetBattleInitialUpdate::Write()
{
    _worldPacket << MsgData; 

    return &_worldPacket;
}

WorldPacket const* WorldPackets::BattlePet::PetBattleFinalRound::Write()
{
    _worldPacket << MsgData;

    return &_worldPacket;
}

void WorldPackets::BattlePet::LeaveQueue::Read()
{
    _worldPacket >> Ticket;
}

WorldPacket const* WorldPackets::BattlePet::PetBattleQueueStatus::Write()
{
    _worldPacket << Msg.Status;
    _worldPacket << uint32(Msg.SlotResult.size());
    _worldPacket << Msg.Ticket;
    for (auto const& map : Msg.SlotResult)
        _worldPacket << map;

    _worldPacket.WriteBit(Msg.ClientWaitTime.has_value());
    _worldPacket.WriteBit(Msg.AverageWaitTime.has_value());
    _worldPacket.FlushBits();

    if (Msg.ClientWaitTime)
        _worldPacket << *Msg.ClientWaitTime;

    if (Msg.AverageWaitTime)
        _worldPacket << *Msg.AverageWaitTime;

    return &_worldPacket;
}

void WorldPackets::BattlePet::PetBattleInput::Read()
{
    _worldPacket >> MoveType;
    _worldPacket >> NewFrontPet;
    _worldPacket >> DebugFlags;
    _worldPacket >> BattleInterrupted;
    _worldPacket >> AbilityID;
    _worldPacket >> Round;
    IgnoreAbandonPenalty = _worldPacket.ReadBit();
}

void WorldPackets::BattlePet::QueueProposeMatchResult::Read()
{
    Accepted = _worldPacket.ReadBit();
}

void WorldPackets::BattlePet::ReplaceFrontPet::Read()
{
    _worldPacket >> FrontPet;
}

void WorldPackets::BattlePet::RequestPVP::Read()
{
    _worldPacket >> Battle.TargetGUID;
    _worldPacket >> Battle.Location;
}

WorldPacket const* WorldPackets::BattlePet::PVPChallenge::Write()
{
    _worldPacket << ChallengerGUID;
    _worldPacket << Location;

    return &_worldPacket;
}


void WorldPackets::BattlePet::BattlePetGuidRead::Read()
{
    _worldPacket >> BattlePetGUID;
}
