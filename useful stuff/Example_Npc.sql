/* Example Npc - Easy to Change */

-- Default Values (entry has to be unique)
SET
@entry = 8000000,
@name = 'Example Npc',
@subname = '',
@IconName = NULL,
@gossip_menu_id = 0,
@minlevel = 60,
@maxlevel = 60,
@faction = 35,
@npcflag = 1,
@scale = 1,
@rank = 0,
@HoverHeight = 1,
@AIName = '',
@ScriptName = '';

-- Cleaning
DELETE FROM `creature_template` WHERE `entry`=@entry;
DELETE FROM `creature_template_model` WHERE `CreatureID`=@entry;

-- Creature Template
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `name`, `femaleName`, `subname`, `TitleAlt`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `HealthScalingExpansion`, `RequiredExpansion`, `VignetteID`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `family`, `trainer_class`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `HealthModifierExtra`, `ManaModifier`, `ManaModifierExtra`, `ArmorModifier`, `DamageModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `CreatureDifficultyID`, `WidgetSetID`, `WidgetSetUnitConditionID`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) 
VALUES (@entry, 0, 0, 0, 0, 0, @name, @name, @subname, NULL, @IconName, 0, @minlevel, @maxlevel, 8, 0, 0, @faction, @npcflag, 1, 1.14286, @scale, 0, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, @AIName, 0, @HoverHeight, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, @ScriptName, 36949);
-- Creature Template Model
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) 
VALUES (@entry, 0, 95016, 1, 1, 36949);



-- Creature Template Movement (not must have)
DELETE FROM `creature_template_movement` WHERE `CreatureId`=@entry;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) 
VALUES (@entry, 1, 0, 1, 0, 0, 0);

-- Creature Template Addon (not must have)
DELETE FROM `creature_template_addon` WHERE `entry`=@entry;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `visibilityDistanceType`, `auras`) 
VALUES (@entry, 0, 0, 33554432, 0, 0, 0, 0, 0, 0, NULL);




/* 
Some extra info:

-- Creature Template Model ------ ------------------------------------------------------

This is where you can set the displayid and size of the creature.

-- Creature Template Addon ------ ------------------------------------------------------

This is where you can set extra options for the npc like emote, mount, path_id etc..

-- Creature Template Addon bytes1 ------------------------------------------------------

1 = Sitting
2 = Sit chair
3 = Sleep
4 = Sit low chair
5 = Sit medium chair
6 = Sit high chair
7 = Shows health bar as empty (combine with the state dead emote to make a creature look dead)
8 = Makes the mob kneel
9 = Submerges the creature below the ground
33554432 = Hover mode
50331648 = Hover mode 2

-- Creature Template Addon bytes2 ------------------------------------------------------

0 = STATE_UNARMED (not prepared weapon)
1 = STATE_MELEE (prepared melee weapon)
2 = STATE_RANGED (prepared ranged weapon)

-- Creature Template Movement ------------------------------------------------------

This is where you can set the movement mode availability of the creature.

*/