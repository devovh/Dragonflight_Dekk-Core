/* ******************************************** *\
   Necrotic Wake -  Entrance & Exit Areatrigger
\* ******************************************** */

SET

@name = "Necrotic Wake",

-- entrance is where you go in
@entrance_template_id = 50014,
@entrance_mapid = 2222,
@entrance_x = -3317.090,
@entrance_y = -4096.583,
@entrance_z = 6599.409,
@entrance_o = 4.754,
-- inside is the location where you will be after entering the entrance
@inside_worldsafeloc_mapid = 2286,
@inside_worldsafeloc_x = -3317.524,
@inside_worldsafeloc_y = -3822.553,
@inside_worldsafeloc_z = 6603.943,
@inside_worldsafeloc_facing = 90,
-- exit is the portal inside the dungeon which ports you out
@exit_template_id = 50015,
@exit_mapid = 2286,
@exit_x = -3316.887,
@exit_y = -3828.576,
@exit_z = 6603.943,
@exit_o = 1.555,
-- outside is the location where you will be after entering the exit
@outside_worldsafeloc_mapid = 2222,
@outside_worldsafeloc_x = -3317.320,
@outside_worldsafeloc_y = -4104.160,
@outside_worldsafeloc_z = 6599.430,
@outside_worldsafeloc_facing = 270,
-- shape is rectangle
@areatrigger_size_x = 2,
@areatrigger_size_y = 2,
@areatrigger_size_z = 5;

/* for dungeon finder correction
SET 
@normal_lfg_id = 2070,
@heroic_lfg_id = 2071,
@mythic_lfg_id = 2114,
@m_plus_lfg_id = 2123;
DELETE FROM `lfg_dungeon_template` WHERE `dungeonId` IN (@normal_lfg_id, @heroic_lfg_id, @mythic_lfg_id, @m_plus_lfg_id);
INSERT INTO `lfg_dungeon_template` (`dungeonId`, `name`, `position_x`, `position_y`, `position_z`, `orientation`, `requiredItemLevel`, `VerifiedBuild`) VALUES
(@normal_lfg_id, @name, @inside_worldsafeloc_x, @inside_worldsafeloc_y, @inside_worldsafeloc_z, 1.542, 0, 0),
(@heroic_lfg_id, @name, @inside_worldsafeloc_x, @inside_worldsafeloc_y, @inside_worldsafeloc_z, 1.542, 0, 0),
(@mythic_lfg_id, @name, @inside_worldsafeloc_x, @inside_worldsafeloc_y, @inside_worldsafeloc_z, 1.542, 0, 0),
(@m_plus_lfg_id, @name, @inside_worldsafeloc_x, @inside_worldsafeloc_y, @inside_worldsafeloc_z, 1.542, 0, 0);
*/

-- cleaning
DELETE FROM areatrigger_template WHERE Id = @entrance_template_id;
DELETE FROM areatrigger_template_actions WHERE AreaTriggerId = @entrance_template_id;
DELETE FROM areatrigger WHERE SpawnId = @entrance_template_id;
DELETE FROM world_safe_locs WHERE ID = @entrance_template_id;
DELETE FROM areatrigger_template WHERE Id = @exit_template_id;
DELETE FROM areatrigger_template_actions WHERE AreaTriggerId = @exit_template_id;
DELETE FROM areatrigger WHERE SpawnId = @exit_template_id;
DELETE FROM world_safe_locs WHERE ID = @exit_template_id;

-- entrance template
INSERT INTO areatrigger_template (Id, IsServerSide, Type, Flags, Data0, Data1, Data2, Data3, Data4, Data5, VerifiedBuild) 
VALUES (@entrance_template_id, '1', '1', '0', '1', '1', '1', '0', '0', '0', '0');
-- entrance action
INSERT INTO areatrigger_template_actions (AreaTriggerId, IsServerSide, ActionType, ActionParam, TargetType) 
VALUES (@entrance_template_id, '1', '2', @entrance_template_id, '5');
-- entrance spawn
INSERT INTO areatrigger (SpawnId, AreaTriggerId, IsServerSide, MapId, PosX, PosY, PosZ, Orientation, PhaseUseFlags, PhaseId, PhaseGroup, Shape, ShapeData0, ShapeData1, ShapeData2, ScriptName, Comment) 
VALUES (@entrance_template_id, @entrance_template_id, '1', @entrance_mapid, @entrance_x, @entrance_y, @entrance_z, @entrance_o , '1', '0', '0', '1', @areatrigger_size_x, @areatrigger_size_y, @areatrigger_size_z, '', CONCAT(@name, ' Entrance'));
-- inside target worldsafeloc
INSERT INTO `world_safe_locs` (`ID`, `MapID`, `LocX`, `LocY`, `LocZ`, `Facing`, `Comment`) 
VALUES (@entrance_template_id, @inside_worldsafeloc_mapid, @inside_worldsafeloc_x, @inside_worldsafeloc_y, @inside_worldsafeloc_z, @inside_worldsafeloc_facing, CONCAT(@name, ' Inside Target'));

-- exit template
INSERT INTO areatrigger_template (Id, IsServerSide, Type, Flags, Data0, Data1, Data2, Data3, Data4, Data5, VerifiedBuild) 
VALUES (@exit_template_id, '1', '1', '0', '1', '1', '1', '0', '0', '0', '0');
-- exit action
INSERT INTO areatrigger_template_actions (AreaTriggerId, IsServerSide, ActionType, ActionParam, TargetType) 
VALUES (@exit_template_id, '1', '2', @exit_template_id, '5');
-- exit spawn
INSERT INTO areatrigger (SpawnId, AreaTriggerId, IsServerSide, MapId, PosX, PosY, PosZ, Orientation, PhaseUseFlags, PhaseId, PhaseGroup, Shape, ShapeData0, ShapeData1, ShapeData2, ScriptName, Comment) 
VALUES (@exit_template_id, @exit_template_id, '1', @exit_mapid, @exit_x, @exit_y, @exit_z, @exit_o , '1', '0', '0', '1', @areatrigger_size_x, @areatrigger_size_y, @areatrigger_size_z, '', CONCAT(@name, ' Exit'));
-- outside target worldsafeloc
INSERT INTO `world_safe_locs` (`ID`, `MapID`, `LocX`, `LocY`, `LocZ`, `Facing`, `Comment`) 
VALUES (@exit_template_id, @outside_worldsafeloc_mapid, @outside_worldsafeloc_x, @outside_worldsafeloc_y, @outside_worldsafeloc_z, @outside_worldsafeloc_facing, CONCAT(@name, ' Outside Target'));
