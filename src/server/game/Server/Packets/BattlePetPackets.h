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

#ifndef BattlePetPackets_h__
#define BattlePetPackets_h__

#include "Packet.h"
#include "PacketUtilities.h"
#include "PetBattle.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "UnitDefines.h"
#include "LFGPacketsCommon.h"
#include <memory>

static uint16 const BATTLE_PET_MAX_JOURNAL_PETS = 1000;
static uint8 const PARTICIPANTS_COUNT = 2;

enum PetBattleEffectTargetType
{
    PET_BATTLE_EFFECT_TARGET_EX_FRONT_PET = 0,
    PET_BATTLE_EFFECT_TARGET_EX_AURA = 1,
    PET_BATTLE_EFFECT_TARGET_EX_STATE = 2,
    PET_BATTLE_EFFECT_TARGET_EX_PET = 3,
    PET_BATTLE_EFFECT_TARGET_EX_STAT_CHANGE = 4,
    PET_BATTLE_EFFECT_TARGET_EX_TRIGGER_ABILITY = 5,
    PET_BATTLE_EFFECT_TARGET_EX_ABILITY_CHANGE = 6,
    PET_BATTLE_EFFECT_TARGET_EX_NPC_EMOTE = 7,
    PET_BATTLE_EFFECT_TARGET_EX_UNK_0 = 8,
    PET_BATTLE_EFFECT_TARGET_EX_UNK_1 = 9
};

namespace WorldPackets
{
    namespace BattlePet
    {
        struct BattlePetOwnerInfo
        {
            ObjectGuid Guid;
            uint32 PlayerVirtualRealm = 0;
            uint32 PlayerNativeRealm = 0;
        };

        struct BattlePet
        {
            ObjectGuid Guid;
            uint32 Species = 0;
            uint32 CreatureID = 0;
            uint32 DisplayID = 0;
            uint16 Breed = 0;
            uint16 Level = 0;
            uint16 Exp = 0;
            uint16 Flags = 0;
            uint32 Power = 0;
            uint32 Health = 0;
            uint32 MaxHealth = 0;
            uint32 Speed = 0;
            uint8 Quality = 0;
            Optional<BattlePetOwnerInfo> OwnerInfo;
            std::string Name;
        };

        struct BattlePetSlot
        {
            BattlePet Pet;
            uint32 CollarID = 0;
            uint8 Index = 0;
            bool Locked = false;
        };

        //< CMSG_BATTLE_PET_REQUEST_JOURNAL
        //< CMSG_BATTLE_PET_REQUEST_JOURNAL_LOCK
        //< CMSG_PET_BATTLE_FINAL_NOTIFY
        //< CMSG_JOIN_PET_BATTLE_QUEUE
        //< CMSG_PET_BATTLE_SCRIPT_ERROR_NOTIFY
        class NullCmsg final : public ClientPacket
        {
        public:
            NullCmsg(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override { }
        };

        class LeaveQueue final : public ClientPacket
        {
        public:
            LeaveQueue(WorldPacket&& packet) : ClientPacket(CMSG_LEAVE_PET_BATTLE_QUEUE, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket Ticket;
        };

        class BattlePetJournal final : public ServerPacket
        {
        public:
            BattlePetJournal() : ServerPacket(SMSG_BATTLE_PET_JOURNAL) { }

            WorldPacket const* Write() override;

            //uint32 NumMaxPets = BATTLE_PET_MAX_JOURNAL_PETS;
            uint16 Trap = 0;
            bool HasJournalLock = false;
            std::vector<BattlePetSlot> Slots;
            std::vector<BattlePet> Pets;
        };

        class BattlePetUpdates final : public ServerPacket
        {
        public:
            BattlePetUpdates() : ServerPacket(SMSG_BATTLE_PET_UPDATES) { }

            WorldPacket const* Write() override;

            std::vector<BattlePet> Pets;

            bool PetAdded = false;
        };

        class PetBattleSlotUpdates final : public ServerPacket
        {
        public:
            PetBattleSlotUpdates() : ServerPacket(SMSG_PET_BATTLE_SLOT_UPDATES, 4 + 1 + 1) { }

            WorldPacket const* Write() override;

            std::vector<BattlePetSlot> Slots;
            bool AutoSlotted = false;
            bool NewSlot = false;
        };

        class BattlePetSetBattleSlot final : public ClientPacket
        {
        public:
            BattlePetSetBattleSlot(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_SET_BATTLE_SLOT, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
            uint8 Slot = 0;
        };

        class BattlePetModifyName final : public ClientPacket
        {
        public:
            BattlePetModifyName(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_MODIFY_NAME, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
            std::string Name;
            DeclinedName DeclinedNames;
        };

        class QueryBattlePetName final : public ClientPacket
        {
        public:
            QueryBattlePetName(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_BATTLE_PET_NAME, std::move(packet)) { }

            void Read() override;

            ObjectGuid BattlePetID;
            ObjectGuid UnitGUID;
        };

        class QueryBattlePetNameResponse final : public ServerPacket
        {
        public:
            QueryBattlePetNameResponse() : ServerPacket(SMSG_QUERY_BATTLE_PET_NAME_RESPONSE, 16 + 4 + 4 + 1 + 2 + 2 + 1) { }

            WorldPacket const* Write() override;

            ObjectGuid BattlePetID;
            int32 CreatureID = 0;
            WorldPackets::Timestamp<> Timestamp;
            bool Allow = false;

            bool HasDeclined = false;
            std::array<std::string, MAX_DECLINED_NAME_CASES> DeclinedNames;
            std::string Name;
        };

        class BattlePetDeletePet final : public ClientPacket
        {
        public:
            BattlePetDeletePet(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_DELETE_PET, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
        };

        class BattlePetDeletePetCheat final : public ClientPacket
        {
        public:
            BattlePetDeletePetCheat(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_DELETE_PET_CHEAT, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
        };

        class BattlePetSetFlags final : public ClientPacket
        {
        public:
            BattlePetSetFlags(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_SET_FLAGS, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
            uint16 Flags = 0;
            uint8 ControlType = 0;
        };

        class BattlePetClearFanfare final : public ClientPacket
        {
        public:
            BattlePetClearFanfare(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_CLEAR_FANFARE, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
        };

        class CageBattlePet final : public ClientPacket
        {
        public:
            CageBattlePet(WorldPacket&& packet) : ClientPacket(CMSG_CAGE_BATTLE_PET, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
        };

        //< CMSG_PET_BATTLE_QUIT_NOTIFY
        class BattlePetGuidRead final : public ClientPacket
        {
        public:
            BattlePetGuidRead(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

            void Read() override;

            ObjectGuid BattlePetGUID;
        };

        class BattlePetDeleted final : public ServerPacket
        {
        public:
            BattlePetDeleted(ObjectGuid battlePetGUID) : ServerPacket(SMSG_BATTLE_PET_DELETED, 16), PetGuid(battlePetGUID) { }

            WorldPacket const* Write() override;

            ObjectGuid PetGuid;
        };

        class BattlePetError final : public ServerPacket
        {
        public:
            BattlePetError() : ServerPacket(SMSG_BATTLE_PET_ERROR, 8) { }

            WorldPacket const* Write() override;

            uint8 Result = 0;
            int32 CreatureID = 0;
        };

        class BattlePetSummon final : public ClientPacket
        {
        public:
            BattlePetSummon(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_SUMMON, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
        };

        class BattlePetUpdateNotify final : public ClientPacket
        {
        public:
            BattlePetUpdateNotify(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_UPDATE_NOTIFY, std::move(packet)) { }

            void Read() override;

            ObjectGuid PetGuid;
        };

        class BattlePetCageDateError final : public ServerPacket
        {
        public:
            BattlePetCageDateError(int32 secondsUntilCanCage) : ServerPacket(SMSG_BATTLE_PET_CAGE_DATE_ERROR, 4), SecondsUntilCanCage(secondsUntilCanCage) { }

            WorldPacket const* Write() override;

            int32 SecondsUntilCanCage = 0;
        };

        class BattlePetTrapLevel final : public ServerPacket
        {
        public:
            BattlePetTrapLevel(int16 trapLevel) : ServerPacket(SMSG_BATTLE_PET_TRAP_LEVEL, 2), TrapLevel(trapLevel) { }

            WorldPacket const* Write() override;

            int16 TrapLevel = 0;
        };

        static uint8 const PARTICIPANTS_COUNT = 2;

        struct PetBattleLocation
        {
            int32 LocationResult = 0; //ok
            TaggedPosition<Position::XYZO> BattleOrigin;
            TaggedPosition<Position::XYZ> PlayerPositions[2];
        };

        class FinalizeLocation final : public ServerPacket
        {
        public:
            FinalizeLocation() : ServerPacket(SMSG_PET_BATTLE_FINALIZE_LOCATION, 4 + 12 + 4 + 12 * 2) { }

            PetBattleLocation Location;

            WorldPacket const* Write() override;
        };

        class BattlePetUpdateDisplayNotify final : public ClientPacket
        {
        public:
            BattlePetUpdateDisplayNotify(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PET_UPDATE_DISPLAY_NOTIFY, std::move(packet)) { }

            void Read() override { }
        };

        struct BattleRequest
        {
            ObjectGuid TargetGUID;
            PetBattleLocation Location;
        };

        class RequestWild final : public ClientPacket
        {
        public:
            RequestWild(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_REQUEST_WILD, std::move(packet)) { }

            void Read() override;

            BattleRequest Battle;
        };

        class RequestUpdate final : public ClientPacket
        {
        public:
            RequestUpdate(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_REQUEST_UPDATE, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetGUID;
            bool Canceled = false;
        };

        //< SMSG_BATTLE_PET_RESTORED
       //< SMSG_BATTLE_PET_REVOKED
        class GuidData final : public ServerPacket
        {
        public:
            GuidData(OpcodeServer opcode) : ServerPacket(opcode, 16) { }

            WorldPacket const* Write() override;

            ObjectGuid BattlePetGUID;
        };

        class RequestFailed final : public ServerPacket
        {
        public:
            RequestFailed(uint8 reason) : ServerPacket(SMSG_PET_BATTLE_REQUEST_FAILED, 1), Reason(reason) { }

            WorldPacket const* Write() override;

            uint8 Reason = 0;
        };

        struct PetBattleEffectTarget
        {
            uint8 Type;
            int8 Petx;

            union TargetParams
            {
                TargetParams() = default;

                int32 Health;
                int32 NewStatValue;
                int32 TriggerAbilityID;
                int32 BroadcastTextID;
                int32 typeUnk0;
                int32 typeUnk1;

                struct
                {
                    uint32 AuraInstanceID;
                    uint32 AuraAbilityID;
                    int32 RoundsRemaining;
                    int32 CurrentRound;
                } Aura;

                struct
                {
                    uint32 StateID;
                    int32 StateValue;
                } State;

                struct
                {
                    int32 ChangedAbilityID;
                    int32 CooldownRemaining;
                    int32 LockdownRemaining;
                } AbilityChange;
            } Params;
        };

        struct Effect
        {
            uint32 AbilityEffectID = 0;
            uint16 Flags = 0;
            uint16 SourceAuraInstanceID = 0;
            uint16 TurnInstanceID = 0;
            uint8 EffectType = 0;
            int8 CasterPBOID = 0;
            uint8 StackDepth = 0;
            std::list<PetBattleEffectTarget> EffectTargetData;
        };

        struct BattlePetAbility
        {
            int32 AbilityID = 0;
            int16 CooldownRemaining = 0;
            int16 LockdownRemaining = 0;
            uint8 AbilityIndex = 0;
            uint8 Pboid = 0;
        };

        struct RoundResult
        {
            uint32 CurRound = 0;
            uint8 NextPetBattleState = 0;
            uint8 NextInputFlags[PARTICIPANTS_COUNT] = { };
            uint8 NextTrapStatus[PARTICIPANTS_COUNT] = { };
            uint16 RoundTimeSecs[PARTICIPANTS_COUNT] = { };
            std::vector<Effect> EffectData;
            std::vector<BattlePetAbility> Ability;
            std::vector<uint8> PetXDied;
        };

        //< SMSG_PET_BATTLE_FIRST_ROUND
        //< SMSG_PET_BATTLE_ROUND_RESULT
        //< SMSG_PET_BATTLE_REPLACEMENTS_MADE
        class BattleRound final : public ServerPacket
        {
        public:
            BattleRound(OpcodeServer opcode) : ServerPacket(opcode, 4 + 1 + 1 + 1 + 2 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            RoundResult MsgData;
        };

        struct BattlePetAura
        {
            int32 AbilityID = 0;
            uint32 InstanceID = 0;
            int32 RoundsRemaining = 0;
            int32 CurrentRound = 0;
            uint8 CasterPBOID = 0;
        };

        struct PetBattlePetUpdate
        {
            BattlePet JournalInfo;
            uint32 NpcTeamMemberID = 0;
            uint16 StatusFlags = 0; // same as Pet.Flags?
            uint8 Slot = 0;
            std::vector<BattlePetAbility> Abilities;
            std::vector<BattlePetAura> Auras;
            std::unordered_map<uint32 /*StateID*/, int32 /*StateValue*/> States;
        };

        struct PetBattlePlayerUpdate
        {
            ObjectGuid CharacterID;
            int32 TrapAbilityID = 0;
            int32 TrapStatus = 0;
            uint16 RoundTimeSecs = 0;
            std::vector<PetBattlePetUpdate> Pets;
            int8 FrontPet = 0;
            uint8 InputFlags = 0;
        };

        struct PetBattleEnviroUpdate
        {
            std::vector<BattlePetAura> Auras;
            std::unordered_map<uint32 /*StateID*/, int32 /*StateValue*/> States;
        };

        struct PetBattleFullUpdate
        {
            PetBattlePlayerUpdate Players[PARTICIPANTS_COUNT] = { };
            PetBattleEnviroUpdate Enviros[3] = { };
            ObjectGuid InitialWildPetGUID;
            uint32 NpcCreatureID = 0;
            uint32 NpcDisplayID = 0;
            uint32 CurRound = 0;
            uint16 WaitingForFrontPetsMaxSecs = 0;
            uint16 PvpMaxRoundTime = 0;
            uint8 ForfeitPenalty = 0;
            int8 CurPetBattleState = 0;
            bool IsPVP = false;
            bool CanAwardXP = false;
        };

        class PetBattleInitialUpdate final : public ServerPacket
        {
        public:
            PetBattleInitialUpdate() : ServerPacket(SMSG_PET_BATTLE_INITIAL_UPDATE, sizeof(PetBattleFullUpdate)) { }

            WorldPacket const* Write() override;

            PetBattleFullUpdate MsgData;
        };

        struct FinalPet
        {
            ObjectGuid Guid;
            uint16 Level = 0;
            uint16 Xp = 0;
            uint32 Health = 0;
            uint32 MaxHealth = 0;
            uint16 InitialLevel = 0;
            uint8 Pboid = 0;
            bool Captured = false;
            bool Caged = false;
            bool SeenAction = false;
            bool AwardedXP = false;
        };

        struct FinalRound
        {
            bool Abandoned = false;
            bool PvpBattle = false;
            bool Winner[PARTICIPANTS_COUNT] = { };
            uint32 NpcCreatureID[PARTICIPANTS_COUNT] = { };
            std::vector<FinalPet> Pets;
        };

        class PetBattleFinalRound final : public ServerPacket
        {
        public:
            PetBattleFinalRound() : ServerPacket(SMSG_PET_BATTLE_FINAL_ROUND, sizeof(FinalRound)) { }

            WorldPacket const* Write() override;

            FinalRound MsgData;
        };

        struct QueueStatus
        {
            uint32 Status = 0;
            std::vector<uint32> SlotResult;
            Optional<uint32> ClientWaitTime;
            Optional<uint32> AverageWaitTime;
            LFG::RideTicket Ticket;
        };

        class PetBattleQueueStatus final : public ServerPacket
        {
        public:
            PetBattleQueueStatus() : ServerPacket(SMSG_PET_BATTLE_QUEUE_STATUS, 4 + 4 + 1 + 1 + 28) { }

            WorldPacket const* Write() override;

            QueueStatus Msg;
        };

        //< SMSG_BATTLE_PET_JOURNAL_LOCK_ACQUIRED
        //< SMSG_PET_BATTLE_FINISHED
        //< SMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH
        //< SMSG_BATTLE_PET_JOURNAL_LOCK_DENIED
        //< SMSG_PET_BATTLE_CHAT_RESTRICTED
        //< SMSG_BATTLE_PET_LICENSE_CHANGED
        //< SMSG_BATTLE_PETS_HEALED
        class NullSMsg final : public ServerPacket
        {
        public:
            NullSMsg(OpcodeServer opcode) : ServerPacket(opcode, 0) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };

        class PetBattleInput final : public ClientPacket
        {
        public:
            PetBattleInput(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_INPUT, std::move(packet)) { }

            void Read() override;

            int32 AbilityID = 0;
            int32 Round = 0;
            uint8 MoveType = 0;
            int8 NewFrontPet = 0;
            uint8 DebugFlags = 0;
            uint8 BattleInterrupted = 0;
            bool IgnoreAbandonPenalty = false;
        };

        class QueueProposeMatchResult final : public ClientPacket
        {
        public:
            QueueProposeMatchResult(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH_RESULT, std::move(packet)) { }

            void Read() override;

            bool Accepted = false;
        };

        class ReplaceFrontPet final : public ClientPacket
        {
        public:
            ReplaceFrontPet(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_REPLACE_FRONT_PET, std::move(packet)) { }

            void Read() override;

            uint8 FrontPet = 0;
        };

        class RequestPVP final : public ClientPacket
        {
        public:
            RequestPVP(WorldPacket&& packet) : ClientPacket(CMSG_PET_BATTLE_REQUEST_PVP, std::move(packet)) { }

            void Read() override;

            BattleRequest Battle;
        };

        class PVPChallenge final : public ServerPacket
        {
        public:
            PVPChallenge() : ServerPacket(SMSG_PET_BATTLE_PVP_CHALLENGE, 16 + 4 + 12 + 4 + 12 * 2) { }

            WorldPacket const* Write() override;

            ObjectGuid ChallengerGUID;
            PetBattleLocation Location;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleLocation& locations);
ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::BattlePet::PetBattleLocation& locations);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePetSlot const& petBattleSlot);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePet const& battlePet);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleEffectTarget const& effectTarget);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::Effect const& effect);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::RoundResult const& roundResult);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::FinalPet const& finalPet);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::FinalRound const& finalRound);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattlePlayerUpdate const& update);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleEnviroUpdate const& update);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::PetBattleFullUpdate const& update);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePetAbility const& ability);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePet::BattlePetAura const& aura);

#endif // BattlePetPackets_h__
