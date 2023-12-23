///*
// * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
// *
// * This program is free software; you can redistribute it and/or modify it
// * under the terms of the GNU General Public License as published by the
// * Free Software Foundation; either version 2 of the License, or (at your
// * option) any later version.
// *
// * This program is distributed in the hope that it will be useful, but WITHOUT
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// * more details.
// *
// * You should have received a copy of the GNU General Public License along
// * with this program. If not, see <http://www.gnu.org/licenses/>.
// */
//
//#include "BattlePetMgr.h"
//#include "DB2Stores.h"
//#include "Containers.h"
//#include "Creature.h"
//#include "DatabaseEnv.h"
//#include "GameTables.h"
//#include "GameTime.h"
//#include "Item.h"
//#include "Log.h"
//#include "ObjectAccessor.h"
//#include "ObjectMgr.h"
//#include "Player.h"
//#include "Realm.h"
//#include "Util.h"
//#include "World.h"
//#include "WorldSession.h"
//
//namespace BattlePets
//{
//    namespace
//    {
//        std::unordered_map<uint16 /*BreedID*/, std::unordered_map<BattlePetState /*state*/, int32 /*value*/, std::hash<std::underlying_type<BattlePetState>::type> >> _battlePetBreedStates;
//        std::unordered_map<uint32 /*SpeciesID*/, std::unordered_map<BattlePetState /*state*/, int32 /*value*/, std::hash<std::underlying_type<BattlePetState>::type> >> _battlePetSpeciesStates;
//        std::unordered_map<uint32 /*CreatureID*/, BattlePetSpeciesEntry const*> _battlePetSpeciesByCreature;
//        std::unordered_map<uint32 /*SpellID*/, BattlePetSpeciesEntry const*> _battlePetSpeciesBySpell;
//        std::unordered_map<uint32 /*SpeciesID*/, std::unordered_set<uint8 /*breed*/>> _availableBreedsPerSpecies;
//        std::unordered_map<uint32 /*SpeciesID*/, uint8 /*quality*/> _defaultQualityPerSpecies;
//    }
//
//    void BattlePet::CalculateStats()
//    {
//        float health = 0.0f;
//        float power = 0.0f;
//        float speed = 0.0f;
//
//        // get base breed stats
//        auto breedState = _battlePetBreedStates.find(PacketInfo.Breed);
//        if (breedState == _battlePetBreedStates.end()) // non existing breed id
//            return;
//
//        health = breedState->second[STATE_STAT_STAMINA];
//        power = breedState->second[STATE_STAT_POWER];
//        speed = breedState->second[STATE_STAT_SPEED];
//
//        // modify stats depending on species - not all pets have this
//        auto speciesState = _battlePetSpeciesStates.find(PacketInfo.Species);
//        if (speciesState != _battlePetSpeciesStates.end())
//        {
//            health += speciesState->second[STATE_STAT_STAMINA];
//            power += speciesState->second[STATE_STAT_POWER];
//            speed += speciesState->second[STATE_STAT_SPEED];
//        }
//
//        // modify stats by quality
//        for (BattlePetBreedQualityEntry const* battlePetBreedQuality : sBattlePetBreedQualityStore)
//        {
//            if (battlePetBreedQuality->QualityEnum == PacketInfo.Quality)
//            {
//                health *= battlePetBreedQuality->StateMultiplier;
//                power *= battlePetBreedQuality->StateMultiplier;
//                speed *= battlePetBreedQuality->StateMultiplier;
//                break;
//            }
//            // TOOD: add check if pet has existing quality
//        }
//
//        // scale stats depending on level
//        health *= PacketInfo.Level;
//        power *= PacketInfo.Level;
//        speed *= PacketInfo.Level;
//
//        // set stats
//        // round, ceil or floor? verify this
//        PacketInfo.MaxHealth = uint32((round(health / 20) + 100));
//        PacketInfo.Power = uint32(round(power / 100));
//        PacketInfo.Speed = uint32(round(speed / 100));
//    }
//
//    void BattlePetMgr::AddBattlePetSpeciesBySpell(uint32 spellId, BattlePetSpeciesEntry const* speciesEntry)
//    {
//        _battlePetSpeciesBySpell[spellId] = speciesEntry;
//    }
//
//    BattlePetSpeciesEntry const* BattlePetMgr::GetBattlePetSpeciesByCreature(uint32 creatureId)
//    {
//        return Trinity::Containers::MapGetValuePtr(_battlePetSpeciesByCreature, creatureId);
//    }
//
//    BattlePetSpeciesEntry const* BattlePetMgr::GetBattlePetSpeciesBySpell(uint32 spellId)
//    {
//        return Trinity::Containers::MapGetValuePtr(_battlePetSpeciesBySpell, spellId);
//    }
//
//    uint16 BattlePetMgr::RollPetBreed(uint32 species)
//    {
//        auto itr = _availableBreedsPerSpecies.find(species);
//        if (itr == _availableBreedsPerSpecies.end())
//            return 3; // default B/B
//
//        return Trinity::Containers::SelectRandomContainerElement(itr->second);
//    }
//
//    BattlePetBreedQuality BattlePetMgr::GetDefaultPetQuality(uint32 species)
//    {
//        auto itr = _defaultQualityPerSpecies.find(species);
//        if (itr == _defaultQualityPerSpecies.end())
//            return BattlePetBreedQuality::Poor; // Default
//
//        return BattlePetBreedQuality(itr->second);
//    }
//
//    uint32 BattlePetMgr::SelectPetDisplay(BattlePetSpeciesEntry const* speciesEntry)
//    {
//        if (CreatureTemplate const* creatureTemplate = sObjectMgr->GetCreatureTemplate(speciesEntry->CreatureID))
//            if (!speciesEntry->GetFlags().HasFlag(BattlePetSpeciesFlags::RandomDisplay))
//                if (CreatureModel const* creatureModel = creatureTemplate->GetRandomValidModel())
//                    return creatureModel->CreatureDisplayID;
//
//        return 0;
//    }
//
//    uint8 BattlePetMgr::GetPetCount(BattlePetSpeciesEntry const* battlePetSpecies, ObjectGuid ownerGuid) const
//    {
//        return uint8(std::count_if(_pets.begin(), _pets.end(), [battlePetSpecies, ownerGuid](std::pair<uint64 const, BattlePet> const& pet)
//            {
//                if (pet.second.PacketInfo.Species != battlePetSpecies->ID)
//                return false;
//
//        if (pet.second.SaveInfo == BATTLE_PET_REMOVED)
//            return false;
//
//        if (battlePetSpecies->GetFlags().HasFlag(BattlePetSpeciesFlags::NotAccountWide))
//            if (!ownerGuid.IsEmpty() && pet.second.PacketInfo.OwnerInfo)
//                if (pet.second.PacketInfo.OwnerInfo->Guid != ownerGuid)
//                    return false;
//
//        return true;
//            }));
//    }
//
//    bool BattlePetMgr::HasMaxPetCount(BattlePetSpeciesEntry const* battlePetSpecies, ObjectGuid ownerGuid) const
//    {
//        uint8 maxPetsPerSpecies = battlePetSpecies->GetFlags().HasFlag(BattlePetSpeciesFlags::LegacyAccountUnique) ? 1 : DEFAULT_MAX_BATTLE_PETS_PER_SPECIES;
//
//        return GetPetCount(battlePetSpecies, ownerGuid) >= maxPetsPerSpecies;
//    }
//
//    uint16 BattlePetMgr::GetMaxPetLevel() const
//    {
//        uint16 level = 0;
//        for (auto& pet : _pets)
//            if (pet.second.SaveInfo != BATTLE_PET_REMOVED)
//                level = std::max(level, pet.second.PacketInfo.Level);
//
//        return level;
//    }
//
//   
//}
