#include "ScriptMgr.h"
#include "Player.h"
#include "InstanceScript.h"
#include "temple_of_sethraliss.h"
#include "GameObject.h"

DoorData const doorData[] =
{
    { 292551, DATA_ADDERIS_AND_ASPIX, DOOR_TYPE_ROOM },
    { 290906, DATA_MEREKTHA, DOOR_TYPE_ROOM },
    { 292414, DATA_GALVAZZT, DOOR_TYPE_ROOM },
};

class instance_temple_of_sethraliss : public InstanceMapScript
{
public:
    instance_temple_of_sethraliss() : InstanceMapScript("instance_temple_of_sethraliss", 1877) {}

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_temple_of_sethraliss_InstanceMapScript(map);
    }

    struct instance_temple_of_sethraliss_InstanceMapScript : public InstanceScript
    {
        instance_temple_of_sethraliss_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadDoorData(doorData);
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
            case GO_SETHRALISS_TREASURE:
                go->SetFlag(GO_FLAG_LOCKED);
                break;
            }
        }
    };

};


void AddSC_instance_temple_of_sethraliss()
{
    new instance_temple_of_sethraliss();
}
