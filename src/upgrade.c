#include "../headers/upgrade.h"
#include "stdio.h"
#include "../headers/textures.h"
#include <math.h>

Rectangle finalBaseRec = {290, 160, 700, 400};
float openingSpeed = 2.0f;

void upgradeStructInit(UpgradeScreen *up){

    up->openingProgress[0] = 0.0f;  //from 0.0 - 1.0
    up->openingProgress[1] = 0.0f;
    up->openingProgress[2] = 0.0f;
    up->currentOpening = 0;

    up->upgradeRecs[0] = (Rectangle){310, 180, 206, 360};
    up->upgradeRecs[1] = (Rectangle){536, 180, 206, 360};
    up->upgradeRecs[2] = (Rectangle){762, 180, 206, 360};
    up->baseUpgradeRecs[0] = (Rectangle){310, 180, 206, 360};
    up->baseUpgradeRecs[1] = (Rectangle){536, 180, 206, 360};
    up->baseUpgradeRecs[2] = (Rectangle){762, 180, 206, 360};

    up->isHovering[0] = false;
    up->isHovering[1] = false;
    up->isHovering[2] = false;


    for(int i = 0; i < NUMBER_OF_UPGRADES; i++){
        up->upgradeLevels[i] = 0;
    }

    up->state = NOT_ACTIVE;

    up->selectedUpgrade = -1;
    
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

void resetUpgradeUI(UpgradeScreen *up){
    for(int i = 0; i < 3; i++){
        up->openingProgress[i] = 0.0f;
        up->isHovering[i] = false;
        up->upgradeRecs[i] = up->baseUpgradeRecs[i];
    }

    up->currentOpening = 0;
    up->selectedUpgrade = -1;
    up->state = OPENING;
}

void updateUpgradeScreen(UpgradeScreen *up, Vector2 mousePos){
    switch (up->state){
        case NOT_ACTIVE:
            break;
        case OPENING:
            
            if(up->currentOpening < 3){
                up->openingProgress[up->currentOpening] += openingSpeed * GetFrameTime();
                if(up->openingProgress[up->currentOpening] >= 1.0f){
                    up->openingProgress[up->currentOpening] = 1.0f;
                    up->currentOpening++;
                }
                
            }

            if(up->currentOpening >= 3){
                up->state = DONE_OPENING;
            }
            break;
        case DONE_OPENING:



            for(int i = 0; i < 3; i++){
                if(CheckCollisionPointRec(mousePos, up->baseUpgradeRecs[i])){
                    up->isHovering[i] = true;

                    //Make upgrade rec bigger
                    up->upgradeRecs[i] = (Rectangle){
                        .x = up->baseUpgradeRecs[i].x - 20,
                        .y = up->baseUpgradeRecs[i].y - 20,
                        .height = up->baseUpgradeRecs[i].height + 40,
                        .width = up->baseUpgradeRecs[i].width + 40,
                    };
                    
                    //Player selected the upgrade
                    if(up->isHovering[i] && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        up->selectedUpgrade = up->upgrade[i];
                        up->state = SELECTED;
                        break;
                    }
                }
                else{
                    up->isHovering[i] = false;
                    //Reset the rec back
                    up->upgradeRecs[i] = (Rectangle){
                        .x = up->baseUpgradeRecs[i].x,
                        .y = up->baseUpgradeRecs[i].y,
                        .height = up->baseUpgradeRecs[i].height,
                        .width = up->baseUpgradeRecs[i].width,
                    };
                    
                    
                }
            }
            break;
        case SELECTED:
            up->state = NOT_ACTIVE;
            break;
        

    }
}

void drawUpgrades(UpgradeScreen *up, Font font){

    if(up->state != NOT_ACTIVE){

        float pulse = (sinf(GetTime() * 2.0f) + 1.0f) / 2.0f;
        float borderThickness = 2.0f + pulse * 4.0f;// oscillates between 2 and 6 (

        for(int i = 0; i < 3; i++){
             // Color){254,231,97,255}
            DrawTexturePro(upgradeTextures[up->upgrade[i]], (Rectangle){0,0,33,58}, up->upgradeRecs[i], (Vector2){0,0}, 0.0f, Fade((up->isHovering[i] ? WHITE : WHITE), up->openingProgress[i]));
            DrawRectangleLinesEx(up->upgradeRecs[i], borderThickness, Fade(up->isHovering[i]? WHITE: (Color){162, 38, 51}, up->openingProgress[i]));
        }


    }
    
    
}