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

#ifndef MiscPackets_h__
#define MiscPackets_h__

#include "Packet.h"
#include "CollectionMgr.h"
#include "CUFProfile.h"
#include "ItemPacketsCommon.h"
#include "MythicPlusPacketsCommon.h" // < DekkCore
#include "ObjectGuid.h"
#include "InspectPackets.h"
#include "Optional.h"
#include "PacketUtilities.h"
#include "Player.h"
#include "Position.h"
#include "SharedDefines.h"
#include <array>
#include <map>

enum UnitStandStateType : uint8;
enum WeatherState : uint32;

namespace WorldPackets
{
    namespace Misc
    {
        class BindPointUpdate final : public ServerPacket
        {
        public:
            BindPointUpdate() : ServerPacket(SMSG_BIND_POINT_UPDATE, 20) { }

            WorldPacket const* Write() override;

            uint32 BindMapID = 0;
            TaggedPosition<Position::XYZ> BindPosition;
            uint32 BindAreaID = 0;
        };

        class PlayerBound final : public ServerPacket
        {
        public:
            PlayerBound() : ServerPacket(SMSG_PLAYER_BOUND, 16 + 4) { }
            PlayerBound(ObjectGuid binderId, uint32 areaId) : ServerPacket(SMSG_PLAYER_BOUND, 16 + 4), BinderID(binderId), AreaID(areaId) { }

            WorldPacket const* Write() override;

            ObjectGuid BinderID;
            uint32 AreaID = 0;
        };

        class InvalidatePlayer final : public ServerPacket
        {
        public:
            InvalidatePlayer() : ServerPacket(SMSG_INVALIDATE_PLAYER, 18) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
        };

        class LoginSetTimeSpeed final : public ServerPacket
        {
        public:
            LoginSetTimeSpeed() : ServerPacket(SMSG_LOGIN_SET_TIME_SPEED, 20) { }

            WorldPacket const* Write() override;

            float NewSpeed = 0.0f;
            int32 ServerTimeHolidayOffset = 0;
            uint32 GameTime = 0;
            uint32 ServerTime = 0;
            int32 GameTimeHolidayOffset = 0;
        };

        class ResetWeeklyCurrency final : public ServerPacket
        {
        public:
            ResetWeeklyCurrency() : ServerPacket(SMSG_RESET_WEEKLY_CURRENCY, 0) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };

        class SetCurrency final : public ServerPacket
        {
        public:
            SetCurrency() : ServerPacket(SMSG_SET_CURRENCY, 12) { }

            WorldPacket const* Write() override;

            int32 Type = 0;
            int32 Quantity = 0;
            CurrencyGainFlags Flags = CurrencyGainFlags(0);
            std::vector<Item::UiEventToast> Toasts;
            Optional<int32> WeeklyQuantity;
            Optional<int32> TrackedQuantity;
            Optional<int32> MaxQuantity;
            Optional<int32> TotalEarned;
            Optional<int32> QuantityChange;
            Optional<CurrencyGainSource> QuantityGainSource;
            Optional<CurrencyDestroyReason> QuantityLostSource;
            Optional<uint32> FirstCraftOperationID;
            Optional<Timestamp<>> NextRechargeTime;
            Optional<Timestamp<>> RechargeCycleStartTime;
            bool SuppressChatLog = false;
        };

        class SetSelection final : public ClientPacket
        {
        public:
            SetSelection(WorldPacket&& packet) : ClientPacket(CMSG_SET_SELECTION, std::move(packet)) { }

            void Read() override;

            ObjectGuid Selection; ///< Target
        };

        class SetupCurrency final : public ServerPacket
        {
        public:
            struct Record
            {
                int32 Type = 0;                       // ID from CurrencyTypes.dbc
                int32 Quantity = 0;
                Optional<int32> WeeklyQuantity;       // Currency count obtained this Week.
                Optional<int32> MaxWeeklyQuantity;    // Weekly Currency cap.
                Optional<int32> TrackedQuantity;
                Optional<int32> MaxQuantity;
                Optional<int32> TotalEarned;
                Optional<Timestamp<>> NextRechargeTime;
                Optional<Timestamp<>> RechargeCycleStartTime;
                uint8 Flags = 0;
            };

            SetupCurrency() : ServerPacket(SMSG_SETUP_CURRENCY, 22) { }

            WorldPacket const* Write() override;

            std::vector<Record> Data;
        };

        class ViolenceLevel final : public ClientPacket
        {
        public:
            ViolenceLevel(WorldPacket&& packet) : ClientPacket(CMSG_VIOLENCE_LEVEL, std::move(packet)) { }

            void Read() override;

            int8 ViolenceLvl = -1; ///< 0 - no combat effects, 1 - display some combat effects, 2 - blood, 3 - bloody, 4 - bloodier, 5 - bloodiest
        };

        class TimeSyncRequest final : public ServerPacket
        {
        public:
            TimeSyncRequest() : ServerPacket(SMSG_TIME_SYNC_REQUEST, 4) { }

            WorldPacket const* Write() override;

            uint32 SequenceIndex = 0;
        };

        class TimeSyncResponse final : public ClientPacket
        {
        public:
            TimeSyncResponse(WorldPacket&& packet) : ClientPacket(CMSG_TIME_SYNC_RESPONSE, std::move(packet)) { }

            void Read() override;

            TimePoint GetReceivedTime() const { return _worldPacket.GetReceivedTime(); }

            uint32 ClientTime = 0; // Client ticks in ms
            uint32 SequenceIndex = 0; // Same index as in request
        };

        class TriggerCinematic final : public ServerPacket
        {
        public:
            TriggerCinematic() : ServerPacket(SMSG_TRIGGER_CINEMATIC, 4) { }

            WorldPacket const* Write() override;

            uint32 CinematicID = 0;
            ObjectGuid ConversationGuid;
        };

        class TriggerMovie final : public ServerPacket
        {
        public:
            TriggerMovie() : ServerPacket(SMSG_TRIGGER_MOVIE, 4) { }

            WorldPacket const* Write() override;

            uint32 MovieID = 0;
        };

        class ServerTimeOffsetRequest final : public ClientPacket
        {
        public:
            ServerTimeOffsetRequest(WorldPacket&& packet) : ClientPacket(CMSG_SERVER_TIME_OFFSET_REQUEST, std::move(packet)) { }

            void Read() override { }
        };

        class ServerTimeOffset final : public ServerPacket
        {
        public:
            ServerTimeOffset() : ServerPacket(SMSG_SERVER_TIME_OFFSET, 4) { }

            WorldPacket const* Write() override;

            Timestamp<> Time;
        };

        class TutorialFlags : public ServerPacket
        {
        public:
            TutorialFlags() : ServerPacket(SMSG_TUTORIAL_FLAGS, 32)
            {
                std::memset(TutorialData, 0, sizeof(TutorialData));
            }

            WorldPacket const* Write() override;

            uint32 TutorialData[MAX_ACCOUNT_TUTORIAL_VALUES];
        };

        class TutorialSetFlag final : public ClientPacket
        {
        public:
            TutorialSetFlag(WorldPacket&& packet) : ClientPacket(CMSG_TUTORIAL, std::move(packet)) { }

            void Read() override;

            uint8 Action = 0;
            uint32 TutorialBit = 0;
        };

        class WorldServerInfo final : public ServerPacket
        {
        public:
            WorldServerInfo() : ServerPacket(SMSG_WORLD_SERVER_INFO, 26) { }

            WorldPacket const* Write() override;

            uint32 DifficultyID     = 0;
            bool IsTournamentRealm  = false;
            bool XRealmPvpAlert     = false;
            bool BlockExitingLoadingScreen = false;     // when set to true, sending SMSG_UPDATE_OBJECT with CreateObject Self bit = true will not hide loading screen
                                                        // instead it will be done after this packet is sent again with false in this bit and SMSG_UPDATE_OBJECT Values for player
            Optional<uint32> RestrictedAccountMaxLevel;
            Optional<uint64> RestrictedAccountMaxMoney;
            Optional<uint32> InstanceGroupSize;
        };

        class SetDungeonDifficulty final : public ClientPacket
        {
        public:
            SetDungeonDifficulty(WorldPacket&& packet) : ClientPacket(CMSG_SET_DUNGEON_DIFFICULTY, std::move(packet)) { }

            void Read() override;

            uint32 DifficultyID = 0;
        };

        class SetRaidDifficulty final : public ClientPacket
        {
        public:
            SetRaidDifficulty(WorldPacket&& packet) : ClientPacket(CMSG_SET_RAID_DIFFICULTY, std::move(packet)) { }

            void Read() override;

            int32 DifficultyID = 0;
            uint8 Legacy = 0;
        };

        class DungeonDifficultySet final : public ServerPacket
        {
        public:
            DungeonDifficultySet() : ServerPacket(SMSG_SET_DUNGEON_DIFFICULTY, 4) { }

            WorldPacket const* Write() override;

            int32 DifficultyID = 0;
        };

        class RaidDifficultySet final : public ServerPacket
        {
        public:
            RaidDifficultySet() : ServerPacket(SMSG_RAID_DIFFICULTY_SET, 4 + 1) { }

            WorldPacket const* Write() override;

            int32 DifficultyID = 0;
            uint8 Legacy = 0;
        };

        class CorpseReclaimDelay : public ServerPacket
        {
        public:
            CorpseReclaimDelay() : ServerPacket(SMSG_CORPSE_RECLAIM_DELAY, 4) { }

            WorldPacket const* Write() override;

            uint32 Remaining = 0;
        };

        class DeathReleaseLoc : public ServerPacket
        {
        public:
            DeathReleaseLoc() : ServerPacket(SMSG_DEATH_RELEASE_LOC, 4 + (3 * 4)) { }

            WorldPacket const* Write() override;

            int32 MapID = 0;
            TaggedPosition<Position::XYZ> Loc;
        };

        class PortGraveyard final : public ClientPacket
        {
        public:
            PortGraveyard(WorldPacket&& packet) : ClientPacket(CMSG_CLIENT_PORT_GRAVEYARD, std::move(packet)) { }

            void Read() override { }
        };

        class PreRessurect : public ServerPacket
        {
        public:
            PreRessurect() : ServerPacket(SMSG_PRE_RESSURECT, 18) { }

            WorldPacket const* Write() override;

            ObjectGuid PlayerGUID;
        };

        class ReclaimCorpse final : public ClientPacket
        {
        public:
            ReclaimCorpse(WorldPacket&& packet) : ClientPacket(CMSG_RECLAIM_CORPSE, std::move(packet)) { }

            void Read() override;

            ObjectGuid CorpseGUID;
        };

        class RepopRequest final : public ClientPacket
        {
        public:
            RepopRequest(WorldPacket&& packet) : ClientPacket(CMSG_REPOP_REQUEST, std::move(packet)) { }

            void Read() override;

            bool CheckInstance = false;
        };

        class RequestCemeteryList final : public ClientPacket
        {
        public:
            RequestCemeteryList(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_CEMETERY_LIST, std::move(packet)) { }

            void Read() override { }
        };

        class RequestCemeteryListResponse final : public ServerPacket
        {
        public:
            RequestCemeteryListResponse() : ServerPacket(SMSG_REQUEST_CEMETERY_LIST_RESPONSE, 1) { }

            WorldPacket const* Write() override;

            bool IsGossipTriggered = false;
            std::vector<uint32> CemeteryID;
        };

        class ResurrectResponse final : public ClientPacket
        {
        public:
            ResurrectResponse(WorldPacket&& packet) : ClientPacket(CMSG_RESURRECT_RESPONSE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Resurrecter;
            uint32 Response = 0;
        };

        class TC_GAME_API Weather final : public ServerPacket
        {
        public:
            Weather();
            Weather(WeatherState weatherID, float intensity = 0.0f, bool abrupt = false);

            WorldPacket const* Write() override;

            bool Abrupt = false;
            float Intensity = 0.0f;
            WeatherState WeatherID = WeatherState(0);
        };

        class StandStateChange final : public ClientPacket
        {
        public:
            StandStateChange(WorldPacket&& packet) : ClientPacket(CMSG_STAND_STATE_CHANGE, std::move(packet)) { }

            void Read() override;

            UnitStandStateType StandState = UnitStandStateType(0);
        };

        class StandStateUpdate final : public ServerPacket
        {
        public:
            StandStateUpdate() : ServerPacket(SMSG_STAND_STATE_UPDATE, 4 + 1) { }
            StandStateUpdate(UnitStandStateType state, uint32 animKitID) : ServerPacket(SMSG_STAND_STATE_UPDATE, 4 + 1), AnimKitID(animKitID), State(state) { }

            WorldPacket const* Write() override;

            uint32 AnimKitID = 0;
            UnitStandStateType State = UnitStandStateType(0);
        };

        class SetAnimTier final : public ServerPacket
        {
        public:
            SetAnimTier(): ServerPacket(SMSG_SET_ANIM_TIER, 16 + 1) { }

            WorldPacket const* Write() override;

            ObjectGuid Unit;
            int32 Tier = 0;
        };

        class StartMirrorTimer final : public ServerPacket
        {
        public:
            StartMirrorTimer() : ServerPacket(SMSG_START_MIRROR_TIMER, 21) { }
            StartMirrorTimer(int32 timer, int32 value, int32 maxValue, int32 scale, int32 spellID, bool paused) :
                ServerPacket(SMSG_START_MIRROR_TIMER, 21), Scale(scale), MaxValue(maxValue), Timer(timer), SpellID(spellID), Value(value), Paused(paused) { }

            WorldPacket const* Write() override;

            int32 Scale = 0;
            int32 MaxValue = 0;
            int32 Timer = 0;
            int32 SpellID = 0;
            int32 Value = 0;
            bool Paused = false;
        };

        class PauseMirrorTimer final : public ServerPacket
        {
        public:
            PauseMirrorTimer() : ServerPacket(SMSG_PAUSE_MIRROR_TIMER, 5) { }
            PauseMirrorTimer(int32 timer, bool paused) : ServerPacket(SMSG_PAUSE_MIRROR_TIMER, 5), Paused(paused), Timer(timer) { }

            WorldPacket const* Write() override;

            bool Paused = true;
            int32 Timer = 0;
        };

        class StopMirrorTimer final : public ServerPacket
        {
        public:
            StopMirrorTimer() : ServerPacket(SMSG_STOP_MIRROR_TIMER, 4) { }
            StopMirrorTimer(int32 timer) : ServerPacket(SMSG_STOP_MIRROR_TIMER, 4), Timer(timer) { }

            WorldPacket const* Write() override;

            int32 Timer = 0;
        };

        class ExplorationExperience final : public ServerPacket
        {
        public:
            ExplorationExperience() : ServerPacket(SMSG_EXPLORATION_EXPERIENCE, 8) { }
            ExplorationExperience(int32 experience, int32 areaID) : ServerPacket(SMSG_EXPLORATION_EXPERIENCE, 8), Experience(experience), AreaID(areaID) { }

            WorldPacket const* Write() override;

            int32 Experience = 0;
            int32 AreaID = 0;
        };

        class LevelUpInfo final : public ServerPacket
        {
        public:
            LevelUpInfo() : ServerPacket(SMSG_LEVEL_UP_INFO, 60) { }

            WorldPacket const* Write() override;

            int32 Level = 0;
            int32 HealthDelta = 0;
            std::array<int32, MAX_POWERS_PER_CLASS> PowerDelta = { };
            std::array<int32, MAX_STATS> StatDelta = { };
            int32 NumNewTalents = 0;
            int32 NumNewPvpTalentSlots = 0;
        };

        class PlayMusic final : public ServerPacket
        {
        public:
            PlayMusic() : ServerPacket(SMSG_PLAY_MUSIC, 4) { }
            PlayMusic(uint32 soundKitID) : ServerPacket(SMSG_PLAY_MUSIC, 4), SoundKitID(soundKitID) { }

            WorldPacket const* Write() override;

            uint32 SoundKitID = 0;
        };

        class RandomRollClient final : public ClientPacket
        {
        public:
            RandomRollClient(WorldPacket&& packet) : ClientPacket(CMSG_RANDOM_ROLL, std::move(packet)) { }

            void Read() override;

            int32 Min = 0;
            int32 Max = 0;
            uint8 PartyIndex = 0;
        };

        class RandomRoll final : public ServerPacket
        {
        public:
            RandomRoll() : ServerPacket(SMSG_RANDOM_ROLL, 16 + 16 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid Roller;
            ObjectGuid RollerWowAccount;
            int32 Min = 0;
            int32 Max = 0;
            int32 Result = 0;
        };

        class EnableBarberShop final : public ServerPacket
        {
        public:
            EnableBarberShop() : ServerPacket(SMSG_ENABLE_BARBER_SHOP, 1) { }

            WorldPacket const* Write() override;

            uint8 CustomizationScope = 0;
        };

        struct PhaseShiftDataPhase
        {
            uint16 PhaseFlags = 0;
            uint16 Id = 0;
        };

        struct PhaseShiftData
        {
            uint32 PhaseShiftFlags = 0;
            std::vector<PhaseShiftDataPhase> Phases;
            ObjectGuid PersonalGUID;
        };

        class PhaseShiftChange final : public ServerPacket
        {
        public:
            PhaseShiftChange() : ServerPacket(SMSG_PHASE_SHIFT_CHANGE, 16 + 4 + 4 + 16 + 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid Client;
            PhaseShiftData Phaseshift;
            std::vector<uint16> PreloadMapIDs;
            std::vector<uint16> UiMapPhaseIDs;
            std::vector<uint16> VisibleMapIDs;
        };

        class ZoneUnderAttack final : public ServerPacket
        {
        public:
            ZoneUnderAttack() : ServerPacket(SMSG_ZONE_UNDER_ATTACK, 4) { }

            WorldPacket const* Write() override;

            int32 AreaID = 0;
        };

        class DurabilityDamageDeath final : public ServerPacket
        {
        public:
            DurabilityDamageDeath() : ServerPacket(SMSG_DURABILITY_DAMAGE_DEATH, 4) { }

            WorldPacket const* Write() override;

            int32 Percent = 0;
        };

        class ObjectUpdateFailed final : public ClientPacket
        {
        public:
            ObjectUpdateFailed(WorldPacket&& packet) : ClientPacket(CMSG_OBJECT_UPDATE_FAILED, std::move(packet)) { }

            void Read() override;

            ObjectGuid ObjectGUID;
        };

        class ObjectUpdateRescued final : public ClientPacket
        {
        public:
            ObjectUpdateRescued(WorldPacket&& packet) : ClientPacket(CMSG_OBJECT_UPDATE_RESCUED, std::move(packet)) { }

            void Read() override;

            ObjectGuid ObjectGUID;
        };

        class PlayObjectSound final : public ServerPacket
        {
        public:
            PlayObjectSound() : ServerPacket(SMSG_PLAY_OBJECT_SOUND, 16 + 16 + 4 + 4 * 3) { }

            WorldPacket const* Write() override;

            ObjectGuid TargetObjectGUID;
            ObjectGuid SourceObjectGUID;
            int32 SoundKitID = 0;
            TaggedPosition<::Position::XYZ> Position;
            int32 BroadcastTextID = 0;
        };

        class TC_GAME_API PlaySound final : public ServerPacket
        {
        public:
            PlaySound() : ServerPacket(SMSG_PLAY_SOUND, 20) { }
            PlaySound(ObjectGuid sourceObjectGuid, int32 soundKitID, int32 broadcastTextId) : ServerPacket(SMSG_PLAY_SOUND, 20),
                SourceObjectGuid(sourceObjectGuid), SoundKitID(soundKitID), BroadcastTextID(broadcastTextId) { }

            WorldPacket const* Write() override;

            ObjectGuid SourceObjectGuid;
            int32 SoundKitID = 0;
            int32 BroadcastTextID = 0;
        };

        class TC_GAME_API PlaySpeakerbotSound final : public ServerPacket
        {
        public:
            PlaySpeakerbotSound() : ServerPacket(SMSG_PLAY_SPEAKERBOT_SOUND, 20) { }
            PlaySpeakerbotSound(ObjectGuid const& sourceObjectGUID, int32 soundKitID)
                : ServerPacket(SMSG_PLAY_SPEAKERBOT_SOUND, 20), SourceObjectGUID(sourceObjectGUID), SoundKitID(soundKitID) { }

            WorldPacket const* Write() override;

            ObjectGuid SourceObjectGUID;
            int32 SoundKitID = 0;
        };

        class CompleteCinematic final : public ClientPacket
        {
        public:
            CompleteCinematic(WorldPacket&& packet) : ClientPacket(CMSG_COMPLETE_CINEMATIC, std::move(packet)) { }

            void Read() override { }
        };

        class NextCinematicCamera final : public ClientPacket
        {
        public:
            NextCinematicCamera(WorldPacket&& packet) : ClientPacket(CMSG_NEXT_CINEMATIC_CAMERA, std::move(packet)) { }

            void Read() override { }
        };

        class CompleteMovie final : public ClientPacket
        {
        public:
            CompleteMovie(WorldPacket&& packet) : ClientPacket(CMSG_COMPLETE_MOVIE, std::move(packet)) { }

            void Read() override { }
        };

        class FarSight final : public ClientPacket
        {
        public:
            FarSight(WorldPacket&& packet) : ClientPacket(CMSG_FAR_SIGHT, std::move(packet)) { }

            void Read() override;

            bool Enable = false;
        };

        class SaveCUFProfiles final : public ClientPacket
        {
        public:
            SaveCUFProfiles(WorldPacket&& packet) : ClientPacket(CMSG_SAVE_CUF_PROFILES, std::move(packet)) { }

            void Read() override;

            Array<std::unique_ptr<CUFProfile>, MAX_CUF_PROFILES> CUFProfiles;
        };

        class LoadCUFProfiles final : public ServerPacket
        {
        public:
            LoadCUFProfiles() : ServerPacket(SMSG_LOAD_CUF_PROFILES, 20) { }

            WorldPacket const* Write() override;

            std::vector<CUFProfile const*> CUFProfiles;
        };

        class PlayOneShotAnimKit final : public ServerPacket
        {
        public:
            PlayOneShotAnimKit() : ServerPacket(SMSG_PLAY_ONE_SHOT_ANIM_KIT, 7 + 2) { }

            WorldPacket const* Write() override;

            ObjectGuid Unit;
            uint16 AnimKitID = 0;
        };

        class SetAIAnimKit final : public ServerPacket
        {
        public:
            SetAIAnimKit() : ServerPacket(SMSG_SET_AI_ANIM_KIT, 16 + 2) { }

            WorldPacket const* Write() override;

            ObjectGuid Unit;
            uint16 AnimKitID = 0;
        };

        class SetMovementAnimKit final : public ServerPacket
        {
        public:
            SetMovementAnimKit() : ServerPacket(SMSG_SET_MOVEMENT_ANIM_KIT, 16 + 2) { }

            WorldPacket const* Write() override;

            ObjectGuid Unit;
            uint16 AnimKitID = 0;
        };

        class SetMeleeAnimKit final : public ServerPacket
        {
        public:
            SetMeleeAnimKit() : ServerPacket(SMSG_SET_MELEE_ANIM_KIT, 16 + 2) { }

            WorldPacket const* Write() override;

            ObjectGuid Unit;
            uint16 AnimKitID = 0;
        };

        class SetPlayHoverAnim final : public ServerPacket
        {
        public:
            SetPlayHoverAnim() : ServerPacket(SMSG_SET_PLAY_HOVER_ANIM, 16 + 1) { }

            WorldPacket const* Write() override;

            ObjectGuid UnitGUID;
            bool PlayHoverAnim = false;
        };

        class OpeningCinematic final : public ClientPacket
        {
        public:
            OpeningCinematic(WorldPacket&& packet) : ClientPacket(CMSG_OPENING_CINEMATIC, std::move(packet)) { }

            void Read() override { }
        };

        class TogglePvP final : public ClientPacket
        {
        public:
            TogglePvP(WorldPacket&& packet) : ClientPacket(CMSG_TOGGLE_PVP, std::move(packet)) { }

            void Read() override { }
        };

        class SetPvP final : public ClientPacket
        {
        public:
            SetPvP(WorldPacket&& packet) : ClientPacket(CMSG_SET_PVP, std::move(packet)) { }

            void Read() override;

            bool EnablePVP = false;
        };

        class SetWarMode final : public ClientPacket
        {
        public:
            SetWarMode(WorldPacket&& packet) : ClientPacket(CMSG_SET_WAR_MODE, std::move(packet)) { }

            void Read() override;

            bool Enable = false;
        };

        class AccountHeirloomUpdate final : public ServerPacket
        {
        public:
            AccountHeirloomUpdate() : ServerPacket(SMSG_ACCOUNT_HEIRLOOM_UPDATE) { }

            WorldPacket const* Write() override;

            bool IsFullUpdate = false;
            std::map<uint32, HeirloomData> const* Heirlooms = nullptr;
            int32 Unk = 0;
        };

        class MountSpecial final : public ClientPacket
        {
        public:
            MountSpecial(WorldPacket&& packet) : ClientPacket(CMSG_MOUNT_SPECIAL_ANIM, std::move(packet)) { }

            void Read() override;

            Array<int32, 2> SpellVisualKitIDs;
            int32 SequenceVariation = 0;
        };

        class SpecialMountAnim final : public ServerPacket
        {
        public:
            SpecialMountAnim() : ServerPacket(SMSG_SPECIAL_MOUNT_ANIM, 16) { }

            WorldPacket const* Write() override;

            ObjectGuid UnitGUID;
            std::vector<int32> SpellVisualKitIDs;
            int32 SequenceVariation = 0;
        };

        class CrossedInebriationThreshold final : public ServerPacket
        {
        public:
            CrossedInebriationThreshold() : ServerPacket(SMSG_CROSSED_INEBRIATION_THRESHOLD, 16 + 4 + 4) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
            int32 ItemID = 0;
            int32 Threshold = 0;
        };

        class SetTaxiBenchmarkMode final : public ClientPacket
        {
        public:
            SetTaxiBenchmarkMode(WorldPacket&& packet) : ClientPacket(CMSG_SET_TAXI_BENCHMARK_MODE, std::move(packet)) { }

            void Read() override;

            bool Enable = false;
        };

        class OverrideLight final : public ServerPacket
        {
        public:
            OverrideLight() : ServerPacket(SMSG_OVERRIDE_LIGHT, 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            int32 AreaLightID = 0;
            int32 TransitionMilliseconds = 0;
            int32 OverrideLightID = 0;
        };

        class DisplayGameError final : public ServerPacket
        {
        public:
            DisplayGameError(GameError error) : ServerPacket(SMSG_DISPLAY_GAME_ERROR, 4 + 1), Error(error) { }
            DisplayGameError(GameError error, int32 arg) : ServerPacket(SMSG_DISPLAY_GAME_ERROR, 4 + 1 + 4), Error(error), Arg(arg) { }
            DisplayGameError(GameError error, int32 arg1, int32 arg2) : ServerPacket(SMSG_DISPLAY_GAME_ERROR, 4 + 1 + 4 + 4), Error(error), Arg(arg1), Arg2(arg2) { }

            WorldPacket const* Write() override;

            GameError Error;
            Optional<int32> Arg;
            Optional<int32> Arg2;
        };

        class AccountMountUpdate final : public ServerPacket
        {
        public:
            AccountMountUpdate() : ServerPacket(SMSG_ACCOUNT_MOUNT_UPDATE) { }

            WorldPacket const* Write() override;

            bool IsFullUpdate = false;
            MountContainer const* Mounts = nullptr;
        };

        class MountSetFavorite final : public ClientPacket
        {
        public:
            MountSetFavorite(WorldPacket&& packet) : ClientPacket(CMSG_MOUNT_SET_FAVORITE, std::move(packet)) { }

            void Read() override;

            uint32 MountSpellID = 0;
            bool IsFavorite = false;
        };

        class CloseInteraction final : public ClientPacket
        {
        public:
            CloseInteraction(WorldPacket&& packet) : ClientPacket(CMSG_CLOSE_INTERACTION, std::move(packet)) { }

            void Read() override;

            ObjectGuid SourceGuid;
        };

        class StartTimer final : public ServerPacket
        {
        public:
            enum TimerType : int32
            {
                Pvp = 0,
                ChallengeMode = 1,
                PlayerCountdown = 2
            };

            StartTimer() : ServerPacket(SMSG_START_TIMER, 12) { }

            WorldPacket const* Write() override;

            Duration<Seconds> TotalTime;
            Duration<Seconds> TimeLeft;
            TimerType Type = Pvp;
        };

        class ConversationLineStarted final : public ClientPacket
        {
        public:
            ConversationLineStarted(WorldPacket&& packet) : ClientPacket(CMSG_CONVERSATION_LINE_STARTED, std::move(packet)) { }

            void Read() override;

            ObjectGuid ConversationGUID;
            uint32 LineID = 0;
        };

        class RequestLatestSplashScreen final : public ClientPacket
        {
        public:
            RequestLatestSplashScreen(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_LATEST_SPLASH_SCREEN, std::move(packet)) { }

            void Read() override { }
        };

        class SplashScreenShowLatest final : public ServerPacket
        {
        public:
            SplashScreenShowLatest() : ServerPacket(SMSG_SPLASH_SCREEN_SHOW_LATEST, 4) { }

            WorldPacket const* Write() override;

            int32 UISplashScreenID = 0;
        };

        class DisplayToast final : public ServerPacket
        {
        public:
            DisplayToast() : ServerPacket(SMSG_DISPLAY_TOAST) { }

            WorldPacket const* Write() override;

            uint64 Quantity = 0;
            ::DisplayToastMethod DisplayToastMethod = ::DisplayToastMethod::DoNotDisplay;
            bool Mailed = false;
            DisplayToastType Type = DisplayToastType::Money;
            uint32 QuestID = 0;
            bool IsSecondaryResult = false;
            Item::ItemInstance Item;
            bool BonusRoll = false;
            int32 LootSpec = 0;
            ::Gender Gender = GENDER_NONE;
            uint32 CurrencyID = 0;
        };



        // DekkCore >
        class CovenantPreviewOpenNpc  final : public ServerPacket
        {
        public:
            CovenantPreviewOpenNpc() : ServerPacket(SMSG_COVENANT_PREVIEW_OPEN_NPC) { }

            WorldPacket const* Write() override;

            ObjectGuid ObjGUID;
            int32 CovenantId = 0;
        };

        class CovenantCallingAvailibilityResponse  final : public ServerPacket
        {
        public:
            CovenantCallingAvailibilityResponse() : ServerPacket(SMSG_COVENANT_CALLINGS_AVAILABILITY_RESPONSE) { }

            WorldPacket const* Write() override;

            bool Availability = true;
            uint32 Index = 0;
            uint32 Data = 0;
        };

        class CovenantRenownSendCatchUpState  final : public ServerPacket
        {
        public:
            CovenantRenownSendCatchUpState() : ServerPacket(SMSG_COVENANT_RENOWN_SEND_CATCHUP_STATE) { }

            WorldPacket const* Write() override;

            bool CatchupState = false;
        };

        class LegendaryCraftingOpenNpc  final : public ServerPacket
        {
        public:
            LegendaryCraftingOpenNpc() : ServerPacket(SMSG_RUNEFORGE_LEGENDARY_CRAFTING_OPEN_NPC, 16 + 1) { }

            WorldPacket const* Write() override;

            ObjectGuid ObjGUID;
            bool IsUpgrade = false;
        };

        class CovenantRenownRequestCatchupState  final : public ClientPacket
        {
        public:
            CovenantRenownRequestCatchupState(WorldPacket&& packet) : ClientPacket(CMSG_COVENANT_RENOWN_REQUEST_CATCHUP_STATE, std::move(packet)) { }

            void Read() override;
        };

        class RequestCovenantCallings  final : public ClientPacket
        {
        public:
            RequestCovenantCallings(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_COVENANT_CALLINGS, std::move(packet)) { }

            void Read() override;

            bool Availability;
        };

        struct VignetteInstanceIDList
        {
            GuidVector IDs;
        };

        struct VignetteClientData
        {
            VignetteClientData(ObjectGuid guid, Position pos, int32 vignetteID, int32 zoneID) : ObjGUID(guid), Pos(pos), VignetteID(vignetteID), AreaID(zoneID) { }

            ObjectGuid ObjGUID;
            TaggedPosition<Position::XYZ> Pos;
            int32 VignetteID = 0;
            int32 AreaID = 0;
            uint32 Unk901_1;
            uint32 Unk901_2;
        };

        struct VignetteClientDataSet
        {
            VignetteInstanceIDList IdList;
            std::vector<VignetteClientData> Data;
        };

        class VignetteUpdate  final : public ServerPacket
        {
        public:
            VignetteUpdate() : ServerPacket(SMSG_VIGNETTE_UPDATE, 20 + 1) { }
            VignetteUpdate(bool update) : ServerPacket(SMSG_VIGNETTE_UPDATE, 20 + 1), ForceUpdate(update) { }

            WorldPacket const* Write() override;

            VignetteClientDataSet Updated;
            VignetteClientDataSet Added;
            VignetteInstanceIDList Removed;
            bool Unk_Bit_901 = false;
            bool ForceUpdate = true;
        };

    struct TaskProgress
          {
            uint32 TaskID = 0;
            time_t FailureTime = time(0);
            uint32 Flags = 0;
            uint32 Unk = 0;
            std::vector<uint16> Counts;
          };

    class IslandOnQueue final : public ClientPacket
    {
    public:
        IslandOnQueue(WorldPacket&& packet) : ClientPacket(CMSG_ISLAND_QUEUE, std::move(packet)) { }

        void Read() override;

        ObjectGuid QueueNPCGuid;
        int32 ActivityID = 0;
    };

    class IslandAzeriteXpGain final : public ServerPacket
    {
    public:
        IslandAzeriteXpGain() : ServerPacket(SMSG_ISLAND_AZERITE_GAIN, 4 + 36 + 36 + 4) { }

        WorldPacket const* Write() override;

        int32 XpGain = 0;
        ObjectGuid PlayerGuid;
        ObjectGuid SourceGuid;
        int32 SourceID = 0;
    };

    class IslandCompleted final : public ServerPacket
    {
    public:
        IslandCompleted() : ServerPacket(SMSG_ISLAND_COMPLETE) { }

        WorldPacket const* Write() override;

        int32 MapID = 0;
        int32 Winner = 0;
        //uint32 GroupTeamSize = 0; DisplayInfos.size()

        //WorldPackets::Inspect::PlayerModelDisplayInfo::Initialize(Player const* player)
        std::vector<WorldPackets::Inspect::PlayerModelDisplayInfo> DisplayInfos;
    };

    class FactionSelectUI final : public ServerPacket
    {
    public:
        FactionSelectUI() : ServerPacket(SMSG_SHOW_NEUTRAL_PLAYER_FACTION_SELECT_UI, 0) { }

        WorldPacket const* Write() override { return &_worldPacket; }
    };

    class FactionSelect final : public ClientPacket
    {
    public:
        FactionSelect(WorldPacket&& packet) : ClientPacket(CMSG_NEUTRAL_PLAYER_SELECT_FACTION, std::move(packet)) { }

        void Read() override;

        uint32 FactionChoice = 0;
    };

    class StreamingMovie final : public ServerPacket
    {
    public:
        StreamingMovie() : ServerPacket(SMSG_STREAMING_MOVIES, 4) { }

        WorldPacket const* Write() override;

        std::vector<int16> MovieIDs;
    };

    class ArchaeologySurveryCast final : public ServerPacket
    {
    public:
        ArchaeologySurveryCast() : ServerPacket(SMSG_ARCHAEOLOGY_SURVERY_CAST, 13) { }

        WorldPacket const* Write() override;

        int32 ResearchBranchID = 0;
        uint32 TotalFinds = 0;
        uint32 NumFindsCompleted = 0;
        bool SuccessfulFind = false;
    };

        struct ModeAttempt
        {
            struct Member
            {
                ObjectGuid Guid;
                uint32 VirtualRealmAddress = 0;
                uint32 NativeRealmAddress = 0;
                uint32 SpecializationID = 0;
            };

            uint32 InstanceRealmAddress = 0;
            uint32 AttemptID = 0;
            uint32 CompletionTime = 0;
            time_t CompletionDate = time(nullptr);
            uint32 MedalEarned = 0;
            std::vector<Member> Members;
        };

        struct ChallengeModeMap
        {
            struct bMember
            {
                ObjectGuid PlayerGuid;
                ObjectGuid GuildGuid;
                uint32 VirtualRealmAddress = 0;
                uint32 NativeRealmAddress = 0;
                uint32 SpecializationID = 0;
                uint32 Unk4 = 0;
                uint32 EquipmentLevel = 0;
            };

            uint32 MapId = 0;
            uint32 BestCompletionMilliseconds = 0;
            uint32 LastCompletionMilliseconds = 0;
            uint32 CompletedChallengeLevel = 0;
            uint32 ChallengeID = 0;
            time_t LastMedalDate = time(nullptr);
            std::vector<uint16> BestSpecID;
            std::array<uint32, 4> Affixes;
            std::vector<bMember> Members;
            time_t MedalDate = time(nullptr);
        };

    class RequestLeadersResult final : public ServerPacket
    {
    public:
        RequestLeadersResult() : ServerPacket(SMSG_CHALLENGE_MODE_REQUEST_LEADERS_RESULT, 20 + 8) { }

        WorldPacket const* Write() override;

        uint32 MapID = 0;
        uint32 ChallengeID = 0;
        time_t LastGuildUpdate = time(nullptr);
        time_t LastRealmUpdate = time(nullptr);
        uint32 GuildLeadersCount = 0;
        uint32 RealmLeadersCount = 0;
        std::vector<ModeAttempt> GuildLeaders;
        std::vector<ModeAttempt> RealmLeaders;
    };

    class NewPlayerRecord final : public ServerPacket
    {
    public:
        NewPlayerRecord() : ServerPacket(SMSG_MYTHIC_PLUS_NEW_WEEK_RECORD, 4) { }

        WorldPacket const* Write() override;

        int32 MapID = 0;
        int32 CompletionMilliseconds = 0;
        uint32 ChallengeLevel = 0;
    };

    class Complete final : public ServerPacket
    {
    public:
        Complete() : ServerPacket(SMSG_CHALLENGE_MODE_COMPLETE, 4) { }

        WorldPacket const* Write() override;

        uint32 MapId = 0;
        uint32 CompletionMilliseconds = 0;
        uint32 ChallengeLevel = 0;
        uint32 ChallengeId = 0;
        uint8 IsCompletedInTimer = 128;
    };

    class Reset final : public ServerPacket
    {
    public:
        Reset(uint32 mapID) : ServerPacket(SMSG_CHALLENGE_MODE_RESET, 4), MapID(mapID) { }

        WorldPacket const* Write() override;

        uint32 MapID = 0;
    };

    class ResetChallengeMode final : public ClientPacket //OK
    {
    public:
        ResetChallengeMode(WorldPacket&& packet) : ClientPacket(CMSG_RESET_CHALLENGE_MODE, std::move(packet)) { }

        void Read() override { }
    };

    class ResetChallengeModeCheat final : public ClientPacket //OK
    {
    public:
        ResetChallengeModeCheat(WorldPacket&& packet) : ClientPacket(CMSG_RESET_CHALLENGE_MODE_CHEAT, std::move(packet)) { }

        void Read() override { }
    };

    class Start final : public ServerPacket
    {
    public:
        Start() : ServerPacket(SMSG_CHALLENGE_MODE_START) { }

        WorldPacket const* Write() override;

        int32 MapID = 0;
        int32 ChallengeID = 0;
        int32 ChallengeLevel = 0;
        int32 DeathCount = 0;

        int32 Affixes1 = 0;
        int32 Affixes2 = 0;
        int32 Affixes3 = 0;
        int32 Affixes4 = 0;

        int32 ClientEncounterStartPlayerInfo = 0;

        uint8 Energized = 128;
    };

    class UpdateDeathCount final : public ServerPacket
    {
    public:
        UpdateDeathCount() : ServerPacket(SMSG_CHALLENGE_MODE_UPDATE_DEATH_COUNT, 4) { }

        WorldPacket const* Write() override;

        uint32 DeathCount = 0;
    };

    class ChangePlayerDifficultyResult final : public ServerPacket
    {
    public:
        ChangePlayerDifficultyResult(uint32 type = 0) : ServerPacket(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT, 4), Type(type) { }

        WorldPacket const* Write() override;

        uint8 Type = 0;
        uint32 InstanceDifficultyID = 0;
        uint32 DifficultyRecID = 0;
    };

    class StopElapsedTimer final : public ServerPacket
    {
    public:
        StopElapsedTimer() : ServerPacket(SMSG_STOP_ELAPSED_TIMER, 5) { }

        WorldPacket const* Write() override;

        int32 TimerID = 0;
        bool KeepTimer = false;
    };

    struct ElaspedTimer
    {
        ElaspedTimer() { }
        ElaspedTimer(uint32 timerID, time_t currentDuration) : TimerID(timerID), CurrentDuration(currentDuration) { }

        uint32 TimerID = 0;
        time_t CurrentDuration = time_t(0);
    };

    class StartElapsedTimer final : public ServerPacket
    {
    public:
        StartElapsedTimer() : ServerPacket(SMSG_START_ELAPSED_TIMER, 8) { }

        WorldPacket const* Write() override;

        ElaspedTimer Timer;
    };

    class StartElapsedTimers final : public ServerPacket
    {
    public:
        StartElapsedTimers() : ServerPacket(SMSG_START_ELAPSED_TIMERS, 4) { }

        WorldPacket const* Write() override;

        std::vector<ElaspedTimer> Timers;
    };

    class Misc final : public ClientPacket
    {
    public:
        Misc(WorldPacket&& packet) : ClientPacket(std::move(packet)) { }

        void Read() override { }
    };

    class StartRequest final : public ClientPacket
    {
    public:
        StartRequest(WorldPacket&& packet) : ClientPacket(CMSG_START_CHALLENGE_MODE, std::move(packet)) { }

        void Read() override;

        uint8 Bag = 0;
        uint32 Slot = 0;
        ObjectGuid GameObjectGUID;
    };

    class StartChallengeMode final : public ClientPacket
    {
    public:
        StartChallengeMode(WorldPacket&& packet) : ClientPacket(CMSG_START_CHALLENGE_MODE, std::move(packet)) { }

        void Read() override;

        ObjectGuid GameObjectGUID;
        uint8 Bag = 0;
        uint32 Slot = 0;
    };

    class RequestLeaders final : public ClientPacket //tested
    {
    public:
        RequestLeaders(WorldPacket&& packet) : ClientPacket(CMSG_CHALLENGE_MODE_REQUEST_LEADERS, std::move(packet)) { }

        void Read() override;

        time_t LastGuildUpdate = time(nullptr);
        time_t LastRealmUpdate = time(nullptr);
        int32 MapId = 0;
        int32 ChallengeID = 0;
    };

    class claimweeklyrewards final : public ClientPacket
    {
    public:
        claimweeklyrewards(WorldPacket&& packet) : ClientPacket(CMSG_CLAIM_WEEKLY_REWARD, std::move(packet)) { }

        void Read() override;

        int32 UNK = 0;
    };

    class WarfrontComplete final : public ServerPacket
    {
    public:
        WarfrontComplete() : ServerPacket(SMSG_WARFRONT_COMPLETE) { }

        WorldPacket const* Write() override;

        uint32 Unk1;
    };

    class SetMaxWeeklyQuantity final : public ServerPacket
    {
    public:
        SetMaxWeeklyQuantity() : ServerPacket(SMSG_SET_MAX_WEEKLY_QUANTITY, 8) { }

        WorldPacket const* Write() override;

        uint32 Type = 0;
        int32 MaxWeeklyQuantity = 0;
    };

    class OpenContainer final : public ServerPacket
    {
    public:
        OpenContainer() : ServerPacket(SMSG_OPEN_CONTAINER) { }

        WorldPacket const* Write() override;

        ObjectGuid Guid;
    };

    class MapObjEvents final : public ServerPacket
    {
    public:
        MapObjEvents() : ServerPacket(SMSG_MAP_OBJ_EVENTS, 12) { }

        WorldPacket const* Write() override;

        uint32 UniqueID = 0;
        uint32 DataSize = 0;
        std::vector<uint8> Unk2;
    };

    class EndLightningStorm final : public ServerPacket
    {
    public:
        EndLightningStorm() : ServerPacket(SMSG_END_LIGHTNING_STORM) { }

        WorldPacket const* Write() override;

        uint32 LightningStormId = 0;
    };

    class ClubFinderResponseCharacterApplicationList final : public ServerPacket
    {
    public:
        ClubFinderResponseCharacterApplicationList() : ServerPacket(SMSG_CLUB_FINDER_RESPONSE_CHARACTER_APPLICATION_LIST) { }

        WorldPacket const* Write() override;

        bool UNK = false;
        uint8 UNK1;
        uint8 UNK2;
        uint8 UNK3;
        uint8 UNK4;
    };

    class ShowTradeSkillResponse final : public ServerPacket
    {
    public:
        ShowTradeSkillResponse() : ServerPacket(SMSG_SHOW_TRADE_SKILL_RESPONSE, 16 + 4 + 12) { }

        WorldPacket const* Write() override;

        ObjectGuid PlayerGUID;
        uint32 SpellId = 0;
        std::vector<int32> SkillLineIDs;
        std::vector<int32> SkillRanks;
        std::vector<int32> SkillMaxRanks;
        std::vector<int32> KnownAbilitySpellIDs;
     };

    class PlayerSkinned final : public ServerPacket
    {
    public:
        PlayerSkinned() : ServerPacket(SMSG_PLAYER_SKINNED, 1) { }

        WorldPacket const* Write() override;

        bool FreeRepop = false;
    };

    class ContributionCollectorContribute final : public ClientPacket
    {
    public:
        ContributionCollectorContribute(WorldPacket&& packet) : ClientPacket(CMSG_CONTRIBUTION_CONTRIBUTE, std::move(packet)) { }

        void Read() override;

        ObjectGuid ContributionTableNpcGuid;
        uint32 OrderIndex = 0;
    };

    class ContributionLastupdaterequest final : public ClientPacket
    {
    public:
        ContributionLastupdaterequest(WorldPacket&& packet) : ClientPacket(CMSG_CONTRIBUTION_LAST_UPDATE_REQUEST, std::move(packet)) { }

        void Read() override;

        int32 unk1 = 0;
        int32 unk2 = 0;
    };

    class ContributionResponse final : public ServerPacket
    {
    public:
        ContributionResponse() : ServerPacket(SMSG_CONTRIBUTION_LAST_UPDATE_RESPONSE, 12) { }

        WorldPacket const* Write() override;

        uint32 Data = 0;
        uint32 ContributionID = 0;
        uint32 ContributionGUID = 0;
    };

    struct ServerSpec
    {
        uint32 Realm;
        uint16 Server;
        uint8 Type;
    };

    struct CommentatorSpellChargeEntry
    {
        uint32 Unk;
        uint32 Unk2;
    };

    struct SpellChargeEntry
    {
        uint32 Unk;
        uint32 Unk2;
        uint32 Unk3;
        uint32 Unk4;
    };

    struct SpellHistoryEntry
    {
        uint32 Unk;
        uint32 Unk2;
        uint32 Unk3;
        uint32 Unk4;
        uint32 Unk5;
        bool Unk6;
        bool Unk7;
        bool Unk8;
    };

    struct CommentatorPlayer
    {
        ObjectGuid Guid;
        ServerSpec UserServer;
    };

    struct CommentatorPlayerData
    {
        ObjectGuid PlayerGUID;
        uint32 DamageDone;
        uint32 DamageTaken;
        uint32 HealingDone;
        uint32 HealingTaken;
        uint32 SpecID;
        uint16 Kills;
        uint16 Deaths;
        uint8 FactionIndex;
        std::list<CommentatorSpellChargeEntry> CommentatorSpellChargeEntries;
        std::list<SpellChargeEntry> SpellChargeEntries;
        std::list<SpellHistoryEntry> SpellHistoryEntries;
    };

    struct CommentatorTeam
    {
        ObjectGuid Guid;
        std::list<CommentatorPlayer> Players;
    };

    struct CommentatorInstance
    {
        uint32 MapID;
        ServerSpec WorldServer;
        uint64 InstanceID;
        uint32 Status;
        CommentatorTeam Teams[2];
    };

    struct CommentatorMap
    {
        uint32 TeamSize;
        uint32 MinLevelRange;
        uint32 MaxLevelRange;
        int32 DifficultyID;
        std::list<CommentatorInstance> Instances;
    };

    class CommentatorStateChanged final : public ServerPacket
    {
    public:
        CommentatorStateChanged() : ServerPacket(SMSG_COMMENTATOR_STATE_CHANGED, 16 + 1) { }

        WorldPacket const* Write() override;

        ObjectGuid Guid;
        bool Enable;
    };

    class CommentatorPlayerInfo final : public ServerPacket
    {
    public:
        CommentatorPlayerInfo() : ServerPacket(SMSG_COMMENTATOR_PLAYER_INFO, 16 + 1) { }

        WorldPacket const* Write() override;

        uint32 MapID;
        std::list<CommentatorPlayerData> PlayerInfo;
        uint64 InstanceID;
        ServerSpec WorldServer;
    };

    class CommentatorMapInfo final : public ServerPacket
    {
    public:
        CommentatorMapInfo() : ServerPacket(SMSG_COMMENTATOR_MAP_INFO, 16 + 1) { }

        WorldPacket const* Write() override;

        uint64 PlayerInstanceID;
        std::list<CommentatorMap> Maps;
    };

    class CommentatorExitInstance final : public ClientPacket
    {
    public:
        CommentatorExitInstance(WorldPacket&& packet) : ClientPacket(CMSG_COMMENTATOR_EXIT_INSTANCE, std::move(packet)) { }

        void Read() override {}
    };

    class CommentatorEnterInstance final : public ClientPacket
    {
    public:
        CommentatorEnterInstance(WorldPacket&& packet) : ClientPacket(CMSG_COMMENTATOR_ENTER_INSTANCE, std::move(packet)) { }

        void Read() override;

        uint32 Enable;
    };

    class CommentatorGetPlayerInfo final : public ClientPacket
    {
    public:
        CommentatorGetPlayerInfo(WorldPacket&& packet) : ClientPacket(CMSG_COMMENTATOR_GET_PLAYER_INFO, std::move(packet)) { }

        void Read() override;

        ServerSpec WorldServer;
        uint32 MapID;
    };

    class CommentatorGetMapInfo final : public ClientPacket
    {
    public:
        CommentatorGetMapInfo(WorldPacket&& packet) : ClientPacket(CMSG_COMMENTATOR_GET_MAP_INFO, std::move(packet)) { }

        void Read() override;

        std::string PlayerName;
    };

    class CommentatorEnable final : public ClientPacket
    {
    public:
        CommentatorEnable(WorldPacket&& packet) : ClientPacket(CMSG_COMMENTATOR_ENABLE, std::move(packet)) { }

        void Read() override;

        uint32 Enable;
    };

    class WeeklyRewardClaimResult final : public ServerPacket
    {
    public:
        WeeklyRewardClaimResult() : ServerPacket(SMSG_WEEKLY_REWARD_CLAIM_RESULT) { }

        WorldPacket const* Write() override;

        int32 Result = 0;
    };

    class ActivateSoulbind final : public ClientPacket
    {
    public:
        ActivateSoulbind(WorldPacket&& packet) : ClientPacket(CMSG_ACTIVATE_SOULBIND, std::move(packet)) { }

        void Read() override;

        uint32 CovenantID;
    };

    class PerksProgramAcitivtyUpdate final : public ServerPacket
    {
    public: PerksProgramAcitivtyUpdate() :ServerPacket(SMSG_PERKS_PROGRAM_ACTIVITY_UPDATE) { }

          WorldPacket const* Write() override;

          int32 ActivityID;
          std::vector<uint32> ActivityCount;         
    };

    class Playerchoicedisplayerror final : public ServerPacket
    {
    public: Playerchoicedisplayerror() :ServerPacket(SMSG_PLAYER_CHOICE_DISPLAY_ERROR) { }

          WorldPacket const* Write() override;

          uint32 choiceid; //maybe
    };

    class ClearTreasurePickerCache final : public ServerPacket
    {
    public: ClearTreasurePickerCache() :ServerPacket(SMSG_CLEAR_TREASURE_PICKER_CACHE) { }

          WorldPacket const* Write() override;

          uint32 treasurepickerid; //maybe
    };

    class AccountCosmeticAdded final : public ServerPacket
    {
    public: AccountCosmeticAdded() :ServerPacket(SMSG_ACCOUNT_COSMETIC_ADDED) { }

          WorldPacket const* Write() override;

          uint32 UNK1; 
    };

    class ActivateSoulbindFailed final : public ServerPacket
    {
    public: ActivateSoulbindFailed() :ServerPacket(SMSG_ACTIVATE_SOULBIND_FAILED) { }

          WorldPacket const* Write() override;

          uint8 unk;
          uint32 CovenantID;
    };

    class ConversationCinematicReady final : public ClientPacket
    {
    public:
        ConversationCinematicReady(WorldPacket&& packet) : ClientPacket(CMSG_CONVERSATION_CINEMATIC_READY, std::move(packet)) { }

        void Read() override;

        ObjectGuid ConversationGUID;
    };

    class PerksProgramReqestPendingRewards final : public ClientPacket
    {
    public:
        PerksProgramReqestPendingRewards(WorldPacket&& packet) : ClientPacket(CMSG_PERKS_PROGRAM_REQUEST_PENDING_REWARDS, std::move(packet)) { }

        void Read() override {}
    };

    class OverrideScreenFlash final : public ClientPacket
    {
    public:
        OverrideScreenFlash(WorldPacket&& packet) : ClientPacket(CMSG_OVERRIDE_SCREEN_FLASH, std::move(packet)) { }

        void Read() override;

        bool BlackScreenOrRedScreen;
    };

    class PlayerChoiceClear final : public ServerPacket
    {
    public: PlayerChoiceClear() :ServerPacket(SMSG_PLAYER_CHOICE_CLEAR) { }

          WorldPacket const* Write() override;

          int32 ChoiceID;
          bool Status;
    };

    class PerksProgramActivityComplete final : public ServerPacket
    {
    public: PerksProgramActivityComplete() :ServerPacket(SMSG_PERKS_PROGRAM_ACTIVITY_COMPLETE) { }

          WorldPacket const* Write() override;

          int32 ActivityID;
    };

    class ApplyMountEquipmentResult final : public ServerPacket
    {
    public: ApplyMountEquipmentResult() :ServerPacket(SMSG_APPLY_MOUNT_EQUIPMENT_RESULT) { }

          WorldPacket const* Write() override;

          ObjectGuid PlayerGuid;
          int32 Unk1;
          uint8 unk;
    };

    class GainMawPower final : public ServerPacket
    {
    public: GainMawPower() :ServerPacket(SMSG_GAIN_MAW_POWER) { }

          WorldPacket const* Write() override;

          ObjectGuid PlayerGuid;
          int32 Power;
    };

    class MultiFloorLeaveFloor final : public ServerPacket
    {
    public: MultiFloorLeaveFloor() :ServerPacket(SMSG_MULTI_FLOOR_LEAVE_FLOOR) { }

          WorldPacket const* Write() override;

          int32 unk1;
          int32 unk2;
          int32 unk3;
          uint8 unk4;
    };

    class ProfessionGossip final : public ServerPacket
    {
    public: ProfessionGossip() :ServerPacket(SMSG_PROFESSION_GOSSIP) { }

          WorldPacket const* Write() override;

          ObjectGuid NpcGUID;
          int32 unk1;
          int32 unk2;
    };

    class AbandonNpeResponse final : public ClientPacket
    {
    public:
        AbandonNpeResponse(WorldPacket&& packet) : ClientPacket(CMSG_ABANDON_NPE_RESPONSE, std::move(packet)) { }

        void Read() override;

        uint8 UNK;
    };

    class AcceptReturningPlayerPrompt final : public ClientPacket
    {
    public:
        AcceptReturningPlayerPrompt(WorldPacket&& packet) : ClientPacket(CMSG_ACCEPT_RETURNING_PLAYER_PROMPT, std::move(packet)) { }

        void Read() override {}
    };

    class AcceptSocialContract final : public ClientPacket
    {
    public:
        AcceptSocialContract(WorldPacket&& packet) : ClientPacket(CMSG_ACCEPT_SOCIAL_CONTRACT, std::move(packet)) { }

        void Read() override {}
    };

    class AddAccountCosmetic final : public ClientPacket
    {
    public:
        AddAccountCosmetic(WorldPacket&& packet) : ClientPacket(CMSG_ADD_ACCOUNT_COSMETIC, std::move(packet)) { }

        void Read() override;

        ObjectGuid Playerguid;
    };

    class ClearNewAppearance final : public ClientPacket
    {
    public:
        ClearNewAppearance(WorldPacket&& packet) : ClientPacket(CMSG_CLEAR_NEW_APPEARANCE, std::move(packet)) { }

        void Read() override;

        int32 unk;
    };

    class AccountNotificationAcknowledge final : public ClientPacket
    {
    public:
        AccountNotificationAcknowledge(WorldPacket&& packet) : ClientPacket(CMSG_ACCOUNT_NOTIFICATION_ACKNOWLEDGED, std::move(packet)) { }

        void Read() override;

        int64 unk;
        int32 unk2;
        int32 unk3;
    };

    class AuctionableTokenSell final : public ClientPacket
    {
    public:
        AuctionableTokenSell(WorldPacket&& packet) : ClientPacket(CMSG_AUCTIONABLE_TOKEN_SELL, std::move(packet)) { }

        void Read() override;

        int64 unkint64;
        int64 CurrentMarketPrice;
        int32 UnkInt32;
    };

    class AuctionableTokenSellAtMarketPrice final : public ClientPacket
    {
    public:
        AuctionableTokenSellAtMarketPrice(WorldPacket&& packet) : ClientPacket(CMSG_AUCTIONABLE_TOKEN_SELL_AT_MARKET_PRICE, std::move(packet)) { }

        void Read() override;

        ObjectGuid TokenGuid;
        uint32 UnkInt32;
        uint32 PendingBuyConfirmations;
        uint64 GuaranteedPrice;
        bool confirmed;
    };

    class BonusRoll final : public ClientPacket
    {
    public:
        BonusRoll(WorldPacket&& packet) : ClientPacket(CMSG_BONUS_ROLL, std::move(packet)) { }

        void Read() override {}
    };

    class CanRedeemTokenForBalance final : public ClientPacket
    {
    public:
        CanRedeemTokenForBalance(WorldPacket&& packet) : ClientPacket(CMSG_CAN_REDEEM_TOKEN_FOR_BALANCE, std::move(packet)) { }

        void Read() override;

        int32 UnkInt32;
    };

    class ChangeBagSlotFlag final : public ClientPacket
    {
    public:
        ChangeBagSlotFlag(WorldPacket&& packet) : ClientPacket(CMSG_CHANGE_BAG_SLOT_FLAG, std::move(packet)) { }

        void Read() override;

        int32 UnkInt;
        int32 UnkInt32;
        bool unknown;
    };

    class ChangeBankBagSlotFlag final : public ClientPacket
    {
    public:
        ChangeBankBagSlotFlag(WorldPacket&& packet) : ClientPacket(CMSG_CHANGE_BANK_BAG_SLOT_FLAG, std::move(packet)) { }

        void Read() override;

        int32 UnkInt;
        int32 UnkInt32;
        bool unknown;
    };

    class CloseRuneforgeInteraction final : public ClientPacket
    {
    public:
        CloseRuneforgeInteraction(WorldPacket&& packet) : ClientPacket(CMSG_CLOSE_RUNEFORGE_INTERACTION, std::move(packet)) { }

        void Read() override {}
    };

    class CommerceTokenGetCount final : public ClientPacket
    {
    public:
        CommerceTokenGetCount(WorldPacket&& packet) : ClientPacket(CMSG_COMMERCE_TOKEN_GET_COUNT, std::move(packet)) { }

        void Read() override;

        int32 count;
    };

    class ContributionLastUpdateRequest final : public ClientPacket
    {
    public:
        ContributionLastUpdateRequest(WorldPacket&& packet) : ClientPacket(CMSG_CONTRIBUTION_LAST_UPDATE_REQUEST, std::move(packet)) { }

        void Read() override;

        int32 ContributionId;
        int32 count;
    };

    class UsedFollow final : public ClientPacket
    {
    public:
        UsedFollow(WorldPacket&& packet) : ClientPacket(CMSG_USED_FOLLOW, std::move(packet)) { }

        void Read() override {}
    };

    class UpgradeRuneforgeLegendary final : public ClientPacket
    {
    public:
        UpgradeRuneforgeLegendary(WorldPacket&& packet) : ClientPacket(CMSG_UPGRADE_RUNEFORGE_LEGENDARY, std::move(packet)) { }

        void Read() override;

        int32 ItemID;
        uint8 UpgradeType;
        uint8 UpgradeLevel;
        uint8 Cost;
        uint8 RemainingUpgradeLevel;
    };

    class ShowTradeSkill final : public ClientPacket
    {
    public:
        ShowTradeSkill(WorldPacket&& packet) : ClientPacket(CMSG_SHOW_TRADE_SKILL, std::move(packet)) { }

        void Read() override;

        ObjectGuid PlayerGUID;
        uint32 SpellID = 0;
        uint32 SkillLineID = 0;
    };

    class QuickJoinSignalToastDisplayed final : public ClientPacket
    {
    public:
        QuickJoinSignalToastDisplayed(WorldPacket&& packet) : ClientPacket(CMSG_QUICK_JOIN_SIGNAL_TOAST_DISPLAYED, std::move(packet)) { }

        void Read() override;

        std::vector<ObjectGuid> UnkGuids;
        ObjectGuid GroupGUID;
        float unk = 0.0f;;
        uint32 Priority = 0;
        bool UnkBit1 = false;
    };
    // < DekkCore           
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::ChallengeModeMap const& challengeModeMap);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::ModeAttempt const& modeAttempt);
#endif // MiscPackets_h__
