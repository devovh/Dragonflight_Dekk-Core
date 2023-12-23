/*
 * Copyright 2023 Fluxurion
 */

#include "Config.h"
#include "MotionMaster.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Log.h"

class FluxTaxiAuraCheck : public PlayerScript
{
public:
    FluxTaxiAuraCheck() : PlayerScript("FluxTaxiAuraCheck") { }

    void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 /*newArea*/)
    {
        // remove Cosmic Taxi auras if player reached the ground
        if ((player->HasAura(323977) || player->HasAura(332323)) && player->GetMotionMaster()->GetCurrentMovementGeneratorType() != FLIGHT_MOTION_TYPE)
        {
            if (player->HasAura(323977))
                player->RemoveAura(323977);

            if (player->HasAura(332323))
                player->RemoveAura(332323);
        }

        if (player->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE && sConfigMgr->GetBoolDefault("Flux.Taxi.Enable", false))
        {
            if (!player->HasAura(323977))
                player->CastSpell(player, 323977, true);

            if (!player->HasAura(332323))
                player->CastSpell(player, 332323, true);
        }
    }
};


void AddSC_Fluxurion_PlayerScripts()
{
    new FluxTaxiAuraCheck();
}
