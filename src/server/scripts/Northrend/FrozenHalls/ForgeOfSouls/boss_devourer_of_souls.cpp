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

#include "ScriptMgr.h"
#include "forge_of_souls.h"
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ScriptedCreature.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "PhasingHandler.h"

 /*
  * @todo
  * - Fix model id during unleash soul -> seems DB issue 36503 is missing (likewise 36504 is also missing).
  * - Fix outro npc movement
  */

enum Yells
{
    SAY_FACE_AGGRO = 0,
    SAY_FACE_ANGER_SLAY = 1,
    SAY_FACE_SORROW_SLAY = 2,
    SAY_FACE_DESIRE_SLAY = 3,
    SAY_FACE_DEATH = 4,
    EMOTE_MIRRORED_SOUL = 5,
    EMOTE_UNLEASH_SOUL = 6,
    SAY_FACE_UNLEASH_SOUL = 7,
    EMOTE_WAILING_SOUL = 8,
    SAY_FACE_WAILING_SOUL = 9,

    SAY_JAINA_OUTRO = 0,
    SAY_SYLVANAS_OUTRO = 0
};

enum Spells
{
    SPELL_PHANTOM_BLAST = 68982,
    H_SPELL_PHANTOM_BLAST = 70322,
    SPELL_MIRRORED_SOUL_PROC_AURA = 69023,
    SPELL_MIRRORED_SOUL_DAMAGE = 69034,
    SPELL_MIRRORED_SOUL_TARGET_SELECTOR = 69048,
    SPELL_MIRRORED_SOUL_BUFF = 69051,
    SPELL_WELL_OF_SOULS = 68820,
    SPELL_UNLEASHED_SOULS = 68939,
    SPELL_WAILING_SOULS_STARTING = 68912,  // Initial spell cast at begining of wailing souls phase
    SPELL_WAILING_SOULS_BEAM = 68875,  // the beam visual
    SPELL_WAILING_SOULS = 68873,  // the actual spell
    H_SPELL_WAILING_SOULS = 70324,
    //    68871, 68873, 68875, 68876, 68899, 68912, 70324,
    // 68899 trigger 68871
};

enum Events
{
    EVENT_PHANTOM_BLAST = 1,
    EVENT_MIRRORED_SOUL = 2,
    EVENT_WELL_OF_SOULS = 3,
    EVENT_UNLEASHED_SOULS = 4,
    EVENT_WAILING_SOULS = 5,
    EVENT_WAILING_SOULS_TICK = 6,
    EVENT_FACE_ANGER = 7,
};

enum Models
{
    DISPLAY_ANGER = 30148,
    DISPLAY_SORROW = 30149,
    DISPLAY_DESIRE = 30150,
};

struct outroPosition
{
    uint32 entry[2];
    Position movePosition[2];
} outroPositions[] =
{
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5590.47f, 2427.79f, 705.935f, 0.802851f }, { 5636.620605f, 2518.944824f, 708.697205f, 5.754216f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5593.59f, 2428.34f, 705.935f, 0.977384f }, { 5638.598633f, 2522.329102f, 708.697205f, 5.754216f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5600.81f, 2429.31f, 705.935f, 0.890118f }, { 5641.459961f, 2527.224365f, 708.697205f, 5.754216f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5600.81f, 2421.12f, 705.935f, 0.890118f }, { 5686.540039f, 2495.989990f, 708.697021f, 2.566470f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5601.43f, 2426.53f, 705.935f, 0.890118f }, { 5684.239746f, 2492.439209f, 708.696777f, 2.566466f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5601.55f, 2418.36f, 705.935f, 1.15192f }, { 5681.783691f, 2488.650391f, 708.696777f, 2.566466f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5598.f, 2429.14f, 705.935f, 1.0472f }, { 5674.958008f, 2529.102539f, 714.693237f, 0.930849f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5594.04f, 2424.87f, 705.935f, 1.15192f }, { 5677.311523f, 2527.351074f, 714.693237f, 0.930849f } } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { { 5597.89f, 2421.54f, 705.935f, 0.610865f }, { 5679.529297f, 2525.699951f, 714.693237f, 0.930849f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { { 5598.57f, 2434.62f, 705.935f, 1.13446f }, { 5669.017090f, 2540.908936f, 714.693237f, 0.332356f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { { 5585.46f, 2417.99f, 705.935f, 1.06465f }, { 5669.942383f, 2538.229248f, 714.693237f, 0.332356f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { { 5605.81f, 2428.42f, 705.935f, 0.820305f }, { 5670.960938f, 2535.278076f, 714.693237f, 0.332356f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { { 5591.61f, 2412.66f, 705.935f, 0.925025f }, { 5692.048340f, 2522.588379f, 714.693237f, 1.428820f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { { 5593.9f, 2410.64f, 705.935f, 0.872665f }, { 5688.507324f, 2523.094482f, 714.693237f, 1.428820f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { { 5586.76f, 2416.73f, 705.935f, 0.942478f }, { 5684.896484f, 2523.610840f, 714.693237, 1.428820f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_3_HORDE }, { { 5592.23f, 2419.14f, 705.935f, 0.855211f }, { 5680.145020f, 2532.512695f, 714.691833f, 0.822518f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_3_HORDE }, { { 5594.61f, 2416.87f, 705.935f, 0.907571f }, { 5690.972168, 2538.061523f, 714.692505f, 3.628703f } } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_3_HORDE }, { { 5589.77f, 2421.03f, 705.935f, 0.855211f }, { 5684.645508f, 2543.597900f, 714.692505f, 4.343395f } } },

    { { NPC_KORELN, NPC_LORALEN }, { { 5602.58f, 2435.95f, 705.935f, 0.959931f }, { 5640.061523f, 2500.716309f, 708.696960f, 0.808187f } } },
    { { NPC_ELANDRA, NPC_KALIRA }, { { 5606.13f, 2433.16f, 705.935f, 0.785398f }, { 5643.911133f, 2496.320801f, 708.696960f, 1.047733f } } },
    { { NPC_JAINA_PART2, NPC_SYLVANAS_PART2 }, { { 5606.12f, 2436.6f, 705.935f, 0.890118f }, { 5643.380859f, 2499.880371f, 708.697083f, 0.925953f } } },

    { { 0, 0 }, { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } } }
};


Position const CrucibleSummonPos = { 5672.294f, 2520.686f, 713.4386f, 0.9599311f };

enum Misc
{
    DATA_THREE_FACED = 1
};

struct boss_devourer_of_souls : public BossAI
{
    boss_devourer_of_souls(Creature* creature) : BossAI(creature, DATA_DEVOURER_OF_SOULS)
    {
        Initialize();
        beamAngle = 0.f;
        beamAngleDiff = 0.f;
        wailingSoulTick = 0;
    }

    void Initialize()
    {
        threeFaced = true;
    }

    void Reset() override
    {
        _Reset();
        me->SetControlled(false, UNIT_STATE_ROOT);
        me->SetDisplayId(DISPLAY_ANGER);
        me->SetReactState(REACT_AGGRESSIVE);

        Initialize();
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        Talk(SAY_FACE_AGGRO);

        if (!me->FindNearestCreature(NPC_CRUCIBLE_OF_SOULS, 60)) // Prevent double spawn
            instance->instance->SummonCreature(NPC_CRUCIBLE_OF_SOULS, CrucibleSummonPos);
        events.ScheduleEvent(EVENT_PHANTOM_BLAST, 5s);
        events.ScheduleEvent(EVENT_MIRRORED_SOUL, 8s);
        events.ScheduleEvent(EVENT_WELL_OF_SOULS, 30s);
        events.ScheduleEvent(EVENT_UNLEASHED_SOULS, 20s);
        events.ScheduleEvent(EVENT_WAILING_SOULS, 60s, 70s);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        uint8 textId = 0;
        switch (me->GetDisplayId())
        {
        case DISPLAY_ANGER:
            textId = SAY_FACE_ANGER_SLAY;
            break;
        case DISPLAY_SORROW:
            textId = SAY_FACE_SORROW_SLAY;
            break;
        case DISPLAY_DESIRE:
            textId = SAY_FACE_DESIRE_SLAY;
            break;
        default:
            break;
        }

        if (textId)
            Talk(textId);
    }

    void JustDied(Unit* killer) override
    {
        _JustDied();

        Position spawnPoint = { 5618.139f, 2451.873f, 705.854f, 0 };

        Talk(SAY_FACE_DEATH);

        int32 entryIndex;
        if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
            entryIndex = 0;
        else
            entryIndex = 1;

        for (int8 i = 0; outroPositions[i].entry[entryIndex] != 0; ++i)
        {
            const uint32 NpcEntry = outroPositions[i].entry[entryIndex];
            const Position SpawnPos = outroPositions[i].movePosition[0];
            const Position EndPos = outroPositions[i].movePosition[1];

            if (TempSummon* summon = instance->instance->SummonCreature(NpcEntry, SpawnPos))
            {
                PhasingHandler::SetAlwaysVisible(summon, true, true);
                if (summon->GetEntry() == NPC_JAINA_PART2 || summon->GetEntry() == NPC_SYLVANAS_PART2)
                {
                    summon->GetMotionMaster()->MovePoint(0, EndPos, true, EndPos.GetOrientation(), {}, MovementWalkRunSpeedSelectionMode::ForceWalk);
                    summon->AddDelayedEvent(15000, [summon]() -> void
                        {
                            summon->AI()->Talk(0);
                        });

                    summon->AddDelayedEvent(25000, [summon, killer]() -> void
                        {
                            Position PortalSpawnPos = Position(5684.6f, 2536.9f, 715.681f, 0.94f);

                            GameObject* PortalVisual1 = summon->SummonGameObject(201960, PortalSpawnPos, QuaternionData::fromEulerAnglesZYX(PortalSpawnPos.GetOrientation(), 0.0f, 0.0f), 3600s, GOSummonType::GO_SUMMON_TIMED_DESPAWN);
                            if (PortalVisual1)
                                PhasingHandler::SetAlwaysVisible(PortalVisual1, true, true);

                            GameObject* PortalVisual2 = summon->SummonGameObject(201961, PortalSpawnPos, QuaternionData::fromEulerAnglesZYX(PortalSpawnPos.GetOrientation(), 0.0f, 0.0f), 3600s, GOSummonType::GO_SUMMON_TIMED_DESPAWN);
                            if (PortalVisual2)
                                PhasingHandler::SetAlwaysVisible(PortalVisual2, true, true);

                            // Enable hackfix to see 5688 areatrigger (which teleports the player to pos)
                            if (Group* group = killer->ToPlayer()->GetGroup())
                            {
                                Group::MemberSlotList const& members = group->GetMemberSlots();

                                for (Group::member_citerator itr = members.begin(); itr != members.end(); ++itr)
                                {
                                    Player* GroupMember = ObjectAccessor::GetPlayer(killer->GetMap(), itr->guid);
                                    if (!GroupMember)
                                        continue;

                                    PhasingHandler::SetAlwaysVisible(GroupMember, true, false);
                                }
                            }
                            else
                            {
                                if (Player* player = killer->ToPlayer())
                                    PhasingHandler::SetAlwaysVisible(player, true, false);
                            }

                        });
                }
                else if (summon->GetEntry() == NPC_KALIRA || summon->GetEntry() == NPC_ELANDRA || summon->GetEntry() == NPC_LORALEN || summon->GetEntry() == NPC_KORELN)
                {
                    summon->GetMotionMaster()->MovePoint(0, EndPos, true, EndPos.GetOrientation(), {}, MovementWalkRunSpeedSelectionMode::ForceWalk);
                }
                else
                {
                    summon->GetMotionMaster()->MovePoint(0, EndPos, true, EndPos.GetOrientation(), {}, MovementWalkRunSpeedSelectionMode::ForceRun);
                }
            }
        }
    }

    void SpellHitTarget(WorldObject* /*target*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == H_SPELL_PHANTOM_BLAST)
            threeFaced = false;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == DATA_THREE_FACED)
            return threeFaced;

        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        // Return since we have no target
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_PHANTOM_BLAST:
                DoCastVictim(SPELL_PHANTOM_BLAST);
                events.ScheduleEvent(EVENT_PHANTOM_BLAST, 5s);
                break;
            case EVENT_MIRRORED_SOUL:
                DoCastAOE(SPELL_MIRRORED_SOUL_TARGET_SELECTOR);
                Talk(EMOTE_MIRRORED_SOUL);
                events.ScheduleEvent(EVENT_MIRRORED_SOUL, 15s, 30s);
                break;
            case EVENT_WELL_OF_SOULS:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                    DoCast(target, SPELL_WELL_OF_SOULS);
                events.ScheduleEvent(EVENT_WELL_OF_SOULS, 20s);
                break;
            case EVENT_UNLEASHED_SOULS:
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                    DoCast(target, SPELL_UNLEASHED_SOULS);
                me->SetDisplayId(DISPLAY_SORROW);
                Talk(SAY_FACE_UNLEASH_SOUL);
                Talk(EMOTE_UNLEASH_SOUL);
                events.ScheduleEvent(EVENT_UNLEASHED_SOULS, 30s);
                events.ScheduleEvent(EVENT_FACE_ANGER, 5s);
                break;
            case EVENT_FACE_ANGER:
                me->SetDisplayId(DISPLAY_ANGER);
                break;

            case EVENT_WAILING_SOULS:
                me->SetDisplayId(DISPLAY_DESIRE);
                Talk(SAY_FACE_WAILING_SOUL);
                Talk(EMOTE_WAILING_SOUL);
                DoCast(me, SPELL_WAILING_SOULS_STARTING);
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                {
                    me->SetFacingToObject(target);
                    DoCast(me, SPELL_WAILING_SOULS_BEAM);
                }

                beamAngle = me->GetOrientation();

                beamAngleDiff = float(M_PI) / 30.0f; // PI/2 in 15 sec = PI/30 per tick
                if (RAND(true, false))
                    beamAngleDiff = -beamAngleDiff;

                me->InterruptNonMeleeSpells(false);
                me->SetReactState(REACT_PASSIVE);

                //Remove any target
                me->SetTarget(ObjectGuid::Empty);

                me->GetMotionMaster()->Clear();
                me->SetControlled(true, UNIT_STATE_ROOT);

                wailingSoulTick = 15;
                events.DelayEvents(18s); // no other events during wailing souls
                events.ScheduleEvent(EVENT_WAILING_SOULS_TICK, 3s); // first one after 3 secs.
                break;

            case EVENT_WAILING_SOULS_TICK:
                beamAngle += beamAngleDiff;
                me->SetFacingTo(beamAngle);

                DoCast(me, SPELL_WAILING_SOULS);

                if (--wailingSoulTick)
                    events.ScheduleEvent(EVENT_WAILING_SOULS_TICK, 1s);
                else
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetDisplayId(DISPLAY_ANGER);
                    me->SetControlled(false, UNIT_STATE_ROOT);
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                    events.ScheduleEvent(EVENT_WAILING_SOULS, 60s, 70s);
                }
                break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    bool threeFaced;

    // wailing soul event
    float beamAngle;
    float beamAngleDiff;
    int8 wailingSoulTick;
};

// 69051 - Mirrored Soul
class spell_devourer_of_souls_mirrored_soul : public SpellScript
{
    PrepareSpellScript(spell_devourer_of_souls_mirrored_soul);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MIRRORED_SOUL_PROC_AURA });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(GetCaster(), SPELL_MIRRORED_SOUL_PROC_AURA, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_devourer_of_souls_mirrored_soul::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 69023 - Mirrored Soul (Proc)
class spell_devourer_of_souls_mirrored_soul_proc : public AuraScript
{
    PrepareAuraScript(spell_devourer_of_souls_mirrored_soul_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MIRRORED_SOUL_DAMAGE });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return GetCaster() && GetCaster()->IsAlive();
    }

    void HandleProc(AuraEffect* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
        args.AddSpellBP0(CalculatePct(damageInfo->GetDamage(), 45));
        GetTarget()->CastSpell(GetCaster(), SPELL_MIRRORED_SOUL_DAMAGE, args);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_devourer_of_souls_mirrored_soul_proc::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_devourer_of_souls_mirrored_soul_proc::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 69048 - Mirrored Soul (Target Selector)
class spell_devourer_of_souls_mirrored_soul_target_selector : public SpellScript
{
    PrepareSpellScript(spell_devourer_of_souls_mirrored_soul_target_selector);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MIRRORED_SOUL_BUFF });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
        targets.clear();
        targets.push_back(target);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_MIRRORED_SOUL_BUFF, false);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_devourer_of_souls_mirrored_soul_target_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_devourer_of_souls_mirrored_soul_target_selector::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class achievement_three_faced : public AchievementCriteriaScript
{
public:
    achievement_three_faced() : AchievementCriteriaScript("achievement_three_faced") { }

    bool OnCheck(Player* /*player*/, Unit* target) override
    {
        if (!target)
            return false;

        if (Creature* Devourer = target->ToCreature())
            if (Devourer->AI()->GetData(DATA_THREE_FACED))
                return true;

        return false;
    }
};

// 5688 - Icecrown Dungeon - Pit of Saron - Entrance Target
// Disable hackfix to see 5688 areatrigger (which teleports the player to pos) & teleport the player
class AreaTrigger_at_fos_portal_to_pos : public AreaTriggerScript
{
public:

    AreaTrigger_at_fos_portal_to_pos() : AreaTriggerScript("AreaTrigger_at_fos_portal_to_pos") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
    {
        PhasingHandler::SetAlwaysVisible(player, player->HasAuraType(SPELL_AURA_PHASE_ALWAYS_VISIBLE), false);

        return false;
    }
};

void AddSC_boss_devourer_of_souls()
{
    RegisterForgeOfSoulsCreatureAI(boss_devourer_of_souls);
    RegisterSpellScript(spell_devourer_of_souls_mirrored_soul);
    RegisterSpellScript(spell_devourer_of_souls_mirrored_soul_proc);
    RegisterSpellScript(spell_devourer_of_souls_mirrored_soul_target_selector);
    new achievement_three_faced();
    new AreaTrigger_at_fos_portal_to_pos();
}
