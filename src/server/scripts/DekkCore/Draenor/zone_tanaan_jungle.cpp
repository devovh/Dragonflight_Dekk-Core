/*
* Copyright 2022 DekkCore
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
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "MapManager.h"
#include "MotionMaster.h"
#include "Player.h"
#include "Unit.h"
#include "Vehicle.h"

namespace TanaanIntroQuests
{
    enum
    {
        QUEST_THE_DARK_PORTAL = 34398,
        QUEST_ONSLAUGHTS_END = 34392,
        QUEST_THE_PORTALS_POWER = 34393,
        QUEST_AZEROTHS_LAST_STAND = 35933,
        QUEST_THE_COST_OF_WAR = 34420,
        QUEST_BLAZE_OF_GLORY = 34422,
        QUEST_BLED_DRY_A = 35240,
        QUEST_BLED_DRY_H = 34421,
        QUEST_VENGEANCE_FOR_THE_FALLEN_A = 35242,
        QUEST_VENGEANCE_FOR_THE_FALLEN_H = 35241,
        QUEST_ALTAR_ALTERCATION = 34423,
        QUEST_KARGATHAR_PROVING_GROUNDS = 34425,
        QUEST_A_POTENTIAL_ALLY_H = 34427,
        QUEST_A_POTENTIAL_ALLY_A = 34478,
        QUEST_KILL_YOUR_HUNDRED = 34429,
        QUEST_MASTERS_OF_SHADOW_A = 34431,
        QUEST_MASTERS_OF_SHADOW_H = 34737,
        QUEST_YREL_A = 34434,
        QUEST_YREL_H = 34740,
        QUEST_KELIDAN_THE_BREAKER_A = 34436,
        QUEST_KELIDAN_THE_BREAKER_H = 34741,
        QUEST_PREPARE_FOR_BATTLE_A = 35019,
        QUEST_PREPARE_FOR_BATTLE_H = 35005,
        QUEST_THE_BATTLE_OF_THE_FORGE = 34439,
        QUEST_GANAR_THE_FROSTWOLF = 34442,
        QUEST_POLISHING_THE_IRON_THRONE = 34925,
        QUEST_THE_PRODIGAL_FROSTWOLF = 34437,
        QUEST_THE_GUNPOWDER_PLOT = 34987,
        QUEST_THE_SHADOW_OF_THE_WORLDBREAKER = 34958,
        QUEST_TAKING_A_TRIP_TO_THE_TOP_OF_THE_TANK = 35747,
        QUEST_A_TASTE_OF_IRON = 34445,
        QUEST_THE_HOME_STRETCH_H = 34446,
        QUEST_THE_HOME_STRETCH_A = 35884,

        QUEST_FLAG_THAELIN_EVENT_COMPLETE = 35253,
        QUEST_FLAG_HANSEL_EVENT_COMPLETE = 35983,
        QUEST_FLAG_ARMY_PUSHED = 35297,
    };
}

namespace TanaanIntroScenes
{
    enum
    {
        SCENE_CHOGALL_FREED = 961,
        SCENE_TERONGOR_FREED = 962,
        SCENE_DETONATION_DWARVES = 937,
        SCENE_GULDAN_REVEAL = 925,
        SCENE_GULDAN_FREED = 808,
        SCENE_RUNNING_AWAY = 806,
        SCENE_TRAIL_OF_FLAME = 934,
        SCENE_RELEASE_SOUTHERN_CAGES = 812,
        SCENE_RELEASE_EASTERN_CAGES = 813,
        SCENE_HOLDOUT = 933,
        SCENE_KILROGG_REVEAL = 940,
        SCENE_EYE_OF_KILROGG = 938,
        SCENE_SNEAKY_ARMY = 942,
        SCENE_BLOOD_BOWLS = 1029,
        SCENE_DESTROY_BRIDGE = 903,
        SCENE_RING_OF_FIRE = 838,
        SCENE_ENTER_KARGATH_ARENA = 945,
        SCENE_LEAVE_KARGATH_ARENA = 946,
        SCENE_CAVE_IN = 956,
        SCENE_MALADAAR_AND_QIANA = 947,
        SCENE_LIADRIN_AND_OLIN = 948,
        SCENE_MALADAAR_AND_YREL_LEAVE = 861,
        SCENE_LIADRIN_AND_YREL_LEAVE = 870,
        SCENE_KELIDAN_PARTICLES_DAMAGE = 858,
        SCENE_NERZHUL_REVEAL = 952,
        SCENE_FROM_CAVE_TO_RIDGE = 922,
        SCENE_ARMING_PRISONERS_H = 906,
        SCENE_ARMING_PRISONERS_A = 981,
        SCENE_BLACKHAND_REVEAL = 896,
        SCENE_KHADGAR_RUN_TO_DAM = 908,
        SCENE_DAM_EXPLOSION = 910,
        SCENE_BUILDING_EXPLOSION_KEG_LEAKING = 893,
        SCENE_GANAR_FREED = 911,
        SCENE_SHOOTING_IRON_HORDE = 871,
        SCENE_IRON_BASTION_RUN_FINALE = 912,
    };
}

namespace TanaanIntroKillCredits
{
    enum
    {
        CREDIT_TERONGOR_FREED = 82607,
        CREDIT_CHOGALL_FREED = 82606,
        CREDIT_ENTER_GULDAN_PRISON = 82573,
        CREDIT_STASIS_RUNE_DESTROYED = 78333,
        CREDIT_MARK_OF_BLACKROCK_DESTROYED = 78887,
        CREDIT_MARK_OF_SHATTERED_HAND_DESTROYED = 78886,
        CREDIT_MARK_OF_THE_BURNING_HAND_DESTROYED = 78885,
        CREDIT_HUTS_BURNED = 81760,
        CREDIT_EASTERN_CAGE_OPENED_H = 78529,
        CREDIT_SOUTHERN_CAGE_OPENED_H = 85141,
        CREDIT_EASTERN_CAGE_OPENED_A = 85142,
        CREDIT_SOUTHERN_CAGE_OPENED_A = 82871,
        CREDIT_SPEAK_WITH_ARIOK = 78556,
        CREDIT_FIND_KHADGAR_BRIDGE = 84456,
        CREDIT_FOLLOW_KHADGAR = 80244,
        CREDIT_ENTER_KARGATH_ARENA = 82139,
        CREDIT_ESCAPE_KARGATH_ARENA = 82140,
        CREDIT_99_KILLS = 82142,
        CREDIT_100_KILLS = 82066,
        CREDIT_FIND_ANKOVA = 79593,
        CREDIT_YREL = 79794,
        CREDIT_KELIDAN = 79702,
    };
}

namespace TanaanIntroQuestObjectives
{
    enum
    {
        OBJ_ENTER_GULDAN_PRISON = 273930,
        OBJ_ESCORT_ARIOK_TO_ALTAR = 273677,
        OBJ_BLOOD_ORB_DESTROYED = 273678,
        OBJ_FIND_KHADGAR_BRIDGE = 274409,
        OBJ_KILL_99_ENEMIES = 273763,
        OBJ_KILL_100_ENEMIES = 273713,
        OBJ_ESCAPE_KARGATH_ARENA = 273759,
        OBJ_FIND_ANKOVA_A = 272929,
        OBJ_FIND_ANKOVA_H = 272984,
        OBJ_KILL_200_ORCS = 273041,
        OBJ_PERSS_THE_MAIN_CANNON = 273397,
    };
}

/// Fel Sludge - 188520
class spell_tanaan_fel_sludge : public AuraScript
{
    PrepareAuraScript(spell_tanaan_fel_sludge);

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* target = GetTarget())
            if (aurEff->GetBase()->GetStackAmount() >= 10)
                target->KillSelf();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_tanaan_fel_sludge::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

enum eMaps
{
    TANAAN_JUNGLE_100_PHASE_MAP = 1464
};

// Zone 6723
class zone_tanaan_jungle_100 : public ZoneScript
{
public:
    zone_tanaan_jungle_100() : ZoneScript("zone_tanaan_jungle_100") { }

    void OnPlayerEnterZone(Player* player) override
    {
        if (!player->IsInFlight() && player->GetMapId() == MAP_DRAENOR)
            player->SeamlessTeleportToMap(TANAAN_JUNGLE_100_PHASE_MAP);
    }

    void OnPlayerExitZone(Player* player) override
    {
        if (!player->IsBeingTeleportedFar())
        {
            if (player->GetMapId() == TANAAN_JUNGLE_100_PHASE_MAP)
                player->SeamlessTeleportToMap(MAP_DRAENOR);
        }
    }
};

class npc_terrorfist : public CreatureScript
{
public:
    npc_terrorfist() : CreatureScript("npc_terrorfist") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_terrorfistAI(creature);
    }

    struct npc_terrorfistAI : public ScriptedAI
    {
        npc_terrorfistAI(Creature* creature) : ScriptedAI(creature) { }

        enum TerrorfistSpells
        {
            SpellSmash             = 172679,
            SpellEnrage            = 32714
        };

        void JustEngagedWith(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(13), [this](TaskContext context)
            {
                me->CastSpell(me, SpellSmash, false);
                context.Repeat(Seconds(13));
            });

            me->GetScheduler().Schedule(Seconds(30), [this](TaskContext context)
            {
                me->AddAura(SpellEnrage, me);
                context.Repeat(Seconds(30));
            });
        }
    };
};

class npc_vengeance : public CreatureScript
{
public:
    npc_vengeance() : CreatureScript("npc_vengeance") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_vengeanceAI(creature);
    }

    struct npc_vengeanceAI : public ScriptedAI
    {
        npc_vengeanceAI(Creature* creature) : ScriptedAI(creature) { }

        enum VengeanceSpells
        {
            SpellEnvelopingShadows = 170178,
            SpellShadowInfusion    = 170180
        };

        void JustEngagedWith(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(17), [this](TaskContext context)
            {
                me->CastSpell(me, SpellEnvelopingShadows, false);
                context.Repeat(Seconds(17));
            });

            me->GetScheduler().Schedule(Seconds(14), [this](TaskContext context)
            {
                me->CastSpell(me, SpellShadowInfusion, false);
                context.Repeat(Seconds(14));
            });
        }
    };
};

class npc_deathtalon : public CreatureScript
{
public:
    npc_deathtalon() : CreatureScript("npc_deathtalon") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_deathtalonAI(creature);
    }

    struct npc_deathtalonAI : public ScriptedAI
    {
        npc_deathtalonAI(Creature* creature) : ScriptedAI(creature) { }

        enum DeathtalonSpells
        {
            SpellHeadbutt          = 158546,
            SpellQuills            = 158564,
            SpellPierce            = 158560,
            SpellScreech           = 167367
        };

        void JustEngagedWith(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(40), [this](TaskContext context)
            {
                me->CastSpell(me, SpellHeadbutt, false);
                context.Repeat(Seconds(40));
            });

            me->GetScheduler().Schedule(Seconds(20), [this](TaskContext context)
            {
                me->CastSpell(me, SpellQuills, false);
                context.Repeat(Seconds(60));
            });

            me->GetScheduler().Schedule(Seconds(15), [this](TaskContext context)
            {
                me->CastSpell(me, SpellPierce, false);
                context.Repeat(Seconds(30));
            });

            me->GetScheduler().Schedule(Seconds(5), [this](TaskContext context)
            {
                me->CastSpell(me, SpellScreech, false);
                context.Repeat(Seconds(90), Seconds(100));
            });
        }
    };
};

struct at_deathtalon_screech : AreaTriggerAI
{
    at_deathtalon_screech(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    enum eSpells
    {
        SPELL_SCREECH_DAMAGE = 158558
    };

    void OnUnitEnter(Unit* unit) override
    {
        if (Unit* caster = at->GetCaster())
            if (caster != unit)
                caster->CastSpell(unit, SPELL_SCREECH_DAMAGE, true);
    }
};

/// Quills - 158564
class spell_deathtalon_quills : public AuraScript
{
    PrepareAuraScript(spell_deathtalon_quills);

    enum
    {
        SPELL_QUILLS_DAMAGE = 158567
    };

    void OnTick(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_QUILLS_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathtalon_quills::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class npc_doomroller : public CreatureScript
{
public:
    npc_doomroller() : CreatureScript("npc_doomroller") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_doomrollerAI(creature);
    }

    enum eNpcs
    {
        NPC_DRIVER      = 94807,
        NPC_PASSENGER   = 95062
    };

    struct npc_doomrollerAI : public ScriptedAI
    {
        npc_doomrollerAI(Creature* creature) : ScriptedAI(creature) { }

        enum DoomrollerSpells
        {
            SpellSiegeNova = 188579,
        };

        void Reset() override
        {
            me->SetFaction(14);

            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
            me->GetMotionMaster()->MovePoint(1, 4092.80f, -760.21f, 2.89f, false);

         /*   if (Vehicle* meVehicle = me->GetVehicleKit())
            {
                meVehicle->AddPassenger(NPC_PASSENGER, 0);
                meVehicle->AddPassenger(NPC_PASSENGER, 2);
                meVehicle->AddPassenger(NPC_PASSENGER, 3);
                meVehicle->AddPassenger(NPC_PASSENGER, 5);
                meVehicle->AddPassenger(NPC_DRIVER, 7);
            }*/
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type == POINT_MOTION_TYPE && point == 1)
            {
                me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                me->SetHomePosition(me->GetPosition());
            }
        }

        void JustEngagedWith(Unit* /*victim*/) override
        {
            me->GetScheduler().Schedule(Seconds(40), [this](TaskContext context)
            {
                me->CastSpell(me, SpellSiegeNova, false);
                context.Repeat(Seconds(40));
            });
        }
    };
};

// @Creature: Gul'dan 78333 - Dark Portal area
class npc_gul_dan_78333 : public CreatureScript
{
public:
    npc_gul_dan_78333() : CreatureScript("npc_gul_dan_78333") {}

    struct npc_gul_dan_78333AI : public ScriptedAI
    {
        npc_gul_dan_78333AI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* p_Unit) override
        {
            Player* l_Player = p_Unit->ToPlayer();
            if (!l_Player)
                return;

            if (l_Player->GetDistance2d(me) >= 40.0f)
                return;

            if (l_Player->GetPositionZ() - 5.0f > me->GetPositionZ())
                return;

            if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_PORTALS_POWER) == QUEST_STATUS_INCOMPLETE &&
                l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_ENTER_GULDAN_PRISON) < 1)
            {
                l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_ENTER_GULDAN_PRISON);
               // l_Player->PlayScene(TanaanIntroScenes::SCENE_GULDAN_REVEAL, l_Player);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_gul_dan_78333AI(creature);
    }
};

void AddSC_tanaan_jungle()
{
    RegisterAuraScript(spell_tanaan_fel_sludge);

    new zone_tanaan_jungle_100();

    new npc_terrorfist();
    new npc_vengeance();
    new npc_doomroller();

    new npc_deathtalon();
    RegisterAreaTriggerAI(at_deathtalon_screech);
    RegisterAuraScript(spell_deathtalon_quills);

    new npc_gul_dan_78333();
}
