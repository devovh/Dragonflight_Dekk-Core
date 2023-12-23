#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "the_motherlode.h"
#include "ScriptedCreature.h"
#include "SpellHistory.h"
#include <sstream>

enum Spells
{
    SPELL_PILES_OF_GOLD_VISUAL = 271801,
    SPELL_STATIC_PULSE = 262347,
    SPELL_FOOTBOMB_LAUNCHER = 269493,
    SPELL_PAY_TO_WIN = 271867,
    SPELL_SHOCKING_CLAWS = 257337,
    SPELL_COIN_MAGNET = 271903,
    SPELL_BLAZING_AZERITE = 256163,
    SPELL_BLAZING_AZERITE_BOSS = 256493,
    SPELL_BLAZING_AZERITE_PLAYERS = 270882,
    SPELL_BOMB_TIMER = 256054,
};

enum Events
{
    EVENT_THROW_COINS = 1,
    EVENT_FOOTBOMB_LAUNCHER,
    EVENT_SHOCKING_CLAW,
    EVENT_DETONATE,
    EVENT_STATIC_PULSE,
    EVENT_COIN_MAGNET,
};

enum Timers
{
    TIMER_STATIC_PULSE = 10 * IN_MILLISECONDS,
    TIMER_FOOTBOMB_LAUNCHER = 17 * IN_MILLISECONDS,
    TIMER_SHOCKING_CLAW = 25 * IN_MILLISECONDS,
    TIMER_COIN_MAGNET = 10 * IN_MILLISECONDS,
    TIMER_THROW_COINS = 35 * IN_MILLISECONDS,
    TIMER_DETONATE = 15 * IN_MILLISECONDS,
};

enum Creatures
{
    BOSS_CO_CROWD_PLUMMELER = 129214,
    NPC_AZERITE_FOOTBOMB = 129246,
    NPC_PILE_OF_GOLD = 138083,
};

enum DisplayId
{
    DISPLAY_BOMB = 49134,
};

enum Actions
{
    ACTION_JUMP = 1,
    ACTION_INIT_SPELL,
};

const Position centerPos = { 1000.73f, -3556.13f, 13.01f }; //55y

enum SoundId
{
    SOUND_AGGRO = 97384,
    SOUND_STATIC = 97381,
    SOUND_SHOCKING_CLAW = 97380,
    SOUND_DEATH = 97385,
    SOUND_COIN_MAGNET = 97379,
};

#define AGGRO_TEXT "Systems engaged. Commence pummeling."
#define STATIC_PULSE "Voltage increased."
#define SHOCKING_CLAW "Lethal force authorized."
#define DEATH_TEXT "Systems... failing. Coin release... malfunction..."
#define COIN_MAGNET "Damaging this unit violates the terms of use."

class bfa_boss_coin_operated_crowd_pummeler : public CreatureScript
{
public:
    bfa_boss_coin_operated_crowd_pummeler() : CreatureScript("bfa_boss_coin_operated_crowd_pummeler")
    {
    }

    struct bfa_boss_coin_operated_crowd_pummeler_AI : public BossAI
    {
        bfa_boss_coin_operated_crowd_pummeler_AI(Creature* creature) : BossAI(creature, DATA_COIN_OPERATED_CROWD_PUMMELER), summons(me)
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
            case NPC_PILE_OF_GOLD:
                summon->SetFaction(35);
                summon->AddAura(SPELL_PILES_OF_GOLD_VISUAL, summon);
                break;
            case NPC_AZERITE_FOOTBOMB:
                summon->SetFaction(35);
                summon->SetDisplayId(DISPLAY_BOMB);
                summon->AI()->DoAction(ACTION_INIT_SPELL);
                summon->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                break;
            }
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
                    me->PlayDirectSound(SOUND_AGGRO);
                    me->Yell(AGGRO_TEXT, LANG_UNIVERSAL, NULL);
                    break;
                case 2:
                    me->PlayDirectSound(SOUND_DEATH);
                    me->Yell(DEATH_TEXT, LANG_UNIVERSAL, NULL);
                    break;
                case 3:
                    me->PlayDirectSound(SOUND_STATIC);
                    me->Yell(STATIC_PULSE, LANG_UNIVERSAL, NULL);
                    break;
                case 4:
                    me->PlayDirectSound(SOUND_COIN_MAGNET);
                    me->Yell(COIN_MAGNET, LANG_UNIVERSAL, NULL);
                    break;
                case 5:
                    me->PlayDirectSound(SOUND_SHOCKING_CLAW);
                    me->Yell(SHOCKING_CLAW, LANG_UNIVERSAL, NULL);
                    break;
                }
            }
        }

        void Reset() override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            summons.DespawnAll();
            events.Reset();
        }

        void EnterEvadeMode(EvadeReason w) override
        {
            summons.DespawnAll();
            _DespawnAtEvade(15s);
        }

        void JustDied(Unit*) override
        {
            SelectSoundAndText(me, 2);
            summons.DespawnAll();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void JustEngagedWith(Unit*) override
        {
            SelectSoundAndText(me, 1);
            me->RemoveAllAuras();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            if (me->GetMap()->IsMythic() || me->GetMap()->IsHeroic())
                events.ScheduleEvent(EVENT_THROW_COINS, 35s);

            events.ScheduleEvent(EVENT_SHOCKING_CLAW, 25s);
            events.ScheduleEvent(EVENT_STATIC_PULSE, 17s);
            events.ScheduleEvent(EVENT_FOOTBOMB_LAUNCHER, 23s);
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
                case EVENT_STATIC_PULSE:
                    SelectSoundAndText(me, 3);
                    me->CastSpell(me->GetVictim(), SPELL_STATIC_PULSE);
                    events.ScheduleEvent(EVENT_STATIC_PULSE, 16s);
                    break;
                case EVENT_SHOCKING_CLAW:
                    me->AddUnitState(UNIT_STATE_ROOT);
                    if (Unit* target = me->GetVictim())
                    {
                        SelectSoundAndText(me, 5);
                        me->CastSpell(Position(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()), SPELL_SHOCKING_CLAWS, true);
                    }
                    events.ScheduleEvent(EVENT_SHOCKING_CLAW, 18s);
                    break;
                case EVENT_THROW_COINS:
                {
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        Position randomPos = me->GetRandomPoint(me->GetPosition(), 10.0f);
                        me->SummonCreature(NPC_PILE_OF_GOLD, randomPos, TEMPSUMMON_MANUAL_DESPAWN);
                    }
                    events.ScheduleEvent(EVENT_COIN_MAGNET, 19s);
                    break;
                }
                case EVENT_FOOTBOMB_LAUNCHER:
                {
                    std::ostringstream str;
                    str << "Coin-Operated Crowd Pummeler casts |cFFF00000|h[Footbomb Launcher]|h|r !";
                    me->TextEmote(str.str().c_str(), 0, true);
                    me->CastSpell(me, SPELL_FOOTBOMB_LAUNCHER);
                    events.ScheduleEvent(EVENT_FOOTBOMB_LAUNCHER, 13s);
                    break;
                }
                case EVENT_COIN_MAGNET:
                    SelectSoundAndText(me, 4);
                    me->CastSpell(me, SPELL_COIN_MAGNET);
                    events.ScheduleEvent(EVENT_THROW_COINS, 18s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

        void OnSpellCast(SpellInfo const* spell) override
        {
            switch (spell->Id)
            {
            case SPELL_SHOCKING_CLAWS:
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                break;
            }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_boss_coin_operated_crowd_pummeler_AI(creature);
    }
};

class bfa_npc_azerite_bomb : public CreatureScript
{
public:
    bfa_npc_azerite_bomb() : CreatureScript("bfa_npc_azerite_bomb") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        Position pos;
        float orientation = player->GetOrientation();
        creature->GetNearPoint(NULL, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 1, orientation);
        creature->GetMotionMaster()->MoveJump(pos.m_positionX, pos.m_positionY, pos.m_positionZ, orientation, 20.0f, 15.0f);
        creature->AI()->DoAction(ACTION_JUMP);
        return true;
    }

    struct bfa_npc_azerite_bomb_AI : public ScriptedAI
    {
        bfa_npc_azerite_bomb_AI(Creature* creature) : ScriptedAI(creature) 
        { 
            me->CastSpell(me, SPELL_BOMB_TIMER, true);
        }

        EventMap events;
        bool exploded;
        bool canAttack;

        void Reset() override
        {
            exploded = false;
            events.Reset();
        }

        void CheckIfBossNearby()
        {
            std::list<Creature*> bossList;
            me->GetCreatureListWithEntryInGrid(bossList, BOSS_CO_CROWD_PLUMMELER, 3.0f);
            if (!bossList.empty())
            {
                for (auto boss : bossList)
                {
                    if (!exploded && canAttack)
                    {
                        exploded = true;
                        boss->AddAura(SPELL_BLAZING_AZERITE_BOSS, boss);
                        me->DespawnOrUnsummon(500ms);
                    }
                }
            }
        }

        void DoAction(int32 action)
        {
            switch (action)
            {
            case ACTION_JUMP:
            {
                canAttack = true;

                me->GetScheduler().Schedule(2s, [this](TaskContext context)
                    {
                        canAttack = false;
                    });
                break;
            }
            case ACTION_INIT_SPELL:
                events.ScheduleEvent(EVENT_DETONATE, 15s);
                break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            CheckIfBossNearby();

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DETONATE:
                    me->CastSpell(me, SPELL_BLAZING_AZERITE, true);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_azerite_bomb_AI(creature);
    }
};

// 256163
class bfa_spell_blazing_azerite : public SpellScriptLoader
{
public:
    bfa_spell_blazing_azerite() : SpellScriptLoader("bfa_spell_blazing_azerite") { }

    class bfa_spell_blazing_azerite_SpellScript : public SpellScript
    {
        PrepareSpellScript(bfa_spell_blazing_azerite_SpellScript);

        void HandleOnHit()
        {
            Unit* target = GetHitUnit();
            Unit* caster = GetCaster();
            if (!caster || !target)
                return;

            caster->CastSpell(target, SPELL_BLAZING_AZERITE_PLAYERS, true);
        }

        void Register()
        {
            OnHit += SpellHitFn(bfa_spell_blazing_azerite_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new bfa_spell_blazing_azerite_SpellScript();
    }
};


// 269493
class bfa_spell_footbomb_launcher : public SpellScriptLoader
{
public:
    bfa_spell_footbomb_launcher() : SpellScriptLoader("bfa_spell_footbomb_launcher") { }

    class bfa_spell_footbomb_launcher_SpellScript : public SpellScript
    {
        PrepareSpellScript(bfa_spell_footbomb_launcher_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            for (uint8 i = 0; i < 3; ++i)
            {
                Position randomPos = caster->GetRandomPoint(centerPos, 15.0f);
                caster->SummonCreature(NPC_AZERITE_FOOTBOMB, randomPos, TEMPSUMMON_MANUAL_DESPAWN);
            }
        }

        void Register()
        {
            AfterCast += SpellCastFn(bfa_spell_footbomb_launcher_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new bfa_spell_footbomb_launcher_SpellScript();
    }
};

// 271903
class bfa_spell_coin_magnet : public SpellScriptLoader
{
public:
    bfa_spell_coin_magnet() : SpellScriptLoader("bfa_spell_coin_magnet") { }

    class bfa_spell_coin_magnet_SpellScript : public SpellScript
    {
        PrepareSpellScript(bfa_spell_coin_magnet_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            std::list<Creature*> coins;
            caster->GetCreatureListWithEntryInGrid(coins, NPC_PILE_OF_GOLD, 15.0f);
            if (!coins.empty())
            {
                for (auto validPile : coins)
                {
                    validPile->DespawnOrUnsummon();
                    caster->CastSpell(caster, SPELL_PAY_TO_WIN, true);
                }
            }
        }

        void Register()
        {
            AfterCast += SpellCastFn(bfa_spell_coin_magnet_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new bfa_spell_coin_magnet_SpellScript();
    }
};

void AddSC_boss_Coin_operated_crowd_pummeler()
{
    new bfa_boss_coin_operated_crowd_pummeler();

    new bfa_npc_azerite_bomb();

    new bfa_spell_blazing_azerite();
    new bfa_spell_footbomb_launcher();
    new bfa_spell_coin_magnet();
}
