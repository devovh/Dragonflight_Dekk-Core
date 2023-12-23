/*
 * Copyright (C) 2020 LatinCoreTeam
 *
 */

#include "ChallengeModeMgr.h"
#include "Challenge.h"
#include "Containers.h"
#include "DB2Stores.h"
#include "GameTables.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "LootPackets.h"
#include "InstanceScript.h"
#include "MiscPackets.h"
#include "GameEventMgr.h"
#include "Util.h"
#include "StringConvert.h"
#include <sstream>
#include "World.h"
#include "WorldStateMgr.h"
#include "ItemBonusMgr.h"

static uint32 stepLeveling[3][16]
{
    // Step 7.0.3 - 7.1.5 start 845
    // 0    1    2    3    4    5    6     7     8     9    10     11    12    13    14    15
    { 0, 0, 0, 0, 5, 5, 10, 10, 15, 15, 20, 25, 25, 30, 35, 40 },
    // Step 7.2.0 - 7.2.5 start 870
    // 0    1    2    3    4    5    6     7     8     9    10     11    12    13    14    15
    { 0, 0, 0, 0, 5, 5, 10, 10, 15, 15, 20, 20, 25, 30, 35, 40 },
    // Step 7.3.0 - 7.3.5 start 890
    // 0    1    2    3    4    5    6     7     8     9    10     11    12    13    14    15
    { 0, 0, 0, 0, 5, 5, 10, 15, 20, 20, 25, 30, 35, 40, 45, 50 }
};

static uint32 stepOplotLeveling[3][16]
{
    // Step 7.0.3 - 7.1.5 start 845
    // 0    1    2     3    4     5     6     7     8     9     10    11    12    13    14    15
    { 0, 0, 5, 10, 15, 20, 20, 25, 25, 30, 35, 35, 40, 45, 50, 55 },
    // Step 7.2.0 - 7.2.5 start 870
    // 0    1    2     3    4     5     6     7     8     9     10    11    12    13    14    15
    { 0, 0, 5, 10, 15, 20, 20, 25, 25, 30, 35, 40, 45, 50, 55, 60 },
    // Step 7.3.0 - 7.3.5 start 890
    // 0    1    2     3    4     5     6     7     8     9     10    11    12    13    14    15
    { 0, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70 }
};

bool ChallengeMember::operator<(const ChallengeMember& i) const
{
    return guid.GetCounter() > i.guid.GetCounter();
}

bool ChallengeMember::operator==(const ChallengeMember& i) const
{
    return guid.GetCounter() == i.guid.GetCounter();
}

ChallengeModeMgr::ChallengeModeMgr() = default;

ChallengeModeMgr::~ChallengeModeMgr()
{
    for (auto v : _challengeMap)
        delete v.second;

    _challengeMap.clear();
    _challengesOfMember.clear();
    _bestForMap.clear();
}

ChallengeModeMgr* ChallengeModeMgr::instance()
{
    static ChallengeModeMgr instance;
    return &instance;
}

void ChallengeModeMgr::CheckBestMapId(ChallengeData* challengeData)
{
    if (!challengeData)
        return;

    if (!_bestForMap[challengeData->ChallengeID] || _bestForMap[challengeData->ChallengeID]->RecordTime > challengeData->RecordTime)
        _bestForMap[challengeData->ChallengeID] = challengeData;
}

void ChallengeModeMgr::CheckBestGuildMapId(ChallengeData* challengeData)
{
    if (!challengeData || !challengeData->GuildID)
        return;

    if (!m_GuildBest[challengeData->GuildID][challengeData->ChallengeID] || m_GuildBest[challengeData->GuildID][challengeData->ChallengeID]->RecordTime > challengeData->RecordTime)
        m_GuildBest[challengeData->GuildID][challengeData->ChallengeID] = challengeData;
}

bool ChallengeModeMgr::CheckBestMemberMapId(ObjectGuid const& guid, ChallengeData* challengeData)
{
    bool isBest = false;
    if (!_challengesOfMember[guid][challengeData->ChallengeID] || _challengesOfMember[guid][challengeData->ChallengeID]->RecordTime > challengeData->RecordTime)
    {
        _challengesOfMember[guid][challengeData->ChallengeID] = challengeData;
        isBest = true;
    }

    if (!_lastForMember[guid][challengeData->ChallengeID] || _lastForMember[guid][challengeData->ChallengeID]->Date < challengeData->Date)
        _lastForMember[guid][challengeData->ChallengeID] = challengeData;

    _challengeWeekList[guid].insert(challengeData);

    return isBest;
}

void ChallengeModeMgr::SaveChallengeToDB(ChallengeData const* challengeData)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE);
    stmt->setUInt32(0, challengeData->ID);
    stmt->setUInt64(1, challengeData->GuildID);
    stmt->setUInt16(2, challengeData->MapID);
    stmt->setUInt16(3, challengeData->ChallengeID);
    stmt->setUInt32(4, challengeData->RecordTime);
    stmt->setUInt32(5, challengeData->Date);
    stmt->setUInt8(6, challengeData->ChallengeLevel);
    stmt->setUInt8(7, challengeData->TimerLevel);
    std::ostringstream affixesListIDs;
    for (uint16 affixe : challengeData->Affixes)
        if (affixe)
            affixesListIDs << affixe << ' ';
    stmt->setString(8, affixesListIDs.str());
    stmt->setUInt32(9, challengeData->ChestID);
    trans->Append(stmt);

        for (auto const& v : challengeData->member)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE_MEMBER);
            stmt->setUInt32(0, challengeData->ID);
            stmt->setUInt64(1, v.guid.GetCounter());
            stmt->setUInt16(2, v.specId);
            stmt->setUInt32(3, v.ChallengeLevel);
            stmt->setUInt32(4, v.Date);
            stmt->setUInt32(5, v.ChestID);
            trans->Append(stmt);
        }

        CharacterDatabase.CommitTransaction(trans);
}

void ChallengeModeMgr::LoadFromDB()
 {
    uint32 oldMSTime = getMSTime();

    if (QueryResult result = CharacterDatabase.Query("SELECT `ID`, `GuildID`, `MapID`, `RecordTime`, `Date`, `ChallengeLevel`, `TimerLevel`, `Affixes`, `ChestID`, `ChallengeID` FROM `challenge`"))
    {
        do
        {
            Field* fields = result->Fetch();

            auto challengeData = new ChallengeData;
            challengeData->ID = fields[0].GetUInt64();
            challengeData->GuildID = fields[1].GetUInt64();
            challengeData->MapID = fields[2].GetUInt16();
            challengeData->ChallengeID = fields[3].GetUInt16();
            challengeData->RecordTime = fields[4].GetUInt32();
            if (challengeData->RecordTime < 10000)
                challengeData->RecordTime *= IN_MILLISECONDS;
            challengeData->Date = fields[5].GetUInt32();
            challengeData->ChallengeLevel = fields[6].GetUInt8();
            challengeData->TimerLevel = fields[7].GetUInt8();
            challengeData->ChestID = fields[9].GetUInt32();

            if (!challengeData->ChallengeID)
                if (MapChallengeModeEntry const* challengeEntry = sDB2Manager.GetChallengeModeByMapID(challengeData->MapID))
                    challengeData->ChallengeID = challengeEntry->ID;

            challengeData->Affixes.fill(0);

            uint8 i = 0;
            for (std::string_view token : Trinity::Tokenize(fields[8].GetString().c_str(), ' ', false))
                if (Optional<int32> affix = Trinity::StringTo<int32>(token))
                    challengeData->Affixes[i] = *affix;

            _challengeMap[challengeData->ID] = challengeData;
            CheckBestMapId(challengeData);
            CheckBestGuildMapId(challengeData);

        } while (result->NextRow());
    }

    if (QueryResult result = CharacterDatabase.Query("SELECT `id`, `member`, `specID`, `ChallengeLevel`, `Date`, `ChestID` FROM `challenge_member`"))
    {
        do
        {
            Field* fields = result->Fetch();
            ChallengeMember member;
            member.guid = ObjectGuid::Create<HighGuid::Player>(fields[1].GetUInt64());
            member.specId = fields[2].GetUInt16();
            member.ChallengeLevel = fields[3].GetUInt32();
            member.Date = fields[4].GetUInt32();
            member.ChestID = fields[5].GetUInt32();

            auto itr = _challengeMap.find(fields[0].GetUInt64());
            if (itr == _challengeMap.end())
                continue;

            itr->second->member.insert(member);
            CheckBestMemberMapId(member.guid, itr->second);
        } while (result->NextRow());
    }

    for (auto v : _challengeMap)
        if (v.second->member.empty())
            CharacterDatabase.PQuery("DELETE FROM `challenge` WHERE `ID` = '{}';", v.first);


    if (QueryResult result = CharacterDatabase.Query("SELECT `guid`, `chestListID`, `date`, `ChallengeLevel` FROM `challenge_oplote_loot`"))
    {
        do
        {
            Field* fields = result->Fetch();
            ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(fields[0].GetUInt64());
            OploteLoot& lootOplote = _oploteWeekLoot[guid];
            lootOplote.Date = fields[2].GetUInt32();
            lootOplote.ChallengeLevel = fields[3].GetUInt32();
            lootOplote.needSave = false;
            lootOplote.guid = guid;

            for (std::string_view chestLists : Trinity::Tokenize(fields[1].GetString().c_str(), ' ', false))
                if (Optional<int32> chestList = Trinity::StringTo<int32>(chestLists))
                    lootOplote.chestListID.insert(*chestList);

        } while (result->NextRow());
    }

    if (sWorld->GetPersistentWorldVariable(World::Challengeaffix1ResetTime) == 0)
        GenerateCurrentWeekAffixes();

    if ((sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX1) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX1) < 15) &&
        (sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX2) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX2) < 15) &&
        (sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX3) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX3) < 15) &&
        (sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX4) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX4) < 15))
        GenerateManualAffixes();

    TC_LOG_INFO("server.loading", ">> Loaded {} Character Challenges in {} ms", uint32(_challengeMap.size()), GetMSTimeDiffToNow(oldMSTime));
}

ChallengeData* ChallengeModeMgr::BestServerChallenge(uint16 ChallengeID)
{
    return Trinity::Containers::MapGetValuePtr(_bestForMap, ChallengeID);
}

ChallengeData* ChallengeModeMgr::BestGuildChallenge(ObjectGuid::LowType const& GuildID, uint16 ChallengeID)
{
    if (!GuildID)
        return nullptr;

    auto itr = m_GuildBest.find(GuildID);
    if (itr == m_GuildBest.end())
        return nullptr;

    return Trinity::Containers::MapGetValuePtr(itr->second, ChallengeID);
}

void ChallengeModeMgr::SetChallengeMapData(ObjectGuid::LowType const& ID, ChallengeData* data)
{
    _challengeMap[ID] = data;
}

ChallengeByMap* ChallengeModeMgr::BestForMember(ObjectGuid const& guid)
{
    return Trinity::Containers::MapGetValuePtr(_challengesOfMember, guid);
}

ChallengeByMap* ChallengeModeMgr::LastForMember(ObjectGuid const& guid)
{
    return Trinity::Containers::MapGetValuePtr(_lastForMember, guid);
}

ChallengeData* ChallengeModeMgr::LastForMemberMap(ObjectGuid const& guid, uint32 ChallengeID)
{
    if (ChallengeByMap* _lastResalt = LastForMember(guid))
    {
        auto itr = _lastResalt->find(ChallengeID);
        if (itr != _lastResalt->end())
            return itr->second;
    }

    return nullptr;
}

ChallengeData* ChallengeModeMgr::BestForMemberMap(ObjectGuid const& guid, uint32 ChallengeID)
{
    if (ChallengeByMap* _lastResalt = BestForMember(guid))
    {
        auto itr = _lastResalt->find(ChallengeID);
        if (itr != _lastResalt->end())
            return itr->second;
    }

    return nullptr;
}

void ChallengeModeMgr::GenerateCurrentWeekAffixes()
{
    // Season 1 DragonFlight
    uint32 affixes[10][4] =
    {
        { Fortified, Raging, Quaking, Thundering},
        { Tyrannical, Bursting, Grievous, Thundering},
        { Fortified, Sanguine, Volcanic, Thundering},
        { Tyrannical, Raging, Storming, Thundering},
        { Fortified, Spiteful, Grievous, Thundering},
        { Tyrannical, Sanguine, Explosive, Thundering},
        { Fortified, Bolstering, Storming, Thundering},
        { Tyrannical, Spiteful, Quaking, Thundering},
        { Fortified, Bursting, Explosive, Thundering},
        { Tyrannical, Bolstering, Volcanic, Thundering},
    };

    auto weekContainer = affixes[GetActiveAffixe()];

    sWorldStateMgr->SetValueAndSaveInDb(WS_CHALLENGE_AFFIXE1_RESET_TIME, weekContainer[0], false, nullptr);
    sWorldStateMgr->SetValueAndSaveInDb(WS_CHALLENGE_AFFIXE2_RESET_TIME, weekContainer[1], false, nullptr);
    sWorldStateMgr->SetValueAndSaveInDb(WS_CHALLENGE_AFFIXE3_RESET_TIME, weekContainer[2], false, nullptr);
    sWorldStateMgr->SetValueAndSaveInDb(WS_CHALLENGE_AFFIXE4_RESET_TIME, weekContainer[3], false, nullptr);
}

void ChallengeModeMgr::GenerateManualAffixes()
{
    sWorld->SetPersistentWorldVariable(World::Challengeaffix1ResetTime, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX1));
    sWorld->SetPersistentWorldVariable(World::Challengeaffix2ResetTime, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX2));
    sWorld->SetPersistentWorldVariable(World::Challengeaffix3ResetTime, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX3));
    sWorld->SetPersistentWorldVariable(World::Challengeaffix4ResetTime, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX4));
}

uint8 ChallengeModeMgr::GetActiveAffixe()
{
    if (sGameEventMgr->IsActiveEvent(126))
        return 0;
    if (sGameEventMgr->IsActiveEvent(127)) 
        return 1;
    if (sGameEventMgr->IsActiveEvent(128))
        return 2;
    if (sGameEventMgr->IsActiveEvent(129))
        return 3;
    if (sGameEventMgr->IsActiveEvent(130))
        return 4;
    if (sGameEventMgr->IsActiveEvent(131))
        return 5;
    if (sGameEventMgr->IsActiveEvent(132))
        return 6;
    if (sGameEventMgr->IsActiveEvent(133))
        return 7;
    if (sGameEventMgr->IsActiveEvent(134))
        return 8;
    if (sGameEventMgr->IsActiveEvent(135))
        return 9;
    if (sGameEventMgr->IsActiveEvent(136))
        return 10;
    if (sGameEventMgr->IsActiveEvent(137))
        return 11;

    return 0;
}

bool ChallengeModeMgr::HasOploteLoot(ObjectGuid const& guid)
{
    return Trinity::Containers::MapGetValuePtr(_oploteWeekLoot, guid);
}

OploteLoot* ChallengeModeMgr::GetOploteLoot(ObjectGuid const& guid)
{
    return Trinity::Containers::MapGetValuePtr(_oploteWeekLoot, guid);
}

void ChallengeModeMgr::SaveOploteLootToDB()
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    for (auto const& v : _oploteWeekLoot)
    {
        if (v.second.needSave)
        {
            auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE_OPLOTE_LOOT);
            stmt->setUInt32(0, v.second.guid.GetCounter());
            std::ostringstream chestLists;
            for (uint32 chestList : v.second.chestListID)
                if (chestList)
                    chestLists << chestList << ' ';
            stmt->setString(1, chestLists.str());
            stmt->setUInt32(2, v.second.Date);
            stmt->setUInt32(3, v.second.ChallengeLevel);
            trans->Append(stmt);
        }
    }
    CharacterDatabase.CommitTransaction(trans);
}

void ChallengeModeMgr::DeleteOploteLoot(ObjectGuid const& guid)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHALLENGE_OPLOTE_LOOT_BY_GUID);
    stmt->setUInt32(0, guid.GetCounter());
    CharacterDatabase.Execute(stmt);

    _oploteWeekLoot.erase(guid);
}

void ChallengeModeMgr::GenerateOploteLoot(bool manual)
{
    bool _manual = false;
    manual = _manual;
    TC_LOG_ERROR("misc", "GenerateOploteLoot manual {} _challengeWeekList {}", _manual, _challengeWeekList.size());

    CharacterDatabase.Query("DELETE FROM challenge_oplote_loot WHERE date <= UNIX_TIMESTAMP()");
    _oploteWeekLoot.clear();

    for (auto const& c : _challengeWeekList)
    {
        for (auto const& v : c.second)
        {
            if (sWorldStateMgr->GetValue(WS_CHALLENGE_LAST_RESET_TIME, nullptr) || v->Date < uint32(sWorldStateMgr->GetValue(WS_CHALLENGE_LAST_RESET_TIME, nullptr) - (7 * DAY)))
                continue;

            if (!manual && (v->Date > uint32(sWorldStateMgr->GetValue(WS_CHALLENGE_KEY_RESET_TIME, nullptr)) || v->Date < uint32(sWorldStateMgr->GetValue(WS_CHALLENGE_LAST_RESET_TIME, nullptr))))
                continue;

            if (!v->ChestID)
                continue;

            auto itr = _oploteWeekLoot.find(c.first);
            if (itr != _oploteWeekLoot.end())
            {
                if (itr->second.ChallengeLevel < v->ChallengeLevel)
                    itr->second.ChallengeLevel = v->ChallengeLevel;

                itr->second.chestListID.insert(v->ChestID);
            }
            else
            {
                OploteLoot& lootOplote = _oploteWeekLoot[c.first];
                lootOplote.Date = sWorld->getNextChallengeKeyReset();
                lootOplote.ChallengeLevel = v->ChallengeLevel;
                lootOplote.needSave = true;
                lootOplote.guid = c.first;
                lootOplote.chestListID.insert(v->ChestID);
            }
        }
    }
    _challengeWeekList.clear();
    SaveOploteLootToDB();
}

bool ChallengeModeMgr::GetStartPosition(uint32 mapID, float& /*x*/, float& /*y*/, float& /*z*/, float& /*o*/, ObjectGuid OwnerGuid)
{
    uint32 WorldSafeLocID = 0;
    switch (mapID)
    {
    case 2441: //TAZAVESH
        WorldSafeLocID = 5105; //add later the correct
        break;
    case 2293: //Theater of Pain
        WorldSafeLocID = 5098;  //add later the correct
        break;
    case 2285: // Spires of ascension
        WorldSafeLocID = 50002;
        break;
    case 2284: //Sanguine depeths
        WorldSafeLocID = 5352; //add later the correct
        break;
    case 2289: // Plaguefall
        WorldSafeLocID = 5102;//add later the correct
        break;
    case 2287: // halls of attonement
        WorldSafeLocID = 50000;
        break;
    case 2291: // de other side
        WorldSafeLocID = 5100; //add later the correct
        break;
    case 2286: // the necrotik wake
        WorldSafeLocID = 5432; //add later the correct
        break;
    case 2290: // mist of tirna
        WorldSafeLocID = 5194; //add later the correct
        break;
        if (Player* keyOwner = ObjectAccessor::FindPlayer(OwnerGuid))
        {
            if (keyOwner->m_challengeKeyInfo.ID == 227) // change for tazavesh later
                WorldSafeLocID = 6022; // 7.2 Karazhan - Challenge Mode Start (Lower Karazhan)
            else
                WorldSafeLocID = 6023; // 7.2 Karazhan - Challenge Mode Start (Upper Karazhan)
        }
        break;
    default:
        break;
    }
    if (WorldSafeLocID == 0)
        return false;

    return false;
}

uint32 ChallengeModeMgr::GetLootTreeMod(int32& levelBonus, uint32& challengeLevel, Challenge* challenge)
{
    auto isOplote = bool(challenge == nullptr);
    if (challenge)
        challengeLevel = std::min(challengeLevel, 15u);

    uint8 levelingStep = sWorld->getIntConfig(CONFIG_CHALLENGE_LEVEL_STEP);
    uint8 leveling = challengeLevel;

    if (sWorld->getIntConfig(CONFIG_CHALLENGE_LEVEL_MAX) < leveling)
        leveling = sWorld->getIntConfig(CONFIG_CHALLENGE_LEVEL_MAX);

    levelBonus = isOplote ? stepOplotLeveling[levelingStep][leveling] : stepLeveling[levelingStep][leveling];

    return 16;
}

uint32 ChallengeModeMgr::GetCAForLoot(Challenge* const challenge, uint32 /*goEntry*/)
{
    if (!challenge)
        return 0;

    switch (challenge->_mapID)
    {
    case 2290: //Mist of tirna
    {
        // Lesser Dungeons
        switch (challenge->GetChallengeLevel())
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 184773;
        case 4:
        case 5:
        case 6:
            return 184773;
        case 7:
        case 8:
        case 9:
            return 184773;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 184773;
        case 15:
        default:
            return 184773;
        }
    }
    case 2287: //halls of attonement
    case 2291: //de othher side
    {
        // Regular Dungeons
        switch (challenge->GetChallengeLevel())
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 184773;
        case 4:
        case 5:
        case 6:
            return 184773;
        case 7:
        case 8:
        case 9:
            return 184773; //Battle-Tested Armor Component
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 184773;
        case 15:
        default:
            return 184773;
        }
    }
    case 2284:
    case 2285:
    {
        // Greater Dungeons
        switch (challenge->GetChallengeLevel())
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 184776;
        case 4:
        case 5:
        case 6:
            return 184776;
        case 7:
        case 8:
        case 9:
            return 184776;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 184776;
        case 15:
        default:
            return 184776;
        }
    }
    default:
        break;
    }

    return 0;
}

uint32 ChallengeModeMgr::GetBigCAForLoot(Challenge* const challenge, uint32 /*goEntry*/, uint32& count)
{
    if (!challenge || challenge->GetChallengeLevel() <= 10)
        return 0;

    if (challenge->GetChallengeLevel() >= 15)
        count = challenge->GetChallengeLevel() - 15;
    else
        count = challenge->GetChallengeLevel() - 10;

    switch (challenge->_mapID)
    {
    case 1754: //freehold 
    {
        // Lesser Dungeons
        return 147808; // Lesser Adept's Spoils
    }
    case 1771: //tol dagor            
    case 1841: //underrot             
    case 1762: //king´s rest          
    case 2097: //op mechagon          
    case 1864: //shrine of the storm  
    case 1822: //siege of boralus  
    case 1862: //waycrest manor     
    {
        // Regular Dungeons
        return 147809; // Adept's Spoils
    }
    case 1877: //temple of serthraliss 
    case 1594: //the motherlode!!      
    {
        // Greater Dungeons
        return 147810; // Greater Adept's Spoils
    }
    case 2290: //Mist of tirna scithe
    {
        // Lesser Dungeons
        return 184776; // Urn of Arena Soil
    }
    case 2286: //Necrotic wake
    case 2291: //De other side
    case 2287: //Halls of atonement
    case 2289: //plaguefall
    case 2284: //sanguine depths
    case 2285: //spires of ascension
    {
        // Regular Dungeons
        return 184776; // Urn of Arena Soil
    }
    case 2293: // Theater of pain
    case 2441: // Tazavesh
    {
        // Greater Dungeons
        return 184776; // Urn of Arena Soil
    }
    case 2516: // The Nokhud Offensive
    case 2515: // The Azure Vault
    case 960: // Temple Of The Jade Serpent
    case 1176: // Shadowmoon Burial Grounds
    case 2521: // Ruby Life Pools
    case 1477: // Halls Of Valor
    case 1571: // Court of Stars
    case 2526: // Algeth'ar Academy
    default:
        break;
    }

    return 0;
}

uint32 ChallengeModeMgr::GetCAForOplote(uint32 challengeLevel)
{
    switch (challengeLevel)
    {
        // Is bug???
    case 0:
    case 1:
        return 0;
    case 2:
    case 3:
        return 184776;
    case 4:
    case 5:
    case 6:
        return 184776;
    case 7:
    case 8:
    case 9:
        return 184776;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return 184776;
    default: // 15+
        return 184776;
    }
}

uint32 ChallengeModeMgr::GetBigCAForOplote(uint32 challengeLevel, uint32& count)
{
    if (challengeLevel <= 10)
        return 0;

    if (challengeLevel >= 15)
        count = challengeLevel - 15;
    else
        count = challengeLevel - 10;

    return 184776;
}

float ChallengeModeMgr::GetChanceItem(uint8 mode, uint32 challengeLevel)
{
    float base_chance = 200.0f;
    float add_chance = 0.0f;

    if (challengeLevel > 10)
        add_chance += (challengeLevel - 10) * 40.0f;

    switch (mode)
    {
    case CHALLENGE_TIMER_LEVEL_3: // 3 chests + 3 levels
    case CHALLENGE_TIMER_LEVEL_2: // 2 chests + 2 levels
    case CHALLENGE_TIMER_LEVEL_1: // 1 chest + 1 level
        base_chance += 100.0f; // 300% is 3 items
        break;
    case CHALLENGE_NOT_IN_TIMER:  // 0 chest
        base_chance += 0.0f; // 200% is 2 items
        break;
    default:
        break;
    }

    base_chance += add_chance;

    return base_chance;
}

bool ChallengeModeMgr::IsChest(uint32 goEntry)
{
    switch (goEntry)
    {
    case 354985: // De other side
    case 354972: // Mist of tirna scihte
    case 354987: // Plaguefall
    case 354991: // theater of pain
    case 354990: // The Necrotic Wake
    case 354989: // Spires of ascension
    case 354986: // Halls of attonement
    case 354988: // Sanguine depts
    case 282736: // Tol Dagor
    case 282737: // Atal'Dazar
    case 288642: // King's Rest
    case 288644: // Siege of Boralus
    case 290544: // The MOTHERLODE!!
    case 290621: // Waycrest Manor
    case 290758: // Temple of Sertraliss
    case 290759: // The Underrot
    case 290761: // Shrine of the Storm
    case 282735: // Freehold
    case 381955: // Ruby Life Pools
    case 381946: // The Azure Vault
    case 381966: // The Nokhud Offensive
    case 381972: // Temple of the Jade Serpent
    case 381971: // Shadowmoon Burial Grounds
    case 381969: // Halls Of Valor
    case 252062: // Halls Of Valor Superior
    case 381970: // Court Of Stars
    case 252687: // Court Of Stars Superior
    case 252688: // Court Of Stars Peerless
        return true;
    default:
        break;
    }

    return false;
}

bool ChallengeModeMgr::IsDoor(uint32 goEntry) //here go the id of doors
{
    switch (goEntry)
    {
    case 211989:
    case 211991:
    case 212972:
    case 211992:
    case 211988:
    case 212282:
    case 212387:
    case 239323:
    case 239408:
        return true;
    default:
        break;
    }

    return false;
}


MapChallengeModeEntry const* ChallengeModeMgr::GetMapChallengeModeEntry(uint32 mapId)
{
    for (uint32 i = 0; i < sMapChallengeModeStore.GetNumRows(); ++i)
        if (MapChallengeModeEntry const* challengeModeEntry = sMapChallengeModeStore.LookupEntry(i))
            if (challengeModeEntry->MapID == mapId)
                return challengeModeEntry;

    return nullptr;
}

MapChallengeModeEntry const* ChallengeModeMgr::GetMapChallengeModeEntryByModeId(uint32 modeId)
{
    for (uint32 i = 0; i < sMapChallengeModeStore.GetNumRows(); ++i)
        if (MapChallengeModeEntry const* challengeModeEntry = sMapChallengeModeStore.LookupEntry(i))
            if (challengeModeEntry->ID == modeId)
                return challengeModeEntry;

    return nullptr;
}

uint32 ChallengeModeMgr::GetDamageMultiplier(uint8 challengeLevel)
{
    if (GtChallengeModeDamageEntry const* challengeDamage = sChallengeModeDamageTable.GetRow(challengeLevel))
        return uint32(100.f * (challengeDamage->Scalar - 1.f));

    return 1;
}

uint32 ChallengeModeMgr::GetHealthMultiplier(uint8 challengeLevel)
{
    if (GtChallengeModeHealthEntry const* challengeHealth = sChallengeModeHealthTable.GetRow(challengeLevel))
        return uint32(100.f * (challengeHealth->Scalar - 1.f));

    return 1;
}

void InstanceScript::SendChallengeModeMapStatsUpdate(Player* player, uint32 challengeId, uint32 recordTime) const
{
    ChallengeByMap* bestMap = sChallengeModeMgr->BestForMember(player->GetGUID());
    if (!bestMap)
        return;

    auto itr = bestMap->find(instance->GetId());
    if (itr == bestMap->end())
        return;

    ChallengeData* best = itr->second;
    if (!best)
        return;

    WorldPackets::Misc::NewPlayerRecord update;
    update.MapID = instance->GetId();
    update.CompletionMilliseconds = best->RecordTime;
    update.ChallengeLevel = challengeId;

    ChallengeMemberList members = best->member;

    if (player)
        player->SendDirectMessage(update.Write());
}

InstanceScript* ChallengeModeMgr::GetInstanceScript() const
{
    return _instanceScript;
}

uint32 ChallengeModeMgr::GetChallengeTimer()
{
    if (!_challengeTimer)
        return 0;

    return _challengeTimer / IN_MILLISECONDS;
}


typedef std::set<ChallengeMember> ChallengeMemberList;

uint32 ChallengeModeMgr::GetRandomChallengeId(uint32 flags/* = 4*/)
{
    std::vector<uint32> challenges;

    // Season1 Dragonflight
   // 399 Ruby Life Pools, 402 Algeth'ar Academy , 401 The Azure Vault, 400 The Nokhud Offensive, 200 Halls of Valor, 210 Court of Stars, 165 Shadowmoon Burial Grounds, 2 Temple of the Jade Serpent
   //
   // Season 2 Dragonflight
   // 406 Halls of Infusion, 405 Brackenhide Hollow, 403 Uldaman: Legacy of Tyr, 404 Neltharus, 206 Neltharion's Lair, 245 Freehold, 251 Underrot, Vortex Pinacle
    for (uint32 i = 0; i < sMapChallengeModeStore.GetNumRows(); ++i)
    {
        if (MapChallengeModeEntry const *challengeModeEntry = sMapChallengeModeStore.LookupEntry(i))
        {
            if (challengeModeEntry->Flags & flags &&
                (challengeModeEntry->ID == 165 ||
                 challengeModeEntry->ID == 200 ||
                 challengeModeEntry->ID == 210 ||
                 challengeModeEntry->ID == 375 || // Mists of Tirna Scithe
                 challengeModeEntry->ID == 376 || // The Necrotic Wake
                 challengeModeEntry->ID == 377 || // De Other Side
                 challengeModeEntry->ID == 378 || // Halls of Atonement
                 challengeModeEntry->ID == 379 || // Plaguefall
                 challengeModeEntry->ID == 380 || // Sanguine Depths
                 challengeModeEntry->ID == 381 || // Spires of Ascension
                 challengeModeEntry->ID == 382 || // Theater of Pain
                 challengeModeEntry->ID == 391 || // Tazavesh: Streets of Wonder
                 challengeModeEntry->ID == 392 || // Tazavesh: So'leah's Gambit
                 challengeModeEntry->ID == 399 ||
                 challengeModeEntry->ID == 400 ||
                 challengeModeEntry->ID == 401 ||
                 challengeModeEntry->ID == 402 ||
                 challengeModeEntry->ID == 403 ||
                 challengeModeEntry->ID == 404 ||
                 challengeModeEntry->ID == 405 ||
                 challengeModeEntry->ID == 406 ||
                 challengeModeEntry->ID == 2  )) // Temp fix, only doable dungeons here
                challenges.push_back(challengeModeEntry->ID);
        }
    }

    if (challenges.empty())
        return 0;

    return Trinity::Containers::SelectRandomContainerElement(challenges);
}

uint32 ChallengeModeMgr::GetRandomChallengeAffixId(uint32 affix, uint32 level/* = 2*/)
{
    std::vector<uint32> affixs;
    switch (affix)
    {
    case 1:
        if (level >= 4)
        {
            affixs.push_back(5);
            affixs.push_back(6);
            affixs.push_back(7);
            affixs.push_back(8);
            affixs.push_back(11);
        }
        break;
    case 2:
        if (level >= 7)
        {
            affixs.push_back(13);
            affixs.push_back(14);
            affixs.push_back(12);
            affixs.push_back(2);
            affixs.push_back(4);
            affixs.push_back(3);
        }
        break;
    case 3:
        if (level >= 10)
        {
            affixs.push_back(9);
            affixs.push_back(10);
            affixs.push_back(15);
        }
        break;
    default:
        break;
    }

    if (affixs.empty())
        return 0;

    return Trinity::Containers::SelectRandomContainerElement(affixs);
}

std::vector<int32> ChallengeModeMgr::GetBonusListIdsForRewards(uint32 baseItemIlevel, uint8 challengeLevel)
{
    if (challengeLevel < 2)
        return {};

    std::vector<std::pair<int32, uint32>> bonusDescriptionByChallengeLevel =
    {
        { 3410, 5  },   // Mythic 2
        { 3411, 5  },   // Mythic 3
        { 3412, 10 },   // Mythic 4
        { 3413, 15 },   // Mythic 5
        { 3414, 20 },   // Mythic 6
        { 3415, 20 },   // Mythic 7
        { 3416, 25 },   // Mythic 8
        { 3417, 25 },   // Mythic 9
        { 3418, 30 },   // Mythic 10
        { 3509, 35 },   // Mythic 11
        { 3510, 40 },   // Mythic 12
        { 3534, 45 },   // Mythic 13
        { 3535, 50 },   // Mythic 14
        { 3535, 55 },   // Mythic 15
    };

    const uint32 baseMythicIlevel = 155;
    std::pair<int32, uint32> bonusAndDeltaPair = bonusDescriptionByChallengeLevel[challengeLevel < 15 ? (challengeLevel - 2): 13];
    return { bonusAndDeltaPair.first, (int32)ItemBonusMgr::GetItemBonusListForItemLevelDelta(baseMythicIlevel - baseItemIlevel + bonusAndDeltaPair.second) };
}

void ChallengeModeMgr::Reward(Player* player, uint8 challengeLevel)
{
    if (!GetMapChallengeModeEntry(player->GetMapId()))
        return;
    uint32 addCA = 0;

    addCA = GetCAForLoot(challengeLevel, player->GetMapId(), false);

    if (addCA)
        player->AddItem(addCA, 1);

    uint32 addBigCA = 0;
    uint32 countBigCA = 0;

    if (challengeLevel > 10)
        addBigCA = GetBigCAForLoot(challengeLevel, player->GetMapId(), false, 0, countBigCA);

    if (addBigCA && countBigCA)
        player->AddItem(addBigCA, countBigCA);

    JournalInstanceEntry const* journalInstance  = sDB2Manager.GetJournalInstanceByMapId(player->GetMapId());
    if (!journalInstance)
        return;

    auto encounters = sDB2Manager.GetJournalEncounterByJournalInstanceId(journalInstance->ID);
    if (!encounters)
        return;

    std::vector<JournalEncounterItemEntry const*> items;
    for (auto encounter : *encounters)
        if (std::vector<JournalEncounterItemEntry const*> const* journalItems = sDB2Manager.GetJournalItemsByEncounter(encounter->ID))
            items.insert(items.end(), journalItems->begin(), journalItems->end());

    if (items.empty())
        return;

    std::vector<ItemTemplate const*> stuffLoots;
    for (JournalEncounterItemEntry const* journalEncounterItem : items)
    {
        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(journalEncounterItem->ItemID);
        if (!itemTemplate)
            continue;

        if (!itemTemplate->IsUsableByLootSpecialization(player, false))
            continue;

        if (itemTemplate->GetInventoryType() != INVTYPE_NON_EQUIP)
            stuffLoots.push_back(itemTemplate);
    }

    ItemTemplate const* randomStuffItem = Trinity::Containers::SelectRandomContainerElement(stuffLoots);
    if (!randomStuffItem)
        return;

    uint32 itemId = randomStuffItem->GetId();
    ItemPosCountVec dest;
    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, 1);
    if (msg != EQUIP_ERR_OK)
    {
        player->SendEquipError(msg, nullptr, nullptr, itemId);
        return;
    }

    std::vector<int32> bonusListIds = GetBonusListIdsForRewards(randomStuffItem->GetBaseItemLevel(), challengeLevel);
    Item* pItem = player->StoreNewItem(dest, itemId, true, GenerateItemRandomBonusListId(itemId), GuidSet(), ItemContext::MythicPlus_End_of_Run, &bonusListIds);
    player->SendNewItem(pItem, 1, true, false, true);

    // This is the old one now tc has implemented displaytoast
    WorldPackets::Misc::DisplayToast displayToast;
    displayToast.Type = DisplayToastType::NewItem;
    displayToast.Quantity = 1;
    displayToast.BonusRoll = pItem->GetItemRandomBonusListId();
    displayToast.DisplayToastMethod = DisplayToastMethod::PersonalLoot;
   //displayToast.bonusListIDs = pItem->m_itemData->BonusListIDs;
    player->SendDirectMessage(displayToast.Write());


    // Hello Sexy Banana
    player->SendDisplayToast(itemId, DisplayToastType::NewItem, false, 1, DisplayToastMethod::PersonalLoot, 0U, pItem);
}

uint32 ChallengeModeMgr::GetCAForLoot(uint32 challengeLevel, uint32 mapID, bool isOplote)
{
    if (!isOplote)
        return 0;

    switch (mapID)
    {
    case 2286: //The Necrotic Wake
    {
        // Lesser Dungeons
        switch (challengeLevel)
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 184378;
        case 4:
        case 5:
        case 6:
            return 184378;
        case 7:
        case 8:
        case 9:
            return 184378;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 184378;
        case 15:
        default:
            return 184378;
        }
    }
    case 2291: //De other side
    case 2287: //Halls of Atonement
    case 2290: //Mists of Tirna Scithe
    case 2289: // plaguefall
    case 2284: //Sanguine Depths
    case 2285: //Spires of Ascension
    case 2293: //Theater of Pain
    {
        // Regular Dungeons
        switch (challengeLevel)
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 184378;
        case 4:
        case 5:
        case 6:
            return 184378;
        case 7:
        case 8:
        case 9:
            return 184378;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 184378;
        case 15:
        default:
            return 184378;
        }
    }
    case 2441: // Tazavesh
    {
        // Greater Dungeons
        switch (challengeLevel)
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 184378;
        case 4:
        case 5:
        case 6:
            return 184378;
        case 7:
        case 8:
        case 9:
            return 184378;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 184378;
        case 15:
        default:
            return 184378;
        }
    }
    default:
        break;
    }

    return 0;
}

uint32 ChallengeModeMgr::GetBigCAForLoot(uint32 challengeLevel, uint32 mapID, bool isOplote, uint32 /*goEntry*/, uint32& count)
{
    if (!isOplote || challengeLevel <= 10)
        return 0;

    if (challengeLevel >= 15)
        count = challengeLevel - 15;
    else
        count = challengeLevel - 10;

    switch (mapID)
    {
    case 2291: //De other side
    {
        // Lesser Dungeons
        return 184378; // Lesser Adept's Spoils
    }
    case 2287: //Halls of Atonement
    case 2290: //Mists of Tirna Scithe
    case 2289: // plaguefall
    case 2284: //Sanguine Depths
    case 2285: //Spires of Ascension
    case 2293: //Theater of Pain
    {
        // Regular Dungeons
        return 184378; // Adept's Spoils
    }
    case 2286: //The Necrotic Wake
    case 2441: // Tazavesh
    {
        // Greater Dungeons
        return 184378; // Greater Adept's Spoils
    }
    default:
        break;
    }

    return 0;
}

bool ChallengeModeMgr::IsPower(uint32 itemId)
{
    switch (itemId)
    {
    case 181477:
    case 184381:
    case 184378:
    case 181541:
    case 184383:
    case 184384:
    case 184382:
    case 181479:
    case 184519:
    /*case 147404:
    case 147405:
    case 147579:
    case 147718:
    case 147809:
    case 147551:
    case 147550:
    case 147549:
    case 147548:
    case 147721:
    case 147819:*/
        return true;
        break;
    default:
        break;
    }
    return false;
}

uint32 ChallengeModeMgr::GetChest(uint32 challangeId)
{
    switch (challangeId)
    {
    case 375:
        return 354972; //Mists of Tirna Scithe
        break;
    case 376:
        return 354990; // The Necrotic Wake
        break;
    case 377:
        return 354985; //  De Other Side
        break;
    case 378:
        return 354986; // Halls of Atonement
        break;
    case 379:
        return 354987; // Plaguefall
        break;
    case 380:
        return 354988; // Sanguine Depths
        break;
    case 381:
        return 354989; // Spires of Ascension
        break;
    case 382:
        return 354991; // Theater of Pain
        break;
    case 391:
        return 354991; // Tazavesh: Streets of Wonder  //chest id??? idk so use theater of pain chest
        break;
    case 392:
        return 354991; // Tazavesh: So'leah's Gambit //chest id???
        break;
    default:
        return 354972;
        break;
    }
    return 0;
}

bool ChallengeModeMgr::HasRecordLastWeek(uint64 p_Guid)
{
    return m_ChallengersOfLastWeek.find(p_Guid) != m_ChallengersOfLastWeek.end();
}
