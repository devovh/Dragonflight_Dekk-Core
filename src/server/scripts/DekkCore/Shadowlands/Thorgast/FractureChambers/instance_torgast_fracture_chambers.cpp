#include "ScriptMgr.h"
#include "Player.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "InstanceScript.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "torgast_fracture_chambers.h"

/**ServerToClient: SMSG_UPDATE_OBJECT (0x27CB) Length: 50 ConnIdx: 1 Time: 05/10/2021 23:55:04.938 Number: 95016
NumObjUpdates: 1
MapID: 2162 (Torghast, la Torre de los Condenados)
HasRemovedObjects: False
**/

/**ServerToClient: SMSG_SCENARIO_STATE (0x2628) Length: 414 ConnIdx: 1 Time: 05/10/2021 23:55:04.938 Number: 95017
ScenarioID: 1849
CurrentStep: 4648
DifficultyID: 0 (0)
**/


//2162
struct instance_torgast_fracture_chambers : public InstanceScript
{
    instance_torgast_fracture_chambers(InstanceMap* map) : InstanceScript(map) { }
};
