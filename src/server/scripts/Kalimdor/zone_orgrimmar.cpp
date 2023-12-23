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

#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Conversation.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "PhasingHandler.h"
#include "TemporarySummon.h"
#include "Transport.h"
#include "Vehicle.h"
#include "Log.h"
#include "Chat.h"
#include "ZoneScript.h"
#include "Weather.h"
#include "Spell.h"

enum ExileReach
{
    QUEST_H_AN_END_TO_BEGINNINGS = 59985,

    QUEST_THE_HORDE_WAY = 31013,
};


// npc_Warlord_Breka_Grimaxe_168431_q59985
struct npc_Warlord_Breka_Grimaxe_168431 : public ScriptedAI
{
    npc_Warlord_Breka_Grimaxe_168431(Creature* creature) : ScriptedAI(creature) { Reset(); }

private:
    bool say;

    void Reset()
    {
        say = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->IsInDist(me, 20.0f))
                if (player->GetQuestStatus(QUEST_H_AN_END_TO_BEGINNINGS) == QUEST_STATUS_COMPLETE)
                {
                    if (!say)
                    {
                        player->AddDelayedEvent(2000, [player]() -> void
                            {
                                player->CastSpell(player, 344826, false);
                            });
                        say = true;
                    }
                }
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_H_AN_END_TO_BEGINNINGS)
            say = false;
    }
};

struct npc_garrosh_Hellscream_39605 : public ScriptedAI
{
    npc_garrosh_Hellscream_39605(Creature* creature) : ScriptedAI(creature) {}

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_THE_HORDE_WAY)
        {
            player->SummonCreature(62087, Position(1667.44f, -4356.1f, 26.3521f, 2.7836f), TEMPSUMMON_TIMED_DESPAWN, 600s);
            me->DespawnOrUnsummon(1s, 125s);
        }
    }
};

// by spell 120753_trigger_121139
class npc_garosh_hord_way_62087 : public CreatureScript
{
public:
    npc_garosh_hord_way_62087() : CreatureScript("npc_garosh_hord_way_62087") { }

    struct npc_garosh_hord_way_62087AI : public EscortAI
    {
        npc_garosh_hord_way_62087AI(Creature* creature) : EscortAI(creature) {}

        EventMap events;
        ObjectGuid playerGuid;
        ObjectGuid CziGUID;

        enum dataType
        {
            NPC_CZI = 60570,
            NPC_CREDIT = 62089,

            EVENT_0 = 1,
            EVENT_1 = 2,
            EVENT_2 = 3,
            EVENT_3 = 4,
            EVENT_4 = 5,
            EVENT_5 = 6,
            EVENT_6 = 7,
            EVENT_7 = 8,
            EVENT_8 = 9,
            EVENT_9,
            EVENT_10,
            EVENT_11,
            EVENT_12,
            EVENT_13,
            EVENT_14,
            EVENT_15,
            EVENT_16,
            EVENT_17,
            EVENT_18,
            EVENT_19,
            EVENT_20,

            EVENT_CZI_0_1,
            EVENT_CZI_0,
            EVENT_CZI_1,

        };

        void Reset()
        {
            me->SetWalk(true);
            playerGuid.Clear();
            CziGUID.Clear();
        }

        void IsSummonedBy(WorldObject* summoner) override
        {
            Player* player = summoner->ToPlayer();
            if (!player)
                return;

            playerGuid = summoner->GetGUID();

            uint32 t = 0;                                        //
            events.ScheduleEvent(EVENT_0, 2s);            //18:12:24.s start + talk
            events.ScheduleEvent(EVENT_1, 5s);            //18:12:29.s
            events.ScheduleEvent(EVENT_2, 8s);            //18:12:37.s
            events.ScheduleEvent(EVENT_3, 6s);            //18:12:43.s
            events.ScheduleEvent(EVENT_4, 5s);            //18:12:48.s
            events.ScheduleEvent(EVENT_5, 5s);            //18:12:54.s
            events.ScheduleEvent(EVENT_6, 1s);            //18:12:55.s
            events.ScheduleEvent(EVENT_7, 2s);            //18:12:57.s
            events.ScheduleEvent(EVENT_8, 9s);            //18:13:06.s
            events.ScheduleEvent(EVENT_CZI_0, 3s);        //18:13:09.s Message: ??, ?????.
            events.ScheduleEvent(EVENT_9, 3s);            //18:13:12.s
            events.ScheduleEvent(EVENT_10, 3s);           //18:13:15.s
            events.ScheduleEvent(EVENT_11, 8s);           //18:13:23.s
            events.ScheduleEvent(EVENT_12, 6s);           //18:13:29.s
            events.ScheduleEvent(EVENT_13, 5s);           //18:13:34.s
            events.ScheduleEvent(EVENT_14, 13s);          //18:13:47.s
            events.ScheduleEvent(EVENT_15, 5s);           //18:13:52.s 
            events.ScheduleEvent(EVENT_16, 11s);          //18:14:03.s
            events.ScheduleEvent(EVENT_CZI_1, 3s);        //18:14:06.s Message: ??... ??, ???????...
            events.ScheduleEvent(EVENT_17, 4s);           //18:14:10.s
            events.ScheduleEvent(EVENT_18, 1s);           //18:14:11.s
            events.ScheduleEvent(EVENT_19, 2s);           //18:14:13.s
            //events.ScheduleEvent(EVENT_20, 20s);           //18:14:33.s
        }

        void WaypointReached(uint32 pointId, uint32 pathId) override
        {
            switch (pointId)
            {
            case 3:
            case 6:
            case 7:
            case 12:
            case 13:
            case 16:
                if (Player* player = ObjectAccessor::GetPlayer(*me, playerGuid))
                    me->SetFacingToObject(player);
                SetEscortPaused(true);
                break;
            case 18:
                events.ScheduleEvent(EVENT_20, 1s);           //18:14:33.000
                SetEscortPaused(true);
                break;
            }
        }

        void initSummon()
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, playerGuid))
                if (Creature* czi = me->FindNearestCreature(NPC_CZI, 200.0f, true))
                {
                    czi->GetMotionMaster()->MoveFollow(player, 2.0f, M_PI / 4);
                    CziGUID = czi->GetGUID();
                }
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            EscortAI::UpdateAI(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_0:
                    LoadPath(me->GetEntry());
                    Start(false);
                    Talk(0);
                    initSummon();
                    break;
                case EVENT_1:
                    Talk(1);
                    break;
                case EVENT_2:
                    Talk(2);
                    break;
                case EVENT_3:
                    SetEscortPaused(false);
                    Talk(3);
                    break;
                case EVENT_4:
                    Talk(4);
                    break;
                case EVENT_5:
                    Talk(5);
                    break;
                case EVENT_6:
                    SetEscortPaused(false);
                    break;
                case EVENT_7:
                    Talk(6);
                    break;
                case EVENT_8:
                    Talk(7);
                    if (Creature* zu = me->GetMap()->GetCreature(CziGUID))
                        me->SetFacingToObject(zu);
                    break;
                case EVENT_9:
                    Talk(8);
                    break;
                case EVENT_10:
                    Talk(9);
                    SetEscortPaused(false);
                    break;
                case EVENT_11:
                    Talk(10);
                    break;
                case EVENT_12:
                    Talk(11);
                    break;
                case EVENT_13:
                    Talk(12);
                    SetEscortPaused(false);
                    break;
                case EVENT_14:
                    Talk(13);
                    SetEscortPaused(false);
                    break;
                case EVENT_15:
                    me->HandleEmoteCommand(static_cast <Emote>(397));
                    break;
                case EVENT_16:
                    Talk(14);
                    if (Creature* zu = me->GetMap()->GetCreature(CziGUID))
                        me->SetFacingToObject(zu);
                    break;
                case EVENT_17:
                    break;
                case EVENT_18:
                    Talk(15);
                    break;
                case EVENT_19:
                    SetEscortPaused(false);
                    if (Player* player = ObjectAccessor::GetPlayer(*me, playerGuid))
                        player->KilledMonsterCredit(NPC_CREDIT, ObjectGuid::Empty);
                    break;
                case EVENT_20:
                    me->Respawn(true);
                    me->DespawnOrUnsummon(1s);
                    if (Creature* zu = me->GetMap()->GetCreature(CziGUID))
                        zu->DespawnOrUnsummon(10s);
                    break;
                case EVENT_CZI_0:
                    if (Creature* zu = me->GetMap()->GetCreature(CziGUID))
                        zu->AI()->Talk(3);
                    break;
                case EVENT_CZI_1:
                    if (Creature* zu = me->GetMap()->GetCreature(CziGUID))
                        zu->AI()->Talk(4);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_garosh_hord_way_62087AI(creature);
    }

};

void AddSC_orgrimmar()
{
    RegisterCreatureAI(npc_Warlord_Breka_Grimaxe_168431);
    RegisterCreatureAI(npc_garrosh_Hellscream_39605);
    new npc_garosh_hord_way_62087();
}
