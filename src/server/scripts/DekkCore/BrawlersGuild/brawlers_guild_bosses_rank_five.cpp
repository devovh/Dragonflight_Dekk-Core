/*
 * Copyright (C) 2022 DekkCore
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

#include "AchievementMgr.h"
#include "BrawlersGuild.h"
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "AreaTriggerAI.h"


enum eSpells
{
    //! 5 rank
    // thwack
    SPELL_FIRE_BALL = 228867, // 4
    SPELL_SECOND_DIE = 229736,
    SPELL_SIT_DOWN = 229152, // 16-18


    // general
    SPELL_CALL_WAVE = 231711,  // 60 duration 36
    SPELL_CALL_WAVE_2 = 231713,  // 60 duration 15

    // razorgrin
    SPELL_BITE = 142734,

    // 5 gnomes
    SPELL_LEPEROUS_SPEW = 133157,
    SPELL_SPRINT = 133171,

    // blackmange
    SPELL_CHARRRGE = 228826,
    SPELL_SHOT_VISUAL = 228815,
};

float coordinates[4]
{
    -142.39f, // x line AL
    2485.23f,  // y start AL 

    2011.79f, // x start H
    -4734.03f // y line H

};

uint32 timer_for_summon[5]
{
    6000,
    10000,
    10000,
    30000,
    16000
};

// 115432 controller
class npc_brawguild_thwack_u_controller : public CreatureScript
{
public:
    npc_brawguild_thwack_u_controller() : CreatureScript("npc_brawguild_thwack_u_controller") {}

    struct npc_brawguild_thwack_u_controllerAI : public BossAI
    {
        npc_brawguild_thwack_u_controllerAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_FIVE) {}

        GuidList special_summons;
        uint8 phase;

        void Reset() override
        {
            phase = 0;
            events.Reset();
            events.RescheduleEvent(1, 5s);

          //  events.RescheduleEvent(3, timer_for_summon[phase++]);

            me->SetReactState(REACT_PASSIVE);

            if (Unit* unit = me->GetOwner())
                if (Player* owner = unit->ToPlayer())
                {
                    if (owner->GetTeamId() == TEAM_ALLIANCE)
                    {
                        // dancers
                        if (Creature* cre = me->SummonCreature(115717, me->GetPositionX() + 5.0f, me->GetPositionY() + 5.0f, me->GetPositionZ(), 3.14f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115712, me->GetPositionX() + 5.0f, me->GetPositionY() - 5.0f, me->GetPositionZ(), 3.14f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115662, me->GetPositionX() + 6.0f, me->GetPositionY() - 8.0f, me->GetPositionZ(), 3.14f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115639, me->GetPositionX() + 6.0f, me->GetPositionY() + 8.0f, me->GetPositionZ(), 3.14f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115654, me->GetPositionX() + 7.0f, me->GetPositionY(), me->GetPositionZ(), 3.14f))
                            special_summons.push_back(cre->GetGUID());

                        // bosses
                        if (Creature* add = owner->SummonCreature(114945, me->GetPositionX() + 4.0f, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                        {
                            add->AI()->AttackStart(owner);
                            summons.Summon(add);
                        }
                        if (Creature* add = owner->SummonCreature(114944, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                        {
                            add->AI()->AttackStart(owner);
                            summons.Summon(add);
                        }

                        // sounds
                        for (uint8 x = 0; x <= 6; ++x)
                            me->SummonCreature(114953, coordinates[0], coordinates[1] + 6 * x, me->GetPositionZ(), me->GetOrientation());


                    }
                    else
                    {
                        // dancers
                        if (Creature* cre = me->SummonCreature(115717, me->GetPositionX() + 5.0f, me->GetPositionY() - 5.0f, me->GetPositionZ(), 1.57f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115712, me->GetPositionX() - 5.0f, me->GetPositionY() - 5.0f, me->GetPositionZ(), 1.57f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115662, me->GetPositionX() - 8.0f, me->GetPositionY() - 6.0f, me->GetPositionZ(), 1.57f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115639, me->GetPositionX() + 8.0f, me->GetPositionY() - 6.0f, me->GetPositionZ(), 1.57f))
                            special_summons.push_back(cre->GetGUID());
                        if (Creature* cre = me->SummonCreature(115654, me->GetPositionX(), me->GetPositionY() - 7.0f, me->GetPositionZ(), 1.57f))
                            special_summons.push_back(cre->GetGUID());

                        // bosses
                        if (Creature* add = owner->SummonCreature(114945, me->GetPositionX(), me->GetPositionY() - 4.0f, me->GetPositionZ(), me->GetOrientation()))
                        {
                            add->AI()->AttackStart(owner);
                            summons.Summon(add);
                        }
                        if (Creature* add = owner->SummonCreature(114944, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                        {
                            add->AI()->AttackStart(owner);
                            summons.Summon(add);
                        }

                        // sounds
                        for (uint8 x = 0; x <= 6; ++x)
                            me->SummonCreature(114953, coordinates[2] + 6 * x, coordinates[3], me->GetPositionZ(), me->GetOrientation());

                    }

                }
        }

        void EnterEvadeMode(EvadeReason /*why*/) override {}
        void JustEngagedWith(Unit* /*who*/) override {}

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);

            if (summon->GetEntry() != 114945 && summon->GetEntry() != 114944)
                summon->AddDelayedEvent(1000, [summon]() -> void
                    {
                        summon->SetReactState(REACT_PASSIVE);
                    });
            else
                summon->RemoveUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1 ));
        }


        void JustDied(Unit* /*who*/) override
        {
            summons.DespawnAll();
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    DoCast(SPELL_CALL_WAVE);
                    events.RescheduleEvent(2, 42s);
                    break;
                case 2:
                    DoCast(SPELL_CALL_WAVE_2);
                    events.RescheduleEvent(1, 20s);
                    break;
                case 3:
                {
                    if (Creature* dancer = me->GetMap()->GetCreature(special_summons.front()))
                    {
                        special_summons.remove(dancer->GetGUID());
                        dancer->RemoveUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1));
                        dancer->SetReactState(REACT_AGGRESSIVE);
                        if (Unit* owner = me->GetOwner())
                            dancer->AI()->AttackStart(owner);
                        dancer->AI()->Talk(0);

                        if (Vehicle* veh = dancer->GetVehicleKit()) // it's need to remove flag from all passengers
                        {
                            Unit* passenger = NULL;
                            uint8 seat = 0;
                            do
                            {
                                passenger = veh->GetPassenger(seat++);
                                if (passenger)
                                {
                                    passenger->RemoveUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1));
                                    if (Unit* owner = me->GetOwner())
                                        passenger->ToCreature()->AI()->AttackStart(owner);
                                }
                                else
                                    break;
                            } while (passenger != nullptr);
                        }
                    }
                   // if (phase < 5)
                     //   events.RescheduleEvent(3, timer_for_summon[phase++]);
                }
                break;
                }
            }

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_brawguild_thwack_u_controllerAI(creature);
    }
};

// 114944 (S) + 114945(W)
class boss_brawguild_thwack_u : public CreatureScript
{
public:
    boss_brawguild_thwack_u() : CreatureScript("boss_brawguild_thwack_u") {}

    struct boss_brawguild_thwack_uAI : public BossAI
    {
        boss_brawguild_thwack_uAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_FIVE) {}

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            if (me->GetEntry() == 114944)
                events.RescheduleEvent(1, 4s);
            else
                events.RescheduleEvent(2, randtime(16s, 18s));
        }

        void JustDied(Unit* /*who*/) override
        {
            if (!me->HasAura(SPELL_SECOND_DIE))
                if (Creature* other = me->FindNearestCreature((me->GetEntry() == 114944 ? 114945 : 114944), 200.0f, true))
                {
                    other->AddAura(SPELL_SECOND_DIE, other);
                    return;
                }

            _Reset();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    DoCast(SPELL_FIRE_BALL);
                    events.RescheduleEvent(1, 4s);
                    break;
                case 2:
                    DoCast(SPELL_SIT_DOWN);
                    events.RescheduleEvent(2, randtime(16s, 18s));
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_thwack_uAI(creature);
    }
};

// 71085
class boss_brawguild_razorgrin : public CreatureScript
{
public:
    boss_brawguild_razorgrin() : CreatureScript("boss_brawguild_razorgrin") {}

    struct boss_brawguild_razorgrinAI : public BossAI
    {
        boss_brawguild_razorgrinAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_FIVE) {}

        void JustEngagedWith(Unit* /*who*/) override {}

        void KilledUnit(Unit* who) override
        {
            if (me->GetOwner() && me->GetOwner()->IsCreature())
                me->GetOwner()->ToCreature()->AI()->KilledUnit(who); // some hack

            if (me->GetOwner() && me->GetOwner()->IsPlayer())
            {
                Player* player = me->GetOwner()->ToPlayer();
                player->AddDelayedEvent(1000, [player]() -> void
                    {

                    });
            }
        }

        void JustDied(Unit* who) override
        {
            if (!who)
                return;

            if (me->GetOwner() && me->GetOwner()->IsCreature())
                me->GetOwner()->ToCreature()->AI()->JustDied(who); // some hack

            if (me->GetOwner() && me->GetOwner()->IsPlayer())
            {
                Player* player = me->GetOwner()->ToPlayer();
                player->AddDelayedEvent(1000, [player]() -> void
                    {

                    });
            }
        }

        void EnterEvadeMode(EvadeReason /*why*/) override
        {
            if (me->GetOwner() && me->GetOwner()->IsCreature())
                me->GetOwner()->ToCreature()->AI()->EnterEvadeMode(); // some hack

            if (me->GetOwner() && me->GetOwner()->IsPlayer())
            {
                Player* player = me->GetOwner()->ToPlayer();
                player->AddDelayedEvent(1000, [player]() -> void
                    {

                    });
            }
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_razorgrinAI(creature);
    }
};

uint32 gnomes[5]
{
    67515, 67514, 67513, 67516, 67511
};
// 67509
class boss_brawguild_five_gnomes_controller : public CreatureScript
{
public:
    boss_brawguild_five_gnomes_controller() : CreatureScript("boss_brawguild_five_gnomes_controller") {}

    struct boss_brawguild_five_gnomes_controllerAI : public BossAI
    {
        boss_brawguild_five_gnomes_controllerAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_FIVE) {}

        uint8 count_adds;
        void Reset() override
        {
            count_adds = 0;
            for (uint32 id : gnomes)
                me->SummonCreature(id, me->GetPositionX() + irand(-5, 5), me->GetPositionY() + irand(-5, 5), me->GetPositionZ(), me->GetOrientation());
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            summon->SetRegenerateHealth(false);
            summon->SetHealth(summon->GetMaxHealth() * (0.6 + 0.1 * count_adds));
            count_adds++;
        }

        void JustEngagedWith(Unit* /*who*/) override {}

        void EnterEvadeMode(EvadeReason /*why*/) override {}

        void JustDied(Unit* /*who*/) override
        {
            _Reset();
        }

        void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/) override
        {

            count_adds--;
            if (count_adds <= 0)
            {
                _Reset();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_five_gnomes_controllerAI(creature);
    }
};

// 67515, 67514, 67513, 67516, 67511
class boss_brawguild_five_gnomes : public CreatureScript
{
public:
    boss_brawguild_five_gnomes() : CreatureScript("boss_brawguild_five_gnomes") {}

    struct boss_brawguild_five_gnomesAI : public BossAI
    {
        boss_brawguild_five_gnomesAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_FIVE) {}

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.RescheduleEvent(1, 4s);
            events.RescheduleEvent(2, randtime(3s, 8s));
        }

        void JustDied(Unit* /*who*/) override
        {
            _Reset();
        }

        void KilledUnit(Unit* who) override
        {
            if (Unit* owner = me->GetOwner())
                if (owner->IsCreature())
                    owner->ToCreature()->AI()->KilledUnit(who); // some hack
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    DoCast(SPELL_LEPEROUS_SPEW);
                    events.RescheduleEvent(1, 4s);
                    break;
                case 2:
                    DoCast(SPELL_SPRINT);
                    events.RescheduleEvent(2, randtime(3s, 8s));
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_five_gnomesAI(creature);
    }
};

// 114902
class boss_brawguild_blackmange : public CreatureScript
{
public:
    boss_brawguild_blackmange() : CreatureScript("boss_brawguild_blackmange") {}

    struct boss_brawguild_blackmangeAI : public BossAI
    {
        boss_brawguild_blackmangeAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_FIVE) {}

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            if (Unit* unit = me->GetOwner())
                if (Player* owner = unit->ToPlayer())
                {
                    bool isAllance = owner->GetTeamId() == TEAM_ALLIANCE;
                    for (int8 x = -3; x <= 16; ++x)
                        if (isAllance)
                            me->SummonCreature(114907, coordinates[0], coordinates[1] + 2 * x, me->GetPositionZ(), me->GetOrientation());
                        else
                            me->SummonCreature(114907, coordinates[2] + 2 * x + 10.0f, coordinates[3], me->GetPositionZ(), me->GetOrientation());
                }
            events.RescheduleEvent(1, 21s);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            if (me->IsInCombat())
                DoZoneInCombat(summon);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    DoCast(SPELL_CHARRRGE);
                    events.RescheduleEvent(1, randtime(19s, 21s));
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_blackmangeAI(creature);
    }
};

uint32 SPELL_FUSE[3]
{
    228795,
    228820,
    228821
};

// 114907
class boss_brawguild_blackmange_guns : public CreatureScript
{
public:
    boss_brawguild_blackmange_guns() : CreatureScript("boss_brawguild_blackmange_guns") {}

    struct boss_brawguild_blackmange_gunsAI : public ScriptedAI
    {
        boss_brawguild_blackmange_gunsAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void Reset() override
        {
            events.Reset();
            events.RescheduleEvent(1, 6s);
        }

        void SpellFinishCast(const SpellInfo* spell)
        {
            if (spell->Id == 228812)
            {
                DoCast(SPELL_SHOT_VISUAL);
                events.RescheduleEvent(1, 2s);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    if (!me->GetOwner() || !me->GetOwner()->IsAlive())
                        me->DespawnOrUnsummon();
                    else
                        DoCast(SPELL_FUSE[urand(0, 2)]);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_blackmange_gunsAI(creature);
    }
};

void AddSC_the_brawlers_guild_bosses_rank_five()
{
    new npc_brawguild_thwack_u_controller();
    new boss_brawguild_thwack_u();
    new boss_brawguild_razorgrin();
    new boss_brawguild_five_gnomes_controller();
    new boss_brawguild_five_gnomes();
    new boss_brawguild_blackmange();
    new boss_brawguild_blackmange_guns();
};
