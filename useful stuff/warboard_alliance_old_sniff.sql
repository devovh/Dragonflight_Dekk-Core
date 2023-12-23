/*
* THIS IS ALL THE ALLIANCE HEROE'S warboard DATA ALL IN ONE PLACE
*
* TODO: need to add rewardquestid to all choices!!!
* TODO: need to sniff location and spawn to other cities + two more for stormwind xD
*
*/

-- alliance warboard spell script linking
DELETE FROM `spell_script_names` WHERE `spell_id`=261654;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (261654, 'spell_warboard');

-- alliance warboard gameobject template

DELETE FROM `gameobject_template` WHERE `entry` = 281339;
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `size`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `Data6`, `Data7`, `Data8`, `Data9`, `Data10`, `Data11`, `Data12`, `Data13`, `Data14`, `Data15`, `Data16`, `Data17`, `Data18`, `Data19`, `Data20`, `Data21`, `Data22`, `Data23`, `Data24`, `Data25`, `Data26`, `Data27`, `Data28`, `Data29`, `Data30`, `Data31`, `Data32`, `Data33`, `Data34`, `ContentTuningId`, `ScriptName`, `VerifiedBuild`) VALUES
(281339, 10, 10016, 'Hero\'s Call Board', '', '', '', 1, 2824, 0, 0, 0, 0, 0, 0, 0, 0, 0, 261654, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 864, 'go_warboard', 42010); -- 281339

DELETE FROM `gameobject_template_addon` WHERE `entry` = 281339;
INSERT INTO `gameobject_template_addon` (`entry`, `faction`, `flags`, `WorldEffectID`, `AIAnimKitID`) VALUES
-- (281339, 12, 262144, 8830, 0); -- 262144 flag makes wow error
(281339, 12, 0, 8830, 0); -- flag 0 is not sniffed value but doesn't make wow error

-- alliance warboard spawn guid checked in 2022.02.01 - dekkcore project zereth database
SET @OGUID := 5000000; -- horde is 5000100

-- cleaning first: remove all alliance warboard old and new
DELETE FROM gameobject WHERE id IN (281339, 206111);

-- alliance warboard spawn Stormwind dwarven district
DELETE FROM `gameobject` WHERE `guid` = @OGUID;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnDifficulties`, `PhaseId`, `PhaseGroup`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(@OGUID, 281339, 0, 1519, 5148, '0', '0', 0, -8823.8505859375, 630.57293701171875, 94.31772613525390625, 3.839725255966186523, 0, 0, -0.93969249725341796, 0.34202045202255249, 120, 255, 0, 42010); -- 281339 (Area: 5148 - Difficulty: 0)

DELETE FROM `gameobject_addon` WHERE `guid` = @OGUID;
INSERT INTO `gameobject_addon` (`guid`, `parent_rotation0`, `parent_rotation1`, `parent_rotation2`, `parent_rotation3`, `WorldEffectID`, `AIAnimKitID`) VALUES
(@OGUID, 0, 0, 0, 1, 8830, 0); -- 281339

-- alliance warboard spawn Stormwind trade district
DELETE FROM `gameobject` WHERE `guid` = @OGUID+1;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnDifficulties`, `PhaseId`, `PhaseGroup`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(@OGUID+1, 281339, 0, 1519, 5150, '0', '0', 0, -8341.76953125, 641.7569580078125, 95.85759735107421875, 3.78736734390258789, 0, 0, -0.94832324981689453, 0.317305892705917358, 120, 255, 0, 42010); -- 281339 (Area: 5150 - Difficulty: 0)

DELETE FROM `gameobject_addon` WHERE `guid` = @OGUID+1;
INSERT INTO `gameobject_addon` (`guid`, `parent_rotation0`, `parent_rotation1`, `parent_rotation2`, `parent_rotation3`, `WorldEffectID`, `AIAnimKitID`) VALUES
(@OGUID+1, 0, 0, 0, 1, 8830, 0); -- 281339


-- player choices for alliance warboard


DELETE FROM `playerchoice` WHERE `ChoiceId`=352;
INSERT INTO `playerchoice` (`ChoiceId`, `UiTextureKitId`, `SoundKitId`, `Question`, `HidewarboardHeader`, `KeepOpenAfterChoice`, `VerifiedBuild`) VALUES
(352, 261, 0, 'The King needs you! Please take a flyer.', 0, 0, 42010);


DELETE FROM `playerchoice_response` WHERE (`ChoiceId`=352 AND `ResponseId`=943 AND `Index`=2) OR (`ChoiceId`=352 AND `ResponseId`=954 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=961 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=956 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=961 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=959 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=957 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=960 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=962 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=962 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=964 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=965 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=965 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=966 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=967 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=968 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=964 AND `Index`=2) OR (`ChoiceId`=352 AND `ResponseId`=968 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=970 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=968 AND `Index`=2) OR (`ChoiceId`=352 AND `ResponseId`=970 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=969 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=971 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=971 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=972 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=972 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=973 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=973 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=977 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=973 AND `Index`=2) OR (`ChoiceId`=352 AND `ResponseId`=977 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=976 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=978 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=978 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=979 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=979 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=980 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=980 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=981 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=980 AND `Index`=2) OR (`ChoiceId`=352 AND `ResponseId`=981 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=991 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=981 AND `Index`=2) OR (`ChoiceId`=352 AND `ResponseId`=991 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=990 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=992 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=989 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=1014 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=1429 AND `Index`=0) OR (`ChoiceId`=352 AND `ResponseId`=1429 AND `Index`=1) OR (`ChoiceId`=352 AND `ResponseId`=3265 AND `Index`=0);
INSERT INTO `playerchoice_response` (`ChoiceId`, `ResponseId`, `ResponseIdentifier`, `Index`, `ChoiceArtFileId`, `Flags`, `WidgetSetId`, `UiTextureAtlasElementID`, `SoundKitId`, `GroupId`, `Header`, `Subheader`, `ButtonTooltip`, `Answer`, `Description`, `Confirmation`, `RewardQuestID`, `UiTextureKitID`, `VerifiedBuild`) VALUES
(352, 943, 3444, 2, 1851102, 0, 0, 0, 0, 0, 'Feralas', '', '', 'Assist Dreamer\'s Rest', 'An insidious sickness falls over the lands of Feralas, all while the Horde and the Grimtotem tauren assail our encampments.\n\nSee what can be done to quell the Grimtotem threat and cure the land.', '', 0, 0, 42010),
(352, 954, 3443, 1, 1851085, 0, 0, 0, 0, 0, 'Redridge Mountains', '', '', 'Protect Lakeshire', 'The people of Lakeshire are worried that another gnoll attack is about to take place.$b$bPrevent the gnolls from mobilizing and ransacking our city.', '', 0, 0, 42010),
(352, 961, 3442, 0, 1851096, 0, 0, 0, 0, 0, 'The Cape of Stranglethorn', '', '', 'Discover Hidden Artifacts', 'The Explorer\'s League is looking for artifacts that may have been unearthed after the Shattering.$b$bHelp them uncover whatever secrets the lands hold.', '', 0, 0, 42010),
(352, 956, 3439, 0, 1851098, 0, 0, 0, 0, 0, 'Duskwood', '', '', 'Join the Night Watch', 'Everlasting night falls over Darkshire, with man-eating wolves and wild worgen threatening the people that live there.$b$bThey need help if they are to survive against the threats that lurk in the shadows.', '', 0, 0, 42010),
(352, 961, 3437, 1, 1851096, 0, 0, 0, 0, 0, 'The Cape of Stranglethorn', '', '', 'Discover Hidden Artifacts', 'The Explorer\'s League is looking for artifacts that may have been unearthed after the Shattering.$b$bHelp them uncover whatever secrets the lands hold.', '', 0, 0, 42010),
(352, 959, 3436, 0, 1851104, 0, 0, 0, 0, 0, 'Northern Stranglethorn', '', '', 'Discover Kurzen\'s Fate', 'The last group of Stormwind\'s soldiers sent into the jungle descended into madness.$b$bUncover what happened to Captain Kurzen, and perhaps we can guarantee the safety of the Alliance\'s troops in the region.', '', 0, 0, 42010),
(352, 957, 3433, 0, 1851113, 0, 0, 0, 0, 0, 'Wetlands', '', '', 'Help the Survey Team', 'The enemies of Ironforge seek to exploit the destruction of the Stonewrought Dam.$b$bHelp the survey team deployed to the region understand where our weaknesses exist and how we can help our dwarven allies rebuild.', '', 0, 0, 42010),
(352, 960, 3430, 0, 1851103, 0, 0, 0, 0, 0, 'The Hinterlands', '', '', 'Help the Wildhammer', 'The Wildhammer clan needs our help protecting their homeland against the Horde.$b$bShow them that their kindness is not forgotten by the Alliance.', '', 0, 0, 42010),
(352, 962, 3427, 0, 1851112, 0, 0, 0, 0, 0, 'Western Plaguelands', '', '', 'Defeat the Forsaken', 'The Forsaken have launched a counterattack to cut off Alliance forces and claim Andorhal for themselves!$b$bStop the Scourge, defeat the Horde, and reclaim Andorhal for the Alliance!', '', 0, 0, 42010),
(352, 962, 3425, 1, 1851112, 0, 0, 0, 0, 0, 'Western Plaguelands', '', '', 'Defeat the Forsaken', 'The Forsaken have launched a counterattack to cut off Alliance forces and claim Andorhal for themselves!$b$bStop the Scourge, defeat the Horde, and reclaim Andorhal for the Alliance!', '', 0, 0, 42010),
(352, 964, 3424, 0, 1851093, 0, 0, 0, 0, 0, 'Badlands', '', '', 'Gab with Gek', 'The goblins of Fuselight are looking for adventurers to tame the wilderness surrounding their newly built town.$b$bDeal with the threats to Fuselight, and the Alliance\'s relationship with the Steamwheedle Cartel will flourish.', '', 0, 0, 42010),
(352, 965, 3421, 0, 1851105, 0, 0, 0, 0, 0, 'Searing Gorge', '', '', 'Investigate the Dwarves', 'Ragnaros\' Dark Iron dwarves are organizing, and appear poised to attack Alliance controlled territories!$b$bWork with the Thorium Brotherhood to uncover their motives and end their incursion.', '', 0, 0, 42010),
(352, 965, 3419, 1, 1851105, 0, 0, 0, 0, 0, 'Searing Gorge', '', '', 'Investigate the Dwarves', 'Ragnaros\' Dark Iron dwarves are organizing, and appear poised to attack Alliance controlled territories!$b$bWork with the Thorium Brotherhood to uncover their motives and end their incursion.', '', 0, 0, 42010),
(352, 966, 3418, 0, 1851094, 0, 0, 0, 0, 0, 'Blasted Lands', '', '', 'Stop the Horde', 'Our forces at Nethergarde Keep need reinforcements to hold back the Horde.$b$bTurn the tide of battle in favor of the Alliance!', '', 0, 0, 42010),
(352, 967, 3415, 0, 1851095, 0, 0, 0, 0, 0, 'Burning Steppes', '', '', 'Speak with Keeshan', 'The Blackrock clan is poised to attack Redridge and other neighboring lands. The aggression of the Blackrock orcs cannot be left unchecked.$b$bWork with Keeshan to stop the Blackrock from advancing across Eastern Kingdoms.\n', '', 0, 0, 42010),
(352, 968, 3412, 0, 1851100, 0, 0, 0, 0, 0, 'Eastern Plaguelands', '', '', 'Travel with Fiona', 'A group of adventurers are traveling to Light\'s Hope Chapel to enlist with the Argent Crusade.$b$bThe caravan leader, Fiona, needs help reaching their destination safely.', '', 0, 0, 42010),
(352, 964, 3409, 2, 1851093, 0, 0, 0, 0, 0, 'Badlands', '', '', 'Gab with Gek', 'The goblins of Fuselight are looking for adventurers to tame the wilderness surrounding their newly built town.$b$bDeal with the threats to Fuselight, and the Alliance\'s relationship with the Steamwheedle Cartel will flourish.', '', 0, 0, 42010),
(352, 968, 3408, 1, 1851100, 0, 0, 0, 0, 0, 'Eastern Plaguelands', '', '', 'Travel with Fiona', 'A group of adventurers are traveling to Light\'s Hope Chapel to enlist with the Argent Crusade.$b$bThe caravan leader, Fiona, needs help reaching their destination safely.', '', 0, 0, 42010),
(352, 970, 3407, 0, 1851122, 0, 0, 0, 0, 0, 'Zangarmarsh', '', '', 'Assist the Expedition', 'The Cenarion Expedition is currently studying the exotic flora and fauna in the marshes of Zangarmarsh.\n\nHelping them with their mission will also help the Alliance gain a better understanding of Outland.', '', 0, 0, 42010),
(352, 968, 3406, 2, 1851100, 0, 0, 0, 0, 0, 'Eastern Plaguelands', '', '', 'Travel with Fiona', 'A group of adventurers are traveling to Light\'s Hope Chapel to enlist with the Argent Crusade.$b$bThe caravan leader, Fiona, needs help reaching their destination safely.', '', 0, 0, 42010),
(352, 970, 3405, 1, 1851122, 0, 0, 0, 0, 0, 'Zangarmarsh', '', '', 'Assist the Expedition', 'The Cenarion Expedition is currently studying the exotic flora and fauna in the marshes of Zangarmarsh.\n\nHelping them with their mission will also help the Alliance gain a better understanding of Outland.', '', 0, 0, 42010),
(352, 969, 3404, 0, 1851116, 0, 0, 0, 0, 0, 'Hellfire Peninsula', '', '', 'Travel to Outland', 'The Dark Portal has reopened and the Burning Legion is now positioned to attack Azeroth yet again.\n\nStop the demon filth from taking control of the Dark Portal and cutting off supplies to our forces in Hellfire Peninsula.', '', 0, 0, 42010),
(352, 971, 3401, 0, 1851121, 0, 0, 0, 0, 0, 'Terokkar Forest', '', '', 'Assist Allerian Stronghold', 'The forest to the south of Shattrath City is both beautiful and dangerous.\n\nThe people in Allerian Stronghold will need all the help they can get to survive in Terokkar Forest.', '', 0, 0, 42010),
(352, 971, 3399, 1, 1851121, 0, 0, 0, 0, 0, 'Terokkar Forest', '', '', 'Assist Allerian Stronghold', 'The forest to the south of Shattrath City is both beautiful and dangerous.\n\nThe people in Allerian Stronghold will need all the help they can get to survive in Terokkar Forest.', '', 0, 0, 42010),
(352, 972, 3398, 0, 1851118, 0, 0, 0, 0, 0, 'Nagrand', '', '', 'Stop the Murkblood', 'The Kurenai of Telaar have been framed for the destruction of an orcish outpost by the Murkblood tribe!\n\nHelp the Kurenai restore their reputation and find peace in Outland.', '', 0, 0, 42010),
(352, 972, 3394, 1, 1851118, 0, 0, 0, 0, 0, 'Nagrand', '', '', 'Stop the Murkblood', 'The Kurenai of Telaar have been framed for the destruction of an orcish outpost by the Murkblood tribe!\n\nHelp the Kurenai restore their reputation and find peace in Outland.', '', 0, 0, 42010),
(352, 973, 3393, 0, 1851115, 0, 0, 0, 0, 0, 'Blade\'s Edge Mountains', '', '', 'Battle the Ogres', 'The ogre threat must be stopped if we are to protect our outposts in Blade\'s Edge Mountains.\n\nDo what must be done to beat them back.', '', 0, 0, 42010),
(352, 973, 3391, 1, 1851115, 0, 0, 0, 0, 0, 'Blade\'s Edge Mountains', '', '', 'Battle the Ogres', 'The ogre threat must be stopped if we are to protect our outposts in Blade\'s Edge Mountains.\n\nDo what must be done to beat them back.', '', 0, 0, 42010),
(352, 977, 3390, 0, 1851153, 0, 0, 0, 0, 0, 'Howling Fjord', '', '', 'Battle the Vrykul', 'The port town of Valgarde is located in the shadows of Utgarde Keep, and is under constant threat from the vrykul that spill forth from its halls.\n\nOur victory against the Scourge begins with defeating the vrykul and controlling the eastern shores of Northrend!', '', 0, 0, 42010),
(352, 973, 3389, 2, 1851115, 0, 0, 0, 0, 0, 'Blade\'s Edge Mountains', '', '', 'Battle the Ogres', 'The ogre threat must be stopped if we are to protect our outposts in Blade\'s Edge Mountains.\n\nDo what must be done to beat them back.', '', 0, 0, 42010),
(352, 977, 3388, 1, 1851153, 0, 0, 0, 0, 0, 'Howling Fjord', '', '', 'Battle the Vrykul', 'The port town of Valgarde is located in the shadows of Utgarde Keep, and is under constant threat from the vrykul that spill forth from its halls.\n\nOur victory against the Scourge begins with defeating the vrykul and controlling the eastern shores of Northrend!', '', 0, 0, 42010),
(352, 976, 3387, 0, 1851148, 0, 0, 0, 0, 0, 'Borean Tundra', '', '', 'Enlist with the Expedition', 'The war against the Scourge has begun!\n\nBecome an enlisted soldier with the Valiance Expedition. Help the Alliance achieve victory on the western shores of Northrend!', '', 0, 0, 42010),
(352, 978, 3384, 0, 1851151, 0, 0, 0, 0, 0, 'Dragonblight', '', '', 'Travel to Dragonblight', 'A direct route into Icecrown exists in the Light-forsaken land of Dragonblight.\n\nSlow the Scourge and give our forces time to build an offensive to lay siege to the Wrathgate.', '', 0, 0, 42010),
(352, 978, 3382, 1, 1851151, 0, 0, 0, 0, 0, 'Dragonblight', '', '', 'Travel to Dragonblight', 'A direct route into Icecrown exists in the Light-forsaken land of Dragonblight.\n\nSlow the Scourge and give our forces time to build an offensive to lay siege to the Wrathgate.', '', 0, 0, 42010),
(352, 979, 3381, 0, 1851152, 0, 0, 0, 0, 0, 'Grizzly Hills', '', '', 'Assist the Military', 'The Horde are sending their forces to pillage our storehouses, raid our encampments, and strip the land of its resources.\n\nFind what we need to break our stalemate with the Horde.', '', 0, 0, 42010),
(352, 979, 3379, 1, 1851152, 0, 0, 0, 0, 0, 'Grizzly Hills', '', '', 'Assist the Military', 'The Horde are sending their forces to pillage our storehouses, raid our encampments, and strip the land of its resources.\n\nFind what we need to break our stalemate with the Horde.', '', 0, 0, 42010),
(352, 980, 3378, 0, 1851157, 0, 0, 0, 0, 0, 'Zul\'Drak', '', '', 'Assist Our Allies', 'The Argent Crusade and the Ebon Blade are attempting to control the Scourge presence in Zul\'Drak.\n\nThe Scourge and any other threats that lie in the Drakkari homeland must be stopped.', '', 0, 0, 42010),
(352, 980, 3376, 1, 1851157, 0, 0, 0, 0, 0, 'Zul\'Drak', '', '', 'Assist Our Allies', 'The Argent Crusade and the Ebon Blade are attempting to control the Scourge presence in Zul\'Drak.\n\nThe Scourge and any other threats that lie in the Drakkari homeland must be stopped.', '', 0, 0, 42010),
(352, 981, 3375, 0, 1851155, 0, 0, 0, 0, 0, 'Sholazar Basin', '', '', 'Find Hemet Nesingwary', 'We have lost contact with Hemet Nesingwary and the expeditionary force he was leading into Sholazar Basin.\n\nFind him and perhaps we will also discover power that will help us with our fight against the Scourge.', '', 0, 0, 42010),
(352, 980, 3374, 2, 1851157, 0, 0, 0, 0, 0, 'Zul\'Drak', '', '', 'Assist Our Allies', 'The Argent Crusade and the Ebon Blade are attempting to control the Scourge presence in Zul\'Drak.\n\nThe Scourge and any other threats that lie in the Drakkari homeland must be stopped.', '', 0, 0, 42010),
(352, 981, 3373, 1, 1851155, 0, 0, 0, 0, 0, 'Sholazar Basin', '', '', 'Find Hemet Nesingwary', 'We have lost contact with Hemet Nesingwary and the expeditionary force he was leading into Sholazar Basin.\n\nFind him and perhaps we will also discover power that will help us with our fight against the Scourge.', '', 0, 0, 42010),
(352, 991, 3372, 0, 1851134, 0, 0, 0, 0, 0, 'Krasarang Wilds', '', '', 'Travel to Krasarang', 'The pandaren of Krasarang Wilds have been beset by unrelenting rain and a general malaise has fallen over the land.\n\nAssist the denizens of the Wilds, and find out what is afflicting the otherwise jovial pandaren.', '', 0, 0, 42010),
(352, 981, 3371, 2, 1851155, 0, 0, 0, 0, 0, 'Sholazar Basin', '', '', 'Find Hemet Nesingwary', 'We have lost contact with Hemet Nesingwary and the expeditionary force he was leading into Sholazar Basin.\n\nFind him and perhaps we will also discover power that will help us with our fight against the Scourge.', '', 0, 0, 42010),
(352, 991, 3370, 1, 1851134, 0, 0, 0, 0, 0, 'Krasarang Wilds', '', '', 'Travel to Krasarang', 'The pandaren of Krasarang Wilds have been beset by unrelenting rain and a general malaise has fallen over the land.\n\nAssist the denizens of the Wilds, and find out what is afflicting the otherwise jovial pandaren.', '', 0, 0, 42010),
(352, 990, 3369, 0, 1851139, 0, 0, 0, 0, 0, 'Valley of the Four Winds', '', '', 'Travel to the Valley', 'The tale of two pandaren and their exploits around Pandaria is echoed by all that have been to the Valley of the Four Winds.\n\nLearn from the Stormstouts what makes Pandaria special.', '', 0, 0, 42010),
(352, 992, 3366, 0, 1851135, 0, 0, 0, 0, 0, 'Kun-Lai Summit', '', '', 'Travel to the Veiled Stair', 'Lorewalker Cho has rescued some of our allies at the battle in Jade Forest and has taken them to Kun-Lai Summit to recover from their injuries.\n\nAssist the members of the Alliance in their recovery, and see that they are fit to continue our campaign on Pandaria.', '', 0, 0, 42010),
(352, 989, 3363, 0, 1851133, 0, 0, 0, 0, 0, 'The Jade Forest', '', '', 'Report to Stormwind Keep', 'A naval battle between the Alliance and Horde have stranded some of our people on an unknown landmass.$b$bBring them back home.', '', 0, 0, 42010),
(352, 1014, 3360, 0, 1851147, 0, 0, 0, 0, 0, 'Tanaan Jungle', '', '', 'Enter the Dark Portal', 'The Iron Horde are invading Azeroth!\n\nWe must stop them to save our world. For Azeroth!', '', 0, 0, 42010),
(352, 1429, 3357, 0, 1851120, 0, 0, 0, 0, 0, 'Broken Shore', '', '', 'Fight the Legion', 'The Burning Legion has returned to Azeroth!', '', 0, 0, 42010),
(352, 1429, 3355, 1, 1851120, 0, 0, 0, 0, 0, 'Broken Shore', '', '', 'Fight the Legion', 'The Burning Legion has returned to Azeroth!', '', 0, 0, 42010),
(352, 3265, 3354, 0, 1668214, 0, 0, 0, 0, 0, 'Chromie\'s Call', '', '', 'Onward to Adventure', 'The Bronze Dragonflight\'s magics are powerful, and allow us to see Azeroth how we choose to.\n\nExplore the world and rise to the challenge in your favorite period in time.', '', 62567, 0, 42010);



