/*
 * Copyright (C) 2022 DekkCore
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

#include "AchievementMgr.h"
#include "BrawlersGuild.h"
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Vehicle.h"

enum eSpells
{
    //! 3 rank

    //splat
    SPELL_SPLIT = 141034,

    // shadowmaster
    SPELL_SHADOW_TORCH = 232504, // 6
    SPELL_SHADOW_BLAST = 232502, // 6
    SPELL_SHADOW_DETONATION_DMG = 232514,
    SPELL_SHADOW_DETONATION_TRIG = 232512,

    // johnny
    SPELL_SUMMON_PET = 229065,
    SPELL_SHOOT = 205689, // 2
    SPELL_VOLLEY = 229137, // 6-8
    SPELL_REVIVE_PET = 229111,
    SPELL_POWERSHOOT_FIX = 229123,
    SPELL_POWERSHOOT = 229124,
    SPELL_POWERSHOOT_PLR = 229127,
    SPELL_POWERSHOOT_NPC = 229125,
};

// 117145
class boss_brawguild_doomflipper : public CreatureScript
{
public:
    boss_brawguild_doomflipper() : CreatureScript("boss_brawguild_doomflipper") {}

    struct boss_brawguild_doomflipperAI : public BossAI
    {
        boss_brawguild_doomflipperAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_THREE) {}

        void UpdateAI(uint32 /*diff*/) override
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_doomflipperAI(creature);
    }
};

// 70736, 70737
class boss_brawguild_splat : public CreatureScript
{
public:
    boss_brawguild_splat() : CreatureScript("boss_brawguild_splat") {}

    struct boss_brawguild_splatAI : public BossAI
    {
        boss_brawguild_splatAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_THREE) {}


        void Reset() override
        {
            summons.DespawnAll();
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.RescheduleEvent(1, 4s);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            summon->SetHealth(me->GetHealth());
            summon->AI()->AttackStart(me->GetVictim());
        }

        void JustDied(Unit* who) override
        {
            if (me->GetEntry() != 70736)
                return;

            _Reset();

            if (!who)
                return;
        }

        void EnterEvadeMode(EvadeReason /*why*/) override
        {
            if (me->GetEntry() != 70736)
                return;

            _Reset();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    if (me->GetEntry() == 70736)
                        DoCast(SPELL_SPLIT);
                    me->SetObjectScale(me->GetHealthPct() * 0.006 + 0.4);
                    events.RescheduleEvent(1, 4s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_splatAI(creature);
    }
};

// 116743
// torch 116747    ball 116746
class boss_brawguild_shadowmaster : public CreatureScript
{
public:
    boss_brawguild_shadowmaster() : CreatureScript("boss_brawguild_shadowmaster") {}

    struct boss_brawguild_shadowmasterAI : public BossAI
    {
        boss_brawguild_shadowmasterAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_THREE) {}

        void Reset() override
        {
            summons.DespawnAll();
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            Talk(0);
            events.RescheduleEvent(1, 2s);
            events.RescheduleEvent(2, 2s + 6s);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            if (summon->GetEntry() == 116746)
                if (Creature* last_torch = me->GetMap()->GetCreature(summon->GetTarget()))
                {
                    summon->CastSpell(last_torch, 232509);
                    summon->GetMotionMaster()->MovePoint(1, last_torch->GetPositionX(), last_torch->GetPositionY(), last_torch->GetPositionZ());
                    summon->DespawnOrUnsummon(3s);
                }
        }

        void JustDied(Unit* who) override
        {

            if (!who)
                return;

        }

        void SpellHitTarget(WorldObject* target, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_SHADOW_TORCH)
                me->SummonCreature(116747, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());

            if (spell->Id == 232509)
            {
                for (auto guid : summons)
                    if (Creature* last_torch = me->GetMap()->GetCreature(guid))
                        if (last_torch->GetEntry() == 116747 && last_torch->GetGUID() != target->GetGUID())
                            target->CastSpell(last_torch, SPELL_SHADOW_BLAST, true);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    DoCast(SPELL_SHADOW_TORCH);
                    events.RescheduleEvent(1, 6s);
                    break;
                case 2:
                    DoCast(SPELL_SHADOW_BLAST);
                    events.RescheduleEvent(2, 6s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_shadowmasterAI(creature);
    }
};


// 116747, 116746
class npc_brawguild_shadowmaster_adds : public CreatureScript
{
public:
    npc_brawguild_shadowmaster_adds() : CreatureScript("npc_brawguild_shadowmaster_adds") { }

    struct npc_brawguild_shadowmaster_addsAI : public ScriptedAI
    {
        npc_brawguild_shadowmaster_addsAI(Creature* creature) : ScriptedAI(creature), summons(me) {}

        SummonList summons;

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            if (me->GetEntry() == 116746)
                DoCast(232503);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            if (summon->GetEntry() == 116746)
                if (Creature* last_torch = me->GetMap()->GetCreature(summon->GetTarget()))
                {
                    summon->CastSpell(last_torch, 232509);
                    summon->GetMotionMaster()->MovePoint(1, last_torch->GetPositionX(), last_torch->GetPositionY(), last_torch->GetPositionZ());
                    summon->DespawnOrUnsummon(3s);
                }
        }

        void JustDied(Unit* /*who*/) override
        {
            summons.DespawnAll();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE && id == 1)
            {
                if (Unit* owner = me->GetOwner())
                    if (Creature* last_torch = me->GetMap()->GetCreature(me->GetTarget()))
                        owner->ToCreature()->AI()->SpellHitTarget(last_torch, sSpellMgr->GetSpellInfo(232509, DIFFICULTY_NONE));
                me->DespawnOrUnsummon();
            }
        }

        void SpellHitTarget(WorldObject* target, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_SHADOW_DETONATION_TRIG)
                DoCast(SPELL_SHADOW_DETONATION_DMG);
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_brawguild_shadowmaster_addsAI(creature);
    }
};

// 115058
class boss_brawguild_johnny : public CreatureScript
{
public:
    boss_brawguild_johnny() : CreatureScript("boss_brawguild_johnny") {}

    struct boss_brawguild_johnnyAI : public BossAI
    {
        boss_brawguild_johnnyAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_THREE) {}

        void Reset() override
        {
            me->SetUnitFlag(UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            summons.DespawnAll();
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            DoCast(SPELL_SUMMON_PET);
            events.RescheduleEvent(1, randtime(6s, 7s));
            events.RescheduleEvent(2, randtime(9s, 12s));
            events.RescheduleEvent(4, 2s);
        }

        void JustDied(Unit* who) override
        {
            _Reset();

            if (!who)
                return;
            Talk(0);
        }

        void SpellHitTarget(WorldObject* target, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_POWERSHOOT_NPC)
            {
                events.Reset();
                events.RescheduleEvent(3, 5s);
            }

            if (spell->Id == SPELL_REVIVE_PET)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                if (target->IsCreature())
                {
                    target->ToCreature()->Respawn(false);
                    if (Unit* owner = me->GetOwner())
                        target->ToCreature()->AI()->AttackStart(owner);
                }
                events.RescheduleEvent(1, randtime(6s, 7s));
                events.RescheduleEvent(2, randtime(9s, 12s));
                events.RescheduleEvent(4, 2s);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    DoCastVictim(SPELL_VOLLEY);
                    events.RescheduleEvent(1, randtime(6s, 7s));
                    break;
                case 2:
                    DoCastVictim(SPELL_POWERSHOOT_FIX);
                    DoCastVictim(SPELL_POWERSHOOT);
                    events.RescheduleEvent(4, 2s);
                    break;
                case 3:
                    me->SetReactState(REACT_PASSIVE);

                    {
                        me->CastSpell(me, 229143);
                    }

                    {
                        DoCast(SPELL_SUMMON_PET);
                        me->SetReactState(REACT_AGGRESSIVE);
                        events.RescheduleEvent(1, randtime(6s, 7s));
                        events.RescheduleEvent(2, randtime(9s, 12s));
                        events.RescheduleEvent(4, 2s);
                    }
                    break;
                case 4:
                    DoCast(SPELL_SHOOT);
                    events.RescheduleEvent(4, 2s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_johnnyAI(creature);
    }
};

// 229124
class spell_brawling_powershot : public SpellScriptLoader
{
public:
    spell_brawling_powershot() : SpellScriptLoader("spell_brawling_powershot") { }

    class spell_brawling_powershot_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_brawling_powershot_SpellScript);

        void HandleOnHit()
        {
            if (!GetCaster())
                return;

            Creature* caster = GetCaster()->ToCreature();
            if (!caster)
                return;

            Unit* owner = caster->GetOwner();
            if (!owner)
                return;

            Player* pPlayer = owner->ToPlayer();
            if (!pPlayer)
                return;

            WorldObject* objTarget = nullptr;

            if (Creature* target = pPlayer->FindNearestCreature(115085, 100.0f, true))
            {
                if (target->IsInBetween(caster, pPlayer, 3.0f))
                    objTarget = target;
                else
                    objTarget = pPlayer;
            }
            else
                objTarget = pPlayer;

            if (objTarget->GetTypeId() != TYPEID_PLAYER)
                GetCaster()->CastSpell(objTarget->ToCreature(), SPELL_POWERSHOOT_NPC, true);
            else
                GetCaster()->CastSpell(objTarget->ToPlayer(), SPELL_POWERSHOOT_PLR, true);
        }

        void Register() override
        {
            OnHit += SpellHitFn(spell_brawling_powershot_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_brawling_powershot_SpellScript();
    }
};

void AddSC_the_brawlers_guild_bosses_rank_three()
{
    new boss_brawguild_doomflipper();
    new boss_brawguild_splat();
    new boss_brawguild_shadowmaster();
    new npc_brawguild_shadowmaster_adds();
    new boss_brawguild_johnny();
    new spell_brawling_powershot();
};
