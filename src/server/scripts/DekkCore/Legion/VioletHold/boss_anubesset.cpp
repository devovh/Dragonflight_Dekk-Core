#include "violet_hold_assault.h"

enum Spells
{
    SPELL_MANDIBLE_STRIKE = 202217,
    SPELL_IMPALE = 202341,
    SPELL_IMPALE_TRIG = 202350,
    SPELL_CALL_OF_THE_SWARM = 201863,

    //Spitting Scarab
    SPELL_POISON_SPIT = 202300,
    SPELL_BURROW = 202304,

    //Blistering Beetle
    SPELL_BURROW_VISUAL = 202496,
    SPELL_FIXATE = 202480,
    SPELL_BLISTERING_OOZE_AT = 202483,
    SPELL_BLISTERING_OOZE_AT_2 = 202487,
    SPELL_BLISTERING_OOZE_DMG = 202485,
    SPELL_SUICIDE = 117624,

    //Heroic+
    SPELL_SUM_BEETLE_FILTER = 202526,
    SPELL_SUM_OOZING_BEETLE = 202513,
};

enum eEvents
{
    EVENT_MANDIBLE_STRIKE = 1,
    EVENT_IMPALE = 2,
    EVENT_CALL_OF_THE_SWARM = 3,
    EVENT_SUMMON_OOZING_BEETLE = 4,

    EVENT_1,
    EVENT_2,
};

//102246
class boss_anubesset : public CreatureScript
{
public:
    boss_anubesset() : CreatureScript("boss_anubesset") {}

    struct boss_anubessetAI : public BossAI
    {
        boss_anubessetAI(Creature* creature) : BossAI(creature, DATA_ANUBESSET)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            removeloot = false;
        }

        std::list<ObjectGuid> listGuid;
        bool removeloot;

        void Reset() override
        {
            _Reset();
        }

        void JustEngagedWith(Unit* who) override
            //01:41
        {
            _JustEngagedWith(who);

            events.RescheduleEvent(EVENT_MANDIBLE_STRIKE, 8s);    //01:49, 02:20, 02:43
            events.RescheduleEvent(EVENT_IMPALE, 18s);            //01:59, 02:22
            events.RescheduleEvent(EVENT_CALL_OF_THE_SWARM, 30s); //02:11, 03:13

            if (me->GetMap()->GetDifficultyID() != DIFFICULTY_NORMAL)
                events.RescheduleEvent(EVENT_SUMMON_OOZING_BEETLE, 30s);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();

            if (removeloot)
                me->RemoveDynamicFlag(UNIT_DYNFLAG_LOOTABLE);
        }

        bool GetObjectData(ObjectGuid const& guid)
        {
            bool find = false;

            for (auto targetGuid : listGuid)
                if (targetGuid == guid)
                    find = true;

            if (!find)
                listGuid.push_back(guid);

            return find;
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_REMOVE_LOOT)
                removeloot = true;
        }

        void SpellHitTarget(WorldObject* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_IMPALE)
            {
                Position pos;
                float dist = 1.0f;
                for (uint16 i = 0; i < 120; i++)
                {
                    float angle = frand(-0.2f, 0.2f);
                    me->GetNearPosition(dist, angle);
                    me->CastSpell(pos, SPELL_IMPALE_TRIG, true);
                    dist += 2;
                }
            }
            if (spell->Id == SPELL_SUM_BEETLE_FILTER)
            {
                Position pos;
                target->GetNearPosition(7.0f, frand(0.0f, 3.0f));
                me->CastSpell(pos, SPELL_SUM_OOZING_BEETLE, true);
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
                case EVENT_MANDIBLE_STRIKE:
                    DoCastVictim(SPELL_MANDIBLE_STRIKE);
                    events.RescheduleEvent(EVENT_MANDIBLE_STRIKE, 23s);
                    break;
                case EVENT_IMPALE:
                    listGuid.clear();
                    if (Unit* pTarget = SelectTarget(SelectTargetMethod::Random, 0, 80.0f, true))
                        DoCast(pTarget, SPELL_IMPALE);
                    events.RescheduleEvent(EVENT_IMPALE, 23s);
                    break;
                case EVENT_CALL_OF_THE_SWARM:
                    DoCast(SPELL_CALL_OF_THE_SWARM);
                    events.RescheduleEvent(EVENT_CALL_OF_THE_SWARM, 60s);
                    break;
                case EVENT_SUMMON_OOZING_BEETLE:
                    DoCast(me, SPELL_SUM_BEETLE_FILTER, true);
                    events.RescheduleEvent(EVENT_SUMMON_OOZING_BEETLE, 16s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_anubessetAI(creature);
    }
};

//102271
class npc_anubesset_spitting_scarab : public CreatureScript
{
public:
    npc_anubesset_spitting_scarab() : CreatureScript("npc_anubesset_spitting_scarab") {}

    struct npc_anubesset_spitting_scarabAI : public ScriptedAI
    {
        npc_anubesset_spitting_scarabAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void Reset() override
        {
            events.Reset();
            events.RescheduleEvent(EVENT_1, 2s);
            events.RescheduleEvent(EVENT_2, 8s);
        }

        void IsSummonedBy(WorldObject* summoner) override
        {
            DoZoneInCombat();
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
                case EVENT_1:
                    if (Unit* pTarget = SelectTarget(SelectTargetMethod::Random, 0, 80.0f, true))
                        DoCast(pTarget, SPELL_POISON_SPIT);
                    events.RescheduleEvent(EVENT_1, 2s);
                    break;
                case EVENT_2:
                {
                    Position pos = me->GetRandomNearPosition(40.0f);
                    me->CastSpell(pos, SPELL_BURROW, TRIGGERED_FULL_MASK);
                    events.RescheduleEvent(EVENT_2, 16s);
                    break;
                }
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_anubesset_spitting_scarabAI(creature);
    }
};

//102540
class npc_anubesset_blistering_beetle : public CreatureScript
{
public:
    npc_anubesset_blistering_beetle() : CreatureScript("npc_anubesset_blistering_beetle") {}

    struct npc_anubesset_blistering_beetleAI : public ScriptedAI
    {
        npc_anubesset_blistering_beetleAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        EventMap events;

        void Reset() override {}

        void IsSummonedBy(WorldObject* summoner) override
        {
            events.RescheduleEvent(EVENT_1, 5s);
            events.RescheduleEvent(EVENT_2, 2s);
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (me->GetOwner())
                if (Unit* summoner = me->GetOwner())
                    me->CastSpell(me, SPELL_BLISTERING_OOZE_AT_2, true);
        }

        bool checkPlayers()
        {
        //    std::list<HostileReference*> threatList = me->getThreatManager().getThreatList();
          //  if (threatList.size() > 1)
           //     return true;

            return false;
        }

       void OnSpellCast(SpellInfo const* spell) override
        {
            switch (spell->Id)
            {
            case SPELL_BLISTERING_OOZE_DMG:
            {
                if (me->GetOwner())
                    if (Unit* summoner = me->GetOwner())
                    {
                        me->CastSpell(me, SPELL_BLISTERING_OOZE_AT_2, true);
                        DoCast(me, SPELL_SUICIDE, true);
                    }
                break;
            }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_1:
                    DoCast(me, SPELL_BLISTERING_OOZE_AT, true);
                    break;
                case EVENT_2:
                {
                    if (me->GetOwner())
                        if (Unit* summoner = me->GetOwner())
                        {
                            if (Unit* target = summoner->GetAI()->SelectTarget(SelectTargetMethod::Random, checkPlayers(), 80.0f, true, true, -SPELL_FIXATE))
                            {
                                DoCast(target, SPELL_FIXATE, true);
                                me->GetMotionMaster()->MovePoint(1, target->GetPosition());
                            }
                            else if (Unit* target2 = summoner->GetAI()->SelectTarget(SelectTargetMethod::Random, checkPlayers(), 80.0f, true))
                                me->GetMotionMaster()->MovePoint(1, target2->GetPosition());
                        }
                    events.RescheduleEvent(EVENT_2, 1s);
                    break;
                }
                default:
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_anubesset_blistering_beetleAI(creature);
    }
};

//202353
class spell_anubesset_impale_filter : public SpellScriptLoader
{
public:
    spell_anubesset_impale_filter() : SpellScriptLoader("spell_anubesset_impale_filter") { }

    class spell_anubesset_impale_filter_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_anubesset_impale_filter_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (Unit* caster = GetCaster())
            {
                for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    targets.remove(*itr++);
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anubesset_impale_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anubesset_impale_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_anubesset_impale_filter_SpellScript();
    }
};

void AddSC_boss_anubesset()
{
    new boss_anubesset();
    new npc_anubesset_spitting_scarab();
    new npc_anubesset_blistering_beetle();
    new spell_anubesset_impale_filter();
}
