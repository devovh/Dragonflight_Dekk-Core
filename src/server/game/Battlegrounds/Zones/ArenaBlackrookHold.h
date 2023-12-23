/*
/ DekkCore By Thordekk
/
*/

#ifndef __BATTLEGROUNDBrh_H
#define __BATTLEGROUNDBrh_H

#include "Arena.h"

class ArenaBlackrookHold : public Battleground
{
public:
    ArenaBlackrookHold(BattlegroundTemplate const* battlegroundTemplate);
    ~ArenaBlackrookHold();

    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    bool SetupBattleground() override;
};

#endif
