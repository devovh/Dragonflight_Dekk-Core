/* This is an example mob script for multiple creatures using multiple spells based on creature entry. Can use more spell for one creature too see Fel Weaver */


// Basic mob script for Mardum:
// 95226 Anguish Jailer, 93112 Felguard Sentry, 93115 Foul Felstalker, 94654 Doomguard Eradicator,
// 99759 Fiendish Creeper, 97706 Fel Weaver
class basic_spellcaster_mob_mardum : public CreatureScript
{
public:
    basic_spellcaster_mob_mardum() : CreatureScript("basic_spellcaster_mob_mardum") { }

    struct basic_spellcaster_mob_mardumAI : public ScriptedAI
    {
        basic_spellcaster_mob_mardumAI(Creature* creature) : ScriptedAI(creature) {}

        enum Spells
        {
            SPELL_ANGUISHED_SOUL = 200502,
            SPELL_BLAZING_BLADE = 200570,
            SPELL_FOUL_FEL = 200417,
            SPELL_SHADOWFLAME = 200608,
            SPELL_DEMON_SPIT = 200782,
            SPELL_FEL_WEAVING = 200748,
            SPELL_LOB_POISON = 200751,
        };

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* who) override
        {
            events.RescheduleEvent(1, 3s);
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
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                        switch (me->GetEntry())
                        {
                        case 95226:
                            DoCast(target, SPELL_ANGUISHED_SOUL);
                            break;
                        case 93112:
                            DoCast(target, SPELL_BLAZING_BLADE);
                            break;
                        case 93115:
                            DoCast(me, SPELL_FOUL_FEL);
                            break;
                        case 94654:
                            DoCast(target, SPELL_SHADOWFLAME);
                            break;
                        case 99759:
                            DoCast(target, SPELL_DEMON_SPIT);
                            break;
                        case 97706:
                            urand(1, 2) == 1 ? DoCast(target, SPELL_FEL_WEAVING) : DoCast(target, SPELL_LOB_POISON);
                            break;
                        }
                    events.RescheduleEvent(1, 3s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }


    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new basic_spellcaster_mob_mardumAI(creature);
    }
};
