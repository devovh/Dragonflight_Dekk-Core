/*
 * Copyright (C) DekkCore
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
#include "GameObject.h"
#include "GameObjectAI.h"
#include "Garrison.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "QuestDef.h"
#include "WodGarrison.h"

enum
{
    QUEST_THE_HOME_OF_THE_FROSTWOLFES   = 33868,
    QUEST_A_SONG_OF_FROST_AND_FIRE      = 33815,
    QUEST_OF_WOLFES_AND_WARRIORS        = 34402,

    // Quest
    QUEST_ESTABLISH_YOUR_GARRISON_HORDE = 34378,

    // Spells
    SPELL_QUEST_34378_KILLCREDIT        = 161033,
    SPELL_CREATE_GARRISON_FROSTFIRE_RIDGE_HORDE = 160767
};

enum
{
    SpellEarthrendingSlam = 165907,
    SpellRampage          = 148852
};

enum
{
    EventEarthrendingSlam = 1,
    EventRampage          = 2,
    TEXT_GENERIC_2 = 3,
    TEXT_GENERIC_1
};

enum
{
    NPC_DUROTAN_BEGIN                       = 78272,
    NPC_OF_WOLFES_AND_WARRIOR_KILL_CREDIT   = 78407,
    NPC_ESTABLISH_YOUR_GARRISON_KILL_CREDIT = 79757,
};

enum
{
    SCENE_LANDING_TO_TOP_OF_HILL    = 771,
    SCENE_TOP_OF_HILL_TO_GARRISON   = 778,
    SCENE_PORTAL_OPENING            = 789,
    SCENE_DUROTAN_DEPARTS           = 798,
};

/// Passive Scene Object
class playerScript_frostridge_landing_to_hill : public PlayerScript
{
public:
    playerScript_frostridge_landing_to_hill() : PlayerScript("playerScript_frostridge_landing_to_hill") { }

    void OnSceneTriggerEvent(Player* player, uint32 p_SceneInstanceID, std::string p_Event) override
    {
        if (!player->GetSceneMgr().HasScene(p_SceneInstanceID, SCENE_LANDING_TO_TOP_OF_HILL))
            return;

        if (p_Event == "durotanIntroduced")
        {
            if (Creature* durotan = player->FindNearestCreature(NPC_DUROTAN_BEGIN, 50.0f))
                durotan->AI()->Talk(0);

            player->KilledMonsterCredit(NPC_DUROTAN_BEGIN);
        }
    }
};

/// Passive Scene Object
class playerScript_frostridge_hill_to_garrison : public PlayerScript
{
public:
    playerScript_frostridge_hill_to_garrison() : PlayerScript("playerScript_frostridge_hill_to_garrison") { }

    void OnSceneComplete(Player* player, uint32 p_SceneInstanceId) override
    {
        if (!player->GetSceneMgr().HasScene(p_SceneInstanceId, SCENE_TOP_OF_HILL_TO_GARRISON))
            return;

        player->KilledMonsterCredit(NPC_OF_WOLFES_AND_WARRIOR_KILL_CREDIT);
    }
};

// 76411 - Drek'Thar - D?ut Givrefeu
struct  npc_drekthar_frostridge_begin : public ScriptedAI
{
    npc_drekthar_frostridge_begin(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_A_SONG_OF_FROST_AND_FIRE)
        {
            player->GetSceneMgr().PlayScene(SCENE_LANDING_TO_TOP_OF_HILL);
            player->ForceCompleteQuest(QUEST_A_SONG_OF_FROST_AND_FIRE);
        }

    }

    struct npc_drekthar_frostridge_beginAI : public ScriptedAI
    {
        npc_drekthar_frostridge_beginAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 waitTime;

        void Reset() override
        {
            waitTime = 1000;
        }

        void UpdateAI(uint32 diff) override
        {
            if (waitTime > diff)
            {
                waitTime -= diff;
                return;
            }

            waitTime = 1000;

            std::list<Player*> playerList;
            me->GetPlayerListInGrid(playerList, 30.0f);

            for (Player* player : playerList)
            {
                if (player->GetQuestStatus(QUEST_THE_HOME_OF_THE_FROSTWOLFES) != QUEST_STATUS_NONE)
                    continue;

                if (const Quest* quest = sObjectMgr->GetQuestTemplate(QUEST_THE_HOME_OF_THE_FROSTWOLFES))
                    player->AddQuestAndCheckCompletion(quest, me);
            }
        }
    };
};

// 78272 - Durotan - D?ut Givrefeu
struct npc_durotan_frostridge_begin : public ScriptedAI
{
    npc_durotan_frostridge_begin(Creature* creature) : ScriptedAI(creature) {  }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_OF_WOLFES_AND_WARRIORS)
        {
            player->GetSceneMgr().PlayScene(SCENE_TOP_OF_HILL_TO_GARRISON);
            player->ForceCompleteQuest(QUEST_OF_WOLFES_AND_WARRIORS);
        }
    }
};

// Revendication 169421
class spell_frostridge_claiming : public SpellScriptLoader
{
public:
    spell_frostridge_claiming() : SpellScriptLoader("spell_frostridge_claiming") { }

    class spell_frostridge_claiming_spellscript : public SpellScript
    {
        PrepareSpellScript(spell_frostridge_claiming_spellscript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (!GetCaster())
                return;

            Player* player = GetCaster()->ToPlayer();

            if (!player)
                return;

            player->GetSceneMgr().PlaySceneByPackageId(SCENE_PORTAL_OPENING, SceneFlag::None);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_frostridge_claiming_spellscript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_frostridge_claiming_spellscript();
    }
};

Position const GarrisonLevelOneCreationPlayerPosition = { 5568.66f, 4635.45f, 146.61f, 5.079972743988037109f };

struct go_master_surveyor : public GameObjectAI
{
    go_master_surveyor(GameObject* go) : GameObjectAI(go) { }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_ESTABLISH_YOUR_GARRISON_HORDE) == QUEST_STATUS_INCOMPLETE)
        {
            me->UseDoorOrButton();
            player->CastSpell(player, SPELL_QUEST_34378_KILLCREDIT, true);
            player->CastSpell(player, SPELL_CREATE_GARRISON_FROSTFIRE_RIDGE_HORDE, true);
            player->NearTeleportTo(GarrisonLevelOneCreationPlayerPosition);
            PhasingHandler::OnConditionChange(player);

            return true;
        }

        return false;
    }
};

// 80167 - Groog
class npc_groog : public CreatureScript
{
    public:
        npc_groog() : CreatureScript("npc_groog") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_groogAI(creature);
        }

        struct npc_groogAI : public ScriptedAI
        {
            npc_groogAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap m_Events;

            void Reset() override
            {
                me->SetFaction(14);
            }

            void JustEngagedWith(Unit* /*p_Victim*/) override
            {
                m_Events.Reset();

                m_Events.ScheduleEvent(EventEarthrendingSlam, 3s);
                m_Events.ScheduleEvent(EventRampage, 7s);
            }

            void UpdateAI(uint32 diff) override
            {
                m_Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || !UpdateVictim())
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case EventEarthrendingSlam:
                        me->CastSpell(me, SpellEarthrendingSlam, false);
                        m_Events.ScheduleEvent(EventEarthrendingSlam, 15s);
                        break;
                    case EventRampage:
                        me->AddAura(SpellRampage, me);
                        m_Events.ScheduleEvent(EventRampage, 15s);
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Rampage - 148852
class spell_groog_rampage : public SpellScriptLoader
{
    public:
        spell_groog_rampage() : SpellScriptLoader("spell_groog_rampage") { }

        class spell_groog_rampage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_groog_rampage_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                Unit* caster = GetCaster();

                if (!caster)
                    return;

                PreventDefaultAction();

                std::list<Player*> playerList;
                caster->GetPlayerListInGrid(playerList, 2.0f);

                caster->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);

                for (Player* player : playerList)
                {
                    if (player->HasUnitState(UNIT_STATE_ROOT))
                        continue;

                    player->KnockbackFrom(player->m_positionX, player->m_positionY, 10.0f);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_groog_rampage_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_groog_rampage_AuraScript();
        }
};

// 79526
class mob_wod_q34375 : public CreatureScript
{
public:
    mob_wod_q34375() : CreatureScript("mob_wod_q34375") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_wod_q34375AI(creature);
    }

    struct mob_wod_q34375AI : public ScriptedAI
    {

        mob_wod_q34375AI(Creature* creature) : ScriptedAI(creature)
        {
        }

        enum data
        {
            SOUNDID = 7514,
            NPC_TREE = 79525
        };

        ObjectGuid treeGUID;
        void MovementInform(uint32 type, uint32 /*id*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;
            if (Creature* tree = me->GetMap()->GetCreature(treeGUID))
                me->SetFacingToObject(tree);
            me->HandleEmoteCommand(EMOTE_STATE_WORK_CHOPWOOD_2);
        }

        void IsSummonedBy(WorldObject* summoner) override
        {
            Player* player = summoner->ToPlayer();
            if (!player)
            {
                //  me->MonsterSay("SCRIPT::mob_wod_q34375 summoner is not player", LANG_UNIVERSAL, ObjectGuid::Empty);
                return;
            }
            me->PlayDirectSound(SOUNDID, player);
            if (Creature* tree = player->FindNearestCreature(NPC_TREE, 10.0f))
            {
                Position pos;
                tree->GetRandomNearPosition(5.0f);
                me->GetMotionMaster()->MovePoint(1, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
                treeGUID = tree->GetGUID();
            }
        }
    };
};

// 230527
class go_wod_q34375 : public GameObjectScript
{
public:
    go_wod_q34375() : GameObjectScript("go_wod_q34375") { }

    struct go_wod_q34375AI : public GameObjectAI
    {
        go_wod_q34375AI(GameObject* go) : GameObjectAI(go)
        {

        }

        enum data
        {
            SPELL_SUMMON = 160657,
            NPC_TREE = 79525,
            __QUEST = 34375,
        };

        EventMap events;
        ObjectGuid treeGUID;

        void InitializeAI()
        {
            Reset();
            events.RescheduleEvent(TEXT_GENERIC_2, 1s);

            me->m_invisibility.AddFlag(INVISIBILITY_QUEST_ZONE_SPECIFIC_1);
            me->m_invisibility.AddValue(INVISIBILITY_QUEST_ZONE_SPECIFIC_1, 1000);
        }

        bool GossipUse(Player* player)
        {
            if (player->GetQuestStatus(__QUEST) != QUEST_STATUS_INCOMPLETE)
                return true;

            Creature* tree = me->GetMap()->GetCreature(treeGUID);

            if (!tree
                || !tree->IsAlive())
                return true;
            player->KillCreditGO(me->GetEntry(), me->GetGUID());

            events.RescheduleEvent(TEXT_GENERIC_1, 15s);

            me->UseDoorOrButton(30, false, player);
            return true;
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            if (uint32 eventId = events.ExecuteEvent())
            {
                if (eventId == TEXT_GENERIC_1)
                {
                    if (Creature* tree = me->GetMap()->GetCreature(treeGUID))
                    {
                        tree->SetCorpseDelay(0);
                        tree->SetRespawnDelay(30);
                        tree->ForcedDespawn();
                    }
                }
                else
                {
                    if (Creature* tree = me->FindNearestCreature(NPC_TREE, 5.0f))
                    {
                        treeGUID = tree->GetGUID();
                        if (!tree->IsAlive())
                            tree->Respawn();
                    }
                    else
                    {
                        Position pos;
                        me->GetRandomNearPosition(5.0f);
                        if (TempSummon* summon = me->SummonCreature(NPC_TREE, pos))
                            treeGUID = me->GetGUID();
                    }
                }
            }
        }
    };

    GameObjectAI* GetAI(GameObject* go) const
    {
        return new go_wod_q34375AI(go);
    }
};

void AddSC_frostfire_ridge()
{
    /* BEGIN */
    new playerScript_frostridge_landing_to_hill();
    new playerScript_frostridge_hill_to_garrison();

    RegisterCreatureAI(npc_drekthar_frostridge_begin);
    RegisterCreatureAI(npc_durotan_frostridge_begin);

    new mob_wod_q34375();
    RegisterGameObjectAI(go_master_surveyor);
    new go_wod_q34375();

    new spell_frostridge_claiming();

    /* BOSS */
    new npc_groog();
    new spell_groog_rampage();
}
