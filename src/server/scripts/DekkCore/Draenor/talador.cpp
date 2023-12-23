////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2015 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////


/// Warlock Soulstealer - 79482
class npc_talador_warlock_soulstealer : public CreatureScript
{
    enum
    {
        WarlockSoulstealer              = 79482,
        DraeneiSpirit                   = 79478,
        KillCredit_RescueDraeneiSpirits = 79499,
        TormentedSoul                   = 160508
    };

    public:
        npc_talador_warlock_soulstealer() : CreatureScript("npc_talador_warlock_soulstealer") { }

        struct npc_talador_warlock_soulstealerAI : public ScriptedAI
        {
            npc_talador_warlock_soulstealerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            /// Called when the creature is killed
            void JustDied(Unit* p_Killer) override
            {
                Player* l_PlayerKiller = nullptr;

                if (p_Killer->IsPlayer())
                    l_PlayerKiller = p_Killer->ToPlayer();

                if (l_PlayerKiller)
                {
                    std::list<Unit*> l_NearCreatures;
                   
                    for (Unit* l_Unit : l_NearCreatures)
                    {
                        if (l_Unit->HasAura(TormentedSoul))
                        {
                            l_Unit->RemoveAura(TormentedSoul);
                            l_Unit->GetMotionMaster()->MoveFleeing(me, 5s);
                            l_Unit->ToCreature()->DespawnOrUnsummon(5s);
                            l_PlayerKiller->KilledMonsterCredit(KillCredit_RescueDraeneiSpirits);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_talador_warlock_soulstealerAI(p_Creature);
        }
};

/// Zorka's Void Gate - 79520
class npc_talador_zorkas_void_gate : public CreatureScript
{
    enum
    {
        BonusObjectiveZorkraFall = 34660,
        ZorkaVoidGate = 79520
    };

    public:
        npc_talador_zorkas_void_gate() : CreatureScript("npc_talador_zorkas_void_gate") { }

        struct npc_talador_zorkas_void_gateAI : public ScriptedAI
        {
            npc_talador_zorkas_void_gateAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void OnSpellClick(Unit* clicker, bool result) override
            {
                if (clicker->IsPlayer() && clicker->ToPlayer()->HasQuest(BonusObjectiveZorkraFall))
                {
                    clicker->ToPlayer()->KilledMonsterCredit(ZorkaVoidGate);
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_talador_zorkas_void_gateAI(p_Creature);
        }
};

void AddSC_talador()
{
    /// Npcs
    new npc_talador_warlock_soulstealer();
    new npc_talador_zorkas_void_gate();
}
