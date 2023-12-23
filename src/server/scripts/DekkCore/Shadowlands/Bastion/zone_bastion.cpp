/*
 * Copyright 2021
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
#include "Conversation.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "PhasingHandler.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "ScriptedGossip.h"
#include "Weather.h"
#include "Spell.h"

uint8 raw_material_num = 0;
uint8 FE_Count = 0;

enum Bastion_Phase_Spell
{
    PHASE_GENERIC = 10100,

    SPELL_EXTRACT_ANIMA = 319782,
    SPELL_ANIMA_CANNISTER = 323931,
    SPELL_BATHE = 308755,
    SPELL_PERSPECTIVE_OVERRIDE = 331260,
    SPELL_KLEIAS_PERSPECTIVE = 331259,
    SPELL_PERSPECTIVE_PHASE_AURA = 331267,     // phase 15874
    SPELL_RIDE_VEHICLE_HARDCODED = 46598,
    SPELL_LEAVE_AREA_TRIGGER = 312223,
    SPELL_CANCEL_CARRY_AURA = 324933,
    SPELL_STACK_TEST = 310636,
    SPELL_ANIMA_SURGE = 310639,
    SPELL_CLEAR_ALL = 310671,
    SPELL_DEEP_INTROSPECTION_STUN = 311068,
    SPELL_MIRROR_IMAGE_AURA = 310787,     // Duplicate the other Unit's image
    SPELL_MIRROR_IMAGE_SPAWN_SHADER = 312495,
    SPELL_DEEP_INTROSPECTION_FADEIN = 311079,
    SPELL_OVERWHELMING_CLARITY = 311085,     // FadeOut_tele
    SPELL_EXTRA_ACTION_BUTTON = 311241,
    SPELL_PLACEHOLDER_RP_PING = 311246,
    SPELL_FLYING_KYRING_BEAM_CHANNEL = 311374,     // CHANNELING
    SPELL_FLYING_KYRING_BEAM_CHANNEL2 = 336607,     // CHANNELING
    SPELL_BANDAGING = 318032,
    SPELL_SUMMON_PELAGOS = 321478,
    SPELL_HELENE_RINGING = 327357,
    SPELL_RING_BELL_LARGE = 312780,
    SPELL_RING_BELL_MEDIUM = 312797,
    SPELL_RING_BELL_SMALL = 312798,
    SPELL_FLYING_KYRIAN_CHANNEL = 311375,     // CHANNELING_NO_BEAM
    SPELL_ATTACK_OF_THE_DARK_KYRIAN = 311595,     // PakageId  2743, SceneId 2386
    SPELL_DARK_KYRIAN_BEAM_CHANNEL = 313485,     // CHANNELING
    SPELL_KYRIAN_FLYING_FAKED_ATTACK = 352487,
    SPELL_LYSONIA_BEAM = 337035,
    SPELL_FLYING_KYRIAN_CHANNEL_OMNI_WITH_AREA = 322011,
    SPELL_FADE_TO_WHITE = 321242,
    SPELL_MAINTAIN_SUMMON = 325732,     // summon kleia, mikanikos
    SPELL_THROW_SLOBBER_BALL = 308783,
    SPELL_BALL = 308919,
    SPELL_ROOT_SELF = 355868,
    SPELL_GATAMATOS_IS_NEAR = 311135,    // Summon gatamatos
    SPELL_ANIMA_MISSILE_REVERSE = 310987,
    SPELL_PLACING = 311288,

    AURA_SHATTERED_SKY_SKYBOX = 328755,
    AURA_ANIMA_CANNISTER = 323931,
    AURA_RITURL_CLEANSING = 308754,
    AURA_COOLDOWN_AURS = 331712,
    AURA_EMPOWERED_AREA = 314751,
    AURA_CARRY_ASPIRANT_TRAY_WITH_SPOOL = 310632,
    AURA_CARRY_ANIMA_CANISTER = 313123,
    AURA_SUCCUMB_TO_DOUBT = 326834,
    AURA_GENERIC_LIFE_BURDEN_SHADER = 312726,
    AURA_PERMANENT_FEIGN_DEATH = 167503,
    AURA_BIG_DRUM = 326935,
    AURA_CANOE = 326929,
    AURA_COOKFIRE = 326932,
    AURA_BABY_TAUREN = 326944,
    AURA_LIFE_BURDEN_SHADER = 312861,
    AURA_TOUCH_OF_THE_MAW = 327433,
    AURA_EMPOWER_EXTRACTOR = 342661,
    AURA_MIKANIKOS_RUNE_DISPLAY = 325515,
    AURA_KNEEL_AND_USE = 123964,
    AURA_ANIMA_SHIELD = 325736,
    AURA_STAFF_COSMETIC = 308626,
    AURA_GATAMATOS_IS_NEAR2 = 347859,
    AURA_OBJECTIVE_PLACEMENT_VISUAL_STATE = 144373,
    AURA_NEAR_TRAINING_GEM = 323377,
};

enum Bastion_DoAction_Event
{
    ACTION_MOVE_SIKA = 100,
    ACTION_CLEANING_SAY_KLELA,
    ACTION_CLEANING_MOVE_KLELA,
    ACTION_KLEIAS_PERSPECTIVE,
    ACTION_ASPIRANT_COMPLETE,
    ACTION_SUMMON_ENEMY_NPC,
    ACTION_SOPHONE_START_PATH,
    ACTION_CARRY_CANISTER_START,
    ACTION_KYRIAN_FLY_ARRAY,
    ACTION_EMBER_COURT,
    ACTION_RINGING_VESPER_OF_PURITY,
    ACTION_RINGING_VESPER_OF_HARMONY,
    ACTION_ERIDIA_LYSONIA_DIALOG2,
    ACTION_ERIDIA_LYSONIA_DIALOG3,
    ACTION_ERIDIA_LYSONIA_DIALOG4,
    ACTION_THE_MNEMONIC_LOCUS,
    ACTION_WHATS_IN_A_MEMORY,
    ACTION_UNLOCK_CEREMONIAL_OFFRING_I,
    ACTION_UNLOCK_CEREMONIAL_OFFRING_II,
    ACTION_TEST_OF_COURAGE,

    EVENT_KALISTHENE_TO_GREETER = 200,
    EVENT_KALISTHENE_TO_KLELA,
    EVENT_HAMPERING_STRIKE,
    EVENT_ERIDIA_LYSONIA_DIALOG1,
    EVENT_SUMMON_MEMORY_FRAGMENT,
};

enum Bastion_Quest
{
    QUEST_SEEK_THE_ASCENDED = 59773,
    QUEST_WELCOME_TO_ETERNITY = 59774,
    QUEST_PARDON_OUR_DUST = 57102,
    QUEST_A_FATE_MOST_NOBLE = 57584,
    QUEST_TROUBLE_IN_PARADISE = 60735,
    QUEST_WALK_THE_PATH_ASPIR = 57261,
    QUEST_THINGS_THAT_HAUNT_US = 57676,
    QUEST_A_SOULBIND_IN_NEED = 57677,
    QUEST_THE_ASPIRANTS_CRUCIBLE = 57709,
    QUEST_A_LIFE_OF_SERVICE = 57710,
    QUEST_A_FORGE_GONE_COLD = 57711,
    QUEST_DROUGHT_CONDITIONS = 57265,
    QUEST_LIGHT_THE_FORGE_FORGELITE = 59920,
    QUEST_WORK_OF_ONES_HANDS = 57713,
    QUEST_THE_TRUE_CRUCIBLE_AWAITES = 57908,
    QUEST_ASSESSING_YOUR_STAMINA = 57909,
    QUEST_ASSESSING_YOUR_STRENGHT = 57288,
    QUEST_CHAMBER_OF_FIRST_REFLECTION = 57291,
    QUEST_THE_FIRST_CLEANSING = 57266,
    QUEST_ALL_AN_ASPIRANT_CAN_DO = 58174,
    QUEST_TEMPLE_OF_PURITY = 57270,
    QUEST_A_TEMPLE_IN_NEED = 57977,
    QUEST_ON_THE_EDGE_OF_A_REVELATION = 57264,
    QUEST_A_WAYWARD_DISCIPLE = 57716,
    QUEST_STEP_BACK_FROM_THAT_LEDGE_MY_FRIEND = 57717,
    QUEST_A_ONCE_SWEET_SOUND = 57037,
    QUEST_DANGEROUS_DISCOURSE = 57719,
    QUEST_THE_ENEMY_YOU_KNOW = 57446,
    QUEST_THE_HAND_OF_DOUBT = 57269,
    QUEST_PURITYS_PREROGATIVE = 57447,
    QUEST_CHASING_A_MEMORY = 58976,
    QUEST_DIRECTIONS_NOT_INCLUDED = 58771,
    QUEST_PRIMES_DIRECTIVE = 58799,
    QUEST_THE_MNEMONIC_LOCUS = 58800,
    QUEST_WHATS_IN_A_MEMORY = 58977,
    QUEST_LYSONIAS_TRUTH = 58978,
    QUEST_I_MADE_YOU = 58979,
    QUEST_MNEMIS_AT_YOUR_SERVICE = 58980,
    QUEST_THE_VAULT_OF_THE_ARCHON = 58843,
    QUEST_A_PARAGONS_REFLECTION = 60180,
    QUEST_LEAVE_IT_TO_MNEMIS = 60013,
    QUEST_DISTRACTIONS_FOR_KALA = 57545,
    QUEST_TOUGH_LOVE = 57568,
    QUEST_FRIENDLY_RIVALRY = 59674,
    QUEST_PHALYNX_MALFUNCTION = 57931,
    QUEST_WE_CAN_REBUILD_HIM = 57933,
    QUEST_COMBAT_DRILLS = 57934,
    QUEST_LASER_LOCATION = 57935,

    QUEST_HOSTILE_RECOLLECTION = 59015,
    QUEST_RSVP_SIKA = 61130,
    QUEST_WANTED_ALTERED_SENTINEL = 60316,
    QUEST_AMONG_THE_KYRIAN = 60491,
    QUEST_A_PROPER_RECEPTION = 60492,
    QUEST_ELYSIAN_HOLD = 57895,
    QUEST_OF_GREAT_RENOWN = 62789,
    QUEST_INTO_THE_RESERVOIR = 62792,
    QUEST_SETTING_THE_GROUND_RULES = 62882,
};

enum Bastion_Npc
{
    NPC_KLEIA_I = 165107,
    NPC_TRAVEL_WITH_KLEIA = 170415,
    NPC_SEE_BASTION_THROUGH_KLEIA_EYES = 170416,
    NPC_ASPIRING_SOUL = 160921,
    NPC_EXCRUCIATING_MEMORY = 158630,
    NPC_FRANTIC_STEWARD = 168245,
    NPC_ETERNAL_WATCHER = 159191,
};

Position const LightTheForgePath[3] =
{
    {-4574.17f, -4951.11f, 6529.59f, 0.33589f}, //0 sophone
    {-4568.77f, -4937.02f, 6530.31f, 5.07152f}, //1 Sika
    {-4576.11f, -4949.68f, 6529.59f, 4.78372f}, //2 forgehand
};

Position const TheWorkOfOnesHandsPos[8] =
{
    {-4572.11f, -4942.36f, 6529.67f, 0.71446f}, //0 forgehand_Pos1
    {-4571.01f, -4938.91f, 6530.12f, 2.05083f}, //1 forgehand_Pos2_setspell_313123 
    {-4568.17f, -4944.07f, 6529.54f, 5.32325f}, //2 forgehand_Pos3_removespel_l313123
    {-4572.15f, -4951.98f, 6529.58f, 1.28122f}, //3 forgehand_Pos4_Phase_10147
    {-4566.48f, -4946.78f, 6529.54f, 1.95477f}, //4 sophone_Pos1
    {-4574.17f, -4951.11f, 6529.59f, 0.33589f}, //5 sophone_Pos2_Phase_10147
    {-4569.57f, -4945.21f, 6529.67f, 5.98103f}, //6 Sika_Pos1
    {-4576.11f, -4949.68f, 6529.59f, 4.78372f}, //7 Sika_Pos2_Phase_10147
};

class bastion_player : public PlayerScript
{
public:
    bastion_player() : PlayerScript("bastion_player") { Initialize(); }
private:
    bool get_Locus_Manual;
    bool Obtain_the_first_offering;
    bool Obtain_the_second_offering;
    bool Gain_entry_the_vault;

    void Initialize()
    {
        get_Locus_Manual = false;
        Obtain_the_first_offering = false;
        Obtain_the_second_offering = false;
        Gain_entry_the_vault = false;
    }

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (!player->GetQuestObjectiveProgress(QUEST_DIRECTIONS_NOT_INCLUDED, 2))
            get_Locus_Manual = false;

        if (player->GetQuestStatus(QUEST_DIRECTIONS_NOT_INCLUDED) == QUEST_STATUS_COMPLETE &&
            player->GetQuestObjectiveProgress(QUEST_DIRECTIONS_NOT_INCLUDED, 2) == 1
            && player->GetAreaId() == 11486 && !get_Locus_Manual)
        {
            get_Locus_Manual = true;
            player->PlayConversation(13552);
        }

        if (!player->GetQuestObjectiveProgress(QUEST_MNEMIS_AT_YOUR_SERVICE, 1))
            Obtain_the_first_offering = false;

        if (player->GetQuestObjectiveProgress(QUEST_MNEMIS_AT_YOUR_SERVICE, 1) && !player->GetQuestObjectiveProgress(QUEST_MNEMIS_AT_YOUR_SERVICE, 2)
            && player->GetAreaId() == 12975 && !Obtain_the_first_offering)
        {
            Obtain_the_first_offering = true;
            Conversation::CreateConversation(14313, player, player->GetPosition(), { player->GetGUID() });
            player->GetScheduler().Schedule(5s, [player](TaskContext context)
                {
                    if (Creature* kleia = player->FindNearestCreature(167370, 15.f, true))
                        kleia->AI()->Talk(1);
                });
            player->GetScheduler().Schedule(9s, [player](TaskContext context)
                {
                    if (Creature* mikanikos = player->FindNearestCreature(167368, 15.f, true))
                        mikanikos->AI()->Talk(1);
                });
        }

        if (!player->GetQuestObjectiveProgress(QUEST_MNEMIS_AT_YOUR_SERVICE, 4))
            Obtain_the_second_offering = false;

        if (player->GetQuestObjectiveProgress(QUEST_MNEMIS_AT_YOUR_SERVICE, 4)
            && player->GetAreaId() == 12975 && !Obtain_the_second_offering)
        {
            Obtain_the_second_offering = true;
            Conversation::CreateConversation(14314, player, player->GetPosition(), { player->GetGUID() });
        }

        if (!player->GetQuestObjectiveProgress(QUEST_THE_VAULT_OF_THE_ARCHON, 1) && !player->GetQuestObjectiveProgress(QUEST_THE_VAULT_OF_THE_ARCHON, 2))
            Gain_entry_the_vault = false;

        if (!player->GetQuestObjectiveProgress(QUEST_THE_VAULT_OF_THE_ARCHON, 0) && player->GetQuestObjectiveProgress(QUEST_THE_VAULT_OF_THE_ARCHON, 1)
            && player->GetQuestObjectiveProgress(QUEST_THE_VAULT_OF_THE_ARCHON, 2)
            && player->GetAreaId() == 12975 && !Gain_entry_the_vault)
        {
            Gain_entry_the_vault = true;
            player->KilledMonsterCredit(167367);
            if (Creature* FX = player->FindNearestCreature(156279, 30.f, true))
                FX->RemoveAura(AURA_ANIMA_SHIELD);
            if (Creature* mikanikos = player->FindNearestCreature(167498, 15.f, true))
                mikanikos->AI()->SetData(1, 1);
        }
        if (!player->GetQuestObjectiveProgress(QUEST_A_PARAGONS_REFLECTION, 0) && player->GetQuestObjectiveProgress(QUEST_A_PARAGONS_REFLECTION, 2)
            && player->GetQuestObjectiveProgress(QUEST_A_PARAGONS_REFLECTION, 3) && player->GetQuestObjectiveProgress(QUEST_A_PARAGONS_REFLECTION, 4)
            && player->GetQuestObjectiveProgress(QUEST_A_PARAGONS_REFLECTION, 5) && player->GetQuestObjectiveProgress(QUEST_A_PARAGONS_REFLECTION, 6)
            && player->GetAreaId() == 13391)
        {
            player->KilledMonsterCredit(167514);
            if (Creature* archon = player->FindNearestCreature(167514, 40.f, true))
                archon->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        }
        if (player->GetAreaId() == 11486 || player->GetAreaId() == 13391)       // The Mnemonic Locus
        {
            if (player->GetQuestStatus(QUEST_LEAVE_IT_TO_MNEMIS) == QUEST_STATUS_COMPLETE)
            {
                if (!player->GetPhaseShift().HasPhase(10156)) PhasingHandler::AddPhase(player, 10156, true);
                if (player->GetPhaseShift().HasPhase(10153)) PhasingHandler::RemovePhase(player, 10153, true);
                if (player->GetPhaseShift().HasPhase(10155)) PhasingHandler::RemovePhase(player, 10155, true);
            }
        }
    };


    void OnLogin(Player* player, bool firstLogin) override
    {
        if (player->HasAura(347859) || player->HasAura(311157) || player->HasAura(322277))   //Gatamatos is Near
        {
            player->RemoveAura(347859);
            player->RemoveAura(311157);
            player->RemoveAura(322277);
        }
    }

    void OnUpdateZone(Player* player, uint32 /*newzone*/, uint32 /*oldzone*/) override
    {
        player->RemoveAura(AURA_SHATTERED_SKY_SKYBOX);
        if (!player->GetPhaseShift().HasPhase(PHASE_GENERIC)) PhasingHandler::AddPhase(player, PHASE_GENERIC, true);
    }

    void OnQuestStatusChange(Player* player, uint32 questId) override
    {
        if (player->GetQuestStatus(QUEST_A_LIFE_OF_SERVICE) == QUEST_STATUS_COMPLETE)
        {
            player->GetScheduler().Schedule(5s, [player](TaskContext context)
                {
                    player->CastSpell(player, SPELL_LEAVE_AREA_TRIGGER);
                });
        }
        if (player->GetQuestStatus(QUEST_DROUGHT_CONDITIONS) == QUEST_STATUS_COMPLETE)
        {
            if (GameObject* extractor = player->FindNearestGameObject(349702, 20.f, false))
            {
                extractor->SetGoState(GO_STATE_READY);
                extractor->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
            }
        }
        if (player->GetQuestStatus(QUEST_ASSESSING_YOUR_STAMINA) == QUEST_STATUS_COMPLETE
            || player->GetQuestStatus(QUEST_ASSESSING_YOUR_STRENGHT) == QUEST_STATUS_COMPLETE)
        {
            if (!player->GetPhaseShift().HasPhase(10148)) PhasingHandler::AddPhase(player, 10148, true);
            if (player->GetPhaseShift().HasPhase(10147)) PhasingHandler::RemovePhase(player, 10147, true);
        }
        if (player->GetQuestStatus(QUEST_LIGHT_THE_FORGE_FORGELITE) == QUEST_STATUS_COMPLETE)
        {
            if (Creature* sophone = player->FindNearestCreature(159473, 20.f, true))
                if (Creature* sika = player->FindNearestCreature(166742, 20.f, true))
                    if (Creature* forgehand = player->FindNearestCreature(159337, 20.f, true))
                    {
                        sophone->AI()->Talk(4);
                        sophone->SetWalk(true); sika->SetWalk(true); forgehand->SetWalk(true);
                        sophone->DespawnOrUnsummon(10s, 1min); sika->DespawnOrUnsummon(10s, 1min); forgehand->DespawnOrUnsummon(10s, 1min);
                        sophone->GetMotionMaster()->MovePoint(0, LightTheForgePath[0], false, 0.33589f);
                        sika->GetMotionMaster()->MovePoint(0, LightTheForgePath[1], false, 5.07152f);
                        forgehand->GetMotionMaster()->MovePoint(0, LightTheForgePath[2], false, 4.78372f);
                    }
            player->GetScheduler().Schedule(8s, [player](TaskContext context)
                {
                    if (!player->GetPhaseShift().HasPhase(10146)) PhasingHandler::AddPhase(player, 10146, true);
                    if (player->GetPhaseShift().HasPhase(10145)) PhasingHandler::RemovePhase(player, 10145, true);
                });
        }
        if (player->GetQuestStatus(QUEST_WORK_OF_ONES_HANDS) == QUEST_STATUS_COMPLETE)
        {
            if (Creature* sophone = player->FindNearestCreature(158807, 20.f, true))
                if (Creature* sika = player->FindNearestCreature(166742, 20.f, true))
                    if (Creature* forgehand = player->FindNearestCreature(159337, 20.f, true))
                        if (Creature* armor = player->FindNearestCreature(159444, 20.f, true))
                        {
                            forgehand->RemoveAura(AURA_CARRY_ANIMA_CANISTER);
                            forgehand->GetMotionMaster()->Clear();
                            sophone->AI()->Talk(9);
                            sophone->GetMotionMaster()->MovePoint(0, TheWorkOfOnesHandsPos[5], false, 0.33589f);
                            sika->GetMotionMaster()->MovePoint(0, TheWorkOfOnesHandsPos[7], false, 5.07152f);
                            forgehand->GetMotionMaster()->MovePoint(0, TheWorkOfOnesHandsPos[3], false, 1.28122f);
                            forgehand->ForcedDespawn(5000); sophone->ForcedDespawn(5000); sophone->ForcedDespawn(5000); armor->ForcedDespawn();
                            player->GetScheduler().Schedule(5s, [player](TaskContext context)
                                {
                                    if (!player->GetPhaseShift().HasPhase(10147)) PhasingHandler::AddPhase(player, 10147, true);
                                    if (player->GetPhaseShift().HasPhase(10146)) PhasingHandler::RemovePhase(player, 10146, true);
                                    if (player->GetPhaseShift().HasPhase(10145)) PhasingHandler::RemovePhase(player, 10145, true);
                                });

                        }
        }
        if (player->GetQuestStatus(QUEST_ALL_AN_ASPIRANT_CAN_DO) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* sophone = player->FindNearestCreature(158807, 10.f, true))
                sophone->GetMotionMaster()->MovePath(15880701, false);
        }
        if (player->GetQuestStatus(QUEST_DIRECTIONS_NOT_INCLUDED) == QUEST_STATUS_COMPLETE
            || player->GetQuestStatus(QUEST_PRIMES_DIRECTIVE) == QUEST_STATUS_COMPLETE)
        {
            if (!player->GetPhaseShift().HasPhase(10152)) PhasingHandler::AddPhase(player, 10152, true);
            if (player->GetPhaseShift().HasPhase(10151)) PhasingHandler::RemovePhase(player, 10151, true);
        }
        if (player->GetQuestStatus(QUEST_LYSONIAS_TRUTH) == QUEST_STATUS_COMPLETE)
        {
            if (!player->GetPhaseShift().HasPhase(10154)) PhasingHandler::AddPhase(player, 10154, true);
            player->GetScheduler().Schedule(17s, [player](TaskContext context)
                {
                    if (Creature* LKM = player->FindNearestCreature(167036, 30.f, true))
                        LKM->AI()->Talk(4);
                });
        }
        if (player->GetQuestStatus(QUEST_MNEMIS_AT_YOUR_SERVICE) == QUEST_STATUS_REWARDED)
        {
            player->RemoveAura(SPELL_MAINTAIN_SUMMON);
            if (Creature* mikanikos = player->FindNearestCreature(167368, 15.f, true))
                if (Creature* kleia = player->FindNearestCreature(167370, 15.f, true))
                {
                    mikanikos->ForcedDespawn();
                    kleia->ForcedDespawn();
                }
            if (!player->GetPhaseShift().HasPhase(10154)) PhasingHandler::AddPhase(player, 10154, true);
        }
        if (player->GetQuestStatus(QUEST_THE_VAULT_OF_THE_ARCHON) == QUEST_STATUS_REWARDED)
        {
            if (!player->GetPhaseShift().HasPhase(10155)) PhasingHandler::AddPhase(player, 10155, true);
            if (player->GetPhaseShift().HasPhase(10154)) PhasingHandler::RemovePhase(player, 10154, true);
        }
        if (player->GetQuestStatus(QUEST_DISTRACTIONS_FOR_KALA) == QUEST_STATUS_REWARDED)
        {
            if (!player->GetPhaseShift().HasPhase(10162)) PhasingHandler::AddPhase(player, 10162, true);
            if (player->GetPhaseShift().HasPhase(10161)) PhasingHandler::RemovePhase(player, 10161, true);
        }
        if (player->GetQuestStatus(QUEST_TOUGH_LOVE) == QUEST_STATUS_REWARDED)
        {
            if (!player->GetPhaseShift().HasPhase(10163)) PhasingHandler::AddPhase(player, 10163, true);
            if (player->GetPhaseShift().HasPhase(10162)) PhasingHandler::RemovePhase(player, 10162, true);
        }
        if (player->GetQuestStatus(QUEST_PHALYNX_MALFUNCTION) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_WE_CAN_REBUILD_HIM) == QUEST_STATUS_COMPLETE)
        {
            if (!player->GetPhaseShift().HasPhase(10173)) PhasingHandler::AddPhase(player, 10173, true);
            if (player->GetPhaseShift().HasPhase(10171)) PhasingHandler::RemovePhase(player, 10171, true);
        }
        OnCheckPhase(player);
    }

    void OnUpdateArea(Player* player, uint32 /*newArea*/, uint32 /*oldArea*/) override
    {
        if (player->GetAreaId() == 11377)       // Aspirant's Crucible
        {
            if (player->GetQuestStatus(QUEST_A_SOULBIND_IN_NEED) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10141)) PhasingHandler::AddPhase(player, 10141, true);
            }
            if (player->GetQuestStatus(QUEST_A_LIFE_OF_SERVICE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10142)) PhasingHandler::AddPhase(player, 10142, true);
                if (player->GetPhaseShift().HasPhase(10141)) PhasingHandler::RemovePhase(player, 10141, true);
            }
            if (player->GetQuestStatus(QUEST_A_FORGE_GONE_COLD) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10144)) PhasingHandler::AddPhase(player, 10144, true);
                if (player->GetPhaseShift().HasPhase(10142)) PhasingHandler::RemovePhase(player, 10142, true);
                if (player->GetPhaseShift().HasPhase(10141)) PhasingHandler::RemovePhase(player, 10141, true);
            }
            if (player->GetQuestStatus(QUEST_LIGHT_THE_FORGE_FORGELITE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10146)) PhasingHandler::AddPhase(player, 10146, true);
                if (player->GetPhaseShift().HasPhase(10145)) PhasingHandler::RemovePhase(player, 10145, true);
                if (player->GetPhaseShift().HasPhase(10144)) PhasingHandler::RemovePhase(player, 10144, true);
                if (player->GetPhaseShift().HasPhase(10142)) PhasingHandler::RemovePhase(player, 10142, true);
            }
            if (player->GetQuestStatus(QUEST_WORK_OF_ONES_HANDS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10147)) PhasingHandler::AddPhase(player, 10147, true);
                if (player->GetPhaseShift().HasPhase(10146)) PhasingHandler::RemovePhase(player, 10146, true);
                if (player->GetPhaseShift().HasPhase(10145)) PhasingHandler::RemovePhase(player, 10145, true);
                if (player->GetPhaseShift().HasPhase(10144)) PhasingHandler::RemovePhase(player, 10144, true);
            }
            if (player->GetQuestStatus(QUEST_ASSESSING_YOUR_STAMINA) == QUEST_STATUS_REWARDED
                || player->GetQuestStatus(QUEST_ASSESSING_YOUR_STRENGHT) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10148)) PhasingHandler::AddPhase(player, 10148, true);
                if (player->GetPhaseShift().HasPhase(10147)) PhasingHandler::RemovePhase(player, 10147, true);
                if (player->GetPhaseShift().HasPhase(10146)) PhasingHandler::RemovePhase(player, 10146, true);
                if (player->GetPhaseShift().HasPhase(10145)) PhasingHandler::RemovePhase(player, 10145, true);
                if (player->GetPhaseShift().HasPhase(10144)) PhasingHandler::RemovePhase(player, 10144, true);
            }
            if (player->GetQuestStatus(QUEST_THE_FIRST_CLEANSING) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_THE_FIRST_CLEANSING) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10149)) PhasingHandler::AddPhase(player, 10149, true);
                if (player->GetPhaseShift().HasPhase(10147)) PhasingHandler::RemovePhase(player, 10147, true);
                if (player->GetPhaseShift().HasPhase(10146)) PhasingHandler::RemovePhase(player, 10146, true);
                if (player->GetPhaseShift().HasPhase(10145)) PhasingHandler::RemovePhase(player, 10145, true);
                if (player->GetPhaseShift().HasPhase(10144)) PhasingHandler::RemovePhase(player, 10144, true);
            }
        }
        if (player->GetAreaId() == 11474)       // Chamber of First Reflection
        {
            if (player->GetQuestStatus(QUEST_ASSESSING_YOUR_STAMINA) == QUEST_STATUS_REWARDED
                || player->GetQuestStatus(QUEST_ASSESSING_YOUR_STRENGHT) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10148)) PhasingHandler::AddPhase(player, 10148, true);
            }
        }
        if (player->GetAreaId() == 13415)       // Aspirant's Awakening
        {
            if (player->GetQuestStatus(QUEST_THE_FIRST_CLEANSING) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_THE_FIRST_CLEANSING) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10149)) PhasingHandler::AddPhase(player, 10149, true);
                if (player->GetPhaseShift().HasPhase(10148)) PhasingHandler::RemovePhase(player, 10148, true);
            }
        }
        if (player->GetAreaId() == 11509)       // Purity's Reflection
        {
            if (player->GetQuestStatus(QUEST_A_SOULBIND_IN_NEED) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10131)) PhasingHandler::AddPhase(player, 10131, true);
            }
            if (player->GetQuestStatus(QUEST_ALL_AN_ASPIRANT_CAN_DO) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10132)) PhasingHandler::AddPhase(player, 10132, true);
                if (player->GetPhaseShift().HasPhase(10131)) PhasingHandler::RemovePhase(player, 10131, true);
            }
            if (player->GetQuestStatus(QUEST_THE_HAND_OF_DOUBT) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10131)) PhasingHandler::AddPhase(player, 10131, true);
                if (!player->GetPhaseShift().HasPhase(10136)) PhasingHandler::AddPhase(player, 10136, true);
                if (player->GetPhaseShift().HasPhase(10132)) PhasingHandler::RemovePhase(player, 10132, true);
            }
        }
        if (player->GetAreaId() == 11014)       // The Temple of Purity
        {
            if (player->GetQuestStatus(QUEST_A_SOULBIND_IN_NEED) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10131)) PhasingHandler::AddPhase(player, 10131, true);
            }
            if (player->GetQuestStatus(QUEST_ALL_AN_ASPIRANT_CAN_DO) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10132)) PhasingHandler::AddPhase(player, 10132, true);
                if (player->GetPhaseShift().HasPhase(10131)) PhasingHandler::RemovePhase(player, 10131, true);
            }
            if (player->GetQuestStatus(QUEST_A_TEMPLE_IN_NEED) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->FindNearestCreature(165110, 100.f, true) && !player->FindNearestCreature(164335, 100.f, true))  //summon_pelagos
                    player->CastSpell(player, SPELL_SUMMON_PELAGOS, true);
            }
            if (player->GetQuestStatus(QUEST_STEP_BACK_FROM_THAT_LEDGE_MY_FRIEND) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10133)) PhasingHandler::AddPhase(player, 10133, true);
                if (!player->GetPhaseShift().HasPhase(10134)) PhasingHandler::AddPhase(player, 10134, true);
                if (!player->GetPhaseShift().HasPhase(10135)) PhasingHandler::AddPhase(player, 10135, true);
                if (player->GetPhaseShift().HasPhase(10132)) PhasingHandler::RemovePhase(player, 10132, true);
            }
            if (player->GetQuestStatus(QUEST_DANGEROUS_DISCOURSE) == QUEST_STATUS_COMPLETE ||
                player->GetQuestStatus(QUEST_DANGEROUS_DISCOURSE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10134)) PhasingHandler::AddPhase(player, 10134, true);
                if (!player->GetPhaseShift().HasPhase(10135)) PhasingHandler::AddPhase(player, 10135, true);
                if (player->GetPhaseShift().HasPhase(10132)) PhasingHandler::RemovePhase(player, 10132, true);
                if (player->GetPhaseShift().HasPhase(10133)) PhasingHandler::RemovePhase(player, 10133, true);
            }
            if (player->GetQuestStatus(QUEST_THE_HAND_OF_DOUBT) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10131)) PhasingHandler::AddPhase(player, 10131, true);
                if (!player->GetPhaseShift().HasPhase(10134)) PhasingHandler::AddPhase(player, 10134, true);
                if (!player->GetPhaseShift().HasPhase(10136)) PhasingHandler::AddPhase(player, 10136, true);
                if (player->GetPhaseShift().HasPhase(10132)) PhasingHandler::RemovePhase(player, 10132, true);
                if (player->GetPhaseShift().HasPhase(10133)) PhasingHandler::RemovePhase(player, 10133, true);
            }
        }
        if (player->GetAreaId() == 11521)       // Purity's Pinnacle
        {
            if (player->GetQuestStatus(QUEST_STEP_BACK_FROM_THAT_LEDGE_MY_FRIEND) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10134)) PhasingHandler::AddPhase(player, 10134, true);
                if (!player->GetPhaseShift().HasPhase(10135)) PhasingHandler::AddPhase(player, 10135, true);
                if (player->GetPhaseShift().HasPhase(10132)) PhasingHandler::RemovePhase(player, 10132, true);
            }
            if (player->GetQuestStatus(QUEST_THE_HAND_OF_DOUBT) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10134)) PhasingHandler::AddPhase(player, 10134, true);
                if (!player->GetPhaseShift().HasPhase(10136)) PhasingHandler::AddPhase(player, 10136, true);
                if (player->GetPhaseShift().HasPhase(10132)) PhasingHandler::RemovePhase(player, 10132, true);
                if (player->GetPhaseShift().HasPhase(10135)) PhasingHandler::RemovePhase(player, 10135, true);
            }
        }
        if (player->GetAreaId() == 11486)       // The Mnemonic Locus
        {
            if (player->GetQuestStatus(QUEST_PURITYS_PREROGATIVE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10151)) PhasingHandler::AddPhase(player, 10151, true);
            }
            if (player->GetQuestStatus(QUEST_CHASING_A_MEMORY) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10152)) PhasingHandler::AddPhase(player, 10152, true);
                if (player->GetPhaseShift().HasPhase(10151)) PhasingHandler::RemovePhase(player, 10151, true);
            }
            if (player->GetQuestStatus(QUEST_THE_MNEMONIC_LOCUS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10153)) PhasingHandler::AddPhase(player, 10153, true);
                if (player->GetPhaseShift().HasPhase(10151)) PhasingHandler::RemovePhase(player, 10151, true);
                if (player->GetPhaseShift().HasPhase(10152)) PhasingHandler::RemovePhase(player, 10152, true);
                if (player->GetPhaseShift().HasPhase(10157)) PhasingHandler::RemovePhase(player, 10157, true);
            }
            if (player->GetQuestStatus(QUEST_LEAVE_IT_TO_MNEMIS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10156)) PhasingHandler::AddPhase(player, 10156, true);
                if (player->GetPhaseShift().HasPhase(10153)) PhasingHandler::RemovePhase(player, 10153, true);
            }
        }
        if (player->GetAreaId() == 12975)       // The Walk of Recollection
        {
            if (player->GetQuestStatus(QUEST_WHATS_IN_A_MEMORY) == QUEST_STATUS_REWARDED
                && player->GetQuestStatus(QUEST_HOSTILE_RECOLLECTION) == QUEST_STATUS_NONE)
            {
                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_HOSTILE_RECOLLECTION))
                    player->AddQuest(quest, nullptr);
            }
            if (player->GetQuestStatus(QUEST_MNEMIS_AT_YOUR_SERVICE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10154)) PhasingHandler::AddPhase(player, 10154, true);
            }
        }
        if (player->GetAreaId() == 13390)       // Vault of Memory
        {
            if (player->GetQuestStatus(QUEST_LYSONIAS_TRUTH) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_LYSONIAS_TRUTH) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10154)) PhasingHandler::AddPhase(player, 10154, true);
            }
            if (player->GetQuestStatus(QUEST_I_MADE_YOU) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10154)) PhasingHandler::RemovePhase(player, 10154, true);
                if (!player->GetPhaseShift().HasPhase(10155)) PhasingHandler::AddPhase(player, 10155, true);
            }
        }
        if (player->GetAreaId() == 13391)       // Vault of Archon
        {
            if (player->GetQuestStatus(QUEST_MNEMIS_AT_YOUR_SERVICE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10154)) PhasingHandler::AddPhase(player, 10154, true);
            }
            if (player->GetQuestStatus(QUEST_THE_VAULT_OF_THE_ARCHON) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10155)) PhasingHandler::AddPhase(player, 10155, true);
                if (player->GetPhaseShift().HasPhase(10154)) PhasingHandler::RemovePhase(player, 10154, true);
            }
            if (player->GetQuestStatus(QUEST_THE_VAULT_OF_THE_ARCHON) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10155)) PhasingHandler::AddPhase(player, 10155, true);
                if (player->GetPhaseShift().HasPhase(10154)) PhasingHandler::RemovePhase(player, 10154, true);
            }
            if (player->GetQuestStatus(QUEST_LEAVE_IT_TO_MNEMIS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10156)) PhasingHandler::AddPhase(player, 10156, true);
                if (player->GetPhaseShift().HasPhase(10155)) PhasingHandler::RemovePhase(player, 10155, true);
            }
        }
        if (player->GetAreaId() == 11487)       // GarDen of Respite
        {
            if (player->GetQuestStatus(QUEST_LEAVE_IT_TO_MNEMIS) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10161)) PhasingHandler::AddPhase(player, 10161, true);
            }
            if (player->GetQuestStatus(QUEST_DISTRACTIONS_FOR_KALA) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10162)) PhasingHandler::AddPhase(player, 10162, true);
                if (player->GetPhaseShift().HasPhase(10161)) PhasingHandler::RemovePhase(player, 10161, true);
            }
            if (player->GetQuestStatus(QUEST_TOUGH_LOVE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10163)) PhasingHandler::AddPhase(player, 10163, true);
                if (player->GetPhaseShift().HasPhase(10162)) PhasingHandler::RemovePhase(player, 10162, true);
            }
        }
        if (player->GetAreaId() == 12793)       // Forgefire Outpost
        {
            if (player->GetQuestStatus(QUEST_FRIENDLY_RIVALRY) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10171)) PhasingHandler::AddPhase(player, 10171, true);
                if (player->GetPhaseShift().HasPhase(10163)) PhasingHandler::RemovePhase(player, 10163, true);
            }
            if (player->GetQuestStatus(QUEST_PHALYNX_MALFUNCTION) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_WE_CAN_REBUILD_HIM) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10173)) PhasingHandler::AddPhase(player, 10173, true);
                if (player->GetPhaseShift().HasPhase(10171)) PhasingHandler::RemovePhase(player, 10171, true);
            }
        }
        if (!player->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(player, 169, true);
        OnCheckPhase(player);
    }

    void OnCheckPhase(Player* player)
    {
        if (player->GetAreaId() == 11013)       // Vestibule Of Eternity
        {
            if (player->HasQuest(QUEST_WELCOME_TO_ETERNITY) || player->GetQuestStatus(QUEST_WELCOME_TO_ETERNITY) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10105)) PhasingHandler::AddPhase(player, 10105, true);
            }
            if (player->GetQuestStatus(QUEST_PARDON_OUR_DUST) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10105)) PhasingHandler::RemovePhase(player, 10105, true);
                if (!player->GetPhaseShift().HasPhase(10110)) PhasingHandler::AddPhase(player, 10110, true);
            }
            if (player->GetQuestStatus(QUEST_TROUBLE_IN_PARADISE) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_TROUBLE_IN_PARADISE) == QUEST_STATUS_REWARDED)
            {
                if (player->GetPhaseShift().HasPhase(10110)) PhasingHandler::RemovePhase(player, 10110, true);
                if (!player->GetPhaseShift().HasPhase(10115)) PhasingHandler::AddPhase(player, 10115, true);
            }
        }
        if (player->GetAreaId() == 11473)       // Aspirant's Rest
        {
            if (player->GetQuestStatus(QUEST_TROUBLE_IN_PARADISE) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10119)) PhasingHandler::AddPhase(player, 10119, true);
            }
            if (player->GetQuestStatus(QUEST_A_SOULBIND_IN_NEED) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_A_SOULBIND_IN_NEED) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10120)) PhasingHandler::AddPhase(player, 10120, true);
                if (player->GetPhaseShift().HasPhase(10119)) PhasingHandler::RemovePhase(player, 10119, true);
            }
            if (player->GetQuestStatus(QUEST_THE_FIRST_CLEANSING) == QUEST_STATUS_REWARDED)
            {
                if (!player->GetPhaseShift().HasPhase(10149)) PhasingHandler::AddPhase(player, 10149, true);
                if (player->GetPhaseShift().HasPhase(10120)) PhasingHandler::RemovePhase(player, 10120, true);
            }
            if (player->GetPhaseShift().HasPhase(169)) PhasingHandler::RemovePhase(player, 169, true);
        }
    }
};

class scene_bastion : public SceneScript
{
public:
    scene_bastion() : SceneScript("scene_bastion") { }

    void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 2743)
            player->NearTeleportTo(-4285.94f, -5381.01f, 6514.8f, 0.882522f);
    }

    // Called when a scene is canceled
    void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 2743)
            AttackoftheDarkKyrianFinish(player);
    }

    // Called when a scene is completed
    void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* sceneTemplate) override
    {
        if (sceneTemplate->ScenePackageId == 2743)
            AttackoftheDarkKyrianFinish(player);
    }

    void AttackoftheDarkKyrianFinish(Player* player)
    {
        if (!player->GetPhaseShift().HasPhase(10133)) PhasingHandler::AddPhase(player, 10133, true);
        if (!player->GetPhaseShift().HasPhase(10134)) PhasingHandler::AddPhase(player, 10134, true);
        if (!player->GetPhaseShift().HasPhase(10135)) PhasingHandler::AddPhase(player, 10135, true);
        if (player->GetPhaseShift().HasPhase(10132)) PhasingHandler::RemovePhase(player, 10132, true);
        player->GetScheduler().Schedule(2s, [player](TaskContext context)
            {
                player->PlayConversation(13540);
            });
        player->GetScheduler().Schedule(7s, [player](TaskContext context)
            {
                if (Creature* eridia = player->FindNearestCreature(159762, 15.f, true))
                    eridia->AI()->Talk(6);
            });
    }

};

// npc_Kleia_166227_q59773
struct npc_Kleia_166227 : public EscortAI
{
    npc_Kleia_166227(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    bool say;
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        say = false;
        m_playerGUID.Clear();
        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
            {
                if (player->HasQuest(QUEST_SEEK_THE_ASCENDED))
                    if (!say)
                    {
                        say = true;
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        Talk(0);
                        me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    }
                if (player->GetQuestStatus(QUEST_WELCOME_TO_ETERNITY) == QUEST_STATUS_NONE)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                }
            }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_WELCOME_TO_ETERNITY)
        {
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_SEEK_THE_ASCENDED)
            say = false;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:
            m_playerGUID = player->GetGUID();
            player->KilledMonsterCredit(166227);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            LoadPath(1329816);
            Start(true);
            break;
        }
        return true;
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
        case 2:
            Talk(1);
            break;
        case 5:
            Talk(2);
            break;
        case 9:
            Talk(3);
            break;
        case 15:
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                player->KilledMonsterCredit(NPC_KLEIA_I);
                PhasingHandler::AddPhase(player, 10105, true);
                me->DespawnOrUnsummon(1s, 30s);
            }
            break;
        }
    }
};

//Quest 60735 Ascended Champion 159704
Position const ascended_champion1[3] =
{
    {-4392.51f, -4248.98f, 6575.12f, 0},
    {-4367.54f, -4248.37f, 6568.04f, 0},
    {-4357.34f, -4244.54f, 6560.73f, 0.532435f},
};
Position const ascended_champion2[3] =
{
    {-4388.19f, -4278.28f, 6589.95f, 0},
    {-4369.84f, -4264.53f, 6579.5f, 0},
    {-4353.07f, -4251.79f, 6564.06f, 0.951156f},
};
Position const ascended_champion3[3] =
{
    {-4305.94f, -4293.18f, 6592.71f, 0},
    {-4333.83f, -4264.48f, 6568.27f, 0},
    {-4342.69f, -4254.49f, 6564.07f, 1.70402f},
};
Position const ascended_champion4[3] =
{
    {-4304.66f, -4260.38f, 6578.03f, 0},
    {-4325.16f, -4255.44f, 6564.62f, 0},
    {-4334.93f, -4250.89f, 6560.73f, 2.09742f},
};

//Quest 60735 Kalisthene 166306
Position const kalisthene[3] =
{
    {-4352.85f, -4271.62f, 6597.42f, 0},
    {-4349.18f, -4256.52f, 6573.03f, 0},
    {-4345.83f, -4244.14f, 6558.74f, 1.2218f},
};

// npc_Kleia_165107_q57102
struct npc_Kleia_165107 : public ScriptedAI
{
    npc_Kleia_165107(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    ObjectGuid kalistheneGUID;
    ObjectGuid   m_playerGUID;
    EventMap _events;
    bool save_pelagos;

    void Reset() override
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
        kalistheneGUID.Clear();
        m_playerGUID = ObjectGuid::Empty;
        save_pelagos = false;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_PARDON_OUR_DUST)
        {
            Talk(0);
            player->GetScheduler().Schedule(Milliseconds(5000), [this, player](TaskContext context)
                {
                    if (Creature* sika = player->FindNearestCreature(166124, 40.0f, true))
                        sika->AI()->Talk(0);
                });
        }
        if (quest->GetQuestId() == QUEST_TROUBLE_IN_PARADISE)
        {
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        }
        if (player->HasQuest(QUEST_THINGS_THAT_HAUNT_US) && player->HasQuest(QUEST_A_SOULBIND_IN_NEED))
        {
            Talk(7);
        }
        if (player->HasQuest(QUEST_THE_ASPIRANTS_CRUCIBLE))
        {
            Talk(9);
            me->GetMotionMaster()->MovePath(16510701, false);
            if (Creature* pelagos = player->FindNearestCreature(165112, 15.f, true))
            {
                pelagos->GetMotionMaster()->MovePath(16511201, false);
                me->AddDelayedEvent(8000, [pelagos]()  ->void { pelagos->AI()->Talk(1); });
                me->AddDelayedEvent(15000, [this]()    ->void { Talk(10); });
            }
        }
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (player->GetQuestStatus(QUEST_TROUBLE_IN_PARADISE) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GossipOptionNpc::None, "<Tell Kleia what you saw in the Maw.>", 25436, GOSSIP_ACTION_INFO_DEF + 0);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 0:
            m_playerGUID = player->GetGUID();
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            Talk(2);
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(0, -4343.8f, -4237.99f, 6555.86f, true, 4.40108f);
            KalistheneFlyAway(player);
            player->GetScheduler().Schedule(4s, [this, player](TaskContext context)
                {
                    if (Creature* kalisth = ObjectAccessor::GetCreature(*me, kalistheneGUID))
                        kalisth->AI()->Talk(0);
                    player->KilledMonsterCredit(NPC_KLEIA_I);
                    _events.ScheduleEvent(EVENT_KALISTHENE_TO_GREETER, 3s);
                });
            break;
        }
        return true;
    }

    void KalistheneFlyAway(Player* player)
    {
        if (Creature* ascend1 = player->SummonCreature(159704, ascended_champion1[0], TEMPSUMMON_TIMED_DESPAWN, 120s))
            ascend1->GetMotionMaster()->MoveSmoothPath(0, ascended_champion1, 3, false, true);
        if (Creature* ascend2 = player->SummonCreature(159704, ascended_champion2[0], TEMPSUMMON_TIMED_DESPAWN, 120s))
            ascend2->GetMotionMaster()->MoveSmoothPath(0, ascended_champion2, 3, false, true);
        if (Creature* ascend3 = player->SummonCreature(159704, ascended_champion3[0], TEMPSUMMON_TIMED_DESPAWN, 120s))
            ascend3->GetMotionMaster()->MoveSmoothPath(0, ascended_champion3, 3, false, true);
        if (Creature* ascend4 = player->SummonCreature(159704, ascended_champion4[0], TEMPSUMMON_TIMED_DESPAWN, 120s))
            ascend4->GetMotionMaster()->MoveSmoothPath(0, ascended_champion4, 3, false, true);
        if (Creature* kalisth = player->SummonCreature(166306, kalisthene[0], TEMPSUMMON_TIMED_DESPAWN, 120s))
        {
            kalistheneGUID = kalisth->GetGUID();
            kalisth->GetMotionMaster()->MoveSmoothPath(0, kalisthene, 3, false, true);
        }

    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_KALISTHENE_TO_GREETER:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (Creature* kalisth = ObjectAccessor::GetCreature(*me, kalistheneGUID))
                    {
                        Talk(3, player);
                        player->GetScheduler().Schedule(7s, [this, kalisth](TaskContext context)
                            {
                                kalisth->AI()->Talk(1);
                            }).Schedule(10s, [this, kalisth](TaskContext context)
                                {
                                    if (Creature* greeter = me->FindNearestCreature(158281, 20.0f, true))
                                        kalisth->CastSpell(greeter, 332985);
                                }).Schedule(13s, [this](TaskContext context)
                                    {
                                        if (Creature* greeter = me->FindNearestCreature(158281, 20.0f, true))
                                        {
                                            greeter->AI()->Talk(3);
                                            greeter->SetAIAnimKitId(18797);
                                        }
                                        _events.ScheduleEvent(EVENT_KALISTHENE_TO_KLELA, 2s);
                                    });
                    }
                break;
            case EVENT_KALISTHENE_TO_KLELA:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (Creature* kalisth = ObjectAccessor::GetCreature(*me, kalistheneGUID))
                    {
                        Talk(4);
                        player->GetScheduler().Schedule(8s, [this, kalisth](TaskContext context)
                            {
                                kalisth->AI()->Talk(2);
                            }).Schedule(15s, [this](TaskContext context)
                                {
                                    Talk(5);
                                }).Schedule(22s, [player, kalisth](TaskContext context)
                                    {
                                        player->KilledMonsterCredit(166306);
                                        kalisth->AI()->Talk(3);
                                    }).Schedule(30s, [this, player, kalisth](TaskContext context)
                                        {
                                            std::list<Creature*> cList = me->FindNearestCreatures(159704, 30.0f);
                                            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                                            {
                                                if (Creature* ascend = *itr)
                                                {
                                                    if (!ascend->GetPhaseShift().HasPhase(10115))
                                                        ascend->ForcedDespawn(1000);
                                                }
                                            }
                                            kalisth->ForcedDespawn(1000);
                                            me->DespawnOrUnsummon(3s, 10s);
                                        });
                    }
                break;
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_ASPIRANT_COMPLETE)
        {
            if (Creature* defender = me->FindNearestCreature(161065, 25.0f, true))
                defender->AI()->Talk(0);
            me->GetScheduler().Schedule(7s, [this](TaskContext context) { Talk(6); });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 15.0f))
            {
                if (player->GetQuestStatus(QUEST_A_SOULBIND_IN_NEED) == QUEST_STATUS_COMPLETE && !save_pelagos)
                {
                    save_pelagos = true;
                    Talk(8);
                    player->GetScheduler().Schedule(Milliseconds(7000), [this, player](TaskContext context)
                        {
                            if (Creature* pelagos = player->FindNearestCreature(165112, 10.0f, true))
                                pelagos->AI()->Talk(0);
                        });
                }
            }
    }
};

// go_anima_canister_349508_q57102
struct go_anima_canister_349508 : public GameObjectAI
{
    go_anima_canister_349508(GameObject* go) : GameObjectAI(go) { }

    bool OnReportUse(Player* player) override
    {
        if (player->HasQuest(QUEST_PARDON_OUR_DUST))
        {
            player->KilledMonsterCredit(166223);
            player->CastSpell(player, 323931);
            if (Creature* sika = player->FindNearestCreature(166124, 5.0f, true))
                if (sika->GetPhaseShift().HasPhase(10105))
                    sika->AI()->Talk(1);
            player->GetScheduler().Schedule(3s, [this, player](TaskContext context)
                {
                    me->DestroyForPlayer(player);
                });
        }
        return true;
    }
};

// 319782 Infusing Anima
struct spell_infusing_anima_319782 : public SpellScript
{
    PrepareSpellScript(spell_infusing_anima_319782);

    void HandleAfterCast()
    {
        if (!GetCaster())
            return;

        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* creTarget = player->GetSelectedUnit())
                    if (Creature* target = creTarget->ToCreature())
                    {
                        if (player->HasAura(AURA_ANIMA_CANNISTER))
                        {
                            switch (target->GetEntry())
                            {

                            case 156398:
                                questProgressIncre(player, target, 3);
                                player->GetScheduler().Schedule(5s, [target](TaskContext context)
                                    {
                                        target->AI()->Talk(0);
                                    });
                                player->GetScheduler().Schedule(Milliseconds(10000), [target, player](TaskContext context)
                                    {
                                        target->SetAIAnimKitId(11860);
                                        if (Creature* steward = player->FindNearestCreature(158504, 15.0f, true))
                                            if (steward->GetSpawnId() == 4000000000097817)
                                                steward->AI()->Talk(0);
                                    });
                                break;
                            case 166164:
                                questProgressIncre(player, target, 2);
                                if (Creature* steward = player->FindNearestCreature(158504, 30.0f, true))
                                    if (steward->GetSpawnId() == 4000000000049715)
                                        steward->AI()->Talk(1);
                                break;
                            case 166163:
                                questProgressIncre(player, target, 5);
                                target->CastSpell(target, 306346);
                                if (Creature* steward = player->FindNearestCreature(158504, 15.0f, true))
                                    if (steward->GetSpawnId() == 4000000000138867)
                                        steward->AI()->Talk(2);
                            }
                        }
                    }
    }

    void questProgressIncre(Player* player, Creature* target, uint8 incre)
    {
        for (uint8 i = 0; i < incre; i++)
        {
            player->KilledMonsterCredit(166161);
        }
        target->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        target->DespawnOrUnsummon(30s, 30s);
        if (player->GetQuestStatus(QUEST_PARDON_OUR_DUST) == QUEST_STATUS_COMPLETE)
            if (Creature* sika = player->FindNearestCreature(166124, 70.0f, true))
                sika->AI()->DoAction(ACTION_MOVE_SIKA);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_infusing_anima_319782::HandleAfterCast);
    }
};

//npc_sika_166124_q57102
struct npc_sika_166124 : public ScriptedAI
{
    npc_sika_166124(Creature* creature) : ScriptedAI(creature) {}

    TaskScheduler _scheduler;

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_MOVE_SIKA)
        {
            _scheduler.Schedule(Milliseconds(1000), [this](TaskContext context)
                {
                    Talk(2);
                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(0, -4338.01f, -4230.11f, 6555.89f, true, 2.61138f);
                    me->DespawnOrUnsummon(60s, 10s);
                });
        }
    }
};

//npc_greeter_mnemis_158281_q57102
struct npc_greeter_mnemis_158281 : public ScriptedAI
{
    npc_greeter_mnemis_158281(Creature* creature) : ScriptedAI(creature)
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_PARDON_OUR_DUST)
        {
            me->SetAIAnimKitId(0);
            player->GetScheduler().Schedule(Milliseconds(3000), [this, player](TaskContext context)
                {
                    if (Creature* mnemis = player->FindNearestCreature(158281, 5.0f, true))
                        mnemis->AI()->Talk(0);
                });
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_A_FATE_MOST_NOBLE)
        {
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 action) override
    {
        if (MenuID != 25893)
            return false;

        switch (action)
        {
        case 0:
        case 1:
        case 2:
            CloseGossipMenuFor(player);
            player->KilledMonsterCredit(158281);
            Talk(1);
            player->GetScheduler().Schedule(Milliseconds(9000), [this, player](TaskContext context)
                {
                    Talk(2);
                    me->DespawnOrUnsummon(60s, 5s);
                }).Schedule(Milliseconds(13500), [this, player](TaskContext context)
                    {
                        if (Creature* kela = player->FindNearestCreature(NPC_KLEIA_I, 10.0f, true))
                        {
                            kela->AI()->Talk(1);
                            kela->GetMotionMaster()->MovePoint(0, -4332.59f, -4212.92f, 6551.16f, true);
                            me->AddDelayedEvent(3500, [kela]()  ->void
                                {
                                    kela->GetMotionMaster()->MovePoint(0, -4303.01f, -4217.23f, 6552.08f, true);
                                });
                            me->AddDelayedEvent(8000, [kela]()  ->void
                                {
                                    kela->DespawnOrUnsummon(50ms, 60s);
                                });
                        }
                    });
                break;
        }
        return true;
    }
};

// go_bench_349889_349528_q57102
struct go_bench_349889_349528 : public GameObjectAI
{
    go_bench_349889_349528(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_A_FATE_MOST_NOBLE) == QUEST_STATUS_INCOMPLETE)
            if (!player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1))
            {
                player->CastSpell(player, 123161);   // sit chir aura
                player->SummonCreature(166273, Position(-4247.62f, -4207.17f, 6551.06f, 3.53954f), TEMPSUMMON_TIMED_DESPAWN, 120s);  // Image of Greeter Mnemis
            }
        return true;
    }
};

//npc_kyrian_aspirant_156228_q57584
struct npc_kyrian_aspirant_156228 : public ScriptedAI
{
    npc_kyrian_aspirant_156228(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool say_nw;
    bool say_se;
    bool say_sw;
    bool quest57676_57677;

    void Reset() override
    {
        say_nw = false;
        say_se = false;
        say_sw = false;
        quest57676_57677 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
            {
                if (!player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1))
                    if (me->GetSpawnId() == 4000000000049669)
                    {
                        if (!say_nw)
                        {
                            say_nw = true;
                            Talk(0);
                            me->DespawnOrUnsummon(60s, 10s);
                        }
                    }
                if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 3))
                    if (me->GetSpawnId() == 4000000000076602)
                    {
                        if (!say_se)
                        {
                            say_se = true;
                            Talk(1);
                            me->DespawnOrUnsummon(60s, 10s);
                        }
                    }
                if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 4))
                    if (me->GetSpawnId() == 4000000000138856)
                    {
                        if (!say_sw)
                        {
                            say_sw = true;
                            Talk(2);
                            me->DespawnOrUnsummon(60s, 10s);
                        }
                    }
                if (player->HasQuest(QUEST_THINGS_THAT_HAUNT_US) || player->HasQuest(QUEST_A_SOULBIND_IN_NEED))
                {
                    if (!quest57676_57677)
                    {
                        if (me->GetSpawnId() == 346571)
                            me->GetMotionMaster()->MovePath(15622800, false);
                        if (me->GetSpawnId() == 346572)
                            me->GetMotionMaster()->MovePath(15622801, false);
                        if (me->GetSpawnId() == 346573)
                            me->GetMotionMaster()->MovePath(15622802, false);
                        quest57676_57677 = true;
                    }
                }
            }
    }
};

//npc_image_of_greeter_mnemis_166273_q57584
struct npc_image_of_greeter_mnemis_166273 : public ScriptedAI
{
    npc_image_of_greeter_mnemis_166273(Creature* creature) : ScriptedAI(creature) {}

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            if (player->GetQuestStatus(QUEST_A_FATE_MOST_NOBLE) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1))
                {
                    Talk(0);
                    player->GetScheduler().Schedule(Milliseconds(5000), [this](TaskContext context)
                        {
                            Talk(1);
                        }).Schedule(Milliseconds(14000), [this](TaskContext context)
                            {
                                Talk(2);
                            }).Schedule(Milliseconds(24000), [this](TaskContext context)
                                {
                                    Talk(3);
                                }).Schedule(Milliseconds(29000), [this, player](TaskContext context)
                                    {
                                        player->KilledMonsterCredit(158599);
                                        player->RemoveAura(123161);
                                    });
                                me->ForcedDespawn(34000);
                }

                if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 3))
                {
                    player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext context)
                        {
                            Talk(4);
                        });
                    me->ForcedDespawn(22000);
                }

                if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 4))
                {
                    player->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext context)
                        {
                            Talk(5);
                        });
                    me->ForcedDespawn(20000);
                }
            }
    }
};

// npc_Kleia_170179_q57584
struct npc_Kleia_170179 : public EscortAI
{
    npc_Kleia_170179(Creature* creature) : EscortAI(creature) { Reset(); }

private:
    bool say;
    bool riturl_aura;

    void Reset() override
    {
        say = false;
        riturl_aura = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 20.0f))
            {
                if (player->GetQuestStatus(QUEST_A_FATE_MOST_NOBLE) == QUEST_STATUS_INCOMPLETE)
                    if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 2))
                    {
                        if (!say)
                        {
                            say = true;
                            Talk(0, player);
                        }
                    }
            }
        }
    }


    void DoAction(int32 action) override
    {
        if (action == ACTION_CLEANING_SAY_KLELA)
        {
            if (!riturl_aura)
            {
                riturl_aura = true;
                Talk(1);
            }
        }
        if (action == ACTION_CLEANING_MOVE_KLELA)
        {
            Talk(3);
            LoadPath(170179);
            Start(false);
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
        case 5:
            me->DespawnOrUnsummon(1s, 60s);
            break;
        }
    }
};


//331712
class spell_burden_of_life_cooldown : public AuraScript
{
    PrepareAuraScript(spell_burden_of_life_cooldown);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
        {
            if (Creature* tariesh = player->SummonCreature(170283, Position(-4270.96f, -4323.85f, 6556.14f, 21.227f), TEMPSUMMON_TIMED_DESPAWN, 30s))
                if (Creature* klela = player->FindNearestCreature(170179, 40.0f, true))
                {
                    tariesh->SetEmoteState(EMOTE_ONESHOT_CHEER);
                    player->GetScheduler().Schedule(Milliseconds(3000), [player, klela, tariesh](TaskContext context)
                        {
                            tariesh->SetEmoteState(EMOTE_ONESHOT_SPECIALATTACK2H);
                            klela->AI()->Talk(2);
                        }).Schedule(Milliseconds(7000), [player, tariesh, klela](TaskContext context)
                            {
                                klela->AI()->DoAction(ACTION_CLEANING_MOVE_KLELA);
                                if (Creature* northeast = player->FindNearestCreature(166332, 40.0f, true))
                                    northeast->DespawnOrUnsummon(1s, 30s);
                                tariesh->DespawnOrUnsummon();
                            });
                }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_burden_of_life_cooldown::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// npc_visit_northeast_station_166332_q57584
struct npc_visit_northeast_station_166332 : public ScriptedAI
{
    npc_visit_northeast_station_166332(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool button_event;

    void Reset() override
    {
        button_event = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 15.0f))
                if (player->GetQuestStatus(QUEST_A_FATE_MOST_NOBLE) == QUEST_STATUS_INCOMPLETE)
                    if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 2))
                    {
                        if (!button_event)
                        {
                            button_event = true;
                            player->CastSpell(player, 308753);  // Extra Action Button
                            if (Creature* sika = player->FindNearestCreature(170179, 40.0f, true))
                                sika->AI()->DoAction(ACTION_CLEANING_SAY_KLELA);
                        }
                    }
    }
};

// go_compendium_of_arrival_335697_q57584
struct go_compendium_of_arrival_335697 : public GameObjectAI
{
    go_compendium_of_arrival_335697(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_A_FATE_MOST_NOBLE) == QUEST_STATUS_INCOMPLETE)
            if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 3))
            {
                player->SummonCreature(166273, Position(-4436.14f, -4268.1f, 6556.34f, 0.172985f), TEMPSUMMON_TIMED_DESPAWN, 30s);  // Image of Greeter Mnemis
                player->KilledMonsterCredit(158329);
            }
        return true;
    }
};

// go_pure_thought_vesper_335698_q57584
struct go_pure_thought_vesper_335698 : public GameObjectAI
{
    go_pure_thought_vesper_335698(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_A_FATE_MOST_NOBLE) == QUEST_STATUS_INCOMPLETE)
            if (player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 1) && !player->GetQuestObjectiveProgress(QUEST_A_FATE_MOST_NOBLE, 4))
            {
                player->SummonCreature(166273, Position(-4396.88f, -4167.27f, 6552.82f, 4.79363f), TEMPSUMMON_TIMED_DESPAWN, 30s);  // Image of Greeter Mnemis
                player->CastSpell(player, 308760);
            }
        return true;
    }
};

Position const KleiaEyePos[2] =
{
    {-4386.04f, -4486.11f, 6567.17f, 3.7235f},
    {-4372.46f, -4506.01f, 6564.54f, 3.9949f},
};

// npc_Kleia_166316
struct npc_Kleia_166316 : public ScriptedAI
{
    npc_Kleia_166316(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    bool kleia_eyes;
    bool aspirant_rest;

    void Initialize()
    {
        kleia_eyes = false;
        aspirant_rest = false;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            if (player->GetQuestStatus(QUEST_WALK_THE_PATH_ASPIR) == QUEST_STATUS_INCOMPLETE)
            {
                m_playerGUID = player->GetGUID();
                me->AddDelayedEvent(10000, [this]() -> void { Talk(0); });
                me->AddDelayedEvent(23000, [this]() -> void { Talk(1); });
            }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_KLEIAS_PERSPECTIVE)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                Talk(4, player);
                me->AddDelayedEvent(9000, [this, player]() -> void
                    {
                        Talk(5);
                        if (Creature* soul = me->SummonCreature(157681, Position(-4393.42f, -4485.64f, 6567.5f, 5.31432f), TEMPSUMMON_TIMED_DESPAWN, 30s))
                        {
                            soul->RemoveAura(244971);
                            soul->SetStandState(UNIT_STAND_STATE_STAND);
                            soul->GetMotionMaster()->MovePath(15861801, false);
                        }
                        if (Creature* acolyte = me->SummonCreature(158618, Position(-4313.87f, -4567.75f, 6545.1f, 2.67695f), TEMPSUMMON_TIMED_DESPAWN, 30s))
                            acolyte->GetMotionMaster()->MovePath(15768101, false);
                        if (Creature* ascended = me->SummonCreature(170123, Position(-4399.66f, -4482.48f, 6573.46f, 5.62139f), TEMPSUMMON_TIMED_DESPAWN, 30s))
                            ascended->GetMotionMaster()->MovePath(17012301, false);
                        me->GetMap()->SetZoneWeather(10534, WEATHER_STATE_FINE, 1.f);
                        if (Creature* eye = me->FindNearestCreature(NPC_SEE_BASTION_THROUGH_KLEIA_EYES, 5.0f, true))
                            eye->RemoveAura(AURA_EMPOWERED_AREA);
                        me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle(), {}, MOTION_SLOT_ACTIVE);
                    });
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Creature* target = who->ToCreature())
        {
            if (target->GetEntry() == NPC_TRAVEL_WITH_KLEIA)
                if (target->IsInDist(me, 25.0f) && !kleia_eyes)
                {
                    kleia_eyes = true;
                    Talk(2);
                    me->GetMotionMaster()->MovePoint(3001, KleiaEyePos[0], true, 3.7235f);
                    target->DespawnOrUnsummon(5s, 1min);
                }
            if (target->GetEntry() == NPC_ASPIRING_SOUL && target->GetSpawnId() == 346569)
                if (target->IsInDist(me, 30.0f) && !aspirant_rest)
                {
                    aspirant_rest = true;
                    Talk(6);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePath(16631601, false);
                }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (type)
        {
        case POINT_MOTION_TYPE:
        {
            if (id == 3001)
            {
                me->PauseMovement(8000);
                me->GetScheduler().Schedule(Milliseconds(9000), [this](TaskContext context)
                    {
                        me->ResumeMovement();
                        me->GetMotionMaster()->MovePoint(3002, KleiaEyePos[1], true, 3.9949f);
                    });
            }
            if (id == 3002)
            {
                Talk(3);
                if (Creature* eye = me->FindNearestCreature(NPC_SEE_BASTION_THROUGH_KLEIA_EYES, 5.0f, true))
                    eye->AddAura(AURA_EMPOWERED_AREA, eye);
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                {
                    player->KilledMonsterCredit(170415);
                    player->AddAura(SPELL_PERSPECTIVE_OVERRIDE, player);
                }
            }
        }
        break;
        }
    }

    void WaypointReached(uint32 pointId, uint32 pathId) override
    {
        if (pointId == 4)
        {
            if (Creature* kleia = me->FindNearestCreature(NPC_KLEIA_I, 30.0f, true))
                kleia->AI()->DoAction(ACTION_ASPIRANT_COMPLETE);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                player->KilledMonsterCredit(NPC_KLEIA_I);
        }
    }
};

// 331259 Kleia's Perspective
struct spell_kleias_perspective_331259 : public SpellScript
{
    PrepareSpellScript(spell_kleias_perspective_331259);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                player->KilledMonsterCredit(170416);
                player->CastSpell(player, SPELL_PERSPECTIVE_PHASE_AURA);
                player->RemoveAura(SPELL_PERSPECTIVE_OVERRIDE);
                if (Creature* kleia = player->FindNearestCreature(166316, 25.0f, true))
                    kleia->AI()->DoAction(ACTION_KLEIAS_PERSPECTIVE);
            }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_kleias_perspective_331259::HandleAfterCast);
    }
};

Position const PelagosPath[3] =
{
    {-4055.28f, -4336.92f, 6506.36f, 4.86929f},
    {-4051.51f, -4352.64f, 6506.36f, 4.76719f},
    {-4048.83f, -4375.81f, 6510.57f, 4.83002f},
};

// Pelagos 166323
struct npc_pelagos_166323 : public ScriptedAI
{
    npc_pelagos_166323(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool player_near;

    void Initialize()
    {
        player_near = false;;
    }

    void Reset() override
    {
        me->SetHover(true);
        me->SetHoverHeight(8);
        me->SummonCreature(NPC_EXCRUCIATING_MEMORY, -4056.76f, -4323.75f, 6506.44f, 4.7639f, TEMPSUMMON_MANUAL_DESPAWN);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 15.0f))
                if (player->GetQuestStatus(QUEST_A_SOULBIND_IN_NEED) == QUEST_STATUS_INCOMPLETE && !player_near)
                {
                    player_near = true;
                    player->KilledMonsterCredit(166323);
                    Talk(0);
                }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == NPC_EXCRUCIATING_MEMORY)
        {
            summon->AI()->Talk(1);
            if (me->IsHovering())
            {
                me->SetHoverHeight(1);
                me->SetHover(false);
            }
            me->SetAIAnimKitId(0);
            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            me->AddDelayedEvent(7000, [this]()  ->void { Talk(2); });
            me->AddDelayedEvent(12000, [this]()  ->void
                {
                    Talk(3);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->GetMotionMaster()->MoveSmoothPath(1001, PelagosPath, 3, false, false);
                });
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
            me->DespawnOrUnsummon(1s, 1min);
    }
};

Position const FSteward_pos[5] =
{
    {-4207.21f, -4823.98f, 6532.41f, 4.69747f},  // Steward01_Spos
    {-4214.38f, -4828.28f, 6532.41f, 5.77385f},  // Steward02_Spos
    {-4205.42f, -4816.08f, 6531.83f, 4.60767f},  // Steward01_Mpos
    {-4218.72f, -4821.15f, 6531.91f, 5.51524f},  // Steward02_Mpos
    {-4217.24f, -4787.69f, 6528.43f, 1.80554f},  // Steward_Endpos
};

// Altered Sentinel 168244
struct npc_altered_sentinel_168244 : public ScriptedAI
{
    npc_altered_sentinel_168244(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    EventMap _events;
    bool player_near;
    ObjectGuid FSteward01;
    ObjectGuid FSteward02;

    void Initialize()
    {
        player_near = false;;
    }

    void Reset() override
    {
        _events.Reset();
        FSteward01 = ObjectGuid::Empty;
        FSteward02 = ObjectGuid::Empty;
        SummonFStewards();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 25.0f))
            {
                if (player->GetQuestStatus(QUEST_WANTED_ALTERED_SENTINEL) == QUEST_STATUS_INCOMPLETE && !player_near)
                {
                    player_near = true;
                    if (Creature* steward1 = ObjectAccessor::GetCreature(*me, FSteward01))
                        if (Creature* steward2 = ObjectAccessor::GetCreature(*me, FSteward02))
                        {
                            steward2->AI()->Talk(0);
                            me->AddDelayedEvent(5000, [steward1]() ->void {steward1->AI()->Talk(1); });
                        }
                }
            }
    }

    void JustEngagedWith(Unit* victim) override
    {
        Talk(0);
        if (Creature* steward1 = ObjectAccessor::GetCreature(*me, FSteward01))
            if (Creature* steward2 = ObjectAccessor::GetCreature(*me, FSteward02))
            {
                steward1->GetMotionMaster()->MovePoint(0, FSteward_pos[2], true, 4.60767f);
                steward2->AI()->Talk(2);
                steward2->GetMotionMaster()->MovePoint(0, FSteward_pos[3], true, 5.51524f);
            }
        _events.ScheduleEvent(EVENT_HAMPERING_STRIKE, 4s);
    }

    void UpdateAI(uint32 diff) override
    {

        if (!UpdateVictim())
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_HAMPERING_STRIKE:
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::MinDistance, 0))
                    me->CastSpell(target, 330940, true);  // Hampering Strike
                _events.Repeat(15s);
                break;
            }
            }
        }
        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer) override
    {
        Talk(1);
        if (Creature* steward1 = ObjectAccessor::GetCreature(*me, FSteward01))
            if (Creature* steward2 = ObjectAccessor::GetCreature(*me, FSteward02))
            {
                steward1->GetMotionMaster()->MovePoint(0, FSteward_pos[4], true);
                steward1->ForcedDespawn(3000);
                steward1->AI()->Talk(3);
                steward2->GetMotionMaster()->MovePoint(0, FSteward_pos[4], true);
                steward2->ForcedDespawn(3000);
            }
    }

    void SummonFStewards()
    {
        if (Creature* steward01 = me->SummonCreature(NPC_FRANTIC_STEWARD, FSteward_pos[0], TEMPSUMMON_MANUAL_DESPAWN))
        {
            steward01->SetWalk(false);
            FSteward01 = steward01->GetGUID();
        }
        if (Creature* steward02 = me->SummonCreature(NPC_FRANTIC_STEWARD, FSteward_pos[1], TEMPSUMMON_MANUAL_DESPAWN))
        {
            steward02->SetWalk(false);
            FSteward02 = steward02->GetGUID();
        }

    }
};

// 309540 Ride Theonara
struct spell_ride_theonara_309540 : public SpellScript
{
    PrepareSpellScript(spell_ride_theonara_309540);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        player->KilledMonsterCredit(158686);
                        player->CastSpell(creTarget, SPELL_RIDE_VEHICLE_HARDCODED, false);
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ride_theonara_309540::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

Position const A_Life_Of_Service_Right_Path[3] =
{
    {-4513.54f, -5187.01f, 6547.98f, 0.46190f},  //0 right_npcs
    {-4511.41f, -5193.39f, 6547.98f, 0.76427f},  //1
    {-4505.79f, -5200.32f, 6547.98f, 0.77213f},  //2
};

Position const A_Life_Of_Service_Left_Path[3] =
{
    {-4470.65f, -5170.58f, 6547.98f, 3.17938f},  //0 left_npcs
    {-4467.24f, -5177.95f, 6547.98f, 3.08513f},  //1
    {-4465.14f, -5186.04f, 6547.98f, 3.14796f},  //2
};

Position const A_Life_Of_Service_Pos[3] =
{
    {-4490.37f, -5182.27f, 6548.06f, 5.06071f},  //0 sophone_path
    {-4501.76f, -5144.91f, 6541.77f, 5.00732f},  //1 kleis_pos
    {-4506.01f, -5146.78f, 6541.81f, 5.09635f},  //2 pelagos_pos
};

Position const ForgeGoneColdSophonePath[3] =
{
    {-4535.52f, -4953.69f, 6527.26f, 1.53385f}, //0 
    {-4536.68f, -4936.14f, 6527.54f, 1.81267f}, //1 
    {-4542.92f, -4933.49f, 6527.54f, 2.18166f}, //2
};

// 158807 Forgelite Sophone
struct npc_forgelite_sophone_158807 : public ScriptedAI
{
    npc_forgelite_sophone_158807(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool player_near;
    ObjectGuid m_playerGUID;

    void Initialize()
    {
        player_near = false;;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 15.0f))
            {
                if (player->GetQuestStatus(QUEST_THE_ASPIRANTS_CRUCIBLE) == QUEST_STATUS_COMPLETE && !player_near)
                {
                    player_near = true;
                    Talk(0);
                }
                if (player->GetQuestStatus(QUEST_A_LIFE_OF_SERVICE) == QUEST_STATUS_COMPLETE && !player_near)
                    if (me->GetPhaseShift().HasPhase(10142))
                    {
                        Talk(3);
                        if (Creature* kleia = me->FindNearestCreature(158862, 5.f))
                        {
                            me->AddDelayedEvent(7000, [kleia]()->void { kleia->AI()->Talk(2); });
                        }
                        player_near = true;
                    }
            }
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            Talk(11);
            CloseGossipMenuFor(player);
            player->AddItem(180968, 1);
        }
        return true;
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_SOPHONE_START_PATH)
        {
            me->GetMotionMaster()->MoveSmoothPath(1001, ForgeGoneColdSophonePath, 3, true);
            if (m_playerGUID.IsEmpty())
                if (Player* player = me->SelectNearestPlayer(35.f))
                    m_playerGUID = player->GetGUID();
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                if (!player->GetPhaseShift().HasPhase(10144)) PhasingHandler::AddPhase(player, 10144, true);
                if (player->GetPhaseShift().HasPhase(10142)) PhasingHandler::RemovePhase(player, 10142, true);
            }
        }
        if (id == 1002) // Ember_Court
        {
            if (Creature* sika = me->FindNearestCreature(166577, 20.f, true))
                sika->AI()->DoAction(ACTION_EMBER_COURT);
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_A_LIFE_OF_SERVICE)
        {
            Talk(2);
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(1001, A_Life_Of_Service_Pos[0]);
            if (Creature* kleia = me->SummonCreature(158862, A_Life_Of_Service_Pos[1], TEMPSUMMON_TIMED_DESPAWN, 1min))
                if (Creature* pelagos = me->SummonCreature(158861, A_Life_Of_Service_Pos[2], TEMPSUMMON_TIMED_DESPAWN, 1min))
                {
                    kleia->GetMotionMaster()->MovePath(15886201, false);
                    pelagos->GetMotionMaster()->MovePath(15886103, false);
                    me->AddDelayedEvent(10000, [kleia]()->void { kleia->AI()->Talk(1); });
                    me->AddDelayedEvent(15000, [this, player]()->void
                        {
                            if (player->GetPhaseShift().HasPhase(10141)) PhasingHandler::RemovePhase(player, 10141, true);
                            if (!player->GetPhaseShift().HasPhase(10142)) PhasingHandler::AddPhase(player, 10142, true);
                            me->GetMotionMaster()->MoveTargetedHome();
                        });
                }
            Aspirants_move();
            player_near = false;
        }

        if (quest->GetQuestId() == QUEST_A_FORGE_GONE_COLD)
        {
            if (Creature* kleia = me->FindNearestCreature(158862, 5.f))
                if (Creature* pelagos = me->FindNearestCreature(158861, 5.f))
                {
                    pelagos->AI()->Talk(0);
                    me->AddDelayedEvent(6000, [kleia]()->void { kleia->AI()->Talk(3); });
                }
        }
        if (quest->GetQuestId() == QUEST_WORK_OF_ONES_HANDS)
        {
            if (Creature* raw_materials = player->FindNearestCreature(159396, 20.f, true))
                if (Creature* forge_vent = player->FindNearestCreature(159391, 25.f, true))
                    if (GameObject* basin = player->FindNearestGameObject(338216, 20.f, false))
                    {
                        raw_materials->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        forge_vent->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        basin->SetFlag(GO_FLAG_NOT_SELECTABLE);
                    }
        }
        if (quest->GetQuestId() == QUEST_THE_TRUE_CRUCIBLE_AWAITES)
        {
            if (Creature* sika = me->FindNearestCreature(166742, 5.f))
            {
                Talk(10);
                me->AddDelayedEvent(6000, [sika]()->void { sika->AI()->Talk(7); });
            }
        }
        if (quest->GetQuestId() == QUEST_CHAMBER_OF_FIRST_REFLECTION)
        {
            if (Creature* kleia = me->FindNearestCreature(158862, 5.f))
                kleia->AI()->Talk(8);
        }
    }

    void Aspirants_move()
    {
        std::list<Creature*> cList = me->FindAllCreaturesInRange(50.f);
        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
        {
            if (Creature* npcs = *itr)
            {
                if (npcs->GetPhaseShift().HasPhase(10141))
                {
                    if (npcs->GetEntry() == 156228)
                    {
                        npcs->SetWalk(false);
                        npcs->DespawnOrUnsummon(3s, 1min);
                        if (npcs->GetSpawnId() == 346578 || npcs->GetSpawnId() == 346579 || npcs->GetSpawnId() == 346580 || npcs->GetSpawnId() == 346581)
                        {
                            npcs->GetMotionMaster()->MovePoint(0, A_Life_Of_Service_Right_Path[urand(0, 2)]);
                        }
                        else
                            npcs->GetMotionMaster()->MovePoint(0, A_Life_Of_Service_Left_Path[urand(0, 2)]);
                    }
                    if (npcs->GetEntry() == 158618)
                    {
                        npcs->SetWalk(false);
                        npcs->DespawnOrUnsummon(3s, 1min);
                        if (npcs->GetSpawnId() == 346582 || npcs->GetSpawnId() == 346583 || npcs->GetSpawnId() == 346584)
                        {
                            npcs->GetMotionMaster()->MovePoint(0, A_Life_Of_Service_Right_Path[urand(0, 2)]);
                        }
                        else
                            npcs->GetMotionMaster()->MovePoint(0, A_Life_Of_Service_Left_Path[urand(0, 2)]);
                    }
                }
            }
        }
    }
};

// 312218 Witness
class aura_witness_312218 : public AuraScript
{
    PrepareAuraScript(aura_witness_312218);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();
        if (!caster || !target)
            return;
        target->AddDelayedEvent(2000, [target]() -> void
            {
                if (!target->GetPhaseShift().HasPhase(10142)) PhasingHandler::AddPhase(target, 10142, true);
                if (!target->GetPhaseShift().HasPhase(169)) PhasingHandler::AddPhase(target, 169, true);
            });
    }

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();
        if (!caster || !target)
            return;
        if (target->GetPhaseShift().HasPhase(10142)) PhasingHandler::RemovePhase(target, 10142, true);
        if (!target->GetPhaseShift().HasPhase(10143)) PhasingHandler::AddPhase(target, 10143, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(aura_witness_312218::OnRemove, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(aura_witness_312218::OnApply, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
    }
};

Position const ChampionNpcPos[6] =
{
    {-4482.77f, -5207.81f, 6551.91f, 1.9646f}, //0 alliance 158991
    {-4471.68f, -5207.92f, 6551.69f, 1.7252f}, //1
    {-4488.71f, -5216.72f, 6551.69f, 2.0511f}, //2
    {-4480.22f, -5206.91f, 6551.91f, 2.0095f}, //3 horde 158990
    {-4468.55f, -5207.17f, 6551.69f, 1.7621f}, //4
    {-4486.07f, -5214.57f, 6551.69f, 2.0472f}, //5
};

Position const MemoryWitnessedPos[8] =
{
    {-4482.33f, -5204.69f, 6551.91f, 5.12598f}, //0 center
    {-4476.31f, -5204.69f, 6551.91f, 5.17703f}, //1 left1
    {-4471.31f, -5203.95f, 6552.02f, 5.07493f}, //2 left2
    {-4487.61f, -5209.25f, 6551.91f, 5.19274f}, //3 Right1
    {-4490.46f, -5213.62f, 6551.69f, 5.25949f}, //4 Right2
    {-4488.21f, -5209.97f, 6552.02f, 5.06319f}, //5 left3
    {-4474.47f, -5205.21f, 6552.02f, 5.14566f}, //6 Right3
};

Position const EternalJailorPos[2] =
{
    {-4476.31f, -5204.69f, 6551.91f, 5.17703f}, //1 left1
    {-4487.61f, -5209.25f, 6551.91f, 5.19274f}, //3 Right1
};

Position const ServantOfTheLichKingPos[3] =
{
    {-4482.33f, -5204.69f, 6551.91f, 5.12598f}, //0 center
    {-4471.31f, -5203.95f, 6552.02f, 5.07493f}, //2 left2
    {-4490.46f, -5213.62f, 6551.69f, 5.25949f}, //4 Right2
};


// 159191 Eternal Watcher 
struct npc_eternal_watcher_159191 : public ScriptedAI
{
    npc_eternal_watcher_159191(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    uint8 EnemyDeathCount;

    void Initialize()
    {
        EnemyDeathCount = 0;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_SUMMON_ENEMY_NPC)
        {
            EnemyNpcPos();
            if (m_playerGUID.IsEmpty())
                if (Player* player = me->SelectNearestPlayer(20.f))
                    m_playerGUID = player->GetGUID();
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            switch (summon->GetEntry())
            {
            case 159019:
            case 158995:
                EnemyDeathCount++;
                if (EnemyDeathCount == 7)
                    EndPhaseResult(player);
                break;
            case 159023:
            case 161308:
                EnemyDeathCount++;
                if (EnemyDeathCount == 5)
                    EndPhaseResult(player);
                break;
            case 158999:
                EnemyDeathCount++;
                if (EnemyDeathCount == 2)
                    EndPhaseResult(player);
                break;
            case 161372:
                EnemyDeathCount++;
                if (EnemyDeathCount == 3)
                    EndPhaseResult(player);
                break;
            case 159021:
                EnemyDeathCount++;
                if (EnemyDeathCount == 1)
                    EndPhaseResult(player);
                break;
            }
        }
    }

    void EnemyNpcPos()
    {
        uint8 rand_enemy = urand(0, 4);
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            if (rand_enemy == 0)
            {
                for (uint8 i = 0; i < 7; i++)
                {
                    uint8 n_entry = urand(0, 1);
                    if (n_entry == 0)
                        me->SummonCreature(159019, MemoryWitnessedPos[i], TEMPSUMMON_MANUAL_DESPAWN); // living wond
                    else
                        me->SummonCreature(158995, MemoryWitnessedPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                }
                Talk(0, player);
                summonChampion(5);
            }
            else if (rand_enemy == 1)
            {
                for (uint8 i = 0; i < 2; i++)
                    me->SummonCreature(158999, EternalJailorPos[i], TEMPSUMMON_MANUAL_DESPAWN); // Eternal Jailor
                Talk(1, player);
                summonChampion(2);
            }
            else if (rand_enemy == 2)
            {
                me->SummonCreature(159021, ChampionNpcPos[1], TEMPSUMMON_MANUAL_DESPAWN);  // Spawn of Deathwing
                Talk(3, player);
                summonChampion(2);
            }
            else if (rand_enemy == 3)
            {
                for (uint8 i = 0; i < 5; i++)
                {
                    uint8 n_entry = urand(0, 1);
                    if (n_entry == 0)
                        me->SummonCreature(159023, MemoryWitnessedPos[i], TEMPSUMMON_MANUAL_DESPAWN); // Endless Scourge
                    else
                        me->SummonCreature(161308, MemoryWitnessedPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                }
                Talk(4, player);
                summonChampion(5);
            }
            else
            {
                for (uint8 i = 0; i < 3; i++)
                    me->SummonCreature(161372, ServantOfTheLichKingPos[i], TEMPSUMMON_MANUAL_DESPAWN); // Servant of the Lich King
                Talk(2, player);
                summonChampion(3);
            }
        }
    }

    void summonChampion(uint8 count)
    {
        for (uint8 i = 0; i <= count; i++)
        {
            uint8 n_entry = urand(0, 1);
            if (n_entry == 0)
                me->SummonCreature(158991, ChampionNpcPos[i], TEMPSUMMON_MANUAL_DESPAWN);
            else
                me->SummonCreature(158990, ChampionNpcPos[i], TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    void despawnChampion()
    {
        std::list<Creature*> cList = me->FindAllCreaturesInRange(30.f);
        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
        {
            if (Creature* champion = *itr)
                if (champion->GetEntry() == 158990 || champion->GetEntry() == 158991)
                    champion->ForcedDespawn();
        }
    }

    void EndPhaseResult(Player* player)
    {
        if (player->GetQuestStatus(QUEST_A_LIFE_OF_SERVICE) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonsterCredit(158968);
            Talk(5, player);
        }
        despawnChampion();
        EnemyDeathCount = 0;
    }

};

// 310061 [DNT] Trigger Wave Event
class spell_trigger_wave_event_310061 : public SpellScript
{
    PrepareSpellScript(spell_trigger_wave_event_310061);

    void HandleEffect(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return;
        if (caster->GetQuestStatus(QUEST_A_LIFE_OF_SERVICE) == QUEST_STATUS_INCOMPLETE)
            if (Creature* watcher = caster->FindNearestCreature(NPC_ETERNAL_WATCHER, 20.f))
                watcher->AI()->DoAction(ACTION_SUMMON_ENEMY_NPC);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_trigger_wave_event_310061::HandleEffect, EFFECT_1, SPELL_EFFECT_SEND_EVENT);
    }
};

Position const ForgeGoneColdSummonPos[3] =
{
    {-4539.38f, -4966.03f, 6527.52f, 1.20428f}, //0 sophone_pos
    {-4540.38f, -4962.24f, 6527.37f, 0.05946f}, //1 kleia_pos
    {-4537.32f, -4962.94f, 6527.39f, 1.11335f}, //2 pelagos_pos
};

Position const ForgeGoneColdKleiaPath[2] =
{
    {-4539.12f, -4946.77f, 6527.89f, 1.85618f},
    {-4543.83f, -4941.83f, 6527.62f, 0.05946f},
};

Position const ForgeGoneColdPelagosPath[2] =
{
    {-4539.12f, -4946.77f, 6527.89f, 1.85618f},
    {-4542.69f, -4943.12f, 6527.61f, 1.11335f},
};

Position const RSVP_SikaPos[7] =
{
    {-4532.92f, -4943.29f, 6529.44f, 1.79191f}, //0 Adonas
    {-4537.98f, -4944.18f, 6527.49f, 1.72643f}, //1 leda 
    {-4540.91f, -4944.53f, 6527.48f, 1.74214f}, //2 Sophone
    {-4537.18f, -4937.37f, 6527.54f, 4.96227f}, //3 Sika
    {-4535.35f, -4964.41f, 6527.38f, 1.50652f}, //4 leda_summon
    {-4539.17f, -4968.51f, 6527.58f, 1.39264f}, //5 Sophone_summon
    {-4526.51f, -4969.55f, 6539.01f, 1.81674f}, //6 Adonas_summon
};

// 166577 Sika 
struct npc_sika_166577 : public ScriptedAI
{
    npc_sika_166577(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool player_near;
    ObjectGuid m_playerGUID;
    ObjectGuid ikaranGuiD;
    ObjectGuid adonasGUID;
    ObjectGuid sophoneGUID;
    ObjectGuid ledaGUID;

    void Initialize()
    {
        player_near = false;;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!me->GetPhaseShift().HasPhase(10142))
            return;

        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 10.0f))
            {
                if (player->GetQuestStatus(QUEST_A_FORGE_GONE_COLD) == QUEST_STATUS_COMPLETE && !player_near)
                {
                    Talk(0);
                    if (Creature* sophone = me->SummonCreature(158807, ForgeGoneColdSummonPos[0], TEMPSUMMON_TIMED_DESPAWN, 1min))
                        if (Creature* kleia = me->SummonCreature(158862, ForgeGoneColdSummonPos[1], TEMPSUMMON_TIMED_DESPAWN, 1min))
                            if (Creature* pelagos = me->SummonCreature(158861, ForgeGoneColdSummonPos[2], TEMPSUMMON_TIMED_DESPAWN, 1min))
                            {
                                sophone->AI()->DoAction(ACTION_SOPHONE_START_PATH);
                                kleia->GetMotionMaster()->MoveSmoothPath(0, ForgeGoneColdKleiaPath, 2, true);
                                pelagos->GetMotionMaster()->MoveSmoothPath(0, ForgeGoneColdPelagosPath, 2, true);
                                me->AddDelayedEvent(5500, [sophone]()->void { sophone->AI()->Talk(4); });
                                me->AddDelayedEvent(10000, [this]()->void { Talk(1); });
                                me->AddDelayedEvent(12500, [sophone]()->void { sophone->AI()->Talk(5); });
                            }
                    player_near = true;
                }
            }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_RSVP_SIKA)
            Talk(4);
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_RSVP_SIKA)
        {
            m_playerGUID = player->GetGUID();
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(1001, RSVP_SikaPos[3], true, 4.96227f);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            Talk(5);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                Ember_Court_Dialog_Summon(player);
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_EMBER_COURT)
            Ember_Court_Dialog();
    }

    void Ember_Court_Dialog_Summon(Player* player)
    {
        if (Creature* adonas = me->SummonCreature(175014, RSVP_SikaPos[6], TEMPSUMMON_TIMED_DESPAWN, 1min))
            if (Creature* sophone = me->SummonCreature(158807, RSVP_SikaPos[5], TEMPSUMMON_TIMED_DESPAWN, 1min))
                if (Creature* leda = me->SummonCreature(159277, RSVP_SikaPos[4], TEMPSUMMON_TIMED_DESPAWN, 1min))
                    if (Creature* ikaran = player->FindNearestCreature(159278, 50.f, true))
                    {
                        ikaranGuiD = ikaran->GetGUID();
                        adonasGUID = adonas->GetGUID();
                        sophoneGUID = sophone->GetGUID();
                        ledaGUID = leda->GetGUID();
                        adonas->SetStandState(UNIT_STAND_STATE_STAND);
                        adonas->SetFlying(true);
                        adonas->SetHover(true);
                        adonas->GetMotionMaster()->MovePoint(0, RSVP_SikaPos[0], true, 1.79191f);
                        sophone->SetWalk(true);
                        leda->SetWalk(true);
                        sophone->GetMotionMaster()->MovePoint(1002, RSVP_SikaPos[2], true, 1.74214f);
                        leda->GetMotionMaster()->MovePoint(0, RSVP_SikaPos[1], true, 1.72643f);
                        ikaran->GetMotionMaster()->MovePath(15927801, false);
                    }
    }

    void Ember_Court_Dialog()
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            if (Creature* ikaran = ObjectAccessor::GetCreature(*me, ikaranGuiD))
                if (Creature* sophone = ObjectAccessor::GetCreature(*me, sophoneGUID))
                    if (Creature* adonas = ObjectAccessor::GetCreature(*me, adonasGUID))
                        if (Creature* leda = ObjectAccessor::GetCreature(*me, ledaGUID))
                        {
                            adonas->SetFlying(false);
                            adonas->SetHover(false);
                            adonas->GetMotionMaster()->MoveFall();
                            ikaran->AI()->Talk(2);
                            me->AddDelayedEvent(6000, [adonas]() ->void {adonas->AI()->Talk(4); });
                            me->AddDelayedEvent(11000, [sophone]() ->void {sophone->AI()->Talk(12); });
                            me->AddDelayedEvent(19000, [this, player]() ->void
                                {
                                    me->SetFacingToObject(player);
                                    Talk(6);
                                    Talk(7);
                                });
                            me->AddDelayedEvent(25000, [leda]() ->void {leda->AI()->Talk(2); });
                            me->AddDelayedEvent(29000, [this, adonas, sophone, leda, ikaran, player]() ->void
                                {
                                    me->GetMotionMaster()->MoveTargetedHome();
                                    ikaran->DespawnOrUnsummon(1s, 30s);
                                    adonas->ForcedDespawn();
                                    sophone->ForcedDespawn();
                                    leda->ForcedDespawn();
                                });
                        }
    }
};

Position const EtherwyrmPath[4] =
{
    {-4400.18f, -5086.43f, 6583.91f, 1.8499f}, //0 
    {-4410.26f, -5093.39f, 6564.27f, 2.4586f}, //1 
    {-4418.61f, -5071.53f, 6551.45f, 1.3079f}, //2
    {-4406.92f, -5067.04f, 6546.69f, 5.9811f}, //3
};

//349702 Anima Extractor 
struct go_anima_extractor_349702 : public GameObjectAI
{
    go_anima_extractor_349702(GameObject* go) : GameObjectAI(go)
    {
        me->SetGoState(GO_STATE_READY);
    }

    void SpellHit(WorldObject* caster, SpellInfo const* /*spellInfo*/)
    {
        me->SetGoState(GO_STATE_ACTIVE);
        me->SetFlag(GO_FLAG_NOT_SELECTABLE);
        if (GameObject* Canister = me->FindNearestGameObject(349706, 5.f, false))
            Canister->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
        if (Creature* etherwyrm = me->SummonCreature(158771, -4383.23f, -5054.18f, 6589.98f, 5.3581f, TEMPSUMMON_MANUAL_DESPAWN))
        {
            etherwyrm->RemoveAura(309740);
            etherwyrm->GetMotionMaster()->MoveSmoothPath(0, EtherwyrmPath, 4, false, true);
            std::list<Creature*> cList = me->FindNearestCreatures(158504, 15.0f);
            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
            {
                if (Creature* steward = *itr)
                    steward->GetMotionMaster()->MovePath(15850401, false);
            }
        }
    }
};

// 159473 Forgelite Sophone
struct npc_forgelite_sophone_159473 : public ScriptedAI
{
    npc_forgelite_sophone_159473(Creature* creature) : ScriptedAI(creature) { }
private:
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_LIGHT_THE_FORGE_FORGELITE)
        {
            m_playerGUID = player->GetGUID();
            me->RemoveAura(324961);
            if (Creature* forgehand = player->FindNearestCreature(159337, 8.f))
            {
                if (forgehand->GetSpawnId() == 346637)
                    forgehand->GetMotionMaster()->MovePath(15933701, false);
            }
            if (Creature* sika = player->FindNearestCreature(166577, 40.0f, true))
            {
                sika->AI()->Talk(2);
                me->AddDelayedEvent(3000, [this]() ->void
                    {
                        Talk(2);
                        me->GetMotionMaster()->MovePath(15947301, false);
                    });
                me->AddDelayedEvent(8000, [sika]() ->void
                    {
                        sika->GetMotionMaster()->MovePath(16657701, false);
                    });
                me->AddDelayedEvent(13000, [sika]() ->void
                    {
                        sika->AI()->Talk(3);
                    });
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE && id == 7)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                me->ForcedDespawn(3000);
                if (!player->GetPhaseShift().HasPhase(10145)) PhasingHandler::AddPhase(player, 10145, true);
                if (player->GetPhaseShift().HasPhase(10144)) PhasingHandler::RemovePhase(player, 10144, true);
            }
        }
    }
};

// 324932 [DNT] Kill Credit: Raw Materials - Basin 338216
struct spell_raw_materials_324932 : public SpellScript
{
    PrepareSpellScript(spell_raw_materials_324932);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                player->CastSpell(player, SPELL_CANCEL_CARRY_AURA);
                if (Creature* sophone = player->FindNearestCreature(159473, 5.0f, true))
                    sophone->AI()->Talk(3);
            }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_raw_materials_324932::HandleAfterCast);
    }
};

// 310646 Add Fuel
struct spell_Add_Fuel_310646 : public SpellScript
{
    PrepareSpellScript(spell_Add_Fuel_310646);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* obj) { return obj->GetEntry() != 159338; });
    }

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestObjectiveProgress(QUEST_WORK_OF_ONES_HANDS, 4)
                    && !player->GetQuestObjectiveProgress(QUEST_WORK_OF_ONES_HANDS, 2))
                {
                    if (Creature* raw_materials = player->FindNearestCreature(159396, 10.f, true))
                        if (Creature* sika = player->FindNearestCreature(166742, 15.f, true))
                            if (Creature* forgehand = player->FindNearestCreature(159337, 15.f, true))
                                if (Creature* sophone = player->FindNearestCreature(158807, 15.f, true))
                                {
                                    raw_material_num++;
                                    for (uint8 i = 0; i < urand(2, 5); i++)
                                    {
                                        player->CastSpell(player, SPELL_STACK_TEST, true);
                                    }
                                    switch (raw_material_num)
                                    {
                                    case 5:
                                        sika->AI()->Talk(3);
                                        break;
                                    case 10:
                                        sika->AI()->Talk(4);
                                        player->CastSpell(player, SPELL_ANIMA_SURGE, true);
                                        raw_materials->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                                        break;
                                    case 15:
                                        sika->AI()->Talk(5);
                                        break;
                                    case 20:
                                        sika->AI()->Talk(6);
                                        break;
                                    case 25:
                                        raw_materials->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                                        player->KilledMonsterCredit(159338);
                                        player->CastSpell(player, SPELL_CLEAR_ALL, true);
                                        sophone->AI()->Talk(6);
                                        sophone->SetWalk(true);
                                        sophone->GetMotionMaster()->MovePoint(1002, TheWorkOfOnesHandsPos[4], true, 1.95477f);
                                        forgehand->RemoveAura(AURA_CARRY_ASPIRANT_TRAY_WITH_SPOOL);
                                        forgehand->GetMotionMaster()->MovePoint(0, TheWorkOfOnesHandsPos[0], true, 1.95477f);
                                        sika->GetMotionMaster()->MovePoint(0, TheWorkOfOnesHandsPos[6], true, 1.95477f);
                                        player->AddDelayedEvent(5000, [sophone]() -> void { sophone->AI()->Talk(7); });
                                        if (Creature* armor = player->FindNearestCreature(159444, 15.f, true))
                                            armor->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                                        raw_material_num = 0;
                                        break;
                                    }
                                }
                }
            }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_Add_Fuel_310646::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
        AfterCast += SpellCastFn(spell_Add_Fuel_310646::HandleAfterCast);
    }
};

// 310687 Holding Steady
struct spell_holding_steady_310687 : public SpellScript
{
    PrepareSpellScript(spell_holding_steady_310687);

    void HandleSendEvent(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (Creature* sophone = caster->FindNearestCreature(158807, 15.f, true))
            if (Creature* forgehand = caster->FindNearestCreature(159337, 15.f, true))
            {
                sophone->AI()->Talk(8);
                forgehand->AI()->DoAction(ACTION_CARRY_CANISTER_START);
            }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_holding_steady_310687::HandleSendEvent, EFFECT_2, SPELL_EFFECT_SEND_EVENT);
    }
};

// 159337 Steward Forgehand
struct npc_steward_forgehand_159337 : public ScriptedAI
{
    npc_steward_forgehand_159337(Creature* creature) : ScriptedAI(creature) { }


    void DoAction(int32 action) override
    {
        if (action == ACTION_CARRY_CANISTER_START)
        {
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(1001, TheWorkOfOnesHandsPos[1], true, 2.05083f);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (id)
        {
        case 1001:
            me->CastSpell(me, AURA_CARRY_ANIMA_CANISTER);
            me->GetMotionMaster()->MovePoint(1002, TheWorkOfOnesHandsPos[2], true, 5.32325f);
            break;
        case 1002:
            me->RemoveAura(AURA_CARRY_ANIMA_CANISTER);
            me->GetMotionMaster()->MovePoint(1001, TheWorkOfOnesHandsPos[1], true, 2.05083f);
            break;
        }
    }

};

// 310647 Open Vent
struct spell_open_vent_310647 : public SpellScript
{
    PrepareSpellScript(spell_open_vent_310647);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Creature* target = GetHitCreature())
                {
                    target->CastSpell(target, 310648, true);
                    player->KilledMonsterCredit(159391);
                    if (Creature* raw_materials = player->FindNearestCreature(159396, 20.f, true))
                        if (Creature* sika = player->FindNearestCreature(166742, 15.f, true))
                        {
                            if (player->GetQuestObjectiveProgress(QUEST_WORK_OF_ONES_HANDS, 4)
                                && !player->GetQuestObjectiveProgress(QUEST_WORK_OF_ONES_HANDS, 2))
                            {
                                raw_materials->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                                sika->AI()->Talk(2);
                            }
                        }
                }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_open_vent_310647::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 159583 Kleia
struct npc_Kleia_159583 : public EscortAI
{
    npc_Kleia_159583(Creature* creature) : EscortAI(creature) {}
private:
    ObjectGuid m_playerGUID;

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_FIRST_CLEANSING)
            Talk(0);
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            m_playerGUID = player->GetGUID();
            player->KilledMonsterCredit(159583);
            Talk(1);
            LoadPath(me->GetEntry());
            Start(true);
        }
        return true;
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (waypointId == 2) Talk(2);
        if (waypointId == 9)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                if (!player->GetPhaseShift().HasPhase(10149)) PhasingHandler::AddPhase(player, 10149, true);
                if (player->GetPhaseShift().HasPhase(10148)) PhasingHandler::RemovePhase(player, 10148, true);
            }
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {

        if (Player* player = killer->ToPlayer())
            player->CastSpell(player, SPELL_OVERWHELMING_CLARITY, true);
        Talk(6);
    }

};

// 312493 gazing 
struct spell_gazing_312493 : public AuraScript
{
    PrepareAuraScript(spell_gazing_312493);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
        {
            player->KilledMonsterCredit(159592);
            if (GameObject* creTarget = player->FindNearestGameObject(338504, 10.f, false))
            {
                creTarget->SetFlag(GO_FLAG_NOT_SELECTABLE);
                player->AddDelayedEvent(60000, [creTarget]() -> void
                    {
                        creTarget->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                    });
            }

            if (player->GetQuestObjectiveProgress(QUEST_THE_FIRST_CLEANSING, 8) == 1)
            {
                player->CastSpell(player, SPELL_DEEP_INTROSPECTION_STUN, true);
                if (Creature* mirror = player->FindNearestCreature(159592, 10.f, true))
                    if (player->GetQuestObjectiveProgress(QUEST_THE_FIRST_CLEANSING, 8) == 1)
                    {
                        if (Creature* shadow = mirror->SummonCreature(159668, mirror->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            player->PlayConversation(17926);
                            player->CastSpell(shadow, SPELL_MIRROR_IMAGE_AURA, true);
                            shadow->CastSpell(shadow, SPELL_MIRROR_IMAGE_SPAWN_SHADER, true);
                            shadow->SetFaction(14);
                        }
                    }
            }
            if (player->GetQuestObjectiveProgress(QUEST_THE_FIRST_CLEANSING, 8) == 2)
            {
                player->CastSpell(player, SPELL_DEEP_INTROSPECTION_STUN, true);
                if (Creature* mirror = player->FindNearestCreature(159592, 10.f, true))
                    if (Creature* shadow = mirror->SummonCreature(159668, mirror->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
                        if (Creature* shadow1 = mirror->SummonCreature(159668, (mirror->GetPositionX() - 3) + rand32() % 5, (mirror->GetPositionY() - 3) + rand32() % 5, mirror->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            player->PlayConversation(17927);
                            player->CastSpell(shadow, SPELL_MIRROR_IMAGE_AURA, true);
                            player->CastSpell(shadow1, SPELL_MIRROR_IMAGE_AURA, true);
                            shadow->CastSpell(shadow, SPELL_MIRROR_IMAGE_SPAWN_SHADER, true);
                            shadow->CastSpell(shadow1, SPELL_MIRROR_IMAGE_SPAWN_SHADER, true);
                            shadow->SetFaction(14);
                            shadow1->SetFaction(14);
                        }
            }
            if (player->GetQuestObjectiveProgress(QUEST_THE_FIRST_CLEANSING, 8) == 3)
            {
                if (Creature* kleia = player->FindNearestCreature(159583, 50.f, true))
                {
                    player->PlayConversation(17928);
                    player->CastSpell(player, SPELL_DEEP_INTROSPECTION_FADEIN, true);
                    kleia->SummonCreature(159665, -4782.11f, -5346.2f, 6505.36f, 4.35467f, TEMPSUMMON_MANUAL_DESPAWN);  //Anguished Memory
                }
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_gazing_312493::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 311095 Teleport
struct spell_teleport_311095 : public SpellScript
{
    PrepareSpellScript(spell_teleport_311095);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                player->GetScheduler().Schedule(3s, [player](TaskContext context)
                    {
                        if (!player->GetPhaseShift().HasPhase(10149)) PhasingHandler::AddPhase(player, 10149, true);
                        if (player->GetPhaseShift().HasPhase(10148)) PhasingHandler::RemovePhase(player, 10148, true);
                    });

            }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_teleport_311095::HandleAfterCast);
    }
};

Position const KyrianAscendedsPos[12] =
{
    {-4725.84f, -5002.78f, 6535.96f, 2.64538f}, //0 Right_champion
    {-4765.79f, -4996.93f, 6534.01f, 1.65327f}, //1 Right1_Ascended
    {-4756.97f, -4993.79f, 6534.45f, 1.46988f}, //2 Right2_Ascended
    {-4746.55f, -4992.31f, 6534.49f, 1.54345f}, //3 Right3_Ascended
    {-4729.53f, -4976.31f, 6535.72f, 3.63866f}, //4 left_champion
    {-4765.82f, -4983.37f, 6534.03f, 4.28589f}, //5 left1_Ascended
    {-4757.81f, -4984.95f, 6534.49f, 4.37147f}, //6 left2_Ascended
    {-4746.53f, -4985.68f, 6534.49f, 4.20393f}, //7 left3_Ascended
    {-4742.76f, -4990.81f, 6532.81f, 0.166436f}, //8 sophone
    {-4742.64f, -4989.03f, 6532.84f, 0.020087f}, //9 kleia
    {-4742.95f, -4987.73f, 6532.68f, 0.06479f},  //10 pelagos
    {-4680.23f, -4986.11f, 6541.85f, 6.20065f},  //11 Kalisthene
};

// 156217 Kalisthene
struct npc_kalisthene_156217 : public ScriptedAI
{
    npc_kalisthene_156217(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool kyrian_fly;

    void Initialize()
    {
        kyrian_fly = false;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            CloseGossipMenuFor(player);
            Talk(0);
            player->GetScheduler().Schedule(10s, [player, this](TaskContext context)
                {
                    Talk(1);
                    player->KilledMonsterCredit(156217);
                    me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                });
        }
        return true;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
            {
                if (!player->GetPhaseShift().HasPhase(10150)) PhasingHandler::AddPhase(player, 10150, true);
                if (player->GetQuestStatus(QUEST_THE_FIRST_CLEANSING) == QUEST_STATUS_REWARDED && !kyrian_fly)
                {
                    kyrian_fly = true;
                    std::list<Creature*> cList = player->FindAllCreaturesInRange(100.f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* kyrian = *itr)
                        {
                            if (kyrian->GetEntry() == 159703 || kyrian->GetEntry() == 159704)
                                kyrian->AI()->DoAction(ACTION_KYRIAN_FLY_ARRAY);
                            if (kyrian->GetEntry() == 158807) // sophone
                                kyrian->GetMotionMaster()->MovePoint(0, KyrianAscendedsPos[8], true, 0.166436f);
                            if (kyrian->GetEntry() == 158862) // kleia
                                kyrian->GetMotionMaster()->MovePoint(0, KyrianAscendedsPos[9], true, 0.020087f);
                            if (kyrian->GetEntry() == 158861) // pelagos
                                kyrian->GetMotionMaster()->MovePoint(0, KyrianAscendedsPos[10], true, 5.09016f);
                        }
                    }
                }

            }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == 60229 || quest->GetQuestId() == 60226 || quest->GetQuestId() == 60225 || quest->GetQuestId() == 60224
            || quest->GetQuestId() == 60223 || quest->GetQuestId() == 60222 || quest->GetQuestId() == 60221 || quest->GetQuestId() == 6022
            || quest->GetQuestId() == 60219 || quest->GetQuestId() == 60218 || quest->GetQuestId() == 60217 || quest->GetQuestId() == 57715)
        {
            Talk(2);
            me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[11], true);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001) me->DespawnOrUnsummon(1s, 60s);
    }

};

// 159703 Kyrian Ascended, 159704 Champion
struct npc_kyrian_ascended_159703_159704 : public ScriptedAI
{
    npc_kyrian_ascended_159703_159704(Creature* creature) : ScriptedAI(creature) {}

    void DoAction(int32 action) override
    {
        if (action == ACTION_KYRIAN_FLY_ARRAY)
        {
            if (me->GetEntry() == 159704)
            {
                switch (me->GetSpawnId())
                {
                case 346664:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[0], true, 2.64538f);
                    break;
                case 346665:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[4], true, 3.63866f);
                    break;
                }
            }
            if (me->GetEntry() == 159703)
            {
                switch (me->GetSpawnId())
                {
                case 346658:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[1], true, 1.65327f);
                    break;
                case 346659:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[2], true, 1.46988f);
                    break;
                case 346660:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[3], true, 1.54345f);
                    break;
                case 346661:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[5], true, 4.28589f);
                    break;
                case 346662:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[6], true, 4.37147f);
                    break;
                case 346663:
                    me->GetMotionMaster()->MovePoint(1001, KyrianAscendedsPos[7], true, 4.20393f);
                    break;
                }
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
            if (me->GetEntry() == 159703)
            {
                me->SetHover(false);
                me->SetHoverHeight(1);
                me->GetMotionMaster()->MoveFall();
            }
    }

};

// 159749 Accept Kalisthene's gift
struct npc_Kalisthenes_gift_159749 : public ScriptedAI
{
    npc_Kalisthenes_gift_159749(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool accept_gift;

    void Initialize()
    {
        accept_gift = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 3.0f) && !accept_gift)
            {
                TheArchonsAnswer(player, 60229);
                TheArchonsAnswer(player, 60226);
                TheArchonsAnswer(player, 60225);
                TheArchonsAnswer(player, 60224);
                TheArchonsAnswer(player, 60223);
                TheArchonsAnswer(player, 60222);
                TheArchonsAnswer(player, 60221);
                TheArchonsAnswer(player, 60220);
                TheArchonsAnswer(player, 60219);
                TheArchonsAnswer(player, 60218);
                TheArchonsAnswer(player, 60217);
                TheArchonsAnswer(player, 57715);
            }
    }

    void TheArchonsAnswer(Player* player, uint32 quest)
    {
        if (player->HasQuest(quest))
        {
            if (player->GetQuestObjectiveProgress(quest, 0) && !player->GetQuestObjectiveProgress(quest, 2))
            {
                accept_gift = true;
                player->CastSpell(player, SPELL_EXTRA_ACTION_BUTTON, true);
                me->AddDelayedEvent(60000, [this]() ->void
                    {
                        accept_gift = false;
                    });
            }
        }
    }
};

// 311245 Kalisthene's Grace
class spell_kalisthenes_grace_311245 : public SpellScript
{
    PrepareSpellScript(spell_kalisthenes_grace_311245);

    void HandleEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster()->ToPlayer())
            caster->CastSpell(caster, SPELL_PLACEHOLDER_RP_PING, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_kalisthenes_grace_311245::HandleEffect, EFFECT_1, SPELL_EFFECT_SEND_EVENT);
    }
};

// 175014 Adonas
struct npc_adonas_175014 : public ScriptedAI
{
    npc_adonas_175014(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool near_player;

    void Reset()override
    {
        me->SetStandState(UNIT_STAND_STATE_KNEEL);
        me->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
    }
    void Initialize()
    {
        near_player = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 10.0f) && !near_player && player->GetQuestStatus(QUEST_RSVP_SIKA) == QUEST_STATUS_INCOMPLETE)
            {
                near_player = true;
                Talk(0);
                Talk(1);
            }
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id != SPELL_BANDAGING)
            return;

        Talk(2);
        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
        me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            CloseGossipMenuFor(player);
            Talk(3);
            player->AddItem(180965, 1);
            me->CanFly();
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->AddDelayedEvent(5000, [this]()-> void
                {
                    me->SetHover(true);
                    me->SetHoverHeight(30.f);
                    me->DespawnOrUnsummon(10s, 60s);
                });
        }
        return true;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001) me->DespawnOrUnsummon(1s, 60s);
    }
};

// 157673 Disciple Kosmas
struct npc_disciple_kosmas_157673 : public ScriptedAI
{
    npc_disciple_kosmas_157673(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool sayToAspirant;
    ObjectGuid m_playerGUID;

    void  Initialize()
    {
        sayToAspirant = false;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_TEMPLE_OF_PURITY)
        {
            Talk(0);
            me->SetHover(true);
            me->SetHoverHeight(15);
            me->AddDelayedEvent(5000, [this]() ->void
                {
                    if (Creature* pelagos = me->FindNearestCreature(158861, 20.f, true))
                        if (Creature* kleia = me->FindNearestCreature(158471, 20.f, true))
                        {
                            pelagos->AI()->SetData(2, 1);     //ToSmartAI
                            kleia->AI()->SetData(2, 1);       //ToSmartAI
                        };
                    me->SetCanFly(true);
                    me->GetMotionMaster()->MovePoint(0, Position(-4083.94f, -4687.15f, 6560.7f), true);
                    me->DespawnOrUnsummon(5s, 60s);
                });
        }
        if (quest->GetQuestId() == QUEST_ON_THE_EDGE_OF_A_REVELATION)
        {
            sayToAspirant = false;
            Talk(1);
        }
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        if (gossipListId == 0)
        {
            m_playerGUID = player->GetGUID();
            CloseGossipMenuFor(player);
            player->KilledMonsterCredit(157673);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            if (Creature* lysonis = me->FindNearestCreature(164129, 30.f, true))
            {
                lysonis->SetRegenerateHealth(false);
                lysonis->SetHealth(lysonis->CountPctFromMaxHealth(40));
                lysonis->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                lysonis->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                me->GetMotionMaster()->MovePoint(0, -4026.53f, -5724.35f, 6728.55f, true);
                me->AddDelayedEvent(4000, [lysonis, this]() -> void
                    {
                        lysonis->AI()->SetData(3, 1);     //ToSmartAI
                        me->Attack(lysonis, true);
                    });
            }
        }
        return true;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
            {
                if (player->GetQuestStatus(QUEST_ON_THE_EDGE_OF_A_REVELATION) == QUEST_STATUS_NONE && !sayToAspirant)
                {
                    sayToAspirant = true;
                    Talk(2);
                }
                if (player->GetQuestStatus(QUEST_THE_ENEMY_YOU_KNOW) == QUEST_STATUS_REWARDED && me->HasAura(65050))
                    me->RemoveAura(65050);
            }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 3 && data == 1)
        {
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            me->AddAura(327433, me); //kneel
            me->AddDelayedEvent(2000, [this]() -> void
                {
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                });
        }
        if (type == 3 && data == 3)
        {
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->GetMotionMaster()->MoveTargetedHome();
            me->DespawnOrUnsummon(4s, 120s);
        }
    }

};

// 157242 168852 168853 Vulnerable Aspirant
struct npc_vulnerable_aspirant : public ScriptedAI
{
    npc_vulnerable_aspirant(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    uint8 death_count;

    void Initialize()
    {
        death_count = 0;
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
    }

    void Reset()override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        CloseGossipMenuFor(player);
        if (player->HasQuest(QUEST_ON_THE_EDGE_OF_A_REVELATION))
        {
            switch (gossipListId)
            {
            case 0:
            case 1:
            case 2:
                if (m_playerGUID.IsEmpty())
                    m_playerGUID = player->GetGUID();
                me->AI()->DoAction(urand(0, 2));
                break;
            }
        }
        return true;
    }

    void DoAction(int32 action) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            switch (action)
            {
            case 0:
                PickSafeAnswer();
                player->KilledMonsterCredit(157242);
                break;
            case 1:
                CongealedDoubtSummon();
                break;
            case 2:
                AspirantTurnHostile();
                break;
            }
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        Player* player = killer->ToPlayer();
        death_count++;
        switch (death_count)
        {
        case 3:
            me->SetAIAnimKitId(0);
            me->RemoveAura(AURA_SUCCUMB_TO_DOUBT);
            player->KilledMonsterCredit(157242);
            MoveForward(me, 15.f);
            me->ForcedDespawn(5000);
            Talk(2);
            death_count = 0;
            break;
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (Player* player = attacker->ToPlayer())
        {
            if (me->GetHealth() <= damage)
            {
                damage = 0;
                me->SetFaction(35);
                me->AttackStop();
                me->SetFullHealth();
                me->RemoveAura(AURA_SUCCUMB_TO_DOUBT);
                player->KilledMonsterCredit(157242);
                me->AddDelayedEvent(2000, [this]() ->void
                    {
                        Talk(3);
                        me->SetAIAnimKitId(0);
                        MoveForward(me, 20.f);
                        me->ForcedDespawn(5000);
                    });
            }
        }
    }

    void PickSafeAnswer()
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        MoveForward(me, 10.f);
        me->ForcedDespawn(5000);
        Talk(0);
    }

    void CongealedDoubtSummon()
    {
        Talk(1);
        me->SetAIAnimKitId(18155); //dizzy
        me->AddAura(AURA_SUCCUMB_TO_DOUBT, me);
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        me->SummonCreature(157560, (me->GetPositionX() - 3) + rand32() % 5, (me->GetPositionY() - 3) + rand32() % 5, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
        me->AddDelayedEvent(2000, [this]() ->void
            {
                me->SummonCreature(157560, (me->GetPositionX() - 3) + rand32() % 5, (me->GetPositionY() - 3) + rand32() % 5, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
            });
        me->AddDelayedEvent(4000, [this]() ->void
            {
                me->SummonCreature(157560, (me->GetPositionX() - 3) + rand32() % 5, (me->GetPositionY() - 3) + rand32() % 5, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
            });
    }

    void AspirantTurnHostile()
    {
        if (Creature* purity = me->FindNearestCreature(159791, 15.f, true))
        {
            purity->AI()->Talk(0);
            purity->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            purity->ForcedDespawn(7000);
            me->AddDelayedEvent(2000, [this, purity]() ->void
                {
                    MoveForward(purity, 20.f);
                });
        }
        me->SetAIAnimKitId(0);
        me->AddAura(AURA_SUCCUMB_TO_DOUBT, me);
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetFaction(16);
    }

    void MoveForward(Unit* creature, float distance)
    {
        Position movePos;
        float ori = creature->GetOrientation();
        float x = creature->GetPositionX() + distance * cos(ori);
        float y = creature->GetPositionY() + distance * sin(ori);
        float z = creature->GetPositionZ();
        creature->GetNearPoint2D(creature, x, y, distance, ori);
        movePos = { x, y, z, ori };  creature->SetWalk(true);
        creature->GetMotionMaster()->MovePoint(0, movePos, false);
    }
};

Position const DanysiasBurdenPos[4] =
{
    {-4029.39f, -5247.11f, 6530.88f, 1.32803f}, //0 Woman
    {-4019.36f, -5249.31f, 6530.88f, 2.40011f}, //1 Knight
    {-4024.63f, -5240.59f, 6530.88f, 4.82217f}, //2 Burden
    {-4032.69f, -5242.99f, 6536.83f, 6.01203f}, //3 Fotima_move
};

// 159840 Disciple Fotima
struct npc_disciple_fotima_159840 : public ScriptedAI
{
    npc_disciple_fotima_159840(Creature* creature) : ScriptedAI(creature) {}
private:
    ObjectGuid m_playerGUID;
    ObjectGuid KBurdenGUID;
    ObjectGuid WBurdenGUID;
    ObjectGuid BurdenGUID;

    void Reset()override
    {
        m_playerGUID = ObjectGuid::Empty;
        KBurdenGUID = ObjectGuid::Empty;
        WBurdenGUID = ObjectGuid::Empty;
        BurdenGUID = ObjectGuid::Empty;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        CloseGossipMenuFor(player);
        if (!player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 1) && gossipListId == 0)
        {
            m_playerGUID = player->GetGUID();
            Talk(0);
            me->AddDelayedEvent(3000, [this]() -> void
                {
                    Talk(1);
                    if (Creature* wburden = me->SummonCreature(160650, DanysiasBurdenPos[0], TEMPSUMMON_TIMED_DESPAWN, 1min))
                        WBurdenGUID = wburden->GetGUID();
                    if (Creature* kburden = me->SummonCreature(160650, DanysiasBurdenPos[1], TEMPSUMMON_TIMED_DESPAWN, 1min))
                        KBurdenGUID = kburden->GetGUID();
                    if (Creature* burden = me->SummonCreature(160650, DanysiasBurdenPos[2], TEMPSUMMON_TIMED_DESPAWN, 1min))
                        BurdenGUID = burden->GetGUID();
                    if (Creature* danysia = me->FindNearestCreature(160645, 20.f, true))
                    {
                        danysia->AI()->Talk(0);
                        danysia->AddAura(AURA_GENERIC_LIFE_BURDEN_SHADER, danysia);
                        danysia->AddAura(AURA_PERMANENT_FEIGN_DEATH, danysia);
                        danysia->SetHover(true);
                        danysia->SetHoverHeight(2);
                    }
                    me->GetMotionMaster()->MovePoint(1001, DanysiasBurdenPos[3], true, 6.01203f);
                });
        }
        return true;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            Talk(2);
            if (Creature* VesperC = me->FindNearestCreature(157622, 20.f, true))
                me->CastSpell(VesperC, SPELL_FLYING_KYRING_BEAM_CHANNEL, true);
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_RINGING_VESPER_OF_PURITY)
        {
            if (Creature* Wburden1 = ObjectAccessor::GetCreature(*me, WBurdenGUID))
                if (Creature* Kburden1 = ObjectAccessor::GetCreature(*me, KBurdenGUID))
                    if (Creature* burden1 = ObjectAccessor::GetCreature(*me, BurdenGUID))
                    {
                        me->SummonCreature(160655, Wburden1->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 10min);
                        me->SummonCreature(160653, Kburden1->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 10min);
                        me->SummonCreature(160654, burden1->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 10min);
                        Wburden1->ForcedDespawn();
                        Kburden1->ForcedDespawn();
                        burden1->ForcedDespawn();

                    }
            if (me->FindNearestCreature(160655, 20.f, true))
            {
                me->AddDelayedEvent(1000, [this]() -> void { Talk(3); });
                me->AddDelayedEvent(9000, [this]() -> void { Talk(4); });
            }
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        if (summon->GetEntry() == 160654)
        {
            if (Creature* kbur = me->FindNearestCreature(160653, 20.f, true))
                kbur->ForcedDespawn();
            if (Creature* wbur = me->FindNearestCreature(160655, 20.f, true))
                wbur->ForcedDespawn();
            if (Creature* danysia = me->FindNearestCreature(160645, 20.f, true))
            {
                me->CastStop();
                me->GetMotionMaster()->MoveTargetedHome();
                Talk(5);
                danysia->RemoveAura(AURA_GENERIC_LIFE_BURDEN_SHADER);
                danysia->RemoveAura(AURA_PERMANENT_FEIGN_DEATH);
                danysia->SetHover(false);
                danysia->SetHoverHeight(1);
                me->AddDelayedEvent(4000, [danysia, this]() -> void
                    {
                        me->SetFacingToObject(danysia);
                        danysia->AI()->Talk(1);
                    });
                me->AddDelayedEvent(7000, [this]() -> void {   Talk(6); });
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    me->AddDelayedEvent(11000, [this, player]() -> void
                        {
                            me->SetFacingToObject(player);
                            Talk(7, player);
                        });
            }
        }
    }
};

// 334847 Vesper of Purity
struct go_vesper_of_purity_334847 : public GameObjectAI
{
    go_vesper_of_purity_334847(GameObject* go) : GameObjectAI(go) { }

    bool OnReportUse(Player* player) override
    {
        if (!player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 1))
        {
            if (Creature* fotima = player->FindNearestCreature(159840, 20.f, true))
                fotima->AI()->DoAction(ACTION_RINGING_VESPER_OF_PURITY);
        }
        if (player->HasQuest(QUEST_A_ONCE_SWEET_SOUND))
        {
            player->KilledMonsterCredit(157622);
        }
        return true;
    }
};

// 165110 Pelagos
struct npc_pelagos_165110 : public ScriptedAI
{
    npc_pelagos_165110(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool first_say;
    bool second_asy;
    bool third_say;

    void Initialize()
    {
        first_say = false;
        second_asy = false;
        third_say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
            {
                if (!player->FindNearestCreature(159840, 50.f, true) &&
                    player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 1) &&
                    !player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 2) && !first_say)
                {
                    first_say = true;
                    Talk(2);
                }
                if (!player->FindNearestCreature(160636, 50.f, true) &&
                    player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 2) &&
                    !player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 3) && !second_asy)
                {
                    second_asy = true;
                    Talk(3);
                }
                if (!player->FindNearestCreature(159841, 50.f, true) &&
                    player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 3) && !third_say)
                {
                    third_say = true;
                    Talk(4);
                    me->ForcedDespawn(10000);
                }
            }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            if (!player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 1))
                me->AddDelayedEvent(25000, [this]() -> void { Talk(1); });
    }
};

Position const GalistosBurdenBurdenPos[15] =
{
    {-4234.44f, -5300.69f, 6515.14f, 4.39227f}, //0 GalistosBurden - BicDrum - aura312726
    {-4236.15f, -5304.62f, 6515.14f, 1.10884f}, //1 FX Stalker - BicDrumAura_326935
    {-4227.03f, -5297.43f, 6515.14f, 4.21171f}, //2 GalistosBurden - Canoe   - aura312726, CanoeAura_326928
    {-4225.01f, -5289.29f, 6515.14f, 2.17151f}, //3 GalistosBurden - aura312726
    {-4223.81f, -5285.54f, 6515.14f, 3.65906f}, //4 GalistosBurden - Cookfire - aura312726 - sit
    {-4226.21f, -5287.14f, 6515.14f, 1.10884f}, //5 FX Stalker - Cookfire_326932
    {-4239.58f, -5290.89f, 6514.97f, 2.97756f}, //6 Helene_Pos1
    {-4230.06f, -5281.86f, 6514.99f, 1.85837f}, //7 Helene_Pos2
    {-4235.74f, -5285.42f, 6514.97f, 2.45135f}, //8 Helene_Pos3
    {-4236.94f, -5298.08f, 6514.97f, 1.54421f}, //9  GalistosBurden1
    {-4230.61f, -5307.91f, 6514.97f, 2.33241f}, //10 GalistosBurden2
    {-4219.62f, -5292.78f, 6514.97f, 1.98211f}, //11 GalistosBurden3
    {-4230.97f, -5295.83f, 6514.97f, 0.80096f}, //12 GalistosBurden1
    {-4228.78f, -5293.59f, 6514.97f, 2.12043f}, //13 FX Stalker - babyTauren_326944
    {-4226.51f, -5291.65f, 6514.97f, 3.67159f}, //14 GalistosBurden2
};

// 160636 Disciple Helene
struct npc_disciple_helene_160636 : public ScriptedAI
{
    npc_disciple_helene_160636(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    ObjectGuid DBurdenGUID;
    ObjectGuid FBurdenGUID;
    uint8 burden_stage;
    uint8 death_count;

    void Initialize()
    {
        burden_stage = 0;
        death_count = 0;
    }

    void Reset()override
    {
        m_playerGUID = ObjectGuid::Empty;
        DBurdenGUID = ObjectGuid::Empty;
        FBurdenGUID = ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            if (DBurdenGUID.IsEmpty())
            {
                if (Creature* drum = player->FindNearestCreature(156279, 10.f, true))
                    if (drum->HasAura(AURA_BIG_DRUM))
                        drum->ForcedDespawn();
                return;
            }
            if (FBurdenGUID.IsEmpty())
            {
                if (Creature* cook = player->FindNearestCreature(156279, 10.f, true))
                    if (cook->HasAura(AURA_COOKFIRE))
                        cook->ForcedDespawn();
                return;
            }
        }
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        CloseGossipMenuFor(player);
        if (player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 1) &&
            !player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 2) && gossipListId == 0)
        {
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            m_playerGUID = player->GetGUID();
            Talk(0, player);
            me->SetWalk(true);
            me->AddDelayedEvent(4000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1001, GalistosBurdenBurdenPos[6], true, 2.97756f);
                });
        }
        return true;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (id)
        {
        case 1001:
            Talk(1);
            me->AddDelayedEvent(4000, [this]() -> void
                {
                    me->CastSpell(me, SPELL_HELENE_RINGING, true);
                    if (Creature* galistos = me->FindNearestCreature(160647, 20.f, true))
                        galistos->AddAura(AURA_GENERIC_LIFE_BURDEN_SHADER, galistos);
                    SummonGalistosBurdenI();
                });
            me->AddDelayedEvent(7000, [this]() -> void
                {
                    Talk(2);
                    me->GetMotionMaster()->MovePoint(1002, GalistosBurdenBurdenPos[8], true, 5.5442f);
                });
            break;
        case 1002:
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                Talk(3, player);
            break;
        case 1003:
            me->CastSpell(me, SPELL_HELENE_RINGING, true);
            SummonGalistosBurdenII();
            break;
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        switch (burden_stage)
        {
        case 1:
            death_count++;
            if (summon->GetGUID() == DBurdenGUID) DBurdenGUID = ObjectGuid::Empty;
            if (summon->GetGUID() == FBurdenGUID) FBurdenGUID = ObjectGuid::Empty;
            if (death_count == 4)
            {
                death_count = 0;
                Talk(4);
                me->GetMotionMaster()->MovePoint(1003, GalistosBurdenBurdenPos[7], true, 2.45135f);
                StalkerForceDespawn();
            }
            break;
        case 2:
            death_count++;
            if (death_count == 3)
            {
                death_count = 0;
                Talk(5);
                me->CastSpell(me, SPELL_HELENE_RINGING, true);
                SummonGalistosBurdenIII();
                me->AddDelayedEvent(5000, [this]() ->void {  Talk(6); });
            }
            break;
        case 3:
            death_count++;
            if (death_count == 2)
            {
                if (Creature* baby = me->FindNearestCreature(156279, 30.f, true))
                    if (baby->HasAura(AURA_BABY_TAUREN))
                        baby->ForcedDespawn();
                Talk(7);
                me->AddDelayedEvent(2000, [this]() ->void
                    {
                        me->CastSpell(me, SPELL_HELENE_RINGING, true);
                        if (Creature* galistos = me->FindNearestCreature(160647, 20.f, true))
                            galistos->AI()->Talk(0);
                        SummonGalistosBurdenIV();
                    });
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    me->AddDelayedEvent(5000, [this, player]() ->void {  Talk(8, player); });
                death_count = 0;
            }
            break;
        case 4:
            death_count++;
            if (summon->GetEntry() == 160698)
            {
                if (summon->GetGUID() == DBurdenGUID) DBurdenGUID = ObjectGuid::Empty;
                if (summon->GetGUID() == FBurdenGUID) FBurdenGUID = ObjectGuid::Empty;
            }
            if (summon->GetEntry() == 160694)
            {
                if (Creature* baby = me->FindNearestCreature(156279, 30.f, true))
                    if (baby->HasAura(AURA_BABY_TAUREN))
                        baby->ForcedDespawn();
            }
            if (death_count == 9)
            {
                death_count = 0;
                Talk(9);
                if (Creature* galistos = me->FindNearestCreature(160647, 20.f, true))
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        me->AddDelayedEvent(4000, [this]() ->void {  Talk(10); });
                        me->AddDelayedEvent(7000, [galistos]() ->void
                            {
                                galistos->RemoveAura(AURA_GENERIC_LIFE_BURDEN_SHADER);
                                galistos->AI()->Talk(1);
                            });
                        me->AddDelayedEvent(10000, [this, player]() ->void
                            {
                                player->KilledMonsterCredit(160636);
                                Talk(11);
                                me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                            });
                    }
                StalkerForceDespawn();
            }
            break;
        }
    }

    void SummonGalistosBurdenI()
    {
        burden_stage = 1;
        me->SummonCreature(160698, GalistosBurdenBurdenPos[3], TEMPSUMMON_TIMED_DESPAWN, 2min);
        if (Creature* Dburden = me->SummonCreature(160698, GalistosBurdenBurdenPos[0], TEMPSUMMON_TIMED_DESPAWN, 2min))
        {
            DBurdenGUID = Dburden->GetGUID();
            Dburden->SetEmoteState(EMOTE_STATE_WORK_CHOPWOOD_2);
            if (Creature* DrumStalker = Dburden->SummonCreature(156279, GalistosBurdenBurdenPos[1], TEMPSUMMON_TIMED_DESPAWN, 2min))
                DrumStalker->AddAura(AURA_BIG_DRUM, DrumStalker);
        }
        if (Creature* Cburden = me->SummonCreature(160698, GalistosBurdenBurdenPos[2], TEMPSUMMON_TIMED_DESPAWN, 2min))
            Cburden->AddAura(AURA_CANOE, Cburden);
        if (Creature* Fburden = me->SummonCreature(160698, GalistosBurdenBurdenPos[4], TEMPSUMMON_TIMED_DESPAWN, 2min))
        {
            FBurdenGUID = Fburden->GetGUID();
            if (Creature* CookStalker = Fburden->SummonCreature(156279, GalistosBurdenBurdenPos[5], TEMPSUMMON_TIMED_DESPAWN, 2min))
            {
                Fburden->SetStandState(UNIT_STAND_STATE_SIT);
                CookStalker->AddAura(AURA_COOKFIRE, CookStalker);
            }
        }
    }

    void SummonGalistosBurdenII()
    {
        burden_stage = 2;
        me->GetMotionMaster()->MovePoint(0, GalistosBurdenBurdenPos[8], true, 1.85837f);
        me->SummonCreature(160695, GalistosBurdenBurdenPos[9], TEMPSUMMON_TIMED_DESPAWN, 2min);
        me->SummonCreature(160695, GalistosBurdenBurdenPos[10], TEMPSUMMON_TIMED_DESPAWN, 2min);
        me->SummonCreature(160695, GalistosBurdenBurdenPos[11], TEMPSUMMON_TIMED_DESPAWN, 2min);
    }

    void SummonGalistosBurdenIII()
    {
        burden_stage = 3;
        if (Creature* Bburden = me->SummonCreature(160694, GalistosBurdenBurdenPos[12], TEMPSUMMON_TIMED_DESPAWN, 2min))
            if (Creature* BabyStalker = Bburden->SummonCreature(156279, GalistosBurdenBurdenPos[13], TEMPSUMMON_TIMED_DESPAWN, 2min))
                BabyStalker->AddAura(AURA_BABY_TAUREN, BabyStalker);
        me->SummonCreature(160694, GalistosBurdenBurdenPos[14], TEMPSUMMON_TIMED_DESPAWN, 2min);
    }

    void SummonGalistosBurdenIV()
    {
        burden_stage = 4;
        me->SummonCreature(160698, GalistosBurdenBurdenPos[3], TEMPSUMMON_TIMED_DESPAWN, 2min);
        if (Creature* Dburden = me->SummonCreature(160698, GalistosBurdenBurdenPos[0], TEMPSUMMON_TIMED_DESPAWN, 2min))
        {
            DBurdenGUID = Dburden->GetGUID();
            Dburden->SetEmoteState(EMOTE_STATE_WORK_CHOPWOOD_2);
            if (Creature* DrumStalker = Dburden->SummonCreature(156279, GalistosBurdenBurdenPos[1], TEMPSUMMON_TIMED_DESPAWN, 2min))
                DrumStalker->AddAura(AURA_BIG_DRUM, DrumStalker);
        }
        if (Creature* Cburden = me->SummonCreature(160698, GalistosBurdenBurdenPos[2], TEMPSUMMON_TIMED_DESPAWN, 2min))
            Cburden->AddAura(AURA_CANOE, Cburden);
        if (Creature* Fburden = me->SummonCreature(160698, GalistosBurdenBurdenPos[4], TEMPSUMMON_TIMED_DESPAWN, 2min))
        {
            FBurdenGUID = Fburden->GetGUID();
            if (Creature* CookStalker = Fburden->SummonCreature(156279, GalistosBurdenBurdenPos[5], TEMPSUMMON_TIMED_DESPAWN, 2min))
            {
                Fburden->SetStandState(UNIT_STAND_STATE_SIT);
                CookStalker->AddAura(AURA_COOKFIRE, CookStalker);
            }
        }

        me->SummonCreature(160695, GalistosBurdenBurdenPos[9], TEMPSUMMON_TIMED_DESPAWN, 2min);
        me->SummonCreature(160695, GalistosBurdenBurdenPos[10], TEMPSUMMON_TIMED_DESPAWN, 2min);
        me->SummonCreature(160695, GalistosBurdenBurdenPos[11], TEMPSUMMON_TIMED_DESPAWN, 2min);

        if (Creature* Bburden = me->SummonCreature(160694, GalistosBurdenBurdenPos[12], TEMPSUMMON_TIMED_DESPAWN, 2min))
            if (Creature* BabyStalker = Bburden->SummonCreature(156279, GalistosBurdenBurdenPos[13], TEMPSUMMON_TIMED_DESPAWN, 2min))
                BabyStalker->AddAura(AURA_BABY_TAUREN, BabyStalker);
        me->SummonCreature(160694, GalistosBurdenBurdenPos[14], TEMPSUMMON_TIMED_DESPAWN, 2min);
    }

    void StalkerForceDespawn()
    {
        std::list<Creature*> cList = me->FindNearestCreatures(156279, 30.0f);
        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
        {
            if (Creature* Stalker = *itr)
                Stalker->ForcedDespawn();
        }
    }
};

Position const DiscipleLykastePos[6] =
{
    {-4144.22f, -5426.43f, 6500.64f, 5.09404f}, //0 LykastePos01
    {-4143.68f, -5437.27f, 6500.64f, 6.13469f}, //1 LykastePos02
    {-4143.68f, -5437.27f, 6510.69f, 6.08914f}, //2 LykastePos03 - fly
    {-4146.65f, -5427.55f, 6505.33f, 0.54031f}, //3 LykastePos04 - fly
    {-4127.71f, -5425.99f, 6500.72f, 1.45271f}, //3 ErisneBurdenSummonPos -  160709
};

// 159841 Disciple Lykaste
struct npc_disciple_lykaste_159841 : public ScriptedAI
{
    npc_disciple_lykaste_159841(Creature* creature) : ScriptedAI(creature) {}
private:
    ObjectGuid m_playerGUID;
    ObjectGuid erisneGUID;
    ObjectGuid burdenGUID;
    ObjectGuid FerisneGUID;

    void Reset()override
    {
        m_playerGUID = ObjectGuid::Empty;
        erisneGUID = ObjectGuid::Empty;
        burdenGUID = ObjectGuid::Empty;
        FerisneGUID = ObjectGuid::Empty;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        CloseGossipMenuFor(player);
        if (player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 2) &&
            !player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 3) && gossipListId == 0)
        {
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            m_playerGUID = player->GetGUID();
            Talk(0);
            me->SetWalk(true);
            me->AddDelayedEvent(4000, [this]() -> void
                {
                    if (Creature* erisne = me->FindNearestCreature(160646, 20.f, true))
                    {
                        erisneGUID = erisne->GetGUID();
                        erisne->AI()->Talk(0);
                    }
                    me->GetMotionMaster()->MovePoint(1001, DiscipleLykastePos[0], true, 5.09404f);
                });
        }
        return true;
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        switch (id)
        {
        case 1001:
            Talk(1);
            me->GetMotionMaster()->MovePoint(1002, DiscipleLykastePos[1], true, 6.13469f);
            break;
        case 1002:
            me->SetHover(true);
            me->SetHoverHeight(10.f);
            me->GetMotionMaster()->MovePoint(1003, DiscipleLykastePos[2], true, 6.08914f);
            break;
        case 1003:
            Talk(2);
            if (Creature* hormony = me->FindNearestCreature(157625, 30.f, true))
                me->CastSpell(hormony, SPELL_FLYING_KYRING_BEAM_CHANNEL);
            if (GameObject* hormony1 = me->FindNearestGameObject(334849, 30.f, false))
                hormony1->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
            break;
        case 1004:
            if (Creature* erisne = ObjectAccessor::GetCreature(*me, erisneGUID))
                if (Creature* burden = ObjectAccessor::GetCreature(*me, burdenGUID))
                {
                    burden->AI()->Talk(0);
                    me->AddDelayedEvent(4000, [erisne]() ->void { erisne->AI()->Talk(1); });
                    me->AddDelayedEvent(7000, [burden]() ->void { burden->AI()->Talk(1); });
                    me->AddDelayedEvent(10000, [this, burden, erisne]() ->void
                        {
                            Talk(3);
                            burden->GetMotionMaster()->MovePoint(0, erisne->GetPosition(), true);
                        });
                    me->AddDelayedEvent(12000, [this, burden, erisne]() ->void
                        {
                            burden->ForcedDespawn();
                            if (Creature* fallenE = me->SummonCreature(168031, erisne->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
                                FerisneGUID = fallenE->GetGUID();
                            erisne->AddAura(65050, erisne);
                        });
                }
            break;
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_RINGING_VESPER_OF_HARMONY)
        {
            if (Creature* erisne = ObjectAccessor::GetCreature(*me, erisneGUID))
                erisne->AddAura(AURA_GENERIC_LIFE_BURDEN_SHADER, erisne);
            if (Creature* burden1 = me->SummonCreature(160710, DiscipleLykastePos[4], TEMPSUMMON_TIMED_DESPAWN, 1min))
            {
                me->AddDelayedEvent(3000, [burden1, this]() ->void
                    {
                        if (Creature* burden2 = me->SummonCreature(160709, DiscipleLykastePos[4], TEMPSUMMON_TIMED_DESPAWN, 1min))
                            burdenGUID = burden2->GetGUID();
                        burden1->ForcedDespawn();
                    });
            }
            me->AddDelayedEvent(3500, [this]() ->void
                {
                    me->CastStop();
                    me->GetMotionMaster()->MovePoint(1004, DiscipleLykastePos[3], true, 0.54031f);
                    me->SetHoverHeight(4);
                });
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 1 && data == 1)
        {
            Talk(6);
            me->AddDelayedEvent(6000, [this]()->void { Talk(7); });
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                me->AddDelayedEvent(13000, [this, player]()->void
                    {
                        if (Creature* erisne = ObjectAccessor::GetCreature(*me, erisneGUID))
                        {
                            erisne->RemoveAura(AURA_GENERIC_LIFE_BURDEN_SHADER);
                            erisne->RemoveAura(65050);
                        }
                        me->SetFacingToObject(player);
                        player->KilledMonsterCredit(159841);
                        player->ForceCompleteQuest(QUEST_A_TEMPLE_IN_NEED);
                        Talk(8);
                    });
                me->AddDelayedEvent(17000, [this]()->void
                    {
                        me->SetHover(false);
                        me->SetHoverHeight(1);
                        me->GetMotionMaster()->MoveTargetedHome();
                        me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    });
            }
        }
    }
};

// 334849 Vesper of Harmony
struct go_vesper_of_harmony_334849 : public GameObjectAI
{
    go_vesper_of_harmony_334849(GameObject* go) : GameObjectAI(go) {}

    bool OnReportUse(Player* player) override
    {
        if (!player->GetQuestObjectiveProgress(QUEST_A_TEMPLE_IN_NEED, 3))
        {
            me->SetFlag(GO_FLAG_NOT_SELECTABLE);
            if (Creature* lykaste = player->FindNearestCreature(159841, 20.f, true))
                lykaste->AI()->DoAction(ACTION_RINGING_VESPER_OF_HARMONY);
        }
        if (player->HasQuest(QUEST_A_ONCE_SWEET_SOUND))
        {
            player->KilledMonsterCredit(157625);
        }
        return true;
    }
};

// 159897 Disciple Nikolon
struct npc_disciple_nikolon_159897 : public ScriptedAI
{
    npc_disciple_nikolon_159897(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    uint8 movement_stage;
    uint8 death_count;

    void Initialize()
    {
        movement_stage = 0;
        death_count = 0;
    }

    void Reset()override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            m_playerGUID = player->GetGUID();
            me->GetMotionMaster()->MovePath(15989701, false);
            movement_stage = 1;
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            if (movement_stage == 1)
            {
                switch (id)
                {
                case 0:
                    Talk(0);
                    break;
                case 2:
                    Talk(1);
                    break;
                case 4:
                    Talk(2);
                    break;
                case 11:
                    Talk(3);
                    me->AddDelayedEvent(4000, [this]() -> void
                        {
                            me->SetAIAnimKitId(12130); //cower
                            for (uint8 i = 0; i < 3; i++)
                            {
                                if (Creature* bitterness = me->SummonCreature(159792, (me->GetPositionX() - 5) + rand32() % 5, (me->GetPositionY() - 5) + rand32() % 5, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                                        bitterness->Attack(player, true);
                                }
                            }
                        });
                    break;
                }
            }
            if (movement_stage == 2)
            {
                switch (id)
                {
                case 0:
                    Talk(4);
                    break;
                case 6:
                    me->PauseMovement();
                    Talk(5);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    me->AddDelayedEvent(5000, [this]() -> void
                        {
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            me->ResumeMovement();
                            Talk(6);
                        });
                    break;
                case 17:
                    Talk(7);
                    me->AddDelayedEvent(3000, [this]() -> void
                        {
                            me->HandleEmoteCommand(EMOTE_STATE_COWER);
                            if (Creature* hatred = me->SummonCreature(160863, (me->GetPositionX() - 5) + rand32() % 5, (me->GetPositionY() - 5) + rand32() % 5, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                                    hatred->Attack(player, true);
                        });
                    break;
                }
            }
            if (movement_stage == 3)
            {
                switch (id)
                {
                case 0:
                    Talk(8);
                    break;
                case 4:
                    if (Creature* eridia = me->FindNearestCreature(159762, 10.f, true))
                        eridia->AI()->Talk(3);
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        player->ForceCompleteQuest(QUEST_A_WAYWARD_DISCIPLE);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    me->AddDelayedEvent(1500, [this]()->void
                        {
                            if (Creature* nikolon = me->FindNearestCreature(159899, 10.f, true))
                                nikolon->RemoveAura(65050);
                            me->ForcedDespawn(500);
                        });
                    break;
                }
            }
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == 159792)
        {
            death_count++;
            if (death_count == 3)
            {
                me->SetAIAnimKitId(0);
                death_count = 0;
                movement_stage = 2;
                me->GetMotionMaster()->MovePath(15989702, false);
            }
        }
        if (summon->GetEntry() == 160863)
        {
            movement_stage = 3;
            me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
            me->GetMotionMaster()->MovePath(15989703, false);
        }
    }
};

Position NikolonsUncertainty[9] =
{
    {-4321.71f, -5428.02f, 6511.04f, 5.09404f}, //0 Nikolon's Uncertainty1
    {-4326.71f, -5428.53f, 6511.04f, 6.13469f}, //1 Nikolon's Uncertainty2
    {-4328.35f, -5423.94f, 6511.04f, 6.08914f}, //2 Nikolon's Uncertainty3
    {-4320.96f, -5419.64f, 6511.04f, 0.54031f}, //3 Nikolon's Uncertainty4
    {-4325.77f, -5416.67f, 6511.04f, 1.45271f}, //4 Nikolon's Uncertainty5
    {-4320.27f, -5425.63f, 6511.04f, 0.54031f}, //5 Nikolon's Doubts1
    {-4328.65f, -5421.69f, 6511.04f, 1.45271f}, //6 Nikolon's Doubts2
    {-4325.17f, -5423.74f, 6511.04f, 4.27895f}, //7 Nikolon's Despair
    {-4312.19f, -5408.17f, 6514.88f, 3.98681f}, //8 pelagos_summonPos
};

// 159906 Eridia
struct npc_eridia_159906 : public ScriptedAI
{
    npc_eridia_159906(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    ObjectGuid nikolonGUID;
    uint8 death_count5;
    uint8 death_count3;
    uint8 death_count2;

    void Initialize()
    {
        death_count5 = 0;
        death_count3 = 0;
        death_count2 = 0;
    }

    void Reset()override
    {
        m_playerGUID = ObjectGuid::Empty;
        nikolonGUID = ObjectGuid::Empty;
    }

    bool OnGossipHello(Player* player) override
    {
        m_playerGUID = player->GetGUID();
        if (!player->GetQuestObjectiveProgress(QUEST_STEP_BACK_FROM_THAT_LEDGE_MY_FRIEND, 0))
        {
            AddGossipItemFor(player, 24794, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            AddGossipItemFor(player, 24794, 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
        if (player->GetQuestObjectiveProgress(QUEST_STEP_BACK_FROM_THAT_LEDGE_MY_FRIEND, 1) &&
            !player->GetQuestObjectiveProgress(QUEST_STEP_BACK_FROM_THAT_LEDGE_MY_FRIEND, 2))
        {
            AddGossipItemFor(player, 24794, 2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
        player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 MenuID, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 0:
            CloseGossipMenuFor(player);
            Talk(0);
            me->CastSpell(me, SPELL_FLYING_KYRIAN_CHANNEL);
            player->KilledMonsterCredit(159906);
            me->AddDelayedEvent(7000, [this]() -> void
                {
                    if (Creature* nikolon = me->FindNearestCreature(159899, 20.f, true))
                    {
                        nikolonGUID = nikolon->GetGUID();
                        nikolon->AI()->Talk(1);
                        CastNikolonRing(nikolon);
                    }
                    summonNikolonsUncertainty(159908);
                });
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            AddGossipItemFor(player, GossipOptionNpc::None, "I see.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            SendGossipMenuFor(player, 40542, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->KilledMonsterCredit(159762);
            if (Creature* pelagos = me->SummonCreature(164335, NikolonsUncertainty[8], TEMPSUMMON_TIMED_DESPAWN, 20s))
            {
                pelagos->GetMotionMaster()->MovePath(16433501, false);
                me->AddDelayedEvent(3000, [pelagos]() -> void { pelagos->AI()->Talk(0); });
            }
            me->AddDelayedEvent(8000, [player]() -> void
                {
                    player->CastSpell(player, SPELL_ATTACK_OF_THE_DARK_KYRIAN);
                });
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            AddGossipItemFor(player, 24794, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
            AddGossipItemFor(player, 24794, 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(me), me->GetGUID());
            break;
        }
        return true;
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (Creature* nikolon = ObjectAccessor::GetCreature(*me, nikolonGUID))
        {
            switch (summon->GetEntry())
            {
            case 159908:
                death_count5++;
                if (death_count5 == 5)
                {
                    me->CastSpell(me, SPELL_FLYING_KYRIAN_CHANNEL);
                    Talk(1);
                    nikolon->SetAIAnimKitId(0);
                    me->AddDelayedEvent(5000, [this, nikolon]() ->void
                        {
                            nikolon->AI()->Talk(2);
                        });
                    me->AddDelayedEvent(13000, [this, nikolon]() ->void
                        {
                            CastNikolonRing(nikolon);
                            summonNikolonsUncertainty(159909);
                        });
                    death_count5 = 0;
                }
                break;
            case 159909:
                death_count3++;
                if (death_count3 == 3)
                {
                    me->CastSpell(me, SPELL_FLYING_KYRIAN_CHANNEL);
                    Talk(2);
                    nikolon->SetAIAnimKitId(0);
                    me->AddDelayedEvent(7000, [this, nikolon]() ->void
                        {
                            nikolon->AI()->Talk(3);
                        });
                    me->AddDelayedEvent(14000, [this, nikolon]() ->void
                        {
                            CastNikolonRing(nikolon);
                            summonNikolonsUncertainty(159910);
                        });
                    death_count3 = 0;
                }
                break;
            case 159910:
                death_count2++;
                if (death_count2 == 2)
                {
                    me->CastSpell(me, SPELL_FLYING_KYRIAN_CHANNEL);
                    Talk(3);
                    nikolon->SetAIAnimKitId(0);
                    me->AddDelayedEvent(7000, [this, nikolon]() ->void
                        {
                            nikolon->AI()->Talk(4);
                        });
                    me->AddDelayedEvent(15000, [this, nikolon]() ->void
                        {
                            me->CastStop();
                            CastNikolonRing(nikolon);
                            me->SummonCreature(159911, NikolonsUncertainty[7], TEMPSUMMON_MANUAL_DESPAWN);
                        });
                    death_count2 = 0;
                }
                break;
            case 159911:
                nikolon->AI()->Talk(5);
                nikolon->CastSpell(nikolon, 332686);
                nikolon->SetAIAnimKitId(0);
                nikolon->SetAIAnimKitId(21325); //lay_4133
                me->AddDelayedEvent(7000, [this, nikolon]() ->void
                    {
                        nikolon->CastSpell(nikolon, 348942);
                        me->SummonCreature(159900, nikolon->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN); // fallen nikolon
                        nikolon->ForcedDespawn(1000);
                        Talk(4);
                    });
                break;
            }
        }
    }

    void summonNikolonsUncertainty(uint32 creature)
    {
        switch (creature)
        {
        case 159908:
            for (uint8 i = 0; i < 5; ++i)
                me->SummonCreature(159908, NikolonsUncertainty[i], TEMPSUMMON_MANUAL_DESPAWN);
            break;
        case 159909:
            for (uint8 i = 0; i < 3; ++i)
                me->SummonCreature(159909, NikolonsUncertainty[i], TEMPSUMMON_MANUAL_DESPAWN);
            break;
        case 159910:
            for (uint8 i = 5; i < 7; ++i)
                me->SummonCreature(159910, NikolonsUncertainty[i], TEMPSUMMON_MANUAL_DESPAWN);
            break;

        }
    }

    void CastNikolonRing(Creature* nikolon)
    {
        me->CastStop();
        nikolon->SetAIAnimKitId(12130); //cower,fear
        nikolon->CastSpell(nikolon, SPELL_RING_BELL_MEDIUM);
        if (Creature* tranquility = me->FindNearestCreature(157624, 20.f, true))
            tranquility->CastSpell(tranquility, SPELL_RING_BELL_LARGE);
        nikolon->SetAIAnimKitId(12130); //cower,fear

    }
};

// 343603 Dark Sermon
struct go_dark_sermon_343603 : public GameObjectAI
{
    go_dark_sermon_343603(GameObject* go) : GameObjectAI(go) { }

    bool OnReportUse(Player* player) override
    {
        if (!player->GetQuestObjectiveProgress(QUEST_DANGEROUS_DISCOURSE, 0))
        {
            player->GetScheduler().Schedule(4s, [player](TaskContext context)
                {
                    player->PlayConversation(13540);
                });
        }
        else if (player->GetQuestObjectiveProgress(QUEST_DANGEROUS_DISCOURSE, 0) == 1)
        {
            player->GetScheduler().Schedule(4s, [player](TaskContext context)
                {
                    player->PlayConversation(13541);
                });
        }
        else if (player->GetQuestObjectiveProgress(QUEST_DANGEROUS_DISCOURSE, 0) == 2)
        {
            player->GetScheduler().Schedule(4s, [player](TaskContext context)
                {
                    player->PlayConversation(13542);
                });
        }
        else if (player->GetQuestObjectiveProgress(QUEST_DANGEROUS_DISCOURSE, 0) == 3)
        {
            player->GetScheduler().Schedule(4s, [player](TaskContext context)
                {
                    player->PlayConversation(13543);
                });
        }
        else if (player->GetQuestObjectiveProgress(QUEST_DANGEROUS_DISCOURSE, 0) == 4)
        {
            player->GetScheduler().Schedule(4s, [player](TaskContext context)
                {
                    player->PlayConversation(13544);
                });
        }
        else
        {
            player->GetScheduler().Schedule(4s, [player](TaskContext context)
                {
                    player->PlayConversation(13545);
                });
        }
        return true;
    }
};

// 168813 Disciple Kosmas
struct npc_disciple_kosmas_168813 : public EscortAI
{
    npc_disciple_kosmas_168813(Creature* creature) : EscortAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    uint8 wpath;
    Vehicle* vehicle;
    EventMap m_events;

    void Initialize()
    {
        wpath = 0;
    }

    void AttackStart(Unit* /*who*/) override {}
    void JustEngagedWith(Unit* /*who*/) override {}
    void OnCharmed(bool /*apply*/) override { }
    void EnterEvadeMode(EvadeReason /*reason*/) override {}

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        vehicle = me->GetVehicleKit();
        if (Player* player = me->SelectNearestPlayer(10.f))
        {
            if (player->HasQuest(QUEST_THE_ENEMY_YOU_KNOW))
                player->EnterVehicle(me);
        }
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (apply)
        {
            if (Player* player = who->ToPlayer())
            {
                m_playerGUID = player->GetGUID();
                player->SetClientControl(me, false);
                Talk(0, player);
                me->AddDelayedEvent(3000, [this]() -> void
                    {
                        wpath = 1;
                        me->GetMotionMaster()->MovePath(16881301, false);
                    });
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            if (wpath == 1)
            {
                if (id == 2) Talk(1);
                if (id == 4) Talk(2);
                if (id == 7) m_events.ScheduleEvent(EVENT_ERIDIA_LYSONIA_DIALOG1, 2s);
            }
            if (wpath == 4)
            {
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    if (id == 0)  player->PlayConversation(13548);;
            }
            if (wpath == 5)
            {
                if (id == 2)
                {
                    if (Creature* lysonia = me->FindNearestCreature(164129, 60.f, true))
                        if (Creature* eridia = me->FindNearestCreature(157678, 60.f, true))
                        {
                            lysonia->CastSpell(eridia, SPELL_LYSONIA_BEAM);
                            eridia->AddAura(AURA_LIFE_BURDEN_SHADER, eridia);
                            eridia->SetAIAnimKitId(19041);
                        }
                    if (Creature* vesiphone = me->FindNearestCreature(167865, 100.f, true))
                        vesiphone->RemoveAura(65050);
                }
                if (id == 3)
                {
                    if (Creature* eridia = me->FindNearestCreature(157678, 60.f, true))
                        eridia->DespawnOrUnsummon(1s, 120s);
                    if (Creature* lysonia = me->FindNearestCreature(164129, 60.f, true))
                        if (Creature* vesiphone = me->FindNearestCreature(167865, 100.f, true))
                        {
                            lysonia->CastStop();
                            vesiphone->CastSpell(lysonia, SPELL_FLYING_KYRING_BEAM_CHANNEL);
                            me->AddDelayedEvent(3000, [lysonia]() -> void { lysonia->DespawnOrUnsummon(1s, 120s); });
                            me->AddDelayedEvent(5000, [this, vesiphone]() -> void
                                {
                                    vesiphone->DespawnOrUnsummon(1s, 120s);
                                    Talk(5);
                                    wpath = 6;
                                    me->GetMotionMaster()->MovePath(16881306, false);
                                });
                        }
                }
            }
            if (wpath == 6)
            {
                if (id == 6)
                {
                    vehicle->RemoveAllPassengers();
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    {
                        if (Creature* lysonia = player->FindNearestCreature(164129, 40.f, true))
                            lysonia->AI()->Talk(5);
                        if (Creature* kosmas = player->FindNearestCreature(157673, 20.f, true))
                            kosmas->RemoveAura(65050);
                        player->ForceCompleteQuest(57446);
                    }
                    me->ForcedDespawn(1000);
                }
            }
        }
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_ERIDIA_LYSONIA_DIALOG2:
            wpath = 3;
            me->GetMotionMaster()->MovePath(16881303, false);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                Conversation::CreateConversation(13546, player, player->GetPosition(), { player->GetGUID() });
            break;
        case ACTION_ERIDIA_LYSONIA_DIALOG3:
            wpath = 4;
            Talk(4);
            me->GetMotionMaster()->MovePath(16881304, false);
            break;
        case ACTION_ERIDIA_LYSONIA_DIALOG4:
            wpath = 5;
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                player->PlayConversation(13550);
            me->GetMotionMaster()->MovePath(16881305, false);
            break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        m_events.Update(diff);

        while (uint32 eventId = m_events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ERIDIA_LYSONIA_DIALOG1:
            {
                if (Creature* eridia = me->FindNearestCreature(157678, 60.f, true))
                    if (Creature* lysonia = me->FindNearestCreature(164129, 60.f, true))
                    {
                        lysonia->AI()->Talk(3);
                        lysonia->CastSpell(eridia, SPELL_KYRIAN_FLYING_FAKED_ATTACK);
                        me->AddDelayedEvent(7000, [eridia, lysonia]() -> void
                            {
                                eridia->CastSpell(lysonia, SPELL_FLYING_KYRING_BEAM_CHANNEL2);
                                eridia->AI()->Talk(0);
                            });
                        me->AddDelayedEvent(10500, [lysonia]() -> void { lysonia->AI()->Talk(4); });
                        me->AddDelayedEvent(18000, [this, eridia, lysonia]() -> void
                            {
                                Talk(3);
                                lysonia->CastSpell(eridia, SPELL_KYRIAN_FLYING_FAKED_ATTACK);
                                eridia->CastStop();
                                wpath = 2;
                                me->GetMotionMaster()->MovePath(16881302, false);
                            });
                    }
                break;
            }
            }
        }
    }
};

// 319525 Hurl Kyrian Hammer
struct spell_hurl_kyrian_hammer_319525 : public SpellScript
{
    PrepareSpellScript(spell_hurl_kyrian_hammer_319525);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                std::list<Creature*> cList = player->FindAllCreaturesInRange(30.f);
                for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                {
                    if (Creature* pv = *itr)
                    {
                        switch (pv->GetEntry())
                        {
                        case 164257:
                            RingBellAction(player, pv);
                            break;
                        case 164254:
                            RingBellAction(player, pv);
                            break;
                        case 164256:
                            RingBellAction(player, pv);
                            break;
                        }
                    }
                }
            }
    }

    void RingBellAction(Player* player, Creature* pv)
    {
        player->KilledMonsterCredit(164237);
        pv->CastSpell(pv, SPELL_RING_BELL_LARGE, false);
        if (Creature* kosmas = player->FindNearestCreature(168813, 10.f, true))
        {
            if (pv->GetEntry() == 164257)
                kosmas->AI()->DoAction(ACTION_ERIDIA_LYSONIA_DIALOG2);
            else if (pv->GetEntry() == 164254)
                kosmas->AI()->DoAction(ACTION_ERIDIA_LYSONIA_DIALOG3);
            else
                kosmas->AI()->DoAction(ACTION_ERIDIA_LYSONIA_DIALOG4);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hurl_kyrian_hammer_319525::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 167865 Vesiphone
struct npc_vesiphone_167865 : public ScriptedAI
{
    npc_vesiphone_167865(Creature* creature) : ScriptedAI(creature)
    {
        me->CastSpell(me, SPELL_FLYING_KYRIAN_CHANNEL_OMNI_WITH_AREA, true);
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 3)
        {
            switch (data)
            {
            case 1:
                me->RemoveAura(SPELL_FLYING_KYRIAN_CHANNEL_OMNI_WITH_AREA);
                me->AddAura(AURA_TOUCH_OF_THE_MAW, me);
                break;
            case 2:
                me->RemoveAura(AURA_TOUCH_OF_THE_MAW);
                if (Creature* lysonia = me->FindNearestCreature(164129, 20.f, true))
                {
                    me->SetFacingToObject(lysonia);
                    Talk(0);
                    me->AddDelayedEvent(4000, [this, lysonia]() -> void
                        {
                            Talk(1);
                            me->CastSpell(lysonia, SPELL_FLYING_KYRING_BEAM_CHANNEL, true);
                            lysonia->SetStandState(UNIT_STAND_STATE_KNEEL);
                        });
                    me->AddDelayedEvent(10000, [this, lysonia]() -> void
                        {
                            Talk(2);
                            me->CastStop();
                            me->SummonCreature(168183, Position(-4026.53f, -5724.35f, 6728.55f, 5.29505f), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120s);
                        });
                    me->AddDelayedEvent(14000, [this, lysonia]() -> void
                        {
                            lysonia->AI()->Talk(10);
                            lysonia->AI()->SetData(3, 3);
                            if (Creature* kosmas = me->FindNearestCreature(157673, 20.f, true))
                                kosmas->AI()->SetData(3, 3);
                            if (Creature* eridia = me->FindNearestCreature(157678, 20.f, true))
                                eridia->AI()->SetData(3, 3);
                            me->SetCanFly(false);
                            me->SetHover(false, true);
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(0, -4022.37f, -5730.72f, 6728.47f, true);
                        });
                }
                break;
            case 4:
                if (Player* player = me->SelectNearestPlayer(35.f))
                {
                    player->KilledMonsterCredit(167865);
                    if (player->GetPhaseShift().HasPhase(10135)) PhasingHandler::RemovePhase(player, 10135, true);
                    if (!player->GetPhaseShift().HasPhase(10136)) PhasingHandler::AddPhase(player, 10136, true);
                }
                break;
            }
        }
    }
};

// 164325 Vesiphone
struct npc_vesiphone_164325 : public ScriptedAI
{
    npc_vesiphone_164325(Creature* creature) : ScriptedAI(creature) {}

private:
    ObjectGuid m_playerGUID;
    Vehicle* vehicle;

    void AttackStart(Unit* /*who*/) override {}
    void JustEngagedWith(Unit* /*who*/) override {}
    void OnCharmed(bool /*apply*/) override { }
    void EnterEvadeMode(EvadeReason /*reason*/) override {}

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        vehicle = me->GetVehicleKit();
        if (Player* player = me->SelectNearestPlayer(15.f))
        {
            if (player->HasQuest(QUEST_PURITYS_PREROGATIVE))
                player->EnterVehicle(me);
        }
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (apply)
        {
            if (Player* player = who->ToPlayer())
            {
                m_playerGUID = player->GetGUID();
                me->GetMotionMaster()->MovePath(16432501, false);
                if (player->GetPhaseShift().HasPhase(10134)) PhasingHandler::RemovePhase(player, 10134, true);
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
                Talk(0);
                break;
            case 3:
                Talk(1);
                break;
            case 5:
                Talk(2);
                break;
            case 13:
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    player->ForceCompleteQuest(QUEST_PURITYS_PREROGATIVE);
                break;
            case 14:
                TalkToKleia();
                break;
            }
        }
        if (type == POINT_MOTION_TYPE && id == 1001) me->ForcedDespawn();
    }

    void TalkToKleia()
    {
        vehicle->RemoveAllPassengers();
        me->AddDelayedEvent(2000, [this]() ->void
            {
                me->GetMotionMaster()->MoveFall();
                if (Creature* pelagos = me->FindNearestCreature(164335, 20.f, true))
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        pelagos->AI()->Talk(1, player);
                }
            });
        me->AddDelayedEvent(7000, [this]() ->void
            {
                Talk(6);
                if (Creature* lysonia = me->FindNearestCreature(168183, 20.f, true))
                    lysonia->RemoveAura(65050);
            });
        me->AddDelayedEvent(14000, [this]() ->void { Talk(7);  });
        me->AddDelayedEvent(17000, [this]() ->void
            {
                me->SetHover(true);
                me->GetMotionMaster()->MovePoint(0, -4135.44f, -5025.04f, 6547.38f, true);
            });
        me->AddDelayedEvent(21000, [this]() ->void
            {
                if (Creature* kleia = me->FindNearestCreature(156238, 20.f, true))
                    kleia->AI()->Talk(0);
                me->GetMotionMaster()->MovePoint(1001, -4131.43f, -4946.36f, 6566.15f, true);
            });
    }
};


Position const MemoryFragmentPos[6] =
{
    {-3593.31f, -5217.51f, 6572.95f, 3.42461f},
    {-3587.41f, -5235.01f, 6572.75f, 3.17521f},
    {-3591.75f, -5251.94f, 6572.75f, 2.54625f},
    {-3620.21f, -5270.64f, 6572.75f, 1.77763f},
    {-3637.53f, -5267.26f, 6572.98f, 1.06129f},
    {-3650.04f, -5254.46f, 6572.83f, 0.69370f},
};

// 167034 Mikanikos
struct npc_mikanikos_167034 : public ScriptedAI
{
    npc_mikanikos_167034(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool player_near;
    bool memory_fragment;
    ObjectGuid kleiaGUID;
    ObjectGuid m_playerGUID;
    EventMap _events;

    void Initialize()
    {
        player_near = false;
        memory_fragment = false;
    }

    void Reset() override
    {
        kleiaGUID = ObjectGuid::Empty;
        m_playerGUID = ObjectGuid::Empty;
        _events.Reset();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 10.0f))
            {
                if (player->HasQuest(QUEST_CHASING_A_MEMORY) && !player_near)
                {
                    player_near = true;
                    if (Creature* KeeperM = me->FindNearestCreature(167037, 10.f, true))
                        KeeperM->AI()->Talk(0);
                    if (Creature* kleia = me->SummonCreature(167038, Position(-3757.75f, -5005.13f, 6577.04f), TEMPSUMMON_TIMED_DESPAWN, 3min))
                        kleia->GetMotionMaster()->MovePath(16703803, false);
                    me->AddDelayedEvent(7000, [this]() -> void { Talk(0); });
                    me->AddDelayedEvent(14000, [this]() -> void
                        {
                            if (Creature* KeeperM = me->FindNearestCreature(167037, 10.f, true))
                                KeeperM->DespawnOrUnsummon(1s, 2min);
                            if (Creature* kleia = me->FindNearestCreature(167038, 10.f, true))
                                kleia->AI()->Talk(2);
                        });
                }
                if (player->HasQuest(QUEST_WHATS_IN_A_MEMORY))
                    if (player->GetQuestObjectiveProgress(QUEST_WHATS_IN_A_MEMORY, 3) == 10 && !memory_fragment)
                    {
                        memory_fragment = true;
                        if (Creature* FLysonia = me->FindNearestCreature(167551, 15.f, true))
                        {
                            FLysonia->SetFaction(35);
                            FLysonia->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        }
                        Talk(9);
                        me->AddDelayedEvent(7000, [player, this]() -> void
                            {
                                if (Creature* kleia = me->FindNearestCreature(167038, 10.f, true))
                                    kleia->AI()->Talk(5, player);
                            });
                    }
            }
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:
            memory_fragment = false;
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            player->KilledMonsterCredit(167034);
            player->CastSpell(player, SPELL_FADE_TO_WHITE);
            me->AddDelayedEvent(10000, [this]()->void {Talk(6); });
            me->AddDelayedEvent(14000, [this]() -> void
                {
                    if (Creature* kleia = me->FindNearestCreature(167038, 10.f, true))
                        kleia->AI()->Talk(4);
                });
            me->AddDelayedEvent(21000, [this]() -> void { Talk(7); });
            me->AddDelayedEvent(29000, [this]() -> void { Talk(8); });
            _events.ScheduleEvent(EVENT_SUMMON_MEMORY_FRAGMENT, 2s);
            CloseGossipMenuFor(player);
            break;
        case 1:
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            m_playerGUID = player->GetGUID();
            player->KilledMonsterCredit(167034);
            if (Creature* LKM = me->FindNearestCreature(167036, 20.f, true))
            {
                //LKM->SetUnkillable(true);
                me->CastSpell(LKM, 46598); //mount
                me->AddDelayedEvent(3000, [player, LKM]() -> void
                    {
                        LKM->AI()->Talk(0);
                        LKM->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                        LKM->Attack(player, true);
                    });
            };
            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (player->HasQuest(QUEST_DIRECTIONS_NOT_INCLUDED) && player->HasQuest(QUEST_PRIMES_DIRECTIVE))
        {
            if (Creature* kleia = me->FindNearestCreature(167038, 10.f, true))
                kleia->AI()->Talk(3);
            else if (Creature* kleia = me->FindNearestCreature(167562, 30.f, true))
                kleia->AI()->Talk(0);
            me->AddDelayedEvent(5000, [this]() -> void { Talk(1); });
        }
        if (player->HasQuest(QUEST_THE_MNEMONIC_LOCUS))
        {
            Talk(2);
            m_playerGUID = player->GetGUID();
            if (Creature* kleia = me->FindNearestCreature(167562, 20.f, true))
                kleiaGUID = kleia->GetGUID();
            if (!player->GetPhaseShift().HasPhase(10157)) PhasingHandler::AddPhase(player, 10157, true);
        }
        if (player->HasQuest(QUEST_WHATS_IN_A_MEMORY))
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        if (player->HasQuest(QUEST_I_MADE_YOU)) Talk(11);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_THE_MNEMONIC_LOCUS)
        {
            me->RemoveAura(AURA_MIKANIKOS_RUNE_DISPLAY);
            me->GetMotionMaster()->MovePoint(1001, Position(-3610.9f, -5258.15f, 6572.31f), true, 5.6868f);
        }
        if (action == ACTION_WHATS_IN_A_MEMORY)
            _events.CancelEvent(EVENT_SUMMON_MEMORY_FRAGMENT);
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_CHASING_A_MEMORY)
            player_near = false;
        if (quest->GetQuestId() == QUEST_THE_MNEMONIC_LOCUS)
        {
            if (!player->GetPhaseShift().HasPhase(10153)) PhasingHandler::AddPhase(player, 10153, true);
            if (player->GetPhaseShift().HasPhase(10152)) PhasingHandler::RemovePhase(player, 10152, true);
            if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
                kleia->GetMotionMaster()->MoveTargetedHome();
            me->DespawnOrUnsummon(10s, 60s);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            me->AddAura(AURA_MIKANIKOS_RUNE_DISPLAY, me);
            if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
            {
                kleia->GetMotionMaster()->MovePoint(0, Position(-3607.13f, -5255.76f, 6572.31f), true, 5.0442f);
                me->AddDelayedEvent(5000, [kleia]() -> void
                    {
                        kleia->AI()->Talk(1);
                    });
                me->AddDelayedEvent(10000, [this]() -> void { Talk(3); });
                me->AddDelayedEvent(13000, [kleia, this]() -> void { kleia->AI()->Talk(2); });
                me->AddDelayedEvent(20000, [kleia]() -> void
                    {
                        kleia->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);
                    });
                me->AddDelayedEvent(21000, [kleia, this]() -> void
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                        {
                            std::list<Creature*> cList = player->FindNearestCreatures(136163, 80.0f);
                            for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                            {
                                if (Creature* chakrams = *itr)
                                    chakrams->AI()->SetData(1, 1);     //ToSmartAI
                            }
                        }
                        if (Creature* FLysonia = me->FindNearestCreature(167551, 10.0f, true))
                            FLysonia->RemoveAura(65050);
                    });
                me->AddDelayedEvent(22000, [this]() -> void { Talk(4); });
                me->AddDelayedEvent(25000, [this]() -> void
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                            player->ForceCompleteQuest(58800);
                        Talk(5);
                    });
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
            case EVENT_SUMMON_MEMORY_FRAGMENT:
                SummonMemoryFragment();
                _events.Repeat(randtime(3s, 6s));
                break;
            }
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 1 && data == 1)
        {
            Talk(10);
            me->AddDelayedEvent(6000, [this]() -> void
                {
                    me->GetMotionMaster()->MovePath(16703401, false);
                });
        }
        if (type == 1 && data == 2) me->GetMotionMaster()->MoveTargetedHome();
        if (type == 1 && data == 3)
        {
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                if (player->GetPhaseShift().HasPhase(10154)) PhasingHandler::RemovePhase(player, 10154, true);
                if (!player->GetPhaseShift().HasPhase(10155)) PhasingHandler::AddPhase(player, 10155, true);
            }
        }
    }

    void SummonMemoryFragment()
    {
        uint8 i = urand(0, 4);
        switch (uint8 rand = urand(0, 5))
        {
        case 0:
        case 1:
        case 2:
        case 3:
            if (Creature* MF = me->SummonCreature(167459, MemoryFragmentPos[i], TEMPSUMMON_TIMED_DESPAWN, 2min))
            {
                MF->CastSpell(MF, 321262);
                switch (i)
                {
                case 0:
                    MF->GetMotionMaster()->MovePath(16745901, false);
                    break;
                case 1:
                    MF->GetMotionMaster()->MovePath(16745902, false);
                    break;
                case 2:
                    MF->GetMotionMaster()->MovePath(16745903, false);
                    break;
                case 3:
                    MF->GetMotionMaster()->MovePath(16745904, false);
                    break;
                case 4:
                    MF->GetMotionMaster()->MovePath(16745905, false);
                    break;
                case 5:
                    MF->GetMotionMaster()->MovePath(16745906, false);
                    break;
                }
            }
            break;
        case 4:
            if (Creature* FMF = me->SummonCreature(162492, MemoryFragmentPos[i], TEMPSUMMON_TIMED_DESPAWN, 2min))
            {
                FMF->CastSpell(FMF, 321262);
                FMF->GetMotionMaster()->MoveRandom(5.f);
            }
            break;
        }
    }
};

// 167211 Memory Extractor
struct npc_memory_extractor_167211 : public ScriptedAI
{
    npc_memory_extractor_167211(Creature* creature) : ScriptedAI(creature) {}

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 3.0f))
            {
                if (player->HasQuest(QUEST_THE_MNEMONIC_LOCUS))
                    if (!player->GetQuestObjectiveProgress(QUEST_THE_MNEMONIC_LOCUS, 1) && !player->HasAura(AURA_EMPOWER_EXTRACTOR))
                    {
                        player->AddAura(AURA_EMPOWER_EXTRACTOR, player);
                    }
            }
    }
};

// 342663 Empower Extractor
class spell_empower_extractor_342663 : public SpellScript
{
    PrepareSpellScript(spell_empower_extractor_342663);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                caster->ToPlayer()->KilledMonsterCredit(167211, ObjectGuid::Empty);
                if (Creature* mikanikos = player->FindNearestCreature(167034, 15.0f, true))
                    mikanikos->AI()->DoAction(ACTION_THE_MNEMONIC_LOCUS);
                AnimaConduitExtract(player, 347076, 1s, 3s);
                AnimaConduitExtract(player, 347078, 4s, 6s);
                AnimaConduitExtract(player, 347079, 7s, 9s);
                AnimaConduitExtract(player, 347080, 10s, 12s);
                AnimaConduitExtract(player, 347081, 13s, 15s);
                AnimaConduitExtract(player, 347083, 16s, 18s);
                AnimaConduitExtract(player, 347084, 19s, 21s);
            }
    }

    void  AnimaConduitExtract(Player* player, uint32 CSpId, Seconds Fsec, Seconds Ssec)
    {
        if (Creature* FXstalker = player->FindNearestCreature(156279, 80.0f, true))
            if (FXstalker->GetSpawnId() == 347067)
            {
                std::list<Creature*> cList = player->FindNearestCreatures(167039, 80.0f);
                for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                {
                    if (Creature* conduit = *itr)
                    {
                        if (conduit->GetSpawnId() == CSpId)
                        {
                            player->GetScheduler().Schedule(Fsec, [FXstalker, conduit](TaskContext context)
                                {
                                    FXstalker->CastSpell(conduit, SPELL_FLYING_KYRING_BEAM_CHANNEL);
                                });
                            player->GetScheduler().Schedule(Ssec, [player, FXstalker, conduit](TaskContext context)
                                {
                                    FXstalker->CastStop();
                                    conduit->DespawnOrUnsummon(1s, 120s);
                                    if (conduit->GetSpawnId() == 347084)
                                    {
                                        player->SummonGameObject(350769, -3623.52f, -5232.95f, 6565.34f, 2.42424f, QuaternionData(0, 0, 0, 0), 1min); //Memory Extractor
                                        PhasingHandler::RemovePhase(player, 10157, true);
                                    }
                                });
                        }
                    }
                }
            }
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_empower_extractor_342663::HandleDummy, EFFECT_1, SPELL_EFFECT_REMOVE_AURA);
    }
};

//326093 Imbuing
struct spell_imbuing_326093 : public SpellScript
{
    PrepareSpellScript(spell_imbuing_326093);

    void HandleDummy(SpellEffIndex effindex)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        //lbplayer->DestroyItemCount(183802, 10, true, false);
                        player->KilledMonsterCredit(167551);
                        if (Creature* mikanikos = player->FindNearestCreature(167034, 15.0f, true))
                            mikanikos->AI()->DoAction(ACTION_WHATS_IN_A_MEMORY);
                        player->GetScheduler().Schedule(3s, [player](TaskContext context)
                            {
                                std::list<Creature*> cList = player->FindAllCreaturesInRange(60.f);
                                for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                                {
                                    if (Creature* Mfragment = *itr)
                                    {
                                        if (Mfragment->GetEntry() == 167459 || Mfragment->GetEntry() == 162492 || Mfragment->GetEntry() == 167551
                                            || Mfragment->GetEntry() == 136163 || Mfragment->GetEntry() == 156279)
                                            Mfragment->ForcedDespawn();
                                    }
                                }
                                player->CastSpell(player, SPELL_FADE_TO_WHITE);
                            });
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_imbuing_326093::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

//325609 Extract Memory
class spell_extract_memory_325609 : public SpellScript
{
    PrepareSpellScript(spell_extract_memory_325609);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* obj)
            {
                if (Creature* target = obj->ToCreature())
                    if (target->IsTrigger() || target->GetEntry() == 167261)
                        return true;

                return false;
            });
    }

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                if (!player->GetQuestObjectiveProgress(QUEST_LYSONIAS_TRUTH, 0))
                {
                    RemoveInvisiableAura(player, 167261, 1);
                }
                else if (player->GetQuestObjectiveProgress(QUEST_LYSONIAS_TRUTH, 0)
                    && !player->GetQuestObjectiveProgress(QUEST_LYSONIAS_TRUTH, 1))
                {
                    RemoveInvisiableAura(player, 167260, 2);
                }
                else if (player->GetQuestObjectiveProgress(QUEST_LYSONIAS_TRUTH, 1)
                    && !player->GetQuestObjectiveProgress(QUEST_LYSONIAS_TRUTH, 2))
                {
                    RemoveInvisiableAura(player, 167257, 3);
                }
                else
                    RemoveInvisiableAura(player, 167267, 4);
            }
    }

    void RemoveInvisiableAura(Player* player, uint32 creature, uint8 data)
    {
        player->KilledMonsterCredit(creature);
        std::list<Creature*> cList = player->FindAllCreaturesInRange(30.f);
        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
        {
            if (Creature* invisiable = *itr)
            {
                if (invisiable->HasAura(65050))
                    invisiable->RemoveAura(65050);
                if (invisiable->GetEntry() == creature)
                    invisiable->AI()->SetData(1, data);
            }
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_extract_memory_325609::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENTRY);
        AfterCast += SpellCastFn(spell_extract_memory_325609::HandleAfterCast);
    }
};


//334629 Rousing
struct spell_rousing_334629 : public SpellScript
{
    PrepareSpellScript(spell_rousing_334629);

    void HandleDummy(SpellEffIndex effindex)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        player->KilledMonsterCredit(167387);
                        creTarget->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        creTarget->SetStandState(UNIT_STAND_STATE_STAND);
                        player->GetScheduler().Schedule(3s, [creTarget](TaskContext context)
                            {
                                creTarget->GetMotionMaster()->MoveRandom(5.f);
                                creTarget->ForcedDespawn(4500);
                            });
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rousing_334629::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

Position const MikanikosSecondPos[3] =
{
    {-3624.65f, -5447.73f, 6504.83f, 5.26609f},
    {-3613.83f, -5460.91f, 6504.68f, 5.6313f},
    {-3610.32f, -5425.91f, 6500.21f, 3.26728f}, // summon_FR_pos
};

// 167368 Mikanikos
struct npc_mikanikos_167368 : public ScriptedAI
{
    npc_mikanikos_167368(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    Unit* owner = nullptr;
    ObjectGuid m_playerGUID;
    uint8 summon_FR;

    void Initialize()
    {
        summon_FR = 0;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            owner = player;
            m_playerGUID = player->GetGUID();
        }
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (me->HasAura(AURA_KNEEL_AND_USE))
            return;

        if (Creature* creature = target->ToCreature())
        {
            if (creature->IsInDist(me, 15.0f))
            {
                if (creature->IsAlive())
                    if (creature->IsHostileToPlayers())
                    {
                        me->GetThreatManager().AddThreat(creature, 1000.0f);
                        creature->GetThreatManager().AddThreat(me, 1000.0f);
                        me->Attack(creature, true);
                    }
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_UNLOCK_CEREMONIAL_OFFRING_I)
        {
            me->AddDelayedEvent(4000, [this]()->void {Talk(0); });
            me->AddDelayedEvent(7500, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1001, Position(-3449.52f, -5415.25f, 6500.98f), true, 5.3591f, 7.0f, MovementWalkRunSpeedSelectionMode::Default);
                    if (Creature* kleia = me->FindNearestCreature(167370, 15.f, true))
                        kleia->AI()->Talk(0);
                });
        }
        if (action == ACTION_UNLOCK_CEREMONIAL_OFFRING_II)
        {
            me->AddDelayedEvent(4000, [this]()->void {Talk(2); });
            me->AddDelayedEvent(7500, [this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(1002, MikanikosSecondPos[0], true, 5.266f, 7.0f, MovementWalkRunSpeedSelectionMode::Default);
                });
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 1001)
        {
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            me->CastSpell(me, AURA_KNEEL_AND_USE);
            if (Creature* FR = me->SummonCreature(166873, Position(-3423.57f, -5406.41f, 6491.57f, 2.42284f), TEMPSUMMON_MANUAL_DESPAWN, 3min))
            {
                summon_FR = 1;
                FR->GetMotionMaster()->MovePath(16687301, false);
            }
        }
        if (id == 1002)
        {
            me->GetMotionMaster()->MovePoint(1003, MikanikosSecondPos[1], true, 5.6313f, 7.0f, MovementWalkRunSpeedSelectionMode::Default);
        }
        if (id == 1003)
        {
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            me->CastSpell(me, AURA_KNEEL_AND_USE);
            me->AddDelayedEvent(10000, [this]() -> void
                {
                    if (Creature* FR = me->SummonCreature(166873, MikanikosSecondPos[2], TEMPSUMMON_MANUAL_DESPAWN, 3min))
                    {
                        summon_FR = 2;
                        FR->GetMotionMaster()->MovePath(16687302, false);
                    }
                });
            me->AddDelayedEvent(25000, [this]() -> void
                {
                    if (Creature* UP = me->SummonCreature(166872, MikanikosSecondPos[2], TEMPSUMMON_MANUAL_DESPAWN, 3min))
                    {
                        summon_FR = 3;
                        UP->GetMotionMaster()->MovePath(16687302, false);
                    }
                });
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == 166873 && summon_FR == 1)
            SummonMobDie(350888, 167364);
        if (summon->GetEntry() == 166872 && summon_FR == 3)
            SummonMobDie(350889, 167365);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!owner || !owner->IsInWorld() || !owner->IsAlive())
        {
            me->ForcedDespawn();
        }
        DoMeleeAttackIfReady();
    }

    void SummonMobDie(uint32 objID, uint32 killID)
    {
        me->RemoveAura(AURA_KNEEL_AND_USE);
        me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        if (GameObject* CoP = me->FindNearestGameObject(objID, 10.f, false))
            CoP->RemoveFlag(GO_FLAG_LOCKED);
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            player->KilledMonsterCredit(killID);
            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle());
        }
    }
};

// 167370 kleia
struct npc_kleia_167370 : public ScriptedAI
{
    npc_kleia_167370(Creature* creature) : ScriptedAI(creature) {}
private:
    Unit* owner = nullptr;

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
            owner = player;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Creature* creature = target->ToCreature())
        {
            if (creature->IsInDist(me, 15.0f))
            {
                if (creature->IsAlive())
                    if (creature->IsHostileToPlayers())
                    {
                        me->GetThreatManager().AddThreat(creature, 1000.0f);
                        creature->GetThreatManager().AddThreat(me, 1000.0f);
                        me->Attack(creature, true);
                    }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!owner || !owner->IsInWorld() || !owner->IsAlive())
        {
            me->ForcedDespawn();
        }
        DoMeleeAttackIfReady();
    }
};

// 174531 Image of Locus-Keeper Mnemis
struct npc_image_of_locus_keeper_mnemis_174531 : public ScriptedAI
{
    npc_image_of_locus_keeper_mnemis_174531(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool near_player;

    void Initialize()
    {
        near_player = false;
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (Player* player = target->ToPlayer())
        {
            if (player->IsInDist(me, 15.0f))
            {
                if (player->HasQuest(QUEST_MNEMIS_AT_YOUR_SERVICE) && !near_player)
                {
                    switch (me->GetSpawnId())
                    {
                    case 347159:
                        NearPlayer(0);
                        break;
                    case 347160:
                        NearPlayer(1);
                        break;
                    case 347161:
                        NearPlayer(2);
                        if (!player->GetQuestObjectiveProgress(QUEST_MNEMIS_AT_YOUR_SERVICE, 0))
                            if (Creature* mikanikos = player->FindNearestCreature(167368, 10.f, true))
                                mikanikos->AI()->DoAction(ACTION_UNLOCK_CEREMONIAL_OFFRING_I);
                        break;
                    case 347162:
                        NearPlayer(3);
                        break;
                    case 347164:
                        NearPlayer(4);
                        break;
                    case 347165:
                        NearPlayer(2);
                        if (!player->GetQuestObjectiveProgress(QUEST_MNEMIS_AT_YOUR_SERVICE, 2))
                            if (Creature* mikanikos = player->FindNearestCreature(167368, 10.f, true))
                                mikanikos->AI()->DoAction(ACTION_UNLOCK_CEREMONIAL_OFFRING_II);
                        break;
                    }

                }
            }
            if (!player->IsInDist(me, 16.0f))
                if (player->HasQuest(QUEST_MNEMIS_AT_YOUR_SERVICE))
                {
                    me->AddAura(65050, me);
                    near_player = false;
                }
        }
    }

    void NearPlayer(uint8 txt)
    {
        near_player = true;
        me->RemoveAura(65050);
        Talk(txt);
    }
};

Position const FRMobsPos[4] =
{
    {-3826.43f, -5288.34f, 6490.91f, 2.42629f}, //0 frist_summon
    {-3830.88f, -5280.71f, 6489.91f, 5.18696f}, //1
    {-3783.27f, -5331.31f, 6500.21f, 2.48911f}, //2second, third_summon
    {-3783.91f, -5325.87f, 6500.12f, 2.44591f}, //3
};

// 167562 kleia
struct npc_kleia_167562 : public ScriptedAI
{
    npc_kleia_167562(Creature* creature) : ScriptedAI(creature) { initialize(); }
private:
    uint8 movement_num;
    uint8 summonMob_count;
    uint32 entry;

    void initialize()
    {
        summonMob_count = 0;
        entry = 0;
        movement_num = 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 1 && data == 1)
        {
            me->GetMotionMaster()->MovePath(16756201, false);
        }
        if (type == 1 && data == 2)
        {
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            summonFRMobs();
        }
        if (type == 1 && data == 3)
        {
            Talk(6);
        }
        if (type == 2 && data == 1)
        {
            movement_num = 3;
            me->GetMotionMaster()->MovePath(16756203, false);
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* killer) override
    {
        Player* player = killer->ToPlayer();
        summonMob_count++;
        switch (summonMob_count)
        {
        case 2:
            summonFRMobs();
            break;
        case 4:
            summonFRMobs();
            break;
        case 6:
            summonMob_count = 0;
            player->KilledMonsterCredit(167499);
            player->PlayConversation(14353);
            if (Creature* FX = player->FindNearestCreature(156279, 50.f, true))
                if (FX->GetSpawnId() == 347180)
                    FX->RemoveAura(AURA_ANIMA_SHIELD);
            me->DespawnOrUnsummon(60s, 60s);
            break;
        }
    }


    void MovementInform(uint32 type, uint32 id) override
    {
        if (movement_num == 3)
        {
            if (id == 0) Talk(7);
            if (id == 3) me->DespawnOrUnsummon(0s, 120s);
        }
    }

    void summonFRMobs()
    {
        if (summonMob_count == 0)
        {
            for (uint8 i = 0; i < 2; i++)
            {
                RandomSummon();
                Creature* FRMob = me->SummonCreature(entry, FRMobsPos[i], TEMPSUMMON_TIMED_DESPAWN, 3min);
            }
        }
        else
        {
            for (uint8 i = 2; i < 4; i++)
            {
                RandomSummon();
                if (Creature* FRMob = me->SummonCreature(entry, FRMobsPos[i], TEMPSUMMON_TIMED_DESPAWN, 3min))
                    FRMob->GetMotionMaster()->MovePath(16750801, false);
            }
        }
    }

    void RandomSummon()
    {
        uint8 rand_enemy = urand(0, 2);
        if (rand_enemy == 0) entry = 167508;
        else if (rand_enemy == 1) entry = 167509;
        else  entry = 167510;
    }
};

Position const summonMi_KlPos[2] =
{
    {-3839.91f, -5215.11f, 6486.72f, 1.51404f}, //0 mikanikos
    {-3835.02f, -5215.45f, 6486.52f, 1.58416f}, //1 kleia
};

// 167504 Vault-Keeper Mnemis
struct npc_vault_keeper_mnemis_167504 : public ScriptedAI
{
    npc_vault_keeper_mnemis_167504(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    bool player_near;
    bool locus_near;
    ObjectGuid m_playerGUID;
    ObjectGuid mikanikosGUID;
    ObjectGuid kleiaGUID;

    void Initialize()
    {
        player_near = false;
        locus_near = false;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
        mikanikosGUID = ObjectGuid::Empty;
        kleiaGUID = ObjectGuid::Empty;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 40.0f))
            {
                if (player->GetQuestObjectiveProgress(QUEST_THE_VAULT_OF_THE_ARCHON, 6)
                    && !player->GetQuestObjectiveProgress(QUEST_THE_VAULT_OF_THE_ARCHON, 3) && !player_near)
                {
                    player_near = true;
                    player->KilledMonsterCredit(167504);
                    Talk(0);
                    if (Creature* mikanikos = me->SummonCreature(167498, summonMi_KlPos[0], TEMPSUMMON_TIMED_DESPAWN, 2min))
                    {
                        mikanikosGUID = mikanikos->GetGUID();
                        mikanikos->GetMotionMaster()->MovePath(16749804, false);
                    }
                    if (Creature* kleia = me->SummonCreature(167562, summonMi_KlPos[1], TEMPSUMMON_TIMED_DESPAWN, 2min))
                    {
                        kleiaGUID = kleia->GetGUID();
                        kleia->GetMotionMaster()->MovePath(16756202, false);
                        me->AddDelayedEvent(7000, [kleia]() -> void { kleia->AI()->Talk(5); });
                    }
                }
                if (player->HasQuest(QUEST_LEAVE_IT_TO_MNEMIS))
                {
                    if (player->FindNearestCreature(167035, 10.f, true) && !locus_near)
                    {
                        locus_near = true;
                        Talk(3);
                        me->AddDelayedEvent(3000, [this]() -> void
                            {
                                if (Creature* locus = me->FindNearestCreature(167035, 10.f, true))
                                    locus->AI()->Talk(4);
                            });
                    }
                }
            }
            if (!player->IsInDist(me, 40.0f)) locus_near = false;
        }
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_A_PARAGONS_REFLECTION)
            Talk(1);
        if (quest->GetQuestId() == QUEST_LEAVE_IT_TO_MNEMIS)
        {
            m_playerGUID = player->GetGUID();
            me->GetMotionMaster()->MovePath(16750401, false);
        }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_THE_VAULT_OF_THE_ARCHON)
        {
            if (Creature* mikanikos = ObjectAccessor::GetCreature(*me, mikanikosGUID))
                if (Creature* kleia = ObjectAccessor::GetCreature(*me, kleiaGUID))
                {
                    mikanikos->ForcedDespawn();
                    kleia->ForcedDespawn();
                }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (id == 0)
        {
            Talk(2);
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                if (Creature* kleia = player->FindNearestCreature(167562, 30.f, true))
                    kleia->AI()->SetData(2, 1);
                if (Creature* mikanikos = player->FindNearestCreature(167498, 30.f, true))
                    mikanikos->AI()->SetData(2, 1);
            }
        }
        if (id == 4)
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
            {
                if (!player->GetPhaseShift().HasPhase(10156)) PhasingHandler::AddPhase(player, 10156, true);
                if (player->GetPhaseShift().HasPhase(10155)) PhasingHandler::RemovePhase(player, 10155, true);
            }
            me->DespawnOrUnsummon(0s, 120s);
        }
    }
};

//325341 Activating
class spell_activating_325341 : public SpellScript
{
    PrepareSpellScript(spell_activating_325341);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Creature* creTarget = GetHitCreature())
                {
                    switch (creTarget->GetEntry())
                    {
                    case 167516:
                        player->KilledMonsterCredit(167516);
                        RemoveInvisiableAura(player, 167519);
                        break;
                    case 167513:
                        player->KilledMonsterCredit(167513);
                        RemoveInvisiableAura(player, 167518);
                        break;
                    case 167511:
                        player->KilledMonsterCredit(167511);
                        RemoveInvisiableAura(player, 167521);
                        break;
                    case 167515:
                        player->KilledMonsterCredit(167515);
                        RemoveInvisiableAura(player, 167520);
                        break;
                    case 174542:
                        player->KilledMonsterCredit(174542);
                        RemoveInvisiableAura(player, 167522);
                        break;
                    case 167514:
                        player->KilledMonsterCredit(167379);
                        RemoveInvisiableAura(player, 167267);
                        RemoveInvisiableAura(player, 167311);
                        RemoveInvisiableAura(player, 167408);
                        break;
                    }
                }
    }

    void RemoveInvisiableAura(Player* player, uint32 crea_num)
    {
        std::list<Creature*> cList = player->FindNearestCreatures(crea_num, 15.0f);
        for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
        {
            if (Creature* invisiable = *itr)
            {
                if (invisiable->HasAura(65050))
                    invisiable->RemoveAura(65050);
                invisiable->AI()->SetData(1, 1);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_activating_325341::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 335614 Vesper of Remembrance
struct go_vesper_of_remembrance_335614 : public GameObjectAI
{
    go_vesper_of_remembrance_335614(GameObject* go) : GameObjectAI(go) { }

    bool OnReportUse(Player* player) override
    {
        if (Creature* FXRing = player->FindNearestCreature(156279, 10.f, true))
            FXRing->CastSpell(FXRing, SPELL_RING_BELL_SMALL);
        if (Creature* korinna = player->FindNearestCreature(170253, 10.f, true))
            korinna->AI()->DoAction(ACTION_TEST_OF_COURAGE);
        me->SetFlag(GO_FLAG_NOT_SELECTABLE);
        return true;
    }
};

Position const TestOfCourage_Pos[8] =
{
    {-3341.34f, -4855.07f, 6537.65f, 5.66501f}, //0 summon_pos
    {-3336.95f, -4854.86f, 6535.92f, 5.81817f}, //1 
    {-3340.91f, -4857.98f, 6536.62f, 5.59144f}, //2
    {-3300.92f, -4887.89f, 6531.16f, 6.01163f}, //3 zosime_pos
    {-3302.41f, -4881.51f, 6531.16f, 6.14122f}, //4
    {-3312.74f, -4880.15f, 6531.16f, 5.80347f}, //5 kala_pos
    {-3294.04f, -4889.28f, 6531.79f, 3.26909f}, //6 kala_jump
    {-3309.68f, -4878.71f, 6545.43f, 2.53621f}, //7 kala_take_off
};

// 170253 Memory of Korinna
struct npc_memory_of_korinna_170253 : public ScriptedAI
{
    npc_memory_of_korinna_170253(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid zosimeGUID;
    uint8 summondie;

    void Initialize()
    {
        summondie = 0;
    }

    void Reset() override
    {
        zosimeGUID = ObjectGuid::Empty;
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == 175038 || summon->GetEntry() == 175039 || summon->GetEntry() == 158179)
        {
            summondie++;
            switch (summondie)
            {
            case 2:
            case 5:
                summonTestOfCourageMob();
                break;
            case 6:
                summondie = 0;
                if (Creature* IS = me->FindNearestCreature(156586, 15.f, true))
                    IS->DespawnOrUnsummon(0s, 80s);
                if (Creature* zosime = ObjectAccessor::GetCreature(*me, zosimeGUID))
                    zosime->CastStop();
                if (GameObject* Remembrance = me->FindNearestGameObject(335614, 10.f, false))
                    Remembrance->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                SummonKala();
                break;
            }
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_TEST_OF_COURAGE)
        {
            Talk(0);
            if (Creature* zosime = me->SummonCreature(176435, TestOfCourage_Pos[0], TEMPSUMMON_TIMED_DESPAWN, 10min))
            {
                zosimeGUID = zosime->GetGUID();
                zosime->GetMotionMaster()->MovePoint(0, TestOfCourage_Pos[3], true, 6.01163f, 10.0f, MovementWalkRunSpeedSelectionMode::ForceRun);
                me->AddDelayedEvent(7000, [zosime]() -> void { zosime->AI()->Talk(0); });
                me->AddDelayedEvent(14000, [this]() -> void { Talk(1); });
                me->AddDelayedEvent(21000, [zosime, this]() -> void
                    {
                        if (Player* player = me->SelectNearestPlayer(25.f))
                            zosime->AI()->Talk(1, player);
                    });
                me->AddDelayedEvent(30000, [zosime, this]() -> void
                    {
                        zosime->CastSpell(zosime, SPELL_FLYING_KYRIAN_CHANNEL);
                        Talk(2);
                        summonTestOfCourageMob();
                    });
            }
        }
    }

    void summonTestOfCourageMob()
    {
        if (summondie == 0)
        {
            for (uint8 i = 0; i < 2; i++)
            {
                if (Creature* aw = me->SummonCreature(175038, TestOfCourage_Pos[i], TEMPSUMMON_TIMED_DESPAWN, 5min))
                    aw->GetMotionMaster()->MovePoint(0, TestOfCourage_Pos[4], true, 6.14122f, 8.0f, MovementWalkRunSpeedSelectionMode::ForceRun);
            }
        }
        if (summondie == 2)
        {
            for (uint8 i = 0; i < 3; i++)
            {
                if (Creature* ac = me->SummonCreature(175039, TestOfCourage_Pos[i], TEMPSUMMON_TIMED_DESPAWN, 5min))
                    ac->GetMotionMaster()->MovePoint(0, TestOfCourage_Pos[4], true, 6.14122f, 8.0f, MovementWalkRunSpeedSelectionMode::ForceRun);
            }
        }
        if (summondie == 5)
        {
            if (Creature* vr = me->SummonCreature(158179, TestOfCourage_Pos[1], TEMPSUMMON_TIMED_DESPAWN, 5min))
                vr->GetMotionMaster()->MovePoint(0, TestOfCourage_Pos[4], true, 6.14122f, 8.0f, MovementWalkRunSpeedSelectionMode::ForceRun);
        }
    }

    void SummonKala()
    {
        if (Creature* kala = me->SummonCreature(158081, TestOfCourage_Pos[1], TEMPSUMMON_TIMED_DESPAWN, 5min))
        {
            kala->GetMotionMaster()->MovePoint(0, TestOfCourage_Pos[5], true, 6.14122f, 8.0f, MovementWalkRunSpeedSelectionMode::ForceRun);
            me->AddDelayedEvent(5000, [kala]() -> void
                {
                    kala->GetMotionMaster()->MoveJump(TestOfCourage_Pos[6], 15, 25);
                });
            me->AddDelayedEvent(10000, [this, kala]() -> void
                {
                    kala->AddAura(AURA_STAFF_COSMETIC, kala);
                    if (GameObject* Crook = me->FindNearestGameObject(340419, 10.f, false))
                        Crook->DestroyForNearbyPlayers();
                    if (Creature* zosime = ObjectAccessor::GetCreature(*me, zosimeGUID))
                        zosime->AI()->Talk(2);
                    kala->SetAnimTier(AnimTier::Fly);
                    kala->SetFlying(true);
                    kala->GetMotionMaster()->MovePath(15808101, false);
                });
            me->AddDelayedEvent(14000, [this, kala]() -> void
                {
                    if (Creature* zosime = ObjectAccessor::GetCreature(*me, zosimeGUID))
                    {
                        zosime->AI()->Talk(3);
                        zosime->GetMotionMaster()->MovePath(17643501, false);
                    }
                });
        }
    }
};

// 337218  Throw Slobber Ball
struct spell_throw_slobber_ball_337218 : public SpellScript
{
    PrepareSpellScript(spell_throw_slobber_ball_337218);
    Unit* target;
    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                if (Creature* IS = player->FindNearestCreature(156586, 30.f, true))
                    if (Creature* kala = player->FindNearestCreature(158254, 30.f, true))
                    {
                        player->CastSpell(IS, SPELL_THROW_SLOBBER_BALL);
                        player->GetScheduler().Schedule(4s, [player, kala, IS](TaskContext context)
                            {
                                kala->GetMotionMaster()->MoveJump(IS->GetPosition(), 10, 15);
                                IS->DespawnOrUnsummon(3s, 25s);
                            });
                        player->GetScheduler().Schedule(5s, [kala](TaskContext context)
                            {
                                kala->CastSpell(kala, SPELL_BALL);
                                kala->CastSpell(kala, SPELL_ROOT_SELF);
                            });
                    }
            }
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_throw_slobber_ball_337218::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 311032 Reactivating
struct spell_reactivating_311032 : public SpellScript
{
    PrepareSpellScript(spell_reactivating_311032);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        player->RemoveAura(SPELL_GATAMATOS_IS_NEAR);
                        player->CastSpell(player, SPELL_GATAMATOS_IS_NEAR);
                        creTarget->DespawnOrUnsummon(0s, 120s);
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_reactivating_311032::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 310987 [DNT] Anima Missile Reverse
struct spell_anima_missile_reverse_310987 : public SpellScript
{
    PrepareSpellScript(spell_anima_missile_reverse_310987);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        if (creTarget->GetEntry() == 159298 && !creTarget->HasAura(310984) && !creTarget->IsHostileToPlayers())
                        {
                            uint8 hostile = urand(0, 1);
                            player->KilledMonsterCredit(159640);
                            if (hostile == 1)
                            {
                                creTarget->SetDefaultMovementType(IDLE_MOTION_TYPE);
                                creTarget->SetFaction(14);
                                creTarget->Attack(player, false);
                            }
                            else
                            {
                                creTarget->GetMotionMaster()->MoveRandom(20.f);
                                creTarget->ForcedDespawn(3000);
                            }
                        }
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_anima_missile_reverse_310987::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 329453 Retrieving
struct spell_retrieving_329453 : public SpellScript
{
    PrepareSpellScript(spell_retrieving_329453);

    void HandleHitTarget(SpellEffIndex effIndex)
    {
        Unit* target = GetHitUnit();

        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                PreventHitDefaultEffect(effIndex);

                if (!player->GetPhaseShift().HasPhase(10171)) PhasingHandler::AddPhase(player, 10171, true);
                if (player->GetPhaseShift().HasPhase(10173)) PhasingHandler::RemovePhase(player, 10173, true);
                if (player->GetQuestStatus(QUEST_WE_CAN_REBUILD_HIM) == QUEST_STATUS_INCOMPLETE)
                {
                    player->AddAura(347859, player);  //Gatamatos is Near
                    player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_CRITERIA_TREE, 82761, 1);
                }
                if (player->GetQuestStatus(QUEST_WE_CAN_REBUILD_HIM) == QUEST_STATUS_REWARDED)
                {
                    if (player->HasQuest(QUEST_COMBAT_DRILLS))
                        player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_CRITERIA_TREE, 82600, 1);
                    if (player->HasQuest(QUEST_LASER_LOCATION))
                        player->UpdateQuestObjectiveProgress(QUEST_OBJECTIVE_CRITERIA_TREE, 82594, 1);
                    player->AddAura(311157, player);  //Gatamatos is Near
                }
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_retrieving_329453::HandleHitTarget, EFFECT_0, SPELL_EFFECT_FORCE_CAST_2);
        OnEffectHitTarget += SpellEffectFn(spell_retrieving_329453::HandleHitTarget, EFFECT_1, SPELL_EFFECT_FORCE_CAST_2);
    }
};

// 311144 Placing
struct spell_placing_311144 : public SpellScript
{
    PrepareSpellScript(spell_placing_311144);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                if (player->HasAura(347859) || player->HasAura(311157))   //Gatamatos is Near
                {
                    if (!player->GetPhaseShift().HasPhase(10173)) PhasingHandler::AddPhase(player, 10173, true);
                    if (player->GetPhaseShift().HasPhase(10171)) PhasingHandler::RemovePhase(player, 10171, true);
                    player->RemoveAura(347859);
                    player->RemoveAura(311157);
                }
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_placing_311144::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 311370 Placing
struct spell_placing_311370 : public SpellScript
{
    PrepareSpellScript(spell_placing_311370);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (Unit* target = player->GetSelectedUnit())
                    if (Creature* creTarget = target->ToCreature())
                    {
                        if (player->GetQuestStatus(QUEST_WE_CAN_REBUILD_HIM) == QUEST_STATUS_INCOMPLETE && player->HasAura(AURA_GATAMATOS_IS_NEAR2))
                        {
                            player->RemoveAura(AURA_GATAMATOS_IS_NEAR2);
                            creTarget->RemoveAura(AURA_OBJECTIVE_PLACEMENT_VISUAL_STATE);
                            creTarget->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                            if (Creature* pelodis = player->FindNearestCreature(158765, 10.f, true))
                                pelodis->AI()->Talk(2);
                        }
                    }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_placing_311370::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 323329 Placing
struct spell_placing_323329 : public SpellScript
{
    PrepareSpellScript(spell_placing_323329);

    void HandleHitTarget(SpellEffIndex effIndex)
    {
        if (effIndex == 0)
            PreventHitDefaultEffect(effIndex);

        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !caster->HasAura(311278) || !target)
            return;

        target->CastSpell(caster, SPELL_PLACING);
        if (Creature* hopo = caster->FindNearestCreature(159609, 10.f, true))
            hopo->AI()->SetData(2, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_placing_323329::HandleHitTarget, EFFECT_0, SPELL_EFFECT_FORCE_CAST_2);
    }
};

Position const repair_Pos[4] =
{
    {-2656.68f, -5060.93f, 6611.67f, 4.93596f}, //0 
    {-2656.97f, -5063.36f, 6611.67f, 1.57053f}, //1 
    {-2654.67f, -5060.72f, 6611.71f, 4.10737f}, //2  
};

// 159793 Forgefire Engineer
struct npc_forgefire_engineer_159793 : public ScriptedAI
{
    npc_forgefire_engineer_159793(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    bool gata_near;

    void Initialize()
    {
        gata_near = false;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        m_playerGUID = summoner->GetGUID();
        Talk(0);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Creature* creature = who->ToCreature())
        {
            if (creature->IsInDist(me, 10.0f) && creature->GetEntry() == 159835 && !gata_near)
            {
                gata_near = true;
                me->GetMotionMaster()->Clear();
                if (FE_Count < 3)
                {
                    if (FE_Count == 0)
                        RepairPos(4.93596f);
                    else if (FE_Count == 1)
                        RepairPos(1.57053f);
                    else
                    {
                        RepairPos(4.10737f);
                        FE_Count = 0;
                    }
                    FE_Count++;
                }
                me->SetCurrentEquipmentId(1);
                me->HandleEmoteCommand(EMOTE_STATE_WORK_MINING);
                me->ForcedDespawn(300000);
                if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
                    player->KilledMonsterCredit(159793);
            }
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 3 && data == 1)
        {
            me->HandleEmoteCommand(EMOTE_STATE_NONE);
            me->SetCurrentEquipmentId(0);
            me->GetMotionMaster()->MoveRandom(10.f);
            me->ForcedDespawn(3000);
        }
    }

    void RepairPos(float ori)
    {
        me->GetMotionMaster()->MovePoint(0, repair_Pos[FE_Count], false, ori);
    }
};


// 338635 Reinforced Armor
struct go_reinforced_armor_338635 : public GameObjectAI
{
    go_reinforced_armor_338635(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasItemCount(173307, 5, false))
        {
            player->KilledMonsterCredit(159775);
            me->SetFlag(GO_FLAG_NOT_SELECTABLE);
            player->GetScheduler().Schedule(5s, [player, this](TaskContext context)
                {
                    me->DestroyForPlayer(player);
                });
        }
        return true;
    }
};


// 160022 Training Gem
struct npc_training_gem_160022 : public ScriptedAI
{
    npc_training_gem_160022(Creature* creature) : ScriptedAI(creature) { Initialize(); }
private:
    ObjectGuid m_playerGUID;
    bool player_near;

    void Initialize()
    {
        player_near = false;
    }

    void Reset() override
    {
        m_playerGUID = ObjectGuid::Empty;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->IsInDist(me, 10.0f) && player->HasQuest(QUEST_LASER_LOCATION) && !player_near)
            {
                m_playerGUID = player->GetGUID();
                player_near = true;
                player->AddAura(AURA_NEAR_TRAINING_GEM, player);
            }
            if (player->GetDistance2d(me) >= 12.0f && player_near)
            {
                player_near = false;
                player->RemoveAura(AURA_NEAR_TRAINING_GEM);
            }
        }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = ObjectAccessor::GetPlayer(*me, m_playerGUID))
        {
            player_near = false;
            player->RemoveAura(AURA_NEAR_TRAINING_GEM);
        }
    }
};

// 311682 Order Search
struct spell_order_search_311682 : public SpellScript
{
    PrepareSpellScript(spell_order_search_311682);

    void HandleHitTarget(SpellEffIndex effindex)
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
            {
                if (effindex == 1)
                {
                    PreventHitDefaultEffect(effindex);
                    std::list<Creature*> cList = player->FindNearestCreatures(160022, 10.0f);
                    for (std::list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); ++itr)
                    {
                        if (Creature* c_gem = *itr)
                        {
                            if (Creature* OP = player->FindNearestCreature(159698, 10.f))
                            {
                                OP->GetMotionMaster()->MoveJump(c_gem->GetPosition(), 10.f, 10.f);
                                player->GetScheduler().Schedule(5s, [player, c_gem](TaskContext context)
                                    {
                                        player->AddItem(173269, 1);
                                        c_gem->Kill(player, c_gem, false);
                                    });
                            }
                        }
                    }
                }
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_order_search_311682::HandleHitTarget, EFFECT_1, SPELL_EFFECT_FORCE_CAST_2);
    }
};

// 176100 IonaSkyblade <Keeper of Renown>
class IonaSkyblade : public CreatureScript
{
public:
    IonaSkyblade() : CreatureScript("IonaSkyblade") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new IonaSkybladeAI(creature);
    }

    struct IonaSkybladeAI : public ScriptedAI
    {
        IonaSkybladeAI(Creature* creature) : ScriptedAI(creature) { }

        void CovenantRenownOpcode(Player* player)
        {
            player->KilledMonsterCredit(175046);
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_OF_GREAT_RENOWN))
            {
                player->ForceCompleteQuest(QUEST_OF_GREAT_RENOWN);
            }

            AddGossipItemFor(player, GossipOptionNpc::Binder, "Show me Covenant renown", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                CovenantRenownOpcode(player);
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }
    };
};

// 328302
struct go_Anima_Conductor : public GameObjectAI
{
    go_Anima_Conductor(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(57903) == QUEST_STATUS_INCOMPLETE)// Choosing Your Purpose
            player->ForceCompleteQuest(57903);

        return true;
    }
};

class npc_polemarch_adrestes_168742 : public CreatureScript
{
public:
    npc_polemarch_adrestes_168742() : CreatureScript("npc_polemarch_adrestes_168742") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_polemarch_adrestes_168742AI(creature);
    }

    struct npc_polemarch_adrestes_168742AI : public ScriptedAI
    {
        npc_polemarch_adrestes_168742AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_AMONG_THE_KYRIAN))
            {
                player->ForceCompleteQuest(QUEST_AMONG_THE_KYRIAN);
            }
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            if (player->GetQuestStatus(QUEST_A_PROPER_RECEPTION) == QUEST_STATUS_INCOMPLETE)
            {
                player->TeleportTo(2222, -2162.9697f, -5298.892f, 6540.847f, 5.482f);
                player->ForceCompleteQuest(QUEST_A_PROPER_RECEPTION);
            }

            if (player->GetQuestStatus(QUEST_A_PROPER_RECEPTION) == QUEST_STATUS_REWARDED)
            {
                me->DespawnOrUnsummon();
            }

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }
    };
};

class npc_polemarch_adrestes_168906 : public CreatureScript
{
public:
    npc_polemarch_adrestes_168906() : CreatureScript("npc_polemarch_adrestes_168906") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_polemarch_adrestes_168906AI(creature);
    }

    struct npc_polemarch_adrestes_168906AI : public ScriptedAI
    {
        npc_polemarch_adrestes_168906AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_ELYSIAN_HOLD))
            {
                player->TeleportTo(2222, -1599.594f, -5852.212f, 6858.558f, 5.443f);
                player->ForceCompleteQuest(QUEST_ELYSIAN_HOLD);
            }
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }
    };
};

class npc_haephus_167745 : public CreatureScript
{
public:
    npc_haephus_167745() : CreatureScript("npc_haephus_167745") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_haephus_167745AI(creature);
    }

    struct npc_haephus_167745AI : public ScriptedAI
    {
        npc_haephus_167745AI(Creature* creature) : ScriptedAI(creature) { }

        void TestOp(Player* player)
        {
            /*WorldPackets::Garrison::GarrisonOpenTalentNpc send;
            send.NpcGUID = player->GetGUID();
            send.GarrTalentTreeID = 327;
            send.FriendshipFactionID = 0;
            player->GetSession()->SendPacket(send.Write());*/
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_INTO_THE_RESERVOIR))
            {
                player->ForceCompleteQuest(QUEST_INTO_THE_RESERVOIR);
            }

            AddGossipItemFor(player, GossipOptionNpc::Binder, "Show me the Sanctum", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                TestOp(player);
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }
    };
};

//154527
class Command_table_bastion : public CreatureScript
{
public:
    Command_table_bastion() : CreatureScript("Command_table_bastion") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new Command_table_bastionAI(creature);
    }

    struct Command_table_bastionAI : public ScriptedAI
    {
        Command_table_bastionAI(Creature* creature) : ScriptedAI(creature) { }

        void CommandtableOpcode(Player* player)
        {
            WorldPackets::Garrison::GarrisonOpenMissionNpcRequest;
            ObjectGuid NpcGUID;
            int32 GarrFollowerTypeID = 123;

            if (auto garrison = player->GetGarrison(GARRISON_TYPE_COVENANT))
                garrison->SendMissionListUpdate(true);
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            AddGossipItemFor(player, GossipOptionNpc::Binder, "Show me command table", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                CommandtableOpcode(player);
                me->SetNpcFlag2(NPCFlags2(UNIT_NPC_FLAG_2_GARRISON_MISSION_NPC | UNIT_NPC_FLAG_2_SHIPMENT_CRAFTER | UNIT_NPC_FLAG_2_GARRISON_ARCHITECT));
                break;
            }
            return true;
        }
    };
};

// Forge of bonds 327598
struct go_forge_of_bounds : public GameObjectAI
{
    go_forge_of_bounds(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        /* WorldPackets::Item::OpenItemForge send;
         send.UNK2 = 3;
         send.UNK5 = false;
         send.Flags = 71210495;
         send.Status = 3951035386;
         send.UNK = true;
         send.Created = true;
         send.UNK1 = 3373646143;
         send.VariableID = 11329;
         send.UNK3 = false;
         send.UNK4 = false;
         send.UNK6 = true;
         send.UNK7 = true;
         send.UNK9 = 0;
         send.UNK8 = true;
         send.UNK10 = 0;
         player->GetSession()->SendPacket(send.Write());*/

        return true;
    }
};

void AddSC_zone_bastion()
{
    new bastion_player();
    new scene_bastion();
    RegisterCreatureAI(npc_Kleia_166227);
    RegisterCreatureAI(npc_Kleia_165107);
    RegisterGameObjectAI(go_anima_canister_349508);
    RegisterSpellScript(spell_infusing_anima_319782);
    RegisterCreatureAI(npc_sika_166124);
    RegisterCreatureAI(npc_greeter_mnemis_158281);
    RegisterGameObjectAI(go_bench_349889_349528);
    RegisterCreatureAI(npc_kyrian_aspirant_156228);
    RegisterCreatureAI(npc_image_of_greeter_mnemis_166273);
    RegisterCreatureAI(npc_Kleia_170179);
    RegisterAuraScript(spell_burden_of_life_cooldown);
    RegisterCreatureAI(npc_visit_northeast_station_166332);
    RegisterGameObjectAI(go_compendium_of_arrival_335697);
    RegisterGameObjectAI(go_pure_thought_vesper_335698);
    RegisterCreatureAI(npc_Kleia_166316);
    RegisterSpellScript(spell_kleias_perspective_331259);
    RegisterCreatureAI(npc_pelagos_166323);
    RegisterCreatureAI(npc_altered_sentinel_168244);
    RegisterSpellScript(spell_ride_theonara_309540);
    RegisterCreatureAI(npc_forgelite_sophone_158807);
    RegisterSpellScript(aura_witness_312218);
    RegisterCreatureAI(npc_eternal_watcher_159191);
    RegisterSpellScript(spell_trigger_wave_event_310061);
    RegisterCreatureAI(npc_sika_166577);
    RegisterGameObjectAI(go_anima_extractor_349702);
    RegisterCreatureAI(npc_forgelite_sophone_159473);
    RegisterSpellScript(spell_raw_materials_324932);
    RegisterSpellScript(spell_Add_Fuel_310646);
    RegisterSpellScript(spell_holding_steady_310687);
    RegisterSpellScript(spell_open_vent_310647);
    RegisterCreatureAI(npc_steward_forgehand_159337);
    RegisterCreatureAI(npc_Kleia_159583);
    RegisterSpellScript(spell_gazing_312493);
    RegisterSpellScript(spell_teleport_311095);
    RegisterCreatureAI(npc_kalisthene_156217);
    RegisterCreatureAI(npc_kyrian_ascended_159703_159704);
    RegisterCreatureAI(npc_Kalisthenes_gift_159749);
    RegisterSpellScript(spell_kalisthenes_grace_311245);
    RegisterCreatureAI(npc_adonas_175014);
    RegisterCreatureAI(npc_disciple_kosmas_157673);
    RegisterCreatureAI(npc_vulnerable_aspirant);
    RegisterCreatureAI(npc_disciple_fotima_159840);
    RegisterGameObjectAI(go_vesper_of_purity_334847);
    RegisterCreatureAI(npc_pelagos_165110);
    RegisterCreatureAI(npc_disciple_helene_160636);
    RegisterCreatureAI(npc_disciple_lykaste_159841);
    RegisterGameObjectAI(go_vesper_of_harmony_334849);
    RegisterCreatureAI(npc_disciple_nikolon_159897);
    RegisterCreatureAI(npc_eridia_159906);
    RegisterGameObjectAI(go_dark_sermon_343603);
    RegisterCreatureAI(npc_disciple_kosmas_168813);
    RegisterSpellScript(spell_hurl_kyrian_hammer_319525);
    RegisterCreatureAI(npc_vesiphone_167865);
    RegisterCreatureAI(npc_vesiphone_164325);
    RegisterCreatureAI(npc_mikanikos_167034);
    RegisterCreatureAI(npc_memory_extractor_167211);
    RegisterSpellScript(spell_empower_extractor_342663);
    RegisterSpellScript(spell_imbuing_326093);
    RegisterSpellScript(spell_extract_memory_325609);
    RegisterSpellScript(spell_rousing_334629);
    RegisterCreatureAI(npc_mikanikos_167368);
    RegisterCreatureAI(npc_kleia_167370);
    RegisterCreatureAI(npc_image_of_locus_keeper_mnemis_174531);
    RegisterCreatureAI(npc_kleia_167562);
    RegisterCreatureAI(npc_vault_keeper_mnemis_167504);
    RegisterSpellScript(spell_activating_325341);
    RegisterGameObjectAI(go_vesper_of_remembrance_335614);
    RegisterCreatureAI(npc_memory_of_korinna_170253);
    RegisterSpellScript(spell_throw_slobber_ball_337218);
    RegisterSpellScript(spell_reactivating_311032);
    RegisterSpellScript(spell_anima_missile_reverse_310987);
    RegisterSpellScript(spell_retrieving_329453);
    RegisterSpellScript(spell_placing_311144);
    RegisterSpellScript(spell_placing_311370);
    RegisterSpellScript(spell_placing_323329);
    RegisterCreatureAI(npc_forgefire_engineer_159793);
    RegisterGameObjectAI(go_reinforced_armor_338635);
    RegisterCreatureAI(npc_training_gem_160022);
    RegisterSpellScript(spell_order_search_311682);

    new IonaSkyblade();
    RegisterGameObjectAI(go_Anima_Conductor);
    new npc_polemarch_adrestes_168742();
    new npc_polemarch_adrestes_168906();
    new npc_haephus_167745();
    new Command_table_bastion();
    RegisterGameObjectAI(go_forge_of_bounds);
}
