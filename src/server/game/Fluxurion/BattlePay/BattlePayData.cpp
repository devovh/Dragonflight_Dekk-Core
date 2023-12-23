/*
 * Copyright 2023 Fluxurion
 */

#include "BattlePayData.h"
#include "BattlePayMgr.h"
#include "Containers.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include <sstream>

BattlePayDataStoreMgr::BattlePayDataStoreMgr() = default;

BattlePayDataStoreMgr::~BattlePayDataStoreMgr() = default;

BattlePayDataStoreMgr* BattlePayDataStoreMgr::instance()
{
    static BattlePayDataStoreMgr instance;
    return &instance;
}

namespace
{
    std::vector<BattlePayData::Group> _productGroups;
    std::vector<BattlePayData::Shop> _shopEntries;
    std::map<uint32, BattlePayData::Product> _products;
    std::map<uint32, BattlePayData::ProductInfo> _productInfos;
    std::map<uint32, BattlePayData::DisplayInfo> _displayInfos;
    std::map<uint32, BattlePayData::ProductAddon> _productAddons;
}

void BattlePayDataStoreMgr::Initialize()
{
    LoadProductAddons();
    LoadDisplayInfos();
    LoadProduct();
    LoadProductGroups();
    LoadShopEntries();
}

void BattlePayDataStoreMgr::LoadProductAddons()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay display info addons ...");
    _productAddons.clear();

    auto result = WorldDatabase.PQuery("SELECT DisplayInfoEntry, DisableListing, DisableBuy, NameColorIndex, ScriptName, Comment FROM battlepay_addon");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        BattlePayData::ProductAddon productAddon;
        productAddon.DisplayInfoEntry = fields[0].GetUInt32();
        productAddon.DisableListing = fields[1].GetUInt8();
        productAddon.DisableBuy = fields[2].GetUInt8();
        productAddon.NameColorIndex = fields[3].GetUInt8();
        productAddon.ScriptName = fields[4].GetString();
        productAddon.Comment = fields[5].GetString();
        _productAddons.insert(std::make_pair(fields[0].GetUInt32(), productAddon));
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} Battlepay product addons in {} ms", uint64(_productAddons.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadDisplayInfos()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay display info ...");
    _displayInfos.clear();

    auto result = WorldDatabase.PQuery("SELECT Entry, CreatureDisplayID, VisualID, Name1, Name2, Name3, Name4, Name5, Name6, Name7, Flags, Unk1, Unk2, Unk3, UnkInt1, UnkInt2, UnkInt3 FROM battlepay_displayinfo");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        BattlePayData::DisplayInfo displayInfo;
        displayInfo.Entry = fields[0].GetUInt32();
        displayInfo.CreatureDisplayID = fields[1].GetUInt32();
        displayInfo.VisualID = fields[2].GetUInt32();
        displayInfo.Name1 = fields[3].GetString();
        displayInfo.Name2 = fields[4].GetString();
        displayInfo.Name3 = fields[5].GetString();
        displayInfo.Name4 = fields[6].GetString();
        displayInfo.Name5 = fields[7].GetString();
        displayInfo.Name6 = fields[8].GetString();
        displayInfo.Name7 = fields[9].GetString();
        displayInfo.Flags = fields[10].GetUInt32();
        displayInfo.Unk1 = fields[11].GetUInt32();
        displayInfo.Unk2 = fields[12].GetUInt32();
        displayInfo.Unk3 = fields[13].GetUInt32();
        displayInfo.UnkInt1 = fields[14].GetUInt32();
        displayInfo.UnkInt2 = fields[15].GetUInt32();
        displayInfo.UnkInt3 = fields[16].GetUInt32();
        _displayInfos.insert(std::make_pair(fields[0].GetUInt32(), displayInfo));
    } while (result->NextRow());

    result = WorldDatabase.PQuery("SELECT Entry, DisplayId, VisualId, Unk, Name, DisplayInfoEntry FROM battlepay_visual");
    if (!result)
        return;

    int visualCounter = 0;

    do
    {
        auto fields = result->Fetch();

        visualCounter++;

        BattlePayData::Visual visualInfo;
        visualInfo.Entry = fields[0].GetUInt32();
        visualInfo.DisplayId = fields[1].GetUInt32();
        visualInfo.VisualId = fields[2].GetUInt32();
        visualInfo.Unk = fields[3].GetUInt32();
        visualInfo.Name = fields[4].GetString();
        visualInfo.DisplayInfoEntry = fields[5].GetUInt32();

        if (_displayInfos.find(visualInfo.DisplayInfoEntry) == _displayInfos.end())
            continue;

        _displayInfos.at(visualInfo.DisplayInfoEntry).Visuals.push_back(visualInfo);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} Battlepay display info with {} visual in {} ms.", uint64(_displayInfos.size()), visualCounter, GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadProductGroups()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay product groups ...");
    _productGroups.clear();

    auto result = WorldDatabase.PQuery("SELECT Entry, GroupId, IconFileDataID, DisplayType, Ordering, Unk, MainGroupID, Name, Description FROM battlepay_group");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        BattlePayData::Group productGroup;
        productGroup.Entry = fields[0].GetUInt32();
        productGroup.GroupId = fields[1].GetUInt32();
        productGroup.IconFileDataID = fields[2].GetUInt32();
        productGroup.DisplayType = fields[3].GetUInt32();
        productGroup.Ordering = fields[4].GetUInt32();
        productGroup.Unk = fields[5].GetUInt32();
        productGroup.MainGroupID = fields[6].GetUInt32();
        productGroup.Name = fields[7].GetString();
        productGroup.Description = fields[8].GetString();
        _productGroups.push_back(productGroup);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} Battlepay product groups in {} ms", uint64(_productGroups.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadProduct()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay products ...");
    _products.clear();
    _productInfos.clear();

    // Product Info
    auto result = WorldDatabase.PQuery("SELECT Entry, ProductId, NormalPriceFixedPoint, CurrentPriceFixedPoint, ProductIds, Unk1, Unk2, UnkInts, Unk3, ChoiceType FROM battlepay_productinfo");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        BattlePayData::ProductInfo productInfo;
        productInfo.Entry = fields[0].GetUInt32();
        productInfo.ProductId = fields[1].GetUInt32();
        productInfo.NormalPriceFixedPoint = fields[2].GetUInt32();
        productInfo.CurrentPriceFixedPoint = fields[3].GetUInt32();
        std::stringstream subproducts_stream(fields[4].GetString());
        std::string subproducts;
        while (std::getline(subproducts_stream, subproducts, ','))
            productInfo.ProductIds.push_back(atol(subproducts.c_str())); // another cool flux stuff: multiple subproducts can be added in one column
        productInfo.Unk1 = fields[5].GetUInt32();
        productInfo.Unk2 = fields[6].GetUInt32();
        productInfo.UnkInts.push_back(fields[7].GetUInt32());
        productInfo.Unk3 = fields[8].GetUInt32();
        productInfo.ChoiceType = fields[9].GetUInt32();

        // we copy store the info for every product - some product info is the same for multiple products
        for (uint32 subproductid : productInfo.ProductIds)
            _productInfos.insert(std::make_pair(subproductid, productInfo));
    } while (result->NextRow());

    // Product
    result = WorldDatabase.PQuery("SELECT Entry, ProductId, Type, Flags, Unk1, DisplayId, ItemId, Unk4, Unk5, Unk6, Unk7, Unk8, Unk9, UnkString, UnkBit, UnkBits, Name FROM battlepay_product");
    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        BattlePayData::Product product;
        product.Entry = fields[0].GetUInt32();
        product.ProductId = fields[1].GetUInt32();
        product.Type = fields[2].GetUInt32();
        product.Flags = fields[3].GetUInt32();
        product.Unk1 = fields[4].GetUInt32();
        product.DisplayId = fields[5].GetUInt32();
        product.ItemId = fields[6].GetUInt32();
        product.Unk4 = fields[7].GetUInt32();
        product.Unk5 = fields[8].GetUInt32();
        product.Unk6 = fields[9].GetUInt32();
        product.Unk7 = fields[10].GetUInt32();
        product.Unk8 = fields[11].GetUInt32();
        product.Unk9 = fields[12].GetUInt32();
        product.UnkString = fields[13].GetString();
        product.UnkBit = fields[14].GetUInt32();
        product.UnkBits = fields[15].GetUInt32();
        product.Name = fields[16].GetString(); // unused in packets but useful in other ways

        _products.insert(std::make_pair(fields[1].GetUInt32(), product));
    } while (result->NextRow());

    // Product Items
    result = WorldDatabase.PQuery("SELECT ID, UnkByte, ItemID, Quantity, UnkInt1, UnkInt2, IsPet, PetResult, Display FROM battlepay_item");
    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        auto productID = fields[1].GetUInt32();
        if (_products.find(productID) == _products.end())
            continue;

        BattlePayData::ProductItem productItem;

        productItem.ItemID = fields[2].GetUInt32();
            if (!sItemStore.LookupEntry(productItem.ItemID))
                continue;
        productItem.Entry = fields[0].GetUInt32();
        productItem.ID = productID;
        productItem.UnkByte = fields[2].GetUInt32();
        productItem.ItemID = fields[3].GetUInt32();
        productItem.Quantity = fields[4].GetUInt32();
        productItem.UnkInt1 = fields[5].GetUInt32();
        productItem.UnkInt2 = fields[6].GetUInt32();
        productItem.IsPet = fields[7].GetUInt32();
        productItem.PetResult = fields[8].GetUInt32();
        _products[productID].Items.push_back(productItem);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} Battlepay product infos and {} Battlepay products in {} ms", uint64(_productInfos.size()), uint64(_products.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadShopEntries()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay shop entries ...");
    _shopEntries.clear();

    auto result = WorldDatabase.PQuery("SELECT Entry, EntryID, GroupID, ProductID, Ordering, VasServiceType, StoreDeliveryType FROM battlepay_shop");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        BattlePayData::Shop shopEntry;
        shopEntry.Entry = fields[0].GetUInt32();
        shopEntry.EntryID = fields[1].GetUInt32();
        shopEntry.GroupID = fields[2].GetUInt32();
        shopEntry.ProductID = fields[3].GetUInt32();
        shopEntry.Ordering = fields[4].GetInt32();
        shopEntry.VasServiceType = fields[5].GetUInt32();
        shopEntry.StoreDeliveryType = fields[6].GetUInt8();
        _shopEntries.push_back(shopEntry);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} Battlepay shop entries in {} ms", uint64(_shopEntries.size()), GetMSTimeDiffToNow(oldMsTime));
}

std::vector<BattlePayData::Group> BattlePayDataStoreMgr::GetProductGroups()
{
    return _productGroups;
}

std::vector<BattlePayData::Shop> BattlePayDataStoreMgr::GetShopEntries()
{
    return _shopEntries;
}

std::map<uint32, BattlePayData::ProductInfo> BattlePayDataStoreMgr::GetProductInfos()
{
    return _productInfos;
}

std::map<uint32, BattlePayData::Product> BattlePayDataStoreMgr::GetProducts()
{
    return _products;
}

uint32 BattlePayDataStoreMgr::GetProductGroupId(uint32 productId)
{
    for (auto const& shop : _shopEntries)
        if (shop.ProductID == productId)
            return shop.GroupID;

    return 0;
}

bool BattlePayDataStoreMgr::ProductExist(uint32 productID)
{
    if (_products.find(productID) != _products.end())
       return true;

    TC_LOG_INFO("server.BattlePay", "ProductExist failed for productID {}", productID);
    return false;
}

bool BattlePayDataStoreMgr::DisplayInfoExist(uint32 displayInfoEntry)
{
    if (_displayInfos.find(displayInfoEntry) != _displayInfos.end())
       return true;

    TC_LOG_INFO("server.BattlePay", "DisplayInfoExist failed for displayInfoEntry {}", displayInfoEntry);
    return false;
}

BattlePayData::Product const* BattlePayDataStoreMgr::GetProduct(uint32 productID)
{
    if (_products.find(productID) == _products.end())
        return nullptr;

    return &_products.at(productID);
}

BattlePayData::DisplayInfo const* BattlePayDataStoreMgr::GetDisplayInfo(uint32 displayInfoEntry)
{
    if (_displayInfos.find(displayInfoEntry) == _displayInfos.end())
        return nullptr;

    return &_displayInfos.at(displayInfoEntry);
}

BattlePayData::Visual const* BattlePayDataStoreMgr::FindVisualForDisplayInfo(uint32 displayInfoEntry)
{
    BattlePayData::Visual* visual = new BattlePayData::Visual();
    auto displayInfo = GetDisplayInfo(displayInfoEntry);

    for (auto _displayInfo : _displayInfos)
    {
        for (auto _visual : _displayInfo.second.Visuals)
        {
            if (displayInfo->Name1 == _visual.Name)
                visual = &_visual;
        }
    }

    return visual;
}

// Custom properties for each product (displayinfoEntry, productInfoEntry, shopEntry are the same)
BattlePayData::ProductAddon const* BattlePayDataStoreMgr::GetProductAddon(uint32 displayInfoEntry)
{
    if (_productAddons.find(displayInfoEntry) == _productAddons.end())
        return nullptr;

    return &_productAddons.at(displayInfoEntry);
}

std::vector<BattlePayData::Product> const* BattlePayDataStoreMgr::GetProductsOfProductInfo(uint32 productInfoEntry)
{
    std::vector<BattlePayData::Product> subproducts;

    for (auto productInfo : _productInfos)
        if (productInfo.second.Entry == productInfoEntry)
            for (uint32 productid : productInfo.second.ProductIds)
            {
                //TC_LOG_INFO("server.BattlePay", "GetProductsOfProductInfo: found product [{}] at productInfo [{}]", productid, productInfoEntry);
                subproducts.push_back(*GetProduct(productid));
            }

    if (subproducts.size() > 0)
        return &subproducts; // warning

    TC_LOG_INFO("server.BattlePay", "GetProductsOfProductInfo failed for productInfoEntry {}", productInfoEntry);
    return nullptr;
}

// This awesome function returns back the productinfo for all the two types of productid!
BattlePayData::ProductInfo const* BattlePayDataStoreMgr::GetProductInfoForProduct(uint32 productID)
{
    // Find product by subproduct id (_productInfos.productids) if not found find it by shop productid (_productInfos.productid)
    if (_productInfos.find(productID) == _productInfos.end())
    {
        for (auto productInfo : _productInfos)
            if (productInfo.second.ProductId == productID)
            {
                TC_LOG_INFO("server.battlepay", "GetProductInfoForProduct succeed {}", productID);
                return &_productInfos.at(productInfo.first);
            }

        TC_LOG_INFO("server.BattlePay", "GetProductInfoForProduct failed for productID {}", productID);
        return nullptr;
    }

    return &_productInfos.at(productID);
}

std::vector<BattlePayData::ProductItem> const* BattlePayDataStoreMgr::GetItemsOfProduct(uint32 productID)
{
    for (auto const& product : _products)
        if (product.second.ProductId == productID)
            return &product.second.Items;

    TC_LOG_INFO("server.BattlePay", "GetItemsOfProduct failed for productid {}", productID);
    return nullptr;
}
