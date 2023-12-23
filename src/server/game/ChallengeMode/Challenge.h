/*
 * Copyright (C) Latin Core Team
 *
 */

#ifndef TRINITY_CHALLENGE_H
#define TRINITY_CHALLENGE_H

#include "Common.h"
#include "FunctionProcessor.h"
#include "Scenario.h"
#include "InstanceScript.h"

struct ChallengeEntry;
class Scenario;

enum ChallengeSpells : uint32
{
    ChallengersMight                = 206150, /// generic creature aura
    ChallengersBurden               = 206151, /// generic player aura
    ChallengerBolstering            = 209859,
    ChallengerNecrotic              = 209858,
    ChallengerOverflowing           = 221772,
    ChallengerSanguine              = 226489,
    ChallengerRaging                = 228318,
    ChallengerSummonVolcanicPlume   = 209861,
    ChallengerVolcanicPlume         = 209862,
    ChallengerBursting              = 240443,
    ChallengerQuake                 = 240447,
    ChallengerGrievousWound         = 240559,

    //Explosive
    SPELL_FEL_EXPLOSIVES_SUMMON_1   = 240444, //Short dist
    SPELL_FEL_EXPLOSIVES_SUMMON_2   = 243110, //Long dist
    SPELL_FEL_EXPLOSIVES_VISUAL     = 240445,
    SPELL_FEL_EXPLOSIVES_DMG        = 240446,

    SPELL_CHALLENGE_ANTIKICK        = 305284,
};

enum ChallengeNpcs : uint32
{
    NpcVolcanicPlume        = 105877,
    NPC_FEL_EXPLOSIVES      = 120651,
};

enum MiscChallengeData : uint32
{
    ChallengeDelayTimer     = 10,

};

class Challenge : public InstanceScript
{
public:
    Challenge(Map* map, Player* player, uint32 instanceID, Scenario* scenario);
    ~Challenge();

    void SetInstanceScript(InstanceScript* instanceScript);
    void OnPlayerEnterForScript(Player* player);
    void OnPlayerLeaveForScript(Player* player);
    void OnPlayerDiesForScript(Player* player);
    void OnCreatureCreateForScript(Creature* creature);
    void OnCreatureRemoveForScript(Creature* creature);
    void OnCreatureUpdateDifficulty(Creature* creature);
    void EnterCombatForScript(Creature* creature, Unit* enemy);
    void CreatureDiesForScript(Creature* creature, Unit* killer);
    void OnGameObjectCreateForScript(GameObject* /*go*/) override {}
    void OnGameObjectRemoveForScript(GameObject* /*go*/) override {}
    void OnUnitCharmed(Unit* unit, Unit* charmer);
    void OnUnitRemoveCharmed(Unit* unit, Unit* charmer);

    void Update(uint32 diff) override;

    bool CanStart();
    void Start();
    void Complete();

    void BroadcastPacket(WorldPacket const* data) const;

    void HitTimer();

    uint32 GetChallengeLevel() const;
    std::array<uint32, 4> GetAffixes() const;
    bool HasAffix(Affixes affix);

    uint32 GetChallengeTimerToNow() const;
    void ModChallengeTimer(uint32 timer);
    uint32 GetChallengeTimer();

    void ResetGo();
    void SendStartTimer(Player* player = nullptr);
    void SendStartElapsedTimer(Player* player = nullptr);
    void SendChallengeModeStart(Player* player = nullptr);
    void SendChallengeModeNewPlayerRecord(Player* player);
    void SummonWall(Player* player);
    uint8 GetItemCount(ObjectGuid guid) const;
    uint8 GetLevelBonus() const;
    void SendDeathCount(Player* player);

    GuidUnorderedSet _challengers;
    bool _checkStart;
    bool _canRun;
    bool _run;
    bool _complete;

    ObjectGuid m_gguid;
    ObjectGuid m_ownerGuid;
    ObjectGuid m_itemGuid;

    uint32 _challengeTimer;
    uint32 _affixQuakingTimer;

    FunctionProcessor m_Functions;
    uint32 _mapID;

private:
    std::map<ObjectGuid, uint8> _countItems;
    ObjectGuid _creator;
    std::array<uint32, 4> _affixes;
    std::bitset<size_t(Affixes::MaxAffixes)> _affixesTest;
    uint16 _chestTimers[3];
    Item* _item{};
    Map* _map{};
    MapChallengeModeEntry const* _challengeEntry{};
    uint32 _challengeLevel;
    uint32 _instanceID;
    uint8 _rewardLevel;
    bool _isKeyDepleted{};
    Scenario* _scenario{};
    uint32 _deathCount{};
    InstanceScript* _instanceScript;
};

#endif
