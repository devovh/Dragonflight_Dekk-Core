/*
 * Copyright 2023 Fluxurion
 */

#include "Creature.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Position.h"
#include "TemporarySummon.h"

struct LevelMaster : public ScriptedAI
{
    LevelMaster(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->CastSpell(me, 342317); // tachyon jump for the effective entering
    }

    bool OnGossipHello(Player* player) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GossipOptionNpc::None, "Give me 1 level!", GOSSIP_SENDER_MAIN, 0);
        AddGossipItemFor(player, GossipOptionNpc::None, "Give me 10 level!", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GossipOptionNpc::None, "Give me level until 50!", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GossipOptionNpc::None, "Give me level until 60!", GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GossipOptionNpc::None, "Thanks, you can go now!", GOSSIP_SENDER_MAIN, 4);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:
            if (player->GetLevel() < 60)
                player->GiveXP(player->GetXPForNextLevel(), player, 1);
            player->CastSpell(player, 325020, true); // ascended nova - healing+aoedmg
            CloseGossipMenuFor(player);
            break;
        case 1:
            if (player->GetLevel() <= 50)
            {
                me->Whisper("You are getting levels in every second for 10 second.", LANG_UNIVERSAL, player);

                me->CastSpell(player, 301178); // holy channeling on player
                player->GetScheduler().Schedule(Milliseconds(10000), [this](TaskContext context)
                    {
                        me->CastStop();
                    });

                for (int i = 0; i < 10; i++)
                {
                    player->GetScheduler().Schedule(Milliseconds(i * 1000), [this, player](TaskContext context)
                        {
                            player->GiveXP(player->GetXPForNextLevel(), player, 1);
                        });
                }
            }
            else me->Whisper("You can't get 10 level because you are over level 50.", LANG_UNIVERSAL, player);
            CloseGossipMenuFor(player);
            break;
        case 2:
            if (player->GetLevel() < 50)
            {
                me->Whisper("You are getting levels in every second until level 50.", LANG_UNIVERSAL, player);

                int lev_diff = 50 - (player->GetLevel());

                me->CastSpell(player, 301178); // holy channeling on player
                player->GetScheduler().Schedule(Milliseconds(lev_diff * 1000), [this](TaskContext context)
                    {
                        me->CastStop();
                    });

                for (int i = 0; i < lev_diff; i++)
                {
                    player->GetScheduler().Schedule(Milliseconds(i * 1000), [this, player](TaskContext context)
                        {
                            player->GiveXP(player->GetXPForNextLevel(), player, 1);
                        });
                }
            }
            else
            {
                if (player->GetLevel() == 50) me->Whisper("You are already level 50.", LANG_UNIVERSAL, player);
                if (player->GetLevel() > 50) me->Whisper("You have more level than 50.", LANG_UNIVERSAL, player);
            }
            CloseGossipMenuFor(player);
            break;
        case 3:
            if (player->GetLevel() < 60)
            {
                me->Whisper("You are getting levels in every second until level 60.", LANG_UNIVERSAL, player);

                int lev_diff = 60 - (player->GetLevel());

                me->CastSpell(player, 301178); // holy channeling on player
                player->GetScheduler().Schedule(Milliseconds(lev_diff * 1000), [this](TaskContext context)
                    {
                        me->CastStop();
                    });

                for (int i = 0; i < lev_diff; i++)
                {
                    player->GetScheduler().Schedule(Milliseconds(i * 1000), [this, player](TaskContext context)
                        {
                            player->GiveXP(player->GetXPForNextLevel(), player, 1);
                        });
                }
            }
            else
            {
                if (player->GetLevel() == 50) me->Whisper("You are already level 50.", LANG_UNIVERSAL, player);
                if (player->GetLevel() > 50) me->Whisper("You have more level than 50.", LANG_UNIVERSAL, player);
            }
            CloseGossipMenuFor(player);
            break;
        case 4:
            me->Whisper("It's always a pleasure to help You! Farewell!", LANG_UNIVERSAL, player);
            me->CastSpell(me, 357699); // channeling effect
            me->ForcedDespawn(3000);
            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }


};

class levelmaster_commandscript : public CommandScript
{
public:
    levelmaster_commandscript() : CommandScript("levelmaster_commandscript") { }

    static bool HandleLevelMasterCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (sConfigMgr->GetBoolDefault("LevelMaster.Enable", true))
        {
            handler->PSendSysMessage("You initiated level master command. The Level Master npc is being summoned for 5 minutes.");
            Player* player = handler->GetSession()->GetPlayer();
            Creature* npc = player->SummonCreature(8000001, player->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 5 * 60s);
        }

        return true;
    }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> CommandTable =
        {
            { "lupmaster",          rbac::RBAC_PERM_COMMAND_GM,         true,   &HandleLevelMasterCommand,        "" }
        };
        return CommandTable;
    }
};

void AddSC_LevelMaster()
{
    RegisterCreatureAI(LevelMaster);
    new levelmaster_commandscript();
}
