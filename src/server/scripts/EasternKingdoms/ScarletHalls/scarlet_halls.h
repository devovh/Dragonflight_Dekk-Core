#ifndef DEF_SCARLET_H
#define DEF_SCARLET_H 

enum Data
{
    DATA_BRAUN = 0,
    DATA_HARLAN = 1,
    DATA_KOEGLER = 2,
    MAX_ENCOUNTER,
};

enum eCreatures
{
    NPC_HOUNDMASTER_BRAUN = 59303,
    NPC_ARMSMASTER_HARLAN = 58632,
    NPC_FLAMEWEAVER_KOEGLER = 59150,
    //Braun summons
    NPC_OBEDIENT_HOUND = 59309,
    //Koegler summons
    NPC_DRAGON_BREATH_TARGET = 59198,
};

enum eGameObjects
{
    GO_BRAUN_DOOR = 210097,
    GO_HARLAN_DOOR = 210480,
};
#endif
