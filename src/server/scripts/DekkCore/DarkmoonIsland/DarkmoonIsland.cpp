/*
 * Copyright (C) 2020 LatinCoreTeam
 * Copyright (C) 2022 DekkCore
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

#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "MotionMaster.h"
#include "GameObject.h"
#include "Transport.h"
#include "TemporarySummon.h"
#include "DarkmoonIsland.h"

class playerscript_darkmoon_carousel : public PlayerScript
{
    public:
        playerscript_darkmoon_carousel() : PlayerScript("playerscript_darkmoon_carousel") {}

        /*virtual void OnEnterTransport(Player* p_Player, GameObject* transport) override
        {
            if (transport->GetEntry() == GOB_DARKMOON_CAROUSEL)
                if (!p_Player->HasAura(SPELL_WHEE))
                    p_Player->CastSpell(p_Player, SPELL_WHEE, false);
        }*/
};

// Whee! - 46668
class spell_darkmoon_carousel_whee : public SpellScriptLoader
{
public:
    spell_darkmoon_carousel_whee() : SpellScriptLoader("spell_darkmoon_carousel_whee") { }

    class spell_darkmoon_carousel_whee_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_darkmoon_carousel_whee_AuraScript);

        uint32 update;

        bool Validate(SpellInfo const* /*spell*/) override
        {
            update = 0;
            return true;
        }

        void OnUpdate(uint32 diff)
        {
            update += diff;

            if (update >= 5000)
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (Transport* transport = dynamic_cast<Transport*>(_player->GetTransport()))
                    {
                        if (transport->GetEntry() == GOB_DARKMOON_CAROUSEL)
                        {
                            if (Aura* aura = GetAura())
                            {
                                uint32 currentMaxDuration = aura->GetMaxDuration();
                                uint32 newMaxDurantion = currentMaxDuration + (5 * MINUTE * IN_MILLISECONDS);
                                newMaxDurantion = newMaxDurantion <= (60 * MINUTE * IN_MILLISECONDS) ? newMaxDurantion : (60 * MINUTE * IN_MILLISECONDS);

                                aura->SetMaxDuration(newMaxDurantion);
                                aura->SetDuration(newMaxDurantion);
                            }
                        }
                    }
                }

                update = 0;
            }
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_darkmoon_carousel_whee_AuraScript::OnUpdate);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_darkmoon_carousel_whee_AuraScript();
    }
};

// To the Staging Area! - 101260
class spell_darkmoon_staging_area_teleport : public SpellScriptLoader
{
public:
    spell_darkmoon_staging_area_teleport() : SpellScriptLoader("spell_darkmoon_staging_area_teleport") { }

    class spell_darkmoon_staging_area_teleport_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_darkmoon_staging_area_teleport_SpellScript);

        bool Load() override
        {
            return GetCaster() != nullptr;
        }

        void RelocateDest(SpellEffIndex /*effIndex*/)
        {
            if (Player* caster = GetCaster()->ToPlayer())
            {
                switch (caster->GetMapId())
                {
                    case 974: //Darkmoon Island
                        if (caster->GetTeamId() == TEAM_HORDE)
                            GetHitDest()->WorldRelocate(WorldLocation(1, -1454.415894f, 207.967484f, -7.790083f, 0.689538f));
                        else
                            GetHitDest()->WorldRelocate(WorldLocation(0, -9517.5f, 82.3f, 59.51f, 2.92168f));
                        break;
                    default:
                        GetHitDest()->WorldRelocate(WorldLocation(974, -3618.669922f, 6315.669922f, 113.190002f, 3.204420f));
                        break;
                }
            }
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(spell_darkmoon_staging_area_teleport_SpellScript::RelocateDest, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_darkmoon_staging_area_teleport_SpellScript();
    }
};

class item_darkmoon_faire_fireworks : public ItemScript
{
public:
    item_darkmoon_faire_fireworks() : ItemScript("item_darkmoon_faire_fireworks") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/, ObjectGuid /*castId*/) override
    {
        AchievementEntry const* AchievFireworksAlliance = sAchievementStore.LookupEntry(6030);
        AchievementEntry const* AchievFireworksHorde = sAchievementStore.LookupEntry(6031);

        if (player->GetTeam() == ALLIANCE)
        {
            player->CompletedAchievement(AchievFireworksAlliance);
            player->CastSpell(player, 103740, false);
        }
        else
        {
            player->CompletedAchievement(AchievFireworksHorde);
            player->CastSpell(player, 103740, false);
        }

        return true;
    }
};

// 54485 - Jessica Rogers
class npc_jessica_rogers : public CreatureScript
{
public:
    npc_jessica_rogers() : CreatureScript("npc_jessica_rogers") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_jessica_rogersAI(creature);
    }

    struct npc_jessica_rogersAI : public ScriptedAI
    {
        npc_jessica_rogersAI(Creature* creature) : ScriptedAI(creature) { }


        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            AddGossipItemFor(player, GossipOptionNpc::None, "How do I play the Ring Toss?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (!player->HasAura(101612) && !player->HasAura(110230) && !player->HasAura(102121) && !player->HasAura(102178) && !player->HasAura(102058) && !player->HasAura(101871))
                AddGossipItemFor(player, GossipOptionNpc::None, "Ready to play! |cFF0000FF(Darkmoon Game Token)|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            SendGossipMenuFor(player, 54485, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                AddGossipItemFor(player, GossipOptionNpc::None, "Alright.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                SendGossipMenuFor(player, 54486, me->GetGUID());
            }

            if (action == GOSSIP_ACTION_INFO_DEF + 2)
            {
                if (player->HasItemCount(71083))
                {
                    CloseGossipMenuFor(player);

                    player->DestroyItemCount(71083, 1, true);
                    player->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    player->AddAura(102058, player);
                    player->SetPower(POWER_ALTERNATE_POWER, 10);

                    return true;
                }
                else
                    SendGossipMenuFor(player, 54603, me->GetGUID());
            }

            if (action == GOSSIP_ACTION_INFO_DEF + 3)
            {
                if (me->IsQuestGiver())
                    player->PrepareQuestMenu(me->GetGUID());

                AddGossipItemFor(player, GossipOptionNpc::None, "How do I play the Ring Toss?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                if (!player->HasAura(101612) && !player->HasAura(110230) && !player->HasAura(102121) && !player->HasAura(102178) && !player->HasAura(102058) && !player->HasAura(101871))
                    AddGossipItemFor(player, GossipOptionNpc::None, "Ready to play! |cFF0000FF(Darkmoon Game Token)|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

                SendGossipMenuFor(player, 54485, me->GetGUID());
            }

            return true;
        }
    };

};

//
class spell_ring_toss : public SpellScriptLoader
{
public:
    spell_ring_toss() : SpellScriptLoader("spell_ring_toss") {}

    class spell_ring_toss_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ring_toss_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            Player* player = GetCaster()->ToPlayer();

            if (Creature* dubenko = caster->FindNearestCreature(54490, 100.0f, true))
            {
                caster->CastSpell(dubenko, 101697, false);
                dubenko->CastSpell(dubenko, 101737, false);
            }

            player->KilledMonsterCredit(54495, ObjectGuid::Empty);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_ring_toss_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnCheckCast += SpellCheckCastFn(spell_ring_toss_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_ring_toss_SpellScript();
    }
};

enum Creatures
{
    NPC_SELINA_DOURMAN = 10445
};

enum Events
{
    EVENT_SELINA_TALK_COOLDOWN = 1
};

enum Gossips
{
    GOSSIP_MENU_SELINA_POIS = 13076,
    GOSSIP_MENU_SELINA_ITEM = 13113
};

enum GossipMenuOptions
{
    GOSSIP_MENU_OPTION_TONK_ARENA_POI = 0,
    GOSSIP_MENU_OPTION_CANNON_POI = 1,
    GOSSIP_MENU_OPTION_WHACK_A_GNOLL_POI = 2,
    GOSSIP_MENU_OPTION_RING_TOSS_POI = 3,
    GOSSIP_MENU_OPTION_SHOOTING_GALLERY_POI = 4,
    GOSSIP_MENU_OPTION_FORTUNE_TELLER_POI = 5
};

enum PointsOfInterest
{
    POI_WHACK_A_GNOLL = 2716,
    POI_CANNON = 2717,
    POI_SHOOTING_GALLERY = 2718,
    POI_TONK_ARENA = 2719,
    POI_FORTUNE_TELLER = 2720,
    POI_RING_TOSS = 2721
};

enum Spells
{
    SPELL_REPLACE_DARKMOON_ADVENTURES_GUIDE = 103413
};

enum Texts
{
    SAY_WELCOME = 0
};

// 10445 - Selina Dourman
struct npc_selina_dourman : public ScriptedAI
{
    npc_selina_dourman(Creature* creature) : ScriptedAI(creature), _talkCooldown(false) { }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        switch (menuId)
        {
        case GOSSIP_MENU_SELINA_POIS:
        {
            uint32 poiId = 0;
            switch (gossipListId)
            {
            case GOSSIP_MENU_OPTION_TONK_ARENA_POI:
                poiId = POI_TONK_ARENA;
                break;
            case GOSSIP_MENU_OPTION_CANNON_POI:
                poiId = POI_CANNON;
                break;
            case GOSSIP_MENU_OPTION_WHACK_A_GNOLL_POI:
                poiId = POI_WHACK_A_GNOLL;
                break;
            case GOSSIP_MENU_OPTION_RING_TOSS_POI:
                poiId = POI_RING_TOSS;
                break;
            case GOSSIP_MENU_OPTION_SHOOTING_GALLERY_POI:
                poiId = POI_SHOOTING_GALLERY;
                break;
            case GOSSIP_MENU_OPTION_FORTUNE_TELLER_POI:
                poiId = POI_FORTUNE_TELLER;
                break;
            default:
                break;
            }
            if (poiId)
                player->PlayerTalkClass->SendPointOfInterest(poiId);
            break;
        }
        case GOSSIP_MENU_SELINA_ITEM:
            me->CastSpell(player, SPELL_REPLACE_DARKMOON_ADVENTURES_GUIDE);
            CloseGossipMenuFor(player);
            break;
        default:
            break;
        }

        return false;
    }

    void DoWelcomeTalk(Unit* talkTarget)
    {
        if (!talkTarget || _talkCooldown)
            return;

        _talkCooldown = true;
        _events.ScheduleEvent(EVENT_SELINA_TALK_COOLDOWN, 30s);
        Talk(SAY_WELCOME, talkTarget);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        switch (_events.ExecuteEvent())
        {
        case EVENT_SELINA_TALK_COOLDOWN:
            _talkCooldown = false;
            break;
        default:
            break;
        }
    }

private:
    EventMap _events;
    bool _talkCooldown;
};

// 7016 - Darkmoon Faire Entrance
class at_darkmoon_faire_entrance : public AreaTriggerScript
{
public:
    at_darkmoon_faire_entrance() : AreaTriggerScript("at_darkmoon_faire_entrance") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
    {
        if (Creature* selinaDourman = player->FindNearestCreature(NPC_SELINA_DOURMAN, 50.f))
            if (npc_selina_dourman* selinaDourmanAI = CAST_AI(npc_selina_dourman, selinaDourman->GetAI()))
                selinaDourmanAI->DoWelcomeTalk(player);

        return true;
    }
};

enum ShootGallery
{
    EVENT_SHOOTGALLERY_START_GAME = 1,
    EVENT_SHOOTGALLERY_FINISH_GAME = 2,
};

class npc_rinling : public CreatureScript
{
public:
    npc_rinling() : CreatureScript("npc_rinling") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rinlingAI(creature);
    }

    struct npc_rinlingAI : public ScriptedAI
    {
        npc_rinlingAI(Creature* creature) : ScriptedAI(creature) { }

        EventMap events;

        bool Active;

        void Reset()
        {
            Active = false;
        }

        void StartGame()
        {
            if (!Active)
            {
                events.ScheduleEvent(EVENT_SHOOTGALLERY_START_GAME, 0s);
                events.ScheduleEvent(EVENT_SHOOTGALLERY_FINISH_GAME, 60s);
            }
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);

            Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHOOTGALLERY_START_GAME:
                    switch (urand(0, 2))
                    {
                    case 0:
                        if (Creature* summon = me->SummonCreature(54231, Position(-4072.19f, 6356.46f, 13.35f, 4.21f), TEMPSUMMON_TIMED_DESPAWN, 5s))
                            summon->CastSpell(summon, 102341, false);

                        me->SummonCreature(54225, -4070.09f, 6354.87f, 12.57f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s);
                        me->SummonCreature(54225, -4068.41f, 6353.09f, 13.24f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s);
                        break;
                    case 1:
                        me->SummonCreature(54225, -4072.19f, 6356.46f, 13.35f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s);

                        if (Creature* summon = me->SummonCreature(54231, -4070.09f, 6354.87f, 12.57f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s))
                            summon->CastSpell(summon, 102341, false);

                        me->SummonCreature(54225, -4068.41f, 6353.09f, 12.24f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s);
                        break;
                    case 2:
                        me->SummonCreature(54225, -4072.19f, 6356.46f, 13.35f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s);
                        me->SummonCreature(54225, -4070.09f, 6354.87f, 12.57f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s);

                        if (Creature* summon = me->SummonCreature(54231, -4068.41f, 6353.09f, 13.24f, 4.21f, TEMPSUMMON_TIMED_DESPAWN, 5s))
                            summon->CastSpell(summon, 102341, false);

                        break;
                    }
                    events.ScheduleEvent(EVENT_SHOOTGALLERY_START_GAME, 5s);
                    break;
                case EVENT_SHOOTGALLERY_FINISH_GAME:
                    Active = false;
                    events.CancelEvent(EVENT_SHOOTGALLERY_START_GAME);
                    break;
                }
            }
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            char const* GOSSIP_BUTTON_1;
            char const* GOSSIP_BUTTON_2;

            switch (LocaleConstant currentlocale = player->GetSession()->GetSessionDbcLocale())
            {
            case 20:    // locales end on 11, case 20 is impossible case, this is just to fix compile warnings.
                break;
            default:
                GOSSIP_BUTTON_1 = "How does the shooting gallery work? ";
                GOSSIP_BUTTON_2 = "I'm ready to shoot! |cFF0000FF(Darkmoon Game Token)|r";
                break;
            };

            AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_BUTTON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (!player->HasAura(101612) && !player->HasAura(110230) && !player->HasAura(102121) && !player->HasAura(102178) && !player->HasAura(102058) && !player->HasAura(101871))
                AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_BUTTON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            SendGossipMenuFor(player, 23002, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
        {
            char const* GOSSIP_BUTTON_1;
            char const* GOSSIP_BUTTON_2;
            char const* GOSSIP_BUTTON_3;

            switch (LocaleConstant currentlocale = player->GetSession()->GetSessionDbcLocale())
            {
            case 20:    // locales end on 11, case 20 is impossible case, this is just to fix compile warnings.
                break;
            default:
                GOSSIP_BUTTON_1 = "How does the shooting gallery work? ";
                GOSSIP_BUTTON_2 = "I'm ready to shoot! |cFF0000FF(Darkmoon Game Token)|r";
                GOSSIP_BUTTON_3 = "Alright.";
                break;
            };

            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_BUTTON_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                SendGossipMenuFor(player, 23003, me->GetGUID());
            }

            if (action == GOSSIP_ACTION_INFO_DEF + 2)
            {
                if (player->HasItemCount(71083))
                {
                    CloseGossipMenuFor(player);

                    player->DestroyItemCount(71083, 1, true);
                    player->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    player->AddAura(101871, player);

                    CAST_AI(npc_rinling::npc_rinlingAI, me->AI())->StartGame();
                    CAST_AI(npc_rinling::npc_rinlingAI, me->AI())->Active = true;

                    return true;
                }
                else
                    SendGossipMenuFor(player, 54603, me->GetGUID());
            }

            if (action == GOSSIP_ACTION_INFO_DEF + 3)
            {
                if (me->IsQuestGiver())
                    player->PrepareQuestMenu(me->GetGUID());

                AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_BUTTON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                if (!player->HasAura(101612) && !player->HasAura(110230) && !player->HasAura(102121) && !player->HasAura(102178) && !player->HasAura(102058) && !player->HasAura(101871))
                    AddGossipItemFor(player, GossipOptionNpc::None, GOSSIP_BUTTON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

                SendGossipMenuFor(player, 23002, me->GetGUID());
            }

            return true;
        }
    };
};

// spells
class spell_gen_repair_damaged_tonk : public SpellScriptLoader
{
public:
    spell_gen_repair_damaged_tonk() : SpellScriptLoader("spell_gen_repair_damaged_tonk") {}

    class spell_gen_repair_damaged_tonk_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_repair_damaged_tonk_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Player* player = GetCaster()->ToPlayer();
            Creature* target = GetHitCreature();

            if (!target)
                return;

            player->KilledMonsterCredit(54504, ObjectGuid::Empty);

            Position myPos = target->GetPosition();
            target->SummonCreature(55356, myPos, TEMPSUMMON_TIMED_DESPAWN, 30s);
            target->DespawnOrUnsummon();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_repair_damaged_tonk_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnCheckCast += SpellCheckCastFn(spell_gen_repair_damaged_tonk_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_repair_damaged_tonk_SpellScript();
    }
};

class spell_gen_shoe_baby : public SpellScriptLoader
{
public:
    spell_gen_shoe_baby() : SpellScriptLoader("spell_gen_shoe_baby") {}

    class spell_gen_shoe_baby_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_shoe_baby_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Player* player = GetCaster()->ToPlayer();
            player->KilledMonsterCredit(54510, ObjectGuid::Empty);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_shoe_baby_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnCheckCast += SpellCheckCastFn(spell_gen_shoe_baby_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_shoe_baby_SpellScript();
    }
};

class spell_cook_crunchy_frog : public SpellScriptLoader
{
public:
    spell_cook_crunchy_frog() : SpellScriptLoader("spell_cook_crunchy_frog") {}

    class spell_cook_crunchy_frog_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_cook_crunchy_frog_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Player* player = GetCaster()->ToPlayer();
            player->AddItem(72058, 1);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_cook_crunchy_frog_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnCheckCast += SpellCheckCastFn(spell_cook_crunchy_frog_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_cook_crunchy_frog_SpellScript();
    }
};

enum InjuriedCarnieYells
{
    SAY_GREETINGS_1 = 0,
    SAY_GREETINGS_2 = 1,
    SAY_GREETINGS_3 = 2,
    SAY_GREETINGS_4 = 3,
    SAY_GREETINGS_5 = 4,
};

class spell_heal_injuried_carnie : public SpellScriptLoader
{
public:
    spell_heal_injuried_carnie() : SpellScriptLoader("spell_heal_injuried_carnie") {}

    class spell_heal_injuried_carnie_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_heal_injuried_carnie_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void HandleHealPct(SpellEffIndex /*effIndex*/)
        {
            Player* player = GetCaster()->ToPlayer();
            Creature* target = GetHitCreature();

            player->KilledMonsterCredit(54518, ObjectGuid::Empty);

            if (!target)
                return;

            switch (urand(0, 4))
            {
            case 0:
                target->AI()->Talk(SAY_GREETINGS_1);
              //  target->HandleEmoteCommand(0);
                target->GetMotionMaster()->MoveRandom(30);
                target->DespawnOrUnsummon(8s);
                break;
            case 1:
                target->AI()->Talk(SAY_GREETINGS_2);
                //target->HandleEmoteCommand(0);
                target->GetMotionMaster()->MoveRandom(30);
                target->DespawnOrUnsummon(8s);
                break;
            case 2:
                target->AI()->Talk(SAY_GREETINGS_3);
              //  target->HandleEmoteCommand(0);
                target->GetMotionMaster()->MoveRandom(30);
                target->DespawnOrUnsummon(8s);
                break;
            case 3:
                target->AI()->Talk(SAY_GREETINGS_4);
              //  target->HandleEmoteCommand(0);
                target->GetMotionMaster()->MoveRandom(30);
                target->DespawnOrUnsummon(8s);
                break;
            case 4:
                target->AI()->Talk(SAY_GREETINGS_5);
             //   target->HandleEmoteCommand(0);
                target->GetMotionMaster()->MoveRandom(30);
                target->DespawnOrUnsummon(8s);
                break;
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_heal_injuried_carnie_SpellScript::HandleHealPct, EFFECT_0, SPELL_EFFECT_HEAL_PCT);
            OnCheckCast += SpellCheckCastFn(spell_heal_injuried_carnie_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_heal_injuried_carnie_SpellScript();
    }
};

class spell_put_up_darkmoon_banner : public SpellScriptLoader
{
public:
    spell_put_up_darkmoon_banner() : SpellScriptLoader("spell_put_up_darkmoon_banner") {}

    class spell_put_up_darkmoon_banner_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_put_up_darkmoon_banner_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Player* player = GetCaster()->ToPlayer();
            player->KilledMonsterCredit(54545, ObjectGuid::Empty);

            if (Creature* loosestones = GetCaster()->FindNearestCreature(54545, 10.0f, true))
            {
                loosestones->SummonGameObject(179965, Position(loosestones->GetPositionX(), loosestones->GetPositionY(), loosestones->GetPositionZ(), loosestones->GetOrientation()), QuaternionData(), 0s);
                loosestones->DespawnOrUnsummon(30s);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_put_up_darkmoon_banner_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnCheckCast += SpellCheckCastFn(spell_put_up_darkmoon_banner_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_put_up_darkmoon_banner_SpellScript();
    }
};

class spell_darkmoon_deathmatch : public SpellScriptLoader
{
public:
    spell_darkmoon_deathmatch() : SpellScriptLoader("spell_darkmoon_deathmatch") {}

    class spell_darkmoon_deathmatch_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_darkmoon_deathmatch_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();

            if (caster->GetPositionY() <= 6400.0f)
                caster->CastSpell(caster, 108919, false);
            else
                caster->CastSpell(caster, 108923, false);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_darkmoon_deathmatch_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            OnCheckCast += SpellCheckCastFn(spell_darkmoon_deathmatch_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_darkmoon_deathmatch_SpellScript();
    }
};

class spell_shoot_gallery_shoot : public SpellScriptLoader
{
public:
    spell_shoot_gallery_shoot() : SpellScriptLoader("spell_shoot_gallery_shoot") {}

    class spell_shoot_gallery_shoot_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_shoot_gallery_shoot_SpellScript);

        SpellCastResult CheckRequirement()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            return SPELL_CAST_OK;
        }

        void OnHit()
        {
            Player* player = GetCaster()->ToPlayer();

            if (GetHitCreature()->GetEntry() == 54231)
            {
                player->KilledMonsterCredit(54231, ObjectGuid::Empty);

                AchievementEntry const* AchievShooter = sAchievementStore.LookupEntry(6022);
                player->CompletedAchievement(AchievShooter);
                player->SetPower(POWER_ALTERNATE_POWER, player->GetPower(POWER_ALTERNATE_POWER) + 1);
            }
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_shoot_gallery_shoot_SpellScript::OnHit);
            OnCheckCast += SpellCheckCastFn(spell_shoot_gallery_shoot_SpellScript::CheckRequirement);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_shoot_gallery_shoot_SpellScript();
    }
};

void AddSC_darkmoon_island()
{
    new playerscript_darkmoon_carousel();
    new npc_jessica_rogers();
    RegisterCreatureAI(npc_selina_dourman);
    new at_darkmoon_faire_entrance();
    new npc_rinling();
    new spell_ring_toss();
    new item_darkmoon_faire_fireworks();
    new spell_darkmoon_carousel_whee();
    new spell_darkmoon_staging_area_teleport();
    new spell_gen_repair_damaged_tonk();
    new spell_gen_shoe_baby();
    new spell_cook_crunchy_frog();
    new spell_heal_injuried_carnie();
    new spell_put_up_darkmoon_banner();
    new spell_darkmoon_deathmatch();
    new spell_shoot_gallery_shoot();
};
