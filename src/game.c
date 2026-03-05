#include "../headers/game.h"
#include "math.h"
#include "../headers/player.h"
#include "../headers/enemy.h"
#include "../headers/textures.h"
#include "raymath.h"
#include "../headers/arena.h"
#include "stdio.h"
#include "stdlib.h"

const int GAME_WIDTH = 1280;
const int GAME_HEIGHT = 720;
RenderTexture2D target;

Game game;

Player player;

Vector2 mousePos;
Vector2 worldMouse;

Enemy enemy[ENEMY_NUM];

bool startGame = false;
bool debugMode = false;

Camera2D camera;

//Normal screenshake and hitstop stuff
int screenShake = 0;
int screenShakeFrameBase = 6;
int screenShakeValue = 7;

float hitStopTime = 0.025;
float hitStopTimer = 0;

//Check for if player hit screenshake has happened yet
bool playerScreenShakeStarted = false;
float hitStopTimePlayer = 1.0f;


float playerHitStopTime = 0.025f;

float enemySpawnTime = 3.5f;

Rectangle enemyAttackRec;

//Level variables
const char *fileName = "../arena/arena_walls.csv";
const char *propsFileName = "../arena/arena_props.csv";
const char *floorFileName = "../arena/arena_floor.csv";

int temp = 0;

void gameInit(){
    InitWindow(GAME_WIDTH, GAME_HEIGHT, "Project Recall");  
    SetTargetFPS(60);

    texturesLoad();

    target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); 

    HideCursor();

    //Loads csv values into array
    csvToArray(game.levelArray, fileName);
    csvToArray(game.propsArray, propsFileName);
    csvToArray(game.floorArray, floorFileName);

    //Create collision Recs
    game.colliderCount = getWallAmount(game.levelArray);
    game.colliderRecs = malloc(getWallAmount(game.levelArray) * sizeof(Rectangle));
    createCollisionRecs(game.levelArray, game.colliderRecs);

    game.enemySpawnTimer = 0;

    playerInit(&player);

    camera.offset = (Vector2){GAME_WIDTH/2, GAME_HEIGHT/2};
    camera.target = player.pos;
    camera.zoom = 0.75f;
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



void gameUpdate(){
    gameSetFullscreen();


    if(hitStopTimer <= 0){

        if(IsKeyPressed(KEY_TAB)){
            startGame = !startGame;
        }
        //Simple debug enemy spawner
        if(IsKeyPressed(KEY_G)){
            enemyInit(enemy, player.pos);
        }
        if(IsKeyPressed(KEY_F1)){
            debugMode = !debugMode;
        }
        

        int enemyUpdateReturn = enemyUpdate(enemy, player.rec, player.axe, player.pos, game.colliderRecs, game.colliderCount);
        //Returns -1 if enemy is hit to start camera shake and hitstop
        if(enemyUpdateReturn == -1){
            screenShake = screenShakeFrameBase;
            hitStopTimer = hitStopTime;
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

        int checkEnemyAttackReturn = checkEnemyAttack();
        //Player update trakes in enemy attack rectangle and also bool to see if any enemies are attacking 
        //the checkEnemyAttack function returns an index with the enemy attacking if one is, returns -1 otherwise
        //We use this index to pass in the proper attack rectangle and create a booleon to pass in too
        playerUpdate(&player, game.colliderRecs, game.colliderCount, enemy[checkEnemyAttackReturn].attackRec, (checkEnemyAttackReturn != -1), enemy[checkEnemyAttackReturn].pos);
        updateCamera();


    }
    else{
        hitStopTimer -= GetFrameTime();
    }
    

}

void spawnEnemies(){
    game.enemySpawnTimer += GetFrameTime();
    if(game.enemySpawnTimer >= enemySpawnTime){
        enemyInit(enemy, player.pos);
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

void gameDraw(){
    BeginTextureMode(target);

        ClearBackground(BLACK);

        BeginMode2D(camera);
            
            //FLoor
            drawFloor(game.floorArray);
            //Walls
            drawLevel(game.levelArray, 0);
            //Props
            drawLevel(game.propsArray, 1);

            playerDraw(&player);
            enemyDraw(enemy);

            
            


        EndMode2D();

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
    EndTextureMode();

    gameResolutionDraw();
}

void drawColliderRecs(){
    for(int i = 0; i < game.colliderCount; i++){
        DrawRectangleLinesEx(game.colliderRecs[i], 2.5f, RED);
    }
}