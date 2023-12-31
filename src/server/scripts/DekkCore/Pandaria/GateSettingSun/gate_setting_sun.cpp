/*
 * Copyright (C) 2021 DekkCore
 * Copyright (C) 2017-2019 AshamaneProject <https://github.com/AshamaneProject>
 * Copyright (C) 2016 Firestorm Servers <https://firestorm-servers.com>
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

#include "gate_setting_sun.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"
#include "GameObject.h"
#include "GameObjectAI.h"

enum spells
{
    SPELL_MANTID_MUNITION_EXPLOSION     = 107153,
    SPELL_EXPLOSE_GATE                  = 115456,

    SPELL_BOMB_CAST_VISUAL              = 106729,
    SPELL_BOMB_AURA                     = 106875
};

class mob_serpent_spine_defender : public CreatureScript
{
public:
    mob_serpent_spine_defender() : CreatureScript("mob_serpent_spine_defender") { }

    struct mob_serpent_spine_defenderAI : public ScriptedAI
    {
        mob_serpent_spine_defenderAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 attackTimer;

        void Reset() override
        {
            attackTimer = urand(1000, 5000);
        }

        void DamageDealt(Unit* /*target*/, uint32& damage, DamageEffectType /*damageType*/) override
        {
            damage = 0;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!me->IsInCombat())
            {
                if (attackTimer <= diff)
                {
                    if (Unit* target = me->SelectNearestTarget(5.0f))
                        if (!target->IsFriendlyTo(me))
                            AttackStart(target);
                }
                else
                    attackTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_serpent_spine_defenderAI(creature);
    }
};

class npc_krikthik_bombarder : public CreatureScript
{
public:
    npc_krikthik_bombarder() : CreatureScript("npc_krikthik_bombarder") { }

    struct npc_krikthik_bombarderAI : public ScriptedAI
    {
        npc_krikthik_bombarderAI(Creature* creature) : ScriptedAI(creature)
        {
            pInstance = creature->GetInstanceScript();
        }

        InstanceScript* pInstance;
        uint32 bombTimer;

        void Reset() override
        {
            me->GetMotionMaster()->MoveRandom(5.0f);
            bombTimer = urand(1000, 7500);
        }

        // Called when spell hits a target
        void SpellHitTarget(WorldObject* target, SpellInfo const* /*spell*/) override
        {
            if (target->GetEntry() == NPC_BOMB_STALKER)
                me->AddAura(SPELL_BOMB_AURA, target->ToUnit());
        }

        void UpdateAI(uint32 diff) override
        {
            if (bombTimer <= diff)
            {
                if (Unit* stalker = pInstance->instance->GetCreature(pInstance->GetGuidData(DATA_RANDOM_BOMB_STALKER)))
                    if (!stalker->HasAura(SPELL_BOMB_AURA))
                        me->CastSpell(stalker, SPELL_BOMB_CAST_VISUAL, true);

                bombTimer = urand(1000, 5000);
            }
            else bombTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_krikthik_bombarderAI (creature);
    }
};

//8359
class AreaTrigger_at_first_door : public AreaTriggerScript
{
    public:
        AreaTrigger_at_first_door() : AreaTriggerScript("at_first_door") {}

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (player->GetInstanceScript())
                player->GetInstanceScript()->SetData(DATA_OPEN_FIRST_DOOR, DONE);

            return false;
        }
};

class go_setting_sun_brasier : public GameObjectScript
{
public:
    go_setting_sun_brasier() : GameObjectScript("go_setting_sun_brasier") { }

    struct go_setting_sun_brasierAI : public GameObjectAI
    {
        go_setting_sun_brasierAI(GameObject* go) : GameObjectAI(go) { }

        bool OnGossipHello(Player* player) override
        {
            if (player->GetInstanceScript())
                player->GetInstanceScript()->SetData(DATA_BRASIER_CLICKED, DONE);

            return false;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_setting_sun_brasierAI(go);
    }

};

class go_setting_sun_temp_portal : public GameObjectScript
{
public:
    go_setting_sun_temp_portal() : GameObjectScript("go_setting_sun_temp_portal") { }

    struct go_setting_sun_temp_portalAI : public GameObjectAI
    {
        go_setting_sun_temp_portalAI(GameObject* go) : GameObjectAI(go) { }

        bool OnGossipHello(Player* player) override
        {
            switch (me->GetEntry())
            {
            case 400001:
                player->NearTeleportTo(1078.96f, 2305.48f, 381.55f, 0.01f);
                break;
            case 400002:
                if (me->GetPositionZ() < 400.0f)
                    player->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 431.0f, me->GetOrientation());
                else
                    player->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 388.5f, me->GetOrientation());
                break;
            }

            return false;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_setting_sun_temp_portalAI(go);
    }
};

class vehicle_artillery_to_wall : public VehicleScript
{
    public:
        vehicle_artillery_to_wall() : VehicleScript("vehicle_artillery_to_wall") {}

        void OnAddPassenger(Vehicle* veh, Unit* /*passenger*/, int8 /*seatId*/) override
        {
            if (veh->GetBase())
                if (veh->GetBase()->ToCreature())
                    if (veh->GetBase()->ToCreature()->AI())
                        veh->GetBase()->ToCreature()->AI()->DoAction(0);
        }

        struct vehicle_artillery_to_wallAI : public ScriptedAI
        {
            vehicle_artillery_to_wallAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 launchEventTimer;

            void Reset() override
            {
                launchEventTimer = 0;
            }

            void DoAction(int32 const /*action*/) override
            {
                launchEventTimer = 2500;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!launchEventTimer)
                    return;

                if (launchEventTimer <= diff)
                {
                    if (me->GetVehicleKit())
                    {
                        if (Unit* passenger = me->GetVehicleKit()->GetPassenger(0))
                        {
                            passenger->ExitVehicle();
                            passenger->GetMotionMaster()->MoveJump(1100.90f, 2304.58f, 381.23f, 0.0f, 30.0f, 50.0f, EVENT_JUMP, true);
                        }
                    }

                    launchEventTimer = 0;
                }
                else launchEventTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new vehicle_artillery_to_wallAI(creature);
        }
};

class go_setting_sun_elevator : public GameObjectScript
{
    public:
        go_setting_sun_elevator() : GameObjectScript("go_setting_sun_elevator") { }

        InstanceScript* m_Instance;

        struct go_setting_sun_elevatorAI : public GameObjectAI
        {
            go_setting_sun_elevatorAI(GameObject* go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player) override
            {
                if (!player)
                    return false;

                std::list<Player*> l_PlayerList;
                GetPlayerListInGrid(l_PlayerList, player, 45.0f);

                for (Player* player : l_PlayerList)
                    player->TeleportTo(962, 1193.399f, 2290.80f, 430.87f, 1.52f);

                return false;
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_setting_sun_elevatorAI(go);
        }
};

#ifndef __clang_analyzer__
void AddSC_gate_setting_sun()
{
    new mob_serpent_spine_defender();
    new npc_krikthik_bombarder();
    new AreaTrigger_at_first_door();
    new go_setting_sun_brasier();
    new go_setting_sun_temp_portal();
    new vehicle_artillery_to_wall();
    new go_setting_sun_elevator();
}
#endif
