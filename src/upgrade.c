#include "../headers/upgrade.h"
#include "stdio.h"
#include "../headers/textures.h"

Rectangle finalBaseRec = {290, 160, 700, 400};

void upgradeStructInit(UpgradeScreen *up){
    up->isActive = false;

    up->baseRec = (Rectangle){290, 160, 700, 400};
    up->openingProgress = 0.0f;  //from 0.0 - 1.0

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

void updateUpgradeScreen(UpgradeScreen *up, Vector2 mousePos){
    switch (up->state){
        case NOT_ACTIVE:
            break;
        case OPENING:
            up->openingProgress += 4.0f * GetFrameTime();
            if(up->openingProgress > 1.0f){
                up->openingProgress = 1.0f;
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

        case SELECTED:

        case CLOSING:

    }
}

void drawUpgrades(UpgradeScreen *up){

    if(up->state != NOT_ACTIVE){
        
        float w = up->baseRec.width * up->openingProgress;
        float h = up->baseRec.height * up->openingProgress;
        float cx = 290 + 700/2.0f;
        float cy = 160 + 400/2.0f;
        Rectangle animatedRec = {cx - w/2.0f, cy - h/2.0f, w, h};

        DrawTexturePro(upgradeScreenBaseTexture, (Rectangle){0,0, 112, 64}, animatedRec, (Vector2){0,0}, 0.0f, WHITE);

        if(up->state == DONE_OPENING){
            for(int i = 0; i < 3; i++){
                DrawTexturePro(upgradeTextures[up->upgrade[i]], (Rectangle){0,0,33,58}, up->upgradeRecs[i], (Vector2){0,0}, 0.0f, up->isHovering[i] ? (Color){254,231,97,255} : WHITE);
            }
        }
    }
    
    
}