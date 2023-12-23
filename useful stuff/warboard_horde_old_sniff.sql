/*
* THIS IS ALL THE HORDE warboard DATA ALL IN ONE PLACE
*
* TODO: need to add rewardquestid to all choices!!!
* TODO: need to sniff location and spawn to other cities + three more for orgrimmar xD
*
*/

-- horde warboard spell script linking
DELETE FROM `spell_script_names` WHERE `spell_id`=261655;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (261655, 'spell_warboard');

-- horde warboard gameobject template

DELETE FROM `gameobject_template` WHERE `entry`=281340;
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `size`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `Data6`, `Data7`, `Data8`, `Data9`, `Data10`, `Data11`, `Data12`, `Data13`, `Data14`, `Data15`, `Data16`, `Data17`, `Data18`, `Data19`, `Data20`, `Data21`, `Data22`, `Data23`, `Data24`, `Data25`, `Data26`, `Data27`, `Data28`, `Data29`, `Data30`, `Data31`, `Data32`, `Data33`, `Data34`, `ContentTuningId`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES 
(281340, 10, 10014, 'Warchief\'s Command Board', 1, 2824, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261655, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 866, '', 'go_warboard', 40966);

DELETE FROM `gameobject_template_addon` WHERE `entry`=281340;
INSERT INTO `gameobject_template_addon` (`entry`, `faction`, `flags`, `WorldEffectID`, `AIAnimKitID`) VALUES
-- (281340, 29, 262144, 8830, 0); -- 262144 flag makes wow error
(281340, 29, 0, 8830, 0); -- flag 0 is not sniffed value but doesn't make wow error

-- horde warboard spawn guid checked in 2022.02.01 - dekkcore project zereth database
SET @OGUID := 5000100; -- alliance is 5000000

-- cleaning first: remove all horde warboard old and new
DELETE FROM gameobject WHERE id IN (281340, 206116, 206109);

-- horde warboard spawn Orgrimmar valley of strength
DELETE FROM `gameobject` WHERE `guid` = @OGUID;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnDifficulties`, `PhaseId`, `PhaseGroup`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(@OGUID, 281340, 1, 1637, 5170, '0', '0', 0, 1600.9896240234375, -4369.73974609375, 21.06940078735351562, 3.630291461944580078, 0, 0, -0.97029495239257812, 0.241925001144409179, 120, 255, 0, 42010); -- 281340 (Area: 5170 - Difficulty: 0)

DELETE FROM `gameobject_addon` WHERE `guid` = @OGUID;
INSERT INTO `gameobject_addon` (`guid`, `parent_rotation0`, `parent_rotation1`, `parent_rotation2`, `parent_rotation3`, `WorldEffectID`, `AIAnimKitID`) VALUES
(@OGUID, 0, 0, 0, 1, 8830, 0); -- 281340


-- player choices for horde warboard

DELETE FROM `playerchoice` WHERE `ChoiceId`=342;
INSERT INTO `playerchoice` (`ChoiceId`, `UiTextureKitId`, `SoundKitId`, `Question`, `HidewarboardHeader`, `KeepOpenAfterChoice`, `VerifiedBuild`) VALUES
(342, 262, 0, 'The Warchief needs you! Take a flyer.', 0, 0, 42010);


DELETE FROM `playerchoice_response` WHERE (`ChoiceId`=342 AND `ResponseId`=1013 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=1429 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=3264 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=871 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=871 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=864 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=871 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=864 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=868 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=864 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=868 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=872 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=872 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=873 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=868 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=873 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=874 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=873 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=874 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=848 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=874 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=848 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=847 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=851 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=851 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=852 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=852 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=853 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=854 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=853 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=882 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=882 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=881 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=854 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=888 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=882 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=888 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=887 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=887 AND `Index`=2) OR (`ChoiceId`=342 AND `ResponseId`=900 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=900 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=1013 AND `Index`=0) OR (`ChoiceId`=342 AND `ResponseId`=1013 AND `Index`=1) OR (`ChoiceId`=342 AND `ResponseId`=1429 AND `Index`=0);
INSERT INTO `playerchoice_response` (`ChoiceId`, `ResponseId`, `ResponseIdentifier`, `Index`, `ChoiceArtFileId`, `Flags`, `WidgetSetId`, `UiTextureAtlasElementID`, `SoundKitId`, `GroupId`, `Header`, `Subheader`, `ButtonTooltip`, `Answer`, `Description`, `Confirmation`, `RewardQuestID`, `UiTextureKitID`, `VerifiedBuild`) VALUES
(342, 1013, 10878, 2, 1851147, 0, 0, 0, 0, 0, 'Tanaan Jungle', '', '', 'Enter the Dark Portal', 'The Iron Horde are invading Azeroth!\n\nWe must stop them to save our world. For Azeroth!', '', 0, 0, 42010),
(342, 1429, 10877, 1, 1851120, 0, 0, 0, 0, 0, 'Broken Shore', '', '', 'Fight the Legion', 'The Burning Legion has returned to Azeroth!', '', 0, 0, 42010),
(342, 3264, 10876, 0, 1668214, 0, 0, 0, 0, 0, 'Chromie\'s Call', '', '', 'Onward to Adventure', 'The Bronze Dragonflight\'s magics are powerful, and allow us to see Azeroth how we choose to.\n\nExplore the world and rise to the challenge in your favorite period in time.', '', 62568, 0, 42010),
(342, 871, 10875, 0, 1851092, 0, 0, 0, 0, 0, 'Arathi Highlands', '', '', 'Travel to Arathi Highlands', 'The Forsaken march on the Highlands, seeking to put it under the watchful eye of the Dark Lady.\n\nClaim the Arathi Highlands for the Horde.', '', 0, 0, 42010),
(342, 871, 10874, 1, 1851092, 0, 0, 0, 0, 0, 'Arathi Highlands', '', '', 'Travel to Arathi Highlands', 'The Forsaken march on the Highlands, seeking to put it under the watchful eye of the Dark Lady.\n\nClaim the Arathi Highlands for the Horde.', '', 0, 0, 42010),
(342, 864, 10873, 0, 1851086, 0, 0, 0, 0, 0, 'Silverpine Forest', '', '', 'Defeat the Gilneans', 'Guerilla forces sent by Gilneas continue to attack Horde outposts in Silverpine Forest.\n\nAssist the Forsaken and crush the Gilnean opposition.', '', 0, 0, 42010),
(342, 871, 10872, 2, 1851092, 0, 0, 0, 0, 0, 'Arathi Highlands', '', '', 'Travel to Arathi Highlands', 'The Forsaken march on the Highlands, seeking to put it under the watchful eye of the Dark Lady.\n\nClaim the Arathi Highlands for the Horde.', '', 0, 0, 42010),
(342, 864, 10871, 1, 1851086, 0, 0, 0, 0, 0, 'Silverpine Forest', '', '', 'Defeat the Gilneans', 'Guerilla forces sent by Gilneas continue to attack Horde outposts in Silverpine Forest.\n\nAssist the Forsaken and crush the Gilnean opposition.', '', 0, 0, 42010),
(342, 868, 10870, 0, 1851080, 0, 0, 0, 0, 0, 'Hillsbrad Foothills', '', '', 'Reclaim the Foothills', 'The Forsaken are continuing their campaign to conquer the Eastern Kingdoms for the Horde.\n\nAssist them with their operations in Hillsbrad.', '', 0, 0, 42010),
(342, 864, 10869, 2, 1851086, 0, 0, 0, 0, 0, 'Silverpine Forest', '', '', 'Defeat the Gilneans', 'Guerilla forces sent by Gilneas continue to attack Horde outposts in Silverpine Forest.\n\nAssist the Forsaken and crush the Gilnean opposition.', '', 0, 0, 42010),
(342, 868, 10868, 1, 1851080, 0, 0, 0, 0, 0, 'Hillsbrad Foothills', '', '', 'Reclaim the Foothills', 'The Forsaken are continuing their campaign to conquer the Eastern Kingdoms for the Horde.\n\nAssist them with their operations in Hillsbrad.', '', 0, 0, 42010),
(342, 872, 10867, 0, 1851104, 0, 0, 0, 0, 0, 'Northern Stranglethorn', '', '', 'Secure Our Trade Routes', 'Recent attacks are hindering our ability to keep supply routes open to Stonard.\n\nAid Commander Aggro\'gosh in creating a safe route to our other outposts.', '', 0, 0, 42010),
(342, 872, 10866, 2, 1851104, 0, 0, 0, 0, 0, 'Northern Stranglethorn', '', '', 'Secure Our Trade Routes', 'Recent attacks are hindering our ability to keep supply routes open to Stonard.\n\nAid Commander Aggro\'gosh in creating a safe route to our other outposts.', '', 0, 0, 42010),
(342, 873, 10864, 0, 1851103, 0, 0, 0, 0, 0, 'The Hinterlands', '', '', 'Assist the Revantusk Tribe', 'One of our troll allies, the Revantusk tribe, are attempting to secure the Hinterlands for their people.\n\nHelp them with their goal. Show them they are not forgotten by the Horde.', '', 0, 0, 42010),
(342, 868, 10863, 2, 1851080, 0, 0, 0, 0, 0, 'Hillsbrad Foothills', '', '', 'Reclaim the Foothills', 'The Forsaken are continuing their campaign to conquer the Eastern Kingdoms for the Horde.\n\nAssist them with their operations in Hillsbrad.', '', 0, 0, 42010),
(342, 873, 10862, 1, 1851103, 0, 0, 0, 0, 0, 'The Hinterlands', '', '', 'Assist the Revantusk Tribe', 'One of our troll allies, the Revantusk tribe, are attempting to secure the Hinterlands for their people.\n\nHelp them with their goal. Show them they are not forgotten by the Horde.', '', 0, 0, 42010),
(342, 874, 10861, 0, 1851096, 0, 0, 0, 0, 0, 'The Cape of Stranglethorn', '', '', 'Assist Sassy Hardwrench', 'The Bilgewater Cartel has built a resort on the shores of Stranglethorn, but are surrounded by naga and pirates lurking in the surrounding area.\n\nSee what the Horde can do to assist the goblins of Hardwrench Hideaway.', '', 0, 0, 42010),
(342, 873, 10860, 2, 1851103, 0, 0, 0, 0, 0, 'The Hinterlands', '', '', 'Assist the Revantusk Tribe', 'One of our troll allies, the Revantusk tribe, are attempting to secure the Hinterlands for their people.\n\nHelp them with their goal. Show them they are not forgotten by the Horde.', '', 0, 0, 42010),
(342, 874, 10859, 1, 1851096, 0, 0, 0, 0, 0, 'The Cape of Stranglethorn', '', '', 'Assist Sassy Hardwrench', 'The Bilgewater Cartel has built a resort on the shores of Stranglethorn, but are surrounded by naga and pirates lurking in the surrounding area.\n\nSee what the Horde can do to assist the goblins of Hardwrench Hideaway.', '', 0, 0, 42010),
(342, 848, 10858, 0, 1851084, 0, 0, 0, 0, 0, 'Northern Barrens', '', '', 'Defend the Barrens', 'The quilboars and centaurs advancing on the Northern Barrens pose a threat to the supply caravans being sent out of Orgrimmar.\n\nStop them!', '', 0, 0, 42010),
(342, 874, 10857, 2, 1851096, 0, 0, 0, 0, 0, 'The Cape of Stranglethorn', '', '', 'Assist Sassy Hardwrench', 'The Bilgewater Cartel has built a resort on the shores of Stranglethorn, but are surrounded by naga and pirates lurking in the surrounding area.\n\nSee what the Horde can do to assist the goblins of Hardwrench Hideaway.', '', 0, 0, 42010),
(342, 848, 10856, 1, 1851084, 0, 0, 0, 0, 0, 'Northern Barrens', '', '', 'Defend the Barrens', 'The quilboars and centaurs advancing on the Northern Barrens pose a threat to the supply caravans being sent out of Orgrimmar.\n\nStop them!', '', 0, 0, 42010),
(342, 847, 10855, 0, 1851071, 0, 0, 0, 0, 0, 'Azshara', '', '', 'Defend Orgrimmar', 'The Alliance are attempting to reverse the efforts made by our forces in Azshara.\n\nPush them back!', '', 0, 0, 42010),
(342, 851, 10852, 0, 1851070, 0, 0, 0, 0, 0, 'Ashenvale', '', '', 'Reinforce the Rampart', 'The might of Ashenvale is bearing down on our forces at the Mor\'shan Rampart.\n\nRepel the night elves and claim Ashenvale\'s resources for the Horde!', '', 0, 0, 42010),
(342, 851, 10850, 1, 1851070, 0, 0, 0, 0, 0, 'Ashenvale', '', '', 'Reinforce the Rampart', 'The might of Ashenvale is bearing down on our forces at the Mor\'shan Rampart.\n\nRepel the night elves and claim Ashenvale\'s resources for the Horde!', '', 0, 0, 42010),
(342, 852, 10849, 0, 1851107, 0, 0, 0, 0, 0, 'Stonetalon Mountains', '', '', 'For the Horde!', 'The Alliance seek to control more territory in Kalimdor. This cannot happen. Suit up and report to Blood Guard Aldo Rockrain in Ashenvale.\n\nThe Warchief\'s eyes are upon you.', '', 0, 0, 42010),
(342, 852, 10847, 1, 1851107, 0, 0, 0, 0, 0, 'Stonetalon Mountains', '', '', 'For the Horde!', 'The Alliance seek to control more territory in Kalimdor. This cannot happen. Suit up and report to Blood Guard Aldo Rockrain in Ashenvale.\n\nThe Warchief\'s eyes are upon you.', '', 0, 0, 42010),
(342, 853, 10846, 0, 1851087, 0, 0, 0, 0, 0, 'Southern Barrens', '', '', 'Fortify the Barrens', 'The Southern Barrens belongs to the Horde, but recent Alliance victories are threatening our control of the land.\n\nShow them that this is our land, and we will defend it.', '', 0, 0, 42010),
(342, 854, 10840, 0, 1851097, 0, 0, 0, 0, 0, 'Desolace', '', '', 'Lend Aid to Desolace', 'The Alliance, the Burning Blade, and demons. We have many enemies, and they must be beat back so that the Horde can launch their campaign on Desolace.\n\nGo to Tethris Aran and speak with Furien.', '', 0, 0, 42010),
(342, 853, 10839, 2, 1851087, 0, 0, 0, 0, 0, 'Southern Barrens', '', '', 'Fortify the Barrens', 'The Southern Barrens belongs to the Horde, but recent Alliance victories are threatening our control of the land.\n\nShow them that this is our land, and we will defend it.', '', 0, 0, 42010),
(342, 882, 10837, 0, 1851122, 0, 0, 0, 0, 0, 'Zangarmarsh', '', '', 'Assist the Expedition', 'The Cenarion Expedition is currently studying the exotic flora and fauna in the marshes of Zangarmarsh.\n\nHelping them with their mission will also help the Horde gain a better understanding of Outland.', '', 0, 0, 42010),
(342, 882, 10835, 1, 1851122, 0, 0, 0, 0, 0, 'Zangarmarsh', '', '', 'Assist the Expedition', 'The Cenarion Expedition is currently studying the exotic flora and fauna in the marshes of Zangarmarsh.\n\nHelping them with their mission will also help the Horde gain a better understanding of Outland.', '', 0, 0, 42010),
(342, 881, 10834, 0, 1851116, 0, 0, 0, 0, 0, 'Hellfire Peninsula', '', '', 'Travel to Outland', 'The Dark Portal has reopened and the Burning Legion is now positioned to attack Azeroth yet again.\n\nStop the demon filth from taking control of the Dark Portal and cutting off supplies to our forces in Hellfire Peninsula.', '', 0, 0, 42010),
(342, 854, 10833, 2, 1851097, 0, 0, 0, 0, 0, 'Desolace', '', '', 'Lend Aid to Desolace', 'The Alliance, the Burning Blade, and demons. We have many enemies, and they must be beat back so that the Horde can launch their campaign on Desolace.\n\nGo to Tethris Aran and speak with Furien.', '', 0, 0, 42010),
(342, 888, 10831, 0, 1851153, 0, 0, 0, 0, 0, 'Howling Fjord', '', '', 'Fly to Vengeance Landing', 'The Forsaken have established a beachhead on the shores of Howling Fjord named Vengeance Landing.\n\nFly to Howling Fjord and assist the Royal Apothecary Society in delivering a plague that will devour the Scourge.', '', 0, 0, 42010),
(342, 882, 10827, 2, 1851122, 0, 0, 0, 0, 0, 'Zangarmarsh', '', '', 'Assist the Expedition', 'The Cenarion Expedition is currently studying the exotic flora and fauna in the marshes of Zangarmarsh.\n\nHelping them with their mission will also help the Horde gain a better understanding of Outland.', '', 0, 0, 42010),
(342, 888, 10826, 1, 1851153, 0, 0, 0, 0, 0, 'Howling Fjord', '', '', 'Fly to Vengeance Landing', 'The Forsaken have established a beachhead on the shores of Howling Fjord named Vengeance Landing.\n\nFly to Howling Fjord and assist the Royal Apothecary Society in delivering a plague that will devour the Scourge.', '', 0, 0, 42010),
(342, 887, 10825, 0, 1851148, 0, 0, 0, 0, 0, 'Borean Tundra', '', '', 'Fly to Warsong Hold', 'Under the command of Garrosh Hellscream, the Warsong Offensive has established an outpost on the western shore of Northrend.\n\nTravel to Borean Tundra and show the Scourge the Horde\'s might.', '', 0, 0, 42010),
(342, 887, 10824, 2, 1851148, 0, 0, 0, 0, 0, 'Borean Tundra', '', '', 'Fly to Warsong Hold', 'Under the command of Garrosh Hellscream, the Warsong Offensive has established an outpost on the western shore of Northrend.\n\nTravel to Borean Tundra and show the Scourge the Horde\'s might.', '', 0, 0, 42010),
(342, 900, 10822, 0, 1851133, 0, 0, 0, 0, 0, 'The Jade Forest', '', '', 'Report to Grommash Hold', 'A naval battle has erupted between the Alliance and Horde, with the Horde securing decisive victories.\n\nDestroy what remains of the weakened Alliance fleet.', '', 0, 0, 42010),
(342, 900, 10820, 1, 1851133, 0, 0, 0, 0, 0, 'The Jade Forest', '', '', 'Report to Grommash Hold', 'A naval battle has erupted between the Alliance and Horde, with the Horde securing decisive victories.\n\nDestroy what remains of the weakened Alliance fleet.', '', 0, 0, 42010),
(342, 1013, 10819, 0, 1851147, 0, 0, 0, 0, 0, 'Tanaan Jungle', '', '', 'Enter the Dark Portal', 'The Iron Horde are invading Azeroth!\n\nWe must stop them to save our world. For Azeroth!', '', 0, 0, 42010),
(342, 1013, 10817, 1, 1851147, 0, 0, 0, 0, 0, 'Tanaan Jungle', '', '', 'Enter the Dark Portal', 'The Iron Horde are invading Azeroth!\n\nWe must stop them to save our world. For Azeroth!', '', 0, 0, 42010),
(342, 1429, 10816, 0, 1851120, 0, 0, 0, 0, 0, 'Broken Shore', '', '', 'Fight the Legion', 'The Burning Legion has returned to Azeroth!', '', 0, 0, 42010);