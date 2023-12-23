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
#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "AchievementPackets.h"
#include "AreaTriggerPackets.h"
#include "Battleground.h"
#include "CharacterPackets.h"
#include "ChallengeModeMgr.h"
#include "Chat.h"
#include "CinematicMgr.h"
#include "ClientConfigPackets.h"
#include "Common.h"
#include "Conversation.h"
#include "Corpse.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "GameTime.h"
#include "GossipDef.h"
#include "Group.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "InstanceLockMgr.h"
#include "InstancePackets.h"
#include "InstanceScript.h"
#include "Item.h"
#include "Language.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "MiscPackets.h"
#include "MythicPlusPacketsCommon.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "OutdoorPvP.h"
#include "Player.h"
#include "RestMgr.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellPackets.h"
#include "WhoListStorage.h"
#include "WhoPackets.h"
#include "World.h"
#include <cstdarg>
#include <zlib.h>

// DekkCore >
#include "WorldPacket.h"
#include "MythicPlusPacketsCommon.h"
#include "GameObject.h"
#include <Contribution/ContributionMgr.h>
#include <ChallengeMode/Challenge.h>
#include <WorldStates/WorldStateMgr.h>
#ifdef ELUNA
#include "LuaEngine.h"
#endif
// < DekkCore

void WorldSession::HandleRepopRequest(WorldPackets::Misc::RepopRequest& /*packet*/)
{
    if (GetPlayer()->IsAlive() || GetPlayer()->HasPlayerFlag(PLAYER_FLAGS_GHOST))
        return;

    if (GetPlayer()->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
        return; // silently return, client should display the error by itself

    // the world update order is sessions, players, creatures
    // the netcode runs in parallel with all of these
    // creatures can kill players
    // so if the server is lagging enough the player can
    // release spirit after he's killed but before he is updated
    if (GetPlayer()->getDeathState() == JUST_DIED)
    {
        TC_LOG_DEBUG("network", "HandleRepopRequestOpcode: got request after player {} {} was killed and before he was updated",
            GetPlayer()->GetName(), GetPlayer()->GetGUID().ToString());
        GetPlayer()->KillPlayer();
    }

#ifdef ELUNA
    sEluna->OnRepop(GetPlayer());
#endif

    //this is spirit release confirm?
    GetPlayer()->RemovePet(nullptr, PET_SAVE_NOT_IN_SLOT, true);
    GetPlayer()->BuildPlayerRepop();
    GetPlayer()->RepopAtGraveyard();
}

void WorldSession::HandleWhoOpcode(WorldPackets::Who::WhoRequestPkt& whoRequest)
{
    WorldPackets::Who::WhoRequest& request = whoRequest.Request;

    TC_LOG_DEBUG("network", "WorldSession::HandleWhoOpcode: MinLevel: {}, MaxLevel: {}, Name: {} (VirtualRealmName: {}), Guild: {} (GuildVirtualRealmName: {}), RaceFilter: {}, ClassFilter: {}, Areas: {}, Words: {}.",
        request.MinLevel, request.MaxLevel, request.Name, request.VirtualRealmName, request.Guild, request.GuildVirtualRealmName,
        request.RaceFilter.RawValue, request.ClassFilter, whoRequest.Areas.size(), request.Words.size());

    // zones count, client limit = 10 (2.0.10)
    // can't be received from real client or broken packet
    if (whoRequest.Areas.size() > 10)
        return;

    // user entered strings count, client limit=4 (checked on 2.0.10)
    // can't be received from real client or broken packet
    if (request.Words.size() > 4)
        return;

    /// @todo: handle following packet values
    /// VirtualRealmNames
    /// ShowEnemies
    /// ShowArenaPlayers
    /// ExactName
    /// ServerInfo

    std::vector<std::wstring> wWords;
    wWords.resize(request.Words.size());
    for (size_t i = 0; i < request.Words.size(); ++i)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleWhoOpcode: Word: {}", request.Words[i].Word);

        // user entered string, it used as universal search pattern(guild+player name)?
        if (!Utf8toWStr(request.Words[i].Word, wWords[i]))
            continue;

        wstrToLower(wWords[i]);
    }

    std::wstring wPlayerName;
    std::wstring wGuildName;

    if (!(Utf8toWStr(request.Name, wPlayerName) && Utf8toWStr(request.Guild, wGuildName)))
        return;

    wstrToLower(wPlayerName);
    wstrToLower(wGuildName);

    // client send in case not set max level value 100 but Trinity supports 255 max level,
    // update it to show GMs with characters after 100 level
    if (whoRequest.Request.MaxLevel >= MAX_LEVEL)
        whoRequest.Request.MaxLevel = STRONG_MAX_LEVEL;

    uint32 team = _player->GetTeam();

    uint32 gmLevelInWhoList = sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST);

    WorldPackets::Who::WhoResponsePkt response;
    response.RequestID = whoRequest.RequestID;

    WhoListInfoVector const& whoList = sWhoListStorageMgr->GetWhoList();
    for (WhoListPlayerInfo const& target : whoList)
    {
        // player can see member of other team only if has RBAC_PERM_TWO_SIDE_WHO_LIST
        if (target.GetTeam() != team && !HasPermission(rbac::RBAC_PERM_TWO_SIDE_WHO_LIST))
            continue;

        // player can see MODERATOR, GAME MASTER, ADMINISTRATOR only if has RBAC_PERM_WHO_SEE_ALL_SEC_LEVELS
        if (target.GetSecurity() > AccountTypes(gmLevelInWhoList) && !HasPermission(rbac::RBAC_PERM_WHO_SEE_ALL_SEC_LEVELS))
            continue;

        // check if target is globally visible for player
        if (_player->GetGUID() != target.GetGuid() && !target.IsVisible())
            if (AccountMgr::IsPlayerAccount(_player->GetSession()->GetSecurity()) || target.GetSecurity() > _player->GetSession()->GetSecurity())
                continue;

        // check if target's level is in level range
        uint8 lvl = target.GetLevel();
        if (lvl < request.MinLevel || lvl > request.MaxLevel)
            continue;

        // check if class matches classmask
        if (request.ClassFilter >= 0 && !(request.ClassFilter & (1 << target.GetClass())))
            continue;

        // check if race matches racemask
        if (!request.RaceFilter.HasRace(target.GetRace()))
            continue;

        if (!whoRequest.Areas.empty())
        {
            if (std::find(whoRequest.Areas.begin(), whoRequest.Areas.end(), int32(target.GetZoneId())) == whoRequest.Areas.end())
                continue;
        }

        std::wstring const& wTargetName = target.GetWidePlayerName();
        if (!(wPlayerName.empty() || wTargetName.find(wPlayerName) != std::wstring::npos))
            continue;

        std::wstring const& wTargetGuildName = target.GetWideGuildName();

        if (!wGuildName.empty() && wTargetGuildName.find(wGuildName) == std::wstring::npos)
            continue;

        if (!wWords.empty())
        {
            std::string aName;
            if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(target.GetZoneId()))
                aName = areaEntry->AreaName[GetSessionDbcLocale()];

            bool show = false;
            for (size_t i = 0; i < wWords.size(); ++i)
            {
                if (!wWords[i].empty())
                {
                    if (wTargetName.find(wWords[i]) != std::wstring::npos ||
                        wTargetGuildName.find(wWords[i]) != std::wstring::npos ||
                        Utf8FitTo(aName, wWords[i]))
                    {
                        show = true;
                        break;
                    }
                }
            }

            if (!show)
                continue;
        }

        WorldPackets::Who::WhoEntry whoEntry;
        if (!whoEntry.PlayerData.Initialize(target.GetGuid(), nullptr))
            continue;

        if (!target.GetGuildGuid().IsEmpty())
        {
            whoEntry.GuildGUID = target.GetGuildGuid();
            whoEntry.GuildVirtualRealmAddress = GetVirtualRealmAddress();
            whoEntry.GuildName = target.GetGuildName();
        }

        whoEntry.AreaID = target.GetZoneId();
        whoEntry.IsGM = target.IsGameMaster();

        response.Response.Entries.push_back(whoEntry);

        // 50 is maximum player count sent to client - can be overridden
        // through config, but is unstable
        if (response.Response.Entries.size() >= sWorld->getIntConfig(CONFIG_MAX_WHO))
            break;
    }

    SendPacket(response.Write());
}

void WorldSession::HandleLogoutRequestOpcode(WorldPackets::Character::LogoutRequest& logoutRequest)
{
    if (!GetPlayer()->GetLootGUID().IsEmpty())
        GetPlayer()->SendLootReleaseAll();

    bool instantLogout = GetPlayer()->IsInFlight();
    if (!logoutRequest.IdleLogout)
        instantLogout |= (GetPlayer()->HasPlayerFlag(PLAYER_FLAGS_RESTING) && !GetPlayer()->IsInCombat())
        || HasPermission(rbac::RBAC_PERM_INSTANT_LOGOUT);

    /// TODO: Possibly add RBAC permission to log out in combat
    bool canLogoutInCombat = GetPlayer()->HasPlayerFlag(PLAYER_FLAGS_RESTING);

    uint32 reason = 0;
    if (GetPlayer()->IsInCombat() && !canLogoutInCombat)
        reason = 1;
    else if (GetPlayer()->IsFalling())
        reason = 3;                                         // is jumping or falling
    else if (GetPlayer()->duel || GetPlayer()->HasAura(9454)) // is dueling or frozen by GM via freeze command
        reason = 2;                                         // FIXME - Need the correct value

    WorldPackets::Character::LogoutResponse logoutResponse;
    logoutResponse.LogoutResult = reason;
    logoutResponse.Instant = instantLogout;
    SendPacket(logoutResponse.Write());

    if (reason)
    {
        SetLogoutStartTime(0);
        return;
    }

    // instant logout in taverns/cities or on taxi or for admins, gm's, mod's if its enabled in worldserver.conf
    if (instantLogout)
    {
        LogoutPlayer(true);
        return;
    }

    // not set flags if player can't free move to prevent lost state at logout cancel
    if (GetPlayer()->CanFreeMove())
    {
        if (GetPlayer()->GetStandState() == UNIT_STAND_STATE_STAND)
            GetPlayer()->SetStandState(UNIT_STAND_STATE_SIT);
        GetPlayer()->SetRooted(true);
        GetPlayer()->SetUnitFlag(UNIT_FLAG_STUNNED);
    }

    SetLogoutStartTime(GameTime::GetGameTime());
}

void WorldSession::HandleLogoutCancelOpcode(WorldPackets::Character::LogoutCancel& /*logoutCancel*/)
{
    // Player have already logged out serverside, too late to cancel
    if (!GetPlayer())
        return;

    SetLogoutStartTime(0);

    SendPacket(WorldPackets::Character::LogoutCancelAck().Write());

    // not remove flags if can't free move - its not set in Logout request code.
    if (GetPlayer()->CanFreeMove())
    {
        //!we can move again
        GetPlayer()->SetRooted(false);

        //! Stand Up
        GetPlayer()->SetStandState(UNIT_STAND_STATE_STAND);

        //! DISABLE_ROTATE
        GetPlayer()->RemoveUnitFlag(UNIT_FLAG_STUNNED);
    }
}

void WorldSession::HandleTogglePvP(WorldPackets::Misc::TogglePvP& /*packet*/)
{
    if (!GetPlayer()->HasPlayerFlag(PLAYER_FLAGS_IN_PVP))
    {
        GetPlayer()->SetPlayerFlag(PLAYER_FLAGS_IN_PVP);
        GetPlayer()->RemovePlayerFlag(PLAYER_FLAGS_PVP_TIMER);
        if (!GetPlayer()->IsPvP() || GetPlayer()->pvpInfo.EndTimer)
            GetPlayer()->UpdatePvP(true, true);
    }
    else if (!GetPlayer()->IsWarModeLocalActive())
    {
        GetPlayer()->RemovePlayerFlag(PLAYER_FLAGS_IN_PVP);
        GetPlayer()->SetPlayerFlag(PLAYER_FLAGS_PVP_TIMER);
        if (!GetPlayer()->pvpInfo.IsHostile && GetPlayer()->IsPvP())
            GetPlayer()->pvpInfo.EndTimer = GameTime::GetGameTime();     // start toggle-off
    }
}

void WorldSession::HandleSetPvP(WorldPackets::Misc::SetPvP& packet)
{
    if (packet.EnablePVP)
    {
        GetPlayer()->SetPlayerFlag(PLAYER_FLAGS_IN_PVP);
        GetPlayer()->RemovePlayerFlag(PLAYER_FLAGS_PVP_TIMER);
        if (!GetPlayer()->IsPvP() || GetPlayer()->pvpInfo.EndTimer)
            GetPlayer()->UpdatePvP(true, true);
    }
    else if (!GetPlayer()->IsWarModeLocalActive())
    {
        GetPlayer()->RemovePlayerFlag(PLAYER_FLAGS_IN_PVP);
        GetPlayer()->SetPlayerFlag(PLAYER_FLAGS_PVP_TIMER);
        if (!GetPlayer()->pvpInfo.IsHostile && GetPlayer()->IsPvP())
            GetPlayer()->pvpInfo.EndTimer = GameTime::GetGameTime();     // start toggle-off
    }
}

void WorldSession::HandleSetWarMode(WorldPackets::Misc::SetWarMode& packet)
{
    _player->SetWarModeDesired(packet.Enable);
}

void WorldSession::HandlePortGraveyard(WorldPackets::Misc::PortGraveyard& /*packet*/)
{
    if (GetPlayer()->IsAlive() || !GetPlayer()->HasPlayerFlag(PLAYER_FLAGS_GHOST))
        return;
    GetPlayer()->RepopAtGraveyard();
}

void WorldSession::HandleRequestCemeteryList(WorldPackets::Misc::RequestCemeteryList& /*packet*/)
{
    uint32 zoneId = _player->GetZoneId();
    uint32 team = _player->GetTeam();

    std::vector<uint32> graveyardIds;
    auto range = sObjectMgr->GraveyardStore.equal_range(zoneId);

    for (auto it = range.first; it != range.second && graveyardIds.size() < 16; ++it) // client max
    {
        ConditionSourceInfo conditionSource(_player);
        if (!sConditionMgr->IsObjectMeetToConditions(conditionSource, it->second.Conditions))
            continue;

        graveyardIds.push_back(it->first);
    }

    if (graveyardIds.empty())
    {
        TC_LOG_DEBUG("network", "No graveyards found for zone {} for {} (team {}) in CMSG_REQUEST_CEMETERY_LIST",
            zoneId, _player->GetGUID().ToString(), team);
        return;
    }

    WorldPackets::Misc::RequestCemeteryListResponse packet;
    packet.IsGossipTriggered = false;
    packet.CemeteryID.reserve(graveyardIds.size());

    for (uint32 id : graveyardIds)
        packet.CemeteryID.push_back(id);

    SendPacket(packet.Write());
}

void WorldSession::HandleSetSelectionOpcode(WorldPackets::Misc::SetSelection& packet)
{
    _player->SetSelection(packet.Selection);
}

void WorldSession::HandleStandStateChangeOpcode(WorldPackets::Misc::StandStateChange& packet)
{
    switch (packet.StandState)
    {
    case UNIT_STAND_STATE_STAND:
    case UNIT_STAND_STATE_SIT:
    case UNIT_STAND_STATE_SLEEP:
    case UNIT_STAND_STATE_KNEEL:
        break;
    default:
        return;
    }

    _player->SetStandState(packet.StandState);
}

void WorldSession::HandleReclaimCorpse(WorldPackets::Misc::ReclaimCorpse& /*packet*/)
{
    if (_player->IsAlive())
        return;

    // do not allow corpse reclaim in arena
    if (_player->InArena())
        return;

    // body not released yet
    if (!_player->HasPlayerFlag(PLAYER_FLAGS_GHOST))
        return;

    Corpse* corpse = _player->GetCorpse();
    if (!corpse)
        return;

    // prevent resurrect before 30-sec delay after body release not finished
    if (time_t(corpse->GetGhostTime() + _player->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP)) > time_t(GameTime::GetGameTime()))
        return;

    if (!corpse->IsWithinDistInMap(_player, CORPSE_RECLAIM_RADIUS, true))
        return;

    // resurrect
    _player->ResurrectPlayer(_player->InBattleground() ? 1.0f : 0.5f);

    // spawn bones
    _player->SpawnCorpseBones();
}

void WorldSession::HandleResurrectResponse(WorldPackets::Misc::ResurrectResponse& packet)
{
    if (GetPlayer()->IsAlive())
        return;

    if (packet.Response != 0) // Accept = 0 Decline = 1 Timeout = 2
    {
        GetPlayer()->ClearResurrectRequestData();           // reject
        return;
    }

    if (!GetPlayer()->IsResurrectRequestedBy(packet.Resurrecter))
        return;

    if (Player* ressPlayer = ObjectAccessor::GetPlayer(*GetPlayer(), packet.Resurrecter))
    {
        if (InstanceScript* instance = ressPlayer->GetInstanceScript())
        {
            if (instance->IsEncounterInProgress())
            {
                if (!instance->GetCombatResurrectionCharges())
                    return;
                else
                    instance->UseCombatResurrection();
            }
        }
    }

    GetPlayer()->ResurrectUsingRequestData();
}

void WorldSession::HandleAreaTriggerOpcode(WorldPackets::AreaTrigger::AreaTrigger& packet)
{
    Player* player = GetPlayer();
    if (player->IsInFlight())
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '{}' {} in flight, ignore Area Trigger ID:{}",
            player->GetName(), player->GetGUID().ToString(), packet.AreaTriggerID);
        return;
    }

    AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(packet.AreaTriggerID);
    if (!atEntry)
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '{}' {} send unknown (by DBC) Area Trigger ID:{}",
            player->GetName(), player->GetGUID().ToString(), packet.AreaTriggerID);
        return;
    }

    if (packet.Entered && !player->IsInAreaTriggerRadius(atEntry))
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '{}' {} too far, ignore Area Trigger ID: {}",
            player->GetName(), player->GetGUID().ToString(), packet.AreaTriggerID);
        return;
    }

    if (player->isDebugAreaTriggers)
        ChatHandler(player->GetSession()).PSendSysMessage(packet.Entered ? LANG_DEBUG_AREATRIGGER_ENTERED : LANG_DEBUG_AREATRIGGER_LEFT, packet.AreaTriggerID);

    if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_AREATRIGGER_CLIENT_TRIGGERED, atEntry->ID, player))
        return;

    if (sScriptMgr->OnAreaTrigger(player, atEntry, packet.Entered))
        return;

    if (player->IsAlive() && packet.Entered)
    {
        // not using Player::UpdateQuestObjectiveProgress, ObjectID in quest_objectives can be set to -1, areatrigger_involvedrelation then holds correct id
        if (std::unordered_set<uint32> const* quests = sObjectMgr->GetQuestsForAreaTrigger(packet.AreaTriggerID))
        {
            bool anyObjectiveChangedCompletionState = false;
            for (uint32 questId : *quests)
            {
                Quest const* qInfo = sObjectMgr->GetQuestTemplate(questId);
                uint16 slot = player->FindQuestSlot(questId);
                if (qInfo && slot < MAX_QUEST_LOG_SIZE && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                {
                    for (QuestObjective const& obj : qInfo->Objectives)
                    {
                        if (obj.Type != QUEST_OBJECTIVE_AREATRIGGER)
                            continue;

                        if (!player->IsQuestObjectiveCompletable(slot, qInfo, obj))
                            continue;

                        if (player->IsQuestObjectiveComplete(slot, qInfo, obj))
                            continue;

                        if (obj.ObjectID != -1 && obj.ObjectID != packet.AreaTriggerID)
                            continue;

                        player->SetQuestObjectiveData(obj, 1);
                        player->SendQuestUpdateAddCreditSimple(obj);
                        anyObjectiveChangedCompletionState = true;
                        break;
                    }

                    if (qInfo->HasFlag(QUEST_FLAGS_COMPLETION_AREA_TRIGGER))
                        player->AreaExploredOrEventHappens(questId);

                    if (player->CanCompleteQuest(questId))
                        player->CompleteQuest(questId);
                }
            }

            if (anyObjectiveChangedCompletionState)
                player->UpdateVisibleGameobjectsOrSpellClicks();
        }
    }

    if (sObjectMgr->IsTavernAreaTrigger(packet.AreaTriggerID))
    {
        // set resting flag we are in the inn
        if (packet.Entered)
            player->GetRestMgr().SetRestFlag(REST_FLAG_IN_TAVERN, atEntry->ID);
        else
            player->GetRestMgr().RemoveRestFlag(REST_FLAG_IN_TAVERN);

        if (sWorld->IsFFAPvPRealm())
        {
            if (packet.Entered)
                player->RemovePvpFlag(UNIT_BYTE2_FLAG_FFA_PVP);
            else
                player->SetPvpFlag(UNIT_BYTE2_FLAG_FFA_PVP);
        }

        return;
    }

    if (Battleground* bg = player->GetBattleground())
        bg->HandleAreaTrigger(player, packet.AreaTriggerID, packet.Entered);

    if (OutdoorPvP* pvp = player->GetOutdoorPvP())
        if (pvp->HandleAreaTrigger(_player, packet.AreaTriggerID, packet.Entered))
            return;

    if (!packet.Entered)
        return;

    AreaTriggerStruct const* at = sObjectMgr->GetAreaTrigger(packet.AreaTriggerID);
    if (!at)
        return;

    bool teleported = false;
    if (player->GetMapId() != at->target_mapId)
    {
        if (!player->IsAlive())
        {
            if (player->HasCorpse())
            {
                // let enter in ghost mode in instance that connected to inner instance with corpse
                uint32 corpseMap = player->GetCorpseLocation().GetMapId();
                do
                {
                    if (corpseMap == at->target_mapId)
                        break;

                    InstanceTemplate const* corpseInstance = sObjectMgr->GetInstanceTemplate(corpseMap);
                    corpseMap = corpseInstance ? corpseInstance->Parent : 0;
                } while (corpseMap);

                if (!corpseMap)
                {
                    SendPacket(WorldPackets::AreaTrigger::AreaTriggerNoCorpse().Write());
                    return;
                }

                TC_LOG_DEBUG("maps", "MAP: Player '{}' has corpse in instance {} and can enter.", player->GetName(), at->target_mapId);
            }
            else
                TC_LOG_DEBUG("maps", "Map::CanPlayerEnter - player '{}' is dead but does not have a corpse!", player->GetName());
        }

        if (TransferAbortParams denyReason = Map::PlayerCannotEnter(at->target_mapId, player))
        {
            switch (denyReason.Reason)
            {
            case TRANSFER_ABORT_MAP_NOT_ALLOWED:
                TC_LOG_DEBUG("maps", "MAP: Player '{}' attempted to enter map with id {} which has no entry", player->GetName(), at->target_mapId);
                break;
            case TRANSFER_ABORT_DIFFICULTY:
                TC_LOG_DEBUG("maps", "MAP: Player '{}' attempted to enter instance map {} but the requested difficulty was not found", player->GetName(), at->target_mapId);
                break;
            case TRANSFER_ABORT_NEED_GROUP:
                TC_LOG_DEBUG("maps", "MAP: Player '{}' must be in a raid group to enter map {}", player->GetName(), at->target_mapId);
                player->SendRaidGroupOnlyMessage(RAID_GROUP_ERR_ONLY, 0);
                break;
            case TRANSFER_ABORT_LOCKED_TO_DIFFERENT_INSTANCE:
                TC_LOG_DEBUG("maps", "MAP: Player '{}' cannot enter instance map {} because their permanent bind is incompatible with their group's", player->GetName(), at->target_mapId);
                break;
            case TRANSFER_ABORT_ALREADY_COMPLETED_ENCOUNTER:
                TC_LOG_DEBUG("maps", "MAP: Player '{}' cannot enter instance map {} because their permanent bind is incompatible with their group's", player->GetName(), at->target_mapId);
                break;
            case TRANSFER_ABORT_TOO_MANY_INSTANCES:
                TC_LOG_DEBUG("maps", "MAP: Player '{}' cannot enter instance map {} because he has exceeded the maximum number of instances per hour.", player->GetName(), at->target_mapId);
                break;
            case TRANSFER_ABORT_MAX_PLAYERS:
                break;
            case TRANSFER_ABORT_ZONE_IN_COMBAT:
                break;
            case TRANSFER_ABORT_NOT_FOUND:
                TC_LOG_DEBUG("maps", "MAP: Player '{}' cannot enter instance map {} because instance is resetting.", player->GetName(), at->target_mapId);
                break;
            default:
                break;
            }

            if (denyReason.Reason != TRANSFER_ABORT_NEED_GROUP)
                player->SendTransferAborted(at->target_mapId, denyReason.Reason, denyReason.Arg, denyReason.MapDifficultyXConditionId);

            if (!player->IsAlive() && player->HasCorpse())
            {
                if (player->GetCorpseLocation().GetMapId() == at->target_mapId)
                {
                    player->ResurrectPlayer(0.5f);
                    player->SpawnCorpseBones();
                }
            }

            return;
        }

        if (Group* group = player->GetGroup())
            if (group->isLFGGroup() && player->GetMap()->IsDungeon())
                teleported = player->TeleportToBGEntryPoint();
    }

    if (!teleported)
    {
        WorldSafeLocsEntry const* entranceLocation = nullptr;
        MapEntry const* mapEntry = sMapStore.AssertEntry(at->target_mapId);
        if (mapEntry->Instanceable())
        {
            // Check if we can contact the instancescript of the instance for an updated entrance location
            if (uint32 targetInstanceId = sMapMgr->FindInstanceIdForPlayer(at->target_mapId, _player))
                if (Map* map = sMapMgr->FindMap(at->target_mapId, targetInstanceId))
                    if (InstanceMap* instanceMap = map->ToInstanceMap())
                        if (InstanceScript* instanceScript = instanceMap->GetInstanceScript())
                            entranceLocation = sObjectMgr->GetWorldSafeLoc(instanceScript->GetEntranceLocation());

            // Finally check with the instancesave for an entrance location if we did not get a valid one from the instancescript
            if (!entranceLocation)
            {
                Group* group = player->GetGroup();
                Difficulty difficulty = group ? group->GetDifficultyID(mapEntry) : player->GetDifficultyID(mapEntry);
                ObjectGuid instanceOwnerGuid = group ? group->GetRecentInstanceOwner(at->target_mapId) : player->GetGUID();
                if (InstanceLock const* instanceLock = sInstanceLockMgr.FindActiveInstanceLock(instanceOwnerGuid, { mapEntry, sDB2Manager.GetDownscaledMapDifficultyData(at->target_mapId, difficulty) }))
                    entranceLocation = sObjectMgr->GetWorldSafeLoc(instanceLock->GetData()->EntranceWorldSafeLocId);
            }
        }

        if (entranceLocation)
            player->TeleportTo(entranceLocation->Loc, TELE_TO_NOT_LEAVE_TRANSPORT);
        else
            player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
    }
}

void WorldSession::HandleUpdateAccountData(WorldPackets::ClientConfig::UserClientUpdateAccountData& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_UPDATE_ACCOUNT_DATA: type {}, time {}, decompressedSize {}",
        packet.DataType, packet.Time.AsUnderlyingType(), packet.Size);

    if (packet.DataType >= NUM_ACCOUNT_DATA_TYPES)
        return;

    if (packet.Size == 0)                               // erase
    {
        SetAccountData(AccountDataType(packet.DataType), 0, "");
        return;
    }

    if (packet.Size > 0xFFFF)
    {
        TC_LOG_ERROR("network", "UAD: Account data packet too big, size {}", packet.Size);
        return;
    }

    ByteBuffer dest(packet.Size, ByteBuffer::Resize{});

    uLongf realSize = packet.Size;
    if (uncompress(dest.contents(), &realSize, packet.CompressedData.contents(), packet.CompressedData.size()) != Z_OK)
    {
        TC_LOG_ERROR("network", "UAD: Failed to decompress account data");
        return;
    }

    std::string adata;
    dest >> adata;

    SetAccountData(AccountDataType(packet.DataType), packet.Time, adata);
}

void WorldSession::HandleRequestAccountData(WorldPackets::ClientConfig::RequestAccountData& request)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_REQUEST_ACCOUNT_DATA: type {}", request.DataType);

    if (request.DataType >= NUM_ACCOUNT_DATA_TYPES)
        return;

    AccountData const* adata = GetAccountData(AccountDataType(request.DataType));

    WorldPackets::ClientConfig::UpdateAccountData data;
    data.Player = _player ? _player->GetGUID() : ObjectGuid::Empty;
    data.Time = adata->Time;
    data.Size = adata->Data.size();
    data.DataType = request.DataType;

    uLongf destSize = compressBound(data.Size);

    data.CompressedData.resize(destSize);

    if (data.Size && compress(data.CompressedData.contents(), &destSize, (uint8 const*)adata->Data.c_str(), data.Size) != Z_OK)
    {
        TC_LOG_ERROR("network", "RAD: Failed to compress account data");
        return;
    }

    data.CompressedData.resize(destSize);

    SendPacket(data.Write());
}

void WorldSession::HandleSetActionButtonOpcode(WorldPackets::Spells::SetActionButton& packet)
{
    uint64 action = ACTION_BUTTON_ACTION(packet.Action);
    uint8 type = ACTION_BUTTON_TYPE(packet.Action);

    TC_LOG_DEBUG("network", "CMSG_SET_ACTION_BUTTON Button: {} Action: {} Type: {}", packet.Index, action, uint32(type));

    if (!packet.Action)
        GetPlayer()->RemoveActionButton(packet.Index);
    else
        GetPlayer()->AddActionButton(packet.Index, action, type);
}

void WorldSession::HandleCompleteCinematic(WorldPackets::Misc::CompleteCinematic& /*packet*/)
{
    // If player has sight bound to visual waypoint NPC we should remove it
    GetPlayer()->GetCinematicMgr()->EndCinematic();
}

void WorldSession::HandleNextCinematicCamera(WorldPackets::Misc::NextCinematicCamera& /*packet*/)
{
    // Sent by client when cinematic actually begun. So we begin the server side process
    GetPlayer()->GetCinematicMgr()->NextCinematicCamera();
}

void WorldSession::HandleCompleteMovie(WorldPackets::Misc::CompleteMovie& /*packet*/)
{
    uint32 movie = _player->GetMovie();
    if (!movie)
        return;

    _player->SetMovie(0);
    sScriptMgr->OnMovieComplete(_player, movie);
}

void WorldSession::HandleSetActionBarToggles(WorldPackets::Character::SetActionBarToggles& packet)
{
    if (!GetPlayer())                                        // ignore until not logged (check needed because STATUS_AUTHED)
    {
        if (packet.Mask != 0)
            TC_LOG_ERROR("network", "WorldSession::HandleSetActionBarToggles in not logged state with value: {}, ignored", uint32(packet.Mask));
        return;
    }

    GetPlayer()->SetMultiActionBars(packet.Mask);
}

void WorldSession::HandlePlayedTime(WorldPackets::Character::RequestPlayedTime& packet)
{
    WorldPackets::Character::PlayedTime playedTime;
    playedTime.TotalTime = _player->GetTotalPlayedTime();
    playedTime.LevelTime = _player->GetLevelPlayedTime();
    playedTime.TriggerEvent = packet.TriggerScriptEvent;  // 0-1 - will not show in chat frame
    SendPacket(playedTime.Write());
}

void WorldSession::HandleWhoIsOpcode(WorldPackets::Who::WhoIsRequest& packet)
{
    TC_LOG_DEBUG("network", "Received whois command from player {} for character {}",
        GetPlayer()->GetName(), packet.CharName);

    if (!HasPermission(rbac::RBAC_PERM_OPCODE_WHOIS))
    {
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
        return;
    }

    if (packet.CharName.empty() || !normalizePlayerName(packet.CharName))
    {
        SendNotification(LANG_NEED_CHARACTER_NAME);
        return;
    }

    Player* player = ObjectAccessor::FindConnectedPlayerByName(packet.CharName);
    if (!player)
    {
        SendNotification(LANG_PLAYER_NOT_EXIST_OR_OFFLINE, packet.CharName.c_str());
        return;
    }

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WHOIS);
    stmt->setUInt32(0, player->GetSession()->GetAccountId());

    PreparedQueryResult result = LoginDatabase.Query(stmt);
    if (!result)
    {
        SendNotification(LANG_ACCOUNT_FOR_PLAYER_NOT_FOUND, packet.CharName.c_str());
        return;
    }

    Field* fields = result->Fetch();
    std::string acc = fields[0].GetString();
    if (acc.empty())
        acc = "Unknown";

    std::string email = fields[1].GetString();
    if (email.empty())
        email = "Unknown";

    std::string lastip = fields[2].GetString();
    if (lastip.empty())
        lastip = "Unknown";

    WorldPackets::Who::WhoIsResponse response;
    response.AccountName = packet.CharName + "'s " + "account is " + acc + ", e-mail: " + email + ", last ip: " + lastip;
    SendPacket(response.Write());
}

void WorldSession::HandleFarSightOpcode(WorldPackets::Misc::FarSight& packet)
{
    if (packet.Enable)
    {
        TC_LOG_DEBUG("network", "Added FarSight {} to player {}", _player->m_activePlayerData->FarsightObject->ToString(), _player->GetGUID().ToString());
        if (WorldObject* target = _player->GetViewpoint())
            _player->SetSeer(target);
        else
            TC_LOG_DEBUG("network", "Player {} {} requests non-existing seer {}", _player->GetName(), _player->GetGUID().ToString(), _player->m_activePlayerData->FarsightObject->ToString());
    }
    else
    {
        TC_LOG_DEBUG("network", "Player {} set vision to self", _player->GetGUID().ToString());
        _player->SetSeer(_player);
    }

    GetPlayer()->UpdateVisibilityForPlayer();
}

void WorldSession::HandleSetTitleOpcode(WorldPackets::Character::SetTitle& packet)
{
    // -1 at none
    if (packet.TitleID > 0)
    {
        if (!GetPlayer()->HasTitle(packet.TitleID))
            return;
    }
    else
        packet.TitleID = 0;

    GetPlayer()->SetChosenTitle(packet.TitleID);
}

void WorldSession::HandleResetInstancesOpcode(WorldPackets::Instance::ResetInstances& /*packet*/)
{
    Map* map = _player->FindMap();
    if (map && map->Instanceable())
        return;

    if (Group* group = _player->GetGroup())
    {
        if (!group->IsLeader(_player->GetGUID()))
            return;

        if (group->isLFGGroup())
            return;

        group->ResetInstances(InstanceResetMethod::Manual, _player);
    }
    else
        _player->ResetInstances(InstanceResetMethod::Manual);
}

void WorldSession::HandleSetDungeonDifficultyOpcode(WorldPackets::Misc::SetDungeonDifficulty& setDungeonDifficulty)
{
    DifficultyEntry const* difficultyEntry = sDifficultyStore.LookupEntry(setDungeonDifficulty.DifficultyID);
    if (!difficultyEntry)
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetDungeonDifficultyOpcode: {} sent an invalid instance mode {}!",
            _player->GetGUID().ToString(), setDungeonDifficulty.DifficultyID);
        return;
    }

    if (difficultyEntry->InstanceType != MAP_INSTANCE)
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetDungeonDifficultyOpcode: {} sent an non-dungeon instance mode {}!",
            _player->GetGUID().ToString(), difficultyEntry->ID);
        return;
    }

    if (!(difficultyEntry->Flags & DIFFICULTY_FLAG_CAN_SELECT))
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player {} sent unselectable instance mode {}!",
            _player->GetGUID().ToString(), difficultyEntry->ID);
        return;
    }

    Difficulty difficultyID = Difficulty(difficultyEntry->ID);
    if (difficultyID == _player->GetDungeonDifficultyID())
        return;

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->Instanceable())
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player (Name: {}, {}) tried to reset the instance while player is inside!",
            _player->GetName(), _player->GetGUID().ToString());
        return;
    }

    Group* group = _player->GetGroup();
    if (group)
    {
        if (!group->IsLeader(_player->GetGUID()))
            return;

        if (group->isLFGGroup())
            return;

        // the difficulty is set even if the instances can't be reset
        group->ResetInstances(InstanceResetMethod::OnChangeDifficulty, _player);
        group->SetDungeonDifficultyID(difficultyID);
    }
    else
    {
        _player->ResetInstances(InstanceResetMethod::OnChangeDifficulty);
        _player->SetDungeonDifficultyID(difficultyID);
        _player->SendDungeonDifficulty();
    }
}

void WorldSession::HandleSetRaidDifficultyOpcode(WorldPackets::Misc::SetRaidDifficulty& setRaidDifficulty)
{
    DifficultyEntry const* difficultyEntry = sDifficultyStore.LookupEntry(setRaidDifficulty.DifficultyID);
    if (!difficultyEntry)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleSetDungeonDifficultyOpcode: {} sent an invalid instance mode {}!",
            _player->GetGUID().ToString(), setRaidDifficulty.DifficultyID);
        return;
    }

    if (difficultyEntry->InstanceType != MAP_RAID)
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetDungeonDifficultyOpcode: {} sent an non-dungeon instance mode {}!",
            _player->GetGUID().ToString(), difficultyEntry->ID);
        return;
    }

    if (!(difficultyEntry->Flags & DIFFICULTY_FLAG_CAN_SELECT))
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player {} sent unselectable instance mode {}!",
            _player->GetGUID().ToString(), difficultyEntry->ID);
        return;
    }

    if (((difficultyEntry->Flags & DIFFICULTY_FLAG_LEGACY) != 0) != setRaidDifficulty.Legacy)
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetDungeonDifficultyOpcode: {} sent not matching legacy difficulty {}!",
            _player->GetGUID().ToString(), difficultyEntry->ID);
        return;
    }

    Difficulty difficultyID = Difficulty(difficultyEntry->ID);
    if (difficultyID == (setRaidDifficulty.Legacy ? _player->GetLegacyRaidDifficultyID() : _player->GetRaidDifficultyID()))
        return;

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->Instanceable())
    {
        TC_LOG_INFO("network", "WorldSession::HandleSetRaidDifficultyOpcode: player (Name: {}, {}) tried to reset the instance while player is inside!",
            _player->GetName(), _player->GetGUID().ToString());
        return;
    }

    Group* group = _player->GetGroup();
    if (group)
    {
        if (!group->IsLeader(_player->GetGUID()))
            return;

        if (group->isLFGGroup())
            return;

        // the difficulty is set even if the instances can't be reset
        group->ResetInstances(InstanceResetMethod::OnChangeDifficulty, _player);
        if (setRaidDifficulty.Legacy)
            group->SetLegacyRaidDifficultyID(difficultyID);
        else
            group->SetRaidDifficultyID(difficultyID);
    }
    else
    {
        _player->ResetInstances(InstanceResetMethod::OnChangeDifficulty);
        if (setRaidDifficulty.Legacy)
            _player->SetLegacyRaidDifficultyID(difficultyID);
        else
            _player->SetRaidDifficultyID(difficultyID);

        _player->SendRaidDifficulty(setRaidDifficulty.Legacy != 0);
    }
}

void WorldSession::HandleSetTaxiBenchmark(WorldPackets::Misc::SetTaxiBenchmarkMode& packet)
{
    if (packet.Enable)
        _player->SetPlayerFlag(PLAYER_FLAGS_TAXI_BENCHMARK);
    else
        _player->RemovePlayerFlag(PLAYER_FLAGS_TAXI_BENCHMARK);
}

void WorldSession::HandleGuildSetFocusedAchievement(WorldPackets::Achievement::GuildSetFocusedAchievement& setFocusedAchievement)
{
    if (Guild* guild = sGuildMgr->GetGuildById(_player->GetGuildId()))
        guild->GetAchievementMgr().SendAchievementInfo(_player, setFocusedAchievement.AchievementID);
}

void WorldSession::HandleServerTimeOffsetRequest(WorldPackets::Misc::ServerTimeOffsetRequest& /*request*/)
{
    WorldPackets::Misc::ServerTimeOffset response;
    response.Time = GameTime::GetSystemTime();
    SendPacket(response.Write());
}

void WorldSession::HandleInstanceLockResponse(WorldPackets::Instance::InstanceLockResponse& packet)
{
    if (!_player->HasPendingBind())
    {
        TC_LOG_INFO("network", "InstanceLockResponse: Player {} {} tried to bind himself/teleport to graveyard without a pending bind!",
            _player->GetName(), _player->GetGUID().ToString());
        return;
    }

    if (packet.AcceptLock)
        _player->ConfirmPendingBind();
    else
        _player->RepopAtGraveyard();

    _player->SetPendingBind(0, 0);
}

void WorldSession::HandleViolenceLevel(WorldPackets::Misc::ViolenceLevel& /*violenceLevel*/)
{
    // do something?
}

void WorldSession::HandleObjectUpdateFailedOpcode(WorldPackets::Misc::ObjectUpdateFailed& objectUpdateFailed)
{
    TC_LOG_ERROR("network", "Object update failed for {} for player {} ({})", objectUpdateFailed.ObjectGUID.ToString(), GetPlayerName(), _player->GetGUID().ToString());

    // If create object failed for current player then client will be stuck on loading screen
    if (_player->GetGUID() == objectUpdateFailed.ObjectGUID)
    {
        LogoutPlayer(true);
        return;
    }

    // Pretend we've never seen this object
    _player->m_clientGUIDs.erase(objectUpdateFailed.ObjectGUID);
}

void WorldSession::HandleObjectUpdateRescuedOpcode(WorldPackets::Misc::ObjectUpdateRescued& objectUpdateRescued)
{
    TC_LOG_ERROR("network", "Object update rescued for {} for player {} ({})", objectUpdateRescued.ObjectGUID.ToString(), GetPlayerName(), _player->GetGUID().ToString());

    // Client received values update after destroying object
    // re-register object in m_clientGUIDs to send DestroyObject on next visibility update
    _player->m_clientGUIDs.insert(objectUpdateRescued.ObjectGUID);
}

void WorldSession::HandleSaveCUFProfiles(WorldPackets::Misc::SaveCUFProfiles& packet)
{
    if (packet.CUFProfiles.size() > MAX_CUF_PROFILES)
    {
        TC_LOG_ERROR("entities.player", "HandleSaveCUFProfiles - {} tried to save more than {} CUF profiles. Hacking attempt?", GetPlayerName(), MAX_CUF_PROFILES);
        return;
    }

    for (uint8 i = 0; i < packet.CUFProfiles.size(); ++i)
        GetPlayer()->SaveCUFProfile(i, std::move(packet.CUFProfiles[i]));

    for (uint8 i = packet.CUFProfiles.size(); i < MAX_CUF_PROFILES; ++i)
        GetPlayer()->SaveCUFProfile(i, nullptr);
}

void WorldSession::SendLoadCUFProfiles()
{
    Player* player = GetPlayer();

    WorldPackets::Misc::LoadCUFProfiles loadCUFProfiles;

    for (uint8 i = 0; i < MAX_CUF_PROFILES; i++)
        if (CUFProfile* cufProfile = player->GetCUFProfile(i))
            loadCUFProfiles.CUFProfiles.push_back(cufProfile);
    SendPacket(loadCUFProfiles.Write());
}

void WorldSession::HandleSetAdvancedCombatLogging(WorldPackets::ClientConfig::SetAdvancedCombatLogging& setAdvancedCombatLogging)
{
    _player->SetAdvancedCombatLogging(setAdvancedCombatLogging.Enable);
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPackets::Misc::MountSpecial& mountSpecial)
{
    WorldPackets::Misc::SpecialMountAnim specialMountAnim;
    specialMountAnim.UnitGUID = _player->GetGUID();
    std::copy(mountSpecial.SpellVisualKitIDs.begin(), mountSpecial.SpellVisualKitIDs.end(), std::back_inserter(specialMountAnim.SpellVisualKitIDs));
    specialMountAnim.SequenceVariation = mountSpecial.SequenceVariation;
    GetPlayer()->SendMessageToSet(specialMountAnim.Write(), false);
}

void WorldSession::HandleMountSetFavorite(WorldPackets::Misc::MountSetFavorite& mountSetFavorite)
{
    _collectionMgr->MountSetFavorite(mountSetFavorite.MountSpellID, mountSetFavorite.IsFavorite);
}

void WorldSession::HandleCloseInteraction(WorldPackets::Misc::CloseInteraction& closeInteraction)
{
    if (_player->PlayerTalkClass->GetInteractionData().SourceGuid == closeInteraction.SourceGuid)
        _player->PlayerTalkClass->GetInteractionData().Reset();

    if (_player->GetStableMaster() == closeInteraction.SourceGuid)
        _player->SetStableMaster(ObjectGuid::Empty);
}

void WorldSession::HandleConversationLineStarted(WorldPackets::Misc::ConversationLineStarted& conversationLineStarted)
{
    if (Conversation* convo = ObjectAccessor::GetConversation(*_player, conversationLineStarted.ConversationGUID))
        sScriptMgr->OnConversationLineStarted(convo, conversationLineStarted.LineID, _player);
}

void WorldSession::HandleRequestLatestSplashScreen(WorldPackets::Misc::RequestLatestSplashScreen& /*requestLatestSplashScreen*/)
{
    UISplashScreenEntry const* splashScreen = nullptr;
    for (auto itr = sUISplashScreenStore.begin(); itr != sUISplashScreenStore.end(); ++itr)
    {
        if (PlayerConditionEntry const* playerCondition = sPlayerConditionStore.LookupEntry(itr->CharLevelConditionID))
            if (!ConditionMgr::IsPlayerMeetingCondition(_player, playerCondition))
                continue;

        splashScreen = *itr;
    }

    WorldPackets::Misc::SplashScreenShowLatest splashScreenShowLatest;
    splashScreenShowLatest.UISplashScreenID = splashScreen ? splashScreen->ID : 0;
    SendPacket(splashScreenShowLatest.Write());
}

void WorldSession::HandleRequestCovenantCallings(WorldPackets::Misc::RequestCovenantCallings& packet)
{
    WorldPackets::Misc::CovenantCallingAvailibilityResponse send;
    send.Availability = true;
    send.Index = 0;
    send.Data = 208; // Index = 1
    SendPacket(send.Write());
}

void WorldSession::HandleCovenantRenownRequestCatchupState(WorldPackets::Misc::CovenantRenownRequestCatchupState& packet)
{
    WorldPackets::Misc::CovenantRenownSendCatchUpState send;
    send.CatchupState = false;
    SendPacket(send.Write());
}

void WorldSession::HandleAssignEquipmentSetSpec(WorldPackets::EquipmentSet::AssignEquipmentSetSpec& packet)
{
}

void WorldSession::HandleChallengeModeStart(WorldPackets::Misc::StartRequest& start)
{
    GameObject* object = _player->GetGameObjectIfCanInteractWith(start.GameObjectGUID, GAMEOBJECT_TYPE_KEYSTONE_RECEPTACLE);
    if (!object)
    {
        TC_LOG_INFO("network", "WORLD: HandleChallengeModeStart - {} not found or you can not interact with it.", start.GameObjectGUID.ToString());
        return;
    }

    Item* key = _player->GetItemByPos(start.Bag, start.Slot);
    if (!key)
    {
        TC_LOG_INFO("network", "WORLD: HandleChallengeModeStart - item in Bag {} and Slot {} not found.", start.Bag, start.Slot);
        return;
    }

    if (key->GetTemplate()->GetClass() != ITEM_CLASS_REAGENT || key->GetTemplate()->GetSubClass() != ITEM_SUBCLASS_KEYSTONE)
    {
        TC_LOG_INFO("network", "WORLD: HandleChallengeModeStart - Tried to start a challenge with item {} which have class {} and subclass {}.",
            key->GetGUID().ToString(),
            key->GetTemplate()->GetClass(),
            key->GetTemplate()->GetSubClass());
        return;
    }

    uint32 challengeModeId = key->GetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID);
    uint32 challengeModeLevel = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL);
    uint32 challengeModeAffix1 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1);
    uint32 challengeModeAffix2 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2);
    uint32 challengeModeAffix3 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3);
    uint32 challengeModeAffix4 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_4);

    MapChallengeModeEntry const* entry = sMapChallengeModeStore.LookupEntry(challengeModeId);
    if (!entry || !challengeModeLevel || entry->MapID != _player->GetMapId())
    {
        TC_LOG_INFO("network", "WORLD: HandleChallengeModeStart - Tried to start a challenge with wrong challengeModeId {} and level {}.", challengeModeId, challengeModeLevel);
        return;
    }

    if (InstanceScript* instanceScript = _player->GetInstanceScript())
        instanceScript->StartChallengeMode(challengeModeId, challengeModeLevel, challengeModeAffix1, challengeModeAffix2, challengeModeAffix3, challengeModeAffix4);

    // Blizzard do not delete the key at challenge start, will require mort research
    _player->DestroyItem(start.Bag, start.Slot, true);
}


void WorldSession::HandleRequestLeaders(WorldPackets::Misc::RequestLeaders& packet)
{
    WorldPackets::Misc::RequestLeadersResult result;
    result.MapID = packet.MapId;
    result.ChallengeID = packet.ChallengeID;

    result.LastGuildUpdate = time(nullptr);
    result.LastRealmUpdate = time(nullptr);

    if (auto bestGuild = sChallengeModeMgr->BestGuildChallenge(_player->GetGuildId(), packet.ChallengeID))
    {
        for (auto itr = bestGuild->member.begin(); itr != bestGuild->member.end(); ++itr)
        {
            WorldPackets::Misc::ModeAttempt guildLeaders;
            guildLeaders.InstanceRealmAddress = GetVirtualRealmAddress();
            guildLeaders.AttemptID = bestGuild->ID;
            guildLeaders.CompletionTime = bestGuild->RecordTime;
            guildLeaders.CompletionDate = bestGuild->Date;
            guildLeaders.MedalEarned = bestGuild->ChallengeLevel;

            for (auto const& v : bestGuild->member)
            {
                WorldPackets::Misc::ModeAttempt::Member memberData;
                memberData.VirtualRealmAddress = GetVirtualRealmAddress();
                memberData.NativeRealmAddress = GetVirtualRealmAddress();
                memberData.Guid = v.guid;
                memberData.SpecializationID = v.specId;
                guildLeaders.Members.emplace_back(memberData);
            }

            result.GuildLeaders.emplace_back(guildLeaders);
        }
    }

    if (ChallengeData* bestServer = sChallengeModeMgr->BestServerChallenge(packet.ChallengeID))
    {
        WorldPackets::Misc::ModeAttempt realmLeaders;
        realmLeaders.InstanceRealmAddress = GetVirtualRealmAddress();
        realmLeaders.AttemptID = bestServer->ID;
        realmLeaders.CompletionTime = bestServer->RecordTime;
        realmLeaders.CompletionDate = bestServer->Date;
        realmLeaders.MedalEarned = bestServer->ChallengeLevel;

        for (auto const& v : bestServer->member)
        {
            WorldPackets::Misc::ModeAttempt::Member memberData;
            memberData.VirtualRealmAddress = GetVirtualRealmAddress();
            memberData.NativeRealmAddress = GetVirtualRealmAddress();
            memberData.Guid = v.guid;
            memberData.SpecializationID = v.specId;
            realmLeaders.Members.emplace_back(memberData);
        }
        result.RealmLeaders.push_back(realmLeaders);
    }

    SendPacket(result.Write());
}

void WorldSession::HandleStartChallengeMode(WorldPackets::Misc::StartChallengeMode& packet)
{
    GameObject* object = _player->GetGameObjectIfCanInteractWith(packet.GameObjectGUID, GAMEOBJECT_TYPE_KEYSTONE_RECEPTACLE);
    if (!object)
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - {} not found or you can not interact with it.", packet.GameObjectGUID.ToString().c_str());
        return;
    }

    Item* key = _player->GetItemByPos(packet.Bag, packet.Slot);
    if (!key)
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - item in Bag {} and Slot {} not found.", packet.Bag, packet.Slot);
        return;
    }

    if (key->GetTemplate()->GetClass() != ITEM_CLASS_REAGENT || key->GetTemplate()->GetSubClass() != ITEM_SUBCLASS_KEYSTONE)
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - Tried to start a challenge with item {} which have class {} and subclass {}.",
        key->GetGUID().ToString().c_str(),
            key->GetTemplate()->GetClass(),
            key->GetTemplate()->GetSubClass());
        return;
    }

    uint32 challengeModeId = key->GetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID);
    uint32 challengeModeLevel = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL);
    uint32 challengeModeAffix1 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1);
    uint32 challengeModeAffix2 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2);
    uint32 challengeModeAffix3 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3);
    uint32 challengeModeAffix4 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_4);

    MapChallengeModeEntry const* entry = sMapChallengeModeStore.LookupEntry(challengeModeId);
    if (!entry || !challengeModeLevel || entry->MapID != _player->GetMapId())
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - Tried to start a challenge with wrong challengeModeId {} and level {}.", challengeModeId, challengeModeLevel);
        return;
    }

    if (InstanceScript* instanceScript = _player->GetInstanceScript())
        instanceScript->StartChallengeMode(challengeModeId, challengeModeLevel, challengeModeAffix1, challengeModeAffix2, challengeModeAffix3, challengeModeAffix4);

    // Blizzard do not delete the key at challenge start, will require mort research
    _player->DestroyItem(packet.Bag, packet.Slot, true);
}

void WorldSession::HandleResetChallengeMode(WorldPackets::Misc::ResetChallengeMode& packet)
{
    if (auto const& instanceScript = _player->GetInstanceScript())
        if (instanceScript->instance->isChallenge())
            instanceScript->ResetChallengeMode();
}

void WorldSession::HandleMythicPlusSeasonData(WorldPackets::MythicPlus::MythicPlusSeasonData& mythicPlusSeasonData)
{
    WorldPackets::MythicPlus::MythicPlusSeasonDataResult result;

    SendPacket(result.Write());
}

void WorldSession::HandleMythicPlusCurrentAffixes(WorldPackets::MythicPlus::MythicPlusCurrentAffixes& mythicPlusCurrentAffixes)
{
    WorldPackets::MythicPlus::MythicPlusCurrentAffixesResult result;

    result.Count = 4;

    result.Affixes[0] = 10;
    result.Affixes[1] = 6;
    result.Affixes[2] = 14;
    result.Affixes[3] = 132;

    result.RequiredSeason[0] = 9;
    result.RequiredSeason[1] = 0;
    result.RequiredSeason[2] = 9;
    result.RequiredSeason[3] = 3;

    SendPacket(result.Write());
}

void WorldSession::HandleMythicPlusRequestMapStats(WorldPackets::MythicPlus::MythicPlusRequestMapStats& mythicPlusRequestMapStats)
{
    WorldPackets::MythicPlus::MythicPlusRequestMapStatsResult result;
    uint32 maps = sChallengeModeMgr->GetRandomChallengeId();
    for (auto& mythicPlusRun : result.mythicPlusRuns)
    {
        mythicPlusRun.MapChallengeModeID = maps;
    }

    for (auto& mythicPlusMembers : result.mythicPlusRuns)
    {
        mythicPlusMembers.Members.size();
        for (auto& member : mythicPlusMembers.Members)
        {
            member.BnetAccountGUID = GetPlayer()->GetGUID();
        }
    }
        SendPacket(result.Write());
}

void WorldSession::HandleContributionCollectorContribute(WorldPackets::Misc::ContributionCollectorContribute& packet)
{
    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(packet.ContributionTableNpcGuid, UNIT_NPC_FLAG_NONE, UNIT_NPC_FLAG_2_CONTRIBUTION_COLLECTOR);
    if (!unit)
        return;

    sContributionMgr.Contribute(GetPlayer(), packet.OrderIndex);
}

void WorldSession::HandleCommentatorEnable(WorldPackets::Misc::CommentatorEnable& packet)
{
}

void WorldSession::HandleCommentatorGetMapInfo(WorldPackets::Misc::CommentatorGetMapInfo& packet)
{
}

void WorldSession::HandleCommentatorGetPlayerInfo(WorldPackets::Misc::CommentatorGetPlayerInfo& packet)
{
}

void WorldSession::HandleCommentatorEnterInstance(WorldPackets::Misc::CommentatorEnterInstance& packet)
{
}

void WorldSession::HandleCommentatorExitInstance(WorldPackets::Misc::CommentatorExitInstance& packet)
{
}

void WorldSession::HandleActivateSoulbind(WorldPackets::Misc::ActivateSoulbind& packet)
{
}

void WorldSession::HandleAbandonNpeResponse(WorldPackets::Misc::AbandonNpeResponse& packet)
{
}

void WorldSession::HandleAcceptReturningPlayerPrompt(WorldPackets::Misc::AcceptReturningPlayerPrompt& packet)
{
}

void WorldSession::HandleAcceptSocialContract(WorldPackets::Misc::AcceptSocialContract& packet)
{
}

void WorldSession::HandleAddAccountCosmetic(WorldPackets::Misc::AddAccountCosmetic& packet)
{
}

void WorldSession::HandleClaimWeeklyRewards(WorldPackets::Misc::claimweeklyrewards& packet)
{
    WorldPackets::Misc::WeeklyRewardClaimResult send;
    send.Result;

    SendPacket(send.Write());
}

void WorldSession::HandleClearNewAppearance(WorldPackets::Misc::ClearNewAppearance& packet)
{
}

void WorldSession::HandleAccountNotificationAcknowledge(WorldPackets::Misc::AccountNotificationAcknowledge& packet)
{
}

void WorldSession::HandleAuctionableTokenSell(WorldPackets::Misc::AuctionableTokenSell& packet)
{
}

void WorldSession::HandleAuctionableTokenSellAtMarketPrice(WorldPackets::Misc::AuctionableTokenSellAtMarketPrice& packet)
{
}

void WorldSession::HandleCanRedeemTokenForBalance(WorldPackets::Misc::CanRedeemTokenForBalance& packet)
{
}

void WorldSession::HandleChangeBagSlotFlag(WorldPackets::Misc::ChangeBagSlotFlag& packet)
{
}

void WorldSession::HandleChangeBankBagSlotFlag(WorldPackets::Misc::ChangeBankBagSlotFlag& packet)
{
}

void WorldSession::HandleCloseRuneforgeInteraction(WorldPackets::Misc::CloseRuneforgeInteraction& packet)
{
}

void WorldSession::HandleCommerceTokenGetCount(WorldPackets::Misc::CommerceTokenGetCount& packet)
{
}

void WorldSession::HandleContributionLastUpdateRequest(WorldPackets::Misc::ContributionLastUpdateRequest& packet)
{
}

void WorldSession::SendChallengeModeMapStatsUpdate(uint32 keyID)
{
    WorldPacket packet(6 * 4);

    for (auto l_ChallengeData : _player->m_CompletedChallenges)
    {
        if (l_ChallengeData.first == keyID)
        {
            CompletedChallenge l_CompletedChallenge = l_ChallengeData.second;

            packet << uint32(l_CompletedChallenge.MapID);
            packet << uint32(l_ChallengeData.first);
            packet << uint32(l_CompletedChallenge.BestCompletion);
            packet << uint32(l_CompletedChallenge.LastCompletion);
            packet << uint32(l_CompletedChallenge.Medal);
            packet << uint32(l_CompletedChallenge.MedalDate);

            uint32 l_SpecCount = 5;
            packet << uint32(l_SpecCount);

            for (uint32 I = 0; I < 3; ++I)
                packet << uint32(0);

            for (uint32 J = 0; J < l_SpecCount; ++J)
                packet << uint16(0);    ///pecID
        }
    }

    SendPacket(&packet);
}

void WorldSession::HandleShowTradeSkill(WorldPackets::Misc::ShowTradeSkill& packet)
{
    if (!sSkillLineStore.LookupEntry(packet.SkillLineID) || !sSpellMgr->GetSpellInfo(packet.SpellID, DIFFICULTY_NONE))
        return;

    Player* player = ObjectAccessor::FindPlayer(packet.PlayerGUID);
    if (!player)
        return;

    std::set<uint32> relatedSkills;
    relatedSkills.insert(packet.SkillLineID);

    for (SkillLineEntry const* skillLine : sSkillLineStore)
    {
        if (skillLine->ParentSkillLineID != packet.SkillLineID)
            continue;

        if (!player->HasSkill(skillLine->ParentSkillLineID))
            continue;

        relatedSkills.insert(skillLine->ParentSkillLineID);
    }

    std::set<uint32> profSpells;
    for (auto const& v : player->GetSpellMap())
    {
        if (v.second.state == PLAYERSPELL_REMOVED)
            continue;

        if (!v.second.active || v.second.disabled)
            continue;

        for (auto const& s : relatedSkills)
            if (IsPartOfSkillLine(s, v.first))
                profSpells.insert(v.first);
    }

    if (profSpells.empty())
        return;

    WorldPackets::Misc::ShowTradeSkillResponse response;
    response.PlayerGUID = packet.PlayerGUID;
    response.SpellId = packet.SpellID;

    for (uint32 const& x : profSpells)
        response.KnownAbilitySpellIDs.push_back(x);

    for (uint32 const& v : relatedSkills)
    {
        response.SkillLineIDs.push_back(v);
        response.SkillRanks.push_back(player->GetSkillValue(v));
        response.SkillMaxRanks.push_back(player->GetMaxSkillValue(v));
    }

    _player->SendDirectMessage(response.Write());
}

void WorldSession::HandleSelectFactionOpcode(WorldPackets::Misc::FactionSelect& selectFaction)
{
    enum FactionSelection
    {
        JOIN_HORDE = 0,
        JOIN_ALLIANCE = 1
    };

    if (_player->GetRace() != RACE_PANDAREN_NEUTRAL)
        return;

    if (selectFaction.FactionChoice == JOIN_ALLIANCE)
    {
        _player->SetRace(RACE_PANDAREN_ALLIANCE);
        _player->SetFactionForRace(RACE_PANDAREN_ALLIANCE);
        _player->SaveToDB();
        _player->LearnSpell(668, false);            // Language Common
        _player->LearnSpell(108130, false);         // Language Pandaren Alliance
        _player->CastSpell(_player, 113244, true);  // Faction Choice Trigger Spell: Alliance
    }
    else if (selectFaction.FactionChoice == JOIN_HORDE)
    {
        _player->SetRace(RACE_PANDAREN_HORDE);
        _player->SetFactionForRace(RACE_PANDAREN_HORDE);
        _player->SaveToDB();
        _player->LearnSpell(669, false);            // Language Orcish
        _player->LearnSpell(108131, false);         // Language Pandaren Horde
        _player->CastSpell(_player, 113245, true);  // Faction Choice Trigger Spell: Horde
    }
}

void WorldSession::HandleAddonList(WorldPackets::ClientConfig::AddonList& packet)
{
    //SMSG_ADDON_LIST_REQUEST
}

void WorldSession::HandleQueryWorldCountwodnTimer(WorldPackets::Instance::QueryWorldCountwodnTimer& packet)
{
    //  Player* player = GetPlayer();
    //  if (Battleground* bg = player->GetBattleground())
       //   bg->HandleStartTimer(packet.Type);
}

void WorldSession::HandleIslandQueue(WorldPackets::Misc::IslandOnQueue& packet)
{
    lfg::LfgDungeonSet newDungeons;
    newDungeons.insert(packet.ActivityID);
    sLFGMgr->JoinLfg(GetPlayer(), ROLE_DAMAGE, newDungeons);

    //alliance
    GetPlayer()->KilledMonsterCredit(139310);
    //horde
    GetPlayer()->KilledMonsterCredit(139309);
}

void WorldSession::SendMythicPlusMapStatsUpdate(uint32 keyID)
{
    WorldPacket packet(6 * 4);

    for (auto const& l_ChallengeData : _player->m_CompletedChallenges)
    {
        if (l_ChallengeData.first == keyID)
        {
            CompletedChallenge l_CompletedChallenge = l_ChallengeData.second;

            packet << uint32(l_CompletedChallenge.MapID);
            packet << uint32(l_ChallengeData.first);
            packet << uint32(l_CompletedChallenge.BestCompletion);
            packet << uint32(l_CompletedChallenge.LastCompletion);
            packet << uint32(l_CompletedChallenge.Medal);
            packet << uint32(l_CompletedChallenge.MedalDate);

            uint32 l_SpecCount = 5;
            packet << uint32(l_SpecCount);

            for (uint32 I = 0; I < 3; ++I)
                packet << uint32(0);

            for (uint32 J = 0; J < l_SpecCount; ++J)
                packet << uint16(0);    ///pecID
        }
    }

    SendPacket(&packet);
}

void WorldSession::SendCovenantPreview(ObjectGuid sender, int32 covenantID)
{
    WorldPackets::Misc::CovenantPreviewOpenNpc result;
    result.ObjGUID = sender;
    result.CovenantId = covenantID;
    SendPacket(result.Write());
}

void WorldSession::SendCovenantCallingAvailibilityResponse(bool availability, uint32 index, uint32 data)
{
    WorldPackets::Misc::CovenantCallingAvailibilityResponse send;
    send.Availability = availability;
    send.Index = index;
    send.Data = data;
    SendPacket(send.Write());
}

void WorldSession::OverrideLight(int32 areaLightID, int32 overrideLightID, int32 transitionMilliseconds)
{
    WorldPackets::Misc::OverrideLight overrideLight;
    overrideLight.AreaLightID = areaLightID;
    overrideLight.OverrideLightID = overrideLightID;
    overrideLight.TransitionMilliseconds = transitionMilliseconds;
    SendPacket(overrideLight.Write());
}

void WorldSession::StartTimer(InstanceMap* instance, int32 timerType, Seconds totalTime, Seconds timeLeft)
{
    WorldPackets::Misc::StartTimer startTimer;
    startTimer.Type = WorldPackets::Misc::StartTimer::TimerType(timerType);
    startTimer.TotalTime = totalTime;
    startTimer.TimeLeft = timeLeft;
    instance->SendToPlayers(startTimer.Write());
}

void WorldSession::SendCovenantRenownSendCatchUpState(bool catchupState)
{
    WorldPackets::Misc::CovenantRenownSendCatchUpState send;
    send.CatchupState = false;
    SendPacket(send.Write());
}
