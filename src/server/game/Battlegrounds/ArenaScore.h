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

#ifndef TRINITY_ARENA_SCORE_H
#define TRINITY_ARENA_SCORE_H

#include "BattlegroundScore.h"

//DekkCore
#include "Util.h"
//DEkkCore

struct TC_GAME_API ArenaScore : public BattlegroundScore
{
    friend class Arena;

    protected:
        ArenaScore(ObjectGuid playerGuid, uint32 team);

        void BuildPvPLogPlayerDataPacket(WorldPackets::Battleground::PVPMatchStatistics::PVPMatchPlayerStatistics& playerData) const override;

        // For Logging purpose
        std::string ToString() const override;

        uint32 PreMatchRating = 0;
        uint32 PreMatchMMR = 0;
        uint32 PostMatchRating = 0;
        uint32 PostMatchMMR = 0;
};

struct TC_GAME_API ArenaTeamScore
{
    friend class Arena;
    friend class Battleground;

    protected:
        ArenaTeamScore();
        virtual ~ArenaTeamScore();

        void Assign(uint32 preMatchRating, uint32 postMatchRating, uint32 preMatchMMR, uint32 postMatchMMR);

        uint32 PreMatchRating = 0;
        uint32 PostMatchRating = 0;
        uint32 PreMatchMMR = 0;
        uint32 PostMatchMMR = 0;
};

//DekkCore
namespace ArenaHelper
{
    const float g_PvpMinCPPerWeek = 1500.f;
    const float g_PvpMaxCPPerWeek = 3000.f;
    const float g_PvpCPNumerator = 1511.26f;
    const float g_PvpCPBaseCoefficient = 1639.28f;
    const float g_PvpCPExpCoefficient = 0.00412f;

    inline float CalculateRatingFactor(int rating)
    {
        return g_PvpCPNumerator / (expf(rating * g_PvpCPExpCoefficient * -1.f) * g_PvpCPBaseCoefficient + 1.0f);
    }

    inline uint32 GetConquestCapFromRating(int rating)
    {
        if (!g_PvpMinCPPerWeek)
            return 0;

        float minRatingFactor = CalculateRatingFactor(1500);
        float maxRatingFactor = CalculateRatingFactor(3000);
        float currentRatingFactor = CalculateRatingFactor(rating);
        float normalizedRatingFactor = RoundToInterval(currentRatingFactor, minRatingFactor, maxRatingFactor);

        return g_PvpMinCPPerWeek + floor(((normalizedRatingFactor - minRatingFactor) / (maxRatingFactor - minRatingFactor)) * (g_PvpMaxCPPerWeek - g_PvpMinCPPerWeek));
    }

  /*  inline uint8 GetSlotByType(uint32 type)
    {
        switch (type)
        {
        case ARENA_TEAM_2v2: return SLOT_ARENA_2V2;
        case ARENA_TEAM_3v3: return SLOT_ARENA_3V3;
        case ARENA_TEAM_5v5: return SLOT_ARENA_5V5;
        default:
            break;
        }
        return 0xFF;
    }

    inline ArenaType GetTypeBySlot(uint8 slot)
    {
        switch (slot)
        {
        case 0:
            return ArenaType::Arena2v2;
        case 1:
            return ArenaType::Arena3v3;
        case 2:
            return ArenaType::Arena5v5;
        default:
            break;
        }
        return ArenaType::ArenaNone;
    }*/

    inline float GetChanceAgainst(uint32 ownRating, uint32 opponentRating)
    {
        // Returns the chance to win against a team with the given rating, used in the rating adjustment calculation
        // ELO system
        return 1.0f / (1.0f + exp(log(10.0f) * (float)((float)opponentRating - (float)ownRating) / 650.0f));
    }

    inline int32 GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float confidence_factor*/, bool rbg = false)
    {
        // 'Chance' calculation - to beat the opponent
        // This is a simulation. Not much info on how it really works
        float chance = GetChanceAgainst(ownRating, opponentRating);
        float won_mod = (won) ? 1.0f : 0.0f;

        // Calculate the rating modification
        float mod;

        if (rbg)
        {
            if (won && ownRating < 1500)
                mod = 192.0f * (won_mod - chance);
            else
                mod = 24.0f * (won_mod - chance);

            return (int32)ceil(mod);
        }

        // TODO: Replace this hack with using the confidence factor (limiting the factor to 2.0f)
        if (won && ownRating < 1300)
        {
            if (ownRating < 1000)
                mod = 48.0f * (won_mod - chance);
            else
                mod = (24.0f + (24.0f * (1300.0f - float(ownRating)) / 300.0f)) * (won_mod - chance);
        }
        else
            mod = 24.0f * (won_mod - chance);

        return (int32)ceil(mod);
    }

    inline int32 GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float& confidence_factor*/)
    {
        // 'Chance' calculation - to beat the opponent
        // This is a simulation. Not much info on how it really works
        float chance = GetChanceAgainst(ownRating, opponentRating);
        float won_mod = (won) ? 1.0f : 0.0f;
        float mod = won_mod - chance;

        // Work in progress:
        /*
        // This is a simulation, as there is not much info on how it really works
        float confidence_mod = min(1.0f - fabs(mod), 0.5f);
        // Apply confidence factor to the mod:
        mod *= confidence_factor
        // And only after that update the new confidence factor
        confidence_factor -= ((confidence_factor - 1.0f) * confidence_mod) / confidence_factor;
        */

        // Real rating modification
        mod *= 24.0f;

        return (int32)ceil(mod);
    }
};
//DekkCore

#endif // TRINITY_ARENA_SCORE_H
