#include "DB2Structure.h"
#include "WowTime.h"
#include "GameTime.h"
#include "WorldStateMgr.h"

static uint8 const g_WorldState_UnpackUnkTable[] =
{
    0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00,
    0x20, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x20, 0x00, 0x20, 0x00,
    0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00,
    0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00,
    0x48, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
    0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
    0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00,
    0x84, 0x00, 0x84, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
    0x10, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
    0x10, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x02, 0x00,
    0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00,
    0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00,
    0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

namespace WorldStateExpressionMathOpcode
{
    enum
    {
        Add = 1,
        Substract = 2,
        Multiply = 3,
        Divide = 4,
        Modulo = 5
    };
}

namespace WorldStateExpressionCompareOpcode
{
    enum
    {
        Equal = 1,
        Different = 2,
        Less = 3,
        LessOrEqual = 4,
        More = 5,
        MoreOrEqual = 6
    };
}

namespace WorldStateExpressionCustomOpType
{
    enum
    {
        PushUInt32 = 1,
        PushWorldStateValue = 2,
        CallFunction = 3
    };
}

namespace WorldStateExpressionEvalResultLogic
{
    enum
    {
        FirstAndSecond = 1,
        FirstOrSecond = 2,
        NotFirstOrNotSecond = 3
    };
}

namespace sWorldStateExpressionFunctions
{
    enum
    {
        None,
        Random,
        Month,
        Day,
        TimeOfDay,
        Region,
        ClockHour,
        DifficultyID,
        HolidayStart,
        HolidayLeft,
        HolidayActive,
        TimerCurrentTime,
        WeekNumber,
        None2,
        None3,
        None4,
        None5,
        None6,
        None7,
        None8,
        None9,
        None10
    };

    const char* Names[] =
    {
        "None",
        "Random",
        "Month",
        "Day",
        "TimeOfDay",
        "Region",
        "ClockHour",
        "DifficultyID",
        "HolidayStart",
        "HolidayLeft",
        "HolidayActive",
        "TimerCurrentTime",
        "WeekNumber",
        "None2",
        "None3",
        "None4",
        "None5",
        "None6",
        "None7",
        "None8",
        "None9",
        "None10"
    };
}

static std::function<int32(Player const*, int32, int32)> WorldStateExpressionFunction[] =
{
    /// WorldStateExpressionFunctions::None
    [](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
    {
        return 0;
    },
    /// WorldStateExpressionFunctions::Random
    [](Player const* /*player*/, int32 min, int32 max) -> int32
{
    return urand(min, max);
},
/// WorldStateExpressionFunctions::Month
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    //WowTime time = MS::Utilities::WowTime::Encode(GameTime::GetGameTime())

    MS::Utilities::WowTime time;
    time.SetUTCTimeFromPosixTime(GameTime::GetGameTime());

    return time.Month + 1;
},
/// WorldStateExpressionFunctions::Day
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    MS::Utilities::WowTime time;
    time.SetUTCTimeFromPosixTime(GameTime::GetGameTime());

    return time.MonthDay + 1;
},
/// WorldStateExpressionFunctions::TimeOfDay
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    MS::Utilities::WowTime time;
    time.SetUTCTimeFromPosixTime(GameTime::GetGameTime());

    return time.GetHourAndMinutes();
},
/// WorldStateExpressionFunctions::Region
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0/*sWorld->GetServerRegionID()*/;
},
/// WorldStateExpressionFunctions::ClockHour
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    MS::Utilities::WowTime time;
    time.SetUTCTimeFromPosixTime(GameTime::GetGameTime());

    if (time.Hour <= 12)
        return time.Hour;

    if (time.Hour - 12)
        return time.Hour - 12;

    return 12;
},
/// WorldStateExpressionFunctions::Region
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0/*sWorld->GetServerRegionID()*/;
},
/// WorldStateExpressionFunctions::DifficultyID
[](Player const* player, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    if (!player)
        return 0;

    return player->GetMap()->GetDifficultyID();
},
/// WorldStateExpressionFunctions::HolidayStart
[](Player const* /*player*/, int32 holidayID, int32 arg2) -> int32
{
    HolidaysEntry const* entry = sHolidaysStore.LookupEntry(holidayID);
    if (!entry || (arg2 > 0 && !entry->Duration[arg2]))
        return 0;

    int l_ChoosedDuration = entry->Duration[arg2];
    if (!l_ChoosedDuration)
        l_ChoosedDuration = 24;

    time_t l_CurrentTime = GameTime::GetGameTime();
    struct tm l_LocalTime;
    l_LocalTime.tm_isdst = -1;

    localtime_r(&l_CurrentTime, &l_LocalTime);

    MS::Utilities::WowTime gameTime;
    gameTime.SetUTCTimeFromPosixTime(l_CurrentTime);
    gameTime.YearDay = l_LocalTime.tm_yday;

    for (auto data : entry->Date)
    {
        MS::Utilities::WowTime time;
        time.Decode(data);

        if (entry->Flags & 1)
        {
            time.YearDay = gameTime.YearDay;
            time.ComputeRegionTime(time);
        }

        if (time.Minute < 0)
            time.Minute = gameTime.Minute;
        if (time.Hour < 0)
            time.Hour = gameTime.Hour;
        if (time.MonthDay < 0)
            time.MonthDay = gameTime.Minute;
        if (time.Month < 0)
            time.Month = gameTime.Month;
        if (time.Year < 0)
        {
            time.Year = gameTime.Year;

            if (gameTime < time)
                --time.Year;
        }

        if (entry->Looping)
        {
            int32 l_i3 = 0;
            int32 v7 = 0;
            int32 v8 = 0;
            do
            {
                if (l_i3 >= arg2)
                {
                    if (l_i3 > arg2)
                        v8 += entry->Duration[l_i3];
                }
                else
                {
                    v7 += entry->Duration[l_i3];
                    if (!l_i3 && !entry->Duration[0])
                        v7 += 24;
                }
                ++l_i3;
            } while (l_i3 < MAX_HOLIDAY_DURATIONS);

            int v10 = MS::Utilities::Globals::InMinutes::Hour * v7;
            int v11 = MS::Utilities::Globals::InMinutes::Hour * v8;
            while (!(gameTime <= time))
            {
                time.AddHolidayDuration(v10);
                time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);
                if (!(gameTime >= time))
                {
                    int result = MS::Utilities::Globals::InMinutes::Day * (time.GetDaysSinceEpoch() - gameTime.GetDaysSinceEpoch()) - gameTime.GetHourAndMinutes();
                    return result + time.GetHourAndMinutes();
                }
                time.AddHolidayDuration(v11);
            }
            return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
        }

        if (gameTime <= time)
        {
            for (auto l_Y = 0; l_Y < arg2 && l_Y < MAX_HOLIDAY_DURATIONS; ++l_Y)
            {
                uint32 value = entry->Duration[l_Y];
                if (!value)
                    value = 24;

                time.AddHolidayDuration(value * MS::Utilities::Globals::InMinutes::Hour);
            }

            time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);

            if (!(gameTime >= time))
                return time.DiffTime(gameTime) / MS::Utilities::Globals::InMinutes::Hour;
        }
    }

    return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
},
/// WorldStateExpressionFunctions::HolidayLeft
[](Player const* /*player*/, int32 holidayID, int32 arg2) -> int32
{
    HolidaysEntry const* entry = sHolidaysStore.LookupEntry(holidayID);

    if (!entry || (arg2 <= 0 && entry->Duration[arg2]))
        return 0;

    int l_ChoosedDuration = entry->Duration[arg2];
    if (!l_ChoosedDuration)
        l_ChoosedDuration = 24;

    time_t l_CurrentTime = GameTime::GetGameTime();
    struct tm l_LocalTime;
    l_LocalTime.tm_isdst = -1;

    localtime_r(&l_CurrentTime, &l_LocalTime);

    MS::Utilities::WowTime gameTime;
    gameTime.SetUTCTimeFromPosixTime(l_CurrentTime);
    gameTime.YearDay = l_LocalTime.tm_yday;

    for (auto data : entry->Date)
    {
        MS::Utilities::WowTime time;
        time.Decode(data);

        if (entry->Flags & 1)
        {
            time.YearDay = gameTime.YearDay;
            time.ComputeRegionTime(time);
        }

        if (time.Minute < 0)
            time.Minute = gameTime.Minute;
        if (time.Hour < 0)
            time.Hour = gameTime.Hour;
        if (time.MonthDay < 0)
            time.MonthDay = gameTime.Minute;
        if (time.Month < 0)
            time.Month = gameTime.Month;
        if (time.Year < 0)
        {
            time.Year = gameTime.Year;

            if (gameTime < time)
                --time.Year;
        }

        if (entry->Looping)
        {
            int32 l_i2 = 0;
            int32 v7 = 0;
            int32 v8 = 0;
            do
            {
                if (l_i2 >= arg2)
                {
                    if (l_i2 > arg2)
                        v8 += entry->Duration[l_i2];
                }
                else
                {
                    v7 += entry->Duration[l_i2];
                    if (!l_i2 && !entry->Duration[0])
                        v7 += 24;
                }
                ++l_i2;
            } while (l_i2 < MAX_HOLIDAY_DURATIONS);

            int v10 = MS::Utilities::Globals::InMinutes::Hour * v7;
            int v11 = MS::Utilities::Globals::InMinutes::Hour * v8;
            while (!(gameTime <= time))
            {
                time.AddHolidayDuration(v10);
                time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);
                if (!(gameTime >= time))
                {
                    int result = MS::Utilities::Globals::InMinutes::Day * (time.GetDaysSinceEpoch() - gameTime.GetDaysSinceEpoch()) - gameTime.GetHourAndMinutes();
                    return result + time.GetHourAndMinutes();
                }
                time.AddHolidayDuration(v11);
            }
            return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
        }

        if (gameTime <= time)
        {
            if (arg2 > 0)
            {
                for (auto l_Y = 0; l_Y < arg2 && l_Y < MAX_HOLIDAY_DURATIONS; ++l_Y)
                {
                    uint32 value = entry->Duration[l_Y];
                    if (!value)
                        value = 24;

                    time.AddHolidayDuration(value * MS::Utilities::Globals::InMinutes::Hour);
                }
            }

            time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);

            if (!(gameTime >= time))
                return time.DiffTime(gameTime) / MS::Utilities::Globals::InMinutes::Hour;
        }
    }

    return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
},
/// WorldStateExpressionFunctions::HolidayActive
[](Player const* /*player*/, int32 /*holidayID*/, int32 /*arg2*/) -> int32
{
//    if (HolidaysEntry const* entry = sHolidaysStore.LookupEntry(holidayID))
//    {
//        auto idx = 0;
//        auto durations = entry->Duration;
//        /*while (idx <= 0 || *durations)
//        {
//            signed int currentDuration = *durations;
//            if (!currentDuration)
//                currentDuration = 24;
//
//            if (WorldStateExpressionFunction[sWorldStateExpressionFunctions::HolidayStart](player, holidayID, idx) < MS::Utilities::Globals::InMinutes::Hour * currentDuration)
//                return 1;
//
//            ++idx;
//            durations += 4;
//
//            if (idx >= MAX_HOLIDAY_DURATIONS)
//                return 0;
//        }*/
//    }

    return 0;
},
/// WorldStateExpressionFunctions::TimerCurrentTime
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return time(nullptr);
},
/// WorldStateExpressionFunctions::WeekNumber
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
//    time_t time = GameTime::GetGameTime();
    return 0/*(time - sWorld->GetServerRaidOrigin()) / WEEK*/;
},
/// WorldStateExpressionFunctions::None2
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None3
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None4
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None5
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None6
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None7
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None8
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None9
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
},
/// WorldStateExpressionFunctions::None10
[](Player const* /*player*/, int32 /*arg1*/, int32 /*arg2*/) -> int32
{
    return 0;
}
};

/// 19865 - sub_A671A3
bool UnkSubFunction(char p_Char)
{
    /// @TODO NEED MORE WORK
    ///if (dword_1368528)
    ///{
    ///    return !!sub_A67152(p_Char, 0);
    ///}

    uint16 value = *(uint16*)&g_WorldState_UnpackUnkTable[p_Char * 2];
    return !!(value & 4);
}

std::string UnpackWorldStateExpression(char const* input)
{
    char const* l_ExpressionStr = input;
    std::string l_Unpacked;

    while (*l_ExpressionStr)
    {
        char l_Out = 0;
        char l_Temp = 0;
        if (!UnkSubFunction(l_ExpressionStr[0]))
            l_Temp = l_ExpressionStr[0] - '7';
        else
            l_Temp = l_ExpressionStr[0] - '0';

        l_Out |= l_Temp;
        l_Out *= 16;

        if (!UnkSubFunction(l_ExpressionStr[1]))
            l_Temp = l_ExpressionStr[1] - '7';
        else
            l_Temp = l_ExpressionStr[1] - '0';

        l_Out |= l_Temp;
        l_Unpacked += l_Out;

        l_ExpressionStr += 2;
    }

    return l_Unpacked;
}

int32 WorldStateExpression_EvalPush(Player const* /*player*/, char const** unpackedExpression, uint32 /*ID*/, int32* _worldStateID)
{
#define UNPACK_UINT8(x) { x = *((uint8*)((char const*)*unpackedExpression)); *unpackedExpression += sizeof(uint8);}
#define UNPACK_INT32(x) { x = *((int32*)((char const*)*unpackedExpression)); *unpackedExpression += sizeof(int32);}
    uint8 opType;
    UNPACK_UINT8(opType);

    *_worldStateID = 0;
    if (opType == WorldStateExpressionCustomOpType::PushUInt32)
    {
        int32 value;
        UNPACK_INT32(value);
        return value;
    }

    if (opType == WorldStateExpressionCustomOpType::PushWorldStateValue)
    {
        int32 l_WorldStateID;
        UNPACK_INT32(l_WorldStateID);

        // TC_LOG_DEBUG(LOG_FILTER_WORLD_QUEST, "INSERT INTO `world_state_expression` (`WorldStateExpressionID`, `WorldStateID`) VALUES ({}, {});", ID, l_WorldStateID); // Need for convert WorldStateExpressionID -> WorldStateID

        *_worldStateID = l_WorldStateID;
      //  if (player)
        //    return sWorldStateMgr.GetValue(l_WorldStateID); TODO
    }
    else if (opType == WorldStateExpressionCustomOpType::CallFunction)
    {
        int32 functionID;
        UNPACK_INT32(functionID);

//        int arg1 = WorldStateExpression_EvalPush(player, unpackedExpression, ID, _worldStateID);
//        int arg2 = WorldStateExpression_EvalPush(player, unpackedExpression, ID, _worldStateID);

        if (uint64(functionID) > (sizeof(WorldStateExpressionFunction) / sizeof(WorldStateExpressionFunction[0])))
            return static_cast<uint32>(-1);

        // return WorldStateExpressionFunction[functionID](player, arg1, arg2);
    }

    return static_cast<uint32>(-1);
#undef UNPACK_INT32
#undef UNPACK_UINT8
}

int32 WorldStateExpression_EvalArithmetic(Player const* player, char const** unpackedExpression, uint32 ID, int32* _worldStateID)
{
#define UNPACK_UINT8(x) { x = *(uint8_t*)(*unpackedExpression); *unpackedExpression += sizeof(uint8_t);}
    int leftValue = WorldStateExpression_EvalPush(player, unpackedExpression, ID, _worldStateID);
    char opperand;
    UNPACK_UINT8(opperand);

    if (!opperand)
        return leftValue;

    int rValue = WorldStateExpression_EvalPush(player, unpackedExpression, ID, _worldStateID);

    switch (opperand)
    {
    case WorldStateExpressionMathOpcode::Add:
        return rValue + leftValue;
    case WorldStateExpressionMathOpcode::Substract:
        return leftValue - rValue;
    case WorldStateExpressionMathOpcode::Multiply:
        return leftValue * rValue;
    case WorldStateExpressionMathOpcode::Divide:
        if (!rValue)
            return 0;
        return leftValue / rValue;
    case WorldStateExpressionMathOpcode::Modulo:
        if (!rValue)
            return 0;
        return leftValue % rValue;
    default:
        break;
    }

    return 0;
#undef UNPACK_UINT8
}

bool WorldStateExpression_EvalCompare(Player const* player, char const** unpackedExpression, uint32 ID, int32* _worldStateID, std::set<WorldQuestState>* state)
{
#define UNPACK_UINT8(x) { x = *(uint8*)(*unpackedExpression); *unpackedExpression += sizeof(uint8);}
    int leftValue = WorldStateExpression_EvalArithmetic(player, unpackedExpression, ID, _worldStateID);
    char opperand;
    UNPACK_UINT8(opperand);


    if (!opperand)
    {
        if (*_worldStateID && state)
            state->insert(std::make_pair(*_worldStateID, leftValue > 0 ? leftValue : 1));

        // TC_LOG_DEBUG(LOG_FILTER_WORLD_QUEST, "INSERT INTO `world_state_expression` (`WorldStateExpressionID`, `WorldStateID`, `Value0`) VALUES ({}, {}, {});", ID, *_worldStateID, leftValue); // Need for convert WorldStateExpressionID -> WorldStateID
        return !!leftValue;
    }

    int32 _worldStateID2 = 0;
    auto rValue = WorldStateExpression_EvalArithmetic(player, unpackedExpression, ID, &_worldStateID2);

    // TC_LOG_DEBUG(LOG_FILTER_WORLD_QUEST, "INSERT INTO `world_state_expression` (`WorldStateExpressionID`, `WorldStateID`, `Value0`, `Value1`) VALUES ({}, {}, {}, {});", ID, *_worldStateID, rValue, opperand); // Need for convert WorldStateExpressionID -> WorldStateID

    switch (opperand)
    {
    case WorldStateExpressionCompareOpcode::Equal:
    {
        if (*_worldStateID && state)
            state->insert(std::make_pair(*_worldStateID, rValue > 0 ? rValue : 1));

        return leftValue == rValue;
    }
    case WorldStateExpressionCompareOpcode::Different:
        return leftValue != rValue;
    case WorldStateExpressionCompareOpcode::Less:
        return leftValue < rValue;
    case WorldStateExpressionCompareOpcode::LessOrEqual:
        return leftValue <= rValue;
    case WorldStateExpressionCompareOpcode::More:
        return leftValue > rValue;
    case WorldStateExpressionCompareOpcode::MoreOrEqual:
        return leftValue >= rValue;
    default:
        break;
    }

    return false;
#undef UNPACK_UINT8
}

bool WorldStateExpression_EvalResult(char logicResult, uint8_t /*evalResultRoutineID*/, bool evalResult, uint8_t /*prevResultRoutineID*/, bool prevResult)
{
    bool result = false;

    switch (logicResult)
    {
    case WorldStateExpressionEvalResultLogic::FirstAndSecond:
        result = evalResult != 0 && prevResult != 0;
        break;
    case WorldStateExpressionEvalResultLogic::FirstOrSecond:
        result = (prevResult || evalResult) != 0;
        break;
    case WorldStateExpressionEvalResultLogic::NotFirstOrNotSecond:
        if (prevResult || evalResult)
            result = prevResult == 0 || evalResult == 0;
        break;
    default:
        break;
    }

    return result;
}

bool WorldStateExpressionEntry::Eval(Player const* player, std::set<WorldQuestState>* state) const
{
#define UNPACK_UINT8(x) { x = *unpackedExpression; unpackedExpression += sizeof(uint8);}

    if (Expression && strlen(Expression))
    {
        auto unpackedExpressionString = UnpackWorldStateExpression(Expression);
        auto unpackedExpression = unpackedExpressionString.c_str();

        char value;
        UNPACK_UINT8(value);

        if (value == 1)
        {
            int32 _worldStateID = 0;
            auto result = WorldStateExpression_EvalCompare(player, &unpackedExpression, ID, &_worldStateID, state);
            char resultLogic;
            UNPACK_UINT8(resultLogic);

            if (resultLogic)
            {
                auto routine2 = WorldStateExpression_EvalCompare(player, &unpackedExpression, ID, &_worldStateID, state);
                result = WorldStateExpression_EvalResult(resultLogic, 2, routine2, 1, result);
                UNPACK_UINT8(resultLogic);

                if (resultLogic)
                {
                    auto routine3 = WorldStateExpression_EvalCompare(player, &unpackedExpression, ID, &_worldStateID, state);
                    result = WorldStateExpression_EvalResult(resultLogic, 3, routine3, 2, result);
                    UNPACK_UINT8(resultLogic);
                }
            }

            return result;
        }
    }

    return false;
#undef UNPACK_UINT8
}
