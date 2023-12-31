/*
 * Copyright (C) 2010 - 2020 Eluna Lua Engine <http://emudevs.com/>
 * This program is free software licensed under GPL version 3
 * Please see the included DOCS/LICENSE.md for more information
 */

#include "Hooks.h"
#include "HookHelpers.h"
#include "LuaEngine.h"
#include "BindingMap.h"
#include "ElunaEventMgr.h"
#include "ElunaIncludes.h"
#include "ElunaTemplate.h"
#include <iostream>

using namespace Hooks;

#define START_HOOK(EVENT) \
    if (!IsEnabled())\
        return;\
    auto key = EventKey<ServerEvents>(EVENT);\
    if (!ServerEventBindings->HasBindingsFor(key))\
        return;\
    LOCK_ELUNA

#define START_HOOK_WITH_RETVAL(EVENT, RETVAL) \
    if (!IsEnabled())\
        return RETVAL;\
    auto key = EventKey<ServerEvents>(EVENT);\
    if (!ServerEventBindings->HasBindingsFor(key))\
        return RETVAL;\
    LOCK_ELUNA

bool Eluna::OnAddonMessage(Player* sender, uint32 type, std::string& msg, Player* receiver, Guild* guild, Group* group, Channel* channel)
{
    std::cout << "PING" << std::endl;
    START_HOOK_WITH_RETVAL(ADDON_EVENT_ON_MESSAGE, true);
    ELUNA_LOG_INFO("custom.AIO", "sender: {}\ntype: {}\nmsg: {}\nreceiver: {}", sender->GetName().c_str(), type, msg.c_str(), receiver->GetName().c_str());
    Push(sender);
    Push(type);

    auto delimeter_position = msg.find('\t');
    if (delimeter_position == std::string::npos)
    {
        Push(msg); // prefix
        Push(); // msg
    }
    else
    {
        std::string prefix = msg.substr(0, delimeter_position);
        std::string content = msg.substr(delimeter_position + 1, std::string::npos);
        Push(prefix);
        Push(content);
    }

    if (receiver)
        Push(receiver);
    else if (guild)
        Push(guild);
    else if (group)
        Push(group);
    else if (channel)
        Push(channel->GetChannelId());
    else
        Push();

    return CallAllFunctionsBool(ServerEventBindings, key, true);
}

void Eluna::OnTimedEvent(int funcRef, uint32 delay, uint32 calls, WorldObject* obj)
{
    LOCK_ELUNA;
    ASSERT(!event_level);

    // Get function
    lua_rawgeti(L, LUA_REGISTRYINDEX, funcRef);

    // Push parameters
    Push(L, funcRef);
    Push(L, delay);
    Push(L, calls);
    Push(L, obj);

    // Call function
    ExecuteCall(4, 0);

    ASSERT(!event_level);
    InvalidateObjects();
}

void Eluna::OnGameEventStart(uint32 eventid)
{
    START_HOOK(GAME_EVENT_START);
    Push(eventid);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnGameEventStop(uint32 eventid)
{
    START_HOOK(GAME_EVENT_STOP);
    Push(eventid);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnLuaStateClose()
{
    START_HOOK(ELUNA_EVENT_ON_LUA_STATE_CLOSE);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnLuaStateOpen()
{
    START_HOOK(ELUNA_EVENT_ON_LUA_STATE_OPEN);
    CallAllFunctions(ServerEventBindings, key);
}

// AreaTrigger
bool Eluna::OnAreaTrigger(Player* pPlayer, AreaTriggerEntry const* pTrigger, bool entered)
{
    START_HOOK_WITH_RETVAL(TRIGGER_EVENT_ON_TRIGGER, false);
    Push(pPlayer);
    Push(pTrigger->ID);
    Push(entered);
    return CallAllFunctionsBool(ServerEventBindings, key);
}

// Weather
void Eluna::OnChange(Weather* /*weather*/, uint32 zone, WeatherState state, float grade)
{
    START_HOOK(WEATHER_EVENT_ON_CHANGE);
    Push(zone);
    Push(state);
    Push(grade);
    CallAllFunctions(ServerEventBindings, key);
}

// Auction House
void Eluna::OnAdd(AuctionHouseObject* ah, AuctionPosting* auction)
{
    START_HOOK(AUCTION_EVENT_ON_ADD);
    Push(ah);
    Push(auction);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnRemove(AuctionHouseObject* ah, AuctionPosting* auction)
{
    START_HOOK(AUCTION_EVENT_ON_REMOVE);
    Push(ah);
    Push(auction);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnSuccessful(AuctionHouseObject* ah, AuctionPosting* auction)
{
    START_HOOK(AUCTION_EVENT_ON_SUCCESSFUL);
    Push(ah);
    Push(auction);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnExpire(AuctionHouseObject* ah, AuctionPosting* auction)
{
    START_HOOK(AUCTION_EVENT_ON_EXPIRE);
    Push(ah);
    Push(auction);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnOpenStateChange(bool open)
{
    START_HOOK(WORLD_EVENT_ON_OPEN_STATE_CHANGE);
    Push(open);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnConfigLoad(bool reload)
{
    START_HOOK(WORLD_EVENT_ON_CONFIG_LOAD);
    Push(reload);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnShutdownInitiate(ShutdownExitCode code, ShutdownMask mask)
{
    START_HOOK(WORLD_EVENT_ON_SHUTDOWN_INIT);
    Push(code);
    Push(mask);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnShutdownCancel()
{
    START_HOOK(WORLD_EVENT_ON_SHUTDOWN_CANCEL);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnWorldUpdate(uint32 diff)
{
    {
        LOCK_ELUNA;
        if (ShouldReload())
            _ReloadEluna();
    }

    eventMgr->globalProcessor->Update(diff);

    START_HOOK(WORLD_EVENT_ON_UPDATE);
    Push(diff);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnStartup()
{
    START_HOOK(WORLD_EVENT_ON_STARTUP);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnShutdown()
{
    START_HOOK(WORLD_EVENT_ON_SHUTDOWN);
    CallAllFunctions(ServerEventBindings, key);
}

/* Map */
void Eluna::OnCreate(Map* map)
{
    START_HOOK(MAP_EVENT_ON_CREATE);
    Push(map);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnDestroy(Map* map)
{
    START_HOOK(MAP_EVENT_ON_DESTROY);
    Push(map);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnPlayerEnter(Map* map, Player* player)
{
    START_HOOK(MAP_EVENT_ON_PLAYER_ENTER);
    Push(map);
    Push(player);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnPlayerLeave(Map* map, Player* player)
{
    START_HOOK(MAP_EVENT_ON_PLAYER_LEAVE);
    Push(map);
    Push(player);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnUpdate(Map* map, uint32 diff)
{
    START_HOOK(MAP_EVENT_ON_UPDATE);
    // enable this for multithread
    // eventMgr->globalProcessor->Update(diff);
    Push(map);
    Push(diff);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnRemove(GameObject* gameobject)
{
    START_HOOK(WORLD_EVENT_ON_DELETE_GAMEOBJECT);
    Push(gameobject);
    CallAllFunctions(ServerEventBindings, key);
}

void Eluna::OnRemove(Creature* creature)
{
    START_HOOK(WORLD_EVENT_ON_DELETE_CREATURE);
    Push(creature);
    CallAllFunctions(ServerEventBindings, key);
}
