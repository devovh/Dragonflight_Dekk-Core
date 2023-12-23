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
#include "CombatAI.h"
#include "MotionMaster.h"
#include "MoveSplineInit.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

enum zone_coldridge_valley
{
    NPC_TROLLING_FOR_INFORMATION_KILL_CREDIT_BUNNY_SE = 37109,
    NPC_TROLLING_FOR_INFORMATION_KILL_CREDIT_BUNNY_SW = 37110,
    NPC_TROLLING_FOR_INFORMATION_KILL_CREDIT_BUNNY_W  = 37111,


    QUEST_TROLLING_FOR_INFORMATION = 24489,
};

// 37108 npc_soothsayer_shikala
class npc_soothsayer_shikala : public CreatureScript
{
public:
    npc_soothsayer_shikala() : CreatureScript("npc_soothsayer_shikala") { }

    struct npc_soothsayer_shikalaAI : public ScriptedAI
    {
        npc_soothsayer_shikalaAI(Creature* c) : ScriptedAI(c) { }

        uint32 timer;
        uint32 phase;
        bool TalkSequenceIsStarted;
        Player* player;

        void Reset()
        {
            timer = 0; TalkSequenceIsStarted = false; phase = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || !who->IsAlive() || TalkSequenceIsStarted)
                return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
            {
                player = who->ToPlayer();
                if (player)
                {
                    if (player->GetQuestStatus(QUEST_TROLLING_FOR_INFORMATION) == QUEST_STATUS_INCOMPLETE)
                    {
                        TalkSequenceIsStarted = true;
                        timer = 2000; phase++;
                    }
                }
            }
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (TalkSequenceIsStarted)
                {
                    if (!player)
                    {
                        timer = 0; phase = 0; TalkSequenceIsStarted = false;
                        return;
                    }
                    if (timer <= diff)
                    {
                        switch (phase)
                        {
                        case 1:
                        {
                            Talk(0); timer = 5000; phase++;
                            break;
                        }
                        case 2:
                        {
                            Talk(1); timer = 5000; phase++;
                            break;
                        }
                        case 3:
                        {
                            Talk(2); timer = 5000; phase++;
                            break;
                        }
                        case 4:
                        {
                            player->KilledMonsterCredit(NPC_TROLLING_FOR_INFORMATION_KILL_CREDIT_BUNNY_SE);
                            timer = 120000; phase++; // cooldown
                            break;
                        }
                        case 5:
                        {
                            timer = 0; phase = 0; TalkSequenceIsStarted = false;
                            break;
                        }
                        }
                    }
                    else
                        timer -= diff;

                    if (!me->IsInRange(player, 0.0f, 15.0F))
                    {
                        timer = 0; phase = 0; TalkSequenceIsStarted = false; // if player gone away, delete cooldown
                    }
                }

            }
            else
                DoMeleeAttackIfReady();

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soothsayer_shikalaAI(creature);
    }
};

// 37173 npc_soothsayer_rikkari
class npc_soothsayer_rikkari : public CreatureScript
{
public:
    npc_soothsayer_rikkari() : CreatureScript("npc_soothsayer_rikkari") { }

    struct npc_soothsayer_rikkariAI : public ScriptedAI
    {
        npc_soothsayer_rikkariAI(Creature* c) : ScriptedAI(c) { }

        uint32 timer;
        uint32 phase;
        bool TalkSequenceIsStarted;
        Player* player;

        void Reset()
        {
            timer = 0; TalkSequenceIsStarted = false; phase = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || !who->IsAlive() || TalkSequenceIsStarted)
                return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
            {
                player = who->ToPlayer();
                if (player)
                {
                    if (player->GetQuestStatus(QUEST_TROLLING_FOR_INFORMATION) == QUEST_STATUS_INCOMPLETE)
                    {
                        TalkSequenceIsStarted = true;
                        timer = 2000; phase++;
                    }
                }
            }
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (TalkSequenceIsStarted)
                {
                    if (!player)
                    {
                        timer = 0; phase = 0; TalkSequenceIsStarted = false;
                        return;
                    }
                    if (timer <= diff)
                    {
                        switch (phase)
                        {
                        case 1:
                        {
                            Talk(0); timer = 5000; phase++;
                            break;
                        }
                        case 2:
                        {
                            Talk(1); timer = 5000; phase++;
                            break;
                        }
                        case 3:
                        {
                            Talk(2); timer = 5000; phase++;
                            break;
                        }
                        case 4:
                        {
                            player->KilledMonsterCredit(NPC_TROLLING_FOR_INFORMATION_KILL_CREDIT_BUNNY_SW);
                            timer = 120000; phase++; // cooldown                                                                                                          
                            break;
                        }
                        case 5:
                        {
                            timer = 0; phase = 0; TalkSequenceIsStarted = false;
                            break;
                        }
                        }
                    }
                    else
                        timer -= diff;

                    if (!me->IsInRange(player, 0.0f, 15.0F))
                    {
                        timer = 0; phase = 0; TalkSequenceIsStarted = false; // if player gone away, delete cooldown
                    }
                }

            }
            else
                DoMeleeAttackIfReady();

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soothsayer_rikkariAI(creature);
    }
};

// 37174 npc_soothsayer_mirimkoa
class npc_soothsayer_mirimkoa : public CreatureScript
{
public:
    npc_soothsayer_mirimkoa() : CreatureScript("npc_soothsayer_mirimkoa") { }

    struct npc_soothsayer_mirimkoaAI : public ScriptedAI
    {
        npc_soothsayer_mirimkoaAI(Creature* c) : ScriptedAI(c) { }

        uint32 timer;
        uint32 phase;
        bool TalkSequenceIsStarted;
        Player* player;

        void Reset()
        {
            timer = 0; TalkSequenceIsStarted = false; phase = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || !who->IsAlive() || TalkSequenceIsStarted)
                return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 10.0f))
            {
                player = who->ToPlayer();
                if (player)
                {
                    if (player->GetQuestStatus(QUEST_TROLLING_FOR_INFORMATION) == QUEST_STATUS_INCOMPLETE)
                    {
                        TalkSequenceIsStarted = true;
                        timer = 2000; phase++;
                    }
                }
            }
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (TalkSequenceIsStarted)
                {
                    if (!player)
                    {
                        timer = 0; phase = 0; TalkSequenceIsStarted = false;
                        return;
                    }
                    if (timer <= diff)
                    {
                        switch (phase)
                        {
                        case 1:
                        {
                            Talk(0); timer = 5000; phase++;
                            break;
                        }
                        case 2:
                        {
                            Talk(1); timer = 5000; phase++;
                            break;
                        }
                        case 3:
                        {
                            Talk(2); timer = 5000; phase++;
                            break;
                        }
                        case 4:
                        {
                            player->KilledMonsterCredit(NPC_TROLLING_FOR_INFORMATION_KILL_CREDIT_BUNNY_W);
                            timer = 120000; phase++; // cooldown                                                                                      
                            break;
                        }
                        case 5:
                        {
                            timer = 0; phase = 0; TalkSequenceIsStarted = false;
                            break;
                        }
                        }
                    }
                    else
                        timer -= diff;

                    if (!me->IsInRange(player, 0.0f, 15.0F))
                    {
                        timer = 0; phase = 0; TalkSequenceIsStarted = false; // if player gone away, delete cooldown
                    }
                }

            }
            else
                DoMeleeAttackIfReady();

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soothsayer_mirimkoaAI(creature);
    }
};

void AddSC_DekkCore_dun_morogh_area_coldridge_valley()
{
    new npc_soothsayer_shikala();
    new npc_soothsayer_rikkari();
    new npc_soothsayer_mirimkoa();
}
