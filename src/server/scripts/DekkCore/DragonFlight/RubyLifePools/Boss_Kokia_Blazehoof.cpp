/*
* DekkCore Team Devs
*
*
*
*
*
*/

#include "ruby_life_pools.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "Spell.h"
#include "Player.h"
#include "ScriptedCreature.h"

enum kokiaSpells
{
    SPELL_MOLTEN_BOULDER_1       = 372107,
    SPELL_MOLTEN_BOULDER_2       = 372811,
    SPELL_RITUAL_OF_BLAZEBINDING = 372863,
    SPELL_SEARING_BLOWS          = 372858,
    SPELL_SEARING_WOUNDS         = 372860,
    SPELL_ALTERED_PSYCHE         = 344663
};

enum kokiaEvents
{
    EVENT_FIREBALL,
    EVENT_RITUAL_OF_BLAZEBINDING,
    EVENT_SEARING_BLOWS,
    EVENT_SEARING_WOUNDS,
};

//189232
class boss_kokia_blazehoof : public CreatureScript
{
public:
    boss_kokia_blazehoof() : CreatureScript("boss_kokia_blazehoof") { }

    struct boss_kokia_blazehoofAI : public BossAI
    {
        boss_kokia_blazehoofAI(Creature* creature) : BossAI(creature, BOSS_KOKIA_BLAZEHOOF)
        {
            Initialize();
        }

        void Initialize()
        {       
        }


        void JustEngagedWith(Unit* who) override
        {
            _JustEngagedWith(who);
            DoZoneInCombat();
            me->Yell("You dare challenge me? Prepare to be burned to ashes!", LANG_UNIVERSAL, NULL);
            events.ScheduleEvent(EVENT_FIREBALL, 4s);
            events.ScheduleEvent(EVENT_RITUAL_OF_BLAZEBINDING, 15s);
            events.ScheduleEvent(EVENT_SEARING_BLOWS, 7s);
            events.ScheduleEvent(EVENT_SEARING_WOUNDS, 9s);
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
                case EVENT_FIREBALL:
                    DoCastVictim(SPELL_MOLTEN_BOULDER_1);
                    events.ScheduleEvent(EVENT_FIREBALL, 9s);
                    break;

                case EVENT_RITUAL_OF_BLAZEBINDING:
                    for (int i = 0; i < 3; i++)
                    {
                        Position pos;
                        GetPositionWithDistInOrientation(me, 5.0f, me->GetOrientation(), pos);
                        me->SummonCreature(NPC_BLAZEBOUND_FIRESTORM, pos, TEMPSUMMON_CORPSE_DESPAWN);
                    }
                    events.ScheduleEvent(EVENT_RITUAL_OF_BLAZEBINDING, 19s);
                    break;

                case EVENT_SEARING_BLOWS:
                    DoCastVictim(SPELL_SEARING_BLOWS);
                    events.ScheduleEvent(EVENT_SEARING_BLOWS, 10s);
                    break;

                case EVENT_SEARING_WOUNDS:
                    DoCastVictim(SPELL_SEARING_WOUNDS);
                    events.ScheduleEvent(EVENT_SEARING_WOUNDS, 15s);
                    break;

                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer) override
        {
            me->Yell("You may have defeated me, but my flames will continue to burn!", LANG_UNIVERSAL, NULL);
            events.Reset();
            me->RemoveGameObjectByEntry(GO_FIRE_WALL, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_kokia_blazehoofAI(creature);
    }
};


void AddSC_Boss_kokia_blazehoof()
{
    new boss_kokia_blazehoof();
};

