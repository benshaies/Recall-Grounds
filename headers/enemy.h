#ifndef ENEMY_H
#define ENEMY_H
#include "raylib.h"
#include "player.h"
#include "animation.h"
#include "particles.h"

    #define ENEMY_NUM 5

    typedef enum{
        HIT,
        NOT_HIT,
        IMMUNE,
    }enemyState;

    typedef struct{
        Vector2 pos;
        Rectangle rec;
        Vector2 dir;
        bool active;
        float speed;
        enemyState state;
        int health;
        float baseHealth;
        Vector2 knockbackDir;
        Color color;
        int hitFrameCount;

        int randomFollowDir;
        bool reachedFollowDir;

        Animation anim;

        bool isAttacking;
        Rectangle attackRec;
        int attackFrameTimer;
        int attackFrameBase;
        float attackCooldownTimer;
        float attackCooldownBase;
        bool inAttackCooldown;

        int type;

        Color healthBarColor;
        
    }Enemy;

    

    void enemyInit(Enemy enemy[], Vector2 playerPos, int type, int scoreMilestone);

    void enemyFollowPlayer(Enemy enemy[], Vector2 playerPos, int i);

    int enemyUpdate(Enemy enemy[], Rectangle playerRec, Weapon axe, Vector2 playerPos, Rectangle rec[], int recNum, ParticleSystem *ps);

    void enemyAttackUpdate(Enemy enemy[], Vector2 playerPos, int i);

    void enemyCollisions(Enemy enemy[], Rectangle rec[], int recNum, int i);

    void enemyDraw(Enemy enemy[]);

    void enemyDelete(Enemy enemy[], int i, ParticleSystem *ps);


#endif