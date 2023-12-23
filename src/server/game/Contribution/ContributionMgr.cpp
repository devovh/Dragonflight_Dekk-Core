/*
    Thordekk
*/
#include "ContributionMgr.h"
#include "MiscPackets.h"
#include "WorldStateMgr.h"
#include "BattlegroundMgr.h"

ContributionMgr& ContributionMgr::Instance()
{
    static ContributionMgr instance;
    return instance;
}

ContributionMgr::ContributionMgr()
{
    _contributionObjects.clear();
    m_nextUpdate = 0;
}

void ContributionMgr::Update(uint32 diff, Player* player)
{
    if (_contributionObjects.empty())
        return;

    m_nextUpdate += diff;
    if (sWorld->getIntConfig(CONFIG_INTERVAL_SAVE) <= m_nextUpdate)
    {
        for (auto& v : _contributionObjects)
        {
            switch (v.second.State)
            {
                case ContributionData::ContributionState::CONTRIBUTION_STATE_ACTIVE:
                    v.second.CurrentLifeTimer += m_nextUpdate;
                    if (v.second.CurrentLifeTimer >= v.second.UpTimeSecs)
                        OnChangeContributionState(player, v.first, ContributionData::ContributionState::CONTRIBUTION_STATE_UNDERATTACK);
                    break;
                case ContributionData::ContributionState::CONTRIBUTION_STATE_BUILDING:
                    v.second.CurrentUnderAtackTimer += m_nextUpdate;
                    if (v.second.CurrentUnderAtackTimer >= v.second.DownTimeSecs)
                        OnChangeContributionState(player, v.first, ContributionData::ContributionState::CONTRIBUTION_STATE_DESTROYED);
                    break;
                default:
                    break;
            }
        }
        m_nextUpdate = 0;
    }
}

void ContributionMgr::Initialize()
{
    for (auto v : sManagedWorldStateStore)
    {
        auto& obj = _contributionObjects[v->ID];
        obj.CurrentLifeTimer = 0;
        obj.CurrentUnderAtackTimer = 0;
        obj.UpTimeSecs = v->UpTimeSecs;
        obj.DownTimeSecs = v->DownTimeSecs;
        obj.State = ContributionData::ContributionState::CONTRIBUTION_STATE_ACTIVE;
        obj.WorldStateVareables[0] = v->CurrentStageWorldStateID;
        obj.WorldStateVareables[1] = v->ProgressWorldStateID;
        obj.WorldStateVareables[2] = v->OccurrencesWorldStateID[3];
    }
}

inline uint32 GetPersonalTracker(uint32 contributionID)
{
    switch (contributionID)
    {
        case ContributionData::CONTRIBUTION_MAGE_TOWER:
            return CURRENCY_TYPE_LEGIONFALL_PERSONAL_TRACKER_MAGE_TOWER;
        case ContributionData::CONTRIBUTION_COMMAND_CENTER:
            return CURRENCY_TYPE_LEGIONFALL_PERSONAL_TRACKER_COMMAND_TOWER;
        case ContributionData::CONTRIBUTION_NETHER_DISRUPTOR:
            return CURRENCY_TYPE_LEGIONFALL_PERSONAL_TRACKER_NETHER_TOWER;
        default:
            return 0;
    }
}

void ContributionMgr::Contribute(Player* player, uint8 /*contributuinID*/)
{
    //@TODO correct packet to usage ContributionResult

    if (!player->HasCurrency(CURRENCY_TYPE_LEGIONFALL_WAR_SUPPLIES, 100))
    {
        WorldPackets::Misc::DisplayGameError display(GameError::ERR_NOT_ENOUGH_CURRENCY);
        player->SendDirectMessage(display.Write());
        return;
    }

    uint32 rewardQuest = 0;
//    for (auto v : sManagedWorldStateInputStore)
//        if (v->ContributionID == contributuinID)
//            rewardQuest = v->RewardQuest;

    if (!rewardQuest)
    {
        WorldPackets::Misc::DisplayGameError display(GameError::ERR_CANT_DO_THAT_RIGHT_NOW);
        player->SendDirectMessage(display.Write());
        return;
    }

    auto quest = sObjectMgr->GetQuestTemplate(rewardQuest);
    if (!quest || player->CanAddQuest(quest, true))
    {
        WorldPackets::Misc::DisplayGameError display(GameError::ERR_CANT_DO_THAT_RIGHT_NOW);
        player->SendDirectMessage(display.Write());
        return;
    }

   // player->ModifyCurrency(CURRENCY_TYPE_LEGIONFALL_WAR_SUPPLIES, -100, false, true);
   // player->ModifyCurrency(GetPersonalTracker(contributuinID), player->GetCurrency(GetPersonalTracker(contributuinID)) + 1, false, true);

    player->AddQuest(quest, nullptr);

    if (player->CanCompleteQuest(rewardQuest))
        player->CompleteQuest(rewardQuest);

//    auto var1 = _contributionObjects[contributuinID].WorldStateVareables[1];

    //! ToDo: registre contributuin at worldstate mgr.
}

void ContributionMgr::OnChangeContributionState(Player* player, uint32 contribuiontID, ContributionData::ContributionState newState)
{
    auto& obj = _contributionObjects[contribuiontID];
    obj.State = newState;

    switch (newState)
    {
    case ContributionData::ContributionState::CONTRIBUTION_STATE_BUILDING:
    {
        obj.CurrentUnderAtackTimer = 0;
        obj.DownTimeSecs = 0;
        break;
    }
    case ContributionData::ContributionState::CONTRIBUTION_STATE_ACTIVE:
    {
        obj.CurrentUnderAtackTimer = 0;
        obj.DownTimeSecs = 0;
        break;
    }
    case ContributionData::ContributionState::CONTRIBUTION_STATE_UNDERATTACK:
    {
        obj.CurrentLifeTimer = 0;
        obj.DownTimeSecs = 0;
        break;
    }
    case ContributionData::ContributionState::CONTRIBUTION_STATE_DESTROYED:
    {
        obj.CurrentLifeTimer = 0;
        obj.CurrentUnderAtackTimer = 0;
        obj.UpTimeSecs = 0;
        obj.DownTimeSecs = 0;
        player->ModifyCurrency(GetPersonalTracker(contribuiontID), player->HasCurrency(GetPersonalTracker(contribuiontID), 1) + 1);
        break;
    }
    default:
        break;
    }

}

void ContributionMgr::ContributionGetState(Player* player, uint32 contributionID, uint32 contributionGuid)
{
    WorldPackets::Misc::ContributionResponse data;
    data.Data = 0;
    data.ContributionID = contributionID;
    data.ContributionGUID = contributionGuid;
    player->SendDirectMessage(data.Write());
}

Contribution::~Contribution()
{

}

Contribution::Contribution() : ID(0), StartTime(0), Progress(0)
{

}

uint32 Contribution::GetTotalProgress(uint32 p_Time) const
{
    uint32 l_MinutesElapsed = (p_Time - StartTime) / TimeConstants::MINUTE;
    return Progress + (Data->AccumulationAmountPerMinute * l_MinutesElapsed);
}

uint32 Contribution::GetTotalDepletion(uint32 p_Time) const
{
    uint32 l_MinutesElapsed = (p_Time - StartTime) / TimeConstants::MINUTE;
    return Data->DepletionAmountPerMinute * l_MinutesElapsed;
}

uint32 Contribution::GetEndStateTime() const
{
    switch (State)
    {
    case eContributionStates::Active:
    {
        return StartTime + Data->UpTimeSecs;
    }
    case eContributionStates::UnderAttack:
    {
        uint32 l_TotalTime = Data->DepletionStateTargetValue / Data->DepletionAmountPerMinute;
        return StartTime + (l_TotalTime * TimeConstants::MINUTE);
    }
    case eContributionStates::Destroyed:
    {
        return StartTime + Data->DownTimeSecs;
    }
    default:
        break;
    }

    return 0;
}

bool Contribution::IsActive() const
{
    switch (State)
    {
    case eContributionStates::Active:
    case eContributionStates::UnderAttack:
        return true;
    default:
        break;
    }

    return false;
}

bool Contribution::IsAfterHalfOfCurrentState() const
{
    uint32 l_EndTime = GetEndStateTime();
    if (!l_EndTime)
        return false;

    uint32 l_Now = sWorld->GetGameTime();
    return l_EndTime - l_Now < l_Now - StartTime;
}

uint32 Contribution::GetHalfCurrentStateTimer() const
{
    uint32 l_EndTime = GetEndStateTime();
    if (!l_EndTime)
        return 0;

    return StartTime + ((l_EndTime - StartTime) / 2);
}
