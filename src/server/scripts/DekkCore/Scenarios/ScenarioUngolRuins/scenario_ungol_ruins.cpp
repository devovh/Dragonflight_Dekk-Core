//scenario_ungol_ruins
/*
*/
#include "Player.h"
#include "ScriptMgr.h"
#include "CombatAI.h"
#include "Creature.h"
#include "CreatureGroups.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Scenario.h"
#include "WorldStatePackets.h"
#include "MiscPackets.h"
#include "WorldSession.h"
#include "SpellAuras.h"

enum sv_events
{
    EVENT_START_TIMER = 1,
    EVENT_GAME_START,
};

enum sv_gos
{
    GATE_01 = 303140,
    GATE_02 = 303141,
    GATE_03 = 303199,
};

enum sv_conversations
{
    HORDE_ON_BEGIN = 6514,
    HORDE_ON_50 = 7498,
    HORDE_ON_80 = 7487,
    HORDE_ON_COMPLETE = 7504,
};

struct scenario_ungol_ruins : public InstanceScript
{
    scenario_ungol_ruins(InstanceMap* map) : InstanceScript(map) { }

    bool isIntr = false;
    bool isComplete = false;

    void InitWorldState(bool Enable = true)
    {
       // DoUpdateWorldState(WORLDSTATE_ALLIANCE_GAIN, 0);
       // DoUpdateWorldState(WORLDSTATE_HORDE_GAIN, 0);
        DoUpdateWorldState(16018, 1);
        DoUpdateWorldState(14253, 1);
        DoUpdateWorldState(13004, 1);
        DoUpdateWorldState(14065, 1);
        DoUpdateWorldState(14246, 1);
        DoUpdateWorldState(14063, 1);
        DoUpdateWorldState(13321, 9);
        DoUpdateWorldState(12889, 1);
        DoUpdateWorldState(12878, 1);
        DoUpdateWorldState(12877, 1);

        DoUpdateWorldState(3191, 20);
        DoUpdateWorldState(3901, 18);
        DoUpdateWorldState(14684, 6000);
        DoUpdateWorldState(15893, 17);
    }


    void OnPlayerEnter(Player* player) override
    {
        InitWorldState();
        if (player->IsInAlliance())
        {
            //TODO
            SetCheckPointPos({ -398.02722f, -1259.3773f, 6.3429f, 6.1705503f });
            player->NearTeleportTo(-398.02722f, -1259.3773f, 6.3429f, 6.1705503f);
        }
        else
        {
            SetCheckPointPos({ -398.02722f, -1259.3773f, 6.3429f, 6.1705503f });
            player->NearTeleportTo(-398.02722f, -1259.3773f, 6.3429f, 6.1705503f);
        }

        WorldPackets::Misc::StartTimer startTimer;
        startTimer.Type = WorldPackets::Misc::StartTimer::Pvp;
        startTimer.TotalTime = Seconds(45);
        startTimer.TimeLeft = Seconds(45);
        instance->SendToPlayers(startTimer.Write());

        events.ScheduleEvent(EVENT_START_TIMER, 3s);
    }


    //void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override
    //{
    //   // packet.Worldstates.emplace_back(uint32(WORLDSTATE_ALLIANCE_GAIN), int32(GetIslandCount()[0]));
    //  //  packet.Worldstates.emplace_back(uint32(WORLDSTATE_HORDE_GAIN), int32(GetIslandCount()[1]));
    //}

    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);

        //Has to be done later, currently it is nothing besides a warnings creator
        /*switch (creature->GetEntry())
        {
        default:
            break;
        }*/
    }

    void OnPlayerLeave(Player* player) override
    {
        player->RemoveAurasDueToSpell(SPELL_AZERITE_RESIDUE);
        player->RemoveAurasDueToSpell(SPELL_ISLAND_COMPLETE);
    }

    void Update(uint32 diff) override
    {
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case EVENT_START_TIMER:

            isComplete = false;
            CastIslandAzeriteAura();
            events.ScheduleEvent(EVENT_GAME_START, 1s);
            //
            break;
        case EVENT_GAME_START:
            if (!isIntr)
            {
                isIntr = true;
            //    DoPlayConversation(HORDE_ON_BEGIN);
            }
            if (!isComplete)
            {
                /*if (GetIslandCount()[0] >= 9000)
                {
                    isComplete = true;
                    IslandComplete(true);
                }
                else if (GetIslandCount()[1] >= 9000)
                {
                    isComplete = true;
                    IslandComplete(false);
                }*/
            }
            events.ScheduleEvent(EVENT_GAME_START, 1s);
            break;
        }
    }

protected:
    EventMap events;
};
// SPELL_AZERITE_RESIDUE = 260738,
class spell_azerite_residue_260738 : public AuraScript
{
public:
    PrepareAuraScript(spell_azerite_residue_260738);

    void HandlePeriodic(AuraEffect const* /**/)
    {
        if (!GetCaster())
            return;

        //if (Aura* aura = GetCaster()->GetAura(SPELL_AZERITE_RESIDUE))
         //   aura->SetStackAmount(std::min((aura->GetStackAmount() + 1), 250));
    }

    void Register()
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_azerite_residue_260738::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

void AddSC_scenario_ungol_ruins()
{
    //RegisterInstanceScript(scenario_ungol_ruins, 1813);
    RegisterSpellScript(spell_azerite_residue_260738);
}

