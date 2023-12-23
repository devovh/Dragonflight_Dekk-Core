/*
* DekkCore Team Devs
*
*
*
*
*
*/

#include "Zone_BrackenhideHollow.h"
#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "InstanceScript.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "Spell.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"

enum GutshootSpells
{
    SPELL_CALL_HIENAS           = 394540,
    SPELL_ENSNARING_TRAP_VISUAL = 385359,
    SPELL_ENSNARING_TRAP_DAMAGE = 384148,
    SPELL_MEAT_TOSS             = 384416,
    SPELL_MASTER_CALL           = 384633,
    SPELL_GUT_SHOOT_TANK        = 384353,
    SPELL_CRIPPLING_BITE        = 384577,
};


enum GutshotEvents
{   
    EVENT_ENSNARING_TRAP,
    EVENT_GUTSHOOT_TANK,
    EVENT_MASTER_CALL,
    EVENT_MEAT_TOSS,
    EVENT_BITE,
    EVENT_CALL_HIENAS,
};

//186116 gutshoot
class Boss_Gutshoot186116 : public CreatureScript
{
public:
    Boss_Gutshoot186116() : CreatureScript("Boss_Gutshoot186116") {}

    struct Boss_Gutshoot186116AI : public ScriptedAI
    {
        Boss_Gutshoot186116AI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_ENSNARING_TRAP, 8s);
            events.ScheduleEvent(EVENT_GUTSHOOT_TANK, 12s);
            events.ScheduleEvent(EVENT_MASTER_CALL, 16s);
            events.ScheduleEvent(EVENT_MEAT_TOSS, 20s);
            events.ScheduleEvent(EVENT_BITE, 14s);
            events.ScheduleEvent(EVENT_CALL_HIENAS, 30s);
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
                case EVENT_ENSNARING_TRAP:
                    DoCastVictim(SPELL_ENSNARING_TRAP_VISUAL);
                    DoCastVictim(SPELL_ENSNARING_TRAP_DAMAGE);
                    events.ScheduleEvent(EVENT_ENSNARING_TRAP, 8s);
                    break;
                case EVENT_GUTSHOOT_TANK:
                    if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 100.0f, true))
                    {
                        me->CastSpell(target, SPELL_GUT_SHOOT_TANK);
                    }
                    events.ScheduleEvent(EVENT_GUTSHOOT_TANK, 12s);
                    break;
                case EVENT_MASTER_CALL:
                    DoCastVictim(SPELL_MASTER_CALL);
                    events.ScheduleEvent(EVENT_MASTER_CALL, 16s);
                    break;
                case EVENT_MEAT_TOSS:
                    if (Unit* target = SelectTarget(SelectTargetMethod::MinDistance, 0, 100.0f, true))
                    {
                        me->CastSpell(target, SPELL_MEAT_TOSS);
                    }
                    events.ScheduleEvent(EVENT_MEAT_TOSS, 20s);
                    break;
                case EVENT_BITE:
                    DoCastVictim(SPELL_CRIPPLING_BITE);
                    events.ScheduleEvent(EVENT_BITE, 14s);
                    break;
                case EVENT_CALL_HIENAS:
                    DoCastRandom(SPELL_CALL_HIENAS, 50, true, 0);
                    events.ScheduleEvent(EVENT_BITE, 30s);
                    break;
                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new Boss_Gutshoot186116AI(creature);
    }
};

void AddSC_Boss_Gutshoot()
{
    new Boss_Gutshoot186116();
}
