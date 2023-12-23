/*
 * Copyright 2021 DekkCore
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
#include "DBCEnums.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldSession.h"
#include "ScriptedGossip.h"
#include "MotionMaster.h"
#include "PassiveAI.h"
#include "ScriptedCreature.h"
#include "Unit.h"
#include "TemporarySummon.h"
#include "ZoneScript.h"
#include "GameObjectAI.h"
#include "PhasingHandler.h"


enum
{
    SPELL_APPRENTICE_RIDING = 33388,
    SPELL_ACHERUS_DEATH_CHARGER = 48778,
    SPELL_RUNEFORGING           = 53428,

    AURA_DOMINION_OVER_ACHERUS  = 54055, // this should be 51721 but it has a problem, so we will use 54055 instead

    AREA_EBON_HOLD_EAST_KINGD = 4281,
    MAP_EASTERN_KINGD         = 0,
    MAP_ALLIED_DK_ICECROWN    = 2297
};


class AlliedRaces : public PlayerScript
{
public:
	AlliedRaces() : PlayerScript("AlliedRaces") { }


    void OnLogin(Player* player, bool firstLogin) override
    {
		if (firstLogin)
		{
            /* Cinematic - Pandaren death knight
            //Store all future Allied races cinematics on first login here, not in CharacterHandler, it will create camera bugs and some other issues
			if (player->GetRace() == RACE_PANDAREN_HORDE || RACE_PANDAREN_ALLIANCE && player->GetMapId() == MAP_ALLIED_DK_ICECROWN && player->GetClass() == CLASS_DEATH_KNIGHT)
            {
                player->GetSceneMgr().PlaySceneByPackageId(2780, SceneFlag::None);
            }
            */

            /* Fix allied race deathknight missing spells */
            // the questline is wierd even on official blizz... so this is permanent fix. - Varjgard
            if ((player->GetClass() == CLASS_DEATH_KNIGHT) && (player->IsAlliedRace()))
            {
                /* Apprentice Riding */
                if (!player->HasSpell(SPELL_APPRENTICE_RIDING))
                    player->LearnSpell(SPELL_APPRENTICE_RIDING, false, 0 , true);

                /* Acherus Death Charger */
                if (!player->HasSpell(SPELL_ACHERUS_DEATH_CHARGER))
                {
                    player->LearnSpell(SPELL_ACHERUS_DEATH_CHARGER, false, 0, true);
                    player->AddActionButton(9, SPELL_ACHERUS_DEATH_CHARGER, ACTION_BUTTON_SPELL);
                }

                /* Runeforging */
                if (!player->HasSpell(SPELL_RUNEFORGING))
                {
                    player->LearnSpell(SPELL_RUNEFORGING, false, 0, true);
                    player->AddActionButton(7, SPELL_RUNEFORGING, ACTION_BUTTON_SPELL);
                }
            }
		}
    }

    void OnUpdateArea(Player* player, uint32 newArea, uint32 /*oldArea*/) override
    {
        /* Fix allied race deathknight missing aura */
        /* Dominion Over Acherus */
        if ((player->GetAreaId() == AREA_EBON_HOLD_EAST_KINGD) && (player->GetMapId() == MAP_EASTERN_KINGD) && (player->IsAlliedRace()))
        {
            if (!player->HasAura(AURA_DOMINION_OVER_ACHERUS))
                player->CastSpell(player, AURA_DOMINION_OVER_ACHERUS);
        }

        if ((newArea != AREA_EBON_HOLD_EAST_KINGD)  && (player->IsAlliedRace()))
        {
            if (player->HasAura(AURA_DOMINION_OVER_ACHERUS))
                player->RemoveAura(AURA_DOMINION_OVER_ACHERUS);
        }
    }
};

// alliedrace dk valkyr battle maiden - npc id 228534
class npc_valkyr_battle_maiden_allied : public CreatureScript
{
public:
    npc_valkyr_battle_maiden_allied() : CreatureScript("npc_valkyr_battle_maiden_allied") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_valkyr_battle_maiden_alliedAI(creature);
    }

    struct npc_valkyr_battle_maiden_alliedAI : public PassiveAI
    {
        npc_valkyr_battle_maiden_alliedAI(Creature* creature) : PassiveAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            FlyBackTimer = 500;
            phase = 0;
            x = 0.f;
            y = 0.f;
            z = 0.f;
        }

        uint32 FlyBackTimer;
        float x, y, z;
        uint32 phase;

        void Reset() override
        {
            me->setActive(true);
            me->SetVisible(false);
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            me->SetCanFly(true);

            me->GetPosition(x, y, z);
            z += 4.0f;
            x -= 3.5f;
            y -= 5.0f;
            me->GetMotionMaster()->Clear();
            me->UpdatePosition(x, y, z, 0.0f);
        }

        void UpdateAI(uint32 diff) override
        {
            if (FlyBackTimer <= diff)
            {
                Player* player = nullptr;
                if (me->IsSummon())
                    if (WorldObject* summoner = me->ToTempSummon()->GetSummoner())
                        player = summoner->ToPlayer();

                if (!player)
                    phase = 3;

                switch (phase)
                {
                    case 0:
                        me->SetWalk(false);
                        me->HandleEmoteCommand(EMOTE_STATE_FLYGRABCLOSED);
                        FlyBackTimer = 500;
                        break;
                    case 1:
                        player->GetClosePoint(x, y, z, me->GetCombatReach());
                        z += 2.5f;
                        x -= 2.0f;
                        y -= 1.5f;
                        me->GetMotionMaster()->MovePoint(0, x, y, z);
                        me->SetTarget(player->GetGUID());
                        me->SetVisible(true);
                        FlyBackTimer = 1;
                        break;
                    case 2:
                        DoCast(player, 51918, true); // this is only for visual effect!
                        me->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_01);
                        Talk(0, player);
                        player->TeleportTo(2297, 498.144653f, -2124.429932f, 840.856934f, 3.065104f);
                        player->ResurrectPlayer(100.0f, false);
                        FlyBackTimer = 3000;
                        break;
                    case 3:
                        me->SetVisible(false);
                        FlyBackTimer = 3000;
                        break;
                    case 4:
                        me->DisappearAndDie();
                        break;
                    default:
                        break;
                }
                ++phase;
            }
            else FlyBackTimer -= diff;
        }
    };
};

// allied dk zone - map id - 2297
class zone_allied_dk : public ZoneScript
{
public:
    zone_allied_dk() : ZoneScript("zone_allied_dk") { }

    void OnPlayerDeath(Player* player) override
    {
        if (player->GetMapId() == MAP_ALLIED_DK_ICECROWN)
            player->SummonCreature(228534, player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0U);
    }

    void OnPlayerExitZone(Player* player) override
    {
        if (player->GetPhaseShift().HasPhase(6000)) PhasingHandler::RemovePhase(player, 6000, true);
    }

};

// NPC 161709 Highlord Darion Mograine
struct npc_161709_darion_alliedracestart : public ScriptedAI
{
    npc_161709_darion_alliedracestart(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        //ClearGossipMenuFor(player);
        AddGossipItemFor(player, GossipOptionNpc::None, "Reporting for duty.", GOSSIP_SENDER_MAIN, 0);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        switch (action)
        {
        case 0:
            player->CastSpell(player, 316983, true);
            if (!player->GetPhaseShift().HasPhase(6000)) PhasingHandler::AddPhase(player, 6000, true);
            Conversation::CreateConversation(13254, player, player->GetPosition(), player->GetGUID(), nullptr);
            me->Talk(0, ChatMsg::CHAT_MSG_MONSTER_SAY, 30.f, player);
            CloseGossipMenuFor(player);
            break;
        }
        return true;
    }

};

// NPC 161711 Knight of the Ebon Blade - should salute to player
struct npc_161711_knight_of_the_ebonblade_alliedracestart : public ScriptedAI
{
    npc_161711_knight_of_the_ebonblade_alliedracestart(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        emotehappened = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if ((player->HasQuest(58903) || player->HasQuest(58902)) && !emotehappened && player->isInFront(me) && player->IsInDist(me, 10.f))
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                emotehappened = true;
                me->AddDelayedEvent(10000, [this]()        // emote cooldown
                    {
                        Reset();
                    });
            }
    }

private:
    bool emotehappened;

};

// npc_163016_LichKing_allieddkstart
struct npc_163016_LichKing_allieddkstart : public ScriptedAI
{
    npc_163016_LichKing_allieddkstart(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == 58877) // Death's Power Grows - Swear Fealty aura has area requirement 12951 - our dk start zone area is 12952 ?! why
            // Swear Fealty - trying with trigger
            player->AddAura(316884, player);
    }

};

//go_death_gate_341883
struct go_death_gate_341883 : public GameObjectAI
{
    go_death_gate_341883(GameObject* go) : GameObjectAI(go) { }

    // Called when a Player clicks a GameObject, before GossipHello
    // prevents achievement tracking if returning true
    bool OnReportUse(Player* player)
    {
        if (!player->HasQuest(58903)) // if player don't have the quest Defender of Azeroth do nothing
            return false;

        return true;
    }
};



void AddSC_AlliedRaces()
{
    RegisterPlayerScript(AlliedRaces);
    new npc_valkyr_battle_maiden_allied();
    new zone_allied_dk();
    RegisterCreatureAI(npc_161709_darion_alliedracestart);
    RegisterCreatureAI(npc_161711_knight_of_the_ebonblade_alliedracestart);
    RegisterCreatureAI(npc_163016_LichKing_allieddkstart);
    RegisterGameObjectAI(go_death_gate_341883);
}
