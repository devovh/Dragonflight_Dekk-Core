#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "GridNotifiers.h"
#include "iron_docks.hpp"
#include "ScriptMgr.h"
#include "TemporarySummon.h"
#include "Vehicle.h"

enum eSpells
{
    /// Misc
    SpellSelfStun = 94563,
    SpellEmoteWork = 43831,
    SpellCarryCrate = 173166,
    SpellCarrySack = 167539,
    SpellApplauseCheer = 84062
};

enum eMovementInformed
{
    MovementInformedClefthoofTargetArrivDest = 1,
    MovementInformedIronStarWallContact
};

enum eEmotes
{
    EmoteWork01 = 133,
    EmoteFight = 36,
};

enum eAction
{
    ActionQuietDeath = 744
};

/// Iron star kill event.
class basicevent_kill : public BasicEvent
{
public:

    explicit basicevent_kill(InstanceScript* p_Instance, ObjectGuid p_KillerGuid, ObjectGuid p_VictimGuid) : m_Instance(p_Instance), m_KillerGuid(p_KillerGuid), m_VictimGuid(p_VictimGuid) { }

    bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
    {
        if (m_Instance != nullptr)
        {
            if (Creature* l_Killer = m_Instance->instance->GetCreature(m_KillerGuid))
            {
                if (Creature* l_Victim = m_Instance->instance->GetCreature(m_VictimGuid))
                    l_Killer->KillSelf(l_Victim);
            }
        }

        return true;
    }

private:
    InstanceScript* m_Instance;
    ObjectGuid m_KillerGuid;
    ObjectGuid m_VictimGuid;
    int m_Modifier;
};

/// Grom'kar Battle Master - 83025
class iron_docks_mob_gromkar_battlemaster : public CreatureScript
{
public:

    iron_docks_mob_gromkar_battlemaster() : CreatureScript("iron_docks_mob_gromkar_battlemaster") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eGromkarEvents
        {
            /// Gromkar Battlemaster
            EventBladestorm = 1,
            EventChainDrag,
            EventChargingSlash
        };

        enum eGromkarSpells
        {
            /// Battlemaster
            SpellBladestorm = 167232,
            SpellChainDrag = 176287,
            SpellChargingSlash = 172889,
            SpellChargingSlashJump = 172885
        };

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* p_Attacker) override
        {
            me->RemoveAllAuras();
            events.ScheduleEvent(eGromkarEvents::EventBladestorm, 25s);
            events.ScheduleEvent(eGromkarEvents::EventChainDrag, 30s);
            events.ScheduleEvent(eGromkarEvents::EventChargingSlash, 10s);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eGromkarEvents::EventBladestorm:
                me->CastSpell(me, eGromkarSpells::SpellBladestorm);
                break;
            case eGromkarEvents::EventChainDrag:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                    me->CastSpell(l_Target, eGromkarSpells::SpellChainDrag);
                events.ScheduleEvent(eGromkarEvents::EventChainDrag, 30s);
                events.ScheduleEvent(eGromkarEvents::EventBladestorm, 2s);
                break;
            case eGromkarEvents::EventChargingSlash:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                    me->CastSpell(l_Target, eGromkarSpells::SpellChargingSlashJump);
                events.ScheduleEvent(eGromkarEvents::EventChargingSlash, 15s);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Grom'kar Dead Eye - 83028
/// Grom'kar Dead Eye - 83764
class iron_docks_mob_gromkar_deadeye : public CreatureScript
{
public:

    iron_docks_mob_gromkar_deadeye() : CreatureScript("iron_docks_mob_gromkar_deadeye") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eDeadeyeEvents
        {
            /// Deadeye
            EventIronShot = 1,
            EventLegShot
        };

        enum eDeadeyeSpells
        {
            /// Deadeye
            SpellIronShot = 167239,
            SpellLegShot = 167240
        };

        uint32 m_VisualTimer;

        void Reset() override
        {
            events.Reset();
            m_VisualTimer = 5 * TimeConstants::IN_MILLISECONDS;
            me->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_ROOT);
            me->SetUnitFlag(UnitFlags(UNIT_FLAG_REMOVE_CLIENT_CONTROL));
        }

        void JustEngagedWith(Unit* p_Attacker) override
        {
            events.ScheduleEvent(eDeadeyeEvents::EventIronShot, 8s);
            events.ScheduleEvent(eDeadeyeEvents::EventLegShot, 25s);
        }

        void MoveInLineOfSight(Unit* p_Who) override
        {
            if (p_Who && p_Who->IsInWorld() && p_Who->GetTypeId() != TypeID::TYPEID_PLAYER && p_Who->GetEntry() == eIronDocksCreatures::CreatureIronStar && me->IsWithinDistInMap(p_Who, 1.2f))
                p_Who->KillSelf(me);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
            {
                if (m_VisualTimer <= p_Diff)
                {
                    if (Creature* l_ArcheryTarget = me->FindNearestCreature(eIronDocksCreatures::CreatureArcheryTarget, 30.0f))
                    {
                        me->CastSpell(l_ArcheryTarget, eDeadeyeSpells::SpellIronShot, true);
                        m_VisualTimer = 6 * TimeConstants::IN_MILLISECONDS;
                    }
                }
                else
                    m_VisualTimer -= p_Diff;
            }

            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eDeadeyeEvents::EventLegShot:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eDeadeyeSpells::SpellLegShot);
                events.ScheduleEvent(eDeadeyeEvents::EventLegShot, 25s);
                break;
            case eDeadeyeEvents::EventIronShot:
                if (Unit* l_Target = me->GetVictim())
                    me->CastSpell(l_Target, eDeadeyeSpells::SpellIronShot);
                events.ScheduleEvent(eDeadeyeEvents::EventIronShot, 8s);
                break;
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Grom'kar Foot Soldier - 83765
/// Grom'kar Footsoldier - 85997
/// Grom'kar Footsoldier - 859977
class iron_docks_mob_gromkar_footsoldier : public CreatureScript
{
public:

    iron_docks_mob_gromkar_footsoldier() : CreatureScript("iron_docks_mob_gromkar_footsoldier") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eFootSoldierEvents
        {
            /// Footsoldier
            EventTacticalKick = 1,
            EventDemoralizingShout,
            EventChargingSlash
        };

        enum eFootSoldierSpells
        {
            /// Footsoldier
            SpellTacticalKick = 169413,
            SpellDemoralizingShout = 169341,
            SpellChargingSlash = 172889,
            SpellChargingSlashJump = 172885,
            SpellKneelDown = 173091
        };

        void Reset() override
        {
            events.Reset();
            me->SetFaction(HostileFaction);
            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            me->RemoveAllAuras();
            events.ScheduleEvent(eFootSoldierEvents::EventTacticalKick, 15s);
            events.ScheduleEvent(eFootSoldierEvents::EventDemoralizingShout, 8s);
            events.ScheduleEvent(eFootSoldierEvents::EventChargingSlash, 25s);
        }

        void MoveInLineOfSight(Unit* p_Who) override
        {
            if (p_Who && p_Who->IsInWorld() && p_Who->GetTypeId() != TypeID::TYPEID_PLAYER && p_Who->GetEntry() == eIronDocksCreatures::CreatureIronStar && me->IsWithinDistInMap(p_Who, 1.2f))
                p_Who->KillSelf(me);

            ScriptedAI::MoveInLineOfSight(p_Who);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eFootSoldierEvents::EventDemoralizingShout:
                me->CastSpell(me, eFootSoldierSpells::SpellDemoralizingShout);
                events.ScheduleEvent(eFootSoldierEvents::EventDemoralizingShout, 25s);
                break;
            case eFootSoldierEvents::EventTacticalKick:
                if (Unit* l_Target = me->GetVictim())
                    me->CastSpell(l_Target, eFootSoldierSpells::SpellTacticalKick);
                events.ScheduleEvent(eFootSoldierEvents::EventTacticalKick, 15s);
                break;
            case eFootSoldierEvents::EventChargingSlash:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eFootSoldierSpells::SpellChargingSlashJump);
                events.ScheduleEvent(eFootSoldierEvents::EventChargingSlash, 25s);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Grom'kar Incinerator - 86809
class iron_docks_mob_gromkar_incinerator : public CreatureScript
{
public:

    iron_docks_mob_gromkar_incinerator() : CreatureScript("iron_docks_mob_gromkar_incinerator") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eIncineratorEvents
        {
            /// Incinerator
            EventIncendinarySlug = 907,
            EventSharpnelBlast = 908
        };

        enum eIncienratorSpells
        {
            /// Incinerator
            SpellIncenarySlugs = 176902, ///< Actual full spell, blizzards are just a bunch of r tards.
            SpellSharpnelBlast = 167516,
            SpellIronShot = 167239
        };

        uint32 m_VisualTimer;

        void Reset() override
        {
            events.Reset();
            m_VisualTimer = 6 * TimeConstants::IN_MILLISECONDS;
            me->AddUnitMovementFlag(MovementFlags::MOVEMENTFLAG_ROOT);
            me->SetUnitFlag(UnitFlags(UNIT_FLAG_REMOVE_CLIENT_CONTROL));
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            events.ScheduleEvent(eIncineratorEvents::EventIncendinarySlug, 18s);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
            {
                if (m_VisualTimer <= p_Diff)
                {
                    if (Creature* l_ArcheryTarget = me->FindNearestCreature(eIronDocksCreatures::CreatureArcheryTarget, 30.0f))
                    {
                        me->CastSpell(l_ArcheryTarget, eIncienratorSpells::SpellIronShot, true);
                        m_VisualTimer = 6 * TimeConstants::IN_MILLISECONDS;
                    }
                }
                else
                    m_VisualTimer -= p_Diff;
            }

            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eIncineratorEvents::EventIncendinarySlug:
                if (Unit* l_Target = me->GetVictim())
                    me->CastSpell(l_Target, eIncienratorSpells::SpellIncenarySlugs);
                events.ScheduleEvent(eIncineratorEvents::EventIncendinarySlug, 18s);
                break;
            default:
                break;
            }

            DoSpellAttackIfReady(eIncienratorSpells::SpellSharpnelBlast);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_iron_docksAI(creature);
    }
};

/// Grom'kar Technician - 81432
class iron_docks_mob_gromkar_technician : public CreatureScript
{
public:

    iron_docks_mob_gromkar_technician() : CreatureScript("iron_docks_mob_gromkar_technician") { }

    struct iron_docks_mob_gromkar_technicianAI : public ScriptedAI
    {
        iron_docks_mob_gromkar_technicianAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eTechnicianSpells
        {
            /// Gromkar Technician
            SpellFlyingHammer = 172703,
            SpellHighExplosiveGrenade = 178298,
            SpellGreaseVial = 172649,
            SpellGreaseVialEffect = 172636,
            SpellArmedWithExplosives = 178296
        };

        enum eTechnicianEvents
        {
            /// Gromkar Technician
            EventGreaseVial = 1,
            EventFlyingHammer,
            EventExplosiveGrenade
        };

        uint32 m_VisualTimer;

        void Reset() override
        {
            events.Reset();
            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
            m_VisualTimer = 6 * TimeConstants::IN_MILLISECONDS;
            me->CastSpell(me, eTechnicianSpells::SpellArmedWithExplosives);
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            me->RemoveAura(eSpells::SpellEmoteWork);
            events.ScheduleEvent(eTechnicianEvents::EventGreaseVial, 25s);
            events.ScheduleEvent(eTechnicianEvents::EventFlyingHammer, 12s);
            events.ScheduleEvent(eTechnicianEvents::EventExplosiveGrenade, 20s);
        }

        void MoveInLineOfSight(Unit* p_Who) override
        {
            if (p_Who && p_Who->IsInWorld() && p_Who->GetTypeId() != TypeID::TYPEID_PLAYER && p_Who->GetEntry() == eIronDocksCreatures::CreatureIronStar && me->IsWithinDistInMap(p_Who, 1.2f))
                p_Who->KillSelf(me);

            ScriptedAI::MoveInLineOfSight(p_Who);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
            {
                if (m_VisualTimer <= p_Diff)
                {
                    if (me->HasAura(eSpells::SpellEmoteWork))
                        me->RemoveAura(eSpells::SpellEmoteWork);

                    switch (urand(0, 1))
                    {
                    case 0: ///< Work
                        me->CastSpell(me, eSpells::SpellEmoteWork);
                        break;
                    case 1: ///< Craft
                      //  me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 133);
                        break;
                    default:
                        break;
                    }

                  //  me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                    m_VisualTimer = 6 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_VisualTimer -= p_Diff;
            }

            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eTechnicianEvents::EventGreaseVial:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eTechnicianSpells::SpellGreaseVial);
                events.ScheduleEvent(eTechnicianEvents::EventGreaseVial, 7s);
                break;
            case eTechnicianEvents::EventFlyingHammer:
                if (Unit* l_Victim = me->GetVictim())
                    me->CastSpell(l_Victim, eTechnicianSpells::SpellFlyingHammer);
                events.ScheduleEvent(eTechnicianEvents::EventFlyingHammer, 14s);
                break;
            case eTechnicianEvents::EventExplosiveGrenade:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eTechnicianSpells::SpellHighExplosiveGrenade);
                me->RemoveAura(eTechnicianSpells::SpellArmedWithExplosives);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_gromkar_technicianAI(p_Creature);
    }
};

/// Siegemaster Olugar - 83026
class iron_docks_mob_siege_master_olugar : public CreatureScript
{
public:

    iron_docks_mob_siege_master_olugar() : CreatureScript("iron_docks_mob_siege_master_olugar") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eOlugarEvents
        {
            /// Siege Master Olugar
            EventGatecrasher = 1,
            EventShatteringStrike
        };

        enum eOlugarSpells
        {
            /// Siege Master Olugar
            SpellGatecrasherDamage = 172963,
            SpellThrowGatecrasher = 172952,
            SpellPitFighter = 173455, ///< Upon disarm
            SpellShatteringStrike = 172982
        };

        uint32 m_VisualTimer;

        void Reset() override
        {
            events.Reset();
            me->SetFaction(HostileFaction);
            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
            m_VisualTimer = 16 * TimeConstants::IN_MILLISECONDS;
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            /// I don't think Darona actually shot barber arrows, wowhead is stupid. Icy veins says it aswell.
            events.ScheduleEvent(eOlugarEvents::EventShatteringStrike, 9s);
            events.ScheduleEvent(eOlugarEvents::EventGatecrasher, 20s);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
            {
                if (m_VisualTimer <= p_Diff)
                {
                    me->Say("Stop showin' off.", Language::LANG_UNIVERSAL, me);
                    m_VisualTimer = 16 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_VisualTimer -= p_Diff;
            }

            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            /// Pit fighter
            if (me->HasAuraType(AuraType::SPELL_AURA_MOD_DISARM))
            {
                if (!me->HasAura(eOlugarSpells::SpellPitFighter))
                    me->AddAura(eOlugarSpells::SpellPitFighter, me);
            }
            else
            {
                if (me->HasAura(eOlugarSpells::SpellPitFighter))
                    me->RemoveAura(eOlugarSpells::SpellPitFighter);
            }

            switch (events.ExecuteEvent())
            {
            case eOlugarEvents::EventShatteringStrike:
            {
                if (Unit* l_Target = me->GetVictim())
                    me->CastSpell(l_Target, eOlugarSpells::SpellShatteringStrike);
                events.ScheduleEvent(eOlugarEvents::EventShatteringStrike, 9s);
                break;
            }
            case eOlugarEvents::EventGatecrasher:
            {
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0F, true))
                    me->CastSpell(l_Target, eOlugarSpells::SpellThrowGatecrasher);
                events.ScheduleEvent(eOlugarEvents::EventGatecrasher, 20s);
                break;
            }
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Hammer Stalker - 86534
class iron_docks_mob_hammer_stalker : public CreatureScript /// Being used for the hammer stalker spell from Throw Gatecrasher of Olugar.
{
public:

    iron_docks_mob_hammer_stalker() : CreatureScript("iron_docks_mob_hammer_stalker") { }

    struct iron_docks_mob_hammer_stalkerAI : public ScriptedAI
    {
        iron_docks_mob_hammer_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eOlugarSpells
        {
            /// Siege Master Olugar
            SpellGatecrasherDamage = 172963
        };

        uint32 m_DamageDiff;

        void Reset() override
        {
            me->SetFaction(HostileFaction);
            m_DamageDiff = 1 * TimeConstants::IN_MILLISECONDS;
            me->SetUnitFlag(UnitFlags(UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE));
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (m_DamageDiff <= p_Diff)
            {
                std::list<Player*> l_ListPlayers;
                me->GetPlayerListInGrid(l_ListPlayers, 2.0f);
                if (!l_ListPlayers.empty())
                {
                    for (auto l_Itr : l_ListPlayers)
                    {
                        l_Itr->CastSpell(l_Itr, eOlugarSpells::SpellGatecrasherDamage);
                    }
                }

                m_DamageDiff = 1 * TimeConstants::IN_MILLISECONDS;
            }
            else
                m_DamageDiff -= p_Diff;
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_hammer_stalkerAI(p_Creature);
    }
};

/// Champion Druna - 81603
class iron_docks_mob_champion_darona : public CreatureScript
{
public:

    iron_docks_mob_champion_darona() : CreatureScript("iron_docks_mob_champion_darona") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eDorunaEvents
        {
            /// Champion Daruna
            EventBurningArrow = 1,
            EventBarbedArrow,
            EventChampionPresence
        };

        enum eDorunaSpells
        {
            /// Champion Darona
            SpellBarbedArrowBarrageDummy = 166923,
            SpellBarbedArrowAreaTrigger = 164278,
            SpellBarbedArrowAura = 164370,
            SpellBarbedArrowDot = 164648,
            SpellBurningArrowDummy = 172810,
            SpellBurningArrowAreaTrigger = 164234,
            SpellBurningArrowDoT = 164632,
            SpellChampionsPresence = 173091
        };

        void Reset() override
        {
            events.Reset();
            me->CastSpell(me, eDorunaSpells::SpellChampionsPresence);
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            events.ScheduleEvent(eDorunaEvents::EventBurningArrow, 8s);
            events.ScheduleEvent(eDorunaEvents::EventBarbedArrow, 11s);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eDorunaEvents::EventBurningArrow:
            {
                me->Yell("Light them up!", Language::LANG_UNIVERSAL, me);
                ///< Commands the Flameslinger to shoot         
                std::list<Creature*> l_ListFlameSlingers;
                me->GetCreatureListWithEntryInGrid(l_ListFlameSlingers, eIronDocksCreatures::CreatureGromkarFlameslinger, 200.0f);
                if (!l_ListFlameSlingers.empty())
                {
                    for (auto l_Itr : l_ListFlameSlingers)
                    {
                        if (l_Itr->IsAIEnabled())
                            l_Itr->GetAI()->DoAction(eIronDocksActions::ActionBurningArrowSingle);
                    }
                }

                events.ScheduleEvent(eDorunaEvents::EventBurningArrow, 30s);
                break;
            }
            case eDorunaEvents::EventBarbedArrow:
            {
                me->CastSpell(me, eDorunaSpells::SpellBarbedArrowBarrageDummy);
                me->AddAura(eDorunaSpells::SpellBarbedArrowAura, me);
                events.ScheduleEvent(eDorunaEvents::EventBarbedArrow, 50s);
                break;
            }
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Pitwarden Gwarnok - 84520
class iron_docks_mob_gwarnok : public CreatureScript
{
public:

    iron_docks_mob_gwarnok() : CreatureScript("iron_docks_mob_gwarnok") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eGwarnokEvents
        {
            /// Gromkar Battlemaster
            EventBladestorm = 1,
            EventChainDrag,
            EventChargingSlash
        };

        enum eGwarnokSpells
        {
            /// Pitwarden Gwarnok
            SpellBladestorm = 167232,
            SpellChainDrag = 176287,
            SpellChargingSlash = 172889,
            SpellChargingSlashJump = 172885,
            SpellBrutalInspiration = 172943
        };

        void Reset() override
        {
            events.Reset();
            me->SetReactState(ReactStates::REACT_DEFENSIVE);
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            events.ScheduleEvent(eGwarnokEvents::EventChargingSlash, 8s);
            events.ScheduleEvent(eGwarnokEvents::EventChainDrag, 15s);
            me->CastSpell(me, eGwarnokSpells::SpellBrutalInspiration);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eGwarnokEvents::EventBladestorm:
            {
                me->CastSpell(me, eGwarnokSpells::SpellBladestorm);
                break;
            }
            case eGwarnokEvents::EventChargingSlash:
            {
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0F, true))
                    me->CastSpell(l_Target, eGwarnokSpells::SpellChargingSlashJump);
                events.ScheduleEvent(eGwarnokEvents::EventChargingSlash, 15s);
                break;
            }
            case eGwarnokEvents::EventChainDrag:
            {
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0F, true))
                {
                    me->CastSpell(l_Target, eGwarnokSpells::SpellChainDrag);
                    l_Target->GetMotionMaster()->MoveJump(*me, 8.0f, 5.0f, 10.0f);
                }
                events.ScheduleEvent(eGwarnokEvents::EventBladestorm, 2s);
                events.ScheduleEvent(eGwarnokEvents::EventChainDrag, 25s);
                break;
            }
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Iron Star - 81247
class iron_docks_mob_iron_star_vehicle : public CreatureScript
{
public:

    iron_docks_mob_iron_star_vehicle() : CreatureScript("iron_docks_mob_iron_star_vehicle") { }

    struct iron_docks_mob_iron_star_vehicleAI : public ScriptedAI
    {
        iron_docks_mob_iron_star_vehicleAI(Creature* p_Creature) : ScriptedAI(p_Creature)
        {
            m_Instance = me->GetInstanceScript();
        }

        InstanceScript* m_Instance;

        void Reset() override
        {
        }

        void UpdateAI(uint32 const p_Diff) override
        {
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_iron_star_vehicleAI(p_Creature);
    }
};

/// Ogron Laborer - 83578
class iron_docks_mob_laborer_ogron : public CreatureScript
{
public:

    iron_docks_mob_laborer_ogron() : CreatureScript("iron_docks_mob_laborer_ogron") { }

    struct iron_docks_mob_laborer_ogronAI : public ScriptedAI
    {
        iron_docks_mob_laborer_ogronAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eOrgonEvents
        {
            /// Ogron
            EventFlurry = 1,
            EventThunderingStomp
        };

        enum eOrgonSpells
        {
            /// Unruly Orgon
            SpellFlurry = 178412,
            SpellFlurryDamage = 178414,
            SpellThunderingStomp = 173135,
            SpellCosmeticRoar = 105533
        };

        enum eOrgonCreatures
        {
            CreatureTriggerCannon = 99657
        };

        bool m_HasShouted;

        void Reset() override
        {
            events.Reset();
            m_HasShouted = false;
            me->SetFaction(HostileFaction);
            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
        }

        void JustEngagedWith(Unit* p_Attacker) override
        {
            me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
            me->SetAIAnimKitId(0);
            events.ScheduleEvent(eOrgonEvents::EventThunderingStomp, 20s);
            events.ScheduleEvent(eOrgonEvents::EventFlurry, 13s);
        }

        void MoveInLineOfSight(Unit* p_Who) override
        {
            if (p_Who && p_Who->IsInWorld() && p_Who->GetTypeId() != TypeID::TYPEID_PLAYER && p_Who->GetEntry() == eIronDocksCreatures::CreatureIronStar && me->IsWithinDistInMap(p_Who, 1.2f))
                p_Who->KillSelf(me);

            if (p_Who && p_Who->IsInWorld() && p_Who->GetTypeId() == TypeID::TYPEID_PLAYER && me->IsWithinDistInMap(p_Who, 10.0f) && !m_HasShouted && me->FindNearestCreature(eOrgonCreatures::CreatureTriggerCannon, 5.0f))
            {
                m_HasShouted = true;
                me->Yell("CRUSH THEM!!", Language::LANG_UNIVERSAL, me);
            }

            ScriptedAI::MoveInLineOfSight(p_Who);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eOrgonEvents::EventThunderingStomp:
                me->CastSpell(me, eOrgonSpells::SpellThunderingStomp);
                events.ScheduleEvent(eOrgonEvents::EventThunderingStomp, 20s);
                break;
            case eOrgonEvents::EventFlurry:
                if (Unit* l_Target = me->GetVictim())
                    me->CastSpell(l_Target, eOrgonSpells::SpellFlurry);
                events.ScheduleEvent(eOrgonEvents::EventFlurry, 13s);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_laborer_ogronAI(p_Creature);
    }
};

/// Grom'kar Deck-Hand - 83697
class iron_docks_mob_gromkar_deck_hand : public CreatureScript
{
public:

    iron_docks_mob_gromkar_deck_hand() : CreatureScript("iron_docks_mob_gromkar_deck_hand") { }

    struct iron_docks_mob_gromkar_deck_handAI : public ScriptedAI
    {
        iron_docks_mob_gromkar_deck_handAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eDeckHandEvents
        {
            /// Deck-hand
            EventHatchetToss = 921
        };

        enum eDeckHandSpells
        {
            /// Deckhand
            SpellHatchetToss = 173112,
        };

        void Reset() override
        {
            events.Reset();

            switch (urand(0, 1)) ///< Handles visual for Deck hands.
            {
            case 0:
                me->CastSpell(me, eSpells::SpellCarrySack);
                break;
            case 1:
                me->CastSpell(me, eSpells::SpellCarryCrate);
                break;
            default:
                break;
            }
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            // Removes to non following units.
            me->RemoveAura(eSpells::SpellEmoteWork);
           // me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
            events.ScheduleEvent(eDeckHandEvents::EventHatchetToss, 10s);
        }

        void MoveInLineOfSight(Unit* p_Who) override
        {
            if (p_Who && p_Who->IsInWorld() && p_Who->GetTypeId() != TypeID::TYPEID_PLAYER && p_Who->GetEntry() == eIronDocksCreatures::CreatureIronStar && me->IsWithinDistInMap(p_Who, 1.2f))
                p_Who->KillSelf(me);

            ScriptedAI::MoveInLineOfSight(p_Who);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eDeckHandEvents::EventHatchetToss:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eDeckHandSpells::SpellHatchetToss);
                events.ScheduleEvent(eDeckHandEvents::EventHatchetToss, 12s);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_gromkar_deck_handAI(p_Creature);
    }
};

/// Grom'kar Chainmaster - 86526
class iron_docks_mob_gromkar_chain_master : public CreatureScript
{
public:

    iron_docks_mob_gromkar_chain_master() : CreatureScript("iron_docks_mob_gromkar_chain_master") { }

    struct iron_docks_mob_gromkar_chain_masterAI : public ScriptedAI
    {
        iron_docks_mob_gromkar_chain_masterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eChainMasterEvents
        {
            EventIronWarCry = 1
        };

        enum eChainMasterSpells
        {
            SpellIronWarCry = 173042
        };

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* p_Who) override
        {
            events.ScheduleEvent(eChainMasterEvents::EventIronWarCry, 10s);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eChainMasterEvents::EventIronWarCry:
                DoCast(eChainMasterSpells::SpellIronWarCry);
                events.ScheduleEvent(eChainMasterEvents::EventIronWarCry, 25s);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_gromkar_chain_masterAI(p_Creature);
    }
};

/// Grom'kar Technician - 83763
class iron_docks_mob_gromkar_technician_deckhand_leader : public CreatureScript
{
public:

    iron_docks_mob_gromkar_technician_deckhand_leader() : CreatureScript("iron_docks_mob_gromkar_technician_deckhand_leader") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature)
        {
        }

        enum eTechnicianEvents
        {
            /// Gromkar Technician
            EventGreaseVial = 1,
            EventFlyingHammer,
            EventExplosiveGrenade
        };

        enum eTechnicianSpells
        {
            /// Gromkar Technician
            SpellFlyingHammer = 172703,
            SpellHighExplosiveGrenade = 178298,
            SpellGreaseVial = 172649,
            SpellGreaseVialEffect = 172636
        };

        std::list<ObjectGuid> m_Workers;

        void Reset() override
        {
            events.Reset();
            me->SetSpeed(UnitMoveType::MOVE_RUN, 0.5f);
        }

        void JustEngagedWith(Unit* p_Attacker) override
        {
            me->RemoveAura(eSpells::SpellEmoteWork);
            events.ScheduleEvent(eTechnicianEvents::EventGreaseVial, 9s);
            events.ScheduleEvent(eTechnicianEvents::EventFlyingHammer, 12s);
            events.ScheduleEvent(eTechnicianEvents::EventExplosiveGrenade, 20s);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eTechnicianEvents::EventGreaseVial:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eTechnicianSpells::SpellGreaseVial);
                events.ScheduleEvent(eTechnicianEvents::EventGreaseVial, 9s);
                break;
            case eTechnicianEvents::EventFlyingHammer:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eTechnicianSpells::SpellFlyingHammer);
                events.ScheduleEvent(eTechnicianEvents::EventFlyingHammer, 12s);
                break;
            case eTechnicianEvents::EventExplosiveGrenade:
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                    me->CastSpell(l_Target, eTechnicianSpells::SpellHighExplosiveGrenade);
                events.ScheduleEvent(eTechnicianEvents::EventExplosiveGrenade, 20s);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Trigger_Cannon_Event_Start - 99658
class iron_docks_mob_stand_third_event : public CreatureScript
{
public:
    iron_docks_mob_stand_third_event() : CreatureScript("iron_docks_mob_stand_third_event") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        bool m_CanEvent;

        void Reset() override
        {
            me->SetFaction(FriendlyFaction);
            m_CanEvent = false;
        }

        void MoveInLineOfSight(Unit* p_Who) override
        {
            if (p_Who && p_Who->IsInWorld() && p_Who->GetTypeId() == TypeID::TYPEID_PLAYER && me->IsWithinDistInMap(p_Who, 15.0f) && !m_CanEvent)
            {
                m_CanEvent = true;

                if (InstanceScript* l_InstanceScript = me->GetInstanceScript())
                    l_InstanceScript->SetData(eIronDocksDatas::DataThirdEvent, uint32(true));
            }

            ScriptedAI::MoveInLineOfSight(p_Who);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Trigger_Cannon_Crash_Fixed_Target - 99657
/// Trigger_Cannon_Crash_Fixed_Target - 100981
/// Trigger_Cannon_Crash_Fixed_Target - 100982
class iron_docks_trigger_cannon : public CreatureScript
{
public:
    iron_docks_trigger_cannon() : CreatureScript("iron_docks_trigger_cannon") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        void Reset() override
        {
            me->SetFaction(HostileFaction);
            me->SetDisplayId(InvisibleDisplay);
           // me->GetMap()->SetObjectVisibility(1000.0f);
            me->SetUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE));
            me->SetReactState(ReactStates::REACT_PASSIVE);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};


/// Archery Target - 79423
class iron_docks_mob_archery_target : public CreatureScript
{
public:

    iron_docks_mob_archery_target() : CreatureScript("iron_docks_mob_archery_target") { }

    struct iron_docks_mob_archery_targetAI : public ScriptedAI
    {
        iron_docks_mob_archery_targetAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        void Reset() override
        {
            me->SetReactState(ReactStates::REACT_PASSIVE);
            me->SetUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_PC));
        }

     void DamageTaken(Unit* attacker, uint32& p_Damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            p_Damage = 0;
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_archery_targetAI(p_Creature);
    }
};

/// Rampaging Clefthoof - 83392
class iron_docks_mob_rampaging_clefthoof : public CreatureScript
{
public:

    iron_docks_mob_rampaging_clefthoof() : CreatureScript("iron_docks_mob_rampaging_clefthoof") { }

    struct mob_iron_docksAI : public ScriptedAI
    {
        mob_iron_docksAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eClefthoofEvents
        {
            /// Rushing
            EventClefthoofStampede = 1
        };

        enum eClefthoofActions
        {
            ActionStopStampede = 1
        };

        enum eClefthoofSpells
        {
            /// Rushing
            SpellClefthoofStampedeVisualMovement = 173350,
            SpellClefthoofStampedeVisualHorning = 173351,
            SpellClefthoofStampedeVisualJump = 173352,
            SpellClefthoofStampedeDummyCast = 173384,
            SpellClefthoofStampedeDamage = 173351,
            SpellClefthoofSpinyHorns = 158349,
            SpellClefthoofTrampled = 173349,
            SpellClefthoofFrenzy = 158337
        };

        ObjectGuid m_StampedeGuid;
        uint32 m_StampedeDiff;
        bool m_Frenzied;
        bool m_Stampede;

        void Reset() override
        {
            events.Reset();
            m_Frenzied = false;
            m_Stampede = false;
            m_StampedeGuid = ObjectGuid::Empty;
            m_StampedeDiff = 1 * TimeConstants::IN_MILLISECONDS;
            me->AddAura(eClefthoofSpells::SpellClefthoofSpinyHorns, me);
        }

        void JustEngagedWith(Unit* p_Attacker) override
        {
            events.ScheduleEvent(eClefthoofEvents::EventClefthoofStampede, 12s);
        }

     void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (!m_Frenzied && me->GetHealthPct() <= 30)
            {
                m_Frenzied = true;
                me->CastSpell(me, eClefthoofSpells::SpellClefthoofFrenzy);
            }
        }

        void DoAction(int32 const p_Action) override
        {
            switch (p_Action)
            {
            case eClefthoofActions::ActionStopStampede:
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                me->CastSpell(me, eClefthoofSpells::SpellClefthoofStampedeDamage);
                me->RemoveAura(eClefthoofSpells::SpellClefthoofStampedeVisualMovement);
                me->RemoveUnitFlag(UnitFlags(UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC));

                if (Unit* l_Target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 100.0f, true))
                {
                    me->Attack(l_Target, true);
                    me->GetMotionMaster()->MoveChase(l_Target);
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (m_Stampede)
            {
                /// Stops Primal Assault when near a player
                if (m_StampedeGuid != ObjectGuid::Empty)
                {
                    if (Player* l_Nearest = ObjectAccessor::GetPlayer(*me, m_StampedeGuid))
                    {
                        if (me->IsWithinMeleeRange(l_Nearest))
                        {
                            m_Stampede = false;
                            m_StampedeGuid = ObjectGuid::Empty;
                            DoAction(eClefthoofActions::ActionStopStampede);
                        }
                    }
                }

                if (m_StampedeDiff <= p_Diff)
                {
                    std::list<Player*> l_PlayerList;

                    l_PlayerList = me->SelectNearestPlayers(2.0f, true);

                    if (l_PlayerList.empty())
                        return;

                    for (Player* l_Itr : l_PlayerList)
                    {
                        if (me->isInFront(l_Itr, float(M_PI)))
                            l_Itr->AddAura(eClefthoofSpells::SpellClefthoofTrampled, l_Itr);
                    }

                    m_StampedeDiff = 2 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_StampedeDiff -= p_Diff;
            }


            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eClefthoofEvents::EventClefthoofStampede:
            {
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 45.0f, true))
                {
                    m_Stampede = true;
                    me->AttackStop();
                    m_StampedeGuid = l_Target->GetGUID();
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    m_StampedeDiff = 1 * TimeConstants::IN_MILLISECONDS;
                    me->CastSpell(l_Target, eClefthoofSpells::SpellClefthoofStampedeDummyCast);
                }

                events.ScheduleEvent(eClefthoofEvents::EventClefthoofStampede, 25s);
                break;
            }
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new mob_iron_docksAI(p_Creature);
    }
};

/// Ironwing Flamespitter - 83389
class iron_docks_mob_ironwing_flamespitter : public CreatureScript
{
public:

    iron_docks_mob_ironwing_flamespitter() : CreatureScript("iron_docks_mob_ironwing_flamespitter") { }

    struct iron_docks_mob_ironwing_flamespitterAI : public ScriptedAI
    {
        iron_docks_mob_ironwing_flamespitterAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eIronwingFlamespitterEvents
        {
            /// Ironwing Flamespitter
            EventLavaBarrage = 1,
            EventLavaBlast
        };

        enum eIronwingFlamespitterSpells
        {
            /// Ironwing Flamespitter
            SpellLavaBarrageDummy = 176356,
            SpellLavaBarrageAreaTrigger = 173482,
            SpellLavaBarrageDot = 173489,
            SpellLavaBlastDummyAura = 173514,
            SpellLavaBlastMissile = 173516,
            SpellLavaBlastDot = 173517,
            SpellLavaBlastAreatrigger = 173518
        };

        void Reset() override
        {
            events.Reset();
        }

        void JustEngagedWith(Unit* p_Attacker) override
        {
            events.ScheduleEvent(eIronwingFlamespitterEvents::EventLavaBlast, 10s);
            events.ScheduleEvent(eIronwingFlamespitterEvents::EventLavaBarrage, 20s);
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(p_Diff);

            if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
            case eIronwingFlamespitterEvents::EventLavaBarrage:
                me->CastSpell(me, eIronwingFlamespitterSpells::SpellLavaBarrageDummy); // dummy
                events.ScheduleEvent(eIronwingFlamespitterEvents::EventLavaBarrage, 20s);
                break;
            case eIronwingFlamespitterEvents::EventLavaBlast:
                me->CastSpell(me, eIronwingFlamespitterSpells::SpellLavaBlastDummyAura); // dummy
                events.ScheduleEvent(eIronwingFlamespitterEvents::EventLavaBlast, 35s);
                break;
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new iron_docks_mob_ironwing_flamespitterAI(p_Creature);
    }
};

/// Flurry - 178421
class iron_docks_spell_flurry_periodic : public SpellScriptLoader
{
public:

    iron_docks_spell_flurry_periodic() : SpellScriptLoader("iron_docks_spell_flurry_periodic") { }

    class iron_docks_auras : public AuraScript
    {
        PrepareAuraScript(iron_docks_auras);

        enum eOrgonSpells
        {
            SpellFlurryDamage = 178414
        };

        void HandlePeriodic(AuraEffect const* p_AurEff)
        {
            PreventDefaultAction();

            if (Unit* l_Caster = GetCaster())
            {
                std::list<Player*> l_PlayerList;
                l_Caster->GetPlayerListInGrid(l_PlayerList, 5.0f);
                if (l_PlayerList.empty())
                    return;

                for (auto l_Itr : l_PlayerList)
                {
                    if (l_Caster->isInFront(l_Itr, float(M_PI) * 0.5f))
                        l_Caster->CastSpell(l_Itr, eOrgonSpells::SpellFlurryDamage);
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(iron_docks_auras::HandlePeriodic, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new iron_docks_auras();
    }
};

/// Launch Forward - 167299
class iron_docks_spell_charge_forward : public SpellScriptLoader
{
public:

    iron_docks_spell_charge_forward() : SpellScriptLoader("iron_docks_spell_charge_forward") { }

    class iron_docks_spell_charge_forward_SpellScript : public SpellScript
    {
        PrepareSpellScript(iron_docks_spell_charge_forward_SpellScript);

        enum eIronStarSpellChargeForwardNpc
        {
            CreatureIronStarNpc = 812477
        };

        void HandleCharge()
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (l_Caster->GetVehicleKit())
                {
                    l_Caster->ToCreature()->DespawnOrUnsummon();
                    l_Caster->GetVehicleKit()->RemoveAllPassengers();
                    l_Caster->SummonCreature(eIronStarSpellChargeForwardNpc::CreatureIronStarNpc, *l_Caster, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN);
                }
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(iron_docks_spell_charge_forward_SpellScript::HandleCharge);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new iron_docks_spell_charge_forward_SpellScript();
    }
};

/// Lava Blast - 173514 
class iron_docks_spell_lava_blast : public SpellScriptLoader
{
public:

    iron_docks_spell_lava_blast() : SpellScriptLoader("iron_docks_spell_lava_blast") { }

    class iron_docks_spell_lava_blast_AuraScript : public AuraScript
    {
        PrepareAuraScript(iron_docks_spell_lava_blast_AuraScript);

        enum eLavaBlastSpells
        {
            SpellLavaBlastTriggerMissile = 173516
        };

        void HandlePeriodic(AuraEffect const* /*p_AurEff*/)
        {
            PreventDefaultAction();

            if (Unit* l_Caster = GetCaster())
            {
                if (l_Caster->IsAIEnabled())
                {
                    if (Unit* l_Target = l_Caster->GetAI()->SelectTarget(SelectTargetMethod::Random, 0, 300.0f, true))
                        l_Caster->CastSpell(l_Target, eLavaBlastSpells::SpellLavaBlastTriggerMissile, true);
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(iron_docks_spell_lava_blast_AuraScript::HandlePeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new iron_docks_spell_lava_blast_AuraScript();
    }
};

/// Barbed Arrow Barrage - 164370
class iron_docks_spell_barbed_arrow_aura : public SpellScriptLoader
{
public:

    iron_docks_spell_barbed_arrow_aura() : SpellScriptLoader("iron_docks_spell_barbed_arrow_aura") { }

    class iron_docks_auraScript : public AuraScript
    {
        PrepareAuraScript(iron_docks_auraScript);

        enum eBarbedarrowSpells
        {
            SpellBarbedArrowAreaTrigger = 164278
        };

        void HandlePeriodic(AuraEffect const* /*p_AurEff*/)
        {
            PreventDefaultAction();

            if (Unit* l_Caster = GetCaster())
            {
                if (l_Caster->IsAIEnabled())
                {
                    if (Unit* l_Target = l_Caster->GetAI()->SelectTarget(SelectTargetMethod::Random, 0, 300.0f, true))
                        l_Caster->CastSpell(l_Target, eBarbedarrowSpells::SpellBarbedArrowAreaTrigger);
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(iron_docks_auraScript::HandlePeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new iron_docks_auraScript();
    }
};

/// Barbed Arrows - 164371 
class iron_docks_spell_barbed_arrow_dummy : public SpellScriptLoader
{
public:

    iron_docks_spell_barbed_arrow_dummy() : SpellScriptLoader("iron_docks_spell_barbed_arrow_dummy") { }

    class iron_docks_spell_barbed_arrow_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(iron_docks_spell_barbed_arrow_dummy_SpellScript);

        enum eBarbedArrowSpells
        {
            SpellBarbedArrowAura = 164370
        };

        void HandleDummy(SpellEffIndex p_EffIndex)
        {
            if (Unit* l_Caster = GetCaster())
                l_Caster->CastSpell(l_Caster, eBarbedArrowSpells::SpellBarbedArrowAura);
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(iron_docks_spell_barbed_arrow_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new iron_docks_spell_barbed_arrow_dummy_SpellScript();
    }
};

/// Trampling Stampede - 173384 
class iron_docks_spell_trampling_stampede : public SpellScriptLoader
{
public:

    iron_docks_spell_trampling_stampede() : SpellScriptLoader("iron_docks_spell_trampling_stampede") { }

    class iron_docks_spell_trampling_stampede_SpellScript : public SpellScript
    {
        PrepareSpellScript(iron_docks_spell_trampling_stampede_SpellScript);

        enum eTramplingStampedeSpells
        {
            SpellClefthoofStampedeVisualMovement = 173350,
            SpellClefthoofStampedeVisualHorning = 173351,
            SpellClefthoofStampedeVisualJump = 173352,
            SpellClefthoofStampedeDummyCast = 173384,
            SpellClefthoofStampedeDamage = 173351,
            SpellClefthoofTrampled = 173349
        };

        void HandleBeforeCast()
        {
            if (GetCaster())
                GetCaster()->AddAura(eTramplingStampedeSpells::SpellClefthoofStampedeVisualMovement, GetCaster());
        }

        void HandleDummy(SpellEffIndex p_EffIndex)
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (Unit* l_Target = GetExplTargetUnit())
                {
                    Position l_Position;
                    l_Position = l_Target->GetPosition();
                    l_Caster->GetMotionMaster()->MoveCharge(l_Position.GetPositionX(), l_Position.GetPositionY(), l_Position.GetPositionZ(), 42.0f, eMovementInformed::MovementInformedClefthoofTargetArrivDest);
                }
            }
        }

        void Register() override
        {
            BeforeCast += SpellCastFn(iron_docks_spell_trampling_stampede_SpellScript::HandleBeforeCast);
            OnEffectLaunch += SpellEffectFn(iron_docks_spell_trampling_stampede_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new iron_docks_spell_trampling_stampede_SpellScript();
    }
};

/// Burning Arrows - 172810
class iron_docks_spell_burning_arrow_aura : public SpellScriptLoader
{
public:

    iron_docks_spell_burning_arrow_aura() : SpellScriptLoader("iron_docks_spell_burning_arrow_aura") { }

    class iron_docks_Spells : public AuraScript
    {
        PrepareAuraScript(iron_docks_Spells);

        enum eBurningArrowSpells
        {
            SpellBurningArrowDummy = 172810,
            SpellBurningArrowAreaTrigger = 164234,
            SpellBurningArrowDoT = 164632
        };

        void HandlePeriodic(AuraEffect const* /*p_AurEff*/)
        {
            PreventDefaultAction();

            if (Unit* l_Caster = GetCaster())
            {
                if (l_Caster->IsAIEnabled())
                {
                    if (Unit* l_Target = l_Caster->GetAI()->SelectTarget(SelectTargetMethod::Random, 0, 300.0f, true))
                        l_Caster->CastSpell(l_Target, eBurningArrowSpells::SpellBurningArrowAreaTrigger);
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(iron_docks_Spells::HandlePeriodic, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new iron_docks_Spells();
    }
};

/// Charging Slash - 172885
class iron_docks_spell_charging_slash_effect : public SpellScriptLoader
{
public:

    iron_docks_spell_charging_slash_effect() : SpellScriptLoader("iron_docks_spell_charging_slash_effect") { }

    class spells_iron_docks_SpellScript : public SpellScript
    {
        PrepareSpellScript(spells_iron_docks_SpellScript);

        enum eChargingSlashSpells
        {
            SpellChargingSlash = 172889,
            SpellChargingSlashJump = 172885
        };

        void OnHitTarget()
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (Unit* l_Target = GetHitUnit())
                    l_Caster->CastSpell(l_Target, eChargingSlashSpells::SpellChargingSlash);
            }
        }

        void Register() override
        {
            OnHit += SpellHitFn(spells_iron_docks_SpellScript::OnHitTarget);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spells_iron_docks_SpellScript();
    }
};

/// Lava Barrage Dummy - 176356
class iron_docks_spell_lava_barrage_dummy : public SpellScriptLoader
{
public:

    iron_docks_spell_lava_barrage_dummy() : SpellScriptLoader("iron_docks_spell_lava_barrage_dummy") { }

    class iron_docks_spells : public SpellScript
    {
        PrepareSpellScript(iron_docks_spells);

        enum eLavaBarrageSpells
        {
            SpellLavaBarrageDummy = 176356,
            SpellLavaBarrageAreaTrigger = 173482,
            SpellLavaBarrageDoT = 173489
        };

        void HandleDummy(SpellEffIndex p_EffIndex)
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (l_Caster->IsAIEnabled())
                {
                    for (uint8 l_I = 0; l_I < 2; l_I++)
                    {
                        if (Unit* l_Target = l_Caster->GetAI()->SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                            l_Caster->CastSpell(l_Target, eLavaBarrageSpells::SpellLavaBarrageAreaTrigger, true);
                    }
                }
            }
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(iron_docks_spells::HandleDummy, SpellEffIndex::EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new iron_docks_spells();
    }
};

/// Burning Arrows - 164628
class iron_docks_area_trigger_burning_arrow : public AreaTriggerEntityScript
{
public:

    iron_docks_area_trigger_burning_arrow() : AreaTriggerEntityScript("iron_docks_area_trigger_burning_arrow") { }

    enum eBurningArrowSpells
    {
        SpellBurningArrowDummy = 172810,
        SpellBurningArrowAreaTrigger = 164234,
        SpellBurningArrowDoT = 164632
    };

    struct iron_docks_area_trigger_burning_arrow_AI : public AreaTriggerAI
    {
        explicit iron_docks_area_trigger_burning_arrow_AI(AreaTrigger* at) : AreaTriggerAI(at) {   }

        void OnRemove() override
        {
            for (auto & it : at->GetInsideUnits())
                if (Unit* ptr = ObjectAccessor::GetUnit(*at, it))
                    if (ptr && ptr->HasAura(eBurningArrowSpells::SpellBurningArrowDoT))
                        ptr->RemoveAura(eBurningArrowSpells::SpellBurningArrowDoT);
        }

        void OnUnitEnter(Unit* target) override
        {
            if (target && at->GetCaster()->IsValidAttackTarget(target))
                at->GetCaster()->CastSpell(target, eBurningArrowSpells::SpellBurningArrowDoT, true);
        }

        void OnUnitExit(Unit* target) override
        {
            if (target && target->HasAura(eBurningArrowSpells::SpellBurningArrowDoT))
                target->RemoveAurasDueToSpell(eBurningArrowSpells::SpellBurningArrowDoT);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* at) const override
    {
        return new iron_docks_area_trigger_burning_arrow_AI(at);
    }
};

/// Lava Barrage - 173484
class iron_docks_area_trigger_lava_barrage_effect : public AreaTriggerEntityScript
{
public:

    iron_docks_area_trigger_lava_barrage_effect() : AreaTriggerEntityScript("iron_docks_area_trigger_lava_barrage_effect") { }

    enum eBarrageLavaSpells
    {
        /// Ironwing Flamespitter
        SpellLavaBarrageDummy = 176356,
        SpellLavaBarrageAreaTrigger = 173482,
        SpellLavaBarrageDot = 173489
    };

    struct iron_docks_area_trigger_lava_barrage_effect_AI : public AreaTriggerAI
    {
        explicit iron_docks_area_trigger_lava_barrage_effect_AI(AreaTrigger* at) : AreaTriggerAI(at) {   }

        void OnRemove() override
        {
            for (auto & it : at->GetInsideUnits())
                if (Unit* ptr = ObjectAccessor::GetUnit(*at, it))
                    if (ptr && ptr->HasAura(eBarrageLavaSpells::SpellLavaBarrageDot))
                        ptr->RemoveAura(eBarrageLavaSpells::SpellLavaBarrageDot);
        }

        void OnUnitEnter(Unit* target) override
        {
            if (target && at->GetCaster()->IsValidAttackTarget(target))
                at->GetCaster()->CastSpell(target, SpellLavaBarrageDot, true);
        }

        void OnUnitExit(Unit* target) override
        {
            if (target && target->HasAura(SpellLavaBarrageDot))
                target->RemoveAurasDueToSpell(SpellLavaBarrageDot);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* at) const override
    {
        return new iron_docks_area_trigger_lava_barrage_effect_AI(at);
    }
};

/// Barbed Arrow Barrage - 164278
class iron_docks_area_trigger_barbed_arrow : public AreaTriggerEntityScript
{
public:

    iron_docks_area_trigger_barbed_arrow() : AreaTriggerEntityScript("iron_docks_area_trigger_barbed_arrow") { }

    enum eBarbedArrowSpells
    {
        SpellBarbedArrowAreaTrigger = 164278,
        SpellBarbedArrowAura = 164370,
        SpellBarbedArrowDot = 164648
    };

    struct iron_docks_area_trigger_barbed_arrow_AI : public AreaTriggerAI
    {
        explicit iron_docks_area_trigger_barbed_arrow_AI(AreaTrigger* at) : AreaTriggerAI(at) { }

        void OnRemove() override
        {
            for (auto & it : at->GetInsideUnits())
                if (Unit* ptr = ObjectAccessor::GetUnit(*at, it))
                    if (ptr && ptr->HasAura(eBarbedArrowSpells::SpellBarbedArrowDot))
                        ptr->RemoveAura(eBarbedArrowSpells::SpellBarbedArrowDot);
        }

        void OnUnitEnter(Unit* target) override
        {
            if (target && at->GetCaster()->IsValidAttackTarget(target))
                at->GetCaster()->CastSpell(target, eBarbedArrowSpells::SpellBarbedArrowDot, true);
        }

        void OnUnitExit(Unit* target) override
        {
            if (target && target->HasAura(eBarbedArrowSpells::SpellBarbedArrowDot))
                target->RemoveAurasDueToSpell(eBarbedArrowSpells::SpellBarbedArrowDot);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* at) const override
    {
        return new iron_docks_area_trigger_barbed_arrow_AI(at);
    }
};

/// Grease Vial - 172629 
class iron_docks_area_trigger_oil_effect : public AreaTriggerEntityScript
{
public:

    iron_docks_area_trigger_oil_effect() : AreaTriggerEntityScript("iron_docks_area_trigger_oil_effect") { }

    enum eGreaseVialSpells
    {
        SpellGreaseVial = 172649,
        SpellGreaseVialEffect = 172636,
        SpellKnockedDown = 172631
    };

    struct iron_docks_area_trigger_oil_effect_AI : public AreaTriggerAI
    {
        explicit iron_docks_area_trigger_oil_effect_AI(AreaTrigger* at) : AreaTriggerAI(at) {  }

        void OnRemove() override
        {
            for (auto & it : at->GetInsideUnits())
                if (Unit* ptr = ObjectAccessor::GetUnit(*at, it))
                    if (ptr && ptr->HasAura(eGreaseVialSpells::SpellGreaseVialEffect))
                        ptr->RemoveAura(eGreaseVialSpells::SpellGreaseVialEffect);
        }

        void OnUnitEnter(Unit* target) override
        {
            if (target && target->IsPlayer())
                at->GetCaster()->CastSpell(target, eGreaseVialSpells::SpellGreaseVialEffect, true);
        }

        void OnUnitExit(Unit* target) override
        {
            if (target && target->HasAura(eGreaseVialSpells::SpellGreaseVialEffect))
                target->RemoveAurasDueToSpell(eGreaseVialSpells::SpellGreaseVialEffect);
        }

        void OnUpdate(uint32 diff) override
        {
            _timerCheck += diff;

            if (_timerCheck >= 2 * TimeConstants::IN_MILLISECONDS)
            {
                _timerCheck = 0;
                for (auto & it : at->GetInsideUnits())
                    if (Player* ptr = ObjectAccessor::GetPlayer(*at, it))
                        if (ptr->HasUnitState(UnitState::UNIT_STATE_CASTING)) // Aura effect hardcoded
                            ptr->CastSpell(ptr, eGreaseVialSpells::SpellKnockedDown);
            }
        }
        uint32 _timerCheck;
    };

    AreaTriggerAI* GetAI(AreaTrigger* at) const override
    {
        return new iron_docks_area_trigger_oil_effect_AI(at);
    }
};

/// Jagget Caltrops - 173325
class iron_docks_area_trigger_jagged_caltrops : public AreaTriggerEntityScript
{
public:

    iron_docks_area_trigger_jagged_caltrops() : AreaTriggerEntityScript("iron_docks_area_trigger_jagged_caltrops") { }

    enum eCaltTrapsSpells
    {
        SpellCultTraps = 173336,
        SpellCultTrapsDamage = 173324
    };

    std::list<ObjectGuid> m_Targets;
    uint32 m_Timer = 1 * TimeConstants::IN_MILLISECONDS;

    struct iron_docks_area_trigger_jagged_caltrops_AI : public AreaTriggerAI
    {
        explicit iron_docks_area_trigger_jagged_caltrops_AI(AreaTrigger* at) : AreaTriggerAI(at) { }

        void OnRemove() override
        {
            for (auto & it : at->GetInsideUnits())
                if (Unit* ptr = ObjectAccessor::GetUnit(*at, it))
                    if (ptr && ptr->HasAura(eCaltTrapsSpells::SpellCultTrapsDamage))
                        ptr->RemoveAura(eCaltTrapsSpells::SpellCultTrapsDamage);
        }

        void OnUnitEnter(Unit* target) override
        {
            if (target && target->IsPlayer())
                at->GetCaster()->CastSpell(target, eCaltTrapsSpells::SpellCultTrapsDamage, true);
        }

        void OnUnitExit(Unit* target) override
        {
            if (target && target->HasAura(eCaltTrapsSpells::SpellCultTrapsDamage))
                target->RemoveAurasDueToSpell(eCaltTrapsSpells::SpellCultTrapsDamage);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* at) const override
    {
        return new iron_docks_area_trigger_jagged_caltrops_AI(at);
    }
};

/// Lava Blast - 173518 
class iron_docks_area_trigger_lava_blast : public AreaTriggerEntityScript
{
public:

    iron_docks_area_trigger_lava_blast() : AreaTriggerEntityScript("iron_docks_area_trigger_lava_blast") { }

    enum eCaltTrapsSpells
    {
        SpellLavaBlastDot = 173517
    };

    struct iron_docks_area_trigger_lava_blast_AI : public AreaTriggerAI
    {
        explicit iron_docks_area_trigger_lava_blast_AI(AreaTrigger* at) : AreaTriggerAI(at) {    }

        void OnRemove() override
        {
            for (auto & it : at->GetInsideUnits())
                if (Unit* ptr = ObjectAccessor::GetUnit(*at, it))
                    if (ptr && ptr->HasAura(eCaltTrapsSpells::SpellLavaBlastDot))
                        ptr->RemoveAura(eCaltTrapsSpells::SpellLavaBlastDot);
        }

        void OnUnitEnter(Unit* target) override
        {
            if (target && at->GetCaster()->IsValidAttackTarget(target))
                at->GetCaster()->CastSpell(target, eCaltTrapsSpells::SpellLavaBlastDot, true);
        }

        void OnUnitExit(Unit* target) override
        {
            if (target && target->HasAura(eCaltTrapsSpells::SpellLavaBlastDot))
                target->RemoveAurasDueToSpell(eCaltTrapsSpells::SpellLavaBlastDot);
        }
    };

    AreaTriggerAI* GetAI(AreaTrigger* at) const override
    {
        return new iron_docks_area_trigger_lava_blast_AI(at);
    }
};

void AddSC_iron_docks()
{
    new iron_docks_mob_gromkar_battlemaster(); /// 83025
    new iron_docks_mob_gromkar_deadeye(); /// 83028 / 83764
    new iron_docks_mob_gromkar_footsoldier(); /// 83765
    new iron_docks_mob_gromkar_incinerator(); /// 86809
    new iron_docks_mob_gromkar_technician(); /// 81432
    new iron_docks_mob_siege_master_olugar(); /// 83026
    new iron_docks_mob_champion_darona(); /// 81603
    new iron_docks_mob_gwarnok(); /// 84520
    new iron_docks_mob_iron_star_vehicle(); /// 81247
    new iron_docks_mob_laborer_ogron(); /// 83578
    new iron_docks_mob_gromkar_deck_hand(); /// 83762
    new iron_docks_mob_gromkar_technician_deckhand_leader(); /// 83763
    new iron_docks_mob_rampaging_clefthoof(); /// 83392
    new iron_docks_mob_ironwing_flamespitter(); /// 83389
    new iron_docks_mob_hammer_stalker(); /// 86534
    new iron_docks_mob_archery_target(); /// 79423
    new iron_docks_spell_flurry_periodic(); /// 178412
    new iron_docks_spell_charge_forward(); /// 167299
    new iron_docks_spell_barbed_arrow_aura(); /// 164370
    new iron_docks_spell_barbed_arrow_dummy(); /// 172810
    new iron_docks_spell_burning_arrow_aura(); /// 172810
    new iron_docks_spell_charging_slash_effect(); /// 172885
    new iron_docks_spell_lava_barrage_dummy(); /// 176356
    new iron_docks_spell_lava_blast(); /// 173514
    new iron_docks_spell_trampling_stampede(); /// 173384
    new iron_docks_area_trigger_burning_arrow();/// 177642 
    new iron_docks_area_trigger_lava_barrage_effect();///176358 
    new iron_docks_area_trigger_barbed_arrow(); ///164278 
    new iron_docks_area_trigger_oil_effect();///172629
    new iron_docks_area_trigger_jagged_caltrops();///173325 
    new iron_docks_area_trigger_lava_blast();///173518
}
