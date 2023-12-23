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

#include "SpellScript.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "TemporarySummon.h"
#include "Creature.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "CreatureAI.h"
#include "GameObjectAI.h"
#include "ScriptedGossip.h"
#include "Pet.h"
#include "PetAI.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "CombatAI.h"
#include "GameObject.h"

enum ZuldazarQuests
{
    QUEST_WELCOME_ZULDAZAR                  = 46957,
    QUEST_RASTAKHAN                         = 46930,
    QUEST_SPEAKER_OF_THE_HORDE              = 46931,
    OBJECTIVE_SUMMON_THE_HORDE              = 291969,
    QUEST_NEED_EACH_OTHER                   = 52131,
    QUEST_SEND_THE_FLEET_HORDE              = 56044,
    QUEST_RIDE_OF_THE_ZANDALARI             = 54140,

    QUEST_SEND_THE_SIGNAL                   = 49758,
    QUEST_ZULDAZAR                          = 47514,
    QUEST_DESTROY_THE_WEAPON                = 49785,
    QUEST_HOPES_BLUE_LIGHT                  = 49884,
    QUEST_A_DIFFERENT_OUTCOME               = 50331,
    QUEST_SAVING_FOR_LATER                  = 48025,
    QUEST_STOP_THE_EVAQUATION               = 51286,
    QUEST_OFFENSIVELY_DEFENSIVE             = 47433,
    QUEST_PAKU_MASTER_OF_WINDS              = 47440,
    QUEST_HOW_TO_TRAIN_YOUR_DIREHORN        = 47261,
    QUEST_A_GOOD_SPANKING                   = 48581,
    QUEST_NO_TROLLS_LEFT_BEHIND             = 49145,
    QUEST_MIDNIGHT_IN_THE_GARDEN_OF_THE_LOA = 47521,
    QUEST_THE_HUNTER                        = 47522,
    QUEST_THE_ANCIENT_ONE                   = 47963,
    QUEST_MISTRESS_OF_LIES                  = 47528,
};

enum ZuldazarSpells
{
    SPELL_TALANJI_EXPOSITION_CONVERSATION_1 = 261541,
    SPELL_TALANJI_EXPOSITION_CONVERSATION_2 = 261549,
    SPELL_TALANJI_EXPOSITION_KILL_CREDIT    = 265711,

    SPELL_RASTAKHAN_GREETINGS_SCENE         = 244534,

    SPELL_PREVIEW_TO_ZANDALAR               = 273387,
};

enum ZuldazarNpcs
{
    NPC_ZOLANI                      = 135441,
    NPC_FOLLOW_ZOLANI_KILL_CREDIT   = 120169,
    NPC_FLEET_SCOUT                 = 140566,
    NPC_BOARDING_SHIP               = 130830,
    NPC_DEYON                       = 128889,
    NPC_KILL_CREDIT_BUSH_01         = 122447,
    NPC_KILL_CREDIT_SPANKING        = 126736,
    NPC_YAZMA                       = 122864,
    NPC_HOLD_SEARCHED               = 138721,
};

enum ZuldazarObjects
{
    GO_CHALICE_OF_CALLING           = 273454,
};

enum ZuldazarOtherEnums
{
    AREA_THE_GREAT_SEAL             = 9598,
    EVENT_START_FLY                 = 1,

    GOSSIP_MENU_0                   = 0,
    GOSSIP_MENU_1                   = 1,
};

Position const FleetScoutPath[4] =
{
    { -2211.135f, 887.061f, 15.813f },
    { -2856.335f, 1000.121f, 37.971f },
    { -3403.116f, 1072.148f, 92.939f },
    { -3516.640f, 1106.287f, 12.677f }
};
using FleetScoutPathSize = std::extent<decltype(FleetScoutPath)>;

Position const HarliPath[3] =
{
    { -3655.419f, 1057.569f, 119.452f },
    { -3105.406f, 694.161f, 47.528f },
    { -3025.553f, 680.530f, 21.311f }
};
using HarliPathSize = std::extent<decltype(HarliPath)>;

Position const CaptainPath[4] =
{
    { -2945.962f, 761.264f, 61.791f },
    { -2862.439f, 575.864f, 200.004f },
    { -2129.446f, 1061.875f, 63.822f },
    { -2051.988f, 967.388f, 10.379f }
};
using CaptainPathSize = std::extent<decltype(CaptainPath)>;

Position const AtaPath[17] =
{
    { -592.034f, 575.073f, 236.727f },
    { -676.101f, 564.972f, 288.394f },
    { -686.911f, 751.269f, 249.687f },
    { -638.587f, 734.484f, 264.252f },
    { -591.855f, 663.559f, 252.156f },
    { -498.734f, 686.881f, 228.387f },
    { -289.806f, 548.037f, 210.564f },
    { -227.072f, 455.147f, 213.479f },
    { -8.986f, 534.179f, 166.142f },
    { 103.299f, 634.506f, 142.839f },
    { 16.359f, 713.684f, 168.302f },
    { -81.618f, 809.418f, 225.33f },
    { -187.624f, 1046.54f, 298.861f },
    { -244.168f, 1149.883f, 323.064f },
    { -292.122f, 976.499f, 433.307f },
    { -543.195f, 810.397f, 395.891f },
    { -622.331f, 903.649f, 353.563f }
};
using AtaPathSize = std::extent<decltype(AtaPath)>;

Position const Ata2Path[3] =
{
    { -559.637f, 943.562f, 457.531f },
    { -766.595f, 441.126f, 291.785f },
    { -1386.42f, -336.053f, 436.605f }
};
using Ata2PathSize = std::extent<decltype(Ata2Path)>;

Position const Ata3Path[2] =
{
    { -361.189f, 1250.722f, 442.476f },
    { -292.22f, 1427.115f, 382.695f }
};
using Ata3PathSize = std::extent<decltype(Ata3Path)>;

// 132332
struct npc_talanji_arrival : public ScriptedAI
{
    npc_talanji_arrival(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_WELCOME_ZULDAZAR)
        {
            me->DestroyForPlayer(player);
            player->SummonCreature(132661, me->GetPosition());
        }
    }
};

// 132661
struct npc_talanji_arrival_escort : public EscortAI
{
    npc_talanji_arrival_escort(Creature* creature) : EscortAI(creature) { }

    void Reset() override
    {
        if (Player* player = me->ToPlayer())
        {
            if (!player->HasQuest(QUEST_WELCOME_ZULDAZAR))
            {
                if (player->HasAura(261486))
                    player->RemoveAura(261486);

                me->DespawnOrUnsummon();
            }
        }
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_WELCOME_ZULDAZAR) == QUEST_STATUS_INCOMPLETE)
            {
                me->Mount(80358);
                Start(false, summoner->GetGUID());
                SetDespawnAtEnd(false);
                summoner->CastSpell(summoner, SPELL_TALANJI_EXPOSITION_CONVERSATION_1, true);
                summoner->ToUnit()->GetScheduler().Schedule(50s, [summoner] (TaskContext /*context*/)
                {
                    if (Player* player = summoner->ToPlayer())
                    {
                        player->KilledMonsterCredit(132661);
                    }
                });
                me->DespawnOrUnsummon(55s);
            }
        }
    }
};


// 138912
struct npc_enforcer_pterrordax : public EscortAI
{
    npc_enforcer_pterrordax(Creature* creature) : EscortAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        Player* player = summoner->ToPlayer();
        if (!player || player->GetQuestStatus(QUEST_RASTAKHAN) != QUEST_STATUS_INCOMPLETE)
        {
            me->ForcedDespawn();
            return;
        }
        me->SetSpeed(MOVE_RUN, 21.f);
        player->EnterVehicle(me);
        player->KilledMonsterCredit(135438);
        Start(false, player->GetGUID());
    }
};

struct npc_talanji_135440 : public ScriptedAI
{
    npc_talanji_135440(Creature* c) : ScriptedAI(c) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        if (player->GetQuestStatus(QUEST_RASTAKHAN) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonsterCredit(135440);
            player->GetSceneMgr().PlaySceneByPackageId(1895, SceneFlag::None);
        }

        return true;
    }
};

// 120740
struct npc_rastakhan_zuldazar_arrival : public ScriptedAI
{
    npc_rastakhan_zuldazar_arrival(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_SPEAKER_OF_THE_HORDE)
            player->SummonCreature(NPC_ZOLANI, -1100.689941f, 817.934021f, 497.243011f, 6.062160f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 2min, player->GetGUID());
    }
};

struct npc_soth_zolani : public EscortAI
{
    npc_soth_zolani(Creature* creature) : EscortAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasQuest(QUEST_SPEAKER_OF_THE_HORDE))
        {
            CloseGossipMenuFor(player);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->SetWalk(true);
            Talk(0);
            me->GetMotionMaster()->MovePoint(1, -1129.335f, 845.836f, 487.865f, true);
            player->GetScheduler().Schedule(27s, [this, player] (TaskContext /*context*/)
            {
                player->KilledMonsterCredit(120169);
                Talk(1);
                me->ForcedDespawn(10000);
            });
        }

        return true;
    }
};

//121000
struct npc_horde_banner : public ScriptedAI
{
    npc_horde_banner(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
    }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (unit->IsPlayer() && unit->GetDistance2d(me) <= 3.0f)
        {
            Player* player = unit->ToPlayer();

            if (player->GetQuestStatus(QUEST_SPEAKER_OF_THE_HORDE) == QUEST_STATUS_INCOMPLETE)
       //     {
         //       if (!player->GetQuestObjectiveData(QUEST_SPEAKER_OF_THE_HORDE, 0))
                {
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    player->AddAura(241211, player);
                    player->KilledMonsterCredit(121000);
                    me->ForcedDespawn(0, 3s);
                }
            }
      //  }
    }
};

// 122690
struct npc_brillin_the_beauty : public ScriptedAI
{
    npc_brillin_the_beauty(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 10.f)
                if (player->GetQuestStatus(QUEST_SPEAKER_OF_THE_HORDE) == QUEST_STATUS_INCOMPLETE)
                //    if (player->GetQuestObjectiveCounter(OBJECTIVE_SUMMON_THE_HORDE) != 0)
                        KillCreditMe(player);
    }
};

// 130984
struct npc_natal_hakata : public ScriptedAI
{
    npc_natal_hakata(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 10.f)
                if (player->GetQuestStatus(QUEST_SPEAKER_OF_THE_HORDE) == QUEST_STATUS_INCOMPLETE)
               //     if (player->GetQuestObjectiveCounter(OBJECTIVE_SUMMON_THE_HORDE) != 0)
                        KillCreditMe(player);
    }
};

// 131443
struct npc_telemancer_oculeth_zuldazar : public ScriptedAI
{
    npc_telemancer_oculeth_zuldazar(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* unit) override
    {
        if (Player* player = unit->ToPlayer())
            if (player->GetDistance(me) < 10.f)
                if (player->GetQuestStatus(QUEST_SPEAKER_OF_THE_HORDE) == QUEST_STATUS_INCOMPLETE)
                //    if (player->GetQuestObjectiveCounter(OBJECTIVE_SUMMON_THE_HORDE) != 0)
                        player->KilledMonsterCredit(135435);
    }
};

// 133050
struct npc_talanji_great_seal : public ScriptedAI
{
    npc_talanji_great_seal(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_NEED_EACH_OTHER)
            player->CastSpell(player, SPELL_PREVIEW_TO_ZANDALAR, true);
    }
};

//126822, Ata the Winglord
struct npc_ata_the_winglord_offensively_defence : public VehicleAI
{
    npc_ata_the_winglord_offensively_defence(Creature* creature) : VehicleAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_OFFENSIVELY_DEFENSIVE) == QUEST_STATUS_INCOMPLETE)
            player->SummonCreature(me->GetEntry(), player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

        return true;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_OFFENSIVELY_DEFENSIVE) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                me->GetMotionMaster()->MoveSmoothPath(AtaPathSize::value, AtaPath, AtaPathSize::value, false, true);
            }
        }
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool /*apply*/) override
    {
        if (Player* player = who->ToPlayer())
            player->KilledMonsterCredit(126822);
    }

    void MovementInform(uint32 /* type */, uint32 id) override
    {
        if (id == AtaPathSize::value)
            me->DespawnOrUnsummon(100ms);
    }
};

//126822, Ata the Winglord
struct npc_ata_the_winglord_paku_master_of_winds : public VehicleAI
{
    npc_ata_the_winglord_paku_master_of_winds(Creature* creature) : VehicleAI(creature) { }

    std::string whichAta;

    void Reset() override
    {
        whichAta = "";
    }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_PAKU_MASTER_OF_WINDS) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(QUEST_MIDNIGHT_IN_THE_GARDEN_OF_THE_LOA) == QUEST_STATUS_INCOMPLETE)
            player->SummonCreature(me->GetEntry(), player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);

        return true;
    }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_PAKU_MASTER_OF_WINDS) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                me->GetMotionMaster()->MoveSmoothPath(Ata2PathSize::value, Ata2Path, Ata2PathSize::value, false, true);
                whichAta = "Ata2";
            }

            if (player->GetQuestStatus(QUEST_MIDNIGHT_IN_THE_GARDEN_OF_THE_LOA) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                me->GetMotionMaster()->MoveSmoothPath(Ata3PathSize::value, Ata3Path, Ata3PathSize::value, false, true);
                whichAta = "Ata3";
            }
        }
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool /*apply*/) override
    {
        if (Player* player = who->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_PAKU_MASTER_OF_WINDS) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(me->GetEntry());

            if (player->GetQuestStatus(QUEST_MIDNIGHT_IN_THE_GARDEN_OF_THE_LOA) == QUEST_STATUS_INCOMPLETE)
                player->ForceCompleteQuest(QUEST_MIDNIGHT_IN_THE_GARDEN_OF_THE_LOA);
        }
    }

    void MovementInform(uint32 /* type */, uint32 id) override
    {
        if (whichAta == "Ata2")
            if (id == Ata2PathSize::value)
                me->DespawnOrUnsummon(100ms);

        if (whichAta == "Ata3")
            if (id == Ata3PathSize::value)
                me->DespawnOrUnsummon(100ms);
    }
};

class npc_pterrordax_paku_master_of_winds : public EscortAI
{
public:
    npc_pterrordax_paku_master_of_winds(Creature* creature) : EscortAI(creature)
    {
        me->SetCanFly(true);
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void OnCharmed(bool isNew) override
    {
        if (me->IsCharmed() && !me->IsEngaged())
            JustEngagedWith(nullptr);
        ScriptedAI::OnCharmed(isNew);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (summoner)
        {
            // me->GetScheduler().Schedule(1s, [this, summoner](TaskContext /*context*/)
            summoner->CastSpell(me, 46598);
            me->SetSpeed(MOVE_RUN, 26);
        }
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool /*apply*/) override
    {
        if (Player* player = who->ToPlayer())
            player->KilledMonsterCredit(127512);
        Start(false, who->GetGUID());
    }


};

enum Spells
{
    SPELL_CALL_PTERRORDAX = 281048
};

//127377
struct npc_paku : public ScriptedAI
{
    npc_paku(Creature* pCreature) : ScriptedAI(pCreature)
    {
    }

    bool OnGossipHello(Player* player) override
    {
        player->KilledMonsterCredit(127377);

        return true;
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        std::list<Player*> players;
        me->GetPlayerListInGrid(players, 75.0f);

        for (Player* player : players)
        {
            if (player->GetPositionZ() <= 400 && !player->IsMounted() && !player->IsOnVehicle(player) && player->HasQuest(47440))
            {
                Talk(0);
                me->GetScheduler().Schedule(Milliseconds(7000), [](TaskContext context)
                {
                    GetContextCreature()->AI()->Talk(1);
                });
                me->GetScheduler().Schedule(Milliseconds(24000), [](TaskContext context)
                {
                    GetContextCreature()->AI()->Talk(2);
                });
                player->CastSpell(player, SPELL_CALL_PTERRORDAX);
            }
        }
    }
};

//263018
class spell_rastari_skull_whistle : public SpellScript
{
	PrepareSpellScript(spell_rastari_skull_whistle);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_NO_TROLLS_LEFT_BEHIND) == QUEST_STATUS_INCOMPLETE)
            {
                std::list<Creature*> spellTargetsCreatures;
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 129204, 25.0f);
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 129205, 25.0f);
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 129203, 25.0f);
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 128963, 25.0f);
                for (auto& target : spellTargetsCreatures)
                {
                    player->KilledMonsterCredit(128963);
                    target->AI()->Talk(0);
                    target->HandleEmoteCommand(static_cast<Emote>(113));
                    target->ForcedDespawn(2000);
                }
            }
    }

	void Register() override
	{
		OnEffectHitTarget += SpellEffectFn(spell_rastari_skull_whistle::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
	}
};

// 254904 - Revitalize Spirit Obelisk
class spell_quest_revitalize_spirit_obelisk : public SpellScript
{
    PrepareSpellScript(spell_quest_revitalize_spirit_obelisk);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;
        Player* player = caster->ToPlayer();
        if (!player)
            return;

        if (Creature* creatureTarget = GetHitCreature())
        {
            if (creatureTarget->GetEntry() == 128468 && creatureTarget->IsAlive())
            {
                player->KilledMonsterCredit(128468);
                player->CastSpell(nullptr, 254931, true);
                creatureTarget->KillSelf();
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_quest_revitalize_spirit_obelisk::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// todo fix the positions and amount of spells 224839
class spell_call_the_storm : public SpellScript
{
    PrepareSpellScript(spell_call_the_storm);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(caster, 224842);
        caster->CastSpell(caster->GetPositionWithDistInOrientation(5, 1.6f), 224846);
        caster->CastSpell(caster->GetPositionWithDistInOrientation(5, 0.5f), 224842);
        caster->CastSpell(caster->GetPositionWithDistInOrientation(5, 0), 224842);
        caster->CastSpell(caster->GetPositionWithDistInOrientation(5, 3), 224842);

    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_call_the_storm::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 260075
class spell_lightning_rod : public SpellScriptLoader
{
public:
    spell_lightning_rod() : SpellScriptLoader("spell_lightning_rod") { }

    class spell_lightning_rod_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_lightning_rod_AuraScript);

        void HandlePeriodicDummy(AuraEffect const* /*auraEff*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            Unit* target = GetTarget();
            if (!target)
                return;

            caster->CastSpell(target, 260073);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_lightning_rod_AuraScript::HandlePeriodicDummy, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_lightning_rod_AuraScript();
    }

};

enum PlotTwist {
    SPELL_CHANNEL = 259228,
    EVENT_SOUL_BLAST = 1,
    SPEll_SOUL_BLAST = 260572,
    EVENT_SUMMON_MASK = 2,
    SPELL_SUMMON_MASKS = 260577,
    EVENT_SOUL_JAUNT = 3,
    SPELL_SOUL_JAUNT = 264007,
    EVENT_GONG = 4,
    //SPELL_GONK = ,
    EVENT_POTION = 5,
    SPELL_POTION = 272315,
    EVENT_URN = 6,
};

struct npc_voljamba : public ScriptedAI
{
    npc_voljamba(Creature* pCreature) : ScriptedAI(pCreature)
    {
    }

    void Reset() override
    {
        me->CastSpell(nullptr, SPELL_CHANNEL);
        gong = false;
        potion = false;
        final = false;
    }


    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SOUL_BLAST, 2s);
        events.ScheduleEvent(EVENT_SUMMON_MASK, 9s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_SOUL_BLAST:
                DoCastVictim(SPEll_SOUL_BLAST);
                events.ScheduleEvent(EVENT_SOUL_BLAST, 4s, 9s);
                break;
            case EVENT_SUMMON_MASK:
                DoCast(SPELL_SUMMON_MASKS);
                events.ScheduleEvent(EVENT_SUMMON_MASK,15s, 20s);
                break;
            case EVENT_GONG:
                Talk(1);
                break;
            case EVENT_POTION:
                Talk(3);
                break;
            default:
                break;
            }
        }
        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (me->GetHealthPct() <= 80 && !gong)
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                me->InterruptNonMeleeSpells(false);
            DoCast(SPELL_SOUL_JAUNT);
            gong = true;
        }

        if (me->GetHealthPct() <= 60 && !potion)
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                me->InterruptNonMeleeSpells(false);
            DoCast(SPELL_SOUL_JAUNT);
            potion = true;
        }

        if (damage >= me->GetHealth())
        {
            damage = 0;
            if (!final)
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    me->InterruptNonMeleeSpells(false);
                Talk(4);
                DoCast(SPELL_SOUL_JAUNT);
                final = true;
            }
        }
    }

    void DoAction(int32 param) override
    {
        if (param == 1)
        {
            if (gong && !potion && !final)
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    me->InterruptNonMeleeSpells(false);
                me->NearTeleportTo(-372.1f, 133.543f, 257.2f, 3.481f);
                Talk(0);
            }
            if (gong && potion && !final)
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    me->InterruptNonMeleeSpells(false);
                me->NearTeleportTo(-384.489f, 161.891f, 257.51f, 3.4f);
                Talk(2);
                DoCast(SPELL_POTION);
                events.ScheduleEvent(EVENT_POTION, 15s);
            }
            if (gong && potion && final)
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    me->InterruptNonMeleeSpells(false);
                me->NearTeleportTo(-351.645f, 136.92f, 257.11f, 5.31f);
                me->KillSelf();

                std::list<Player*> players;
                me->GetPlayerListInGrid(players, 50.0f);

                for (Player* player : players)
                {
                    player->KilledMonsterCredit(131241);
                }
            }
        }
    }


private:
    bool gong;
    bool potion;
    bool final;
    EventMap events;
};

//264007
class spell_soul_jaunt : public SpellScript
{
    PrepareSpellScript(spell_soul_jaunt);

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Creature* creature = caster->ToCreature())
            creature->AI()->DoAction(1);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_soul_jaunt::HandleAfterCast);
    }
};

class scene_warpack : public SceneScript
{
public:
    scene_warpack() : SceneScript("scene_warpack") { }

    void OnSceneTriggerEvent(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/, std::string const& triggerName)
    {
        if (triggerName == "Charge")
        {
            if (Creature* loti = player->FindNearestCreature(134132, 5))
            {
                loti->AI()->SetData(0, 1);
            }
        }
    }
};

//123000
struct npc_captain_rezokon_123000 : public ScriptedAI
{
    npc_captain_rezokon_123000(Creature* c) : ScriptedAI(c) { }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 0 && player->HasQuest(QUEST_SEND_THE_FLEET_HORDE))  //Let's go.
        {
            player->SendMovieStart(919);
            player->TeleportTo(1718, 107.0f, 29.0f, -30.0f, 0.33f);
        }
     //   if (action == GOSSIP_ACTION_INFO_DEF + 1 && !player->GetQuestObjectiveData(QUEST_RIDE_OF_THE_ZANDALARI, 0))
        {
            player->KilledMonsterCredit(146830);
        }
        return true;
    }
};

//146812
struct npc_rakera_146812 : public ScriptedAI
{
    npc_rakera_146812(Creature* c) : ScriptedAI(c) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
     //   if (!player->GetQuestObjectiveData(QUEST_RIDE_OF_THE_ZANDALARI, 1))
            player->KilledMonsterCredit(146812);

        return true;
    }
};

//146851
struct npc_hexlord_146851 : public ScriptedAI
{
    npc_hexlord_146851(Creature* c) : ScriptedAI(c) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
//        if (!player->GetQuestObjectiveData(QUEST_RIDE_OF_THE_ZANDALARI, 2))
        {
            player->KilledMonsterCredit(146851);
            me->AI()->Talk(0);
        }

        return true;
    }
};

//135690
struct npc_dread_admiral_tattersail_135690 : public ScriptedAI
{
    npc_dread_admiral_tattersail_135690(Creature* c) : ScriptedAI(c) { }
    bool OnGossipHello(Player* player) override
    {
        AddGossipItemFor(player, GossipOptionNpc::None, "Set sail for Drustvar.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);
        AddGossipItemFor(player, GossipOptionNpc::None, "Set sail for Stormsong Valley.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        AddGossipItemFor(player, GossipOptionNpc::None, "Set sail for Tiragarde Sound.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        SendGossipMenuFor(player, 34711, me->GetGUID());

        player->PrepareQuestMenu(me->GetGUID());

        if (player->GetAreaId() == 9699)
        {
            AddGossipItemFor(player, GossipOptionNpc::None, "Take us back to Zuldazar.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            SendGossipMenuFor(player, 34711, me->GetGUID());
            player->PrepareQuestMenu(me->GetGUID());
        }

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 action) override
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 0)   //Set sail for Drustvar.
        {
            player->TeleportTo(1643, -406.0f, 4122.0f, 4.08f, 4.24f);
        }

        if (action == GOSSIP_ACTION_INFO_DEF + 1)   //Set sail for Stormsong Valley.
        {
            player->TeleportTo(1643, 4246.0f, 406.0f, 0.58f, 2.51f);
        }

        if (action == GOSSIP_ACTION_INFO_DEF + 2)   //Set sail for Tiragarde Sound.
        {
            player->TeleportTo(1643, -218.0f, -1554.0f, 2.83f, 0.76f);
        }

        if (action == GOSSIP_ACTION_INFO_DEF + 3)   //Take us back to Zuldazar.
        {
            player->TeleportTo(1642, -2141.0f, 775.0f, 9.0f, 1.59f);
        }

        return true;
    }
};

class zuldazar_phase_manager : public PlayerScript
{
public:
    zuldazar_phase_manager() : PlayerScript("zuldazar_phase_manager") { }

    uint32 PHASE_UPDATE_DELAY      = 5000; // phase update delay in milliseconds
    uint32 PHASE_UPDATE_DELAY_DIFF = PHASE_UPDATE_DELAY;

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (player->GetAreaId() == AREA_THE_GREAT_SEAL)
        {
            if (PHASE_UPDATE_DELAY_DIFF <= diff)
            {
                if (player->GetQuestStatus(QUEST_ZULDAZAR) == QUEST_STATUS_NONE)
                {
                    if (player->GetLevel() >= 30)
                    {
                        if (const Quest* quest_1 = sObjectMgr->GetQuestTemplate(QUEST_ZULDAZAR))
                        {
                            player->AddQuest(quest_1, nullptr);
                            player->ForceCompleteQuest(QUEST_ZULDAZAR);
                        }
                    }
                }

                PHASE_UPDATE_DELAY_DIFF = PHASE_UPDATE_DELAY;
            }
            else
                PHASE_UPDATE_DELAY_DIFF -= diff;
        }
    }
};

//273454, Chalice of Calling
struct go_chalice_of_calling : public GameObjectAI
{
    go_chalice_of_calling(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->KillCreditGO(GO_CHALICE_OF_CALLING);

        return true;
    }
};

//140566, Fleet Scout
struct npc_fleet_scout : public VehicleAI
{
    npc_fleet_scout(Creature* creature) : VehicleAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_SEND_THE_SIGNAL) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                player->ForceCompleteQuest(QUEST_SEND_THE_SIGNAL);
                me->GetMotionMaster()->MoveSmoothPath(FleetScoutPathSize::value, FleetScoutPath, FleetScoutPathSize::value, false, true);
            }
        }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_SEND_THE_SIGNAL) == QUEST_STATUS_INCOMPLETE)
                player->SummonCreature(me->GetEntry(), player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    void MovementInform(uint32 /* type */, uint32 id) override
    {
        if (id == FleetScoutPathSize::value)
            me->DespawnOrUnsummon(100ms);
    }
};

//130759, Harli the Swift
struct npc_harli_the_swift : public VehicleAI
{
    npc_harli_the_swift(Creature* creature) : VehicleAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_DESTROY_THE_WEAPON) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                player->KilledMonsterCredit(NPC_BOARDING_SHIP);
                me->GetMotionMaster()->MoveSmoothPath(HarliPathSize::value, HarliPath, HarliPathSize::value, false, true);
            }
        }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_DESTROY_THE_WEAPON) == QUEST_STATUS_INCOMPLETE)
                player->SummonCreature(me->GetEntry(), player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    void MovementInform(uint32 /* type */, uint32 id) override
    {
        if (id == HarliPathSize::value)
            me->DespawnOrUnsummon(100ms);
    }
};

//130833, Captain Grez'ko
struct npc_captain_grezko : public VehicleAI
{
    npc_captain_grezko(Creature* creature) : VehicleAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_HOPES_BLUE_LIGHT) == QUEST_STATUS_INCOMPLETE)
            {
                me->GetVehicleKit();
                player->EnterVehicle(me);
                player->ForceCompleteQuest(QUEST_HOPES_BLUE_LIGHT);
                me->GetMotionMaster()->MoveSmoothPath(CaptainPathSize::value, CaptainPath, CaptainPathSize::value, false, true);
            }
        }
    }

    void OnSpellClick(Unit* clicker, bool /*result*/) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(QUEST_HOPES_BLUE_LIGHT) == QUEST_STATUS_INCOMPLETE)
                player->SummonCreature(me->GetEntry(), player->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    void MovementInform(uint32 /* type */, uint32 id) override
    {
        if (id == CaptainPathSize::value)
            me->DespawnOrUnsummon(100ms);
    }
};

//277859, Jailer Cage
struct go_jailer_cage : public GameObjectAI
{
    go_jailer_cage(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->KilledMonsterCredit(NPC_DEYON);

        return false;
    }
};

//137645, Wavesinger Zara
struct npc_wavesinger_zara : public ScriptedAI
{
    npc_wavesinger_zara(Creature* c) : ScriptedAI(c) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        switch (gossipListId)
        {
            case GOSSIP_MENU_0:
                if (player->GetQuestStatus(QUEST_A_DIFFERENT_OUTCOME) == QUEST_STATUS_INCOMPLETE)
                {
                    player->ForceCompleteQuest(QUEST_A_DIFFERENT_OUTCOME);
                    player->NearTeleportTo(-1231.878f, -1201.594f, 2.892f, 0.849f, false);
                }
                break;
            default:
                break;
        }

        return true;
    }
};

//248495, Scroll of Illusion
class spell_scroll_of_illusion : public SpellScript
{
    PrepareSpellScript(spell_scroll_of_illusion);

    void HandleScript()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Player* player = caster->ToPlayer())
            if (player->GetQuestStatus(QUEST_SAVING_FOR_LATER) == QUEST_STATUS_INCOMPLETE)
            {
                std::list<Creature*> spellTargetsCreatures;
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 125171, 10.0f);
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 125169, 10.0f);
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 125170, 10.0f);
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 125173, 10.0f);
                player->GetCreatureListWithEntryInGrid(spellTargetsCreatures, 131406, 10.0f);
                for (auto& target : spellTargetsCreatures)
                {
                    player->KilledMonsterCredit(target->GetEntry());
                }
            }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_scroll_of_illusion::HandleScript);
    }
};

//137075, Lieutenant Dennis Grimtale
struct npc_lieutenant_dennis_grimtale : public ScriptedAI
{
    npc_lieutenant_dennis_grimtale(Creature* c) : ScriptedAI(c) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->HasQuest(QUEST_STOP_THE_EVAQUATION))
        {
//            if (player->GetQuestObjectiveData(QUEST_STOP_THE_EVAQUATION, 0) && player->GetQuestObjectiveData(QUEST_STOP_THE_EVAQUATION, 2))
                player->ForceCompleteQuest(QUEST_STOP_THE_EVAQUATION);
        }

        return false;
    }
};

//122009, Kraal Master B'khor
struct npc_kral_master_bkhor : public ScriptedAI
{
    npc_kral_master_bkhor(Creature* c) : ScriptedAI(c) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        switch (gossipListId)
        {
            case GOSSIP_MENU_0:
                if (player->GetQuestStatus(QUEST_HOW_TO_TRAIN_YOUR_DIREHORN) == QUEST_STATUS_INCOMPLETE)
                {
                    player->ForceCompleteQuest(QUEST_HOW_TO_TRAIN_YOUR_DIREHORN);
                    Talk(0);
                }
                break;
            default:
                break;
        }

        return true;
    }
};

//270918, Soothing Lilybud
struct go_soothing_lilybud : public GameObjectAI
{
    go_soothing_lilybud(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->KilledMonsterCredit(NPC_KILL_CREDIT_BUSH_01);
        me->SetLootState(GO_JUST_DEACTIVATED, player);

        return false;
    }
};

//126732, Pinky
struct npc_pinky : public ScriptedAI
{
    npc_pinky(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        me->SetFaction(14);
    }

    void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        if (Player* player = attacker->ToPlayer())
            if (player->GetQuestStatus(QUEST_A_GOOD_SPANKING) == QUEST_STATUS_INCOMPLETE)
                if (me->GetHealthPct() <= 80)
                {
                    player->KilledMonsterCredit(NPC_KILL_CREDIT_SPANKING);
                    me->SetFaction(35);
                    me->GetMotionMaster()->MoveFleeing(player, 3s);
                    me->GetScheduler().Schedule(Milliseconds(120000), [](TaskContext context)
                    {
                        GetContextCreature()->AI()->Reset();
                    });
                }
    }
};

//124827, Gonk
struct npc_gonk : public ScriptedAI
{
    npc_gonk(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        switch (gossipListId)
        {
            case GOSSIP_MENU_0:
                if (player->GetQuestStatus(QUEST_THE_HUNTER) == QUEST_STATUS_INCOMPLETE)
                {
                    player->KilledMonsterCredit(me->GetEntry());
                    Talk(2);
                }
                break;
            default:
                break;
        }

        return true;
    }

    void OnQuestReward(Player* player, Quest const* quest, LootItemType /*type*/, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_THE_HUNTER)
            if (const Quest* quest_1 = sObjectMgr->GetQuestTemplate(QUEST_THE_ANCIENT_ONE))
            {
                player->AddQuest(quest_1, nullptr);
                player->ForceCompleteQuest(QUEST_THE_ANCIENT_ONE);
            }
    }
};

//122866, Vol'kaal <Zanchuli Council>
struct npc_volkaal : public ScriptedAI
{
    npc_volkaal(Creature* creature) : ScriptedAI(creature) { }
    bool talked1;
    bool talked2;

    void Reset() override
    {
        ScriptedAI::Reset();
        talked1 = false;
        talked2 = false;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->GetQuestStatus(QUEST_MISTRESS_OF_LIES) == QUEST_STATUS_INCOMPLETE)
                if (player->IsInDist(me, 40.0f) && !talked1)
                    if (Creature* yazma = me->FindNearestCreature(NPC_YAZMA, 50.0f))
                    {
                        talked1 = true;
                        yazma->AI()->Talk(0);
                    }
    }

    void JustEngagedWith(Unit* who) override
    {
        if (Player* player = who->ToPlayer())
            if (player->GetQuestStatus(QUEST_MISTRESS_OF_LIES) == QUEST_STATUS_INCOMPLETE)
            {
                Talk(0);

                if (Creature* yazma = me->FindNearestCreature(NPC_YAZMA, 50.0f))
                {
                    yazma->GetScheduler().Schedule(Milliseconds(4000), [](TaskContext context)
                    {
                        GetContextCreature()->AI()->Talk(1);
                    });
                }
            }
    }

   void DamageTaken(Unit* attacker, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (Player* player = attacker->ToPlayer())
            if (player->GetQuestStatus(QUEST_MISTRESS_OF_LIES) == QUEST_STATUS_INCOMPLETE)
                if (me->GetHealth() < me->CountPctFromMaxHealth(50))
                    if (Creature* yazma = me->FindNearestCreature(NPC_YAZMA, 50.0f))
                        if (!talked2)
                        {
                            talked2 = true;
                            yazma->AI()->Talk(2);
                        }
    }

    void JustDied(Unit* killer) override
    {
        if (Player* player = killer->ToPlayer())
            if (player->GetQuestStatus(QUEST_MISTRESS_OF_LIES) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* yazma = me->FindNearestCreature(NPC_YAZMA, 50.0f))
                {
                    yazma->AI()->Talk(3);
                    yazma->GetScheduler().Schedule(Milliseconds(8000), [player](TaskContext context)
                    {
                        GetContextCreature()->AI()->Talk(4);
                        player->ForceCompleteQuest(QUEST_MISTRESS_OF_LIES);
                    });
                }
            }
    }
};

//290997, Mogu Weapon
struct go_mogu_weapon : public GameObjectAI
{
    go_mogu_weapon(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        player->KilledMonsterCredit(NPC_HOLD_SEARCHED);

        return false;
    }
};

//49489 49490
struct quest_Needs_a_Little_Body__The_Urn_of_Voices : public QuestScript
{
    quest_Needs_a_Little_Body__The_Urn_of_Voices() : QuestScript("quest_Needs_a_Little_Body__The_Urn_of_Voices") { }

    void OnQuestStatusChange(Player* player, Quest const* /*quest*/, QuestStatus oldStatus, QuestStatus /*newStatus*/) override
    {
        if (oldStatus == QUEST_STATUS_NONE)
        {
            if (player->GetQuestStatus(49489) != QUEST_STATUS_NONE && player->GetQuestStatus(49490) != QUEST_STATUS_NONE)
            {
                player->SummonCreature(129907, Position(-457.821f, 392.892f, 177.829f, 5.50005f), TEMPSUMMON_MANUAL_DESPAWN);
            }
        }
    }
};

// 47433
struct quest_forbidden_practices : public QuestScript
{
    quest_forbidden_practices() : QuestScript("quest_forbidden_practices") { }

    void OnQuestObjectiveChange(Player* player, Quest const* /*quest*/, QuestObjective const& objective, int32 /*oldAmount*/, int32 /*newAmount*/) override
    {
        if (objective.ID == 292610)
        {
            if (player->GetQuestObjectiveCounter(292610) == 0 && player->HasQuest(47433))
            {
                player->SummonCreature(126564, Position(-485.618f, 749.658f, 293.962f, 1.62767f), TEMPSUMMON_MANUAL_DESPAWN);
            }
        }
    }
};

void AddSC_zone_zuldazar()
{
    RegisterCreatureAI(npc_talanji_arrival);
    RegisterCreatureAI(npc_talanji_arrival_escort);
    RegisterCreatureAI(npc_enforcer_pterrordax);
    RegisterCreatureAI(npc_talanji_135440);
    RegisterCreatureAI(npc_rastakhan_zuldazar_arrival);
    RegisterCreatureAI(npc_soth_zolani);
    RegisterCreatureAI(npc_brillin_the_beauty);
    RegisterCreatureAI(npc_horde_banner);
    RegisterCreatureAI(npc_natal_hakata);
    RegisterCreatureAI(npc_telemancer_oculeth_zuldazar);
    RegisterCreatureAI(npc_talanji_great_seal);
    RegisterCreatureAI(npc_ata_the_winglord_offensively_defence);
    RegisterCreatureAI(npc_ata_the_winglord_paku_master_of_winds);
    RegisterCreatureAI(npc_pterrordax_paku_master_of_winds);
    RegisterCreatureAI(npc_paku);
    RegisterSpellScript(spell_rastari_skull_whistle);
    RegisterSpellScript(spell_quest_revitalize_spirit_obelisk);
    RegisterSpellScript(spell_call_the_storm);
    new spell_lightning_rod();
    RegisterCreatureAI(npc_voljamba);
    RegisterSpellScript(spell_soul_jaunt);
    new scene_warpack();
    RegisterCreatureAI(npc_captain_rezokon_123000);
    RegisterCreatureAI(npc_rakera_146812);
    RegisterCreatureAI(npc_hexlord_146851);
    RegisterCreatureAI(npc_dread_admiral_tattersail_135690);
    RegisterPlayerScript(zuldazar_phase_manager);
    RegisterGameObjectAI(go_chalice_of_calling);
    RegisterCreatureAI(npc_fleet_scout);
    RegisterCreatureAI(npc_harli_the_swift);
    RegisterCreatureAI(npc_captain_grezko);
    RegisterGameObjectAI(go_jailer_cage);
    RegisterCreatureAI(npc_wavesinger_zara);
    RegisterSpellScript(spell_scroll_of_illusion);
    RegisterCreatureAI(npc_lieutenant_dennis_grimtale);
    RegisterCreatureAI(npc_kral_master_bkhor);
    RegisterGameObjectAI(go_soothing_lilybud);
    RegisterCreatureAI(npc_pinky);
    RegisterCreatureAI(npc_gonk);
    RegisterCreatureAI(npc_volkaal);
    RegisterGameObjectAI(go_mogu_weapon);
    RegisterQuestScript(quest_Needs_a_Little_Body__The_Urn_of_Voices);
    RegisterQuestScript(quest_forbidden_practices);
}
