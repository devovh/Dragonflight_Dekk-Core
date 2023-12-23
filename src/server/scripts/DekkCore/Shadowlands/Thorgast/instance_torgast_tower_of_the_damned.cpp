#include "ScriptMgr.h"
#include "Player.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "InstanceScript.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "torghast_tower_of_the_damned.h"

struct instance_torgast_tower_of_the_damned : public InstanceScript
{
    instance_torgast_tower_of_the_damned(InstanceMap* map) : InstanceScript(map) { } 
};

void AddSC_instance_torgast_tower_of_the_damned()
{
    RegisterInstanceScript(instance_torgast_tower_of_the_damned, 2162);
}
