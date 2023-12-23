/*
* DekkCore
*
*
*
*
* 
*/

#include "ScriptMgr.h"

enum Spells
{
    SPELL_CONTROLLED_CALAMITY = 361201,
    SPELL_BANISHMENT_MARK     = 361632,
};

enum Quests
{
    QUEST_ANTROS_SLAIN = 66619,
};

//182466
class boss_antros : public CreatureScript
{
public:
    boss_antros() : CreatureScript("boss_antros") { }

    struct boss_antrosAI : public ScriptedAI
    {
        boss_antrosAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetUnitFlag(UnitFlags(UNIT_FLAG_REMOVE_CLIENT_CONTROL));
        }

        void JustEngagedWith(Unit* unit) override
        {
            events.RescheduleEvent(1, 8s); // SPELL_CONTROLLED_CALAMITY
            events.RescheduleEvent(2, 10s); // SPELL_BANISHMENT_MARK
        }

        void JustDied(Unit* who) override
        {
            if (Player* player = who->ToPlayer())
            if (player->hasQuest(QUEST_ANTROS_SLAIN))
                player->ForceCompleteQuest(QUEST_ANTROS_SLAIN);
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
                    events.RescheduleEvent(1, 22s);
                    break;
                case 2:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_antrosAI(creature);
    }
};

void AddSC_boss_antros()
{
    new boss_antros();
}
