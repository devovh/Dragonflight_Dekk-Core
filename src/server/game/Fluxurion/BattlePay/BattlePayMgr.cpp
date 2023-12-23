/*
 * Copyright 2023 Fluxurion
 */

#include "Flux.h"
#include "Common.h"
#include "CollectionMgr.h"
#include "ObjectMgr.h"
#include "BattlePayMgr.h"
#include "BattlePet.h"
#include "CollectionMgr.h"
#include "WorldSession.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Player.h"
#include "BattlePayData.h"
#include "DatabaseEnv.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "CollectionMgr.h"
#include <sstream>
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Language.h"
#include "SpellPackets.h"
#include "Chat.h"
#include "DB2Stores.h"
#include "BattlePayPackets.h"
#include "LanguageMgr.h"
#include "Pet.h"
#include "Item.h"

using namespace Battlepay;

BattlepayManager::BattlepayManager(WorldSession* session)
{
    _session = session;
    _purchaseIDCount = 0;
    _distributionIDCount = 0;
    _walletName = "Credits";
}

BattlepayManager::~BattlepayManager() = default;

void BattlepayManager::RegisterStartPurchase(Purchase purchase)
{
    _actualTransaction = purchase;
}

uint64 BattlepayManager::GenerateNewPurchaseID()
{
    return uint64(0x1E77800000000000 | ++_purchaseIDCount);
}

uint64 BattlepayManager::GenerateNewDistributionId()
{
    return uint64(0x1E77800000000000 | ++_distributionIDCount);
}

Purchase* BattlepayManager::GetPurchase()
{
    return &_actualTransaction;
}

std::string const& BattlepayManager::GetDefaultWalletName() const
{
    return _walletName;
}

uint32 BattlepayManager::GetShopCurrency() const
{
    return sWorld->getIntConfig(CONFIG_BATTLE_PAY_CURRENCY);
}

bool BattlepayManager::IsAvailable() const
{
    if (AccountMgr::IsAdminAccount(_session->GetSecurity()))
        return true;

    return sWorld->getBoolConfig(CONFIG_BATTLE_PAY_ENABLED);
}

void BattlepayManager::SavePurchase(Purchase * purchase)
{
    auto productInfo = *sBattlePayDataStore->GetProductInfoForProduct(purchase->ProductID);
    auto displayInfo = *sBattlePayDataStore->GetDisplayInfo(productInfo.Entry);
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_PURCHASE);
    stmt->setUInt32(0, _session->GetAccountId());
    stmt->setUInt32(1, GetVirtualRealmAddress());
    stmt->setUInt32(2, _session->GetPlayer() ? _session->GetPlayer()->GetGUID().GetCounter() : 0);
    stmt->setUInt32(3, purchase->ProductID);
    stmt->setString(4, displayInfo.Name1);
    stmt->setUInt32(5, purchase->CurrentPrice);
    stmt->setString(6, _session->GetRemoteAddress());
    LoginDatabase.Execute(stmt);
}

static void LearnSkillRecipesHelper(Player * player, uint32 skillId)
{
    uint32 classmask = player->GetClassMask();

    for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
    {
        SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
        if (!skillLine)
            continue;

        // wrong skill
        if (skillLine->SkillLine != int32(skillId))
            continue;

        // not high rank
        if (skillLine->SupercedesSpell)
            continue;

        // skip racial skills
        if (!skillLine->RaceMask.IsEmpty())
            continue;

        // skip wrong class skills
        if (skillLine->ClassMask && (skillLine->ClassMask & classmask) == 0)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(skillLine->Spell, DIFFICULTY_NONE);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
            continue;

        player->LearnSpell(skillLine->Spell, false);
    }
}

bool LearnAllRecipesInProfession(Player * player, SkillType skill)
{
    SkillLineEntry const* SkillInfo = sSkillLineStore.LookupEntry(skill);
    if (!SkillInfo)
        return false;
    ChatHandler handler(player->GetSession());

    LearnSkillRecipesHelper(player, SkillInfo->ID);

    uint16 maxLevel = player->GetPureMaxSkillValue(SkillInfo->ID);
    player->SetSkill(SkillInfo->ID, player->GetSkillStep(SkillInfo->ID), maxLevel, maxLevel);

    return true;
}

void BattlepayManager::ProcessDelivery(Purchase * purchase)
{
    auto player = _session->GetPlayer();
    if (!player)
        return;

    auto productInfo = *sBattlePayDataStore->GetProductInfoForProduct(purchase->ProductID);
    std::vector<uint32> itemstosendinmail;

    for (auto productId : productInfo.ProductIds)
    {
        auto product = sBattlePayDataStore->GetProduct(productId);
        auto item = sObjectMgr->GetItemTemplate(product->Flags);
        std::vector<uint32> itemsToSendIfInventoryFull;

        switch (product->Type)
        {
        case Battlepay::Item_: // 0
            itemsToSendIfInventoryFull.clear();
            if (item && player)
                if (player->GetFreeInventorySpace() > product->Unk1)
                    Fluxurion::AddItemWithToast(player, product->Flags, product->Unk1, {});
                else
                    Fluxurion::SendABunchOfItemsInMail(player, { product->Flags }, "Ingame Shop item delivery", {});
            else
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);

            for (BattlePayData::ProductItem _item : *sBattlePayDataStore->GetItemsOfProduct(product->ProductId))
            {
                if (sObjectMgr->GetItemTemplate(_item.ItemID))
                    if (player->GetFreeInventorySpace() > _item.Quantity)
                        Fluxurion::AddItemWithToast(player, _item.ItemID, _item.Quantity, {});
                    else
                        itemsToSendIfInventoryFull.push_back(_item.ItemID); // problem if the quantity > 0
            }

            if (itemsToSendIfInventoryFull.size() > 0)
                Fluxurion::SendABunchOfItemsInMail(player, itemsToSendIfInventoryFull, "Ingame Shop Item Delivery", {});
            break;

        case Battlepay::LevelBoost: // 1
            if (product->ProductId == 572) // level 50 boost
            {
                player->SetLevel(50);
                Fluxurion::GearUpByLoadout(player, 9, { 6771 });
                player->InitTalentForLevel();
                player->InitStatsForLevel();
                player->UpdateSkillsForLevel();
                player->LearnDefaultSkills();
                player->LearnSpecializationSpells();
                player->UpdateAllStats();
                player->SetFullHealth();
                player->SetFullPower(POWER_MANA);
            }
            if (product->ProductId == 630) // level 60 boost
            {
                player->SetLevel(60);
                Fluxurion::GearUpByLoadout(player, 9, { 6771 });
                player->InitTalentForLevel();
                player->InitStatsForLevel();
                player->UpdateSkillsForLevel();
                player->LearnDefaultSkills();
                player->LearnSpecializationSpells();
                player->UpdateAllStats();
                player->SetFullHealth();
                player->SetFullPower(POWER_MANA);
            }
            break;

        case Battlepay::Pet: // 2
            if (player) // if logged in
                player->AddBattlePetByCreatureId(product->ItemId, true, true);
            else
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::Mount: // 3
            _session->GetCollectionMgr()->AddMount(product->DisplayId, MountStatusFlags::MOUNT_STATUS_NONE);
            break;            

        case Battlepay::WoWToken: // 4
            if (item && player)
                if (player->GetFreeInventorySpace() > product->Unk1)
                    Fluxurion::AddItemWithToast(player, product->Flags, product->Unk1, {});
                else
                    Fluxurion::SendABunchOfItemsInMail(player, { product->Flags }, "Ingame Shop - WoW Token Delivery", {});
            else
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::NameChange: // 5
            if (player) // if logged in
                player->SetAtLoginFlag(AtLoginFlags::AT_LOGIN_RENAME);
            else
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::FactionChange: // 6
            if (player) // if logged in
                player->SetAtLoginFlag(AtLoginFlags::AT_LOGIN_CHANGE_FACTION); // not ok for 6 or 3 faction change - only does once yet
            else
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::RaceChange: // 8
            if (player) // if logged in
                player->SetAtLoginFlag(AtLoginFlags::AT_LOGIN_CHANGE_RACE); // not ok for 6 or 3 faction change - only does once yet
            else
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::CharacterTransfer: // 11
                // if u have multiple realms u have to implement this xD otherwise it sends error
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::Toy: // 14
            _session->GetCollectionMgr()->AddToy(product->Flags, false, product->Unk1);
            break;

        case Battlepay::Expansion: // 18
            if (player) // if logged in
                //player->SendMovieStart(936); // Play SL Intro - xD what else in a private server we don't sell expansions
                player->SendMovieStart(957); // Play SL Outro - we are preparing for dragonflight xD
            break;

        case Battlepay::GameTime: // 20
            if (item && player)
                if (player->GetFreeInventorySpace() > product->Unk1)
                    Fluxurion::AddItemWithToast(player, product->Flags, product->Unk1, {});
                else
                    Fluxurion::SendABunchOfItemsInMail(player, { product->Flags }, "Ingame Shop - WoW Token Delivery", {});
            else
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::GuildNameChange: // 21
        case Battlepay::GuildFactionChange: // 22
        case Battlepay::GuildTransfer: // 23
        case Battlepay::GuildFactionTranfer: // 24
            // Not implemented yet - need some more guild functions e.g.: getmembers
                _session->SendStartPurchaseResponse(_session, *GetPurchase(), Battlepay::Error::PurchaseDenied);
            break;

        case Battlepay::TransmogAppearance: // 26
            _session->GetCollectionMgr()->AddTransmogSet(product->Unk7);
            break;

        /// Customs:
        case Battlepay::ItemSet:
        {
            ItemTemplateContainer const& its = sObjectMgr->GetItemTemplateStore();
            for (auto const& itemTemplatePair : its)
            {
                if (itemTemplatePair.second.GetItemSet() != product->Flags)
                    continue;

                ItemPosCountVec dest;
                InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemTemplatePair.first, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = player->StoreNewItem(dest, itemTemplatePair.first, true, {}, GuidSet());

                    player->SendNewItem(item, 1, true, false);
                }
                else
                {
                    itemstosendinmail.push_back(itemTemplatePair.second.GetId());
                }
            }
            if (itemstosendinmail.size() > 0)
                Fluxurion::SendABunchOfItemsInMail(player, itemstosendinmail, "Ingame Shop - You bought an item set!", {});
        }
        break;
		
                case Battlepay::Gold: // 30
                    if (player)
                        player->ModifyMoney(product->Unk7);
                    break;

                case Battlepay::Currency: // 31
                if (player)
                    player->ModifyCurrency(product->Flags, product->Unk1); // implement currencyID in DB
                break;
/*
                case Battlepay::CharacterCustomization:
                    if (player)
                        player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
                    break;

                // Script by Legolast++
                case Battlepay::ProfPriAlchemy:

                    player->HasSkill(SKILL_ALCHEMY);
                    player->HasSkill(SKILL_SHADOWLANDS_ALCHEMY);
                    LearnAllRecipesInProfession(player, SKILL_ALCHEMY);
                    break;

                case Battlepay::ProfPriSastre:

                    player->HasSkill(SKILL_TAILORING);
                    player->HasSkill(SKILL_SHADOWLANDS_TAILORING);
                    LearnAllRecipesInProfession(player, SKILL_TAILORING);
                    break;
                case Battlepay::ProfPriJoye:

                    player->HasSkill(SKILL_JEWELCRAFTING);
                    player->HasSkill(SKILL_SHADOWLANDS_JEWELCRAFTING);
                    LearnAllRecipesInProfession(player, SKILL_JEWELCRAFTING);
                    break;
                case Battlepay::ProfPriHerre:

                    player->HasSkill(SKILL_BLACKSMITHING);
                    player->HasSkill(SKILL_SHADOWLANDS_BLACKSMITHING);
                    LearnAllRecipesInProfession(player, SKILL_BLACKSMITHING);
                    break;
                case Battlepay::ProfPriPele:

                    player->HasSkill(SKILL_LEATHERWORKING);
                    player->HasSkill(SKILL_SHADOWLANDS_LEATHERWORKING);
                    LearnAllRecipesInProfession(player, SKILL_LEATHERWORKING);
                    break;
                case Battlepay::ProfPriInge:

                    player->HasSkill(SKILL_ENGINEERING);
                    player->HasSkill(SKILL_SHADOWLANDS_ENGINEERING);
                    LearnAllRecipesInProfession(player, SKILL_ENGINEERING);
                    break;
                case Battlepay::ProfPriInsc:

                    player->HasSkill(SKILL_INSCRIPTION);
                    player->HasSkill(SKILL_SHADOWLANDS_INSCRIPTION);
                    LearnAllRecipesInProfession(player, SKILL_INSCRIPTION);
                    break;
                case Battlepay::ProfPriEncha:

                    player->HasSkill(SKILL_ENCHANTING);
                    player->HasSkill(SKILL_SHADOWLANDS_ENCHANTING);
                    LearnAllRecipesInProfession(player, SKILL_ENCHANTING);
                    break;
                case Battlepay::ProfPriDesu:

                    player->HasSkill(SKILL_SKINNING);
                    player->HasSkill(SKILL_SHADOWLANDS_SKINNING);
                    LearnAllRecipesInProfession(player, SKILL_SKINNING);
                    break;
                case Battlepay::ProfPriMing:

                    player->HasSkill(SKILL_MINING);
                    player->HasSkill(SKILL_SHADOWLANDS_MINING);
                    LearnAllRecipesInProfession(player, SKILL_MINING);
                    break;
                case Battlepay::ProfPriHerb:

                    player->HasSkill(SKILL_HERBALISM);
                    player->HasSkill(SKILL_SHADOWLANDS_HERBALISM);
                    LearnAllRecipesInProfession(player, SKILL_HERBALISM);
                    break;

                case Battlepay::ProfSecCoci:

                    player->HasSkill(SKILL_COOKING);
                    player->HasSkill(SKILL_SHADOWLANDS_COOKING);
                    LearnAllRecipesInProfession(player, SKILL_COOKING);
                    break;

                case Battlepay::Promo:
                    if (!player)
                        // Ridding
                    player->LearnSpell(33388, true);
                    player->LearnSpell(33391, true);
                    player->LearnSpell(34090, true);
                    player->LearnSpell(34091, true);
                    player->LearnSpell(90265, true);
                    player->LearnSpell(54197, true);
                    player->LearnSpell(90267, true);
                    // Mounts
                    player->LearnSpell(63956, true);

                    break;
                case Battlepay::RepClassic:
                    if (!player)
                        player->SetReputation(21, 42000);
                    player->SetReputation(576, 42000);
                    player->SetReputation(87, 42000);
                    player->SetReputation(92, 42000);
                    player->SetReputation(93, 42000);
                    player->SetReputation(609, 42000);
                    player->SetReputation(529, 42000);
                    player->SetReputation(909, 42000);
                    player->SetReputation(369, 42000);
                    player->SetReputation(59, 42000);
                    player->SetReputation(910, 42000);
                    player->SetReputation(349, 42000);
                    player->SetReputation(809, 42000);
                    player->SetReputation(749, 42000);
                    player->SetReputation(270, 42000);
                    player->SetReputation(470, 42000);
                    player->SetReputation(577, 42000);
                    player->SetReputation(70, 42000);
                    player->SetReputation(1357, 42000);
                    player->SetReputation(1975, 42000);

                    if (player->GetTeam() == ALLIANCE)
                    {
                        player->SetReputation(890, 42000);
                        player->SetReputation(1691, 42000);
                        player->SetReputation(1419, 42000);
                        player->SetReputation(69, 42000);
                        player->SetReputation(930, 42000);
                        player->SetReputation(47, 42000);
                        player->SetReputation(1134, 42000);
                        player->SetReputation(54, 42000);
                        player->SetReputation(730, 42000);
                        player->SetReputation(509, 42000);
                        player->SetReputation(1353, 42000);
                        player->SetReputation(72, 42000);
                        player->SetReputation(589, 42000);
                    }
                    else // Repu Horda
                    {
                        player->SetReputation(1690, 42000);
                        player->SetReputation(1374, 42000);
                        player->SetReputation(1133, 42000);
                        player->SetReputation(81, 42000);
                        player->SetReputation(729, 42000);
                        player->SetReputation(68, 42000);
                        player->SetReputation(889, 42000);
                        player->SetReputation(510, 42000);
                        player->SetReputation(911, 42000);
                        player->SetReputation(76, 42000);
                        player->SetReputation(1352, 42000);
                        player->SetReputation(530, 42000);
                    }
                    player->GetSession()->SendNotification("|cff00FF00Se ha aumentado todas las Reputaciones Clasicas!");
                    return;
                    break;
                case Battlepay::RepBurnig:
                    if (!player)
                        player->SetReputation(1015, 42000);
                    player->SetReputation(1011, 42000);
                    player->SetReputation(933, 42000);
                    player->SetReputation(967, 42000);
                    player->SetReputation(970, 42000);
                    player->SetReputation(942, 42000);
                    player->SetReputation(1031, 42000);
                    player->SetReputation(1012, 42000);
                    player->SetReputation(990, 42000);
                    player->SetReputation(932, 42000);
                    player->SetReputation(934, 42000);
                    player->SetReputation(935, 42000);
                    player->SetReputation(1077, 42000);
                    player->SetReputation(1038, 42000);
                    player->SetReputation(989, 42000);

                    if (player->GetTeam() == ALLIANCE)
                    {
                        player->SetReputation(946, 42000);
                        player->SetReputation(978, 42000);
                    }
                    else // Repu Horda
                    {
                        player->SetReputation(941, 42000);
                        player->SetReputation(947, 42000);
                        player->SetReputation(922, 42000);
                    }
                    player->GetSession()->SendNotification("|cff00FF00Se ha aumentado todas las Reputaciones Burning Crusade!");
                    return;
                    break;
                case Battlepay::RepTLK:
                    if (!player)
                        player->SetReputation(1242, 42000);
                    player->SetReputation(1376, 42000);
                    player->SetReputation(1387, 42000);
                    player->SetReputation(1135, 42000);
                    player->SetReputation(1158, 42000);
                    player->SetReputation(1173, 42000);
                    player->SetReputation(1171, 42000);
                    player->SetReputation(1204, 42000);
                    if (player->GetTeam() == ALLIANCE)
                    {
                        player->SetReputation(1177, 42000);
                        player->SetReputation(1174, 42000);
                    }
                    else // Repu Horda
                    {
                        player->SetReputation(1172, 42000);
                        player->SetReputation(1178, 42000);
                    }
                    player->SetReputation(529, 42000);
                    player->GetSession()->SendNotification("|cff00FF00Se ha aumentado todas las Reputaciones The Lich King!");
                    return;
                    break;
                case Battlepay::RepCata:
                    if (!player)
                        player->SetReputation(1091, 42000);
                    player->SetReputation(1098, 42000);
                    player->SetReputation(1106, 42000);
                    player->SetReputation(1156, 42000);
                    player->SetReputation(1090, 42000);
                    player->SetReputation(1119, 42000);
                    player->SetReputation(1073, 42000);
                    player->SetReputation(1105, 42000);
                    player->SetReputation(1104, 42000);

                    if (player->GetTeam() == ALLIANCE)
                    {
                        player->SetReputation(1094, 42000);
                        player->SetReputation(1050, 42000);
                        player->SetReputation(1068, 42000);
                        player->SetReputation(1126, 42000);
                        player->SetReputation(1037, 42000);
                    }
                    else // Repu Horda
                    {
                        player->SetReputation(1052, 42000);
                        player->SetReputation(1067, 42000);
                        player->SetReputation(1124, 42000);
                        player->SetReputation(1064, 42000);
                        player->SetReputation(1085, 42000);
                    }
                    player->GetSession()->SendNotification("|cff00FF00Se ha aumentado todas las Reputaciones Cataclismo!");
                    return;
                    break;
                case Battlepay::RepPanda:
                    if (!player)
                        player->SetReputation(1216, 42000);
                    player->SetReputation(1435, 42000);
                    player->SetReputation(1277, 42000);
                    player->SetReputation(1359, 42000);
                    player->SetReputation(1275, 42000);
                    player->SetReputation(1492, 42000);
                    player->SetReputation(1281, 42000);
                    player->SetReputation(1283, 42000);
                    player->SetReputation(1279, 42000);
                    player->SetReputation(1273, 42000);
                    player->SetReputation(1341, 42000);
                    player->SetReputation(1345, 42000);
                    player->SetReputation(1337, 42000);
                    player->SetReputation(1272, 42000);
                    player->SetReputation(1351, 42000);
                    player->SetReputation(1302, 42000);
                    player->SetReputation(1269, 42000);
                    player->SetReputation(1358, 42000);
                    player->SetReputation(1271, 42000);
                    player->SetReputation(1282, 42000);
                    player->SetReputation(1440, 42000);
                    player->SetReputation(1270, 42000);
                    player->SetReputation(1278, 42000);
                    player->SetReputation(1280, 42000);
                    player->SetReputation(1276, 42000);

                    if (player->GetTeam() == ALLIANCE)
                    {
                        player->SetReputation(1242, 42000);
                        player->SetReputation(1376, 42000);
                        player->SetReputation(1387, 42000);

                    }
                    else // Repu Horda
                    {
                        player->SetReputation(1388, 42000);
                        player->SetReputation(1228, 42000);
                        player->SetReputation(1375, 42000);
                    }
                    player->GetSession()->SendNotification("|cff00FF00Se ha aumentado todas las Reputaciones de Pandaria!");
                    return;
                    break;
                case Battlepay::RepDraenor:
                    if (!player)
                        player->SetReputation(1850, 42000);
                    player->SetReputation(1515, 42000);
                    player->SetReputation(1520, 42000);
                    player->SetReputation(1732, 42000);
                    player->SetReputation(1735, 42000);
                    player->SetReputation(1741, 42000);
                    player->SetReputation(1849, 42000);
                    player->SetReputation(1737, 42000);
                    player->SetReputation(1711, 42000);
                    player->SetReputation(1736, 42000);
                    // Repu Alianza
                    if (player->GetTeam() == ALLIANCE)
                    {
                        player->SetReputation(1731, 42000);
                        player->SetReputation(1710, 42000);
                        player->SetReputation(1738, 42000);
                        player->SetReputation(1733, 42000);
                        player->SetReputation(1847, 42000);
                        player->SetReputation(1682, 42000);
                    }
                    else // Repu Horda
                    {
                        player->SetReputation(1740, 42000);
                        player->SetReputation(1681, 42000);
                        player->SetReputation(1445, 42000);
                        player->SetReputation(1708, 42000);
                        player->SetReputation(1848, 42000);
                        player->SetReputation(1739, 42000);
                    }
                    player->GetSession()->SendNotification("|cff00FF00Se ha aumentado todas las Reputaciones de Draenor!");
                    return;
                    break;
                case Battlepay::RepLegion:
                    if (!player)
                        player->SetReputation(1919, 42000);
                    player->SetReputation(1859, 42000);
                    player->SetReputation(1900, 42000);
                    player->SetReputation(1899, 42000);
                    player->SetReputation(1989, 42000);
                    player->SetReputation(1947, 42000);
                    player->SetReputation(1894, 42000);
                    player->SetReputation(1984, 42000);
                    player->SetReputation(1862, 42000);
                    player->SetReputation(1861, 42000);
                    player->SetReputation(1860, 42000);
                    player->SetReputation(1815, 42000);
                    player->SetReputation(1883, 42000);
                    player->SetReputation(1828, 42000);
                    player->SetReputation(1948, 42000);
                    player->SetReputation(2018, 42000);
                    player->SetReputation(1888, 42000);
                    player->SetReputation(2045, 42000);
                    player->SetReputation(2170, 42000);
                    player->SetReputation(2165, 42000);
                    player->GetSession()->SendNotification("|cff00FF00Se ha aumentado todas las Reputaciones de Legion!");
                    return;
                    break;
                case Battlepay::PremadePve:
                    if (!player)
                        // Bags
                        for (int slot = INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; slot++)
                            player->EquipNewItem(slot, 142075, ItemContext::NONE, true);

                    player->GiveLevel(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
                    player->InitTalentForLevel();
                    player->ModifyMoney(200000000);
                    player->LearnSpell(71810, true); // Montura vermis
                    player->EquipNewItem(EQUIPMENT_SLOT_NECK, 178827, ItemContext::NONE, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_BACK, 180123, ItemContext::NONE, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER1, 179355, ItemContext::NONE, true);
                    player->EquipNewItem(EQUIPMENT_SLOT_FINGER2, 179355, ItemContext::NONE, true);
                    player->LearnSpell(33388, true); // Equitacion
                    player->LearnSpell(33391, true);
                    player->LearnSpell(34090, true);
                    player->LearnSpell(34091, true);
                    player->LearnSpell(90265, true);
                    player->LearnSpell(54197, true);
                    player->LearnSpell(90267, true);
                    player->LearnSpell(115913, true);
                    player->LearnSpell(110406, true);
                    player->LearnSpell(104381, true);

                    if (player->GetClass() == CLASS_SHAMAN)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178692, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178703, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 178700, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178798, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178815, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178778, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178288, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178796, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178708, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178737, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 178712, ItemContext::NONE, true);
                        player->AddItem(179331, 1);
                        player->AddItem(178754, 1);
                    }
                    if (player->GetClass() == CLASS_HUNTER)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178692, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178703, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 178700, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178798, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178815, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178778, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178288, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178796, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 179331, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 180112, ItemContext::NONE, true);
                        player->AddItem(178735, 1);
                    }
                    if (player->GetClass() == CLASS_MAGE)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178759, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178704, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 180109, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178705, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 179335, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178761, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178740, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178831, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 178708, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 178772, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178829, ItemContext::NONE, true);
                    }
                    if (player->GetClass() == CLASS_PRIEST)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178759, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178704, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 180109, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178705, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 179335, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178761, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178740, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178831, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 178708, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 178772, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178829, ItemContext::NONE, true);
                    }
                    if (player->GetClass() == CLASS_WARLOCK)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178759, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178704, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 180109, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178705, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 179335, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178761, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178740, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178831, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 178708, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 178772, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178829, ItemContext::NONE, true);
                    }
                    if (player->GetClass() == CLASS_DEMON_HUNTER)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 180106, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178741, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 180111, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178832, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178835, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 180108, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178763, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178731, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178861, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178854, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 178479, ItemContext::NONE, true);
                    }
                    if (player->GetClass() == CLASS_ROGUE)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 180106, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178741, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 180111, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178832, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178835, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 180108, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178763, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178731, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178861, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178854, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 178479, ItemContext::NONE, true);
                    }
                    if (player->GetClass() == CLASS_MONK)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 180106, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178741, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 180111, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178832, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178835, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 180108, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178763, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178731, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178861, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178754, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 179328, ItemContext::NONE, true);
                        player->AddItem(180097, 1);
                        player->AddItem(180166, 1);
                    }
                    if (player->GetClass() == CLASS_DRUID)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 180106, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 178741, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 180111, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178832, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178835, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 180108, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178763, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 178731, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178861, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 179582, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_OFFHAND, 184528, ItemContext::NONE, true);
                        player->AddItem(180097, 1);
                        player->AddItem(180166, 1);
                    }
                    if (player->GetClass() == CLASS_WARRIOR)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178694, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 180113, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 179326, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178775, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178814, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178818, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178802, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 180101, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178861, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178780, ItemContext::NONE, true);
                        player->AddItem(178712, 1);
                        player->AddItem(178474, 1);
                    }
                    if (player->GetClass() == CLASS_PALADIN)
                    {
                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178694, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 180113, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 179326, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178775, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178814, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178818, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178802, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 180101, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178861, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178780, ItemContext::NONE, true);
                        player->AddItem(178712, 1);
                        player->AddItem(178474, 1);
                    }
                    if (player->GetClass() == CLASS_DEATH_KNIGHT)
                    {
                        if (Quest const* quest = sObjectMgr->GetQuestTemplate(12801)) // La luz del alba
                        {
                            player->AddQuest(quest, NULL);
                            player->CompleteQuest(quest->GetQuestId());
                            player->RewardQuest(quest, LootItemType::Item, 0, nullptr, false);
                        }
                        if (player->GetTeamId() == TEAM_ALLIANCE)
                            player->TeleportTo(0, -8829.8710f, 625.3872f, 94.1712f, 3.808243f);
                        else
                            player->TeleportTo(1, 1570.6693f, -4399.3388f, 16.0058f, 3.382241f);

                        player->LearnSpell(53428, true); // runeforging
                        player->LearnSpell(53441, true); // runeforging
                        player->LearnSpell(54586, true); // runeforging credit
                        player->LearnSpell(48778, true); //acherus deathcharger
                        player->LearnSkillRewardedSpells(776, 375, RACE_NONE);
                        player->LearnSkillRewardedSpells(960, 375, RACE_NONE);

                        player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 178694, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WRISTS, 180113, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_WAIST, 179326, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_HANDS, 178775, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 178814, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_LEGS, 178818, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_SHOULDERS, 178802, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_FEET, 180101, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET1, 179350, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_TRINKET2, 178861, ItemContext::NONE, true);
                        player->EquipNewItem(EQUIPMENT_SLOT_MAINHAND, 178780, ItemContext::NONE, true);
                        player->AddItem(178712, 1);
                        player->AddItem(178712, 1);
                    }
                    break;
                case Battlepay::VueloDL:
                    if (!player)
                        player->AddItem(128706, 1);
                    player->CompletedAchievement(sAchievementStore.LookupEntry(10018));
                    player->CompletedAchievement(sAchievementStore.LookupEntry(11190));
                    player->CompletedAchievement(sAchievementStore.LookupEntry(11446));
                    player->GetSession()->SendNotification("|cff00FF00Has aprendido poder volar en las Islas Abruptas, Costas Abruptas y Draenor");
                    break;
                    */
                    //default:
                        //break;
        }
    }
    /*
    if (!product->ScriptName.empty())
        sScriptMgr->OnBattlePayProductDelivery(_session, product);
        */
}

bool BattlepayManager::AlreadyOwnProduct(uint32 itemId) const
{
    auto const& player = _session->GetPlayer();
    if (player)
    {
        auto itemTemplate = sObjectMgr->GetItemTemplate(itemId);
        if (!itemTemplate)
            return true;

        for (auto itr : itemTemplate->Effects)
            if (itr->TriggerType == ITEM_SPELLTRIGGER_ON_LEARN && player->HasSpell(itr->SpellID))
                return true;

        uint8 inventoryEnd = INVENTORY_SLOT_ITEM_START + player->GetInventorySlotCount();
        for (uint8 i = INVENTORY_SLOT_ITEM_START; i < inventoryEnd; i++)
            if (player->GetItemCount(itemId))
                return true;
    }

    return false;
}

void BattlepayManager::SendProductList()
{
    WorldPackets::BattlePay::ProductListResponse response;
    Player* player = _session->GetPlayer(); // it's a false value if player is in character screen

    if (!IsAvailable())
    {
        response.Result = ProductListResult::LockUnk1;
        _session->SendPacket(response.Write());
        return;
    }

    response.Result = ProductListResult::Available;
    response.CurrencyID = GetShopCurrency() > 0 ? GetShopCurrency() : 1;

    // BATTLEPAY GROUP 
    for (auto& itr : sBattlePayDataStore->GetProductGroups())
    {
        WorldPackets::BattlePay::Group group;
        group.GroupId = itr.GroupId;
        group.IconFileDataID = itr.IconFileDataID;
        group.DisplayType = itr.DisplayType;
        group.Ordering = itr.Ordering;
        group.Unk = itr.Unk;
        group.MainGroupID = itr.MainGroupID;
        group.Name = itr.Name;
        group.Description = itr.Description;

        response.ProductGroups.emplace_back(group);
    }

    // BATTLEPAY SHOP
    for (auto& itr : sBattlePayDataStore->GetShopEntries())
    {
        WorldPackets::BattlePay::Shop shop;
        shop.EntryID = itr.EntryID;
        shop.GroupID = itr.GroupID;
        shop.ProductID = itr.ProductID;
        shop.Ordering = itr.Ordering;
        shop.VasServiceType = itr.VasServiceType;
        shop.StoreDeliveryType = itr.StoreDeliveryType;

        // shop entry and display entry must be the same
        auto data = WriteDisplayInfo(itr.Entry);
        if (std::get<0>(data))
        {
            shop.Display.emplace();
            shop.Display = std::get<1>(data);
        }

        // when logged out don't show everything
        if (!player && shop.StoreDeliveryType != 2)
            continue;

        auto productAddon = sBattlePayDataStore->GetProductAddon(itr.Entry);
        if (productAddon)
            if (productAddon->DisableListing > 0)
                continue;

        response.Shops.emplace_back(shop);
    }

    // BATTLEPAY PRODUCT INFO
    for (auto& itr : sBattlePayDataStore->GetProductInfos())
    {
        auto& productInfo = itr.second;

        auto productAddon = sBattlePayDataStore->GetProductAddon(productInfo.Entry);
        if (productAddon)
            if (productAddon->DisableListing > 0)
                continue;

        WorldPackets::BattlePay::ProductInfo productinfo;
        productinfo.ProductId = productInfo.ProductId;
        productinfo.NormalPriceFixedPoint = productInfo.NormalPriceFixedPoint;
        productinfo.CurrentPriceFixedPoint = productInfo.CurrentPriceFixedPoint;
        productinfo.ProductIds = productInfo.ProductIds;
        productinfo.Unk1 = productInfo.Unk1;
        productinfo.Unk2 = productInfo.Unk2;
        productinfo.UnkInts = productInfo.UnkInts;
        productinfo.Unk3 = productInfo.Unk3;
        productinfo.ChoiceType = productInfo.ChoiceType;

        // productinfo entry and display entry must be the same
        auto data = WriteDisplayInfo(productInfo.Entry);
        if (std::get<0>(data))
        {
            productinfo.Display.emplace();
            productinfo.Display = std::get<1>(data);
        }

        response.ProductInfos.emplace_back(productinfo);
    }

    for (auto& itr : sBattlePayDataStore->GetProducts())
    {
        auto& product = itr.second;
        auto productInfo = sBattlePayDataStore->GetProductInfoForProduct(product.ProductId);

        auto productAddon = sBattlePayDataStore->GetProductAddon(productInfo->Entry);
        if (productAddon)
            if (productAddon->DisableListing > 0)
                continue;

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
            for (auto item : *sBattlePayDataStore->GetItemsOfProduct(product.ProductId))
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

                if (sBattlePayDataStore->DisplayInfoExist(productInfo->Entry))
                {
                    // productinfo entry and display entry must be the same
                    auto data = WriteDisplayInfo(productInfo->Entry);
                    if (std::get<0>(data))
                    {
                        pItem.Display.emplace();
                        pItem.Display = std::get<1>(data);
                    }
                }
                else
                    TC_LOG_INFO("server.battlepay", "Can't find displayinfo for product {} with product info entry: {}", product.Name, productInfo->Entry);

                pProduct.Items.emplace_back(pItem);
            }

        // productinfo entry and display entry must be the same
        auto data = WriteDisplayInfo(productInfo->Entry);
        if (std::get<0>(data))
        {
            pProduct.Display.emplace();
            pProduct.Display = std::get<1>(data);
        }

        response.Products.emplace_back(pProduct);

    }

    /*
    // debug
    TC_LOG_INFO("server.BattlePay", "SendProductList with {} ProductInfos, {} Products, {} Shops. CurrencyID: {}.", response.ProductInfos.size(), response.Products.size(), response.Shops.size(), response.CurrencyID);
    for (int i = 0; i != response.ProductInfos.size(); i++)
    {
        TC_LOG_INFO("server.BattlePay", "({}) ProductInfo: ProductId [%zu], First SubProductId [%zu], CurrentPriceFixedPoint [%lu]", i, response.ProductInfos[i].ProductId, response.ProductInfos[i].ProductIds[0], response.ProductInfos[i].CurrentPriceFixedPoint);
        TC_LOG_INFO("server.BattlePay", "({}) Products: ProductId [%zu], UnkString [{}]", i, response.Products[i].ProductId, response.Products[i].UnkString.c_str());
        TC_LOG_INFO("server.BattlePay", "({}) Shops: ProductId [%zu]", i, response.Shops[i].ProductID);
    }
    */

    _session->SendPacket(response.Write());
}

std::tuple<bool, WorldPackets::BattlePay::DisplayInfo> BattlepayManager::WriteDisplayInfo(uint32 displayInfoEntry)
{
    auto qualityColor = [](uint32 displayInfoOrProductInfoEntry) -> std::string
    {
        auto productAddon = sBattlePayDataStore->GetProductAddon(displayInfoOrProductInfoEntry);
        if (!productAddon)
           return "|cffffffff";

        switch (sBattlePayDataStore->GetProductAddon(displayInfoOrProductInfoEntry)->NameColorIndex)
        {
        case 0:
            return "|cffffffff";
        case 1:
            return "|cff1eff00";
        case 2:
            return "|cff0070dd";
        case 3:
            return "|cffa335ee";
        case 4:
            return "|cffff8000";
        case 5:
            return "|cffe5cc80";
        case 6:
            return "|cffe5cc80";
        default:
            return "|cffffffff";
        }
    };

    auto info = WorldPackets::BattlePay::DisplayInfo();

    auto displayInfo = sBattlePayDataStore->GetDisplayInfo(displayInfoEntry);
    if (!displayInfo)
        return std::make_tuple(false, info);

    info.CreatureDisplayID = displayInfo->CreatureDisplayID;
    info.VisualID = displayInfo->VisualID;
    info.Name1 = qualityColor(displayInfoEntry) + displayInfo->Name1;
    info.Name2 = displayInfo->Name2;
    info.Name3 = displayInfo->Name3;
    info.Name4 = displayInfo->Name4;
    info.Name5 = displayInfo->Name5;
    info.Name6 = displayInfo->Name6;
    info.Name7 = displayInfo->Name7;
    info.Flags = displayInfo->Flags;
    info.Unk1 = displayInfo->Unk1;
    info.Unk2 = displayInfo->Unk2;
    info.Unk3 = displayInfo->Unk3;
    info.UnkInt1 = displayInfo->UnkInt1;
    info.UnkInt2 = displayInfo->UnkInt2;
    info.UnkInt3 = displayInfo->UnkInt3;

    for (int v = 0; v < displayInfo->Visuals.size(); v++)
    {
        BattlePayData::Visual visual = displayInfo->Visuals[v];

        if (!visual.Entry)
            visual = *sBattlePayDataStore->FindVisualForDisplayInfo(displayInfoEntry);

        WorldPackets::BattlePay::Visual _Visual;
        _Visual.Name = visual.Name;
        _Visual.DisplayId = visual.DisplayId;
        _Visual.VisualId = visual.VisualId;
        _Visual.Unk = visual.Unk;

        info.Visuals.push_back(_Visual);
    }

    if (displayInfo->Flags)
        info.Flags = displayInfo->Flags;

    return std::make_tuple(true, info);
}

void BattlepayManager::SendAccountCredits()
{
    auto sessionId = _session->GetAccountId();

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BATTLE_PAY_ACCOUNT_CREDITS);
    stmt->setUInt32(0, _session->GetAccountId());
    PreparedQueryResult result = LoginDatabase.Query(stmt);

    auto sSession = sWorld->FindSession(sessionId);
    if (!sSession)
        return;

    uint64 balance = 0;
    if (result)
        balance = result->Fetch()[0].GetUInt32();

    auto player = sSession->GetPlayer();
    if (!player)
        return;

    SendBattlePayMessage(2, "");
}

void BattlepayManager::SendBattlePayDistribution(uint32 productId, uint8 status, uint64 distributionId, ObjectGuid targetGuid)
{
    WorldPackets::BattlePay::DistributionUpdate distributionBattlePay;
    auto product = *sBattlePayDataStore->GetProduct(productId);

    auto productInfo = *sBattlePayDataStore->GetProductInfoForProduct(productId);

    distributionBattlePay.DistributionObject.DistributionID = distributionId;
    distributionBattlePay.DistributionObject.Status = status;
    distributionBattlePay.DistributionObject.ProductID = productId;
    distributionBattlePay.DistributionObject.Revoked = false; // not needed for us

    if (!targetGuid.IsEmpty())
    {
        distributionBattlePay.DistributionObject.TargetPlayer = targetGuid;
        distributionBattlePay.DistributionObject.TargetVirtualRealm = GetVirtualRealmAddress();
        distributionBattlePay.DistributionObject.TargetNativeRealm = GetVirtualRealmAddress();
    }

    WorldPackets::BattlePay::Product productData;
    
    productData.ProductId = product.ProductId;
    productData.Type = product.Type;
    productData.Flags = product.Flags;
    productData.Unk1 = product.Unk1;
    productData.DisplayId = product.DisplayId;
    productData.ItemId = product.ItemId;
    productData.Unk4 = product.Unk4;
    productData.Unk5 = product.Unk5;
    productData.Unk6 = product.Unk6;
    productData.Unk7 = product.Unk7;
    productData.Unk8 = product.Unk8;
    productData.Unk9 = product.Unk9;
    productData.UnkString = product.UnkString;
    productData.UnkBit = product.UnkBit;
    productData.UnkBits = product.UnkBits;
    
    for (auto item : *sBattlePayDataStore->GetItemsOfProduct(product.ProductId))
    {
        WorldPackets::BattlePay::ProductItem productItem;

        productItem.ID = item.ID;
        productItem.UnkByte = item.UnkByte;
        productItem.ItemID = item.ItemID;
        productItem.Quantity = item.Quantity;
        productItem.UnkInt1 = item.UnkInt1;
        productItem.UnkInt2 = item.UnkInt2;
        productItem.IsPet = item.IsPet;
        productItem.PetResult = item.PetResult;

        auto data = WriteDisplayInfo(productInfo.Entry);
        if (std::get<0>(data))
        {
            productItem.Display.emplace();
            productItem.Display = std::get<1>(data);
        }
    }

    auto data = WriteDisplayInfo(productInfo.Entry);
    if (std::get<0>(data))
    {
        productData.Display.emplace();
        productData.Display = std::get<1>(data);
    }

    distributionBattlePay.DistributionObject.Product = std::move(productData);
    _session->SendPacket(distributionBattlePay.Write());
}

void BattlepayManager::AssignDistributionToCharacter(ObjectGuid const& targetCharGuid, uint64 distributionId, uint32 productId, uint16 specId, uint16 choiceId)
{
    WorldPackets::BattlePay::UpgradeStarted upgrade;
    upgrade.CharacterGUID = targetCharGuid;
    _session->SendPacket(upgrade.Write());

    WorldPackets::BattlePay::BattlePayStartDistributionAssignToTargetResponse assignResponse;
    assignResponse.DistributionID = distributionId;
    assignResponse.unkint1 = specId;
    assignResponse.unkint2 = choiceId;
    _session->SendPacket(upgrade.Write());

    auto purchase = GetPurchase();
    purchase->Status = Battlepay::DistributionStatus::BATTLE_PAY_DIST_STATUS_ADD_TO_PROCESS;

    SendBattlePayDistribution(productId, purchase->Status, distributionId, targetCharGuid);
}

void BattlepayManager::Update(uint32 diff)
{
    TC_LOG_INFO("server.BattlePay", "BattlepayManager::Update");
    /*
    auto& data = _actualTransaction;
    auto product = sBattlePayDataStore->GetProduct(data.ProductID);

    switch (data.Status)
    {
    case Battlepay::Properties::DistributionStatus::BATTLE_PAY_DIST_STATUS_ADD_TO_PROCESS:
    {
        
        switch (product->Type)
        {
        case CharacterBoost:
        {
            auto const& player = data.TargetCharacter;
            if (!player)
                break;

            WorldPackets::BattlePay::BattlePayCharacterUpgradeQueued responseQueued;
            responseQueued.EquipmentItems = sDB2Manager.GetItemLoadOutItemsByClassID(player->getClass(), 3)[0];
            responseQueued.Character = data.TargetCharacter;
            _session->SendPacket(responseQueued.Write());

            data.Status = DistributionStatus::BATTLE_PAY_DIST_STATUS_PROCESS_COMPLETE;
            SendBattlePayDistribution(data.ProductID, data.Status, data.DistributionId, data.TargetCharacter);
            break;
        }
        default:
            break;
        }
        break;
        
    }
    case Battlepay::Properties::DistributionStatus::BATTLE_PAY_DIST_STATUS_PROCESS_COMPLETE: //send SMSG_BATTLE_PAY_VAS_PURCHASE_STARTED
    {
        switch (product->WebsiteType)
        {
        case CharacterBoost:
        {
            data.Status = DistributionStatus::BATTLE_PAY_DIST_STATUS_FINISHED;
            SendBattlePayDistribution(data.ProductID, data.Status, data.DistributionId, data.TargetCharacter);
            break;
        }
        default:
            break;
        }
        break;
    }
    case Battlepay::Properties::DistributionStatus::BATTLE_PAY_DIST_STATUS_FINISHED:
    {
        switch (product->WebsiteType)
        {
        case CharacterBoost:
            SendBattlePayDistribution(data.ProductID, data.Status, data.DistributionId, data.TargetCharacter);
            break;
        default:
            break;
        }
        break;
    }
    case Battlepay::Properties::DistributionStatus::BATTLE_PAY_DIST_STATUS_AVAILABLE:
    case Battlepay::Properties::DistributionStatus::BATTLE_PAY_DIST_STATUS_NONE:
    default:
        break;
    }
    */
}

void BattlepayManager::SendBattlePayMessage(uint32 bpaymessageID, std::string name, uint32 value) const
{
    std::ostringstream msg;
    if (bpaymessageID == 1)
        msg << "The purchase '" << name << "' was successful!";
    if (bpaymessageID == 2)
        msg << "Remaining credits: " << GetBattlePayCredits() / 10000 << " .";

    if (bpaymessageID == 10)
        msg << "You cannot purchase '" << name << "' . Contact a game master to find out more.";
    if (bpaymessageID == 11)
        msg << "Your bags are too full to add : " << name << " .";
    if (bpaymessageID == 12)
        msg << "You have already purchased : " << name << " .";

    if (bpaymessageID == 20)
        msg << "The battle pay credits have been updated for the character '" << name << "' ! Available credits:" << value << " .";
    if (bpaymessageID == 21)
        msg << "You must enter an amount !";
    if (bpaymessageID == 3)
        msg << "You have now '" << value << "' credits.";

    ChatHandler(_session).SendSysMessage(msg.str().c_str());
}

uint32 BattlepayManager::GetBattlePayCredits() const
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BATTLE_PAY_ACCOUNT_CREDITS);

    stmt->setUInt32(0, _session->GetBattlenetAccountId());

    PreparedQueryResult result_don = LoginDatabase.Query(stmt);

    if (!result_don)
        return 0;

    Field* fields = result_don->Fetch();
    uint32 credits = fields[0].GetUInt32();

    return credits * 10000; // currency precision .. in retail it like gold and copper .. 10 usd is 100000 battlepay credit
}

bool BattlepayManager::HasBattlePayCredits(uint32 count) const
{
    if (GetBattlePayCredits() >= count)
        return true;

    ChatHandler chH = ChatHandler(_session);
    chH.PSendSysMessage(20000, count);
    return false;
}

bool BattlepayManager::UpdateBattlePayCredits(uint64 price) const
{
    //TC_LOG_INFO("server.BattlePay", "UpdateBattlePayCredits: GetBattlePayCredits(): {} - price: %lu", GetBattlePayCredits(), price);
    uint64 calcCredit = (GetBattlePayCredits() - price) / 10000;
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BATTLE_PAY_ACCOUNT_CREDITS);
    stmt->setUInt64(0, calcCredit);
    stmt->setUInt32(1, _session->GetBattlenetAccountId());
    LoginDatabase.Execute(stmt);

    return true;
}

bool BattlepayManager::ModifyBattlePayCredits(uint64 credits) const
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BATTLE_PAY_ACCOUNT_CREDITS);
    stmt->setUInt64(0, credits);
    stmt->setUInt32(1, _session->GetBattlenetAccountId());
    LoginDatabase.Execute(stmt);
    SendBattlePayMessage(3, "", credits);

    return true;
}

void BattlepayManager::SendBattlePayBattlePetDelivered(ObjectGuid petguid, uint32 creatureID) const
{
    WorldPackets::BattlePay::BattlePayBattlePetDelivered response;
    response.DisplayID = creatureID;
    response.BattlePetGuid = petguid;
    _session->SendPacket(response.Write());
    TC_LOG_ERROR("", "Send BattlePayBattlePetDelivered guid: %lu && creatureID: {}", petguid.GetCounter(), creatureID);
}

void BattlepayManager::AddBattlePetFromBpayShop(uint32 battlePetCreatureID) const
{
     //if (BattlePetSpeciesEntry const* speciesEntry = BattlePets::BattlePetMgr::GetBattlePetSpeciesByCreature(battlePetCreatureID))
     //{
     //   // _session->GetBattlePetMgr()->AddPet(speciesEntry->ID, BattlePets::BattlePetMgr::SelectPetDisplay(speciesEntry),
     //     //   BattlePets::BattlePetMgr::RollPetBreed(speciesEntry->ID), BattlePets::BattlePetMgr::GetDefaultPetQuality(speciesEntry->ID));

     //     //it gives back false information need to get the pet guid from the add pet method somehow
     //    SendBattlePayBattlePetDelivered(ObjectGuid::Create<HighGuid::BattlePet>(sObjectMgr->GetGenerator<HighGuid::BattlePet>().Generate()), speciesEntry->CreatureID);
     //}
}
