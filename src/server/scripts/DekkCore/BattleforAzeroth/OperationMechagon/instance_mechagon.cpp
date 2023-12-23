#include "ScriptMgr.h"
#include "Player.h"
#include "InstanceScript.h"
#include "Conversation.h"
#include "ObjectMgr.h"
#include "operation_mechagon.h"

/*DoorData const doorData[] =
{
    { , , DOOR_TYPE_ROOM },
    { , , DOOR_TYPE_ROOM },
    { , , DOOR_TYPE_ROOM },
};*/

class instance_operation_mechagon : public InstanceMapScript
{
public:
    instance_operation_mechagon() : InstanceMapScript("instance_operation_mechagon", 2097) { }

    struct instance_operation_mechagon_InstanceMapScript : public InstanceScript
    {
        instance_operation_mechagon_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
        }

        void OnPlayerEnter(Player* player) override
        {
            Conversation::CreateConversation(CONVERSATION_OPERATION_MECHAGON_INTRO, player, player->GetPosition(), { player->GetGUID() });
        }

        void OnCreatureCreate(Creature* /*cre*/) override
        {
            //    if (CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(cre->GetEntry()))
            //  {
            //    if (cInfo->MechanicImmuneMask == 0)
            //  {
            //    if (instance->IsMythic())
            //  if (cre->GetFaction() == 16)
            //  cre->SetBaseHealth(cre->GetMaxHealth() * 6.0f);

            //    if (instance->IsHeroic())
            //    if (cre->GetFaction() == 16)
            //      cre->SetBaseHealth(cre->GetMaxHealth() * 5.0f);

            //   else
            //   if (cre->GetFaction() == 16)
            //    cre->SetBaseHealth(cre->GetMaxHealth() * 4.0f);
            //     }
            //     }
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_operation_mechagon_InstanceMapScript(map);
    }
};

void AddSC_instance_operation_mechagon()
{
    new instance_operation_mechagon();
}
