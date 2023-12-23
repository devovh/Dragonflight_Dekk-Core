#ifndef WORLD_STATE_MGR_H
#define WORLD_STATE_MGR_H

#include "Common.h"
#include "WorldState.h"
#include "Define.h"

class Map;

namespace WorldPackets::WorldState
{
    class InitWorldStates;
}

class TC_GAME_API WorldStateMgr
{
public:
    static WorldStateMgr* instance();

    void LoadFromDB();

    WorldStateTemplate const* GetWorldStateTemplate(int32 worldStateId) const;

    int32 GetValue(int32 worldStateId, Map const* map) const;
    void SetValue(int32 worldStateId, int32 value, bool hidden, Map* map);
    void SaveValueInDb(int32 worldStateId, int32 value);
    void SetValueAndSaveInDb(int32 worldStateId, int32 value, bool hidden, Map* map);

    WorldStateValueContainer GetInitialWorldStatesForMap(Map const* map) const;

    void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& initWorldStates, Map const* map, uint32 playerAreaId) const;
};

#define sWorldStateMgr WorldStateMgr::instance()

#endif
