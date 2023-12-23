////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////


/// Tarlna the Ageless - 81535
class boss_tarlna_the_ageless : public CreatureScript
{
    public:
        boss_tarlna_the_ageless() : CreatureScript("boss_tarlna_the_ageless") { }

        enum eDatas
        {
            MainHandEquipID         = 118056,
            MaxGiantLasherSpawns    = 15,
            HealthScalingCoeff      = 10,
            DamageScalingCoeff      = 1
        };

        enum eSpells
        {
            SouthshoreMobScalingAura    = 169704,
            SpellColossalBlow           = 175973,
            SpellSavageVines            = 176001,   ///< Trigger 176004 after 8s
            SpellGrowUntamedMandragora  = 176013,   ///< Missile 176014 on random destination
            SpellGenesisMissile         = 175975,   ///< Missile 175980 on random destination (summon Giant Lasher)
            SpellGenesis                = 175979    ///< Trigger 175978 after 6s -> SPELL_EFFECT_226 on TARGET_UNIT_SRC_AREA_ENTRY miscA = 422409
        };

        enum eEvents
        {
            EventColossalBlow = 1,
            EventSavageVines,
            EventGrowUntamedMandragora,
            EventGenesis
        };

        struct boss_tarlna_the_agelessAI : public ScriptedAI
        {
            boss_tarlna_the_agelessAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset()
            {
                m_Events.Reset();

                me->RemoveAura(eSpells::SouthshoreMobScalingAura);

                summons.DespawnAll();

                SetEquipmentSlots(false, eDatas::MainHandEquipID);
            }

            void JustDied(Unit* /*p_Killer*/)
            {
                summons.DespawnAll();
            }

            void EnterCombat(Unit*)
            {
                me->CastSpell(me, eSpells::SouthshoreMobScalingAura, true);
                m_Events.ScheduleEvent(eEvents::EventColossalBlow, 14s);
                m_Events.ScheduleEvent(eEvents::EventSavageVines, 7s);
                m_Events.ScheduleEvent(eEvents::EventGrowUntamedMandragora, 18s);
                m_Events.ScheduleEvent(eEvents::EventGenesis, 20s);
            }

            void UpdateAI(uint32 const p_Diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 100.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventColossalBlow:
                    {
                        if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random))
                        {
                            me->SetFacingTo(me->GetAbsoluteAngle(l_Target));
                            me->CastSpell(l_Target, eSpells::SpellColossalBlow, false);
                        }

                        m_Events.ScheduleEvent(eEvents::EventColossalBlow, 60s);
                        break;
                    }
                    case eEvents::EventSavageVines:
                    {
                        if (Unit* l_Target = SelectTarget(SelectTargetMethod::MaxThreat))
                        {
                            me->SetFacingTo(me->GetAbsoluteAngle(l_Target));
                            me->CastSpell(l_Target, eSpells::SpellSavageVines, false);
                        }

                        m_Events.ScheduleEvent(eEvents::EventSavageVines, 20s);
                        break;
                    }
                    case eEvents::EventGrowUntamedMandragora:
                    {
                        me->CastSpell(me, eSpells::SpellGrowUntamedMandragora, true);
                        m_Events.ScheduleEvent(eEvents::EventGrowUntamedMandragora, 30s);
                        break;
                    }
                    case eEvents::EventGenesis:
                    {
                        SummonGiantLashers(eDatas::MaxGiantLasherSpawns);

                        if (Unit* l_Target = SelectTarget(SelectTargetMethod::MaxThreat))
                        {
                            me->SetFacingTo(me->GetAbsoluteAngle(l_Target));
                            me->CastSpell(l_Target, eSpells::SpellGenesis, false);
                        }

                        m_Events.ScheduleEvent(eEvents::EventGenesis, 45s);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void SummonGiantLashers(uint32 p_Count)
            {
                for (uint32 l_I = 0; l_I < p_Count; ++l_I)
                    me->CastSpell(me, eSpells::SpellGenesisMissile, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_tarlna_the_agelessAI(p_Creature);
        }
};

/// Drov the Ruiner - 81252
class boss_drov_the_ruiner : public CreatureScript
{
    public:
        boss_drov_the_ruiner() : CreatureScript("boss_drov_the_ruiner") { }

        enum eDatas
        {
            HealthScalingCoeff  = 10,
            DamageScalingCoeff  = 1,
            RumblingGoren       = 88106,
            FrenziedGoren       = 88119
        };

        enum eSpells
        {
            SouthshoreMobScalingAura    = 169704,
            SpellColossalSlam           = 175791,   ///< Damaging in front cone
            SpellGigaSmash              = 175953,   ///< Damaging all ennemies
            SpellCallOfEarth            = 175827,   ///< Periodic trigger 172911 -> launchs 175835 missile (summon and AoE damage)
            GorenEmergeSearcher         = 175911,   ///< Triggers 175915
            GorenEmerge                 = 175912
        };

        enum eEvents
        {
            EventColossalSlam = 1,
            EventCallOfEarth,
            EventGigaSmash
        };

        enum eAction
        {
            ActionGorenSubmerge
        };

        struct boss_drov_the_ruinerAI : public ScriptedAI
        {
            boss_drov_the_ruinerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            std::list<ObjectGuid> m_GorenList;

            void Reset() override
            {
                m_Events.Reset();

                summons.DespawnAll();
                m_GorenList.clear();

                me->RemoveAura(eSpells::SouthshoreMobScalingAura);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                summons.DespawnAll();
                m_GorenList.clear();
            }

            void JustSummoned(Creature* p_Summoned) override
            {
                summons.Summon(p_Summoned);

                if (p_Summoned->GetEntry() == eDatas::RumblingGoren)
                    m_GorenList.emplace_back(p_Summoned->GetGUID());
                else if (p_Summoned->GetEntry() == eDatas::FrenziedGoren)
                {
                    if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random))
                        p_Summoned->AI()->AttackStart(l_Target);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                me->CastSpell(me, eSpells::SouthshoreMobScalingAura, true);
                m_Events.ScheduleEvent(eEvents::EventColossalSlam, 6s);
                m_Events.ScheduleEvent(eEvents::EventCallOfEarth, 18s);
                m_Events.ScheduleEvent(eEvents::EventGigaSmash, 13s);
            }

            void SpellHitTarget(WorldObject* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::GorenEmergeSearcher)
                    me->CastSpell(p_Target, eSpells::GorenEmerge, true);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionGorenSubmerge)
                {
                    me->CastSpell(me, eSpells::GorenEmergeSearcher, true);

                    for (ObjectGuid l_Guid : m_GorenList)
                    {
                        if (Creature* l_Goren = Creature::GetCreature(*me, l_Guid))
                            l_Goren->AI()->DoAction(eAction::ActionGorenSubmerge);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 100.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventColossalSlam:
                        if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random))
                            me->SetFacingTo(me->GetAbsoluteAngle(l_Target));
                        me->CastSpell(me, eSpells::SpellColossalSlam, false);
                        m_Events.ScheduleEvent(eEvents::EventColossalSlam, 20s);
                        break;
                    case eEvents::EventCallOfEarth:
                        me->CastSpell(me, eSpells::SpellCallOfEarth, false);
                        m_Events.ScheduleEvent(eEvents::EventCallOfEarth, 90s);
                        break;
                    case eEvents::EventGigaSmash:
                        me->CastSpell(me, eSpells::SpellGigaSmash, true);
                        m_Events.ScheduleEvent(eEvents::EventGigaSmash, 25s);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
         
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_drov_the_ruinerAI(p_Creature);
        }
};

/// Untamed Mandragora - 88150
class npc_untamed_mandragora : public CreatureScript
{
    public:
        npc_untamed_mandragora() : CreatureScript("npc_untamed_mandragora") { }

        enum eSpells
        {
            SpellNoxiousSpitAT  = 176035
        };

        enum eEvents
        {
            EventNoxiousSpit = 1
        };

        struct npc_untamed_mandragoraAI : public CreatureAI
        {
            npc_untamed_mandragoraAI(Creature* p_Creature) : CreatureAI(p_Creature) { }

            EventMap m_Events;

            void Reset()
            {
                m_Events.Reset();

            }

            void EnterCombat(Unit* /*p_Attacker*/)
            {
                m_Events.ScheduleEvent(eEvents::EventNoxiousSpit, 8s);
            }

            void UpdateAI(const uint32 p_Diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventNoxiousSpit:
                        if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random))
                            me->CastSpell(l_Target, eSpells::SpellNoxiousSpitAT, true);
                        m_Events.ScheduleEvent(eEvents::EventNoxiousSpit, 8s);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_untamed_mandragoraAI(p_Creature);
        }
};

/// Giant Lasher - 88142
class npc_giant_lasher : public CreatureScript
{
    public:
        npc_giant_lasher() : CreatureScript("npc_giant_lasher") { }

        enum eSpells
        {
            SpellGenesisAura    = 175977,  ///< SPELL_AURA_395 -> AreaTrigger
            SpellGenesisAwake   = 175978,
            SpellSubmerged      = 175123,
            SpellLasherVenom    = 176000
        };

        struct npc_giant_lasherAI : public CreatureAI
        {
            npc_giant_lasherAI(Creature* p_Creature) : CreatureAI(p_Creature)
            {
                m_CheckPlayerTimer = 0;
            }

            uint32 m_CheckPlayerTimer;

            void Reset()
            {
                m_CheckPlayerTimer = 500;

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->CastSpell(me, eSpells::SpellGenesisAura, true);
                me->CastSpell(me, eSpells::SpellSubmerged, true);
            }

            //void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo)
            //{
            //    if (p_SpellInfo->Id == eSpells::SpellGenesisAwake)
            //    {
            //        m_CheckPlayerTimer = 0;

            //        me->SetReactState(ReactStates::REACT_AGGRESSIVE);
            //        me->RemoveAura(eSpells::SpellGenesisAura);
            //        me->RemoveAura(eSpells::SpellSubmerged);
            //    }
            //}

            void UpdateAI(uint32 const p_Diff)
            {
                if (me->HasAura(eSpells::SpellGenesisAura))
                {
                    CheckNearPlayers(p_Diff);
                    return;
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                /// No melee attack
                if (Unit* l_Target = SelectTarget(SelectTargetMethod::Random))
                    me->CastSpell(l_Target, eSpells::SpellLasherVenom, false);
            }

            void CheckNearPlayers(uint32 const p_Diff)
            {
                if (!m_CheckPlayerTimer)
                    return;

                if (m_CheckPlayerTimer <= p_Diff)
                {
                    if (Player* l_Player = me->SelectNearestPlayer(5.0f))
                        me->DespawnOrUnsummon();

                    m_CheckPlayerTimer = 500;
                }
                else
                    m_CheckPlayerTimer -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_giant_lasherAI(p_Creature);
        }
};

/// Rumbling Goren - 88106
class npc_drov_rumbling_goren : public CreatureScript
{
    public:
        npc_drov_rumbling_goren() : CreatureScript("npc_drov_rumbling_goren") { }

        enum eAction
        {
            ActionGorenSubmerge
        };

        enum eSpells
        {
            GrownGorenSubmerge  = 175258,
            ShreddingCharge     = 175923,
            CrushingChargeDmg   = 175920
        };

        enum eEvents
        {
            EventCheckPlayer = 1,
            EventMove
        };

        struct npc_drov_rumbling_gorenAI : public CreatureAI
        {
            npc_drov_rumbling_gorenAI(Creature* p_Creature) : CreatureAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->CastSpell(me, eSpells::ShreddingCharge, true);

                me->SetSpeed(MOVE_RUN, 4.0f);
                me->SetSpeed(MOVE_WALK, 4.0f);

                m_Events.ScheduleEvent(eEvents::EventCheckPlayer, 1s);
                m_Events.ScheduleEvent(eEvents::EventMove, 1s);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionGorenSubmerge)
                {
                    me->GetMotionMaster()->Clear();
                    me->RemoveAura(eSpells::ShreddingCharge);
                    me->CastSpell(me, eSpells::GrownGorenSubmerge, false);
                    me->DespawnOrUnsummon(8s);
                    m_Events.CancelEvent(eEvents::EventCheckPlayer);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventCheckPlayer:
                    {
                        std::list<Unit*> l_TargetList;
                        float l_Radius = 4.0f;

                        for (Unit* l_Unit : l_TargetList)
                            me->CastSpell(l_Unit, eSpells::CrushingChargeDmg, true);

                        m_Events.ScheduleEvent(eEvents::EventCheckPlayer, 1s);
                        break;
                    }
                    case eEvents::EventMove:
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveRandom(50.0f);
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_drov_rumbling_gorenAI(p_Creature);
        }
};

/// Frenzied Rumbler - 88119
class npc_drov_frenzied_rumbler : public CreatureScript
{
    public:
        npc_drov_frenzied_rumbler() : CreatureScript("npc_drov_frenzied_rumbler") { }

        enum eSpell
        {
            SpellAcidBreath = 175915
        };

        enum eEvent
        {
            EventAcidBreath = 1
        };

        struct npc_drov_frenzied_rumblerAI : public CreatureAI
        {
            npc_drov_frenzied_rumblerAI(Creature* p_Creature) : CreatureAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void JustEngagedWith(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(eEvent::EventAcidBreath, 5s);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventAcidBreath:
                        if (Unit* l_Target = SelectTarget(SelectTargetMethod::MaxThreat))
                            me->CastSpell(l_Target, eSpell::SpellAcidBreath, true);
                        m_Events.ScheduleEvent(eEvent::EventAcidBreath, 7s);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_drov_frenzied_rumblerAI(p_Creature);
        }
};

/// Call of Earth - 175827
class spell_drov_call_of_earth : public SpellScriptLoader
{
    public:
        spell_drov_call_of_earth() : SpellScriptLoader("spell_drov_call_of_earth") { }

        class spell_drov_call_of_earth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_drov_call_of_earth_AuraScript);

            enum eAction
            {
                ActionGorenSubmerge
            };

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (GetDuration() <= (GetMaxDuration() / 2))
                    return;
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Drov = GetCaster()->ToCreature())
                {
                    if (l_Drov->AI())
                        l_Drov->AI()->DoAction(eAction::ActionGorenSubmerge);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_drov_call_of_earth_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_drov_call_of_earth_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_drov_call_of_earth_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_drov_call_of_earth_AuraScript();
        }
};

/// Colossal Slam - 175791
class spell_drov_colossal_slam : public SpellScriptLoader
{
    public:
        spell_drov_colossal_slam() : SpellScriptLoader("spell_drov_colossal_slam") { }

        class spell_drov_colossal_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_drov_colossal_slam_SpellScript);

            enum eSpell
            {
                ColossalSlamAoE = 175793,
                TargetRestrict  = 24228
            };

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                if (l_Restriction == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;
           
            }

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                    l_Caster->CastSpell(l_Caster, eSpell::ColossalSlamAoE, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_drov_colossal_slam_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_drov_colossal_slam_SpellScript();
        }
};

/// Punt Podling - 174732
class spell_quest_gorgrond_punt_podling : public SpellScriptLoader
{
    enum
    {
        ThornyLeafling      = 85809,
        PodlingPuntCredit   = 85815
    };

    public:
        /// Constructor
        spell_quest_gorgrond_punt_podling() : SpellScriptLoader("spell_quest_gorgrond_punt_podling") { }

        class spell_quest_gorgrond_punt_podling_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_quest_gorgrond_punt_podling_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Caster && l_Target && l_Caster->IsPlayer())
                {
                    if (l_Target->GetEntry() == ThornyLeafling)
                    {
                        l_Caster->ToPlayer()->KilledMonsterCredit(PodlingPuntCredit);
                        l_Target->ToCreature()->DespawnOrUnsummon(3s);
                    }
                }
            }

            /// Register all effect
            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_quest_gorgrond_punt_podling_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        /// Get spell script
        SpellScript* GetSpellScript() const override
        {
            return new spell_quest_gorgrond_punt_podling_SpellScript();
        }
};

/// Toxic Slimemold - 85732
class npc_gorgrond_toxic_slimemold : public CreatureScript
{
    public:
        npc_gorgrond_toxic_slimemold() : CreatureScript("npc_gorgrond_toxic_slimemold") { }

        struct npc_gorgrond_toxic_slimemoldAI : public CreatureAI
        {
            uint32 m_Timer;

            npc_gorgrond_toxic_slimemoldAI(Creature* p_Creature) : CreatureAI(p_Creature), m_Timer(0) { }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Timer += p_Diff;

                if (m_Timer >= 500)
                {
                    Player* l_Player = me->FindNearestPlayer(MIN_MELEE_REACH);

                    if (l_Player)
                    {
                        l_Player->KilledMonsterCredit(me->GetEntry());
                        me->DespawnOrUnsummon();
                    }

                    m_Timer = 0;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_gorgrond_toxic_slimemoldAI(p_Creature);
        }
};

/// Showing "Mercy" - 170792
class spell_quest_gorgrond_showing_mercy_peon : public SpellScriptLoader
{
    enum
    {
        Grom_karPeon = 85540,
    };

    public:
        /// Constructor
        spell_quest_gorgrond_showing_mercy_peon() : SpellScriptLoader("spell_quest_gorgrond_showing_mercy_peon") { }

        class spell_quest_gorgrond_showing_mercy_peon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_quest_gorgrond_showing_mercy_peon_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                if (l_Caster && l_Target && l_Caster->IsPlayer())
                {
                    if (l_Target->GetEntry() == Grom_karPeon)
                        l_Target->ToCreature()->DespawnOrUnsummon();
                }
            }

            /// Register all effect
            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_quest_gorgrond_showing_mercy_peon_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        /// Get spell script
        SpellScript* GetSpellScript() const override
        {
            return new spell_quest_gorgrond_showing_mercy_peon_SpellScript();
        }
};

void AddSC_gorgrond()
{
    /// Bosses
    new boss_tarlna_the_ageless();
    new boss_drov_the_ruiner();

    /// Npcs
    new npc_untamed_mandragora();
    new npc_giant_lasher();
    new npc_drov_rumbling_goren();
    new npc_drov_frenzied_rumbler();
    new npc_gorgrond_toxic_slimemold();

    /// Spells
    new spell_drov_call_of_earth();
    new spell_drov_colossal_slam();
    new spell_quest_gorgrond_punt_podling();
    new spell_quest_gorgrond_showing_mercy_peon();
}

