/*
* DekkCore Team Devs
*
*
*
*
*
*/

#include "ruby_life_pools.h"
#include "AreaTriggerAI.h"
#include "AreaTrigger.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "InstanceScript.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "Spell.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"

enum Spells
{
    SPELL_AWAKEN_WHELPS = 373046,
    SPELL_FRIGID_SHARD = 372808,
    SPELL_FROST_INFUSION = 373727,
    SPELL_HAIL_BOMBS = 396044,
    SPELL_CHILLSTORM = 372851,// need check on sniff later
    SPELL_CHILLSTORM_DAMAGE = 385518, // need check on sniff later
    SPELL_CHILLSTORM_EXPLOSION = 397076, // need check on sniff later
};

enum Events
{
    EVENT_AWAKEN_WHELPS = 1,
    EVENT_CHILLSTORM = 2,
    EVENT_FRIGID_SHARD = 3,
    EVENT_FROST_INFUSION = 4,
    EVENT_HAIL_BOMBS = 5
};

class boss_melidrussa_chillworn : public CreatureScript
{
public:
    boss_melidrussa_chillworn() : CreatureScript("boss_melidrussa_chillworn") { }

    struct boss_melidrussa_chillwornAI : public ScriptedAI
    {
        boss_melidrussa_chillwornAI(Creature* creature) : ScriptedAI(creature, DATA_MELIDRUSSA_CHILLWORN) { }

        void Reset() override
        {
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            //Talk(SAY_AGGRO); need add creature text
            instance->SetBossState(DATA_MELIDRUSSA_CHILLWORN, IN_PROGRESS);
            me->RemoveUnitFlag(UNIT_FLAG_CAN_SWIM);
            _events.ScheduleEvent(EVENT_AWAKEN_WHELPS, 10s);
            _events.ScheduleEvent(EVENT_CHILLSTORM, 20s);
            _events.ScheduleEvent(EVENT_FRIGID_SHARD, 30s);
            _events.ScheduleEvent(EVENT_FROST_INFUSION, 40s);
            _events.ScheduleEvent(EVENT_HAIL_BOMBS, 50s);
        }

        void JustDied(Unit* killer) override
        {
            instance->SetBossState(DATA_MELIDRUSSA_CHILLWORN, DONE);
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_AWAKEN_WHELPS:
                    DoCastSelf(SPELL_AWAKEN_WHELPS);
                    _events.ScheduleEvent(EVENT_AWAKEN_WHELPS, 10s);
                    break;
                case EVENT_CHILLSTORM:
                    DoCastVictim(SPELL_CHILLSTORM);
                    _events.ScheduleEvent(EVENT_CHILLSTORM, 20s);
                    break;
                case EVENT_FRIGID_SHARD:
                    DoCastVictim(SPELL_FRIGID_SHARD);
                    _events.ScheduleEvent(EVENT_FRIGID_SHARD, 30s);
                    break;
                case EVENT_FROST_INFUSION: //casted on whelps need script summons later
                   // DoCast(SPELL_FROST_INFUSION);
                    //_events.ScheduleEvent(EVENT_FROST_INFUSION, 40s);
                    break;
                case EVENT_HAIL_BOMBS:
                    DoCastAOE(SPELL_HAIL_BOMBS);
                    _events.ScheduleEvent(EVENT_HAIL_BOMBS, 50s);
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        InstanceScript* _instance;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_melidrussa_chillwornAI(creature);
    }
};

//372808
class spellmelidrussaFrigidShard : public SpellScript
{
    PrepareSpellScript(spellmelidrussaFrigidShard);

    void HandleOnHit()
    {
        if (Player* player = GetHitPlayer())
            player->DealDamage(player, player, 44084, NULL, DOT, SPELL_SCHOOL_MASK_FROST, nullptr, DIRECT_DAMAGE);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spellmelidrussaFrigidShard::HandleOnHit);
    }
};

//
class MelidrussaHailbombs : public SpellScript
{
    PrepareSpellScript(MelidrussaHailbombs);

    void HandleOnHit()
    {
        if (Player* player = GetHitPlayer())
        {
            player->DealDamage(player, player, 8817, NULL, DOT, SPELL_SCHOOL_MASK_FROST, nullptr, DIRECT_DAMAGE);
            if (player->HasAura(396044))
                player->DealDamage(player, player, 66126,NULL, DOT, SPELL_SCHOOL_MASK_FROST, nullptr, DIRECT_DAMAGE);
        }
    }
    void Register() override
    {
        OnHit += SpellHitFn(MelidrussaHailbombs::HandleOnHit);
    }
};

//PORTAL CUSTOM TO NEXT BOSS 369316
class go_portal_customs: public GameObjectScript
{
public:
    go_portal_customs() : GameObjectScript("go_portal_customs") {}

    struct go_portal_customsAI : public GameObjectAI
    {
        go_portal_customsAI(GameObject* go) : GameObjectAI(go)
        {
        }

        bool OnGossipHello(Player* player) override
        {
          
            player->TeleportTo(2521, 1585.283f, -207.162f, 311.222f, 0.853f);
   
            return true;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_portal_customsAI(go);
    }
};

void AddSC_boss_melidrussa_chillworn()
{
    new boss_melidrussa_chillworn();
    new spellmelidrussaFrigidShard();
    new MelidrussaHailbombs();
    new go_portal_customs();
}
