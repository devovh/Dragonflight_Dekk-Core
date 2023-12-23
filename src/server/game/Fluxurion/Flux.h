/*
 * Copyright 2023 Fluxurion
 */

#include "Chat.h"
#include "ChatCommand.h"
#include "Common.h"
#include "Containers.h"
#include "Conversation.h"
#include "DisableMgr.h"
#include "DB2Stores.h"
#include "GossipDef.h"
#include "Item.h"
#include "InstanceLockMgr.h"
#include "LFG.h"
#include "LFGPackets.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "QuestPackets.h"
#include "RBAC.h"
#include "ReputationMgr.h"
#include "SpellInfo.h"
#include "UpdateFields.h"
#include "WorldSession.h"

enum ChromieTime
{
    CHROMIE_TIME_CURRENT = 0,
    CHROMIE_TIME_CATACLYSM = 5,
    CHROMIE_TIME_OUTLAND = 6,
    CHROMIE_TIME_NORTHREND = 7,
    CHROMIE_TIME_PANDARIA = 8,
    CHROMIE_TIME_DRAENOR = 9,
    CHROMIE_TIME_LEGION = 10,
    CHROMIE_TIME_SHADOWLANDS = 14,
};

enum FluxLFGType
{
    FLUX_LFG_TYPE_PVP = 0,
    FLUX_LFG_TYPE_INSTANCE = 1,
    FLUX_LFG_TYPE_RAID = 2,
    FLUX_LFG_TYPE_QUEST = 3, // deprecated
    FLUX_LFG_TYPE_ZONE = 4,
    FLUX_LFG_TYPE_HEROIC = 5, // deprecated
    FLUX_LFG_TYPE_CATEGORY = 6
};

enum FluxLFGSubType
{
    FLUX_LFG_SUBTYPE_NONE = 0, // Zones and 10v10 bg
    FLUX_LFG_SUBTYPE_DUNGEON = 1,
    FLUX_LFG_SUBTYPE_RAID = 2,
    FLUX_LFG_SUBTYPE_SCENARIO = 3,
    FLUX_LFG_SUBTYPE_TIMEWALKING_RAID = 4,
    FLUX_LFG_SUBTYPE_OPENWORLD_PVP = 5,
    FLUX_LFG_SUBTYPE_EXPEDITION = 6,
};

enum FluxLFGDifficulty
{
    FLUX_LFG_DIFF_ZONE = 0,
    FLUX_LFG_DIFF_DUNGEON_NORMAL = 1,
    FLUX_LFG_DIFF_DUNGEON_HEROIC = 2,
    FLUX_LFG_DIFF_RAID_10MAN_NORMAL_OLD = 3,
    FLUX_LFG_DIFF_RAID_25MAN_NORMAL_OLD = 4,
    FLUX_LFG_DIFF_RAID_10MAN_HEROIC_OLD = 5,
    FLUX_LFG_DIFF_RAID_25MAN_HEROIC_OLD = 6,
    FLUX_LFG_DIFF_RAID_LFR_OLD = 7,
    FLUX_LFG_DIFF_DUNGEON_MYTHIC_KEYSTONE = 8,
    FLUX_LFG_DIFF_RAID_40MAN = 9,
    FLUX_LFG_DIFF_SCENARIO_HEROIC_1 = 11,
    FLUX_LFG_DIFF_SCENARIO_NORMAL_1 = 12,
    FLUX_LFG_DIFF_RAID_NORMAL = 14,
    FLUX_LFG_DIFF_RAID_HEROIC = 15,
    FLUX_LFG_DIFF_RAID_MYTHIC = 16,
    FLUX_LFG_DIFF_RAID_LFR = 17,
    FLUX_LFG_DIFF_DUNGEON_EVENT = 19,
    FLUX_LFG_DIFF_SCENARIO_EVENT_1 = 20,
    FLUX_LFG_DIFF_DUNGEON_MYTHIC = 23,
    FLUX_LFG_DIFF_SCENARIO_WORLDPVP = 25,
    FLUX_LFG_DIFF_SCENARIO_EVENT_2 = 30,
    FLUX_LFG_DIFF_RAID_TIMEWALKING = 33,
    FLUX_LFG_DIFF_SCENARIO_NORMAL_2 = 38,
    FLUX_LFG_DIFF_SCENARIO_HEROIC_2 = 39,
    FLUX_LFG_DIFF_SCENARIO_MYTHIC = 40,
    FLUX_LFG_DIFF_SCENARIO_PVP = 45,
    // there is more i got bored
};

// Universal Entity - Extending Object with more functions without hard touch TrinityCore files.
class TC_GAME_API Fluxurion
{
public:

    /// Player Functions >
    static bool HasQuest(Player* player, uint32 questID);
    static void ForceCompleteQuest(Player* player, uint32 questID);
    static void AddItemWithToast(Player* player, uint32 itemID, uint16 quantity, std::vector<int32> const& bonusIDs /*= std::vector<int32>()*/);
    static void SendABunchOfItemsInMail(Player* player, std::vector<uint32> BunchOfItems, std::string const& subject, std::vector<int32> const& bonusIDs /*= std::vector<int32>()*/);
    static bool StoreNewItemWithBonus(Player* player, uint32 item_id, uint32 item_count, std::vector<int32> const& bonusIDs /*= std::vector<int32>()*/);
    static void GearUpByLoadout(Player* player, uint32 loadout_purpose, std::vector<int32> const& bonusIDs /*= std::vector<int32>()*/);
    static bool CanTakeQuestFromSpell(Player* player, uint32 questGiverSpellId);
    static void SendPlayerChoice(Player* player, ObjectGuid sender, int32 choiceId);
    static void AddCreditForQuestObjective(Player* player, uint32 questID, uint32 objectiveID);

    /// WorldObject Functions >
    static Position GetPositionFrontOf(WorldObject* worldObject, float distance)
    {
        Position pos = worldObject->GetPosition();

        pos.m_positionX = worldObject->GetPositionX() + (distance * cos(worldObject->GetOrientation()));
        pos.m_positionY = worldObject->GetPositionY() + (distance * sin(worldObject->GetOrientation()));

        return (pos);
    }

    /// UpdateField Functions >
    template<typename T>
    static void SetUpdateFieldValue(Player* player, UF::UpdateFieldSetter<T> setter, typename UF::UpdateFieldSetter<T>::value_type value)
    {
        if (UF::SetUpdateFieldValue(setter, std::move(value)) && player->IsInWorld())
            player->AddToObjectUpdate();
    }

    /// ChromieTime Functions >
    static void SetChromieTime(Player* player, uint16 chromieTime);
    static uint16 GetChromieTime(Player* player);
    static std::string GetChromieTimeName(Player* player);
    static uint8 GetChromieTimeExpansionLevel(uint8 chromieTime);

    /// LFG Functions >
    static lfg::LfgLockMap GetLockedDungeons(ObjectGuid guid);
    static LFGDungeonsEntry GetDungeonEntry(uint32 dungeonID);

    /// Spell Functions >
    static bool IsWarboardSpell(uint32 spellId)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId, DIFFICULTY_NONE);

        for (SpellEffectInfo const& effect : spellInfo->GetEffects())
        {
            if (effect.Effect == SPELL_EFFECT_QUEST_START && (spellId > 257809 && spellId < 258608 || spellId == 343486 || spellId == 343485))
                return true;
        }

        return false;
    }
};
