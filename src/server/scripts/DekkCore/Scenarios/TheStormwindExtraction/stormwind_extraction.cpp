/*
 * Copyright (C) 2017-2019 AshamaneProject <https://github.com/AshamaneProject>
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

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "CombatAI.h"
#include "GameObjectAI.h"
#include "Scenario.h"
#include "ScriptMgr.h"
#include "stormwind_extraction.h"

// 281475 - Sewer access portal
struct go_se_sewer_access_portal : public GameObjectAI
{
    go_se_sewer_access_portal(GameObject* go) : GameObjectAI(go) { }
    /*
    void Reset() override
    {
        me->GetScheduler().CancelAll();
        me->GetScheduler().Schedule(1s, [this](TaskContext context)
        {
            if (Player* player = me->SelectNearestPlayer(1.f))
                if (Scenario* scenario = player->GetScenario())
                    if (scenario->CheckCompletedCriteriaTree(CRITERIA_TREE_OPEN_SEWERS, player))
                    {
                        player->CastSpell(player, SPELL_TELEPORT_STOCKADE, true);
                        scenario->SendScenarioEvent(player, SCENARIO_EVENT_ENTER_STOCKADE);

                        if (InstanceScript* instanceScript = me->GetInstanceScript())
                            instanceScript->SetData(SCENARIO_EVENT_ENTER_STOCKADE, 1);
                        return;
                    }

            context.Repeat();
        });
    }
    */
    bool OnGossipHello(Player* player) override
    {
        if (Scenario* scenario = player->GetScenario())
        {
           /* if (scenario->CheckCompletedCriteriaTree(CRITERIA_TREE_OPEN_SEWERS, player))
            {             
                scenario->SendScenarioEvent(player, SCENARIO_EVENT_ENTER_STOCKADE);
                player->CompletedCriteriaTreeId(CRITERIA_TREE_ENTER_THE_STOCKADES);

                return true;
            }*/
        }
        return false;
    }
};

// 134037
struct npc_se_thalyssra : public CombatAI
{
    npc_se_thalyssra(Creature * creature) : CombatAI(creature) { }

    void DoAction(int32 actionId) override
    {
        m_currentCombatMovement = actionId;

      /*  if (actionId == 1)
            MoveCombat(Position(-8690.216797f, 902.533203f, 53.732910f));
        else if (actionId == 2)
            MoveCombat(Position(-8741.128906f, 885.286560f, 52.815895f));
        else if (actionId == 3)
        {
            me->NearTeleportTo(Position(-8697.854492f, 899.180908f, 53.731392f));
            MoveCombat(Position(-8645.526367f, 773.394714f, 45.399426f));
        }*/
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        CombatAI::MovementInform(type, id);

      /*  if (type == POINT_MOTION_TYPE && id == POINT_ID_COMBAT_MOVEMENT)
        {
            if (m_currentCombatMovement == 1)
                instance->SetData(SCENARIO_EVENT_FIND_ROKHAN, 1);
            else if (m_currentCombatMovement == 2)
                instance->SetData(EVENT_FIND_PRISONNERS, 1);
            else if (m_currentCombatMovement == 3)
                instance->SetData(EVENT_END_OF_PRISON_REACHED, 1);
        }*/
    }

private:
    int32 m_currentCombatMovement = 0;
};

// 134120
struct npc_se_saurfang : public ScriptedAI
{
    npc_se_saurfang(Creature * creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        me->RemoveAurasDueToSpell(SPELL_CHAT_BUBBLE);
        player->PlayConversation(CONVERSATION_SAURFANG);

        player->GetScheduler().Schedule(42s, [](TaskContext context)
        {
            if (InstanceScript* instanceScript = GetContextPlayer()->GetInstanceScript())
                instanceScript->DoSendScenarioEvent(SCENARIO_EVENT_FREE_SAURFANG);

        }).Schedule(47s, [](TaskContext context)
        {
         //   GetContextPlayer()->CompletedCriteriaTreeId(CRITERIA_TREE_RELEASE_SAURFANG);
        });

        return false;
    }
};

//287550
struct go_se_talanji_zul_cell_door : public GameObjectAI
{
    go_se_talanji_zul_cell_door(GameObject* go) : GameObjectAI(go) { }

    void OnStateChanged(uint32 state) override
    {
        //if (state == GO_ACTIVATED)
        {
        
            if (InstanceScript* instanceScript = me->GetInstanceScript())
            {
              //  instanceScript->DoSendEventScenario(SCENARIO_EVENT_FREE_PRISONNERS);
                instanceScript->SetData(SCENARIO_EVENT_FREE_PRISONNERS, 1);
            }
        }
    }
};

// 2119
class scene_se_jaina_and_zul : public SceneScript
{
public:
    scene_se_jaina_and_zul() : SceneScript("scene_se_jaina_and_zul") { }

    void OnSceneEnd(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
    {
        player->GetScheduler().Schedule(3s, [](TaskContext context)
        {
            Player* player = GetContextPlayer();
            if (!player)
                return;

            player->CastSpell(player, SPELL_SCENARIO_COMPLETE, true);

            player->AddMovieDelayedAction(857, [player]
            {
                player->CastSpell(player, SPELL_SCENARIO_COMPLETE_TELEPORT, true);
            });
        });
    }
};

struct at_se_boat_final : AreaTriggerAI
{
    at_se_boat_final(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        Player* player = unit->ToPlayer();
        if (!player)
            return;

        player->CastSpell(player, SPELL_SCENARIO_COMPLETE, true);

        player->AddMovieDelayedAction(857, [player]
        {
            player->CastSpell(player, SPELL_SCENARIO_COMPLETE_TELEPORT, true);
        });
    }
};

void AddSC_stormwind_extraction()
{
    RegisterGameObjectAI(go_se_sewer_access_portal);
    RegisterCreatureAI(npc_se_thalyssra);
    RegisterCreatureAI(npc_se_saurfang);
    RegisterGameObjectAI(go_se_talanji_zul_cell_door);
    RegisterSceneScript(scene_se_jaina_and_zul);
    RegisterAreaTriggerAI(at_se_boat_final);
}
