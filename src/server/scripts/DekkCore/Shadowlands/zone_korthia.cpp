/*
 * Copyright DekkCore team 2023
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

#include "Creature.h"
#include "GameObjectAI.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "TemporarySummon.h"

enum Areakorthia
{
    AREA_KORTHIA = 13570,
    AREA_KEEPERS_RESPITE = 13625,
    AREA_SEEKERS_QUORUM = 13628,
    AREA_SCHOLARS_DEN = 13627,
    AREA_HOPES_ASCENT = 13630,
    AREA_VAULT_OF_CECRETS = 13631,
    AERA_PATH_OF_UNDERSTANDING = 13677,
};

enum eventkorthia
{
    EVENT_THE_CITY_OF_SECRETS_I = 100,
    EVENT_THE_CITY_OF_SECRETS_II,
    EVENT_THE_CITY_OF_SECRETS_III,
    EVENT_WHO_IS_THE_MAW_WALKER = 200,
    EVENT_CHARGE_OF_THE_COVENANTS = 300,
    EVENT_SUMMONCOVENANT_RANDOM,
    EVENT_LET_THE_ANIMA_FLOW_PHASE_I = 400,
    EVENT_LET_THE_ANIMA_FLOW_PHASE_II,
    EVENT_LET_THE_ANIMA_FLOW_PHASE_III,
    EVENT_LET_THE_ANIMA_FLOW_PHASE_IV,
    EVENT_ANIMA_FLOW_START = 500,
    EVENT_ANIMA_FLOW_ZONE_START,
    EVENT_ANIMA_HOOKING_OVER_START,
    EVENT_INTO_THE_VAULT_I = 600,
    EVENT_INTO_THE_VAULT_II,
    EVENT_ATTUNING_SENARA_TO_LODESTAFF = 700,
};

enum Actionkorthia
{
    ACTION_ECHOES_OF_FATE_COMPLETE = 10100,
    ACTION_LET_THE_ANIMA_FLOW = 10200,
    ACTION_ANIMA_FLOW_START = 10300,
    ACTION_INTO_THE_VAULT_FINAL = 10400,
    ACTION_INTERRUPT_THE_INTERROGATIONS = 10500,
    ACTION_ATTUNING_SENARA_TO_LODESTAFF = 10600,
};

enum Questkorthia
{
    QUEST_KORTHIA_THE_CITY_OF_SECRETS = 63663,
    QUEST_WHO_IS_THE_MAW_WALKER = 63994,
    QUEST_OPENING_TO_ORIBOS = 63665,
    QUEST_CHARGE_OF_THE_COVENANTS = 64007,
    QUEST_A_MATTER_OF_URGENCY = 63755,
    QUEST_RESCUED_FROM_TORMENT = 63756,
    QUEST_THE_LAST_PLACE_YOU_LOOK = 63758,
    QUEST_ROOTING_THE_LOOTERS = 63757,
    QUEST_REDIRECT_THE_SEARCH = 63759,
    QUEST_ECHOES_OF_FATE = 63760,
    QUEST_UNDER_THE_ILLUSION = 63762,
    QUEST_THEY_COULD_BE_ANYONE = 63763,
    QUEST_VAULT_OF_SECRETS = 63703,
    QUEST_LET_THE_ANIMA_FLOW = 63706,
    QUEST_SECRETS_OF_THE_VAULT = 63709,
    QUEST_THE_ANIMA_TRAIL = 63710,
    QUEST_BONE_TOOLS = 63711,
    QUEST_LOST_RECORD = 63712,
    QUEST_HOOKING_OVER = 63713,
    QUEST_TO_THE_VAULT = 63714,
    QUEST_DEFENDING_THE_VAULT = 63717,
    QUEST_KEEPERS_OF_KORTHIA = 63722,
    QUEST_INTO_THE_VAULT = 63725,
    QUEST_UNTANGLING_THE_SIGIL = 63726,
    QUEST_THE_PRIMUS_RETURNS = 63727,
    QUEST_INTERRUPT_THE_INTERROGATIONS = 63732,
    QUEST_CARVING_OUT_A_PATH = 63733,
    QEUST_THE_SUNDERED_STAFF = 63734,
    QUEST_AN_INFUSION_OF_ANIMA = 63736,
    QUEST_CONSULTING_THE_EXPERTS = 63740,
    QEUST_HOPE_ASCENDING = 63739,
    QUEST_FINDING_ONES_TRUE_PURPOSE = 63737,
    QUEST_ESTABLISHING_THE_ARCHIVE = 63738,
};

enum Spellkorthia
{
    SPELL_STUN_AURA = 163091,
    SPELL_TAL_GALANS_TRIAL1 = 353275,
    SPELL_TAL_GALANS_TRIAL2 = 353282,
    SPELL_CHAINELING = 353370,
    SPELL_ATTUNING = 351172,
    SPELL_RESTORING = 357775,
    SPELL_SUMMON_KAH_THAN = 351166,
    SPELL_SUMMON_HEIMIR = 351530,
    SPELL_GRAPPLING_HOOK_VISUAL = 352420,
    SPELL_GRAPPLING_HOOK = 352423,
    SPELL_CHARGE_FORWARD = 352581,
    SPELL_SUMMON_TAL_YOR = 352598,
    SPELL_SUMMON_TAL_RAHL = 352600,
    SPELL_TELEPORT_EFFECT = 355673,
    SPELL_BONDS_OF_BROTHERHOOD = 352810,
    SPELL_COLLECTED = 352799,
    SPELL_SCHOLAR_CASTING_FX = 352825,
    SPELL_DRAINING = 352909,
    SPELL_CAN_DRAIN_AURA = 352904,
    SPELL_SUMMON_SCHOLAR_ROH_SUIR = 353034,
    SPELL_RITUAL_PARTICIPATION = 353190,    //add ritual_aura
    SPELL_ATTUNE_THE_LODESTAFF = 353188,
    SPELL_ATTUNING_LODESTAFF = 353191,
    SPELL_ATTUNING_CANCLE = 353212,
    SPELL_EMPOWERING_NPC_SMALL = 353205,
    SPELL_EMPOWERING_NPC_BIG = 353207,
    SPELL_RITUAL_CIRCLE = 344211,    //circle_aura


    SCENE_UNDER_THE_ILLUSION = 356227,  //packageID 3289, sceneID 2781
    SCENE_OPEN_VAULT = 352739,  //packageID 3252, sceneID 2752
    SCENE_KEEPERS_RESPITE_MEETING = 352890,  //packageID 3260, sceneID 2756
};

enum Npckorthia
{
    NPC_ATTENDANT_PROTECTOR = 174917,
    NPC_FOREST_DEFENDER = 179421,
    NPC_BLADEGUARD_CAPTAIN = 179603,
    NPC_VENTHYR_NOBLE = 177700,
    NPC_VENTHYR_ARISTOCRAT = 177698,
    NPC_KYRIAN_STEWARD = 177699,
    NPC_KYRIAN_ASCENDANT = 177697,
    NPC_VERAYN = 177236,
    NPC_CARETAKER_KAH_THAN = 177610,
    NPC_CONCERNED_KAH_THAN = 177924,
    NPC_FATESCRIBE_ROH_KALO = 178235,
    NPC_AZODIUS = 177243,
    NPC_TAL_GALAN = 177423,
    NPC_HEIMIR = 178157,
    NPC_ANIMS_RIFT = 178540,
    NPC_GRAPPLE_POINT = 178546,
    NPC_TAL_GALAN_HOPE = 177451,
    NPC_XORUL_THE_INSATIABLE = 177537,
    NPC_DRAIN_KILLCREDIT = 177605,
};

Npckorthia covenants[8] =
{
    NPC_ATTENDANT_PROTECTOR,
    NPC_FOREST_DEFENDER,
    NPC_BLADEGUARD_CAPTAIN,
    NPC_VENTHYR_NOBLE,
    NPC_VENTHYR_ARISTOCRAT,
    NPC_KYRIAN_STEWARD,
    NPC_KYRIAN_ASCENDANT,
    NPC_VERAYN,
};

class player_korthia : public PlayerScript
{
public:
    player_korthia() : PlayerScript("player_korthia") { }
private:
    bool IntoVault;

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (!player->HasQuest(QUEST_INTO_THE_VAULT) || player->GetLevel() != 60)
            return;
        IntoVault = false;
        if (player->IsFalling() && player->GetPositionZ() <= 4862.91f && player->GetAreaId() == AREA_VAULT_OF_CECRETS && !IntoVault)
        {
            IntoVault = true;
            player->SetCanFly(true);
            player->CastSpell(player, SPELL_TELEPORT_EFFECT, false);
            player->GetScheduler().Schedule(2s, [player](TaskContext context)
                {
                    player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_AREA_TRIGGER_ENTER, 22940, 1);
                    player->NearTeleportTo(3182.66f, 6086.52f, 4798.27f, 0.9211f);
                });
        }
    }

    void OnLogout(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_ECHOES_OF_FATE) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* kahthan = GetClosestCreatureWithEntry(player, NPC_CARETAKER_KAH_THAN, 30.0f))
                kahthan->DespawnOrUnsummon(3s, 30s);
        }
    }


    void OnUpdateZone(Player* player, uint32 newzone, uint32 /*oldZone*/) override
    {
        if (player->GetZoneId() == AREA_KORTHIA)  //korthia
        {
            if (player->GetQuestStatus(QUEST_LET_THE_ANIMA_FLOW) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_TO_THE_VAULT) == QUEST_STATUS_NONE)
            {
                player->SummonCreature(NPC_ANIMS_RIFT, Position(3066.82f, 5968.88f, 4847.64f, 0.09444f), TEMPSUMMON_TIMED_DESPAWN, 15min);
                player->SummonCreature(NPC_ANIMS_RIFT, Position(2993.29f, 6604.89f, 4876.35f, 1.19268f), TEMPSUMMON_TIMED_DESPAWN, 15min);
                player->SummonCreature(NPC_ANIMS_RIFT, Position(2997.96f, 6896.93f, 4867.35f, 2.61874f), TEMPSUMMON_TIMED_DESPAWN, 15min);
            }
        }
    }

    void OnPlayerRepop(Player* player) override
    {
        OnCheckPhase(player);
    }

    void OnPlayerResurrect(Player* player) override
    {
        OnCheckPhase(player);
    }

    void OnQuestStatusChange(Player* player, uint32 questId) override
    {
        OnCheckPhase(player);
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/) override
    {
        OnCheckPhase(player);

        if (player->GetAreaId() == AREA_SEEKERS_QUORUM)
        {
            if (player->HasQuest(QUEST_RESCUED_FROM_TORMENT) || player->HasQuest(QUEST_THE_LAST_PLACE_YOU_LOOK) || player->HasQuest(QUEST_ROOTING_THE_LOOTERS))
                if (!player->FindNearestCreature(NPC_CONCERNED_KAH_THAN, 150.f, true))
                {
                    player->CastSpell(player, SPELL_SUMMON_KAH_THAN, false);
                }
        }
        if (player->GetAreaId() == AREA_VAULT_OF_CECRETS)
        {
            if (player->HasQuest(QUEST_DEFENDING_THE_VAULT) || player->HasQuest(QUEST_KEEPERS_OF_KORTHIA))
                player->CastSpell(player, SPELL_CHARGE_FORWARD, false);
        }
    }

    void OnCheckPhase(Player* player)
    {
        if (player->GetAreaId() == AREA_KORTHIA)
        {
            if (player->GetQuestStatus(QUEST_LET_THE_ANIMA_FLOW) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_LET_THE_ANIMA_FLOW) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(13615)) PhasingHandler::AddPhase(player, 13615, true);
            }
            if (!player->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(player, 169, true);
        }

        if (player->GetAreaId() == AREA_KEEPERS_RESPITE)
        {
            if (player->GetQuestStatus(QUEST_KORTHIA_THE_CITY_OF_SECRETS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(13601)) PhasingHandler::AddPhase(player, 13601, true);
            }
            if (player->GetQuestStatus(QUEST_CHARGE_OF_THE_COVENANTS) == QUEST_STATUS_COMPLETE)
            {
                if (!player->FindNearestCreature(177927, 15.0f, true))
                {
                    player->SummonCreature(177927, 3251.87f, 5814.35f, 4939.49f, 4.84907f, TEMPSUMMON_MANUAL_DESPAWN);
                }
                if (player->GetPhaseShift().HasPhase(13601)) PhasingHandler::RemovePhase(player, 13601, true);
            }
            if (player->GetQuestStatus(QUEST_CHARGE_OF_THE_COVENANTS) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(13601)) PhasingHandler::RemovePhase(player, 13601, true);
                if (!player->GetPhaseShift().HasPhase(13602)) PhasingHandler::AddPhase(player, 13602, true);
                if (!player->GetPhaseShift().HasPhase(17292)) PhasingHandler::AddPhase(player, 17292, true);
                if (!player->GetPhaseShift().HasPhase(17293)) PhasingHandler::AddPhase(player, 17293, true);
            }
            if (player->GetQuestStatus(QUEST_INTO_THE_VAULT) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_INTO_THE_VAULT) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(13601)) PhasingHandler::RemovePhase(player, 13601, true);
                if (!player->GetPhaseShift().HasPhase(13602)) PhasingHandler::AddPhase(player, 13602, true);
                if (!player->GetPhaseShift().HasPhase(13604)) PhasingHandler::AddPhase(player, 13604, true);
                if (!player->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(player, 169, true);
            }
            if (player->GetQuestStatus(QUEST_THE_PRIMUS_RETURNS) == QUEST_STATUS_INCOMPLETE
                || player->GetQuestStatus(QUEST_THE_PRIMUS_RETURNS) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_THE_PRIMUS_RETURNS) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(13601)) PhasingHandler::RemovePhase(player, 13601, true);
                if (player->GetPhaseShift().HasPhase(13602)) PhasingHandler::RemovePhase(player, 13602, true);
                if (player->GetPhaseShift().HasPhase(13604)) PhasingHandler::RemovePhase(player, 13604, true);
                if (!player->GetPhaseShift().HasPhase(13605)) PhasingHandler::AddPhase(player, 13605, true);
                if (!player->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(player, 169, true);
            }
        }
    }
};

class scene_korthia : public SceneScript
{
public:
    scene_korthia() : SceneScript("scene_korthia") { }

    void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3260)
        {
            if (player->GetPhaseShift().HasPhase(13601)) PhasingHandler::RemovePhase(player, 13601, true);
            if (player->GetPhaseShift().HasPhase(13602)) PhasingHandler::RemovePhase(player, 13602, true);
            if (player->GetPhaseShift().HasPhase(13604)) PhasingHandler::RemovePhase(player, 13604, true);
            if (player->GetPhaseShift().HasPhase(13605)) PhasingHandler::RemovePhase(player, 13605, true);
            if (!player->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(player, 169, true);
        }
    }

    // Called when a scene is canceled
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3252)
            player->KilledMonsterCredit(178676);
        if (sceneTemplate->ScenePackageId == 3289)
            Finish(player);
        if (sceneTemplate->ScenePackageId == 3260)
            primusReturnFinish(player);
    }

    // Called when a scene is completed
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 3252)
            player->KilledMonsterCredit(178676);
        if (sceneTemplate->ScenePackageId == 3289)
            Finish(player);
        if (sceneTemplate->ScenePackageId == 3260)
            primusReturnFinish(player);
    }

    void Finish(Player* player)
    {
        player->SummonCreature(NPC_AZODIUS, Position(3410.53f, 6042.33f, 4910.55f, 4.7945f), TEMPSUMMON_TIMED_DESPAWN, 5min);
    }
    void primusReturnFinish(Player* player)
    {
        player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_AREA_TRIGGER_ENTER, 22947, 1);
        if (!player->GetPhaseShift().HasPhase(13605)) PhasingHandler::AddPhase(player, 13605, true);
    }
};

Position const Venari_path[4] =
{
    {3280.17f, 5767.36f, 4938.27f, 0},
    {3287.78f, 5754.54f, 4939.65f, 0},
    {3285.42f, 5741.01f, 4939.51f, 0},
    {3279.19f, 5727.87f, 4939.12f, 0},
};

//177227 tal galan
struct npc_tal_galan_177227 : public  ScriptedAI
{
public:
    npc_tal_galan_177227(Creature* creature) : ScriptedAI(creature) {}

private:
    EventMap _events;
    bool say63663;
    ObjectGuid kp_leftGUID1;
    ObjectGuid kp_leftGUID2;
    ObjectGuid kp_rightGUID1;
    ObjectGuid kp_rightGUID2;
    ObjectGuid venariGUID;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        _events.Reset();
        say63663 = false;
        m_playerGUID = ObjectGuid::Empty;
        kp_leftGUID1 = ObjectGuid::Empty;
        kp_leftGUID2 = ObjectGuid::Empty;
        kp_rightGUID1 = ObjectGuid::Empty;
        kp_rightGUID2 = ObjectGuid::Empty;
        venariGUID = ObjectGuid::Empty;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_OPENING_TO_ORIBOS)
        {
            if (Creature* venari = me->FindNearestCreature(177202, 10.0f))
            {
                venari->AI()->Talk(7);
                venari->GetMotionMaster()->MoveSmoothPath(1004, Venari_path, 4, true, false);
                venari->ForcedDespawn(15000);
            }
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_KORTHIA_THE_CITY_OF_SECRETS)
        {
            if (Creature* venari = ObjectAccessor::GetCreature(*me, venariGUID))
                venari->ForcedDespawn(1000);
            me->ForcedDespawn(1000);
        }
    }

    bool OnGossipHello(Player* player) override    //npc_txt appear to top
    {
        if ((player->GetQuestStatus(QUEST_KORTHIA_THE_CITY_OF_SECRETS) == QUEST_STATUS_INCOMPLETE) && player->GetQuestObjectiveProgress(QUEST_KORTHIA_THE_CITY_OF_SECRETS, 0))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "Lead the way, Tal-Galan.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            SendGossipMenuFor(player, 43335, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }
        if (player->GetQuestStatus(QUEST_KORTHIA_THE_CITY_OF_SECRETS) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_KORTHIA_THE_CITY_OF_SECRETS) == QUEST_STATUS_REWARDED)
        {
            player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        }
        if ((player->GetQuestStatus(QUEST_WHO_IS_THE_MAW_WALKER) == QUEST_STATUS_INCOMPLETE))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm prepared for your Maw Walker test.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, 42958, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }
        if (player->GetQuestStatus(QUEST_WHO_IS_THE_MAW_WALKER) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_WHO_IS_THE_MAW_WALKER) == QUEST_STATUS_REWARDED)
        {
            player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF + 0)
        {
            CloseGossipMenuFor(player);
            player->KilledMonsterCredit(180469);
            _events.ScheduleEvent(EVENT_THE_CITY_OF_SECRETS_III, 1s);
        }
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            CloseGossipMenuFor(player);
            m_playerGUID = player->GetGUID();
            Talk(9);
            me->CastSpell(player, SPELL_CHAINELING, true);
            player->AddAura(SPELL_STUN_AURA, player);
            player->AddAura(SPELL_TAL_GALANS_TRIAL1, player);
            _events.ScheduleEvent(EVENT_WHO_IS_THE_MAW_WALKER, 8s);
        }
        return true;
    }


    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();

        if (Player* player = summoner->ToPlayer())
            if (player->HasQuest(QUEST_KORTHIA_THE_CITY_OF_SECRETS))
                summonKorthianProtector();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (player->IsInDist(me, 30.0f) && player->HasQuest(QUEST_KORTHIA_THE_CITY_OF_SECRETS))
                    if (!say63663)
                    {
                        say63663 = true;
                        _events.ScheduleEvent(EVENT_THE_CITY_OF_SECRETS_I, 1s);
                    }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                switch (eventId)
                {
                case EVENT_THE_CITY_OF_SECRETS_I:
                {
                    if (Creature* venari = me->SummonCreature(177202, 3439.06f, 5940.07f, 4883.01f, 1.74492f, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        venariGUID = venari->GetGUID();
                        venari->AI()->DoAction(2);
                    }
                    Talk(0);
                    me->AddDelayedEvent(8000, [this, player]() -> void
                        {
                            Talk(1);
                            std::list<Creature*> cList = me->FindNearestCreatures(177450, 10.0f);
                            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                            {
                                if (Creature* kp = *itr)
                                {
                                    kp->SetFacingToObject(player);
                                    kp->HandleEmoteCommand(EMOTE_STATE_READY2H_ALLOW_MOVEMENT);
                                }
                            }
                        });
                }
                break;
                case EVENT_THE_CITY_OF_SECRETS_II:
                {
                    Talk(2);
                    me->AddDelayedEvent(8000, [this, player]() -> void
                        {
                            Talk(3);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                        });
                    me->AddDelayedEvent(11000, [this, player]() -> void
                        {
                            player->KilledMonsterCredit(177209);
                            if (Creature* venari = ObjectAccessor::GetCreature(*me, venariGUID))
                                venari->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                            std::list<Creature*> cList = me->FindNearestCreatures(177450, 10.0f);
                            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                            {
                                if (Creature* kp = *itr)
                                {
                                    kp->HandleEmoteCommand(EMOTE_STATE_STAND);
                                    kp->GetMotionMaster()->MoveTargetedHome();
                                }
                            }
                        });
                }
                break;
                case EVENT_THE_CITY_OF_SECRETS_III:
                    Talk(4);
                    me->AddDelayedEvent(4000, [this]() -> void
                        {
                            if (Creature* venari = ObjectAccessor::GetCreature(*me, venariGUID))
                            {
                                venari->AI()->Talk(4);
                                venari->GetMotionMaster()->MovePath(17720201, false);
                            }
                            me->GetMotionMaster()->MovePath(17722701, false);
                        });
                    break;
                case EVENT_WHO_IS_THE_MAW_WALKER:
                    Talk(10);
                    me->AddDelayedEvent(7000, [this, player]() -> void
                        {
                            player->AddAura(SPELL_TAL_GALANS_TRIAL2, player);
                        });
                    me->AddDelayedEvent(13000, [this]() -> void
                        {
                            if (Creature* venari = me->FindNearestCreature(177202, 10.0f))
                                venari->AI()->Talk(5);
                        });
                    me->AddDelayedEvent(20000, [this]() -> void
                        {
                            Talk(11);
                        });
                    me->AddDelayedEvent(28000, [this]() -> void
                        {
                            if (Creature* venari = me->FindNearestCreature(177202, 10.0f))
                                venari->AI()->Talk(6);
                        });
                    me->AddDelayedEvent(34000, [this, player]() -> void
                        {
                            Talk(12);
                            player->ForceCompleteQuest(QUEST_WHO_IS_THE_MAW_WALKER);
                            me->RemoveAura(SPELL_CHAINELING);
                            player->RemoveAura(SPELL_STUN_AURA);
                            player->RemoveAura(SPELL_TAL_GALANS_TRIAL1);
                            player->RemoveAura(SPELL_TAL_GALANS_TRIAL2);
                        });
                    break;
                }
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            switch (id)
            {
            case 1:
                if (Creature* kpleft1 = ObjectAccessor::GetCreature(*me, kp_leftGUID1))
                    if (Creature* kpleft2 = ObjectAccessor::GetCreature(*me, kp_leftGUID2))
                        if (Creature* kpright1 = ObjectAccessor::GetCreature(*me, kp_rightGUID1))
                            if (Creature* kpright2 = ObjectAccessor::GetCreature(*me, kp_rightGUID2))
                            {
                                kpright1->GetMotionMaster()->MovePath(1774501, false);
                                kpleft1->GetMotionMaster()->MovePath(1774502, false);
                                me->AddDelayedEvent(1500, [this, kpleft2, kpright2]() -> void
                                    {
                                        kpright2->GetMotionMaster()->MovePath(1774501, false);
                                        kpleft2->GetMotionMaster()->MovePath(1774502, false);
                                    });
                            }
                break;
            case 3:
                Talk(5);
                break;
            case 5:
                Talk(6);
                break;
            case 8:
                Talk(7);
                break;
            case 10:
                Talk(8);
                break;
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == 3) _events.ScheduleEvent(EVENT_THE_CITY_OF_SECRETS_II, 7s);
    }

    void summonKorthianProtector()
    {
        if (Creature* kpl1 = me->SummonCreature(177450, 3325.19f, 5946.71f, 4882.15f, 2.6952f, TEMPSUMMON_MANUAL_DESPAWN)) //  Korthian Protector_left
            kp_leftGUID1 = kpl1->GetGUID();
        if (Creature* kpl2 = me->SummonCreature(177450, 3326.35f, 5948.63f, 4882.16f, 2.6952f, TEMPSUMMON_MANUAL_DESPAWN))
            kp_leftGUID2 = kpl2->GetGUID();
        if (Creature* kpr1 = me->SummonCreature(177450, 3328.96f, 5953.21f, 4881.89f, 2.6952f, TEMPSUMMON_MANUAL_DESPAWN)) //  Korthian Protector_Right
            kp_rightGUID1 = kpr1->GetGUID();
        if (Creature* kpr2 = me->SummonCreature(177450, 3327.92f, 5951.11f, 4881.99f, 2.6952f, TEMPSUMMON_MANUAL_DESPAWN))
            kp_rightGUID2 = kpr2->GetGUID();
    }
};

//Waystone to Oribos 368216
struct go_waystone_to_oribos_368216 : public GameObjectAI
{
    go_waystone_to_oribos_368216(GameObject* go) : GameObjectAI(go)
    {
        me->SetGoState(GO_STATE_READY);
    }

    bool OnReportUse(Player* player) override
    {
        if ((player->GetQuestStatus(QUEST_OPENING_TO_ORIBOS) == QUEST_STATUS_INCOMPLETE) && !player->GetQuestObjectiveProgress(QUEST_OPENING_TO_ORIBOS, 1))
        {
            player->CastSpell(player, SPELL_ATTUNING, false);
            player->GetScheduler().Schedule(Milliseconds(7000), [this, player](TaskContext context)
                {
                    me->SetGoState(GO_STATE_ACTIVE);
                });
        }
        if ((player->GetQuestStatus(QUEST_OPENING_TO_ORIBOS) == QUEST_STATUS_INCOMPLETE) && player->GetQuestObjectiveProgress(QUEST_OPENING_TO_ORIBOS, 1) && !player->GetQuestObjectiveProgress(QUEST_OPENING_TO_ORIBOS, 3))
        {
            player->ForceCompleteQuest(QUEST_OPENING_TO_ORIBOS);
            player->TeleportTo(2222, -1758.319f, 1359.19f, 5451.51f, 3.9254f);
        }
        if (player->GetQuestStatus(QUEST_INTO_THE_VAULT) == QUEST_STATUS_REWARDED)
        {
            player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                {
                    player->TeleportTo(2222, -1758.319f, 1359.19f, 5451.51f, 3.9254f);
                });
        }
        return true;
    }


};

Position summonCovenantPosition1 = { 3271.81f, 5763.31f, 4941.53f, 0.8645f };
Position summonCovenantPosition2 = { 3261.45f, 5758.25f, 4941.55f, 2.6355f };

//177927 tal galan
struct npc_tal_galan_177927 : public  ScriptedAI
{
public:
    npc_tal_galan_177927(Creature* creature) : ScriptedAI(creature) {}

private:
    EventMap _events;
    ObjectGuid bolvarGUID;

    void Reset() override
    {
        _events.Reset();
        bolvarGUID = ObjectGuid::Empty;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_VAULT_OF_SECRETS))
        {
            Talk(3);
            me->AddDelayedEvent(8000, [this]() -> void
                {
                    if (Creature* bolvar = me->FindNearestCreature(177228, 15.0f, true))
                        bolvar->AI()->Talk(4);
                });
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            if (player->HasQuest(QUEST_CHARGE_OF_THE_COVENANTS))
            {
                _events.ScheduleEvent(EVENT_SUMMONCOVENANT_RANDOM, 1s);
                _events.ScheduleEvent(EVENT_CHARGE_OF_THE_COVENANTS, 3s);
                if (Creature* bolvar = me->SummonCreature(177228, 3265.22f, 5770.33f, 4938.93f, 1.8656f, TEMPSUMMON_MANUAL_DESPAWN))
                    bolvarGUID = bolvar->GetGUID();
            }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_CHARGE_OF_THE_COVENANTS:
            {
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, 3267.47f, 5777.91f, 4938.09f, true);
                me->AddDelayedEvent(5000, [this]() -> void
                    {
                        Talk(0);
                    });
                me->AddDelayedEvent(15000, [this]() -> void
                    {
                        if (Creature* bolvar = ObjectAccessor::GetCreature(*me, bolvarGUID))
                        {
                            bolvar->AI()->Talk(0);
                            bolvar->SetWalk(true);
                            bolvar->GetMotionMaster()->MovePoint(0, 3247.33f, 5812.81f, 4939.43f, true, 5.3586f);
                            me->GetMotionMaster()->MovePoint(0, 3251.87f, 5814.35f, 4939.49f, true, 4.84907f);
                        }
                    });
                me->AddDelayedEvent(25000, [this]() -> void
                    {
                        Talk(1);
                        _events.CancelEvent(EVENT_SUMMONCOVENANT_RANDOM);
                    });
            }
            break;
            case EVENT_SUMMONCOVENANT_RANDOM:
                SummonCovenant(covenants[urand(0, 7)]);
                _events.Repeat(randtime(1s, 1s));
                break;
            }
        }
    }

    void SummonCovenant(uint32 creature)
    {
        switch (uint8 rand = urand(0, 1))
        {
        case 0:
            if (Creature* covenant1 = me->SummonCreature(creature, summonCovenantPosition1, TEMPSUMMON_TIMED_DESPAWN, 15s))
            {
                if (covenant1->GetEntry() == NPC_ATTENDANT_PROTECTOR)
                    covenant1->SetAIAnimKitId(0);
                covenant1->GetMotionMaster()->MovePath(17792701, false);
            }
            break;
        case 1:
            if (Creature* covenant2 = me->SummonCreature(creature, summonCovenantPosition1, TEMPSUMMON_TIMED_DESPAWN, 15s))
            {
                if (covenant2->GetEntry() == NPC_ATTENDANT_PROTECTOR)
                    covenant2->SetAIAnimKitId(0);
                covenant2->GetMotionMaster()->MovePath(17792702, false);
            }
            break;
        }
    }
};

//177228 highlord_bolvar_fordragon
struct npc_highlord_bolvar_fordragon_177228 : public  ScriptedAI
{
public:
    npc_highlord_bolvar_fordragon_177228(Creature* creature) : ScriptedAI(creature) {}
private:
    ObjectGuid m_playerGUID;
    bool quest63727;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
        quest63727 = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (Player* player = target->ToPlayer())
        {
            if (target->GetDistance2d(me) <= 40.0f)
            {
                if (player->HasQuest(QUEST_THE_PRIMUS_RETURNS) && player->GetPhaseShift().HasPhase(13605) && !quest63727)
                {
                    quest63727 = true;
                    player->CastSpell(player, SCENE_KEEPERS_RESPITE_MEETING);
                }
            }
            if (target->GetDistance2d(me) > 40.0f && quest63727)
                if (player->GetQuestStatus(QUEST_THE_PRIMUS_RETURNS) == QUEST_STATUS_REWARDED)
                    quest63727 = false;
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_CHARGE_OF_THE_COVENANTS)
        {
            if (Creature* talgalan = me->FindNearestCreature(177927, 15.0f, true))
                if (!talgalan->GetPhaseShift().HasPhase(13602))
                    talgalan->ForcedDespawn(500);
            me->ForcedDespawn(500);
        }

        if (quest->GetQuestId() == QUEST_THEY_COULD_BE_ANYONE)
        {
            Talk(1);
            me->AddDelayedEvent(12000, [this]() -> void
                {
                    Talk(2);
                });
            me->AddDelayedEvent(22000, [this]() -> void
                {
                    if (Creature* galan = me->FindNearestCreature(177927, 15.0f))
                        galan->AI()->Talk(1);
                });
            me->AddDelayedEvent(30000, [this]() -> void
                {
                    Talk(3);
                });
        }
    }

    bool OnGossipHello(Player* player) override
    {
        m_playerGUID = player->GetGUID();
        if (me->IsQuestGiver())
        {
            player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());  //Gossip_menu_id must have number.
        }
        if (player->HasQuest(QUEST_UNTANGLING_THE_SIGIL))
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "What should we do with the sigil?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            SendGossipMenuFor(player, 42929, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*MenuID*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 0:
            CloseGossipMenuFor(player);
            Talk(5);
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(1001, Position{ 3237.78f, 5810.21f, 4939.06f }, true, 0.2976f);
            break;
        }
        return true;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            me->AddDelayedEvent(3000, [this]() ->void { Talk(6); });
            if (Creature* galan = me->FindNearestCreature(177927, 15.f, true))
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    galan->SetFacingToObject(me);
                    me->AddDelayedEvent(15000, [this, galan]() ->void { galan->AI()->Talk(4); });
                    me->AddDelayedEvent(23000, [this]() ->void { Talk(7); });
                    if (Creature* bh = player->FindNearestCreature(177568, 15.f, true))
                    {
                        me->AddDelayedEvent(30000, [bh, player]() ->void
                            {
                                bh->SetFacingToObject(player);
                                bh->AI()->Talk(0);
                            });
                        me->AddDelayedEvent(37000, [this, galan, bh, player]() ->void
                            {
                                player->KilledMonsterCredit(178761);
                                Talk(8);
                                me->GetMotionMaster()->MovePoint(0, Position{ 3247.33f, 5812.81f, 4939.43f }, true, 5.3586f);
                                galan->GetMotionMaster()->MoveTargetedHome();
                                bh->SetWalk(true);
                                bh->GetMotionMaster()->MovePoint(0, Position{ 3257.12f, 5790.35f, 4939.12f }, true, 5.3124f);
                                bh->DespawnOrUnsummon(6s, 2min);
                            });
                    }
                }
        }
    }
};

//180525 Surveyer Mnemis
struct npc_surveyer_mnemis_180525 : public  ScriptedAI
{
public:
    npc_surveyer_mnemis_180525(Creature* creature) : ScriptedAI(creature) {}

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            CloseGossipMenuFor(player);
            Talk(0);
            me->HandleEmoteCommand(EMOTE_STATE_WORK_SMITH);
            me->AddDelayedEvent(5000, [this, player]() -> void
                {
                    if (Creature* gorger = me->SummonCreature(177245, 2955.73f, 6175.28f, 4809.96f, 4.45462f, TEMPSUMMON_TIMED_DESPAWN, 600s))
                    {
                        gorger->SetWalk(false);
                        gorger->GetMotionMaster()->MovePoint(0, 2948.23f, 6153.43f, 4811.34f, true);
                    }
                });
            me->AddDelayedEvent(35000, [this, player]() -> void
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                    if (!player->HasItemCount(187438, 1, false))
                        player->CastSpell(player, 357681, false);
                });
        }
        return true;
    }
};

//187504 Mikanikos' Restorative Contraption
class item_restorative_contraption : public ItemScript
{
public:
    item_restorative_contraption() : ItemScript("item_restorative_contraption") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        if (Unit* target = player->GetSelectedUnit())
            if (Creature* creature = target->ToCreature())
                if (target->GetStandState() == UNIT_STAND_STATE_DEAD)
                {
                    switch (target->GetEntry())
                    {
                    case 180554:
                        setItemEffect(creature, player);
                        break;
                    case 180555:
                        setItemEffect(creature, player);
                        break;
                    case 180556:
                        setItemEffect(creature, player);
                        break;
                    case 180557:
                        setItemEffect(creature, player);
                        break;
                    }
                }
        return true;
    }

    void setItemEffect(Creature* creature, Player* player)
    {
        player->CastSpell(creature, SPELL_RESTORING);
        player->AddDelayedEvent(3500, [player, creature]() -> void
            {
                creature->RemoveAura(314629);
                creature->SetStandState(UNIT_STAND_STATE_STAND);
                creature->SetWalk(false);
                creature->GetMotionMaster()->MoveRandom(20.0f);
                player->KilledMonsterCredit(180555);
                creature->ForcedDespawn(5000);
            });
    }
};

static const Position kahthanMovePathRFT[4] =
{
    { 2947.66f, 6019.62f, 4807.34f, 0 },
    { 2939.54f, 6024.54f, 4807.27f, 0 },
    { 2922.25f, 6032.89f, 4803.69f, 0 },
    { 2893.53f, 6014.91f, 4797.97f, 0 },
};

static const Position kahthanMovePathEOF[4] =
{
    { 3451.73f, 6101.34f, 4917.51f, 0 },
    { 3429.54f, 6085.55f, 4913.72f, 0 },
    { 3422.79f, 6076.05f, 4912.91f, 0 },
    { 3414.37f, 6054.01f, 4914.07f, 0 },
};

//177610 Caretaker Kah-Than
struct npc_caretaker_kah_than_177610 : public  ScriptedAI
{
public:
    npc_caretaker_kah_than_177610(Creature* creature) : ScriptedAI(creature) {}

private:
    bool say63755;
    bool say63759;
    bool say63760;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        say63755 = false;
        say63759 = false;
        say63760 = false;
        m_playerGUID = ObjectGuid::Empty;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Player* player = target->ToPlayer())
        {
            if (!say63755)
            {
                if (player->GetDistance2d(me) <= 10.0f)
                    if (player->HasQuest(QUEST_A_MATTER_OF_URGENCY))
                    {
                        say63755 = true;
                        Talk(0);
                    }
            }
            if (!say63759)
            {
                if (player->GetDistance2d(me) <= 10.0f)
                    if (player->HasQuest(QUEST_REDIRECT_THE_SEARCH))
                    {
                        say63759 = true;
                        Talk(2, player);
                    }
            }

            if (!say63760)
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    if (player->GetDistance2d(me) <= 20.0f)
                    {
                        if (player->GetQuestStatus(QUEST_ECHOES_OF_FATE) == QUEST_STATUS_NONE
                            || player->GetQuestStatus(QUEST_RESCUED_FROM_TORMENT) == QUEST_STATUS_NONE
                            || player->GetQuestStatus(QUEST_THE_LAST_PLACE_YOU_LOOK) == QUEST_STATUS_NONE)
                        {
                            me->GetMotionMaster()->MoveTargetedHome();
                            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                            m_playerGUID = ObjectGuid::Empty;
                        }
                    }
                }
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_ECHOES_OF_FATE_COMPLETE)
        {
            me->AddDelayedEvent(5000, [this]()
                {
                    say63760 = true;
                    Talk(7);
                    me->SummonCreature(NPC_CARETAKER_KAH_THAN, 3413.26f, 6008.61f, 4906.28f, 1.5731f, TEMPSUMMON_MANUAL_DESPAWN);
                    me->GetMotionMaster()->MoveSmoothPath(5, kahthanMovePathEOF, 4, true, false);
                    me->ForcedDespawn(15000);
                });
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_RESCUED_FROM_TORMENT) && player->HasQuest(QUEST_THE_LAST_PLACE_YOU_LOOK))
        {
            Talk(1, player);
            me->GetMotionMaster()->MoveSmoothPath(4, kahthanMovePathRFT, 4, true, false);
        }

        if (player->HasQuest(QUEST_ECHOES_OF_FATE))
        {
            Talk(3);
            m_playerGUID = player->GetGUID();
            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle(), {}, MOTION_SLOT_ACTIVE);
        }

        if (player->HasQuest(QUEST_UNDER_THE_ILLUSION))
            me->SummonGameObject(369115, 3413.47f, 6012.08f, 4906.31f, 5.83177f, QuaternionData(0, 0, 0, 0), 1min); // summonGameObject Transept
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_A_MATTER_OF_URGENCY) say63755 = false;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 4) me->DespawnOrUnsummon(1s, 60s);
    }
};


static const Position concernedkahthanMovePath[3] =
{
    { 2519.48f, 6463.53f, 4825.65f, 0 },
    { 2557.21f, 6426.65f, 4852.59f, 0 },
    { 2573.42f, 6404.36f, 4852.28f, 0 },
};

//177924 Caretaker Kah-Than
struct npc_concerned_kah_than_177924 : public  ScriptedAI
{
public:
    npc_concerned_kah_than_177924(Creature* creature) : ScriptedAI(creature) {}
private:
    bool item185827;
    bool item185830;
    bool item185831;
    bool item185771;
    bool say63758;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        item185827 = false;
        item185830 = false;
        item185831 = false;
        item185771 = false;
        say63758 = false;
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();

        if (Player* player = summoner->ToPlayer())
            if (player->GetAreaId() == AREA_SEEKERS_QUORUM)
                if (player->HasQuest(QUEST_RESCUED_FROM_TORMENT) || player->HasQuest(QUEST_THE_LAST_PLACE_YOU_LOOK))
                {
                    me->AddDelayedEvent(urand(5000, 10000), [this]() -> void
                        {
                            Talk(0);
                        });
                    me->AddDelayedEvent(urand(40000, 60000), [this]() -> void
                        {
                            Talk(1);
                        });
                }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_REDIRECT_THE_SEARCH))
        {
            Talk(12, player);
            me->GetMotionMaster()->MoveSmoothPath(3, concernedkahthanMovePath, 3, true, false);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 3) me->ForcedDespawn(500);
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Player* player = target->ToPlayer())
        {
            if (player->GetDistance2d(me) <= 20.0f)
                if (player->HasQuest(QUEST_ROOTING_THE_LOOTERS))
                {
                    // 	Korthian Scrollcase
                    if (player->HasItemCount(185827, 1))
                        if (!item185827)
                        {
                            item185827 = true;
                            player->KilledMonsterCredit(177844);
                            Talk(8);
                        }
                    if (!player->HasItemCount(185827, 1)) item185827 = false;
                    // Weight of Judgment
                    if (player->HasItemCount(185830, 1))
                        if (!item185830)
                        {
                            item185830 = true;
                            player->KilledMonsterCredit(177844);
                            Talk(9);
                        }
                    if (!player->HasItemCount(185830, 1)) item185830 = false;
                    // 	Axe of Storms
                    if (player->HasItemCount(185831, 1))
                        if (!item185831)
                        {
                            item185831 = true;
                            player->KilledMonsterCredit(177844);
                            Talk(10);
                        }
                    if (!player->HasItemCount(185831, 1)) item185831 = false;
                    //Korthian Artifacts found
                    if (player->HasItemCount(185771, 1))
                        if (!item185771)
                        {
                            item185771 = true;
                            player->KilledMonsterCredit(177844);
                            Talk(11);
                        }
                    if (!player->HasItemCount(185771, 1)) item185771 = false;
                }
        }

        if (Creature* creature = target->ToCreature())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (creature->GetDistance2d(me) <= 10.0f)
                    if (creature->GetEntry() == 177846)
                        if (player->HasQuest(QUEST_THE_LAST_PLACE_YOU_LOOK))
                            if (!say63758)
                            {
                                player->KilledMonsterCredit(177846);
                                say63758 = true;
                                Talk(2);
                            }
        }
    }
};

//368385 Unbroken Anima Container
struct go_unbroken_anima_container_368385 : public GameObjectAI
{
    go_unbroken_anima_container_368385(GameObject* go) : GameObjectAI(go) {}

    bool OnGossipHello(Player* player) override
    {
        if (!player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 0))
        {
            player->SummonCreature(NPC_FATESCRIBE_ROH_KALO, 3346.48f, 6017.75f, 4894.53f, 4.9012f, TEMPSUMMON_TIMED_DESPAWN, 30s);
            player->KilledMonsterCredit(177843);
            me->DestroyForPlayer(player);
        }
        return true;
    }
};

//368386 Unburned Incense
struct go_unburned_incense_368386 : public GameObjectAI
{
    go_unburned_incense_368386(GameObject* go) : GameObjectAI(go) {}

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 0) && !player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 1))
        {
            player->SummonCreature(NPC_FATESCRIBE_ROH_KALO, 3325.61f, 6104.23f, 4898.67f, 4.747f, TEMPSUMMON_TIMED_DESPAWN, 30s);
            player->KilledMonsterCredit(177842);
            me->DestroyForPlayer(player);
        }
        return true;
    }
};

//368387 Small Offering
struct go_small_offering_368387 : public GameObjectAI
{
    go_small_offering_368387(GameObject* go) : GameObjectAI(go) {}

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 1) && !player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 2))
        {
            player->SummonCreature(NPC_FATESCRIBE_ROH_KALO, 3465.94f, 6107.83f, 4923.01f, 2.898f, TEMPSUMMON_TIMED_DESPAWN, 30s);
            player->KilledMonsterCredit(177937);
            me->DestroyForPlayer(player);
        }
        return true;
    }
};

//178235 Fatescribe Roh-Kalo
struct npc_fatescribe_roh_kalo_178235 : public  ScriptedAI
{
public:
    npc_fatescribe_roh_kalo_178235(Creature* creature) : ScriptedAI(creature) {}

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (!player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 0))
            {
                player->GetScheduler().Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        Talk(0);
                    });
                player->GetScheduler().Schedule(Milliseconds(10000), [this](TaskContext context)
                    {
                        Talk(1);
                    });
                player->GetScheduler().Schedule(Milliseconds(17000), [this](TaskContext context)
                    {
                        if (Creature* kahthan = me->FindNearestCreature(NPC_CARETAKER_KAH_THAN, 20.0f, true))
                            kahthan->AI()->Talk(4);
                    });
            }
            if (player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 0) && !player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 1))
            {
                player->GetScheduler().Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        Talk(2);
                    });
                player->GetScheduler().Schedule(Milliseconds(10000), [this](TaskContext context)
                    {
                        Talk(3);
                    });
                player->GetScheduler().Schedule(Milliseconds(20500), [this](TaskContext context)
                    {
                        if (Creature* kahthan = me->FindNearestCreature(NPC_CARETAKER_KAH_THAN, 20.0f, true))
                            kahthan->AI()->Talk(5);
                    });
            }
            if (player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 1) && !player->GetQuestObjectiveProgress(QUEST_ECHOES_OF_FATE, 2))
            {
                player->GetScheduler().Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        Talk(4);
                    });
                player->GetScheduler().Schedule(Milliseconds(10000), [this](TaskContext context)
                    {
                        Talk(5);
                    });
                player->GetScheduler().Schedule(Milliseconds(16000), [this](TaskContext context)
                    {
                        if (Creature* kahthan = me->FindNearestCreature(NPC_CARETAKER_KAH_THAN, 20.0f, true))
                        {
                            kahthan->AI()->Talk(6);
                            kahthan->AI()->DoAction(ACTION_ECHOES_OF_FATE_COMPLETE);
                        }
                    });
            }
        }
    }
};

// 351205 Dispel Illusion
class spell_dispel_illusion_351205 : public SpellScriptLoader
{
public:
    spell_dispel_illusion_351205() : SpellScriptLoader("spell_dispel_illusion_351205") { }

    class spell_dispel_illusion_351205_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dispel_illusion_351205_SpellScript);

        void HandleDummy(SpellEffIndex /*effindex*/)
        {
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                {
                    player->CastSpell(player, SCENE_UNDER_THE_ILLUSION, false);
                    if (Creature* kahthan = player->FindNearestCreature(NPC_CARETAKER_KAH_THAN, 20.0f))
                        kahthan->ForcedDespawn(5000);
                    if (GameObject* transept = player->FindNearestGameObject(369115, 20.0f))
                        transept->DestroyForPlayer(player);
                }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dispel_illusion_351205_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dispel_illusion_351205_SpellScript();
    }
};

//177449 Bonesmith Heirmir
struct npc_bonesmith_heirmir_177449 : public  ScriptedAI
{
public:
    npc_bonesmith_heirmir_177449(Creature* creature) : ScriptedAI(creature) {}
private:
    bool say63703;

    void Reset() override
    {
        say63703 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 10.0f) && player->HasQuest(QUEST_VAULT_OF_SECRETS))
                if (!say63703)
                {
                    say63703 = true;
                    Talk(0);
                    player->ForceCompleteQuest(QUEST_VAULT_OF_SECRETS);
                    player->SummonCreature(NPC_TAL_GALAN, 3195.2f, 5960.35f, 4868.75f, 2.89048f, TEMPSUMMON_MANUAL_DESPAWN);
                }
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_LET_THE_ANIMA_FLOW)
        {
            player->CastSpell(player, SPELL_SUMMON_HEIMIR, false);
            me->DespawnOrUnsummon(1s, 60s);
        }
    }
};

Position const tal_galan_path[3] =
{
    {3174.8f, 5964.35f, 4864.98f, 0},
    {3157.05f, 5978.55f, 4859.72f, 0},
    {3143.45f, 5983.54f, 4857.44f, 2.764f},
};

Position const RecordKeeper_path[5] =
{
    {3171.13f, 5977.71f, 4863.39f},
    {3169.91f, 5975.38f, 4863.07f},
    {3168.84f, 5972.31f, 4862.81f},
    {3166.86f, 5969.62f, 4862.49f},
    {3179.38f, 5974.04f, 4865.52f},
};

//177423 tal galan
struct npc_tal_galan_177423 : public  ScriptedAI
{
public:
    npc_tal_galan_177423(Creature* creature) : ScriptedAI(creature) {}

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 50.0f) && player->GetQuestStatus(QUEST_VAULT_OF_SECRETS) == QUEST_STATUS_REWARDED)
                me->RemoveAura(65050);
        }
    }


    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_VAULT_OF_SECRETS))
            {
                me->GetMotionMaster()->MoveSmoothPath(3, tal_galan_path, 3, true, false);
                me->AddDelayedEvent(15000, [this]() -> void { Talk(1); });
            }
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_ANIMA_TRAIL)
        {
            Talk(3);
            if (Creature* rk = player->FindNearestCreature(178200, 10.f, true))
                me->SetFacingToObject(rk);
            player->GetScheduler().Schedule(5s, [this, player](TaskContext context)
                {
                    RecordKeeperPath(player);
                    if (Creature* hr = me->FindNearestCreature(177449, 10.f, true))
                    {
                        hr->AI()->Talk(3);
                        hr->SetFacingToObject(me);
                        me->SetFacingToObject(hr);
                    }
                });
            player->GetScheduler().Schedule(15s, [this, player](TaskContext context) { Talk(4); });
            player->GetScheduler().Schedule(24s, [this, player](TaskContext context)
                {
                    if (Creature* hr = me->FindNearestCreature(177449, 10.f, true))
                        hr->AI()->Talk(4);
                });
            player->GetScheduler().Schedule(34s, [this, player](TaskContext context)
                {
                    if (Creature* hr = me->FindNearestCreature(177449, 10.f, true))
                        hr->GetMotionMaster()->MovePath(17744901, false);
                    Talk(5);
                });
            player->GetScheduler().Schedule(40s, [this, player](TaskContext context)
                {
                    if (Creature* hr = me->FindNearestCreature(177449, 10.f, true))
                        hr->AI()->Talk(5);
                });
            player->GetScheduler().Schedule(48s, [this, player](TaskContext context)
                {
                    Talk(6);
                    me->GetMotionMaster()->MovePath(17744901, false);
                });
        }
    }

    void RecordKeeperPath(Player* player)
    {
        int8 i = 0;
        std::list<Creature*> creatureList = player->FindNearestCreatures(178200, 20.0f);
        for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
        {
            if (Creature* rk = (*itr))
            {
                rk->SetWalk(true);
                rk->DespawnOrUnsummon(5s, 2min);
                rk->GetMotionMaster()->MovePoint(0, RecordKeeper_path[i], true);
            }
            i++;
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_VAULT_OF_SECRETS)
        {
            if (Creature* galan = me->FindNearestCreature(NPC_TAL_GALAN, 30.0f, true))
            {
                galan->RemoveAura(65050);
                me->ForcedDespawn(500);
            }
        }
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
        {
            player->PrepareQuestMenu(me->GetGUID());
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        }

        if (player->GetQuestStatus(QUEST_SECRETS_OF_THE_VAULT) == QUEST_STATUS_INCOMPLETE)
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready to see the message regarding what is within the vault.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, 42834, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*MenuID*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            CloseGossipMenuFor(player);
            Talk(0);
            me->AddDelayedEvent(6000, [this]()->void
                {
                    if (auto* bh = me->FindNearestCreature(177449, 10.f, true))
                        bh->AI()->Talk(1);
                    me->SummonCreature(178225, Position(3131.06f, 5990.97f, 4854.13f, 5.8346f), TEMPSUMMON_TIMED_DESPAWN, 2min);
                });
            me->AddDelayedEvent(11000, [this]()->void
                {
                    if (auto* primus = me->FindNearestCreature(178225, 20.f, true))
                        primus->AI()->Talk(0);
                });
            me->AddDelayedEvent(22000, [this]()->void
                {
                    if (auto* primus = me->FindNearestCreature(178225, 20.f, true))
                        primus->AI()->Talk(1);
                });
            me->AddDelayedEvent(29000, [this]()->void
                {
                    if (auto* primus = me->FindNearestCreature(178225, 20.f, true))
                        primus->ForcedDespawn();
                    if (auto* bh = me->FindNearestCreature(177449, 10.f, true))
                        bh->AI()->Talk(2);
                });
            me->AddDelayedEvent(36000, [this, player]()->void
                {
                    Talk(1);
                    player->KilledMonsterCredit(177423);
                });
            break;
        }
        return true;
    }
};

Position const enemy_mawsworn_path[4] =
{
    {3169.95f, 5826.98f, 4814.19f, 0},
    {3184.88f, 5820.69f, 4814.39f, 0},
    {3197.35f, 5825.47f, 4813.51f, 0},
    {3207.91f, 5836.14f, 4815.78f, 0},
};

Position const bonesmith_heirmir_path[3] =
{
    {3177.98f, 5818.08f, 4814.65f, 0},
    {3166.89f, 5836.08f, 4813.53f, 0},
    {3162.11f, 5862.94f, 4812.11f, 0},
};

//178157 Bonesmith Heirmir
struct npc_bonesmith_heirmir_178157 : public  ScriptedAI
{
public:
    npc_bonesmith_heirmir_178157(Creature* creature) : ScriptedAI(creature) {}

private:
    EventMap _events;
    bool eventMutilator;
    uint8 mawswornDiedCount;

    void Reset() override
    {
        _events.Reset();
        eventMutilator = false;
        mawswornDiedCount = 0;
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_LET_THE_ANIMA_FLOW) _events.ScheduleEvent(EVENT_LET_THE_ANIMA_FLOW_PHASE_I, 3s);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        switch (summon->GetEntry())
        {
        case 177291:
            mawswornDiedCount++;
            if (mawswornDiedCount == 2)
                _events.ScheduleEvent(EVENT_LET_THE_ANIMA_FLOW_PHASE_III, 3s);
            break;
        case 177292:
            summonMawswornXalvez();
            break;
        case 178191:
            summon->AI()->Talk(1);
            _events.ScheduleEvent(EVENT_LET_THE_ANIMA_FLOW_PHASE_IV, 3s);
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_LET_THE_ANIMA_FLOW_PHASE_I:
                me->AI()->Talk(1);
                me->GetMotionMaster()->MovePoint(1001, 3206.21f, 5839.17f, 4815.76f, true, 1.3201f);
                _events.ScheduleEvent(EVENT_LET_THE_ANIMA_FLOW_PHASE_II, 3s);
                break;
            case EVENT_LET_THE_ANIMA_FLOW_PHASE_II:
                summonMawswornCorruptor();
                break;
            case EVENT_LET_THE_ANIMA_FLOW_PHASE_III:
                summonMawswornMutilator();
                break;
            case EVENT_LET_THE_ANIMA_FLOW_PHASE_IV:
            {
                if (GameObject* ac = me->FindNearestGameObject(368296, 10.0f))
                {
                    ac->RemoveFlag(GO_FLAG_IN_USE);
                    ac->SetGoState(GO_STATE_ACTIVE);
                }
                if (Creature* rift = me->FindNearestCreature(NPC_ANIMS_RIFT, 10.0f, true))
                {
                    rift->AI()->DoAction(ACTION_ANIMA_FLOW_START);
                    me->SummonCreature(NPC_ANIMS_RIFT, Position(3066.82f, 5968.88f, 4847.64f, 0.09444f), TEMPSUMMON_TIMED_DESPAWN, 10min);
                    me->SummonCreature(NPC_ANIMS_RIFT, Position(2993.29f, 6604.89f, 4876.35f, 1.19268f), TEMPSUMMON_TIMED_DESPAWN, 10min);
                    me->SummonCreature(NPC_ANIMS_RIFT, Position(2997.96f, 6896.93f, 4867.35f, 2.61874f), TEMPSUMMON_TIMED_DESPAWN, 10min);
                }
                me->ResumeMovement();
                me->HandleEmoteCommand(EMOTE_STATE_NONE);
                me->GetMotionMaster()->Clear();
                me->AddDelayedEvent(2500, [this]() -> void
                    {
                        Talk(2);
                        me->GetMotionMaster()->MoveSmoothPath(1002, bonesmith_heirmir_path, 3, false, false);
                    });
            }
            break;
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            me->PauseMovement();
            me->HandleEmoteCommand(EMOTE_STATE_WORK_MINING);
        }
        if (id == 1002)
            me->ForcedDespawn(500);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_LET_THE_ANIMA_FLOW))
            {
                me->AddDelayedEvent(5000, [this]() -> void
                    {
                        Talk(0);
                    });
            }
        }
    }

    void summonMawswornCorruptor()
    {
        if (Creature* corruptor = me->SummonCreature(177291, Position(3163.53f, 5843.25f, 4812.13f, 5.05084f), TEMPSUMMON_TIMED_DESPAWN, 300s))
            corruptor->GetMotionMaster()->MoveSmoothPath(0, enemy_mawsworn_path, 4, false, false);
        me->AddDelayedEvent(1000, [this]() -> void
            {
                if (Creature* corruptor = me->SummonCreature(177291, Position(3163.53f, 5843.25f, 4812.13f, 5.05084f), TEMPSUMMON_TIMED_DESPAWN, 300s))
                    corruptor->GetMotionMaster()->MoveSmoothPath(0, enemy_mawsworn_path, 4, false, false);
            });
    }
    void summonMawswornMutilator()
    {
        if (Creature* mutilator = me->SummonCreature(177292, Position(3163.53f, 5843.25f, 4812.13f, 5.05084f), TEMPSUMMON_TIMED_DESPAWN, 300s))
            mutilator->GetMotionMaster()->MoveSmoothPath(0, enemy_mawsworn_path, 4, false, false);
    }
    void summonMawswornXalvez()
    {
        if (Creature* Xalvez = me->SummonCreature(178191, Position(3163.53f, 5843.25f, 4812.13f, 5.05084f), TEMPSUMMON_TIMED_DESPAWN, 300s))
        {
            Xalvez->GetMotionMaster()->MoveSmoothPath(0, enemy_mawsworn_path, 4, false, false);
            me->AddDelayedEvent(4500, [this, Xalvez]() -> void
                {
                    Xalvez->AI()->Talk(0);
                });
        }
    }
};

//368296 Anima Conduit
struct go_anima_conduit_368296 : public GameObjectAI
{
    go_anima_conduit_368296(GameObject* go) : GameObjectAI(go) {}
private:
    bool say63706;

    void Reset() override
    {
        say63706 = false;
    }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasQuest(QUEST_LET_THE_ANIMA_FLOW))
        {
            if (!say63706)
            {
                say63706 = true;
                me->SetGoState(GO_STATE_READY);
                me->SetFlag(GO_FLAG_IN_USE);
                if (Creature* heimir = player->FindNearestCreature(NPC_HEIMIR, 30.0f, true))
                    heimir->AI()->DoAction(ACTION_LET_THE_ANIMA_FLOW);
            }
        }
        return true;
    }
};

//178540 Anima Rift
struct npc_anima_rift_178540 : public  ScriptedAI
{
public:
    npc_anima_rift_178540(Creature* creature) : ScriptedAI(creature) {}
private:
    EventMap _events;
    Unit* owner = nullptr;

    void Reset() override
    {
        _events.Reset();
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            owner = player;
            _events.ScheduleEvent(EVENT_ANIMA_FLOW_ZONE_START, 1s);
            _events.ScheduleEvent(EVENT_ANIMA_HOOKING_OVER_START, 1s);
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_ANIMA_FLOW_START)
        {
            _events.ScheduleEvent(EVENT_ANIMA_FLOW_START, 1s);
            _events.ScheduleEvent(EVENT_ANIMA_FLOW_ZONE_START, 1s);
            me->DespawnOrUnsummon(10min, 10s);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!owner || !owner->IsInWorld() || !owner->IsAlive())
        {
            me->ForcedDespawn();
        }

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ANIMA_FLOW_START:
                if (Creature* animaflow = me->SummonCreature(172953, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 70s))
                    animaflow->GetMotionMaster()->MovePath(17295301, false);
                _events.Repeat(randtime(2s, 5s));
                break;
            case EVENT_ANIMA_FLOW_ZONE_START:

                if (Creature* animaflow = me->SummonCreature(172953, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 70s))
                {
                    if (animaflow->GetAreaId() == AREA_SCHOLARS_DEN)
                        animaflow->GetMotionMaster()->MovePath(17295302, false);
                }
                if (Creature* animaflow = me->SummonCreature(172953, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 70s))
                {
                    if (animaflow->GetAreaId() == AREA_HOPES_ASCENT)
                        animaflow->GetMotionMaster()->MovePath(17295303, false);
                }
                if (Creature* animaflow = me->SummonCreature(172953, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 70s))
                {
                    if (animaflow->GetAreaId() == AREA_KORTHIA)
                        if (!me->FindNearestCreature(178581, 10.f, true))
                            animaflow->GetMotionMaster()->MovePath(17295304, false);
                }
                _events.Repeat(randtime(2s, 5s));
                break;
            case EVENT_ANIMA_HOOKING_OVER_START:
                if (Creature* animaflow = me->SummonCreature(172953, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 70s))
                    if (me->FindNearestCreature(178581, 10.f, true))
                        animaflow->GetMotionMaster()->MovePath(17295305, false);
                _events.Repeat(randtime(2s, 5s));
                break;
            }
        }
    }
};

enum heirmir_grappling_event
{
    EVENT_GRAPPLING_346457 = 346457,
};

Position const heirmir_grappling_path_I[3] =
{
    {3020.45f, 6713.07f, 4862.46f, 1.64407f},
    {3027.54f, 6725.22f, 4858.16f, 0.57200f},
    {3052.33f, 6741.79f, 4853.28f, 1.21496f},
};

Position const heirmir_grappling_path_II[5] =
{
    {3058.38f, 6815.79f, 4854.66f},
    {3056.15f, 6836.69f, 4860.61f},
    {3058.08f, 6860.33f, 4865.46f},
    {3039.31f, 6867.83f, 4866.56f},
    {3033.61f, 6877.85f, 4860.94f},
};

Position const heirmir_grappling_path_III[5] =
{
    {2984.11f, 6889.84f, 4825.59f},
    {2974.94f, 6905.41f, 4821.54f},
    {2983.19f, 6922.78f, 4827.21f},
    {2978.06f, 6940.38f, 4827.91f},
    {2974.26f, 6950.93f, 4821.54f},
};

//177452 Bonesmith Heirmir
struct npc_bonesmith_heirmir_177452 : public  ScriptedAI
{
public:
    npc_bonesmith_heirmir_177452(Creature* creature) : ScriptedAI(creature) {}
private:
    ObjectGuid m_playerGUID;
    EventMap _events;
    uint8 smoothpath1 = 1;
    uint8 smoothpath2 = 2;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
        _events.Reset();
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_BONE_TOOLS)
        {
            Talk(1);
        }
        if (quest->GetQuestId() == QUEST_HOOKING_OVER)
        {
            Talk(2);
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(1001, heirmir_grappling_path_I[0], true, 1.64407f);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            switch (id)
            {
            case 1001:
                me->HandleEmoteCommand(EMOTE_ONESHOT_WORK);
                if (Creature* point = me->FindNearestCreature(NPC_GRAPPLE_POINT, 10.0f, true))
                    point->RemoveAura(65050);
                me->AddDelayedEvent(3500, [this]() -> void
                    {
                        if (Creature* galan = me->FindNearestCreature(NPC_TAL_GALAN_HOPE, 20.0f, true))
                            galan->AI()->Talk(1);
                    });
                me->AddDelayedEvent(9500, [this]() -> void
                    {
                        Talk(3);
                        me->GetMotionMaster()->MovePoint(1002, heirmir_grappling_path_I[1], true, 1.64407f);
                    });
                break;
            case 1002:
                HeirmirGappling(346456);
                break;
            case 1003:
                HeirmirGappling(346457);
                break;
            }
        }
        if (id == 1004 && smoothpath1)
        {
            if (Creature* point = me->FindNearestCreature(NPC_GRAPPLE_POINT, 10.0f, true))
                point->RemoveAura(65050);
            HeirmirGappling(346459);
        }
        if (id == 1005 && smoothpath2)
        {
            if (Creature* point = me->FindNearestCreature(NPC_GRAPPLE_POINT, 10.0f, true))
                point->RemoveAura(65050);
            HeirmirGappling(346461);
        }
    }

    void HeirmirGappling(uint32 spawnid)
    {
        std::list<Creature*> cList = me->FindNearestCreatures(NPC_GRAPPLE_POINT, 150.0f);
        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
        {
            if (Creature* point = *itr)
            {
                if (point->GetSpawnId() == spawnid)
                {
                    me->SetFacingToObject(point);
                    if (Creature* GP_tool = me->SummonCreature(NPC_GRAPPLE_POINT, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(1500)))
                        GP_tool->GetMotionMaster()->MoveJump(point->GetPosition(), 15, 30);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_THROWN);

                    me->AddDelayedEvent(1500, [this, point]() -> void
                        {
                            point->RemoveAura(65050);
                        });
                    me->AddDelayedEvent(3000, [this, point, spawnid]() -> void
                        {
                            if (spawnid == 346456)
                                Talk(4);
                            me->CastSpell(me, 352413, false);
                        });
                    me->AddDelayedEvent(7000, [this, point]() -> void
                        {
                            me->AddAura(SPELL_GRAPPLING_HOOK_VISUAL, point);
                        });
                    me->AddDelayedEvent(9000, [this, point]() -> void
                        {
                            me->CastSpell(point, SPELL_GRAPPLING_HOOK, false);
                        });
                    me->AddDelayedEvent(11000, [this, point, spawnid]() -> void
                        {
                            point->RemoveAura(SPELL_GRAPPLING_HOOK_VISUAL);
                            switch (spawnid)
                            {
                            case 346456:
                                me->GetMotionMaster()->MovePoint(1003, heirmir_grappling_path_I[2], true);
                                break;
                            case 346457:
                                me->GetMotionMaster()->MoveSmoothPath(1004, heirmir_grappling_path_II, 5, false, false);
                                break;
                            case 346459:
                                me->GetMotionMaster()->MoveSmoothPath(1005, heirmir_grappling_path_III, 5, false, false);
                                break;
                            case 346461:
                                me->DespawnOrUnsummon(3s, 1min);
                                break;
                            }
                        });
                }
            }
        }
    }

};

// 351205 Dispel Illusion
class spell_beacon_activating_351760 : public SpellScriptLoader
{
public:
    spell_beacon_activating_351760() : SpellScriptLoader("spell_beacon_activating_351760") { }

    class spell_beacon_activating_351760_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_beacon_activating_351760_SpellScript);

        void HandleAfterCast()
        {
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                {
                    player->CastSpell(player, 351754, false);
                }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_beacon_activating_351760_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_beacon_activating_351760_SpellScript();
    }
};

Position const Cracked_Record_path_i[8] =
{
    {3017.12f, 6683.65f, 4868.71f},
    {3004.99f, 6667.67f, 4870.39f},
    {3017.43f, 6651.59f, 4872.75f},
    {3030.01f, 6661.11f, 4875.69f},
    {3044.14f, 6667.08f, 4879.97f},
    {3059.94f, 6663.54f, 4883.28f},
    {3063.04f, 6649.38f, 4883.99f},
    {3053.22f, 6635.52f, 4887.03f},
};

Position const Cracked_Record_path_ii[7] =
{
    {3073.17f, 6626.38f, 4886.64f},
    {3065.27f, 6611.38f, 4891.08f},
    {3049.98f, 6599.27f, 4899.61f},
    {3032.73f, 6590.88f, 4902.85f},
    {3011.67f, 6591.31f, 4905.32f},
    {3002.66f, 6594.76f, 4905.64f},
    {3010.33f, 6604.51f, 4905.86f},
};

Position const Cracked_Record_path_iii[9] =
{
    {3000.28f, 6590.92f, 4905.66f},
    {2981.21f, 6589.14f, 4908.82f},
    {2970.99f, 6579.73f, 4914.03f},
    {2952.84f, 6560.61f, 4922.65f},
    {2937.47f, 6554.24f, 4926.04f},
    {2921.71f, 6553.41f, 4931.51f},
    {2908.01f, 6562.97f, 4936.24f},
    {2908.37f, 6577.36f, 4941.16f},
    {2914.62f, 6594.54f, 4943.21f},
};


uint8 CrackedRecordPathNumber01 = 0;
uint8 CrackedRecordPathNumber02 = 0;
uint8 CrackedRecordPathNumber03 = 0;

//178296 npc_animaflow
struct npc_animaflow_178296 : public  ScriptedAI
{
public:
    npc_animaflow_178296(Creature* creature) : ScriptedAI(creature) {}
private:
    ObjectGuid m_playerGUID;
    uint8 m_gamePhase;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
        m_gamePhase = 0;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Player* player = target->ToPlayer())
        {
            if (player->GetDistance2d(me) >= 15.0f)
            {
                if (CrackedRecordPathNumber01 > 0)
                    CrackedRecordPathNumber01--;
                if (CrackedRecordPathNumber02 > 0)
                    CrackedRecordPathNumber02--;
                if (CrackedRecordPathNumber03 > 0)
                    CrackedRecordPathNumber03--;
            }
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();

        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestObjectiveProgress(QUEST_LOST_RECORD, 0) && !player->GetQuestObjectiveProgress(QUEST_LOST_RECORD, 1))
            {
                m_gamePhase = 1;
                me->GetMotionMaster()->MovePoint(CrackedRecordPathNumber01, Cracked_Record_path_i[CrackedRecordPathNumber01], true);
            }
            if (player->GetQuestObjectiveProgress(QUEST_LOST_RECORD, 0) && player->HasItemCount(185716, 1, false))
            {
                m_gamePhase = 2;
                me->GetMotionMaster()->MovePoint(CrackedRecordPathNumber02, Cracked_Record_path_ii[CrackedRecordPathNumber02], true);
            }
            if (player->GetQuestObjectiveProgress(QUEST_LOST_RECORD, 0) && player->HasItemCount(185716, 2, false))
            {
                m_gamePhase = 3;
                me->GetMotionMaster()->MovePoint(CrackedRecordPathNumber03, Cracked_Record_path_iii[CrackedRecordPathNumber03], true);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            if (m_gamePhase == 1 && id == CrackedRecordPathNumber01 && CrackedRecordPathNumber01 < 8)
            {
                CrackedRecordPathNumber01++;
                if (CrackedRecordPathNumber01 == 7)
                {
                    if (me->FindNearestGameObject(367972, 10.f, true) == NULL)
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            player->SummonGameObject(367972, 3053.22f, 6635.52f, 4887.03f, 1.0768f, QuaternionData(), 0s);
                    }
                }
            }
            if (m_gamePhase == 2 && id == CrackedRecordPathNumber02 && CrackedRecordPathNumber02 < 7)
            {
                CrackedRecordPathNumber02++;
                if (CrackedRecordPathNumber02 == 6)
                {
                    if (me->FindNearestGameObject(367972, 10.f, true) == NULL)
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            player->SummonGameObject(367972, 3010.33f, 6604.51f, 4905.86f, 1.2253f, QuaternionData(), 0s);
                    }
                }
            }
            if (m_gamePhase == 3 && id == CrackedRecordPathNumber03 && CrackedRecordPathNumber03 < 9)
            {
                CrackedRecordPathNumber03++;
                if (CrackedRecordPathNumber03 == 8)
                {
                    if (me->FindNearestGameObject(367972, 10.f, true) == NULL)
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            player->SummonGameObject(367972, 2914.62f, 6594.54f, 4943.21f, 1.1735f, QuaternionData(), 0s);
                            player->RemoveAura(351754);
                        }
                        m_gamePhase = 0;
                        CrackedRecordPathNumber01 = 0;
                        CrackedRecordPathNumber02 = 0;
                        CrackedRecordPathNumber03 = 0;
                    }
                }
            }
        }
    }
};

// spell_matrix_beacon_351755
class spell_matrix_beacon_351755 : public SpellScriptLoader
{
public:
    spell_matrix_beacon_351755() : SpellScriptLoader("spell_matrix_beacon_351755") { }

    class spell_matrix_beacon_351755_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_matrix_beacon_351755_SpellScript);

        void HandleSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                {
                    player->SummonCreature(178296, player->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 2s);
                }
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_matrix_beacon_351755_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_matrix_beacon_351755_SpellScript();
    }
};

//352413 modified grappling
struct spell_modified_grappling_352413 : public SpellScript
{
    PrepareSpellScript(spell_modified_grappling_352413);

    void HandleDummy(SpellEffIndex effindex)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        player->AddAura(SPELL_GRAPPLING_HOOK_VISUAL, creTarget);
                        player->GetScheduler().Schedule(2s, [player, creTarget](TaskContext context)
                            {
                                player->CastSpell(creTarget, SPELL_GRAPPLING_HOOK, false);
                            });
                        player->GetScheduler().Schedule(4s, [creTarget](TaskContext context)
                            {
                                creTarget->RemoveAura(SPELL_GRAPPLING_HOOK_VISUAL);
                            });
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_modified_grappling_352413::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//352423 Grappling Hook
struct spell_grappling_hook_352423 : public SpellScript
{
    PrepareSpellScript(spell_grappling_hook_352423);

    void HandleDummy(SpellEffIndex effindex)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        creTarget->RemoveAura(SPELL_GRAPPLING_HOOK_VISUAL);
                    }
    }

    void Register() override
    {
        OnEffectSuccessfulDispel += SpellEffectFn(spell_grappling_hook_352423::HandleDummy, EFFECT_0, SPELL_EFFECT_JUMP_DEST_2);
    }
};

//178580 bonesmith heirmir
struct npc_bonesmith_heirmir_178580 : public  ScriptedAI
{
public:
    npc_bonesmith_heirmir_178580(Creature* creature) : ScriptedAI(creature) {}
private:
    uint8 questIncomlete = 0;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            CloseGossipMenuFor(player);
            Talk(0);
            me->GetMotionMaster()->MovePath(17858001, false);
            questIncomlete = 1;
            m_playerGUID = player->GetGUID();
        }
        return true;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            if (questIncomlete == 1 && id == 1)
            {
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                me->AddDelayedEvent(2500, [this]() -> void { Talk(1); });
                me->AddDelayedEvent(7000, [this]() -> void
                    {
                        Talk(2);
                        if (Creature* Xorul = me->SummonCreature(NPC_XORUL_THE_INSATIABLE, Position{ 2950.64f, 7237.06f, 4911.25f, 4.971f }, TEMPSUMMON_MANUAL_DESPAWN))
                            Xorul->GetMotionMaster()->MoveJump(Position{ 2952.56f, 7206.53f, 4900.76f, 4.546f }, 15, 25);
                    });
                questIncomlete = 0;
            }
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            player->SummonCreature(NPC_ANIMS_RIFT, Position(2950.58f, 7236.47f, 4911.53f, 4.7076f), TEMPSUMMON_TIMED_DESPAWN, 10min);
        me->SetStandState(UNIT_STAND_STATE_STAND);
        Talk(6);
        me->GetMotionMaster()->MovePath(17858002, false);
    }
};

//177459 protector kah_rev
struct npc_protector_kah_rev_177459 : public  ScriptedAI
{
public:
    npc_protector_kah_rev_177459(Creature* creature) : ScriptedAI(creature) {}
private:
    bool say63714;

    void Reset() override
    {
        say63714 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 10.0f))
                if (!say63714)
                {
                    say63714 = true;
                    Talk(0);
                }
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_DEFENDING_THE_VAULT || quest->GetQuestId() == QUEST_KEEPERS_OF_KORTHIA)
        {
            if (!player->FindNearestCreature(178640, 50.f, true))
            {
                player->CastSpell(player, SPELL_CHARGE_FORWARD, false);
                if (Creature* bh = me->FindNearestCreature(177452, 10.f, true))
                    bh->DespawnOrUnsummon(1s, 1min);
                if (Creature* tal = me->FindNearestCreature(177451, 10.f, true))
                    tal->DespawnOrUnsummon(1s, 1min);
                me->DespawnOrUnsummon(1s, 1min);
            }
        }
    }
};

//332315 collect_death_mote
struct spell_collect_death_mote_332315 : public SpellScript
{
    PrepareSpellScript(spell_collect_death_mote_332315);

    void HandleDummy(SpellEffIndex effindex)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = GetHitUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        target->DestroyForPlayer(player);
                        player->KilledMonsterCredit(177549);
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_collect_death_mote_332315::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 352607 spell_releasing
class spell_releasing_352607 : public SpellScriptLoader
{
public:
    spell_releasing_352607() : SpellScriptLoader("spell_releasing_352607") { }

    class spell_releasing_352607_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_releasing_352607_SpellScript);

        void HandleDummy(SpellEffIndex effindex)
        {
            PreventHitDefaultEffect(effindex);
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                    if (Unit* target = player->GetSelectedUnit())
                        if (Creature* creTarget = target->ToCreature())
                        {
                            switch (creTarget->GetEntry())
                            {
                            case 178654:
                                player->CastSpell(player, SPELL_SUMMON_TAL_YOR, false);
                                player->KilledMonsterCredit(178654);
                                creTarget->DespawnOrUnsummon(1s, 1min);
                                break;
                            case 178656:
                                if (Creature* rahl = player->SummonCreature(178650, creTarget->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 5min))
                                    rahl->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle());
                                player->KilledMonsterCredit(178656);
                                creTarget->DespawnOrUnsummon(1s, 1min);
                                break;
                            }
                        }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_releasing_352607_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_releasing_352607_SpellScript();
    }
};

//178647 secutor_mevix
struct npc_secutor_mevix_178647 : public  ScriptedAI
{
public:
    npc_secutor_mevix_178647(Creature* creature) : ScriptedAI(creature) {}

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_DEFENDING_THE_VAULT || quest->GetQuestId() == QUEST_KEEPERS_OF_KORTHIA)
        {
            std::list<Creature*> cList = player->FindAllCreaturesInRange(50.f);
            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
            {
                if (Creature* invisiable = *itr)
                {
                    switch (invisiable->GetEntry())
                    {
                    case  178638:
                        invisiable->ForcedDespawn();
                        break;
                    case  178639:
                        invisiable->ForcedDespawn();
                        break;
                    case  178640:
                        invisiable->ForcedDespawn();
                        break;
                    case  178649:
                        invisiable->ForcedDespawn();
                        break;
                    case  178650:
                        invisiable->ForcedDespawn();
                        break;
                    }

                    if (invisiable->HasAura(65050))
                        invisiable->RemoveAura(65050);
                }
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 15.0f) && player->GetQuestStatus(QUEST_KEEPERS_OF_KORTHIA) == QUEST_STATUS_REWARDED)
            {
                std::list<Creature*> cList = player->FindAllCreaturesInRange(40.f);
                for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                {
                    if (Creature* invisiable = *itr)
                    {
                        if (invisiable->HasAura(65050))
                            invisiable->RemoveAura(65050);
                    }
                }
            }
        }
    }
};

Position const into_the_vault_path[10] =
{
    {3201.13f, 6108.69f, 4799.52f, 0.79335f},    //0 bh_first_pos, tal_forth_pos
    {3179.66f, 6118.43f, 4800.51f, 1.94004f},    //1 tal_first_pos, tal_third_pos
    {3186.53f, 6116.15f, 4799.94f, 2.52909f},    //2 bh_second_pos
    {3198.11f, 6112.44f, 4799.81f, 5.92201f},    //3 bh third_pos
    {3192.22f, 6103.74f, 4798.79f, 1.0329f},     //4 RK_pos
    {3204.38f, 6113.94f, 4807.99f, 3.96637f},    //5 kalvez_pos
    {3193.96f, 6099.95f, 4798.44f, 4.01742f},    //6 bh_forth_pos
    {3186.91f, 6104.38f, 4799.16f, 2.12067f},    //7 bh_fiveth_pos
    {3188.81f, 6101.11f, 4798.83f, 5.14836f},    //8 tal_fiveth_pos
    {3190.54f, 6097.32f, 4798.48f, 5.14051f},    //9 end_pos
};

//177562 tal_galan
struct npc_tal_galan_177562 : public  ScriptedAI
{
public:
    npc_tal_galan_177562(Creature* creature) : ScriptedAI(creature) {}
private:
    bool player_flying;
    EventMap _events;
    ObjectGuid m_playerGUID;
    ObjectGuid heirmirGUID;

    void Reset() override
    {
        player_flying = false;
        _events.Reset();
        m_playerGUID = ObjectGuid::Empty;
        heirmirGUID = ObjectGuid::Empty;
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_INTO_THE_VAULT_FINAL)
        {
            me->GetMotionMaster()->MovePoint(1003, into_the_vault_path[1], true, 1.94004f);
            if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
            {
                bh->AI()->Talk(4);
                bh->GetMotionMaster()->MovePoint(0, into_the_vault_path[6], true, 4.01742f);
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 15.0f) && player->HasQuest(QUEST_INTO_THE_VAULT))
            {
                if (!player_flying)
                {
                    player_flying = true;
                    m_playerGUID = player->GetGUID();
                    player->SetCanFly(false);
                    Talk(0);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(1001, into_the_vault_path[1], true, 1.94004f);
                    if (Creature* bh = me->FindNearestCreature(177564, 10.f, true))
                    {
                        heirmirGUID = bh->GetGUID();
                        bh->GetMotionMaster()->MovePoint(0, into_the_vault_path[0], true, 0.79335f);
                    }
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_INTO_THE_VAULT_I:
                me->GetMotionMaster()->MovePoint(1002, into_the_vault_path[0], true, 0.79335f);
                if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
                {
                    me->AddDelayedEvent(3000, [this, bh]() ->void
                        {
                            Talk(2);
                            bh->GetMotionMaster()->MovePoint(0, into_the_vault_path[3], true, 2.52909f);
                        });
                }
                break;
            case EVENT_INTO_THE_VAULT_II:
                if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
                    if (Creature* xalvez = me->FindNearestCreature(177566, 10.f, true))
                        if (Creature* RK = me->FindNearestCreature(177567, 10.f, true))
                            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            {
                                bh->AI()->Talk(2);
                                me->AddDelayedEvent(4000, [this, xalvez]()  ->void { xalvez->AI()->Talk(1); });
                                me->AddDelayedEvent(11000, [this]()         ->void { Talk(4); });
                                me->AddDelayedEvent(16000, [this, xalvez]() ->void { xalvez->AI()->Talk(2); });
                                me->AddDelayedEvent(20000, [this, bh, player, xalvez, RK]()
                                    {
                                        bh->AI()->Talk(3);
                                        player->RemoveAura(SPELL_COLLECTED);
                                        bh->RemoveAura(SPELL_COLLECTED);
                                        me->RemoveAura(SPELL_COLLECTED);
                                        xalvez->SetFaction(16);
                                        xalvez->SetHover(false);
                                        xalvez->SetHoverHeight(1);
                                        xalvez->Attack(bh, true);
                                        RK->SetFaction(16);
                                    });
                            }
                break;
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            switch (id)
            {
            case 1001:
                if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
                {
                    bh->AI()->Talk(0);
                    me->AddDelayedEvent(5000, [this]() ->void { Talk(1); });
                    bh->SetWalk(true);
                    me->AddDelayedEvent(8000, [bh]() ->void
                        {
                            bh->GetMotionMaster()->MovePoint(0, into_the_vault_path[2], true, 2.52909f);
                            ; });
                    me->AddDelayedEvent(13000, [bh]() ->void { bh->AI()->Talk(1); });
                }
                _events.ScheduleEvent(EVENT_INTO_THE_VAULT_I, 16s);
                break;
            case 1002:
                if (Creature* xalvez = me->SummonCreature(177566, 3176.71f, 6087.71f, 4798.71f, 0.8757f, TEMPSUMMON_MANUAL_DESPAWN))
                    if (Creature* RK = me->SummonCreature(177567, 3180.71f, 6083.72f, 4798.21f, 0.7226f, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        me->AddDelayedEvent(3000, [this, xalvez, RK]() ->void
                            {
                                if (GameObject* SigilVault = me->FindNearestGameObject(367515, 10.f, false))
                                    SigilVault->SetGoState(GO_STATE_ACTIVE);
                                Talk(3);
                                xalvez->GetMotionMaster()->MovePoint(0, into_the_vault_path[5], true, 3.96637f);
                                xalvez->SetHover(true);
                                xalvez->SetHoverHeight(4);
                                xalvez->SetFaction(36);
                                RK->SetFaction(36);
                                RK->GetMotionMaster()->MovePoint(0, into_the_vault_path[4], true, 1.0329f);
                                xalvez->CastSpell(RK, SPELL_BONDS_OF_BROTHERHOOD, false);
                                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                                    if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
                                    {
                                        xalvez->CastSpell(player, SPELL_COLLECTED, false);
                                        me->AddAura(SPELL_COLLECTED, me);
                                        bh->AddAura(SPELL_COLLECTED, bh);
                                    }
                            });
                        me->AddDelayedEvent(7000, [this, xalvez]() ->void { xalvez->AI()->Talk(0); });
                    }
                _events.ScheduleEvent(EVENT_INTO_THE_VAULT_II, 13s);
                break;
            case 1003:
                Talk(7);
                if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
                {
                    me->AddDelayedEvent(5000, [bh]() ->void
                        {
                            bh->GetMotionMaster()->MovePoint(0, into_the_vault_path[7], true, 2.12067f);
                        });
                    me->AddDelayedEvent(8000, [bh]() ->void { bh->AI()->Talk(5); });
                    me->AddDelayedEvent(11000, [this]() ->void
                        {
                            me->GetMotionMaster()->MovePoint(1004, into_the_vault_path[8], true, 5.14836f);
                        });
                }
                break;
            case 1004:
                Talk(8);
                me->CastSpell(me, SPELL_SCHOLAR_CASTING_FX, false);
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
                    {
                        me->AddDelayedEvent(5000, [this, player]() ->void
                            {
                                if (!player->GetPhaseShift().HasPhase(13630)) PhasingHandler::AddPhase(player, 13630, true);
                                if (!player->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(player, 169, true);
                                me->RemoveAura(SPELL_SCHOLAR_CASTING_FX);
                            });
                        me->AddDelayedEvent(8000, [this, bh]() ->void
                            {
                                me->GetMotionMaster()->MovePoint(0, into_the_vault_path[9], true, 5.14836f);
                                bh->GetMotionMaster()->MovePoint(0, into_the_vault_path[9], true, 5.14836f);
                                bh->DespawnOrUnsummon(4s, 1min);
                                me->DespawnOrUnsummon(5s, 1min);
                            });
                    }
                break;
            }
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == 177566)
        {
            if (Creature* RK = me->FindNearestCreature(177567, 20.f, true))
                RK->AI()->Talk(0);
            if (me->FindNearestCreature(177567, 20.f, false))
            {
                summon->AI()->Talk(4);
                afterKillXalvez();
            }
        }
        if (summon->GetEntry() == 177567)
        {
            if (Creature* xalvez = me->FindNearestCreature(177566, 20.f, true))
                xalvez->AI()->Talk(3);
            if (me->FindNearestCreature(177566, 20.f, false))
            {
                summon->AI()->Talk(1);
                afterKillXalvez();
            }
        }
    }

    void afterKillXalvez()
    {
        me->AddDelayedEvent(2000, [this]() ->void
            {
                Talk(5);
                if (Creature* bh = ObjectAccessor::GetCreature(*me, heirmirGUID))
                {
                    bh->GetMotionMaster()->Clear();
                    bh->GetMotionMaster()->MovePoint(0, into_the_vault_path[3], true, 2.52909f);
                }
            });
        if (Creature* primus = me->FindNearestCreature(178225, 10.f, true))
        {
            me->AddDelayedEvent(11000, [this, primus]() ->void
                {
                    primus->RemoveAura(65050);
                    primus->AI()->Talk(4);
                });
            me->AddDelayedEvent(18000, [this, primus]() ->void { primus->AI()->Talk(5); });
            me->AddDelayedEvent(27000, [this, primus]() ->void
                {
                    Talk(6);
                    primus->AddAura(65050, primus);
                    if (Creature* sigil = me->FindNearestCreature(178758, 10.f, true))
                        sigil->RemoveAura(65050);
                });
        }
    }
};

// 352820 Retrieving
struct spell_retrieving_352820 : public SpellScript
{
    PrepareSpellScript(spell_retrieving_352820);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                if (Creature* TalGalan = player->FindNearestCreature(177562, 20.f, true))
                    TalGalan->AI()->DoAction(ACTION_INTO_THE_VAULT_FINAL);
            }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_retrieving_352820::HandleAfterCast);
    }
};

//178713 Archivist Roh-Senara
struct npc_roh_senara_178713 : public  ScriptedAI
{
public:
    npc_roh_senara_178713(Creature* creature) : ScriptedAI(creature) {}
private:
    bool summonTzench;

    void Reset() override
    {
        summonTzench = false;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_CARVING_OUT_A_PATH) && player->HasQuest(QEUST_THE_SUNDERED_STAFF))
            Talk(3);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 45.0f))
            {
                if (player->GetQuestStatus(QUEST_INTERRUPT_THE_INTERROGATIONS) == QUEST_STATUS_INCOMPLETE && !me->HasAura(65050))
                {
                    if (!me->FindNearestCreature(178727, 50.f, true) && !summonTzench)
                    {
                        summonTzench = true;
                        me->SummonCreature(178727, Position(2657.14f, 5876.44f, 4791.11f, 5.70816f), TEMPSUMMON_MANUAL_DESPAWN);  //Interrogator Tzench
                    }
                }

                if (player->GetQuestStatus(QUEST_INTERRUPT_THE_INTERROGATIONS) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_INTERRUPT_THE_INTERROGATIONS) == QUEST_STATUS_REWARDED)
                    me->RemoveAura(65050);
            }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (GameObject* Cage = me->FindNearestGameObject(368604, 10.0f))   //Mawsworn Cage
            Cage->RemoveFlag(GO_FLAG_LOCKED);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_INTERRUPT_THE_INTERROGATIONS)
            interruptTheInterrogations();
    }

    void interruptTheInterrogations()
    {
        Talk(0);
        me->AddDelayedEvent(3500, [this]() ->void
            {
                Talk(1);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, 2636.68f, 5875.79f, 4790.32f, true);
                me->DespawnOrUnsummon(5s, 1min);
            });
    }

};

//368604 Mawsworn Cage
struct go_mawsworn_cage_368604 : public GameObjectAI
{
    go_mawsworn_cage_368604(GameObject* go) : GameObjectAI(go) {}

    void SpellHit(WorldObject* caster, SpellInfo const* /*spellInfo*/)
    {
        if (Player* player = caster->ToPlayer())
        {
            player->KilledMonsterCredit(177556);
            if (Creature* senara = player->FindNearestCreature(178713, 10.f, true))
                senara->AI()->DoAction(ACTION_INTERRUPT_THE_INTERROGATIONS);
        }
    }
};

//186157 item_Lodestaff
class item_lodestaff : public ItemScript
{
public:
    item_lodestaff() : ItemScript("item_lodestaff") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid /*castId*/) override
    {
        std::list<Creature*> spellTargetsList;
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 177245, 10.0f); // Ravenous Gorger
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 177198, 10.0f); // Ravenous Scavenger
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 177295, 10.0f); // Colossal Worldeater 
        player->GetCreatureListWithEntryInGrid(spellTargetsList, 178880, 10.0f); // Hungering Mite
        for (auto& targets : spellTargetsList)
        {
            if (targets->HasAura(SPELL_CAN_DRAIN_AURA))
            {
                switch (targets->GetEntry())
                {
                case 177245:
                    questProgressIncre(player, targets, 12);
                    break;
                case 177198:
                    questProgressIncre(player, targets, 9);
                    break;
                case 177295:
                    questProgressIncre(player, targets, 16);
                    break;
                case 178880:
                    questProgressIncre(player, targets, 6);
                    break;
                }
            }
        }
        return true;
    }

    void questProgressIncre(Player* player, Creature* targets, uint8 incre)
    {
        player->CastSpell(targets, SPELL_DRAINING, true);
        for (uint8 i = 0; i < incre; i++)
        {
            player->KilledMonsterCredit(NPC_DRAIN_KILLCREDIT);
        }
        player->GetScheduler().Schedule(3s, [targets](TaskContext context)
            {
                targets->RemoveAura(SPELL_CAN_DRAIN_AURA);
            });
    }
};

//368617 368676 368677 368678 Memorial to Archivist
struct go_archivist_teachings : public GameObjectAI
{
    go_archivist_teachings(GameObject* go) : GameObjectAI(go) {}
private:
    uint8 talkNumber;

    bool OnReportUse(Player* player) override
    {
        if (me->GetGoState() == GO_STATE_READY)
        {
            switch (me->GetEntry())
            {
            case 368617:
                talkNumber = 1;
                player->KilledMonsterCredit(178853);  // Archivist Roh - Gema visited
                if (Creature* npc = me->SummonCreature(178755, 2689.21f, 6669.08f, 4810.44f, 4.7665f, TEMPSUMMON_TIMED_DESPAWN, 1min))
                    ArchivistTalk(player, npc, talkNumber);
                break;
            case 368676:
                talkNumber = 2;
                player->KilledMonsterCredit(178939);  // Archivist Roh - Luda visited
                if (Creature* npc = me->SummonCreature(179026, 2865.91f, 6758.03f, 4816.06f, 3.0961f, TEMPSUMMON_TIMED_DESPAWN, 1min))
                    ArchivistTalk(player, npc, talkNumber);
                break;
            case 368677:
                talkNumber = 3;
                player->KilledMonsterCredit(178940);  // Archivist Roh - Kiel visited
                if (Creature* npc = me->SummonCreature(179027, 2750.88f, 6928.09f, 4836.22f, 5.6879f, TEMPSUMMON_TIMED_DESPAWN, 1min))
                    ArchivistTalk(player, npc, talkNumber);
                break;
            case 368678:
                talkNumber = 4;
                player->KilledMonsterCredit(178941);  // Archivist Roh - Mah visited
                if (Creature* npc = me->SummonCreature(179032, 2551.08f, 6813.35f, 4844.01f, 6.0914f, TEMPSUMMON_TIMED_DESPAWN, 1min))
                    ArchivistTalk(player, npc, talkNumber);
                break;
            }
        }
        return true;
    }

    void ArchivistTalk(Player* player, Creature* npc, uint8 talkNumber)
    {
        if (Creature* suir = player->FindNearestCreature(178914, 20.f, true))
        {
            suir->AI()->Talk(talkNumber);
            player->AddDelayedEvent(4000, [npc, player, suir]() -> void
                {
                    npc->AI()->Talk(0);
                    if (player->GetQuestStatus(QUEST_CONSULTING_THE_EXPERTS) == QUEST_STATUS_COMPLETE)
                        suir->ForcedDespawn();
                });
        }
        me->SetGoState(GO_STATE_ACTIVE);
    }
};

//177928 Archivist Roh-Senara
struct npc_roh_senara_177928 : public  ScriptedAI
{
public:
    npc_roh_senara_177928(Creature* creature) : ScriptedAI(creature) {}

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (!me->FindNearestCreature(178914, 50.f, true))
        {
            if (player->HasQuest(QUEST_AN_INFUSION_OF_ANIMA) && player->HasQuest(QUEST_CONSULTING_THE_EXPERTS))
            {
                Talk(0);
                player->CastSpell(player, SPELL_SUMMON_SCHOLAR_ROH_SUIR, true);
                if (Creature* suir = me->FindNearestCreature(177931, 30.f, true))
                    suir->DespawnOrUnsummon(1s, 1min);
            }
        }
        if (player->HasQuest(QEUST_HOPE_ASCENDING))
        {
            me->GetMotionMaster()->MovePath(17792801, false);
            if (Creature* suir = me->FindNearestCreature(177931, 10.f, true))
            {
                suir->AI()->Talk(0);
                suir->GetMotionMaster()->MovePath(17793101, false);
            }
            me->AddDelayedEvent(9000, [this]() -> void { Talk(1); });
            me->AddDelayedEvent(19000, [this]() -> void { Talk(2); });
        }
    }
};

//178991 Archivist Roh-Senara
struct npc_roh_senara_178991 : public  ScriptedAI
{
public:
    npc_roh_senara_178991(Creature* creature) : ScriptedAI(creature) {}
private:
    EventMap _events;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        _events.Reset();
        m_playerGUID = ObjectGuid::Empty;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_FINDING_ONES_TRUE_PURPOSE)
        {
            m_playerGUID = player->GetGUID();
            Talk(0);
            me->SummonCreature(178993, 3094.62f, 6670.17f, 4888.45f, 6.2291f, TEMPSUMMON_TIMED_DESPAWN, 1min);   //Fate Fragment
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_ATTUNING_SENARA_TO_LODESTAFF)
        {
            Talk(1);
            _events.ScheduleEvent(EVENT_ATTUNING_SENARA_TO_LODESTAFF, 17s);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                if (Creature* lodestaff = me->FindNearestCreature(178998, 15.f, true))
                {
                    switch (eventId)
                    {
                    case EVENT_ATTUNING_SENARA_TO_LODESTAFF:
                        Talk(2);
                        player->GetScheduler().Schedule(15s, [this, player, lodestaff](TaskContext context)
                            {
                                Talk(3);
                                if (Creature* gema = me->SummonCreature(179000, 3099.37f, 6679.81f, 4889.21f, 4.9563f, TEMPSUMMON_TIMED_DESPAWN, 2min))
                                    gema->CastSpell(lodestaff, SPELL_EMPOWERING_NPC_SMALL, false);
                            });
                        player->GetScheduler().Schedule(20s, [this, player, lodestaff](TaskContext context)
                            {
                                Talk(4);
                                if (Creature* Luda = me->SummonCreature(179035, 3096.36f, 6677.03f, 4888.82f, 5.56112f, TEMPSUMMON_TIMED_DESPAWN, 2min))
                                    Luda->CastSpell(lodestaff, SPELL_EMPOWERING_NPC_SMALL, false);
                            });
                        player->GetScheduler().Schedule(25s, [this, player, lodestaff](TaskContext context)
                            {
                                Talk(5);
                                if (Creature* Kiel = me->SummonCreature(179036, 3098.18f, 6661.99f, 4888.76f, 1.01758f, TEMPSUMMON_TIMED_DESPAWN, 2min))
                                    Kiel->CastSpell(lodestaff, SPELL_EMPOWERING_NPC_SMALL, false);
                            });
                        player->GetScheduler().Schedule(30s, [this, player, lodestaff](TaskContext context)
                            {
                                Talk(6);
                                if (Creature* Mah = me->SummonCreature(179037, 3095.61f, 6666.08f, 4888.49f, 0.444243f, TEMPSUMMON_TIMED_DESPAWN, 2min))
                                    Mah->CastSpell(lodestaff, SPELL_EMPOWERING_NPC_SMALL, false);
                            });
                        player->GetScheduler().Schedule(35s, [this, player, lodestaff](TaskContext context) { Talk(7); });
                        player->GetScheduler().Schedule(48s, [this, player, lodestaff](TaskContext context)
                            {
                                me->AddAura(352815, me);
                                me->CastSpell(lodestaff, SPELL_EMPOWERING_NPC_BIG, false);
                                me->SummonCreature(179403, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 2min);
                                if (Creature* suir = me->FindNearestCreature(178992, 15.f, true))
                                    suir->AI()->Talk(0);
                            });
                        player->GetScheduler().Schedule(54s, [this, player](TaskContext context)
                            {
                                Talk(8);
                                player->KilledMonsterCredit(179009);
                            });
                        player->GetScheduler().Schedule(58s, [this, player](TaskContext context)
                            {
                                player->CastSpell(player, SPELL_ATTUNING_CANCLE, false);
                                std::list<Creature*> cList = me->FindAllCreaturesInRange(20.f);
                                for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                                {
                                    if (Creature* hasAura = *itr)
                                    {
                                        if (hasAura->HasAura(352815))
                                            hasAura->ForcedDespawn(1000);
                                    }
                                }
                                me->DespawnOrUnsummon(2s, 1min);
                            });
                        break;
                    }
                }
        }
    }
};

//178993 Fate Fragment_circle_aura_npc
struct npc_fate_fragment_178993 : public  ScriptedAI
{
public:
    npc_fate_fragment_178993(Creature* creature) : ScriptedAI(creature) {}
private:
    bool accept_circle;

    void Reset() override
    {
        accept_circle = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 2.0f) && player->HasQuest(QUEST_FINDING_ONES_TRUE_PURPOSE))
                if (!accept_circle)
                {
                    accept_circle = true;
                    player->CastSpell(player, SPELL_RITUAL_PARTICIPATION, true);
                    me->ForcedDespawn(1000);
                }
        }
    }
};

// 353188 Attune the Lodestaff
class spell_attune_the_lodestaff_353188 : public SpellScript
{
    PrepareSpellScript(spell_attune_the_lodestaff_353188);

    void HandleSendEvent(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            caster->RemoveAura(SPELL_RITUAL_PARTICIPATION);
            if (Creature* staff = caster->FindNearestCreature(178998, 15.f, true))
                caster->CastSpell(staff, SPELL_ATTUNING_LODESTAFF, false);
            if (Creature* senara = caster->FindNearestCreature(178991, 15.f, true))
                senara->AI()->DoAction(ACTION_ATTUNING_SENARA_TO_LODESTAFF);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_attune_the_lodestaff_353188::HandleSendEvent, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
    }
};

static const Position Establishing_ScholarPos[2] =
{
    { 3314.23f, 5855.18f, 4904.85f, 6.074f },   // 178313 suir_pos
    { 3320.77f, 5852.21f, 4904.97f, 2.798f },   // 178991 senara_pos
};

//178257 Archivist Roh-Suir
struct npc_archivist_roh_suir_178257 : public  ScriptedAI
{
public:
    npc_archivist_roh_suir_178257(Creature* creature) : ScriptedAI(creature) {}
private:
    bool player_sight;

    void Reset() override
    {
        player_sight = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 100.0f) && player->HasQuest(QUEST_ESTABLISHING_THE_ARCHIVE))
                if (!player_sight)
                {
                    player_sight = true;
                    me->AddAura(65050, me);
                    me->SummonCreature(178313, Establishing_ScholarPos[0], TEMPSUMMON_TIMED_DESPAWN, 2min);
                    if (Creature* senara = me->SummonCreature(178991, Establishing_ScholarPos[1], TEMPSUMMON_TIMED_DESPAWN, 2min))
                        senara->AddAura(352815, senara);
                }
        }
    }
};

// 223600 Capturing - npc_Drab_Gromit_179344
class spell_capturing_223600 : public SpellScriptLoader
{
public:
    spell_capturing_223600() : SpellScriptLoader("spell_capturing_223600") { }

    class spell_capturing_223600_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_capturing_223600_SpellScript);

        void HandleDummy(SpellEffIndex /*effindex*/)
        {
            if (Unit* caster = GetCaster())
                if (Player* player = caster->ToPlayer())
                    if (Creature* target = GetHitCreature())
                    {
                        target->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        player->KilledMonsterCredit(179344);
                        target->ForcedDespawn(1000);
                    }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_capturing_223600_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_capturing_223600_SpellScript();
    }
};

void AddSC_zone_korthia()
{
    new player_korthia();
    new scene_korthia();
    RegisterCreatureAI(npc_tal_galan_177227);
    RegisterGameObjectAI(go_waystone_to_oribos_368216);
    RegisterCreatureAI(npc_tal_galan_177927);
    RegisterCreatureAI(npc_highlord_bolvar_fordragon_177228);
    RegisterCreatureAI(npc_surveyer_mnemis_180525);
    RegisterItemScript(item_restorative_contraption);
    RegisterCreatureAI(npc_caretaker_kah_than_177610);
    RegisterCreatureAI(npc_concerned_kah_than_177924);
    RegisterGameObjectAI(go_unbroken_anima_container_368385);
    RegisterGameObjectAI(go_unburned_incense_368386);
    RegisterGameObjectAI(go_small_offering_368387);
    RegisterCreatureAI(npc_fatescribe_roh_kalo_178235);
    new spell_dispel_illusion_351205();
    RegisterCreatureAI(npc_bonesmith_heirmir_177449);
    RegisterCreatureAI(npc_tal_galan_177423);
    RegisterCreatureAI(npc_bonesmith_heirmir_178157);
    RegisterGameObjectAI(go_anima_conduit_368296);
    RegisterCreatureAI(npc_anima_rift_178540);
    RegisterCreatureAI(npc_bonesmith_heirmir_177452);
    new spell_beacon_activating_351760();
    RegisterCreatureAI(npc_animaflow_178296);
    new spell_matrix_beacon_351755();
    RegisterSpellScript(spell_modified_grappling_352413);
    RegisterSpellScript(spell_grappling_hook_352423);
    RegisterCreatureAI(npc_bonesmith_heirmir_178580);
    RegisterCreatureAI(npc_protector_kah_rev_177459);
    RegisterSpellScript(spell_collect_death_mote_332315);
    new spell_releasing_352607();
    RegisterCreatureAI(npc_secutor_mevix_178647);
    RegisterCreatureAI(npc_tal_galan_177562);
    RegisterSpellScript(spell_retrieving_352820);
    RegisterCreatureAI(npc_roh_senara_178713);
    RegisterGameObjectAI(go_mawsworn_cage_368604);
    RegisterItemScript(item_lodestaff);
    RegisterGameObjectAI(go_archivist_teachings);
    RegisterCreatureAI(npc_roh_senara_177928);
    RegisterCreatureAI(npc_roh_senara_178991);
    RegisterCreatureAI(npc_fate_fragment_178993);
    RegisterSpellScript(spell_attune_the_lodestaff_353188);
    RegisterCreatureAI(npc_archivist_roh_suir_178257);
    new spell_capturing_223600();
}
