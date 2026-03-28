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
        NOT_ACTIVE,
        OPENING,
        DONE_OPENING,
        SELECTED,
        CLOSING,
    }UpgradeScreenState;

    typedef struct{
        bool active;

        float openingProgress[3];
        int currentOpening;

        Rectangle upgradeRecs[3];
        Rectangle baseUpgradeRecs[3];
        bool isHovering[3];
        Upgrades upgrade[3];
        int upgradeLevels[NUMBER_OF_UPGRADES];

        Upgrades selectedUpgrade;

        UpgradeScreenState state;
    }UpgradeScreen;


    void upgradeStructInit(UpgradeScreen *up);

    void createUpgrades(UpgradeScreen *up);

    void resetUpgradeUI(UpgradeScreen *up);

    void updateUpgradeScreen(UpgradeScreen *up, Vector2 mousePos);

    void drawUpgrades(UpgradeScreen *up, Font font);
    


#endif