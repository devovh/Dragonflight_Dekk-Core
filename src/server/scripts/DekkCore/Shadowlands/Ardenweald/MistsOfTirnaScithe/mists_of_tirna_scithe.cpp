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
#include "Conversation.h"
#include "Creature.h"
#include "ScriptMgr.h"

// 164929
struct npc_mots_tirnenn_villager : public ScriptedAI
{
    npc_mots_tirnenn_villager(Creature* c) : ScriptedAI(c) { }

    enum Spells
    {
        SPELL_SOUL_SHACKLE          = 321952,
        SPELL_BEWILDERING_POLLEN    = 321968,
        SPELL_REFRESHING_MIST       = 300155,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(10s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BEWILDERING_POLLEN);
            context.Repeat();
        });
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPctDamaged(20, damage))
        {
            if (me->HasAura(SPELL_SOUL_SHACKLE))
            {
                me->RemoveAurasDueToSpell(SPELL_SOUL_SHACKLE);

                me->GetScheduler().Schedule(2s, [this](TaskContext /*context*/)
                {
                    Talk(0);
                    DoCastAOE(SPELL_REFRESHING_MIST);
                });
            }
        }
    }
};

// 164920
struct npc_mots_drust_soulcleaver : public ScriptedAI
{
    npc_mots_drust_soulcleaver(Creature* c) : ScriptedAI(c) { }

    enum Spells
    {
        SPELL_HAND_OF_THROS     = 322569,
        SPELL_SOUL_SPLIT        = 322557,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(1s, 3s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_HAND_OF_THROS);
            context.Repeat(10s, 20s);
        });

        me->GetScheduler().Schedule(8s, 10s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_HAND_OF_THROS);
            context.Repeat(10s, 20s);
        });
    }
};

enum DurstHarvesterSpells
{
    SPELL_SPIRIT_BOLT           = 322767,
    SPELL_HARVEST_ESSENCE_AT    = 322938,
    SPELL_HARVEST_ESSENCE_LEECH = 322939,
};

// 164921
struct npc_mots_drust_harvester : public ScriptedAI
{
    npc_mots_drust_harvester(Creature* c) : ScriptedAI(c) { }

    void Reset() override
    {
        ScriptedAI::Reset();

        SetBaseAttackSpell(SPELL_SPIRIT_BOLT);
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, [this](TaskContext context)
        {
            DoCastAOE(SPELL_HARVEST_ESSENCE_AT);
            DoCastAOE(SPELL_HARVEST_ESSENCE_LEECH, true);
            context.Repeat(13s, 20s);
        });
    }
};

// Spell 322938, AT 23563
struct at_mots_harvest_essence : public AreaTriggerAI
{
    at_mots_harvest_essence(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitExit(Unit* target) override
    {
        target->RemoveAurasDueToSpell(SPELL_HARVEST_ESSENCE_LEECH);
    }
};

enum SpellsDurstBoughbreaker
{
    SPELL_FURIOUS_THRASHING     = 324909,
    SPELL_BRAMBLE_BURST         = 324923,
    SPELL_BRAMBLE_BURST_AT      = 325006,
    SPELL_BRAMBLE_BURST_DAMAGE  = 325027,
};

// 164926
struct npc_mots_drust_boughbreaker : public ScriptedAI
{
    npc_mots_drust_boughbreaker(Creature* c) : ScriptedAI(c) { }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->HealthBelowPctDamaged(50, damage))
            DoCastAOE(SPELL_FURIOUS_THRASHING);
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, [this](TaskContext context)
        {
            DoCastAOE(SPELL_BRAMBLE_BURST);
            context.Repeat();
        });
    }

    void SpellHitTarget(WorldObject* target, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_BRAMBLE_BURST)
            me->CastSpell(target, SPELL_BRAMBLE_BURST_AT);
    }
};

// Spell 322938, AT 23930
struct at_mots_bramble_burst : public AreaTriggerAI
{
    at_mots_bramble_burst(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (Unit* caster = at->GetCaster())
            if (target->IsPlayer())
                caster->CastSpell(target, SPELL_BRAMBLE_BURST_DAMAGE, true);
    }

    void OnUnitExit(Unit* target) override
    {
        target->RemoveAurasDueToSpell(SPELL_BRAMBLE_BURST_DAMAGE);
    }
};

enum SpellMistveilerDefender
{
    SPELL_PROTECTOR_RAGE        = 324736,
    SPELL_DEFENDER_ENRAGED      = 324737,
    SPELL_SPEAR_FLURRY_AT       = 331718,
    SPELL_SPEAR_FLURRY_DAMAGE   = 331721,
};

// All Maze npc should trigger protector rage
struct npc_mots_maze_npc : public ScriptedAI
{
    npc_mots_maze_npc(Creature* c) : ScriptedAI(c) { }

    void JustDied(Unit* /*attacker*/) override
    {
        std::vector<Creature*> defenders;
        me->GetCreatureListWithEntryInGrid(defenders, NPC_MISTVEIL_DEFENDER, 20.f);

        for (Creature* defender : defenders)
            if (defender->HasAura(SPELL_PROTECTOR_RAGE))
                defender->CastSpell(defender, SPELL_DEFENDER_ENRAGED, true);
    }
};

// 163058
// 171772
struct npc_mots_mistveil_defender : public npc_mots_maze_npc
{
    npc_mots_mistveil_defender(Creature* c) : npc_mots_maze_npc(c) { }

    void Reset() override
    {
        ScriptedAI::Reset();

        DoCastSelf(SPELL_PROTECTOR_RAGE);
    }

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(3s, 5s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SPEAR_FLURRY_AT);
            context.Repeat(10s, 15s);
        });
    }
};

// Spell 331718, AT 25051
struct at_mots_spear_flurry : public AreaTriggerAI
{
    at_mots_spear_flurry(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (Unit* caster = at->GetCaster())
            if (target->IsPlayer())
                caster->CastSpell(target, SPELL_SPEAR_FLURRY_DAMAGE, true);
    }

    void OnUnitExit(Unit* target) override
    {
        target->RemoveAurasDueToSpell(SPELL_SPEAR_FLURRY_DAMAGE);
    }
};

// 166299
struct npc_mots_mistveil_tender : public npc_mots_maze_npc
{
    npc_mots_mistveil_tender(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_NOURISH_THE_FOREST    = 324914,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(10s, 20s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_NOURISH_THE_FOREST);
            context.Repeat(20s, 30s);
        });
    }
};

// 166301
struct npc_mots_mistveil_stalker : public npc_mots_maze_npc
{
    npc_mots_mistveil_stalker(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_MISTVEIL_BITE = 324987,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, 15s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_MISTVEIL_BITE);
            context.Repeat(10s, 30s);
        });
    }
};

// 166304
struct npc_mots_mistveil_stinger : public npc_mots_maze_npc
{
    npc_mots_mistveil_stinger(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_ANIMA_INJECTION = 325223,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, 15s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_ANIMA_INJECTION);
            context.Repeat(10s, 30s);
        });
    }
};

// 166275
struct npc_mots_mistveil_shaper : public npc_mots_maze_npc
{
    npc_mots_mistveil_shaper(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_BRAMBLETHORN_COAT         = 324776,
        SPELL_BRAMBLETHORN_ENTANGLEMENT = 324859,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, 15s, [this](TaskContext context)
        {
            DoCastRandomFriendlyCreature(SPELL_BRAMBLETHORN_COAT, 20.f);
            context.Repeat(10s, 30s);
        });
    }
};

// 166276
struct npc_mots_mistveil_guardian : public npc_mots_maze_npc
{
    npc_mots_mistveil_guardian(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_BUCKING_RAMPAGE   = 331743,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, 15s, [this](TaskContext context)
        {
            DoCastAOE(SPELL_BUCKING_RAMPAGE);
            context.Repeat(10s, 15s);
        });
    }
};

// 173720
struct npc_mots_mistveil_gorgegullet : public npc_mots_maze_npc
{
    npc_mots_mistveil_gorgegullet(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_TONGUE_LASHING        = 340300,
        SPELL_POISONOUS_SECRETIONS  = 340304,
        SPELL_CRUSHING_LEAP         = 340305,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, 15s, [this](TaskContext context)
        {
            DoCastAOE(SPELL_POISONOUS_SECRETIONS);
            context.Repeat(20s, 30s);
        });

        me->GetScheduler().Schedule(4s, 10s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_TONGUE_LASHING);
            context.Repeat(9s, 15s);
        });

        me->GetScheduler().Schedule(10s, 15s, [this](TaskContext context)
        {
            DoCastRandom(SPELL_CRUSHING_LEAP, 0.0f);
            context.Repeat(10s, 20s);
        });
    }
};

// 167111
struct npc_mots_spinemaw_staghorn : public npc_mots_maze_npc
{
    npc_mots_spinemaw_staghorn(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_SIMULATE_RESISTANCE   = 326046,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, 15s, [this](TaskContext context)
        {
            DoCastAOE(SPELL_SIMULATE_RESISTANCE);
            context.Repeat(20s, 30s);
        });
    }
};

// 172312
struct npc_mots_spinemaw_gorger : public npc_mots_maze_npc
{
    npc_mots_spinemaw_gorger(Creature* c) : npc_mots_maze_npc(c) { }

    enum Spells
    {
        SPELL_ACID_GLOBULE   = 326021,
    };

    void JustEngagedWith(Unit* /*attacker*/) override
    {
        me->GetScheduler().Schedule(5s, 10s, [this](TaskContext context)
        {
            DoCastAOE(SPELL_ACID_GLOBULE);
            context.Repeat(10s, 15s);
        });
    }
};

/*******************
 *   MAZE SYSTEM   *
 *******************/

enum Actions
{
    ACTION_DISPLAY_MARKER   = 1,
    ACTION_HIDE_MARKER      = 2,
    ACTION_OPEN_MIST_DOOR   = 3,
};

std::map<uint8, Position> mistcallerMazePositions =
{
    { 2, { -6986.797363f, 2248.858154f, 5481.021973f, 3.920448f }},
    { 4, { -7069.695313f, 2191.980225f, 5488.137207f, 0.547158f }},
    { 8, { -7065.382324f, 2273.932617f, 5477.210449f, 5.353799f }},
};

// 166885
struct npc_mistcaller_maze_entrance : public ScriptedAI
{
    npc_mistcaller_maze_entrance(Creature* c) : ScriptedAI(c) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!_introDone && me->GetDistance(who) < 100.f)
        {
            Talk(0);

            instance->DoCastSpellOnPlayers(SPELL_CONVERSATION_INITIAL_MAZE);

            std::vector<Creature*> defenders;
            me->GetCreatureListWithEntryInGrid(defenders, NPC_MISTVEIL_DEFENDER_MAZE_ENTRANCE, 20.f);
            for (Creature* defender : defenders)
                defender->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);

            me->GetScheduler().Schedule(500ms, [this](TaskContext /*context*/)
            {
                std::vector<Creature*> soulCleavers;
                me->GetCreatureListWithEntryInGrid(soulCleavers, NPC_DRUST_SOULCLEAVER, 20.f);
                for (Creature* soulCleaver : soulCleavers)
                    me->Kill(me, soulCleaver);
            });

            if (uint32 nextMazeZone = instance->GetData(DATA_MAZE_NEXT_DOOR))
                me->GetMotionMaster()->MovePoint(0, mistcallerMazePositions[nextMazeZone]);

            me->DespawnOrUnsummon(5s);

            _introDone = true;
        }
    }

private:
    bool _introDone = false;
};

// 166188
struct npc_mots_clue_stalker : public ScriptedAI
{
    npc_mots_clue_stalker(Creature* c) : ScriptedAI(c) { }

    void SetData(uint32 index, uint32 data) override
    {
        markerId = index;
        behaviour = data;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_DISPLAY_MARKER:
                me->SendPlaySpellVisualKit(markerId, 2, 0);
                break;
            case ACTION_HIDE_MARKER:
                me->SendPlaySpellVisualKit(markerId, 0, 1250);
                break;
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (markerId && instance && who->IsPlayer())
        {
            float distance = me->GetDistance(who);
            if (distance >= 1.5f && distance <= 7.f)
                if (me->isInBack(who))
                    instance->SetData(DATA_MAZE_DOOR_REACHED, behaviour);
        }
    }

private:
    uint32 markerId = 0;
    uint32 behaviour = CLUE_STALKER_NO_ACTION;
};

// Spell 324723, AT 1000000 (custom, not in sniffs)
struct at_mots_maze_clue_revealer : public AreaTriggerAI
{
    at_mots_maze_clue_revealer(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        VALIDATE_CASTER();

        if (target->IsPlayer())
            if (Creature* creature = caster->ToCreature())
                creature->AI()->DoAction(ACTION_DISPLAY_MARKER);
    }

    void OnUnitExit(Unit* target) override
    {
        if (at->GetInsidePlayers().size() != 0)
            return;

        VALIDATE_CASTER();

        if (Creature* creature = caster->ToCreature())
            creature->AI()->DoAction(ACTION_HIDE_MARKER);
    }
};

// Spell 335176, AT 25437
struct at_mots_maze_trash_apparition : public AreaTriggerAI
{
    at_mots_maze_trash_apparition(AreaTrigger* at) : AreaTriggerAI(at) { }

    void OnUnitEnter(Unit* target) override
    {
        if (Unit* caster = at->GetCaster())
            if (target->IsPlayer())
                caster->CastSpell(caster, SPELL_MIST_NPC_APPEAR_MISSILE, false);
    }
};

/*
 * Return a list of 4 indexes in MazeSymbolsVisualKits.
 * First one is always the solution
 */
std::vector<uint8> GenerateSymbols()
{
    std::vector<uint8> allFlags = { MAZE_VISUAL_FLAG_LEAF,   MAZE_VISUAL_FLAG_FILLED, MAZE_VISUAL_FLAG_CIRCLE,
                                    MAZE_VISUAL_FLAG_FLOWER, MAZE_VISUAL_FLAG_EMPTY,  MAZE_VISUAL_FLAG_NO_CIRCLE };
    uint8 commonFlag = Trinity::Containers::SelectRandomContainerElement(allFlags);

    std::map<uint8, uint8> wrongMazeSymbols = MazeSymbols;

   /* Trinity::Containers::RandomResize(wrongMazeSymbols, [commonFlag](auto const& mazeSymbolItr)
    {
        return mazeSymbolItr.second & commonFlag;
    }, 3);*/

    std::map<uint8, uint8> flagCounter =
    {
        { MAZE_VISUAL_FLAG_LEAF,    0 },
        { MAZE_VISUAL_FLAG_FILLED,  0 },
        { MAZE_VISUAL_FLAG_CIRCLE,  0 },
    };

    std::map<uint8, uint8> flagOpposites =
    {
        { MAZE_VISUAL_FLAG_LEAF,    MAZE_VISUAL_FLAG_FLOWER     },
        { MAZE_VISUAL_FLAG_FILLED,  MAZE_VISUAL_FLAG_EMPTY      },
        { MAZE_VISUAL_FLAG_CIRCLE,  MAZE_VISUAL_FLAG_NO_CIRCLE  },
    };

    for (auto const& mazeSymbolItr : wrongMazeSymbols)
        for (auto& flagCounterItr : flagCounter)
            if (mazeSymbolItr.second & flagCounterItr.first)
                flagCounterItr.second++;

    uint8 solutionFlag = 0;
    for (auto& flagCounterItr : flagCounter)
        solutionFlag |= flagCounterItr.second < 2 ? flagCounterItr.first : flagOpposites[flagCounterItr.first];

    std::map<uint8, uint8> solutionMazeSymbols = MazeSymbols;
   /* Trinity::Containers::RandomResize(solutionMazeSymbols, [solutionFlag](auto const& mazeSymbolItr)
    {
        return mazeSymbolItr.second == solutionFlag;
    }, 0);*/

    std::vector<uint8> result;
    if (solutionMazeSymbols.empty())
        return result;

    result.push_back(solutionMazeSymbols.begin()->first);

    for (auto const& mazeSymbolItr : wrongMazeSymbols)
        result.push_back(mazeSymbolItr.first);

    return result;
}

/**************************
 *   END OF MAZE SYSTEM   *
 **************************/

// 15119
struct conversation_mots_mistcaller_end : public ConversationScript
{
    conversation_mots_mistcaller_end() : ConversationScript("conversation_mots_mistcaller_end") { }

    //void OnConversationCreate(Conversation* /*conversation*/, Unit* creator) override
    //{
    //    if (InstanceScript* instanceScript = creator->GetInstanceScript())
    //    {
    //        if (Unit* mistcaller = instanceScript->GetCreature(NPC_MISTCALLER_FRIENDLY_END))
    //            conversation->AddActor(mistcaller->GetGUID(),0, 0);

    //        if (Unit* droman = instanceScript->GetCreature(NPC_DROMAN_AT_MISTCALLER_END))
    //            conversation->AddActor(droman->GetGUID(), 1);
    //    }
    //}
};

// 15122
struct conversation_mots_tredova_end : public ConversationScript
{
    conversation_mots_tredova_end() : ConversationScript("conversation_mots_tredova_end") { }

  /*  void OnConversationCreate(Conversation* conversation, Unit* creator) override
    {
        if (InstanceScript* instanceScript = creator->GetInstanceScript())
        {
            if (Unit* droman = instanceScript->GetCreature(NPC_DROMAN_AT_TREDOVA_END))
                conversation->AddActor(droman->GetGUID(), 0);
        }
    }*/
};

void AddSC_mists_of_tirna_scithe()
{
    RegisterCreatureAI(npc_mots_tirnenn_villager);
    RegisterCreatureAI(npc_mots_drust_soulcleaver);
    RegisterCreatureAI(npc_mots_drust_harvester);
    RegisterCreatureAI(npc_mots_drust_boughbreaker);
    RegisterCreatureAI(npc_mots_maze_npc);
    RegisterCreatureAI(npc_mots_mistveil_defender);
    RegisterCreatureAI(npc_mots_mistveil_tender);
    RegisterCreatureAI(npc_mots_mistveil_stalker);
    RegisterCreatureAI(npc_mots_mistveil_stinger);
    RegisterCreatureAI(npc_mots_mistveil_shaper);
    RegisterCreatureAI(npc_mots_mistveil_guardian);
    RegisterCreatureAI(npc_mots_mistveil_gorgegullet);
    RegisterCreatureAI(npc_mots_spinemaw_staghorn);
    RegisterCreatureAI(npc_mots_spinemaw_gorger);
    RegisterCreatureAI(npc_mistcaller_maze_entrance);
    RegisterCreatureAI(npc_mots_clue_stalker);
    RegisterAreaTriggerAI(at_mots_harvest_essence);
    RegisterAreaTriggerAI(at_mots_bramble_burst);
    RegisterAreaTriggerAI(at_mots_spear_flurry);
    RegisterAreaTriggerAI(at_mots_maze_clue_revealer);
    RegisterAreaTriggerAI(at_mots_maze_trash_apparition);
    RegisterConversationScript(conversation_mots_mistcaller_end);
    RegisterConversationScript(conversation_mots_tredova_end);
}
