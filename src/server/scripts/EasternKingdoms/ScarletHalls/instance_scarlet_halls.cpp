#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scarlet_halls.h"
#include "GameObject.h"

DoorData const doorData[] =
{
    {GO_BRAUN_DOOR,       DATA_BRAUN,         DOOR_TYPE_PASSAGE},
    {GO_HARLAN_DOOR,      DATA_HARLAN,        DOOR_TYPE_ROOM},
    {0,                   0,                  DOOR_TYPE_ROOM}, // END
};

class instance_scarlet_halls : public InstanceMapScript
{
public:
    instance_scarlet_halls() : InstanceMapScript("instance_scarlet_halls", 1001) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_scarlet_halls_InstanceMapScript(map);
    }

    struct instance_scarlet_halls_InstanceMapScript : public InstanceScript
    {
        instance_scarlet_halls_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetBossNumber(MAX_ENCOUNTER);
        }

        ObjectGuid BraunGUID;
        ObjectGuid HarlanGUID;
        ObjectGuid KoeglerGUID;

        void Initialize()
        {
            LoadDoorData(doorData);
            BraunGUID.Clear();
            HarlanGUID.Clear();
            KoeglerGUID.Clear();
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            return true;
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
            case GO_HARLAN_DOOR:
            case GO_BRAUN_DOOR:
                AddDoor(go, true);
                break;
            default:
                break;
            }
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
            case NPC_HOUNDMASTER_BRAUN:
                BraunGUID = creature->GetGUID();
                break;
            case NPC_ARMSMASTER_HARLAN:
                HarlanGUID = creature->GetGUID();
                break;
            case NPC_FLAMEWEAVER_KOEGLER:
                KoeglerGUID = creature->GetGUID();
                break;
            }
        }

    };
};

void AddSC_instance_scarlet_halls()
{
    new instance_scarlet_halls();
}
