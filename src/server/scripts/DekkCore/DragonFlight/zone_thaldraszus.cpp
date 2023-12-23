/*
 * Copyright 2023 DekkCore
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

#include "GameObject.h"
#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"

enum thaldrazsusquest
{
  QUEST_AN_ANOMALOUS_SHRINE = 67093,
  QUEST_Exeunt_Triumphant = 66124,
  QUEST_SHARP_PRACTICE = 66078,
  QUEST_THE_COURAGE_OF_ONES_CONVICTION = 65939,
  QUEST_A_CRACK_IN_THE_WORLD           = 72976,
  QUEST_THE_PATIENCE_OF_PRINCESS       = 72981,
};

enum thaldrazsusevent
{
    EVENT_DO_CAST_SPELL,
    EVENT_STOP_DUEL,
    SPELL_DUEL = 52996,
    SPELL_DUEL_TRIGGERED = 52990,
    SPELL_DUEL_VICTORY = 52994,
    SPELL_DUEL_FLAG = 52991,
    KILL_CREDIT_BLACKTALON = 187158,
};

class Quest67093 : public PlayerScript
{
public:
    Quest67093() : PlayerScript("Quest67093") { }

    void OnCreatureKill(Player* killer, Creature* killed)
    {
        if (killer->HasQuest(QUEST_AN_ANOMALOUS_SHRINE))
            killer->ForceCompleteQuest(QUEST_AN_ANOMALOUS_SHRINE);     
    };
};

//187278 majordomo selistra
class npc_majordomo_selistra187278 : public CreatureScript
{
public:
    npc_majordomo_selistra187278() : CreatureScript("npc_majordomo_selistra187278") { }

    struct npc_majordomo_selistra187278AI : public ScriptedAI
    {
        npc_majordomo_selistra187278AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_Exeunt_Triumphant))
                player->ForceCompleteQuest(QUEST_Exeunt_Triumphant);

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_majordomo_selistra187278AI(creature);
    }
};

#define GOSSIP_LETS_TEST "Let's duel."

class npc_blacktalon_duelist : public ScriptedAI
{
public:
    npc_blacktalon_duelist(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        //_events.Reset();
        me->RestoreFaction();
        me->SetReactState(REACT_DEFENSIVE);
    }

    bool OnGossipHello(Player* player) override
    {
        if (player->GetQuestStatus(QUEST_SHARP_PRACTICE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_SHARP_PRACTICE) == QUEST_STATUS_INCOMPLETE)
            AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_LETS_TEST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 gossipListId) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (GOSSIP_ACTION_INFO_DEF + 1)
        {
            me->AI()->SetGUID(player->GetGUID());
            me->SetFaction(FACTION_TEMPLATE_FLAG_PVP);
            me->RemoveUnitFlag(UNIT_FLAG_IMMUNE_TO_PC);
            me->SetReactState(REACT_AGGRESSIVE);

            player->CastSpell(me, SPELL_DUEL, false);
            player->CastSpell(player, SPELL_DUEL_FLAG, true);

            me->AI()->AttackStart(player);
            CloseGossipMenuFor(player);
        }
        return true;
    }

    void JustEngagedWith(Unit* who) override {
        _events.ScheduleEvent(EVENT_DO_CAST_SPELL, 1s);
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/)override
    {
        if (damage >= me->GetHealth())
        {
            damage = me->GetHealth() - 1;
            _events.Reset();
            me->RemoveAllAuras();
            me->SetFaction(35);
            me->AttackStop();
            attacker->AttackStop();
            attacker->ClearInCombat();
            attacker->ToPlayer()->KilledMonsterCredit(KILL_CREDIT_BLACKTALON);
            _events.ScheduleEvent(EVENT_STOP_DUEL, 1s);
        }
    }

    void UpdateAI(uint32 diff) override {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent()) {
            switch (eventId) {
            case EVENT_DO_CAST_SPELL:
                _events.RescheduleEvent(EVENT_DO_CAST_SPELL, 4s);
                break;
            case EVENT_STOP_DUEL:
                me->Say("Fine duel.", LANG_UNIVERSAL, nullptr);
                me->GetMotionMaster()->MoveTargetedHome();
                break;
            default:
                break;
            }
        }
        DoMeleeAttackIfReady();
    }

    void SetGUID(ObjectGuid const& guid, int32 /*p_Index*/) override
    {
        _playerGuid = guid;
    }

private:
    EventMap _events;
    ObjectGuid _playerGuid = ObjectGuid::Empty;

};

//186681 Wrathion
class npc_wrathion186681 : public CreatureScript
{
public:
    npc_wrathion186681() : CreatureScript("npc_wrathion186681") { }

    struct npc_wrathion186681AI : public ScriptedAI
    {
        npc_wrathion186681AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_THE_COURAGE_OF_ONES_CONVICTION))
                player->ForceCompleteQuest(QUEST_THE_COURAGE_OF_ONES_CONVICTION);
                player->TeleportTo(2444, 2096.082f, 2308.810f, 242.460f, 5.258f);

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_wrathion186681AI(creature);
    }
};

//201281 Ebyssian
class npc_Ebyssian201281 : public CreatureScript
{
public:
    npc_Ebyssian201281() : CreatureScript("npc_Ebyssian201281") { }

    struct npc_Ebyssian201281AI : public ScriptedAI
    {
        npc_Ebyssian201281AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->HasQuest(QUEST_A_CRACK_IN_THE_WORLD))
                player->ForceCompleteQuest(QUEST_A_CRACK_IN_THE_WORLD);

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_Ebyssian201281AI(creature);
    }
};

//386486 Portal to zaralek Caverns
class go_portal_zaralek_kavern : public GameObjectScript
{
public:
    go_portal_zaralek_kavern() : GameObjectScript("go_portal_zaralek_kavern") {}

    struct go_portal_zaralek_kavernAI : public GameObjectAI
    {
        go_portal_zaralek_kavernAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
            player->TeleportTo(2454, 498.986f, 1374.295f, -140.073f, 2.4865f);

            if (player->HasQuest(QUEST_THE_PATIENCE_OF_PRINCESS))
                player->ForceCompleteQuest(QUEST_THE_PATIENCE_OF_PRINCESS);

            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_portal_zaralek_kavernAI(go);
    }
};

void AddSC_zone_thaldraszus()
{
    new Quest67093();
    new npc_majordomo_selistra187278();
    RegisterCreatureAI(npc_blacktalon_duelist);
    new npc_wrathion186681();
    new npc_Ebyssian201281();
    new go_portal_zaralek_kavern();
}
