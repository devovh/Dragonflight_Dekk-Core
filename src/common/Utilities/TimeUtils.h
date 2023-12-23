/*
 * This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WARHEAD_TIME_UTILS_H_
#define WARHEAD_TIME_UTILS_H_

#include "Define.h"
#include "Duration.h"
#include <string_view>

enum class TimeFmt : uint8
{
    FullText,       // 1 Days 2 Hours 3 Minutes 4 Seconds 5 Milliseconds 6 Microseconds
    ShortText,      // 1d 2h 3m 4s 5ms 6us
    Numeric         // 1:2:3:4:5:6
};

namespace Warhead::Time
{
    TC_COMMON_API Seconds TimeStringTo(std::string_view timestring);

    template<class T>
    TC_COMMON_API std::string ToTimeString(std::string_view durationTime, uint8 outCount = 3, TimeFmt timeFormat = TimeFmt::ShortText);

    TC_COMMON_API std::string ToTimeString(Microseconds durationTime, uint8 outCount = 3, TimeFmt timeFormat = TimeFmt::ShortText);
}

#endif
