#include "ScriptMgr.h"
#include "temple_of_sethraliss.h"
#include "ScriptedGossip.h"
#include <sstream>

enum Spells
{
    SPELL_EMPOWERMENT = 269670,
    SPELL_LIFE_FORCE = 274149,
    SPELL_CARDIAC_SHOCK = 279015,
    SPELL_LIFE_FORCE_HEAL = 274149,
    SPELL_CHAIN_LIGHTNING = 268061,
    SPELL_SNAKE_CHARM = 268008,
    SPELL_JOLT = 279000,
    SPELL_TAINT_DEBUFF = 267944,

    SPELL_TAINT_CHANNEL = 273677,
    SPELL_TAINT_VISUAL = 267759,
    SPELL_HEART_ATTACK = 268007,

    SPELL_PULSE = 268024,
    SPELL_PLAGUE = 269686,
    SPELL_SPAWN_FRAGMENT = 278885,
    SPELL_LAVA_BURST = 274642,
    SPELL_FLAME_SHOCK = 268013
};

enum Events
{
    EVENT_SUMMON_TOADS = 1,
    EVENT_SUMMON_HEXER,
    EVENT_SUMMON_GUARDIAN,
    EVENT_SUMMON_DOCTOR,
    EVENT_CHECK_PLAYERS,
    EVENT_PULSE,
    EVENT_CHAIN_LIGHTNING,
    EVENT_SNAKE_CHARM,
    EVENT_CHANNEL,
    EVENT_HEART_ATTACK,
    EVENT_LIGHTNING_STRIKE,
};

enum Timers
{
    TIMER_PULSE = 30 * IN_MILLISECONDS,
    TIMER_LIGHTNING_STRIKE = 15 * IN_MILLISECONDS,

    TIMER_CHECK_PLAYERS = 2 * IN_MILLISECONDS,

    TIMER_SUMMON_TOADS = 25 * IN_MILLISECONDS,
    TIMER_SUMMON_GUARDIAN = 15 * IN_MILLISECONDS,
    TIMER_SUMMON_DOCTOR = 20 * IN_MILLISECONDS,

    TIMER_CHAIN_LIGHTNING = 8 * IN_MILLISECONDS,
    TIMER_SNAKE_CHARM = 25 * IN_MILLISECONDS,
    TIMER_HEART_ATTACK = 5 * IN_MILLISECONDS,
};

enum Actions
{
    ACTION_START = 1,
    ACTION_COMPLETE,
};

enum Creatures
{
    BOSS_AVATAR_OF_SETHRALISS = 133392,

    NPC_HOODOO_HEXER_BFA = 136250,
    NPC_PLAGUE_DOCTOR_BFA = 139949,
    NPC_HEART_GUARDIAN_BFA = 139946,
    NPC_PLAGUE_TOAD_BFA = 137233,
    NPC_ENERGY_FRAGMENT_BFA = 142929,
};

Position HoodooPos[4] =
{
    { 4142.10f, 3678.26f, -42.95f },
    { 4167.25f, 3692.94f, -42.95f },
    { 4181.25f, 3668.73f, -42.95f },
    { 4156.75f, 3653.69f, -42.95f },
};

Position ToadPos[5] =
{
    { 4142.10f, 3678.26f, -42.95f },
    { 4167.25f, 3692.94f, -42.95f },
    { 4181.25f, 3668.73f, -42.95f },
    { 4156.75f, 3653.69f, -42.95f },
    { 4168.72f, 3655.34f, -43.25f },
};

#define AVATAR_START "The hex is shattered, but the temple's defenses have awakened!"
#define SECOND_TEXT_AVATAR "Should you fail, I am lost!"
#define AVATAR_COMPLETED "The storm has broken, and I am myself again. Thank you."
#define FIRST_TEXT_AVATAR "Restore me and I will disable them!"

class bfa_boss_avatar_of_sethraliss : public CreatureScript
{
public:
    bfa_boss_avatar_of_sethraliss() : CreatureScript("bfa_boss_avatar_of_sethraliss")
    {
    }

    struct bfa_boss_avatar_of_sethraliss_AI : public BossAI
    {
        bfa_boss_avatar_of_sethraliss_AI(Creature* creature) : BossAI(creature, DATA_AVATAR_OF_SETHRALISS), summons(me)
        {
            instance = me->GetInstanceScript();
            me->SetRegenerateHealth(false);
        }

        InstanceScript* instance;
        EventMap events;
        bool first;
        bool combatText;
        bool second;
        bool third;
        bool done;
        SummonList summons;

        bool OnGossipHello(Player* player) override
        {
            if (!me || !player)
                return false;

            AddGossipItemFor(player, GossipOptionNpc::None, "We are here to help you!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            SendGossipMenuFor(player, 1, me);

            return true;
        }

        bool OnGossipSelect(Player* /*player*/, uint32 /*sender*/, uint32 action) override
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                me->AI()->DoAction(ACTION_START);
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            }

            return true;
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
                    me->Yell(AVATAR_COMPLETED, LANG_UNIVERSAL, NULL);
                    break;
                case 2:
                    me->Yell(AVATAR_START, LANG_UNIVERSAL, NULL);
                    break;
                case 3:
                    me->Yell(SECOND_TEXT_AVATAR, LANG_UNIVERSAL, NULL);
                    break;
                case 4:
                    me->Yell(FIRST_TEXT_AVATAR, LANG_UNIVERSAL, NULL);
                    break;
                }
            }
        }

        void SummonToads()
        {
            std::ostringstream str;
            str << "A plague of toads |cFFF00000|h[Rains]|h|r down!";
            me->TextEmote(str.str().c_str());

            for (uint8 i = 0; i < 5; ++i)
                me->SummonCreature(NPC_PLAGUE_TOAD_BFA, ToadPos[i], TEMPSUMMON_MANUAL_DESPAWN);
        }

        void SummonHoodooHexer()
        {
            for (uint8 i = 0; i < 4; ++i)
                me->SummonCreature(NPC_HOODOO_HEXER_BFA, HoodooPos[i], TEMPSUMMON_MANUAL_DESPAWN);
        }

        void SummonGuardian()
        {
            for (uint8 i = 0; i < 2; ++i)
                me->SummonCreature(NPC_HEART_GUARDIAN_BFA, ToadPos[i], TEMPSUMMON_MANUAL_DESPAWN);
        }

        void SummonDoctor()
        {
            for (uint8 i = 0; i < 1; ++i)
                me->SummonCreature(NPC_PLAGUE_DOCTOR_BFA, HoodooPos[i], TEMPSUMMON_MANUAL_DESPAWN);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_HOODOO_HEXER_BFA:
                summon->AddUnitState(UNIT_STATE_ROOT);
                summon->SetInCombatWithZone();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, summon);
                break;
            case NPC_PLAGUE_TOAD_BFA:
            case NPC_PLAGUE_DOCTOR_BFA:
            case NPC_HEART_GUARDIAN_BFA:
                summon->SetInCombatWithZone();
                break;
            }
        }

        void Reset() override
        {
            events.Reset();
            first = false;
            summons.DespawnAll();
            done = false;
            combatText = false;
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            second = false;
            third = false;
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterEvadeMode(EvadeReason /*w*/) override
        {
            _DespawnAtEvade(15s);
        }

        void Start()
        {
            combatText = true;
            SelectSoundAndText(me, 2);

            std::ostringstream str;
            str << "Restore the Avatar to full health.";
            me->TextEmote(str.str().c_str(), 0, true);

            me->SetHealth(me->GetMaxHealth() * 0.10f);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            events.ScheduleEvent(EVENT_SUMMON_HEXER, 1s);
            events.ScheduleEvent(EVENT_SUMMON_GUARDIAN, 15s);
            events.ScheduleEvent(EVENT_SUMMON_TOADS, 25s);

            events.ScheduleEvent(EVENT_PULSE, 30s);
            events.ScheduleEvent(EVENT_LIGHTNING_STRIKE, 15s);

            events.ScheduleEvent(EVENT_CHECK_PLAYERS, 2s);

            if (me->GetMap()->IsHeroic() || me->GetMap()->IsMythic())
                events.ScheduleEvent(EVENT_SUMMON_DOCTOR, 20s);
        }

        void CheckHexTaint()
        {
            std::list<Creature*> hexers;
            me->GetCreatureListWithEntryInGrid(hexers, NPC_HOODOO_HEXER_BFA, 35.0f);
            if (!hexers.empty())
                me->AddAura(SPELL_TAINT_DEBUFF, me);
            else if (hexers.empty())
            {
                std::ostringstream str;
                str << "The Avatar is no longer |cFFF00000|h[Tainted]|h|r , your heals are now fully effective!";
                me->TextEmote(str.str().c_str());
                me->RemoveAura(SPELL_TAINT_DEBUFF);
            }
        }

        void Complete()
        {
            combatText = false;
            SelectSoundAndText(me, 1);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            summons.DespawnAll();

            events.Reset();
            me->SummonGameObject(GO_SETHRALISS_TREASURE, 4149.73f, 3665.59f, -43.0365f, 3.68391f, QuaternionData(0, 0, -0.963461f, 0.267849f), 0s);

        }

        void DoAction(int32 action) override
        {
            switch (action)
            {
            case ACTION_START:
                Start();
                break;
            case ACTION_COMPLETE:
                Complete();
                break;
            }
        }

        void HealReceived(Unit* /*healer*/, uint32& heal) override
        {
            if (me->HasAura(SPELL_TAINT_DEBUFF))
                heal = 0;

            if (me->HealthAbovePct(40) && !first)
            {
                first = true;
                SelectSoundAndText(me, 4);
                SummonHoodooHexer();
            }

            if (me->HealthAbovePct(60) && !second)
            {
                second = true;
                SelectSoundAndText(me, 3);
                SummonHoodooHexer();
            }

            if (me->HealthAbovePct(80) && !third)
            {
                third = true;
                SummonHoodooHexer();
            }

            if (me->HealthAbovePct(99) && !done)
            {
                done = true;
                me->AI()->DoAction(ACTION_COMPLETE);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            if(combatText)
                CheckHexTaint();

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CHECK_PLAYERS:
                {
                    std::list<Player*> playerList;
                    me->GetPlayerListInGrid(playerList, 50.0f);
                    if (playerList.empty())
                    {
                        for (auto player : playerList)
                        {
                            if (!player->IsGameMaster())
                                EnterEvadeMode(EVADE_REASON_NO_HOSTILES);
                        }
                    }

                    events.ScheduleEvent(EVENT_CHECK_PLAYERS, 2s);
                    break;
                }
                case EVENT_PULSE:
                {
                    std::list<Player*> playerList;
                    me->GetPlayerListInGrid(playerList, 100.0f);
                    if (playerList.size())
                    {
                        for (auto player : playerList)
                            me->CastSpell(player, SPELL_PULSE, true);
                    }

                    events.ScheduleEvent(EVENT_PULSE, 30s);
                    break;
                }
                case EVENT_LIGHTNING_STRIKE:
                {
                    std::list<Player*> playerList;
                    me->GetPlayerListInGrid(playerList, 100.0f);
                    if (playerList.size())
                    {
                        if (playerList.size() > 2)
                            playerList.resize(2);

                        for (auto player : playerList)
                        {
                            me->CastSpell(player, SPELL_CARDIAC_SHOCK);
                        }
                    }

                    events.ScheduleEvent(EVENT_LIGHTNING_STRIKE, 15s);
                    break;
                }
                case EVENT_SUMMON_DOCTOR:
                    SummonDoctor();
                    events.ScheduleEvent(EVENT_SUMMON_DOCTOR, 20s);
                    break;
                case EVENT_SUMMON_GUARDIAN:
                    SummonGuardian();
                    events.ScheduleEvent(EVENT_SUMMON_GUARDIAN, 15s);
                    break;
                case EVENT_SUMMON_HEXER:
                    SummonHoodooHexer();
                    break;
                case EVENT_SUMMON_TOADS:
                    SummonToads();
                    events.ScheduleEvent(EVENT_SUMMON_TOADS, 25s);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_boss_avatar_of_sethraliss_AI(creature);
    }
};

class bfa_npc_hoodoo_hexer : public CreatureScript
{
public:
    bfa_npc_hoodoo_hexer() : CreatureScript("bfa_npc_hoodoo_hexer")
    {
    }

    struct bfa_npc_hoodoo_hexer_AI : public ScriptedAI
    {
        bfa_npc_hoodoo_hexer_AI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void JustEngagedWith(Unit*)
        {
            me->SetReactState(REACT_PASSIVE);
            events.ScheduleEvent(EVENT_CHANNEL, 1s);
        }

        void UpdateAI(uint32 diff)
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
                case EVENT_CHANNEL:
                    if (Creature* boss = me->FindNearestCreature(BOSS_AVATAR_OF_SETHRALISS, 200.0f))
                    {
                        me->AI()->AttackStart(boss);
                        me->CastSpell(boss, SPELL_TAINT_CHANNEL);
                    }
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_hoodoo_hexer_AI(creature);
    }
};

class bfa_npc_heart_guardian : public CreatureScript
{
public:
    bfa_npc_heart_guardian() : CreatureScript("bfa_npc_heart_guardian")
    {
    }

    struct bfa_npc_heart_guardian_AI : public ScriptedAI
    {
        bfa_npc_heart_guardian_AI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset() override
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*at*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (damage >= me->GetHealth())
            {
                damage = 0;
                me->SummonCreature(NPC_ENERGY_FRAGMENT_BFA, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), TEMPSUMMON_DEAD_DESPAWN);
                std::ostringstream str;
                str << "An Energy Fragment forms nearby, use it to |cFFF00000|h[Energize]|h|r the Avatar!";
                me->TextEmote(str.str().c_str());
                me->DespawnOrUnsummon(500ms);
            }
        }

        void JustEngagedWith(Unit*) override
        {
            events.ScheduleEvent(EVENT_HEART_ATTACK, 5s);
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
                case EVENT_HEART_ATTACK:
                    if (Unit* target = me->GetVictim())
                        me->CastSpell(target, SPELL_HEART_ATTACK);
                    events.ScheduleEvent(EVENT_HEART_ATTACK, 5s);
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_heart_guardian_AI(creature);
    }
};

class bfa_npc_plague_toad : public CreatureScript
{
public:
    bfa_npc_plague_toad() : CreatureScript("bfa_npc_plague_toad")
    {
    }

    struct bfa_npc_plague_toad_AI : public ScriptedAI
    {
        bfa_npc_plague_toad_AI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void JustEngagedWith(Unit*)
        {
            std::list<Player*> playerList;
            me->GetPlayerListInGrid(playerList, 100.0f);
            if (playerList.size())
            {
                if (playerList.size() > 1)
                    playerList.resize(1);

                for (auto player : playerList)
                {
                    me->AI()->AttackStart(player);
                }
            }
        }

        void UpdateAI(uint32 /*diff*/)
        {
            if (Unit* target = me->GetVictim())
            {
                if (target->GetDistance(me) <= 2.0f)
                {
                    me->CastSpell(target, SPELL_PLAGUE, true);
                    me->DespawnOrUnsummon();
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_plague_toad_AI(creature);
    }
};

class bfa_npc_plague_doctor : public CreatureScript
{
public:
    bfa_npc_plague_doctor() : CreatureScript("bfa_npc_plague_doctor")
    {
    }

    struct bfa_npc_plague_doctor_AI : public ScriptedAI
    {
        bfa_npc_plague_doctor_AI(Creature* creature) : ScriptedAI(creature)
        {
        }

        EventMap events;

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 8s);
            events.ScheduleEvent(EVENT_SNAKE_CHARM, 25s);
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
                case EVENT_SNAKE_CHARM:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                        me->CastSpell(target, SPELL_SNAKE_CHARM);
                    events.ScheduleEvent(EVENT_SNAKE_CHARM, 25s);
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                        me->CastSpell(target, SPELL_CHAIN_LIGHTNING);
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 8s);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_plague_doctor_AI(creature);
    }
};

class bfa_npc_energy_fragment : public CreatureScript
{
public:
    bfa_npc_energy_fragment() : CreatureScript("bfa_npc_energy_fragment") { }

    bool OnGossipHello(Player* player, Creature* me)
    {
        if (!me || !player)
            return false;

        player->CastSpell(player, SPELL_LIFE_FORCE, true);
        me->DespawnOrUnsummon();
        return true;
    }

    struct bfa_npc_energy_fragment_AI : public ScriptedAI
    {
        bfa_npc_energy_fragment_AI(Creature* creature) : ScriptedAI(creature)
        {
            me->AddAura(SPELL_SPAWN_FRAGMENT, me);
            me->SetFaction(35);
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bfa_npc_energy_fragment_AI(creature);
    }
};

void AddSC_boss_avatar_of_sethraliss()
{
    new bfa_boss_avatar_of_sethraliss();

    new bfa_npc_energy_fragment();
    new bfa_npc_heart_guardian();
    new bfa_npc_hoodoo_hexer();
    new bfa_npc_plague_doctor();
    new bfa_npc_plague_toad();
}
