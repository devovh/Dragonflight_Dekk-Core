//
#ifndef GarrisonMission_h__
#define GarrisonMission_h__

#include "GarrisonConstants.h"
#include "Packets/GarrisonPackets.h"
#include <Entities/Player/Player.h>

struct Followersss;

struct Missionssss
{
    WorldPackets::Garrison::GarrisonMission PacketInfo;
    std::list<uint64> CurrentFollowerDBIDs;
    //ObjectDBState DbState = DB_STATE_NEW;

    void Start(Player* owner, std::vector<uint64> const &followers);
    void BonusRoll(Player* onwer);
    void Complete(Player* owner);
    bool HasBonusRoll();
    float ComputeSuccessChance();
    float CalcChance(float a, float b, float c);
   // uint32 GetDuration(Player* owner);
    std::vector<Followersss*> GetMissionFollowers(uint32 garrMissionId);
};

#endif // GarrisonMission_h__
