#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Zone_BrackenhideHollow.h"

enum Spells
{
    SPELL_CONSUME = 377222,
    SPELL_DECAY_SPRAY = 376811,
    SPELL_VINE_WHIP = 377559,
    SPELL_GRASPING_VINES = 376934,
    SPELL_DECAY_SPRAY_SUMMON = 376797,//summon 1 slime need set to 4 
    SPELL_GUSHING_OOZE = 381770,//used by npc slime
    SPELL_BURST = 378057, //used by npc slime
};

enum Events
{
    EVENT_CONSUME = 1,
    EVENT_DECAY_SPRAY,
    EVENT_VINE_WHIP,
    EVENT_GRASPING_VINES,
    EVENT_DECAY_SPRAY_SUMMON,
    EVENT_RESET,
};

enum TreemouthNpcs
{
    NPC_DECAY_SLIME = 192481
};

enum GraspingVines
{
    DRAG_DURATION = 4000,
};

class boss_treemouth : public CreatureScript
{
public:
    boss_treemouth() : CreatureScript("boss_treemouth") {}

    struct boss_treemouthAI : public ScriptedAI
    {
        boss_treemouthAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            events.Reset();
            me->RemoveAurasDueToSpell(SPELL_VINE_WHIP);
        }

        void JustEngagedWith(Unit* who) override
        {
            ScriptedAI::JustEngagedWith(who);
            events.ScheduleEvent(EVENT_CONSUME, 6s);
            events.ScheduleEvent(EVENT_DECAY_SPRAY, 10s);
            events.ScheduleEvent(SPELL_VINE_WHIP, 15s);
            events.ScheduleEvent(EVENT_DECAY_SPRAY_SUMMON, 13s);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 1);
        }

        void JustDied(Unit* /*killer*/)
        {
            events.Reset();
        }

        void EnterEvadeMode(EvadeReason /*why*/) override
        {
            summons.DespawnAll();
            _EnterEvadeMode();
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CONSUME:
                    DoCastVictim(SPELL_CONSUME);
                    events.ScheduleEvent(EVENT_CONSUME, 6s);
                    break;
                case EVENT_DECAY_SPRAY:
                    DoCast(me->GetVictim(), SPELL_DECAY_SPRAY);
                    events.ScheduleEvent(EVENT_DECAY_SPRAY, 10s);
                    break;
                case SPELL_VINE_WHIP:
                    DoCastVictim(SPELL_VINE_WHIP);
                    events.ScheduleEvent(SPELL_VINE_WHIP, 15s);
                    break;
                case EVENT_DECAY_SPRAY_SUMMON:
                    DoCastVictim(SPELL_DECAY_SPRAY_SUMMON);
                    events.ScheduleEvent(SPELL_DECAY_SPRAY_SUMMON, 13s);
                    break;
                }
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_treemouthAI(creature);
    }
};

//npc=192481 decay slime
class npc_decaying_slime : public CreatureScript
{
public:
    npc_decaying_slime() : CreatureScript("npc_decaying_slime") {}

    struct npc_decaying_slimeAI : public ScriptedAI
    {
        npc_decaying_slimeAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoZoneInCombat();
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->HealthBelowPct(50))
            {
                DoCast(me->GetVictim(), SPELL_GUSHING_OOZE);
            }
            else
            {
                DoCast(me, SPELL_BURST);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_decaying_slimeAI(creature);
    }
};

//377222
class spell_consume : public AuraScript
{
    PrepareAuraScript(spell_consume);
    
    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
        {
            // Apply damage every 1.5 seconds
            uint32 tickDamage = 40631;
            GetTarget()->DealDamage(caster, GetTarget(), tickDamage, 0);
        }
    }

    void HandleAuraApplied(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
        {
            Unit* target = GetTarget();

            if (!target || !target->IsPlayer())
                return;

            if (aurEff->GetEffIndex() == 0) // First effect index is the absorb
            {
                // Set up absorb damage
                uint32 absorbedDamage = 218877;
            }
        }
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_consume::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

//376811 
class spell_decay_spray : public SpellScriptLoader
{
public:
    spell_decay_spray() : SpellScriptLoader("spell_decay_spray") {}

    class spell_decay_spray_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_decay_spray_AuraScript);

        void HandleTick(AuraEffect const* aurEff)
        {
            if (Unit* caster = GetCaster())
            {
                // Check if the caster is a creature (NPC) and not a player
                if (caster->IsCreature() && !caster->ToCreature()->GetVictim())
                {
                    // Find all players within the cone in front of the caster
                    std::list<Player*> players;
                    caster->SelectNearbyTarget();

                    for (Player* player : players)
                    {
                        // Inflict nature damage to each player
                        caster->DealDamage(caster, player, aurEff->GetAmount(), 0);
                    }
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_decay_spray_AuraScript::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_decay_spray_AuraScript();
    }
};

//376934
class spell_grasping_vines : public SpellScript
{
    PrepareSpellScript(spell_grasping_vines);

    void HandleSpellEffect(const SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> players;
            caster->GetPlayerListInGrid(players, 100.0f); // Adjust the range as needed

            // Drag each player towards the caster
            for (Player* player : players)
            {
                if (player->IsAlive())
                {
                    // Apply the drag effect
                    player->SetDisableGravity(true);
                    player->m_movementInfo.AddMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);

                    // Schedule the player to be consumed after the drag duration
                    player->AddDelayedEvent(DRAG_DURATION, [this, player]()
                        {
                        if (player->IsAlive())
                        {
                            // Stop dragging and apply the consume effect
                            player->SetDisableGravity(false);
                            player->m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
                            player->CastSpell(player, SPELL_CONSUME, true);
                        }
                        });
                }
            }
        }
    }
    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_grasping_vines::HandleSpellEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_boss_treemouth()
{
    new boss_treemouth();
    new npc_decaying_slime();
    new spell_consume();
    new spell_decay_spray();
    RegisterSpellScript(spell_grasping_vines);
}
