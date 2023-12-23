/*
 * Copyright 2023 DekkCore Team
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
#include "halls_of_attonement.h"

enum Spells
{   
    SPELL_BLOOD_TORRENT_MISSILE = 326389,
    SPELL_BLOOD_TORRENT_PERIODIC = 319703,
    SPELL_STONE_CALL_CAST = 319733,
    SPELL_CURSE_OF_STONE_CAST = 328206,
    SPELL_CURSE_OF_STONE_PERIODIC = 319603,
    SPELL_CURSE_OF_STONE_MISSILE = 327411,
    SPELL_CURSE_OF_STONE_SLOW = 319602,
    SPELL_TURNED_TO_STONE = 319611,
    SPELL_STONE_SHATTERING_LEAP = 319941,
    SPELL_SHATTERED = 344874,
    SPELL_STONE_SHATTERING_LEAP_DAMAGE = 319592,
    SPELL_STONE_FORM = 319724,
    SPELL_VOLATILE_TRANSFORMATION = 328125,
    SPELL_VILLAINOUS_BOLT = 328322,
};

enum Events
{
    EVENT_ENERGY_GAIN = 1,
    EVENT_BLOOD_TORRENT,
    EVENT_CUSRE_OF_STONE,
    EVENT_TORRENT_MISSILE,
    EVENT_STONE_CALL_CAST,
    EVENT_STONE_SHATTERING_LEAP,
};
//164185
struct boss_echelon : public BossAI
{
    boss_echelon(Creature* c) : BossAI(c, DATA_ECHELON) { }

    void Reset() override
    {
        _events.Reset();
        _events.ScheduleEvent(EVENT_ENERGY_GAIN, 15s);
        _events.ScheduleEvent(EVENT_BLOOD_TORRENT, 8s);
        _events.ScheduleEvent(EVENT_TORRENT_MISSILE, 10s);
        _events.ScheduleEvent(EVENT_STONE_CALL_CAST, 15s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->GetPower(POWER_ENERGY) >= 100)
        {
            me->SetPower(POWER_ENERGY, 0);
            me->CastSpell(nullptr, SPELL_CURSE_OF_STONE_CAST, false);
            _events.ScheduleEvent(SPELL_STONE_SHATTERING_LEAP, 3s);
        }
        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ENERGY_GAIN:
                me->ModifyPower(POWER_ENERGY, urand(+3, +5));
                DoCastVictim(SPELL_STONE_SHATTERING_LEAP);
                _events.Repeat(15s);
                break;

            case EVENT_BLOOD_TORRENT:
                DoCastVictim(SPELL_BLOOD_TORRENT_MISSILE);
                _events.Repeat(15s, 25s);
                break;
            case EVENT_STONE_CALL_CAST:
                DoCastVictim(SPELL_STONE_CALL_CAST);
                _events.Repeat(30s, 35s);
                break;
            case EVENT_STONE_SHATTERING_LEAP:
                if (Unit* target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 100.0f, true))
                    me->CastSpell(target, SPELL_STONE_SHATTERING_LEAP, false);
                break;
            default:
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        Talk(0);
        _events.ScheduleEvent(EVENT_ENERGY_GAIN, 20s);
        _events.ScheduleEvent(EVENT_TORRENT_MISSILE, 10s);
        _events.ScheduleEvent(EVENT_STONE_CALL_CAST, 30s);
    }

    void OnSpellCast(SpellInfo const* spell) override
    {
        switch (spell->Id)
        {
        case SPELL_STONE_CALL_CAST:
            for (uint8 i = 0; i < 6; i++)
            {
                me->SummonCreature(NPC_UNDYING_STONEFIEND, me->GetRandomNearPosition(25.0f), TEMPSUMMON_MANUAL_DESPAWN);
            }
            break;

        case SPELL_CURSE_OF_STONE_CAST:
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {                
                me->CastSpell(target, SPELL_CURSE_OF_STONE_MISSILE, true);
                me->AddAura(SPELL_CURSE_OF_STONE_SLOW, target);
            }
            break;

        case SPELL_STONE_SHATTERING_LEAP:
            {
                std::list<Creature*> undyingFiendsList;
                me->GetCreatureListWithEntryInGrid(undyingFiendsList, NPC_UNDYING_STONEFIEND, 100.0f);
                for (Creature* undyingFiend : undyingFiendsList)
                {
                    if (undyingFiend->HasAura(SPELL_STONE_FORM))
                        me->KillSelf();
                }
            }
            break;
        }
    } 

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) 
    { 
        if (summon->GetEntry() == NPC_UNDYING_STONEFIEND)
            summon->CastSpell(nullptr, SPELL_VOLATILE_TRANSFORMATION, true);
    }

    void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override 
    { 
        switch (spellInfo->Id)
        {
        case SPELL_STONE_SHATTERING_LEAP_DAMAGE:
            me->AddAura(SPELL_SHATTERED, me);
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
        Talk(1);
        me->RemoveAllAreaTriggers();
    }
    private:
        EventMap _events;
};

//23349
struct at_blood_torrent : public AreaTriggerAI
{
    at_blood_torrent(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (!target->HasAura(SPELL_BLOOD_TORRENT_PERIODIC))
            at->GetCaster()->CastSpell(target, SPELL_BLOOD_TORRENT_PERIODIC, true);
    }

    void OnUnitExit(Unit* target) override
    {
        if (!target->IsPlayer())
            return;

        if (target->HasAura(SPELL_BLOOD_TORRENT_PERIODIC))
            target->RemoveAura(SPELL_BLOOD_TORRENT_PERIODIC);
    }
};

//319603
class aura_curse_of_stone : public AuraScript
{
    PrepareAuraScript(aura_curse_of_stone);

    void OnRemove(const AuraEffect* /* aurEff */, AuraEffectHandleModes /*mode*/)
    {
        Unit* target;
        if (GetTarget()) 
            GetTarget()->AddAura(SPELL_TURNED_TO_STONE, target); 
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(aura_curse_of_stone::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

//164363
struct npc_undying_stonefiend : public ScriptedAI
{
    npc_undying_stonefiend(Creature* c) : ScriptedAI(c) { stoneForm = false; }

private:
    bool stoneForm;

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(SPELL_VILLAINOUS_BOLT, 3s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        if (stoneForm && me->GetHealthPct() > 5)
        {
            stoneForm = false;
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveAura(SPELL_STONE_FORM);
            events.ScheduleEvent(SPELL_VILLAINOUS_BOLT, 3s);
        }
        switch (eventId)
        {
        case SPELL_VILLAINOUS_BOLT:
            DoCastRandom(SPELL_VILLAINOUS_BOLT, 100.0f, false);
            events.Repeat(4s, 5s);
            break;
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPct(5) && !stoneForm)
        {
            stoneForm = true;
            events.Reset();
            me->SetReactState(REACT_PASSIVE);
            me->AddAura(SPELL_STONE_FORM, me); 
        }
    }
};

void AddSC_boss_echelon()
{
    RegisterCreatureAI(boss_echelon);
    RegisterAreaTriggerAI(at_blood_torrent);
    RegisterAuraScript(aura_curse_of_stone);
    RegisterCreatureAI(npc_undying_stonefiend);
}
