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

#ifndef TRINITYCORE_CRAFTING_PACKETS_COMMON_H
#define TRINITYCORE_CRAFTING_PACKETS_COMMON_H

#include "ItemPacketsCommon.h"
#include "ObjectGuid.h"

namespace WorldPackets::Crafting
{
struct SpellReducedReagent
{
    int32 ItemID = 0;
    int32 Quantity = 0;
};

struct CraftingData
{
    int32 CraftingQualityID = 0;
    float QualityProgress = 0.0f;
    int32 SkillLineAbilityID = 0;
    int32 CraftingDataID = 0;
    int32 Multicraft = 0;
    int32 SkillFromReagents = 0;
    int32 Skill = 0;
    int32 CritBonusSkill = 0;
    float field_1C = 0.0f;
    uint64 field_20 = 0;
    bool IsCrit = false;
    bool field_29 = false;
    bool field_2A = false;
    bool BonusCraft = false;
    std::vector<SpellReducedReagent> ResourcesReturned;
    uint32 OperationID = 0;
    ObjectGuid ItemGUID;
    int32 Quantity = 0;
    Item::ItemInstance OldItem;
    Item::ItemInstance NewItem;
    int32 EnchantID = 0;
};

//DekkCore
class CraftingHouseHelloResponse final : public ServerPacket
{
public:
    CraftingHouseHelloResponse() : ServerPacket(SMSG_CRAFTING_HOUSE_HELLO_RESPONSE) { }

    WorldPacket const* Write() override;

    ObjectGuid NpcGuid;
    uint8 enabled;
};

struct CraftingOrderClientContext
{
    uint8 OrderType = 0;
    uint32 Offset = 0;
    uint8 ForCrafter;
    uint8 IsMyOrders;
    uint8 field_3;
    uint8 field_4;
};

struct CraftingOrderBucketInfo
{
    uint8 SkillLineAbilityID;
    int32 NumAvailable;
    uint64 TipAmountMax;
    uint64 TipAmountAvg;
};

class CraftingOrderListOrdersResponse final : public ServerPacket
{
public:
    CraftingOrderListOrdersResponse() : ServerPacket(SMSG_CRAFTING_ORDER_LIST_ORDERS_RESPONSE) { }

    WorldPacket const* Write() override;

    uint8 Result;
    uint32 bucketCount;
    uint32 orderCount;
    uint32 DesiredDelay;
    uint32 NumOrders;
    bool HasMoreResults;
    bool IsSorted;
};

class CraftingOrderUpdateState final : public ServerPacket
{
public:
    CraftingOrderUpdateState() : ServerPacket(SMSG_CRAFTING_ORDER_UPDATE_STATE) { }

    WorldPacket const* Write() override;

    int64 unk;
    uint8 unk2;
    uint8 unk3;
    ObjectGuid CrafterGUID;
    ObjectGuid PersonalCrafterGUID;
    int32 unk4;
    int32 unk5;
    uint8 unk6;
};

class CraftingOrderFulfillResult final : public ServerPacket
{
public:
    CraftingOrderFulfillResult() : ServerPacket(SMSG_CRAFTING_ORDER_FULFILL_RESULT) { }

    WorldPacket const* Write() override;

    uint8 unk;
    int64 unk2;
    int64 unk3;
    uint8 unk4;
    ObjectGuid CrafterGUID;
    int32 unk5;
    int32 unk6;
    int32 unk7;
};

class UpdateCraftingNpcRecipes final : public ClientPacket
{
public:
    UpdateCraftingNpcRecipes(WorldPacket&& packet) : ClientPacket(CMSG_UPDATE_CRAFTING_NPC_RECIPES, std::move(packet)) { }

    void Read() override;

    ObjectGuid NpcGuid;
};
//DekkCore   
ByteBuffer& operator<<(ByteBuffer& data, SpellReducedReagent const& spellReducedReagent);
ByteBuffer& operator<<(ByteBuffer& data, CraftingData const& craftingData);
}
#endif // TRINITYCORE_CRAFTING_PACKETS_COMMON_H
