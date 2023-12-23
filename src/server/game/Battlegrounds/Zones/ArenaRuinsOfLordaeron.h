/*
/ DekkCore By Thordekk
/
*/

#ifndef __BATTLEGROUNDRl_H
#define __BATTLEGROUNDRl_H

#include "Arena.h"

class ArenaRuinsOfLordaeron : public Battleground
{
public:
    ArenaRuinsOfLordaeron(BattlegroundTemplate const* battlegroundTemplate);
    ~ArenaRuinsOfLordaeron();

    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    bool SetupBattleground() override;
};

#endif
