#include "ScriptMgr.h"
#include "the_motherlode.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"

enum Spells
{
    SPELL_RAGING_GAZE_BEAM = 257582,
    SPELL_RAGING_GAZE_SELECTOR = 271526,
    SPELL_CALL_EARTHRAGER 	= 257593,
    SPELL_AZERITE_INFUSION_MISSILE = 271698,
    SPELL_JAGGED_CUT = 257543,
    SPELL_RESONANT_PULSE = 258622,
    SPELL_RESONANT_QUAKE = 258627,
    SPELL_TECTONIC_SMASH = 275907,

    SPELL_FRACKING_TOTEM_BUTTON = 257455,
    SPELL_FRACKING_TOTEM_CAST = 257480,
    SPELL_FRACKING_TOTEM_STUN = 257481,
    SPELL_FRACKING_TOTEM_SUMMON = 268204,

};

enum Events
{
    EVENT_FIXATE = 1,
    EVENT_AZERITE_INFUSION,
    EVENT_JAGGED_CUT,
    EVENT_CALL_EARTHRAGER,
    EVENT_RESONANT_PULSE,
    EVENT_RESONANT_QUAKE,
    EVENT_TECTONIC_SMASH,

    EVENT_TOTEM_TARGET,
};

enum Timers
{
    TIMER_AZERITE_INFUSION = 28 * IN_MILLISECONDS,
    TIMER_CALL_EARTHRAGER = 15 * IN_MILLISECONDS,
    TIMER_RESONANT_PULSE = 20 * IN_MILLISECONDS,
    TIMER_TECTONIC_SMASH = 40 * IN_MILLISECONDS,

    TIMER_TOTEM_TARGET = 2 * IN_MILLISECONDS,
};
enum Creatures
{
    BOSS_AZEROKK = 129227,
    NPC_EARTHRAGERS = 129802,
    NPC_FRACKING_TOTEM = 136500,
    NPC_FRACKING_TOTEM_PRE = 129804,
};

const Position centerPos = { 1215.50f, -3325.10f, 57.33f }; // 40y

enum SoundId
{
    SOUND_AZERITE_INFUSION = 97390,
    SOUND_CALL_EARTHRAGER = 97384,
    SOUND_RESONANT_PULSE = 97381,
    SOUND_TECTONIC_SMASH = 115344,
    SOUND_KILL = 97380,
    SOUND_DEATH = 97379,
    SOUND_AGGRO = 97385,
};

const Position earthragerPos[4] =
{
    { 1235.39f, -3324.67f, 56.98f },
    { 1213.96f, -3346.69f, 55.51f },
    { 1213.57f, -3304.01f, 55.71f },
    { 1194.62f, -3321.71f, 55.17f },
};

const Position frackingPos[2] =
{
    { 1183.08f, -3304.60f, 56.83f },
    { 1185.00f, -3301.67f, 56.67f },
};

#define AZERITE_INFUSION "You are not prepared!!!!"
#define CALL_EARTHRAGER "Burn..."
#define RESONANT_PULSE "Figth with me."
#define TECTONIC_SMASH "Arg"
#define KILL_TEXT "I will kill you"
#define DEATH_TEXT "Oh nooo..."
#define AGGRO_TEXT "Come here..."

class bfa_boss_azerokk : public CreatureScript
{
public:
    bfa_boss_azerokk() : CreatureScript("bfa_boss_azerokk")
    {
    }
    struct bfa_boss_azerokk_AI : public BossAI
    {
        bfa_boss_azerokk_AI(Creature* creature) : BossAI(creature, DATA_AZEROKK), summons(me)
        {
            instance = me->GetInstanceScript();
        }

        EventMap events;
        InstanceScript* instance;
        SummonList summons;

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_EARTHRAGERS:
                summon->SetInCombatWithZone();
                break;
            }

        }

        void Reset() override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            summons.DespawnAll();
            events.Reset();
        }

        void KilledUnit(Unit* /*a*/) override
        {
            SelectSoundAndText(me, 3);
        }

        void SelectSoundAndText(Creature* me, uint32 selectedTextSound = 0)
        {
            if (!me)
                return;

            if (me)
            {
                switch (selectedTextSound)
                {
                case 1:
                    me->PlayDirectSound(SOUND_AGGRO);
                    me->Yell(AGGRO_TEXT, LANG_UNIVERSAL, NULL);
                    break;
                case 2:
                    me->PlayDirectSound(SOUND_DEATH);
                    me->Yell(DEATH_TEXT, LANG_UNIVERSAL, NULL);
                    break;
                case 3:
                    me->PlayDirectSound(SOUND_KILL);
                    me->Yell(KILL_TEXT, LANG_UNIVERSAL, NULL);
                    break;
                case 4:
                    me->PlayDirectSound(SOUND_AZERITE_INFUSION);
                    me->Yell(AZERITE_INFUSION, LANG_UNIVERSAL, NULL);
                    break;
                case 5:
                    me->PlayDirectSound(SOUND_CALL_EARTHRAGER);
                    me->Yell(CALL_EARTHRAGER, LANG_UNIVERSAL, NULL);
                    break;
                case 6:
                    me->PlayDirectSound(SOUND_TECTONIC_SMASH);
                    me->Yell(TECTONIC_SMASH, LANG_UNIVERSAL, NULL);
                    break;
                case 7:
                    me->PlayDirectSound(SOUND_RESONANT_PULSE);
                    me->Yell(RESONANT_PULSE, LANG_UNIVERSAL, NULL);
                    break;
                }
            }
        }

        void JustDied(Unit*) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            summons.DespawnAll();
            SelectSoundAndText(me, 2);
        }

        void EnterEvadeMode(EvadeReason /*why*/) override
        {
            summons.DespawnAll();
            _DespawnAtEvade(15s);
        }

        void JustEngagedWith(Unit*) override
        {
            SelectSoundAndText(me, 1);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            for (uint8 i = 0; i < 4; ++i)
                me->SummonCreature(NPC_EARTHRAGERS, earthragerPos[i], TEMPSUMMON_DEAD_DESPAWN);
            for (uint8 i = 0; i < 2; ++i)
                me->SummonCreature(NPC_FRACKING_TOTEM_PRE, frackingPos[i], TEMPSUMMON_MANUAL_DESPAWN);

            if (me->GetMap()->IsMythic() || me->GetMap()->IsHeroic())
                events.ScheduleEvent(EVENT_TECTONIC_SMASH, 40s);

            events.ScheduleEvent(EVENT_CALL_EARTHRAGER, 15s);
            events.ScheduleEvent(EVENT_AZERITE_INFUSION, 28s);
            events.ScheduleEvent(EVENT_RESONANT_PULSE, 20s);
        }

        void OnSpellCast(SpellInfo const* spell) override
        {

            switch (spell->Id)
            {
            case SPELL_RESONANT_PULSE:
            {
                std::list<Creature*> ragers;
                me->GetCreatureListWithEntryInGrid(ragers, NPC_EARTHRAGERS, 100.0f);
                if (!ragers.empty())
                {
                    for (auto rager : ragers)
                        rager->CastSpell(rager->GetVictim(), SPELL_RESONANT_QUAKE);
                }
                break;
            }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_RESONANT_PULSE:
                    SelectSoundAndText(me, 7);
                    me->CastSpell(me, SPELL_RESONANT_PULSE);
                    events.ScheduleEvent(EVENT_RESONANT_PULSE, 20s);
                    break;
                case EVENT_AZERITE_INFUSION:
                {
                    SelectSoundAndText(me, 4);
                    std::list<Creature*> clist;
                    me->GetCreatureListWithEntryInGrid(clist, NPC_EARTHRAGERS, 100.0f);
                    if (!clist.empty())
                    {
                        if (clist.size() >= 1)
                            clist.resize(1);

                        for (auto earthrager : clist)
                            me->CastSpell(earthrager, SPELL_AZERITE_INFUSION_MISSILE);
                    }

                    events.ScheduleEvent(EVENT_AZERITE_INFUSION, 15s);
                    break;
                }
                case EVENT_TECTONIC_SMASH:
                    SelectSoundAndText(me, 6);
                    me->CastSpell(me->GetVictim(), SPELL_TECTONIC_SMASH);
                    events.ScheduleEvent(EVENT_TECTONIC_SMASH, 14s);
                    break;
                case EVENT_CALL_EARTHRAGER:
                    SelectSoundAndText(me, 5);
                    me->CastSpell(me, SPELL_CALL_EARTHRAGER);
                    events.ScheduleEvent(EVENT_CALL_EARTHRAGER, 15s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_boss_azerokk_AI(creature);
    }
};

class bfa_npc_earthrager : public CreatureScript
{
public:
    bfa_npc_earthrager() : CreatureScript("bfa_npc_earthrager")
    {
    }
    struct bfa_npc_earthrager_AI : public ScriptedAI
    {
        bfa_npc_earthrager_AI(Creature* creature) : ScriptedAI(creature)
        {
        }

        EventMap events;

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit*) override
        {
            events.ScheduleEvent(EVENT_FIXATE, 2s);
            events.ScheduleEvent(EVENT_JAGGED_CUT, 3s);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FIXATE:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                    {
                        me->CastSpell(target, SPELL_RAGING_GAZE_BEAM, true);
                        me->Attack(target, true);
                    }
                    break;
                case EVENT_JAGGED_CUT:
                {
                    if (Unit* target = me->GetVictim())
                    {
                        if(me->IsWithinMeleeRange(target))
                            me->CastSpell(target, SPELL_JAGGED_CUT, true);
                    }
                    events.ScheduleEvent(EVENT_JAGGED_CUT, 3s);
                    break;
                }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_earthrager_AI(creature);
    }
};

class bfa_npc_fracking_totem : public CreatureScript
{
public:
    bfa_npc_fracking_totem() : CreatureScript("bfa_npc_fracking_totem")
    {
    }
    struct bfa_npc_fracking_totem_AI : public ScriptedAI
    {
        bfa_npc_fracking_totem_AI(Creature* creature) : ScriptedAI(creature)
        {
        }

        bool canStun;

        void Reset() override
        {
            canStun = false;
        }

        void StunRager()
        {
            if (!canStun)
            {
                canStun = true;
                me->DespawnOrUnsummon(1min);

                std::list<Creature*> clist;
                me->GetCreatureListWithEntryInGrid(clist, NPC_EARTHRAGERS, 100.0f);
                if (!clist.empty())
                {
                    if (clist.size() >= 1)
                        clist.resize(1);

                    for (auto earthrager : clist)
                        me->CastSpell(earthrager, SPELL_FRACKING_TOTEM_STUN);
                }
            }
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            StunRager();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_fracking_totem_AI(creature);
    }
};

class bfa_npc_fracking_totem_selector : public CreatureScript
{
public:
    bfa_npc_fracking_totem_selector() : CreatureScript("bfa_npc_fracking_totem_selector") { }

    struct bfa_npc_fracking_totem_selector_AI : public ScriptedAI
    {
        bfa_npc_fracking_totem_selector_AI(Creature* creature) : ScriptedAI(creature)
        {
        }

    bool OnGossipHello(Player* player) override
    {
        player->CastSpell(player, SPELL_FRACKING_TOTEM_BUTTON);
        me->DespawnOrUnsummon(500ms);
        return true;
    }

 };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_fracking_totem_selector_AI(creature);
    }
};

// 257480
class bfa_spell_fracking_totem_summon : public SpellScriptLoader
{
public:
    bfa_spell_fracking_totem_summon() : SpellScriptLoader("bfa_spell_fracking_totem_summon") { }

    class bfa_spell_fracking_totem_summon_SpellScript : public SpellScript
    {
        PrepareSpellScript(bfa_spell_fracking_totem_summon_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            caster->CastSpell(caster, SPELL_FRACKING_TOTEM_SUMMON);
        }

        void Register()
        {
            AfterCast += SpellCastFn(bfa_spell_fracking_totem_summon_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new bfa_spell_fracking_totem_summon_SpellScript();
    }
};

void AddSC_boss_azerokk()
{
    new bfa_boss_azerokk();

    new bfa_npc_earthrager();
    new bfa_npc_fracking_totem();
    new bfa_npc_fracking_totem_selector();

    new bfa_spell_fracking_totem_summon();
}
