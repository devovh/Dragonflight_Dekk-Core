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

#include "Scenario.h"
#include "Log.h"
#include "Map.h"
#include "Challenge.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Group.h"
#include "Player.h"
#include "ScenarioMgr.h"
#include "ScenarioPackets.h"


Scenario::Scenario(Map* map, ScenarioData const* scenarioData) : _map(map), _data(scenarioData),
_guid(ObjectGuid::Create<HighGuid::Scenario>(map->GetId(), scenarioData->Entry->ID, map->GenerateLowGuid<HighGuid::Scenario>())),
_currentstep(nullptr)
{
    ASSERT(_data);

    for (std::pair<uint8 const, ScenarioStepEntry const*> const& scenarioStep : _data->Steps)
        SetStepState(scenarioStep.second, SCENARIO_STEP_NOT_STARTED);

    if (ScenarioStepEntry const* step = GetFirstStep())
        SetStep(step);
    else
        TC_LOG_ERROR("scenario", "Scenario::Scenario: Could not launch Scenario (id: {}), found no valid scenario step", _data->Entry->ID);
}

Scenario::~Scenario()
{
    for (ObjectGuid guid : _players)
        if (Player* player = ObjectAccessor::GetPlayer(_map, guid))
            SendBootPlayer(player);

    _players.clear();
}

void Scenario::Reset()
{
    CriteriaHandler::Reset();
    SetStep(GetFirstStep());
}

void Scenario::CompleteStep(ScenarioStepEntry const* step)
{
    if (Quest const* quest = sObjectMgr->GetQuestTemplate(step->RewardQuestID))
        for (ObjectGuid guid : _players)
            if (Player* player = ObjectAccessor::GetPlayer(_map, guid))
                player->RewardQuest(quest, LootItemType::Item, 0, nullptr, false);

    if (step->IsBonusObjective())
        return;

    ScenarioStepEntry const* newStep = nullptr;
    for (auto const& _step : _data->Steps)
    {
        if (_step.second->IsBonusObjective())
            continue;

        if (GetStepState(_step.second) == SCENARIO_STEP_DONE)
            continue;

        if (!newStep || _step.second->OrderIndex < newStep->OrderIndex)
            newStep = _step.second;
    }

    SetStep(newStep);
    if (IsComplete())
        CompleteScenario();
    else
        TC_LOG_ERROR("scenario", "Scenario::CompleteStep: Scenario (id: {}, step: {}) was completed, but could not determine new step, or validate scenario completion.", step->ScenarioID, step->ID);
}

void Scenario::CompleteScenario()
{
    return SendPacket(WorldPackets::Scenario::ScenarioCompleted(_data->Entry->ID).Write());
}

void Scenario::SetStep(ScenarioStepEntry const* step)
{
    _currentstep = step;
    if (step)
        SetStepState(step, SCENARIO_STEP_IN_PROGRESS);

    WorldPackets::Scenario::ScenarioState scenarioState;
    BuildScenarioState(&scenarioState);
    SendPacket(scenarioState.Write());
}

void Scenario::OnPlayerEnter(Player* player)
{
    _players.insert(player->GetGUID());
    SendScenarioState(player);
}

void Scenario::OnPlayerExit(Player* player)
{
    _players.erase(player->GetGUID());
    SendBootPlayer(player);
}

bool Scenario::IsComplete()
{
    for (std::pair<uint8 const, ScenarioStepEntry const*> const& scenarioStep : _data->Steps)
    {
        if (scenarioStep.second->IsBonusObjective())
            continue;

        if (GetStepState(scenarioStep.second) != SCENARIO_STEP_DONE)
            return false;
    }

    return true;
}

ScenarioEntry const* Scenario::GetEntry() const
{
    return _data->Entry;
}

ScenarioStepState Scenario::GetStepState(ScenarioStepEntry const* step)
{
    std::map<ScenarioStepEntry const*, ScenarioStepState>::const_iterator itr = _stepStates.find(step);
    if (itr == _stepStates.end())
        return SCENARIO_STEP_INVALID;

    return itr->second;
}

void Scenario::SendCriteriaUpdate(Criteria const * criteria, CriteriaProgress const * progress, Seconds timeElapsed, bool timedCompleted) const
{
    WorldPackets::Scenario::ScenarioProgressUpdate progressUpdate;
    progressUpdate.CriteriaProgress.Id = criteria->ID;
    progressUpdate.CriteriaProgress.Quantity = progress->Counter;
    progressUpdate.CriteriaProgress.Player = progress->PlayerGUID;
    progressUpdate.CriteriaProgress.Date = progress->Date;
    if (criteria->Entry->StartTimer)
        progressUpdate.CriteriaProgress.Flags = timedCompleted ? 1 : 0;

    progressUpdate.CriteriaProgress.TimeFromStart = timeElapsed;
    progressUpdate.CriteriaProgress.TimeFromCreate = Seconds::zero();

    SendPacket(progressUpdate.Write());
}

bool Scenario::CanUpdateCriteriaTree(Criteria const * /*criteria*/, CriteriaTree const * tree, Player * /*referencePlayer*/) const
{
    ScenarioStepEntry const* step = tree->ScenarioStep;
    if (!step)
        return false;

    if (step->ScenarioID != _data->Entry->ID)
        return false;

    ScenarioStepEntry const* currentStep = GetStep();
    if (!currentStep)
        return false;

    if (step->IsBonusObjective())
        return true;

    return currentStep == step;
}

bool Scenario::CanCompleteCriteriaTree(CriteriaTree const* tree)
{
    ScenarioStepEntry const* step = tree->ScenarioStep;
    if (!step)
        return false;

    ScenarioStepState const state = GetStepState(step);
    if (state == SCENARIO_STEP_DONE)
        return false;

    ScenarioStepEntry const* currentStep = GetStep();
    if (!currentStep)
        return false;

    if (!step->IsBonusObjective())
        if (step != currentStep)
            return false;

    return CriteriaHandler::CanCompleteCriteriaTree(tree);
}

void Scenario::CompletedCriteriaTree(CriteriaTree const* tree, Player* /*referencePlayer*/)
{
    ScenarioStepEntry const* step = ASSERT_NOTNULL(tree->ScenarioStep);
    if (!IsCompletedStep(step))
        return;

    SetStepState(step, SCENARIO_STEP_DONE);
    CompleteStep(step);
}

bool Scenario::IsCompletedStep(ScenarioStepEntry const* step)
{
    CriteriaTree const* tree = sCriteriaMgr->GetCriteriaTree(step->Criteriatreeid);
    if (!tree)
        return false;

    return IsCompletedCriteriaTree(tree);
}

void Scenario::SendPacket(WorldPacket const* data) const
{
    for (ObjectGuid guid : _players)
        if (Player* player = ObjectAccessor::GetPlayer(_map, guid))
            player->SendDirectMessage(data);
}

void Scenario::BuildScenarioState(WorldPackets::Scenario::ScenarioState* scenarioState)
{
    scenarioState->ScenarioGUID = _guid;
    scenarioState->ScenarioID = _data->Entry->ID;
    if (ScenarioStepEntry const* step = GetStep())
        scenarioState->CurrentStep = step->ID;
    scenarioState->CriteriaProgress = GetCriteriasProgress();
    scenarioState->BonusObjectives = GetBonusObjectivesData();
    // Don't know exactly what this is for, but seems to contain list of scenario steps that we're either on or that are completed
    for (std::pair<ScenarioStepEntry const* const, ScenarioStepState> const& state : _stepStates)
    {
        if (state.first->IsBonusObjective())
            continue;

        switch (state.second)
        {
            case SCENARIO_STEP_IN_PROGRESS:
            case SCENARIO_STEP_DONE:
                break;
            case SCENARIO_STEP_NOT_STARTED:
            default:
                continue;
        }

        scenarioState->PickedSteps.push_back(state.first->ID);
    }
    scenarioState->ScenarioComplete = IsComplete();
}

ScenarioStepEntry const* Scenario::GetFirstStep() const
{
    // Do it like this because we don't know what order they're in inside the container.
    ScenarioStepEntry const* firstStep = nullptr;
    for (std::pair<uint8 const, ScenarioStepEntry const*> const& scenarioStep : _data->Steps)
    {
        if (scenarioStep.second->IsBonusObjective())
            continue;

        if (!firstStep || scenarioStep.second->OrderIndex < firstStep->OrderIndex)
            firstStep = scenarioStep.second;
    }

    return firstStep;
}

ScenarioStepEntry const* Scenario::GetLastStep() const
{
    // Do it like this because we don't know what order they're in inside the container.
    ScenarioStepEntry const* lastStep = nullptr;
    for (std::pair<uint8 const, ScenarioStepEntry const*> const& scenarioStep : _data->Steps)
    {
        if (scenarioStep.second->IsBonusObjective())
            continue;

        if (!lastStep || scenarioStep.second->OrderIndex > lastStep->OrderIndex)
            lastStep = scenarioStep.second;
    }

    return lastStep;
}

void Scenario::SendScenarioState(Player* player)
{
    WorldPackets::Scenario::ScenarioState scenarioState;
    BuildScenarioState(&scenarioState);
    player->SendDirectMessage(scenarioState.Write());
}

std::vector<WorldPackets::Scenario::BonusObjectiveData> Scenario::GetBonusObjectivesData()
{
    std::vector<WorldPackets::Scenario::BonusObjectiveData> bonusObjectivesData;
    for (std::pair<uint8 const, ScenarioStepEntry const*> const& scenarioStep : _data->Steps)
    {
        if (!scenarioStep.second->IsBonusObjective())
            continue;

        if (sCriteriaMgr->GetCriteriaTree(scenarioStep.second->Criteriatreeid))
        {
            WorldPackets::Scenario::BonusObjectiveData bonusObjectiveData;
            bonusObjectiveData.BonusObjectiveID = scenarioStep.second->ID;
            bonusObjectiveData.ObjectiveComplete = GetStepState(scenarioStep.second) == SCENARIO_STEP_DONE;
            bonusObjectivesData.push_back(bonusObjectiveData);
        }
    }

    return bonusObjectivesData;
}

std::vector<WorldPackets::Achievement::CriteriaProgress> Scenario::GetCriteriasProgress()
{
    std::vector<WorldPackets::Achievement::CriteriaProgress> criteriasProgress;

    if (!_criteriaProgress.empty())
    {
        for (std::pair<uint32 const, CriteriaProgress> const& progressPair : _criteriaProgress)
        {
            WorldPackets::Achievement::CriteriaProgress criteriaProgress;
            criteriaProgress.Id = progressPair.first;
            criteriaProgress.Quantity = progressPair.second.Counter;
            criteriaProgress.Date = progressPair.second.Date;
            criteriaProgress.Player = progressPair.second.PlayerGUID;
            criteriasProgress.push_back(criteriaProgress);
        }
    }

    return criteriasProgress;
}

CriteriaList const& Scenario::GetCriteriaByType(CriteriaType type, uint32 /*asset*/) const
{
    return sCriteriaMgr->GetScenarioCriteriaByTypeAndScenario(type, _data->Entry->ID);
}

void Scenario::SendBootPlayer(Player* player)
{
    WorldPackets::Scenario::ScenarioVacate scenarioBoot;
    scenarioBoot.ScenarioGUID = _guid;
    scenarioBoot.ScenarioID = _data->Entry->ID;
    player->SendDirectMessage(scenarioBoot.Write());
}

//DekkCore
void Scenario::CreateChallenge(Player* player) //declarar instances bfa +sl + df 
{
    Map* map = GetMap();
    if (!player || !map || !map->ToInstanceMap())
        return;

    MapChallengeModeEntry const* m_challengeEntry = player->GetGroup() ? player->GetGroup()->m_challengeEntry : player->m_challengeKeyInfo.challengeEntry;
    if (!m_challengeEntry)
        return;

    _challenge = new Challenge(map, player, GetInstanceId(), this);

    if (!_challenge || !_challenge->_canRun)
        return;


    if (Map* map_ = GetMap())
    {
        if (InstanceMap* instanceMap = map_->ToInstanceMap())
        {
            if (InstanceScript* script = instanceMap->GetInstanceScript())
            {
                script->SetChallenge(_challenge);
                _challenge->SetInstanceScript(script);
            }
        }
    }
    
    switch (m_challengeEntry->ID)
    {
    case 197: // Eye of Azshara
             scenarioId = 1169;
        break;
    case 198: // Darkheart Thicket
        scenarioId = 1172;
        break;
    case 199: // Black Rook Hold
        scenarioId = 1166;
        break;
    case 200: // Halls of Valor
        scenarioId = 1046;
        break;
    case 206: // Neltharion's Lair
        scenarioId = 1174;
        break;
    case 207: // Vault of the Wardens
        scenarioId = 1173;
        break;
    case 208: // Maw of Souls
        scenarioId = 1175;
        break;
    case 209: // The Arcway
        scenarioId = 1177;
        break;
    case 210: // Court of Stars
        scenarioId = 1178;
        break;
    case 227: // Return to Karazhan: Lower
        scenarioId = 1309;
        break;
    case 233: // Cathedral of Eternal Night
        scenarioId = 1335;
        break;
    case 234: // Return to Karazhan: Upper
        scenarioId = 1308;
        break;
    default:
        break;
    }

    _scenarioEntry = sScenarioStore.LookupEntry(scenarioId);
    ASSERT(_scenarioEntry);

   // ScenarioSteps const* _steps = sScenarioMgr->GetScenarioSteps(scenarioId, _challenge->HasAffix(Affixes::Teeming));
  //  ASSERT(_steps); TODO THOR

    currentStep = 0;
   // steps = *_steps;
    if (steps.size() <= currentStep)
    {
        TC_LOG_DEBUG("sql", "CreateChallenge steps {} currentStep {}", steps.size(), currentStep);
        return;
    }

 //   currentTree = GetScenarioCriteriaByStep(currentStep);
    ActiveSteps.clear();
    ActiveSteps.push_back(steps[currentStep]->ID);

    for (auto const& step : steps)
        SetStepState(step, SCENARIO_STEP_NOT_STARTED);

  //  SetCurrentStep(0);

    TC_LOG_ERROR("LOG_FILTER_CHALLENGE", "{} {}, mapID: {}, scenarioID: {}", __FUNCTION__, __LINE__, map->GetId(), scenarioId);
}

Challenge* Scenario::GetChallenge()
{
    return _challenge;
}

Map* Scenario::GetMap()
{
    return curMap;
}

uint32 Scenario::GetInstanceId() const
{
    return instanceId;
}

void Scenario::SendScenarioEvent(Player* player, uint32 eventId)
{
    UpdateCriteria(CriteriaType::AnyoneTriggerGameEventScenario, eventId, 0, 0, nullptr, player);
}

uint32 Scenario::GetScenarioId() const
{
    return scenarioId;
}

bool Scenario::IsCompleted(bool bonus) const
{
    return currentStep == GetStepCount(bonus);
}

uint8 Scenario::GetStepCount(bool withBonus) const
{
    if (withBonus)
        return steps.size();

    uint8 count = 0;
    for (auto const& v : steps)
        if (!v->IsBonusObjective())
            ++count;

    return count;
}

uint32 Scenario::GetCurrentStep() const
{
    return currentStep;
}

void Scenario::SendStepUpdate(Player* player, bool full)
{
    WorldPackets::Scenario::ScenarioState state;
    state.BonusObjectives = GetBonusObjectivesData();
    state.ScenarioID = GetScenarioId();
    state.CurrentStep = currentStep < steps.size() ? steps[currentStep]->ID : -1;
    state.ScenarioComplete = IsCompleted(false);
    state.PickedSteps = ActiveSteps;

    std::vector<ScenarioSpellData> const* scSpells = sObjectMgr->GetScenarioSpells(GetScenarioId());
    if (scSpells)
    {
        for (std::vector<ScenarioSpellData>::const_iterator itr = scSpells->begin(); itr != scSpells->end(); ++itr)
        {
            // if ((*itr).StepId == state.ActiveSteps)
            if ((*itr).StepId == GetCurrentStep())
            {
                WorldPackets::Scenario::ScenarioSpellUpdate spellUpdate;
                spellUpdate.Usable = true;
                spellUpdate.SpellID = (*itr).Spells;
                state.Spells.emplace_back(spellUpdate);
            }
        }
    }

    //if (full)
    //{
      //  CriteriaProgressMap const* progressMap = GetAchievementMgr().GetCriteriaProgressMap();
      //  if (!progressMap->empty())
      //  {
            //for (CriteriaProgressMap::const_iterator itr = progressMap->begin(); itr != progressMap->end(); ++itr)
          //  {
            //    CriteriaProgress const& treeProgress = itr->second;
            //    CriteriaTreeEntry const* criteriaTreeEntry = sCriteriaTreeStore.LookupEntry(itr->first);
            //    if (!criteriaTreeEntry)
            //        continue;

            //    WorldPackets::Achievement::CriteriaTreeProgress progress;
            //    progress.Id = criteriaTreeEntry->CriteriaID;
            //    progress.Quantity = treeProgress.Counter;
            //    progress.Player = ObjectGuid::Create<HighGuid::Scenario>(0, GetScenarioId(), 1); // whats the fuck ?
            //    progress.Flags = 0;
            //    progress.Date = time(nullptr) - treeProgress.date;
            //    progress.TimeFromStart = time(nullptr) - treeProgress.date;
            //    progress.TimeFromCreate = time(nullptr) - treeProgress.date;
            //    state.Progress.push_back(progress);
            //}
     //   }
  //  }

    if (player)
        player->SendDirectMessage(state.Write());
 //   else
   //     BroadCastPacket(state.Write());

    if (full && _challenge)
    {
        _challenge->SendChallengeModeStart(player);
        _challenge->SendStartElapsedTimer(player);
    }
}

//DekkCore
