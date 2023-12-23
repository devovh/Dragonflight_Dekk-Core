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
    SPELL_DRAW_SOUL = 319521,
    SPELL_DRAW_SOUL_STUN = 319531,
    SPELL_SOULLESS = 319539,
    SPELL_GRASPING_HANDS_PERIODIC_DAMAGE_AND_ROOT = 333710,
    SPELL_GRASPING_HANDS_MISSILE = 333713, //at 25292
    SPELL_SPECTRAL_REACH = 319669,
    SPELL_PHANTASMAL_PARASITE = 319626,
    SPELL_PHANTASMAL_PARASITE_DAMAGE = 319765,
};

//162309
struct boss_kultharok : public BossAI
{
    boss_kultharok(Creature* c) : BossAI(c, DATA_KULTHAROK) { }

    void Reset() override
    {
        BossAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        events.ScheduleEvent(SPELL_DRAW_SOUL, 5s);
        events.ScheduleEvent(SPELL_GRASPING_HANDS_MISSILE, 10s);
        events.ScheduleEvent(SPELL_PHANTASMAL_PARASITE, 15s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
        case SPELL_DRAW_SOUL:
        {
            std::list <Player*> targetList;
            me->GetPlayerListInGrid(targetList, 80.0f);
            if (targetList.size() > 2)
                targetList.resize(2);

            for (Player* targets : targetList)
            {
                me->AddAura(SPELL_SOULLESS, targets);
                me->CastSpell(targets, SPELL_DRAW_SOUL, false);
            }
            events.Repeat(20s, 25s);
            break;
        }

        case SPELL_GRASPING_HANDS_MISSILE:
            DoCastRandom(SPELL_GRASPING_HANDS_MISSILE, 100.0f, true);
            events.Repeat(26s);
            break;

        case SPELL_PHANTASMAL_PARASITE:
            me->CastSpell(nullptr, SPELL_PHANTASMAL_PARASITE, true);
            std::list <Player*> targetList;
            me->GetPlayerListInGrid(targetList, 80.0f);
            if (targetList.size() > 2)
                targetList.resize(2);

            for (Player* targets : targetList)
            {
                me->AddAura(SPELL_PHANTASMAL_PARASITE, targets);
            }
            events.Repeat(30s);
            break;
        }
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
        instance->SetBossState(DATA_KULTHAROK, DONE);
    }
};

//319626
class aura_phantasmal_parasite : public AuraScript
{
    PrepareAuraScript(aura_phantasmal_parasite);

    void OnTick(AuraEffect const* auraEff)
    {
        if (GetCaster() && GetTarget())
            GetCaster()->CastSpell(GetTarget(), SPELL_PHANTASMAL_PARASITE_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(aura_phantasmal_parasite::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

void AddSC_boss_kultharok()
{
    RegisterCreatureAI(boss_kultharok);
    RegisterAuraScript(aura_phantasmal_parasite);
}
