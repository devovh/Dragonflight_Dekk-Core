/*
 * Copyright (C) DekkCore
 * Copyright (C) 2008-2017 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "PassiveAI.h"
#include "PhasingHandler.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "MotionMaster.h"
#include "TemporarySummon.h"
#include "ScriptedEscortAI.h"
#include "CombatAI.h"
#include "Item.h"
#include "ObjectMgr.h"
#include "GridNotifiersImpl.h"

enum ClassHallDH
{
    /// DH Quests
    QUEST_CALL_OF_THE_ILLIDARI_39261 = 39261,
    QUEST_CALL_OF_THE_ILLIDARI_39047 = 39047,


    QUEST_WEAPONS_OF_LEGEND_1 = 40816,
    QUEST_WEAPONS_OF_LEGEND_2 = 40814,

    QUEST_SPOILS_OF_VICTORY = 41070,

    QUEST_ONE_MORE_LEGEND = 44043,

    // Havoc
    QUEST_MAKING_ARRANGEMENTS_1 = 40819,
    QUEST_BY_ANY_MEANS_1 = 39051,
    QUEST_THE_HUNT_1 = 39247,

    QUEST_MAKING_ARRANGEMENTS_2 = 41120,
    QUEST_BY_ANY_MEANS_2 = 41121,
    QUEST_THE_HUNT_2 = 41119,

    // Vengeance
    QUEST_VENGEANCE_WILL_BE_OURS_1 = 40249,
    QUEST_VENGEANCE_WILL_BE_OURS_2 = 41863,

    /// Texts
    KORVAS_BLOODTHORN_TEXT_00 = 0,
    KORVAS_BLOODTHORN_TEXT_01 = 1,

    /// Spells
    SPELL_WEAPONS_OF_LEGEND_PLAYER_CHOICE = 201092,
    SPELL_CALL_OF_THE_WARBLADES = 208464,
    SPELL_FELLSOUL_SLAM = 216164,
    SPELL_SIGIL_OF_POWER = 216228,
    SPELL_SOUL_CARVER = 216188,

    /// Events
    EVENT_CALL_OF_THE_WARBLADES = 1,
    EVENT_FELLSOUL_SLAM = 2,
    EVENT_SIGIL_OF_POWER = 3,
    EVENT_SOUL_CARVER = 4,

    /// Misc
    PLAYER_CHOICE_DH_SELECTION = 255,

    ACTION_RESPONSEID_1 = 600, // Havoc
    ACTION_RESPONSEID_2 = 601, // Vengeance
    QUEST_THE_POWER_TO_SURVIVE = 40816,
    SPELL_PLAYERCHOICE = 201092,
    PLAYER_CHOICE_DH_ARTIFACT_SELECTION = 255,
    PLAYER_CHOICE_DH_HAVOC = 641,
    PLAYER_CHOICE_DH_VENGEANCE = 640,
    SPELL_DH_SPEC_HAVOK = 201093,
    SPELL_DH_SPEC_VENGEANCE = 201094,
    QUEST_ALDRACHI_WARBLADES_CHOSEN = 40818,
    QUEST_TWINBLADES_OFTHE_DECEIVER_CHOSEN = 40817,
    KILL_CREDIT_DH_ARTIFACT_CHOSEN = 105177,
};

enum DataTypes
{
    DATA_STAGE_7 = 7
};


class PlayerScript_Class_Hall_DH : public PlayerScript
{
public:
    PlayerScript_Class_Hall_DH() : PlayerScript("PlayerScript_Class_Hall_DH") {}

    void OnUpdateArea(Player* player, uint32 /*newArea*/, uint32 /*oldArea*/) override
    {
        if (player->GetAreaId() == 8023)       // The_Fel_Hammer
        {
            if (player->GetQuestStatus(QUEST_SPOILS_OF_VICTORY) == QUEST_STATUS_NONE)
            {
                if (!player->GetPhaseShift().HasPhase(7610)) PhasingHandler::AddPhase(player, 7610, true);
            }
            if (player->GetQuestStatus(QUEST_SPOILS_OF_VICTORY) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(7610)) PhasingHandler::RemovePhase(player, 7610, true);
                if (!player->GetPhaseShift().HasPhase(7611)) PhasingHandler::AddPhase(player, 7611, true);
            }
        }
        if (!player->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(player, 169, true);
    }

};


struct npc_korvas_bloodthorn_99343 : public ScriptedAI
{
    npc_korvas_bloodthorn_99343(Creature* creature) : ScriptedAI(creature)
    {
        SayHi = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!who || !who->IsInWorld())
            return;

        if (!me->IsWithinDist(who, 25.0f, false))
            return;

        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();

        if (!player)
            return;

        me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle());

        if (!SayHi)
        {
            SayHi = true;
            Talk(KORVAS_BLOODTHORN_TEXT_00, player);
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_CALL_OF_THE_ILLIDARI_39261 || quest->GetQuestId() == QUEST_CALL_OF_THE_ILLIDARI_39047)
        {
            Talk(KORVAS_BLOODTHORN_TEXT_01, player);
            me->DespawnOrUnsummon(5s);
        }
    }

private:
    bool SayHi;
};

class PlayerScript_DH_artifact_choice : public PlayerScript
{
public:
    PlayerScript_DH_artifact_choice() : PlayerScript("PlayerScript_DH_artifact_choice") {}

    void OnCompleteQuestChoice(Player* player, uint32 choiceID, uint32 responseID)
    {
        if (choiceID != PLAYER_CHOICE_DH_ARTIFACT_SELECTION)
            return;

        switch (responseID)
        {
        case PLAYER_CHOICE_DH_HAVOC:
        {
            player->RemoveRewardedQuest(QUEST_ALDRACHI_WARBLADES_CHOSEN);
            player->KilledMonsterCredit(KILL_CREDIT_DH_ARTIFACT_CHOSEN);

            if (ChrSpecializationEntry const* spec = sChrSpecializationStore.AssertEntry(577))
                player->ActivateTalentGroup(spec);

            break;
        }
        case PLAYER_CHOICE_DH_VENGEANCE:
        {
            player->RemoveRewardedQuest(QUEST_TWINBLADES_OFTHE_DECEIVER_CHOSEN);
            player->KilledMonsterCredit(KILL_CREDIT_DH_ARTIFACT_CHOSEN);

            if (ChrSpecializationEntry const* spec = sChrSpecializationStore.AssertEntry(581))
                player->ActivateTalentGroup(spec);

            break;
        }
        default:
            break;
        }
    }
};

struct go_mardum_control_console : public GameObjectAI
{
    go_mardum_control_console(GameObject* go) : GameObjectAI(go) {}


    enum eCage {
        NPC_ALTRUIS = 95237,
        NPC_KAYN = 95240,
        DATA_ACTION_START = 56,
        QUEST_UNBRIDLED_POWER_ALTRUIS = 41060,
        QUEST_UNBRIDLED_POWER_KAYN = 41037,
        SPELL_ACTIVATING_CONTROL_CONSOLE = 216679,
    };

    bool OnGossipHello(Player* player) override
    {
        me->SetGoState(GO_STATE_READY);
        if (player->GetQuestStatus(QUEST_UNBRIDLED_POWER_ALTRUIS) == QUEST_STATUS_INCOMPLETE) {
            player->CastSpell(me, SPELL_ACTIVATING_CONTROL_CONSOLE, false);
            if (Creature* creature = me->FindNearestCreature(NPC_ALTRUIS, 25.0f, true)) {
                if (TempSummon* waypointAltruis = player->SummonCreature(creature->GetEntry(), creature->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    waypointAltruis->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    waypointAltruis->RemoveNpcFlag(UNIT_NPC_FLAG_VENDOR);
                    waypointAltruis->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    waypointAltruis->AI()->SetData(DATA_ACTION_START, DATA_ACTION_START);
                    creature->ForcedDespawn(1000, Seconds(15));
                }
                me->ResetDoorOrButton();
                return true;
            }
        }
        if (player->GetQuestStatus(QUEST_UNBRIDLED_POWER_KAYN) == QUEST_STATUS_INCOMPLETE) {
            player->CastSpell(me, SPELL_ACTIVATING_CONTROL_CONSOLE, false);
            if (Creature* creature = me->FindNearestCreature(NPC_KAYN, 25.0f, true)) {
                if (TempSummon* waypointKayn = player->SummonCreature(creature->GetEntry(), creature->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    waypointKayn->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    waypointKayn->RemoveNpcFlag(UNIT_NPC_FLAG_VENDOR);
                    waypointKayn->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    waypointKayn->AI()->SetData(DATA_ACTION_START, DATA_ACTION_START);
                    creature->ForcedDespawn(1000, Seconds(15));
                }
                me->ResetDoorOrButton();
                return true;
            }
        }
        return true;
    }
};

class npc_mardum_altruis_ch : public CreatureScript
{
public:
    npc_mardum_altruis_ch() : CreatureScript("npc_mardum_altruis_ch") { }

    enum {
        TEXT_1 = 0, // Whenever you\'re ready.
        TEXT_2 = 1, // This way.Your forces await.
        TEXT_3 = 2, // The Fel Hammer is ours at last. Gaze upon the might of the Twinblades of the Deceiver!
        TEXT_4 = 3, // We will follow our champion into battle, and we will strike down the Legion once and for all!
        TEXT_5 = 4, // Glory to the Illidari!
        EVENT_START_MOVE = 1,
        EVENT_SAY_1 = 3,
        EVENT_SAY_ASHTONGUE = 4,
        EVENT_SAY_COILSKAR = 5,
        EVENT_SAY_SHIVARRA = 6,
        EVENT_SAY_3 = 7,
        EVENT_SAY_4 = 8,
        EVENT_DESPAWN = 9,
        NPC_KILLCREDIT = 102921,
        SPELL_ASSUMING_CONTROL = 203459,
        QUEST_UNBRIDLED_POWER = 41060,
        NPC_GAARDOUN = 98650,
        NPC_LADY_STHENO = 98624,
        NPC_MOTHER_MALEVOLENCE = 98632,
        DATA_START_ANIM = 56,
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_UNBRIDLED_POWER)
            creature->AI()->Talk(TEXT_1);

        return true;
    }

    struct npc_mardum_altruis_ch_AI : public ScriptedAI
    {
        npc_mardum_altruis_ch_AI(Creature* creature) : ScriptedAI(creature) {
            Initialize();
        }

        void Reset() override
        {
            _events.Reset();
        }

        void Initialize() {}

        void UpdateAI(uint32 diff) override
        {
            UpdateVictim();
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_START_MOVE:
                    _events.ScheduleEvent(EVENT_START_MOVE + 1, 2s);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MoveJump(1538.26f, 1417.64f, 237.1087f, 5.8971f, 10.0f, 0.0f);
                    break;
                case EVENT_START_MOVE + 1:
                    _events.ScheduleEvent(EVENT_SAY_1, 7s);
                    me->GetMotionMaster()->MovePoint(1, 1551.416f, 1414.738f, 237.1089f);
                    break;
                case EVENT_SAY_1:
                    _events.ScheduleEvent(EVENT_SAY_ASHTONGUE, 8s);
                    Talk(TEXT_3);
                    break;
                case EVENT_SAY_ASHTONGUE:
                    _events.ScheduleEvent(EVENT_SAY_COILSKAR, 5s);
                    if (Creature* gaardoun = me->FindNearestCreature(NPC_GAARDOUN, me->GetVisibilityRange(), true))
                        gaardoun->AI()->Talk(0);
                    break;
                case EVENT_SAY_COILSKAR:
                    _events.ScheduleEvent(EVENT_SAY_SHIVARRA, 5s);
                    if (Creature* ladyStheno = me->FindNearestCreature(NPC_LADY_STHENO, me->GetVisibilityRange(), true))
                        ladyStheno->AI()->Talk(0);
                    break;
                case EVENT_SAY_SHIVARRA:
                    _events.ScheduleEvent(EVENT_SAY_3, 5s);
                    if (Creature* motherMalevolence = me->FindNearestCreature(NPC_MOTHER_MALEVOLENCE, me->GetVisibilityRange(), true))
                        motherMalevolence->AI()->Talk(0);
                    break;
                case EVENT_SAY_3:
                    _events.ScheduleEvent(EVENT_SAY_4, 7s);
                    Talk(TEXT_4);
                    break;
                case EVENT_SAY_4:
                    _events.ScheduleEvent(EVENT_DESPAWN, 1s);
                    Talk(TEXT_5);
                    break;
                case EVENT_DESPAWN: {
                    std::list<Player*> players;
                    me->GetPlayerListInGrid(players, 50.0f);
                    for (Player* player : players) {
                        if (player->GetQuestStatus(QUEST_UNBRIDLED_POWER) == QUEST_STATUS_INCOMPLETE)
                            player->CastSpell(player, SPELL_ASSUMING_CONTROL, true);
                    }
                    me->DespawnOrUnsummon();
                    break;
                }
                default:
                    break;
                }
            }
            // no melee attacks
        }

        void SetData(uint32 id, uint32 /*value*/) override
        {
            switch (id) {
            case DATA_START_ANIM:
                Talk(TEXT_2);
                _events.ScheduleEvent(EVENT_START_MOVE, 7s);
                break;
            default:
                break;
            }
        }

    private:
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mardum_altruis_ch_AI(creature);
    }
};

enum eGaardoun {
    FOLLOW_GAARDOUN_KILLCREDIT = 103047,
    PHASE_NONE = 0,
    PHASE_CONTINUE = -1,
    DATA_EVENT_STARTER_GUID = 0,
    SAY_KHADGAR_1 = 3,
    SAY_KHADGAR_2 = 4,
    SAY_KHADGAR_3 = 5,
    SAY_KHADGAR_4 = 6,
    WP_START = 1,
    WP_START_TALK = 2,
    WP_NEXT_TALK = 6,
    WP_LAST_TALK = 10,
};

// npc 98650 for the quest 41070 'Spoils of Victory'
class npc_mardum_battlelord_gaardoun : public CreatureScript
{
public:
    npc_mardum_battlelord_gaardoun() : CreatureScript("npc_mardum_battlelord_gaardoun") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mardum_battlelord_gaardounAI(creature);
    }

    struct npc_mardum_battlelord_gaardounAI : public EscortAI
    {
        npc_mardum_battlelord_gaardounAI(Creature* creature) : EscortAI(creature) { Initialize(); }

    private:
        int8 _phase;
        uint32 _moveTimer;
        ObjectGuid m_playerGuid;
        EventMap _events;

        void Initialize()
        {
            _phase = PHASE_NONE;
            _moveTimer = 0;
        }

        void Reset() override
        {
            Initialize();
            _events.Reset();
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            if (player->GetQuestStatus(QUEST_SPOILS_OF_VICTORY) == QUEST_STATUS_INCOMPLETE)
            {

                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                me->RemoveNpcFlag(UNIT_NPC_FLAG_VENDOR);
                me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                me->AI()->Talk(1);
                m_playerGuid = player->GetGUID();
                player->KilledMonsterCredit(98650, ObjectGuid::Empty);
                LoadPath(me->GetEntry());
                Start(false);
                SetDespawnAtFar(false);
            }

            return true;
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            if (UpdateVictim())
            {
                DoMeleeAttackIfReady();
            }

            if (HasEscortState(STATE_ESCORT_NONE))
                return;

            EscortAI::UpdateAI(diff);

            if (_phase)
            {
                if (_moveTimer <= diff)
                {
                    switch (_phase)
                    {
                    case PHASE_CONTINUE:
                        SetEscortPaused(false);
                        _moveTimer = 0 * IN_MILLISECONDS;
                        _phase = PHASE_NONE;
                        break;
                    default:
                        break;
                    }
                }
                else if (!me->IsInCombat())
                    _moveTimer -= diff;
            }
        }

        void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
        {

            switch (waypointId)
            {
            case WP_START_TALK:
                Talk(2);
                break;
            case WP_NEXT_TALK:
                Talk(3);
                break;
            case WP_LAST_TALK:
                Talk(4);
                break;
            case 12:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGuid))
                {
                    player->KilledMonsterCredit(FOLLOW_GAARDOUN_KILLCREDIT);
                    if (player->GetPhaseShift().HasPhase(7610)) PhasingHandler::RemovePhase(player, 7610, true);
                    if (!player->GetPhaseShift().HasPhase(7611)) PhasingHandler::AddPhase(player, 7611, true);
                }
                me->DespawnOrUnsummon(2s, 30s);
                me->SetOrientation(2.990351f);
                break;
            default:
                break;
            }
        }
    };
};

enum eQuests {
    QUEST_CURSED_FORGE_OF_NATHREZIM = 41064,
    OBJECTIVE_CURSED_FORGE_OF_NATHREZIM = 46840,
};

// npc 103025 for the quest 41064 'Cursed Forge of the Nathrezim'
class npc_mardum_battlelord_gaardoun_103025 : public CreatureScript
{
public:
    npc_mardum_battlelord_gaardoun_103025() : CreatureScript("npc_mardum_battlelord_gaardoun_103025") { }

    bool OnQuestAccept(Player* player, Creature* /*creature*/, Quest const* quest) override
    {
        //if (quest->GetQuestId() == QUEST_CURSED_FORGE_OF_NATHREZIM)
        //{
        //    if (Aura* artifactAura = player->GetAura(ARTIFACTS_ALL_WEAPONS_GENERAL_WEAPON_EQUIPPED_PASSIVE))
        //     //   if (Item* artifact = player->GetItemByGuid(artifactAura->GetCastItemGUID()))
        //           // if (artifact->GetTotalPurchasedArtifactPowers() == 0)
        //             //   artifact->GiveArtifactXp(100, nullptr, 0);
        //}
        return true;
    }
};

// temporary until necessary criteria will be implemented
class PlayerScript_mardum_artifact_empowered : public PlayerScript
{
public:
    PlayerScript_mardum_artifact_empowered() : PlayerScript("PlayerScript_mardum_artifact_empowered") {}

    uint32 checkTimer = 1000;

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (checkTimer <= diff)
        {
            if (player->GetClass() == CLASS_DEMON_HUNTER && player->GetZoneId() == 8022 && player->GetQuestStatus(QUEST_CURSED_FORGE_OF_NATHREZIM) == QUEST_STATUS_INCOMPLETE)
            {
                if (Aura* artifactAura = player->GetAura(ARTIFACTS_ALL_WEAPONS_GENERAL_WEAPON_EQUIPPED_PASSIVE))
                    //  if (Item* artifact = player->GetItemByGuid(artifactAura->GetCastItemGUID()))
                         // if (artifact->GetTotalPurchasedArtifactPowers() != 0)
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_CURSED_FORGE_OF_NATHREZIM))
                        for (QuestObjective const& obj : quest->GetObjectives())
                            if (obj.ObjectID == OBJECTIVE_CURSED_FORGE_OF_NATHREZIM) {
                                player->SetQuestObjectiveData(obj, 1);
                                player->SendQuestUpdateAddCredit(quest, ObjectGuid::Empty, obj, 1);
                            }
            }

            checkTimer = 1000;
        }
        else checkTimer -= diff;
    }
};

enum eQuest41066 {
    QUEST_THE_HUNTERS_GAZE = 41066,
    GO_EMPOWERED_NETHER_CRUCIBLE = 250677,
    KILLCREDIT_SPELL_GAZING = 203803,
    KILLCREDIT_GAZE_UPON_LEGION = 103088,
    SPELL_GAZING = 203802,
    SCENE_GAZE_UPON_LEGION_ID = 1203,
};

// scene 1203 "Gazing Upon Legion" - teleport player back to the fel hammer on complete
class scene_gaze_upon_legion : public SceneScript
{
public:
    scene_gaze_upon_legion() : SceneScript("scene_gaze_upon_legion") { }

    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        player->TeleportTo(1519, 1568.698f, 1414.967f, 217.799f, 3.3862f);
        player->KilledMonsterCredit(KILLCREDIT_GAZE_UPON_LEGION, ObjectGuid::Empty);
        player->RemoveAurasDueToSpell(SPELL_GAZING);
    }
};

// gameobject 250677 for quest 41066 (The Hunters Gaze)
struct go_mardum_empowered_nether_crusible : public GameObjectAI
{
    go_mardum_empowered_nether_crusible(GameObject* go) : GameObjectAI(go) {}


    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        if (player->GetQuestStatus(QUEST_THE_HUNTERS_GAZE) == QUEST_STATUS_INCOMPLETE)
        {
            player->CastSpell(player, SPELL_GAZING);
        }
        return true;
    }
};

// playerscript to start scene after teleportation
class PlayerScript_start_gazing : public PlayerScript
{
public:
    PlayerScript_start_gazing() : PlayerScript("PlayerScript_start_gazing") {}

    uint32 checkTimer = 200;
    bool _sceneStarted = false;

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (checkTimer <= diff)
        {
            if (player->GetClass() == CLASS_DEMON_HUNTER && player->GetAreaId() == 8553 &&
                player->GetQuestStatus(QUEST_THE_HUNTERS_GAZE) == QUEST_STATUS_INCOMPLETE &&
                !_sceneStarted)
            {
                _sceneStarted = true;
                player->GetSceneMgr().PlayScene(SCENE_GAZE_UPON_LEGION_ID);
                // player->AddDelayedConversation(2000, 6801); // update sql file
            }
            checkTimer = 200;
        }
        else checkTimer -= diff;
    }
};

class q_paradise_lost : public QuestScript
{
public:
    q_paradise_lost() : QuestScript("q_paradise_lost") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus oldStatus, QuestStatus newStatus) override
    {
        if (oldStatus == QUEST_STATUS_NONE && newStatus == QUEST_STATUS_INCOMPLETE)
        {
            player->CastSpell(player, 191808); // Azsuna Chosen
        }
    }
};

class q_stormheim : public QuestScript
{
public:
    q_stormheim() : QuestScript("q_stormheim") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus oldStatus, QuestStatus newStatus) override
    {
        if (oldStatus == QUEST_STATUS_NONE && newStatus == QUEST_STATUS_INCOMPLETE)
        {
            player->CastSpell(player, 191812); // Stormheim Chosen
        }
    }
};

class q_the_lone_mountain : public QuestScript
{
public:
    q_the_lone_mountain() : QuestScript("q_the_lone_mountain") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus oldStatus, QuestStatus newStatus) override
    {
        if (oldStatus == QUEST_STATUS_NONE && newStatus == QUEST_STATUS_INCOMPLETE)
        {
            player->CastSpell(player, 191813); // Highmountain Chosen
        }
    }
};

class q_the_tranquil_forest : public QuestScript
{
public:
    q_the_tranquil_forest() : QuestScript("q_the_tranquil_forest") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus oldStatus, QuestStatus newStatus) override
    {
        if (oldStatus == QUEST_STATUS_NONE && newStatus == QUEST_STATUS_INCOMPLETE)
        {
            player->CastSpell(player, 191809); // Val'sharah Chosen
        }
    }
};

void AddSC_class_hall_dh()
{
    new PlayerScript_Class_Hall_DH();
    RegisterCreatureAI(npc_korvas_bloodthorn_99343);
    new PlayerScript_DH_artifact_choice();
    RegisterGameObjectAI(go_mardum_control_console);
    new npc_mardum_altruis_ch();
    new npc_mardum_battlelord_gaardoun();
    //   RegisterCreatureAI(npc_mardum_battlelord_gaardoun_103025);
    new PlayerScript_mardum_artifact_empowered();
    new PlayerScript_start_gazing();
    RegisterGameObjectAI(go_mardum_empowered_nether_crusible);
    new scene_gaze_upon_legion();
    new q_paradise_lost();
    new q_stormheim();
    new q_the_lone_mountain();
    new q_the_tranquil_forest();
}
