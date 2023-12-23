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

#include "ScriptedCreature.h"
#include "Conversation.h"
#include "Creature.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"

enum Quests
{
    QUEST_TRAVELS_NAZMIR             = 47103,
    QUEST_NAZMIR_FORBIDDEN_SWAMP     = 48535,
	QUEST_IN_THE_DARK                = 47105,
	QUEST_THAT_NOT_ONE_IS_LEFT_ALIVE = 47264,
	QUEST_UNSEEMLY_BURIAL            = 47130,
	QUEST_ENDING_BLOOD_TROLLS        = 47262,
	QUEST_TIME_OF_REVELATIONS        = 47263,
	QUEST_THE_HELP_OF_THE_LOA        = 47188,
	QUEST_THE_SHADOW_OF_DEATH        = 47241,
    QUEST_ITS_ALIVE                  = 54213,
    QUEST_WE_HAVE_THEM_CONCERNED     = 54244,
    QUEST_PARTING_MISTS              = 54275,
    QUEST_FLY_OUT_TO_MEET_THEM       = 54280,
    QUEST_DUBIOUS_OFFERING           = 51129,
};

enum Creatures
{
    NPC_RIDE_ROBOT_VEHICLE = 147313,
    NPC_RIDING_RAPTOR_VEHICLE = 147318,
    NPC_PTERRODAX_VEHICLE = 143701,
};

enum Scenes
{
    SCENE_ZULDAZAR_ATTACK = 2415,
};

enum ConversationQuestsNazmir
{
    //Conversation Rokhan ID 126549
    CONVERSATION_ROKHAN_NAZMIR_1 = 5800,
    CONVERSATION_ROKHAN_NAZMIR_2 = 6446,
};

//146921
struct npc_princess_talanji_146921 : public ScriptedAI
{
    npc_princess_talanji_146921(Creature* c) : ScriptedAI(c) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_WE_HAVE_THEM_CONCERNED)
            player->SummonCreature(NPC_RIDING_RAPTOR_VEHICLE, me->GetPosition());
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        if (!player->GetQuestObjectiveProgress(QUEST_ITS_ALIVE, 0))
        {
            me->AI()->Talk(0);
            player->KilledMonsterCredit(146921);
            player->GetScheduler().Schedule(5s, [this, player](TaskContext /*context*/)
            {
                me->SummonCreature(NPC_RIDE_ROBOT_VEHICLE, player->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 90s);
            });
        }

        if (player->HasQuest(QUEST_PARTING_MISTS) && !player->GetQuestObjectiveProgress(QUEST_PARTING_MISTS, 2))
        {
            player->KilledMonsterCredit(148888);
            player->GetScheduler().Schedule(5s, [player](TaskContext /*context*/)
            {
                player->GetSceneMgr().PlaySceneByPackageId(SCENE_ZULDAZAR_ATTACK, SceneFlag::None);
            });
        }

        return true;
    }
};

//147318
struct npc_riding_raptor_147318 : public ScriptedAI
{
    npc_riding_raptor_147318(Creature* c) : ScriptedAI(c) { /*Vehicle* vehicle = me->GetVehicleKit();*/ }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_WE_HAVE_THEM_CONCERNED))
            {
                me->SetWalk(false);
                player->EnterVehicle(me);
                me->GetMotionMaster()->MovePoint(1, 1705.989f, 1806.780f, 12.516f, true);
                player->KilledMonsterCredit(me->GetEntry());
            }
            if (player->HasQuest(QUEST_PARTING_MISTS))
            {
                me->SetWalk(false);
                player->EnterVehicle(me);
                me->GetMotionMaster()->MovePoint(2, 2429.223f, 1762.098f, 0.265f, true);
                player->KilledMonsterCredit(147686);
            }
        }
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (point == 1)
            me->DespawnOrUnsummon();

        if (point == 2)
            me->DespawnOrUnsummon();
    }
};

//147233
struct npc_rokhan_147233 : public ScriptedAI
{
    npc_rokhan_147233(Creature* c) : ScriptedAI(c) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_PARTING_MISTS)
        {
            player->KilledMonsterCredit(147669);
            player->NearTeleportTo(1884.685f, 1775.752f, -0.199f, false);
            player->GetScheduler().Schedule(5s, [this, player](TaskContext /*context*/)
            {
                player->SummonCreature(NPC_RIDING_RAPTOR_VEHICLE, me->GetPosition());
            });
        }
    }
};

//147075
struct npc_general_rakera_147075 : public ScriptedAI
{
    npc_general_rakera_147075(Creature* c) : ScriptedAI(c) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_FLY_OUT_TO_MEET_THEM)
            player->SummonCreature(NPC_PTERRODAX_VEHICLE, me->GetPosition());
    }
};

//143701
struct npc_pterrodax_143701 : public ScriptedAI
{
    npc_pterrodax_143701(Creature* c) : ScriptedAI(c) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (Player* player = summoner->ToPlayer())
        {
            if (player->HasQuest(QUEST_FLY_OUT_TO_MEET_THEM))
            {
                player->EnterVehicle(me);
                me->GetMotionMaster()->MovePoint(1, -349.566f, 1171.464f, 316.705f, true);
                player->KilledMonsterCredit(147707);
            }
        }
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (point == 1)
            me->DespawnOrUnsummon();
    }
};

//257255
class spell_play_chapter_1 : public AuraScript
{
    PrepareAuraScript(spell_play_chapter_1);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(caster, 242682);
        if (Creature* tzena = caster->FindNearestCreature(121530, 5))
        {
            caster->CastSpell(tzena, 46598);
        }
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_play_chapter_1::OnRemove, EFFECT_2, SPELL_AURA_PLAY_SCENE, AURA_EFFECT_HANDLE_REAL);
    }
};

//126549
class npc_rokhan_126549 : public ScriptedAI
{
public:
    npc_rokhan_126549(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* unit) override
    {
        Player* player = unit->ToPlayer();
        if (!player)
            return;

        if (player->GetDistance(me) > 10.0f)
            return;

        if (!player->HasQuest(QUEST_TRAVELS_NAZMIR) || player->GetQuestObjectiveProgress(QUEST_TRAVELS_NAZMIR, 0))
            return;

        player->PlayConversation(ConversationQuestsNazmir::CONVERSATION_ROKHAN_NAZMIR_1);
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_NAZMIR_FORBIDDEN_SWAMP)
        {
            if (Creature* rokhan = player->SummonCreature(me->GetEntry(), me->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN, 0s, 0, true))
            {
                rokhan->AI()->SetGUID(player->GetGUID());

                me->DestroyForPlayer(player);
            }
        }

    }

    void SetGUID(ObjectGuid const& guid, int32 /*index*/) override
    {
        m_playerGUID = guid;
        me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);

//        me->GetScheduler().Schedule(1s, [](TaskContext /*context*/)
//            {
//                //player->PlayConversation(ConversationQuestsNazmir::CONVERSATION_ROKHAN_NAZMIR_2);
//               //me->GetMotionMaster()->MovePoint(1, 0f, 0f, 0f);
//            })
//            .Schedule(2s, [this](TaskContext /*context*/)
//                {
//
//                });
    }

private:
    Player* GetPlayer() { return ObjectAccessor::GetPlayer(*me, m_playerGUID); }

    ObjectGuid m_playerGUID;
};

// 122689
class npc_pakuai_rokota_122689 : public ScriptedAI
{
public:
    npc_pakuai_rokota_122689(Creature* creature) : ScriptedAI(creature) { }

    //bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    //{

    //}
};

//Zalamar Messenger Bat 136457
class zalamarmessengerbat136457 : public CreatureScript
{
public:
    zalamarmessengerbat136457() : CreatureScript("zalamarmessengerbat136457") { }

    struct zalamarmessengerbat136457AI : public ScriptedAI
    {
        zalamarmessengerbat136457AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready to go to Zalamar!", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                if (player->HasQuest(QUEST_DUBIOUS_OFFERING))
                    player->ForceCompleteQuest(QUEST_DUBIOUS_OFFERING);
                player->TeleportTo(1642, 1871.943f, 1791.729f, -116.232f, 0.918f);

                CloseGossipMenuFor(player);
                break;
            }

            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new zalamarmessengerbat136457AI(creature);
    }
};

void AddSC_zone_nazmir()
{
    RegisterCreatureAI(npc_princess_talanji_146921);
    RegisterCreatureAI(npc_riding_raptor_147318);
    RegisterCreatureAI(npc_rokhan_147233);
    RegisterCreatureAI(npc_general_rakera_147075);
    RegisterCreatureAI(npc_pterrodax_143701);
    RegisterSpellScript(spell_play_chapter_1);
    RegisterCreatureAI(npc_rokhan_126549);
    RegisterCreatureAI(npc_pakuai_rokota_122689);
    new zalamarmessengerbat136457();
}
