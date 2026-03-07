#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "arena.h"

    extern Vector2 mousePos;
    extern Vector2 worldMouse;

    typedef enum{
          NOT_STARTED,
          WAVE_DISPLAY,
          WAVE_ACTIVE,
          WAVE_DONE,
    }GameplayState;

    typedef struct{
        float enemySpawnTimer;
        int levelArray[LEVEL_HEIGHT][LEVEL_WIDTH];
        int propsArray[LEVEL_HEIGHT][LEVEL_WIDTH];
        int floorArray[FlOOR_HEIGHT][FLOOR_WIDTH];
        Rectangle *colliderRecs;
        int colliderCount;

        int currentWave;

        GameplayState playState;

    }Game;

    

    void gameInit();

    void gameSetFullscreen();

    void gameUpdate();

    void gameResolutionDraw();

    void gameDraw();

    void spawnEnemies();

    void drawColliderRecs();

#endif
