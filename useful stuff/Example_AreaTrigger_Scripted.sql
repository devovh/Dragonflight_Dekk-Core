/* This is a scripted areatrigger! */

SET

@name = "De Other Side - Blood-wrought Door Opener",
@scriptname = "at_deotherside_hakkardoor",

@template_id = 55000,
@spawn_mapid = 2291,
@spawn_x = 2740.46,
@spawn_y = -1762.75,
@spawn_z = 54.87,
@spawn_o = 1.51,
-- shape is rectangle
@areatrigger_size_x = 5,
@areatrigger_size_y = 5,
@areatrigger_size_z = 5;

-- cleaning
DELETE FROM areatrigger_template WHERE Id = @template_id;
DELETE FROM areatrigger WHERE SpawnId = @template_id;
DELETE FROM `areatrigger_scripts` WHERE `entry`=@template_id;

-- template
INSERT INTO areatrigger_template (Id, IsServerSide, Type, Flags, Data0, Data1, Data2, Data3, Data4, Data5, VerifiedBuild) 
VALUES (@template_id, '1', '1', '0', '1', '1', '1', '0', '0', '0', '0');
-- spawn
INSERT INTO areatrigger (SpawnId, AreaTriggerId, IsServerSide, MapId, PosX, PosY, PosZ, Orientation, PhaseUseFlags, PhaseId, PhaseGroup, Shape, ShapeData0, ShapeData1, ShapeData2, ScriptName, Comment) 
VALUES (@template_id, @template_id, '1', @spawn_mapid, @spawn_x, @spawn_y, @spawn_z, @spawn_o , '1', '0', '0', '1', @areatrigger_size_x, @areatrigger_size_y, @areatrigger_size_z, @scriptname, @name);
-- script link
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES (@template_id, @scriptname);
