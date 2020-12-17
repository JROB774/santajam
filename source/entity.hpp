#pragma once

// General-purpose entity systems and functionality.

GLOBAL constexpr float ENTITY_MOVE_SPEED  = 20.0f;
GLOBAL constexpr float ENTITY_COLOR_SPEED =  4.0f;
GLOBAL constexpr float ENTITY_TURN_SPEED  = 15.0f;
GLOBAL constexpr float ENTITY_TURN_ANGLE  = 10.0f;
GLOBAL constexpr float ENTITY_HIT_ANGLE   = 30.0f;

enum Direction { DIR_N, DIR_E, DIR_S, DIR_W };

struct Entity;

typedef void(*EntityBehavior)(Entity& e);

struct Entity
{
    std::string type;

    std::string faction;

    int initiative; // Controls what entity acts first.
    int health;

    EntityBehavior behavior;

    struct { int x,y; } pos, old_pos;

    // Drawing stuff.
    struct
    {
        Vec2 pos;
        SDL_Rect clip;

        struct
        {
            float current;
            float target;
        } angle;
        struct
        {
            Vec4 current;
            Vec4 target;
        } color;
    } draw;

    // For sorting entities.
    bool operator < (const Entity& e)
    {
        return (initiative < e.initiative);
    }
};

INTERNAL void   InitEntities ();
INTERNAL void UpdateEntities (std::vector<Entity>& entities);
INTERNAL void RenderEntities (std::vector<Entity>& entities);

//
// BEHAVIORS
//

INTERNAL void Entity_BehaviorPlayer (Entity& e);
INTERNAL void Entity_BehaviorWander (Entity& e);

GLOBAL const std::map<std::string,EntityBehavior> ENTITY_BEHAVIOR
{
    { "BehaviorPlayer", Entity_BehaviorPlayer },
    { "BehaviorWander", Entity_BehaviorWander },
};
