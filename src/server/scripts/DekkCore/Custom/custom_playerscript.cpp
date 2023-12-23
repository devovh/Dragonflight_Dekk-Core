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

#include "Conversation.h"
#include "Creature.h"
#include "Channel.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "DBCEnums.h"
#include "GameEventMgr.h"
#include "GameObjectAI.h"
#include "MiscPackets.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "PhasingHandler.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "World.h"
#include "WorldStatePackets.h"
#include "ScriptMgr.h"
#include "PetBattle.h"
#include "Common.h"
#include "BattlePetSystem.h"
#include <Configuration/Config.h>

enum miscsl
{
    SPELL_CREATE_GARRISON_ARTIFACT_1 = 309389,
    SPELL_CREATE_GARRISON_ARTIFACT_2 = 309390,
    SPELL_CREATE_GARRISON_ARTIFACT_3 = 309391,
    SPELL_CREATE_GARRISON_SL         = 322277,
};

 //player_scripts
class ps_sl_covenant : public PlayerScript
{
public:
    ps_sl_covenant() : PlayerScript("ps_sl_covenant") { }

    void OnPlayerChoiceResponse(Player* player, uint32 choiceId, uint32 responseId) override
    {
        if (choiceId != 644)
            return;
        //printf("OnPlayerChoiceResponse %d\n", responseId);
        switch (responseId)
        {
        case 1230://Venthyr Pre
            player->GetSession()->SendCovenantPreview(player->GetGUID(), 2);
            break;
        case 1231://Necrolords
            player->SetCovenant(Covenant::Necrolord);
            break;
        case 1233://Night Fae
            player->SetCovenant(Covenant::NightFae);
            break;
        case 1227://Kyrian
            player->SetCovenant(Covenant::Kyrian);
            break;
        case 1229://Venthyr
            player->SetCovenant(Covenant::Venthyr);
            break;
        case 1232://Necrolords Pre
            player->GetSession()->SendCovenantPreview(player->GetGUID(), 4);
            break;
        case 1234://Night Fae Pre
            player->GetSession()->SendCovenantPreview(player->GetGUID(), 3);
            break;
        case 1228://Kyrian Pre
            player->GetSession()->SendCovenantPreview(player->GetGUID(), 1);
            break;
        default:
            break;
        }
    }
};

// 355352
struct go_shadowlands_covenant_map : public GameObjectAI
{
    go_shadowlands_covenant_map(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(62000) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(57878) == QUEST_STATUS_INCOMPLETE)// Choosing Your Purpose
            player->SendPlayerChoice(player->GetGUID(), 644);

        return true;
    }
};
/*
171589  ???
171821 ????
171795 ??
171787 ???
*/
struct npc_quest_choosing_your_purpose : public ScriptedAI
{
    npc_quest_choosing_your_purpose(Creature* creature) : ScriptedAI(creature) {  }

    bool OnGossipHello(Player* player) override
    {
        uint32 menuId = 0;

        if (me->GetEntry() == 171589)
            menuId = 26041;
        if (me->GetEntry() == 171821)
            menuId = 26133;
        if (me->GetEntry() == 171795)
            menuId = 26132;
        if (me->GetEntry() == 171787)
            menuId = 26131;

        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (player->GetQuestStatus(62000) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(57878) == QUEST_STATUS_INCOMPLETE)// Choosing Your Purpose
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, menuId, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            AddGossipItemFor(player, menuId, 3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            AddGossipItemFor(player, menuId, 4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            AddGossipItemFor(player, menuId, 5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

            SendGossipMenuFor(player, player->GetGossipTextId(menuId, me), me->GetGUID());

            return true;
        }

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);

        player->KilledMonsterCredit(me->GetEntry());
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF:
            if (me->GetEntry() == 171589)player->GetSession()->SendCovenantPreview(player->GetGUID(), 2);
            if (me->GetEntry() == 171821) player->GetSession()->SendCovenantPreview(player->GetGUID(), 4);
            if (me->GetEntry() == 171795) player->GetSession()->SendCovenantPreview(player->GetGUID(), 3);
            if (me->GetEntry() == 171787) player->GetSession()->SendCovenantPreview(player->GetGUID(), 1);
            break;

        case GOSSIP_ACTION_INFO_DEF + 3:
            player->RemoveAurasDueToSpell(340835);
            player->RemoveAurasDueToSpell(341085);

            player->RemoveAurasDueToSpell(341086);
            player->RemoveAurasDueToSpell(341089);

            player->RemoveAurasDueToSpell(341090);
            player->RemoveAurasDueToSpell(341091);

            player->RemoveAurasDueToSpell(341092);
            player->RemoveAurasDueToSpell(341093);

            player->RemoveAurasDueToSpell(341174);
            player->RemoveAurasDueToSpell(341170);
            player->RemoveAurasDueToSpell(341166);
            player->RemoveAurasDueToSpell(341168);
            if (me->GetEntry() == 171589)
                player->CastSpell(player, 341174, true);
            if (me->GetEntry() == 171821)
                player->CastSpell(player, 341170, true);
            if (me->GetEntry() == 171795)
                player->CastSpell(player, 341166, true);
            if (me->GetEntry() == 171787)
                player->CastSpell(player, 341168, true);

            CloseGossipMenuFor(player);
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            CloseGossipMenuFor(player);
            player->CastSpell(player, 341271, true);//Anima Infusion
            player->GetSpellHistory()->ResetCooldown(341092, true);
            player->GetSpellHistory()->ResetCooldown(341093, true);
            player->GetSpellHistory()->ResetCooldown(341090, true);
            player->GetSpellHistory()->ResetCooldown(341091, true);
            player->GetSpellHistory()->ResetCooldown(341086, true);
            player->GetSpellHistory()->ResetCooldown(341089, true);
            player->GetSpellHistory()->ResetCooldown(340835, true);
            player->GetSpellHistory()->ResetCooldown(341085, true);

            if (me->GetEntry() == 171589)
            {
                player->GetSpellHistory()->ResetCooldown(317485, true);
                player->GetSpellHistory()->ResetCooldown(300728, true);
            }
            if (me->GetEntry() == 171821)
            {
                player->GetSpellHistory()->ResetCooldown(324143, true);
                player->GetSpellHistory()->ResetCooldown(324631, true);
            }
            if (me->GetEntry() == 171795)
            {
                player->GetSpellHistory()->ResetCooldown(325886, true);
                player->GetSpellHistory()->ResetCooldown(310143, true);
            }
            if (me->GetEntry() == 171787)
            {
                player->GetSpellHistory()->ResetCooldown(324739, true);
                player->GetSpellHistory()->ResetCooldown(324143, true);
                player->GetSpellHistory()->ResetCooldown(324631, true);
            }
            player->GetSpellHistory()->ResetCooldown(313347, true);
            player->GetSpellHistory()->ResetCooldown(326526, true);

            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            CloseGossipMenuFor(player);
            ClearGossipMenuFor(player);
            if (me->GetEntry() == 171589) SendGossipMenuFor(player, player->GetGossipTextId(26516, me), me->GetGUID());
            if (me->GetEntry() == 171821) SendGossipMenuFor(player, player->GetGossipTextId(26517, me), me->GetGUID());
            if (me->GetEntry() == 171795) SendGossipMenuFor(player, player->GetGossipTextId(26514, me), me->GetGUID());
            if (me->GetEntry() == 171787) SendGossipMenuFor(player, player->GetGossipTextId(26513, me), me->GetGUID());
            break;
        }

        return true;
    }
};

class PlayerScript_WeeklyQuests : public PlayerScript
{
public:
    PlayerScript_WeeklyQuests() : PlayerScript("PlayerScript_WeeklyQuests") {}

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (!player)
            return;

        if (!sGameEventMgr->IsActiveEvent(130))
        {
           // player->RemoveActiveQuest(62631, true); // Evento Bonificacion Misiones de mundo
            player->RemoveAura(225788);
        }
        else
            player->CastSpell(player, 225788, false);

        if (!sGameEventMgr->IsActiveEvent(131))
        {
          //  player->RemoveActiveQuest(62637); // Evento Bonificacion Campos de Batalla
            player->RemoveAura(186403);
        }
        else
            player->CastSpell(player, 186403, false);

        if (!sGameEventMgr->IsActiveEvent(132))
        {
        //    player->RemoveActiveQuest(62639); // Evento Bonificacion Mascotas
            player->RemoveAura(186406);
        }
        else
            player->CastSpell(player, 186406, false);

        if (!sGameEventMgr->IsActiveEvent(133))
        {
           // player->RemoveActiveQuest(62640); // Evento Bonificacion Escaramusas
            player->RemoveAura(186401);
        }
        else
            player->CastSpell(player, 186401, false);

        if (!sGameEventMgr->IsActiveEvent(134))
        {
           // player->RemoveActiveQuest(62638); // Evento Bonificacion Mamorras Miticas
            player->RemoveAura(225787);
        }
        else
            player->CastSpell(player, 225787, false);
    }

    void OnMapChanged(Player* player) override
    {
        if (!player)
            return;

        if (!sGameEventMgr->IsActiveEvent(130))
            player->RemoveAura(225788);
        if (!sGameEventMgr->IsActiveEvent(131))
            player->RemoveAura(186403);
        if (!sGameEventMgr->IsActiveEvent(132))
            player->RemoveAura(186406);
        if (!sGameEventMgr->IsActiveEvent(133))
            player->RemoveAura(186401);
        if (!sGameEventMgr->IsActiveEvent(134))
            player->RemoveAura(225787);

    }
};

enum Create_Garrison
{
    SPELL_CREATE_WAR_CAMPAIGN_H = 273381,
    SPELL_CREATE_WAR_CAMPAIGN_A = 273382
};

class player_level_rewards : public PlayerScript
{
public:
    player_level_rewards() : PlayerScript("player_level_rewards") {}

    void OnLevelChanged(Player* player, uint8 oldLevel) override
    {
        if (oldLevel <= 20 && player->GetLevel() >= 21) // Create WarCampaing
        {
            player->GetTeam() == ALLIANCE ? player->CastSpell(player, SPELL_CREATE_WAR_CAMPAIGN_A, false) : player->CastSpell(player, SPELL_CREATE_WAR_CAMPAIGN_H, false);
        }

        if (oldLevel <= 9 && player->GetLevel() >= 10)
        {
            switch (player->GetRace()) // Heritage Armor
            {
            case RACE_VOID_ELF:
                if (player->GetQuestStatus(49928) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(49928))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_LIGHTFORGED_DRAENEI:
                if (player->GetQuestStatus(49782) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(49782))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_NIGHTBORNE:
                if (player->GetQuestStatus(49784) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(49784))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_HIGHMOUNTAIN_TAUREN:
                if (player->GetQuestStatus(49783) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(49783))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_DARK_IRON_DWARF:
                if (player->GetQuestStatus(51483) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(51483))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_MAGHAR_ORC:
                if (player->GetQuestStatus(51484) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(51484))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_ZANDALARI_TROLL:
                if (player->GetQuestStatus(53721) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(53721))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_KUL_TIRAN:
                if (player->GetQuestStatus(53722) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(53722))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_VULPERA:
                if (player->GetQuestStatus(58435) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(58435))
                        player->AddQuest(quest, nullptr);
                break;
            case RACE_MECHAGNOME:
                if (player->GetQuestStatus(58436) == QUEST_STATUS_NONE)
                    if (const Quest* quest = sObjectMgr->GetQuestTemplate(58436))
                        player->AddQuest(quest, nullptr);
                break;
            default:
                break;
            }
        }
    }
};

class Boss_Announcer : public PlayerScript
{
public:
    Boss_Announcer() : PlayerScript("Boss_Announcer") {}

    void OnCreatureKill(Player* player, Creature* boss)
    {
        if (sConfigMgr->GetBoolDefault("Boss.Announcer.Enable", true))
        {
            if (boss->isWorldBoss())
            {
                std::string plr = player->GetName();
                std::string boss_n = boss->GetName();
                std::string tag_colour = "7bbef7";
                std::string plr_colour = "7bbef7";
                std::string boss_colour = "ff0000";
                std::ostringstream stream;
                stream << "|CFF" << tag_colour <<

                    "|rThe group of|r""|r|cff" << plr_colour << " " << plr << "|r Killed "" |CFF" << boss_colour << "[" << boss_n << "]|r " "World Boss" << "! Congratulations!";
                sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
            }
        }
    };
};

class DemonHunterTalentTempFix : public PlayerScript
{
public:
    DemonHunterTalentTempFix() : PlayerScript("DemonHunterTalentTempFix") { }

    void OnLogin(Player* player, bool /* firstLogin */) override
    {
        if (player->GetClass() == CLASS_DEMON_HUNTER && !player->HasSpell(200749))
            player->LearnSpell(200749, false);
    }

    void OnLevelChanged(Player* player, uint8 /*oldLevel*/) override
    {
        if (player->GetClass() == CLASS_DEMON_HUNTER && !player->HasSpell(200749))
            player->LearnSpell(200749, false);
    }
};

class OnWodGarrisonArrival : public PlayerScript
{
public:
    OnWodGarrisonArrival() : PlayerScript("OnWodGarrisonArrival") { }

    enum Pregarrquest
    {
        QUEST_A_STEP_THREE_PROPHET           = 34575,
        QUEST_H_THE_HOME_OF_THE_FROST_WOLVES = 33868,
    };

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (player->GetQuestStatus(QUEST_A_STEP_THREE_PROPHET) != QUEST_STATUS_REWARDED)
            return;

        if (player->GetQuestStatus(QUEST_H_THE_HOME_OF_THE_FROST_WOLVES) != QUEST_STATUS_REWARDED)
            return;

        if (player->IsInAlliance() && (player->GetZoneId() == 6719))
            player->AddQuest(sObjectMgr->GetQuestTemplate(QUEST_A_STEP_THREE_PROPHET), nullptr);

        if (player->IsInHorde() && (player->GetZoneId() == 6720))
            player->AddQuest(sObjectMgr->GetQuestTemplate(QUEST_H_THE_HOME_OF_THE_FROST_WOLVES), nullptr);
    }
};

class CloseGossipWindowHackFix : public PlayerScript
{
public:
    CloseGossipWindowHackFix() : PlayerScript("CloseGossipWindowHackFix") { }

    // Called when player accepts some quest
    void OnQuestAccept(Player* player, Quest const* /*quest*/) override
    {
        player->PlayerTalkClass->SendCloseGossip();
        player->PlayerTalkClass->GetInteractionData().Reset();
    }

    // Called when player rewards some quest
    void OnQuestReward(Player* player, Quest const* /*quest*/) override
    {
        player->PlayerTalkClass->SendCloseGossip();
        player->PlayerTalkClass->GetInteractionData().Reset();
    }
};

class ps_sl_CreateGarrison : public PlayerScript
{
public:
    ps_sl_CreateGarrison() : PlayerScript("ps_sl_CreateGarrison") { }


    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (player->GetMapId() == 2222)
        {
            player->CastSpell(player, SPELL_CREATE_GARRISON_ARTIFACT_1, true);
            player->CastSpell(player, SPELL_CREATE_GARRISON_ARTIFACT_2, true);
            player->CastSpell(player, SPELL_CREATE_GARRISON_ARTIFACT_3, true);
            player->CastSpell(player, SPELL_CREATE_GARRISON_SL, true);
        }
    }
};

class DragonflightArrival : public PlayerScript
{
public:
    DragonflightArrival() : PlayerScript("DragonflightArrival") { }

    enum dfquest
    {
        QUEST_THE_DRAGON_ISLES_AWAIT_ALLIANCE = 65436,
        QUEST_THE_DRAGON_ISLES_AWAIT_HORDE = 65435,
    };

    void OnLogin(Player* player, bool firstLogin) override
    {

        if (firstLogin)
        {
            switch (player->GetTeam())
            {
            case ALLIANCE:

                if (player->GetLevel() == 10)
                    player->AddQuest(sObjectMgr->GetQuestTemplate(QUEST_THE_DRAGON_ISLES_AWAIT_ALLIANCE), nullptr);
                player->ForceCompleteQuest(QUEST_THE_DRAGON_ISLES_AWAIT_ALLIANCE);

                break;

            case HORDE:
                if (player->GetLevel() == 10)
                    player->AddQuest(sObjectMgr->GetQuestTemplate(QUEST_THE_DRAGON_ISLES_AWAIT_HORDE), nullptr);
                player->ForceCompleteQuest(QUEST_THE_DRAGON_ISLES_AWAIT_HORDE);
                break;
            }
        }
    }
};

class DragonRidingAchievements : public PlayerScript
{
public:
    DragonRidingAchievements() : PlayerScript("DragonRidingAchievements") { }

    void OnLogin(Player* player, bool firstLogin) override
    {
        if (firstLogin)
            { 
            player->CastSpell(player, 384557);
            player->CompletedAchievement(sAchievementStore.LookupEntry(16051));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16107));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16106));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16105));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16104));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16103));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16102));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16101));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16100));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16099));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16098));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16073));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16072));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16071));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16070));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16069));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16068));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16067));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16066));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16065));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16064));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16063));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16062));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16061));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16060));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16059));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16058));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16057));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16056));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16055));
            player->CompletedAchievement(sAchievementStore.LookupEntry(15987));
            player->CompletedAchievement(sAchievementStore.LookupEntry(15986));
            player->CompletedAchievement(sAchievementStore.LookupEntry(15985));
            player->CompletedAchievement(sAchievementStore.LookupEntry(15988));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16052));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16054));
            player->CompletedAchievement(sAchievementStore.LookupEntry(15991));
            player->CompletedAchievement(sAchievementStore.LookupEntry(15990));
            player->CompletedAchievement(sAchievementStore.LookupEntry(15989));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16666));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16667));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16670));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16668));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16669));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16671));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16672));
            player->CompletedAchievement(sAchievementStore.LookupEntry(16673));
        }
    }
};


class System_Taberna : public PlayerScript
{
public:
    System_Taberna() : PlayerScript("System_Taberna") { }

    void AddIcon(Player* player, std::string& msg, uint32 lang)
    {
        if (player->isGMChat())
            return;
        if (player->GetTeamId() == 0)
            msg = "|TInterface/FriendsFrame/PlusManz-Alliance:20:21|t" + msg;
        else if (player->GetTeamId() == 1)
            msg = "|TInterface/FriendsFrame/PlusManz-Horde:20:21|t" + msg;

    };

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel) override
    {
        std::string canal = channel->GetName();
        std::transform(canal.begin(), canal.end(), canal.begin(), ::tolower);
        if (canal == "|cFFFFF569taberna|r")  AddIcon(player, msg, lang);
    }
};

class PromotionNewPlayers : public PlayerScript
{
public:
    PromotionNewPlayers() :PlayerScript("PromotionNewPlayers") {}

    enum Values
    {
        April2023 = 1422777600,  ///< Timestamp of 1st of April 2023.
        May252023 = 1427065200,  ///< Timestamp of 25nd of May 2023.
        June152023 = 1426374000,  ///< Timestamp of 15nd of June 2023.
        SwiftNetherDrake = 37015,   ///< SpellId.
        ImperialQuilen = 85870,   ///< ItemId
        ApprenticeRidingSkill = 33388
    };

    void OnLogin(Player* p_Player, bool /*firstLogin*/) override
    {
        if (time(NULL) < Values::April2023 && p_Player->GetTotalPlayedTime() == 0)
        {
            p_Player->LearnSpell(Values::ApprenticeRidingSkill, false);
            p_Player->LearnSpell(Values::SwiftNetherDrake, false);
        }

        if (time(nullptr) < Values::May252023
            && p_Player->GetSession()->GetAccountJoinDate() >= Values::May252023
            && p_Player->GetTotalPlayedTime() == 0)
            p_Player->AddItem(Values::ImperialQuilen, 1);
    }
};

class AccountAchievements : public PlayerScript
{
    static const bool limitrace = true; // This set to true will only achievements from chars on the same team, do what you want. NOT RECOMMANDED TO BE CHANGED!!!
    static const bool limitlevel = true; // This checks the player's level and will only add achievements to players of that level.
    int minlevel = 70; // It's set to players of the level 60. Requires limitlevel to be set to true.
    int setlevel = 1; // Dont Change

public:
    AccountAchievements() : PlayerScript("AccountAchievements") { }

    void OnLogin(Player* pPlayer, bool /*firstlogin*/)
    {
        if (sConfigMgr->GetBoolDefault("Accountwide.Achievements", true)) {
            std::vector<uint32> Guids;
            QueryResult result1 = CharacterDatabase.PQuery("SELECT guid, race FROM characters WHERE account = {}", pPlayer->GetSession()->GetAccountId());
            if (!result1)
                return;

            do
            {
                Field* fields = result1->Fetch();

                //uint32 guid = fields[0].GetUInt32();
                uint32 race = fields[1].GetUInt8();

                if ((Player::TeamForRace(race) == Player::TeamForRace(pPlayer->GetRace())) || !limitrace)
                    Guids.push_back(result1->Fetch()[0].GetUInt32());

            } while (result1->NextRow());

            std::vector<uint32> Achievement;

            for (auto& i : Guids)
            {
                QueryResult result2 = CharacterDatabase.PQuery("SELECT achievement FROM character_achievement WHERE guid = {}", i);
                if (!result2)
                    continue;

                do
                {
                    Achievement.push_back(result2->Fetch()[0].GetUInt32());
                } while (result2->NextRow());
            }

            for (auto& i : Achievement)
            {
                auto sAchievement = sAchievementStore.LookupEntry(i);
                AddAchievements(pPlayer, sAchievement->ID);
            }
        }

        else {
            return;
        }
    }

    void AddAchievements(Player* player, uint32 AchievementID)
    {
        if (limitlevel)
            setlevel = minlevel;

        if (player->GetLevel() >= setlevel)
            player->CompletedAchievement(sAchievementStore.LookupEntry(AchievementID));
    }
};

class PlayerScriptPetBattle : public PlayerScript
{
public:
    PlayerScriptPetBattle() : PlayerScript("PlayerScriptPetBattle") { }

    void OnMovementInform(Player* player, uint32 mveType, uint32 id) override
    {
        if (player && mveType == POINT_MOTION_TYPE && id == PET_BATTLE_ENTER_MOVE_SPLINE_ID)
        {
            m_Mutex.lock();
            m_DelayedPetBattleStart[player->GetGUID()] = getMSTime() + 1000;
            m_Mutex.unlock();
        }
    }

    void OnUpdate(Player* player, uint32 /*diff*/) override
    {
        m_Mutex.lock();

        if (m_DelayedPetBattleStart.find(player->GetGUID()) != m_DelayedPetBattleStart.end())
        {
            if (m_DelayedPetBattleStart[player->GetGUID()] > getMSTime())
            {
                m_DelayedPetBattleStart.erase(m_DelayedPetBattleStart.find(player->GetGUID()));

                if (PetBattle* battle = sPetBattleSystem->GetBattle(player->_petBattleId))
                {
                    player->SetUnitFlag(UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_NPC); // Immuned only to NPC
                    player->SetControlled(true, UNIT_STATE_ROOT);
                    battle->Begin();
                }
            }
        }

        m_Mutex.unlock();
    }

    std::map<ObjectGuid, uint32> m_DelayedPetBattleStart;
    std::mutex m_Mutex;
};

void AddCustom_playerscript()
{
    new ps_sl_covenant();
    RegisterGameObjectAI(go_shadowlands_covenant_map);
    RegisterCreatureAI(npc_quest_choosing_your_purpose);
    new PlayerScript_WeeklyQuests();
    RegisterPlayerScript(player_level_rewards);
    RegisterPlayerScript(Boss_Announcer);
    RegisterPlayerScript(DemonHunterTalentTempFix); // TEMP FIX! remove when playerchoice has been fixed properly.
    RegisterPlayerScript(OnWodGarrisonArrival);
    RegisterPlayerScript(CloseGossipWindowHackFix);
    new ps_sl_CreateGarrison();
    new DragonflightArrival();
    new DragonRidingAchievements();
    new System_Taberna();
    new PromotionNewPlayers();
    new AccountAchievements();
    new PlayerScriptPetBattle;
}
