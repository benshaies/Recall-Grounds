#ifndef TEXTURES_H
#define TEXTURES_H
#include "raylib.h"
#include "upgrade.h"

extern Texture2D playerIdleTexture, playerSideTexture, levelTilesetTexture,
    enemyOneRunTexture[3], enemyHitTexture, cursorTexture, axeThrowTexture,
    axeBaseTexture;
extern Texture2D heartTexture, heartHalfTexture, playerHurtTexture,
    heartEmptyTexture, enemy2RunTexture[3], enemy2HitTexture,
    enemyOneAttackTexture;
extern Texture2D upgradeScreenBaseTexture, playerImmuneTexture;
extern Texture2D upgradeTextures[NUMBER_OF_UPGRADES];
extern Texture2D enemy1RunVariery2Texture, gameOverTexture;
extern Texture2D playerDeadTexture;
extern Texture2D mainMenuTexture;

void texturesLoad();

void texturesUnload();

#endif
