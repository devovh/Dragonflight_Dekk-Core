/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Banner.h"
#include "GitRevision.h"
#include "StringFormat.h"

void Trinity::Banner::Show(char const* applicationName, void(*log)(char const* text), void(*logExtraInfo)())
{
    log(Trinity::StringFormat("{} ({})", GitRevision::GetFullVersion(), applicationName).c_str());
    log(R"(<Ctrl-C> to stop.)");
    log(R"(########..########.##....##.##....##..######...#######..########..########.)");
    log(R"(##.....##.##.......##...##..##...##..##....##.##.....##.##.....##.##.......)");
    log(R"(##.....##.##.......##..##...##..##...##.......##.....##.##.....##.##.......)");
    log(R"(##.....##.######...#####....#####....##.......##.....##.########..######...)");
    log(R"(##.....##.##.......##..##...##..##...##.......##.....##.##...##...##.......)");
    log(R"(##.....##.##.......##...##..##...##..##....##.##.....##.##....##..##.......)");
    log(R"(########..########.##....##.##....##..######...#######..##.....##.########.)");
    log(R"(+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+ +-+-+-+-+-+-+)");
    log(R"(|D|e|k|k|C|o|r|e| |D|o|n|a|t|o|r| |R|e|p|a|c|k|)");
    log(R"(+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+ +-+-+-+-+-+-+)");
    log(R"(http://dekkcoreteam.com/                  \\__/)");

    if (logExtraInfo)
        logExtraInfo();
}
