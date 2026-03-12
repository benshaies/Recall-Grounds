#ifndef TEXTURES_H
#define TEXTURES_H
#include "raylib.h"

extern Texture2D playerIdleTexture, playerSideTexture, levelTilesetTexture, enemyIdleTexture, enemyHitTexture, cursorTexture, axeThrowTexture, axeBaseTexture;
extern Texture2D heartTexture, heartHalfTexture, playerHurtTexture, heartEmptyTexture, enemy2RunTexture, enemy2HitTexture;



void texturesLoad();

void texturesUnload();

#endif