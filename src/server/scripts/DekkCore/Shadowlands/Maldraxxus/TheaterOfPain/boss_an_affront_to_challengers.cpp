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
    SPELL_NECROMANTIC_BOLT = 320300,
    SPELL_SEARING_DEATH = 333231,
    SPELL_SEARING_DEATH_DAMAGE = 333292,
    SPELL_ONE_WITH_DEATH = 320293,
    SPELL_SPECTRAL_TRANSFERENCE = 320272,
    SPELL_NOXIOUS_SPORES_MISSILE = 324118,
    SPELL_NOXIOUS_SPORES_CREATE_AT = 320179, //23400
    SPELL_ENRAGE = 324085,
    SPELL_FIXATE = 326892,
    SPELL_MORTAL_STRIKE = 320069,
    SPELL_SLAM = 320063,
    SPELL_PLAGUE_BOLT = 320120,
    SPELL_GENETIC_ALTERATION = 320248,
    SPELL_NOXIOUS_SPORES_DAMAGE = 320180,
};

//164461,164451,164463
struct boss_an_affront_to_challengers : public BossAI
{
    boss_an_affront_to_challengers(Creature* c) : BossAI(c, DATA_AN_AFFRONT_TO_CHALLENGERS), deathCounter(0) { }

private:
    bool sathel30;
    bool dessia30;
    bool paceran30;
    uint8 deathCounter = 0;

    void Reset() override
    {
        switch (me->GetEntry())
        {
        case NPC_SATHEL_THE_ACCURSED:
            BossAI::Reset();
            me->SetPowerType(POWER_MANA);
            me->SetMaxPower(POWER_MANA, 100);
            me->SetPower(POWER_MANA, 100);
            sathel30 = false;
            break;

        case NPC_DESSIA_THE_DECAPITATOR:
            BossAI::Reset();
            me->SetPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 0);
            me->AddAura(AURA_OVERRIDE_POWER_COLOR_RAGE, me);
            dessia30 = false;
            break;

        case NPC_PACERAN_THE_VIRULENT:
            BossAI::Reset();
            me->SetPowerType(POWER_MANA);
            me->SetMaxPower(POWER_MANA, 100);
            me->SetPower(POWER_MANA, 100);
            paceran30 = false;
            break;
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        switch (me->GetEntry())
        {
        case NPC_SATHEL_THE_ACCURSED:
            _JustEngagedWith(who);
            Talk(0);
            events.ScheduleEvent(SPELL_NECROMANTIC_BOLT, 2s);
            events.ScheduleEvent(SPELL_SEARING_DEATH, 7s);
            break;

        case NPC_DESSIA_THE_DECAPITATOR:
            _JustEngagedWith(who);
            Talk(0);
            events.ScheduleEvent(SPELL_MORTAL_STRIKE, 3s);
            events.ScheduleEvent(SPELL_SLAM, 7s);
            break;

        case NPC_PACERAN_THE_VIRULENT:
            _JustEngagedWith(who);
            Talk(0);
            events.ScheduleEvent(SPELL_PLAGUE_BOLT, 2s);
            events.ScheduleEvent(SPELL_GENETIC_ALTERATION, 7s);
            break;
        }      
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        switch (me->GetEntry())
        {
        case NPC_SATHEL_THE_ACCURSED:
            if (me->HealthBelowPct(30) && !sathel30)
            {
                sathel30 = true;
                me->CastSpell(nullptr, SPELL_ONE_WITH_DEATH, false);
            }
            break;

        case NPC_DESSIA_THE_DECAPITATOR:
            if (me->HealthBelowPct(30) && !dessia30)
            {
                dessia30 = true;
                me->CastSpell(nullptr, SPELL_ENRAGE, true);
              //  if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                   // me->AddThreat(target, 100.0f, SPELL_SCHOOL_MASK_NORMAL);
            }
            break;

        case NPC_PACERAN_THE_VIRULENT:
            if (me->HealthBelowPct(30) && !paceran30)
            {
                paceran30 = true;
                events.ScheduleEvent(SPELL_NOXIOUS_SPORES_CREATE_AT, 1s);
            }
            break;
        }
    } 

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
        case SPELL_NECROMANTIC_BOLT:
            DoCastRandom(SPELL_NECROMANTIC_BOLT, 100.0f, false);
            events.Repeat(3s);
            break;

        case SPELL_SEARING_DEATH:
            DoCastRandom(SPELL_SEARING_DEATH, 100.0f, false);
            events.Repeat(15s);
            break;

        case SPELL_NOXIOUS_SPORES_CREATE_AT:
        {
            std::list<Player*> targetList;
            me->GetPlayerListInGrid(targetList, 100.0f);
            for (Player* targets : targetList)
            {
                me->CastSpell(targets->GetPosition(), SPELL_NOXIOUS_SPORES_MISSILE, true);
                me->CastSpell(targets->GetPosition(), SPELL_NOXIOUS_SPORES_CREATE_AT, true);
            }
            events.Repeat(8s);
            break;
        }

        case SPELL_PLAGUE_BOLT:
            DoCastRandom(SPELL_PLAGUE_BOLT, 100.0f, false);
            events.Repeat(3s);
            break;

        case SPELL_MORTAL_STRIKE:
            DoCastVictim(SPELL_MORTAL_STRIKE, false);
            events.Repeat(10s);
            break;

        case SPELL_SLAM:
            DoCastVictim(SPELL_SLAM, false);
            events.Repeat(13s, 15s);
            break;
        }
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        switch (me->GetEntry())
        {
        case NPC_SATHEL_THE_ACCURSED:
            _JustReachedHome();
            break;

        case NPC_DESSIA_THE_DECAPITATOR:
            _JustReachedHome();
            break;

        case NPC_PACERAN_THE_VIRULENT:
            _JustReachedHome();
            break;
        }
    }

    void JustDied(Unit* /*who*/) override
    {
        switch (me->GetEntry())
        {
        case NPC_SATHEL_THE_ACCURSED:
        case NPC_PACERAN_THE_VIRULENT:
            _JustDied();
            break;

        case NPC_DESSIA_THE_DECAPITATOR:
            _JustDied();
            if (GameObject* trapdoor = me->FindNearestGameObject(GO_NECRO_ARENA_TRAPDOOR, 150.0f))
            {
                trapdoor->SetGoState(GO_STATE_ACTIVE);
                if (Creature* trapdoorRift = me->FindNearestCreature(170703, 150.0f, true))
                    trapdoorRift->KillSelf();
            }
            break;
        }
    }
};

//320248
class aura_genetic_alteration : public AuraScript
{
    PrepareAuraScript(aura_genetic_alteration);

    void OnTick(AuraEffect const* auraEff)
    {
        if (GetCaster() && GetTarget())
            GetCaster()->CastSpell(GetTarget()->GetPosition(), SPELL_NOXIOUS_SPORES_CREATE_AT, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_genetic_alteration::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

//23400
struct at_noxious_spores : public AreaTriggerAI
{
    at_noxious_spores(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (!target->HasAura(SPELL_NOXIOUS_SPORES_DAMAGE))
            at->GetCaster()->AddAura(SPELL_NOXIOUS_SPORES_DAMAGE, target);
    }

    void OnUnitExit(Unit* target) override
    {
        if (target->HasAura(SPELL_NOXIOUS_SPORES_DAMAGE))
            target->RemoveAura(SPELL_NOXIOUS_SPORES_DAMAGE);
    }
};

void AddSC_boss_an_affront_to_challengers()
{
    RegisterCreatureAI(boss_an_affront_to_challengers);
    RegisterAuraScript(aura_genetic_alteration);
    RegisterAreaTriggerAI(at_noxious_spores);
}
