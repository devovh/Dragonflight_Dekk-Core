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
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "Flux.h"

enum eSpells
{
    //! 1 rank

    // ooliss
    SPELL_HORRIFIC_PURSUIT = 233224,

    // grandpa
    SPELL_THROW_TOY_1 = 140967, //urand 140967 140962
    SPELL_THROW_TOY_2 = 140962, //urand 140967 140962
    SPELL_THROW_TOY_BAD = 141020,
    SPELL_SONG_OF_FLEIT = 140982, // 21 ?
    SPELL_KANTATA_OF_FLEIT = 140983, // 48 ?   (heal)
    SPELL_GRUMMKEPACK = 140950, // on 50% and after 140986
    SPELL_BAD_LUCKYDO = 140986,
    SPELL_THROW_LUCKY_FLEIT = 140988, // on 7%

    // oso
    SPELL_SHOTGUN_ROAR = 234489, // 11 ?
    SPELL_CLAWSTROPHOBIC = 234313, // 21 ?
    SPELL_JUMP_GRIZZLY = 234365, // 21 ? (3 times)

    // gnomes
    SPELL_PREPARED_TO_SPELL = 229758, // ?????
    SPELL_WHIRLWIND = 234046, // 20

    SPELL_LAVA_BURST = 229393, // 4
    SPELL_MOLTEN_SLAG = 229394, // 10

    SPELL_LIGHTING_CRASH = 229471, // 7    
};

// 117133
class boss_brawguild_ooliss : public CreatureScript
{
public:
    boss_brawguild_ooliss() : CreatureScript("boss_brawguild_ooliss") {}

    struct boss_brawguild_oolissAI : public BossAI
    {
        boss_brawguild_oolissAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_ONE) {}

        void Reset() override
        {
            events.Reset();
            me->ApplySpellImmune(0, IMMUNITY_ID, 236110, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INSTAKILL, true);
            me->AddDelayedEvent(500, [this]() -> void
                {
                    Player* OwnerPlayer = ObjectAccessor::FindPlayer(me->GetPrivateObjectOwner());
                    if (OwnerPlayer)
                        AttackStart(OwnerPlayer);
                });
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.RescheduleEvent(1, 25s);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->HasAura(SPELL_HORRIFIC_PURSUIT))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                {
                    me->CastSpell(me, 233226);
                    Position pos;
                    me->NearTeleportTo(pos);
                    me->AddUnitState(UNIT_STATE_ROOT);
                    events.RescheduleEvent(2, 5s);
                    events.RescheduleEvent(1, 25s);
                    break;
                }
                case 2:
                {
                    me->AddDelayedEvent(2400, [this]() -> void
                        {
                            if (me && me->IsAlive())
                                me->ClearUnitState(UNIT_STATE_ROOT);
                        });

                    if (Unit* owner = me->GetOwner())
                        me->CastSpell(owner, SPELL_HORRIFIC_PURSUIT);

                    me->SetWalk(true);
                    me->AddDelayedEvent(2400 + 7500, [this]() -> void
                        {
                            me->SetWalk(false);
                        });
                    break;
                }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_oolissAI(creature);
    }
};

// 70678
class boss_brawguild_grandpa : public CreatureScript
{
public:
    boss_brawguild_grandpa() : CreatureScript("boss_brawguild_grandpa") {}

    struct boss_brawguild_grandpaAI : public BossAI
    {
        boss_brawguild_grandpaAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_ONE) {}

        uint32 healthPct;

        void Reset() override
        {
            events.Reset();
            healthPct = 50;
            summons.DespawnAll();
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_GRUMMKEPACK, true);
            me->AddDelayedEvent(500, [this]() -> void
                {
                    Player* OwnerPlayer = ObjectAccessor::FindPlayer(me->GetPrivateObjectOwner());
                    if (OwnerPlayer)
                        AttackStart(OwnerPlayer);
                });
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.RescheduleEvent(1, 4s); // throw
            events.RescheduleEvent(2, 18s); // song
            events.RescheduleEvent(3, 8s); // kantata urand(40, 48)
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            switch (summon->GetEntry())
            {
            case 70735:
            case 70693:
                summon->CastSpell(summon, 140975);
                break;
            case 70692:
                summon->CastSpell(summon, 140971);
                break;
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (me->HealthBelowPct(healthPct))
            {
                if (healthPct == 50)
                {
                    healthPct -= 43;
                    events.RescheduleEvent(5, 5s);
                    events.RescheduleEvent(4, randtime(5s, 8s));
                }
                else
                {
                    healthPct = 0;
                    Talk(1);
                    DoCast(SPELL_THROW_LUCKY_FLEIT);
                }
            }
        }

        void JustDied(Unit* /*who*/) override
        {
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
                {
                    float x;
                    float y;
                    me->GetNearPoint2D(nullptr, x, y, 20.0f, frand(-M_PI / 3, M_PI / 3));
                    Position pos = me->GetPosition();
                    pos.m_positionX = x;
                    pos.m_positionY = y;

                    SpellCastTargets targets;
                    targets.SetDst(pos);
                    CastSpellTargetArg targetarg;
                    targetarg.Targets = targets;
                    if (me->HasAura(SPELL_BAD_LUCKYDO))
                    {
                        me->CastSpell(targetarg, SPELL_THROW_TOY_BAD);
                    }
                    else
                    {
                        me->CastSpell(targetarg, (urand(1, 2) == 1 ? SPELL_THROW_TOY_1 : SPELL_THROW_TOY_2));
                    }
                    events.RescheduleEvent(1, 4s);
                    break;
                }
                case 2:
                    DoCast(SPELL_SONG_OF_FLEIT);
                    events.RescheduleEvent(2, randtime(18s, 25s));
                    break;
                case 3:
                    DoCast(SPELL_KANTATA_OF_FLEIT);
                    events.RescheduleEvent(3, randtime(35s, 48s));
                    break;
                case 4:
                {
                    if (Unit* owner = me->GetOwner())
                    {
                        std::list<Creature*> adds;
                        GetCreatureListWithEntryInGrid(adds, me, 70735, 60.0f);
                        if (!adds.empty())
                            for (std::list<Creature*>::iterator itr = adds.begin(); itr != adds.end(); ++itr)
                            {
                                (*itr)->CastSpell(owner, 141053);
                                if ((*itr)->GetExactDist2d(owner) <= 5.f)
                                {
                                    (*itr)->CastSpell(owner, 140954, true);
                                    (*itr)->DespawnOrUnsummon();
                                }
                            }
                    }
                    break;
                }
                case 5:
                    Talk(0);
                    if (Unit* owner = me->GetOwner())
                        me->CastSpell(owner, SPELL_GRUMMKEPACK);
                    DoCast(SPELL_BAD_LUCKYDO);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_grandpaAI(creature);
    }
};

// 117753
class boss_brawguild_oso : public CreatureScript
{
public:
    boss_brawguild_oso() : CreatureScript("boss_brawguild_oso") {}

    struct boss_brawguild_osoAI : public BossAI
    {
        boss_brawguild_osoAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_ONE) {}

        void Reset() override
        {
            events.Reset();
            if (Creature* morozec = me->SummonCreature(117759, me->GetPositionX() + 3.0f, me->GetPositionY() + 3.0f, me->GetPositionZ(), me->GetOrientation()))
            {
                me->SetReactState(REACT_PASSIVE);
                morozec->SetReactState(REACT_PASSIVE);
                morozec->AI()->Talk(0);
                me->SetFacingToObject(morozec);
                me->StopMoving();
                morozec->AddDelayedEvent(6000, [morozec, this]() -> void
                    {
                        me->SetFacingToObject(morozec);
                        me->CastSpell(morozec, 93330);
                        morozec->CastSpell(morozec, 234213);
                        morozec->DespawnOrUnsummon(250ms);
                    });

                me->AddDelayedEvent(7500, [this]() -> void
                    {
                        TempSummon* moroz = me->SummonCreature(117860, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN);
                        if (moroz)
                        {
                            moroz->SetOwnerGUID(me->GetGUID());
                            moroz->GetMotionMaster()->MoveFollow(me, 0.f, 0.f);
                            moroz->SetReactState(REACT_PASSIVE);
                            moroz->SetDisplayId(11686); // invisible
                            moroz->AI()->Talk(0);

                            me->ApplySpellImmune(SPELL_SHOTGUN_ROAR, IMMUNITY_ID, SPELL_SHOTGUN_ROAR, true);
                            moroz->ApplySpellImmune(SPELL_SHOTGUN_ROAR, IMMUNITY_ID, SPELL_SHOTGUN_ROAR, true);
                        }

                        me->SetReactState(REACT_AGGRESSIVE);
                        Player* OwnerPlayer = ObjectAccessor::FindPlayer(me->GetPrivateObjectOwner());
                        if (OwnerPlayer)
                            AttackStart(OwnerPlayer);
                    });
            }
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.RescheduleEvent(1, 12s);
            events.RescheduleEvent(2, randtime(20s, 21s));
            events.RescheduleEvent(3, randtime(21s, 22s));
        }

        void DoGrizzlyJump()
        {
            Position dest = me->GetPosition();
            if (Player* OwnerPlayer = ObjectAccessor::FindPlayer(me->GetPrivateObjectOwner()))
                dest = OwnerPlayer->GetPosition();

            TempSummon* GrizzlyTarget = me->SummonCreature(117938, dest, TEMPSUMMON_CORPSE_DESPAWN);
            if (GrizzlyTarget)
            {
                GrizzlyTarget->CastSpell(GrizzlyTarget, 234361, true);

                me->CastSpell(GrizzlyTarget, SPELL_JUMP_GRIZZLY);

                me->AddDelayedEvent(2000, [this, GrizzlyTarget]() -> void
                    {
                        if (GrizzlyTarget)
                        {
                            GrizzlyTarget->DespawnOrUnsummon(1s);
                        }
                    });
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
                    me->CastSpell(me->GetVictim(), SPELL_SHOTGUN_ROAR);
                    if (urand(1, 4) != 1)
                        if (Creature* moroz = me->FindNearestCreature(117860, 30.0f, true))
                            moroz->AI()->Talk(1);
                    events.RescheduleEvent(1, 12s);
                    break;
                case 2:
                    me->CastSpell(me->GetVictim(), SPELL_CLAWSTROPHOBIC);
                    events.RescheduleEvent(2, randtime(20s, 22s));
                    break;
                case 3:

                    DoGrizzlyJump();
                    me->AddDelayedEvent(4000, [this]() -> void
                        {
                            DoGrizzlyJump();
                        });

                    me->AddDelayedEvent(8000, [this]() -> void
                        {
                            DoGrizzlyJump();
                        });

                    events.RescheduleEvent(1, randtime(12s, 14s));
                    events.RescheduleEvent(3, randtime(21s, 22s));
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_osoAI(creature);
    }
};

// 115292, 115294, 115295
class boss_brawguild_gnomes : public CreatureScript
{
public:
    boss_brawguild_gnomes() : CreatureScript("boss_brawguild_gnomes") {}

    struct boss_brawguild_gnomesAI : public BossAI
    {
        boss_brawguild_gnomesAI(Creature* creature) : BossAI(creature, DATA_BOSS_RANK_ONE) {}

        uint32 healthPct;

        void Reset() override
        {
            events.Reset();
            switch (me->GetEntry())
            {
            case 115292:
                me->SummonCreature(115294, me->GetPositionX(), me->GetPositionY() + 5.0f, me->GetPositionZ(), me->GetOrientation());
                me->SummonCreature(115295, me->GetPositionX(), me->GetPositionY() - 5.0f, me->GetPositionZ(), me->GetOrientation());
                me->SummonCreature(115346, me->GetPositionX() - 5.0f, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                break;
            case 115294:
                me->SummonCreature(115347, me->GetPositionX() - 5.0f, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                break;
            case 115295:
                me->SummonCreature(115348, me->GetPositionX() - 5.0f, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                break;
            }
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            switch (me->GetEntry())
            {
            case 115292:
                events.RescheduleEvent(1, 20s);
                break;
            case 115294:
                events.RescheduleEvent(2, 4s);
                events.RescheduleEvent(3, 10s);
                break;
            case 115295:
                events.RescheduleEvent(4, 7s);
                break;
            }
        }

        void JustDied(Unit* who) override
        {
            if (!who)
                return;

            if (Unit* unit = me->GetOwner())
                if (Creature* owner = unit->ToCreature())
                    if (owner->IsAlive())
                        owner->Kill(owner, owner, false);
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
                    DoCast(SPELL_PREPARED_TO_SPELL);
                    DoCast(SPELL_WHIRLWIND);
                    events.RescheduleEvent(1, 20s);
                    break;
                case 2:
                    DoCast(SPELL_LAVA_BURST);
                    events.RescheduleEvent(2, 4s);
                    break;
                case 3:
                    DoCast(SPELL_MOLTEN_SLAG);
                    events.RescheduleEvent(3, 10s);
                    break;
                case 4:
                    me->CastSpell(me->GetVictim(), SPELL_LIGHTING_CRASH);
                    events.RescheduleEvent(4, 7s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_brawguild_gnomesAI(creature);
    }
};

void AddSC_the_brawlers_guild_bosses_rank_one()
{
    new boss_brawguild_ooliss();
    new boss_brawguild_grandpa();
    new boss_brawguild_oso();
    new boss_brawguild_gnomes();
};
