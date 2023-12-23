/*
 * Copyright (C) 2022 DekkCore
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

#include "Garrison.h"
#include "GarrisonAI.h"
#include "Player.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"

class instance_ffr_horde_garrison_level_1 : public InstanceMapScript
{
public:
    instance_ffr_horde_garrison_level_1() : InstanceMapScript("instance_ffr_horde_garrison_level_1", 1152) { }

    struct instance_ffr_horde_garrison_level_1_InstanceMapScript : public InstanceScript
    {
        instance_ffr_horde_garrison_level_1_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_ffr_horde_garrison_level_1_InstanceMapScript(map);
    }
};

class instance_ffr_horde_garrison_level_2 : public InstanceMapScript
{
public:
    instance_ffr_horde_garrison_level_2() : InstanceMapScript("instance_ffr_horde_garrison_level_2", 1330 /* Blizzard remade this map */) { }

    struct instance_ffr_horde_garrison_level_2_InstanceMapScript : public InstanceScript
    {
        instance_ffr_horde_garrison_level_2_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_ffr_horde_garrison_level_2_InstanceMapScript(map);
    }
};

class instance_ffr_horde_garrison_level_3 : public InstanceMapScript
{
public:
    instance_ffr_horde_garrison_level_3() : InstanceMapScript("instance_ffr_horde_garrison_level_3", 1154) { }

    struct instance_ffr_horde_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_ffr_horde_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_ffr_horde_garrison_level_3_InstanceMapScript(map);
    }
};

class instance_smv_alliance_garrison_level_1 : public InstanceMapScript
{
public:
    instance_smv_alliance_garrison_level_1() : InstanceMapScript("instance_smv_alliance_garrison_level_1", 1158) { }

    struct instance_smv_alliance_garrison_level_1_InstanceMapScript : public InstanceScript
    {
        instance_smv_alliance_garrison_level_1_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }

        void OnPlayerEnter(Player* player) override
        {
            if (Garrison* garrison = player->GetGarrison(GARRISON_TYPE_GARRISON))
                garrison->Enter();
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_smv_alliance_garrison_level_1_InstanceMapScript(map);
    }
};

class instance_smv_alliance_garrison_level_2 : public InstanceMapScript
{
public:
    instance_smv_alliance_garrison_level_2() : InstanceMapScript("instance_smv_alliance_garrison_level_2", 1331 /* Blizzard remade this map */) { }

    struct instance_smv_alliance_garrison_level_2_InstanceMapScript : public InstanceScript
    {
        instance_smv_alliance_garrison_level_2_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_smv_alliance_garrison_level_2_InstanceMapScript(map);
    }
};

class instance_smv_alliance_garrison_level_3 : public InstanceMapScript
{
public:
    instance_smv_alliance_garrison_level_3() : InstanceMapScript("instance_smv_alliance_garrison_level_3", 1160) { }

    struct instance_smv_alliance_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_smv_alliance_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_smv_alliance_garrison_level_3_InstanceMapScript(map);
    }
};

class instance_gor_alliance_garrison_level_1 : public InstanceMapScript
{
public:
    instance_gor_alliance_garrison_level_1() : InstanceMapScript("instance_gor_alliance_garrison_level_1", 1187) { }

    struct instance_gor_alliance_garrison_level_1_InstanceMapScript : public InstanceScript
    {
        instance_gor_alliance_garrison_level_1_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_gor_alliance_garrison_level_1_InstanceMapScript(map);
    }
};

class instance_gor_alliance_garrison_level_2 : public InstanceMapScript
{
public:
    instance_gor_alliance_garrison_level_2() : InstanceMapScript("instance_gor_alliance_garrison_level_2", 1188) { }

    struct instance_gor_alliance_garrison_level_2_InstanceMapScript : public InstanceScript
    {
        instance_gor_alliance_garrison_level_2_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_gor_alliance_garrison_level_2_InstanceMapScript(map);
    }
};

class instance_gor_alliance_garrison_level_3 : public InstanceMapScript
{
public:
    instance_gor_alliance_garrison_level_3() : InstanceMapScript("instance_gor_alliance_garrison_level_3", 1189) { }

    struct instance_gor_alliance_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_gor_alliance_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_gor_alliance_garrison_level_3_InstanceMapScript(map);
    }
};

class instance_gor_horde_garrison_level_0 : public InstanceMapScript
{
public:
    instance_gor_horde_garrison_level_0() : InstanceMapScript("instance_gor_horde_garrison_level_0", 1232) { }

    struct instance_gor_horde_garrison_level_0_InstanceMapScript : public InstanceScript
    {
        instance_gor_horde_garrison_level_0_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_gor_horde_garrison_level_0_InstanceMapScript(map);
    }
};

class instance_gor_horde_garrison_level_3 : public InstanceMapScript
{
public:
    instance_gor_horde_garrison_level_3() : InstanceMapScript("instance_gor_horde_garrison_level_3", 1233) { }

    struct instance_gor_horde_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_gor_horde_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_gor_horde_garrison_level_3_InstanceMapScript(map);
    }
};

class instance_tal_alliance_garrison_level_0 : public InstanceMapScript
{
public:
    instance_tal_alliance_garrison_level_0() : InstanceMapScript("instance_tal_alliance_garrison_level_0", 1234) { }

    struct instance_tal_alliance_garrison_level_0_InstanceMapScript : public InstanceScript
    {
        instance_tal_alliance_garrison_level_0_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_tal_alliance_garrison_level_0_InstanceMapScript(map);
    }
};

class instance_tal_alliance_garrison_level_3 : public InstanceMapScript
{
public:
    instance_tal_alliance_garrison_level_3() : InstanceMapScript("instance_tal_alliance_garrison_level_3", 1235) { }

    struct instance_tal_alliance_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_tal_alliance_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_tal_alliance_garrison_level_3_InstanceMapScript(map);
    }
};

class instance_tal_horde_garrison_level_0 : public InstanceMapScript
{
public:
    instance_tal_horde_garrison_level_0() : InstanceMapScript("instance_tal_horde_garrison_level_0", 1236) { }

    struct instance_tal_horde_garrison_level_0_InstanceMapScript : public InstanceScript
    {
        instance_tal_horde_garrison_level_0_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_tal_horde_garrison_level_0_InstanceMapScript(map);
    }
};

class instance_tal_horde_garrison_level_3 : public InstanceMapScript
{
public:
    instance_tal_horde_garrison_level_3() : InstanceMapScript("instance_tal_horde_garrison_level_3", 1237) { }

    struct instance_tal_horde_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_tal_horde_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_tal_horde_garrison_level_3_InstanceMapScript(map);
    }
};

class instance_soa_alliance_garrison_0 : public InstanceMapScript
{
public:
    instance_soa_alliance_garrison_0() : InstanceMapScript("instance_soa_alliance_garrison_0", 1238) { }

    struct instance_soa_alliance_garrison_0_InstanceMapScript : public InstanceScript
    {
        instance_soa_alliance_garrison_0_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_soa_alliance_garrison_0_InstanceMapScript(map);
    }
};

class instance_soa_alliance_garrison_3 : public InstanceMapScript
{
public:
    instance_soa_alliance_garrison_3() : InstanceMapScript("instance_soa_alliance_garrison_3", 1239) { }

    struct instance_soa_alliance_garrison_3_InstanceMapScript : public InstanceScript
    {
        instance_soa_alliance_garrison_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_soa_alliance_garrison_3_InstanceMapScript(map);
    }
};

class instance_soa_horde_garrison_0 : public InstanceMapScript
{
public:
    instance_soa_horde_garrison_0() : InstanceMapScript("instance_soa_horde_garrison_0", 1240) { }

    struct instance_soa_horde_garrison_0_InstanceMapScript : public InstanceScript
    {
        instance_soa_horde_garrison_0_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_soa_horde_garrison_0_InstanceMapScript(map);
    }
};

class instance_soa_horde_garrison_3 : public InstanceMapScript
{
public:
    instance_soa_horde_garrison_3() : InstanceMapScript("instance_soa_horde_garrison_3", 1241) { }

    struct instance_soa_horde_garrison_3_InstanceMapScript : public InstanceScript
    {
        instance_soa_horde_garrison_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_soa_horde_garrison_3_InstanceMapScript(map);
    }
};

class instance_nag_alliance_garrison_level_0 : public InstanceMapScript
{
public:
    instance_nag_alliance_garrison_level_0() : InstanceMapScript("instance_nag_alliance_garrison_level_0", 1242) { }

    struct instance_nag_alliance_garrison_level_0_InstanceMapScript : public InstanceScript
    {
        instance_nag_alliance_garrison_level_0_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_nag_alliance_garrison_level_0_InstanceMapScript(map);
    }
};

class instance_nag_alliance_garrison_level_3 : public InstanceMapScript
{
public:
    instance_nag_alliance_garrison_level_3() : InstanceMapScript("instance_nag_alliance_garrison_level_3", 1243) { }

    struct instance_nag_alliance_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_nag_alliance_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_nag_alliance_garrison_level_3_InstanceMapScript(map);
    }
};

class instance_nag_horde_garrison_level_0 : public InstanceMapScript
{
public:
    instance_nag_horde_garrison_level_0() : InstanceMapScript("instance_nag_horde_garrison_level_0", 1244) { }

    struct instance_nag_horde_garrison_level_0_InstanceMapScript : public InstanceScript
    {
        instance_nag_horde_garrison_level_0_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_nag_horde_garrison_level_0_InstanceMapScript(map);
    }
};

class instance_nag_horde_garrison_level_3 : public InstanceMapScript
{
public:
    instance_nag_horde_garrison_level_3() : InstanceMapScript("instance_nag_horde_garrison_level_3", 1245) { }

    struct instance_nag_horde_garrison_level_3_InstanceMapScript : public InstanceScript
    {
        instance_nag_horde_garrison_level_3_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_nag_horde_garrison_level_3_InstanceMapScript(map);
    }
};

void AddSC_garrison_level_1()
{
    /* Horde Garrisons*/

   // Frostfire Ridge
    new instance_ffr_horde_garrison_level_1();
    new instance_ffr_horde_garrison_level_2();
    new instance_ffr_horde_garrison_level_3();

    // Gorgrond
    new instance_gor_horde_garrison_level_0();
    new instance_gor_horde_garrison_level_3();

    // Talador
    new instance_tal_horde_garrison_level_0();
    new instance_tal_horde_garrison_level_3();

    // Spikes of Arak
    new instance_soa_horde_garrison_0();
    new instance_soa_horde_garrison_3();

    // Nagrand
    new instance_nag_horde_garrison_level_0();
    new instance_nag_horde_garrison_level_3();

    /* Alliance Garrisons */

    // Shadowmoon Valley
    new instance_smv_alliance_garrison_level_1();
    new instance_smv_alliance_garrison_level_2();
    new instance_smv_alliance_garrison_level_3();

    // Gorgrond
    new instance_gor_alliance_garrison_level_1();
    new instance_gor_alliance_garrison_level_2();
    new instance_gor_alliance_garrison_level_3();

    // Talador
    new instance_tal_alliance_garrison_level_0();
    new instance_tal_alliance_garrison_level_3();

    // Spikes of Arak
    new instance_soa_alliance_garrison_0();
    new instance_soa_alliance_garrison_3();

    // Nagrand
    new instance_nag_alliance_garrison_level_0();
    new instance_nag_alliance_garrison_level_3();
};
