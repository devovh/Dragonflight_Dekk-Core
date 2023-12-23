/*
 * Copyright 2023 DekkCore
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

#include "GameObject.h"
#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"

enum wakingshorequests
{
   QUEST_EXPLORERS_IN_PERIL      = 70122,
   QUEST_EXPLORERS_IN_PERIL_H    = 65452,
   QUEST_THE_CALL_OF_THE_ISLES   = 70197,
   QUEST_LIBRARY_OF_ALEXSTRASZA  = 66000,
   QUEST_FROM_SUCH_GREAT_HEIGHTS = 66101,
   QUEST_THE_SPARK               = 66354,
   QUEST_A_LAST_HOPE             = 66001,
};

enum wakingshorespells
{
    SPELL_LIBRARY_QUEST_CREDIT_Q_1 = 373082, //create book
    SPELL_LIBRARY_QUEST_CREDIT_Q_2 = 373992, //create book
    SPELL_LIBRARY_QUEST_CREDIT_Q_3 = 373993,
    SPELL_LIBRARY_QUEST_CREDIT_Q_4 = 373994,
    SPELL_LIBRARY_QUEST_CREDIT_Q_5 = 373995,
    SPELL_LIBRARY_QUEST_CREDIT_Q_6 = 373996
};

class go_protodragon_rib_cage : public GameObjectScript
{
public:
    go_protodragon_rib_cage() : GameObjectScript("go_protodragon_rib_cage") {}

    struct go_protodragon_rib_cageAI : public GameObjectAI
    {
        go_protodragon_rib_cageAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_EXPLORERS_IN_PERIL))
            {
                player->KilledMonsterCredit(194139);
            }

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_protodragon_rib_cageAI(go);
    }
};

// Npc Teleporter 19999999
class npc_teleporter_19999999 : public CreatureScript
{
public:
    npc_teleporter_19999999() : CreatureScript("npc_teleporter_19999999") { }

    struct npc_teleporter_19999999AI : public ScriptedAI
    {
        npc_teleporter_19999999AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready to go to Stormwind!", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                if (player->HasQuest(QUEST_THE_CALL_OF_THE_ISLES))
                    player->ForceCompleteQuest(QUEST_THE_CALL_OF_THE_ISLES);
                player->TeleportTo(0, -8644.435f, 1323.734f, 5.234f, 1.733f);

                CloseGossipMenuFor(player);
                break;
            }

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_teleporter_19999999AI(creature);
    }
};

//376941 The Titanic Comedy
class go_titanic_comedy : public GameObjectScript
{
public:
    go_titanic_comedy() : GameObjectScript("go_titanic_comedy") {}

    struct go_titanic_comedyAI : public GameObjectAI
    {
        go_titanic_comedyAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_LIBRARY_OF_ALEXSTRASZA))
            {
                player->CastSpell(player, SPELL_LIBRARY_QUEST_CREDIT_Q_1);
            }

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_titanic_comedyAI(go);
    }
};

//376942 The Scales of Wrath
class go_scales_of_wrath : public GameObjectScript
{
public:
    go_scales_of_wrath() : GameObjectScript("go_scales_of_wrath") {}

    struct go_scales_of_wrathAI : public GameObjectAI
    {
        go_scales_of_wrathAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_LIBRARY_OF_ALEXSTRASZA))
            {
                player->CastSpell(player, SPELL_LIBRARY_QUEST_CREDIT_Q_2);
            }

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_scales_of_wrathAI(go);
    }
};

//376943 Dracuixote
class go_Dracuixote : public GameObjectScript
{
public:
    go_Dracuixote() : GameObjectScript("go_Dracuixote") {}

    struct go_DracuixoteAI : public GameObjectAI
    {
        go_DracuixoteAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_LIBRARY_OF_ALEXSTRASZA))
            {
                player->CastSpell(player, SPELL_LIBRARY_QUEST_CREDIT_Q_3);
                player->ForceCompleteQuest(QUEST_LIBRARY_OF_ALEXSTRASZA);
            }

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_DracuixoteAI(go);
    }
};

//376944 The Lord of the Wings
class go_lord_of_the_wings : public GameObjectScript
{
public:
    go_lord_of_the_wings() : GameObjectScript("go_lord_of_the_wings") {}

    struct go_lord_of_the_wingsAI : public GameObjectAI
    {
        go_lord_of_the_wingsAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_LIBRARY_OF_ALEXSTRASZA))
            {
                player->CastSpell(player, SPELL_LIBRARY_QUEST_CREDIT_Q_4);
            }

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_lord_of_the_wingsAI(go);
    }
};

//376945 The Red and the Black Flights
class go_red_and_black_flights : public GameObjectScript
{
public:
    go_red_and_black_flights() : GameObjectScript("go_red_and_black_flights") {}

    struct go_red_and_black_flightsAI : public GameObjectAI
    {
        explicit go_red_and_black_flightsAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_LIBRARY_OF_ALEXSTRASZA))
            {
                player->CastSpell(player, SPELL_LIBRARY_QUEST_CREDIT_Q_5);
            }

            return true;
        }

    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_red_and_black_flightsAI(go);
    }
};

//376946 Pride and Protodrakes
class go_pride_and_protodrakes : public GameObjectScript
{
public:
    go_pride_and_protodrakes() : GameObjectScript("go_pride_and_protodrakes") {}

    struct go_pride_and_protodrakesAI : public GameObjectAI
    {
        go_pride_and_protodrakesAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_LIBRARY_OF_ALEXSTRASZA))
            {
                player->CastSpell(player, SPELL_LIBRARY_QUEST_CREDIT_Q_6);
            }
            return true;
        }

    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_pride_and_protodrakesAI(go);
    }
};

//190171 Ruby Egg Guardian
class npc_ruby_egg_guardian : public CreatureScript
{
public:
    npc_ruby_egg_guardian() : CreatureScript("npc_ruby_egg_guardian") { }

    struct npc_ruby_egg_guardianAI : public ScriptedAI
    {
        npc_ruby_egg_guardianAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_LIBRARY_OF_ALEXSTRASZA))
                player->ForceCompleteQuest(QUEST_LIBRARY_OF_ALEXSTRASZA);

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ruby_egg_guardianAI(creature);
    }
};

// horde 379137
class go_protodragon_rib_cage_379137 : public GameObjectScript
{
public:
    go_protodragon_rib_cage_379137() : GameObjectScript("go_protodragon_rib_cage_379137") {}

    struct go_protodragon_rib_cage_379137AI : public GameObjectAI
    {
        go_protodragon_rib_cage_379137AI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_EXPLORERS_IN_PERIL_H))
            {
                player->KilledMonsterCredit(189838); //poopy
            }

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_protodragon_rib_cage_379137AI(go);
    }
};

//193707 surveyors disk
class npc_surveyors_disk : public CreatureScript
{
public:
    npc_surveyors_disk() : CreatureScript("npc_surveyors_disk") { }

    struct npc_surveyors_diskAI : public ScriptedAI
    {
        npc_surveyors_diskAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_FROM_SUCH_GREAT_HEIGHTS))
                player->ForceCompleteQuest(QUEST_FROM_SUCH_GREAT_HEIGHTS);

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_surveyors_diskAI(creature);
    }
};

//377013
class go_earthwardersforge_377013 : public GameObjectScript
{
public:
    go_earthwardersforge_377013() : GameObjectScript("go_earthwardersforge_377013") {}

    struct go_earthwardersforge_377013AI : public GameObjectAI
    {
        go_earthwardersforge_377013AI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            if (player->HasQuest(QUEST_THE_SPARK))
            {
                player->ForceCompleteQuest(QUEST_THE_SPARK);
            }

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_earthwardersforge_377013AI(go);
    }
};

//190155 caretaker azkra
class npc_caretaker_azkra190155 : public CreatureScript
{
public:
    npc_caretaker_azkra190155() : CreatureScript("npc_caretaker_azkra190155") { }

    struct npc_caretaker_azkra190155AI : public ScriptedAI
    {
        npc_caretaker_azkra190155AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_A_LAST_HOPE))
                player->ForceCompleteQuest(QUEST_A_LAST_HOPE);

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_caretaker_azkra190155AI(creature);
    }
};

void AddSC_zone_the_waking_shores()
{
    new go_protodragon_rib_cage();
    new npc_teleporter_19999999();
    new go_titanic_comedy();
    new go_scales_of_wrath();
    new go_Dracuixote();
    new go_lord_of_the_wings();
    new go_red_and_black_flights();
    new go_pride_and_protodrakes();
    new npc_ruby_egg_guardian();
    new go_protodragon_rib_cage_379137();
    new npc_surveyors_disk();
    new go_earthwardersforge_377013();
    new npc_caretaker_azkra190155();
}
