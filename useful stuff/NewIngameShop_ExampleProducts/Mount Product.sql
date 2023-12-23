-- Mount Product with DisplayInfo

SET
-- Customizable data:
@IconFileID = 3939983,
@Name = 'Wandering Ancient',
@Description = 'A staunch defender of the forests of Azeroth, the ancients are also known for their glorious canopies of leaves, which change color with the seasons.',
@CreatureDisplayID = 100463,
@MountSpellID = 348162,
@NormalPrice = 250000,
@CurrentPrice = 250000,
@Ordering = 4, -- (SELECT MAX(`Ordering`) FROM `battlepay_shop`) + 1,
@VisualPositionID = 72,
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
(@ProductEntry, @ProductInfoLinkID, 3, 0, 0, @MountSpellID, 0, 0, 2, 0, 0, 0, 0, 19, 0, 0, @Name);

INSERT INTO `battlepay_productinfo` (`Entry`, `ProductId`, `NormalPriceFixedPoint`, `CurrentPriceFixedPoint`, `ProductIds`, `Unk1`, `Unk2`, `UnkInts`, `Unk3`, `ChoiceType`) VALUES 
(@DisplayInfoEntry, @ShopLinkID, @NormalPrice, @CurrentPrice, @ProductInfoLinkID, 15, 0, 0, 0, 3);

INSERT INTO `battlepay_shop` (`Entry`, `EntryID`, `GroupID`, `ProductID`, `Ordering`, `VasServiceType`, `StoreDeliveryType`) VALUES 
(@DisplayInfoEntry, @ShopEntryID, 15, @ShopLinkID, @Ordering, 0, 0);
-- ----------------------