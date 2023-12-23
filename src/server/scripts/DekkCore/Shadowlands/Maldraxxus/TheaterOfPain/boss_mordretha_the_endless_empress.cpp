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
    SPELL_REAPING_SCYTHE = 324079,
    SPELL_DARK_DEVASTATION_SUMMON_STALKER = 324170, //NPC_DEVASTATION_STALKER, missing at data, spellmisc 21570
    SPELL_DARK_DEVASTATION_CREATE_AT = 323608,
    SPELL_DARK_DEVASTATION_AT_DAMAGE = 323681,
    SPELL_MANIFEST_DEATH = 324449,
    SPELL_MANIFEST_DEATH_EXP = 324514,
    EVENT_GRASPING_RIFT = 1,
    SPELL_GRASPING_RIFT_MAIN = 323825, //missing both ats
    SPELL_ECHOES_OF_CARNAGE = 339573,
    SPELL_ECHO_OF_BATTLE_POOF_OUT = 339561,
    SPELL_ECHO_OF_BATTLE_DAMAGE = 339550,
    SPELL_GHOSTLY_CHARGE_DAMAGE_KNOCK = 339751,
    SPELL_GHOSTLY_CHARGE_APPLY_AT = 339706, //missing at
};

//165946
struct boss_modretha_the_endless_empress : public BossAI
{
    boss_modretha_the_endless_empress(Creature* c) : BossAI(c, DATA_MORDRETHA) { }

private:
    bool mordretha50;

    void Reset() override
    {
        BossAI::Reset();
        if (instance->GetBossState(DATA_KULTHAROK) != DONE)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetVisible(false);
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        events.ScheduleEvent(SPELL_REAPING_SCYTHE, 5s);
        events.ScheduleEvent(SPELL_DARK_DEVASTATION_SUMMON_STALKER, 10s);
        events.ScheduleEvent(SPELL_MANIFEST_DEATH, 15s);
        events.ScheduleEvent(EVENT_GRASPING_RIFT, 20s);
        mordretha50 = false;
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    { 
        if (me->GetHealthPct() == 50 && !mordretha50)
        {
            mordretha50 = true;
            me->CastSpell(nullptr, SPELL_ECHOES_OF_CARNAGE, false);
            uint8 randomMechanic = urand(0, 1);
            switch (randomMechanic)
            {
            case 0: //Echo of Battle
                me->CastSpell(nullptr, SPELL_ECHO_OF_BATTLE_POOF_OUT, false);
                break;
            case 1: //Ghostly Charge
                break;
            }
        }
    }

    void JustSummoned(Creature* summon) override
    { 
        if (summon->GetEntry() == NPC_GRASPING_RIFT)
        {
            summon->CanFly();
            summon->SetFlying(true);
            summon->SetReactState(REACT_PASSIVE);
            summon->CastSpell(nullptr, SPELL_GRASPING_RIFT_MAIN, false);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (instance->GetBossState(DATA_KULTHAROK) == DONE)
            if (!me->IsVisible())
            {
                me->SetVisible(true);
                me->SetReactState(REACT_DEFENSIVE);
            }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case SPELL_REAPING_SCYTHE:
                DoCastVictim(SPELL_REAPING_SCYTHE, false);
                events.Repeat(15s, 18s);
                break;

            case SPELL_DARK_DEVASTATION_SUMMON_STALKER:
                me->SummonCreature(NPC_DEVASTATION_STALKER, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
                events.Repeat(20s, 25s);
                break;

            case SPELL_MANIFEST_DEATH:
            {
                me->CastSpell(nullptr, SPELL_MANIFEST_DEATH, true);
                std::list<Player*> targetList;
                me->GetPlayerListInGrid(targetList, 100.0f);
                for (Player* targets : targetList)
                {
                    me->AddAura(SPELL_MANIFEST_DEATH, targets);
                }
                events.Repeat(26s, 30s);
                break;
            }

            case EVENT_GRASPING_RIFT:
                me->SummonCreature(NPC_GRASPING_RIFT, me->GetRandomNearPosition(25), TEMPSUMMON_MANUAL_DESPAWN);
                events.Repeat(31s, 35s);
                break;
            }
        }
    }
};

//324449
class aura_manifest_death : public AuraScript
{
    PrepareAuraScript(aura_manifest_death);

    void OnRemove(const AuraEffect* /* aurEff */, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetTarget())
            GetCaster()->CastSpell(GetTarget(), SPELL_MANIFEST_DEATH_EXP, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(aura_manifest_death::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_modretha_the_endless_empress()
{
    RegisterCreatureAI(boss_modretha_the_endless_empress);
    RegisterAuraScript(aura_manifest_death);
}
