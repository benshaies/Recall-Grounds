#include "../headers/upgrade.h"
#include "stdio.h"

Rectangle finalBaseRec = {290, 160, 700, 400};

void resetUpgrades(UpgradeScreen *up){
    up->upgrade[0] = -1;
    up->upgrade[1] = -1;
    up->upgrade[2] = -1;

    up->unavailableUpgrades[0] = -1;
    up->unavailableUpgrades[1] = -1;
    up->unavailableUpgrades[2] = -1;
}

void createUpgrades(UpgradeScreen *up){
    int count = 0;
    while (count < 3){
        int ran = GetRandomValue(0,6);
        bool duplicate = false;
        for(int i = 0; i < count; i++){
            if(up->upgrade[i] == ran){duplicate = true; break;}
        }

        if(!duplicate) up->upgrade[count++] = ran;
    }
}

void drawUpgrades(UpgradeScreen *up){

}