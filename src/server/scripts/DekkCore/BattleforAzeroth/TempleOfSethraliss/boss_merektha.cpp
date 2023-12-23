#include "ScriptMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "temple_of_sethraliss.h"
#include <sstream>

enum Spells
{
    SPELL_NOXIOUS_BREATH = 272657,
    SPELL_NOXIOUS_BREATH_AT_TRIGGER = 263925, //at 12750
    SPELL_BLINDING_SAND = 263914,
    SPELL_DUST_CLOUD_DUMMY = 256359,
    SPELL_DUST_CLOUD_MISSILE = 256336, // npc 134390, spawn spell 267047

    SPELL_BURROW_KNOCKBACK = 264206,
    SPELL_BURROW_CREAT_AT = 264194, // at 12779
    SPELL_A_KNOT_OF_SNAKES = 263958,

    SPELL_CYTOTOXIN = 267027,
};

enum Events
{
    EVENT_NOXIOUS_BREATH = 1,
    EVENT_BURROW,
    EVENT_EMERGE,
    EVENT_BLINDING_SAND,
    EVENT_A_KNOT_OF_SNAKES,

    EVENT_CYTOTOXIN,
};

enum Timers
{
    TIMER_EMERGE = 30 * IN_MILLISECONDS,
    TIMER_NOXIOUS_BREATH = 15 * IN_MILLISECONDS,
    TIMER_BLIDING_SAND = 28 * IN_MILLISECONDS,
    TIMER_A_KNOT_OF_SNAKES = 35 * IN_MILLISECONDS,
    TIMER_CYTOTOXIN = 10 * IN_MILLISECONDS,
};

enum Creatures
{
    BOSS_MEREKTHA = 133384,
    NPC_SAND_CRUSTED_STRIKER_BFA = 134390,
    NPC_VENOMOUS_OPHIDIAN = 135562,
    NPC_A_KNOT_OF_SNAKES = 135029,
};

const Position centerPos = { 3549.29f, 3423.07f, 51.68f }; //55y

//toxic pool and dust cloud AT visual

class bfa_boss_merektha : public CreatureScript
{
public:
    bfa_boss_merektha() : CreatureScript("bfa_boss_merektha")
    {
    }

    struct bfa_boss_merektha_AI : public BossAI
    {
        bfa_boss_merektha_AI(Creature* creature) : BossAI(creature, DATA_MEREKTHA), summons(me)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
        SummonList summons;
        bool phase2;
        bool phase3;
        uint8 burrow;

        void Reset() override
        {
            events.Reset();
            summons.DespawnAll();
            burrow = 0;
            phase2 = false;
            phase3 = false;

            me->RemoveAura(167209);
            me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);

            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_SAND_CRUSTED_STRIKER_BFA:
            case NPC_VENOMOUS_OPHIDIAN:
                summon->SetInCombatWithZone();
                break;
            }
        }

        void JustDied(Unit*) override
        {
            me->RemoveAura(167209);
            summons.DespawnAll();
            me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterEvadeMode(EvadeReason /*w*/) override
        {
            Reset();
            _DespawnAtEvade(15s);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            switch (summon->GetEntry())
            {
            case NPC_A_KNOT_OF_SNAKES:
                me->CastStop(SPELL_A_KNOT_OF_SNAKES);
                break;
            }
        }

        void DamageTaken(Unit* /*at*/, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (me->HealthBelowPct(60) && !phase2)
            {
                phase2 = true;

                ++burrow;

                std::ostringstream str;
                str << "Merektha |cFFF00000|h[Burrows]|h|r across the battlefield!";
                me->TextEmote(str.str().c_str(), 0, true);

                events.Reset();
                events.ScheduleEvent(EVENT_BURROW, 1s);
                events.ScheduleEvent(EVENT_EMERGE, 30s);

                for (uint8 i = 0; i < 4; ++i)
                    me->SummonCreature(NPC_VENOMOUS_OPHIDIAN, 3550.0f, 3447.0f, 51.0f, TEMPSUMMON_MANUAL_DESPAWN);
            }

            if (me->HealthBelowPct(45) && !phase3)
            {
                phase3 = true;

                ++burrow;

                std::ostringstream str;
                str << "Merektha |cFFF00000|h[Burrows]|h|r across the battlefield!";
                me->TextEmote(str.str().c_str(), 0, true);

                events.Reset();
                events.ScheduleEvent(EVENT_BURROW, 1s);
                events.ScheduleEvent(EVENT_EMERGE, 30s);

                for (uint8 i = 0; i < 4; ++i)
                    me->SummonCreature(NPC_SAND_CRUSTED_STRIKER_BFA, 3550.0f, 3447.0f, 51.0f, TEMPSUMMON_MANUAL_DESPAWN);
            }
        }

        void JustEngagedWith(Unit*) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

           if (me->GetMap()->IsHeroic() || me->GetMap()->IsMythic())
                events.ScheduleEvent(EVENT_A_KNOT_OF_SNAKES, 12s);

           events.ScheduleEvent(EVENT_NOXIOUS_BREATH, 15s);
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
                case EVENT_NOXIOUS_BREATH:
                    if (Unit* target = me->GetVictim())
                        me->CastSpell(target, SPELL_NOXIOUS_BREATH);
                    events.ScheduleEvent(EVENT_NOXIOUS_BREATH, 20s);
                    break;
                case EVENT_BLINDING_SAND:
                {
                    std::ostringstream str;
                    str << "Merektha is about to hit you with |cFFF00000|h[Blinding Sand]|h|r, look away!";
                    me->TextEmote(str.str().c_str(), 0, true);

                    if (Unit* target = me->GetVictim())
                        me->CastSpell(target, SPELL_BLINDING_SAND);
                    events.ScheduleEvent(EVENT_BLINDING_SAND, 14s);
                    break;
                }
                case EVENT_A_KNOT_OF_SNAKES:
                {
                    std::list<Unit*> targets;

                    if (!targets.empty())
                        if (targets.size() >= 1)
                            targets.resize(1);
                    for (auto target : targets)
                    {
                        me->CastSpell(target, SPELL_A_KNOT_OF_SNAKES, true);
                        me->SummonCreature(NPC_A_KNOT_OF_SNAKES, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);

                        std::ostringstream str;
                        str << target->GetName() << " is enveloped by |cFFF00000|h[A Knot of Snakes]|h|r free him!";
                        me->TextEmote(str.str().c_str(), 0, true);
                    }

                    events.ScheduleEvent(EVENT_A_KNOT_OF_SNAKES, 9s);
                    break;
                }
                case EVENT_BURROW:
                    me->SetObjectScale(0.1f);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveRandom(30.0f);
                    me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                    me->AddAura(167209, me);
                    break;
                case EVENT_EMERGE:
                    me->SetObjectScale(1.0f);
                    me->RemoveAura(167209);
                    me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                    me->GetMotionMaster()->Clear();
                    if (Unit* target = me->GetVictim())
                        me->Attack(target, true);
                    if (burrow == 1)
                        events.ScheduleEvent(EVENT_BLINDING_SAND, 16s);
                    else if (burrow > 1)
                    {
                        events.ScheduleEvent(EVENT_NOXIOUS_BREATH, 9s);
                        events.ScheduleEvent(EVENT_BLINDING_SAND, 15s);
                    }

                    if (me->GetMap()->IsHeroic() || me->GetMap()->IsMythic())
                        events.ScheduleEvent(EVENT_A_KNOT_OF_SNAKES, 19s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_boss_merektha_AI(creature);
    }
};

class bfa_npc_venomous_ophidian : public CreatureScript
{
public:
    bfa_npc_venomous_ophidian() : CreatureScript("bfa_npc_venomous_ophidian")
    {
    }

    struct bfa_npc_venomous_ophidian_AI : public ScriptedAI
    {
        bfa_npc_venomous_ophidian_AI(Creature* creature) : ScriptedAI(creature)
        {
        }

        EventMap events;

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit*) override
        {
            events.ScheduleEvent(EVENT_CYTOTOXIN, 16s);
        }

        Creature* GetMerektha()
        {
            return me->FindNearestCreature(BOSS_MEREKTHA, 200.0f, true);
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
                case EVENT_CYTOTOXIN:
                    if (Creature* merektha = GetMerektha())
                        if (Unit* target = merektha->GetVictim())
                            me->CastSpell(target, SPELL_CYTOTOXIN);
                    events.ScheduleEvent(EVENT_CYTOTOXIN, 16s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_venomous_ophidian_AI(creature);
    }
};

void AddSC_boss_merektha()
{
    new bfa_boss_merektha();

    new bfa_npc_venomous_ophidian();
}
