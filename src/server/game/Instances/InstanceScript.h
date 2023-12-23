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

#ifndef TRINITY_INSTANCE_DATA_H
#define TRINITY_INSTANCE_DATA_H

#include "ZoneScript.h"
#include "Common.h"
#include "Duration.h"
#include "Optional.h"
#include "Position.h"
#include <array>
#include <map>
#include <set>
#include <variant>
#include <bitset>

#define OUT_SAVE_INST_DATA             TC_LOG_DEBUG("scripts", "Saving Instance Data for Instance {} (Map {}, Instance Id {})", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE    TC_LOG_DEBUG("scripts", "Saving Instance Data for Instance {} (Map {}, Instance Id {}) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a)          TC_LOG_DEBUG("scripts", "Loading Instance Data for Instance {} (Map {}, Instance Id {}). Input is '{}'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE    TC_LOG_DEBUG("scripts", "Instance Data Load for Instance {} (Map {}, Instance Id: {}) is complete.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL        TC_LOG_ERROR("scripts", "Unable to load Instance Data for Instance {} (Map {}, Instance Id: {}).", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())

class AreaBoundary;
class Creature;
class Challenge;
class GameObject;
class InstanceMap;
class ModuleReference;
class PersistentInstanceScriptValueBase;
class Player;
class Unit;
struct DungeonEncounterEntry;
struct InstanceSpawnGroupInfo;
enum class CriteriaType : uint8;
enum class CriteriaStartEvent : uint8;
enum Difficulty : uint8;
enum EncounterCreditType : uint8;
enum Affixes : uint32;

//DekkCore

enum Affixes : uint32
{
    Overflowing = 1,
    Skittish = 2,
    Volcanic = 3,
    Necrotic = 4,
    Teeming = 5,
    Raging = 6,
    Bolstering = 7,
    Sanguine = 8,
    Tyrannical = 9,
    Fortified = 10,
    Bursting = 11,
    Grievous = 12,
    Explosive = 13,
    Quaking = 14,
    Relentless = 15,
    Infested = 16,
    Reaping = 117,
    Beguiling = 119,
    Awakened = 120,
    Prideful = 121,
    Inspiring = 122,
    Spiteful = 123,
    Storming = 124,
    Tormented = 128,
    Infernal = 129,
    Encrypted = 130,
    Shrouded = 131,
    Thundering = 132,
    MaxAffixes
};
/* not working on linux gcc
constexpr auto AFFIXES_ALL =
((1 << Affixes::Overflowing) | (1 << Affixes::Skittish) | (1 << Affixes::Volcanic) |
    (1 << Affixes::Necrotic) | (1 << Affixes::Teeming) | (1 << Affixes::Raging) |
    (1 << Affixes::Bolstering) | (1 << Affixes::Sanguine) | (1 << Affixes::Tyrannical) |
    (1 << Affixes::Fortified) | (1 << Affixes::Bursting) | (1 << Affixes::Grievous) |
    (1 << Affixes::FelExplosives) | (1 << Affixes::Quaking) | (1 << Affixes::Relentless) |
    (1 << Affixes::Infested) | (1 << Affixes::Reaping) | (1 << Affixes::Beguiling) | (1 << Affixes::Awakened));*/

/*uint32 const AFFIXES_ALL[MaxAffixes] =
{
    (1 << Affixes::Overflowing)   | (1 << Affixes::Skittish) | (1 << Affixes::Volcanic)   |
    (1 << Affixes::Necrotic)      | (1 << Affixes::Teeming)  | (1 << Affixes::Raging)     |
    (1 << Affixes::Bolstering)    | (1 << Affixes::Sanguine) | (1 << Affixes::Tyrannical) |
    (1 << Affixes::Fortified)     | (1 << Affixes::Bursting) | (1 << Affixes::Grievous)   |
    (1 << Affixes::FelExplosives) | (1 << Affixes::Quaking)  | (1 << Affixes::Relentless) |
    (1 << Affixes::Infested)      | (1 << Affixes::Reaping)  | (1 << Affixes::Beguiling)  | (1 << Affixes::Awakened)
};*/
//Dekkcore
enum EncounterFrameType
{
    ENCOUNTER_FRAME_SET_COMBAT_RES_LIMIT    = 0,
    ENCOUNTER_FRAME_RESET_COMBAT_RES_LIMIT  = 1,
    ENCOUNTER_FRAME_ENGAGE                  = 2,
    ENCOUNTER_FRAME_DISENGAGE               = 3,
    ENCOUNTER_FRAME_UPDATE_PRIORITY         = 4,
    ENCOUNTER_FRAME_ADD_TIMER               = 5,
    ENCOUNTER_FRAME_ENABLE_OBJECTIVE        = 6,
    ENCOUNTER_FRAME_UPDATE_OBJECTIVE        = 7,
    ENCOUNTER_FRAME_DISABLE_OBJECTIVE       = 8,
    ENCOUNTER_FRAME_UNK7                    = 9,    // Seems to have something to do with sorting the encounter units
    ENCOUNTER_FRAME_ADD_COMBAT_RES_LIMIT    = 10,
    // DekkCore >
    ENCOUNTER_FRAME_INSTANCE_END                = 11,
    ENCOUNTER_FRAME_UPDATE_SUPPRESSING_RELEASE  = 13,
    ENCOUNTER_FRAME_UPDATE_ALLOWING_RELEASE     = 12,
    ENCOUNTER_FRAME_INSTANCE_START              = 14
    // < DekkCore
};

// EnumUtils: DESCRIBE THIS
enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4,
    TO_BE_DECIDED = 5
};

enum DoorType
{
    DOOR_TYPE_ROOM          = 0,    // Door can open if encounter is not in progress
    DOOR_TYPE_PASSAGE       = 1,    // Door can open if encounter is done
    DOOR_TYPE_SPAWN_HOLE    = 2,    // Door can open if encounter is in progress, typically used for spawning places
    MAX_DOOR_TYPES
};

static constexpr uint32 MAX_DUNGEON_ENCOUNTERS_PER_BOSS = 4;

struct DungeonEncounterData
{
    uint32 BossId;
    std::array<uint32, MAX_DUNGEON_ENCOUNTERS_PER_BOSS> DungeonEncounterId;
};

struct DoorData
{
    uint32 entry, bossId;
    DoorType type;
};

struct BossBoundaryEntry
{
    uint32 BossId;
    AreaBoundary const* Boundary;
};

struct TC_GAME_API BossBoundaryData
{
    typedef std::vector<BossBoundaryEntry> StorageType;
    typedef StorageType::const_iterator const_iterator;

    BossBoundaryData(std::initializer_list<BossBoundaryEntry> data) : _data(data) { }
    ~BossBoundaryData();
    const_iterator begin() const { return _data.begin(); }
    const_iterator end() const { return _data.end(); }

    private:
        StorageType _data;
};

struct MinionData
{
    uint32 entry, bossId;
};

struct ObjectData
{
    uint32 entry;
    uint32 type;
};

typedef std::vector<AreaBoundary const*> CreatureBoundary;

struct BossInfo
{
    BossInfo() : state(TO_BE_DECIDED) { DungeonEncounters.fill(nullptr); }

    DungeonEncounterEntry const* GetDungeonEncounterForDifficulty(Difficulty difficulty) const;

    EncounterState state;
    GuidSet door[MAX_DOOR_TYPES];
    GuidSet minion;
    CreatureBoundary boundary;
    std::array<DungeonEncounterEntry const*, MAX_DUNGEON_ENCOUNTERS_PER_BOSS> DungeonEncounters;
};

struct DoorInfo
{
    explicit DoorInfo(BossInfo* _bossInfo, DoorType _type)
        : bossInfo(_bossInfo), type(_type) { }
    BossInfo* bossInfo;
    DoorType type;
};

struct MinionInfo
{
    explicit MinionInfo(BossInfo* _bossInfo) : bossInfo(_bossInfo) { }
    BossInfo* bossInfo;
};

struct UpdateBossStateSaveDataEvent
{
    DungeonEncounterEntry const* DungeonEncounter;
    uint32 BossId;
    EncounterState NewState;
};

struct UpdateAdditionalSaveDataEvent
{
    explicit UpdateAdditionalSaveDataEvent(char const* key, std::variant<int64, double> value) : Key(key), Value(value) { }

    char const* Key;
    std::variant<int64, double> Value;
};

typedef std::multimap<uint32 /*entry*/, DoorInfo> DoorInfoMap;
typedef std::pair<DoorInfoMap::const_iterator, DoorInfoMap::const_iterator> DoorInfoMapBounds;

typedef std::map<uint32 /*entry*/, MinionInfo> MinionInfoMap;
typedef std::map<uint32 /*type*/, ObjectGuid /*guid*/> ObjectGuidMap;
typedef std::map<uint32 /*entry*/, uint32 /*type*/> ObjectInfoMap;

class TC_GAME_API InstanceScript : public ZoneScript
{
    public:
        explicit InstanceScript(InstanceMap* map);

        virtual ~InstanceScript();

        InstanceMap* instance;

        // On instance load, exactly ONE of these methods will ALWAYS be called:
        // if we're starting without any saved instance data
        virtual void Create();
        // if we're loading existing instance save data
        void Load(char const* data);

        // When save is needed, this function generates the data
        std::string GetSaveData();

        std::string UpdateBossStateSaveData(std::string const& oldData, UpdateBossStateSaveDataEvent const& event);
        std::string UpdateAdditionalSaveData(std::string const& oldData, UpdateAdditionalSaveDataEvent const& event);
        Optional<uint32> GetEntranceLocationForCompletedEncounters(uint32 completedEncountersMask) const;
        virtual Optional<uint32> ComputeEntranceLocationForCompletedEncounters(uint32 completedEncountersMask) const;

        virtual void Update(uint32 /*diff*/) { }
        void UpdateCombatResurrection(uint32 diff);

        // Used by the map's CannotEnter function.
        // This is to prevent players from entering during boss encounters.
        virtual bool IsEncounterInProgress() const;

        // Called when a creature/gameobject is added to map or removed from map.
        // Insert/Remove objectguid to dynamic guid store
        virtual void OnCreatureCreate(Creature* creature) override;
        virtual void OnCreatureRemove(Creature* creature) override;

        virtual void OnGameObjectCreate(GameObject* go) override;
        virtual void OnGameObjectRemove(GameObject* go) override;

        ObjectGuid GetObjectGuid(uint32 type) const;
        virtual ObjectGuid GetGuidData(uint32 type) const override;

        Creature* GetCreature(uint32 type);
        GameObject* GetGameObject(uint32 type);

        // Triggers a GameEvent
        // * If source is nullptr then event is triggered for each player in the instance as "source"
        void TriggerGameEvent(uint32 gameEventId, WorldObject* source = nullptr, WorldObject* target = nullptr) override;

        // Called when a player successfully enters the instance.
        virtual void OnPlayerEnter(Player* /*player*/) { }
        // Called when a player successfully leaves the instance.
        virtual void OnPlayerLeave(Player* /*player*/) { }

        // Handle open / close objects
        // * use HandleGameObject(0, boolen, GO); in OnObjectCreate in instance scripts
        // * use HandleGameObject(GUID, boolen, nullptr); in any other script
        void HandleGameObject(ObjectGuid guid, bool open, GameObject* go = nullptr);

        // Change active state of doors or buttons
        void DoUseDoorOrButton(ObjectGuid guid, uint32 withRestoreTime = 0, bool useAlternativeState = false);
        void DoCloseDoorOrButton(ObjectGuid guid);

        // Respawns a GO having negative spawntimesecs in gameobject-table
        void DoRespawnGameObject(ObjectGuid guid, Seconds timeToDespawn = 1min);

        // Sends world state update to all players in instance
        void DoUpdateWorldState(int32 worldStateId, int32 value);

        // Send Notify to all players in instance
        void DoSendNotifyToInstance(char const* format, ...);

        // Update Achievement Criteria for all players in instance
        void DoUpdateCriteria(CriteriaType type, uint32 miscValue1 = 0, uint32 miscValue2 = 0, Unit* unit = nullptr);

        // Remove Auras due to Spell on all players in instance
        void DoRemoveAurasDueToSpellOnPlayers(uint32 spell, bool includePets = false, bool includeControlled = false);
        void DoRemoveAurasDueToSpellOnPlayer(Player* player, uint32 spell, bool includePets = false, bool includeControlled = false);

        // Cast spell on all players in instance
        void DoCastSpellOnPlayers(uint32 spell, bool includePets = false, bool includeControlled = false);
        void DoCastSpellOnPlayer(Player* player, uint32 spell, bool includePets = false, bool includeControlled = false);

        // Return wether server allow two side groups or not
        static bool ServerAllowsTwoSideGroups();

        virtual bool SetBossState(uint32 id, EncounterState state);
        EncounterState GetBossState(uint32 id) const { return id < bosses.size() ? bosses[id].state : TO_BE_DECIDED; }
        static char const* GetBossStateName(uint8 state);
        CreatureBoundary const* GetBossBoundary(uint32 id) const { return id < bosses.size() ? &bosses[id].boundary : nullptr; }
        DungeonEncounterEntry const* GetBossDungeonEncounter(uint32 id) const;
        DungeonEncounterEntry const* GetBossDungeonEncounter(Creature const* creature) const;

        // Achievement criteria additional requirements check
        // NOTE: not use this if same can be checked existed requirement types from AchievementCriteriaRequirementType
        virtual bool CheckAchievementCriteriaMeet(uint32 /*criteria_id*/, Player const* /*source*/, Unit const* /*target*/ = nullptr, uint32 /*miscvalue1*/ = 0);

        // Checks boss requirements (one boss required to kill other)
        virtual bool CheckRequiredBosses(uint32 /*bossId*/, Player const* /*player*/ = nullptr) const { return true; }

        // Checks encounter state at kill/spellcast
        void UpdateEncounterStateForKilledCreature(uint32 creatureId, Unit* source);
        void UpdateEncounterStateForSpellCast(uint32 spellId, Unit* source);

        // Used only during loading
        void SetCompletedEncountersMask(uint32 newMask);

        // Returns completed encounters mask for packets
        uint32 GetCompletedEncounterMask() const { return completedEncounters; }

        bool IsEncounterCompleted(uint32 dungeonEncounterId) const;
        bool IsEncounterCompletedInMaskByBossId(uint32 completedEncountersMask, uint32 bossId) const;

        uint32 GetEncounterCount() const { return uint32(bosses.size()); }

        // Sets the entrance location (WorldSafeLoc) id
        void SetEntranceLocation(uint32 worldSafeLocationId);

        // Sets a temporary entrance that does not get saved to db
        void SetTemporaryEntranceLocation(uint32 worldSafeLocationId) { _temporaryEntranceId = worldSafeLocationId; }

        // Get's the current entrance id
        uint32 GetEntranceLocation() const { return _temporaryEntranceId ? _temporaryEntranceId : _entranceId; }

        // Only used by areatriggers that inherit from OnlyOnceAreaTriggerScript
        void MarkAreaTriggerDone(uint32 id) { _activatedAreaTriggers.insert(id); }
        void ResetAreaTriggerDone(uint32 id) { _activatedAreaTriggers.erase(id); }
        bool IsAreaTriggerDone(uint32 id) const { return _activatedAreaTriggers.find(id) != _activatedAreaTriggers.end(); }

        void SendEncounterUnit(uint32 type, Unit* unit = nullptr, uint8 priority = 0);
        void SendEncounterStart(uint32 inCombatResCount = 0, uint32 maxInCombatResCount = 0, uint32 inCombatResChargeRecovery = 0, uint32 nextCombatResChargeTime = 0);
        void SendEncounterEnd();

        void SendBossKillCredit(uint32 encounterId);

        // ReCheck PhaseTemplate related conditions
        void UpdatePhasing();

        void InitializeCombatResurrections(uint8 charges = 1, uint32 interval = 0);
        void AddCombatResurrectionCharge();
        void UseCombatResurrection();
        void ResetCombatResurrections();
        uint8 GetCombatResurrectionCharges() const { return _combatResurrectionCharges; }
        uint32 GetCombatResurrectionChargeInterval() const;

        void RegisterPersistentScriptValue(PersistentInstanceScriptValueBase* value) { _persistentScriptValues.push_back(value); }
        std::string const& GetHeader() const { return headers; }
        std::vector<PersistentInstanceScriptValueBase*>& GetPersistentScriptValues() { return _persistentScriptValues; }

        //DekkCore
        void SetChallenge(Challenge* challenge);
        uint32 getScenarionStep() const;
        void SendMythicPlusMapStatsUpdate(Player* player, uint32 challengeId, uint32 recordTime) const;
        //Dekkcore
    protected:
        void SetHeaders(std::string const& dataHeaders);
        void SetBossNumber(uint32 number) { bosses.resize(number); }
        void LoadBossBoundaries(BossBoundaryData const& data);
        void LoadDoorData(DoorData const* data);
        void LoadMinionData(MinionData const* data);
        void LoadObjectData(ObjectData const* creatureData, ObjectData const* gameObjectData);
        template<typename T>
        void LoadDungeonEncounterData(T const& encounters)
        {
            for (DungeonEncounterData const& encounter : encounters)
                LoadDungeonEncounterData(encounter.BossId, encounter.DungeonEncounterId);
        }

        void AddObject(Creature* obj, bool add);
        void AddObject(GameObject* obj, bool add);
        void AddObject(WorldObject* obj, uint32 type, bool add);

        virtual void AddDoor(GameObject* door, bool add);
        void AddMinion(Creature* minion, bool add);

        virtual void UpdateDoorState(GameObject* door);
        void UpdateMinionState(Creature* minion, EncounterState state);

        void UpdateSpawnGroups();

        // Exposes private data that should never be modified unless exceptional cases.
        // Pay very much attention at how the returned BossInfo data is modified to avoid issues.
        BossInfo* GetBossInfo(uint32 id);

        // Override this function to validate all additional data loads
        virtual void AfterDataLoad() { }

        bool _SkipCheckRequiredBosses(Player const* player = nullptr) const;

    private:
        static void LoadObjectData(ObjectData const* creatureData, ObjectInfoMap& objectInfo);
        void LoadDungeonEncounterData(uint32 bossId, std::array<uint32, MAX_DUNGEON_ENCOUNTERS_PER_BOSS> const& dungeonEncounterIds);
        void UpdateEncounterState(EncounterCreditType type, uint32 creditEntry, Unit* source);

        std::string headers;
        std::vector<BossInfo> bosses;
        std::vector<PersistentInstanceScriptValueBase*> _persistentScriptValues;
        DoorInfoMap doors;
        MinionInfoMap minions;
        ObjectInfoMap _creatureInfo;
        ObjectInfoMap _gameObjectInfo;
        ObjectGuidMap _objectGuids = {};
        uint32 completedEncounters; // DEPRECATED, REMOVE
        std::vector<InstanceSpawnGroupInfo> const* const _instanceSpawnGroups;
        std::unordered_set<uint32> _activatedAreaTriggers;
        uint32 _entranceId;
        uint32 _temporaryEntranceId;
        uint32 _combatResurrectionTimer;
        uint8 _combatResurrectionCharges; // the counter for available battle resurrections
        bool _combatResurrectionTimerStarted;

    #ifdef TRINITY_API_USE_DYNAMIC_LINKING
        // Strong reference to the associated script module
        std::shared_ptr<ModuleReference> module_reference;
    #endif // #ifndef TRINITY_API_USE_DYNAMIC_LINKING

        friend class debug_commandscript;




        // DekkCore >
        public:
            void AddTimedDelayedOperation(uint32 timeout, std::function<void()>&& function)
            {
                emptyWarned = false;
                timedDelayedOperations.push_back(std::pair<uint32, std::function<void()>>(timeout, function));
            }

            std::vector<std::pair<int32, std::function<void()>>>timedDelayedOperations; ///< Delayed operations
            bool emptyWarned; ///< Warning when there are no more delayed operations

            // Execute the parameter function for all players in instance
            void DoOnPlayers(std::function<void(Player*)>&& function);

            void DoAddAuraOnPlayers(uint32 spell);

            void DoCompleteAchievement(uint32 achievement);

            //Scenarios
            void DoSendScenarioEvent(uint32 eventId);
            void GetScenarioByID(Player* p_Player, uint32 p_ScenarioId);
            void DoStartMovie(uint32 movieId);
            void DoNearTeleportPlayers(const Position pos, bool casting);

            void DoPlayConversation(uint32 conversationId);

            void DoTeleportPlayers(uint32 mapId, const Position pos);
            void DoPlayScenePackageIdOnPlayers(uint32 scenePackageId);

             std::array<uint32, 2> GetIslandCount() const { return _islandCount; }

            enum IslandSpells
            {
                SPELL_AZERITE_RESIDUE = 260738,
                SPELL_AZERITE_ENERGY = 265703,
                SPELL_ISLAND_COMPLETE = 245618, // island - complete
            };

            void CastIslandAzeriteAura();
            void GiveIslandAzeriteXpGain(Player* player, ObjectGuid guid, int32 xp);
            void IslandComplete(bool winnerIsAlliance);

            Optional<Position> _checkPointPosition;
            void SetCheckPointPos(Position pos) { _checkPointPosition = pos; }
            Optional<Position> GetCheckPoint() { return _checkPointPosition; }


            // Challenge
            static uint32 const ChallengeModeOrb = 246779;
            static uint32 const ChallengeModeDoor = 239323;

            Challenge* GetChallenge() const;
            bool IsChallenge() const;
            void ResetChallengeMode();
            void RepopPlayersAtGraveyard();

            void AddChallengeModeChests(ObjectGuid chestGuid, uint8 chestLevel);
            ObjectGuid GetChellngeModeChests(uint8 chestLevel);
            void AddChallengeModeDoor(ObjectGuid doorGuid);
            void AddChallengeModeOrb(ObjectGuid orbGuid);
            void StartChallengeMode(uint8 modeid, uint8 level, uint8 affix1, uint8 affix2, uint8 affix3, uint8 affix4);
            void CompleteChallengeMode();
            bool IsChallengeModeStarted() const { return _challengeModeStarted; }
            uint8 GetChallengeModeId() const { return _challengeModeId; }
            uint8 GetChallengeModeLevel() const { return _challengeModeLevel; }
            std::array<uint32, 4> GetAffixes() const;
            bool HasAffix(Affixes affix);
            uint32 GetChallengeModeCurrentDuration() const;
            void SendChallengeModeStart(Player* player = nullptr) const;
            void SendChallengeModeDeathCount(Player* player = nullptr) const;
            void SendChallengeModeElapsedTimer(Player* player = nullptr) const;
            void SendChallengeModeMapStatsUpdate(Player* player, uint32 challengeId, uint32 recordTime) const;
            void SendChallengeModeNewPlayerRecord(Player* player);
            void OnPlayerExit(Player* player);
            void OnPlayerDeath(Player*) override;
            void OnUnitDeath(Unit* unit) override;

            void CastChallengeCreatureSpell(Creature* creature);
            void CastChallengePlayerSpell(Player* player);
            void CastChallengeCreatureSpellOnDeath(Creature* creature);


            // load scenario after challenge mode started
            void SetChallengeModeScenario(uint32 scenarioId) { _challengeModeScenario = scenarioId; }
            void SetChallengeDoorPos(Position pos) { _challengeModeDoorPosition = pos; }
            void SetChallengeStartPos(Position pos) { _challengeModeStartPosition = pos; _checkPointPosition = pos; }
            virtual void SpawnChallengeModeRewardChest() { }
            void SetFontOfPowerPos(Position pos) { _challengeModeFontOfPowerPosition = pos; }
            void SetFontOfPowerPos2(Position pos) { _challengeModeFontOfPowerPosition2 = pos; }
            void SpawnFontOfPower();

            virtual void ShowChallengeDoor() { }
            virtual void HideChallengeDoor() { }

            void AfterChallengeModeStarted();

            std::vector<ObjectGuid> _challengeDoorGuids;
            std::vector<ObjectGuid> _challengeChestGuids;
            ObjectGuid _challengeOrbGuid;
            ObjectGuid _challengeChest;

    private:
            Challenge* _challenge;
            uint32 _inCombatResCount;
            uint32 _maxInCombatResCount;
            uint32 _combatResChargeTime;
            uint32 _nextCombatResChargeTime;
            bool _challengeModeStarted;
            uint8 _challengeModeId;
            uint8 _challengeModeLevel;
            std::array<uint32, 4> _affixes;
            std::bitset<size_t(Affixes::MaxAffixes)> _affixesTest;
            uint32 _challengeModeStartTime;
            uint32 _challengeModeDeathCount;
            Optional<uint32> _challengeModeScenario;
            Optional<Position> _challengeModeDoorPosition;
            Optional<Position> _challengeModeFontOfPowerPosition;
            Optional<Position> _challengeModeFontOfPowerPosition2;
            Optional<Position> _challengeModeStartPosition;
            std::array<uint32, 2> _islandCount;
            uint32 scenarioStep;

    protected:
            GuidUnorderedSet _challengers;
            uint32 _challengeTimer;
            uint32 _affixQuakingTimer;
            uint32 _challengeLevel;
            bool _isKeyDepleted;
            uint32 _mapID;
            uint8 _rewardLevel;
        // < DekkCore
};

class TC_GAME_API PersistentInstanceScriptValueBase
{
protected:
    PersistentInstanceScriptValueBase(InstanceScript& instance, char const* name, std::variant<int64, double> value);

public:
    virtual ~PersistentInstanceScriptValueBase();

    char const* GetName() const { return _name; }

    UpdateAdditionalSaveDataEvent CreateEvent() const
    {
        return UpdateAdditionalSaveDataEvent(_name, _value);
    }

    void LoadValue(int64 value)
    {
        _value.emplace<int64>(value);
    }

    void LoadValue(double value)
    {
        _value.emplace<double>(value);
    }

protected:
    void NotifyValueChanged();

    InstanceScript& _instance;
    char const* _name;
    std::variant<int64, double> _value;
};

template<typename T>
class PersistentInstanceScriptValue : public PersistentInstanceScriptValueBase
{
public:
    PersistentInstanceScriptValue(InstanceScript& instance, char const* name, T value = {})
        : PersistentInstanceScriptValueBase(instance, name, WrapValue(value))
    {
    }

    operator T() const
    {
        return std::visit([](auto v) { return static_cast<T>(v); }, _value);
    }

    PersistentInstanceScriptValue& operator=(T value)
    {
        _value = WrapValue(value);
        NotifyValueChanged();
        return *this;
    }

    void LoadValue(T value)
    {
        _value = WrapValue(value);
    }

private:
    static std::variant<int64, double> WrapValue(T value)
    {
        if constexpr (std::is_integral_v<T> || std::is_enum_v<T>)
            return int64(value);
        else if constexpr (std::is_floating_point_v<T>)
            return double(value);
        else
            return {};
    }
};

#endif // TRINITY_INSTANCE_DATA_H
