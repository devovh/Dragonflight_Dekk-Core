//scenario_verdant_wilds
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

struct scenario_verdant_wilds : public InstanceScript
{
    scenario_verdant_wilds(InstanceMap* map) : InstanceScript(map) { }

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
            SetCheckPointPos({ 15198.165f, 14626.65f, 6.097f, 1.0589f });
            player->NearTeleportTo(15198.165f, 14626.65f, 6.097f, 1.0589f);
        }
        else
        {
            SetCheckPointPos({ 15007.695f, 14726.907f, 6.3429f, 1.6861409f });
            player->NearTeleportTo(15007.695f, 14726.907f, 6.3429f, 1.6861409f);
        }

        player->GetSession()->StartTimer(this->instance, WorldPackets::Misc::StartTimer::Pvp, Seconds(45), Seconds(45));

        events.ScheduleEvent(EVENT_START_TIMER, 45s);
    }

    //void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override
    //{
    //   // packet.Worldstates.emplace_back(uint32(WORLDSTATE_ALLIANCE_GAIN), int32(GetIslandCount()[0]));
    //  //  packet.Worldstates.emplace_back(uint32(WORLDSTATE_HORDE_GAIN), int32(GetIslandCount()[1]));
    //}

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
               // DoPlayConversation(HORDE_ON_BEGIN);
            }
            if (!isComplete)
            {
              /*  if (GetIslandCount()[0] >= 9000)
                {
                    isComplete = true;
                    IslandComplete(true);
                }
                else if(GetIslandCount()[1] >= 9000)
                {
                    isComplete = true;
                    IslandComplete(false);*/
               // }
            }
            events.ScheduleEvent(EVENT_GAME_START, 1s);
            break;
        }
    }

protected:
    EventMap events;
};

void AddSC_scenario_verdant_wilds()
{
    RegisterInstanceScript(scenario_verdant_wilds, 1882);
}

