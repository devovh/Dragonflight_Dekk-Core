-- Transmog Product with DisplayInfo

SET
-- Customizable data:
@IconFileID = 3752758,
@Name = 'Vestments of the Eternal Traveler',
@Description = 'Vestments of the Eternal Traveler',
@NormalPrice = 200000,
@CurrentPrice = 200000,
@VisualPositionID_card = 420,
@VisualPositionID_inspect = 293,
@TransmogSetID = 1913,
@Ordering = 4,
@GroupID = 139,
-- Autofill data:
@ProductEntry = (SELECT MAX(`Entry`) FROM `battlepay_product`) + 1,
@ShopLinkID = (SELECT MAX(`ProductID`) FROM `battlepay_shop`) + 1,
@ShopEntryID = (SELECT MAX(`EntryID`) FROM `battlepay_shop`) + 1,
@ProductInfoLinkID = (SELECT MAX(`ProductID`) FROM `battlepay_productinfo`) + 1,
@DisplayInfoEntry = (SELECT MAX(`Entry`) FROM `battlepay_displayinfo`) + 1,
@VisualEntry = (SELECT MAX(`Entry`) FROM `battlepay_visual`) + 1;
-- ----------------------
INSERT INTO `battlepay_displayinfo` (`Entry`, `CreatureDisplayID`, `VisualID`, `Name1`, `Name2`, `Name3`, `Name4`, `Name5`, `Name6`, `Name7`, `Flags`, `Unk1`, `Unk2`, `Unk3`, `UnkInt1`, `UnkInt2`, `UnkInt3`) VALUES 
(@DisplayInfoEntry, @IconFileID, @VisualPositionID_card, @Name, '', @Description, '', '', '', '', 16, 0, 0, 0, 9, 0, 0);

INSERT INTO `battlepay_visual` (`Entry`, `DisplayInfoEntry`, `Name`, `DisplayId`, `VisualId`, `Unk`) VALUES 
(@VisualEntry, @DisplayInfoEntry, @Name, 0, @VisualPositionID_inspect, @TransmogSetID);

INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(@ProductEntry, @ProductInfoLinkID, 26, 0, 0, 0, 0, 0, 2, 0, @TransmogSetID, 0, 0, 29, 0, 0, @Name);

INSERT INTO `battlepay_productinfo` (`Entry`, `ProductId`, `NormalPriceFixedPoint`, `CurrentPriceFixedPoint`, `ProductIds`, `Unk1`, `Unk2`, `UnkInts`, `Unk3`, `ChoiceType`) VALUES 
(@DisplayInfoEntry, @ShopLinkID, @NormalPrice, @CurrentPrice, @ProductInfoLinkID, 15, 0, 0, 0, 14);

INSERT INTO `battlepay_shop` (`Entry`, `EntryID`, `GroupID`, `ProductID`, `Ordering`, `VasServiceType`, `StoreDeliveryType`) VALUES 
(@DisplayInfoEntry, @ShopEntryID, @GroupID, @ShopLinkID, @Ordering, 0, 0);
-- ----------------------

