#include "ScriptedEscortAI.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scarlet_halls.h"

enum Spells
{
    SPELL_COSMETIC_FLAME = 126645,
    SPELL_PYROBLAST = 113690,
    SPELL_QUICKENED_MIND = 113682,
    SPELL_FIREBALL_VOLLEY = 113691,
    SPELL_TELEPORT = 113626,
    SPELL_DRAGON_BREATH = 113641,
    SPELL_BOOK_BURNER = 113366,
    SPELL_BURNING_BOOKS = 113616,
};

enum Says
{
    SAY_ENTER_PLAYER = 0,
    SAY_AGGRO = 1,
    SAY_BOOK_BURN = 2,
    SAY_BREATH_WARNING = 3,
    SAY_BREATH = 4,
    SAY_DEATH = 5,
};

enum eEvants
{
    ACTION_INTRO = 1,

    EVENT_INTRO = 2,
    EVENT_PYROBLAST = 3,
    EVENT_QUICKENED_MIND = 4,
    EVENT_FIREBALL_VOLLEY = 5,
    EVENT_TELEPORT = 6,
    EVENT_BREATH_START = 7,
    EVENT_BREATH_FINISH = 8,
    EVENT_BOOK_BURNER = 9,
};

class boss_flameweaver_koegler : public CreatureScript
{
public:
    boss_flameweaver_koegler() : CreatureScript("boss_flameweaver_koegler") {}

    struct boss_flameweaver_koeglerAI : public BossAI
    {
        boss_flameweaver_koeglerAI(Creature* creature) : BossAI(creature, DATA_KOEGLER)
        {
            instance = me->GetInstanceScript();
            intro = true;
        }

        InstanceScript* instance;
        EventMap events;

        uint32 Check_Timer;

        bool Orient;
        bool intro;

        void Reset()
        {
            Orient = false;
            Check_Timer = 500;
            events.Reset();
            events.RescheduleEvent(EVENT_INTRO, 1s);
            summons.DespawnAll();
            me->RemoveAllAuras();
            me->SetReactState(REACT_AGGRESSIVE);
            _Reset();
        }

        void JustEngagedWith(Unit* who) override
        {
            Talk(SAY_AGGRO);
            events.CancelEvent(EVENT_INTRO);
            events.RescheduleEvent(EVENT_PYROBLAST, 4s);
            events.RescheduleEvent(EVENT_QUICKENED_MIND, 8s);
            events.RescheduleEvent(EVENT_FIREBALL_VOLLEY, 14s);
            events.RescheduleEvent(EVENT_TELEPORT, 30s);
            events.RescheduleEvent(EVENT_BOOK_BURNER, 22s);
            _JustEngagedWith(who);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
        }

        void DoAction(const int32 action) override
        {
            if (action == ACTION_INTRO && intro)
            {
                intro = false;
                Talk(SAY_ENTER_PLAYER);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim() && me->IsInCombat())
                return;

            if (me->GetDistance(me->GetHomePosition()) > 30.0f)
            {
                EnterEvadeMode();
                return;
            }

            events.Update(diff);

            if (Orient)
            {
                if (Check_Timer <= diff)
                {
                    if (Unit* target = me->FindNearestCreature(NPC_DRAGON_BREATH_TARGET, 30.0f))
                        me->SetFacingToObject(target);

                    Check_Timer = 500;
                }
                else Check_Timer -= diff;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_INTRO:
                    DoCast(SPELL_COSMETIC_FLAME);
                    events.RescheduleEvent(EVENT_INTRO, 5s);
                    break;
                case EVENT_PYROBLAST:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_PYROBLAST);
                    events.RescheduleEvent(EVENT_PYROBLAST, 1s);
                    break;
                case EVENT_QUICKENED_MIND:
                    DoCast(SPELL_QUICKENED_MIND);
                    events.RescheduleEvent(EVENT_QUICKENED_MIND, 30s);
                    break;
                case EVENT_FIREBALL_VOLLEY:
                    DoCast(SPELL_FIREBALL_VOLLEY);
                    events.RescheduleEvent(EVENT_FIREBALL_VOLLEY, 30s);
                    break;
                case EVENT_TELEPORT:
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->SummonCreature(NPC_DRAGON_BREATH_TARGET, 1279.58f, 549.58f, 12.90f);
                    DoCast(SPELL_TELEPORT);
                    Talk(SAY_BREATH);
                    events.RescheduleEvent(EVENT_TELEPORT, 46s);
                    events.RescheduleEvent(EVENT_BREATH_START, 1s);
                    break;
                case EVENT_BREATH_START:
                    Talk(SAY_BREATH_WARNING);
                    Orient = true;
                    DoCast(SPELL_DRAGON_BREATH);
                    events.RescheduleEvent(EVENT_BREATH_FINISH, 1s);
                    break;
                case EVENT_BREATH_FINISH:
                    Orient = false;
                    me->SetReactState(REACT_AGGRESSIVE);
                    break;
                case EVENT_BOOK_BURNER:
                    Talk(SAY_BOOK_BURN);
                    DoCast(SPELL_BOOK_BURNER);
                    events.RescheduleEvent(EVENT_BOOK_BURNER, 32s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_flameweaver_koeglerAI(creature);
    }
};

class npc_dragon_breath_target : public CreatureScript
{
public:
    npc_dragon_breath_target() : CreatureScript("npc_dragon_breath_target") {}

    struct npc_dragon_breath_targetAI : public EscortAI
    {
        npc_dragon_breath_targetAI(Creature* creature) : EscortAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            InitWaypoint();
        }

        InstanceScript* instance;

        void InitWaypoint()
        {
            AddWaypoint(0, 1284.60f, 563.04f, 13.85f);
            AddWaypoint(1, 1298.03f, 568.60f, 13.85f);
            AddWaypoint(2, 1311.47f, 563.04f, 13.85f);
            AddWaypoint(3, 1317.03f, 549.60f, 13.85f);
            AddWaypoint(4, 1311.47f, 536.17f, 13.85f);
            AddWaypoint(5, 1298.03f, 530.60f, 13.85f);
            AddWaypoint(6, 1284.60f, 536.17f, 13.85f);
            AddWaypoint(7, 1279.03f, 549.60f, 13.85f);
        }

        void SpellHitTarget(WorldObject* target, SpellInfo const* spell) override
        {
            if (!instance)
                return;

            if (spell->Id == SPELL_DRAGON_BREATH)
            {
                Start(false);
            }
        }

        void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
        {
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragon_breath_targetAI(creature);
    }
};

class npc_book_case : public CreatureScript
{
public:
    npc_book_case() : CreatureScript("npc_book_case") {}

    struct npc_book_caseAI : public ScriptedAI
    {
        npc_book_caseAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void SpellHitTarget(WorldObject* target, SpellInfo const* spell) override
        {
            if (!instance)
                return;

            if (spell->Id == SPELL_BOOK_BURNER)
                me->CastSpell(me, SPELL_BURNING_BOOKS, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_book_caseAI(creature);
    }
};

class at_koegler_enter_room : public AreaTriggerScript
{
public:
    at_koegler_enter_room() : AreaTriggerScript("at_koegler_enter_room") { }

    bool OnTrigger(Player* pPlayer, AreaTriggerEntry const* trigger) override
    {
        if (pPlayer->IsGameMaster())
            return false;

        if (InstanceScript* instance = pPlayer->GetInstanceScript())
        {
            if (Creature* koegler = instance->instance->GetCreature(instance->GetGuidData(NPC_FLAMEWEAVER_KOEGLER)))
                koegler->AI()->DoAction(ACTION_INTRO);
        }
        return true;
    }
};

void AddSC_flameweaver_koegler()
{
    new boss_flameweaver_koegler();
    new npc_dragon_breath_target();
    new npc_book_case();
    new at_koegler_enter_room();
}
