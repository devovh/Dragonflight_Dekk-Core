/*
 * Copyright 2021 AshamaneCore
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

#include "mists_of_tirna_scithe.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"

enum MistcallerCreatures
{
    NPC_ILLUSIONARY_CLONE   = 165108,
    NPC_ILLUSIONARY_VULPIN  = 165251,
    NPC_DODGE_BALL_TARGET   = 165231,
    NPC_DODGE_BALL_TARGET_2 = 172477,
};

enum MistcallerSpells
{
    SPELL_GUESSING_GAME             = 336499,
    SPELL_PENALIZING_BURST          = 321669,
    SPELL_OOPSIE                    = 321837,

    // normal = only 1 player, Heroic+ = all players
    SPELL_DODGE_BALL                = 321834,
    SPELL_DODGE_BALL_BALL           = 321836,
    SPELL_DODGE_BALL_ARROW          = 336752,
    SPELL_DODGE_BALL_SUMMON_STALKER = 336755,
    SPELL_DODGE_BALL_DAMAGE         = 336759,

    SPELL_PATTY_CAKE                = 321828,
    SPELL_PATTY_CAKE_ALLOW_DECAST   = 321830,

    SPELL_FREEZE_TAG                = 341709,
    SPELL_FREEZE_TAG_FIXATION       = 321891,
    SPELL_FREEZE_TAG_TARGET         = 326183,
    SPELL_FREEZING_BURST            = 321893,

    SPELL_CONVERSATION_DEFEAT       = 331572,
};

std::vector<Position> illusionaryClonesPositions =
{
    { -7187.540039f, 2423.229980f, 5459.620117f, 5.605860f },
    { -7155.729980f, 2435.820068f, 5459.500000f, 2.435880f },
    { -7150.870117f, 2407.020020f, 5459.520020f, 2.368790f },
    { -7175.859863f, 2402.550049f, 5459.450195f, 1.388280f },
};

std::map<uint8, uint32> BossSymbolsVisualKits
{
    { 1, SPELL_BOSS_VISUAL_KIT_EMPTY_FLOWER         },
    { 2, SPELL_BOSS_VISUAL_KIT_EMPTY_FLOWER_CIRCLE  },
    { 3, SPELL_BOSS_VISUAL_KIT_FILLED_FLOWER        },
    { 4, SPELL_BOSS_VISUAL_KIT_FILLED_FLOWER_CIRCLE },
    { 5, SPELL_BOSS_VISUAL_KIT_EMPTY_LEAF           },
    { 6, SPELL_BOSS_VISUAL_KIT_EMPTY_LEAF_CIRCLE    },
    { 7, SPELL_BOSS_VISUAL_KIT_FILLED_LEAF          },
    { 8, SPELL_BOSS_VISUAL_KIT_FILLED_LEAF_CIRCLE   },
};

// 164501
struct boss_mistcaller : public BossAI
{
    boss_mistcaller(Creature* creature) : BossAI(creature, DATA_MISTCALLER)
    {
        ApplyDefaultBossImmuneMask();
    }

    void ScheduleTasks() override
    {
        Talk(0);

        events.ScheduleEvent(SPELL_DODGE_BALL, 8100ms);
        events.ScheduleEvent(SPELL_PATTY_CAKE, 13400ms);
        events.ScheduleEvent(SPELL_FREEZE_TAG, 18400ms);

        damageEvents.ScheduleEventBelowHealthPct(SPELL_GUESSING_GAME, 70);
        damageEvents.ScheduleEventBelowHealthPct(SPELL_GUESSING_GAME, 40);
        damageEvents.ScheduleEventBelowHealthPct(SPELL_GUESSING_GAME, 10);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case SPELL_GUESSING_GAME:
                Talk(2);
                DoCastSelf(SPELL_GUESSING_GAME);
                break;
            case SPELL_PATTY_CAKE:
                DoCastVictim(SPELL_PATTY_CAKE_ALLOW_DECAST);
                DoCastVictim(SPELL_PATTY_CAKE);
                events.ScheduleEvent(SPELL_PATTY_CAKE, 20s, 26s);
                break;
            case SPELL_FREEZE_TAG:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 1))
                {
                    me->CastSpell(target, SPELL_FREEZE_TAG_TARGET, true);
                    me->CastSpell(target, SPELL_FREEZE_TAG, false);
                }
                events.ScheduleEvent(SPELL_FREEZE_TAG, 20s, 22s);
                break;
            case SPELL_DODGE_BALL:
            {
                Talk(1);
                if (!me->HasAura(SPELL_GUESSING_GAME))
                {
                    if (IsHeroic())
                        instance->DoCastSpellOnPlayers(SPELL_DODGE_BALL_SUMMON_STALKER, me, true);
                    else
                        DoCastVictim(SPELL_DODGE_BALL_SUMMON_STALKER);

                    DoCastVictim(SPELL_DODGE_BALL);
                }

                events.ScheduleEvent(SPELL_DODGE_BALL, 14600ms, 18s);
                break;
            }
        }
    }

    void JustSummoned(Creature* summon) override
    {
        BossAI::JustSummoned(summon);

        if (summon->GetEntry() == NPC_DODGE_BALL_TARGET ||
            summon->GetEntry() == NPC_DODGE_BALL_TARGET_2)
        {
            me->CastSpell(summon, SPELL_DODGE_BALL_ARROW, true);
        }

        if (summon->GetEntry() == NPC_ILLUSIONARY_VULPIN)
        {
            instance->DoOnPlayers([summon](Player* player)
            {
                if (player->HasAura(SPELL_FREEZE_TAG_TARGET))
                    summon->CastSpell(player, SPELL_FREEZE_TAG_FIXATION, false);
            });
        }
    }

   void OnSpellCast(SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_GUESSING_GAME)
            SpawnIllusionaryClone();

        if (spell->Id == SPELL_DODGE_BALL)
        {
            if (IsHeroic())
            {
                instance->DoOnPlayers([this](Player* player)
                {
                    if (player != me->GetVictim())
                    {
                        me->CastSpell(player, SPELL_DODGE_BALL_BALL, true);
                        me->CastSpell(player, SPELL_DODGE_BALL_DAMAGE, true);
                    }
                });
            }
        }
    }

    void SetData(uint32 index, uint32 data) override
    {
        if (index == DATA_MISTCALLER_CLUE_KILLED)
        {
            Talk(3);
            me->RemoveAurasDueToSpell(SPELL_GUESSING_GAME);
            summons.DespawnEntry(NPC_ILLUSIONARY_CLONE);
        }
    }

    void JustDied(Unit* attacker) override
    {
        BossAI::JustDied(attacker);

        me->SummonCreature(NPC_MISTCALLER_FRIENDLY_END, -7179.379883f, 2452.550049f, 5465.689941f, 5.331790f);

        if (Creature* droman = me->SummonCreature(NPC_DROMAN_AT_MISTCALLER_END, -7152.703613f, 2401.101563f, 5459.061523f, 2.039730f))
            droman->GetMotionMaster()->MovePoint(0, { -7166.991211f, 2430.085205f, 5459.236328f, 2.042085f });

        instance->DoCastSpellOnPlayers(SPELL_CONVERSATION_DEFEAT);
    }

private:
    void SpawnIllusionaryClone()
    {
        std::vector<uint8> symbols = GenerateSymbols();

        uint8 positionCount = 0;
        std::vector<uint8> positions = { 0, 1, 2, 3 };
        Trinity::Containers::RandomShuffle(positions);

        for (uint8 symbol : symbols)
            if (Creature* creature = me->SummonCreature(NPC_ILLUSIONARY_CLONE, illusionaryClonesPositions[positions[positionCount]]))
                creature->AI()->SetData(BossSymbolsVisualKits[symbol], positionCount++ == 0);
    }
};

// 165108
struct npc_mistcaller_illusionary_clone : public ScriptedAI
{
    npc_mistcaller_illusionary_clone(Creature* c) : ScriptedAI(c)
    {
        SetCombatMovement(false);
        SetBaseAttackSpell(SPELL_PENALIZING_BURST);
    }

    void Reset() override
    {
        events.ScheduleEvent(SPELL_PENALIZING_BURST, 5s);
    }

    void SetData(uint32 index, uint32 data) override
    {
        me->SendPlaySpellVisualKit(index, 2, 0);
        isSelectedClue = data != 0;
    }

    void JustDied(Unit* /*attacker*/) override
    {
        if (isSelectedClue)
            instance->SetData(DATA_MISTCALLER_CLUE_KILLED, 1);
        else
            DoCastAOE(SPELL_OOPSIE, true);
    }

private:
    bool isSelectedClue = false;
};

// Spell 326223, AT 23571
struct at_freeze_tag_fixation : public AreaTriggerAI
{
    at_freeze_tag_fixation(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (Unit* caster = at->GetCaster())
        {
            if (target->IsPlayer())
            {
                caster->CastSpell(target, SPELL_FREEZING_BURST, false);

                at->SetDuration(0);
                caster->Kill(caster, caster);
            }
        }
    }
};

void AddSC_boss_mistcaller()
{
    RegisterCreatureAI(boss_mistcaller);
    RegisterCreatureAI(npc_mistcaller_illusionary_clone);
    RegisterAreaTriggerAI(at_freeze_tag_fixation);
}
