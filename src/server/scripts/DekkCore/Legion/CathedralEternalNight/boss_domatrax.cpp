#include "cathedral_of_eternal_night.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "AreaTriggerTemplate.h"
#include "TemporarySummon.h"

enum Says
{
    SAY_INTRO = 0,
    SAY_AGGRO = 1,
    SAY_WAVE = 2,
    SAY_WAVE_TWO = 3,
    SAY_CHAOTIC_ENERGY = 4,
    SAY_CHAOTIC_ENERGY_1 = 5,
    SAY_DEATH = 6,
};

enum Spells
{
    SPELL_WAVE_ONE = 235822,
    SPELL_WAVE_TWO = 235862,

    SPELL_FELSOUL_CLEAVE = 236543, // 18
    SPELL_CHAOTIC_ENERGY = 239339, // (visual only ?)
    SPELL_CHAOTIC_ENERGY_DMG = 234107, // 

    SPELL_ENRAGE = 243157, // 25%

    // egida
    SPELL_EGIDA_DISPLAY_BAR = 236421,
    SPELL_EGIDA_AT = 239350,
    SPELL_EGIDA_AT_BUFF = 238410,
    SPELL_EGIDA_OUTRO = 239352,

    // portals (118834)
    SPELL_PORTAL_PERIODIC_ONE = 235827,  // heroic - imps,
    SPELL_PORTAL_PERIODIC_TWO = 235881,  // heroic - guards, mythic - shivarras
    SPELL_PORTAL_MYTHIC_ADD = 241622,

    // other
    SPELL_VISUAL_SPAWN = 211762,
    SPELL_INTRO = 241001,
    SPELL_OUTRO = 241382,
};

enum eEvents
{
    EVENT_FELSOUL_CLEAVE = 1,
    EVENT_CHAOTIC_ENERGY,
    EVENT_WAVE_ONE,
    EVENT_WAVE_TWO,
    EVENT_ENRAGE
};

Position const portals_position[4]
{
    {-519.67f, 2548.22f, 533.94f, 3.94f},
    {-522.056f, 2504.01f, 533.94f, 2.24f},
    {-564.17f, 2503.62f, 533.94f, 0.77f},
    {-561.85f, 2549.77f, 533.94f, 5.45f}
};

// 118804
class boss_domatrax : public CreatureScript
{
public:
    boss_domatrax() : CreatureScript("boss_domatrax") {}

    struct boss_domatraxAI : public BossAI
    {
        boss_domatraxAI(Creature* creature) : BossAI(creature, DATA_DOMATRAX) {}

        uint8 phase = 0;
        uint32 const phases_hp_required[3] = { 90, 50, 25 };

        void Reset() override
        {
            _Reset();

            phase = 0;
            me->SetPower(POWER_ENERGY, 0);

            if (Creature* egida = instance->instance->GetCreature(instance->GetGuidData(NPC_EGIDA_START)))
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, egida);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_INSTANCE_END, egida);

                egida->RemoveAurasDueToSpell(SPELL_EGIDA_DISPLAY_BAR);
                egida->RemoveAurasDueToSpell(SPELL_EGIDA_AT);
            }

            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
            me->SetReactState(REACT_PASSIVE);
            me->SetVisible(false);
        }

        void JustEngagedWith(Unit* who) override
        {
            Talk(SAY_AGGRO);
            _JustEngagedWith(who);

            phase = 0;
            me->SetPower(POWER_ENERGY, 0);

            events.RescheduleEvent(EVENT_FELSOUL_CLEAVE, 8s);
            events.RescheduleEvent(EVENT_CHAOTIC_ENERGY, 1s);

            if (Creature* egida = instance->instance->GetCreature(instance->GetGuidData(NPC_EGIDA_START)))
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, egida);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_INSTANCE_START, egida);

                egida->CastSpell(egida, SPELL_EGIDA_DISPLAY_BAR);
                egida->CastSpell(egida, SPELL_EGIDA_AT);
            }

        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);

            summon->CastSpell(summon, SPELL_VISUAL_SPAWN);
            if (summon->GetEntry() == NPC_PORTAL_OWNER)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, summon);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_INSTANCE_START, summon);

                summon->SetReactState(REACT_PASSIVE);
                summon->CastSpell(summon, phase == 1 ? SPELL_PORTAL_PERIODIC_ONE : SPELL_PORTAL_PERIODIC_TWO); // periodic summon adds

                me->SummonCreature(phase == 1 ? NPC_IMP : (me->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC ? NPC_FELGUARD : NPC_SHIVARRA), summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ(), summon->GetOrientation());

                if (me->GetMap()->GetDifficultyID() != DIFFICULTY_HEROIC) // mythic and other. normal mode isn't exist
                    summon->CastSpell(summon, SPELL_PORTAL_MYTHIC_ADD);
            }

            DoZoneInCombat(summon);
        }


        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            if (summon->GetEntry() == NPC_PORTAL_OWNER)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_INSTANCE_END, summon);
            }
        }

       void OnSpellCast(SpellInfo const* spell) override
        {

            switch (spell->Id)
            {
            case SPELL_WAVE_TWO:
                Talk(SAY_WAVE_TWO);
                // no break
            case SPELL_WAVE_ONE:
                Talk(SAY_WAVE);
                uint8 position = urand(0, 3);
                for (uint8 i = 0; i < 2; ++i)
                {
                    me->SummonCreature(NPC_PORTAL_OWNER, portals_position[position++]);

                    if (position >= 4)
                        position = 0;
                }
                break;
            }
        }

        void DoAction(int32 const action) override
        {
            me->SetVisible(true);
            DoCast(SPELL_INTRO);
            Talk(SAY_INTRO);

            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
            me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
            me->SetReactState(REACT_DEFENSIVE);

        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);
            _JustDied();

            if (Creature* egida = instance->instance->GetCreature(instance->GetGuidData(NPC_EGIDA_START)))
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, egida);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_INSTANCE_END, egida);

                egida->RemoveAurasDueToSpell(SPELL_EGIDA_DISPLAY_BAR);
                egida->RemoveAurasDueToSpell(SPELL_EGIDA_AT);

                me->CastSpell(egida, SPELL_OUTRO, true);
                egida->CastSpell(egida, SPELL_EGIDA_OUTRO);
                egida->DespawnOrUnsummon(3s);

                egida->SummonCreature(NPC_EGIDA_GIVE_BUFF, egida->GetPositionX() + frand(-3, 3), egida->GetPositionY() + frand(-3, 3), egida->GetPositionZ(), egida->GetOrientation());
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (phase < 3 && me->HealthBelowPct(phases_hp_required[phase]))
            {
                switch (phase++)
                {
                case 0:
                    events.RescheduleEvent(EVENT_WAVE_ONE, 10s);
                    break;
                case 1:
                    events.RescheduleEvent(EVENT_WAVE_TWO, 10s);
                    break;
                case 2:
                    events.RescheduleEvent(EVENT_ENRAGE, 10s);
                    break;
                default:
                    break;
                }
            }
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
                case EVENT_FELSOUL_CLEAVE:
                    DoCast(SPELL_FELSOUL_CLEAVE);
                    events.RescheduleEvent(EVENT_FELSOUL_CLEAVE, 18s);
                    break;
                case EVENT_CHAOTIC_ENERGY:
                    if (me->GetPower(POWER_ENERGY) < 100)
                    {
                        me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) + urand(2, 4));
                        events.RescheduleEvent(EVENT_CHAOTIC_ENERGY, 1s);
                    }
                    else
                    {
                        Talk(SAY_CHAOTIC_ENERGY);
                        Talk(SAY_CHAOTIC_ENERGY_1);
                        DoCast(SPELL_CHAOTIC_ENERGY);
                        DoCast(SPELL_CHAOTIC_ENERGY_DMG);
                        events.RescheduleEvent(EVENT_CHAOTIC_ENERGY, 5s);
                    }
                    break;
                case EVENT_WAVE_ONE:
                    DoCast(SPELL_WAVE_ONE);
                    break;
                case EVENT_WAVE_TWO:
                    DoCast(SPELL_WAVE_TWO);
                    break;
                case EVENT_ENRAGE:
                    DoCast(SPELL_ENRAGE);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_domatraxAI(creature);
    }
};

// 118884
class npc_coen_egida : public CreatureScript
{
public:
    npc_coen_egida() : CreatureScript("npc_coen_egida") {}

    struct npc_coen_egidaAI : public ScriptedAI
    {
        npc_coen_egidaAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
        }

        void Reset() override {}

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            me->RemoveAurasDueToSpell(144373);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);

            if (InstanceScript* instance = me->GetInstanceScript())
                if (Creature* domatrax = instance->instance->GetCreature(instance->GetGuidData(BOSS_DOMATRAX)))
                    domatrax->AI()->DoAction(true);

        return true;
        }

        void UpdateAI(uint32 diff) override { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_coen_egidaAI(creature);
    }
};

// 14825
class areatrigger_domatrax_egida_shield : public AreaTriggerScript
{
public:
    areatrigger_domatrax_egida_shield() : AreaTriggerScript("areatrigger_domatrax_egida_shield") { }

    struct areatrigger_domatrax_egida_shieldAI : AreaTriggerAI
    {
        areatrigger_domatrax_egida_shieldAI(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger)
        {
            scale_by_one_pct = 1;
        }

        float scale_by_one_pct{};

        void OnUpdate(uint32 diff) override
        {
            Unit* caster = at->GetCaster();
            if (!caster)
                return;

            float new_scale = scale_by_one_pct * caster->GetPower(POWER_ALTERNATE_POWER);
            if (!new_scale)
            {
                at->Remove();
                return;
            }

            at->SetObjectScale(new_scale);
        }

    };

    AreaTriggerAI* GetAI(AreaTrigger* areatrigger) const
    {
        return new areatrigger_domatrax_egida_shieldAI(areatrigger);
    }
};

// 235827, 235881
class spell_domatrax_portals : public SpellScriptLoader
{
public:
    spell_domatrax_portals() : SpellScriptLoader("spell_domatrax_portals") { }

    class spell_domatrax_portals_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_domatrax_portals_AuraScript);

        void OnTick(AuraEffect const* /*aurEff*/)
        {
            Unit* caster = GetCaster();
            if (!caster || caster->GetEntry() != NPC_PORTAL_OWNER)
                return;

            Unit* owner = caster->GetOwner();
            if (!owner || owner->GetEntry() != BOSS_DOMATRAX)
                return;

            owner->SummonCreature(GetSpellInfo()->Id == SPELL_PORTAL_PERIODIC_ONE ? NPC_IMP : (caster->GetMap()->GetDifficultyID() == DIFFICULTY_HEROIC ? NPC_FELGUARD : NPC_SHIVARRA), caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), caster->GetOrientation());
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_domatrax_portals_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_domatrax_portals_AuraScript();
    }
};



void AddSC_boss_domatrax()
{
    new boss_domatrax();
    new npc_coen_egida();
    new areatrigger_domatrax_egida_shield();
    new spell_domatrax_portals();
}
