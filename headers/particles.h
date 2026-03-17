#ifndef PARTICLES_H
#define PARTICLES_H
#include <raylib.h>
#define MAX_PARTICLES 1024

    typedef enum{
        NORMAL,
        EXPANDING_RING,
    }ParticleType;
    
    typedef struct{
        ParticleType type;

       //Normal particle variables 
       Vector2 pos;
       bool active;
       float life;
       float lifeMax;
       Color color;
       Vector2 velocity;
       float size;

       //Expanding Specific variables
       float expandingRate;
       float ringThickness;

       
    }Particle;

    typedef struct{
        Particle pool[MAX_PARTICLES];
    }ParticleSystem;

    void spawnParticles(ParticleSystem *ps, Vector2 pos, float lifeMax, Color color, Vector2 velocity, float size);

    void spawnParticlesExpandingRing(ParticleSystem *ps, Vector2 pos, float lifeMax, Color color, float size, float expandingRate, float ringThickness);

    void updateParticles(ParticleSystem *ps);

    void drawParticles(ParticleSystem *ps);

#endif
