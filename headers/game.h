#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "arena.h"

    extern Vector2 mousePos;
    extern Vector2 worldMouse;

    typedef enum{
        MAIN_MENU,
        PLAYING,
        DEAD,
        UPGRADE_SCREEN,
        TESTING,
    }GameState;

    typedef struct{
        float timer;
        float delay;
        bool triggered;
        bool particleTriggered;
    }TimedEvent;


    typedef struct{
        float enemySpawnTimer;
        int levelArray[LEVEL_HEIGHT][LEVEL_WIDTH];
        int propsArray[LEVEL_HEIGHT][LEVEL_WIDTH];
        int floorArray[FlOOR_HEIGHT][FLOOR_WIDTH];
        Rectangle *colliderRecs;
        int colliderCount;

        GameState state;

        int score;
        int scoreThresholdNum;

        int enemiesKilled;
        float timeSurvived;
    }Game;

    

    void gameInit();

    void gameSetFullscreen();

    void gameUpdate();

    void resetTimedEvent(TimedEvent *event, float delay);

    bool updateTimedEvent(TimedEvent *event);

    void gameResolutionDraw();

    void gameDraw();

    void spawnEnemies();

    void drawColliderRecs();

#endif
