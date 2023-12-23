/*
* Copyright DekkCore 
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "InstanceScript.h"
#include "SpellAuras.h"

enum Spells
{
    SPELL_INVOKE_DESTINY     = 351680,// PHASE 1
    SPELL_DIVINES_PROBE      = 353603,// PHASE 1
    SPELL_FATED_CONJUNCTION  = 350355,// PHASE 1
    SPELL_CALL_OF_ETERNITY   = 356065,// PHASE 1
    SPELL_REALIGN_FATE       = 351969,// PHASE 2
    SPELL_DARKEST_DESTINY    = 353122,// PHASE 2
    SPELL_UNSTABLE_ACCRETION = 353696,// PHASE 2
};

 enum edata
{
     DATA_FATESCRIBE_ROH_KALO,
 };

//179390
struct Boss_Fatescribe_Roh_Kalo : public BossAI
{
    Boss_Fatescribe_Roh_Kalo(Creature* c) : BossAI(c, DATA_FATESCRIBE_ROH_KALO) { }

    void Reset() override
    {
        BossAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        JustEngagedWith(who);
        events.ScheduleEvent(SPELL_INVOKE_DESTINY, 5s);
      //  events.ScheduleEvent(SPELL_, 0s)
      //  events.ScheduleEvent(SPELL_, 0s);
    }

    void OnSpellFinished(SpellInfo const* spellInfo) override
    {
        switch (spellInfo->Id)
        {
        case SPELL_INVOKE_DESTINY:
        {
            std::list<Player*> targetList;
            me->GetPlayerListInGrid(targetList, 100.0f);
            for (Player* targets : targetList)
            {
                me->CastSpell(targets->GetPosition(), SPELL_INVOKE_DESTINY, true);
            }
            break;
        }

        ////case SPELL_:
        //{
        //    std::list<AreaTrigger*> atList;
        //    me->GetAreaTriggerListWithSpellIDInRange(atList, SPELL_, 100.0f);
        //    for (AreaTrigger* at : atList)
        //    {
        //        if (at->GetPositionZ() < -45.0f)
        //        {
        //            at->GetCaster()->CastSpell(at->GetPosition(), SPELL_, true);
        //            at->Remove();
        //        }
        //    }
        //    std::list<Player*> targetList;
        //    me->GetPlayerListInGrid(targetList, 100.0f);
        //    for (Player* targets : targetList)
        //    {
        //        if (targets->GetPositionZ() < -45.0f && !targets->HasAura(SPELL_))
        //            me->CastSpell(targets, SPELL_, true);
        //    }
        //    break;
        //}
        }    
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
        case SPELL_INVOKE_DESTINY:
            me->CastSpell(nullptr, SPELL_INVOKE_DESTINY, true);
            events.Repeat(20s, 25s); 
            break;

        ////case SPELL_:
        //    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0F, true))
        //    {
        //        me->CastSpell(target, SPELL_, true);
        //        me->AddAura(SPELL_, target);
        //    }
        //    events.Repeat(0s);
        //    break;

        ////case SPELL_1:
        //    me->CastSpell(nullptr, SPELL_, false);
        //    events.Repeat(0s);
        //    break;
        }
    }

    void JustReachedHome() override
    {
        _JustReachedHome();
        me->RemoveAllAreaTriggers();
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        me->RemoveAllAreaTriggers();
        _DespawnAtEvade();
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        me->RemoveAllAreaTriggers();
    }
};

void AddSC_Boss_Fatescribe_Roh_Kalo()
{
    RegisterCreatureAI(Boss_Fatescribe_Roh_Kalo);
};
