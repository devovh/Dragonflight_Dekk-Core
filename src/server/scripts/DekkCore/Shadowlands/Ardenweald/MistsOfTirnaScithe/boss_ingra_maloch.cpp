/*
 * Copyright 2021 AshamaneCore
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

#include "mists_of_tirna_scithe.h"
#include "Creature.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "ScriptMgr.h"

enum IngraMalochSpells
{
    // Ingra Maloch
    SPELL_SOUL_SHACKLE_PRE_FIGHT_INTRO          = 321010,

    SPELL_SOUL_SHACKLE_VISUAL                   = 321005,
    SPELL_SOUL_SHACKLE_EFFECT                   = 321006,
    SPELL_FORCE_COMPLIANCE                      = 323138,
    SPELL_SPIRIT_BOLT                           = 323057,

    SPELL_EMBRACE_DARKNESS                      = 323149,
    SPELL_EMBRACE_DARKNESS_DAMAGE               = 325697,
    SPELL_DEATH_SHROUD                          = 323146,

    SPELL_REPULSIVE_VISAGE                      = 328756,

    // Droman Oulfarran
    SPELL_BEWILDERING_POLLEN                    = 323137,

    SPELL_TEARS_OF_THE_FOREST                   = 323177,
    SPELL_TEARS_OF_THE_FOREST_MISSILE_PLAYER    = 323197,
    SPELL_TEARS_OF_THE_FOREST_MISSILE_RANDOM    = 323202,
    SPELL_ANIMA_PUDDLE_DAMAGE                   = 323250,

    SPELL_WEAKENED_RESOLVE                      = 331440,

    SPELL_DROMAN_WRATH                          = 323059,
};

// 166644
struct boss_ingra_maloch : public BossAI
{
    boss_ingra_maloch(Creature* creature) : BossAI(creature, DATA_INGRA_MALOCH)
    {
        ApplyDefaultBossImmuneMask();
    }

    Creature* GetDromanOulfarran() { return instance->GetCreature(NPC_DROMAN_OULFARRAN); }

    void Reset() override
    {
        BossAI::Reset();

        if (!_introDone)
        {
            me->GetScheduler().Schedule(5s, [this](TaskContext context)
            {
                if (Creature* droman = GetDromanOulfarran())
                {
                    me->SetFacingToObject(droman);
                    me->CastSpell(droman, SPELL_SOUL_SHACKLE_PRE_FIGHT_INTRO, false);
                }
            });

            _introDone = true;
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Creature* droman = GetDromanOulfarran())
        {
            if (droman->HasAura(SPELL_SOUL_SHACKLE_PRE_FIGHT_INTRO))
            {
                if (me->GetDistance(who) < 90.f)
                {
                    droman->RemoveAurasDueToSpell(SPELL_SOUL_SHACKLE_PRE_FIGHT_INTRO);

                    std::vector<Creature*> drustBoughbreakers;
                    me->GetCreatureListWithEntryInGrid(drustBoughbreakers, 164926, 50.f);

                    for (Creature* drustBoughbreaker : drustBoughbreakers)
                    {
                        Position movePos = { -6948.976563f, 2026.847656f, 5526.767090f, 3.615172f };

                        if (drustBoughbreaker->GetPositionX() > -6910.f)
                            movePos = { -6946.854492f, 2011.383911f, 5526.072266f, 3.532707f };

                        drustBoughbreaker->GetMotionMaster()->MovePoint(0, movePos);
                    }
                }
            }
        }
    }

    void ScheduleTasks() override
    {
        Talk(0);

        events.ScheduleEvent(SPELL_SPIRIT_BOLT, 5s);
        events.ScheduleEvent(SPELL_FORCE_COMPLIANCE, 15s, 20s);
        events.ScheduleEvent(SPELL_EMBRACE_DARKNESS, 30s);
        events.ScheduleEvent(SPELL_REPULSIVE_VISAGE, 45s, 46s);
    }

    void JustEngagedWith(Unit* attacker) override
    {
        Creature* droman = GetDromanOulfarran();

        if (attacker != droman)
            _JustEngagedWith(attacker);

        if (droman)
            if (!droman->IsInCombat())
                droman->AI()->DoZoneInCombat();
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case SPELL_SPIRIT_BOLT:
                DoCastVictim(SPELL_SPIRIT_BOLT);
                events.ScheduleEvent(SPELL_SPIRIT_BOLT, 5s);
                break;
            case SPELL_FORCE_COMPLIANCE:
                if (Creature* droman = GetDromanOulfarran())
                    me->CastSpell(droman, SPELL_FORCE_COMPLIANCE, false);

                events.ScheduleEvent(SPELL_FORCE_COMPLIANCE, 15s, 20s);
                break;
            case SPELL_EMBRACE_DARKNESS:
                DoCastSelf(SPELL_EMBRACE_DARKNESS);
                break;
            case SPELL_REPULSIVE_VISAGE:
                DoCastAOE(SPELL_REPULSIVE_VISAGE);
                events.ScheduleEvent(SPELL_REPULSIVE_VISAGE, 45s, 46s);
                break;
        }
    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_DROMAN_WRATH)
            events.Reset();
    }

    void OnAuraRemoved(SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_DROMAN_WRATH)
        {
            if (Creature* droman = GetDromanOulfarran())
            {
                Talk(1);
                me->CastSpell(droman, SPELL_SOUL_SHACKLE_VISUAL, false);
                ScheduleTasks();
                events.DelayEvents(3s);
            }
        }
    }

    void JustDied(Unit* /*attacker*/) override
    {
        _JustDied();

        if (Creature* droman = GetDromanOulfarran())
            droman->AI()->DoAction(0);
    }

private:
    bool _introDone = false;
};

// 164567
struct npc_droman_oulfarran : public ScriptedAI
{
    npc_droman_oulfarran(Creature* c) : ScriptedAI(c)
    {
        ApplyDefaultBossImmuneMask();
    }

    Creature* GetIngraMaloch() { return instance->GetCreature(NPC_INGRA_MALOCH); }

    void Reset() override
    {
        if (Creature* ingra = GetIngraMaloch())
            if (ingra->IsAlive())
                me->AddAura(SPELL_SOUL_SHACKLE_EFFECT, me);
    }

    void JustEngagedWith(Unit* attacker) override
    {
        events.ScheduleEvent(SPELL_BEWILDERING_POLLEN, 15s, 20s);

        if (Creature* ingra = GetIngraMaloch())
            if (!ingra->IsInCombat())
                ingra->AI()->DoZoneInCombat();
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (Creature* ingra = GetIngraMaloch())
            if (ingra->IsInCombat())
                return;

        ScriptedAI::EnterEvadeMode(why);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        if (eventId == SPELL_BEWILDERING_POLLEN)
        {
            DoCastVictim(SPELL_BEWILDERING_POLLEN);
            events.ScheduleEvent(SPELL_BEWILDERING_POLLEN, 15s, 20s);
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPctDamaged(20, damage))
        {
            Talk(0);
            me->RemoveAurasDueToSpell(SPELL_SOUL_SHACKLE_EFFECT);
        }
    }

    void OnAuraRemoved(SpellInfo const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
            case SPELL_SOUL_SHACKLE_PRE_FIGHT_INTRO:
            case SPELL_SOUL_SHACKLE_VISUAL:
                if (Creature* ingra = GetIngraMaloch())
                    ingra->CastSpell(me, SPELL_SOUL_SHACKLE_EFFECT, true);

                me->SetFullHealth();
                break;
            case SPELL_SOUL_SHACKLE_EFFECT:
            {
                if (Creature* ingra = GetIngraMaloch())
                {
                    if (ingra->IsAlive())
                    {
                        AttackStart(ingra);
                        me->CastSpell(ingra, SPELL_DROMAN_WRATH, false);
                        me->RemoveAllAreaTriggers();
                    }
                }
                break;
            }
        }

    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_FORCE_COMPLIANCE)
        {
            DoCastSelf(SPELL_WEAKENED_RESOLVE);
            DoCastAOE(SPELL_TEARS_OF_THE_FOREST);
        }
    }

    void DoAction(int32 action) override
    {
        me->RemoveAurasDueToSpell(SPELL_SOUL_SHACKLE_EFFECT);
    }
};

// 323187
class spell_droman_oulfarran_tears_of_the_forest : public SpellScript
{
    PrepareSpellScript(spell_droman_oulfarran_tears_of_the_forest);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        InstanceScript* instanceScript = caster->GetInstanceScript();
        if (!instanceScript)
            return;

        uint32 missileCount = caster->GetAuraCount(SPELL_WEAKENED_RESOLVE);

        instanceScript->DoOnPlayers([caster, missileCount](Player* player)
        {
            caster->CastSpell(player, SPELL_TEARS_OF_THE_FOREST_MISSILE_PLAYER, true);

            for (uint8 i = 0; i < missileCount; ++i)
                caster->CastSpell(player, SPELL_TEARS_OF_THE_FOREST_MISSILE_RANDOM, true);
        });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_droman_oulfarran_tears_of_the_forest::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Spell 323149, AT 24338
struct at_ingra_maloch_embrace_darkness : public AreaTriggerAI
{
    at_ingra_maloch_embrace_darkness(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (Unit* caster = at->GetCaster())
            if (target->IsPlayer())
                caster->CastSpell(target, SPELL_DEATH_SHROUD, true);
    }

    void OnUnitExit(Unit* target) override
    {
        target->RemoveAurasDueToSpell(SPELL_DEATH_SHROUD);
    }
};

// Spell 323263, AT 19709
struct at_droman_oulfarran_anima_puddle : public AreaTriggerAI
{
    at_droman_oulfarran_anima_puddle(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (Unit* caster = at->GetCaster())
            if (target->IsPlayer())
                caster->CastSpell(target, SPELL_ANIMA_PUDDLE_DAMAGE, true);
    }

    void OnUnitExit(Unit* target) override
    {
        target->RemoveAurasDueToSpell(SPELL_ANIMA_PUDDLE_DAMAGE);
    }
};

void AddSC_boss_ingra_maloch()
{
    RegisterCreatureAI(boss_ingra_maloch);
    RegisterCreatureAI(npc_droman_oulfarran);
    RegisterSpellScript(spell_droman_oulfarran_tears_of_the_forest);

    RegisterAreaTriggerAI(at_ingra_maloch_embrace_darkness);
    RegisterAreaTriggerAI(at_droman_oulfarran_anima_puddle);
}
