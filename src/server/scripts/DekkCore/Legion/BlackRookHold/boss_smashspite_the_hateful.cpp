#include "black_rook_hold.h"
#include "TemporarySummon.h"

enum Says
{
    SAY_AGGRO = 0,
    SAY_HATEFUL_GAZE = 1,
    SAY_WARN_HATEFUL_GAZE = 2,
    SAY_HATEFUL_GAZE_CHARGE = 3,
    SAY_EARTHSHAKING_STOMP = 4,
    SAY_BRUTAL_HAYMAKER = 5,
    SAY_DEATH = 6,
};

enum Spells
{
    SPELL_ZERO_REGEN = 72242,
    SPELL_BRUTALITY_PROC = 198114,
    SPELL_EARTHSHAKING_STOMP = 198073,
    SPELL_BRUTAL_HAYMAKER = 198245,

    //Heroic
    SPELL_HATEFUL_GAZE = 198079,
    SPELL_HATEFUL_CHARGE = 198250,
    SPELL_HATEFUL_CHARGE_DMG = 198080,
    SPELL_HATEFUL_CHARGE_DEBUF = 224188,

    SPELL_FEL_VOMIT = 198446,
    SPELL_FEL_VOMIT_DMG = 198499,

    //Other
    SPELL_SIC_BATS = 203163,
};

enum eEvents
{
    EVENT_EARTHSHAKING_STOMP = 1,
    EVENT_FEL_ATTACK = 2,
    EVENT_HATEFUL_GAZE = 3,
};

Position const felPos[10] =
{
    {3377.12f, 7379.05f, 260.46f}, //100759
    {3405.04f, 7360.28f, 250.10f},
    {3410.22f, 7311.27f, 250.10f},
    {3392.56f, 7317.72f, 260.46f},
    {3379.99f, 7295.96f, 268.62f},
    {3388.76f, 7284.25f, 237.00f},
    {3257.77f, 7188.85f, 255.80f},
    {3352.13f, 7259.52f, 251.29f},
    {3206.34f, 7261.78f, 255.80f},
    {3267.14f, 7182.61f, 244.89f}
};

Position const centrPos = { 3269.27f, 7289.28f, 231.47f };

//98949
struct boss_smashspite_the_hateful : public BossAI
{
    boss_smashspite_the_hateful(Creature* creature) : BossAI(creature, DATA_SMASHSPITE) {}

    ObjectGuid felbatContainer[10];
    std::list<uint32> felTimer;

    void Reset() override
    {
        _Reset();
        DoCast(me, SPELL_ZERO_REGEN, true);
        DoCast(me, SPELL_BRUTALITY_PROC, true);
        me->SetPower(POWER_ENERGY, 0);
        me->SetReactState(REACT_AGGRESSIVE);

        for (uint8 i = 0; i < 10; ++i)
        {
            if (auto summon = me->SummonCreature(NPC_FEL_BAT, felPos[i]))
                felbatContainer[i] = summon->GetGUID();
        }
    }

    void JustEngagedWith(Unit* who) //override
    {
      _JustEngagedWith(who);
        Talk(SAY_AGGRO);
        felTimer = { 25000, 18000, 14000, 8000 };
        events.RescheduleEvent(EVENT_EARTHSHAKING_STOMP, 12s);
        events.RescheduleEvent(EVENT_FEL_ATTACK, 25s);
        if (me->GetMap()->GetDifficultyID() != DIFFICULTY_LFR && me->GetMap()->GetDifficultyID() != DIFFICULTY_NORMAL)
            events.RescheduleEvent(EVENT_HATEFUL_GAZE, 6s);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
        Talk(SAY_DEATH);
        DoCast(me, 205271, true);

        instance->instance->SummonCreature(102094, Position(3191.87f, 7364.25f, 223.95f, 5.36f));
        instance->instance->SummonCreature(102095, Position(3196.1f, 7367.85f, 223.96f, 5.38f));
    }

    void SpellHitTarget(WorldObject* target, const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_HATEFUL_CHARGE_DMG)
        {
            DoCast(target->ToUnit(), SPELL_HATEFUL_CHARGE_DEBUF, true);
            me->StopMoving();
            me->AttackStop();
            me->SetReactState(REACT_AGGRESSIVE);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id == SPELL_HATEFUL_CHARGE)
            me->SetReactState(REACT_AGGRESSIVE);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (me->GetDistance(centrPos) >= 30.0f)
        {
            EnterEvadeMode();
            return;
        }

        if (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_EARTHSHAKING_STOMP:
                Talk(SAY_EARTHSHAKING_STOMP);
                DoCast(SPELL_EARTHSHAKING_STOMP);
                events.RescheduleEvent(EVENT_EARTHSHAKING_STOMP, 17s);
                break;
            case EVENT_FEL_ATTACK:
                if (auto fel = me->GetCreature(*me, felbatContainer[urand(0, 9)]))
                    fel->AI()->DoAction(true);
                if (me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC || me->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_KEYSTONE)
                {
                    if (felTimer.size() > 1)
                        felTimer.pop_front();
                    events.RescheduleEvent(EVENT_FEL_ATTACK, 10s);
                }
                else
                    events.RescheduleEvent(EVENT_FEL_ATTACK, 25s);
                break;
            case EVENT_HATEFUL_GAZE:
                if (auto target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 100.0f, true))
                {
                    Talk(SAY_HATEFUL_GAZE);
                    Talk(SAY_WARN_HATEFUL_GAZE);
                    me->StopMoving();
                    me->AttackStop();
                    me->CastSpell(target, SPELL_HATEFUL_GAZE, false);
                    events.RescheduleEvent(EVENT_EARTHSHAKING_STOMP, 10s);
                }
                events.RescheduleEvent(EVENT_HATEFUL_GAZE, 25s);
                break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

//100759
struct npc_smashspite_fel_bat : public ScriptedAI
{
    npc_smashspite_fel_bat(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
    }

    void Reset() override {}

    void DoAction(int32 const action) override
    {
        me->CastSpell(me, SPELL_FEL_VOMIT);
    }
    void UpdateAI(uint32 diff) override {}
};

//102781
struct npc_brh_fel_bat : public ScriptedAI
{
    npc_brh_fel_bat(Creature* creature) : ScriptedAI(creature) {}

    ObjectGuid targetGUID;
    bool wpRun = false;
    uint32 despawnTimer = 0;
    uint32 findTargetTimer = 0;

    void Reset() override
    {
        me->SetReactState(REACT_AGGRESSIVE);
        targetGUID.Clear();

        if (me->IsSummon() && !wpRun)
        {
            me->GetMotionMaster()->MovePath(10278100, frand(-3.0f, 3.0f));
            despawnTimer = 60000;
        }
    }

    void SpellHit(WorldObject* caster, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_SIC_BATS && targetGUID.IsEmpty())
        {
            me->AttackStop();
            findTargetTimer = 200;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (despawnTimer)
        {
            if (despawnTimer <= diff)
            {
                despawnTimer = 0;

                if (!me->IsInCombat())
                    me->DespawnOrUnsummon();
                else
                    despawnTimer = 5000;
            }
            else
                despawnTimer -= diff;
        }

        if (findTargetTimer)
        {
            if (findTargetTimer <= diff)
            {
                findTargetTimer = 0;
                auto target = me->GetVictim();

                if (target && target->HasAura(SPELL_SIC_BATS))
                    findTargetTimer = 1000;
                else if (auto target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true, true, SPELL_SIC_BATS))
                {
                    targetGUID = target->GetGUID();
                    AttackStart(target);
                }
                else
                {
                    targetGUID.Clear();
                    me->SetReactState(REACT_AGGRESSIVE);
                }
            }
            else
                findTargetTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

//198114
class spell_smashpite_brutality_proc : public AuraScript
{
    PrepareAuraScript(spell_smashpite_brutality_proc);

    void OnProc(AuraEffect* aurEff, ProcEventInfo& eventInfo)
    {
        auto caster = GetCaster()->ToCreature();
        if (!caster || caster->HasUnitState(UNIT_STATE_CASTING))
            return;

        uint8 powerCount = caster->GetPower(POWER_ENERGY);

        if (powerCount < 100)
        {
            if (caster->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC|| caster->GetMap()->GetDifficultyID() == DIFFICULTY_MYTHIC_KEYSTONE)
                caster->SetPower(POWER_ENERGY, powerCount + 10);
            else
                caster->SetPower(POWER_ENERGY, powerCount + urand(6, 9));
        }
        else if (caster->GetReactState() == REACT_AGGRESSIVE && caster->GetVictim())
        {
            caster->AI()->Talk(SAY_BRUTAL_HAYMAKER);
            caster->CastSpell(caster->GetVictim(), SPELL_BRUTAL_HAYMAKER);
        }
    }

  

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_smashpite_brutality_proc::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_boss_smashspite_the_hateful()
{
    RegisterCreatureAI(boss_smashspite_the_hateful);
    RegisterCreatureAI(npc_smashspite_fel_bat);
    RegisterCreatureAI(npc_brh_fel_bat);
    RegisterSpellScript(spell_smashpite_brutality_proc);
    
}
