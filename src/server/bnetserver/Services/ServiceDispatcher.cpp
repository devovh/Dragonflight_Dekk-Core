/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 */

#include "ServiceDispatcher.h"

Battlenet::ServiceDispatcher::ServiceDispatcher()
{
    AddService<Services::Account>();
    AddService<Services::Authentication>();
    AddService<Service<club::v1::membership::ClubMembershipService>>();
    AddService<Service<club::v1::ClubService>>();
    AddService<Services::Connection>();
    AddService<Service<friends::v1::FriendsService>>();
    AddService<Services::GameUtilities>();
    AddService<Service<presence::v1::PresenceService>>();
    AddService<Service<report::v1::ReportService>>();
    AddService<Service<report::v2::ReportService>>();
    AddService<Service<resources::v1::ResourcesService>>();
    AddService<Service<user_manager::v1::UserManagerService>>();
}

void Battlenet::ServiceDispatcher::Dispatch(Session* session, uint32 serviceHash, uint32 token, uint32 methodId, MessageBuffer buffer)
{
    auto itr = _dispatchers.find(serviceHash);
    if (itr != _dispatchers.end())
        itr->second(session, token, methodId, std::move(buffer));
    else
        TC_LOG_DEBUG("session.rpc", "{} tried to call invalid service 0x{:X}", session->GetClientInfo(), serviceHash);
}

Battlenet::ServiceDispatcher& Battlenet::ServiceDispatcher::Instance()
{
    static ServiceDispatcher instance;
    return instance;
}
