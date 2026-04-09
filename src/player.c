#include "../headers/player.h"
#include "../headers/game.h"
#include "../headers/textures.h"
#include "raymath.h"
#include "stdio.h"

int player_width = 75;
int player_height = 75;

int axeWidth = 50;
int axeHeight = 50;

const int axeHoverRadius = 60;

float axeBaseSpeed = 10;
float axeRecallSpeed = 13;
float axeSpeedIncrementThrown = 0.5;
float axeSpeedIncrementRecall = 0.75;

float playerBaseSpeed = 5.0f;

int playerKnockbackFrames = 10;
int playerKnockbackFramesBase = 10;

int playerImmunityFrames = 20;
int playerImmunityFramesBase = 20;

// The collision rec for player
Rectangle tempRec;

void playerInit(Player *player, bool resetting) {
  if (resetting) {
    axeWidth = AXE_WIDTH;
    axeHeight = AXE_HEIGHT;

    axeBaseSpeed = AXE_SPEED;
    axeRecallSpeed = AXE_RECALL_SPEED;

    playerBaseSpeed = PLAYER_SPEED;
  }

  player->pos = (Vector2){1280 / 2, 720 / 2};
  player->rec = (Rectangle){player->pos.x - player_width / 2,
                            player->pos.y - player_height / 2, player_width,
                            player_height};
  player->speed = 5.0f;
  player->dir = (Vector2){0, 0};
  player->state = NOTHING;
  player->lives = 6;
  player->baseLives = 6;
  player->knockbackDir = (Vector2){0, 0};

  // Weapon stuff
  player->axe.pos = (Vector2){player->pos.x, player->pos.y};
  player->axe.rec =
      (Rectangle){player->axe.pos.x - axeWidth / 2,
                  player->axe.pos.y - axeHeight / 2, axeWidth, axeHeight};
  player->axe.throwSpeed = axeBaseSpeed;
  player->axe.recallSpeed = axeRecallSpeed;
  player->axe.state = HOLDING;
  player->axe.attackPos = (Vector2){0, 0};
  player->axe.attackCheckRadius = 15.0;
  player->axe.damage = 50;
  player->axe.currentDrawRotation = 0.0f;
  player->justThrown = false;

  player->axe.isCurrentlyDeflected = false;
  player->axe.deflectedCooldown = 0;

  if (!resetting) {
    animationInit(&player->axe.anim, 0, axeThrowTexture, 16, 4, 0, 0);
    // Animations
    animationInit(&player->playerIdleAnim, 0, playerIdleTexture, 16, 4, 0, 0);
    animationInit(&player->playerSideAnim, 0, playerSideTexture, 16, 5, 0, 0);
    animationInit(&player->playerDeadAnim, 0, playerDeadTexture, 16, 18, 0, 0);
  }

  // Aniamtion related variables
  player->animationDir = 1;
  player->animState = IDLE;

  // Player upgrade varaibles
  for (int i = 0; i < NUMBER_OF_UPGRADES; i++) {
    player->upgradeLevels[i] = 0;
  }
}

void playerUpdate(Player *player, Rectangle rec[], int recNum,
                  Rectangle enemyAttackRec, bool isEnemyAttacking,
                  Vector2 enemyAttackingPos) {
  playerCollisions(player, rec, recNum);

  playerMovement(player, enemyAttackRec, isEnemyAttacking);

  axeUpdate(player, rec, recNum);

  // If enemy is attacking, checks if player gets hit by the attack
  if (isEnemyAttacking) {
    checkPlayerHit(player, enemyAttackRec, enemyAttackingPos);
  }
}

void playerMovement(Player *player, Rectangle enemyAttackRec,
                    bool isEnemyAttacking) {

  switch (player->state) {
  case NOTHING:
    break;
  case PULLING_IN:
    player->dir = (Vector2){player->axe.pos.x - player->pos.x,
                            player->axe.pos.y - player->pos.y};
    player->speed += 1;

    if (player->axe.state == HOLDING) {
      player->state = NOTHING;
      player->speed = playerBaseSpeed;
    }
    break;
  case HURT:
    player->pos.x += player->knockbackDir.x * 15;
    player->pos.y += player->knockbackDir.y * 15;

    playerKnockbackFrames--;

    if (playerKnockbackFrames <= 0) {
      player->state = IMMUNITY;
      player->lives--;
      playerKnockbackFrames = playerKnockbackFramesBase;
    }

    break;

  case IMMUNITY:
    playerImmunityFrames--;

    if (playerImmunityFrames <= 0) {
      player->state = NOTHING;
      playerImmunityFrames = playerImmunityFramesBase;
    }

    break;
  }
  if (player->axe.state != HOLDING && IsKeyPressed(KEY_SPACE) &&
      player->state != HURT && player->state != IMMUNITY) {
    player->state = PULLING_IN;
  }

  if (player->state != PULLING_IN) {
    player->speed = playerBaseSpeed;
    // Get input
    if (IsKeyDown(KEY_A)) {
      player->dir.x -= 1;
      player->animationDir = -1;
    } else if (IsKeyDown(KEY_D)) {
      player->dir.x += 1;
      player->animationDir = 1;
    }

    if (IsKeyDown(KEY_W)) {
      player->dir.y -= 1;
    } else if (IsKeyDown(KEY_S)) {
      player->dir.y += 1;
    }
  }

  // Update anim state
  if (player->dir.x != 0 || player->dir.y != 0) {
    player->animState = RUNNING;

  } else {
    player->animState = IDLE;
  }

  // Add direction to player but first normalize
  if (player->state == NOTHING || player->state == PULLING_IN) {
    player->dir = Vector2Normalize(player->dir);
    player->pos.x += player->dir.x * player->speed;
    player->pos.y += player->dir.y * player->speed;
  }

  // Reset dir
  player->dir = (Vector2){0, 0};

  // Update rec
  player->rec = (Rectangle){player->pos.x - player_width / 2,
                            player->pos.y - player_height / 2, player_width,
                            player_height};
}

void playerCollisions(Player *player, Rectangle rec[], int recNum) {
  for (int i = 0; i < recNum; i++) {
    if (CheckCollisionRecs(player->rec, rec[i])) {

      tempRec = GetCollisionRec(player->rec, rec[i]);

      // Left side collision
      if (tempRec.width < tempRec.height) {
        if (tempRec.x < player->pos.x) {
          player->pos.x += tempRec.width;
        } else if (tempRec.x > player->pos.x) {
          player->pos.x -= tempRec.width;
        }
      } else if (tempRec.height < tempRec.width) {
        if (tempRec.y > player->pos.y) {
          player->pos.y -= tempRec.height;
        } else if (tempRec.y < player->pos.y) {
          player->pos.y += tempRec.height;
        }
      }
    }
  }
}

bool checkPlayerHit(Player *player, Rectangle enemyAttackRec,
                    Vector2 enemyAttackingPos) {
  if (player->upgradeLevels[IMMUNE_WHILE_PULLING_IN] == 0 ||
      player->state != PULLING_IN) {

    if (CheckCollisionRecs(enemyAttackRec, player->rec)) {
      player->state = HURT;
      player->knockbackDir =
          Vector2Normalize((Vector2){player->pos.x - enemyAttackingPos.x,
                                     player->pos.y - enemyAttackingPos.y});
      return true;
    } else {
      return false;
    }
  }
}

bool applyPlayerUpgrade(Player *player, Upgrades selectedUpgrade) {
  switch (selectedUpgrade) {
  case BIGGER_WEAPON:
    player->upgradeLevels[BIGGER_WEAPON]++;
    axeWidth += 10;
    axeHeight += 10;
    return true;
  case FASTER_WEAPON:
    player->upgradeLevels[FASTER_WEAPON]++;
    axeBaseSpeed += 1.5f;
    axeRecallSpeed += 1.25f;
    return true;
  case INCREASED_DAMAGE:
    player->upgradeLevels[INCREASED_DAMAGE]++;
    player->axe.damage += 25;
    return true;
  case INCREASE_PLAYER_SPEED:
    player->upgradeLevels[INCREASE_PLAYER_SPEED]++;
    playerBaseSpeed += 1.5f;
    return true;
  case GAIN_HEALTH:
    player->upgradeLevels[GAIN_HEALTH]++;
    player->lives = Clamp(player->lives + 1, 0, player->baseLives);
    return true;
  case IMMUNE_WHILE_PULLING_IN:
    player->upgradeLevels[IMMUNE_WHILE_PULLING_IN]++;
    if (player->upgradeLevels[IMMUNE_WHILE_PULLING_IN] >= 1) {
      player->upgradeLevels[IMMUNE_WHILE_PULLING_IN] = 1;
    }
    return true;
  default:
    return false;
  }
}

void axeUpdate(Player *player, Rectangle rec[], int recNum) {
  switch (player->axe.state) {
  case HOLDING:

    // Setting axe position based on mouse Pos
    Vector2 mouseDirection = {worldMouse.x - player->pos.x,
                              worldMouse.y - player->pos.y};
    mouseDirection = Vector2Normalize(mouseDirection);
    double angle = atan2(mouseDirection.y, mouseDirection.x);
    player->axe.pos =
        (Vector2){player->pos.x + cosf(angle) * axeHoverRadius,
                  player->pos.y + 10 + sinf(angle) * axeHoverRadius};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      player->justThrown = true;
      player->axe.attackPos = worldMouse;
      player->axe.dir = (Vector2){player->axe.attackPos.x - player->axe.rec.x,
                                  player->axe.attackPos.y - player->axe.rec.y};
      player->axe.dir = Vector2Normalize(player->axe.dir);
      player->axe.state = THROWN;
    }

    break;

  case THROWN:
    player->axe.pos.x += player->axe.dir.x * player->axe.throwSpeed;
    player->axe.pos.y += player->axe.dir.y * player->axe.throwSpeed;
    player->justThrown = false;

    player->axe.throwSpeed += axeSpeedIncrementThrown;

    if (CheckCollisionCircleRec(player->axe.attackPos,
                                player->axe.attackCheckRadius,
                                player->axe.rec)) {
      player->axe.attackPos = (Vector2){0, 0};
      player->axe.dir = (Vector2){0, 0};
      player->axe.state = DONE_THROW;

      player->axe.throwSpeed = axeBaseSpeed;
    }

    // Check collision with wall
    for (int i = 0; i < recNum; i++) {
      if (CheckCollisionRecs(player->axe.rec, rec[i])) {
        player->axe.attackPos = (Vector2){0, 0};
        player->axe.dir = (Vector2){0, 0};
        player->axe.state = DONE_THROW;
        player->axe.throwSpeed = axeBaseSpeed;
      }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      player->axe.attackPos = (Vector2){0, 0};
      player->axe.dir = (Vector2){0, 0};
      player->axe.state = RECALL;

      player->axe.throwSpeed = axeBaseSpeed;
    }
    break;

  case DONE_THROW:
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      player->axe.dir = (Vector2){player->pos.x - 32.5 - player->axe.pos.x,
                                  player->pos.y - 32.5 - player->axe.pos.y};
      player->axe.state = RECALL;
    }
    break;

  case RECALL:
    if (!CheckCollisionRecs(player->rec, player->axe.rec)) {
      player->axe.dir = (Vector2){player->pos.x - player->axe.pos.x,
                                  player->pos.y - player->axe.pos.y};
      player->axe.dir = Vector2Normalize(player->axe.dir);
      player->axe.pos.x += player->axe.dir.x * player->axe.recallSpeed;
      player->axe.pos.y += player->axe.dir.y * player->axe.recallSpeed;
      player->axe.recallSpeed += axeSpeedIncrementRecall;
    } else {
      player->axe.state = HOLDING;
      player->axe.recallSpeed = axeRecallSpeed;
    }
    break;
  }

  // Returns
  if ((CheckCollisionRecs(player->rec, player->axe.rec)) &&
      player->state == PULLING_IN) {
    player->axe.state = HOLDING;
  }

  // Update axe rec
  player->axe.rec =
      (Rectangle){player->axe.pos.x - axeWidth / 2,
                  player->axe.pos.y - axeHeight / 2, axeWidth, axeHeight};
}

void deflectAxe(Player *player) {
  player->axe.dir.x *= -1;
  player->axe.dir.y *= -1;
}

void playerDraw(Player *player) {

  Rectangle drawRec = {player->axe.rec.x, player->axe.rec.y,
                       player->axe.rec.width, player->axe.rec.height};

  if (player->axe.state == HOLDING) {
    DrawTexturePro(axeBaseTexture, (Rectangle){0, 0, 16, 16}, drawRec,
                   (Vector2){0, 0}, 1, WHITE);
  } else if (player->axe.state == DONE_THROW) {
    DrawTexturePro(axeBaseTexture, (Rectangle){0, 0, 16, 16}, drawRec,
                   (Vector2){25, 25}, player->axe.currentDrawRotation, WHITE);
  } else {
    int direction = 1;
    player->axe.currentDrawRotation = GetRandomValue(0, 360);
    playAnimation(&player->axe.anim, drawRec, direction, 0.05);
  }

  if (player->state == HURT) {
    DrawTexturePro(playerHurtTexture, (Rectangle){0, 0, 16, 16}, player->rec,
                   (Vector2){0, 0}, 0.0f, WHITE);
  } else if (player->state == PULLING_IN &&
             player->upgradeLevels[IMMUNE_WHILE_PULLING_IN] == 1) {
    DrawTexturePro(playerImmuneTexture, (Rectangle){0, 0, 16, 16}, player->rec,
                   (Vector2){0, 0}, 0.0f, WHITE);
  } else {
    switch (player->animState) {
    case IDLE:
      playAnimation(&player->playerIdleAnim, player->rec, player->animationDir,
                    0.25);
      break;

    case RUNNING:
      playAnimation(&player->playerSideAnim, player->rec, player->animationDir,
                    0.15);
      break;
    }
  }
}
