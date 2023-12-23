/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MotionMaster.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"

struct npc_bg_ab_arathor_gryphon_rider_leader : public ScriptedAI
{
    npc_bg_ab_arathor_gryphon_rider_leader(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->GetMotionMaster()->MoveSmoothPath(1, _path, std::size(_path), false, true);
    }

    void MovementInform(uint32 /*type*/, uint32 id) override
    {
        if (id != 1)
            return;

        std::list<Creature*> followers;
        me->GetCreatureListWithEntryInGrid(followers, me->GetEntry());
        for (Creature* follower : followers)
            follower->DespawnOrUnsummon(500ms);

        me->DespawnOrUnsummon(500ms);
    }

private:
    Position const _path[13] =
    {
        { 1394.0209f, 1388.9167f, 153.32262f },
        { 1386.4341f, 1381.7153f, 154.89734f },
        { 1378.8473f, 1374.5139f, 156.47206f },
        { 1234.1875f, 1240.4185f, 156.47206f },
        { 1094.5555f, 1138.9271f, 92.392740f },
        { 1001.7864f, 1033.8993f, 92.392740f },
        { 984.01215f, 991.08680f, 92.392740f },
        { 975.03125f, 916.90625f, 92.392740f },
        { 962.58510f, 842.50180f, 92.392740f },
        { 954.42017f, 765.47570f, 92.392740f },
        { 949.10240f, 676.44617f, 143.17484f },
        { 962.75000f, 423.92014f, 219.66528f },
        { 993.89930f, 76.503470f, 219.66528f }
    };
};

struct npc_bg_ab_defiler_bat_rider_leader : public ScriptedAI
{
    npc_bg_ab_defiler_bat_rider_leader(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->GetMotionMaster()->MoveSmoothPath(1, _path, std::size(_path), false, true);
    }

    void MovementInform(uint32 /*type*/, uint32 id) override
    {
        if (id != 1)
            return;

        std::list<Creature*> followers;
        me->GetCreatureListWithEntryInGrid(followers, me->GetEntry());
        for (Creature* follower : followers)
            follower->DespawnOrUnsummon(500ms);

        me->DespawnOrUnsummon(500ms);
    }

private:
    Position const _path[15] =
    {
        { 629.96870f, 376.45140f, 178.24315f },
        { 635.94270f, 390.70140f, 183.16116f },
        { 641.91670f, 404.95140f, 188.07918f },
        { 637.18750f, 570.82640f, 188.07918f },
        { 664.32294f, 666.79170f, 188.07918f },
        { 747.86115f, 754.01910f, 188.07918f },
        { 772.32810f, 794.37850f, 153.96823f },
        { 821.42365f, 961.00520f, 86.460680f },
        { 854.28820f, 1031.4028f, 86.460680f },
        { 894.32640f, 1095.7466f, 86.460680f },
        { 973.75000f, 1197.5052f, 86.460680f },
        { 1048.9098f, 1304.7448f, 86.460680f },
        { 1105.8663f, 1379.1945f, 134.64104f },
        { 1198.5817f, 1537.4548f, 221.42233f },
        { 1349.2379f, 1750.9497f, 328.39390f }
    };
};

struct npc_bg_ab_the_black_bride: public ScriptedAI
{
    npc_bg_ab_the_black_bride(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        // TODO
        // not sure if this is smooth path
        me->GetMotionMaster()->MoveSmoothPath(1, _path, std::size(_path), true);
        me->DespawnOrUnsummon(2min);
    }

private:
    Position const _path[6] =
    {
        { 656.09375f, 636.46265f, -9.3995950f },
        { 656.09375f, 636.46180f, -9.5859970f },
        { 656.09375f, 636.46094f, -9.7723980f },
        { 656.96875f, 638.25780f, -10.272398f },
        { 657.25180f, 638.83510f, -10.535215f },
        { 657.92017f, 640.82640f, -10.932798f }
    };
};

struct npc_bg_ab_radulf_leder : public ScriptedAI
{
    npc_bg_ab_radulf_leder(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        // TODO
        // not sure if this is smooth path
        me->GetMotionMaster()->MoveSmoothPath(1, _path, std::size(_path), true);
        me->DespawnOrUnsummon(2min);
    }

private:
    Position const _path[8] =
    {
        { 1392.1213f, 1345.5967f, 0.215494220f },
        { 1389.2448f, 1344.7500f, -0.23260853f },
        { 1386.3682f, 1343.9033f, -0.68071127f },
        { 1385.4092f, 1343.6211f, -0.93071127f },
        { 1385.0764f, 1343.5226f, -0.87394860f },
        { 1383.3301f, 1342.5479f, -1.30571130f },
        { 1381.5840f, 1341.5732f, -1.68071130f },
        { 1381.3038f, 1341.4166f, -1.76017900f }
    };
};

struct npc_bg_ab_dominic_masonwrite : ScriptedAI
{
    npc_bg_ab_dominic_masonwrite(Creature* creature) : ScriptedAI(creature) { }

    enum Spells
    {
        SPELL_READ_SCROLL = 122236
    };

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void JustAppeared() override
    {
        StartScript();
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        switch (pointId)
        {
            case 1:
                me->SetFacingTo(2.042035102844238281f);
                DoCastSelf(SPELL_READ_SCROLL);
                _scheduler.Schedule(20s, [this](TaskContext)
                {
                    me->RemoveAurasDueToSpell(SPELL_READ_SCROLL);
                    me->GetMotionMaster()->MoveSmoothPath(2, _path2, std::size(_path2), true);
                });
                break;
            case 2:
                me->SetFacingTo(4.539567470550537109f);
                StartScript();
                break;
            default:
                break;
        }
    }

    void StartScript()
    {
        DoCastSelf(SPELL_READ_SCROLL);
        _scheduler.Schedule(20s, [this](TaskContext context)
        {
            me->RemoveAurasDueToSpell(SPELL_READ_SCROLL);
            me->SetFacingTo(1.431169986724853515f);
            context.Schedule(1s, [this](TaskContext)
            {
                me->GetMotionMaster()->MoveSmoothPath(1, _path1, std::size(_path1), true);
            });
        });
    }

private:
    TaskScheduler _scheduler;

    Position const _path1[12] =
    {
        { 683.56946f, 681.07990f, -15.072695f },
        { 681.10070f, 682.70830f, -15.185122f },
        { 677.38544f, 684.03644f, -15.193178f },
        { 673.01390f, 684.93750f, -14.901796f },
        { 668.88196f, 686.10070f, -14.651186f },
        { 665.32294f, 687.55035f, -14.100773f },
        { 662.53990f, 690.65454f, -14.015690f },
        { 661.71180f, 694.51560f, -13.638004f },
        { 661.72220f, 699.52260f, -13.538639f },
        { 663.10070f, 703.08330f, -13.669305f },
        { 665.31600f, 706.72570f, -13.534661f },
        { 667.43750f, 709.48615f, -13.347998f }
    };

    Position const _path2[9] =
    {
        { 666.94617f, 704.70830f, -13.737280f },
        { 665.11804f, 701.34720f, -13.938592f },
        { 663.72920f, 696.17365f, -14.034611f },
        { 663.91320f, 691.64240f, -14.134220f },
        { 665.21530f, 686.40280f, -14.101261f },
        { 668.96875f, 681.58680f, -14.139223f },
        { 672.43580f, 679.47050f, -14.551455f },
        { 678.19100f, 679.64580f, -15.018129f },
        { 684.04517f, 676.04100f, -14.635073f }
    };
};

// Lumber Mill
struct npc_bg_ab_lumberjack_wood_carrier : ScriptedAI
{
    enum Spells
    {
        SPELL_LUMBERJACKIN = 290604,
        SPELL_CARRY_WOOD = 244453
    };

    enum Items
    {
        ITEM_AXE_1H = 109579
    };

    static constexpr float orientation_path_back = 5.585053443908691406f;

    npc_bg_ab_lumberjack_wood_carrier(Creature* creature) : ScriptedAI(creature), _startOrientation(0.0f)  { }

    virtual size_t GetPath1Size() const = 0;
    virtual Position const* GetPath1() const = 0;
    virtual float GetOrientation1() const = 0;
    virtual size_t GetPath2Size() const = 0;
    virtual Position const* GetPath2() const = 0;

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void JustAppeared() override
    {
        _startOrientation = me->GetOrientation();
        StartScript();
    }

    void StartScript()
    {
        me->SetVirtualItem(0, ITEM_AXE_1H);
        DoCastSelf(SPELL_LUMBERJACKIN);

        _scheduler.Schedule(10s, [this](TaskContext context)
        {
            me->RemoveAurasDueToSpell(SPELL_LUMBERJACKIN);
            me->SetVirtualItem(0, 0);

            DoCastSelf(SPELL_CARRY_WOOD, true);

            context.Schedule(1s, [this](TaskContext context)
            {
                me->SetFacingTo(GetOrientation1());
                context.Schedule(1s, [this](TaskContext)
                {
                    // this is a hack
                    // Flags: 2432696320 (Unknown5, Steering, Unknown10)
                    me->GetMotionMaster()->MoveSmoothPath(1, GetPath1(), GetPath1Size(), true);
                });
            });
        });
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        switch (pointId)
        {
            case 1:
                me->RemoveAurasDueToSpell(SPELL_CARRY_WOOD);
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                _scheduler.Schedule(3500ms, [this](TaskContext)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_STAND);
                    me->SetFacingTo(orientation_path_back);
                    // this is a hack
                    // Flags: 2432696320 (Unknown5, Steering, Unknown10)
                    // In the path it can sometimes switch between smooth path like below
                    // Flags: 2436890624 (UncompressedPath, Unknown5, Steering, Unknown10)
                    me->GetMotionMaster()->MoveSmoothPath(2, GetPath2(), GetPath2Size(), true);
                });
                break;
            case 2:
                me->SetFacingTo(_startOrientation);
                StartScript();
                break;
            default:
                break;
        }
    }

private:
    TaskScheduler _scheduler;
    float _startOrientation;
};

struct npc_bg_ab_lumberjack_wood_carrier_1 : npc_bg_ab_lumberjack_wood_carrier
{
    npc_bg_ab_lumberjack_wood_carrier_1(Creature* creature) : npc_bg_ab_lumberjack_wood_carrier(creature) { }

    size_t GetPath1Size() const override { return std::size(_path); }
    Position const* GetPath1() const override { return _path; }
    float GetOrientation1() const override { return 6.161012172698974609f; }
    size_t GetPath2Size() const override { return std::size(_pathBack); }
    Position const* GetPath2() const override { return _pathBack; }

private:
    Position const _path[21] =
    {
        { 763.60940f, 1162.6997f, 19.185547f },
        { 768.91320f, 1163.0938f, 17.919909f },
        { 773.53990f, 1162.6198f, 16.651964f },
        { 780.72394f, 1160.9202f, 14.971056f },
        { 786.81430f, 1158.3073f, 13.368517f },
        { 792.07640f, 1156.3004f, 12.370470f },
        { 796.70996f, 1154.4209f, 11.467760f },
        { 801.67190f, 1154.1736f, 10.690050f },
        { 806.19100f, 1155.5330f, 10.758654f },
        { 810.42365f, 1157.9254f, 11.216784f },
        { 813.33510f, 1160.5642f, 11.446154f },
        { 815.10420f, 1162.7795f, 11.703234f },
        { 815.94794f, 1165.0591f, 11.685045f },
        { 815.08680f, 1167.5747f, 11.673909f },
        { 812.89240f, 1170.6632f, 11.548909f },
        { 810.76390f, 1173.1250f, 11.820969f },
        { 809.62680f, 1175.0139f, 11.820969f },
        { 808.92883f, 1177.5538f, 11.869650f },
        { 808.85940f, 1179.9062f, 11.919118f },
        { 808.67017f, 1183.1892f, 11.886209f },
        { 808.11804f, 1189.6077f, 11.820969f }
    };

    Position const _pathBack[26] =
    {
        { 817.11804f, 1161.6423f, 11.936632f },
        { 819.98090f, 1157.6528f, 12.288439f },
        { 821.38196f, 1154.5312f, 12.427843f },
        { 821.38196f, 1152.1841f, 11.845812f },
        { 820.17480f, 1150.5879f, 11.578234f },
        { 819.78820f, 1150.0764f, 11.346544f },
        { 817.35940f, 1148.5834f, 11.095690f },
        { 813.87500f, 1148.1649f, 10.398058f },
        { 809.02430f, 1148.3959f, 10.198229f },
        { 803.09375f, 1149.1736f, 10.717760f },
        { 798.41600f, 1150.9414f, 11.717760f },
        { 794.29865f, 1153.0452f, 12.283068f },
        { 790.55554f, 1155.2795f, 12.967272f },
        { 786.27606f, 1158.0330f, 13.725695f },
        { 786.27637f, 1158.0332f, 13.967760f },
        { 785.34375f, 1158.3945f, 14.217760f },
        { 783.47850f, 1159.1172f, 14.717760f },
        { 782.63020f, 1159.4462f, 14.820543f },
        { 782.62990f, 1159.4463f, 14.592760f },
        { 781.66895f, 1159.7236f, 14.967760f },
        { 779.74710f, 1160.2783f, 15.342760f },
        { 777.82520f, 1160.8330f, 15.592760f },
        { 776.86426f, 1161.1104f, 15.967760f },
        { 776.10420f, 1161.3317f, 16.085192f },
        { 770.10450f, 1161.4316f, 17.842760f },
        { 757.39760f, 1162.5000f, 20.350830f }
    };
};

struct npc_bg_ab_lumberjack_wood_carrier_2 : npc_bg_ab_lumberjack_wood_carrier
{
    npc_bg_ab_lumberjack_wood_carrier_2(Creature* creature) : npc_bg_ab_lumberjack_wood_carrier(creature) { }

    size_t GetPath1Size() const override { return std::size(_path); }
    Position const* GetPath1() const override { return _path; }
    float GetOrientation1() const override { return 2.914699792861938476f; }
    size_t GetPath2Size() const override { return std::size(_pathBack); }
    Position const* GetPath2() const override { return _pathBack; }

private:
    Position const _path[27] =
    {
        { 898.26910f, 1182.4705f, 6.0160184f },
        { 894.47266f, 1181.2090f, 7.2055936f },
        { 890.85767f, 1181.6632f, 8.2749290f },
        { 887.24650f, 1182.3420f, 8.8231470f },
        { 883.76740f, 1183.5087f, 9.3508570f },
        { 880.16320f, 1184.9149f, 9.5843770f },
        { 875.34375f, 1186.5903f, 9.7420920f },
        { 870.68400f, 1188.0504f, 9.2020530f },
        { 864.78125f, 1189.4827f, 8.8908010f },
        { 860.43750f, 1190.4618f, 9.6863340f },
        { 857.81080f, 1190.9844f, 10.066461f },
        { 855.62150f, 1191.5798f, 10.027032f },
        { 853.07294f, 1192.7466f, 9.8742000f },
        { 850.10940f, 1193.5764f, 9.9905330f },
        { 846.69965f, 1194.5851f, 10.115777f },
        { 846.69920f, 1194.5850f, 10.316216f },
        { 842.94100f, 1195.4080f, 10.293267f },
        { 837.82810f, 1196.1442f, 10.341363f },
        { 834.32465f, 1196.6146f, 10.759820f },
        { 829.89410f, 1196.9358f, 11.304280f },
        { 827.27080f, 1197.5798f, 11.441854f },
        { 825.77780f, 1197.6406f, 11.613729f },
        { 821.12500f, 1196.4305f, 11.951375f },
        { 818.02606f, 1194.6024f, 11.995651f },
        { 814.62150f, 1190.9723f, 11.954003f },
        { 813.22570f, 1190.0868f, 11.948299f },
        { 810.54517f, 1190.8403f, 11.924964f }
    };

    Position const _pathBack[21] =
    {
        { 817.30900f, 1192.8160f, 11.9687850f },
        { 819.99304f, 1195.8330f, 11.8855790f },
        { 823.78300f, 1199.3611f, 11.6324050f },
        { 826.57810f, 1200.3785f, 11.3140460f },
        { 831.10240f, 1200.7952f, 10.9239090f },
        { 838.42535f, 1200.7344f, 10.5952425f },
        { 842.16490f, 1200.3854f, 10.3479280f },
        { 847.22220f, 1199.1945f, 9.81621600f },
        { 853.81600f, 1197.1754f, 9.54570900f },
        { 855.68555f, 1196.4648f, 9.31621600f },
        { 858.21704f, 1195.5000f, 9.14629500f },
        { 858.21680f, 1195.5000f, 9.31621600f },
        { 862.91406f, 1193.7910f, 9.06621600f },
        { 864.79297f, 1193.1074f, 8.81621600f },
        { 865.60590f, 1192.8142f, 8.75603600f },
        { 870.72570f, 1190.3055f, 8.93142300f },
        { 874.51044f, 1188.2760f, 9.14053000f },
        { 879.54170f, 1186.2344f, 9.25857200f },
        { 885.18230f, 1184.0868f, 8.80288300f },
        { 890.51390f, 1182.8698f, 8.10024600f },
        { 904.16700f, 1182.9531f, 3.92786800f }
    };
};

struct npc_bg_ab_lumberjack_wood_carrier_3 : npc_bg_ab_lumberjack_wood_carrier
{
    npc_bg_ab_lumberjack_wood_carrier_3(Creature* creature) : npc_bg_ab_lumberjack_wood_carrier(creature) { }

    size_t GetPath1Size() const override { return std::size(_path); }
    Position const* GetPath1() const override { return _path; }
    float GetOrientation1() const override { return 1.326450228691101074f; }
    size_t GetPath2Size() const override { return std::size(_pathBack); }
    Position const* GetPath2() const override { return _pathBack; }

private:
    Position const _path[24] =
    {
        { 816.76390f, 1103.9497f, 11.596175f },
        { 817.82810f, 1106.5435f, 12.693831f },
        { 818.93230f, 1110.2517f, 13.963119f },
        { 819.54785f, 1113.1875f, 14.296615f },
        { 820.28990f, 1116.9098f, 14.549300f },
        { 821.73090f, 1122.4913f, 14.443831f },
        { 821.30554f, 1128.2240f, 12.163192f },
        { 820.90970f, 1131.8403f, 11.538314f },
        { 819.78820f, 1135.9271f, 10.516588f },
        { 818.54865f, 1139.9688f, 10.094103f },
        { 818.52080f, 1144.5399f, 10.668322f },
        { 817.79517f, 1149.6111f, 11.359972f },
        { 817.69446f, 1154.2396f, 11.824816f },
        { 816.99133f, 1159.2448f, 11.906115f },
        { 814.95140f, 1164.3854f, 11.646105f },
        { 812.21180f, 1169.9531f, 11.548909f },
        { 808.78644f, 1173.4896f, 11.820969f },
        { 808.72490f, 1173.6804f, 11.820969f },
        { 808.10240f, 1175.6111f, 11.820969f },
        { 807.93750f, 1176.5817f, 11.860204f },
        { 807.80554f, 1177.7709f, 11.958746f },
        { 806.81600f, 1180.4392f, 11.944935f },
        { 804.06946f, 1183.7361f, 11.951467f },
        { 802.97394f, 1184.8455f, 11.956316f }
    };

    Position const _pathBack[25] =
    {
        { 807.85420f, 1175.5747f, 11.8209690f },
        { 809.12500f, 1173.4010f, 11.8209690f },
        { 810.95830f, 1171.2188f, 11.8209690f },
        { 812.53644f, 1169.5399f, 11.5489090f },
        { 813.74133f, 1165.9635f, 11.5396595f },
        { 814.00520f, 1163.1823f, 11.5712760f },
        { 813.55206f, 1154.9531f, 11.0806750f },
        { 813.35767f, 1148.5382f, 10.3807240f },
        { 812.93750f, 1141.3750f, 10.2206900f },
        { 813.04170f, 1137.6805f, 9.91246200f },
        { 813.62150f, 1133.6476f, 9.74705700f },
        { 813.99900f, 1130.6709f, 10.2966150f },
        { 814.68054f, 1127.7743f, 11.0800620f },
        { 814.99023f, 1126.8232f, 11.6716150f },
        { 815.57117f, 1125.0416f, 12.3871910f },
        { 815.57130f, 1125.0420f, 12.0466150f },
        { 815.91700f, 1124.1035f, 12.9216150f },
        { 816.26270f, 1123.1650f, 13.1716150f },
        { 816.60840f, 1122.2266f, 13.5466150f },
        { 816.71180f, 1121.9497f, 13.8431230f },
        { 817.82227f, 1119.1631f, 14.5466150f },
        { 818.89580f, 1116.0087f, 14.7801350f },
        { 818.67190f, 1112.9427f, 14.3497150f },
        { 817.30383f, 1107.4410f, 12.8464190f },
        { 814.28820f, 1096.7656f, 10.5019870f }
    };
};

struct npc_bg_ab_lumberjack_wood_carrier_4 : npc_bg_ab_lumberjack_wood_carrier
{
    npc_bg_ab_lumberjack_wood_carrier_4(Creature* creature) : npc_bg_ab_lumberjack_wood_carrier(creature) { }

    size_t GetPath1Size() const override { return std::size(_path); }
    Position const* GetPath1() const override { return _path; }
    float GetOrientation1() const override { return 2.914699792861938476f; }
    size_t GetPath2Size() const override { return std::size(_pathBack); }
    Position const* GetPath2() const override { return _pathBack; }

private:
    Position const _path[21] =
    {
        { 820.56250f, 1253.2118f, 22.741510f },
        { 817.42190f, 1250.6771f, 22.510065f },
        { 814.84030f, 1247.6459f, 22.467340f },
        { 813.21010f, 1243.2986f, 21.401056f },
        { 813.59720f, 1238.7344f, 19.852716f },
        { 814.07640f, 1234.1945f, 18.955744f },
        { 815.24304f, 1228.7240f, 17.935383f },
        { 817.93400f, 1222.2882f, 14.859212f },
        { 819.77606f, 1218.1060f, 13.147786f },
        { 821.81430f, 1213.1094f, 11.934529f },
        { 824.09894f, 1208.4010f, 11.673909f },
        { 824.52780f, 1205.6423f, 11.407796f },
        { 824.08510f, 1202.4670f, 11.471272f },
        { 822.54517f, 1198.9531f, 11.780965f },
        { 821.11633f, 1195.6910f, 11.798909f },
        { 818.05900f, 1190.7067f, 11.952708f },
        { 816.19100f, 1188.0104f, 11.904458f },
        { 814.51044f, 1186.5642f, 11.880142f },
        { 811.92883f, 1185.3577f, 11.820969f },
        { 807.23440f, 1185.7310f, 11.880688f },
        { 804.28300f, 1186.1771f, 11.921508f }
    };

    Position const _pathBack[23] =
    {
        { 819.90454f, 1193.0139f, 11.996414f },
        { 822.26390f, 1195.8330f, 11.798909f },
        { 825.00000f, 1197.9160f, 11.673909f },
        { 825.22920f, 1198.7535f, 11.545857f },
        { 825.96580f, 1200.6133f, 11.298909f },
        { 826.05900f, 1200.8490f, 11.320027f },
        { 826.64240f, 1204.1476f, 11.359090f },
        { 826.43230f, 1206.6997f, 11.314534f },
        { 825.08984f, 1211.5137f, 11.798909f },
        { 824.85767f, 1212.3507f, 11.968465f },
        { 824.85740f, 1212.3506f, 11.798909f },
        { 824.50195f, 1214.3193f, 12.298909f },
        { 823.96875f, 1217.2725f, 13.423909f },
        { 823.90625f, 1217.6077f, 13.562703f },
        { 822.64060f, 1222.0173f, 14.755696f },
        { 821.30760f, 1226.8369f, 16.673908f },
        { 819.63020f, 1232.4479f, 18.505207f },
        { 818.15625f, 1237.7986f, 19.616755f },
        { 817.15970f, 1242.3923f, 20.581354f },
        { 817.34720f, 1246.1354f, 21.580378f },
        { 818.43750f, 1249.5295f, 22.212702f },
        { 820.62850f, 1251.7291f, 22.377863f },
        { 825.77954f, 1255.8837f, 22.169000f }
    };
};

struct npc_bg_ab_lumberjack_wanderer : ScriptedAI
{
    npc_bg_ab_lumberjack_wanderer(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        StartScript();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void StartScript()
    {
        _scheduler.Schedule(5s, 10s, [this](TaskContext context)
        {
            me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
            context.Schedule(3500ms, [this](TaskContext)
            {
                // this is a hack
                // Flags: 2432696320 (Unknown5, Steering, Unknown10)
                me->GetMotionMaster()->MoveSmoothPath(1, _path, std::size(_path), true);
            });
        });
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        switch (pointId)
        {
            case 1:
                me->SetEmoteState(EMOTE_STATE_USE_STANDING);
                _scheduler.Schedule(6s, [this](TaskContext)
                {
                    me->SetEmoteState(EMOTE_ONESHOT_NONE);
                    // this is a hack
                    // Flags: 2432696320 (Unknown5, Steering, Unknown10)
                    // Although, in this path suddenly smooth path is being used
                    // Flags: 2436890624 (UncompressedPath, Unknown5, Steering, Unknown10)
                    me->GetMotionMaster()->MoveSmoothPath(2, _pathBack, std::size(_pathBack), true);
                });
                break;
            case 2:
                me->SetFacingTo(1.8916287f);
                StartScript();
                break;
            default:
                break;
        }
    }

private:
    TaskScheduler _scheduler;

    Position const _path[15] =
    {
        { 856.48090f, 1229.4810f, 14.3900420f },
        { 857.34180f, 1227.6758f, 13.9702425f },
        { 858.63380f, 1224.9688f, 13.5952425f },
        { 858.67365f, 1224.8854f, 13.3668490f },
        { 858.67380f, 1224.8857f, 13.5952425f },
        { 859.46680f, 1223.0498f, 13.2202425f },
        { 860.25977f, 1221.2139f, 12.9702425f },
        { 860.60420f, 1220.4166f, 12.7002230f },
        { 861.36456f, 1215.1354f, 11.9964880f },
        { 859.28990f, 1211.0104f, 11.4966100f },
        { 851.72740f, 1204.9861f, 9.79202000f },
        { 847.78990f, 1202.3403f, 9.79787900f },
        { 844.04865f, 1198.8004f, 10.1364070f },
        { 838.91490f, 1195.3785f, 10.5601130f },
        { 836.15625f, 1193.8334f, 11.0199520f }
    };

    Position const _pathBack[7] =
    {
        { 863.21530f, 1207.7517f, 10.196927f },
        { 865.19100f, 1212.7709f, 10.503690f },
        { 866.67535f, 1217.4166f, 11.192437f },
        { 866.73785f, 1221.1337f, 12.000665f },
        { 865.20490f, 1224.3438f, 12.579007f },
        { 860.36804f, 1229.3802f, 13.751737f },
        { 854.49480f, 1233.6702f, 15.472480f }
    };
};

struct npc_bg_ab_lumberjack_passive : ScriptedAI
{
    npc_bg_ab_lumberjack_passive(Creature* creature) : ScriptedAI(creature) { }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void JustAppeared() override
    {
        me->SetEmoteState(EMOTE_STATE_SIT_CHAIR_MED);
        _scheduler.Schedule(5s, 10s, [this](TaskContext context)
        {
            me->PlayOneShotAnimKitId(17347); // drink
            context.Repeat();
        });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_bg_ab_lumberjack : ScriptedAI
{
    npc_bg_ab_lumberjack(Creature* creature) : ScriptedAI(creature) { }

    enum Spells
    {
        SPELL_LUMBERJACKIN = 290604
    };

    void JustAppeared() override
    {
        DoCastSelf(SPELL_LUMBERJACKIN);
    }
};

// Blacksmith
struct npc_bg_ab_blacksmith_sitting : ScriptedAI
{
    npc_bg_ab_blacksmith_sitting(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->SetEmoteState(EMOTE_STATE_SIT_CHAIR_MED);
        _scheduler.Schedule(5s, 10s, [this](TaskContext context)
        {
            me->PlayOneShotAnimKitId(5182);
            context.Repeat();
        });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_bg_ab_blacksmith_talking : ScriptedAI
{
    npc_bg_ab_blacksmith_talking(Creature* creature) : ScriptedAI(creature) { }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void JustAppeared() override
    {
        _scheduler.Schedule(10s, 15s, [this](TaskContext context)
        {
            me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
            context.Repeat();
        });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_bg_ab_blacksmith_working_base : ScriptedAI
{
    static constexpr int32 ACTION_RESTORE_WEAPON = 1;
    static constexpr uint32 SPELL_BLACKSMITH_WORKING = 261985;

    npc_bg_ab_blacksmith_working_base(Creature* creature, uint32 createdItem, bool removeItem) : ScriptedAI(creature), _originalOrientation(0.0f), _createdItem(createdItem), _removeItem(removeItem) { }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void JustAppeared() override
    {
        _originalOrientation = me->GetOrientation();
        StartScript();
    }

    void MovementInform(uint32 /*type*/, uint32 id) override
    {
        switch (id)
        {
            case 1:
                if (_removeItem)
                    me->LoadEquipment(0, true);

                me->SetEmoteState(EMOTE_STATE_USE_STANDING);
                _scheduler.Schedule(5s, [this](TaskContext /*context*/)
                {
                    me->SetEmoteState(EMOTE_ONESHOT_NONE);
                    me->GetMotionMaster()->MoveSmoothPath(2, GetPath2(), GetPath2Size(), true);
                });
                break;
            case 2:
                me->SetFacingTo(_originalOrientation);
                StartScript();
                break;
            default:
                break;
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_RESTORE_WEAPON:
                me->SetVirtualItem(0, _createdItem);
                break;
            default:
                break;
        }
    }

    void StartScript()
    {
        DoCastSelf(SPELL_BLACKSMITH_WORKING);
        _scheduler.Schedule(10s, 20s, [this](TaskContext context)
        {
            me->SetFacingTo(5.829399585723876953f); // what's up with this facing? Its being used multiple times

            context.Schedule(1s, [this](TaskContext /*context*/)
            {
                me->GetMotionMaster()->MoveSmoothPath(1, GetPath1(), GetPath1Size(), true);
            });
        });
    }

    virtual size_t GetPath1Size() const = 0;
    virtual Position const* GetPath1() const = 0;
    virtual size_t GetPath2Size() const = 0;
    virtual Position const* GetPath2() const = 0;

private:
    TaskScheduler _scheduler;
    float _originalOrientation;
    uint32 _createdItem;
    bool _removeItem;
};

struct npc_bg_ab_blacksmith_working_1 : npc_bg_ab_blacksmith_working_base
{
    static constexpr uint32 ITEM_SWORD_SCIMATAR_BADASS = 2179;
    npc_bg_ab_blacksmith_working_1(Creature* creature) : npc_bg_ab_blacksmith_working_base(creature, ITEM_SWORD_SCIMATAR_BADASS, true) { }

    size_t GetPath1Size() const override { return std::size(_path); }
    Position const* GetPath1() const override { return _path; }
    size_t GetPath2Size() const override { return std::size(_path2); }
    Position const* GetPath2() const override { return _path2; }
private:
    Position const _path[5] =
    {
        { 972.2552f, 995.43750f, -44.371353f },
        { 974.3698f, 998.61804f, -44.399430f },
        { 974.4549f, 1000.1684f, -44.628548f },
        { 974.7083f, 1003.1945f, -44.339120f },
        { 976.2066f, 1005.1927f, -44.089120f }
    };

    Position const _path2[3] =
    {
        { 974.00000f, 1000.8090f, -44.497078f },
        { 970.75867f, 997.15280f, -44.493546f },
        { 967.70800f, 996.73785f, -44.027725f }
    };
};

struct npc_bg_ab_blacksmith_working_2 : npc_bg_ab_blacksmith_working_base
{
    static constexpr uint32 ITEM_SWORD = 155799; // no name found
    npc_bg_ab_blacksmith_working_2(Creature* creature) : npc_bg_ab_blacksmith_working_base(creature, ITEM_SWORD, false) { }

    size_t GetPath1Size() const override { return std::size(_path); }
    Position const* GetPath1() const override { return _path; }
    size_t GetPath2Size() const override { return std::size(_path2); }
    Position const* GetPath2() const override { return _path2; }

private:
    Position const _path[3] =
    {
        { 993.61285f, 1004.66140f, -42.179028f },
        { 995.52344f, 1002.85455f, -42.179028f },
        { 995.81250f, 1000.52780f, -42.426790f }
    };

    Position const _path2[3] =
    {
        { 995.12680f, 1005.9514f, -42.179028f },
        { 992.38196f, 1005.8177f, -42.179028f },
        { 988.95490f, 1005.0070f, -42.179030f }
    };
};

struct npc_bg_ab_blacksmith_stone_carrier : ScriptedAI
{
    static constexpr uint32 SPELL_CARRY_STONE = 282906;

    npc_bg_ab_blacksmith_stone_carrier(Creature* creature) : ScriptedAI(creature), _originalOrientation(0.0f) { }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

    void JustAppeared() override
    {
        _originalOrientation = me->GetOrientation();
        StartScript();
    }

    void MovementInform(uint32 /*type*/, uint32 id) override
    {
        switch (id)
        {
            case 1:
                me->RemoveAurasDueToSpell(SPELL_CARRY_STONE);
                me->SetEmoteState(EMOTE_STATE_USE_STANDING);
                _scheduler.Schedule(5s, [this](TaskContext /*context*/)
                {
                    me->SetEmoteState(EMOTE_ONESHOT_NONE);
                    me->GetMotionMaster()->MoveSmoothPath(2, _path2, std::size(_path2), true);
                });
                break;
            case 2:
                me->SetFacingTo(_originalOrientation);
                StartScript();
                break;
            default:
                break;
        }
    }

    void StartScript()
    {
        me->SetEmoteState(EMOTE_STATE_USE_STANDING);
        _scheduler.Schedule(5s, [this](TaskContext context)
        {
            me->SetEmoteState(EMOTE_ONESHOT_NONE);
            me->SetFacingTo(5.340707302093505859f);
            DoCastSelf(SPELL_CARRY_STONE);
            context.Schedule(1s, [this](TaskContext /*context*/)
            {
                me->GetMotionMaster()->MoveSmoothPath(1, _path, std::size(_path), true);
            });
        });
    }

private:
    TaskScheduler _scheduler;
    float _originalOrientation;

    Position const _path[28] =
    {
        { 975.20830f, 970.99830f, -44.298965f },
        { 977.93054f, 968.85940f, -44.310684f },
        { 980.48090f, 967.64580f, -44.676650f },
        { 983.70830f, 966.57294f, -45.271420f },
        { 983.70800f, 966.57324f, -44.986263f },
        { 985.69434f, 966.33300f, -44.736263f },
        { 986.68750f, 966.21290f, -44.486263f },
        { 987.31250f, 966.13544f, -44.531185f },
        { 991.44965f, 966.54865f, -43.586850f },
        { 996.54340f, 967.62680f, -43.219130f },
        { 999.69270f, 969.21010f, -43.496720f },
        { 1002.9462f, 970.83160f, -43.768970f },
        { 1006.0521f, 973.06600f, -43.890675f },
        { 1008.3768f, 975.68230f, -43.877980f },
        { 1010.1250f, 979.04690f, -43.887135f },
        { 1011.0000f, 987.86115f, -43.916065f },
        { 1010.8698f, 992.03300f, -44.185230f },
        { 1009.9462f, 994.50696f, -44.191334f },
        { 1008.6162f, 997.19630f, -44.435230f },
        { 1008.2379f, 997.95830f, -44.361378f },
        { 1006.3750f, 1000.4583f, -43.751490f },
        { 1003.4427f, 1002.5417f, -42.423004f },
        { 999.16670f, 1005.2535f, -42.599384f },
        { 994.89060f, 1007.7465f, -42.179028f },
        { 991.21010f, 1009.6059f, -42.179028f },
        { 987.20490f, 1011.2656f, -42.658790f },
        { 985.44100f, 1011.3264f, -42.672867f },
        { 982.34204f, 1009.6250f, -42.658398f }
    };

    Position const _path2[28] =
    {
        { 984.96356f, 1012.9236f, -42.639954f },
        { 982.86285f, 1014.7570f, -42.382156f },
        { 978.98615f, 1017.9688f, -44.173225f },
        { 974.40970f, 1020.5521f, -44.693733f },
        { 970.86456f, 1019.9774f, -45.456795f },
        { 968.49830f, 1019.3299f, -45.332040f },
        { 966.33856f, 1018.0625f, -45.338310f },
        { 964.56600f, 1015.5052f, -45.591484f },
        { 963.10940f, 1011.6476f, -45.570244f },
        { 961.63196f, 1006.4132f, -45.967705f },
        { 958.08680f, 999.61804f, -47.041462f },
        { 956.02780f, 996.21010f, -47.037556f },
        { 954.94794f, 992.51390f, -47.363970f },
        { 954.96180f, 990.39240f, -46.776447f },
        { 957.01044f, 987.75350f, -45.411457f },
        { 957.01074f, 987.75390f, -45.590656f },
        { 957.99510f, 987.57910f, -45.215656f },
        { 958.97950f, 987.40430f, -44.715656f },
        { 959.79340f, 987.25867f, -44.386555f },
        { 960.79297f, 987.27440f, -44.215656f },
        { 962.79297f, 987.30566f, -43.965656f },
        { 963.19100f, 987.31250f, -44.110188f },
        { 966.20490f, 987.55383f, -43.965656f },
        { 971.07640f, 986.16320f, -44.103043f },
        { 972.71180f, 984.27080f, -44.174330f },
        { 973.83160f, 981.18400f, -44.183730f },
        { 974.39930f, 978.69965f, -44.251846f },
        { 971.12850f, 976.04200f, -43.859390f }
    };
};

// Farm
struct npc_bg_ab_farmer_talking : ScriptedAI
{
    npc_bg_ab_farmer_talking(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        _scheduler.Schedule(9s, 11s, [this](TaskContext context)
        {
            me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
            context.Repeat();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    TaskScheduler _scheduler;
};

// Spells
// 261985 - Blacksmith Working
class spell_bg_ab_blacksmith_working : public AuraScript
{
    PrepareAuraScript(spell_bg_ab_blacksmith_working);

    static constexpr uint32 ITEM_BLACKSMITH_HAMMER = 5956;

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetUnitOwner()->SetVirtualItem(0, ITEM_BLACKSMITH_HAMMER);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetUnitOwner())
            if (UnitAI* ai = owner->GetAI())
                ai->DoAction(npc_bg_ab_blacksmith_working_base::ACTION_RESTORE_WEAPON);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectRemoveFn(spell_bg_ab_blacksmith_working::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_bg_ab_blacksmith_working::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_arathi_basin()
{
    RegisterCreatureAI(npc_bg_ab_arathor_gryphon_rider_leader);
    RegisterCreatureAI(npc_bg_ab_defiler_bat_rider_leader);
    RegisterCreatureAI(npc_bg_ab_the_black_bride);
    RegisterCreatureAI(npc_bg_ab_radulf_leder);
    RegisterCreatureAI(npc_bg_ab_dominic_masonwrite);
    RegisterCreatureAI(npc_bg_ab_lumberjack);
    RegisterCreatureAI(npc_bg_ab_lumberjack_wood_carrier_1);
    RegisterCreatureAI(npc_bg_ab_lumberjack_wood_carrier_2);
    RegisterCreatureAI(npc_bg_ab_lumberjack_wood_carrier_3);
    RegisterCreatureAI(npc_bg_ab_lumberjack_wanderer);
    RegisterCreatureAI(npc_bg_ab_lumberjack_wood_carrier_4);
    RegisterCreatureAI(npc_bg_ab_lumberjack_passive);
    RegisterCreatureAI(npc_bg_ab_blacksmith_sitting);
    RegisterCreatureAI(npc_bg_ab_blacksmith_talking);
    RegisterCreatureAI(npc_bg_ab_blacksmith_working_1);
    RegisterCreatureAI(npc_bg_ab_blacksmith_working_2);
    RegisterCreatureAI(npc_bg_ab_blacksmith_stone_carrier);
    RegisterCreatureAI(npc_bg_ab_farmer_talking);
    RegisterSpellScript(spell_bg_ab_blacksmith_working);
}