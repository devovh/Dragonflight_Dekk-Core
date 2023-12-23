/*
 * Copyright 2021
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
#include "GameObject.h"
#include "GameObjectAI.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "darkmaul_citadel.h"
#include "TemporarySummon.h"

//156501
struct boss_ravnyr : public BossAI
{
    boss_ravnyr(Creature* creature) : BossAI(creature, DATA_RAVNYR) { }

    void Reset() override
    {
        BossAI::Reset();
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC));
        me->SetUnitFlag2(UnitFlags2(UNIT_FLAG2_FEIGN_DEATH | UNIT_FLAG_UNINTERACTIBLE));
        me->SetPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 100);
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        Talk(0);
        events.ScheduleEvent(SPELL_NECROTIC_BREATH, 3s);
        events.ScheduleEvent(SPELL_WING_BUFFET, 8s);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
        case ACTION_AWAKEN:
            std::list<Player*> players;
            me->GetPlayerListInGrid(players, 250.f);
            Player* firstPlayer;

            for (Player* player : players)
            {
                firstPlayer = player;
                break;
            }

            if (firstPlayer)
            {
                firstPlayer->GetScheduler().Schedule(Milliseconds(3000), [this](TaskContext context)
                {
                    me->RemoveUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC));
                    me->RemoveUnitFlag2(UnitFlags2(UNIT_FLAG2_FEIGN_DEATH | UNIT_FLAG_UNINTERACTIBLE));
                    me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                });
            }
            break;
        }
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
        case SPELL_NECROTIC_BREATH:
            DoCastVictim(SPELL_NECROTIC_BREATH, false);
            events.Repeat(25s);
            break;

        case SPELL_WING_BUFFET:
            DoCastAOE(SPELL_WING_BUFFET, false);
            events.Repeat(20s);
            break;
        }
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        if (auto* tunkToor = me->FindNearestGameObject(GO_RAVNYR_EXIT, 100.0f))
            tunkToor->SetGoState(GO_STATE_ACTIVE);

        std::list<Player*> playerList;
        me->GetPlayerListInGrid(playerList, 100.0f);
        uint8 counter = 0;
        for (auto& players : playerList)
        {
            players->ForceCompleteQuest(55992);
            players->ForceCompleteQuest(59984);
            // // me->SetLootRecipient(players);
            
            if (counter == 0)
            {
                switch (players->GetTeam())
                {
                    case ALLIANCE:
                        if (Creature* captainGarrick = me->SummonCreature(161350, players->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0U))
                            captainGarrick->AI()->Talk(0);
                        break;
                    case HORDE:
                        if (Creature* breka = me->SummonCreature(167675, players->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN, 0s, 0U))
                            breka->AI()->Talk(0);
                        break;
                }
            }

            counter++;
        }
    }
};

void AddSC_boss_ravnyr()
{
    RegisterCreatureAI(boss_ravnyr);
}
