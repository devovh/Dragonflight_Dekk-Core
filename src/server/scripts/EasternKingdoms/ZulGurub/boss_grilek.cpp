/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
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

#include "zulgurub.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"

enum Yells
{
};

enum Spells
{
    SPELL_AVARTAR                = 24646,                  //The Enrage Spell
    SPELL_GROUNDTREMOR           = 6524,
};

enum Events
{
};

struct boss_grilek : public BossAI
{
    boss_grilek(Creature* creature) : BossAI(creature, DATA_GRILEK)
    {   
    }

    uint32 Avartar_Timer;
    uint32 GroundTremor_Timer;

    void Reset() override
    {
        Avartar_Timer = 15000 + rand() % 10000;
        GroundTremor_Timer = 8000 + rand() % 8000;
    }

    void JustDied(Unit* /*killer*/) override
    {
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;
        //Avartar_Timer
        if (Avartar_Timer <= diff)
        {
            DoCast(me, SPELL_AVARTAR);
            Unit* pTarget = NULL;

            pTarget = SelectTarget(SelectTargetMethod::Random, 1);

            if (GetThreat(me->GetVictim()))
                ModifyThreatByPercent(me->GetVictim(), -50);
            if (pTarget)
                AttackStart(pTarget);

            Avartar_Timer = 25000 + rand() % 10000;
        }
        else Avartar_Timer -= diff;

        //GroundTremor_Timer
        if (GroundTremor_Timer <= diff)
        {
            DoCast(me->GetVictim(), SPELL_GROUNDTREMOR);
            GroundTremor_Timer = 12000 + rand() % 4000;
        }
        else GroundTremor_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_grilek()
{
    RegisterZulGurubCreatureAI(boss_grilek);
}
