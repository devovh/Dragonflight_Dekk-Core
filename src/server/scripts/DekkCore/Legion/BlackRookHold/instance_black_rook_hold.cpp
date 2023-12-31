#include "black_rook_hold.h"
#include "GameObject.h"
#include <sstream>

DoorData const doorData[] =
{
    {GO_AMALGAM_DOOR_1,         DATA_AMALGAM,           DOOR_TYPE_ROOM,},
    {GO_AMALGAM_DOOR_2,         DATA_AMALGAM,           DOOR_TYPE_ROOM,},
    {GO_AMALGAM_DOOR_3,         DATA_AMALGAM,           DOOR_TYPE_ROOM,},
    {GO_AMALGAM_DOOR_4,         DATA_AMALGAM,           DOOR_TYPE_ROOM,},
    {GO_ILLYSANNA_DOOR_1,       DATA_ILLYSANNA,         DOOR_TYPE_PASSAGE,},
    {GO_ILLYSANNA_DOOR_2,       DATA_ILLYSANNA,         DOOR_TYPE_PASSAGE,},
    {GO_ILLYSANNA_DOOR_3,       DATA_ILLYSANNA,         DOOR_TYPE_ROOM,},
    {GO_SMASHSPITE_DOOR,        DATA_SMASHSPITE,        DOOR_TYPE_ROOM,},
    {GO_SMASH_KURT_DOOR_1,      DATA_SMASHSPITE,        DOOR_TYPE_PASSAGE,},
    {GO_SMASH_KURT_DOOR_2,      DATA_SMASHSPITE,        DOOR_TYPE_PASSAGE,},
    {GO_KURTALOS_DOOR,          DATA_KURTALOS,          DOOR_TYPE_ROOM,},
};

class instance_black_rook_hold : public InstanceMapScript
{
public:
    instance_black_rook_hold() : InstanceMapScript("instance_black_rook_hold", 1501) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_black_rook_hold_InstanceMapScript(map);
    }

    struct instance_black_rook_hold_InstanceMapScript : public InstanceScript
    {
        instance_black_rook_hold_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetBossNumber(MAX_ENCOUNTER);

            LoadDoorData(doorData);

            events.RescheduleEvent(DATA_STAIRS_BOULDER_1, 3s);
        }

        EventMap events;
        WorldLocation loc_res_pla;
        std::list<ObjectGuid> IllysannaGateGUID;
        std::list<ObjectGuid> felspiteDominatorGUID;
        ObjectGuid AmalgamGateGUID;

        uint32 illysannaIntroState = 0;
        uint32 KurtalosState = 0;;
        uint32 AmalgamState = 0;

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
            case NPC_FELSPITE_DOMINATOR:
            {
                if (GetBossState(DATA_SMASHSPITE) != DONE)
                {
                    if (felspiteDominatorGUID.size() < 2 && creature->GetDistance(3224.31f, 7335.72f, 226.0f) < 10.0f)
                        felspiteDominatorGUID.push_back(creature->GetGUID());
                    if (felspiteDominatorGUID.size() == 2)
                        events.RescheduleEvent(DATA_SMASHSPITE_FEL_BAT_EVENT, 8s);
                }
                break;
            }
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
            case GO_AMALGAM_DOOR_1:
            case GO_AMALGAM_DOOR_2:
            case GO_AMALGAM_DOOR_3:
            case GO_AMALGAM_DOOR_4:
            case GO_ILLYSANNA_DOOR_1:
            case GO_ILLYSANNA_DOOR_2:
            case GO_ILLYSANNA_DOOR_3:
            case GO_SMASHSPITE_DOOR:
            case GO_SMASH_KURT_DOOR_1:
            case GO_SMASH_KURT_DOOR_2:
            case GO_KURTALOS_DOOR:
                AddDoor(go, true);
                break;
            case GO_AMALGAM_DOOR_EXIT:
                AmalgamGateGUID = go->GetGUID();
                if (GetBossState(DATA_AMALGAM) == DONE)
                    go->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_ILLYSANNA_DOOR_4:
            case GO_ILLYSANNA_DOOR_5:
                IllysannaGateGUID.push_back(go->GetGUID());
                break;
            default:
                break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            return true;
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
            case DATA_KURTALOS_STATE:
                KurtalosState = data;
                break;
            case DATA_AMALGAM_OUTRO:
                if (data == 0)
                    ++AmalgamState;
                if (data == DONE)
                {
                    HandleGameObject(AmalgamGateGUID, true);
                }
                break;
            case DATA_ILLYSANNA_INTRO:
                illysannaIntroState = data;
                if (data == DONE)
                {
                    for (auto guid : IllysannaGateGUID)
                        HandleGameObject(guid, true);
                }
                else
                {
                    for (auto guid : IllysannaGateGUID)
                        HandleGameObject(guid, false);
                }
                break;
            default:
                break;
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
            case DATA_KURTALOS_STATE:
                return KurtalosState;
            case DATA_AMALGAM_OUTRO:
                return AmalgamState;
            case DATA_ILLYSANNA_INTRO:
                return illysannaIntroState;
            }
            return 0;
        }

        bool HasPlayerUpperThan(float minZ)
        {
            bool valid = false;

            DoOnPlayers([&](Player* player)
            {
                if (player && player->GetPositionZ() >= minZ)
                    valid = true;
            });

            return valid;
        }

        void CreatureDies(Creature* creature, Unit* k)
        {
            switch (creature->GetEntry())
            {
            case NPC_FELSPITE_DOMINATOR:
                if (!felspiteDominatorGUID.empty())
                {
                    felspiteDominatorGUID.remove(creature->GetGUID());
                    if (felspiteDominatorGUID.empty())
                        events.CancelEvent(DATA_SMASHSPITE_FEL_BAT_EVENT);
                }
                break;
            }
        }
    };
};

void AddSC_instance_black_rook_hold()
{
    new instance_black_rook_hold();
}
