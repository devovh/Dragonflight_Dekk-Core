/*
 * Copyright 2023 Fluxurion
 */

#ifndef _BATTLE_PAY_DATA_STORE_H
#define _BATTLE_PAY_DATA_STORE_H

#include "BattlePayPackets.h"
#include "BattlePayMgr.h"

namespace BattlePayData
{
    struct ProductAddon
    {
        uint32 DisplayInfoEntry;
        uint8 DisableListing;
        uint8 DisableBuy;
        uint8 NameColorIndex;
        std::string ScriptName;
        std::string Comment;
    };

    struct Visual
    {
        uint32 Entry;
        std::string Name;
        uint32 DisplayId;
        uint32 VisualId;
        uint32 Unk;
        uint32 DisplayInfoEntry;
    };

    struct DisplayInfo
    {
        uint32 Entry; // have to be same as productinfo entry and shop entry
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
        Optional<uint32> Unk3;
        uint32 UnkInt1;
        uint32 UnkInt2;
        uint32 UnkInt3;

        std::vector<Visual> Visuals;
    };

    struct ProductInfo
    {
        uint32 Entry; // have to be same as displayinfo entry and shop entry
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
        uint32 Entry;
        uint32 ID;
        uint8 UnkByte;
        uint32 ItemID;
        uint32 Quantity;
        uint32 UnkInt1;
        uint32 UnkInt2;
        bool IsPet;
        Optional<uint32> PetResult;
        Optional<WorldPackets::BattlePay::DisplayInfo> Display;
    };

    struct Product
    {
        uint32 Entry;
        uint32 ProductId;
        uint8 Type;
        uint32 Flags;
        uint32 Unk1;
        uint32 DisplayId;
        uint32 ItemId;
        uint32 Unk4;
        uint32 Unk5;
        uint32 Unk6;
        uint32 Unk7;
        uint32 Unk8;
        uint32 Unk9;
        std::string UnkString;
        bool UnkBit;
        Optional<uint32> UnkBits;
        std::vector<ProductItem> Items;
        Optional<DisplayInfo> Display;
        std::string Name;
    };

    struct Group
    {
        uint32 Entry;
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
        uint32 Entry; // have to be same as displayinfo entry and productinfo entry
        uint32 EntryID;
        uint32 GroupID;
        uint32 ProductID;
        uint32 Ordering;
        uint32 VasServiceType;
        uint8 StoreDeliveryType;
        uint32 DisplayInfoEntry;
        Optional<DisplayInfo> Display;
    };
};

class TC_GAME_API BattlePayDataStoreMgr
{
    BattlePayDataStoreMgr();
    ~BattlePayDataStoreMgr();

public:
    static BattlePayDataStoreMgr* instance();

    void Initialize();
    std::vector<BattlePayData::Group> GetProductGroups();
    std::vector<BattlePayData::Shop> GetShopEntries();
    std::map<uint32, BattlePayData::ProductInfo> GetProductInfos();
    std::map<uint32, BattlePayData::Product> GetProducts();
    std::vector<BattlePayData::Product> const* GetProductsOfProductInfo(uint32 productInfoEntry);
    std::vector<BattlePayData::ProductItem> const* GetItemsOfProduct(uint32 productID);
    BattlePayData::Product const* GetProduct(uint32 productID);
    BattlePayData::ProductInfo const* GetProductInfoForProduct(uint32 productID);
    BattlePayData::DisplayInfo const* GetDisplayInfo(uint32 id);
    BattlePayData::Visual const* FindVisualForDisplayInfo(uint32 displayInfoEntry);
    BattlePayData::ProductAddon const* GetProductAddon(uint32 displayInfoEntry);
    uint32 GetProductGroupId(uint32 productId);
    bool ProductExist(uint32 productID);
    bool DisplayInfoExist(uint32 displayInfoEntry);

private:
    void LoadProductAddons();
    void LoadProductGroups();
    void LoadProduct();
    void LoadShopEntries();
    void LoadDisplayInfos();
};

#define sBattlePayDataStore BattlePayDataStoreMgr::instance()

#endif
