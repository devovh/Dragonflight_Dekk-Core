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
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "TemporarySummon.h"

enum TredovaSpells
{
    SPELL_CONSUMPTION                       = 322450,
    SPELL_CONSUMPTION_INTERRUPT_AURA        = 322535,
    SPELL_CONSUMPTION_REMOVE_INTERRUPT_AURA = 322535,
    SPELL_GORGING_SHIELD                    = 322527,
    SPELL_ANIMA_REJECTION                   = 322465,
    SPELL_ANIMA_SHEDDING                    = 322526,

    SPELL_ACCELERATED_INCUBATION            = 322550,
    SPELL_ACCELERATED_INCUBATION_SUMMON     = 322551,

    SPELL_DECOMPOSITION_POOL                = 326308,
    SPELL_DECOMPOSING_ACID                  = 326309,

    SPELL_MIND_LINK                         = 322614,
    SPELL_MIND_LINK_MAIN                    = 322648,
    SPELL_MIND_LINK_TARGET                  = 322649,
    SPELL_MIND_LINK_LINK                    = 331172,
    SPELL_MIND_LINK_REMOVE                  = 331432,

    SPELL_MARKED_PREY                       = 322563,
    SPELL_MARKED_PREY_FIXATE                = 322572,

    SPELL_ACID_EXPULSION                    = 322651,
    SPELL_ACID_EXPULSION_MISSILE            = 322654,
    SPELL_ACID_EXPULSION_AREATRIGGER        = 322706,
    SPELL_DIGESTIVE_ACID                    = 323851,

    SPELL_CONVERSATION_DEFEAT               = 331599,
};

std::vector<Position> incubationSummonPositions = {
    { -7389.007813f, 2887.488770f, 5341.211426f, 3.527797f },
    { -7346.084961f, 2898.540039f, 5341.338867f, 0.971325f },
    { -7322.211426f, 2853.929688f, 5342.281738f, 6.123530f },
    { -7334.873535f, 2806.480469f, 5343.364258f, 5.785802f },
    { -7356.692871f, 2784.939941f, 5341.754883f, 5.008254f },
    { -7385.641113f, 2787.020020f, 5341.660645f, 4.411346f },
    { -7430.945313f, 2800.632324f, 5345.967773f, 3.457085f },
    { -7436.810059f, 2864.750244f, 5340.751465f, 2.224009f },
};

 // 164517
struct boss_tredova : public BossAI
{
    boss_tredova(Creature* creature) : BossAI(creature, DATA_TREDOVA)
    {
        ApplyDefaultBossImmuneMask();
    }

    void ScheduleTasks() override
    {
        Talk(4);

        events.ScheduleEvent(SPELL_CONSUMPTION, 20s);
        events.ScheduleEvent(SPELL_ACCELERATED_INCUBATION, 10s);
        events.ScheduleEvent(SPELL_MARKED_PREY, 20s);
        events.ScheduleEvent(SPELL_MIND_LINK, 40s);
        events.ScheduleEvent(SPELL_ACID_EXPULSION, 10s, 20s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case SPELL_CONSUMPTION:
                Talk(0);
                if (Unit* lakali = instance->GetCreature(NPC_COCOON_OF_LAKALI))
                    DoCast(lakali, SPELL_CONSUMPTION);

                events.ScheduleEvent(SPELL_CONSUMPTION, 40s);
                break;
            case SPELL_ACCELERATED_INCUBATION:
                Talk(2);
                DoCastAOE(SPELL_ACCELERATED_INCUBATION);
                events.ScheduleEvent(SPELL_ACCELERATED_INCUBATION, 40s);
                events.ScheduleEvent(SPELL_MARKED_PREY, 50s);
                break;
            case SPELL_MIND_LINK:
                DoCastRandom(SPELL_ACCELERATED_INCUBATION, 0.f);
                events.ScheduleEvent(SPELL_MIND_LINK, 30s, 40s);
                break;
            case SPELL_MARKED_PREY:
                DoCastRandom(SPELL_MARKED_PREY, 0.f, false, 0, 1);
                break;
            case SPELL_ACID_EXPULSION:
                Talk(1);
                DoCastAOE(SPELL_ACID_EXPULSION);
                events.ScheduleEvent(SPELL_ACID_EXPULSION, 10s, 20s);
                break;
        }
    }

   void OnSpellCast(SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
            case SPELL_CONSUMPTION:
                DoCastAOE(SPELL_CONSUMPTION_REMOVE_INTERRUPT_AURA);
                break;
            case SPELL_ACCELERATED_INCUBATION:
                for (uint8 i = 0; i < 4; ++i)
                {
                    Position summonPosition = Trinity::Containers::SelectRandomContainerElement(incubationSummonPositions);
                    me->CastSpell(summonPosition, SPELL_ACCELERATED_INCUBATION_SUMMON, true);
                }
                break;
            case SPELL_ACID_EXPULSION:
                instance->DoOnPlayers([this](Player* player)
                {
                    me->CastSpell(player, SPELL_ACID_EXPULSION_MISSILE, true);

                    if (GetDifficulty() == Difficulty::DIFFICULTY_MYTHIC) {
                        me->GetScheduler().Schedule(3s, [this, player](TaskContext /*context*/)
                        {
                            me->CastSpell(player, SPELL_ACID_EXPULSION_AREATRIGGER, true);
                        });
                    }
                });
                break;
        }
    }

    void JustDied(Unit* attacker) override
    {
        BossAI::JustDied(attacker);

        if (Creature* droman = me->SummonCreature(NPC_DROMAN_AT_TREDOVA_END, -7336.600098f, 2828.360107f, 5342.750000f, 2.891950f))
            droman->GetMotionMaster()->MovePoint(0, { -7372.386230f, 2841.078369f, 5341.208984f, 2.830375f });

        instance->DoCastSpellOnPlayers(SPELL_CONVERSATION_DEFEAT);
    }
};

// 165108
struct npc_tredova_gormling_larva : public ScriptedAI
{
    npc_tredova_gormling_larva(Creature* c) : ScriptedAI(c) { }

    void JustDied(Unit* /*attacker*/) override
    {
        DoCastAOE(SPELL_DECOMPOSITION_POOL, true);
    }
};

// 322450
class aura_tredova_consumption : public AuraScript
{
    PrepareAuraScript(aura_tredova_consumption);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(nullptr, SPELL_ANIMA_REJECTION, true);
            caster->CastSpell(nullptr, SPELL_ANIMA_SHEDDING, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_tredova_consumption::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 322527
class aura_tredova_gorging_shield : public AuraScript
{
    PrepareAuraScript(aura_tredova_gorging_shield);

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(nullptr, SPELL_CONSUMPTION_INTERRUPT_AURA, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(aura_tredova_gorging_shield::AfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 322648
class aura_tredova_mind_link : public AuraScript
{
    PrepareAuraScript(aura_tredova_mind_link);

    void OnAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        auto linkedApplications = GetTarget()->GetTargetAuraApplications(SPELL_MIND_LINK_LINK);
        for (AuraApplication* linkedApplication : linkedApplications)
        {
            if (Unit* target = linkedApplication->GetTarget())
            {
                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_MIND_LINK_MAIN, caster->GetMap()->GetDifficultyID()))
                {
                    SpellNonMeleeDamage damageInfo(caster, target, spellInfo, { spellInfo->GetSpellXSpellVisualId(caster),0 }, SPELL_SCHOOL_MASK_SHADOW);
                    damageInfo.damage = dmgInfo.GetDamage();
                    caster->DealDamageMods(caster, damageInfo.target, damageInfo.damage, &damageInfo.absorb);
                    target->DealSpellDamage(&damageInfo, true);
                    target->SendSpellNonMeleeDamageLog(&damageInfo);
                }
            }
        }

        absorbAmount = 0;
    }

    void Register() override
    {
        OnEffectAbsorb += AuraEffectAbsorbFn(aura_tredova_mind_link::OnAbsorb, EFFECT_1);
    }
};

// 322563
class aura_tredova_marked_prey : public AuraScript
{
    PrepareAuraScript(aura_tredova_marked_prey);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            if (GuidList* gormlingLarvaGuids = caster->GetSummonList(NPC_GORMLING_LARVA))
                for (ObjectGuid gormlingLarvaGuid : *gormlingLarvaGuids)
                    if (Creature* gormlingLarva = caster->GetMap()->GetCreature(gormlingLarvaGuid))
                        gormlingLarva->CastSpell(GetTarget(), SPELL_MARKED_PREY_FIXATE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_tredova_marked_prey::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Spell 326308, AT 24096
struct at_tredova_decomposition_pool : public AreaTriggerAI
{
    at_tredova_decomposition_pool(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (target->IsPlayer())
            target->CastSpell(target, SPELL_DECOMPOSING_ACID, false);
    }
};

// Spell 322648, AT 24507
struct at_tredova_mind_link : public AreaTriggerAI
{
    at_tredova_mind_link(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnCreate() override
    {
        if (Unit* caster = at->GetCaster())
            for (ObjectGuid guid : at->GetInsidePlayers())
                if (guid != caster->GetGUID())
                    if (Player* targetPlayer = caster->GetMap()->GetPlayer(guid))
                        caster->CastSpell(targetPlayer, SPELL_MIND_LINK_LINK, true);
    }

    void OnUnitExit(Unit* target) override
    {
        if (at->GetInsidePlayers().size() == 1)
            if (Unit* caster = at->GetCaster())
                caster->RemoveAurasDueToSpell(SPELL_MIND_LINK_MAIN);

        target->RemoveAurasDueToSpell(SPELL_MIND_LINK_LINK);
    }
};

// Spell 322706, AT 23631
struct at_tredova_acid_expulsion : public AreaTriggerAI
{
    at_tredova_acid_expulsion(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        target->CastSpell(target, SPELL_DIGESTIVE_ACID, true);
    }

    void OnUnitExit(Unit* target) override
    {
        target->RemoveAurasDueToSpell(SPELL_DIGESTIVE_ACID);
    }
};

void AddSC_boss_tredova()
{
    RegisterCreatureAI(boss_tredova);
    RegisterCreatureAI(npc_tredova_gormling_larva);
    RegisterAuraScript(aura_tredova_consumption);
    RegisterAuraScript(aura_tredova_gorging_shield);
    RegisterAuraScript(aura_tredova_mind_link);
    RegisterAuraScript(aura_tredova_marked_prey);
    RegisterAreaTriggerAI(at_tredova_decomposition_pool);
    RegisterAreaTriggerAI(at_tredova_mind_link);
    RegisterAreaTriggerAI(at_tredova_acid_expulsion);
}
