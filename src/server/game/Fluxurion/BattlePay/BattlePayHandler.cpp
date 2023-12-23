/*
 * Copyright 2023 Fluxurion
 */

#include "Bag.h"
#include "BattlePayPackets.h"
#include "BattlePayMgr.h"
#include "BattlePayData.h"
#include "Config.h"
#include "Containers.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "Random.h"
#include <sstream>

auto GetBagsFreeSlots = [](Player* player) -> uint32
{
    uint32 freeBagSlots = 0;
    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
        if (auto bag = player->GetBagByPos(i))
            freeBagSlots += bag->GetFreeSlots();

    uint8 inventoryEnd = INVENTORY_SLOT_ITEM_START + player->GetInventorySlotCount();
    for (uint8 i = INVENTORY_SLOT_ITEM_START; i < inventoryEnd; i++)
        if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            ++freeBagSlots;

    return freeBagSlots;
};

void WorldSession::SendStartPurchaseResponse(WorldSession* session, Battlepay::Purchase const& purchase, Battlepay::Error const& result)
{
    WorldPackets::BattlePay::StartPurchaseResponse response;
    response.PurchaseID = purchase.PurchaseID;
    response.ClientToken = purchase.ClientToken;
    response.PurchaseResult = result;
    session->SendPacket(response.Write());
};

void WorldSession::SendPurchaseUpdate(WorldSession* session, Battlepay::Purchase const& purchase, uint32 result)
{
    WorldPackets::BattlePay::PurchaseUpdate packet;
    WorldPackets::BattlePay::Purchase data;
    data.PurchaseID = purchase.PurchaseID;
    data.UnkLong = 0;
    data.UnkLong2 = 0;
    data.Status = purchase.Status;
    data.ResultCode = result;
    data.ProductID = purchase.ProductID;
    data.UnkInt = purchase.ServerToken;
    data.WalletName = session->GetBattlePayMgr()->GetDefaultWalletName();
    packet.Purchase.emplace_back(data);
    session->SendPacket(packet.Write());
};

void WorldSession::HandleGetPurchaseListQuery(WorldPackets::BattlePay::GetPurchaseListQuery& /*packet*/)
{
    WorldPackets::BattlePay::PurchaseListResponse packet; // @TODO
    SendPacket(packet.Write());
}

void WorldSession::HandleUpdateVasPurchaseStates(WorldPackets::BattlePay::UpdateVasPurchaseStates& /*packet*/)
{
    WorldPackets::BattlePay::EnumVasPurchaseStatesResponse response;
    response.Result = 0;
    SendPacket(response.Write());
}

void WorldSession::HandleBattlePayDistributionAssign(WorldPackets::BattlePay::DistributionAssignToTarget& packet)
{
    if (!GetBattlePayMgr()->IsAvailable())
        return;

    GetBattlePayMgr()->AssignDistributionToCharacter(packet.TargetCharacter, packet.DistributionID, packet.ProductID, packet.SpecializationID, packet.ChoiceID);
}

void WorldSession::HandleGetProductList(WorldPackets::BattlePay::GetProductList& /*packet*/)
{
    if (!GetBattlePayMgr()->IsAvailable())
        return;

    GetBattlePayMgr()->SendProductList();
    GetBattlePayMgr()->SendAccountCredits();
}

void WorldSession::SendMakePurchase(ObjectGuid targetCharacter, uint32 clientToken, uint32 productID, WorldSession* session)
{
    if (!session || !session->GetBattlePayMgr()->IsAvailable())
        return;

    auto mgr = session->GetBattlePayMgr();
    auto player = session->GetPlayer();
    auto accountID = session->GetAccountId();

    Battlepay::Purchase purchase;
    purchase.ProductID = productID;
    purchase.ClientToken = clientToken;
    purchase.TargetCharacter = targetCharacter;
    purchase.Status = Battlepay::UpdateStatus::Loading;
    purchase.DistributionId = mgr->GenerateNewDistributionId();

    BattlePayData::ProductInfo productInfo = *sBattlePayDataStore->GetProductInfoForProduct(productID);

    purchase.CurrentPrice = uint64(productInfo.CurrentPriceFixedPoint);

    mgr->RegisterStartPurchase(purchase);

    auto accountCredits = GetBattlePayMgr()->GetBattlePayCredits();
    auto purchaseData = mgr->GetPurchase();

    if (accountCredits < static_cast<uint64>(purchaseData->CurrentPrice))
    {
        SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::InsufficientBalance);
        return;
    }

    for (uint32 productId : productInfo.ProductIds)
        if (sBattlePayDataStore->ProductExist(productId))
        {
            BattlePayData::Product product = *sBattlePayDataStore->GetProduct(productId);

            // if buy is disabled in product addons
            auto productAddon = sBattlePayDataStore->GetProductAddon(productInfo.Entry);
            if (productAddon)
                if (productAddon->DisableBuy > 0)
                    SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::PurchaseDenied);

            if (!product.Items.empty())
            {
                if (player)
                    if (product.Items.size() > GetBagsFreeSlots(player))
                    {
                        GetBattlePayMgr()->SendBattlePayMessage(11, product.Name);
                        SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::PurchaseDenied);
                        return;
                    }

                for (auto itr : product.Items)
                {
                    if (mgr->AlreadyOwnProduct(itr.ItemID))
                    {
                        GetBattlePayMgr()->SendBattlePayMessage(12, product.Name);
                        SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::PurchaseDenied);
                        return;
                    }
                }
            }
        }
        else
        {
            SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::PurchaseDenied);
            return;
        }

    purchaseData->PurchaseID = mgr->GenerateNewPurchaseID();
    purchaseData->ServerToken = urand(0, 0xFFFFFFF);

    SendStartPurchaseResponse(session, *purchaseData, Battlepay::Error::Ok);
    SendPurchaseUpdate(session, *purchaseData, Battlepay::Error::Ok);

    WorldPackets::BattlePay::ConfirmPurchase confirmPurchase;
    confirmPurchase.PurchaseID = purchaseData->PurchaseID;
    confirmPurchase.ServerToken = purchaseData->ServerToken;
    session->SendPacket(confirmPurchase.Write());
};

void WorldSession::HandleBattlePayStartPurchase(WorldPackets::BattlePay::StartPurchase& packet)
{
    SendMakePurchase(packet.TargetCharacter, packet.ClientToken, packet.ProductID, this);
}

void WorldSession::HandleBattlePayConfirmPurchase(WorldPackets::BattlePay::ConfirmPurchaseResponse& packet)
{
    if (!GetBattlePayMgr()->IsAvailable())
        return;

    auto purchase = GetBattlePayMgr()->GetPurchase();
    if (!purchase)
        return;

    auto productInfo = *sBattlePayDataStore->GetProductInfoForProduct(purchase->ProductID);
    auto displayInfo = *sBattlePayDataStore->GetDisplayInfo(productInfo.Entry);

    if (purchase->Lock)
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    if (purchase->ServerToken != packet.ServerToken || !packet.ConfirmPurchase || purchase->CurrentPrice != packet.ClientCurrentPriceFixedPoint)
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    auto accountBalance = GetBattlePayMgr()->GetBattlePayCredits();
    if (accountBalance < static_cast<uint64>(purchase->CurrentPrice))
    {
        SendPurchaseUpdate(this, *purchase, Battlepay::Error::PurchaseDenied);
        return;
    }

    purchase->Lock = true;
    purchase->Status = Battlepay::UpdateStatus::Finish;

    SendPurchaseUpdate(this, *purchase, Battlepay::Error::Other);
    GetBattlePayMgr()->SavePurchase(purchase);
    GetBattlePayMgr()->ProcessDelivery(purchase);
    GetBattlePayMgr()->UpdateBattlePayCredits(purchase->CurrentPrice);
    if (!displayInfo.Name1.empty())
        GetBattlePayMgr()->SendBattlePayMessage(1, displayInfo.Name1);
    GetBattlePayMgr()->SendProductList();
}

void WorldSession::HandleBattlePayAckFailedResponse(WorldPackets::BattlePay::BattlePayAckFailedResponse& /*packet*/)
{
}

void WorldSession::HandleBattlePayRequestPriceInfo(WorldPackets::BattlePay::BattlePayRequestPriceInfo& packet)
{
}

void WorldSession::SendDisplayPromo(int32 promotionID /*= 0*/)
{
    SendPacket(WorldPackets::BattlePay::DisplayPromotion(promotionID).Write());

    if (!GetBattlePayMgr()->IsAvailable())
        return;

    uint32 promoProductID = Trinity::Containers::SelectRandomContainerElement(sBattlePayDataStore->GetProducts()).second.ProductId;

    if (!sBattlePayDataStore->ProductExist(promoProductID))
        return;

    auto& product = *sBattlePayDataStore->GetProduct(promoProductID);
    WorldPackets::BattlePay::DistributionListResponse packet;
    packet.Result = Battlepay::Error::Ok;

    WorldPackets::BattlePay::DistributionObject data;
    data.TargetPlayer;
    data.DistributionID = GetBattlePayMgr()->GenerateNewDistributionId();
    data.PurchaseID = GetBattlePayMgr()->GenerateNewPurchaseID();
    data.Status = Battlepay::DistributionStatus::BATTLE_PAY_DIST_STATUS_AVAILABLE;
    data.ProductID = promoProductID;
    data.TargetVirtualRealm = 0;
    data.TargetNativeRealm = 0;
    data.Revoked = false;

    auto& productInfo = *sBattlePayDataStore->GetProductInfoForProduct(product.ProductId);

    // BATTLEPAY PRODUCTS
    WorldPackets::BattlePay::Product pProduct;
    pProduct.ProductId = product.ProductId;
    pProduct.Type = product.Type;
    pProduct.Flags = product.Flags;
    pProduct.Unk1 = product.Unk1;
    pProduct.DisplayId = product.DisplayId;
    pProduct.ItemId = product.ItemId;
    pProduct.Unk4 = product.Unk4;
    pProduct.Unk5 = product.Unk5;
    pProduct.Unk6 = product.Unk6;
    pProduct.Unk7 = product.Unk7;
    pProduct.Unk8 = product.Unk8;
    pProduct.Unk9 = product.Unk9;
    pProduct.UnkString = product.UnkString;
    pProduct.UnkBit = product.UnkBit;
    pProduct.UnkBits = product.UnkBits;

    // BATTLEPAY ITEM
    if (product.Items.size() > 0)
        for (auto& item : *sBattlePayDataStore->GetItemsOfProduct(product.ProductId))
        {
            WorldPackets::BattlePay::ProductItem pItem;
            pItem.ID = item.ID;
            pItem.UnkByte = item.UnkByte;
            pItem.ItemID = item.ItemID;
            pItem.Quantity = item.Quantity;
            pItem.UnkInt1 = item.UnkInt1;
            pItem.UnkInt2 = item.UnkInt2;
            pItem.IsPet = item.IsPet;
            pItem.PetResult = item.PetResult;

            if (sBattlePayDataStore->DisplayInfoExist(productInfo.Entry))
            {
                // productinfo entry and display entry must be the same
                auto data = GetBattlePayMgr()->WriteDisplayInfo(productInfo.Entry);
                if (std::get<0>(data))
                {
                    pItem.Display.emplace();
                    pItem.Display = std::get<1>(data);
                }
            }

            pProduct.Items.emplace_back(pItem);
        }

    // productinfo entry and display entry must be the same
    auto display = GetBattlePayMgr()->WriteDisplayInfo(productInfo.Entry);
    if (std::get<0>(display))
    {
        pProduct.Display.emplace();
        pProduct.Display = std::get<1>(display);
    }

    data.Product.emplace();
    data.Product = pProduct;

    packet.DistributionObject.emplace_back(data);

    SendPacket(packet.Write());
}

void WorldSession::SendSyncWowEntitlements()
{
    WorldPackets::BattlePay::SyncWowEntitlements packet;
    SendPacket(packet.Write());
}
