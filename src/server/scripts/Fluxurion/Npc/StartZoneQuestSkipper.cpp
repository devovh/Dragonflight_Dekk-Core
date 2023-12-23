/*
 * Copyright 2023 Fluxurion
 */

#include "Creature.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"

class StartZoneQuestSkipper : public CreatureScript
{
public:
    StartZoneQuestSkipper() : CreatureScript("StartZoneQuestSkipper") { }

    struct StartZoneQuestSkipperAI : public ScriptedAI
    {
        StartZoneQuestSkipperAI(Creature* creature) : ScriptedAI(creature)
        {
            // If enabled in config make it visible if disabled make it invisible!
            if (sConfigMgr->GetBoolDefault("StartZone.QuestSkipper.Npc.Enable", false))
                me->SetVisible(true);
            else
                me->SetVisible(false);
        }

        bool OnGossipHello(Player* player) override
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GossipOptionNpc::None, "I've done this before! Can you send me further in time?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "I will send you further in time. You will get levels, quest rewards and being teleported. You wanna do this?", 0, false);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 gossipListId, uint32 menuId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);

            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->CastSpell(player, 326524, true); // oribos portal effect DNT

                // time_delay to see the fancy portal effect first
                player->GetScheduler().Schedule(2s, [this, player](TaskContext context)
                    {
                        // Demon Hunter
                        if (player->GetClass() == CLASS_DEMON_HUNTER)
                            skipdhstartzone(player);

                        // Death Knight
                        if (player->GetClass() == CLASS_DEATH_KNIGHT)
                            skipdkstartzone(player);

                        // Goblin
                        if (player->GetRace() == RACE_GOBLIN)
                            skipgoblinstartzone(player);

                        // Worgen
                        if (player->GetRace() == RACE_WORGEN)
                            skipworgenstartzone(player);

                        // Pandaren
                        if (player->GetRace() == RACE_PANDAREN_HORDE || player->GetRace() == RACE_PANDAREN_ALLIANCE || player->GetRace() == RACE_PANDAREN_NEUTRAL)
                            skippandastartzone(player);
                    });

                player->GetScheduler().Schedule(15s, [player](TaskContext context)
                    {
                        ChatHandler(player->GetSession()).SendSysMessage("|cff00ffff You traveled in time, you might feel dizzy.|r");
                        player->CastSpell(player, 320975, true); // dizzy effect :D
                    });

                CloseGossipMenuFor(player);
                break;
            }

            return true;
        }

        //
        // Demon Hunter Start Zone Skipping
        //
        void skipdhstartzone(Player* player)
        {
            player->AddItem(129192, 1);
            player->AddItem(128946, 1);
            player->AddItem(128948, 1);
            player->AddItem(133580, 1);
            player->AddItem(129195, 1);
            player->AddItem(128944, 1);
            player->AddItem(128958, 1);
            player->AddItem(128945, 1);
            player->AddItem(128956, 1);
            player->AddItem(132243, 1);
            //alliance items
            if (player->GetTeam() == ALLIANCE) {
                player->AddItem(128953, 1);
                player->AddItem(128952, 1);
                player->AddItem(128951, 1);
                player->AddItem(128947, 1);
                player->AddItem(128954, 1);
                player->AddItem(128950, 1);
                player->AddItem(128955, 1);
                player->AddItem(128949, 1);
                player->AddItem(138970, 1);
            }
            //horde items
            if (player->GetTeam() == HORDE) {
                player->AddItem(133317, 1);
                player->AddItem(133312, 1);
                player->AddItem(133313, 1);
                player->AddItem(133316, 1);
                player->AddItem(133314, 1);
                player->AddItem(133311, 1);
                player->AddItem(133310, 1);
                player->AddItem(133315, 1);
                player->AddItem(141650, 1);
            }

            // abilities without quest rewarding gain
            player->LearnSpell(200175, true);     // felsaber mount
            player->LearnSpell(188501, true);     // spectral sight
            player->LearnSpell(217832, true);     // imprison
            player->LearnSpell(196718, true);     // darkness
            player->LearnSpell(188499, true);     // bladedance
            player->LearnSpell(178740, true);     // immolation aura
            player->LearnSpell(191427, true);     // Metamorphosis
            player->SetLevel(10);
            player->UpdateSkillsForLevel();
            player->LearnDefaultSkills();

            //Rewarding the ability giver quests
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(39049)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(38766)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(39495)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(40051)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(40254)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(38723)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(39683)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            if (player->GetTeam() == ALLIANCE) {
                if (Quest const* quest = quest = sObjectMgr->GetQuestTemplate(39688)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
                if (Quest const* quest = quest = sObjectMgr->GetQuestTemplate(40255)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            }
            if (player->GetTeam() == HORDE) {
                if (Quest const* quest = quest = sObjectMgr->GetQuestTemplate(39694)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
                if (Quest const* quest = quest = sObjectMgr->GetQuestTemplate(40256)) { player->RewardQuest(quest, LootItemType::Item, 0, me); }
            }

            //adding the last quest					
            if (player->GetTeam() == ALLIANCE) {
                if (Quest const* quest = quest = sObjectMgr->GetQuestTemplate(39689))
                {
                    player->AddQuest(quest, NULL);
                    player->CompleteQuest(quest->GetQuestId());
                }
            }
            if (player->GetTeam() == HORDE) {
                if (Quest const* quest = quest = sObjectMgr->GetQuestTemplate(39690))
                {
                    player->AddQuest(quest, NULL);
                    player->CompleteQuest(quest->GetQuestId());
                }
            }
            //teleporting to khadgar for the last quest
            player->TeleportTo(1468, 4152, -874, 293, 0);
        }

        //
        // Death Knight Start Zone Skipping
        //
        void skipdkstartzone(Player* player)
        {
            player->AddItem(38632, 1);
            player->AddItem(38633, 1);
            player->AddItem(38661, 1);
            player->AddItem(38662, 1);
            player->AddItem(38663, 1);
            player->AddItem(38664, 1);
            player->AddItem(38665, 1);
            player->AddItem(38666, 1);
            player->AddItem(38667, 1);
            player->AddItem(38668, 1);
            player->AddItem(38669, 1);
            player->AddItem(38670, 1);
            player->AddItem(38670, 1);
            player->AddItem(38671, 1);
            player->AddItem(38672, 1);
            player->AddItem(38674, 1);
            player->AddItem(38675, 1);
            player->AddItem(38707, 1);
            player->AddItem(39320, 1);
            player->AddItem(39322, 1);
            player->AddItem(40483, 1);
            player->LearnSpell(33391, true);
            player->LearnSpell(48778, true);
            player->LearnSpell(50977, true);
            player->LearnSpell(53428, true);
            player->SetSkill(776, 1, 1, 1);
            player->SetLevel(10);
            player->UpdateSkillsForLevel();
            player->LearnDefaultSkills();
            player->TeleportTo(0, 2356.424561f, -5663.146973f, 382.25519f, 0.533163f);
            ChatHandler(player->GetSession()).PSendSysMessage("Lich King heard your wish!");
        }

        //
        // Goblin Start Zone Skipping
        //
        void skipgoblinstartzone(Player* player)
        {
            player->AddItem(49884, 1); // Kaja'Cola
            player->AddItem(48768, 1); // Kablooey Bombs
            player->AddItem(56148, 1); // Bank Robber's Bag
            player->LearnSpell(69046, true); // Pack Hobgoblin
            player->SetLevel(5);
            player->UpdateSkillsForLevel();
            player->LearnDefaultSkills();
            player->TeleportTo(1, 1570.62f, -4403.35f, 15.9722f, 6.15468f); // orgrimmar port loc
        }

        //
        // Worgen Start Zone Skipping
        //
        void skipworgenstartzone(Player* player)
        {
            player->AddItem(58255, 1); // Lord Walden's Top Hat
            player->AddItem(52039, 1); // Shipwright's Bag
            player->AddItem(2723, 5); // Bottle of Dalaran Noir
            player->LearnSpell(72792, true); // Learn Racials
            player->LearnSpell(72857, true); // Learn Two Forms
            player->LearnSpell(95759, true); // Learn Darkflight
            player->SetLevel(5);
            player->UpdateSkillsForLevel();
            player->LearnDefaultSkills();
            player->TeleportTo(0, -8866.73f, 592.2f, 92.8267f, 0.64961f); // stormwind port loc
        }

        //
        // Pandaren Start Zone Skipping
        //
        void skippandastartzone(Player* player)
        {
            player->AddItem(73242, 1); // Master's Haversack
            player->SetLevel(5);
            player->UpdateSkillsForLevel();
            player->LearnDefaultSkills();
            if (player->IsInAlliance())
                player->TeleportTo(0, -8866.73f, 592.2f, 92.8267f, 0.64961f); // stormwind port loc
            if (player->IsInHorde())
                player->TeleportTo(1, 1570.62f, -4403.35f, 15.9722f, 6.15468f); // orgrimmar port loc
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new StartZoneQuestSkipperAI(creature);
    }
};

void AddSC_StartZoneQuestSkipper()
{
    new StartZoneQuestSkipper();
}
