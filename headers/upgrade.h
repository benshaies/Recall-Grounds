#ifndef UPGRADE_H
#define UPGRADE_H
#include <raylib.h>

#define NUMBER_OF_UPGRADES 7

    typedef enum{
        BIGGER_WEAPON,
        FASTER_WEAPON,
        DEFLECT_OFF_WALLS,
        INCREASED_DAMAGE,
        
        INCREASE_PLAYER_SPEED,
        GAIN_HEALTH,
        IMMUNE_WHILE_PULLING_IN,
    }Upgrades;

    typedef enum{
        OPENING,
        DONE_OPENING,
        SELECTED,
    }UpgradeScreenState;

    typedef struct{
        bool isActive;
        Rectangle baseRec;
        Rectangle upgradeRecs[3];
        Upgrades upgrade[3];
        int unavailableUpgrades[3];
        int upgradeLevels[NUMBER_OF_UPGRADES];

        UpgradeScreenState state;
    }UpgradeScreen;


    void resetUpgrades(UpgradeScreen *up);

    void createUpgrades(UpgradeScreen *up);

    void drawUpgrades(UpgradeScreen *up);
    


#endif