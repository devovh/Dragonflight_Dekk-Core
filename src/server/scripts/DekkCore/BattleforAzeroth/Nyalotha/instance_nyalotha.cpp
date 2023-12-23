#include "ScriptMgr.h"
#include "Player.h"
#include "InstanceScript.h"
#include "nyalotha.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "ScriptedCreature.h"

const Position annex_of_prophecy_platform_pos = { -1760.0f, -43.0f, -518.0f, 6.26f };

class instance_nyalotha : public InstanceMapScript
{
public:
    instance_nyalotha() : InstanceMapScript("instance_nyalotha", 2217) {}

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_nyalotha_InstanceMapScript(map);
    }

    struct instance_nyalotha_InstanceMapScript : public InstanceScript
    {
        instance_nyalotha_InstanceMapScript(InstanceMap* map) : InstanceScript(map)

        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
        }

        void OnPlayerEnter(Player* player) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (instance->GetBossState(DATA_WRATHION) == DONE)
                    player->TeleportTo(2217, -1760.0f, -43.0f, -518.0f, 6.26f);
        }
    };
};

void AddSC_instance_nyalotha()
{
    new instance_nyalotha();
}
