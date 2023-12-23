#include "Player.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "Opcodes.h"
#include "MiscPackets.h"
#include "TemporarySummon.h"
#include "DB2Stores.h"
#include "Garrison.h"
#include "GarrisonMgr.h"
#include "Language.h"
#include "Player.h"
#include "RBAC.h"
#include "WarCampaign.h"

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <PlayerBotMgr.h>

using namespace Trinity::ChatCommands;

class covenant_commandscript : public CommandScript
{
public:
    covenant_commandscript() : CommandScript("covenant_commandscript") { }

    static bool HandleCovenantCommand(ChatHandler* handler, const char* args)
    {
        if (WorldSession* session = handler->GetSession())
        {
            if (Player* player = session->GetPlayer())
            {
                if (player->IsGameMaster()) // This part only for gm's
                {
                    if (!*args)
                    {
                        handler->PSendSysMessage("Covenant commands allows to initiate covenant opcodes to check them. Usage example: .covenant venthyr");
                        handler->PSendSysMessage("Subcommands: kyrian, venthyr, nightfae, necrolord, callavre, renown");
                        return true;
                    }
                }
                else // This part for players
                {
                    if (!*args)
                    {
                        handler->PSendSysMessage("This command allows you to preview covenants. Usage example: .covenant venthyr");
                        handler->PSendSysMessage("Subcommands: kyrian, venthyr, nightfae, necrolord");
                        return true;
                    }
                }

                std::string param = (char*)args;

                // These params can be used by players and gm's too
                if (param == "kyrian")
                {
                    handler->PSendSysMessage("Preview Kyrian Covenant command initiated.");
                    player->GetSession()->SendCovenantPreview(player->GetGUID(), 1);
                }

                if (param == "venthyr")
                {
                    handler->PSendSysMessage("Preview Venthyr Covenant command initiated.");
                    player->GetSession()->SendCovenantPreview(player->GetGUID(), 2);
                }

                if (param == "nightfae")
                {
                    handler->PSendSysMessage("Preview Night Fae Covenant command initiated.");
                    player->GetSession()->SendCovenantPreview(player->GetGUID(), 3);
                }

                if (param == "necrolord")
                {
                    handler->PSendSysMessage("Preview Necrolord Covenant command initiated.");
                    player->GetSession()->SendCovenantPreview(player->GetGUID(), 4);
                }

                if (player->IsGameMaster()) // This part only for gm's
                {
                    if (param == "callavre")
                    {
                        handler->PSendSysMessage("Covenant Calling Availability Response command initiated.");
                        player->GetSession()->SendCovenantCallingAvailibilityResponse(true, 1, 208);
                    }

                    if (param == "renown")
                    {
                        handler->PSendSysMessage("Covenant Renown Npc UI command initiated.");

                    }

                    if (param == "choose")
                    {
                        handler->PSendSysMessage("Choose your covenant UI command initiated.");

                        player->SendPlayerChoice(player->GetGUID(), 644);

                    }
                }
            }

        }

        return true;
    }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable CovenantTestCommandTable =
        {
            { "covenant",          rbac::RBAC_PERM_COMMAND_GM,         true,   &HandleCovenantCommand,        "" },
        };

        return CovenantTestCommandTable;
    }
};

class transmog_commandscript : public CommandScript
{
public:
    transmog_commandscript() : CommandScript("transmog_commandscript") { }

    static bool HandleTransmogCommand(ChatHandler* handler, const char* args)
    {
        if (WorldSession* session = handler->GetSession())
        {
            if (Player* player = session->GetPlayer())
            {
                handler->PSendSysMessage("You initiated transmogrification command. We called Mystic Birdhat for your assistance.");

                // summon Mystic Birdhat <Transmogrifier> 64515
                if (Creature* birdhat = player->SummonCreature(64515, player->GetRandomNearPosition(10.0f), TEMPSUMMON_TIMED_DESPAWN, 1min))
                {
                    birdhat->Say("Hello! Someone said you want to transmogrify! Go on! I'm here for 1 minute only.", LANG_UNIVERSAL);
                    birdhat->SetEmoteState(Emote::EMOTE_STATE_DANCE);
                }
            }
        }

        return true;
    }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable TMOGCommandTable =
        {
            { "transmog",          rbac::RBAC_PERM_COMMAND_GM,          true,   &HandleTransmogCommand,        "" },
            { "tmog",          	   rbac::RBAC_PERM_COMMAND_GM,         	true,   &HandleTransmogCommand,        "" },
        };

        return TMOGCommandTable;
    }
};

class garrison_commandscript : public CommandScript
{
public:
    garrison_commandscript() : CommandScript("garrison_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable garrisonFollowerCommandTable =
        {
            { "add", rbac::RBAC_PERM_COMMAND_ACHIEVEMENT_ADD,       false, &HandleGarrisonFollowerAddCommand,   "" },
        };

        static ChatCommandTable garrisonMissionCommandTable =
        {
            { "add", rbac::RBAC_PERM_COMMAND_ACHIEVEMENT_ADD,       false, &HandleGarrisonMissionAddCommand,    "" },
            { "generate", rbac::RBAC_PERM_COMMAND_ACHIEVEMENT_ADD,       false, &HandleGarrisonGenerateMissionsCommand,    "" },
        };

        static ChatCommandTable garrisonCommandTable =
        {
            { "follower", rbac::RBAC_PERM_COMMAND_ACHIEVEMENT_ADD,      false, NULL, "", garrisonFollowerCommandTable },
            { "mission",  rbac::RBAC_PERM_COMMAND_ACHIEVEMENT_ADD,      false, NULL, "", garrisonMissionCommandTable },
            { "create", rbac::RBAC_PERM_COMMAND_ACHIEVEMENT_ADD,       false, &HandleGarrisonCreateCommand,    "" },
        };

        static ChatCommandTable commandTable =
        {
            { "garrison", rbac::RBAC_PERM_COMMAND_ACHIEVEMENT_ADD,      false, NULL, "", garrisonCommandTable },
        };

        return commandTable;
    }

    static bool HandleGarrisonFollowerAddCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 followerId = atoi((char*)args);
        target->AddGarrisonFollower(followerId);
        return true;
    }

    static bool HandleGarrisonMissionAddCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 missionId = atoi((char*)args);
        target->AddGarrisonMission(missionId);
        return true;
    }

    static bool HandleGarrisonCreateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 garrId = atoi((char*)args);
        target->CreateGarrison(garrId);
        return true;
    }

    static bool HandleGarrisonGenerateMissionsCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GarrisonType garType = target->GetCurrentGarrison();

        switch (target->GetMap()->GetEntry()->ExpansionID)
        {
        case EXPANSION_WARLORDS_OF_DRAENOR:     garType = GARRISON_TYPE_GARRISON;       break;
        case EXPANSION_LEGION:                  garType = GARRISON_TYPE_CLASS_HALL;     break;
        case EXPANSION_BATTLE_FOR_AZEROTH:      garType = GARRISON_TYPE_WAR_CAMPAIGN;   break;
        case EXPANSION_SHADOWLANDS:             garType = GARRISON_TYPE_COVENANT;       break;
        default:                                garType = GARRISON_TYPE_COVENANT;       break;
        }

        target->SetCurrentGarrison(garType);

        if (Garrison* garrison = target->GetGarrison((GarrisonType)garType))
            garrison->GenerateMissions();

        target->SendGarrisonInfo();
        target->SendGarrisonRemoteInfo();
        return true;
    }
};

class bot_commandscript : public CommandScript
{
public:
    bot_commandscript() : CommandScript("bot_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable MybotcommandTable =
        {
           { "login",       rbac::RBAC_PERM_COMMAND_GM,       false, &OnlineCmd,   "" },
           { "logout",	    rbac::RBAC_PERM_COMMAND_GM,  false, &OutlineCmd,        ""},
           { "max",			rbac::RBAC_PERM_COMMAND_GM,  false, &MaxCmd,            ""},
        };

        static ChatCommandTable commandTable =
        {
            { "bot",             rbac::RBAC_PERM_COMMAND_GM,  false, NULL,            "", MybotcommandTable }
        };
        return commandTable;
    }

    static bool MaxCmd(ChatHandler* handler, const char* args)
    {
        int max = atoi(args);
        if (max > 0)
            sPlayerBotMgr->SetMax(max);
        return true;
    }

    static bool OnlineCmd(ChatHandler* handler, const char* args)
    {
        sPlayerBotMgr->AllPlayerBotRandomLogin(args);
        return true;
    }

    static bool OutlineCmd(ChatHandler* handler, const char* args)
    {
        sPlayerBotMgr->AllPlayerBotLogout();
        return true;
    }
};

void AddSC_Custom_Commands()
{
    new covenant_commandscript();
    new transmog_commandscript();
    new garrison_commandscript();
    // new bot_commandscript();
}
