#include "../headers/textures.h"
#include "raylib.h"

Texture2D playerIdleTexture, playerSideTexture, levelTilesetTexture,
    playerHurtTexture, playerImmuneTexture, playerDeadTexture;
Texture2D enemyOneRunTexture[3], enemyHitTexture, enemy2RunTexture[3],
    enemy2HitTexture;
Texture2D cursorTexture;
Texture2D axeThrowTexture, axeBaseTexture;
Texture2D heartTexture, heartHalfTexture, heartEmptyTexture;
Texture2D upgradeScreenBaseTexture;
Texture2D upgradeTextures[NUMBER_OF_UPGRADES];
Texture2D gameOverTexture;

void texturesLoad() {
  playerIdleTexture = LoadTexture("../assets/playerIdle.png");
  playerSideTexture = LoadTexture("../assets/playerSide.png");
  levelTilesetTexture = LoadTexture("../assets/levelTileset.png");

  enemyOneRunTexture[0] = LoadTexture("../assets/enemyRunt.png");
  enemyOneRunTexture[1] = LoadTexture("../assets/enemyRunType2.png");
  enemyOneRunTexture[2] = LoadTexture("../assets/enemyOneRunVariety3.png");

  enemyHitTexture = LoadTexture("../assets/enemyHit.png");

  enemy2RunTexture[0] = LoadTexture("../assets/enemy2Run.png");
  enemy2RunTexture[1] = LoadTexture("../assets/enemy2RunType2.png");
  enemy2RunTexture[2] = LoadTexture("../assets/enemy2RunType3.png");

  enemy2HitTexture = LoadTexture("../assets/enemy2Hit.png");

  cursorTexture = LoadTexture("../assets/cursor.png");

  axeThrowTexture = LoadTexture("../assets/axeThrow.png");
  axeBaseTexture = LoadTexture("../assets/anchorBase.png");

  heartTexture = LoadTexture("../assets/heart.png");
  heartHalfTexture = LoadTexture("../assets/heartHalf.png");
  heartEmptyTexture = LoadTexture("../assets/heartEmpty.png");

  playerHurtTexture = LoadTexture("../assets/playerHurt.png");
  playerImmuneTexture = LoadTexture("../assets/playerImmune.png");
  playerDeadTexture = LoadTexture("../assets/playerDead.png");

  upgradeScreenBaseTexture = LoadTexture("../assets/upgradeScreenBase.png");

  upgradeTextures[0] = LoadTexture("../assets/upgradeOne.png");
  upgradeTextures[1] = LoadTexture("../assets/upgradeTwo.png");
  upgradeTextures[2] = LoadTexture("../assets/upgradeThree.png");
  upgradeTextures[3] = LoadTexture("../assets/upgradeFour.png");
  upgradeTextures[4] = LoadTexture("../assets/upgradeFive.png");
  upgradeTextures[5] = LoadTexture("../assets/upgradeSix.png");

  gameOverTexture = LoadTexture("../assets/gameOver.png");

  SetTextureFilter(levelTilesetTexture, TEXTURE_FILTER_POINT);
}

void texturesUnload() {
  UnloadTexture(playerIdleTexture);
  UnloadTexture(playerSideTexture);
  UnloadTexture(levelTilesetTexture);

  UnloadTexture(enemyOneRunTexture[0]);
  UnloadTexture(enemyOneRunTexture[1]);
  UnloadTexture(enemyOneRunTexture[2]);

  UnloadTexture(enemyHitTexture);

  UnloadTexture(enemy2RunTexture[0]);
  UnloadTexture(enemy2RunTexture[1]);
  UnloadTexture(enemy2RunTexture[2]);

  UnloadTexture(enemy2HitTexture);

  UnloadTexture(cursorTexture);

  UnloadTexture(axeThrowTexture);
  UnloadTexture(axeBaseTexture);

  UnloadTexture(heartTexture);
  UnloadTexture(heartHalfTexture);
  UnloadTexture(heartEmptyTexture);

  UnloadTexture(playerHurtTexture);
  UnloadTexture(playerImmuneTexture);

  UnloadTexture(upgradeScreenBaseTexture);

  UnloadTexture(gameOverTexture);
}
