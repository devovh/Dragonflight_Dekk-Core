--
-- Mount+Pet featured bundle with full size product card
--
SET
-- DisplayInfo Data
@DisplayInfoEntry = 100, -- must have been unique in all tables
@Name = "Wen Lo, The River\'s Edge + Drakks",
@Description = "Get the eye of the tiger when you hop into the saddle of Wen Lo, the River\'s Edge, a fearsome mount who will help you embrace the thrill of the flight. Get this rawrsome tiger + Drakks a new pet from Dragonflight!', 'Includes: - Wen Lo, The River\'s Edge mount - Drakks pet",
-- Visual Data (only need if it's new)
@MountVisualEntry = 100,
@MountVisualName = "Wen Lo, The River\'s Edge",
@MountCreatureDisplayID = 102038,
@PetVisualEntry = 101, 
@PetVisualName = "Drakks",
@PetCreatureDisplayID = 102304,
-- Product Data
@ProductInfoID = 1000,
@NormalPrice = 779400,
@CurrentPrice = 659400,
@BundleProductIDs = "1000, 1001", -- as string
@MountProductID = 1000,
@MountName = "Wen Lo, The River\'s Edge",
@MountSpellID = 359317,
@PetProductID = 1001,
@PetName = "Drakks",
@PetItemID = 359317,
@GroupID = 158; -- "Featured" group id: 158


DELETE FROM `battlepay_displayinfo` WHERE `Entry`=@DisplayInfoEntry;
INSERT INTO `battlepay_displayinfo` (`Entry`, `CreatureDisplayID`, `VisualID`, `Name1`, `Name2`, `Name3`, `Name4`, `Name5`, `Name6`, `Name7`, `Flags`, `Unk1`, `Unk2`, `Unk3`, `UnkInt1`, `UnkInt2`, `UnkInt3`) VALUES 
(@DisplayInfoEntry, 0, 281, @Name, "", @Description, "", "", "", "", 16, 0, 13323, 0, 4, 0, 0);
DELETE FROM `battlepay_visual` WHERE `Entry`=@PetVisualEntry;
INSERT INTO `battlepay_visual` (`Entry`, `DisplayInfoEntry`, `Name`, `DisplayId`, `VisualId`, `Unk`) VALUES 
(@PetVisualEntry, @DisplayInfoEntry, @PetVisualName, @PetCreatureDisplayID, 10, 0);
DELETE FROM `battlepay_visual` WHERE `Entry`=@MountVisualEntry;
INSERT INTO `battlepay_visual` (`Entry`, `DisplayInfoEntry`, `Name`, `DisplayId`, `VisualId`, `Unk`) VALUES 
(@MountVisualEntry, @DisplayInfoEntry, @MountVisualName, @MountCreatureDisplayID, 284, 0);
DELETE FROM `battlepay_productinfo` WHERE `Entry`=@DisplayInfoEntry;
INSERT INTO `battlepay_productinfo` (`Entry`, `ProductId`, `NormalPriceFixedPoint`, `CurrentPriceFixedPoint`, `ProductIds`, `Unk1`, `Unk2`, `UnkInts`, `Unk3`, `ChoiceType`) VALUES 
(@DisplayInfoEntry, @ProductInfoID, @NormalPrice, @CurrentPrice, @BundleProductIDs, 15, 0, 717, 0, 18);
DELETE FROM `battlepay_shop` WHERE `Entry`=@DisplayInfoEntry;
INSERT INTO `battlepay_shop` (`Entry`, `EntryID`, `GroupID`, `ProductID`, `Ordering`, `VasServiceType`, `StoreDeliveryType`) VALUES 
(@DisplayInfoEntry, 334, @GroupID, @ProductInfoID, 0, 0, 0);
DELETE FROM `battlepay_product` WHERE `Entry`=@DisplayInfoEntry;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(@DisplayInfoEntry, @MountProductID, 3, 0, 0, @MountSpellID, 0, 0, 2, 0, 0, 0, 0, 17, 0, 0, @MountName);
DELETE FROM `battlepay_product` WHERE `Entry`=@DisplayInfoEntry+1;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(@DisplayInfoEntry+1, @MountProductID, 2, 0, 0, 0, @PetItemID, 0, 2, 0, 0, 0, 0, 17, 0, 0, @PetName);