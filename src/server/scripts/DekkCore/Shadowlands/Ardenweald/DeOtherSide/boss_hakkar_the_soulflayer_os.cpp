/*
* Copyright 2021 Thordekk
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
#include "SpellAuraEffects.h"
#include "de_other_side.h"

enum Spells
{
    EVENT_SUMMON_SON_OF_HAKKAR = 1,
    EVENT_ENERGY_GAIN,
    SPELL_BLOOD_NOVA = 323544,
    SPELL_SPILLED_ESSENCE_AT_DAMAGE = 323569,
    SPELL_SPILLED_ESSENCE_APPLY_AT = 323567,
    SPELL_CORRUPTED_BLOOD = 322746,
    SPELL_BLOOD_BARRIER_CAST = 322759,
    SPELL_BLOOD_BARRIER_ABSORB = 322773,
    SPELL_BLOOD_BARRAGE_CREATE_AT_CHANNEL = 323064,
    SPELL_BLOOD_BARRAGE_MOD_DAMAGE_DONE = 323118,
    SPELL_PIERCING_BARB = 322736,
    SPELL_BLOOD_BARRAGE_MISSILE = 323111,
    EVENT_CORRUPTED_BLOOD,
    EVENT_BLOOD_BARRAGE_CREATE_AT_CHANNEL,
    EVENT_PIERCING_BARB,
    SPELL_CORRUPTED_BLOOD_2 = 323166,
};

#define AGGRO "Face the wrath of the Soulflayer!"
#define DIE "The power... was mine..."
#define BLOOD "Your blood sustains me!"
#define ENERGY "His power is mine! I will live again!"
#define SUMMON "Atal'ai! Rise in service of Hakkar!"

//164558
struct boss_hakkar_the_soulflayer_os : public BossAI
{
    boss_hakkar_the_soulflayer_os(Creature* c) : BossAI(c, DATA_HAKKAR_THE_SOULFLAYER) { }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        me->Yell(AGGRO, LANG_UNIVERSAL, NULL);
        _events.ScheduleEvent(EVENT_ENERGY_GAIN, 1s);
        _events.ScheduleEvent(EVENT_PIERCING_BARB, 3s);
        _events.ScheduleEvent(EVENT_SUMMON_SON_OF_HAKKAR, 15s);
        _events.ScheduleEvent(EVENT_CORRUPTED_BLOOD, 10s);
        _events.ScheduleEvent(EVENT_BLOOD_BARRAGE_CREATE_AT_CHANNEL, 5s);
    }

    void Reset() override
    {
        Talk(0);
        _events.Reset();
        me->SetPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 0);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())

        {
            switch (eventId)
            {
            case EVENT_ENERGY_GAIN:
                me->ModifyPower(POWER_ENERGY, urand(1, 5));
                me->Yell(ENERGY, LANG_UNIVERSAL, NULL);
                _events.Repeat(1s);
                break;

            case EVENT_PIERCING_BARB:
                DoCastVictim(SPELL_PIERCING_BARB);
                _events.Repeat(8s, 10s);
                break;

            case EVENT_SUMMON_SON_OF_HAKKAR:
                me->SummonCreature(NPC_SON_OF_HAKKAR, me->GetRandomNearPosition(30.0f), TEMPSUMMON_MANUAL_DESPAWN);
                DoCastVictim(344663);
                me->Yell(SUMMON, LANG_UNIVERSAL, NULL);
                _events.Repeat(20s, 30s);
                break;

            case EVENT_CORRUPTED_BLOOD:
                DoCastVictim(SPELL_CORRUPTED_BLOOD);
                DoCastVictim(SPELL_CORRUPTED_BLOOD_2);
                _events.Repeat(12s, 15s);
                break;
            }

            DoMeleeAttackIfReady();
        }
    }

    void OnSpellCast(SpellInfo const* spell) override
    {

        switch (spell->Id)
        {
        case SPELL_BLOOD_BARRIER_CAST:
            me->CastSpell(nullptr, SPELL_BLOOD_BARRIER_ABSORB, true);
            me->CastSpell(nullptr, SPELL_BLOOD_BARRAGE_CREATE_AT_CHANNEL, false);
            me->Yell(BLOOD, LANG_UNIVERSAL, NULL);
            break;
        }
    }

    void JustReachedHome() override
    {
        _JustReachedHome();
        me->RemoveAllAreaTriggers();
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        me->RemoveAllAreaTriggers();
        _DespawnAtEvade();
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        me->Yell(DIE, LANG_UNIVERSAL, NULL);
        me->RemoveAllAreaTriggers();
    }
private:
    EventMap _events;
};

//322746
class aura_corrupted_blood : public AuraScript
{
    PrepareAuraScript(aura_corrupted_blood);

    void OnTick(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetTarget())
        {
            std::list<Player*> targetList;
            GetTarget()->GetPlayerListInGrid(targetList, 7.0f);
            for (Player* targets : targetList)
            {
                if (!targets->HasAura(SPELL_CORRUPTED_BLOOD))
                    GetCaster()->CastSpell(targets, SPELL_CORRUPTED_BLOOD, true);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(aura_corrupted_blood::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

//323064
class aura_blood_barrage : public AuraScript
{
    PrepareAuraScript(aura_blood_barrage);

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        if (GetCaster())
        {
            std::list<Player*> targetList;
            GetTarget()->GetPlayerListInGrid(targetList, 100.0f);
            for (Player* targets : targetList)
            {
                GetCaster()->CastSpell(targets->GetPosition(), SPELL_BLOOD_BARRAGE_MISSILE, true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_blood_barrage::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

void AddSC_boss_hakkar_the_soulflayer_os()
{
    RegisterCreatureAI(boss_hakkar_the_soulflayer_os);
    RegisterSpellScript(aura_corrupted_blood);
    RegisterSpellScript(aura_blood_barrage);
}
