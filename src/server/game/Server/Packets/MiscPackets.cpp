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

#include "MiscPackets.h"
#include "Common.h"
#include "InspectPackets.h"

WorldPacket const* WorldPackets::Misc::BindPointUpdate::Write()
{
    _worldPacket << BindPosition;
    _worldPacket << uint32(BindMapID);
    _worldPacket << uint32(BindAreaID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::InvalidatePlayer::Write()
{
    _worldPacket << Guid;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::LoginSetTimeSpeed::Write()
{
    _worldPacket.AppendPackedTime(ServerTime);
    _worldPacket.AppendPackedTime(GameTime);
    _worldPacket << float(NewSpeed);
    _worldPacket << uint32(ServerTimeHolidayOffset);
    _worldPacket << uint32(GameTimeHolidayOffset);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::SetCurrency::Write()
{
    _worldPacket << int32(Type);
    _worldPacket << int32(Quantity);
    _worldPacket << uint32(Flags);
    _worldPacket << uint32(Toasts.size());

    for (WorldPackets::Item::UiEventToast const& toast : Toasts)
        _worldPacket << toast;

    _worldPacket.WriteBit(WeeklyQuantity.has_value());
    _worldPacket.WriteBit(TrackedQuantity.has_value());
    _worldPacket.WriteBit(MaxQuantity.has_value());
    _worldPacket.WriteBit(TotalEarned.has_value());
    _worldPacket.WriteBit(SuppressChatLog);
    _worldPacket.WriteBit(QuantityChange.has_value());
    _worldPacket.WriteBit(QuantityGainSource.has_value());
    _worldPacket.WriteBit(QuantityLostSource.has_value());
    _worldPacket.WriteBit(FirstCraftOperationID.has_value());
    _worldPacket.WriteBit(NextRechargeTime.has_value());
    _worldPacket.WriteBit(RechargeCycleStartTime.has_value());
    _worldPacket.FlushBits();

    if (WeeklyQuantity)
        _worldPacket << int32(*WeeklyQuantity);

    if (TrackedQuantity)
        _worldPacket << int32(*TrackedQuantity);

    if (MaxQuantity)
        _worldPacket << int32(*MaxQuantity);

    if (TotalEarned)
        _worldPacket << int32(*TotalEarned);

    if (QuantityChange)
        _worldPacket << int32(*QuantityChange);

    if (QuantityGainSource)
        _worldPacket << int32(*QuantityGainSource);

    if (QuantityLostSource)
        _worldPacket << int32(*QuantityLostSource);

    if (FirstCraftOperationID)
        _worldPacket << uint32(*FirstCraftOperationID);

    if (NextRechargeTime)
        _worldPacket << *NextRechargeTime;

    if (RechargeCycleStartTime)
        _worldPacket << *RechargeCycleStartTime;

    return &_worldPacket;
}

void WorldPackets::Misc::SetSelection::Read()
{
    _worldPacket >> Selection;
}

WorldPacket const* WorldPackets::Misc::SetupCurrency::Write()
{
    _worldPacket << uint32(Data.size());

    for (Record const& data : Data)
    {
        _worldPacket << int32(data.Type);
        _worldPacket << int32(data.Quantity);

        _worldPacket.WriteBit(data.WeeklyQuantity.has_value());
        _worldPacket.WriteBit(data.MaxWeeklyQuantity.has_value());
        _worldPacket.WriteBit(data.TrackedQuantity.has_value());
        _worldPacket.WriteBit(data.MaxQuantity.has_value());
        _worldPacket.WriteBit(data.TotalEarned.has_value());
        _worldPacket.WriteBit(data.NextRechargeTime.has_value());
        _worldPacket.WriteBit(data.RechargeCycleStartTime.has_value());
        _worldPacket.WriteBits(uint8(data.Flags), 5);
        _worldPacket.FlushBits();

        if (data.WeeklyQuantity)
            _worldPacket << uint32(*data.WeeklyQuantity);
        if (data.MaxWeeklyQuantity)
            _worldPacket << uint32(*data.MaxWeeklyQuantity);
        if (data.TrackedQuantity)
            _worldPacket << uint32(*data.TrackedQuantity);
        if (data.MaxQuantity)
            _worldPacket << int32(*data.MaxQuantity);
        if (data.TotalEarned)
            _worldPacket << int32(*data.TotalEarned);
        if (data.NextRechargeTime)
            _worldPacket << *data.NextRechargeTime;
        if (data.RechargeCycleStartTime)
            _worldPacket << *data.RechargeCycleStartTime;
    }

    return &_worldPacket;
}

void WorldPackets::Misc::ViolenceLevel::Read()
{
    _worldPacket >> ViolenceLvl;
}

WorldPacket const* WorldPackets::Misc::TimeSyncRequest::Write()
{
    _worldPacket << SequenceIndex;

    return &_worldPacket;
}

void WorldPackets::Misc::TimeSyncResponse::Read()
{
    _worldPacket >> SequenceIndex;
    _worldPacket >> ClientTime;
}

WorldPacket const* WorldPackets::Misc::ServerTimeOffset::Write()
{
    _worldPacket << Time;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::TriggerMovie::Write()
{
    _worldPacket << uint32(MovieID);

    return &_worldPacket;
}
WorldPacket const* WorldPackets::Misc::TriggerCinematic::Write()
{
    _worldPacket << uint32(CinematicID);
    _worldPacket << ConversationGuid;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::TutorialFlags::Write()
{
    _worldPacket.append(TutorialData, MAX_ACCOUNT_TUTORIAL_VALUES);

    return &_worldPacket;
}

void WorldPackets::Misc::TutorialSetFlag::Read()
{
    Action = _worldPacket.ReadBits(2);

    if (Action == TUTORIAL_ACTION_UPDATE)
        _worldPacket >> TutorialBit;
}

WorldPacket const* WorldPackets::Misc::WorldServerInfo::Write()
{
    _worldPacket << uint32(DifficultyID);
    _worldPacket.WriteBit(IsTournamentRealm);
    _worldPacket.WriteBit(XRealmPvpAlert);
    _worldPacket.WriteBit(BlockExitingLoadingScreen);
    _worldPacket.WriteBit(RestrictedAccountMaxLevel.has_value());
    _worldPacket.WriteBit(RestrictedAccountMaxMoney.has_value());
    _worldPacket.WriteBit(InstanceGroupSize.has_value());

    if (RestrictedAccountMaxLevel)
        _worldPacket << uint32(*RestrictedAccountMaxLevel);

    if (RestrictedAccountMaxMoney)
        _worldPacket << uint64(*RestrictedAccountMaxMoney);

    if (InstanceGroupSize)
        _worldPacket << uint32(*InstanceGroupSize);

    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Misc::SetDungeonDifficulty::Read()
{
    _worldPacket >> DifficultyID;
}

void WorldPackets::Misc::SetRaidDifficulty::Read()
{
    _worldPacket >> DifficultyID;
    _worldPacket >> Legacy;
}

WorldPacket const* WorldPackets::Misc::DungeonDifficultySet::Write()
{
    _worldPacket << int32(DifficultyID);
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::RaidDifficultySet::Write()
{
    _worldPacket << int32(DifficultyID);
    _worldPacket << uint8(Legacy);
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::CorpseReclaimDelay::Write()
{
    _worldPacket << Remaining;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::DeathReleaseLoc::Write()
{
    _worldPacket << MapID;
    _worldPacket << Loc;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PreRessurect::Write()
{
    _worldPacket << PlayerGUID;

    return &_worldPacket;
}

void WorldPackets::Misc::ReclaimCorpse::Read()
{
    _worldPacket >> CorpseGUID;
}

void WorldPackets::Misc::RepopRequest::Read()
{
    CheckInstance = _worldPacket.ReadBit();
}

WorldPacket const* WorldPackets::Misc::RequestCemeteryListResponse::Write()
{
    _worldPacket.WriteBit(IsGossipTriggered);
    _worldPacket.FlushBits();

    _worldPacket << uint32(CemeteryID.size());
    for (uint32 cemetery : CemeteryID)
        _worldPacket << cemetery;

    return &_worldPacket;
}

void WorldPackets::Misc::ResurrectResponse::Read()
{
    _worldPacket >> Resurrecter;
    _worldPacket >> Response;
}

WorldPackets::Misc::Weather::Weather() : ServerPacket(SMSG_WEATHER, 4 + 4 + 1) { }

WorldPackets::Misc::Weather::Weather(WeatherState weatherID, float intensity /*= 0.0f*/, bool abrupt /*= false*/)
    : ServerPacket(SMSG_WEATHER, 4 + 4 + 1), Abrupt(abrupt), Intensity(intensity), WeatherID(weatherID) { }

WorldPacket const* WorldPackets::Misc::Weather::Write()
{
    _worldPacket << uint32(WeatherID);
    _worldPacket << float(Intensity);
    _worldPacket.WriteBit(Abrupt);

    _worldPacket.FlushBits();
    return &_worldPacket;
}

void WorldPackets::Misc::StandStateChange::Read()
{
    uint32 state;
    _worldPacket >> state;

    StandState = UnitStandStateType(state);
}

WorldPacket const* WorldPackets::Misc::StandStateUpdate::Write()
{
    _worldPacket << uint32(AnimKitID);
    _worldPacket << uint8(State);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::SetAnimTier::Write()
{
    _worldPacket << Unit;
    _worldPacket.WriteBits(Tier, 3);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PlayerBound::Write()
{
    _worldPacket << BinderID;
    _worldPacket << uint32(AreaID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StartMirrorTimer::Write()
{
    _worldPacket << int32(Timer);
    _worldPacket << int32(Value);
    _worldPacket << int32(MaxValue);
    _worldPacket << int32(Scale);
    _worldPacket << int32(SpellID);
    _worldPacket.WriteBit(Paused);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PauseMirrorTimer::Write()
{
    _worldPacket << int32(Timer);
    _worldPacket.WriteBit(Paused);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StopMirrorTimer::Write()
{
    _worldPacket << int32(Timer);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ExplorationExperience::Write()
{
    _worldPacket << int32(AreaID);
    _worldPacket << int32(Experience);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::LevelUpInfo::Write()
{
    _worldPacket << int32(Level);
    _worldPacket << int32(HealthDelta);

    for (int32 power : PowerDelta)
        _worldPacket << power;

    for (int32 stat : StatDelta)
        _worldPacket << stat;

    _worldPacket << int32(NumNewTalents);
    _worldPacket << int32(NumNewPvpTalentSlots);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PlayMusic::Write()
{
    _worldPacket << uint32(SoundKitID);

    return &_worldPacket;
}

void WorldPackets::Misc::RandomRollClient::Read()
{
    _worldPacket >> Min;
    _worldPacket >> Max;
    _worldPacket >> PartyIndex;
}

WorldPacket const* WorldPackets::Misc::RandomRoll::Write()
{
    _worldPacket << Roller;
    _worldPacket << RollerWowAccount;
    _worldPacket << int32(Min);
    _worldPacket << int32(Max);
    _worldPacket << int32(Result);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::EnableBarberShop::Write()
{
    _worldPacket << uint8(CustomizationScope);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::PhaseShiftDataPhase const& phaseShiftDataPhase)
{
    data << uint16(phaseShiftDataPhase.PhaseFlags);
    data << uint16(phaseShiftDataPhase.Id);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::PhaseShiftData const& phaseShiftData)
{
    data << uint32(phaseShiftData.PhaseShiftFlags);
    data << uint32(phaseShiftData.Phases.size());
    data << phaseShiftData.PersonalGUID;
    for (WorldPackets::Misc::PhaseShiftDataPhase const& phaseShiftDataPhase : phaseShiftData.Phases)
        data << phaseShiftDataPhase;

    return data;
}

WorldPacket const* WorldPackets::Misc::PhaseShiftChange::Write()
{
    _worldPacket << Client;
    _worldPacket << Phaseshift;
    _worldPacket << uint32(VisibleMapIDs.size() * 2);           // size in bytes
    for (uint16 visibleMapId : VisibleMapIDs)
        _worldPacket << uint16(visibleMapId);                   // Active terrain swap map id

    _worldPacket << uint32(PreloadMapIDs.size() * 2);           // size in bytes
    for (uint16 preloadMapId : PreloadMapIDs)
        _worldPacket << uint16(preloadMapId);                   // Inactive terrain swap map id

    _worldPacket << uint32(UiMapPhaseIDs.size() * 2);           // size in bytes
    for (uint16 uiMapPhaseId : UiMapPhaseIDs)
        _worldPacket << uint16(uiMapPhaseId);                   // phase id, controls only map display (visible on all maps)

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ZoneUnderAttack::Write()
{
    _worldPacket << int32(AreaID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::DurabilityDamageDeath::Write()
{
    _worldPacket << int32(Percent);

    return &_worldPacket;
}

void WorldPackets::Misc::ObjectUpdateFailed::Read()
{
    _worldPacket >> ObjectGUID;
}

void WorldPackets::Misc::ObjectUpdateRescued::Read()
{
    _worldPacket >> ObjectGUID;
}

WorldPacket const* WorldPackets::Misc::PlayObjectSound::Write()
{
    _worldPacket << int32(SoundKitID);
    _worldPacket << SourceObjectGUID;
    _worldPacket << TargetObjectGUID;
    _worldPacket << Position;
    _worldPacket << int32(BroadcastTextID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PlaySound::Write()
{
    _worldPacket << int32(SoundKitID);
    _worldPacket << SourceObjectGuid;
    _worldPacket << int32(BroadcastTextID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PlaySpeakerbotSound::Write()
{
    _worldPacket << SourceObjectGUID;
    _worldPacket << int32(SoundKitID);

    return &_worldPacket;
}

void WorldPackets::Misc::FarSight::Read()
{
    Enable = _worldPacket.ReadBit();
}

void WorldPackets::Misc::SaveCUFProfiles::Read()
{
    CUFProfiles.resize(_worldPacket.read<uint32>());
    for (std::unique_ptr<CUFProfile>& cufProfile : CUFProfiles)
    {
        cufProfile = std::make_unique<CUFProfile>();

        uint8 strLen = _worldPacket.ReadBits(7);

        // Bool Options
        for (uint8 option = 0; option < CUF_BOOL_OPTIONS_COUNT; option++)
            cufProfile->BoolOptions.set(option, _worldPacket.ReadBit());

        // Other Options
        _worldPacket >> cufProfile->FrameHeight;
        _worldPacket >> cufProfile->FrameWidth;

        _worldPacket >> cufProfile->SortBy;
        _worldPacket >> cufProfile->HealthText;

        _worldPacket >> cufProfile->TopPoint;
        _worldPacket >> cufProfile->BottomPoint;
        _worldPacket >> cufProfile->LeftPoint;

        _worldPacket >> cufProfile->TopOffset;
        _worldPacket >> cufProfile->BottomOffset;
        _worldPacket >> cufProfile->LeftOffset;

        cufProfile->ProfileName = _worldPacket.ReadString(strLen);
    }
}

WorldPacket const* WorldPackets::Misc::LoadCUFProfiles::Write()
{
    _worldPacket << uint32(CUFProfiles.size());

    for (CUFProfile const* cufProfile : CUFProfiles)
    {
        _worldPacket.WriteBits(cufProfile->ProfileName.size(), 7);

        // Bool Options
        for (uint8 option = 0; option < CUF_BOOL_OPTIONS_COUNT; option++)
            _worldPacket.WriteBit(cufProfile->BoolOptions[option]);

        // Other Options
        _worldPacket << cufProfile->FrameHeight;
        _worldPacket << cufProfile->FrameWidth;

        _worldPacket << cufProfile->SortBy;
        _worldPacket << cufProfile->HealthText;

        _worldPacket << cufProfile->TopPoint;
        _worldPacket << cufProfile->BottomPoint;
        _worldPacket << cufProfile->LeftPoint;

        _worldPacket << cufProfile->TopOffset;
        _worldPacket << cufProfile->BottomOffset;
        _worldPacket << cufProfile->LeftOffset;

        _worldPacket.WriteString(cufProfile->ProfileName);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PlayOneShotAnimKit::Write()
{
    _worldPacket << Unit;
    _worldPacket << uint16(AnimKitID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::SetAIAnimKit::Write()
{
    _worldPacket << Unit;
    _worldPacket << uint16(AnimKitID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::SetMovementAnimKit::Write()
{
    _worldPacket << Unit;
    _worldPacket << uint16(AnimKitID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::SetMeleeAnimKit::Write()
{
    _worldPacket << Unit;
    _worldPacket << uint16(AnimKitID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::SetPlayHoverAnim::Write()
{
    _worldPacket << UnitGUID;
    _worldPacket.WriteBit(PlayHoverAnim);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Misc::SetPvP::Read()
{
    EnablePVP = _worldPacket.ReadBit();
}

void WorldPackets::Misc::SetWarMode::Read()
{
    Enable = _worldPacket.ReadBit();
}

WorldPacket const* WorldPackets::Misc::AccountHeirloomUpdate::Write()
{
    _worldPacket.WriteBit(IsFullUpdate);
    _worldPacket.FlushBits();

    _worldPacket << int32(Unk);

    // both lists have to have the same size
    _worldPacket << uint32(Heirlooms->size());
    _worldPacket << uint32(Heirlooms->size());

    for (auto const& item : *Heirlooms)
        _worldPacket << int32(item.first);

    for (auto const& flags : *Heirlooms)
        _worldPacket << uint32(flags.second.flags);

    return &_worldPacket;
}

void WorldPackets::Misc::MountSpecial::Read()
{
    SpellVisualKitIDs.resize(_worldPacket.read<uint32>());
    _worldPacket >> SequenceVariation;
    for (int32& spellVisualKitId : SpellVisualKitIDs)
        _worldPacket >> spellVisualKitId;
}

WorldPacket const* WorldPackets::Misc::SpecialMountAnim::Write()
{
    _worldPacket << UnitGUID;
    _worldPacket << uint32(SpellVisualKitIDs.size());
    _worldPacket << int32(SequenceVariation);
    if (!SpellVisualKitIDs.empty())
        _worldPacket.append(SpellVisualKitIDs.data(), SpellVisualKitIDs.size());

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::CrossedInebriationThreshold::Write()
{
    _worldPacket << Guid;
    _worldPacket << int32(Threshold);
    _worldPacket << int32(ItemID);

    return &_worldPacket;
}

void WorldPackets::Misc::SetTaxiBenchmarkMode::Read()
{
    Enable = _worldPacket.ReadBit();
}

WorldPacket const* WorldPackets::Misc::OverrideLight::Write()
{
    _worldPacket << int32(AreaLightID);
    _worldPacket << int32(OverrideLightID);
    _worldPacket << int32(TransitionMilliseconds);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::DisplayGameError::Write()
{
    _worldPacket << uint32(Error);
    _worldPacket.WriteBit(Arg.has_value());
    _worldPacket.WriteBit(Arg2.has_value());
    _worldPacket.FlushBits();

    if (Arg)
        _worldPacket << int32(*Arg);

    if (Arg2)
        _worldPacket << int32(*Arg2);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::AccountMountUpdate::Write()
{
    _worldPacket.WriteBit(IsFullUpdate);
    _worldPacket << uint32(Mounts->size());

    for (auto [spellId, flags] : *Mounts)
    {
        _worldPacket << int32(spellId);
        _worldPacket.WriteBits(flags, 4);
    }

    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Misc::MountSetFavorite::Read()
{
    _worldPacket >> MountSpellID;
    IsFavorite = _worldPacket.ReadBit();
}

void WorldPackets::Misc::CloseInteraction::Read()
{
    _worldPacket >> SourceGuid;
}

WorldPacket const* WorldPackets::Misc::StartTimer::Write()
{
    _worldPacket << TotalTime;
    _worldPacket << TimeLeft;
    _worldPacket << int32(Type);

    return &_worldPacket;
}

void WorldPackets::Misc::ConversationLineStarted::Read()
{
    _worldPacket >> ConversationGUID;
    _worldPacket >> LineID;
}

WorldPacket const* WorldPackets::Misc::SplashScreenShowLatest::Write()
{
    _worldPacket << int32(UISplashScreenID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::DisplayToast::Write()
{
    _worldPacket << uint64(Quantity);
    _worldPacket << uint8(AsUnderlyingType(DisplayToastMethod));
    _worldPacket << uint32(QuestID);

    _worldPacket.WriteBit(Mailed);
    _worldPacket.WriteBits(AsUnderlyingType(Type), 2);
    _worldPacket.WriteBit(IsSecondaryResult);

    switch (Type)
    {
        case DisplayToastType::NewItem:
            _worldPacket.WriteBit(BonusRoll);
            _worldPacket << Item;
            _worldPacket << int32(LootSpec);
            _worldPacket << int8(Gender);
            break;
        case DisplayToastType::NewCurrency:
            _worldPacket << uint32(CurrencyID);
            break;
        default:
            break;
    }

    _worldPacket.FlushBits();

    return &_worldPacket;
}



// DekkCore >
WorldPacket const* WorldPackets::Misc::CovenantPreviewOpenNpc::Write()
{
    _worldPacket << ObjGUID;
    _worldPacket << CovenantId;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::CovenantCallingAvailibilityResponse::Write()
{
    _worldPacket << uint32(Availability);
    _worldPacket << uint32(Index);

    if (Index > 0)
        _worldPacket << Data;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::CovenantRenownSendCatchUpState::Write()
{
    _worldPacket.FlushBits();
    _worldPacket.WriteBit(CatchupState);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::LegendaryCraftingOpenNpc::Write()
{
    _worldPacket << ObjGUID;
    _worldPacket << uint8(IsUpgrade);

    return &_worldPacket;
}

void WorldPackets::Misc::CovenantRenownRequestCatchupState::Read()
{
}

void WorldPackets::Misc::RequestCovenantCallings::Read()
{
    Availability = _worldPacket.ReadBit();
}

WorldPacket const* WorldPackets::Misc::VignetteUpdate::Write()
{
    _worldPacket.WriteBit(ForceUpdate);
    _worldPacket.WriteBit(Unk_Bit_901);
    _worldPacket.FlushBits();

    _worldPacket << static_cast<uint32>(Removed.IDs.size());
    for (ObjectGuid const& ID : Removed.IDs)
        _worldPacket << ID;

    _worldPacket << static_cast<uint32>(Added.IdList.IDs.size());
    for (ObjectGuid const& ID : Added.IdList.IDs)
        _worldPacket << ID;

    _worldPacket << static_cast<uint32>(Added.Data.size());
    for (auto const& x : Added.Data)
    {
        _worldPacket << x.Pos;
        _worldPacket << x.ObjGUID;
        _worldPacket << x.VignetteID;
        _worldPacket << x.AreaID;
        _worldPacket << x.Unk901_1;
        _worldPacket << x.Unk901_2;
    }

    _worldPacket << static_cast<uint32>(Updated.IdList.IDs.size());
    for (ObjectGuid const& ID : Updated.IdList.IDs)
        _worldPacket << ID;

    _worldPacket << static_cast<uint32>(Updated.Data.size());
    for (auto const& x : Updated.Data)
    {
        _worldPacket << x.Pos;
        _worldPacket << x.ObjGUID;
        _worldPacket << x.VignetteID;
        _worldPacket << x.AreaID;
        _worldPacket << x.Unk901_1;
        _worldPacket << x.Unk901_2;
    }

    return &_worldPacket;
}

ByteBuffer & operator<<(ByteBuffer & data, WorldPackets::Misc::TaskProgress const& progress)
{
    data << progress.TaskID;
    data << uint32(progress.FailureTime);
    data << progress.Flags;
    data << progress.Unk;
    for (uint16 const& x : progress.Counts)
         data << x;

        return data;
}

WorldPacket const* WorldPackets::Misc::IslandAzeriteXpGain::Write()
{
    _worldPacket << XpGain;
    _worldPacket << PlayerGuid;
    _worldPacket << SourceGuid;
    _worldPacket << SourceID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::IslandCompleted::Write()
{
    _worldPacket << MapID;
    _worldPacket << Winner;
    _worldPacket << DisplayInfos.size();

    if (!DisplayInfos.empty())
        for (auto displayInfo : DisplayInfos)
        {
            _worldPacket << displayInfo.GUID;
            _worldPacket << int32(displayInfo.SpecializationID);
            _worldPacket << uint32(displayInfo.Items.size());
            _worldPacket.WriteBits(displayInfo.Name.length(), 6);
            _worldPacket << uint8(displayInfo.GenderID);
           // _worldPacket << uint8(displayInfo.Skin);
          //  _worldPacket << uint8(displayInfo.HairColor);
           // _worldPacket << uint8(displayInfo.HairStyle);
           // _worldPacket << uint8(displayInfo.FacialHairStyle);
         //   _worldPacket << uint8(displayInfo.Face);
            _worldPacket << uint8(displayInfo.Race);
            _worldPacket << uint8(displayInfo.ClassID);
           // _worldPacket.append(displayInfo.CustomDisplay.data(), displayInfo.CustomDisplay.size());
            _worldPacket.WriteString(displayInfo.Name);

            for (WorldPackets::Inspect::InspectItemData const& itemData : displayInfo.Items)
            {
                _worldPacket << itemData.CreatorGUID;
                _worldPacket << uint8(itemData.Index);
                _worldPacket << uint32(itemData.AzeritePowers.size());
                _worldPacket << uint32(itemData.AzeriteEssences.size());
                if (!itemData.AzeritePowers.empty())
                    _worldPacket.append(itemData.AzeritePowers.data(), itemData.AzeritePowers.size());
                _worldPacket << itemData.Item;
                _worldPacket.WriteBit(itemData.Usable);
                _worldPacket.WriteBits(itemData.Enchants.size(), 4);
                _worldPacket.WriteBits(itemData.Gems.size(), 2);
                _worldPacket.FlushBits();
                for (WorldPackets::Inspect::AzeriteEssenceData const& azeriteEssenceData : itemData.AzeriteEssences)
                {
                    _worldPacket << uint32(azeriteEssenceData.Index);
                    _worldPacket << uint32(azeriteEssenceData.AzeriteEssenceID);
                    _worldPacket << uint32(azeriteEssenceData.Rank);
                    _worldPacket.WriteBit(azeriteEssenceData.SlotUnlocked);
                    _worldPacket.FlushBits();
                }
                for (WorldPackets::Inspect::InspectEnchantData const& enchantData : itemData.Enchants)
                {
                    _worldPacket << uint32(enchantData.Id);
                    _worldPacket << uint8(enchantData.Index);
                }
                for (WorldPackets::Item::ItemGemData const& gem : itemData.Gems)
                    _worldPacket << gem;
            }

        }

    return &_worldPacket;
}

void WorldPackets::Misc::IslandOnQueue::Read()
{
    _worldPacket >> QueueNPCGuid;
    _worldPacket >> ActivityID;
}

WorldPacket const* WorldPackets::Misc::StreamingMovie::Write()
{
    _worldPacket << static_cast<uint32>(MovieIDs.size());
    for (auto const& v : MovieIDs)
        _worldPacket << v;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ArchaeologySurveryCast::Write()
{
    _worldPacket << int32(ResearchBranchID);
    _worldPacket << uint32(TotalFinds);
    _worldPacket << uint32(NumFindsCompleted);
    _worldPacket.WriteBit(SuccessfulFind);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::RequestLeadersResult::Write()
{
    _worldPacket << MapID;
    _worldPacket << ChallengeID;
    _worldPacket.AppendPackedTime(LastGuildUpdate);
    _worldPacket.AppendPackedTime(LastRealmUpdate);
    _worldPacket << GuildLeadersCount;
    _worldPacket << RealmLeadersCount;

    _worldPacket << static_cast<uint32>(GuildLeaders.size());
    _worldPacket << static_cast<uint32>(RealmLeaders.size());

    for (auto const& guildLeaders : GuildLeaders)
    {
        _worldPacket << guildLeaders.InstanceRealmAddress;
        _worldPacket << guildLeaders.AttemptID;
        _worldPacket << guildLeaders.CompletionTime;
        _worldPacket << guildLeaders.CompletionDate;
        _worldPacket << guildLeaders.MedalEarned;
    }

    for (auto const& realmLeaders : RealmLeaders)
    {
        _worldPacket << realmLeaders.InstanceRealmAddress;
        _worldPacket << realmLeaders.AttemptID;
        _worldPacket << realmLeaders.CompletionTime;
        _worldPacket << realmLeaders.CompletionDate;
        _worldPacket << realmLeaders.MedalEarned;
    }

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::ModeAttempt const& modeAttempt)
{
    data << modeAttempt.InstanceRealmAddress;
    data << modeAttempt.AttemptID;
    data << modeAttempt.CompletionTime;
    data.AppendPackedTime(modeAttempt.CompletionDate);
    data << modeAttempt.MedalEarned;
    data << static_cast<uint32>(modeAttempt.Members.size());
    for (auto const& map : modeAttempt.Members)
    {
        data << map.VirtualRealmAddress;
        data << map.NativeRealmAddress;
        data << map.Guid;
        data << map.SpecializationID;
    }

    return data;
}


ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::ChallengeModeMap const& challengeModeMap)
{
    data.FlushBits();
    data << challengeModeMap.MapId;
    data << challengeModeMap.ChallengeID;
    data << challengeModeMap.CompletedChallengeLevel;
    data << challengeModeMap.LastCompletionMilliseconds / 1000;
    data.AppendPackedTime(challengeModeMap.LastMedalDate - challengeModeMap.LastCompletionMilliseconds);
    data.AppendPackedTime(challengeModeMap.LastMedalDate);

    data << challengeModeMap.BestCompletionMilliseconds;
    data.AppendPackedTime(challengeModeMap.MedalDate);

    for (auto const& v : challengeModeMap.Affixes)
        data << v;

    data << static_cast<uint32>(challengeModeMap.Members.size());

    for (auto const& map : challengeModeMap.Members)
    {
        data.FlushBits();
        data << map.PlayerGuid;
        data << map.GuildGuid;
        data << map.VirtualRealmAddress;
        data << map.NativeRealmAddress;
        data << map.SpecializationID;
        data << map.Unk4;
        data << map.EquipmentLevel;
    }

    data << static_cast<uint32>(challengeModeMap.BestSpecID.size());
    for (auto const& map : challengeModeMap.BestSpecID)
        data << map;

    return data;
}

WorldPacket const* WorldPackets::Misc::NewPlayerRecord::Write()
{
    _worldPacket << (int32)MapID;
    _worldPacket << (int32)CompletionMilliseconds;
    _worldPacket << (uint32)ChallengeLevel;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::Complete::Write()
{
    _worldPacket << ChallengeLevel;
    _worldPacket << MapId;
    _worldPacket << ChallengeId;
    _worldPacket << ChallengeLevel;

    _worldPacket << IsCompletedInTimer;
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::Reset::Write()
{
    _worldPacket << MapID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::Start::Write()
{
    _worldPacket << MapID;
    _worldPacket << ChallengeID;
    _worldPacket << ChallengeLevel;

    _worldPacket << Affixes1;
    _worldPacket << Affixes2;
    _worldPacket << Affixes3;
    _worldPacket << Affixes4;

    _worldPacket << DeathCount;
    _worldPacket << ClientEncounterStartPlayerInfo;

    _worldPacket << Energized;

    for (uint32 i = 0; i < DeathCount; ++i)
    {
        _worldPacket << DeathCount;
    }

    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::UpdateDeathCount::Write()
{
    _worldPacket << DeathCount;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ChangePlayerDifficultyResult::Write()
{
    _worldPacket.WriteBits(Type, 4);

    switch (Type)
    {
    case 5:
    {
        _worldPacket.WriteBit(false);
        _worldPacket << uint32(2766309915);
        break;
    }
    case 11:
    {
        _worldPacket << InstanceDifficultyID;
        _worldPacket << DifficultyRecID;
        break;
    }
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StopElapsedTimer::Write()
{
    _worldPacket << TimerID;
    _worldPacket.WriteBit(KeepTimer);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StartElapsedTimers::Write()
{
    _worldPacket << static_cast<uint32>(Timers.size());
    for (auto const& v : Timers)
    {
        _worldPacket << v.TimerID;
        _worldPacket << uint32(v.CurrentDuration);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StartElapsedTimer::Write()
{
    _worldPacket << Timer.TimerID;
    _worldPacket << uint32(Timer.CurrentDuration);

    return &_worldPacket;
}

void WorldPackets::Misc::StartRequest::Read()
{
    _worldPacket >> Bag;
    _worldPacket >> Slot;
    _worldPacket >> GameObjectGUID;
}

void WorldPackets::Misc::StartChallengeMode::Read()
{
    _worldPacket >> Bag;
    _worldPacket >> Slot;
    _worldPacket >> GameObjectGUID;
}

void WorldPackets::Misc::RequestLeaders::Read()
{
    LastGuildUpdate = _worldPacket.read<uint32>();
    LastRealmUpdate = _worldPacket.read<uint32>();
    _worldPacket >> MapId;
    _worldPacket >> ChallengeID;
}

WorldPacket const* WorldPackets::Misc::WarfrontComplete::Write()
{
    _worldPacket << uint32(Unk1);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::SetMaxWeeklyQuantity::Write()
{
    _worldPacket << Type;
    _worldPacket << MaxWeeklyQuantity;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::OpenContainer::Write()
{
    _worldPacket << Guid;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::MapObjEvents::Write()
{
    _worldPacket << UniqueID;
    _worldPacket << DataSize;
    _worldPacket << static_cast<uint32>(Unk2.size());
    for (auto const& itr : Unk2)
        _worldPacket << itr;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::EndLightningStorm::Write()
{
    _worldPacket << LightningStormId;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ClubFinderResponseCharacterApplicationList::Write()
{
    _worldPacket.WriteBit(UNK);
    _worldPacket << UNK1;
    _worldPacket << UNK2;
    _worldPacket << UNK3;
    _worldPacket << UNK4;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ShowTradeSkillResponse::Write()
{
  _worldPacket << PlayerGUID;
  _worldPacket << SpellId;
  _worldPacket << static_cast<uint32>(SkillLineIDs.size());
  _worldPacket << static_cast<uint32>(SkillRanks.size());
  _worldPacket << static_cast<uint32>(SkillMaxRanks.size());
  _worldPacket << static_cast<uint32>(KnownAbilitySpellIDs.size());

       for (auto const& v : SkillLineIDs)
       _worldPacket << v;

       for (auto const& c : SkillRanks)
       _worldPacket << c;

       for (auto const& z : SkillMaxRanks)
        _worldPacket << z;

       for (auto const& t : KnownAbilitySpellIDs)
       _worldPacket << t;

        return &_worldPacket;
  }

WorldPacket const* WorldPackets::Misc::PlayerSkinned::Write()
{
    _worldPacket.WriteBit(FreeRepop);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Misc::ContributionCollectorContribute::Read()
{
    _worldPacket >> ContributionTableNpcGuid;
    _worldPacket >> OrderIndex;
}

WorldPacket const* WorldPackets::Misc::ContributionResponse::Write()
{
    _worldPacket << Data;
    _worldPacket << ContributionID;
    _worldPacket << ContributionGUID;

    return &_worldPacket;
}

void WorldPackets::Misc::CommentatorEnable::Read()
{
    _worldPacket >> Enable;
}

void WorldPackets::Misc::CommentatorGetMapInfo::Read()
{
    uint8 playerNameLen = _worldPacket.ReadBits(6);
    std::string PlayerName = _worldPacket.ReadString(playerNameLen);
}

void WorldPackets::Misc::CommentatorGetPlayerInfo::Read()
{
    uint32 MapId;
    _worldPacket >> MapId;
    uint32 Realm;
    uint16 Server;
    uint8 Type;
    _worldPacket >> Realm;
    _worldPacket >> Server;
    _worldPacket >> Type;
}

void WorldPackets::Misc::CommentatorEnterInstance::Read()
{
    uint32 MapId, InstanceId, DifficultyId;
    _worldPacket >> MapId;
    uint32 Realm;
    uint16 Server;
    uint8 Type;
    _worldPacket >> Realm;
    _worldPacket >> Server;
    _worldPacket >> Type;
    _worldPacket >> InstanceId;
    _worldPacket >> DifficultyId;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::ServerSpec const& server)
{
    data << server.Realm;
    data << server.Server;
    data << server.Type;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::CommentatorPlayer const& player)
{
    data << player.Guid;
    data << player.UserServer;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::CommentatorTeam const& team)
{
    data << team.Guid;
    data << static_cast<int32>(team.Players.size());

    for (auto const& player : team.Players)
        data << player;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::CommentatorInstance const& instance)
{
    data << instance.MapID;
    data << instance.WorldServer;
    data << instance.InstanceID;
    data << instance.Status;

    for (auto const& team : instance.Teams)
        data << team;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::CommentatorMap const& map)
{
    data << map.TeamSize;
    data << map.DifficultyID;
    data << map.MinLevelRange;
    data << map.MaxLevelRange;
    data << static_cast<int32>(map.Instances.size());

    for (auto const& instance : map.Instances)
        data << instance;

    return data;
}

WorldPacket const* WorldPackets::Misc::CommentatorMapInfo::Write()
{
    _worldPacket << PlayerInstanceID;
    _worldPacket << static_cast<int32>(Maps.size());

    for (auto const& map : Maps)
        _worldPacket << map;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::CommentatorSpellChargeEntry const& entry)
{
    data << entry.Unk;
    data << entry.Unk2;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::SpellHistoryEntry const& entry)
{
    data << entry.Unk;
    data << entry.Unk2;
    data << entry.Unk3;
    data << entry.Unk4;
    data << entry.Unk5;

    data.WriteBit(entry.Unk6);
    data.WriteBit(entry.Unk7);
    data.WriteBit(entry.Unk8);
    data.FlushBits();

    if (entry.Unk6)
        data << uint32(0);

    if (entry.Unk7)
        data << uint32(0);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::SpellChargeEntry const& entry)
{
    data << entry.Unk;
    data << entry.Unk2;
    data << entry.Unk3;
    data << entry.Unk4;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Misc::CommentatorPlayerData const& pdata)
{
    data << pdata.PlayerGUID;
    data << pdata.FactionIndex;
    data << pdata.DamageDone;
    data << pdata.Kills;
    data << pdata.Deaths;
    data << pdata.DamageTaken;
    data << pdata.HealingDone;
    data << pdata.HealingTaken;
    data << pdata.SpecID;
    data << static_cast<int32>(pdata.SpellHistoryEntries.size());
    data << static_cast<int32>(pdata.SpellChargeEntries.size());
    data << static_cast<int32>(pdata.CommentatorSpellChargeEntries.size());

    for (auto const& entry : pdata.SpellChargeEntries)
        data << entry;

    for (auto const& entry : pdata.CommentatorSpellChargeEntries)
        data << entry;

    for (auto const& entry : pdata.SpellHistoryEntries)
        data << entry;

    return data;
}

WorldPacket const* WorldPackets::Misc::CommentatorPlayerInfo::Write()
{
    _worldPacket << MapID;
    _worldPacket << WorldServer;
    _worldPacket << InstanceID;
    _worldPacket << static_cast<int32>(PlayerInfo.size());
    _worldPacket.WriteBit(true);

    for (auto const& info : PlayerInfo)
        _worldPacket << info;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::CommentatorStateChanged::Write()
{
    _worldPacket << Guid;
    _worldPacket.WriteBit(true);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::WeeklyRewardClaimResult::Write()
{
    _worldPacket << int32(Result);

    return &_worldPacket;
}

void WorldPackets::Misc::FactionSelect::Read()
{
    _worldPacket >> FactionChoice;
}

void WorldPackets::Misc::ActivateSoulbind::Read()
{
    _worldPacket >> CovenantID;
}

WorldPacket const* WorldPackets::Misc::PerksProgramAcitivtyUpdate::Write()
{
    _worldPacket << int32(ActivityID);

    for (auto const& activity : ActivityCount)
        _worldPacket << activity;

    return &_worldPacket;
}

void WorldPackets::Misc::claimweeklyrewards::Read()
{
    _worldPacket >> UNK;
}

void WorldPackets::Misc::ContributionLastupdaterequest::Read()
{
    _worldPacket >> unk1;
    _worldPacket >> unk2;
}

WorldPacket const* WorldPackets::Misc::Playerchoicedisplayerror::Write()
{
    _worldPacket << uint32(choiceid);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ClearTreasurePickerCache::Write()
{
    _worldPacket << uint32(treasurepickerid);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::AccountCosmeticAdded::Write()
{
    _worldPacket << uint32(UNK1);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ActivateSoulbindFailed::Write()
{
    _worldPacket << uint32(CovenantID);
    _worldPacket << uint8(unk);

    return &_worldPacket;
}

void WorldPackets::Misc::ConversationCinematicReady::Read()
{
    _worldPacket >> ConversationGUID;
}

void WorldPackets::Misc::OverrideScreenFlash::Read()
{
    _worldPacket >> BlackScreenOrRedScreen;
}

WorldPacket const* WorldPackets::Misc::PlayerChoiceClear::Write()
{
    _worldPacket << int32(ChoiceID);
    _worldPacket << Status;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PerksProgramActivityComplete::Write()
{
    _worldPacket << int32(ActivityID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ApplyMountEquipmentResult::Write()
{
    _worldPacket << PlayerGuid;
    _worldPacket << Unk1;
    _worldPacket << unk;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::GainMawPower::Write()
{
    _worldPacket << PlayerGuid;
    _worldPacket << Power;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::MultiFloorLeaveFloor::Write()
{
    _worldPacket << unk1;
    _worldPacket << unk2;
    _worldPacket << unk3;
    _worldPacket << unk4;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::ProfessionGossip::Write()
{
    _worldPacket << NpcGUID;
    _worldPacket << unk1;
    _worldPacket << unk2;

    return &_worldPacket;
}

void WorldPackets::Misc::AbandonNpeResponse::Read()
{
    _worldPacket >> UNK;
}

void WorldPackets::Misc::AddAccountCosmetic::Read()
{
    _worldPacket >> Playerguid;
}

void WorldPackets::Misc::ClearNewAppearance::Read()
{
    _worldPacket >> unk;
}

void WorldPackets::Misc::AccountNotificationAcknowledge::Read()
{
    _worldPacket >> unk;
    _worldPacket >> unk2;
    _worldPacket >> unk3;
}

void WorldPackets::Misc::AuctionableTokenSell::Read()
{
    _worldPacket >> unkint64;
    _worldPacket >> CurrentMarketPrice;
    _worldPacket >> UnkInt32;
}

void WorldPackets::Misc::AuctionableTokenSellAtMarketPrice::Read()
{
    _worldPacket >> TokenGuid;
    _worldPacket >> UnkInt32;
    _worldPacket >> PendingBuyConfirmations;
    _worldPacket >> GuaranteedPrice;
    _worldPacket >> confirmed;
}

void WorldPackets::Misc::CanRedeemTokenForBalance::Read()
{
    _worldPacket >> UnkInt32;
}

void WorldPackets::Misc::ChangeBagSlotFlag::Read()
{
    _worldPacket >> UnkInt;
    _worldPacket >> UnkInt32;
    _worldPacket >> unknown;
}

void WorldPackets::Misc::ChangeBankBagSlotFlag::Read()
{
    _worldPacket >> UnkInt;
    _worldPacket >> UnkInt32;
    _worldPacket >> unknown;
}

void WorldPackets::Misc::CommerceTokenGetCount::Read()
{
    _worldPacket >> count;
}

void WorldPackets::Misc::ContributionLastUpdateRequest::Read()
{
    _worldPacket >> ContributionId;
    _worldPacket >> count;
}

void WorldPackets::Misc::UpgradeRuneforgeLegendary::Read()
{
    _worldPacket >>  ItemID;
    _worldPacket >> UpgradeType;
    _worldPacket >> UpgradeLevel;
    _worldPacket >> Cost;
    _worldPacket >> RemainingUpgradeLevel;
}

void WorldPackets::Misc::ShowTradeSkill::Read()
{
    _worldPacket >> PlayerGUID;
    _worldPacket >> SpellID;
    _worldPacket >> SkillLineID;
}

void WorldPackets::Misc::QuickJoinSignalToastDisplayed::Read()
{
    _worldPacket >> GroupGUID;
    _worldPacket >> Priority;
    _worldPacket >> unk;
    UnkGuids.resize(_worldPacket.read<uint32>());
    for (auto& v : UnkGuids)
        _worldPacket >> v;

    UnkBit1 = _worldPacket.ReadBit();
}
//
