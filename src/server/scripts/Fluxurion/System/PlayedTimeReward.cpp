/*
 * Copyright 2023 Fluxurion
 */

/*
*
* Played Time Reward custom script.
*
* Copyright Fluxurion - DekkCore - 2022
*
###################################################################################################
# Played Time Reward Custom Script #
####################################
#
# - Rewards the player with the selected option and repeats the process within the interval.
# - Interval is in minute. Gold is in gold, not copper. BattleCoin is the ingame shop currency.
# - Example currencyIDs: 1191 Valor, 1813 Reservoir Anima, 2009 Cosmic Flux, 1166 Timewarped Badge
#

PlayedTimeReward.Interval         = 30
PlayedTimeReward.Gold             = 50
PlayedTimeReward.Currency_ID      = 1166
PlayedTimeReward.Currency_Count   = 50
PlayedTimeReward.BattleCoins      = 5

#
###################################################################################################
*/

#include "Player.h"
#include "Chat.h"
#include "Config.h"
#include "ScriptMgr.h"

class playedtimereward : public PlayerScript
{
public:
    playedtimereward() : PlayerScript("playedtimereward") { }

    void OnUpdate(Player* player, uint32 diff)
    {
        if (ptr_Interval > 0)
        {
            if (ptr_Interval <= diff)
            {
                std::string PTRewardText = "|cff00ffff[PlayedTimeReward]: You are rewarded for staying online.";
                // Money
                if (ptr_Money > 0)
                {
                    ptr_Money = ptr_Money * 10000;
                    player->ModifyMoney(ptr_Money);
                }

                // Currency
                if (ptr_Currency_Count > 0)
                    player->AddCurrency(ptr_Currency_ID, ptr_Currency_Count, CurrencyGainSource::GarrisonBuildingRefund);

                // Battle Coins
                if (ptr_BattleCoins > 0)
                {
                    player->GetSession()->GetBattlePayMgr()->ModifyBattlePayCredits((player->GetSession()->GetBattlePayMgr()->GetBattlePayCredits() / 10000) + ptr_BattleCoins);
                }

                std::ostringstream message;

                ptr_Money = ptr_Money / 10000;

                if (ptr_Money > 0 and ptr_Currency_Count > 0 and ptr_BattleCoins > 0)
                    message
                    << "|cff00ffff |TInterface\\icons\\spell_holy_borrowedtime:15 |t [PlayedTimeReward]: You earned "
                    << " |TInterface\\icons\\wow_store:15|t " << ptr_BattleCoins << " ingame shop credit, "
                    << " |TInterface\\icons\\pvecurrency-justice:15|t " << ptr_Currency_Count << " currency and "
                    << " |TInterface\\icons\\inv_misc_coin_01:15|t " << ptr_Money << " gold reward for staying online. |r";

                if (ptr_Money > 0 and ptr_Currency_Count > 0 and ptr_BattleCoins == 0)
                    message
                    << "|cff00ffff |TInterface\\icons\\spell_holy_borrowedtime:15 |t [PlayedTimeReward]: You earned "
                    << " |TInterface\\icons\\pvecurrency-justice:15|t " << ptr_Currency_Count << " currency and "
                    << " |TInterface\\icons\\inv_misc_coin_01:15|t " << ptr_Money << " gold reward for staying online. |r";

                if (ptr_Money > 0 and ptr_Currency_Count == 0 and ptr_BattleCoins > 0)
                    message
                    << "|cff00ffff |TInterface\\icons\\spell_holy_borrowedtime:15 |t [PlayedTimeReward]: You earned "
                    << " |TInterface\\icons\\wow_store:15|t " << ptr_BattleCoins << " ingame shop credit and "
                    << " |TInterface\\icons\\inv_misc_coin_01:15|t " << ptr_Money << " gold reward for staying online. |r";

                if (ptr_Money == 0 and ptr_Currency_Count > 0 and ptr_BattleCoins > 0)
                    message
                    << "|cff00ffff |TInterface\\icons\\spell_holy_borrowedtime:15 |t [PlayedTimeReward]: You earned "
                    << " |TInterface\\icons\\wow_store:15|t " << ptr_BattleCoins << " ingame shop credit, "
                    << " |TInterface\\icons\\pvecurrency-justice:15|t " << ptr_Currency_Count << " currency reward for staying online. |r";

                if (ptr_Money == 0 and ptr_Currency_Count == 0 and ptr_BattleCoins > 0)
                    message
                    << "|cff00ffff |TInterface\\icons\\spell_holy_borrowedtime:15 |t [PlayedTimeReward]: You earned "
                    << " |TInterface\\icons\\wow_store:15|t " << ptr_BattleCoins << " ingame shop credit reward for staying online. |r";

                if (ptr_Money == 0 and ptr_Currency_Count > 0 and ptr_BattleCoins == 0)
                    message
                    << "|cff00ffff |TInterface\\icons\\spell_holy_borrowedtime:15 |t [PlayedTimeReward]: You earned "
                    << " |TInterface\\icons\\pvecurrency-justice:15|t " << ptr_Currency_Count << " currency reward for staying online. |r";

                if (ptr_Money > 0 and ptr_Currency_Count == 0 and ptr_BattleCoins == 0)
                    message
                    << "|cff00ffff |TInterface\\icons\\spell_holy_borrowedtime:15 |t [PlayedTimeReward]: You earned "
                    << " |TInterface\\icons\\inv_misc_coin_01:15|t " << ptr_Money << " gold reward for staying online. |r";

                ChatHandler(player->GetSession()).SendSysMessage(message.str().c_str());

                ptr_Interval = sConfigMgr->GetIntDefault("PlayedTimeReward.Interval", 0) * 60000;
            }
            else
                ptr_Interval -= diff;
        }
    }

private:
    uint32 ptr_Interval = sConfigMgr->GetIntDefault("PlayedTimeReward.Interval", 0) * 60000;
    uint32 ptr_Money = sConfigMgr->GetIntDefault("PlayedTimeReward.Gold", 0);
    uint32 ptr_Currency_ID = sConfigMgr->GetIntDefault("PlayedTimeReward.Currency_ID", 0);
    uint32 ptr_Currency_Count = sConfigMgr->GetIntDefault("PlayedTimeReward.Currency_Count", 0);
    uint32 ptr_BattleCoins = sConfigMgr->GetIntDefault("PlayedTimeReward.BattleCoins", 0);

};

void AddSC_PlayedTimeReward()
{
    new playedtimereward();
}
