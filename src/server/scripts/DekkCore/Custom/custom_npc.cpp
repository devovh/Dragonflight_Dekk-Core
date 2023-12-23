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
#include "ScriptedGossip.h"
#include "LFGMgr.h"
#include "MiscPackets.h"
#include "ObjectAccessor.h"
#include "Vehicle.h"
#include "Item.h"
#include "BattlePetDataStore.h"
#include "BattlePetSystem.h"
#include "PetBattle.h"
#include "PhasingHandler.h"
#include "Flux.h"

enum ColdheartInterstitiaGossips
{
    GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA = 26658,
};

//175000, 175009, 175025,175026, 175027,175028, 175029
class npc_coldheart_interstitia_175026 : public CreatureScript
{
public:
    npc_coldheart_interstitia_175026() : CreatureScript("npc_coldheart_interstitia_175026") { }

    struct npc_coldheart_interstitia_175026AI : public ScriptedAI
    {
        npc_coldheart_interstitia_175026AI(Creature* creature) : ScriptedAI(creature) { }

        bool issend = false;
        void MoveInLineOfSight(Unit* who) override
        {
            if (!who->IsPlayer())
                return;
            if (me->GetDistance2d(who) < 5.0f)
            {
                if (Player* player = who->ToPlayer())
                {
                    if (issend)
                        return;
                    issend = true;
                    ClearGossipMenuFor(player);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 0 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 1 + 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 2 + 2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 3 + 3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 4 + 4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 5 + 5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 6 + 6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                    AddGossipItemFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, 7 + 7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
                    SendGossipMenuFor(player, GOSSIP_MENU_SELECT_COLDHEART_INTERSTITIA, me->GetGUID());

                }
            }
            else
                issend = false;
        }

        uint32 GetPostionOffset()
        {
            uint32 offset = 0;
            switch (me->GetEntry())
            {
            case 175000: offset = 0; break;
            case 175009: offset = 1; break;
            case 175025: offset = 2; break;
            case 175026: offset = 3; break;
            case 175027: offset = 4; break;
            case 175028: offset = 5; break;
            case 175029: offset = 6; break;
            default:
                break;
            }
            return offset;
        }

        void SendMenu(Player* player, uint32 spellId, int32 cost = 0, uint32 level = 1)
        {
            uint32 lfg = 0;
            uint32 scenario = 0;
            std::set<uint32> lfgId;
            if (player->HasCurrency(1828, cost))
            {
                player->CastSpell(player, spellId, true);
                player->ModifyCurrency(1828, -cost);
            }
            else
            {
                if (player->IsGameMaster())
                    player->CastSpell(player, spellId, true);
                else
                    return;
            }
            //Torghast: The Hall of Skodus
            if (GetPostionOffset() == 0)
            {
                switch (level)
                {
                case 1:lfg = 2100; scenario = 1848; break;
                case 2:lfg = 2128; scenario = 1863; break;
                case 3:lfg = 2129; scenario = 1865; break;
                case 4:lfg = 2130; scenario = 1866; break;
                case 5:lfg = 2131; scenario = 1867; break;
                case 6:lfg = 2132; scenario = 1868; break;
                case 7:lfg = 2133; scenario = 1869; break;
                case 8:lfg = 2134; scenario = 1870; break;
                default: break;
                }
            }
            //broken bone room
            if (GetPostionOffset() == 1)
            {
                switch (level)
                {
                case 1:lfg = 2101; scenario = 1849; break;
                case 2:lfg = 2135; scenario = 1871; break;
                case 3:lfg = 2136; scenario = 1872; break;
                case 4:lfg = 2137; scenario = 1873; break;
                case 5:lfg = 2138; scenario = 1874; break;
                case 6:lfg = 2139; scenario = 1875; break;
                case 7:lfg = 2140; scenario = 1876; break;
                case 8:lfg = 2141; scenario = 1877; break;
                default: break;
                }
            }
            //Soul Forge
            if (GetPostionOffset() == 2)
            {
                switch (level)
                {
                case 1:lfg = 2102; scenario = 1850; break;
                case 2:lfg = 2142; scenario = 1878; break;
                case 3:lfg = 2143; scenario = 1879; break;
                case 4:lfg = 2144; scenario = 1880; break;
                case 5:lfg = 2145; scenario = 1881; break;
                case 6:lfg = 2146; scenario = 1882; break;
                case 7:lfg = 2147; scenario = 1883; break;
                case 8:lfg = 2148; scenario = 1884; break;
                default: break;
                }
            }
            //heart-to-heart gap
            if (GetPostionOffset() == 3)
            {
                switch (level)
                {
                case 1:lfg = 2103; scenario = 1851; break;
                case 2:lfg = 2149; scenario = 1885; break;
                case 3:lfg = 2150; scenario = 1886; break;
                case 4:lfg = 2151; scenario = 1887; break;
                case 5:lfg = 2152; scenario = 1888; break;
                case 6:lfg = 2153; scenario = 1889; break;
                case 7:lfg = 2154; scenario = 1890; break;
                case 8:lfg = 2155; scenario = 1891; break;
                default: break;
                }
            }
            //Moltrega
            if (GetPostionOffset() == 4)
            {
                switch (level)
                {
                case 1:lfg = 2104; scenario = 1852; break;
                case 2:lfg = 2156; scenario = 1892; break;
                case 3:lfg = 2157; scenario = 1893; break;
                case 4:lfg = 2158; scenario = 1894; break;
                case 5:lfg = 2159; scenario = 1895; break;
                case 6:lfg = 2160; scenario = 1896; break;
                case 7:lfg = 2161; scenario = 1897; break;
                case 8:lfg = 2162; scenario = 1898; break;
                default: break;
                }
            }
            //upper area
            if (GetPostionOffset() == 5)
            {
                switch (level)
                {
                case 1:lfg = 2105; scenario = 1853; break;
                case 2:lfg = 2163; scenario = 1899; break;
                case 3:lfg = 2164; scenario = 1900; break;
                case 4:lfg = 2165; scenario = 1901; break;
                case 5:lfg = 2166; scenario = 1902; break;
                case 6:lfg = 2167; scenario = 1903; break;
                case 7:lfg = 2168; scenario = 1904; break;
                case 8:lfg = 2169; scenario = 1905; break;
                default: break;
                }
            }
            //Twisted Cloister
            if (GetPostionOffset() == 6)
            {
                switch (level)
                {
                case 1:lfg = 2106; scenario = 1857; break;
                case 2:lfg = 2170; scenario = 1906; break;
                case 3:lfg = 2171; scenario = 1907; break;
                case 4:lfg = 2172; scenario = 1908; break;
                case 5:lfg = 2173; scenario = 1909; break;
                case 6:lfg = 2174; scenario = 1910; break;
                case 7:lfg = 2175; scenario = 1911; break;
                case 8:lfg = 2176; scenario = 1912; break;
                default: break;
                }
            }
            if (scenario)
            {
                lfgId.insert(lfg);
                player->SetScenarioId(scenario);
                sLFGMgr->JoinLfg(player, lfg::LfgRoles::PLAYER_ROLE_DAMAGE, lfgId);
            }
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            uint32 const sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);

            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF + 1: SendMenu(player, 344276, 120, 1); break;
            case GOSSIP_ACTION_INFO_DEF + 2: SendMenu(player, 344277, 220, 2); break;
            case GOSSIP_ACTION_INFO_DEF + 3: SendMenu(player, 344278, 305, 3); break;
            case GOSSIP_ACTION_INFO_DEF + 4: SendMenu(player, 344279, 375, 4); break;
            case GOSSIP_ACTION_INFO_DEF + 5: SendMenu(player, 344280, 435, 5); break;
            case GOSSIP_ACTION_INFO_DEF + 6: SendMenu(player, 344281, 485, 6); break;
            case GOSSIP_ACTION_INFO_DEF + 7: SendMenu(player, 344282, 530, 7); break;
            case GOSSIP_ACTION_INFO_DEF + 8: SendMenu(player, 344283, 570, 8); break;
            }
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_coldheart_interstitia_175026AI(creature);
    }
};

enum
{
    QUEST_FEAR_NO_EVIL_WORGEN_WARRIOR = 28813,
    QUEST_FEAR_NO_EVIL_ALLIANCE = 29082,
    QUEST_FEAR_NO_EVIL_ALLIANCE_2 = 28809,
    QUEST_FEAR_NO_EVIL_ALLIANCE_3 = 28808,
    QUEST_FEAR_NO_EVIL_ALLIANCE_4 = 28811,
    QUEST_FEAR_NO_EIVL_ALLIANCE_5 = 28810,
    QUEST_FEAR_NO_EVIL_ALLIANCE_6 = 28806,
    QUEST_FEAR_NO_EVIL_ALLIANCE_NIGHT_ELF_WARLOCK_DK = 28812,
};

//180653 item id
class item_challenge_key : public ItemScript
{
public:
    item_challenge_key() : ItemScript("item_challenge_key") { }

    bool OnCreate(Player* player, Item* item) override
    {
        if (player->HasItemCount(180653, 1, false))
        {
            player->InitChallengeKey(item);
            item->SetState(ITEM_CHANGED);
        }
        else
        {
            player->CreateChallengeKey(item);
            item->SetState(ITEM_CHANGED);
        }
        return false;
    }
};

//143967
class npc_expedition_map : public CreatureScript
{
public:
    npc_expedition_map() : CreatureScript("npc_expedition_map") { }

    struct npc_expedition_mapAI : public ScriptedAI
    {
        npc_expedition_mapAI(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        return false;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        lfg::LfgDungeonSet newDungeons;
        newDungeons.insert(1768);
        sLFGMgr->JoinLfg(player, ROLE_DAMAGE, newDungeons);

        //alliance
        player->KilledMonsterCredit(139310);
        //horde
        player->KilledMonsterCredit(139309);

        // 1768 map 1955 spawndiff 12

        // 1724 spawndiff 38
        // 1743 spawndiff 39 heroic
        // 1746 spawndiff 40
        // 1763 spawndiff 45

        // 1726 1736 1737 1762


        return true;
    }
};

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_expedition_mapAI(creature);
    }
};

/*######
## npc_elder_kuruti
######*/

#define GOSSIP_ITEM_KUR1 "Greetings, elder. It is time for your people to end their hostility towards the draenei and their allies."
#define GOSSIP_ITEM_KUR2 "I did not mean to deceive you, elder. The draenei of Telredor thought to approach you in a way that would seem familiar to you."
#define GOSSIP_ITEM_KUR3 "I will tell them. Farewell, elder."

class npc_elder_kuruti : public CreatureScript
{
public:
    npc_elder_kuruti() : CreatureScript("npc_elder_kuruti") {}

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_elder_kurutiAI(creature);
    }

    struct npc_elder_kurutiAI : public ScriptedAI
    {
        npc_elder_kurutiAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (player->GetQuestStatus(9803) == QUEST_STATUS_INCOMPLETE)
                AddGossipItemFor(player, GossipOptionNpc::Binder, GOSSIP_ITEM_KUR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

            SendGossipMenuFor(player, 9226, me->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF:
                    AddGossipItemFor(player, GossipOptionNpc::Binder, GOSSIP_ITEM_KUR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    SendGossipMenuFor(player, 9227, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 1:
                    AddGossipItemFor(player, GossipOptionNpc::Binder, GOSSIP_ITEM_KUR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    SendGossipMenuFor(player, 9229, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    if (!player->HasItemCount(24573))
                    {
                        ItemPosCountVec dest;
                        uint32 itemId = 24573;
                        InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, 1, NULL);
                        if (msg == EQUIP_ERR_OK)
                        {
                            player->StoreNewItem(dest, itemId, true);
                        }
                        else
                            player->SendEquipError(msg, NULL, NULL, itemId);
                    }
                    SendGossipMenuFor(player, 9231, me->GetGUID());
                    break;
            }
            return true;
        }
    };

};

enum quests
{
    QUEST_TO_THE_DRAGON_ISLES_H = 65444,
};

// Skarukaru  197288
class npc_Skarukaru_197288 : public CreatureScript
{
public:
    npc_Skarukaru_197288() : CreatureScript("npc_Skarukaru_197288") { }

    struct npc_Skarukaru_197288AI : public ScriptedAI
    {
        npc_Skarukaru_197288AI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GossipOptionNpc::None, "I'm ready to go to waking shore!", GOSSIP_SENDER_MAIN, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            switch (action)
            {
            case 0:
                if (player->HasQuest(QUEST_TO_THE_DRAGON_ISLES_H))
                    player->ForceCompleteQuest(QUEST_TO_THE_DRAGON_ISLES_H);
                player->TeleportTo(2444, 3887.969f, -1863.859f, 5.724f, 1.7802f);

                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_Skarukaru_197288AI(creature);
    }
};

/// Thonk's Spyglass - 52514 
class item_thonks_spyglass_52514 : public ItemScript
{
public:
    item_thonks_spyglass_52514() : ItemScript("item_thonks_spyglass_52514") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets, ObjectGuid castId) override
    {
        if (player->GetQuestStatus(25187) == QUEST_STATUS_INCOMPLETE)
        {
            if (!player->GetReqKillOrCastCurrentCount(25187, 39357))
                player->CastSpell(player, 73741, TRIGGERED_NONE);
            else if (!player->GetReqKillOrCastCurrentCount(25187, 39358))
                player->CastSpell(player, 73763, TRIGGERED_NONE);
            else if (!player->GetReqKillOrCastCurrentCount(25187, 39359))
                player->CastSpell(player, 73764, TRIGGERED_NONE);
            else if (!player->GetReqKillOrCastCurrentCount(25187, 39360))
                player->CastSpell(player, 73765, TRIGGERED_NONE);
            player->ForceCompleteQuest(25187);
        }

        return false;
    }
};

enum
{
    PetBattleTrainerFightActionID = GOSSIP_ACTION_INFO_DEF + 0xABCD
};

struct npc_PetBattleTrainer : ScriptedAI
{
    npc_PetBattleTrainer(Creature* me) : ScriptedAI(me)
    {
        bIsTrainer = false;

        if (sBattlePetDataStore->GetPetBattleTrainerTeam(me->GetEntry()).empty())
            return;

        QuestObjectives CreatureBattleObjectives = Hoff::GetCreatureQuestObjectivesFromType(me->GetEntry(), QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC);
        for (QuestObjective Objective : CreatureBattleObjectives)
        {
            if (Objective.ObjectID == me->GetEntry())
            {
                bIsTrainer = true;
                questIDs.insert(Objective.QuestID);
            }
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (bIsTrainer)
        {
            bool check = false;
            for (auto questID : questIDs)
                if (player->GetQuestStatus(questID) == QUEST_STATUS_INCOMPLETE)
                {
                    check = true;
                    break;
                }
            if (check)
                if (BroadcastTextEntry const* bct = sBroadcastTextStore.LookupEntry(62660))
                    AddGossipItemFor(player, GossipOptionNpc::None, DB2Manager::GetBroadcastTextValue(bct, player->GetSession()->GetSessionDbLocaleIndex()), GOSSIP_SENDER_MAIN, 0);
        }

        player->TalkedToCreature(me->GetEntry(), me->GetGUID());
        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == 0)
        {
            CloseGossipMenuFor(player);

            static float distance = 10.0f;


            Position PlayerFightPos = Fluxurion::GetPositionFrontOf(me, distance);
            Position trainerPosition = Position(me->m_positionX, me->m_positionY, me->m_positionZ, me->m_orientation);

            PlayerFightPos.m_orientation = atan2(trainerPosition.GetPositionY() - PlayerFightPos.GetPositionY(), trainerPosition.GetPositionX() - PlayerFightPos.GetPositionX());
            PlayerFightPos.m_orientation = (PlayerFightPos.m_orientation >= 0.0f) ? PlayerFightPos.m_orientation : 2 * M_PI + PlayerFightPos.m_orientation;

            Position battleCenterPosition = Position((PlayerFightPos.m_positionX + trainerPosition.m_positionX) / 2, (PlayerFightPos.m_positionY + trainerPosition.m_positionY) / 2, 0.0f, trainerPosition.m_orientation + M_PI);
            battleCenterPosition.m_positionZ = player->GetMap()->GetHeight(player->GetPhaseShift(), battleCenterPosition.m_positionX, battleCenterPosition.m_positionY, MAX_HEIGHT);

            PetBattleRequest* battleRequest = sPetBattleSystem->CreateRequest(player->GetGUID());
            battleRequest->OpponentGuid = me->GetGUID();
            battleRequest->PetBattleCenterPosition = battleCenterPosition;
            battleRequest->TeamPosition[PET_BATTLE_TEAM_1] = PlayerFightPos;
            battleRequest->TeamPosition[PET_BATTLE_TEAM_2] = trainerPosition;
            battleRequest->RequestType = PET_BATTLE_TYPE_PVE;

            eBattlePetRequests canEnterResult = sPetBattleSystem->CanPlayerEnterInPetBattle(player, battleRequest);
            if (canEnterResult != BATTLE_PET_REQUEST_OK)
            {
                player->GetSession()->SendPetBattleRequestFailed(canEnterResult);
                sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);
                return true;
            }

            std::shared_ptr<BattlePetInstance> playerPets[MAX_PET_BATTLE_SLOTS];
            std::shared_ptr<BattlePetInstance> wildBattlePets[MAX_PET_BATTLE_SLOTS];

            for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
            {
                playerPets[i] = nullptr;
                wildBattlePets[i] = nullptr;
            }

            uint32 wildsPetCount = 0;
            for (BattlePetNpcTeamMember& v : sBattlePetDataStore->GetPetBattleTrainerTeam(me->GetEntry()))
            {
                if (wildsPetCount >= MAX_PET_BATTLE_SLOTS)
                    break;

                auto battlePetInstance = std::make_shared<BattlePetInstance>();

                uint32 l_DisplayID = 0;

                //if (sBattlePetSpeciesStore.LookupEntry(v.Specie) && sObjectMgr->GetCreatureTemplate(sBattlePetSpeciesStore.LookupEntry(v.Specie)->CreatureID))
                //{
                //    l_DisplayID = sObjectMgr->GetCreatureTemplate(sBattlePetSpeciesStore.LookupEntry(v.Specie)->CreatureID)->model[0];

                //    if (!l_DisplayID)
                //    {
                //        l_DisplayID = sObjectMgr->GetCreatureTemplate(sBattlePetSpeciesStore.LookupEntry(v.Specie)->CreatureID)->Modelid[1];

                //       /* if (!l_DisplayID)
                //        {
                //            l_DisplayID = sObjectMgr->GetCreatureTemplate(sBattlePetSpeciesStore.LookupEntry(v.Specie)->CreatureID)->Modelid[2];
                //            if (!l_DisplayID)
                //                l_DisplayID = sObjectMgr->GetCreatureTemplate(sBattlePetSpeciesStore.LookupEntry(v.Specie)->CreatureID)->Modelid[3];
                //        }*/
                //          }
                //      }

                battlePetInstance->JournalID.Clear();
                battlePetInstance->Slot = 0;
                battlePetInstance->NameTimeStamp = 0;
                battlePetInstance->Species = v.Specie;
                battlePetInstance->DisplayModelID = l_DisplayID;
                battlePetInstance->XP = 0;
                battlePetInstance->Flags = 0;
                battlePetInstance->Health = 20000;

                uint8 randQuality = sBattlePetDataStore->GetRandomQuailty();
                battlePetInstance->Quality = v.minquality > randQuality ? v.minquality : randQuality;
                battlePetInstance->Breed = sBattlePetDataStore->GetRandomBreedID(v.BreedIDs);
                battlePetInstance->Level = std::max(urand(v.minlevel, v.maxlevel), static_cast<uint32>(1));

                for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
                    battlePetInstance->Abilities[i] = v.Ability[i];

                wildBattlePets[wildsPetCount] = battlePetInstance;
                wildBattlePets[wildsPetCount]->OriginalCreature.Clear();
                wildsPetCount++;
            }

            size_t playerPetCount = 0;
            std::shared_ptr<BattlePet>* petSlots = player->GetBattlePetCombatTeam();
            for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
            {
                if (!petSlots[i])
                    continue;

                if (playerPetCount >= MAX_PET_BATTLE_SLOTS || playerPetCount >= player->GetUnlockedPetBattleSlot())
                    break;

                playerPets[playerPetCount] = std::make_shared<BattlePetInstance>();
                playerPets[playerPetCount]->CloneFrom(petSlots[i]);
                playerPets[playerPetCount]->Slot = playerPetCount;
                playerPets[playerPetCount]->OriginalBattlePet = petSlots[i];

                ++playerPetCount;
            }

            player->GetSession()->SendPetBattleFinalizeLocation(battleRequest);

            PetBattle* petBattle = sPetBattleSystem->CreateBattle();

            petBattle->Teams[PET_BATTLE_TEAM_1]->OwnerGuid = player->GetGUID();
            petBattle->Teams[PET_BATTLE_TEAM_1]->PlayerGuid = player->GetGUID();
            petBattle->Teams[PET_BATTLE_TEAM_2]->OwnerGuid = me->GetGUID();

            for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
            {
                if (playerPets[i])
                    petBattle->AddPet(PET_BATTLE_TEAM_1, playerPets[i]);

                if (wildBattlePets[i])
                    petBattle->AddPet(PET_BATTLE_TEAM_2, wildBattlePets[i]);
            }

            petBattle->BattleType = battleRequest->RequestType;
            petBattle->PveBattleType = PVE_BATTLE_PET_TRAINER;

            player->_petBattleId = petBattle->ID;

            sPetBattleSystem->RemoveRequest(battleRequest->RequesterGuid);

            float MovementSpeed = player->GetSpeed(UnitMoveType::MOVE_RUN);
            float MovementTimeSeconds = Hoff::CalculateMovementTimeSeconds(player->GetPosition(), PlayerFightPos, MovementSpeed);
            uint32 MovementTimeMs = static_cast<uint32>(MovementTimeSeconds * 1000.f);

            player->GetMotionMaster()->MovePoint(0xA42BA70B, PlayerFightPos, true, PlayerFightPos.GetOrientation());
            me->AddDelayedEvent(MovementTimeMs + 250, [this, petBattle]()
                 {
                     if (petBattle)
                     {
                         petBattle->Begin();
                     }
                 });

             for (size_t i = 0; i < MAX_PET_BATTLE_SLOTS; ++i)
             {
                 if (playerPets[i])
                     playerPets[i] = nullptr;

                 if (wildBattlePets[i])
                     wildBattlePets[i] = nullptr;
             }
        }
        else
        {
            CloseGossipMenuFor(player);
        }

        return true;
    }

protected:

    bool bIsTrainer;
    std::set<uint32> questIDs{};
};

void AddCustom_npc()
{
    new npc_coldheart_interstitia_175026();
    new item_challenge_key();
    new npc_expedition_map();
    new npc_elder_kuruti();
    new npc_Skarukaru_197288();
    RegisterCreatureAI(npc_PetBattleTrainer);

    /// Items
    new item_thonks_spyglass_52514();
}
