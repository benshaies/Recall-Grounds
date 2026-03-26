#include "../headers/textures.h"

Texture2D playerIdleTexture, playerSideTexture, levelTilesetTexture, playerHurtTexture;
Texture2D enemyIdleTexture, enemyHitTexture, enemy2RunTexture, enemy2HitTexture;
Texture2D cursorTexture;
Texture2D axeThrowTexture, axeBaseTexture;
Texture2D heartTexture, heartHalfTexture, heartEmptyTexture;
Texture2D upgradeScreenBaseTexture;
Texture2D upgradeTextures[NUMBER_OF_UPGRADES];

void texturesLoad(){
    playerIdleTexture = LoadTexture("../assets/playerIdle.png");
    playerSideTexture = LoadTexture("../assets/playerSide.png");
    levelTilesetTexture = LoadTexture("../assets/levelTileset.png");

    enemyIdleTexture = LoadTexture("../assets/enemyRunt.png");
    enemyHitTexture = LoadTexture("../assets/enemyHit.png");
    enemy2RunTexture = LoadTexture("../assets/enemy2Run.png");
    enemy2HitTexture = LoadTexture("../assets/enemy2Hit.png");

    cursorTexture = LoadTexture("../assets/cursor.png");

    axeThrowTexture = LoadTexture("../assets/axeThrow.png");
    axeBaseTexture = LoadTexture("../assets/anchorBase.png");

    heartTexture = LoadTexture("../assets/heart.png");
    heartHalfTexture = LoadTexture("../assets/heartHalf.png");
    heartEmptyTexture = LoadTexture("../assets/heartEmpty.png");

    playerHurtTexture = LoadTexture("../assets/playerHurt.png");

    upgradeScreenBaseTexture = LoadTexture("../assets/upgradeScreenBase.png");

    upgradeTextures[0] = LoadTexture("../assets/upgradeOne.png");
    upgradeTextures[1] = LoadTexture("../assets/upgradeTwo.png");
    upgradeTextures[2] = LoadTexture("../assets/upgradeThree.png");

    SetTextureFilter(levelTilesetTexture, TEXTURE_FILTER_POINT);

}

void texturesUnload(){
    UnloadTexture(playerIdleTexture);
    UnloadTexture(playerSideTexture);
    UnloadTexture(levelTilesetTexture);

    UnloadTexture(enemyIdleTexture);
    UnloadTexture(enemyHitTexture);
    UnloadTexture(enemy2RunTexture);
    UnloadTexture(enemy2HitTexture);

    UnloadTexture(cursorTexture);

    UnloadTexture(axeThrowTexture);
    UnloadTexture(axeBaseTexture);

    UnloadTexture(heartTexture);
    UnloadTexture(heartHalfTexture);
    UnloadTexture(heartEmptyTexture);

    UnloadTexture(playerHurtTexture);

    UnloadTexture(upgradeScreenBaseTexture);
}