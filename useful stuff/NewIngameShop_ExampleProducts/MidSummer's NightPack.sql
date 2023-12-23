--
-- Example Big Bundle With only one promo picture full size product card without visuals
-- MidSummer Night's Pack ( last day was 07.01. )
--

-- WARNING! It's just a backup but you have to add new unique entry id's if you want to use it.

DELETE FROM `battlepay_displayinfo` WHERE `Entry`=61;
INSERT INTO `battlepay_displayinfo` (`Entry`, `CreatureDisplayID`, `VisualID`, `Name1`, `Name2`, `Name3`, `Name4`, `Name5`, `Name6`, `Name7`, `Flags`, `Unk1`, `Unk2`, `Unk3`, `UnkInt1`, `UnkInt2`, `UnkInt3`) VALUES 
(61, 0, 258, 'A Midsummer Night\'s Pack', '', 'This is what dreams are made of! Start off the summer right with an epic 10-item collection of three playful pets, including a brand-new Lil\' Ursoc, two enchanting transmog sets, four dreamy mounts, and a whimsical toy. Offer valid through July 4, 2022.', 'If you already own one or more items from A Midsummer Night\'s Pack, its price will automatically adjust. - Pet: Lil\' Ursoc (new!) - Pet: Blinky - Pet: Blossoming Ancient - Mount: Sunwarmed Furline - Mount: Sylverian Dreamer - Mount: Enchanted Fey Dragon - Mount: Celestial Steed - Transmog set: Celestial Observer\'s Ensemble - Transmog set: Sprite Darter\'s Wings - Toy: Transmorpher Beacon', '', '', '', 16, 0, 14288, 0, 4, 0, 0);

DELETE FROM `battlepay_productinfo` WHERE `Entry`=61;
INSERT INTO `battlepay_productinfo` (`Entry`, `ProductId`, `NormalPriceFixedPoint`, `CurrentPriceFixedPoint`, `ProductIds`, `Unk1`, `Unk2`, `UnkInts`, `Unk3`, `ChoiceType`) VALUES 
(61, 747, 1800000, 649900, '274,272,640,550,620,646,266,581,617,549', 32783, 0, 734, 0, 14);

DELETE FROM `battlepay_product` WHERE `Entry`=11;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(11, 646, 2, 0, 1, 0, 185586, 0, 2, 0, 0, 0, 0, 10, 0, 0, 'Lil\' Ursoc');
DELETE FROM `battlepay_product` WHERE `Entry`=12;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(12, 620, 2, 0, 0, 0, 179589, 0, 2, 0, 0, 0, 0, 6, 0, 0, 'Blinky');
DELETE FROM `battlepay_product` WHERE `Entry`=13;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(13, 640, 3, 0, 0, 317177, 0, 0, 2, 0, 0, 0, 0, 17, 0, 0, 'Sunwarmed Furline');
DELETE FROM `battlepay_product` WHERE `Entry`=14;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(14, 581, 26, 0, 0, 0, 0, 0, 2, 0, 1914, 0, 0, 21, 0, 0, 'Sprite Darter\'s Wings');
DELETE FROM `battlepay_product` WHERE `Entry`=15;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(15, 617, 26, 0, 0, 0, 0, 0, 2, 0, 2200, 0, 0, 29, 0, 0, 'Celestial Observer\'s Ensemble');
DELETE FROM `battlepay_product` WHERE `Entry`=54;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(54, 549, 3, 0, 0, 290132, 0, 0, 2, 0, 0, 0, 0, 17, 0, 0, 'Sylverian Dreamer');
DELETE FROM `battlepay_product` WHERE `Entry`=55;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(55, 550, 14, 166779, 1, 0, 0, 0, 2, 0, 0, 0, 0, 19, 0, 0, 'Transmorpher Beacon');
DELETE FROM `battlepay_product` WHERE `Entry`=78;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(78, 272, 3, 0, 0, 75614, 0, 0, 2, 0, 0, 0, 0, 15, 0, 0, 'Celestial Steed');
DELETE FROM `battlepay_product` WHERE `Entry`=82;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(82, 266, 2, 0, 0, 0, 71488, 0, 2, 0, 0, 0, 0, 18, 0, 0, 'Blossoming Ancient');
DELETE FROM `battlepay_product` WHERE `Entry`=89;
INSERT INTO `battlepay_product` (`Entry`, `ProductID`, `Type`, `Flags`, `Unk1`, `DisplayId`, `ItemId`, `Unk4`, `Unk5`, `Unk6`, `Unk7`, `Unk8`, `Unk9`, `UnkString`, `UnkBit`, `UnkBits`, `Name`) VALUES 
(89, 274, 3, 0, 0, 142878, 0, 0, 2, 0, 0, 0, 0, 20, 0, 0, 'Enchanted Fey Dragon');

DELETE FROM `battlepay_shop` WHERE `Entry`=61;
INSERT INTO `battlepay_shop` (`Entry`, `EntryID`, `GroupID`, `ProductID`, `Ordering`, `VasServiceType`, `StoreDeliveryType`) VALUES 
(61, 367, 17, 747, 0, 0, 0);
