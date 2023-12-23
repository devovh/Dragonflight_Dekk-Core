/*
 * Copyright 2021 DEKKCORE
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

#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "CombatAI.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GridNotifiersImpl.h"
#include "LFGMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PhasingHandler.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "ScriptedEscortAI.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "SceneHelper.h"
#include "TemporarySummon.h"
#include "Unit.h"
#include "Item.h"

enum VoldunQuests
{
    QUEST_MEMORY_BREACH = 48988,
    QUEST_CLEANSE_THE_MIND = 48887,

};

enum VoldunQuestObjectives
{
    QUEST_OBJECTIVE_RAKERAS_JOURNAL_PAGE = 290887,
    QUEST_OBJECTIVE_MEMORY_SEARCHED = 293542,
    QUEST_OBJECTIVE_MEDITATE = 293348,

};

enum VoldunQuestSpells
{
    SPELL_MEDITATING = 253810,
    SPELL_INNER_DEMON1 = 272182,
    SPELL_INNER_DEMON2 = 272176,

};

// 48887 Cleanse the Mind
class quest_cleanse_the_mind : public SpellScript
{
    PrepareSpellScript(quest_cleanse_the_mind);

    void HandleAfterCast() {

        if (Player* player = GetCaster()->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_CLEANSE_THE_MIND) == QUEST_STATUS_INCOMPLETE)
            {
                if (player->GetQuestObjectiveCounter(QUEST_OBJECTIVE_MEDITATE) == 0)
                {
                    player->GetScheduler().Schedule(6s, [player](TaskContext /*context*/)
                        {
                            if (player->HasAura(SPELL_MEDITATING))
                            {
                                Position spawnPos = { 551.202f, 3137.56f, 121.447f, 3.94f };
                                player->KilledMonsterCredit(127840);

                                if (TempSummon* tempSumm = player->SummonCreature(138292, spawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300s))
                                {
                                    player->CastSpell(tempSumm, SPELL_INNER_DEMON2);
                                    tempSumm->AddAura(SPELL_INNER_DEMON1, tempSumm);
                                    tempSumm->SetFaction(16);
                                }
                            }
                        });
                }
            }
        }
    }

    void Register()
    {
        AfterCast += SpellCastFn(quest_cleanse_the_mind::HandleAfterCast);
    }
};

// 48988 Memory Breach
struct quest_memory_breach : public QuestScript
{
    quest_memory_breach() : QuestScript("quest_memory_breach") { }

    void OnQuestObjectiveChange(Player* player, Quest const* /*quest*/, QuestObjective const& objective, int32 /*oldAmount*/, int32 /*newAmount*/) override
    {
        if (player->HasQuest(QUEST_MEMORY_BREACH))
            if (objective.ID == QUEST_OBJECTIVE_MEMORY_SEARCHED)
                switch (player->GetQuestObjectiveCounter(QUEST_OBJECTIVE_MEMORY_SEARCHED))
                {
                case 0: player->GetSceneMgr().PlaySceneByPackageId(2189, SceneFlag::None); break;
                case 2: player->GetSceneMgr().PlaySceneByPackageId(2193, SceneFlag::None); break;
                case 5: player->GetSceneMgr().PlaySceneByPackageId(2194, SceneFlag::None); break;
                case 7: player->GetSceneMgr().PlaySceneByPackageId(2199, SceneFlag::None); break;
                }

    }
};

void AddSC_zone_voldun()
{
    RegisterSpellScript(quest_cleanse_the_mind);
    RegisterQuestScript(quest_memory_breach);
}
