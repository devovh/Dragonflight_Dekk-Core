/*
 * Copyright 2022 BaldazzarCore-Shadowlands
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

#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "InstanceScript.h"
#include "theater_of_pain.h"

enum Spells
{
    SPELL_BRUTAL_COMBO_PERIODIC = 320644,
    SPELL_SUMMON_OPRESSIVE_BANNER = 331618,
    SPELL_OPRESSIVE_BANNER_VISUAL_APPLY_AT = 331603, //25045
    SPELL_OPRESSIVE_BANNER_AURA = 331606,
    SPELL_MIGHT_OF_MALDRAXXUS = 320050,
    SPELL_SEISMIC_LEAP_DAMAGE = 320789,
    SPELL_CRUSHING_SLAM = 317231,
    SPELL_MASSIVE_CLEAVE = 320729,
    SPELL_DEAFEANING_CRASH = 339415,
};

//162329
struct boss_xav_the_unfallen : public BossAI
{
    boss_xav_the_unfallen(Creature* c) : BossAI(c, DATA_XAV_THE_UNFALLEN) { }

    void Reset() override
    {
        BossAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        events.ScheduleEvent(SPELL_BRUTAL_COMBO_PERIODIC, 5s);
        events.ScheduleEvent(SPELL_SUMMON_OPRESSIVE_BANNER, 10s);
        events.ScheduleEvent(SPELL_MIGHT_OF_MALDRAXXUS, 15s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
        case SPELL_BRUTAL_COMBO_PERIODIC:
            DoCastVictim(SPELL_BRUTAL_COMBO_PERIODIC, false);
            events.Repeat(15s, 18s);
            break;

        case SPELL_SUMMON_OPRESSIVE_BANNER:
            me->SummonCreature(NPC_OPRESSIVE_BANNER, me->GetRandomNearPosition(15), TEMPSUMMON_MANUAL_DESPAWN);
            events.Repeat(22s, 25s);

         case SPELL_MIGHT_OF_MALDRAXXUS:
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 100.0f, true))
            {
                me->SetFacingToObject(target, true);
                me->CastSpell(target, SPELL_CRUSHING_SLAM, false);
                me->GetScheduler().Schedule(3s, [this, target](TaskContext context)
                {
                    if (me && target)
                    {
                        me->CastSpell(target, SPELL_MASSIVE_CLEAVE, false);
                        me->GetScheduler().Schedule(3s, [this, target](TaskContext context)
                        {
                            if (me)
                                me->CastSpell(nullptr, SPELL_DEAFEANING_CRASH, false);
                        });
                    }
                });
            } 
            events.Repeat(26s, 30s);
            break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() == NPC_OPRESSIVE_BANNER)
            summon->CastSpell(nullptr, SPELL_OPRESSIVE_BANNER_VISUAL_APPLY_AT, true);
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _JustReachedHome();
        me->RemoveAllAreaTriggers();
        _DespawnAtEvade();
    }

    void JustDied(Unit* /*victim*/) override
    {
        _JustDied();
        me->RemoveAllAreaTriggers();
    }
};

//320644
class aura_brutal_combo : public AuraScript
{
    PrepareAuraScript(aura_brutal_combo);

    void OnTick(AuraEffect const* auraEff)
    {
        if (GetCaster() && GetCaster()->GetVictim())
            return;
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_brutal_combo::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

//25045
struct at_opressive_banner : public AreaTriggerAI
{
    at_opressive_banner(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (!target->HasAura(SPELL_OPRESSIVE_BANNER_AURA))
            at->GetCaster()->CastSpell(target, SPELL_OPRESSIVE_BANNER_AURA, true);
    }

    void OnUnitExit(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->HasAura(SPELL_OPRESSIVE_BANNER_AURA))
            target->RemoveAura(SPELL_OPRESSIVE_BANNER_AURA);
    }
};

void AddSC_boss_xav_the_unfallen()
{
    RegisterCreatureAI(boss_xav_the_unfallen);
    RegisterAuraScript(aura_brutal_combo);
    RegisterAreaTriggerAI(at_opressive_banner);
}
