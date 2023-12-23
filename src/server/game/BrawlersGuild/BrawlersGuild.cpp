/*
 * Copyright DekkCore
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

#include "Common.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "World.h"
#include "BrawlersGuild.h"
#include "Chat.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "MotionMaster.h"
#include "TemporarySummon.h"
#include "Hoff.h" // < Hoff

uint32 BrawlersBoss[MAX_BRAWLERS_RANK][BOSS_PER_RANK] =
{
    {NPC_OSO, NPC_GRANDPA, NPC_OOLISS, NPC_ULRICH},
    {NPC_DIPPY, NPC_BILL_THE_JANITOR, NPC_SANORIAK, NPC_MASTER_PAKU},
    {NPC_DOOMFLIPPER, NPC_SPLAT, NPC_SHADOWMASTER, NPC_JOHNNY},
    {NPC_BURNSTACHIO, NPC_MEATBALL, NPC_MAX_MEHABLAST, NPC_STITCHES},
    {NPC_THWACK_U, NPC_RAZORGRIN, NPC_FIVE_GNOMES, NPC_BLACKMANGE},
    {NPC_TOPPS, NPC_MILLIE, NPC_CARL, NPC_OGREWATCH},
    {NPC_NIBBLEH, NPC_SERPENT, NPC_EPICUS, NPC_RAYD},
    {0, 0, 0, 0},
};

uint32 BrawlersFaction[MAX_BRAWLERS_GUILDS] =
{
    2011, 2010
};

uint32 BrawlersSound[MAX_BRAWLERS_GUILDS][MAX_BRAWLERS_SOUNDS] =
{
    {34746, 34745},
    {34747, 34744}
};

uint32 BrawlersFightAmbiance[MAX_BRAWLERS_GUILDS] =
{
    131338, 136329
};

enum EBrawlerAudienceReaction
{
    BRAWLER_AUDIENCE_REACTION_POSITIVE,
    BRAWLER_AUDIENCE_REACTION_NEGATIVE,
    BRAWLER_AUDIENCE_REACTION_WIN,
    BRAWLER_AUDIENCE_REACTION_LOSS
};

uint32 BrawlerAudienceReaction[4][MAX_BRAWLERS_GUILDS] =
{
    {34757, 34761}, // BRAWLER_REACTION_POSITIVE
    {34756, 34760}, // BRAWLER_REACTION_NEGATIVE
    {34758, 34762}, // BRAWLER_REACTION_WIN
    {34755, 34759} // BRAWLER_REACTION_LOSS
};


Position const BrawlersTeleportLocations[] =
{
    {-107.07f, 2500.48f, -49.10f, 3.19f}, // start alliance
    {-88.36f, 2471.78f, -43.11f, 2.08f}, // end alliance
    {2032.42f, -4764.95f, 86.77f, 1.57f}, // start horde
    {1996.72f, -4767.74f, 86.77f, 0.45f},// end horde

    {-132.49f, 2499.12f, -49.10f, 0.01f}, // boss alliance
    {2030.56f, -4739.14f, 86.77f, 4.71f}, // boss horde

    {-140.43f, 2516.33f, -49.10f, 6.24f}, // SPECIAL: Nibleah A
    {2049.56f, -4734.45f, 86.76f, 4.71f}  // SPECIAL: Nibleah H
};

BrawlersGuild::BrawlersGuild(uint32 id, Map* map)
{
    _ID = id;
    _brawlState = BRAWL_STATE_WAITING;
    _prepareCombatTimer.SetCurrent(0);
    _prepareCombatTimer.SetInterval(0);
    _combatTimer.SetCurrent(0);
    _combatTimer.SetInterval(0);
    _transitionTimer.SetCurrent(0);
    _transitionTimer.SetInterval(0);
    m_map = map;
}

BrawlersGuild::~BrawlersGuild() = default;

void BrawlersGuild::Update(uint32 diff)
{
    for (auto& it : _waitList)
        if (!ObjectAccessor::FindPlayer(it))
            RemovePlayer(it);

    if (!ObjectAccessor::FindPlayer(_current))
    {
        EndCombat(false);
    }
    else
    {
        CheckBossState();
    }

    auto needUpdateAura = !_removeList.empty();

    RemovePlayers();

    _removeList.clear();

    if (needUpdateAura)
        UpdateAllAuras();

    UpdateBrawl(diff);
}

void BrawlersGuild::AddPlayer(Player* player)
{
    if (!player)
        return;
    if (GetPlayerRank(player) > 7)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("Your rank very high! Now realeased only rank 1,2,3, 4, 5, 6, 7");
        return;
    }

    _waitList.push_back(player->GetGUID());
    UpdateAura(player, _waitList.size());
}

void BrawlersGuild::RemovePlayers()
{
    for (auto& it : _removeList)
    {
        _waitList.remove(it);
        if (auto player = ObjectAccessor::FindPlayer(it))
            player->RemoveAura(SPELL_QUEUED_FOR_BRAWL);
    }
}

void BrawlersGuild::RemovePlayer(Player* player)
{
    if (player)
        RemovePlayer(player->GetGUID());
}

void BrawlersGuild::RemovePlayer(ObjectGuid guid)
{
    _removeList.push_back(guid);
}

void BrawlersGuild::UpdateAura(Player* player, uint32 rank)
{
    if (!player)
        return;

    if (!player->HasAura(SPELL_QUEUED_FOR_BRAWL))
        player->CastSpell(player, SPELL_QUEUED_FOR_BRAWL, true);

    if (auto aura = player->GetAura(SPELL_QUEUED_FOR_BRAWL))
        if (auto eff = aura->GetEffect(EFFECT_0))
            eff->SetAmount(rank);
}

void BrawlersGuild::UpdateAllAuras()
{
    uint32 rank = 1;
    for (auto& it : _waitList)
    {
        if (auto player = ObjectAccessor::FindPlayer(it))
        {
            UpdateAura(player, rank);
            ++rank;
        }
        else
            RemovePlayer(it);
    }
}

void BrawlersGuild::UpdateBrawl(uint32 diff)
{
    _prepareCombatTimer.Update(diff);
    _combatTimer.Update(diff);
    _transitionTimer.Update(diff);

    switch (_brawlState)
    {
    case BRAWL_STATE_WAITING:
        if (!_waitList.empty())
        {
            _current = _waitList.front();
            RemovePlayer(_current);

            if (Player* player = ObjectAccessor::FindPlayer(_current))
            {
                player->RemoveAura(SPELL_QUEUED_FOR_BRAWL);
                player->TeleportTo(player->GetMapId(), BrawlersTeleportLocations[player->GetTeamId() == TEAM_ALLIANCE ? 0 : 2]);

                player->AddDelayedEvent(25, [player]() -> void
                    {
                        player->CastSpell(player, SPELL_ARENA_TELEPORTATION, true);
                    });

                player->PlayDirectSound(BrawlerAudienceReaction[BRAWLER_AUDIENCE_REACTION_POSITIVE][_ID]);


                _prepareCombatTimer.SetCurrent(0);
                _prepareCombatTimer.SetInterval(6000);
                SetBrawlState(BRAWL_STATE_PREPARE_COMBAT);

                if (auto ann = Hoff::FindMapCreature(m_map, _announcer))
                {
                    PlayFightSound(true);

                    ann->GetMotionMaster()->Clear();
                    ann->GetMotionMaster()->MovePath((player->GetTeamId() == TEAM_ALLIANCE ? 11854504 : 11854503), false);
                    uint32 repRank = GetPlayerRank(player);
                    //ObjectGuid player_guid = player->GetGUID();

                    ann->AddDelayedEvent(1000, [ann, repRank, player]() -> void
                        {
                            ann->AI()->Talk(repRank, player);
                        });
                }

            }
        }
        break;
    case BRAWL_STATE_PREPARE_COMBAT:
        if (_prepareCombatTimer.Passed())
        {
            if (Player* player = ObjectAccessor::FindPlayer(_current))
            {
                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_ARENA_FORCE_REACTION, DIFFICULTY_NONE))
                {
                    if (Aura* FoundAura = player->GetAura(SPELL_ARENA_FORCE_REACTION))
                    {
                        player->RemoveAura(FoundAura);
                    }
                    player->AddAura(spellInfo, MAX_EFFECT_MASK, player);
                }

                if (auto entry = GetBossForPlayer(player))
                {
                    uint8 select_points = (player->GetTeamId() == TEAM_ALLIANCE ? 4 : 5);

                    if (entry == NPC_NIBBLEH) // hack
                        select_points += 2;

                    Position BossPos = BrawlersTeleportLocations[select_points];
                    if (TempSummon* summon = player->SummonCreature(entry, BossPos.GetPositionX(), BossPos.GetPositionY(), BossPos.GetPositionZ(), BossPos.GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120s, player->GetGUID()))
                    {
                        //summon->SetOwnerGUID(player->GetGUID());
                        _boss = summon->GetGUID();
                        _combatTimer.SetCurrent(0);
                        _combatTimer.SetInterval(120000);
                        SetBrawlState(BRAWL_STATE_COMBAT);

                        if (auto ann = Hoff::FindMapCreature(m_map, _announcer))
                        {
                            if (summon)
                            {
                                ann->AI()->SetData(summon->GetEntry(), 0);
                            }
                        }

                        player->PlayDirectSound(BrawlerAudienceReaction[BRAWLER_AUDIENCE_REACTION_POSITIVE][_ID]);
                        summon->CastSpell(summon, 132633);
                        return;
                    }
                }
            }
            _prepareCombatTimer.SetCurrent(0);

            EndCombat(false);
        }
        break;
    case BRAWL_STATE_COMBAT:
        if (_combatTimer.Passed())
        {
            _combatTimer.SetCurrent(0);
            EndCombat(false, true);
        }
        break;
    case BRAWL_STATE_TRANSITION:
        if (_transitionTimer.Passed())
        {
            _transitionTimer.SetCurrent(0);
            SetBrawlState(BRAWL_STATE_WAITING);
        }
        break;
    default:
        break;
    }
}

void BrawlersGuild::EndCombat(bool win, bool time)
{
    PlayFightSound(false);

    if (Player* player = ObjectAccessor::FindPlayer(_current))
    {
        if (!player->IsAlive())
        {
            player->CreateCorpse();
            player->SpawnCorpseBones(false);

            player->TeleportTo(player->GetMapId(), BrawlersTeleportLocations[player->GetTeamId() == TEAM_ALLIANCE ? 1 : 3]);

            player->AddDelayedEvent(25, [player]() -> void
                {
                    player->CastSpell(player, SPELL_ARENA_TELEPORTATION, true);
                });

            player->AddDelayedEvent(2000, [player]() -> void
                {
                    if (!player->IsAlive())
                    {
                        player->ResurrectPlayer(1.0f);
                    }

                    if (Aura* FoundAura = player->GetAura(SPELL_ARENA_FORCE_REACTION))
                    {
                        player->RemoveAura(FoundAura);
                    }

                    if (Aura* FoundAura = player->GetAura(SPELL_MOVE_FORWARD))
                    {
                        player->RemoveAura(FoundAura);
                    }
                });
        }
        else
        {
            player->AddDelayedEvent(2000, [player]() -> void
                {
                    player->TeleportTo(player->GetMapId(), BrawlersTeleportLocations[player->GetTeamId() == TEAM_ALLIANCE ? 1 : 3]);

                    if (Aura* FoundAura = player->GetAura(SPELL_ARENA_FORCE_REACTION))
                    {
                        player->RemoveAura(FoundAura);
                    }

                    if (Aura* FoundAura = player->GetAura(SPELL_MOVE_FORWARD))
                    {
                        player->RemoveAura(FoundAura);
                    }
                });

            player->AddDelayedEvent(2025, [player]() -> void
                {
                    player->CastSpell(player, SPELL_ARENA_TELEPORTATION, true);
                });
        }

        _transitionTimer.SetCurrent(0);
        _transitionTimer.SetInterval(5000);

        if (win)
        {
            player->PlayDirectSound(BrawlerAudienceReaction[BRAWLER_AUDIENCE_REACTION_WIN][_ID]);

            player->PlayDirectSound(BrawlersSound[_ID][VICTORY]);
            player->AddItem(92718, 1);

            auto rep = player->GetReputation(BrawlersFaction[player->GetTeamId()]) + REPUTATION_PER_RANK;
            if (rep > MAX_BRAWLERS_REPUTATION)
                rep = MAX_BRAWLERS_REPUTATION;

            uint32 oldRank = GetPlayerRank(player);
            player->SetReputation(BrawlersFaction[player->GetTeamId()], rep);
            if (oldRank < GetPlayerRank(player))
            {
                SendCongratulations(player);
                switch (GetPlayerRank(player))
                {
                case 3: // 4
                    if (player->GetTeamId() == TEAM_ALLIANCE && !player->HasAchieved(ACHIEVEMENT_RANK_FOUR_A))
                        if (auto achievementEntry = sAchievementStore.LookupEntry(ACHIEVEMENT_RANK_FOUR_A))
                            player->CompletedAchievement(achievementEntry);

                    if (player->GetTeamId() == TEAM_HORDE && !player->HasAchieved(ACHIEVEMENT_RANK_FOUR_H))
                        if (auto achievementEntry = sAchievementStore.LookupEntry(ACHIEVEMENT_RANK_FOUR_H))
                            player->CompletedAchievement(achievementEntry);
                    break;
                case 7: // 8
                    if (player->GetTeamId() == TEAM_ALLIANCE && !player->HasAchieved(ACHIEVEMENT_RANK_EIGHT_A))
                        if (auto achievementEntry = sAchievementStore.LookupEntry(ACHIEVEMENT_RANK_EIGHT_A))
                            player->CompletedAchievement(achievementEntry);

                    if (player->GetTeamId() == TEAM_HORDE && !player->HasAchieved(ACHIEVEMENT_RANK_EIGHT_H))
                        if (auto achievementEntry = sAchievementStore.LookupEntry(ACHIEVEMENT_RANK_EIGHT_H))
                            player->CompletedAchievement(achievementEntry);
                    break;
                default:
                    break;
                }
            }

            if (player->GetTeamId() == TEAM_ALLIANCE && !player->HasAchieved(ACHIEVEMENT_WIN_BRAWL_A))
                if (auto achievementEntry = sAchievementStore.LookupEntry(ACHIEVEMENT_WIN_BRAWL_A))
                    player->CompletedAchievement(achievementEntry);

            if (player->GetTeamId() == TEAM_HORDE && !player->HasAchieved(ACHIEVEMENT_WIN_BRAWL_H))
                if (auto achievementEntry = sAchievementStore.LookupEntry(ACHIEVEMENT_WIN_BRAWL_H))
                    player->CompletedAchievement(achievementEntry);

            if (auto ann = Hoff::FindMapCreature(m_map, _announcer))
            {
                ann->GetMotionMaster()->Clear();
                ann->GetMotionMaster()->MovePath((player->GetTeamId() == TEAM_ALLIANCE ? 11854502 : 11854503), true);
                ann->AI()->Talk(YELL_BIZMO_PLAYER_WIN, player);
            }
        }
        else
        {
            player->PlayDirectSound(BrawlerAudienceReaction[BRAWLER_AUDIENCE_REACTION_LOSS][_ID]);
            player->PlayDirectSound(BrawlersSound[_ID][DEFEAT]);

            if (auto ann = Hoff::FindMapCreature(m_map, _announcer))
            {
                ann->GetMotionMaster()->Clear();
                ann->GetMotionMaster()->MovePath((player->GetTeamId() == TEAM_ALLIANCE ? 11854502 : 11854503), true);
                ann->AI()->Talk(YELL_BIZMO_PLAYER_LOSS, player);
            }
        }

        if (time)
        {
            player->CastSpell(player, SPELL_EXPLOSION_0, true);
            player->CastSpell(player, SPELL_EXPLOSION_1, true);
            player->CastSpell(player, SPELL_EXPLOSION_2, true);
            player->CastSpell(player, SPELL_EXPLOSION_3, true);
            //player->KillSelf(player);
        }
        player->SaveToDB();
    }

    _current.Clear();

    if (auto cboss = Hoff::FindMapCreature(m_map, _boss))
    {
        cboss->AI()->JustDied(nullptr); // for remove adds, AT and some

        cboss->AddDelayedEvent(2000, [cboss]() -> void
            {
                cboss->DespawnOrUnsummon();
            });
    }

    _boss.Clear();

    if (_brawlState == BRAWL_STATE_COMBAT)
    {
        SetBrawlState(BRAWL_STATE_TRANSITION);
    }
}

uint32 BrawlersGuild::GetPlayerRank(Player* player)
{
    if (player)
        return player->GetReputation(BrawlersFaction[player->GetTeamId()]) / (REPUTATION_PER_RANK * BOSS_PER_RANK);

    return 0;
}

uint32 BrawlersGuild::GetPlayerSubRank(Player* player)
{
    if (player)
        return (player->GetReputation(BrawlersFaction[player->GetTeamId()]) / REPUTATION_PER_RANK) % BOSS_PER_RANK;

    return 0;
}

uint32 BrawlersGuild::GetBossForPlayer(Player* player)
{
    if (!player)
        return 0;

    auto rank = GetPlayerRank(player);
    auto subrank = GetPlayerSubRank(player);

    if (rank < MAX_BRAWLERS_RANK && subrank < BOSS_PER_RANK)
        return BrawlersBoss[rank][subrank];

    return 0;
}

void BrawlersGuild::CheckBossState()
{
    if (Creature* cboss = Hoff::FindMapCreature(m_map, _boss))
    {
        if (!cboss->IsAlive())
        {
            BossReport(_current, true);
            return;
        }
    }
    if (Player* CurrentPlayer = ObjectAccessor::FindPlayer(_current))
    {
        if (!CurrentPlayer->IsAlive())
        {
            BossReport(_current, false);
            return;
        }
    }
}

void BrawlersGuild::BossReport(ObjectGuid guid, bool win)
{
    if (_current == guid && _brawlState == BRAWL_STATE_COMBAT)
    {
        EndCombat(win);
    }
}

bool BrawlersGuild::IsPlayerInBrawl(Player* player)
{
    if (!player || !_current)
        return false;

    return player->GetGUID() == _current;
}

void BrawlersGuild::SetBrawlState(uint32 state)
{
    _brawlState = state;
}

void BrawlersGuild::SetAnnouncer(ObjectGuid guid)
{
    _announcer = guid;
}

void BrawlersGuild::PlayFightSound(bool play)
{
    if (auto ann = Hoff::FindMapCreature(m_map, _announcer))
    {
        if (play)
        {
            ann->CastSpell(ann, BrawlersFightAmbiance[_ID], true);
        }
        else
            ann->RemoveAura(BrawlersFightAmbiance[_ID]);
    }
}

uint32 BrawlersGuild::GetPlayerPosition(Player* player)
{
    if (!player)
        return 0;

    uint32 i = 1;
    for (auto& it : _waitList)
    {
        if (player->GetGUID() == it)
            return i;
        ++i;
    }

    return 0;
}

void BrawlersGuild::SendCongratulations(Player* player)
{
    if (!player)
        return;

    std::list<Creature*> fanats;
    GetCreatureListWithEntryInGrid(fanats, player, (player->GetTeamId() == TEAM_ALLIANCE ? 68992 : 68404), 70.0f);
    GetCreatureListWithEntryInGrid(fanats, player, (player->GetTeamId() == TEAM_ALLIANCE ? 68994 : 68403), 70.0f);
    GetCreatureListWithEntryInGrid(fanats, player, (player->GetTeamId() == TEAM_ALLIANCE ? 68406 : 68990), 70.0f);
    if (fanats.empty())
        return;

    for (auto& fanat : fanats)
        if (urand(1, 3) == 2)
            fanat->AI()->Talk(1);

}
