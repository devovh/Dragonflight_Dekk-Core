-- Pet Product with DisplayInfo

SET
-- Customizable data:
@IconFileID = 3038273,
@Name = 'Anima Wyrmling',
@Description = 'Even the smallest creatures in the Shadowlands rely on anima to survive.',
@CreatureDisplayID = 93349,
@BattlePetCreatureID = 157969,
@NormalPrice = 100000,
@CurrentPrice = 100000,
@Ordering = 0, -- (SELECT MAX(`Ordering`) FROM `battlepay_shop`) + 1,
@VisualPositionID = 10, -- for pet it's always 10
-- Autofill data:
@ProductEntry = (SELECT MAX(`Entry`) FROM `battlepay_product`) + 1,
@ShopLinkID = (SELECT MAX(`ProductID`) FROM `battlepay_shop`) + 1,
@ShopEntryID = (SELECT MAX(`EntryID`) FROM `battlepay_shop`) + 1,
@ProductInfoLinkID = (SELECT MAX(`ProductID`) FROM `battlepay_productinfo`) + 1,
@DisplayInfoEntry = (SELECT MAX(`Entry`) FROM `battlepay_displayinfo`) + 1,
@VisualEntry = (SELECT MAX(`Entry`) FROM `battlepay_visual`) + 1;
-- ----------------------
INSERT INTO `battlepay_displayinfo` (`Entry`, `CreatureDisplayID`, `VisualID`, `Name1`, `Name2`, `Name3`, `Name4`, `Name5`, `Name6`, `Name7`, `Flags`, `Unk1`, `Unk2`, `Unk3`, `UnkInt1`, `UnkInt2`, `UnkInt3`) VALUES 
(@DisplayInfoEntry, @IconFileID, @VisualPositionID, @Name, '', @Description, '', '', '', '', 0, 0, 0, 0, 0, 0, 0);

INSERT INTO `battlepay_visual` (`Entry`, `DisplayInfoEntry`, `Name`, `DisplayId`, `VisualId`, `Unk`) VALUES 
(@VisualEntry, @DisplayInfoEntry, @Name, @CreatureDisplayID, @VisualPositionID, 0);

INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(@ProductEntry, @ProductInfoLinkID, 2, 0, 0, 0, @BattlePetCreatureID, 0, 2, 0, 0, 0, 0, 19, 0, 0, @Name);

INSERT INTO `battlepay_productinfo` (`Entry`, `ProductId`, `NormalPriceFixedPoint`, `CurrentPriceFixedPoint`, `ProductIds`, `Unk1`, `Unk2`, `UnkInts`, `Unk3`, `ChoiceType`) VALUES 
(@DisplayInfoEntry, @ShopLinkID, @NormalPrice, @CurrentPrice, @ProductInfoLinkID, 47, 0, 0, 0, 2);

INSERT INTO `battlepay_shop` (`Entry`, `EntryID`, `GroupID`, `ProductID`, `Ordering`, `VasServiceType`, `StoreDeliveryType`) VALUES 
(@DisplayInfoEntry, @ShopEntryID, 13, @ShopLinkID, @Ordering, 0, 0);
-- ----------------------