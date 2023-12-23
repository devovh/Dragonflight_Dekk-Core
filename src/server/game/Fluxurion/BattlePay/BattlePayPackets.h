/*
 * Copyright 2023 Fluxurion
 */


#ifndef BattlePayPackets_h__
#define BattlePayPackets_h__

#include "Packet.h"
#include "ItemPackets.h"
#include "WorldSession.h"

namespace WorldPackets
{
    namespace BattlePay
    {
        struct Visual
        {
            std::string Name;
            uint32 DisplayId;
            uint32 VisualId;
            uint32 Unk;
        };

        struct DisplayInfo
        {
            Optional<uint32> CreatureDisplayID;
            Optional<uint32> VisualID;
            std::string Name1;
            std::string Name2;
            std::string Name3;
            std::string Name4;
            std::string Name5;
            std::string Name6;
            std::string Name7;
            Optional<uint32> Flags;
            Optional<uint32> Unk1;
            Optional<uint32> Unk2;
            Optional<uint32> Unk3; // UiTextureAtlasMemberID
            uint32 UnkInt1;
            uint32 UnkInt2;
            uint32 UnkInt3;

            std::vector<Visual> Visuals;
        };

        struct ProductInfo
        {
            uint32 ProductId;
            uint64 NormalPriceFixedPoint;
            uint64 CurrentPriceFixedPoint;
            std::vector<uint32> ProductIds;
            uint32 Unk1;
            uint32 Unk2;
            std::vector<uint32> UnkInts;
            uint32 Unk3;
            uint32 ChoiceType;
            Optional<DisplayInfo> Display;
        };

        struct ProductItem
        {
            uint32 ID;
            uint8 UnkByte;
            uint32 ItemID;
            uint32 Quantity;
            uint32 UnkInt1;
            uint32 UnkInt2;
            bool IsPet;
            Optional<uint32> PetResult;
            Optional<DisplayInfo> Display;
        };

        struct Product
        {
            uint32 ProductId;
            uint8 Type;
            uint32 Flags; // ItemEntry
            uint32 Unk1; // ItemCount
            uint32 DisplayId; // MountSpellID
            uint32 ItemId; // BattlePetCreatureID
            uint32 Unk4;
            uint32 Unk5;
            uint32 Unk6;
            uint32 Unk7; // TransmogSetID
            uint32 Unk8;
            uint32 Unk9;
            std::string UnkString;
            bool UnkBit;
            Optional<uint32> UnkBits;
            std::vector<ProductItem> Items;
            Optional<DisplayInfo> Display;
        };

        struct Group
        {
            uint32 GroupId;
            uint32 IconFileDataID;
            uint8 DisplayType;
            uint32 Ordering;
            uint32 Unk;
            uint32 MainGroupID;
            std::string Name;
            std::string Description;
        };

        struct Shop
        {
            uint32 EntryID;
            uint32 GroupID;
            uint32 ProductID;
            uint32 Ordering;
            uint32 VasServiceType;
            uint8 StoreDeliveryType;
            Optional<DisplayInfo> Display;
        };

        struct DistributionObject
        {
            Optional<Product> Product;
            ObjectGuid TargetPlayer;
            uint64 DistributionID = 0;
            uint64 PurchaseID = 0;
            uint32 Status = 0;
            uint32 ProductID = 0;
            uint32 TargetVirtualRealm = 0;
            uint32 TargetNativeRealm = 0;
            bool Revoked = false;
        };

        struct Purchase
        {
            uint64 PurchaseID = 0;
            uint64 UnkLong = 0;
            uint64 UnkLong2 = 0;
            uint32 Status = 0;
            uint32 ResultCode = 0;
            uint32 ProductID = 0;
            uint32 UnkInt = 0;
            std::string WalletName;
        };

        struct VasPurchase
        {
            std::vector<uint32> ItemIDs;
            ObjectGuid PlayerGuid;
            uint64 UnkLong = 0;
            uint32 UnkInt = 0;
            uint32 UnkInt2 = 0;
        };


        class GetProductList final : public ClientPacket
        {
        public:
            GetProductList(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_GET_PRODUCT_LIST, std::move(packet)) { }

            void Read() override { }
        };

        class GetPurchaseListQuery final : public ClientPacket
        {
        public:
            GetPurchaseListQuery(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_GET_PURCHASE_LIST, std::move(packet)) { }

            void Read() override { }
        };

        class UpdateVasPurchaseStates final : public ClientPacket
        {
        public:
            UpdateVasPurchaseStates(WorldPacket&& packet) : ClientPacket(CMSG_UPDATE_VAS_PURCHASE_STATES, std::move(packet)) { }

            void Read() override { }
        };

        class PurchaseListResponse final : public ServerPacket
        {
        public:
            PurchaseListResponse() : ServerPacket(SMSG_BATTLE_PAY_GET_PURCHASE_LIST_RESPONSE, 8) { }

            WorldPacket const* Write() override;

            uint32 Result = 0;
            std::vector<Purchase> Purchase;
        };

        class SyncWowEntitlements final : public ServerPacket
        {
        public:
            SyncWowEntitlements() : ServerPacket(SMSG_SYNC_WOW_ENTITLEMENTS) { }

            WorldPacket const* Write() override;

            std::vector<uint32> purchaseCount;
            std::vector<uint32> productCount;
            std::vector<Product> Product;
        };

        class DistributionListResponse final : public ServerPacket
        {
        public:
            DistributionListResponse() : ServerPacket(SMSG_BATTLE_PAY_GET_DISTRIBUTION_LIST_RESPONSE, 8) { }

            WorldPacket const* Write() override;

            uint32 Result = 0;
            std::vector<DistributionObject> DistributionObject;
        };

        class EnumVasPurchaseStatesResponse final : public ServerPacket
        {
        public:
            EnumVasPurchaseStatesResponse() : ServerPacket(SMSG_ENUM_VAS_PURCHASE_STATES_RESPONSE, 4) { }

            WorldPacket const* Write() override;

            uint8 Result = 0;
        };

        class DistributionUpdate final : public ServerPacket
        {
        public:
            DistributionUpdate() : ServerPacket(SMSG_BATTLE_PAY_DISTRIBUTION_UPDATE, 100) { }

            WorldPacket const* Write() override;

            DistributionObject DistributionObject;
        };

        class ProductListResponse final : public ServerPacket
        {
        public:
            ProductListResponse() : ServerPacket(SMSG_BATTLE_PAY_GET_PRODUCT_LIST_RESPONSE, 4000) { }

            WorldPacket const* Write() override;

            uint32 Result = 0;
            uint32 CurrencyID = 0;
            std::vector<ProductInfo> ProductInfos;
            std::vector<Product> Products;
            std::vector<Group> ProductGroups;
            std::vector<Shop> Shops;
        };

        class StartPurchase final : public ClientPacket
        {
        public:
            StartPurchase(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_START_PURCHASE, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetCharacter;
            uint32 ClientToken = 0;
            uint32 ProductID = 0;
        };

        class StartPurchaseResponse final : public ServerPacket
        {
        public:
            StartPurchaseResponse() : ServerPacket(SMSG_BATTLE_PAY_START_PURCHASE_RESPONSE, 16) { }

            WorldPacket const* Write() override;

            uint64 PurchaseID = 0;
            uint32 ClientToken = 0;
            uint32 PurchaseResult = 0;
        };

        class BattlePayAckFailed final : public ServerPacket
        {
        public:
            BattlePayAckFailed() : ServerPacket(SMSG_BATTLE_PAY_ACK_FAILED, 16) { }

            WorldPacket const* Write() override;

            uint64 PurchaseID = 0;
            uint32 ClientToken = 0;
            uint32 PurchaseResult = 0;
        };

        class PurchaseUpdate final : public ServerPacket
        {
        public:
            PurchaseUpdate() : ServerPacket(SMSG_BATTLE_PAY_PURCHASE_UPDATE, 4) { }

            WorldPacket const* Write() override;

            std::vector<Purchase> Purchase;
        };

        class ConfirmPurchase final : public ServerPacket
        {
        public:
            ConfirmPurchase() : ServerPacket(SMSG_BATTLE_PAY_CONFIRM_PURCHASE, 20) { }

            WorldPacket const* Write() override;

            uint64 PurchaseID = 0;
            uint32 ServerToken = 0;
        };

        class ConfirmPurchaseResponse final : public ClientPacket
        {
        public:
            ConfirmPurchaseResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_CONFIRM_PURCHASE_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint64 ClientCurrentPriceFixedPoint = 0;
            uint32 ServerToken = 0;
            bool ConfirmPurchase = false;
        };

        class BattlePayAckFailedResponse final : public ClientPacket
        {
        public:
            BattlePayAckFailedResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_ACK_FAILED_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint32 ServerToken = 0;
        };

        class DeliveryEnded final : public ServerPacket
        {
        public:
            DeliveryEnded() : ServerPacket(SMSG_BATTLE_PAY_DELIVERY_ENDED, 12) { }

            WorldPacket const* Write() override;

            std::vector<Item::ItemInstance> item;
            uint64 DistributionID = 0;
        };

        class BattlePayDeliveryStarted final : public ServerPacket
        {
        public:
            BattlePayDeliveryStarted() : ServerPacket(SMSG_BATTLE_PAY_DELIVERY_STARTED, 8) { }

            WorldPacket const* Write() override;

            uint64 DistributionID = 0;
        };

        class UpgradeStarted final : public ServerPacket
        {
        public:
            UpgradeStarted() : ServerPacket(SMSG_CHARACTER_UPGRADE_STARTED, 16) { }

            WorldPacket const* Write() override;

            ObjectGuid CharacterGUID;
        };

        class DistributionAssignToTarget final : public ClientPacket
        {
        public:
            DistributionAssignToTarget(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_DISTRIBUTION_ASSIGN_TO_TARGET, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetCharacter;
            uint64 DistributionID = 0;
            uint32 ProductID = 0;
            uint16 SpecializationID = 0;
            uint16 ChoiceID = 0;
        };

        /* class BattlePayVasPurchaseStarted final : public ServerPacket
        {
        public:
            BattlePayVasPurchaseStarted() : ServerPacket(SMSG_BATTLE_PAY_VAS_PURCHASE_STARTED, 4 + 4 + 16 + 8 + 4 + 4) { }

            WorldPacket const* Write() override;

            VasPurchase VasPurchase;
            uint32 UnkInt = 0;
        }; */

        /* class CharacterClassTrialCreate final : public ServerPacket
        {
        public:
            CharacterClassTrialCreate() : ServerPacket(SMSG_CHARACTER_CLASS_TRIAL_CREATE, 4) { }

            WorldPacket const* Write() override;

            uint32 Result = 0;
        }; */

        /* class BattlePayQueryClassTrialResult final : public ClientPacket
        {
        public:
            BattlePayQueryClassTrialResult(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_QUERY_CLASS_TRIAL_BOOST_RESULT, std::move(packet)) { }

            void Read() override { }
        }; */

        /* class BattlePayCharacterUpgradeQueued final : public ServerPacket
        {
        public:
            BattlePayCharacterUpgradeQueued() : ServerPacket(SMSG_CHARACTER_UPGRADE_QUEUED, 4 + 16) { }

            WorldPacket const* Write() override;

            std::vector<uint32> EquipmentItems;
            ObjectGuid Character;
        }; */

        /* class BattlePayTrialBoostCharacter final : public ClientPacket
        {
        public:
            BattlePayTrialBoostCharacter(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_TRIAL_BOOST_CHARACTER, std::move(packet)) { }

            void Read() override;

            ObjectGuid Character;
            uint32 SpecializationID = 0;
        }; */

        /* class BattlePayVasPurchaseList final : public ServerPacket
        {
        public:
            BattlePayVasPurchaseList() : ServerPacket(SMSG_BATTLE_PAY_VAS_PURCHASE_LIST, 4) { }

            WorldPacket const* Write() override;

            std::vector<VasPurchase> VasPurchase;
        }; */

        /* class PurchaseDetails final : public ServerPacket
        {
        public:
            PurchaseDetails() : ServerPacket(SMSG_BATTLE_PAY_PURCHASE_DETAILS, 20) { }

            WorldPacket const* Write() override;

            uint64 UnkLong = 0;
            uint32 UnkInt = 0;
            uint32 VasPurchaseProgress = 0;
            std::string Key;
            std::string Key2;
        }; */

        /* class PurchaseUnk final : public ServerPacket
        {
        public:
            PurchaseUnk() : ServerPacket(SMSG_BATTLE_PAY_PURCHASE_UNK, 20) { }

            WorldPacket const* Write() override;

            uint32 UnkInt = 0;
            std::string Key;
            uint8 UnkByte = 0;
        }; */

        class BattlePayBattlePetDelivered final : public ServerPacket
        {
        public:
            BattlePayBattlePetDelivered() : ServerPacket(SMSG_BATTLE_PAY_BATTLE_PET_DELIVERED, 20) { }

            WorldPacket const* Write() override;

            ObjectGuid BattlePetGuid;
            uint32 DisplayID = 0;
        };

        /* class PurchaseDetailsResponse final : public ClientPacket
        {
        public:
            PurchaseDetailsResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_PURCHASE_DETAILS_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint8 UnkByte = 0;
        }; */

        /* class PurchaseUnkResponse final : public ClientPacket
        {
        public:
            PurchaseUnkResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_PURCHASE_UNK_RESPONSE, std::move(packet)) { }

            void Read() override;

            std::string Key;
            std::string Key2;
        }; */

        class DisplayPromotion final : public ServerPacket
        {
        public:
            DisplayPromotion(uint32 ID) : ServerPacket(SMSG_DISPLAY_PROMOTION, 4), PromotionID(ID) { }

            WorldPacket const* Write() override;

            uint32 PromotionID = 0;
        };

        /* class BattlepayUnk final : public ServerPacket
        {
        public:
            BattlepayUnk(uint32 unkInt) : ServerPacket(SMSG_BATTLE_PAY_UNK, 4), UnkInt(unkInt) { }

            WorldPacket const* Write() override;

            uint32 UnkInt = 0;
        }; */

        class BattlePayStartDistributionAssignToTargetResponse final : public ServerPacket
        {
        public:
            BattlePayStartDistributionAssignToTargetResponse() : ServerPacket(SMSG_BATTLE_PAY_START_DISTRIBUTION_ASSIGN_TO_TARGET_RESPONSE, 16) { }

            WorldPacket const* Write() override;

            uint64 DistributionID = 0;
            uint32 unkint1 = 0;
            uint32 unkint2 = 0;
        };

        class BattlePayRequestPriceInfo final : public ClientPacket
        {
        public:
            BattlePayRequestPriceInfo(WorldPacket&& packet) : ClientPacket(CMSG_BATTLE_PAY_REQUEST_PRICE_INFO, std::move(packet)) { }

            void Read() override { }

            uint8 UnkByte = 0;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePay::DisplayInfo const& displayInfo);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePay::Product const& product);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePay::DistributionObject const& object);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::BattlePay::Purchase const& purchase);

#endif // BattlePayPackets_h__
