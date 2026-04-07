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
#include "../headers/upgrade.h"

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

float enemySpawnTime = 2.0f;

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

bool playerHitParticleStarted = false;

//Gameplay specific variables
bool isBoomerangDeflected = false;

//Upgrade screen stuff
Rectangle baseRec = {290, 160, 700, 400};
UpgradeScreen upgradeScreen;
bool startUpgrades = false;

//Gameover screen variables
Rectangle gameOverRec = {390, -400, 500, 400};
float gameOverRecVelY = 0.0f;
bool hasFallen = false;
float gameOverRecGravity = 1.5f;
int bounceCount = 0;

Font font;

TimedEvent displayTimeSurvied;

TimedEvent displayEnemiesKilled;
TimedEvent displayScore;

Rectangle quitButtonRec;
Color quitButtonColor = (Color) {24, 20, 37, 255};
Color secondaryColor = (Color){232, 183, 150, 255};
Rectangle playAgainButtonRec;



void gameInit(){
    InitWindow(GAME_WIDTH, GAME_HEIGHT, "Project Recall");  
    SetTargetFPS(60);

    font = LoadFontEx("../PixeloidSans-Bold.ttf", 64, 0, 0);

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
    game.scoreThresholdNum = 1;

    game.enemiesKilled = 0;
    game.timeSurvived = 0.0f;

    upgradeStructInit(&upgradeScreen);

    //Define Reset timed event stuff
    resetTimedEvent(&displayTimeSurvied, 1.5f);
    resetTimedEvent(&displayEnemiesKilled, 1.5f);
    resetTimedEvent(&displayScore, 2.0f);
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

    game.score = (game.timeSurvived * 0.25) + (game.enemiesKilled * 10);
}

void manageParticles(){
    //Spawn in player particles
    if(player.animState == RUNNING){
        Vector2 trailStartPos;

        if(player.animationDir == -1) trailStartPos = (Vector2){player.pos.x + player.rec.width/2 - 10 + GetRandomValue(-5, 5), player.pos.y + player.rec.height/2 - 10 +  GetRandomValue(-5, 5)};
        else trailStartPos = (Vector2){player.pos.x - player.rec.width/2 + 10 + GetRandomValue(-5, 5), player.pos.y + player.rec.height/2 - 5 +  GetRandomValue(-5, 5)};
        
        //Only activates every other 3rd frame
        if(playerRunParticleCheck == 2){
            spawnParticles(&ps, trailStartPos, GetRandomValue(1,5) * 0.05, playerTrailColor, (Vector2){-player.dir.x * GetRandomValue(1,2),GetRandomValue(-1, 1)}, 5);
            playerRunParticleCheck = 0;
        }
        else playerRunParticleCheck++;
    }

    //Player hit particle effects
    if(player.state == HURT && !playerHitParticleStarted){
        for(int i = 0; i < 6; i ++){
            spawnParticles(&ps, player.pos, 1.0, enemyHitParticleColor, (Vector2){GetRandomValue(-5,5), GetRandomValue(-5,5)}, GetRandomValue(10, 15));
        }
        playerHitParticleStarted = true;
    }
    if(player.state == NOTHING){
        playerHitParticleStarted = false;
    }

    if(player.state == PULLING_IN){
        spawnParticles(
            &ps,
            player.pos,
            GetRandomValue(1,5) * 0.01,
            WHITE,
            (Vector2){GetRandomValue(-2,2), GetRandomValue(-2, 2)},
            GetRandomValue(1,10)
        );
        spawnParticles(
            &ps,
            player.pos,
            GetRandomValue(1,5) * 0.025,
            WHITE,
            (Vector2){GetRandomValue(-2,2), GetRandomValue(-2, 2)},
            GetRandomValue(1,10)
        );
    }



    //Boomerang particles
    if( (player.axe.state == THROWN || player.axe.state == RECALL)){
        spawnParticles(&ps, player.axe.pos, GetRandomValue(1,3) * 0.15, boomerangTrailColor, (Vector2){GetRandomValue(1,3), GetRandomValue(0,2)}, GetRandomValue(3 + player.upgradeLevels[BIGGER_WEAPON] * 2,6 + player.upgradeLevels[BIGGER_WEAPON] * 4));
        spawnParticles(&ps, player.axe.pos, GetRandomValue(1,3) * 0.15, boomerangTrailColor2, (Vector2){ GetRandomValue(1,3), GetRandomValue(0,2)}, GetRandomValue(3 + player.upgradeLevels[BIGGER_WEAPON] * 2,6 + player.upgradeLevels[BIGGER_WEAPON] * 4));
    }

    //Enemy particles when hit
    for(int i = 0; i < ENEMY_NUM; i++){

        if(!enemy[i].active)continue;;
        if(enemy[i].state != HIT) continue;

        spawnParticles(&ps, enemy[i].pos, GetRandomValue(1,3) * 0.15, enemyHitParticleColor, (Vector2){GetRandomValue(-10,10), GetRandomValue(-10,10)}, GetRandomValue(10,20));
        spawnParticles(&ps, enemy[i].pos, GetRandomValue(1,3) * 0.15, enemyHitParticleColor, (Vector2){GetRandomValue(-10,10), GetRandomValue(-10,10)}, GetRandomValue(10,20));
        spawnParticles(&ps, enemy[i].pos, GetRandomValue(1,3) * 0.15, enemyHitParticleColor, (Vector2){GetRandomValue(-10,10), GetRandomValue(-10,10)}, GetRandomValue(10,20));

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
        
        if(IsKeyPressed(KEY_F1)){
            debugMode = !debugMode;
        }

        if(IsKeyPressed(KEY_L)){
            game.state = DEAD;
        }

        int enemyUpdateReturn = enemyUpdate(enemy, player.rec, player.axe, player.pos, game.colliderRecs, game.colliderCount, &ps);
        //Returns -1 - Enemy hit
        //Returns 1 - Enemy dead
        //Returns 2 - Shield enemy hit during throw
        
        if(enemyUpdateReturn == -1){ // Enemy hit
            screenShake = 15;
            hitStopTimer = hitStopTime * 2;
        }
        //Enemy killed and spawn particles
        else if(enemyUpdateReturn == 1){
            game.enemiesKilled++;
            spawnParticlesExpandingRing(&ps, player.axe.pos, 0.25, enemy->healthBarColor, 5, GetRandomValue(10,20), GetRandomValue(30, 50)); //(ps, spawnPos, lifeSpan, color, startSize, expandingRate, ring thickness)
        }
        //Shield Enemy is hit during throw
        else if(enemyUpdateReturn == 2 && !isBoomerangDeflected){
            deflectAxe(&player);
            isBoomerangDeflected = true;
            spawnParticlesExpandingRing(&ps, player.axe.pos, 0.2, WHITE, 5, 10, 20);
            screenShake = 20;
        }
        //Reset deflected boolean
        if(player.axe.state != THROWN){
            isBoomerangDeflected = false;
        }

        

        //If player hurt and screenshake hasnt started, start it once
        if(player.state == HURT && !playerScreenShakeStarted){
            playerScreenShakeStarted = true;
            screenShake = screenShakeFrameBase + 10;
            hitStopTimer = hitStopTime ;
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

void gameUpgradeScreenDraw(){
    drawUpgrades(&upgradeScreen, font);
}

void gameUpdateDeadScreen(){

    if(!hasFallen){
        gameOverRecVelY += gameOverRecGravity;
    }

    if(gameOverRec.y >= 160){
        gameOverRecVelY *= 0.75f;
        gameOverRecVelY *= -1;
        bounceCount++;
    }

    if(bounceCount >= 4){
        gameOverRec.y = 160;
        hasFallen = true;
        gameOverRecVelY = 0.0f;
        bounceCount = 0;
    }

    //Update rec
    gameOverRec.y += gameOverRecVelY;

    if(hasFallen){
        if(updateTimedEvent(&displayTimeSurvied)){
            if(updateTimedEvent(&displayEnemiesKilled)){
                updateTimedEvent(&displayScore);
            }
        }
    }
}

void gameUpdate(){

    switch (game.state){
        case MAIN_MENU:
            break;
        case PLAYING:
            gamePlayingUpdate();
            if(startUpgrades || IsKeyPressed(KEY_U)){
                game.state = UPGRADE_SCREEN;;
                createUpgrades(&upgradeScreen, player.upgradeLevels[IMMUNE_WHILE_PULLING_IN] == 1, player.lives == 6);
                resetUpgradeUI(&upgradeScreen);
            }
            if(game.score >= (game.scoreThresholdNum * 125)){
                startUpgrades = true;
                game.scoreThresholdNum++;
            }
            else{
                startUpgrades = false;
            }

            //Player died
            if(player.lives <= 0){
                game.state = DEAD;
            }

            break;
        case DEAD:
            gameUpdateDeadScreen();
            updateParticles(&ps);
            cameraShake();
            break;
        case UPGRADE_SCREEN:
            updateUpgradeScreen(&upgradeScreen, mousePos);

            if(upgradeScreen.selectedUpgrade != -1){
                if(applyPlayerUpgrade(&player, upgradeScreen.selectedUpgrade)){
                    game.state = PLAYING;
                    upgradeScreen.selectedUpgrade = -1;
                }
            }        
            break;
        case TESTING:
            break;
    }
}

void resetTimedEvent(TimedEvent *event, float delay){
    event->triggered = false;
    event->delay = delay;
    event->timer = 0.0f;
    event->particleTriggered = false;
}

bool updateTimedEvent(TimedEvent *event){
    if(!event->triggered){
        event->timer += GetFrameTime();

        if(event->timer >= event->delay){
            event->triggered = true;
        }
    }

    return event->triggered;
}

void spawnEnemies(){
    game.enemySpawnTimer += GetFrameTime();
    if(game.enemySpawnTimer >= enemySpawnTime){

        //25 percent chang of spawning shield enemy
        int random = GetRandomValue(1,4);
        if(random == 4){
            enemyInit(enemy, player.pos, 2, game.scoreThresholdNum);
        }
        else{
            enemyInit(enemy, player.pos, 1, game.scoreThresholdNum);
        }
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

            playerDraw(&player);

            enemyDraw(enemy, game.state == UPGRADE_SCREEN);

            if(game.state == PLAYING){
                drawParticles(&ps);
            }
            

            if(debugMode){
                drawColliderRecs();
            }

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
        if(game.state != DEAD){
            DrawText((TextFormat("%d", game.score)), 1150, 5, 50, WHITE);
        }
        
       
        if(debugMode){
            switch(upgradeScreen.state){
                case NOT_ACTIVE:
                    DrawText("NOT_ACTIVE", 0,0, 50, GREEN);
                    break;
                case OPENING:
                    DrawText("OPENING", 0,0, 50, GREEN);
                    break;
                case DONE_OPENING:
                    DrawText("DONE_OPENING", 0,0, 50, GREEN);
                    break;
                case SELECTED:
                    DrawText("SELECTED", 0,0, 50, GREEN);
                    break;
            }
        }
}

void gameDeadScreenDraw(){

    DrawRectangle(0,0, 1280, 720, Fade(BLACK, 0.4));

    DrawTexturePro(gameOverTexture, (Rectangle){0,0,80, 64}, gameOverRec, (Vector2){0,0}, 0.0f, WHITE);
    DrawTextEx(font, "GAME OVER", (Vector2){gameOverRec.x+ 60, gameOverRec.y + 20}, 64, 5, (Color){162, 38, 51, 255});
    
    DrawTextEx(font, "Time Survived:", (Vector2){gameOverRec.x + 30, gameOverRec.y + 100}, 30, 2, quitButtonColor);

    DrawTextEx(font, "Enemies Killed:", (Vector2){gameOverRec.x + 30, gameOverRec.y + 150}, 30, 2, quitButtonColor);


    DrawTextEx(font, "FINAL SCORE", (Vector2){gameOverRec.x + 125, gameOverRec.y + 210}, 34, 3, quitButtonColor);

    if(displayTimeSurvied.triggered){
        int minutes = (int)game.timeSurvived / 60;
        int secs = (int)game.timeSurvived % 60;
        
        DrawTextEx(font, TextFormat("%d:%02d", minutes, secs), (Vector2){gameOverRec.x + 300, gameOverRec.y + 100}, 30, 2,WHITE);
        if(!displayTimeSurvied.particleTriggered){
            spawnParticlesExpandingRing(&ps, (Vector2){gameOverRec.x + 300+ 25, gameOverRec.y + 100 + 10}, 0.5, WHITE, 5, 5, 50);
            screenShake = screenShakeFrameBase + 10;
            displayTimeSurvied.particleTriggered = true;
        }
        
    }

    if(displayEnemiesKilled.triggered){
        DrawTextEx(font, TextFormat("%d", game.enemiesKilled), (Vector2){gameOverRec.x + 300, gameOverRec.y + 150}, 30, 2,WHITE);
        if(!displayEnemiesKilled.particleTriggered){
            spawnParticlesExpandingRing(&ps, (Vector2){gameOverRec.x + 300 + 10, gameOverRec.y + 150 + 5}, 0.5, WHITE, 5, 5, 50);
            screenShake = screenShakeFrameBase + 10;
            displayEnemiesKilled.particleTriggered = true;
        }
    }

    if(displayScore.triggered){
        DrawTextEx(font, TextFormat("%d", game.score), (Vector2){gameOverRec.x + 225, gameOverRec.y + 250}, 50, 3, WHITE);

        if(!displayScore.particleTriggered){
            spawnParticlesExpandingRing(&ps, (Vector2){gameOverRec.x + 225 + 10, gameOverRec.y + 250 + 10}, 0.75, WHITE, 5, 15, 50);
            screenShake = screenShakeFrameBase + 10;
            displayScore.particleTriggered = true;
        }
    }

    if(displayScore.triggered){
        quitButtonRec = (Rectangle){gameOverRec.x + 50, gameOverRec.y + 300, 150, 60};
        playAgainButtonRec = (Rectangle){gameOverRec.x + 300, gameOverRec.y + 300, 150, 60};

        DrawRectangleRounded(quitButtonRec, 0.5, 1, quitButtonColor);
        DrawRectangleRoundedLinesEx(quitButtonRec, 0.5, 1, 2.5, secondaryColor);

        DrawRectangleRounded(playAgainButtonRec, 0.5, 1, quitButtonColor);
        DrawRectangleRoundedLinesEx(playAgainButtonRec, 0.5, 1, 2.5, secondaryColor);

        DrawTextEx(font, "QUIT", (Vector2){quitButtonRec.x + 25, quitButtonRec.y + 10}, 40, 2, WHITE);
        DrawTextEx(font, "PLAY\nAGAIN", (Vector2){playAgainButtonRec.x + 25, playAgainButtonRec.y}, 30, 2, WHITE);
    }

    
    
}

//DRAWING
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
                gamePlayingDraw();
                gameDeadScreenDraw();
                drawParticles(&ps);
                break;  
            case UPGRADE_SCREEN:
                gamePlayingDraw();
                gameUpgradeScreenDraw();
                break;
            case TESTING:  
                drawParticles(&ps);
                break;
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