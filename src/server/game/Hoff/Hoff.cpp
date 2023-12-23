/*
 * Copyright 2023 Hoff
 */

#include "Hoff.h"

Creature* Hoff::FindMapCreature(Map* InMap, ObjectGuid const& guid)
{
    if (!InMap)
        return nullptr;

    Creature* creature = InMap->GetObjectsStore().Find<Creature>(guid);
    return creature && creature->IsInWorld() ? creature : nullptr;
}

QuestObjectives Hoff::GetCreatureQuestObjectives(int32 CreatureEntry)
{
    QuestObjectives OutObjectives;

    // GetCreatureQuestRelations is for starts, GetCreatureQuestInvolvedRelations is for ends
    QuestRelationResult QuestRelationsStarts = sObjectMgr->GetCreatureQuestRelations(CreatureEntry);
    QuestRelationResult QuestRelationEnds = sObjectMgr->GetCreatureQuestInvolvedRelations(CreatureEntry);

    for (uint32 QuestID : QuestRelationsStarts)
    {
        const Quest* quest = sObjectMgr->GetQuestTemplate(QuestID);
        if (!quest)
        {
            continue;
        }
        QuestObjectives ThisQuestObjectives = quest->GetObjectives();
        for (QuestObjective Objective : ThisQuestObjectives)
        {
            OutObjectives.emplace_back(Objective);
        }
    }

    for (uint32 QuestID : QuestRelationEnds)
    {
        const Quest* quest = sObjectMgr->GetQuestTemplate(QuestID);
        if (!quest)
        {
            continue;
        }
        QuestObjectives ThisQuestObjectives = quest->GetObjectives();
        for (QuestObjective Objective : ThisQuestObjectives)
        {
            OutObjectives.emplace_back(Objective);
        }
    }

    return OutObjectives;
}

QuestObjectives Hoff::GetCreatureQuestObjectivesFromType(int32 CreatureEntry, QuestObjectiveType ObjectiveType)
{
    QuestObjectives OutObjectives;

    QuestObjectives AllCreatureObjectives = GetCreatureQuestObjectives(CreatureEntry);
    for (QuestObjective Objective : AllCreatureObjectives)
    {
        if (Objective.Type == ObjectiveType)
        {
            OutObjectives.emplace_back(Objective);
        }
    }
    return OutObjectives;
}

float Hoff::CalculateMovementTimeSeconds(Position StartPos, Position EndPos, float MovementSpeed)
{
    float Distance = StartPos.GetExactDist2d(EndPos);

    return (MovementSpeed != 0.f) ? Distance / MovementSpeed : 0.f;
}

int32 Hoff::GetLfgGroupEntranceLocID(Group* OwningGroup)
{
    const lfg::LFGDungeonData* dungeon = nullptr;

    if (OwningGroup && OwningGroup->isLFGGroup())
    {
        dungeon = sLFGMgr->GetLFGDungeon(sLFGMgr->GetDungeon(OwningGroup->GetGUID()));
    }

    if (!dungeon)
    {
        return 0;
    }

    WorldLocation DungeonEntrance = WorldLocation(dungeon->map, dungeon->x, dungeon->y, dungeon->z, dungeon->o);

    std::unordered_map<uint32, WorldSafeLocsEntry> entries;

    for (auto&& kvp : sObjectMgr->GetWorldSafeLocs())
    {
        if (kvp.second.Loc.GetMapId() == dungeon->map)
            entries.emplace(kvp);
    }

    float ClosestDistance = 15.f;
    int32 ClosestLocID = 0;
    for (const auto& entry : entries)
    {
        WorldSafeLocsEntry const& locEntry = entry.second;
        WorldLocation loc = locEntry.Loc;

        float distance = loc.GetExactDist2d(DungeonEntrance);
        if (distance < ClosestDistance)
        {
            ClosestDistance = distance;
            ClosestLocID = locEntry.ID;
        }
    }

    return ClosestLocID;
}

Position Hoff::GetTargetFollowPosition(Unit* ParentUnit, EFollowAngle Angle, float Distance)
{
    float FollowAngle = 0.f;
    switch (Angle) {
    default:
    case FOLLOW_ANGLE_FRONT:
        FollowAngle = static_cast<float>(2 * M_PI);
        break;
    case FOLLOW_ANGLE_FRONT_LEFT:
        FollowAngle = static_cast<float>(M_PI / 4);
        break;
    case FOLLOW_ANGLE_FRONT_RIGHT:
        FollowAngle = static_cast<float>((7 * M_PI) / 4);
        break;
    case FOLLOW_ANGLE_LEFT:
        FollowAngle = static_cast<float>(M_PI / 2);
        break;
    case FOLLOW_ANGLE_RIGHT:
        FollowAngle = static_cast<float>((3 * M_PI) / 2);
        break;
    case FOLLOW_ANGLE_BACK:
        FollowAngle = static_cast<float>(M_PI);
        break;
    case FOLLOW_ANGLE_BACK_LEFT:
        FollowAngle = static_cast<float>((3 * M_PI) / 4);
        break;
    case FOLLOW_ANGLE_BACK_RIGHT:
        FollowAngle = static_cast<float>((5 * M_PI) / 4);
        break;
    }

    FollowAngle += ParentUnit->GetOrientation();

    Position OutPos = ParentUnit->GetPosition();
    OutPos.m_positionX += Distance * cosf(FollowAngle);
    OutPos.m_positionY += Distance * sinf(FollowAngle);

    float destx, desty, destz, ground, floor;
    destx = OutPos.m_positionX;
    desty = OutPos.m_positionY;

    ground = ParentUnit->GetMapHeight(destx, desty, MAX_HEIGHT);
    floor = ParentUnit->GetMapHeight(destx, desty, OutPos.m_positionZ);
    destz = std::fabs(ground - OutPos.m_positionZ) <= std::fabs(floor - OutPos.m_positionZ) ? ground : floor;

    float step = Distance / 10.0f;

    for (uint8 j = 0; j < 10; ++j)
    {
        // do not allow too big z changes
        if (std::fabs(OutPos.m_positionZ - destz) > 6)
        {
            destx -= step * std::cos(FollowAngle);
            desty -= step * std::sin(FollowAngle);
            ground = ParentUnit->GetMap()->GetHeight(ParentUnit->GetPhaseShift(), destx, desty, MAX_HEIGHT, true);
            floor = ParentUnit->GetMap()->GetHeight(ParentUnit->GetPhaseShift(), destx, desty, OutPos.m_positionZ, true);
            destz = std::fabs(ground - OutPos.m_positionZ) <= std::fabs(floor - OutPos.m_positionZ) ? ground : floor;
        }
        // we have correct destz now
        else
        {
            OutPos.Relocate(destx, desty, destz);
            break;
        }
    }

    Trinity::NormalizeMapCoord(OutPos.m_positionX);
    Trinity::NormalizeMapCoord(OutPos.m_positionY);
    ParentUnit->UpdateGroundPositionZ(OutPos.m_positionX, OutPos.m_positionY, OutPos.m_positionZ);

    return OutPos;
}
