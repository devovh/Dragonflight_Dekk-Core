/*
* DekkCore
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

#include "arena_of_annihiliation.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "ObjectGuid.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_DIZZY                   = 123930,
    SPELL_ANGRY                   = 123936,
    SPELL_HEADBUTT                = 123931,
    SPELL_FLAMELINE_AREA          = 123959,
    SPELL_GROWTH                  = 122213,
    SPELL_TRAILBLAZE              = 123971,
    SPELL_FLYING_SERPENT_KICK_EFF = 127807,
};

// Break the wall of arena
bool HasCorrectRangeForSpin(float x, float y, float z, ObjectGuid m_owner)
{
    Position pos = { x, y, z, frand(0, 2 * M_PI) };
    bool m_available = true;
   // Unit* caster = ObjectAccessor::GetUnit(m_owner);

   // if (!caster)
     //   return m_available;

    // check if got value > dist between center of arena and wall collision
    if (ArenaPos.GetExactDist2d(pos.GetPositionX(), pos.GetPositionY()) > 50.0f)
        m_available = false;

    return m_available;
}

// make mercesed spawn
Position GetCoaxingSpiritSpawnPos(float m_ori, uint64 casterGUID, float m_dist = 14.5f)
{
    Unit* caster;// = ObjectAccessor::FindUnit(casterGUID);

 //   if (!caster)
  //      return { 0.0f, 0.0f, 0.0f, 0.0f };

    float x, y;
    GetPositionWithDistInOrientation(caster, m_dist, m_ori, x, y);

    Position pos = { x, y, caster->GetPositionZ(), 0.0f };

    // At spawn spirit should look at Liuyang
    return { x, y, caster->GetPositionZ(), pos.GetAbsoluteAngle(caster) };
    return true; //hack
}

// current range to Liuyang
float GetCircleRange(uint64 casterGUID, uint64 LiuyangGUID)
{
   // if (Unit* m_caster = ObjectAccessor::FindUnit(casterGUID))
     //   if (Unit* Liuyang = ObjectAccessor::FindUnit(LiuyangGUID))
      //      return m_caster->GetExactDist2d(Liuyang);

    return 0.0f;
}

// Gurgthock 63315
class npc_arena_of_annihiliation_gurgthock : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_gurgthock() : CreatureScript("npc_arena_of_annihiliation_gurgthock") { }

        enum iEvents
        {
            EVENT_INTRO   = 1,
            EVENT_PREPARE = 2,
        };

        struct npc_arena_of_annihiliation_gurgthockAI : public ScriptedAI
        {
            npc_arena_of_annihiliation_gurgthockAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap nonCombatEvents;
            bool intro;

            void InitializeAI() override 
            {
                intro = false;
                instance = me->GetInstanceScript();
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (instance && !intro && who && who->GetTypeId() == TYPEID_PLAYER) // npc have both location, include scenario, need check before activate
                {
                    intro = true;
                    Talk(TALK_INTRO_1);
                    nonCombatEvents.ScheduleEvent(EVENT_INTRO, 3s);
                }

                ScriptedAI::MoveInLineOfSight(who);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO:
                            Talk(TALK_INTRO_2);
                            nonCombatEvents.ScheduleEvent(EVENT_PREPARE, 3s);
                            break;
                        case EVENT_PREPARE:
                            Talk(TALK_INTRO_3);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_gurgthockAI(creature);
        }
};

// Scar Shell 63311
class npc_arena_of_annihiliation_scar_shell : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_scar_shell() : CreatureScript("npc_arena_of_annihiliation_scar_shell") { }

        enum iSpells
        {
            SPELL_STONE_SPIN    = 123928,
            SPELL_CRUSHING_BITE = 123918,
        };

        enum iEvents
        {
            EVENT_STONE_SPIN    = 1,
            EVENT_CRUSHING_BITE = 2,
            EVENT_COLLIS        = 3,
        };

        struct npc_arena_of_annihiliation_scar_shellAI : public BossAI
        {
            npc_arena_of_annihiliation_scar_shellAI(Creature* creature) : BossAI(creature, DATA_SCAR_SHELL) { }

            uint64 targetGUID;
            bool HasEnrage;
            float x, y;

            void Reset() override
            {
                _Reset();
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                JustEngagedWith(who);

                events.ScheduleEvent(EVENT_CRUSHING_BITE, 13s);
                events.ScheduleEvent(EVENT_STONE_SPIN, 20s);
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        me->SetVisible(true);
                        me->GetMotionMaster()->MovePoint(1, ArenaPos);
                        break;
                    case ACTION_REMOVE_SHELL:
                        me->SetSpeed(MOVE_RUN, 1.1f);
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

           //     if (instance)
               //     instance->SendScenarioProgressUpdate(CriteriaProgressData(invChallengerType.find(me->GetEntry())->second, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));

            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 1:
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                        me->SetHomePosition(*me);
                        break;
                    case 2:
                        events.ScheduleEvent(EVENT_COLLIS, 2s);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STONE_SPIN:
                            /*if (Unit* vict = me->GetVictim())
                            {
                                targetGUID = vict->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(vict), SPELL_STONE_SPIN);
                            }*/
                            events.ScheduleEvent(EVENT_STONE_SPIN, 29s);
                            break;
                        case EVENT_CRUSHING_BITE:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_CRUSHING_BITE, false);

                            events.ScheduleEvent(EVENT_CRUSHING_BITE, 7s);
                            break;
                        case EVENT_COLLIS:
                        {
                          //  do
                            {
                                x = me->GetPositionX() + frand(-80.0f, 80.0f); // less random spawn in melee zone
                                y = me->GetPositionY() + frand(-80.0f, 80.0f);
                            }
                            // then check if our planned pos not in wall/texture.
                            // now we`ll check each used position over current.
                            while (!HasCorrectRangeForSpin(x, y, me->GetPositionZ(), me->GetGUID()));

                            me->GetMotionMaster()->MovePoint(2, x, y, me->GetPositionZ());
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_scar_shellAI(creature);
        }
};

// Jol Grum 63312
class npc_arena_of_annihiliation_jol_grum : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_jol_grum() : CreatureScript("npc_arena_of_annihiliation_jol_grum") { }

        enum iSpells
        {
            SPELL_ILL_TEMPERED = 123933,
            SPELL_SLAM         = 123934,
        };

        enum iEvents
        {
            EVENT_HEADBUTT = 1,
            EVENT_STOMP    = 2,
        };

        struct npc_arena_of_annihiliation_jol_grumAI : public BossAI
        {
            npc_arena_of_annihiliation_jol_grumAI(Creature* creature) : BossAI(creature, DATA_JOL_GRUM) { }

            uint64 targetGUID;
            bool HasEnrage;

            void Reset() override
            {
                _Reset();
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith(who);

                me->CastSpell(me, SPELL_ILL_TEMPERED, false);
                events.ScheduleEvent(EVENT_HEADBUTT, 7s);
                events.ScheduleEvent(EVENT_STOMP, 20s);
            }

            void DoAction(int32 actionId) override
            {
                me->SetVisible(true);
                me->GetMotionMaster()->MovePoint(1, ArenaPos);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

             //   if (instance)
                   // instance->SendScenarioProgressUpdate(CriteriaProgressData(invChallengerType.find(me->GetEntry())->second, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                    me->CastSpell(me, SPELL_SLAM, false);

                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    me->SetHomePosition(*me);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HEADBUTT:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_HEADBUTT, false);

                            events.ScheduleEvent(EVENT_HEADBUTT, 7s);
                            break;
                        case EVENT_STOMP:
                          //  me->PrepareChanneledCast(me->GetAngle(ArenaPos.GetPositionX(), ArenaPos.GetPositionY()));
                            me->GetMotionMaster()->MoveJump(ArenaPos, 25.0f, 30.0f, EVENT_JUMP);
                            events.ScheduleEvent(EVENT_STOMP, 20s);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_jol_grumAI(creature);
        }
};

// Little Liuyang 63313
class npc_arena_of_annihiliation_little_liuyang : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_little_liuyang() : CreatureScript("npc_arena_of_annihiliation_little_liuyang") { }

        enum iSpells
        {
            SPELL_FIRE_WALL      = 123961, // on dummy
            SPELL_FLAME_WALL     = 123966,
            SPELL_COSMETIC_FIRE  = 131207,
            SPELL_FIRE_BALL      = 123958,
            SPELL_FLAMELINE      = 123960,
        };

        enum iEvents
        {
            EVENT_FIREBALL         = 1,
            EVENT_FLAMELINE        = 2,
            EVENT_FLAMELINE_REMOVE = 3,
            EVENT_FIREWALL         = 4,
        };

        struct npc_arena_of_annihiliation_little_liuyangAI : public BossAI
        {
            npc_arena_of_annihiliation_little_liuyangAI(Creature* creature) : BossAI(creature, DATA_LIUYANG) { }

            uint64 targetGUID;
            bool HasEnrage;
            uint32 m_LineStep;

            void Reset() override
            {
                _Reset();
                events.Reset();
                SetCombatMovement(false);
                m_LineStep = 4;
                me->GetMap()->SetWorldStateValue(WORLDSTATE_BEAT_THE_HEAT, 1, false);
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith(who);
                events.ScheduleEvent(EVENT_FLAMELINE, 12s);
                events.ScheduleEvent(EVENT_FIREBALL, 4s);
                events.ScheduleEvent(EVENT_FIREWALL, 36s);

                if (!me->HasAura(SPELL_GROWTH))
                    me->CastSpell(me, SPELL_GROWTH, false);
            }

            void DoAction(int32 actionId) override
            {
                me->SetVisible(true);
                me->GetMotionMaster()->MovePoint(1, ArenaPos);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_FLAMELINE)
                    return m_LineStep;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_FLAMELINE)
                    m_LineStep = data;
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

           //     if (instance)
            //        instance->SendScenarioProgressUpdate(CriteriaProgressData(invChallengerType.find(me->GetEntry())->second, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));

                std::list<Creature*> Waves;
                GetCreatureListWithEntryInGrid(Waves, me, NPC_FLAME_WALL, VISIBLE_RANGE);

                for (auto&& itr : Waves)
                    itr->DespawnOrUnsummon();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    me->SetHomePosition(*me);
                    me->CastSpell(me, SPELL_GROWTH, false);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) && ((me->GetCurrentSpell(CURRENT_GENERIC_SPELL) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id != SPELL_FIRE_BALL) || !me->HasAura(SPELL_FLAMELINE)))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLAMELINE:
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 150.0f, true))
                                me->CastSpell(target, SPELL_FLAMELINE, false);

                            events.ScheduleEvent(EVENT_FLAMELINE, 20s);
                            events.ScheduleEvent(EVENT_FLAMELINE_REMOVE, 15s); // 50y/4= 12,5 ticks <-> 12,5/250ms->3250s
                            break;
                        case EVENT_FIREBALL:
                            if (me->HasAura(SPELL_FLAMELINE))
                            {
                                events.RescheduleEvent(EVENT_FIREBALL, 4s);
                                break;
                            }
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 150.0f, true))
                                me->CastSpell(target, SPELL_FIRE_BALL, false);

                            events.ScheduleEvent(EVENT_FIREBALL, 4s);
                            break;
                        case EVENT_FIREWALL:
                            me->CastSpell(me, SPELL_FLAME_WALL, false);
                            events.ScheduleEvent(EVENT_FIREWALL, 50s);
                            break;
                        case EVENT_FLAMELINE_REMOVE:
                            me->RemoveAura(SPELL_FLAMELINE);
                            m_LineStep = 4;
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_little_liuyangAI(creature);
        }
};

// Flamecoaxing Spirit 63539
class npc_arena_of_annihiliation_flamecoaxing_spirit : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_flamecoaxing_spirit() : CreatureScript("npc_arena_of_annihiliation_flamecoaxing_spirit") { }

        enum iEvents
        {
            EVENT_INIT_WAVE = 1,
            EVENT_PUSH_WAVE = 2,
        };

        struct npc_arena_of_annihiliation_flamecoaxing_spiritAI : public CreatureAI
        {
            npc_arena_of_annihiliation_flamecoaxing_spiritAI(Creature* creature) : CreatureAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            float x, y, step;
            std::list<uint64> WaveTrigger;

            void IsSummonedBy(WorldObject* summoner) override
            {
                instance = me->GetInstanceScript();
                me->SetUnitFlag(UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
                step = 0.0f;

                // Summon 6 waves
                if (instance)
                    if (Unit* Liuyang = ObjectAccessor::GetUnit(*me, instance->GetGuidData(NPC_LITTLE_LIUYANG)))
                        for (uint8 i = 0; i < 6; ++i)
                            if (TempSummon* m_wave = me->SummonCreature(NPC_FLAME_WALL, GetCoaxingSpiritSpawnPos(Liuyang->GetAbsoluteAngle(me) - M_PI / 6, TEMPSUMMON_MANUAL_DESPAWN)))
                               // WaveTrigger.push_back(m_wave->GetGUID());

                events.ScheduleEvent(EVENT_INIT_WAVE, 2s);
            }

            void JustDied(Unit* /*killer*/) override
            {
               // for (auto&& itr : WaveTrigger)
                //    if (Creature* wave = ObjectAccessor::GetCreature(*me, itr))
                  //      wave->DespawnOrUnsummon();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    events.ScheduleEvent(EVENT_PUSH_WAVE, 2s);
            }

            //void HandleMoveOn(uint64 casterGUID, float dist)
            //{
            //    if (Unit* m_caster = ObjectAccessor::FindUnit(casterGUID))
            //    {
            //        GetPositionWithDistInOrientation(m_caster, dist, Position::NormalizeOrientation(m_caster->GetOrientation() + M_PI), x, y);
            //        Movement::MoveSplineInit init(m_caster);
            //        init.MoveTo(x, y, m_caster->GetPositionZ());
            //        init.SetVelocity(2.0f);
            //      //  init.SetOrientationInversed();
            //        init.Launch();
            //    }
            //}

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INIT_WAVE:
                        {
                           // HandleMoveOn(me->GetGUID(), 9.0f);
                            events.ScheduleEvent(EVENT_PUSH_WAVE, 4s);

                            // Wave move
                            for (auto&& itr : WaveTrigger)
                            {
                            //    HandleMoveOn(itr, step);
                                step += 4.0f;
                            }
                            break;
                        }
                        case EVENT_PUSH_WAVE:
                           // if (Unit* Liuyang = ObjectAccessor::GetUnit(*me, instance ? instance->GetGuidData(NPC_LITTLE_LIUYANG) : 0))
                               // me->GetMotionMaster()->MovePoint(0, GetCoaxingSpiritSpawnPos(Position::NormalizeOrientation(Liuyang->GetAngle(me) + M_PI / 6), Liuyang->GetGUID(), GetCircleRange(me->GetGUID(), Liuyang->GetGUID())).GetPositionX(), GetCoaxingSpiritSpawnPos(Position::NormalizeOrientation(Liuyang->GetAngle(me) + M_PI / 6), Liuyang->GetGUID(), GetCircleRange(me->GetGUID(), Liuyang->GetGUID())).GetPositionY(), GetCoaxingSpiritSpawnPos(Position::NormalizeOrientation(Liuyang->GetAngle(me) + M_PI / 6), Liuyang->GetGUID(), GetCircleRange(me->GetGUID(), Liuyang->GetGUID())).GetPositionZ());

                            step = 0.0f;

                          /*  for (auto&& itr : WaveTrigger)
                            {
                                if (Creature* wave = ObjectAccessor::GetCreature(*me, itr))
                                {
                                    wave->SetSpeed(MOVE_RUN, 1.1f + step);

                                    step += 0.35f;

                                    if (step >= 1.4f)
                                        step = 1.45f;

                                    wave->AI()->DoAction(ACTION_INTRO);
                                }
                            }*/
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_flamecoaxing_spiritAI(creature);
        }
};

// Flame wall 63534
class npc_arena_of_annihiliation_flame_wall : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_flame_wall() : CreatureScript("npc_arena_of_annihiliation_flame_wall") { }

        enum iEvents
        {
            EVENT_MOVE = 1,
        };

        struct npc_arena_of_annihiliation_flame_wallAI : public CreatureAI
        {
            npc_arena_of_annihiliation_flame_wallAI(Creature* creature) : CreatureAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(WorldObject* summoner) override
            {
                instance = me->GetInstanceScript();
                me->SetUnitFlag(UNIT_FLAG_PACIFIED);
                me->SetReactState(REACT_PASSIVE);
            }

            void DoAction(int32 actionId) override 
            {
                events.ScheduleEvent(EVENT_MOVE, 2s);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    events.ScheduleEvent(EVENT_MOVE, 2s);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_MOVE)
                    {
                     //   if (Unit* Liuyang = ObjectAccessor::GetUnit(*me, instance ? instance->GetData64(NPC_LITTLE_LIUYANG) : 0))
                        //    me->GetMotionMaster()->MovePoint(0, GetCoaxingSpiritSpawnPos(Position::NormalizeOrientation(Liuyang->GetAngle(me) + M_PI / 6), Liuyang->GetGUID(), GetCircleRange(me->GetGUID(), Liuyang->GetGUID())).GetPositionX(), GetCoaxingSpiritSpawnPos(Position::NormalizeOrientation(Liuyang->GetAngle(me) + M_PI / 6), Liuyang->GetGUID(), GetCircleRange(me->GetGUID(), Liuyang->GetGUID())).GetPositionY(), GetCoaxingSpiritSpawnPos(Position::NormalizeOrientation(Liuyang->GetAngle(me) + M_PI / 6), Liuyang->GetGUID(), GetCircleRange(me->GetGUID(), Liuyang->GetGUID())).GetPositionZ());
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_flame_wallAI(creature);
        }
};

// Chagan Firehoof 63318
class npc_arena_of_annihiliation_chagan_firehoof : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_chagan_firehoof() : CreatureScript("npc_arena_of_annihiliation_chagan_firehoof") { }

        enum iSpells
        {
            SPELL_TRAILBLAZE_CHARGE = 123969,
            SPELL_HAMMER_TIME       = 123974,
            SPELL_TRIALBLAZE_AUR    = 123977,
        };

        enum iEvents
        {
            EVENT_TRAILBLAZE  = 1,
            EVENT_HAMMER_TIME = 2,
        };

        struct npc_arena_of_annihiliation_chagan_firehoofAI : public BossAI
        {
            npc_arena_of_annihiliation_chagan_firehoofAI(Creature* creature) : BossAI(creature, DATA_CHAGAN) { }

            uint64 targetGUID;
            bool HasEnrage;

            void Reset() override
            {
                _Reset();
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith(who);
                events.ScheduleEvent(EVENT_TRAILBLAZE,18s);
                events.ScheduleEvent(EVENT_HAMMER_TIME,9s);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        me->SetVisible(true);

                        if (TempSummon* Batu = me->SummonCreature(NPC_BATU, *me, TEMPSUMMON_MANUAL_DESPAWN))
                            me->EnterVehicle(0);
                        break;
                    case ACTION_REMOVE_VEHICLE:
                        me->ExitVehicle();
                        me->GetMotionMaster()->MoveJump(ArenaPos.GetPositionX() + frand(3.0f, 6.0f), ArenaPos.GetPositionY(), ArenaPos.GetPositionZ(), 20.0f, 10.0f, EVENT_JUMP);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                        me->SetHomePosition(*me);
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_TRAILBLAZE)
                    summon->CastSpell(summon, SPELL_TRIALBLAZE_AUR, false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

               // if (instance)
                  //  instance->SendScenarioProgressUpdate(CriteriaProgressData(invChallengerType.find(me->GetEntry())->second, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));

                if (Creature* Batu = GetClosestCreatureWithEntry(me, NPC_BATU, 150.0f, true))
                    Batu->DespawnOrUnsummon(2s);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TRAILBLAZE:
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_TRAILBLAZE_CHARGE, false);
                            else if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_TRAILBLAZE, false);

                            events.ScheduleEvent(EVENT_TRAILBLAZE, 17s);
                            break;
                        case EVENT_HAMMER_TIME:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_HAMMER_TIME, false);

                            events.ScheduleEvent(EVENT_HAMMER_TIME, 10s);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_chagan_firehoofAI(creature);
        }
};

// Batu 63872
class npc_arena_of_annihiliation_batu : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_batu() : CreatureScript("npc_arena_of_annihiliation_batu") { }

        enum iEvents
        {
            EVENT_YAK_BASH = 1,
        };

        enum iSpells
        {
            SPELL_YAK_BASH = 120077,
        };

        struct npc_arena_of_annihiliation_batuAI : public ScriptedAI
        {
            npc_arena_of_annihiliation_batuAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            uint64 targetGUID;

            void IsSummonedBy(WorldObject* summoner) override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                instance = me->GetInstanceScript();
                me->GetMotionMaster()->MovePoint(1, ArenaPos);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_YAK_BASH, 8s);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    me->SetHomePosition(*me);

                  //  if (Creature* chagan = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(NPC_CHAGAN_FIREHOOF) : 0))
                   //     chagan->AI()->DoAction(ACTION_REMOVE_VEHICLE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_YAK_BASH)
                    {
                        if (Unit* vict = me->GetVictim())
                            me->CastSpell(vict, SPELL_YAK_BASH, false);

                        events.ScheduleEvent(EVENT_YAK_BASH, 9s);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_batuAI(creature);
        }
};

// Cloudbender Kobo, Satay Buy, Maki Waterblade 63316, 64281, 64280
class npc_arena_of_annihiliation_assassin : public CreatureScript
{
    public:
        npc_arena_of_annihiliation_assassin() : CreatureScript("npc_arena_of_annihiliation_assassin") { }

        enum iEvents
        {
            EVENT_SPEED_OF_JINJA = 1,
            EVENT_CYCLONE_KICK,
            EVENT_SERPENT_KICK,
            EVENT_TWISTER,
            EVENT_CRASHING_SLASH,
            EVENT_WHIRLPOOL,
        };

        enum iSpells
        {
            SPELL_SPEED_OF_JINJA          = 125510,
            SPELL_CYCLONE_KICK            = 125579,
            SPELL_FLYING_SERPENT_KICK     = 127806,
            SPELL_TWISTER                 = 131683, // not proc from serpent kick
            SPELL_TWISTER_AUR             = 131693, // periodic dmg + wind aura
            SPELL_CRASHING_SLASH          = 125568,
            SPELL_WHIRLPOOL               = 125564,
            SPELL_WHIRLPOOL_EFF           = 125566,
        };

        struct npc_arena_of_annihiliation_assassinAI : public BossAI
        {
            npc_arena_of_annihiliation_assassinAI(Creature* creature) : BossAI(creature, DATA_ASSASSIN) { }

            uint64 targetGUID;
            bool HasEnrage;

            void Reset() override
            {
                _Reset();
                events.Reset();
                targetGUID = 0;
            }

            void IsSummonedBy(WorldObject* summoner) override
            {
                if (me->GetEntry() != NPC_MAKI_WATERBLADE)
                    return;

                me->SetReactState(REACT_PASSIVE);
                me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith(who);

                switch (me->GetEntry())
                {
                    case NPC_SATAY_BYU:
                        events.ScheduleEvent(EVENT_SPEED_OF_JINJA, 15s);
                        break;
                    case NPC_CLOUDBENDER_KOBO:
                        events.ScheduleEvent(SPELL_CYCLONE_KICK, 22s);
                        events.ScheduleEvent(EVENT_SERPENT_KICK, 13s);
                        events.ScheduleEvent(EVENT_TWISTER, 11s);
                        break;
                    case NPC_MAKI_WATERBLADE:
                        events.ScheduleEvent(EVENT_WHIRLPOOL, 15s);
                        events.ScheduleEvent(EVENT_CRASHING_SLASH, 6s);
                        break;
                }
            }

            void DoAction(int32 actionId) override
            {
                me->SetVisible(true);
                me->GetMotionMaster()->MovePoint(1, ArenaPos);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_TWISTER)
                {
                    summon->CastSpell(summon, SPELL_TWISTER_AUR, false);
                    summon->GetMotionMaster()->MoveRandom(7.0f);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 1)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    me->SetHomePosition(*me);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_WHIRLPOOL))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SERPENT_KICK:
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_FLYING_SERPENT_KICK, false);
                            else if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_FLYING_SERPENT_KICK, false);
                            break;
                        case EVENT_CYCLONE_KICK:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_CYCLONE_KICK, false);

                            events.ScheduleEvent(SPELL_CYCLONE_KICK, 22s);
                            break;
                        case EVENT_SPEED_OF_JINJA:
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, NonTankTargetSelector(me)))
                                me->GetMotionMaster()->MoveJump(*target, 25.0f, 9.0f, EVENT_JUMP);
                            else if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_SPEED_OF_JINJA, false);

                            events.ScheduleEvent(EVENT_SPEED_OF_JINJA, 15s);
                            break;
                        case EVENT_TWISTER:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_TWISTER, false);

                            events.ScheduleEvent(EVENT_TWISTER, 17s);
                            break;
                        case EVENT_WHIRLPOOL:
                            DoCast(me, SPELL_WHIRLPOOL);
                            events.ScheduleEvent(EVENT_WHIRLPOOL, 40s);
                            break;
                        case EVENT_CRASHING_SLASH:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_CRASHING_SLASH);

                            events.ScheduleEvent(EVENT_CRASHING_SLASH, 7s);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_arena_of_annihiliation_assassinAI(creature);
        }
};

// Stone Spin 123928
class spell_arena_of_annihiliation_stone_spin : public SpellScriptLoader
{
    public:
        spell_arena_of_annihiliation_stone_spin() : SpellScriptLoader("spell_arena_of_annihiliation_stone_spin") { }

        class spell_arena_of_annihiliation_stone_spin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_arena_of_annihiliation_stone_spin_AuraScript);

            void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (GetCaster() && GetCaster()->ToCreature())
                {
                    GetCaster()->ToCreature()->AI()->DoAction(ACTION_REMOVE_SHELL);
                    GetCaster()->CastSpell(GetCaster(), SPELL_DIZZY, false);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_arena_of_annihiliation_stone_spin_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_arena_of_annihiliation_stone_spin_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_arena_of_annihiliation_stone_spin_SpellScript);

            void HandleAfterCast()
            {
                Unit* m_caster = GetCaster();

                if (!m_caster)
                    return;

                do
                {
                    x = m_caster->GetPositionX() + frand(-80.0f, 80.0f); // less random spawn in melee zone
                    y = m_caster->GetPositionY() + frand(-80.0f, 80.0f);
                }
                // then check if our planned pos not in wall/texture.
                // now we`ll check each used position over current.
                while (!HasCorrectRangeForSpin(x, y, m_caster->GetPositionZ(), m_caster->GetGUID()));

                m_caster->SetSpeed(MOVE_RUN, 3.85f);
                m_caster->GetMotionMaster()->MovePoint(2, x, y, m_caster->GetPositionZ());
            }

            private:
                float x, y;

            void Register() override
            {
                AfterCast += SpellCastFn(spell_arena_of_annihiliation_stone_spin_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_arena_of_annihiliation_stone_spin_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_arena_of_annihiliation_stone_spin_AuraScript();
        }
};

// Ill Tempered 123933
class spell_arena_of_annihiliation_ill_tempered : public SpellScriptLoader
{
    public:
        spell_arena_of_annihiliation_ill_tempered() : SpellScriptLoader("spell_arena_of_annihiliation_ill_tempered") { }

        class spell_arena_of_annihiliation_ill_tempered_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_arena_of_annihiliation_ill_tempered_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    // Check target moving.
                    if (owner->isMoving())
                        if (!owner->HasAura(SPELL_ANGRY))
                            owner->CastSpell(owner, SPELL_ANGRY, false);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_arena_of_annihiliation_ill_tempered_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_arena_of_annihiliation_ill_tempered_AuraScript();
        }
};

class HeadbuttTargetSelector
{
    public:
        HeadbuttTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToPlayer() && object->ToPlayer()->HasAura(SPELL_HEADBUTT);
        }
};

// Headbutt 123931
class spell_arena_of_annihiliation_headbutt : public SpellScript
{
    PrepareSpellScript(spell_arena_of_annihiliation_headbutt);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {

                std::list<Player*> PlayersInArea;
                GetPlayerListInGrid(PlayersInArea, caster, 150.0f);
                PlayersInArea.remove_if(HeadbuttTargetSelector());

                if (PlayersInArea.empty())
                    return;

                // We should select new target as victim after headbutt current
                if (Player* itr = Trinity::Containers::SelectRandomContainerElement(PlayersInArea))
                {
                    caster->GetThreatManager().ResetAllThreat();
                    caster->GetThreatManager().AddThreat(itr, 100.0f);
                    caster->AI()->AttackStart(itr);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_arena_of_annihiliation_headbutt::HandleScriptEffect, EFFECT_3, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Slam 123934
class spell_arena_of_annihiliation_slam : public SpellScriptLoader
{
    public:
        spell_arena_of_annihiliation_slam() : SpellScriptLoader("spell_arena_of_annihiliation_slam") { }

        class spell_arena_of_annihiliation_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_arena_of_annihiliation_slam_SpellScript);

            void HandleAfterCast()
            {
                if (GetCaster() && GetCaster()->ToCreature())
                {
                    if (Player* itr = GetCaster()->FindNearestPlayer(150.0f))
                    {
                      //  GetCaster()->ToCreature()->RemoveChanneledCast(itr->GetGUID());
                        GetCaster()->GetMotionMaster()->MoveChase(itr);
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_arena_of_annihiliation_slam_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_arena_of_annihiliation_slam_SpellScript();
        }
};

// FlameLine 123960
class spell_arena_of_annihiliation_flame_line : public AuraScript
{
    PrepareAuraScript(spell_arena_of_annihiliation_flame_line);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (uint32 step = owner->AI()->GetData(TYPE_FLAMELINE))
            {
                GetPositionWithDistInOrientation(owner, (float)step, owner->GetOrientation(), x, y);
               // owner->CastSpell(x, y, owner->GetPositionZ(), SPELL_FLAMELINE_AREA, true);
                step += 4;
                owner->AI()->SetData(TYPE_FLAMELINE, step);
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_arena_of_annihiliation_flame_line::OnTrigger, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Flame Wall 123966
class spell_arena_of_annihiliation_flame_wall : public SpellScriptLoader
{
    public:
        spell_arena_of_annihiliation_flame_wall() : SpellScriptLoader("spell_arena_of_annihiliation_flame_wall") { }

        class spell_arena_of_annihiliation_flame_wall_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_arena_of_annihiliation_flame_wall_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* m_caster = GetCaster())
                    for (uint8 i = 0; i <= 2; ++i)
                        m_caster->SummonCreature(NPC_FLAMECOAXING_SPIRIT, GetCoaxingSpiritSpawnPos(Position::NormalizeOrientation(WALL_DIFF + WALL_DIFF*i), TEMPSUMMON_MANUAL_DESPAWN));
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_arena_of_annihiliation_flame_wall_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_arena_of_annihiliation_flame_wall_SpellScript();
        }
};

// Trailblaze 123969
class spell_arena_of_annihiliation_trailblaze : public SpellScriptLoader
{
    public:
        spell_arena_of_annihiliation_trailblaze() : SpellScriptLoader("spell_arena_of_annihiliation_trailblaze") { }

        class spell_arena_of_annihiliation_trailblaze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_arena_of_annihiliation_trailblaze_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_TRAILBLAZE, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_arena_of_annihiliation_trailblaze_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_arena_of_annihiliation_trailblaze_SpellScript();
        }
};

// Flying Serpent Kick 127806
class spell_arena_of_annihiliation_serpent_kick : public SpellScriptLoader
{
    public:
        spell_arena_of_annihiliation_serpent_kick() : SpellScriptLoader("spell_arena_of_annihiliation_serpent_kick") { }

        class spell_arena_of_annihiliation_serpent_kick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_arena_of_annihiliation_serpent_kick_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_FLYING_SERPENT_KICK_EFF, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_arena_of_annihiliation_serpent_kick_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_arena_of_annihiliation_serpent_kick_SpellScript();
        }
};

// Lava Pool 123967, Flame Wall 123965
class spell_arena_of_annihiliation_lava_pool : public SpellScript
{
    PrepareSpellScript(spell_arena_of_annihiliation_lava_pool);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Player* target = GetHitPlayer())
                caster->GetMap()->SetWorldStateValue(WORLDSTATE_BEAT_THE_HEAT, 0, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_arena_of_annihiliation_lava_pool::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Whirlpool 125564
class spell_arena_of_annihiliation_whirlpool : public AuraScript
{
    PrepareAuraScript(spell_arena_of_annihiliation_whirlpool);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, 125566, true); // smth wrong with default triggering
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_arena_of_annihiliation_whirlpool::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

//// 385. Summoned by 125564 - Whirlpool
//class sat_arena_of_annihiliation_whirlpool : public IAreaTriggerAura
//{
//    Position forceMovementPos;
//
//    bool CheckTriggering(WorldObject* triggering) override
//    {
//        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
//    }
//
//    void OnTriggeringApply(WorldObject* triggering) override
//    {
//        if (Player* target = triggering->ToPlayer())
//        {
//            if (Unit* caster = GetCaster())
//            {
//                forceMovementPos = caster->GetPosition();
//                target->SendApplyMovementForce(true, forceMovementPos, 4.5f);
//            }
//        }
//    }
//
//    /*void OnTriggeringRemove(WorldObject* triggering) override
//    {
//        if (Player* target = triggering->ToPlayer())
//            target->SendApplyMovementForce(false, forceMovementPos);
//    }*/
//};

// Tiger Temple Gong 212974
class go_arena_of_annihiliation_gong : public GameObjectScript
{
    public:
        go_arena_of_annihiliation_gong() : GameObjectScript("go_arena_of_annihiliation_gong") { }

      /*  bool OnGossipHello(Player* player, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();

            if (instance && player)
            {
                instance->SetData(DATA_CALL, IN_PROGRESS);
                go->SetUnitFlag(GO_FLAG_INTERACT_COND);

                instance->SendScenarioProgressUpdate(CriteriaProgressData(CRITERIA_GONG, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));
                return true;
            }

            return false;
        }*/
};

void AddSC_arena_of_annihiliation()
{
    new npc_arena_of_annihiliation_gurgthock();
    new npc_arena_of_annihiliation_scar_shell();
    new npc_arena_of_annihiliation_jol_grum();
    new npc_arena_of_annihiliation_little_liuyang();
    new npc_arena_of_annihiliation_flamecoaxing_spirit();
    new npc_arena_of_annihiliation_flame_wall();
    new npc_arena_of_annihiliation_chagan_firehoof();
    new npc_arena_of_annihiliation_batu();
    new npc_arena_of_annihiliation_assassin();
    new spell_arena_of_annihiliation_stone_spin();
    new spell_arena_of_annihiliation_ill_tempered();
    new spell_arena_of_annihiliation_headbutt();
    new spell_arena_of_annihiliation_slam();
    new spell_arena_of_annihiliation_flame_line();
    new spell_arena_of_annihiliation_flame_wall();
    new spell_arena_of_annihiliation_trailblaze();
    new spell_arena_of_annihiliation_serpent_kick();
    new spell_arena_of_annihiliation_lava_pool();
    new spell_arena_of_annihiliation_whirlpool();
    //new sat_arena_of_annihiliation_whirlpool();
   // new go_arena_of_annihiliation_gong();
}
