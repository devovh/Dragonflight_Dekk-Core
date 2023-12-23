/*
 * Copyright 2022 BaldazzarCore-Shadowlands
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
#include "theater_of_pain.h"

ObjectGuid mordrethaGuid;
ObjectGuid necroArenaDoorGuid;

 //2293
struct instance_theater_of_pain : public InstanceScript
{
    instance_theater_of_pain(InstanceMap* map) : InstanceScript(map) { }

    void OnCreatureCreate(Creature* cr) override
    {  
        switch (cr->GetEntry())
        {
        case NPC_MORDRETHA:
            mordrethaGuid = cr->GetGUID();
            break;
        }
    }

    void OnGameObjectCreate(GameObject* go) override
    {  
        switch (go->GetEntry())
        {
        case GO_NECRO_ARENA_TRAPDOOR:
            necroArenaDoorGuid = go->GetGUID();
            break;
        }
    }

    bool SetBossState(uint32 type, EncounterState state) override
    {
        if (!InstanceScript::SetBossState(type, state))
            return false;

        switch(type)
        {
        case NPC_KULTHAROK:
            if (state == DONE)
            {
                if (Creature* mordretha = instance->GetCreature(mordrethaGuid))
                {
                    mordretha->SetReactState(REACT_DEFENSIVE);
                    mordretha->SetVisible(true);
                    mordretha->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                }
                if (GameObject* necroTrapDoor = instance->GetGameObject(necroArenaDoorGuid))
                    necroTrapDoor->SetGoState(GO_STATE_READY);
            }
            break;
        }

        return true;
    }
};

//165785
struct npc_maldraxxian_honor_guard : public ScriptedAI
{
    npc_maldraxxian_honor_guard(Creature* c) : ScriptedAI(c) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_NON_ATTACKABLE));
    }
};

//174651
struct npc_disgraced_contender_174651 : public ScriptedAI
{
    npc_disgraced_contender_174651(Creature* c) : ScriptedAI(c) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UnitFlags(UNIT_FLAG_NON_ATTACKABLE));
    }

    void MoveInLineOfSight(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->GetDistance2d(me) <= 5.0f)
            target->NearTeleportTo(2810.934f, -2546.195f, 3263.074f, 0.016f, false);
    }
};

//354858
struct go_portal_354858 : public GameObjectAI
{
    go_portal_354858(GameObject* go) : GameObjectAI(go) { }

private:
    TaskScheduler scheduler;

    void Reset() override
    {
        me->GetScheduler().CancelAll();
        me->GetScheduler().Schedule(1s, [this] (TaskContext context)
        {
            if (!me)
                return;

            std::list<Player*> targetList;
            me->GetPlayerListInGrid(targetList, 10.0f);
            if (targetList.empty())
                return;

            for (Player* targets : targetList)
            {
                if (targets->IsInCombat())
                    return;

                if (me->GetPositionX() == 3017.29f)
                    targets->NearTeleportTo(3098.791f, -2654.123f, 3128.170f, 2.342f, false);

                if (me->GetPositionX() == 2968.780)
                    targets->NearTeleportTo(2943.654f, -2559.198f, 3122.249f, 2.479f, false);
            }

            if (me->IsInWorld())
                context.Repeat(3s);
        });
    } 

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

void AddSC_instance_theater_of_pain()
{
    RegisterInstanceScript(instance_theater_of_pain, 2293);
    RegisterCreatureAI(npc_maldraxxian_honor_guard);
    RegisterCreatureAI(npc_disgraced_contender_174651);
    RegisterGameObjectAI(go_portal_354858);
}
