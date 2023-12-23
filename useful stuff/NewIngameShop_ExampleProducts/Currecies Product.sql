-- Currencies Product

SET
-- Customizable data:
@IconFileID = 3743738,
@Name = 'Soul Ash',
@Description = 'A raw source of power found within Torghast. Used to fuel the Runecarver\'s Chamber.',
@NormalPrice = 50000,
@CurrentPrice = 50000,
@CurrencyID = 1828, -- https://www.wowhead.com/currency=1828/soul-ash
@CurrencyCount = 500,
@Ordering = 2,
@GroupID = 10,

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
(@ProductEntry, @ProductInfoLinkID, 31, @CurrencyID, @CurrencyCount, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, @Name);

INSERT INTO `battlepay_productinfo` (`Entry`, `ProductId`, `NormalPriceFixedPoint`, `CurrentPriceFixedPoint`, `ProductIds`, `Unk1`, `Unk2`, `UnkInts`, `Unk3`, `ChoiceType`) VALUES 
(@DisplayInfoEntry, @ShopLinkID, @NormalPrice, @CurrentPrice, @ProductInfoLinkID, 15, 0, 0, 0, 14);

INSERT INTO `battlepay_shop` (`Entry`, `EntryID`, `GroupID`, `ProductID`, `Ordering`, `VasServiceType`, `StoreDeliveryType`) VALUES 
(@DisplayInfoEntry, @ShopEntryID, @GroupID, @ShopLinkID, @Ordering, 0, 0);
-- ----------------------
