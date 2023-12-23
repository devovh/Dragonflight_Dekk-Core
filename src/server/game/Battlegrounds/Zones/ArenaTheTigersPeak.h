/*
/ DekkCore By Thordekk
/
*/

#ifndef __BATTLEGROUNDTTP_H
#define __BATTLEGROUNDTTP_H

#include "Arena.h"


class ArenaTheTigersPeak : public Battleground
{
public:
    ArenaTheTigersPeak(BattlegroundTemplate const* battlegroundTemplate);
    ~ArenaTheTigersPeak();

    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    bool SetupBattleground() override;
};

#endif
