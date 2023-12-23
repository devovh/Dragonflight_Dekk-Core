#include "ScriptMgr.h"
#include "the_motherlode.h"
#include "AreaTrigger.h"
#include "SpellAuraDefines.h"
#include "SpellAuraEffects.h"
#include "AreaTriggerAI.h"
#include <sstream>

enum Spells
{
    SPELL_CHEMICAL_BURN_MISSILE = 259856,

    SPELL_AZERITE_CATALYST_AURA = 259533,
    SPELL_AZERITE_CATALYST_MISSILE = 259327,
    SPELL_AZERITE_CATALYST_AT = 259023, // 259787 ?! or this

    SPELL_PROPELLANT_BLAST_PUSHBACK = 270075,
    SPELL_PROPELLANT_BLAST_AURA = 260103,
    SPELL_PROPELLANT_BLAST	= 260669, // also AT

	SPELL_TOXIC_SLUDGE = 269831,
    SPELL_SEARING_REAGENT	= 280497,
	
};

enum Events
{
    EVENT_PROPELLANT_BLAST = 1,
    EVENT_CHEMICAL_BURN,
    EVENT_AZERITE_CATALYST,
    EVENT_SEARING_REAGENT,

    EVENT_CATALYST_PATCHES,
};

enum Timers
{
    TIMER_PROPELLANT_BLAST = 20 * IN_MILLISECONDS,
    TIMER_CHEMICAL_BURN = 30 * IN_MILLISECONDS,
    TIMER_SEARING_REAGENT = 2 * IN_MILLISECONDS,
    TIMER_AZERITE_CATALYST = 12 * IN_MILLISECONDS,

    TIMER_CATALYST_PATCHES = 40 * IN_MILLISECONDS,
};

enum Creatures
{
    BOSS_RIXXA_FLUXFLAME = 129231,

    NPC_CATALYST_PIPE_STALKER = 141160,
    NPC_SPRAY_STALKER = 137452,
};

const Position centerPos = { 1273.14f, -3698.62f, 26.71f }; //40 y

#define AGGRO_TEXT "If you want a job done right... use bigger explosives!"
#define AZERITE_CATALYST "Lookin' for Azerite? Have a face full!"
#define PROPELLANT_BLAST "Safety first!"
#define DEATH_TEXT "I shoulda... gotten... hazzard pay.."

// pusback movement force, hardcoded atm and areatrigger visual not working so mechanic won't work either

class bfa_boss_rixxa_fluxflame : public CreatureScript
{
public:
    bfa_boss_rixxa_fluxflame() : CreatureScript("bfa_boss_rixxa_fluxflame")
    {
    }
    struct bfa_boss_rixxa_fluxflame_AI : public BossAI
    {
        bfa_boss_rixxa_fluxflame_AI(Creature* creature) : BossAI(creature, DATA_RIXXA_FLUXFLAME)
        {
            instance = me->GetInstanceScript();
        }

        EventMap events;
        InstanceScript* instance;

        void Reset() override
        {
            events.Reset();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void SelectSoundAndText(Creature* me, uint32  selectedTextSound = 0)
        {
            if (!me)
                return;

            if (me)
            {
                switch (selectedTextSound)
                {
                case 1:
                    me->Yell(AGGRO_TEXT, LANG_UNIVERSAL, NULL);
                    break;
                case 2:
                    me->Yell(DEATH_TEXT, LANG_UNIVERSAL, NULL);
                    break;
                case 3:
                    me->Yell(PROPELLANT_BLAST, LANG_UNIVERSAL, NULL);
                    break;
                case 4:
                    me->Yell(AZERITE_CATALYST, LANG_UNIVERSAL, NULL);
                    break;
                }
            }
        }

        void JustDied(Unit*) override
        {
            SelectSoundAndText(me, 2);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterEvadeMode(EvadeReason w) override
        {
            _DespawnAtEvade(15s);
        }

        void JustEngagedWith(Unit*) override
        {
            if (me->GetMap()->IsHeroic() || me->GetMap()->IsMythic())
                events.ScheduleEvent(EVENT_CATALYST_PATCHES, 15s);
            SelectSoundAndText(me, 1);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            events.ScheduleEvent(EVENT_PROPELLANT_BLAST, 17s);
            events.ScheduleEvent(EVENT_AZERITE_CATALYST, 22s);
            events.ScheduleEvent(EVENT_CHEMICAL_BURN, 30s);
            events.ScheduleEvent(EVENT_SEARING_REAGENT, 14s);
        }

        void HandlePropellantBlast() // movement force is not currently working so.. we do the pushback spell until then
        {
            std::list<Player*> PlayerList;
            GetPlayerListInGrid(PlayerList, me, 80.0f);
            for (auto player : PlayerList)
                if (me->isInFront(player, 3.14f / 2.0f))
                {
                    me->CastSpell(player, SPELL_PROPELLANT_BLAST_PUSHBACK, true);
                    if (Aura* propAura = me->AddAura(SPELL_PROPELLANT_BLAST_AURA, player))
                    {
                        propAura->SetMaxDuration(2000);
                        propAura->SetDuration(2000);
                    }
                }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!UpdateVictim())
                return;

            if (me->HasAura(SPELL_PROPELLANT_BLAST))
                HandlePropellantBlast();

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_PROPELLANT_BLAST:
                {
                    SelectSoundAndText(me, 3);
                    std::ostringstream str;
                    str << "Rixxa Fluxflame is casting |cFFF00000|h[Propellant Blast]|h|r !";
                    me->TextEmote(str.str().c_str(), 0, true);

                    std::list<Player*> playerList;
                    me->GetPlayerListInGrid(playerList, 50.0f);

                    if (!playerList.empty())
                    {
                        if (playerList.size() >= 1)
                            playerList.resize(1);

                        for (auto player : playerList)
                            me->CastSpell(player, SPELL_PROPELLANT_BLAST);
                    }
                    break;
                }
                case EVENT_CHEMICAL_BURN:
                {
                    std::list<Player*> playerList;
                    me->GetPlayerListInGrid(playerList, 50.0f);

                    if (!playerList.empty())
                    {
                        if (playerList.size() >= 2)
                            playerList.resize(2);

                        for (auto player : playerList)
                            me->CastSpell(player, SPELL_CHEMICAL_BURN_MISSILE);
                    }

                    events.ScheduleEvent(EVENT_CHEMICAL_BURN, 15s);
                    break;
                }
                case EVENT_AZERITE_CATALYST:
                    // no visual yet, PENDING
                    break;
                case EVENT_CATALYST_PATCHES:
                    // no visual yet, PENDING
                    break;
                case EVENT_SEARING_REAGENT:
                    if(Unit* target = me->GetVictim())
                        me->CastSpell(target, SPELL_SEARING_REAGENT);
                    events.ScheduleEvent(EVENT_SEARING_REAGENT, 13s);
                    break;
                }
            }
            //no melee
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_boss_rixxa_fluxflame_AI(creature);
    }
};

void AddSC_boss_rixxafluxflame()
{
    new bfa_boss_rixxa_fluxflame();
}
