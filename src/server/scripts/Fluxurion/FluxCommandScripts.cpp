/*
 * Copyright 2023 Fluxurion
 */

#include "Flux.h"

class chromie_time_commandscript : public CommandScript
{
public:
    chromie_time_commandscript() : CommandScript("chromie_time_commandscript") { }

    Trinity::ChatCommands::ChatCommandTable GetCommands() const override
    {
        static Trinity::ChatCommands::ChatCommandTable chromieTimeSet_CommandTable =
        {
                        { "set",          HandleChromieTimeSetCommand,          rbac::RBAC_PERM_COMMAND_GM,          Trinity::ChatCommands::Console::Yes },
                        { "info",          HandleChromieTimeInfoCommand,          rbac::RBAC_PERM_COMMAND_GM,          Trinity::ChatCommands::Console::Yes },
        };

        static Trinity::ChatCommands::ChatCommandTable chromieTime_CommandTable =
        {
            { "chromietime",            chromieTimeSet_CommandTable },
        };

        return chromieTime_CommandTable;
    }

    static bool HandleChromieTimeInfoCommand(ChatHandler* handler, std::string const& args)
    {
        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint16 chromieTime = Fluxurion::GetChromieTime(target);
        std::string chromieTimeName = Fluxurion::GetChromieTimeName(target);

        handler->PSendSysMessage("Your selected chromie time is {} ({}).", chromieTime, chromieTimeName);

        return true;
    }

    static bool HandleChromieTimeSetCommand(ChatHandler* handler, uint8 args)
    {
        if (!args)
            return false;

        Player* target = handler->getSelectedPlayerOrSelf();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Fluxurion::SetChromieTime(target, args);

        return true;
    }

};

class gear_commandscript : public CommandScript
{
public:
    gear_commandscript() : CommandScript("gear_commandscript") { }

    static bool HandleGearCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        if (WorldSession* session = handler->GetSession())
        {
            if (Player* player = session->GetPlayer())
            {
                char const* cloadoutID = strtok((char*)args, " ");

                std::string param = (char*)args;

                if (param == "loadout")
                {
                    char const* cloadoutID = strtok(nullptr, " ");

                int32 loadoutID = 1;

                if (cloadoutID)
                    loadoutID = strtol(cloadoutID, nullptr, 10);

                if (loadoutID == 0)
                    loadoutID = 1;

                    std::vector<int32> bonusListIDs;
                char const* bonuses = strtok(nullptr, " ");

                char const* context = strtok(nullptr, " ");

                // semicolon separated bonuslist ids (parse them after all arguments are extracted by strtok!)
                if (bonuses)
                    for (std::string_view token : Trinity::Tokenize(bonuses, ';', false))
                        if (Optional<int32> bonusListId = Trinity::StringTo<int32>(token))
                            bonusListIDs.push_back(*bonusListId);

                handler->PSendSysMessage("Gearing up with loadout items..");
                Fluxurion::GearUpByLoadout(player, loadoutID, { bonusListIDs }); // no bonus
                }
            }

        }

        return true;
    }

    Trinity::ChatCommands::ChatCommandTable GetCommands() const override
    {
        static Trinity::ChatCommands::ChatCommandTable gear_CommandTable =
        {
             { "gear",          HandleGearCommand,          rbac::RBAC_PERM_COMMAND_GM,          Trinity::ChatCommands::Console::No },
        };

        return gear_CommandTable;
    }

};

class playspellvisual_commandscript : public CommandScript
{
public:
    playspellvisual_commandscript() : CommandScript("playspellvisual_commandscript") { }

    static bool HandlePlaySpellVisualCommand(ChatHandler* handler, char const* args)
    {
        if (WorldSession* session = handler->GetSession())
        {
            if (Player* player = session->GetPlayer())
            {
                if (!*args)
                {
                    handler->PSendSysMessage("You can initiate playspellvisual function with command. .psv #spellvisualid or .playspellvisual #spellvisualid");
                    return true;
                }

                std::string param = (char*)args;

                if (param != "")
                {
                    handler->PSendSysMessage("Playing spell visual: {}", param);
                    if (player->GetSelectedUnit())
                        player->SendPlaySpellVisual(player->GetSelectedUnit(), stoi(param), 0, 0, 60, false);
                    else
                        player->SendPlaySpellVisual(player, stoi(param), 0, 0, 60, false);
                }
            }

        }

        return true;
    }

    Trinity::ChatCommands::ChatCommandTable GetCommands() const override
    {
        static Trinity::ChatCommands::ChatCommandTable PSVCommandTable =
        {
             { "playspellvisual",          HandlePlaySpellVisualCommand,          rbac::RBAC_PERM_COMMAND_GM,          Trinity::ChatCommands::Console::No },
             { "psv",                      HandlePlaySpellVisualCommand,          rbac::RBAC_PERM_COMMAND_GM,          Trinity::ChatCommands::Console::No },
        };
        return PSVCommandTable;
    }

};

class getfollowangle_commandscript : public CommandScript
{
public:
    getfollowangle_commandscript() : CommandScript("getfollowangle_commandscript") { }

    static bool HandleGetFACommand(ChatHandler* handler)
    {
        Creature* target = handler->getSelectedCreature();

        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CreatureTemplate const* cInfo = target->GetCreatureTemplate();

        handler->PSendSysMessage("GetFollowAngle Command:");
        handler->PSendSysMessage("Target Entry: %u", target->GetEntry());
        handler->PSendSysMessage("Target Name: %s", target->GetName().c_str());
        handler->PSendSysMessage("Target Follow Angle: %f", target->GetFollowAngle());
        handler->PSendSysMessage("Target Follow Dist: %f", target->GetFollowDist());
        if (target->GetOwner())
        {
            handler->PSendSysMessage("Target Owner: %s", target->GetOwner()->GetName().c_str());
            handler->PSendSysMessage("Owner m_Controlled size: %u", target->GetOwner()->m_Controlled.size());
        }
        else
            handler->PSendSysMessage("Target has no owner.");
        if (target->HasUnitTypeMask(UNIT_MASK_MINION))
            handler->PSendSysMessage("Target is Minion.");
        if (target->HasUnitTypeMask(UNIT_MASK_GUARDIAN))
            handler->PSendSysMessage("Target is Guardian.");
        if (target->HasUnitTypeMask(UNIT_MASK_HUNTER_PET))
            handler->PSendSysMessage("Target is Hunter Pet.");

        return true;
    }

    Trinity::ChatCommands::ChatCommandTable GetCommands() const override
    {
        static Trinity::ChatCommands::ChatCommandTable getfa_CommandTable =
        {
             { "getfa",          HandleGetFACommand,          rbac::RBAC_PERM_COMMAND_GM,          Trinity::ChatCommands::Console::No },
        };

        return getfa_CommandTable;
    }

};

void AddSC_FluxCommands()
{
    new chromie_time_commandscript();
    new gear_commandscript();
    new playspellvisual_commandscript();
    new getfollowangle_commandscript();
}
