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

#ifndef ScenePackets_h__
#define ScenePackets_h__

#include "Packet.h"
#include "Object.h"

namespace WorldPackets
{
    namespace Scenes
    {
        class TC_GAME_API PlayScene final : public ServerPacket
        {
        public:
            PlayScene() : ServerPacket(SMSG_PLAY_SCENE, 34) { }

            WorldPacket const* Write() override;

            int32 SceneID = 0;
            uint32 PlaybackFlags = 0;
            uint32 SceneInstanceID = 0;
            int32 SceneScriptPackageID = 0;
            ObjectGuid TransportGUID;
            Position Location;
            bool Encrypted = false;
        };

        class TC_GAME_API CancelScene final : public ServerPacket
        {
        public:
            CancelScene() : ServerPacket(SMSG_CANCEL_SCENE, 4) { }

            WorldPacket const* Write() override;

            int32 SceneInstanceID = 0;
        };

        class SceneTriggerEvent final : public ClientPacket
        {
        public:
            SceneTriggerEvent(WorldPacket&& packet) : ClientPacket(CMSG_SCENE_TRIGGER_EVENT, std::move(packet)) { }

            void Read() override;

            uint32 SceneInstanceID = 0;
            std::string Event;
        };

        class ScenePlaybackComplete final : public ClientPacket
        {
        public:
            ScenePlaybackComplete(WorldPacket&& packet) : ClientPacket(CMSG_SCENE_PLAYBACK_COMPLETE, std::move(packet)) { }

            void Read() override;

            uint32 SceneInstanceID = 0;
        };

        class ScenePlaybackCanceled final : public ClientPacket
        {
        public:
            ScenePlaybackCanceled(WorldPacket&& packet) : ClientPacket(CMSG_SCENE_PLAYBACK_CANCELED, std::move(packet)) { }

            void Read() override;

            uint32 SceneInstanceID = 0;
        };

        //DekkCore
        //< SMSG_SCENE_OBJECT_PET_BATTLE_FIRST_ROUND
        //< SMSG_SCENE_OBJECT_PET_BATTLE_ROUND_RESULT
        //< SMSG_SCENE_OBJECT_PET_BATTLE_REPLACEMENTS_MADE
        class PetBattleRound final : public ServerPacket
        {
        public:
            PetBattleRound(OpcodeServer opcode) : ServerPacket(opcode) { }

            WorldPacket const* Write() override;

            ObjectGuid SceneObjectGUID;
          //  BattlePet::RoundResult MsgData; 
        };

        class SceneObjectFinalRound final : public ServerPacket
        {
        public:
            SceneObjectFinalRound() : ServerPacket(SMSG_SCENE_OBJECT_PET_BATTLE_FINAL_ROUND) { }

            WorldPacket const* Write() override;

            ObjectGuid SceneObjectGUID;
          //  BattlePet::FinalRound MsgData; 
        };

        class PetBattleFinished final : public ServerPacket
        {
        public:
            PetBattleFinished() : ServerPacket(SMSG_SCENE_OBJECT_PET_BATTLE_FINISHED, 16) { }

            WorldPacket const* Write() override;

            ObjectGuid SceneObjectGUID;
        };

        class SceneObjectPetBattleInitialUpdate final : public ServerPacket
        {
        public:
            SceneObjectPetBattleInitialUpdate() : ServerPacket(SMSG_SCENE_OBJECT_PET_BATTLE_INITIAL_UPDATE) { }

            WorldPacket const* Write() override;

            ObjectGuid SceneObjectGUID;
            //BattlePet::PetBattleFullUpdate MsgData; 
        };
    }
}

#endif // ScenePackets_h__
