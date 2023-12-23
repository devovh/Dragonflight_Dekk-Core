#include "ScriptMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "temple_of_sethraliss.h"

enum Spells
{
    SPELL_CONSUME_CHARGE = 266512,
    SPELL_CAPACITANCE = 266511,
    SPELL_INDUCTION = 265974,
    //Energy core
    SPELL_BEAM = 265973,
    SPELL_ENERGY_CORE_VISUAL = 265977,
    SPELL_SUMMON_ENERGY_CORE = 274006,
    SPELL_ARC = 265986,
    SPELL_GALVANIZE = 266923,
};

enum Events
{
    EVENT_CHECK_ENERGY = 1,
    EVENT_ENERGY_CORE,
    EVENT_INDUCTION,

    EVENT_CHECK_INBETWEEN,
};

enum Timers
{
    TIMER_CHECK_ENERGY = 1 * IN_MILLISECONDS,
    TIMER_ENERGY_CORE = 15 * IN_MILLISECONDS,
    TIMER_INDUCTION = 5 * IN_MILLISECONDS,

    TIMER_CHECK_INBETWEEN = 1 * IN_MILLISECONDS,
};

enum Creatures
{
    BOSS_GALVAZZT = 133389,
    NPC_ENERGY_CORE_BFA = 135445,
};

const Position centerPos = { 3704.30f, 3412.07f, 6.75f }; //30y

class bfa_boss_galvazzt : public CreatureScript
{
public:
    bfa_boss_galvazzt() : CreatureScript("bfa_boss_galvazzt")
    {
    }

    struct bfa_boss_galvazzt_AI : public BossAI
    {
        bfa_boss_galvazzt_AI(Creature* creature) : BossAI(creature, DATA_GALVAZZT), summons(me)
        {
            me->RemoveUnitFlag2(UNIT_FLAG2_REGENERATE_POWER);
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        SummonList summons;

        void Reset() override
        {
            events.Reset();
            me->SetPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 0);

            summons.DespawnAll();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_ENERGY_CORE_BFA:
                summon->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                summon->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                summon->AddUnitState(UNIT_STATE_ROOT);
                summon->AddAura(SPELL_ENERGY_CORE_VISUAL, summon);
                break;
            }
        }

        void JustDied(Unit*) override
        {
            summons.DespawnAll();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterEvadeMode(EvadeReason /*w*/) override
        {
            _DespawnAtEvade(15s);
        }

        void JustEngagedWith(Unit*) override
        {
            me->SetPower(POWER_ENERGY, 0);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            events.ScheduleEvent(EVENT_CHECK_ENERGY, 12s);
            events.ScheduleEvent(EVENT_INDUCTION, 9s);
            events.ScheduleEvent(EVENT_ENERGY_CORE, 11s);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CHECK_ENERGY:
                    if (me->GetPower(POWER_ENERGY) == 100)
                    {
                        me->CastSpell(me, SPELL_CONSUME_CHARGE);
                        me->SetPower(POWER_ENERGY, 0);
                    }

                    events.ScheduleEvent(EVENT_CHECK_ENERGY, 15s);
                    break;
                case EVENT_INDUCTION:
                {
                    std::list<Player*> playerList;
                    me->GetPlayerListInGrid(playerList, 100.0f);
                    if (playerList.size())
                    {
                        for (auto player : playerList)
                            me->CastSpell(player, SPELL_INDUCTION, true);
                    }

                    events.ScheduleEvent(EVENT_INDUCTION, 12s);
                    break;
                }
                case EVENT_ENERGY_CORE:
                    Position pos;
                    pos = me->GetRandomNearPosition(20.0f);
                    me->SummonCreature(NPC_ENERGY_CORE_BFA, pos, TEMPSUMMON_TIMED_DESPAWN);

                    events.ScheduleEvent(EVENT_ENERGY_CORE, 9s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_boss_galvazzt_AI(creature);
    }
};


class bfa_npc_energy_core : public CreatureScript
{
public:
    bfa_npc_energy_core() : CreatureScript("bfa_npc_energy_core")
    {
    }

    struct bfa_npc_energy_core_AI : public ScriptedAI
    {
        bfa_npc_energy_core_AI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 timer;

        void Reset() override
        {
            timer = 0;
        }

        void UpdateAI(uint32 diff) override
        {
            if (timer <= diff)
            {
                if (Creature* galvazzt = me->FindNearestCreature(NPC_GALVAZZT, 100.0f, true))
                {
                    std::list<Player*> playerList;
                    me->GetPlayerListInGrid(playerList, 100.0f);
                    if (playerList.size())
                    {
                        for (auto player : playerList)
                        {
                            if (player->IsInBetween(me, galvazzt, 3.0f))
                            {
                                me->CastSpell(player, SPELL_GALVANIZE, true);
                                me->CastSpell(player, SPELL_BEAM, true);
                            }
                            else
                            {
                                galvazzt->SetPower(POWER_ENERGY, galvazzt->GetPower(POWER_ENERGY) + urand(1, 5));
                                me->CastSpell(galvazzt, SPELL_BEAM, true);
                            }
                        }
                    }
                }
                timer = 1500;
            }
            else timer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_energy_core_AI(creature);
    }
};

void AddSC_boss_galvazzt()
{
    new bfa_boss_galvazzt();

    new bfa_npc_energy_core();
}
