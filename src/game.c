#include "../headers/game.h"
#include "math.h"
#include "../headers/player.h"
#include "../headers/enemy.h"
#include "../headers/textures.h"
#include "raymath.h"
#include "../headers/arena.h"
#include "stdio.h"
#include "stdlib.h"
#include "../headers/particles.h"
#include <string.h>

const int GAME_WIDTH = 1280;
const int GAME_HEIGHT = 720;
RenderTexture2D target;

Game game;

Player player;

Vector2 mousePos;
Vector2 worldMouse;

Enemy enemy[ENEMY_NUM];

bool debugMode = false;

Camera2D camera;

//Normal screenshake and hitstop stuff
int screenShake = 0;
int screenShakeFrameBase = 6;
int screenShakeValue = 7;

float hitStopTime = 0.025;
float hitStopTimer = 0;

//Throwing boomerang screenshake
int boomerangThrownScreenShake = 0;

//Check for if player hit screenshake has happened yet
bool playerScreenShakeStarted = false;
float hitStopTimePlayer = 1.0f;

float playerHitStopTime = 0.025f;

float enemySpawnTime = 2.5f;

Rectangle enemyAttackRec;

//Level variables
const char *fileName = "../arena/arena_walls.csv";
const char *propsFileName = "../arena/arena_props.csv";
const char *floorFileName = "../arena/arena_floor.csv";

int temp = 0;

//Game wave system varaibles
bool startGame = false;

//Health rectangles
Rectangle healthRecs[3];

//Particle variables
ParticleSystem ps;

int playerRunParticleCheck = 0;
Color playerTrailColor = {33, 19, 25, 255};

int boomerangThrowParticleCheck = 0;
Color boomerangTrailColor = {18, 78, 137, 255};
Color boomerangTrailColor2 = {44, 232, 245, 255};

Color enemyHitParticleColor = {162, 38, 51, 255};

void gameInit(){
    InitWindow(GAME_WIDTH, GAME_HEIGHT, "Project Recall");  
    SetTargetFPS(60);

    game.state = PLAYING;
    memset(&ps, 0, sizeof(ParticleSystem));

    texturesLoad();

    target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); 

    HideCursor();

    //Loads csv values into array
    csvToArray(game.levelArray, fileName);
    csvToArray(game.propsArray, propsFileName);
    csvToArray(game.floorArray, floorFileName);

    //Init health recs
    healthRecs[0] = (Rectangle){10,5, 40, 40};
    healthRecs[1] = (Rectangle){55, 5, 40, 40};
    healthRecs[2] = (Rectangle){95, 5, 40, 40};

    //Create collision Recs
    game.colliderCount = getWallAmount(game.levelArray);
    game.colliderRecs = malloc(getWallAmount(game.levelArray) * sizeof(Rectangle));
    createCollisionRecs(game.levelArray, game.colliderRecs);

    game.enemySpawnTimer = 0;

    playerInit(&player);

    camera.offset = (Vector2){GAME_WIDTH/2, GAME_HEIGHT/2};
    camera.target = player.pos;
    camera.zoom = 0.75f;

    game.score = 0;
    game.enemiesKilled = 0;
    game.timeSurvived = 0.0f;
}

void cameraShake(){
    if(screenShake > 0){
        screenShake--;
        camera.offset = (Vector2){camera.offset.x + GetRandomValue(-screenShakeValue,screenShakeValue), camera.offset.y + GetRandomValue(-screenShakeValue,screenShakeValue)};
    }
    else{
        //camera.offset = (Vector2){GAME_WIDTH/2, GAME_HEIGHT/2};
        camera.offset.x = Lerp(camera.offset.x, GAME_WIDTH/2, 5* GetFrameTime());
        camera.offset.y = Lerp(camera.offset.y, GAME_HEIGHT/2, 5 * GetFrameTime());
    }
}

void updateCamera(){

    Vector2 mouseDir = { worldMouse.x - player.pos.x, worldMouse.y - player.pos.y };
    mouseDir = Vector2Normalize(mouseDir);
    mouseDir = Vector2Scale(mouseDir, 80);

    Vector2 desiredTarget = {
        player.pos.x + mouseDir.x,
        player.pos.y + mouseDir.y
    };

    camera.target.x = (Lerp(camera.target.x, desiredTarget.x, 10 * GetFrameTime()));
    camera.target.y = (Lerp(camera.target.y, desiredTarget.y, 10 * GetFrameTime()));

    

    cameraShake();

}


void gameSetFullscreen(){
    if(IsKeyPressed(KEY_F11)){
        ToggleBorderlessWindowed();
    }
}

//Checks if enemy is attacking, stores attack rec and returns true
int checkEnemyAttack(){
    int returnValue = -1;
    for(int i = 0; i < ENEMY_NUM; i++){
        if(enemy[i].active && enemy[i].isAttacking){
            returnValue = i;
        }

    }

    return returnValue;
}

void updateScore(){
    game.timeSurvived += GetFrameTime();

    game.score = (game.timeSurvived * 2) + (game.enemiesKilled * 5);
}

void manageParticles(){
    //Spawn in player particles
    if(player.animState == RUNNING){
        Vector2 trailStartPos;

        if(player.animationDir == -1) trailStartPos = (Vector2){player.pos.x + player.rec.width/2 - 10 + GetRandomValue(-5, 5), player.pos.y + player.rec.height/2 - 10 +  GetRandomValue(-5, 5)};
        else trailStartPos = (Vector2){player.pos.x - player.rec.width/2 + 10 + GetRandomValue(-5, 5), player.pos.y + player.rec.height/2 - 5 +  GetRandomValue(-5, 5)};
        
        //Only activates every other 3rd frame
        if(playerRunParticleCheck == 2){
            spawnParticles(&ps, trailStartPos, GetRandomValue(1,5) * 0.05, playerTrailColor, (Vector2){-player.dir.x * GetRandomValue(1,2),GetRandomValue(-1, 1)}, 2.5f);
            playerRunParticleCheck = 0;
        }
        else playerRunParticleCheck++;
    }

    //Boomerang particles
    if( (player.axe.state == THROWN || player.axe.state == RECALL)){
        spawnParticles(&ps, player.axe.pos, GetRandomValue(1,3) * 0.15, boomerangTrailColor, (Vector2){GetRandomValue(1,3), GetRandomValue(0,2)}, GetRandomValue(1,3));
        spawnParticles(&ps, player.axe.pos, GetRandomValue(1,3) * 0.15, boomerangTrailColor2, (Vector2){ GetRandomValue(1,3), GetRandomValue(0,2)}, GetRandomValue(1,3));
    }

    //Enemy particles when hit
    for(int i = 0; i < ENEMY_NUM; i++){

        if(!enemy[i].active)continue;;
        if(enemy[i].state != HIT) continue;

        spawnParticles(&ps, enemy[i].pos, GetRandomValue(1,3) * 1, enemyHitParticleColor, (Vector2){GetRandomValue(-10,10), GetRandomValue(-10,10)}, GetRandomValue(2,6));
        spawnParticles(&ps, enemy[i].pos, GetRandomValue(1,3) * 1, enemyHitParticleColor, (Vector2){GetRandomValue(-10,10), GetRandomValue(-10,10)}, GetRandomValue(2,6));
        spawnParticles(&ps, enemy[i].pos, GetRandomValue(1,3) * 1, enemyHitParticleColor, (Vector2){GetRandomValue(-10,10), GetRandomValue(-10,10)}, GetRandomValue(2,6));


    }    

    updateParticles(&ps);
}

void gamePlayingUpdate(){
    gameSetFullscreen();
    updateScore();

    if(hitStopTimer <= 0){
    
        if(IsKeyPressed(KEY_ENTER)){
            startGame = !startGame;
        }
        if(startGame){
            spawnEnemies();
        }

        //Simple debug enemy spawner
        if(IsKeyPressed(KEY_G)){
            enemyInit(enemy, player.pos, 2);
        }
        if(IsKeyPressed(KEY_F1)){
            debugMode = !debugMode;
        }

        int enemyUpdateReturn = enemyUpdate(enemy, player.rec, player.axe, player.pos, game.colliderRecs, game.colliderCount, &ps);
        //Returns -1 if enemy is hit to start camera shake and hitstop, returns 1 if enemy is killed
        if(enemyUpdateReturn == -1){ // Enemy hit
            screenShake = screenShakeFrameBase;
            hitStopTimer = hitStopTime;
        }
        //Enemy killed and spawn particles
        else if(enemyUpdateReturn == 1){
            game.enemiesKilled++;
            spawnParticlesExpandingRing(&ps, player.axe.pos, 0.4, enemyHitParticleColor, 5, GetRandomValue(10,20), GetRandomValue(30, 50));
        }

        //If player hurt and screenshake hasnt started, start it once
        if(player.state == HURT && !playerScreenShakeStarted){
            playerScreenShakeStarted = true;
            screenShake = screenShakeFrameBase;
            hitStopTimer = hitStopTime;
        }

        //Reset screenshake check variable if player isnt hurt anymore
        if(player.state != HURT){
            playerScreenShakeStarted = false;
        }

        //Start screenshakle if player throws boomerang
        if(player.justThrown){
            screenShake = boomerangThrownScreenShake;
        }
        int checkEnemyAttackReturn = checkEnemyAttack();

        //Player update takes in enemy attack rectangle and also bool to see if any enemies are attacking 
        //the checkEnemyAttack function returns an index with the enemy attacking if one is, returns -1 otherwise
        //We use this index to pass in the proper attack rectangle and create a booleon to pass in too
        playerUpdate(&player, game.colliderRecs, game.colliderCount, enemy[checkEnemyAttackReturn].attackRec, (checkEnemyAttackReturn != -1), enemy[checkEnemyAttackReturn].pos);
        updateCamera();

        manageParticles();

    }
    else{
        hitStopTimer -= GetFrameTime();
    }
}

void gameUpdate(){

    switch (game.state){
        case MAIN_MENU:
            break;
        case PLAYING:
            gamePlayingUpdate();
            break;
        case DEAD:
            break;
        case UPGRADE_SCREEN:
            break;
        case TESTING:
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                spawnParticles(&ps, worldMouse, 0.5, WHITE, (Vector2){GetRandomValue(-3,3), -5}, 10.0f);
            }
            updateParticles(&ps);
            break;
    }
}

void spawnEnemies(){
    game.enemySpawnTimer += GetFrameTime();
    if(game.enemySpawnTimer >= enemySpawnTime){
        enemyInit(enemy, player.pos, GetRandomValue(1,2));
        game.enemySpawnTimer = 0;
    }
}

void gameResolutionDraw(){
    BeginDrawing();
        ClearBackground(WHITE);

        float scale = fmin(
            (float)GetScreenWidth()  / GAME_WIDTH,
            (float)GetScreenHeight() / GAME_HEIGHT
        );

        float scaledWidth  = GAME_WIDTH  * scale;
        float scaledHeight = GAME_HEIGHT * scale;

        float offsetX = (GetScreenWidth()  - scaledWidth)  * 0.5f;
        float offsetY = (GetScreenHeight() - scaledHeight) * 0.5f;

        Rectangle src = { 
            0, 0, 
            (float)target.texture.width, 
            -(float)target.texture.height   // flip vertically
        };

        Rectangle dst = {
            offsetX, offsetY,
            scaledWidth, scaledHeight
        };

        DrawTexturePro(target.texture, src, dst, (Vector2){0,0}, 0.0f, WHITE);

        mousePos = GetMousePosition();

        mousePos.x = (mousePos.x - offsetX) / scale;
        mousePos.y = (mousePos.y - offsetY) / scale;
        worldMouse = GetScreenToWorld2D(mousePos, camera);


    EndDrawing();
}

void gamePlayingDraw(){
        
        BeginMode2D(camera);
            
            //FLoor
            drawFloor(game.floorArray);
            //Walls
            drawLevel(game.levelArray, 0);
            //Props
            drawLevel(game.propsArray, 1);

            drawParticles(&ps);

            playerDraw(&player);
            enemyDraw(enemy);

            

        EndMode2D();

        // Draw player health
        for (int i = 0; i < 3; i++) {
            int threshold = (i + 1) * 2;
            Texture2D tex;

            if (player.lives >= threshold) {
                tex = heartTexture;
            } else if (player.lives == threshold - 1) {
                tex = heartHalfTexture;
            } else {
                tex = heartEmptyTexture;
            }

            DrawTexturePro(tex, (Rectangle){0,0,16,16}, healthRecs[i], (Vector2){0,0}, 0.0, WHITE);
        }

        //Score display
        DrawText((TextFormat("%d", game.score)), 1150, 5, 50, WHITE);
       
        
        

        if(debugMode){
            switch(player.state){
                case NOTHING:
                    DrawText("NOTHING", 0,0, 50, GREEN);
                    break;
                case PULLING_IN:
                    DrawText("PULLING_IN", 0,0, 50, GREEN);
                    break;
                case HURT:
                    DrawText("HURT", 0,0, 50, GREEN);
                    break;
                case IMMUNITY:
                    DrawText("IMMUNITY", 0,0, 50, GREEN);
                    break;
                    
            }
        }
        
        //Draw cursor

        DrawTexturePro(cursorTexture, (Rectangle){0,0,16,16}, (Rectangle){mousePos.x - 15, mousePos.y - 15, 30, 30}, (Vector2){0,0}, 0.0f, WHITE); 
}

void gameDraw(){
    BeginTextureMode(target);

        ClearBackground(BLACK);

        switch (game.state){
            case MAIN_MENU:
                break;
            case PLAYING:
                gamePlayingDraw();
                break;
            case DEAD:
                break;  
            case UPGRADE_SCREEN:
                break;
            case TESTING:  
                drawParticles(&ps);
                break;
        }

    EndTextureMode();

    gameResolutionDraw();
}

void drawColliderRecs(){
    for(int i = 0; i < game.colliderCount; i++){
        DrawRectangleLinesEx(game.colliderRecs[i], 2.5f, RED);
    }
}