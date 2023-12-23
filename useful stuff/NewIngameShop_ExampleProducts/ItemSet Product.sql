-- ItemSet Product

SET
-- Customizable data:
@IconFileID = 3883816,
@Name = 'Soulbreaker\'s Ebony Vestments',  -- https://www.wowhead.com/item-set=1462/soulbreakers-ebony-vestments
@Description = 'A Warlock item set with 5 pieces. Has set bonuses at 2 and 4 pieces.',
@NormalPrice = 100000,
@CurrentPrice = 100000,
@ItemSet = 1462,
@Ordering = 1,
@GroupID = 139,

-- Autofill data:
@ProductEntry = (SELECT MAX(`Entry`) FROM `battlepay_product`) + 1,
@ShopLinkID = (SELECT MAX(`ProductID`) FROM `battlepay_shop`) + 1,
@ShopEntryID = (SELECT MAX(`EntryID`) FROM `battlepay_shop`) + 1,
@ProductInfoLinkID = (SELECT MAX(`ProductID`) FROM `battlepay_productinfo`) + 1,
@DisplayInfoEntry = (SELECT MAX(`Entry`) FROM `battlepay_displayinfo`) + 1;
-- ----------------------
INSERT INTO `battlepay_displayinfo` (`Entry`, `CreatureDisplayID`, `VisualID`, `Name1`, `Name2`, `Name3`, `Name4`, `Name5`, `Name6`, `Name7`, `Flags`, `Unk1`, `Unk2`, `Unk3`, `UnkInt1`, `UnkInt2`, `UnkInt3`) VALUES 
(@DisplayInfoEntry, @IconFileID, 0, @Name, '', @Description, '', '', '', '', 0, 0, 0, 0, 0, 0, 0);

INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(@ProductEntry, @ProductInfoLinkID, 100, @ItemSet, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, @Name);

INSERT INTO `battlepay_productinfo` (`Entry`, `ProductId`, `NormalPriceFixedPoint`, `CurrentPriceFixedPoint`, `ProductIds`, `Unk1`, `Unk2`, `UnkInts`, `Unk3`, `ChoiceType`) VALUES 
(@DisplayInfoEntry, @ShopLinkID, @NormalPrice, @CurrentPrice, @ProductInfoLinkID, 15, 0, 0, 0, 14);

INSERT INTO `battlepay_shop` (`Entry`, `EntryID`, `GroupID`, `ProductID`, `Ordering`, `VasServiceType`, `StoreDeliveryType`) VALUES 
(@DisplayInfoEntry, @ShopEntryID, @GroupID, @ShopLinkID, @Ordering, 0, 0);