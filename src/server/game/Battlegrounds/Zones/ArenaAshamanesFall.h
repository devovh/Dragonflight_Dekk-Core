/*
/ DekkCore By Thordekk
/
*/

#ifndef __BATTLEGROUNDVsh_H
#define __BATTLEGROUNDVsh_H

#include "Arena.h"

class ArenaAshamanesFall : public Battleground
{
public:
    ArenaAshamanesFall(BattlegroundTemplate const* battlegroundTemplate);
    ~ArenaAshamanesFall();

    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    bool SetupBattleground() override;
};

#endif
