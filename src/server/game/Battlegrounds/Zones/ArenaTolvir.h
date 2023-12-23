/*
/ DekkCore By Thordekk
/
*/

#ifndef __BATTLEGROUNDTV_H
#define __BATTLEGROUNDTV_H

#include "Arena.h"

class ArenaTolvir : public Battleground
{
public:
    ArenaTolvir(BattlegroundTemplate const* battlegroundTemplate);
    ~ArenaTolvir();

    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    bool SetupBattleground() override;
};
#endif
