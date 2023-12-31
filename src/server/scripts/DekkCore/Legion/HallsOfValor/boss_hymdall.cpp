#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "halls_of_valor.h"

enum Says
{
    SAY_AGGRO = 0,
    SAY_HORN = 1,
    SAY_HORN_EMOTE = 2,
    SAY_BLOODLETTING = 3,
    SAY_DEATH = 4,
};

enum Spells
{
    SPELL_HORN_OF_VALOR = 191284,
    SPELL_DANCING_BLADE = 193235,
    SPELL_DANCING_BLADE_AT = 193214,
    SPELL_BLOODLETTING_SWEEP = 193092,

    //Heroic
    SPELL_BALL_LIGHTNING_AT_FRONT = 193240,
    SPELL_BALL_LIGHTNING_AT_BACK = 188405,

    //Drake
    SPELL_STORM_BREATH_VIS = 188404,
    SPELL_STORM_BREATH_AT = 192959,
};

enum eEvents
{
    EVENT_HORN_OF_VALOR = 1,
    EVENT_SUM_S_DRAKE = 2,
    EVENT_DANCING_BLADE = 3,
    EVENT_BLOODLETTING_SWEEP = 4,

    EVENT_1,
    EVENT_2,
};

Position const drakePos[6] =
{
    {3566.12f, 675.0f,  626.44f, 4.67f},
    {3566.12f, 383.00f, 626.44f, 1.60f},
    {3534.45f, 675.02f, 626.44f, 4.67f},
    {3534.45f, 383.00f, 626.44f, 1.60f},
    {3502.45f, 675.04f, 626.44f, 4.67f},
    {3502.45f, 383.00f, 626.44f, 1.60f}
};

//94960
class boss_hymdall : public CreatureScript
{
public:
    boss_hymdall() : CreatureScript("boss_hymdall") {}

    struct boss_hymdallAI : public BossAI
    {
        boss_hymdallAI(Creature* creature) : BossAI(creature, DATA_HYMDALL)
        {
            encounterComplete = false;
        }

        bool encounterComplete;
        uint8 drakeCount;
        uint8 randDrakePos;

        void Reset() override
        {
            if (encounterComplete)
                return;

            _Reset();
            drakeCount = 0;
            randDrakePos = 0;
        }

        void JustEngagedWith(Unit* who) override
        {
            if (encounterComplete)
                return;

            Talk(SAY_AGGRO); //I will be the judge now.
            _JustEngagedWith(who);
            events.RescheduleEvent(EVENT_HORN_OF_VALOR, 6s);
            events.RescheduleEvent(EVENT_DANCING_BLADE, 15s);
            events.RescheduleEvent(EVENT_BLOODLETTING_SWEEP, 20s);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);

            if (summon->GetEntry() == NPC_DANCING_BLADE)
            {
                summon->SetReactState(REACT_PASSIVE);
                summon->CastSpell(summon, SPELL_DANCING_BLADE_AT, true);
            }
        }

        void JustReachedHome() override
        {
            if (encounterComplete)
            {
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                me->DespawnOrUnsummon(5s);
            }
        }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (me->HealthBelowPct(11))
            {
                if (!encounterComplete)
                {
                    encounterComplete = true;
                    me->AttackStop();
                    Talk(SAY_DEATH);
                    _JustDied();
                    instance->DoUpdateCriteria(CriteriaType::DefeatDungeonEncounter, 1805);
                    instance->DoUpdateCriteria(CriteriaType::KillCreature, me->GetEntry(), 1, 0);
                    me->SetFaction(35);
                    me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetUnitFlag(UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->CombatStop();
                    me->GetMotionMaster()->MoveTargetedHome();
                }
                damage = 0;
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
                case EVENT_HORN_OF_VALOR:
                {
                    Talk(SAY_HORN);
                    Talk(SAY_HORN_EMOTE);
                    DoCast(SPELL_HORN_OF_VALOR);
                    if (drakeCount < 3)
                        drakeCount++;
                    randDrakePos = urand(0, 5);
                    for (uint8 i = 0; i < drakeCount; i++)
                        events.RescheduleEvent(EVENT_SUM_S_DRAKE, 5s);
                    events.RescheduleEvent(EVENT_HORN_OF_VALOR, 30s);
                    break;
                }
                case EVENT_SUM_S_DRAKE:
                    me->SummonCreature(NPC_STORM_DRAKE, drakePos[randDrakePos]);
                    randDrakePos += 2;
                    if (randDrakePos > 5)
                        randDrakePos = 0;
                    break;
                case EVENT_DANCING_BLADE:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 80.0f, true))
                        DoCast(target, SPELL_DANCING_BLADE);
                    events.RescheduleEvent(EVENT_DANCING_BLADE, 11s);
                    break;
                case EVENT_BLOODLETTING_SWEEP:
                    DoCastVictim(SPELL_BLOODLETTING_SWEEP, true);
                    Talk(SAY_BLOODLETTING);
                    events.RescheduleEvent(EVENT_BLOODLETTING_SWEEP, 26s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_hymdallAI(creature);
    }
};

//97788
class npc_hymdall_storm_drake : public CreatureScript
{
public:
    npc_hymdall_storm_drake() : CreatureScript("npc_hymdall_storm_drake") { }

    struct npc_hymdall_storm_drakeAI : public ScriptedAI
    {
        npc_hymdall_storm_drakeAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetCanFly(true);
            me->SetSpeed(MOVE_FLIGHT, 3.5f);
        }

        EventMap events;

        void Reset() override {}

        void IsSummonedBy(WorldObject* summoner) override
        {
            events.RescheduleEvent(EVENT_1, 3s);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            events.Reset();
            me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                {
                    DoCast(SPELL_STORM_BREATH_VIS);
                    float x, y, z;
                    me->GetClosePoint(x, y, z, me->GetCombatReach(), 220.0f);
                    me->GetMotionMaster()->MovePoint(1, x, y, z, false);
                    events.RescheduleEvent(EVENT_2, 1s);
                    break;
                }
                case EVENT_2:
                    if (Creature* staticField = me->FindNearestCreature(NPC_STATIC_FIELD, 15.0f))
                        if (!staticField->HasAura(SPELL_STORM_BREATH_AT))
                        {
                            staticField->CastSpell(staticField, SPELL_STORM_BREATH_AT, true);

                            if (me->GetMap()->GetDifficultyID() != DIFFICULTY_NORMAL)
                            {
                                staticField->CastSpell(staticField, SPELL_BALL_LIGHTNING_AT_FRONT, true);
                                staticField->CastSpell(staticField, SPELL_BALL_LIGHTNING_AT_BACK, true);
                            }
                        }
                    events.RescheduleEvent(EVENT_2, 2s);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_hymdall_storm_drakeAI(creature);
    }
};

// trash 198599
class spell_thunderstrike : public SpellScriptLoader
{
public:
    spell_thunderstrike() : SpellScriptLoader("spell_thunderstrike") { }

    class spell_thunderstrike_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_thunderstrike_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
                if (Creature* caster = target->FindNearestCreature(95842, 40.0f, true))
                {
                    caster->CastSpell(target, 198605, true);
                }
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_thunderstrike_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_thunderstrike_AuraScript();
    }
};

void AddSC_boss_hymdall()
{
    new boss_hymdall();
    new npc_hymdall_storm_drake();
    new spell_thunderstrike();
}
