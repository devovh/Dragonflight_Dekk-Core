/*
 * Copyright 2021 
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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "ObjectMgr.h"

//world quest 46236/[stonebound-soldiers]
struct npc_legionfall_soldier_119141 : public ScriptedAI
{
    npc_legionfall_soldier_119141(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool /*result*/)
    {
        if (Player* player = clicker->ToPlayer())
        {
            if (player->GetQuestStatus(46236) == QUEST_STATUS_INCOMPLETE)
            {
                me->RemoveAurasDueToSpell(236514);
                me->AI()->Talk(urand(0, 2));
                player->KilledMonsterCredit(me->GetEntry());
             //   me->GetMotionMaster()->MoveAwayAndDespawn(15.0f, 3000);
            }
        }
    }
};
//world quest 46821/[creepy-crawlies]
struct npc_bone_crawler_grub_116951 : public ScriptedAI
{
    npc_bone_crawler_grub_116951(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!who || !who->IsInWorld())
            return;
        if (!me->IsWithinDist(who, 3.0f, false))
            return;
        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;
        if (player->HasQuest(46821))
        {
            player->KilledMonsterCredit(me->GetEntry());
            me->CastSpell(me, 37989, true);
        }
    }
};
//121302 121305 121312 121313 , ,120076
//QUEST 46499/[spiders-huh?] 46501/[grave-robbin] 46509/[tomb-raidering] 46510/Ship Graveyard 46511/[were-treasure-hunters] 46666/[the-motherlode]
struct npc_treasure_master_iksreeged_121302 : public ScriptedAI
{
    npc_treasure_master_iksreeged_121302(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!me->IsWithinDist(who, 15.0f, false))
            return;
        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;
        ///HAVE MORE QUEST IN THE SAME CODE
        if ( player->HasQuest(46499)|| player->HasQuest(46501)|| player->HasQuest(46509) || player->HasQuest(46510))
        {
            //talk and runaway
            me->AI()->Talk(0);
            player->KilledMonsterCredit(me->GetEntry());
           // me->GetMotionMaster()->MoveAwayAndDespawn(-15.0f, 3000);
        }
    }
};
//QUEST 46245 Begin Construction
struct npc_legionfall_construction_table_119305 : public ScriptedAI
{
    npc_legionfall_construction_table_119305(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!who || !who->IsInWorld())
            return;
        if (!me->IsWithinDist(who, 15.0f, false))
            return;
        Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!player)
            return;
        if (player->HasQuest(46245) )
            player->ForceCompleteQuest(46245);
    }

};

/// Called by Light as a Feather - 240980
    /// Last Update 7.3.5 - Build 25996
class spell_broken_shore_light_as_a_feather : public SpellScriptLoader
{
public:
    spell_broken_shore_light_as_a_feather() : SpellScriptLoader("spell_broken_shore_light_as_a_feather") { }

    class spell_broken_shore_light_as_a_feather_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_broken_shore_light_as_a_feather_AuraScript);

        enum eSpells
        {
            LightAsAFeather = 241538
        };

        void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->IsMounted())
            {
                l_Caster->RemoveAura(eSpells::LightAsAFeather);
                return;
            }

            if (!l_Caster->HasAura(eSpells::LightAsAFeather))
                l_Caster->CastSpell(l_Caster, eSpells::LightAsAFeather, true);
        }

        void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::LightAsAFeather);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_broken_shore_light_as_a_feather_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_light_as_a_feather_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_broken_shore_light_as_a_feather_AuraScript();
    }
};

/// Called by Reinforced Reins - 240985
    /// Last Update 7.3.5 - Build 25996
class spell_broken_shore_reinforced_reins : public SpellScriptLoader
{
public:
    spell_broken_shore_reinforced_reins() : SpellScriptLoader("spell_broken_shore_reinforced_reins") { }

    class spell_broken_shore_reinforced_reins_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_broken_shore_reinforced_reins_AuraScript);

        enum eSpells
        {
            ReinforcedReins = 241540
        };

        void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->IsMounted())
            {
                l_Caster->RemoveAura(eSpells::ReinforcedReins);
                return;
            }

            if (!l_Caster->HasAura(eSpells::ReinforcedReins))
                l_Caster->CastSpell(l_Caster, eSpells::ReinforcedReins, true);
        }

        void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(eSpells::ReinforcedReins);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_broken_shore_reinforced_reins_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_reinforced_reins_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_broken_shore_reinforced_reins_AuraScript();
    }
};

/// Called by Unstable Tether - 243069
/// Last Update 7.3.5 - Build 25996
class spell_broken_shore_unstable_tether : public SpellScriptLoader
{
public:
    spell_broken_shore_unstable_tether() : SpellScriptLoader("spell_broken_shore_unstable_tether") { }

    class spell_broken_shore_unstable_tether_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_broken_shore_unstable_tether_AuraScript);

        void HandleAfterRemove(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Creature* l_Creature = l_Caster->ToCreature();
            if (!l_Creature)
                return;

            l_Creature->DespawnOrUnsummon(1s);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_broken_shore_unstable_tether_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_broken_shore_unstable_tether_AuraScript();
    }
};

/// Dancing Blade - 241861
   /// Last Update 7.3.5 - Build 25996
class spell_npc_dancing_blade : public CreatureScript
{
public:
    spell_npc_dancing_blade() : CreatureScript("npc_dancing_blade") { }

    struct spell_npc_dancing_bladeAI : public ScriptedAI
    {
        spell_npc_dancing_bladeAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            DancingBlade = 241860
        };

        void IsSummonedBy(WorldObject* p_Summoner) override
        {
            if (!p_Summoner)
                return;

            me->CastSpell(me, eSpells::DancingBlade, true);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const
    {
        return new spell_npc_dancing_bladeAI(p_Creature);
    }
};

// Called by Landslide - 241888
//Last Update 7.3.5 - Build 25996
class spell_broken_shore_landslide : public SpellScriptLoader
{
      public:
            spell_broken_shore_landslide() : SpellScriptLoader("spell_broken_shore_landslide") { }

            class spell_broken_shore_landslide_SpellScript : public SpellScript
            {
                PrepareSpellScript(spell_broken_shore_landslide_SpellScript);

                enum eSpells
                {
                    Landslide = 241889
                };

                void HandleAfterCast()
                {
                    Unit* l_Caster = GetCaster();
                    if (!l_Caster)
                        return;

                    for (uint8 l_I = 0; l_I < 25; l_I++)
                    {
                        Position l_Pos;
                    //    l_Caster->SimplePosXYRelocationByAngle(l_Pos, frand(0.0f, 25.0f), frand(- M_PI / 6.0f, M_PI / 6.0f));
                        l_Caster->CastSpell(l_Pos, eSpells::Landslide, true);
                    }
                }

                void Register() override
                {
                    AfterCast += SpellCastFn(spell_broken_shore_landslide_SpellScript::HandleAfterCast);
                }
            };

            SpellScript* GetSpellScript() const override
            {
                return new spell_broken_shore_landslide_SpellScript();
            }
    };

/// Called by Ancient Manashards - 238541
/// Last Update 7.3.5 - Build 26365
class spell_broken_shore_ancient_manashards : public SpellScriptLoader
{
public:
    spell_broken_shore_ancient_manashards() : SpellScriptLoader("spell_broken_shore_ancient_manashards") { }

    class spell_broken_shore_ancient_manashards_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_broken_shore_ancient_manashards_SpellScript);

        enum eSpells
        {
            AncientManashards = 238542
        };

        void HandleAfterHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            l_Caster->CastSpell(l_Target, eSpells::AncientManashards, true);
        }

        void FilterTargets(std::list<WorldObject*>& p_Targets)
        {
            if (p_Targets.empty())
                return;

            Trinity::Containers::RandomResize(p_Targets, 1);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_broken_shore_ancient_manashards_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            AfterHit += SpellHitFn(spell_broken_shore_ancient_manashards_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_broken_shore_ancient_manashards_SpellScript();
    }
};

void AddSC_broken_shore()
{
    RegisterCreatureAI(npc_legionfall_soldier_119141);
    RegisterCreatureAI(npc_bone_crawler_grub_116951);
    RegisterCreatureAI(npc_treasure_master_iksreeged_121302);
    RegisterCreatureAI(npc_legionfall_construction_table_119305);

    new spell_broken_shore_light_as_a_feather();
    new spell_broken_shore_reinforced_reins();
    new spell_broken_shore_unstable_tether();
    new spell_npc_dancing_blade();
    new spell_broken_shore_landslide();
}
