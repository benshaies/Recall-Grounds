#ifndef PLAYER_H
#define PLAYER_H
#include "../headers/arena.h"
#include "../headers/upgrade.h"
#include "animation.h"
#include "raylib.h"

#define AXE_WIDTH 50
#define AXE_HEIGHT 50

#define AXE_SPEED 10
#define AXE_RECALL_SPEED 13

#define PLAYER_SPEED 5.0

typedef enum {
  NOTHING,
  PULLING_IN,
  HURT,
  IMMUNITY,
} State;

typedef enum {
  IDLE,
  RUNNING,
} AnimState;

typedef enum {
  HOLDING,
  THROWN,
  DONE_THROW,
  RECALL,
} State2; // Axe State

typedef struct {
  Vector2 pos;
  Rectangle rec;
  float throwSpeed;
  float recallSpeed;
  State2 state;

  Vector2 attackPos;
  float attackCheckRadius;
  Vector2 dir;

  float damage;

  Animation anim;

  float currentDrawRotation;

  bool isCurrentlyDeflected;
  int deflectedCooldown;

} Weapon;

typedef struct {
  // Player specific
  Vector2 pos;
  Rectangle rec;
  float speed;
  Vector2 dir;
  State state;
  int baseLives;
  int lives;
  Vector2 knockbackDir;

  // Axe varaibles
  Weapon axe;

  // Animations
  Animation playerIdleAnim;
  Animation playerSideAnim;
  Animation playerDeadAnim;
  bool justThrown;

  // Animation related variables
  int animationDir;
  AnimState animState;

  // Upgrade variables
  int upgradeLevels[NUMBER_OF_UPGRADES];
} Player;

void playerInit(Player *player, bool resetting);

void playerUpdate(Player *player, Rectangle rec[], int recNum,
                  Rectangle enemyAttackRec, bool isEnemyAttacking,
                  Vector2 enemyAttackingPos);

void playerMovement(Player *player, Rectangle enemyAttackRec,
                    bool isEnemyAttacking);

void playerCollisions(Player *player, Rectangle rec[], int recNum);

bool checkPlayerHit(Player *player, Rectangle enemyAttackRec,
                    Vector2 enemyAttackingPos);

bool applyPlayerUpgrade(Player *player, Upgrades selectedUpgrade);

void axeUpdate(Player *player, Rectangle rec[], int recNum);

void deflectAxe(Player *player);

void playerDraw(Player *player);

#endif
