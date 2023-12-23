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

/*
Blasted_Lands
Quest support: 3628.
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "Player.h"
#include "Group.h"

enum DeathlyUsher
{
    SPELL_TELEPORT_SINGLE               = 12885,
    SPELL_TELEPORT_SINGLE_IN_GROUP      = 13142,
    SPELL_TELEPORT_GROUP                = 27686
};

enum TanaanZones
{
    MAP_TANAAN = 1265
};

enum TanaanMovies
{
    MOVIE_ENTER_PORTAL = 199
};

enum TanaanCreature
{
    THROUGH_THE_PORTAL_CREDIT = 78419
};

enum eMaps
{
    BLASTED_LANDS_DRAENOR_PHASE = 1190,
    EASTERN_KINGDOM_MAP_ID = 0,
    BLASTED_LANDS_ZONE_ID = 4,
    QUEST_WOD_THE_DARK_PORTAL = 34398
};

enum eSpell
{
    TimeTravelling = 176111
};

enum eGossips
{
    TraveledGossip = 88206, // Show me the Blasted Lands before the invasion.
    BaseGossip = 88207 // Show me the Blasted Lands after the invasion.
};

// Zidormi - 88206 - change the map,and give aura for the time travelling
class npc_blasted_lands_zidormi : public CreatureScript
{
public:
    npc_blasted_lands_zidormi() : CreatureScript("npc_blasted_lands_zidormi") { }

    struct npc_blasted_lands_zidormiAI : public ScriptedAI
    {
        npc_blasted_lands_zidormiAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->TalkedToCreature(me->GetEntry(), me->GetGUID());
            switch (player->GetMapId())
            {
            case eMaps::EASTERN_KINGDOM_MAP_ID:
                player->PrepareGossipMenu(me, BaseGossip, true);
                break;
            case eMaps::BLASTED_LANDS_DRAENOR_PHASE:
                player->PrepareGossipMenu(me, TraveledGossip, true);
                break;
            }
            player->SendPreparedGossip(me);
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (player->GetMapId())
            {
            case EASTERN_KINGDOM_MAP_ID:
                player->CastSpell(player, eSpell::TimeTravelling, true);
                // cast and remove just for the effect of time travel
                if (player->HasAura(eSpell::TimeTravelling))
                {
                    player->RemoveAura(eSpell::TimeTravelling);
                    player->PlayerTalkClass->SendCloseGossip();
                    // Enter To Draenor Blasted Lands
                    player->SeamlessTeleportToMap(BLASTED_LANDS_DRAENOR_PHASE);
                }
                break;
            case BLASTED_LANDS_DRAENOR_PHASE:
                player->PlayerTalkClass->SendCloseGossip();
                // Exit From Draenor Blasted Lands
                player->SeamlessTeleportToMap(EASTERN_KINGDOM_MAP_ID);
                player->CastSpell(player, eSpell::TimeTravelling, true);
                break;
            }

            return true;
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_blasted_lands_zidormiAI(p_Creature);
    }
};

struct npc_archmage_khadgar_gossip : public ScriptedAI
{
    npc_archmage_khadgar_gossip(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetTeamId() == TEAM_ALLIANCE)
        {
            if (player->GetQuestStatus(36881) == QUEST_STATUS_INCOMPLETE)
            {
                player->TeleportTo(MAP_TANAAN, 4066.7370f, -2381.9917f, 94.858f, 2.90f);
                player->SendMovieStart(MOVIE_ENTER_PORTAL);
                player->KilledMonsterCredit(THROUGH_THE_PORTAL_CREDIT);
            }
            else if (player->GetQuestStatus(34398) == QUEST_STATUS_INCOMPLETE)
            {
                player->TeleportTo(MAP_TANAAN, 4066.7370f, -2381.9917f, 94.858f, 2.90f);
                player->SendMovieStart(MOVIE_ENTER_PORTAL);
                player->KilledMonsterCredit(THROUGH_THE_PORTAL_CREDIT);
            }
        }
        else if (player->GetTeamId() == TEAM_HORDE)
        {
            if (player->GetQuestStatus(36881) == QUEST_STATUS_INCOMPLETE)
            {
                player->TeleportTo(MAP_TANAAN, 4066.7370f, -2381.9917f, 94.858f, 2.90f);
                player->SendMovieStart(MOVIE_ENTER_PORTAL);
                player->KilledMonsterCredit(THROUGH_THE_PORTAL_CREDIT);
            }
            else if (player->GetQuestStatus(34398) == QUEST_STATUS_INCOMPLETE)
            {
                player->TeleportTo(MAP_TANAAN, 4066.7370f, -2381.9917f, 94.858f, 2.90f);
                player->SendMovieStart(MOVIE_ENTER_PORTAL);
                player->KilledMonsterCredit(THROUGH_THE_PORTAL_CREDIT);
            }
        }
        return true;
    }
};

/// Dark Portal phasing
class PlayerScript_DarkPortal_Phasing : public PlayerScript
{
public:
    PlayerScript_DarkPortal_Phasing() : PlayerScript("PlayerScript_DarkPortal_Phasing") {}

    void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) override
    {
        if (player->GetMapId() == BLASTED_LANDS_DRAENOR_PHASE || player->GetMapId() == EASTERN_KINGDOM_MAP_ID)
        {
            // Enter To Draenor Blasted Lands
            if (newZone == BLASTED_LANDS_ZONE_ID && player->GetMapId() == EASTERN_KINGDOM_MAP_ID && !player->HasAura(eSpell::TimeTravelling))
                player->SeamlessTeleportToMap(BLASTED_LANDS_DRAENOR_PHASE);

            // Exit From Draenor Blasted Lands
            if (newZone != BLASTED_LANDS_ZONE_ID && player->GetMapId() == BLASTED_LANDS_DRAENOR_PHASE)
            {
                player->SeamlessTeleportToMap(EASTERN_KINGDOM_MAP_ID);

                if (player->HasAura(eSpell::TimeTravelling))
                    player->RemoveAura(eSpell::TimeTravelling);
            }
        }
    }
};

void AddSC_DekkCore_blasted_lands()
{
    new npc_blasted_lands_zidormi();
    RegisterCreatureAI(npc_archmage_khadgar_gossip);
    new PlayerScript_DarkPortal_Phasing();
}
