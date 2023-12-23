#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "SpellAuraEffects.h"
#include "Map.h"
#include "necrotic_wake.h"

enum SurgeonSpells
{
	//Surgeon Stitchflesh
	SPELL_NOXIOUS_FOG      = 327100,
	SPELL_EMBALMING_ICHOR  = 327664,
	SPELL_STITCHNEEDLE     = 320200,
	SPELL_SEVER_FLESH      = 334488,
	SPELL_MORBID_FIXATION  = 343555,
	SPELL_ESCAPE           = 320359,
	//Stitchflesh's Creation
    SPELL_MEAT_HOOK        = 320208,
    SPELL_MEAT_HOOK_2      = 322548,
    SPELL_MEAT_HOOK_3      = 322681,
    SPELL_AWAKEN_CREATION  = 320358,
    SPELL_FESTERING_ROT    = 348170,
    SPELL_LETHARGY         = 326868,
    SPELL_MEERAHS_JUKEBOX  = 288865,
    SPELL_MUTILATE         = 320376,
    SPELL_SHATTERED_PSYCHE = 344663,
    SPELL_SPELL_SEVER_FLESH = 334488,
};

enum SurgeonCreatures
{
	NPC_STITCHFLESHS_CREATION = 164578,
	NPC_STITCHING_ASSISTANT   = 173044,
	NPC_SEPARATION_ASSISTANT  = 167731,
	NPC_GOREGRIND_BITS        = 163622,
	NPC_GOREGRIND             = 163621,
	NPC_ROTSPEW               = 163620,
	NPC_ROTSPEW_LEFTOVERS     = 163623,
};

//166882
struct boss_surgeon_stitchflesh : public BossAI
{
    boss_surgeon_stitchflesh(Creature* creature) : BossAI(creature, DATA_SURGEON_STITCHFLESH) { }

    void Reset() override
    {
        BossAI::Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith(who);
        Talk(0);
      //  if (auto* encounterDoor = me->FindNearestGameObject(GO_SURGEON_STITCHFLESH_EXIT, 100.0f))
          //  encounterDoor->SetGoState(GO_STATE_READY);
        events.ScheduleEvent(SPELL_NOXIOUS_FOG, 3s);
        events.ScheduleEvent(SPELL_EMBALMING_ICHOR, 5s);
        events.ScheduleEvent(SPELL_STITCHNEEDLE, 10s);
        events.ScheduleEvent(SPELL_SPELL_SEVER_FLESH, 3s);
        events.ScheduleEvent(SPELL_MORBID_FIXATION, 3s);
        events.ScheduleEvent(SPELL_ESCAPE, 3s);
    }

    void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override 
    {
        Unit* ptarget = me->GetVictim();

        switch (spellInfo->Id)
        {
        case SPELL_NOXIOUS_FOG:
            me->AddAura(SPELL_NOXIOUS_FOG, ptarget);
            break;
        case SPELL_EMBALMING_ICHOR:
            me->AddAura(SPELL_EMBALMING_ICHOR, ptarget);
            break;
        case SPELL_STITCHNEEDLE:
            me->AddAura(SPELL_STITCHNEEDLE, ptarget);
            break;
        case SPELL_SPELL_SEVER_FLESH:
            me->AddAura(SPELL_SPELL_SEVER_FLESH, ptarget);
            break;
        case SPELL_MORBID_FIXATION:
            me->AddAura(SPELL_MORBID_FIXATION, ptarget);
            break;
        case SPELL_ESCAPE:
            me->AddAura(SPELL_ESCAPE, ptarget);
            break;                   
        }
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {                        
        case SPELL_NOXIOUS_FOG:
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {
                me->CastSpell(target, SPELL_NOXIOUS_FOG), false;
                AddTimedDelayedOperation(2600, [this, target]() -> void
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        me->CastSpell(target, SPELL_NOXIOUS_FOG, true);
                    }
                });
            }
            events.Repeat(20s);
            break;
        case SPELL_EMBALMING_ICHOR:
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {
                me->CastSpell(target, SPELL_EMBALMING_ICHOR), false;
                AddTimedDelayedOperation(2600, [this, target]() -> void
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        me->CastSpell(target, SPELL_EMBALMING_ICHOR, true);
                    }
                });
            }
            events.Repeat(20s);
            break;
        case SPELL_STITCHNEEDLE:
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {
                me->CastSpell(target, SPELL_STITCHNEEDLE), false;
                AddTimedDelayedOperation(2600, [this, target]() -> void
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        me->CastSpell(target, SPELL_STITCHNEEDLE, true);
                    }
                });
            }
            events.Repeat(20s);
            break;
        case SPELL_SEVER_FLESH:
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {
                me->CastSpell(target, SPELL_SEVER_FLESH), false;
                AddTimedDelayedOperation(2600, [this, target]() -> void
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        me->CastSpell(target, SPELL_SEVER_FLESH, true);
                    }
                });
            }
            events.Repeat(20s);
            break;
        case SPELL_MORBID_FIXATION:
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {
                me->CastSpell(target, SPELL_MORBID_FIXATION), false;
                AddTimedDelayedOperation(2600, [this, target]() -> void
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        me->CastSpell(target, SPELL_MORBID_FIXATION, true);
                    }
                });
            }
            events.Repeat(20s);
            break;
        case SPELL_ESCAPE:
            if (Unit* target = SelectTarget(SelectTargetMethod::MinDistance, 0, 100.0f, true))
            {
                me->CastSpell(target, SPELL_ESCAPE), false;
                AddTimedDelayedOperation(2600, [this, target]() -> void
                {
                    for (uint8 i = 0; i < 3; i++)
                    {
                        me->CastSpell(target, SPELL_ESCAPE, true);
                    }
                });
            }
            events.Repeat(20s);
            break; 
        }
    }

    void JustReachedHome() override
    {
        _JustReachedHome();
        me->RemoveAllAreaTriggers();
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        Talk(1);
        me->RemoveAllAreaTriggers();
        //   if (auto* encounterDoor = me->FindNearestGameObject(GO_SURGEON_STITCHFLESH_EXIT, 100.0f))
            //   encounterDoor->SetGoState(GO_STATE_ACTIVE);
    };
};

//166882 
void AddSC_boss_surgeon_stitchflesh()
{
    RegisterCreatureAI(boss_surgeon_stitchflesh);
}
