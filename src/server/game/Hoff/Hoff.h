/*
 * Copyright 2023 Hoff
 */

#ifndef HOFF_H
#define HOFF_H

#include "Chat.h"
#include "ChatCommand.h"
#include "Common.h"
#include "Containers.h"
#include "Conversation.h"
#include "Creature.h"
#include "DisableMgr.h"
#include "DB2Stores.h"
#include "GossipDef.h"
#include "Item.h"
#include "InstanceLockMgr.h"
#include "LFG.h"
#include "LFGPackets.h"
#include "Group.h"
#include "LFGMgr.h"
#include "Mail.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Position.h"
#include "QuestDef.h"
#include "QuestPackets.h"
#include "RBAC.h"
#include "ReputationMgr.h"
#include "SpellInfo.h"
#include "UpdateFields.h"
#include "WorldSession.h"

enum EDungeonCategories
{
    LFG_CATEGORY_CLASSIC_NORMAL = 258,
    LFG_CATEGORY_BURNINGCRUSADE_NORMAL = 259,
    LFG_CATEGORY_BURNINGCRUSADE_HEROIC = 260,
    LFG_CATEGORY_LICHKING_NORMAL = 261,
    LFG_CATEGORY_LICHKING_HEROIC = 262,
    LFG_CATEGORY_CATACLYSM_NORMAL = 300,
    LFG_CATEGORY_CATACLYSM_HEROIC = 301,
    LFG_CATEGORY_HOUROFTWILIGHT_HEROIC = 434,
    LFG_CATEGORY_PANDARIA_NORMAL = 463,
    LFG_CATEGORY_PANDARIA_HEROIC = 462,
    LFG_CATEGORY_DRAENOR_NORMAL = 788,
    LFG_CATEGORY_DRAENOR_HEROIC = 789,
    LFG_CATEGORY_LEGION_NORMAL = 1045,
    LFG_CATEGORY_LEGION_HEROIC = 1046,
    LFG_CATEGORY_BFA_NORMAL = 1670,
    LFG_CATEGORY_BFA_HEROIC = 1671,
    LFG_CATEGORY_SHADOWLANDS_NORMAL = 2086,
    LFG_CATEGORY_SHADOWLANDS_HEROIC = 2087,
    LFG_CATEGORY_DRAGONFLIGHT_NORMAL = 2350,
    LFG_CATEGORY_DRAGONFLIGHT_HEROIC = 2351,
    LFG_CATEGORY_SEASONAL = 230,
    LFG_CATEGORY_DARKMAULCITADEL,
    LFG_CATEGORY_SCENARIOS,
    LFG_CATEGORY_TIMEWALKING_RAID
};

enum EDungeonGroups
{
    GROUP_ALL = 0,
    GROUP_CLASSIC_NORMAL = 1,
    GROUP_BURNINGCRUSADE_NORMAL = 2,
    GROUP_BURNINGCRUSADE_HEROIC = 3,
    GROUP_LICHKING_NORMAL = 4,
    GROUP_LICHKING_HEROIC = 5,
    GROUP_CATACLYSM_NORMAL = 13,
    GROUP_CATACLYSM_HEROIC = 12,
    GROUP_HOUROFTWILIGHT_HEROIC = 0,
    GROUP_PANDARIA_NORMAL = 37,
    GROUP_PANDARIA_HEROIC = 36,
    GROUP_DRAENOR_NORMAL = 47,
    GROUP_DRAENOR_HEROIC = 48,
    GROUP_LEGION_NORMAL = 53,
    GROUP_LEGION_HEROIC = 52,
    GROUP_BFA_NORMAL = 57,
    GROUP_BFA_HEROIC = 58,
    GROUP_SHADOWLANDS_NORMAL = 165,
    GROUP_SHADOWLANDS_HEROIC = 164,
    GROUP_DRAGONFLIGHT_NORMAL = 172,
    GROUP_DRAGONFLIGHT_HEROIC = 171,
    GROUP_SEASONAL = 11,
    GROUP_DARKMAULCITADEL = 166,
    GROUP_SCENARIOS = 38,
    GROUP_TIMEWALKING_RAID = 38
};

enum EFollowAngle
{
    FOLLOW_ANGLE_FRONT,
    FOLLOW_ANGLE_FRONT_LEFT,
    FOLLOW_ANGLE_FRONT_RIGHT,
    FOLLOW_ANGLE_LEFT,
    FOLLOW_ANGLE_RIGHT,
    FOLLOW_ANGLE_BACK,
    FOLLOW_ANGLE_BACK_LEFT,
    FOLLOW_ANGLE_BACK_RIGHT
};

class TC_GAME_API Hoff
{
public:

    /**
    * Since HashMapHolder is only used for players, we use this instead
    * @param InMap - Map creature is in
    * @param guid - Creature GUID
    * @return Creature*
    */
    static Creature* FindMapCreature(Map* InMap, ObjectGuid const& guid);

    /**
    * Returns all quest objectives of a creature, including starts and ends
    * @param CreatureEntry
    * @return QuestObjectives
    */
    static QuestObjectives GetCreatureQuestObjectives(int32 CreatureEntry);

    /**
    * Returns all quest objectives of a specific type from a creature, including starts and ends
    * @param CreatureEntry
    * @param ObjectiveType
    * @return QuestObjectives
    */
    static QuestObjectives GetCreatureQuestObjectivesFromType(int32 CreatureEntry, QuestObjectiveType ObjectiveType);

    /**
    * How long (in seconds) to reach from StartPos to EndPos going a certain speed
    * @param StartPos
    * @param EndPos
    * @param MovementSpeed
    * @return float Time (seconds)
    */
    static float CalculateMovementTimeSeconds(Position StartPos, Position EndPos, float MovementSpeed);

    /**
    * Very specific use cases, for finding entrance locations without an instance lock
    * @param OwningGroup
    * @return int32
    */
    static int32 GetLfgGroupEntranceLocID(Group* OwningGroup);

    /**
    * Get a valid position for a follower/pet to walk on at the given angle
    * @param ParentUnit The original unit the follower is following
    * @param Angle The angle to follow at
    * @param Distance The distance to follow at
    * @return Position The target position
    */
    static Position GetTargetFollowPosition(Unit* ParentUnit, EFollowAngle Angle, float Distance);
};

#endif
