-- Some hotfixes db table hashes for example:
/*
item: 1344507586
item_sparse: 2442913102
item_modified_appearance: 3834752085
item_x_item_effect: 13330255
spell: 3776013982
spell_name: 1187407512
transmog_set: 356071576
*/
-- To get more hash check https://github.com/TrinityCore/WowPacketParser/blob/master/WowPacketParser/Enums/DB2Hash.cs 
-- And convert to decimal https://www.rapidtables.com/convert/number/hex-to-decimal.html


/* ********************************** *\
	Example Custom Item - Consumable
\* ********************************** */

SET
@ItemID = 500000,
@Name = "Flux\'s Example Usable Item",
@Description = "Use: Exampling.",
@Quality = 7,
@ItemEffectID = 140670,
@VerifiedBuild = 44730;

DELETE FROM `item` WHERE `ID`=@ItemID;
INSERT INTO `item` (`ID`, `ClassID`, `SubclassID`, `Material`, `InventoryType`, `SheatheType`, `SoundOverrideSubclassID`, `IconFileDataID`, `ItemGroupSoundsID`, `ContentTuningID`, `ModifiedCraftingReagentItemID`, `VerifiedBuild`) VALUES 
(@ItemID, 0, 8, 0, 0, 0, -1, 4238929, 24, 0, 0, @VerifiedBuild);

DELETE FROM `item_sparse` WHERE `ID`=@ItemID;
INSERT INTO `item_sparse` (`ID`, `AllowableRace`, `Description`, `Display3`, `Display2`, `Display1`, `Display`, `ExpansionID`, `DmgVariance`, `LimitCategory`, `DurationInInventory`, `QualityModifier`, `BagFamily`, `StartQuestID`, `LanguageID`, `ItemRange`, `StatPercentageOfSocket1`, `StatPercentageOfSocket2`, `StatPercentageOfSocket3`, `StatPercentageOfSocket4`, `StatPercentageOfSocket5`, `StatPercentageOfSocket6`, `StatPercentageOfSocket7`, `StatPercentageOfSocket8`, `StatPercentageOfSocket9`, `StatPercentageOfSocket10`, `StatPercentEditor1`, `StatPercentEditor2`, `StatPercentEditor3`, `StatPercentEditor4`, `StatPercentEditor5`, `StatPercentEditor6`, `StatPercentEditor7`, `StatPercentEditor8`, `StatPercentEditor9`, `StatPercentEditor10`, `Stackable`, `MaxCount`, `RequiredAbility`, `SellPrice`, `BuyPrice`, `VendorStackCount`, `PriceVariance`, `PriceRandomValue`, `Flags1`, `Flags2`, `Flags3`, `Flags4`, `FactionRelated`, `ModifiedCraftingReagentItemID`, `ContentTuningID`, `PlayerLevelToItemLevelCurveID`, `ItemNameDescriptionID`, `RequiredTransmogHoliday`, `RequiredHoliday`, `GemProperties`, `SocketMatchEnchantmentId`, `TotemCategoryID`, `InstanceBound`, `ZoneBound1`, `ZoneBound2`, `ItemSet`, `LockID`, `PageID`, `ItemDelay`, `MinFactionID`, `RequiredSkillRank`, `RequiredSkill`, `ItemLevel`, `AllowableClass`, `ArtifactID`, `SpellWeight`, `SpellWeightCategory`, `SocketType1`, `SocketType2`, `SocketType3`, `SheatheType`, `Material`, `PageMaterialID`, `Bonding`, `DamageDamageType`, `StatModifierBonusStat1`, `StatModifierBonusStat2`, `StatModifierBonusStat3`, `StatModifierBonusStat4`, `StatModifierBonusStat5`, `StatModifierBonusStat6`, `StatModifierBonusStat7`, `StatModifierBonusStat8`, `StatModifierBonusStat9`, `StatModifierBonusStat10`, `ContainerSlots`, `MinReputation`, `RequiredPVPMedal`, `RequiredPVPRank`, `RequiredLevel`, `InventoryType`, `OverallQualityID`, `VerifiedBuild`) VALUES 
(@ItemID, -1, @Description, NULL, NULL, NULL, @Name, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 65536, 24576, 67108864, 16384, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, @Quality, @VerifiedBuild);

DELETE FROM `item_x_item_effect` WHERE `ID`=@ItemID;
INSERT INTO `item_x_item_effect` (`ID`, `ItemEffectID`, `ItemID`, `VerifiedBuild`) VALUES 
(@ItemID, @ItemEffectID, @ItemID, @VerifiedBuild);

DELETE FROM `hotfix_data` WHERE (`ID`=@ItemID) AND (`TableHash` IN (1344507586, 2442913102, 13330255));
INSERT INTO `hotfix_data` (`Id`, `TableHash`, `RecordId`, `Status`) VALUES 
-- hotfixes.item data
(@ItemID, 1344507586, @ItemID, 1),
-- hotfixes.item_sparse data
(@ItemID, 2442913102, @ItemID, 1),
-- hotfixes.item_x_item_effect data
(@ItemID, 13330255, @ItemID, 1);


/* ********************************** *\
	Example Custom Spell Name
\* ********************************** */

DELETE FROM `spell_name` WHERE `ID`=167349;
INSERT INTO `spell_name` (`ID`, `Name`, `VerifiedBuild`) VALUES 
(167349, 'ZoloCraft Aura', 44730);
DELETE FROM `hotfix_data` WHERE `Id`=167349 AND `TableHash`=1187407512 AND `RecordId`=167349;
INSERT INTO `hotfix_data` (`Id`, `UniqueId`, `TableHash`, `RecordId`, `Status`, `VerifiedBuild`) VALUES 
(167349, 0, 1187407512, 167349, 1, 44730);


/* ********************************** *\
	Example Custom TransmogSet Mod
\* ********************************** */
DELETE FROM `transmog_set` WHERE `ID`=2200 AND `VerifiedBuild`=44730;
INSERT INTO `transmog_set` (`Name`, `ID`, `ClassMask`, `TrackingQuestID`, `Flags`, `TransmogSetGroupID`, `ItemNameDescriptionID`, `ParentTransmogSetID`, `Unknown810`, `ExpansionID`, `PatchID`, `UiOrder`, `PlayerConditionID`, `VerifiedBuild`) VALUES 
('Celestial Observer\'s Ensemble', 2200, 0, 0, 0, 0, 0, 0, 1, 8, 90100, 10, 0, 44730);
DELETE FROM `hotfix_data` WHERE `Id`=2200 AND `TableHash`=356071576 AND `RecordId`=2200;
INSERT INTO `hotfix_data` (`Id`, `UniqueId`, `TableHash`, `RecordId`, `Status`, `VerifiedBuild`) VALUES (2200, 0, 356071576, 2200, 1, 44730);
