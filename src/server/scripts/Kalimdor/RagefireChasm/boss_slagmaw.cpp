/*
 * Copyright (C) 2020 DekkCore
 *
 * This SourceCode is NOT free a software. Please hold everything Private
 * and read our Terms
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_LAVA_SPIT = 119434,
    SPELL_SUBMERGE  = 120384
};

enum Events
{
    EVENT_LAVA_SPIT,
    EVENT_SUBMERGE
};

class boss_slagmaw : public CreatureScript
{
    public:
        boss_slagmaw() : CreatureScript("boss_slagmaw") { }

        struct boss_slagmawAI : public ScriptedAI
        {
            boss_slagmawAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void JustDied(Unit* /*who*/) override
            {
                me->RemoveUnitFlag(UnitFlags(UNIT_FLAG_NON_ATTACKABLE));
                me->SetDynamicFlag(UNIT_DYNFLAG_LOOTABLE);
            }


            void Reset() override
            {
                events.Reset();
                me->SetUnitFlag(UnitFlags(UNIT_FLAG_NON_ATTACKABLE));
            }

            void AttackStart(Unit* target) override
            {
                if (me->Attack(target, true))
                    DoStartNoMovement(target);
            }

            void UpdateAI(uint32 const diff) override
            {

                if(me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_slagmawAI(creature);
        }
};

class boss_slagmaw_head : public CreatureScript
{
    public:
        boss_slagmaw_head() : CreatureScript("boss_slagmaw_head") { }

        struct boss_slagmaw_headAI : public ScriptedAI
        {
            boss_slagmaw_headAI(Creature* creature) : ScriptedAI(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_LAVA_SPIT, 8s);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* SlagmawBody = me->FindNearestCreature(61463, 50.0f))
                    me->Kill(SlagmawBody, SlagmawBody);

                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const diff) override
            {
                if(!UpdateVictim())
                    return;

                events.Update(diff);

                if(me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_LAVA_SPIT:
                            DoCastVictim(SPELL_LAVA_SPIT);
                            events.ScheduleEvent(EVENT_LAVA_SPIT, 5s);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_slagmaw_headAI(creature);
        }
};

void AddSC_boss_slagmaw()
{
    new boss_slagmaw();
    new boss_slagmaw_head();
}
