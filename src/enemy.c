#include "../headers/enemy.h"
#include "raymath.h"
#include "stdio.h"
#include "../headers/textures.h"

float distanceFromPlayerRadius = 200;
float enemyMeleeAttackRadius = 100.0f;

void enemyInit(Enemy enemy[], Vector2 playerPos){
    for (int i = 0; i < ENEMY_NUM; i++){
        if(!enemy[i].active){

            //Deciding enemy Position
            Vector2 enemySpawnPosition = {GetRandomValue(150, 2500), GetRandomValue(150, 1300)};
            
            
            while(CheckCollisionCircleRec(playerPos, distanceFromPlayerRadius, (Rectangle){enemySpawnPosition.x, enemySpawnPosition.y, 50, 50})){
                enemySpawnPosition = (Vector2){GetRandomValue(150, 2500), GetRandomValue(150, 1300)};
            }

            enemy[i].pos = enemySpawnPosition;
            enemy[i].rec = (Rectangle){enemy[i].pos.x, enemy[i].pos.y, 50, 50};
            enemy[i].speed = GetRandomValue(2,4);
            enemy[i].dir = Vector2Normalize((Vector2){playerPos.x - enemy[i].pos.x, playerPos.y - enemy[i].pos.y});
            enemy[i].active = true;
            enemy[i].health = GetRandomValue(100, 300);
            enemy[i].baseHealth = 100.0f;
            enemy[i].state = NOT_HIT;
            enemy[i].knockbackDir = (Vector2){0,0};
            enemy[i].color = RED;
            
            enemy[i].hitFrameCount = 0;

            enemy[i].randomFollowDir = GetRandomValue(1,4);
            enemy[i].reachedFollowDir = false;

            animationInit(&enemy[i].anim, 0, enemyIdleTexture, 16, 4, 0, 0);

            //Attack stuff
            enemy[i].isAttacking = false;
            enemy[i].attackRec = (Rectangle){0,0,0,0};
            enemy[i].attackFrameBase = 60;
            enemy[i].attackFrameTimer = 60;
            enemy[i].attackCooldownBase = 2.0f;
            enemy[i].attackCooldownTimer = 2.0f;
            enemy[i].inAttackCooldown = false;
            break;
        }
    }
}

void enemyFollowPlayer(Enemy enemy[], Vector2 playerPos, int i){
    if(!enemy[i].reachedFollowDir){
        Vector2 point;
        switch (enemy[i].randomFollowDir){
            case 1:
                point = (Vector2){playerPos.x, playerPos.y - 200};
                break;
            case 2:
                point = (Vector2){playerPos.x + 200, playerPos.y};
                break;
            case 3:
                point = (Vector2){playerPos.x, playerPos.y + 200};
                break;
            case 4:
                point = (Vector2){playerPos.x - 200, playerPos.y};
                break;
            }

        enemy[i].dir = Vector2Normalize((Vector2){point.x - enemy[i].pos.x, point.y - enemy[i].pos.y});
        if(CheckCollisionCircleRec(point, 20.0f, enemy[i].rec)){
            enemy[i].reachedFollowDir = true;
        }

    }
    else{
        enemy[i].dir = Vector2Normalize((Vector2){playerPos.x - enemy[i].pos.x, playerPos.y - enemy[i].pos.y});
    }
    
}   

int enemyUpdate(Enemy enemy[], Rectangle playerRec, Weapon axe, Vector2 playerPos, Rectangle rec[], int recNum){
    int returnValue = 0;
    for(int i = 0; i < ENEMY_NUM; i++){
        if(enemy[i].active){

            enemyAttackUpdate(enemy, playerPos, i);
            enemyCollisions(enemy, rec, recNum, i);

            //Update the attack cooldown
            if(enemy[i].inAttackCooldown){
                enemy[i].attackCooldownTimer -= GetFrameTime();

                if(enemy[i].attackCooldownTimer <= 0){
                    enemy[i].inAttackCooldown = false;
                }
            }
            
            //Enemy stops when attacking melee
            if(!enemy[i].isAttacking){
                enemyFollowPlayer(enemy, playerPos, i);
                enemy[i].pos.x += enemy[i].dir.x * enemy[i].speed;
                enemy[i].pos.y += enemy[i].dir.y * enemy[i].speed;
                enemy[i].rec = (Rectangle){enemy[i].pos.x, enemy[i].pos.y, 50, 50};
            }

            //Enemy hit check (Only acitvates if enemy isnt currently hit) 
            //1 = THROWN
            //2 = RECALL
            if(CheckCollisionRecs(enemy[i].rec, axe.rec) && enemy[i].state == NOT_HIT && (axe.state == 1 || axe.state == 3)) {
                float damageMultiplier = 1.0f;
                if(axe.state == 3){
                    damageMultiplier = 1.5f;
                }
                enemy[i].knockbackDir = Vector2Normalize(axe.dir);

                enemy[i].color = WHITE;
                enemy[i].health -= axe.damage * damageMultiplier;
                enemy[i].state = HIT;

                returnValue = -1;
                break;
            }

            //Add knockback to enemy
            if(enemy[i].state == HIT){
                enemy[i].pos.x += enemy[i].knockbackDir.x * axe.throwSpeed;
                enemy[i].pos.y += enemy[i].knockbackDir.y * axe.throwSpeed;

                enemy[i].hitFrameCount++;

                if(enemy[i].hitFrameCount >= 4){
                    enemy[i].state = IMMUNE;
                    enemy[i].hitFrameCount = 0;
                
                }
            }

            //Enemy is hit but immune now
            if(enemy[i].state == IMMUNE){
                if(!CheckCollisionRecs(enemy[i].rec, axe.rec)){
                    enemy[i].state = NOT_HIT;
                }
            }

            //If axe and enemy not colliding, resets state
            if(!CheckCollisionRecs(enemy[i].rec,axe.rec) && enemy[i].state != IMMUNE){
                enemy[i].state = NOT_HIT;
                enemy[i].color = RED;
            }

            //Remove enemy once health is 0
            if(enemy[i].health <= 0){
                enemyDelete(enemy, i);
            }

        }
        
    }
    return returnValue;
}

void enemyAttackUpdate(Enemy enemy[], Vector2 playerPos, int i){
    if(!enemy[i].isAttacking && CheckCollisionCircleRec(playerPos, enemyMeleeAttackRadius, enemy[i].rec) && !enemy[i].inAttackCooldown){
        enemy[i].isAttacking = true;
        Vector2 dirToPlayer = Vector2Normalize((Vector2){playerPos.x - enemy[i].pos.x, playerPos.y - enemy[i].pos.y});
        enemy[i].attackRec = (Rectangle){enemy[i].pos.x + dirToPlayer.x * 100, enemy[i].pos.y + dirToPlayer.y * 100, 50, 50};
    }

    if(enemy[i].isAttacking){
        enemy[i].attackFrameTimer--;

        if(enemy[i].attackFrameTimer <= 0){
            enemy[i].attackFrameTimer = enemy[i].attackFrameBase;
            enemy[i].isAttacking = false;
            enemy[i].inAttackCooldown = true;
        }
    }
}

void enemyCollisions(Enemy enemy[], Rectangle rec[], int recNum, int i){
    for (int x = 0; x < recNum; x++) {
        if (CheckCollisionRecs(enemy[i].rec, rec[x])) {
            Rectangle collisionRec = GetCollisionRec(enemy[i].rec, rec[x]);

            // Horizontal collision (thinner overlap = side hit)
            if (collisionRec.width < collisionRec.height) {
                if (enemy[i].rec.x < rec[x].x)
                    enemy[i].pos.x -= collisionRec.width;  // push left
                else
                    enemy[i].pos.x += collisionRec.width;  // push right
            }
            // Vertical collision (shorter overlap = top/bottom hit)
            else if (collisionRec.height < collisionRec.width) {
                if (enemy[i].rec.y < rec[x].y)
                    enemy[i].pos.y -= collisionRec.height;  // push up
                else
                    enemy[i].pos.y += collisionRec.height;  // push down
            }

            // Sync rectangle position to match updated pos
            enemy[i].rec.x = enemy[i].pos.x;
            enemy[i].rec.y = enemy[i].pos.y;
        }
}
    
}

void enemyDraw(Enemy enemy[]){
    for(int i = 0; i < ENEMY_NUM; i++){
        if(enemy[i].active){
            //DrawRectangleRec(enemy[i].rec, enemy[i].color);
            Rectangle animRec = {enemy[i].rec.x - 25, enemy[i].rec.y - 25, enemy[i].rec.width + 50, enemy[i].rec.height + 50};
            
            if(enemy[i].isAttacking){
                DrawRectangleRec(enemy[i].attackRec, (Color){255,0,0, enemy[i].attackFrameTimer * 4});
            }

            if(!enemy[i].state == HIT){
                int animDir;
                if(enemy[i].dir.x >= 0){
                    animDir = 1;
                }
                else if(enemy[i].dir.x < 0){
                    animDir = -1;
                }
                playAnimation(&enemy[i].anim, animRec, animDir, 0.15);
            }
            else{
                DrawTexturePro(enemyHitTexture, (Rectangle){0,0,16,16}, animRec, (Vector2){0,0}, 0.0f, WHITE);
            }

            DrawRectangle(enemy[i].rec.x, enemy[i].rec.y - 50, (int)(enemy[i].rec.width * (enemy[i].health/enemy[i].baseHealth)), 20, WHITE );
        }
    }
}

void enemyDelete(Enemy enemy[], int i){
    enemy[i].active = false;
    enemy[i].dir = (Vector2){0,0};
    enemy[i].pos = (Vector2){0,0};
    enemy[i].rec = (Rectangle){0,0,0,0};
    enemy[i].speed = 0;
    enemy[i].health = -1;
    enemy[i].state = NOT_HIT;
}
