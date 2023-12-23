////

#include "GarrisonMission.h"
#include "Garrison.h"
#include "GarrisonMgr.h"

void Missionssss::Start(Player* owner, std::vector<uint64> const &followers)
{
    /*
        GARRISON_ERROR_MISSION_REQUIRES_100_TO_START = 82,
        GARRISON_ERROR_INVALID_MISSION_BONUS_ABILITY = 76,
        GARRISON_ERROR_NO_MISSION_NPC = 51,
        GARRISON_ERROR_MISSION_NPC_NOT_AVAILABLE = 52,
        GARRISON_ERROR_NOT_ON_MISSION = 41,
        GARRISON_ERROR_ALREADY_COMPLETED_MISSION = 42,
        GARRISON_ERROR_MISSION_EXISTS = 33,
        GARRISON_ERROR_INVALID_MISSION = 34,
        GARRISON_ERROR_INVALID_MISSION_TIME = 35,
        GARRISON_ERROR_INVALID_MISSION_REWARD_INDEX = 36,
        GARRISON_ERROR_MISSION_NOT_OFFERED = 37,
        GARRISON_ERROR_MISSION_SIZE_INVALID = 39,
     */

    auto missionStartFailed([this, followers, owner](uint32 result) -> void
    {
        WorldPackets::Garrison::GarrisonStartMissionResult missionRes;
        missionRes.Result = result;
        missionRes.Mission = PacketInfo;
        missionRes.Followers.insert(missionRes.Followers.begin(), followers.begin(), followers.end());
        owner->SendDirectMessage(missionRes.Write());
    });

    auto garrison = owner->GetGarrison(GARRISON_TYPE_GARRISON);
    if (!garrison)
    {
        missionStartFailed(GARRISON_ERROR_NO_GARRISON);
        return;
    }

    if (PacketInfo.MissionState != MISSION_STATE_AVAILABLE)
    {
        missionStartFailed(GARRISON_ERROR_MISSION_EXPIRED);
        return;
    }

    if (PacketInfo.StartTime != time_t(0))
    {
        missionStartFailed(GARRISON_ERROR_MISSION_EXPIRED);
        return;
    }

    auto missionEntry = sGarrMissionStore.AssertEntry(PacketInfo.MissionRecID);
    if (missionEntry->MissionCost && !owner->HasCurrency(missionEntry->MissionCostCurrencyTypesID, missionEntry->MissionCost))
    {
        missionStartFailed(GARRISON_ERROR_MISSION_START_CONDITION_FAILED);
        return;
    }

    if (followers.empty() || followers.size() > missionEntry->MaxFollowers)
    {
        missionStartFailed(GARRISON_ERROR_MISSION_MISSING_REQUIRED_FOLLOWER);
        return;
    }

    for (auto f : followers)
    {
        auto follower = garrison->GetFollower(f);
        if (!follower)
        {
            missionStartFailed(GARRISON_ERROR_INVALID_FOLLOWER);
            return;
        }

        if (follower->PacketInfo.CurrentMissionID != 0)
        {
            missionStartFailed(GARRISON_ERROR_FOLLOWER_ALREADY_ON_MISSION); /// or GARRISON_ERROR_ALREADY_ON_MISSION ?
            return;
        }

        if (follower->PacketInfo.CurrentBuildingID != 0)
        {
            missionStartFailed(GARRISON_ERROR_FOLLOWER_IN_BUILDING);
            return;
        }

       /* if (follower->TypeID != missionEntry->GarrFollowerTypeID)
        {
            missionStartFailed(GARRISON_ERROR_FOLLOWER_TYPE_MISMATCH);
            return;
        }*/

        //@TODO GarrTypeID check here too?
        if (missionEntry->TargetLevel && uint32(missionEntry->TargetLevel) > follower->PacketInfo.FollowerLevel)
        {
            missionStartFailed(GARRISON_ERROR_INVALID_FOLLOWER_LEVEL);
            return;
        }

        if (follower->PacketInfo.FollowerStatus & GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE)
        {
            missionStartFailed(GARRISON_ERROR_FOLLOWER_INACTIVE);
            return;
        }

       // follower->DbState = DB_STATE_CHANGED;
        follower->PacketInfo.CurrentMissionID = PacketInfo.MissionRecID;
        CurrentFollowerDBIDs.push_back(f);
    }

    PacketInfo.MissionState = MISSION_STATE_IN_PROGRESS;
  //  PacketInfo.SuccessChance = garrison->GetMissionSuccessChance(this, missionEntry);
    PacketInfo.OfferDuration = Seconds(missionEntry->MissionDuration);
    PacketInfo.OfferDuration = Seconds(missionEntry->OfferDuration);
    PacketInfo.StartTime = time(nullptr);
    //DbState = DB_STATE_CHANGED;

    WorldPackets::Garrison::GarrisonStartMissionResult missionRes;
    missionRes.Result = GARRISON_SUCCESS;
    missionRes.Mission = PacketInfo;
    missionRes.Followers.insert(missionRes.Followers.begin(), followers.begin(), followers.end());
    owner->SendDirectMessage(missionRes.Write());

    if (missionEntry->MissionCost > 0)
        owner->ModifyCurrency(missionEntry->MissionCostCurrencyTypesID, missionEntry->MissionCost * -1);

    garrison->SendMissionListUpdate(false);

    owner->UpdateCriteria(CriteriaType::StartGarrisonMission, PacketInfo.MissionRecID);
}

void Missionssss::Complete(Player* owner)
{
    auto garrison = owner->GetGarrison(GARRISON_TYPE_COVENANT);
    if (!garrison)
        return;

    auto missionEntry = sGarrMissionStore.LookupEntry(PacketInfo.MissionRecID);
    if (!missionEntry)
        return;

    //! just do it. should never do it.
    //if (!PacketInfo.SuccessChance)
        //PacketInfo.SuccessChance = garrison->GetMissionSuccessChance(this, missionEntry);

    auto bonus = roll_chance_f(PacketInfo.SuccessChance);
    if (bonus)
    {
        PacketInfo.MissionState = MISSION_STATE_WAITING_BONUS;

        if (PacketInfo.SuccessChance > 100 && roll_chance_f(PacketInfo.SuccessChance - 100))
            PacketInfo.MissionState = MISSION_STATE_WAITING_OWERMAX_BONUS;
    }
    else
        PacketInfo.MissionState = MISSION_STATE_COMPLETED;

    WorldPackets::Garrison::GarrisonCompleteMissionResult completeMissionResult; // is not that opcode need to investigate
    completeMissionResult.Result = GARRISON_SUCCESS;
    completeMissionResult.MissionData = PacketInfo;
    completeMissionResult.MissionRecID = PacketInfo.MissionRecID;
    completeMissionResult.Succeeded = bonus;

    for (auto f : CurrentFollowerDBIDs)
    {
        if (auto follower = garrison->GetFollower(f))
        {
            //follower->DbState = DB_STATE_CHANGED;

         //   completeMissionResult.followerData.emplace_back();
         //   auto& data = completeMissionResult.followerData.back();
           // data.FollowerDbID = follower->PacketInfo.DbID;

            if (!bonus)
                follower->PacketInfo.CurrentMissionID = 0;

            auto followerEntry = sGarrFollowerStore.AssertEntry(follower->PacketInfo.GarrFollowerID);
            if (followerEntry->Vitality)
            {
                if (follower->PacketInfo.Health > 0)
                    follower->PacketInfo.Health -= 1;

                //remove.
                if (follower->PacketInfo.Health <= 0)
                {
                    ASSERT(followerEntry->Vitality < 5);
                   // garrison->DecrementTroopCount(followerEntry->Vitality);

                    //! unlock
                    //for (auto& v : garrison->GetFollowers())
                    //{
                    //    if (v.second.PacketInfo.FollowerStatus & GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE)
                    //    {
                    //     //   v.second.PacketInfo.FollowerStatus = v.second.PacketInfo.FollowerStatus & ~GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE;
                    //      //  v.second.DbState = DB_STATE_CHANGED;

                    //       // WorldPackets::Garrison::GarrisonFollowerChangedAbilities followers;
                    //      //  followers.Follower = v.second.PacketInfo;
                    //      //  owner->SendDirectMessage(followers.Write());
                    //        break;
                    //    }
                    //}

                    WorldPackets::Garrison::GarrisonRemoveFollowerResult removeFollowerResult;
                    removeFollowerResult.FollowerDBID = follower->PacketInfo.DbID;
                    removeFollowerResult.GarrTypeID = followerEntry->GarrTypeID;
                    removeFollowerResult.Result = 3;
                    removeFollowerResult.Destroyed = 0;
                    owner->SendDirectMessage(removeFollowerResult.Write());

                    //follower->DbState = DB_STATE_REMOVED;
//                    follower->PacketInfo.FollowerStatus != GarrisonFollowerStatus::FOLLOWER_STATUS_EXHAUSTED;
                }
            }
        }
    }

    owner->SendDirectMessage(completeMissionResult.Write());

    for (auto followerID : CurrentFollowerDBIDs)
    {
        if (auto follower = garrison->GetFollower(followerID))
        {
            WorldPackets::Garrison::GarrisonFollowerChangedXP data;
            data.TotalXp = missionEntry->BaseFollowerXP;
            data.Follower = follower->PacketInfo;

         //   follower->GiveXP(missionEntry->BaseFollowerXP);
         //   garrison->ReTrainFollower(nullptr, follower->PacketInfo.GarrFollowerID);

            data.Follower2 = follower->PacketInfo;
            owner->SendDirectMessage(data.Write());
        }
    }

    if (!bonus)
    {
        CurrentFollowerDBIDs.clear();
        //garrison->RemoveMissionByGuid(PacketInfo.DbID);
        garrison->SendMissionListUpdate(true);
    }

    owner->UpdateCriteria(CriteriaType::SucceedGarrisonMission, PacketInfo.MissionRecID);
//  //  owner->UpdateAchievementCriteria(CRITERIA_TYPE_COMPLETE_GARRISON_MISSION_COUNT, true);
}

bool Missionssss::HasBonusRoll()
{
    return roll_chance_f(ComputeSuccessChance());
}

void Missionssss::BonusRoll(Player* owner)
{
    auto garrison = owner->GetGarrison(GARRISON_TYPE_COVENANT);
    if (!garrison)
        return;

    PacketInfo.MissionState = PacketInfo.MissionState == MISSION_STATE_WAITING_BONUS ? MISSION_STATE_COMPLETED : MISSION_STATE_COMPLETED_OWERMAX;

    WorldPackets::Garrison::GarrisonMissionBonusRollResult res;
    res.MissionData = PacketInfo;
    res.MissionRecID = PacketInfo.MissionRecID;
    res.Result = GARRISON_SUCCESS;
    owner->SendDirectMessage(res.Write());

//    auto missionRewardEntry = sGarrisonMgr.GetMissionRewardByRecID(PacketInfo.RecID);
    for (auto f : CurrentFollowerDBIDs)
    {
        auto follower = garrison->GetFollower(f);
        if (!follower)
            continue;

        //follower->DbState = DB_STATE_CHANGED;
        follower->PacketInfo.CurrentMissionID = 0;

        auto followerEntry = sGarrFollowerStore.LookupEntry(follower->PacketInfo.GarrFollowerID);
        if (followerEntry->Vitality)
        {
            if (follower->PacketInfo.Durability > 0)
                follower->PacketInfo.Durability -= 1;

            //remove.
            if (follower->PacketInfo.Durability <= 0)
            {
              //  ASSERT(followerEntry->Vitality < 5);
//                garrison->DecrementTroopCount(followerEntry->Vitality);

                //! unlock
              //  for (auto& v : garrison->GetFollowers())
             //   {
               //     if (v.second.PacketInfo.FollowerStatus & GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE)
               //     {
                       // v.second.PacketInfo.FollowerStatus = v.second.PacketInfo.FollowerStatus & ~GarrisonFollowerStatus::FOLLOWER_STATUS_INACTIVE;
                     //   v.second.DbState = DB_STATE_CHANGED;

                       // WorldPackets::Garrison::GarrisonFollowerChangedAbilities followers;
        //                followers.Follower = v.second.PacketInfo;
        //                owner->SendDirectMessage(followers.Write());
//        //                break;
//        //            }
//        //        }
//
                WorldPackets::Garrison::GarrisonRemoveFollowerResult removeFollowerResult;
                removeFollowerResult.FollowerDBID = follower->PacketInfo.DbID;
                removeFollowerResult.GarrTypeID = followerEntry->GarrTypeID;
                removeFollowerResult.Result = 3;
                removeFollowerResult.Destroyed = 0;
                owner->SendDirectMessage(removeFollowerResult.Write());

              //  follower->DbState = DB_STATE_REMOVED;
            }
       }

        if (/*!missionRewardEntry || */follower->PacketInfo.FollowerStatus & GarrisonFollowerStatus::FOLLOWER_STATUS_NO_XP_GAIN)
            continue;

//        if (missionRewardEntry->HasFollowerXPReward())
// //       {
// //           WorldPackets::Garrison::GarrisonFollowerChangedXP data;
// //           data.TotalXp = missionRewardEntry->RewardXP;
// //           data.Follower = follower->PacketInfo;
//
// //           follower->GiveXP(missionRewardEntry->RewardXP);
// //           garrison->ReTrainFollower(nullptr, follower->PacketInfo.GarrFollowerID);
//
// //           data.Follower2 = follower->PacketInfo;
// //           owner->SendDirectMessage(data.Write());
// //       }*/
    }

     CurrentFollowerDBIDs.clear();

    ObjectGuid guid;

//    garrison->RewardMission(PacketInfo.MissionRecID);
    if (PacketInfo.MissionState == MISSION_STATE_COMPLETED_OWERMAX)
    //    garrison->RewardMission(PacketInfo.MissionRecID, true);
   // garrison->RemoveMissionByGuid(PacketInfo.DbID);
    garrison->SendMissionListUpdate(true);
}

float Missionssss::ComputeSuccessChance()
{
    return 100.0f;
}

float Missionssss::CalcChance(float a, float b, float c)
{
    float d = 0.0f;
    if (c >= 0)
        d = ((b - a) * c) + a;
    else
        d = (c + 1) * a;

    return d;
}

std::vector<Followersss*> Missionssss::GetMissionFollowers(uint32 /*garrMissionId*/)
{
    std::vector<Followersss*> missionFollowers;
//    for (auto f : CurrentFollowerDBIDs)
      //  if (Followersss* follower = garrison->GetFollower(f))
         //   missionFollowers.push_back(follower);
    return missionFollowers;
}

//uint32 Mission::GetDuration(Player* owner)
//{
//    uint32 duration = PacketInfo.MissionDuration;
//
//  //  if (Garrison* garrison = owner->GetGarrison())
//  //  {
//  //  for (auto f : CurrentFollowerDBIDs)
//  //  {
// //   if (Follower* follower = garrison->GetFollower(f))
////    {
//    // Epic Mount
////    for (GarrAbilityEntry const* entry : follower->PacketInfo.AbilityID)
////    {
////    if (entry && entry->ID == 221)
////    duration *= 0.5f;
////    }
////    }
////    }
////    }*/
//
//    return duration;
//}
