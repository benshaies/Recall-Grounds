#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "animation.h"
#include "../headers/arena.h"


    typedef enum{
        NOTHING,
        PULLING_IN,
        HURT,
        IMMUNITY, 
    }State;

    typedef enum{
        IDLE,
        RUNNING,
    }AnimState;

    typedef enum{
        HOLDING,
        THROWN,
        DONE_THROW,
        RECALL,
    }State2;

    typedef struct{
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

    }Weapon;

    typedef struct{
        //Player specific
        Vector2 pos;
        Rectangle rec;
        float speed;
        Vector2 dir;
        State state;
        int baseLives;
        int lives;
        Vector2 knockbackDir;

        //Axe varaibles
        Weapon axe;

        //Animations
        Animation playerIdleAnim;
        Animation playerSideAnim;

        //Animation related variables
        int animationDir;
        AnimState animState;
    }Player;

    void playerInit(Player *player);

    void playerUpdate(Player *player, Rectangle rec[], int recNum, Rectangle enemyAttackRec, bool isEnemyAttacking);

    void playerMovement(Player *player, Rectangle enemyAttackRec, bool isEnemyAttacking);

    void playerCollisions(Player *player, Rectangle rec[], int recNum);

    bool checkPlayerHit(Player *player, Rectangle enemyAttackRec);

    void axeUpdate(Player *player, Rectangle rec[], int recNum);

    void playerDraw(Player *player);

#endif
