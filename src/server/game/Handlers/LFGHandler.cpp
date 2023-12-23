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

#include "CharacterCache.h"
#include "DB2Stores.h"
#include "WorldSession.h"
#include "GameTime.h"
#include "Group.h"
#include "LFGMgr.h"
#include "LFGPackets.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"

//DekkCore
#include <Chat.h>
#include <LFGListMgr.h>
//DekkCore

#include "Flux.h" // < Fluxurion

void WorldSession::HandleLfgJoinOpcode(WorldPackets::LFG::DFJoin& dfJoin)
{
    if (!sLFGMgr->isOptionEnabled(lfg::LFG_OPTION_ENABLE_DUNGEON_FINDER | lfg::LFG_OPTION_ENABLE_RAID_BROWSER) ||
        (GetPlayer()->GetGroup() && GetPlayer()->GetGroup()->GetLeaderGUID() != GetPlayer()->GetGUID() &&
            (GetPlayer()->GetGroup()->GetMembersCount() == MAX_GROUP_SIZE || !GetPlayer()->GetGroup()->isLFGGroup())))
        return;

    if (dfJoin.Slots.empty())
    {
        TC_LOG_DEBUG("lfg", "CMSG_DF_JOIN {} no dungeons selected", GetPlayerInfo());
        return;
    }

    lfg::LfgDungeonSet newDungeons;
    for (uint32 slot : dfJoin.Slots)
    {
        uint32 dungeon = slot & 0x00FFFFFF;
        if (sLFGDungeonsStore.LookupEntry(dungeon))
            newDungeons.insert(dungeon);
    }

    TC_LOG_DEBUG("lfg", "CMSG_DF_JOIN {} roles: {}, Dungeons: {}", GetPlayerInfo(), dfJoin.Roles, uint8(newDungeons.size()));

    sLFGMgr->JoinLfg(GetPlayer(), uint8(dfJoin.Roles), newDungeons);
}

void WorldSession::HandleLfgLeaveOpcode(WorldPackets::LFG::DFLeave& dfLeave)
{
    Group* group = GetPlayer()->GetGroup();

    TC_LOG_DEBUG("lfg", "CMSG_DF_LEAVE {} in group: {} sent guid {}.",
        GetPlayerInfo(), group ? 1 : 0, dfLeave.Ticket.RequesterGuid.ToString());

    // Check cheating - only leader can leave the queue
    if (!group || group->GetLeaderGUID() == dfLeave.Ticket.RequesterGuid)
        sLFGMgr->LeaveLfg(dfLeave.Ticket.RequesterGuid);
}

void WorldSession::HandleLfgProposalResultOpcode(WorldPackets::LFG::DFProposalResponse& dfProposalResponse)
{
    TC_LOG_DEBUG("lfg", "CMSG_LFG_PROPOSAL_RESULT {} proposal: {} accept: {}",
        GetPlayerInfo(), dfProposalResponse.ProposalID, dfProposalResponse.Accepted ? 1 : 0);
    sLFGMgr->UpdateProposal(dfProposalResponse.ProposalID, GetPlayer()->GetGUID(), dfProposalResponse.Accepted);
}

void WorldSession::HandleLfgSetRolesOpcode(WorldPackets::LFG::DFSetRoles& dfSetRoles)
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    Group* group = GetPlayer()->GetGroup();
    if (!group)
    {
        TC_LOG_DEBUG("lfg", "CMSG_DF_SET_ROLES {} Not in group",
            GetPlayerInfo());
        return;
    }
    ObjectGuid gguid = group->GetGUID();
    TC_LOG_DEBUG("lfg", "CMSG_DF_SET_ROLES: Group {}, Player {}, Roles: {}",
        gguid.ToString(), GetPlayerInfo(), dfSetRoles.RolesDesired);
    sLFGMgr->UpdateRoleCheck(gguid, guid, dfSetRoles.RolesDesired);
}

void WorldSession::HandleLfgSetBootVoteOpcode(WorldPackets::LFG::DFBootPlayerVote& dfBootPlayerVote)
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    TC_LOG_DEBUG("lfg", "CMSG_LFG_SET_BOOT_VOTE {} agree: {}",
        GetPlayerInfo(), dfBootPlayerVote.Vote ? 1 : 0);
    sLFGMgr->UpdateBoot(guid, dfBootPlayerVote.Vote);
}

void WorldSession::HandleLfgTeleportOpcode(WorldPackets::LFG::DFTeleport& dfTeleport)
{
    TC_LOG_DEBUG("lfg", "CMSG_DF_TELEPORT {} out: {}",
        GetPlayerInfo(), dfTeleport.TeleportOut ? 1 : 0);
    sLFGMgr->TeleportPlayer(GetPlayer(), dfTeleport.TeleportOut, true);
}

void WorldSession::HandleDFGetSystemInfo(WorldPackets::LFG::DFGetSystemInfo& dfGetSystemInfo)
{
    TC_LOG_DEBUG("lfg", "CMSG_DF_GET_SYSTEM_INFO {} for {}", GetPlayerInfo(), (dfGetSystemInfo.Player ? "player" : "party"));

    if (dfGetSystemInfo.Player)
        SendLfgPlayerLockInfo();
    else
        SendLfgPartyLockInfo();
}

void WorldSession::HandleDFGetJoinStatus(WorldPackets::LFG::DFGetJoinStatus& /*dfGetJoinStatus*/)
{
    TC_LOG_DEBUG("lfg", "CMSG_DF_GET_JOIN_STATUS {}", GetPlayerInfo());

    if (!GetPlayer()->isUsingLfg())
        return;

    ObjectGuid guid = GetPlayer()->GetGUID();
    lfg::LfgUpdateData updateData = sLFGMgr->GetLfgStatus(guid);

    if (GetPlayer()->GetGroup())
    {
        SendLfgUpdateStatus(updateData, true);
        updateData.dungeons.clear();
        SendLfgUpdateStatus(updateData, false);
    }
    else
    {
        SendLfgUpdateStatus(updateData, false);
        updateData.dungeons.clear();
        SendLfgUpdateStatus(updateData, true);
    }
}

void WorldSession::SendLfgPlayerLockInfo()
{
    TC_LOG_DEBUG("lfg", "SMSG_LFG_PLAYER_INFO {}", GetPlayerInfo());

    // Get Random dungeons that can be done at a certain level and expansion
    uint8 level = GetPlayer()->GetLevel();
    uint32 contentTuningReplacementConditionMask = GetPlayer()->m_playerData->CtrOptions->ContentTuningConditionMask;
    // Fluxurion >
    lfg::LfgDungeonSet const& randomDungeons = sLFGMgr->GetRandomAndSeasonalDungeons(GetPlayer(), Fluxurion::GetChromieTimeExpansionLevel(Fluxurion::GetChromieTime(GetPlayer())), contentTuningReplacementConditionMask);

    WorldPackets::LFG::LfgPlayerInfo lfgPlayerInfo;

    // Get player locked Dungeons
    for (auto const& lock : Fluxurion::GetLockedDungeons(_player->GetGUID()))
        lfgPlayerInfo.BlackList.Slot.emplace_back(lock.first, lock.second.lockStatus, lock.second.requiredItemLevel, lock.second.currentItemLevel, lock.second.softLock);
    // < Fluxurion

    for (uint32 slot : randomDungeons)
    {
        lfgPlayerInfo.Dungeon.emplace_back();
        WorldPackets::LFG::LfgPlayerDungeonInfo& playerDungeonInfo = lfgPlayerInfo.Dungeon.back();
        playerDungeonInfo.Slot = slot;
        playerDungeonInfo.CompletionQuantity = 1;
        playerDungeonInfo.CompletionLimit = 1;
        playerDungeonInfo.CompletionCurrencyID = 0;
        playerDungeonInfo.SpecificQuantity = 0;
        playerDungeonInfo.SpecificLimit = 1;
        playerDungeonInfo.OverallQuantity = 0;
        playerDungeonInfo.OverallLimit = 1;
        playerDungeonInfo.PurseWeeklyQuantity = 0;
        playerDungeonInfo.PurseWeeklyLimit = 0;
        playerDungeonInfo.PurseQuantity = 0;
        playerDungeonInfo.PurseLimit = 0;
        playerDungeonInfo.Quantity = 1;
        playerDungeonInfo.CompletedMask = 0;
        playerDungeonInfo.EncounterMask = 0;

        if (lfg::LfgReward const* reward = sLFGMgr->GetRandomDungeonReward(slot, level))
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(reward->firstQuest))
            {
                playerDungeonInfo.FirstReward = !GetPlayer()->CanRewardQuest(quest, false);
                if (!playerDungeonInfo.FirstReward)
                    quest = sObjectMgr->GetQuestTemplate(reward->otherQuest);

                if (quest)
                {
                    playerDungeonInfo.Rewards.RewardMoney = _player->GetQuestMoneyReward(quest);
                    playerDungeonInfo.Rewards.RewardXP = _player->GetQuestXPReward(quest);
                    for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; ++i)
                        if (uint32 itemId = quest->RewardItemId[i])
                            playerDungeonInfo.Rewards.Item.emplace_back(itemId, quest->RewardItemCount[i]);

                    for (uint32 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
                        if (uint32 curencyId = quest->RewardCurrencyId[i])
                            playerDungeonInfo.Rewards.Currency.emplace_back(curencyId, quest->RewardCurrencyCount[i]);
                }
            }
        }
    }

    SendPacket(lfgPlayerInfo.Write());;
}

void WorldSession::SendLfgPartyLockInfo()
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    WorldPackets::LFG::LfgPartyInfo lfgPartyInfo;

    // Get the locked dungeons of the other party members
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* plrg = itr->GetSource();
        if (!plrg)
            continue;

        ObjectGuid pguid = plrg->GetGUID();
        if (pguid == guid)
            continue;

        lfgPartyInfo.Player.emplace_back();
        WorldPackets::LFG::LFGBlackList& lfgBlackList = lfgPartyInfo.Player.back();
        lfgBlackList.PlayerGuid = pguid;
        for (auto const& lock : Fluxurion::GetLockedDungeons(pguid))
            lfgBlackList.Slot.emplace_back(lock.first, lock.second.lockStatus, lock.second.requiredItemLevel, lock.second.currentItemLevel, 0);
    }

    TC_LOG_DEBUG("lfg", "SMSG_LFG_PARTY_INFO {}", GetPlayerInfo());
    SendPacket(lfgPartyInfo.Write());;
}

void WorldSession::SendLfgUpdateStatus(lfg::LfgUpdateData const& updateData, bool party)
{
    bool join = false;
    bool queued = false;

    switch (updateData.updateType)
    {
    case lfg::LFG_UPDATETYPE_JOIN_QUEUE_INITIAL:            // Joined queue outside the dungeon
        join = true;
        break;
    case lfg::LFG_UPDATETYPE_JOIN_QUEUE:
    case lfg::LFG_UPDATETYPE_ADDED_TO_QUEUE:                // Rolecheck Success
        join = true;
        queued = true;
        break;
    case lfg::LFG_UPDATETYPE_PROPOSAL_BEGIN:
        join = true;
        break;
    case lfg::LFG_UPDATETYPE_UPDATE_STATUS:
        join = updateData.state != lfg::LFG_STATE_ROLECHECK && updateData.state != lfg::LFG_STATE_NONE;
        queued = updateData.state == lfg::LFG_STATE_QUEUED;
        break;
    default:
        break;
    }

    TC_LOG_DEBUG("lfg", "SMSG_LFG_UPDATE_STATUS {} updatetype: {}, party {}",
        GetPlayerInfo(), updateData.updateType, party ? "true" : "false");

    WorldPackets::LFG::LFGUpdateStatus lfgUpdateStatus;
    if (WorldPackets::LFG::RideTicket const* ticket = sLFGMgr->GetTicket(_player->GetGUID()))
        lfgUpdateStatus.Ticket = *ticket;

    lfgUpdateStatus.SubType = lfg::LFG_QUEUE_DUNGEON; // other types not implemented
    lfgUpdateStatus.Reason = updateData.updateType;
    std::transform(updateData.dungeons.begin(), updateData.dungeons.end(), std::back_inserter(lfgUpdateStatus.Slots), [](uint32 dungeonId)
        {
            return sLFGMgr->GetLFGDungeonEntry(dungeonId);
        });
    lfgUpdateStatus.RequestedRoles = sLFGMgr->GetRoles(_player->GetGUID());
    //lfgUpdateStatus.SuspendedPlayers;
    lfgUpdateStatus.IsParty = party;
    lfgUpdateStatus.NotifyUI = true;
    lfgUpdateStatus.Joined = join;
    lfgUpdateStatus.LfgJoined = updateData.updateType != lfg::LFG_UPDATETYPE_REMOVED_FROM_QUEUE;
    lfgUpdateStatus.Queued = queued;
    lfgUpdateStatus.QueueMapID = sLFGMgr->GetDungeonMapId(_player->GetGUID());

    SendPacket(lfgUpdateStatus.Write());
}

void WorldSession::SendLfgRoleChosen(ObjectGuid guid, uint8 roles)
{
    TC_LOG_DEBUG("lfg", "SMSG_LFG_ROLE_CHOSEN {} guid: {} roles: {}",
        GetPlayerInfo(), guid.ToString(), roles);

    WorldPackets::LFG::RoleChosen roleChosen;
    roleChosen.Player = guid;
    roleChosen.RoleMask = roles;
    roleChosen.Accepted = roles > 0;
    SendPacket(roleChosen.Write());
}

void WorldSession::SendLfgRoleCheckUpdate(lfg::LfgRoleCheck const& roleCheck)
{
    lfg::LfgDungeonSet dungeons;
    if (roleCheck.rDungeonId)
        dungeons.insert(roleCheck.rDungeonId);
    else
        dungeons = roleCheck.dungeons;

    TC_LOG_DEBUG("lfg", "SMSG_LFG_ROLE_CHECK_UPDATE {}", GetPlayerInfo());
    WorldPackets::LFG::LFGRoleCheckUpdate lfgRoleCheckUpdate;
    lfgRoleCheckUpdate.PartyIndex = 127;
    lfgRoleCheckUpdate.RoleCheckStatus = roleCheck.state;
    std::transform(dungeons.begin(), dungeons.end(), std::back_inserter(lfgRoleCheckUpdate.JoinSlots), [](uint32 dungeonId)
        {
            return sLFGMgr->GetLFGDungeonEntry(dungeonId);
        });
    lfgRoleCheckUpdate.GroupFinderActivityID = 0;
    if (!roleCheck.roles.empty())
    {
        // Leader info MUST be sent 1st :S
        uint8 roles = roleCheck.roles.find(roleCheck.leader)->second;
        lfgRoleCheckUpdate.Members.emplace_back(roleCheck.leader, roles, ASSERT_NOTNULL(sCharacterCache->GetCharacterCacheByGuid(roleCheck.leader))->Level, roles > 0);

        for (lfg::LfgRolesMap::const_iterator it = roleCheck.roles.begin(); it != roleCheck.roles.end(); ++it)
        {
            if (it->first == roleCheck.leader)
                continue;

            roles = it->second;
            lfgRoleCheckUpdate.Members.emplace_back(it->first, roles, ASSERT_NOTNULL(sCharacterCache->GetCharacterCacheByGuid(it->first))->Level, roles > 0);
        }
    }

    SendPacket(lfgRoleCheckUpdate.Write());
}

void WorldSession::SendLfgJoinResult(lfg::LfgJoinResultData const& joinData)
{
    TC_LOG_DEBUG("lfg", "SMSG_LFG_JOIN_RESULT {} checkResult: {} checkValue: {}",
        GetPlayerInfo(), joinData.result, joinData.state);

    WorldPackets::LFG::LFGJoinResult lfgJoinResult;
    if (WorldPackets::LFG::RideTicket const* ticket = sLFGMgr->GetTicket(GetPlayer()->GetGUID()))
        lfgJoinResult.Ticket = *ticket;
    lfgJoinResult.Result = joinData.result;
    if (joinData.result == lfg::LFG_JOIN_ROLE_CHECK_FAILED)
        lfgJoinResult.ResultDetail = joinData.state;
    else if (joinData.result == lfg::LFG_JOIN_NO_SLOTS)
        lfgJoinResult.BlackListNames = joinData.playersMissingRequirement;

    for (lfg::LfgLockPartyMap::const_iterator it = joinData.lockmap.begin(); it != joinData.lockmap.end(); ++it)
    {
        lfgJoinResult.BlackList.emplace_back();
        WorldPackets::LFG::LFGBlackList& blackList = lfgJoinResult.BlackList.back();
        blackList.PlayerGuid = it->first;

        for (lfg::LfgLockMap::const_iterator itr = it->second.begin(); itr != it->second.end(); ++itr)
        {
            TC_LOG_TRACE("lfg", "SendLfgJoinResult:: {} DungeonID: {} Lock status: {} Required itemLevel: {} Current itemLevel: {}",
                it->first.ToString(), (itr->first & 0x00FFFFFF), itr->second.lockStatus, itr->second.requiredItemLevel, itr->second.currentItemLevel);

            blackList.Slot.emplace_back(itr->first, itr->second.lockStatus, itr->second.requiredItemLevel, itr->second.currentItemLevel, 0);
        }
    }

    SendPacket(lfgJoinResult.Write());
}

void WorldSession::SendLfgQueueStatus(lfg::LfgQueueStatusData const& queueData)
{
    TC_LOG_DEBUG("lfg", "SMSG_LFG_QUEUE_STATUS {} state: {}, dungeon: {}, waitTime: {}, "
        "avgWaitTime: {}, waitTimeTanks: {}, waitTimeHealer: {}, waitTimeDps: {}, "
        "queuedTime: {}, tanks: {}, healers: {}, dps: {}",
        GetPlayerInfo(), lfg::GetStateString(sLFGMgr->GetState(GetPlayer()->GetGUID())), queueData.dungeonId, queueData.waitTime, queueData.waitTimeAvg,
        queueData.waitTimeTank, queueData.waitTimeHealer, queueData.waitTimeDps,
        queueData.queuedTime, queueData.tanks, queueData.healers, queueData.dps);

    WorldPackets::LFG::LFGQueueStatus lfgQueueStatus;
    if (WorldPackets::LFG::RideTicket const* ticket = sLFGMgr->GetTicket(GetPlayer()->GetGUID()))
        lfgQueueStatus.Ticket = *ticket;
    lfgQueueStatus.Slot = sLFGMgr->GetLFGDungeonEntry(queueData.dungeonId);
    lfgQueueStatus.AvgWaitTimeMe = queueData.waitTime;
    lfgQueueStatus.AvgWaitTime = queueData.waitTimeAvg;
    lfgQueueStatus.AvgWaitTimeByRole[0] = queueData.waitTimeTank;
    lfgQueueStatus.AvgWaitTimeByRole[1] = queueData.waitTimeHealer;
    lfgQueueStatus.AvgWaitTimeByRole[2] = queueData.waitTimeDps;
    lfgQueueStatus.LastNeeded[0] = queueData.tanks;
    lfgQueueStatus.LastNeeded[1] = queueData.healers;
    lfgQueueStatus.LastNeeded[2] = queueData.dps;
    lfgQueueStatus.QueuedTime = queueData.queuedTime;
    SendPacket(lfgQueueStatus.Write());
}

void WorldSession::SendLfgPlayerReward(lfg::LfgPlayerRewardData const& rewardData)
{
    if (!rewardData.rdungeonEntry || !rewardData.sdungeonEntry || !rewardData.quest)
        return;

    TC_LOG_DEBUG("lfg", "SMSG_LFG_PLAYER_REWARD {} rdungeonEntry: {}, sdungeonEntry: {}, done: {}",
        GetPlayerInfo(), rewardData.rdungeonEntry, rewardData.sdungeonEntry, rewardData.done);

    WorldPackets::LFG::LFGPlayerReward lfgPlayerReward;
    lfgPlayerReward.QueuedSlot = rewardData.rdungeonEntry;
    lfgPlayerReward.ActualSlot = rewardData.sdungeonEntry;
    lfgPlayerReward.RewardMoney = GetPlayer()->GetQuestMoneyReward(rewardData.quest);
    lfgPlayerReward.AddedXP = GetPlayer()->GetQuestXPReward(rewardData.quest);

    for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; ++i)
        if (uint32 itemId = rewardData.quest->RewardItemId[i])
            lfgPlayerReward.Rewards.emplace_back(itemId, rewardData.quest->RewardItemCount[i], 0, false);

    for (uint32 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
        if (uint32 curencyId = rewardData.quest->RewardCurrencyId[i])
            lfgPlayerReward.Rewards.emplace_back(curencyId, rewardData.quest->RewardCurrencyCount[i], 0, true);

    SendPacket(lfgPlayerReward.Write());
}

void WorldSession::SendLfgBootProposalUpdate(lfg::LfgPlayerBoot const& boot)
{
    lfg::LfgAnswer playerVote = boot.votes.find(GetPlayer()->GetGUID())->second;
    uint8 votesNum = 0;
    uint8 agreeNum = 0;
    int32 secsleft = int32((boot.cancelTime - GameTime::GetGameTime()) / 1000);
    for (const auto& vote : boot.votes)
    {
        if (vote.second != lfg::LFG_ANSWER_PENDING)
        {
            ++votesNum;
            if (vote.second == lfg::LFG_ANSWER_AGREE)
                ++agreeNum;
        }
    }
    TC_LOG_DEBUG("lfg", "SMSG_LFG_BOOT_PROPOSAL_UPDATE {} inProgress: {} - "
        "didVote: {} - agree: {} - victim: {} votes: {} - agrees: {} - left: {} - "
        "needed: {} - reason {}",
        GetPlayerInfo(), uint8(boot.inProgress), uint8(playerVote != lfg::LFG_ANSWER_PENDING),
        uint8(playerVote == lfg::LFG_ANSWER_AGREE), boot.victim.ToString(), votesNum, agreeNum,
        secsleft, lfg::LFG_GROUP_KICK_VOTES_NEEDED, boot.reason);

    WorldPackets::LFG::LfgBootPlayer lfgBootPlayer;
    lfgBootPlayer.Info.VoteInProgress = boot.inProgress;
    lfgBootPlayer.Info.VotePassed = agreeNum >= lfg::LFG_GROUP_KICK_VOTES_NEEDED;
    lfgBootPlayer.Info.MyVoteCompleted = playerVote != lfg::LFG_ANSWER_PENDING;
    lfgBootPlayer.Info.MyVote = playerVote == lfg::LFG_ANSWER_AGREE;
    lfgBootPlayer.Info.Target = boot.victim;
    lfgBootPlayer.Info.TotalVotes = votesNum;
    lfgBootPlayer.Info.BootVotes = agreeNum;
    lfgBootPlayer.Info.TimeLeft = secsleft;
    lfgBootPlayer.Info.VotesNeeded = lfg::LFG_GROUP_KICK_VOTES_NEEDED;
    lfgBootPlayer.Info.Reason = boot.reason;
    SendPacket(lfgBootPlayer.Write());
}

void WorldSession::SendLfgUpdateProposal(lfg::LfgProposal const& proposal)
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    ObjectGuid gguid = proposal.players.find(guid)->second.group;
    bool silent = !proposal.isNew && gguid == proposal.group;
    uint32 dungeonEntry = proposal.dungeonId;

    TC_LOG_DEBUG("lfg", "SMSG_LFG_PROPOSAL_UPDATE {} state: {}",
        GetPlayerInfo(), proposal.state);

    // show random dungeon if player selected random dungeon and it's not lfg group
    if (!silent)
    {
        lfg::LfgDungeonSet const& playerDungeons = sLFGMgr->GetSelectedDungeons(guid);
        if (playerDungeons.find(proposal.dungeonId) == playerDungeons.end())
            dungeonEntry = (*playerDungeons.begin());
    }

    WorldPackets::LFG::LFGProposalUpdate lfgProposalUpdate;
    if (WorldPackets::LFG::RideTicket const* ticket = sLFGMgr->GetTicket(GetPlayer()->GetGUID()))
        lfgProposalUpdate.Ticket = *ticket;
    lfgProposalUpdate.InstanceID = 0;
    lfgProposalUpdate.ProposalID = proposal.id;
    lfgProposalUpdate.Slot = sLFGMgr->GetLFGDungeonEntry(dungeonEntry);
    lfgProposalUpdate.State = proposal.state;
    lfgProposalUpdate.CompletedMask = proposal.encounters;
    lfgProposalUpdate.ValidCompletedMask = true;
    lfgProposalUpdate.ProposalSilent = silent;
    lfgProposalUpdate.IsRequeue = !proposal.isNew;

    for (auto const& player : proposal.players)
    {
        lfgProposalUpdate.Players.emplace_back();
        auto& proposalPlayer = lfgProposalUpdate.Players.back();
        proposalPlayer.Roles = player.second.role;
        proposalPlayer.Me = player.first == guid;
        proposalPlayer.MyParty = !player.second.group.IsEmpty() && player.second.group == proposal.group;
        proposalPlayer.SameParty = !player.second.group.IsEmpty() && player.second.group == gguid;
        proposalPlayer.Responded = player.second.accept != lfg::LFG_ANSWER_PENDING;
        proposalPlayer.Accepted = player.second.accept == lfg::LFG_ANSWER_AGREE;
    }

    SendPacket(lfgProposalUpdate.Write());
}

void WorldSession::SendLfgDisabled()
{
    TC_LOG_DEBUG("lfg", "SMSG_LFG_DISABLED {}", GetPlayerInfo());
    SendPacket(WorldPackets::LFG::LFGDisabled().Write());
}

void WorldSession::SendLfgOfferContinue(uint32 dungeonEntry)
{
    TC_LOG_DEBUG("lfg", "SMSG_LFG_OFFER_CONTINUE {} dungeon entry: {}",
        GetPlayerInfo(), dungeonEntry);
    SendPacket(WorldPackets::LFG::LFGOfferContinue(sLFGMgr->GetLFGDungeonEntry(dungeonEntry)).Write());
}

void WorldSession::SendLfgTeleportError(lfg::LfgTeleportResult err)
{
    TC_LOG_DEBUG("lfg", "SMSG_LFG_TELEPORT_DENIED {} reason: {}",
        GetPlayerInfo(), err);
    SendPacket(WorldPackets::LFG::LFGTeleportDenied(err).Write());
}

void WorldSession::HandleLFGListGetStatus(WorldPackets::LFGList::LFGListGetStatus& packet)
{
    WorldPackets::LFGList::LFGListUpdateStatus status;
    SendPacket(status.Write());
}

void WorldSession::HandleLFGListApplyToGroup(WorldPackets::LFGList::LFGListApplyToGroup& packet)
{
    if (GetPlayer()->GetGroup()) // need for rolecheck and result
        ChatHandler(GetPlayer()->GetSession()).PSendSysMessage("You can't join it while you in group!");
    else
        sLFGListMgr->OnPlayerApplyForGroup(GetPlayer(), &packet.application.ApplicationTicket, packet.application.ActivityID, packet.application.Comment, packet.application.Role);

}

void WorldSession::HandleLFGListCancelApplication(WorldPackets::LFGList::LFGListCancelApplication& packet)
{
    if (auto entry = sLFGListMgr->GetEntryByApplicant(packet.ApplicantTicket))
        sLFGListMgr->ChangeApplicantStatus(entry->GetApplicant(packet.ApplicantTicket.Id), LFGListApplicationStatus::Cancelled);
}

void WorldSession::HandleLFGListDeclineApplicant(WorldPackets::LFGList::LFGListDeclineApplicant& packet)
{
    if (!_player->GetGroup()->GetMemberFlags(_player->GetGUID()) && !_player->GetGroup()->IsLeader(_player->GetGUID()))
        return;

    if (auto entry = sLFGListMgr->GetEntrybyGuid(packet.ApplicantTicket.Id))
        sLFGListMgr->ChangeApplicantStatus(entry->GetApplicant(packet.ApplicationTicket.Id), LFGListApplicationStatus::Declined);
}

void WorldSession::HandleLFGListInviteApplicant(WorldPackets::LFGList::LFGListInviteApplicant& packet)
{
    if (!_player->GetGroup()->GetMemberFlags(_player->GetGUID()) && !_player->GetGroup()->IsLeader(_player->GetGUID()))
        return;

    if (auto entry = sLFGListMgr->GetEntrybyGuid(packet.ApplicantTicket.Id))
    {
        auto applicant = entry->GetApplicant(packet.ApplicationTicket.Id);
        applicant->RoleMask = (*packet.Applicant.begin()).Role;

        sLFGListMgr->ChangeApplicantStatus(applicant, LFGListApplicationStatus::Invited);
    }
}

void WorldSession::HandleLFGListInviteResponse(WorldPackets::LFGList::LFGListInviteResponse& packet)
{
    sLFGListMgr->ChangeApplicantStatus(sLFGListMgr->GetApplicationByID(packet.ApplicantTicket.Id), packet.Accept ? LFGListApplicationStatus::InviteAccepted : LFGListApplicationStatus::InviteDeclined);
}

void WorldSession::HandleLFGListJoin(WorldPackets::LFGList::LFGListJoin& packet)
{
    auto list = new LFGListEntry;
    list->GroupFinderActivityData = sGroupFinderActivityStore.LookupEntry(packet.Request.ActivityID);
    list->ItemLevel = packet.Request.ItemLevel;
    list->HonorLevel = packet.Request.HonorLevel;
    list->GroupName = packet.Request.GroupName;
    list->Comment = packet.Request.Comment;
    list->VoiceChat = packet.Request.VoiceChat;
    list->TypeActivity = packet.Request.TypeActivity;
    list->PrivateGroup = packet.Request.PrivateGroup;
    list->HasQuest = packet.Request.HasQuest;
    list->AutoAccept = packet.Request.AutoAccept;
    if (packet.Request.HasQuest)
        list->QuestID = *packet.Request.QuestID;
    list->minChallege = packet.Request.minChallege;
    if (list->minChallege)
        packet.Request.MinMyticPlusRating;
    list->ApplicationGroup = nullptr;
    ChatHandler(GetPlayer()->GetSession()).PSendSysMessage("GroupFinder Join");
    sLFGListMgr->Insert(list, GetPlayer());
}

void WorldSession::HandleLFGListLeave(WorldPackets::LFGList::LFGListLeave& packet)
{
    auto entry = sLFGListMgr->GetEntrybyGuid(packet.ApplicationTicket.Id);
    if (!entry || !entry->ApplicationGroup->IsLeader(GetPlayer()->GetGUID()))
        return;

    sLFGListMgr->Remove(packet.ApplicationTicket.Id, GetPlayer());
}

void WorldSession::HandleLFGListSearch(WorldPackets::LFGList::LFGListSearch& packet)
{
    WorldPackets::LFGList::LFGListSearchResults results;
    /*if (!sGroupFinderCategoryStore.LookupEntry(packet.CategoryID))
    {
        SendPacket(results.Write()); TOODO AD DB2
        return;
    }*/

    auto list = sLFGListMgr->GetFilteredList(packet.CategoryID, packet.SearchTerms, packet.LanguageSearchFilter, GetPlayer());
    results.AppicationsCount = list.size();

    for (auto& lfgEntry : list)
    {
        WorldPackets::LFGList::ListSearchResult result;
        auto group = lfgEntry->ApplicationGroup;
        if (!group)
            continue;

        auto leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());
        if (!leader)
            continue;

        if (lfgEntry->PrivateGroup)
            if (GetPlayer()->GetGuildId() == 0 || GetPlayer()->GetGuildId() != leader->GetGuildId())
                continue;

        auto activityID = lfgEntry->GroupFinderActivityData->ID;

        result.ApplicationTicket.RequesterGuid = group->GetGUID();
        result.ApplicationTicket.Id = group->GetGUID().GetCounter();
        result.ApplicationTicket.Type = WorldPackets::LFG::RideType::LfgListApplication;
        result.ApplicationTicket.Time = lfgEntry->CreationTime;
        result.LastTouchedVoiceChat = group->GetLeaderGUID();
        result.PartyGUID = group->GetLeaderGUID();
        result.BNetFriends = group->GetLeaderGUID();
        result.GuildMates = group->GetLeaderGUID();
        result.VirtualRealmAddress = GetVirtualRealmAddress();
        result.CompletedEncounters = 0;
        result.Age = lfgEntry->CreationTime;
        result.ResultID = 3;
        result.ApplicationStatus = AsUnderlyingType(LFGListApplicationStatus::None);

        for (auto const& member : group->GetMemberSlots())
        {
            uint8 role = member.roles >= 2 ? std::log2(member.roles) - 1 : member.roles;
            result.Members.emplace_back(member._class, role);
        }
        for (auto const& member : lfgEntry->ApplicationsContainer)
            if (auto applicant = member.second.GetPlayer())
                result.Members.emplace_back(applicant->GetClass(), member.second.RoleMask);

        result.JoinRequest.ActivityID = activityID;
        result.JoinRequest.ItemLevel = lfgEntry->ItemLevel;
        result.JoinRequest.AutoAccept = lfgEntry->AutoAccept;
        result.JoinRequest.TypeActivity = lfgEntry->TypeActivity;

        result.JoinRequest.HasQuest = lfgEntry->HasQuest;
        result.JoinRequest.GroupName = lfgEntry->GroupName;
        result.JoinRequest.Comment = lfgEntry->Comment;
        result.JoinRequest.VoiceChat = lfgEntry->VoiceChat;
        result.JoinRequest.minChallege = lfgEntry->minChallege;
        result.JoinRequest.PrivateGroup = lfgEntry->PrivateGroup;

        if (result.JoinRequest.HasQuest = true)
            result.JoinRequest.QuestID = lfgEntry->QuestID;

        if (result.JoinRequest.minChallege)
            result.JoinRequest.MinMyticPlusRating;


        results.SearchResults.emplace_back(result);
    }

    SendPacket(results.Write());
}

void WorldSession::HandleLFGListUpdateRequest(WorldPackets::LFGList::LFGListUpdateRequest& packet)
{
    auto entry = sLFGListMgr->GetEntrybyGuid(packet.Ticket.Id);
    if (!entry || !entry->ApplicationGroup->IsLeader(_player->GetGUID()))
        return;

    entry->AutoAccept = packet.UpdateRequest.AutoAccept;
    entry->GroupName = packet.UpdateRequest.GroupName;
    entry->Comment = packet.UpdateRequest.Comment;
    entry->VoiceChat = packet.UpdateRequest.VoiceChat;
    entry->HonorLevel = packet.UpdateRequest.HonorLevel;
    if (packet.UpdateRequest.HasQuest)
        entry->QuestID = *packet.UpdateRequest.QuestID;

    if (packet.UpdateRequest.ItemLevel < sLFGListMgr->GetPlayerItemLevelForActivity(entry->GroupFinderActivityData, _player))
        entry->ItemLevel = packet.UpdateRequest.ItemLevel;
    entry->PrivateGroup = packet.UpdateRequest.PrivateGroup;

    sLFGListMgr->AutoInviteApplicantsIfPossible(entry);
    sLFGListMgr->SendLFGListStatusUpdate(entry);
}

void WorldSession::HandleLfgCompleteReadyCheck(WorldPackets::LFG::CompleteReadyCheck& /*packet*/)
{
}

void WorldSession::HandleMythicPlusRequestWeeklyRewards(WorldPackets::MythicPlus::MythicPlusRequestWeeklyRewards& /*packet*/)
{
    WorldPackets::MythicPlus::Rewards weeklyResult;

    SendPacket(weeklyResult.Write());
}

void WorldSession::HandleLFGListClubFinderRequestPendingClubList(WorldPackets::LFGList::LFGListClubFinderRequestPendingClubList& /*packet*/)
{
//    packet.Queued;
}

void WorldSession::HandleRequestLFGListBlackList(WorldPackets::LFGList::RequestLFGListBlacklist& /*packet*/)
{
    WorldPackets::LFGList::LFGListUpdateBlacklist blacklist;
    SendPacket(blacklist.Write()); /// Activity and Reason loop - We dont need it
}
