/*
 * Copyright 2023 Fluxurion
 */

/* To worldserver.conf:
###################################################################################################
# Login Announcer #
###################
#
# System message tells everyone when a player enters the world
# with information about name, level, race, class. (fancy icons included)
#

Login.Announcer.Enable = 1

#
###################################################################################################
*/
#include "Config.h"
#include "World.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Log.h"
#include "Flux.h"
#include <sstream>


class login_announcer : public PlayerScript
{
public:
    login_announcer() : PlayerScript("login_announcer") { }

    void OnLogin(Player* player, bool /*loginFirst*/)
    {
        if (sConfigMgr->GetBoolDefault("Login.Announcer.Enable", false))
        {

            uint32 pLevel = player->GetLevel();
            std::string pClass, pRace, pGender, pCovenant, pFaction, pLoginText;

            switch (player->GetClass())
            {
            case CLASS_WARLOCK:
                pClass = "|TInterface\\icons\\classicon_warlock:15|t";
                break;
            case CLASS_WARRIOR:
                pClass = "|TInterface\\icons\\classicon_warrior:15|t";
                break;
            case CLASS_MAGE:
                pClass = "|TInterface\\icons\\classicon_mage:15|t";
                break;
            case CLASS_SHAMAN:
                pClass = "|TInterface\\icons\\classicon_shaman:15|t";
                break;
            case CLASS_DEATH_KNIGHT:
                pClass = "|TInterface\\icons\\spell_deathknight_classicon:15|t";
                break;
            case CLASS_DRUID:
                pClass = "|TInterface\\icons\\classicon_druid:15|t";
                break;
            case CLASS_HUNTER:
                pClass = "|TInterface\\icons\\classicon_hunter:15|t";
                break;
            case CLASS_PALADIN:
                pClass = "|TInterface\\icons\\classicon_paladin:15|t";
                break;
            case CLASS_ROGUE:
                pClass = "|TInterface\\icons\\classicon_rogue:15|t";
                break;
            case CLASS_PRIEST:
                pClass = "|TInterface\\icons\\classicon_priest:15|t";
                break;
            case CLASS_DEMON_HUNTER:
                pClass = "|TInterface\\icons\\classicon_demonhunter:15|t";
                break;
            case CLASS_MONK:
                pClass = "|TInterface\\icons\\classicon_monk:15|t";
                break;
            }

            switch (player->GetGender())
            {
            case GENDER_MALE:
                pGender = "Male";
                break;
            case GENDER_FEMALE:
                pGender = "Female";
                break;
            case GENDER_UNKNOWN:
                pGender = "Gay";
                break;
            case GENDER_NONE:
                pGender = "Nongender";
                break;
            }

            switch (player->GetRace())
            {
            case RACE_HUMAN:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_human_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_human_female:15|t";
                break;
            case RACE_ORC:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_orc_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_orc_female:15|t";
                break;
            case RACE_DWARF:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_dwarf_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_dwarf_female:15|t";
                break;
            case RACE_NIGHTELF:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_nightelf_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_nightelf_female:15|t";
                break;
            case RACE_UNDEAD_PLAYER:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_undead_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_undead_female:15|t";
                break;
            case RACE_TAUREN:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_tauren_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_tauren_female:15|t";
                break;
            case RACE_GNOME:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_gnome_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_gnome_female:15|t";
                break;
            case RACE_TROLL:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_troll_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_troll_female:15|t";
                break;
            case RACE_GOBLIN:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_goblinhead:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_femalegoblinhead:15|t";
                break;
            case RACE_BLOODELF:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_bloodelf_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_bloodelf_female:15|t";
                break;
            case RACE_DRAENEI:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_draenei_male:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_draenei_female:15|t";
                break;
            case RACE_WORGEN:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_worganhead:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_worganhead:15|t";
                break;
            case RACE_PANDAREN_NEUTRAL:
                if (pGender == "Male") pRace = "|TInterface\\icons\\pandarenracial_innerpeace:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_pandaren_female:15|t";
                break;
            case RACE_PANDAREN_HORDE:
                if (pGender == "Male") pRace = "|TInterface\\icons\\pandarenracial_innerpeace:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_pandaren_female:15|t";
                break;
            case RACE_PANDAREN_ALLIANCE:
                if (pGender == "Male") pRace = "|TInterface\\icons\\pandarenracial_innerpeace:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_pandaren_female:15|t";
                break;
            case RACE_NIGHTBORNE:
                if (pGender == "Male") pRace = "|TInterface\\icons\\inv_nightbornemale:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\inv_nightbornefemale:15|t";
                break;
            case RACE_VOID_ELF:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_alliedrace_voidelf:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_alliedrace_voidelf:15|t";
                break;
            case RACE_HIGHMOUNTAIN_TAUREN:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_alliedrace_highmountaintauren:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_alliedrace_highmountaintauren:15|t";
                break;
            case RACE_LIGHTFORGED_DRAENEI:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_alliedrace_lightforgeddraenei:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_alliedrace_lightforgeddraenei:15|t";
                break;
            case RACE_ZANDALARI_TROLL:
                if (pGender == "Male") pRace = "|TInterface\\icons\\inv_zandalarimalehead:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\inv_zandalarifemalehead:15|t";
                break;
            case RACE_KUL_TIRAN:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_alliedrace_kultiranhuman:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_alliedrace_kultiranhuman:15|t";
                break;
            case RACE_DARK_IRON_DWARF:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_alliedrace_darkirondwarf:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_alliedrace_darkirondwarf:15|t";
                break;
            case RACE_VULPERA:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_alliedrace_vulpera:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_alliedrace_vulpera:15|t";
                break;
            case RACE_MAGHAR_ORC:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_character_orc_male_brn:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_character_orc_female_brn:15|t";
                break;
            case RACE_MECHAGNOME:
                if (pGender == "Male") pRace = "|TInterface\\icons\\achievement_alliedrace_mechagnome:15|t";
                if (pGender == "Female") pRace = "|TInterface\\icons\\achievement_alliedrace_mechagnome:15|t";
                break;
            }

            switch (player->GetCovenant())
            {
            case Covenant::Kyrian:
                pCovenant = "|TInterface\\icons\\ui_sigil_kyrian:15|t";
                break;
            case Covenant::NightFae:
                pCovenant = "|TInterface\\icons\\ui_sigil_nightfae:15|t";
                break;
            case Covenant::Necrolord:
                pCovenant = "|TInterface\\icons\\ui_sigil_necrolord:15|t";
                break;
            case Covenant::Venthyr:
                pCovenant = "|TInterface\\icons\\ui_sigil_venthyr:15|t";
                break;
            }

            std::ostringstream loginmsg, infomsg;

            switch (player->GetTeamId())
            {
            case TEAM_ALLIANCE:
                pFaction = "|cff0026FF|TInterface\\icons\\ui_allianceicon:15|t|r";
                break;
            case TEAM_HORDE:
                pFaction = "|cffFF0000|TInterface\\icons\\ui_hordeicon:15|t|r";
                break;
            case TEAM_NEUTRAL:
                pFaction = "|cFFF5F5DC|TInterface\\icons\\vas_guildfactionchange:15|t|r";
                break;
            }

            switch (urand(1, 4))
            {
            case 1:
                pLoginText = "|cff4CFF00 has entered the world!|r";
                    break;
            case 2:
                pLoginText = "|cff4CFF00 has logged in!|r";
                    break;
            case 3:
                pLoginText = "|cff4CFF00 has spawned!|r";
                    break;
            case 4:
                pLoginText = "|cff4CFF00 has dropped in!|r";
                    break;
            }

            loginmsg
                << "|cff3DAEFF|TInterface\\icons\\inv_misc_token_boost_generic:15|t [Login Announcer]: "
                << "|cffFFD800|TInterface\\icons\\inv_prg_icon_puzzle04:15|t" << player->GetName() << pLoginText << " |TInterface\\icons\\inv_prg_icon_puzzle02:15|t"
                << "|cff3DAEFF [" << pLevel << "] " << pCovenant << " " << pFaction << " " << pRace << " " << pClass << "|r";

            sWorld->SendServerMessage(SERVER_MSG_STRING, loginmsg.str().c_str());

            TC_LOG_INFO("server.LoginAnnouncer", "[LoginAnnouncer]: {} Lvl {} player has entered the world! ChromieTime: {}", player->GetName(), pLevel, Fluxurion::GetChromieTime(player));

        }
    }
};

void AddSC_login_announcer()
{
    RegisterPlayerScript(login_announcer);
}
