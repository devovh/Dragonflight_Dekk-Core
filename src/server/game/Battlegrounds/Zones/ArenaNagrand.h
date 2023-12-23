/*
/ DekkCore By Thordekk
/
*/

#ifndef __BATTLEGROUNDNA_H
#define __BATTLEGROUNDNA_H

#include "Arena.h"

class ArenaNagrandArena : public Battleground
{
public:
    ArenaNagrandArena(BattlegroundTemplate const* battlegroundTemplate);
    ~ArenaNagrandArena();

    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    bool SetupBattleground() override;
};

#endif
