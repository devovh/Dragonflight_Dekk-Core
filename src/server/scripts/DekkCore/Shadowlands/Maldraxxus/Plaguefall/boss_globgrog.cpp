/*
 * Copyright 2021 
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

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "DB2Stores.h"
#include "CellImpl.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "G3DPosition.hpp"
#include <G3D/Box.h>
#include <G3D/CoordinateFrame.h>
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "MovementTypedefs.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "UpdateFields.h"
#include "G3D/Vector2.h"
#include "plaguefall.h"

enum Spells
{
    SPELL_SLIME_WAVE          = 324667,
    SPELL_SLIME_WAVE_FRONT    = 326242,
    SPELL_SLIME_WAVE_AMBUSH   = 332865,
    SPELL_PLAGUE_STOMP        = 324527,
    SPELL_DEBILITATING_PLAGUE = 324652,
    SPELL_BECKON              = 324490,
    SPELL_BECKON_SLIME_SUMM   = 324459,
    SPELL_CONSUME_SLIME       = 319780,
    SPELL_LETHARGY            = 326868,
    SPELL_SHATTERED_PSYCHE    = 344663,

};

enum Actions
{
    EVENT_ENERGY = 1,
};

//164255
struct boss_globgrog : public BossAI
{
    boss_globgrog(Creature* creature) : BossAI(creature, DATA_GLOBGROG) { }

    void Reset() override
    {
        BossAI::Reset();
        me->SetPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 0);
        me->AddAura(AURA_OVERRIDE_POWER_COLOR_GREEN, me);
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        Talk(0);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        events.ScheduleEvent(EVENT_ENERGY, 1s);
        events.ScheduleEvent(SPELL_PLAGUE_STOMP, 5s);
        events.ScheduleEvent(SPELL_SLIME_WAVE, 20s);
        events.ScheduleEvent(SPELL_BECKON, 15s);
        events.ScheduleEvent(SPELL_BECKON_SLIME_SUMM, 25s);
    }

    void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
        case SPELL_PLAGUE_STOMP:
            if (target->IsUnit())
                me->AddAura(SPELL_DEBILITATING_PLAGUE, target->ToUnit());
            break;
        }
    }

    void DoAction(int32 action) override
    {
        if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 250.0f, true, true))

        if (me->GetPower(POWER_ENERGY) == 100)
        {
            me->ModifyPower(POWER_ENERGY, -100);
            events.ScheduleEvent(SPELL_BECKON, 1s);
            me->CastSpell(target, SPELL_BECKON, true);
        }
        if (Creature* morsel = me->FindNearestCreature(NPC_SLIMY_MORSEL, 10.0f, true))
        {
            me->CastSpell(morsel, SPELL_CONSUME_SLIME, true);
            morsel->DespawnOrUnsummon();
        }
        if (Creature* smorgasboard = me->FindNearestCreature(NPC_SLIMY_SMORGASBORD, 10.0f, true))
        {
            me->CastSpell(smorgasboard, SPELL_CONSUME_SLIME, true);
            smorgasboard->DespawnOrUnsummon();
        }
        Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 250.0f, true, true);

        switch (action)
        {
        case EVENT_ENERGY:
            me->ModifyPower(POWER_ENERGY, +2);
            events.Repeat(1s);
            break;

        case SPELL_PLAGUE_STOMP:
            Talk(1);
            DoCastAOE(SPELL_PLAGUE_STOMP, true);
            me->CastSpell(target, SPELL_PLAGUE_STOMP, true);
            events.Repeat(20s);
            break;

        case SPELL_SLIME_WAVE:
            Talk(2);
            DoCastRandom(SPELL_SLIME_WAVE, 100.0f, true);
            me->CastSpell(target, SPELL_SLIME_WAVE, true);
            events.Repeat(30s);
            break;

        case SPELL_BECKON:
            Talk(3);
            me->CastSpell(nullptr, SPELL_BECKON, true);
            scheduler.Schedule(1s, [this](TaskContext /*task*/)    
            {
                    me->SummonCreature(NPC_SLIMY_MORSEL, me->GetRandomNearPosition(30.0f), TEMPSUMMON_MANUAL_DESPAWN);
                if (IsMythic())
                    me->SummonCreature(NPC_SLIMY_SMORGASBORD, me->GetRandomNearPosition(30.0f), TEMPSUMMON_MANUAL_DESPAWN);
            });
            break;
        }
    }


    void EnterEvadeMode(EvadeReason why) override
    {
        _JustReachedHome();
        me->DespawnCreaturesInArea(NPC_SLIMY_MORSEL, 125.0f);
        me->DespawnCreaturesInArea(NPC_SLIMY_SMORGASBORD, 125.0f);
        me->RemoveAllAreaTriggers();
        _DespawnAtEvade();
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        Talk(4);
        me->RemoveAllAreaTriggers();
        me->DespawnCreaturesInArea(NPC_SLIMY_MORSEL, 125.0f);
        me->DespawnCreaturesInArea(NPC_SLIMY_SMORGASBORD, 125.0f);
    }
    private:
        TaskScheduler _scheduler;
};

//164362,171887
struct npc_globgrog_add : public ScriptedAI
{
    npc_globgrog_add(Creature* c) : ScriptedAI(c) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        me->SetReactState(REACT_PASSIVE);
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        me->SetWalk(true);
        me->GetMotionMaster()->MoveFollow(summoner->ToUnit(), PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
    }
};

void AddSC_boss_globgrog()
{
    RegisterCreatureAI(boss_globgrog);
    RegisterCreatureAI(npc_globgrog_add);
}
