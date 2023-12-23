#include "ScriptMgr.h"
#include "Player.h"
#include "InstanceScript.h"
#include "kings_rest.h"

class instance_kings_rest : public InstanceMapScript
{
public:
    instance_kings_rest() : InstanceMapScript("instance_kings_rest", 1762) {}

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_kings_rest_InstanceMapScript(map);
    }

    struct instance_kings_rest_InstanceMapScript : public InstanceScript
    {
        instance_kings_rest_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetBossNumber(EncounterCount);
        }


        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
            case GO_THE_COUNCILS_CACHE:
            case GO_BOON_OF_THE_FIRST_KING:
                go->SetFlag(GO_FLAG_LOCKED);
                break;
            }
        }
    };

};

enum Zul
{
    SPELL_SHADOW_BARRAGE = 272388,
    SPELL_DARK_REVELATION = 271640,
    EVENT_SHADOW_BARRAGE = 1,
    EVENT_DARK_REVELATION,
};

//138489
struct npc_shadow_of_zul : public ScriptedAI
{
    npc_shadow_of_zul(Creature* c) : ScriptedAI(c) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void JustEngagedWith(Unit*) override
    {
        events.ScheduleEvent(EVENT_SHADOW_BARRAGE, 5s);
        events.ScheduleEvent(EVENT_DARK_REVELATION, 10s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
        case EVENT_SHADOW_BARRAGE:
            DoCastVictim(SPELL_SHADOW_BARRAGE);
            events.Repeat(3s);
            break;
        case EVENT_DARK_REVELATION:
            DoCastRandom(SPELL_DARK_REVELATION, 100.0f);
            me->SummonCreature(NPC_MINION_OF_ZUL, me->GetPosition());
            events.Repeat(10s);
            break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        summon->AI()->DoZoneInCombat();
    }

private:
    EventMap events;
};

void AddSC_instance_kings_rest()
{
    new instance_kings_rest();
    RegisterCreatureAI(npc_shadow_of_zul);
}
