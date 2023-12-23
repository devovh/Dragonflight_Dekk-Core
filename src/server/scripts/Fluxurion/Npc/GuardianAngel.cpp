/*
 * Copyright 2023 Fluxurion
 */

#include "Creature.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "GameTime.h"
#include "PhasingHandler.h"
#include "ScriptedCreature.h"
#include <sstream>
#include "TemporarySummon.h"

class GuardianAngel_Spawn : public PlayerScript
{
public:
    GuardianAngel_Spawn() : PlayerScript("GuardianAngel_Spawn") { }

    void SummonGuardianAngel(Player* player)
    {
        // if the guardian angle enabled in config
        if (sConfigMgr->GetBoolDefault("GuardianAngel.Enable", true))
        {
            // we get the mapids
            std::string allowedmapids = sConfigMgr->GetStringDefault("GuardianAngel.AllowedMapIDs", "");
            std::stringstream allowedmapidsstream(allowedmapids);
            std::string temp;
            std::unordered_set<int> allowedmapids_;

            bool guardianangelwatchinghere = false;

            // we check the mapids
            while (std::getline(allowedmapidsstream, temp, ','))
                allowedmapids_.insert(atol(temp.c_str()));
            for (auto allowedmap = allowedmapids_.begin(); allowedmap != allowedmapids_.end(); ++allowedmap)
            {
                uint32 number = *(allowedmap);
                if (player->GetMapId() == number) guardianangelwatchinghere = true;
                //debug to check mapids in config...yes i tested it and successfully gets multiple ids!!! yeey
                //ChatHandler(player->GetSession()).PSendSysMessage("Actual mapids from config: %s",(std::to_string(number)));
            }

            // if player is on the specified maps the guardian angel watches lol
            if (guardianangelwatchinghere)
            {
                // summon guardian angel for a while
                uint32 minutesforassist = sConfigMgr->GetIntDefault("GuardianAngel.Minutes.For.Assist", 1);
                Creature* npc = player->SummonCreature(8000000, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, minutesforassist * 60s);
            }

        }
    }


    // if the player getting killed by a mob
    void OnPlayerKilledByCreature(Creature* killer, Player* player)
    {
        SummonGuardianAngel(player);
    }

    // if the player getting killed by a player
    void OnPVPKill(Player* killer, Player* player)
    {
        SummonGuardianAngel(player);
    }


    // if the player getting low hp NOTWORKING??%?%
    void OnPlayerTakeDamage(Player* player, uint32 damage, SpellSchoolMask schoolMask)
    {
        if (player->GetHealthPct() <= 30)
        {
            SummonGuardianAngel(player);
        }
    }

    //working but u need multiple mobs
    void OnCreatureKill(Player* player /*killer*/, Creature* /*killed*/)
    {
        if (player->GetHealthPct() <= 30)
        {
            SummonGuardianAngel(player);
        }
    }

};



struct GuardianAngel : public ScriptedAI
{
    GuardianAngel(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->SetHoverHeight(2.0f);

        if (Player* player = summoner->ToPlayer())
        {
            // npc whisper player
            me->Whisper("Hold on! I'll help you!", LANG_UNIVERSAL, player);
            me->CastSpell(me, 321425); // electro jump

            if (!player->IsAlive())  // if player is dead angel will ress and heal
            {
                me->AddDelayedEvent(4000, [this, player]() -> void
                    {
                        player->ResurrectPlayer(0.3f);
                        me->CastSpell(player, 345201); // guardian angel - healing for 60% and does aoe dmg
                    });

                me->AddDelayedEvent(8000, [this, player]() -> void
                    {
                        player->CastSpell(player, 345323, true); // 100% crit for 30sec
                        me->CastSpell(player, 287419, true); // angelic reneval - healing full hp

                        // add generic phase if player is on exile reach
                        if (player->GetMapId() == 2175)
                            if (!player->GetPhaseShift().HasPhase(12940))
                                PhasingHandler::AddPhase(player, 12940, true);
                    });

                me->AddDelayedEvent(12000, [this, player]() -> void
                    {
                        me->CastSpell(player, 325326, true); // ascended eruption - healing+aoedmg
                    });
                me->AddDelayedEvent(13000, [this, player]() -> void
                    {
                        me->CastSpell(player, 325326, true); // ascended eruption - healing+aoedmg
                    });
                me->AddDelayedEvent(14000, [this, player]() -> void
                    {
                        me->CastSpell(player, 325326, true); // ascended eruption - healing+aoedmg
                    });
            }
            else  // if player is alive angel will heal
            {
                me->CastSpell(player, 345201); // guardian angel - healing for 60% and does aoe dmg

                me->AddDelayedEvent(3000, [this, player]() -> void
                    {
                        me->CastSpell(player, 345201, true); // guardian angel - healing for 60% and does aoe dmg
                    });
                me->AddDelayedEvent(6000, [this, player]() -> void
                    {
                        me->CastSpell(player, 345201, true); // guardian angel - healing for 60% and does aoe dmg
                    });
                me->AddDelayedEvent(9000, [this, player]() -> void
                    {
                        me->CastSpell(player, 345201, true); // guardian angel - healing for 60% and does aoe dmg
                    });

                me->AddDelayedEvent(10000, [this, player]() -> void
                    {
                        me->ForcedDespawn();
                    });
            }

        }
    }
    /*

        void OwnerAttacked(Unit* target)
        {
            if (Player* player = me->GetOwner()->ToPlayer())
            {
                // if the player can't hold
                if (player->GetHealthPct() <= 30)
                {
                    me->CastSpell(me, 325326, true);
                    player->CastSpell(player, 287419, true);
                }

                me->CastSpell(player, 347223); // fanatical barrier

                // do some attacking lol
                events.ScheduleEvent(339450, 3s); // anima burst
                events.ScheduleEvent(158431, 5s); // blinding flash

            }
        }

        void ExecuteEvent(uint32 eventId)
        {
            switch (eventId)
            {
            case 339450:
                me->CastSpell(me->SelectNearestHostileUnitInAggroRange(), 339450);
                events.Repeat(3s, 4s);
                break;

            case 158431:
                me->CastSpell(me, 158431);
                events.Repeat(5s, 10s);
                break;
            }
        }

    */

    bool OnGossipHello(Player* player) override
    {
        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GossipOptionNpc::None, "Heal pls!", GOSSIP_SENDER_MAIN, 0);
        AddGossipItemFor(player, GossipOptionNpc::None, "Did you bring some food?", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GossipOptionNpc::None, "Can you repair my armor?", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GossipOptionNpc::None, "Follow me!", GOSSIP_SENDER_MAIN, 3);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        switch (action)
        {
        case 0:

            me->CastSpell(me, 325326, true); // just for the effect
            player->CastSpell(player, 287419, true); // angelic reneval - healing full hp

            CloseGossipMenuFor(player);
            break;
        case 1:
            player->CastSpell(player, 304971, true);
            player->AddItem(80610, 5);

            CloseGossipMenuFor(player);
            break;
        case 2:
            me->CastSpell(me, 324427);
            player->DurabilityRepairAll(false, 0, false);
            CloseGossipMenuFor(player);
            break;
        case 3:

            me->CastSpell(player, 342317);
            me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle());
            me->SetOwnerGUID(player->GetGUID());
            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }


};

void AddSC_GuardianAngel()
{
    new GuardianAngel_Spawn;
    RegisterCreatureAI(GuardianAngel);
}
