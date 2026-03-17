#include "../headers/particles.h"
#include "stdio.h"


void spawnParticles(ParticleSystem *ps, Vector2 pos, float lifeMax, Color color, Vector2 velocity, float size){
    
    for(int i = 0; i < MAX_PARTICLES; i++){

        if(!ps->pool[i].active){
            ps->pool[i] = (Particle){
                .pos = pos,
                .life = lifeMax,
                .lifeMax = lifeMax,
                .color = color,
                .velocity = velocity,
                .size = size,
                .active = true,
                .type = NORMAL,
            };
            return;
        }
    }
}

void spawnParticlesExpandingRing(ParticleSystem *ps, Vector2 pos, float lifeMax, Color color, float size, float expandingRate, float ringThickness){
    
    for(int i = 0; i < MAX_PARTICLES; i++){

        if(!ps->pool[i].active){
            ps->pool[i] = (Particle){
                .active = true,
                .pos = pos,
                .life = lifeMax,
                .lifeMax = lifeMax,
                .color = color,
                .size = size,
                .expandingRate = expandingRate,
                .ringThickness = ringThickness,
                .velocity = (Vector2){0,0},
                .type = EXPANDING_RING,
            };
            return;
        }
    }
}


void updateParticles(ParticleSystem *ps){
    for(int i = 0; i < MAX_PARTICLES; i++){

        if(!ps->pool[i].active) continue;


        ps->pool[i].pos.x += ps->pool[i].velocity.x;
        ps->pool[i].pos.y += ps->pool[i].velocity.y;

        ps->pool[i].velocity.x *= 0.92f;
        ps->pool[i].velocity.y *= 0.92f;

        ps->pool[i].life -= GetFrameTime();

        if(ps->pool[i].type == EXPANDING_RING){
            ps->pool[i].size += ps->pool[i].expandingRate;
        }


        if(ps->pool[i].life <= 0.0f){
            ps->pool[i].active = false;
        }
    }
}

void drawParticles(ParticleSystem *ps){
    for(int i = 0; i < MAX_PARTICLES; i++){
        if(!ps->pool[i].active) continue;

        switch (ps->pool[i].type){
            case NORMAL:
                DrawCircleV(ps->pool[i].pos, ps->pool[i].size, ps->pool[i].color);
                break;
            case EXPANDING_RING:
                DrawRing(ps->pool[i].pos, ps->pool[i].size - (ps->pool[i].ringThickness * (ps->pool[i].life/ps->pool[i].lifeMax)), ps->pool[i].size, 0, 360, 64, ps->pool[i].color);

        }
        
    }
}
