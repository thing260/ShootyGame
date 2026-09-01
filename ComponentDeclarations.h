//
// Created by matt on 8/20/26.
//

#ifndef FLECTEST_COMPONENTDECLARATIONS_H
#define FLECTEST_COMPONENTDECLARATIONS_H
#include "raylib.h"
// Components
struct Health {
    float HP;
};
struct Position {
    float X;
    float Y;
};

struct Velocity {
    float X;
    float Y;
};
struct RayRay {
    Position Start;
    Velocity End;
};
struct Box {
    Vector2 Size;
    Texture2D *Texture = nullptr;
};


//Tags
struct Dynamic { //This Object is Moving and Processed in Collision as such
};
struct Static { //This Object is Not Moving and Processed in Collision as such
};
struct Player { //This Object is Controlled By the Player, Used to be a Tag, now as jump for jumping
    bool Jump = false;
};
struct CameraFocus { //This Object is the Focus Of the Camera (Only one allowed or weird shit happens)
};
struct Phantom { //Has Collision, but does not RespondBack, EI, only used to trigger events
};

//Other/Helper

struct CollisionData {
    bool Collision;
    float TimeHit;
    int AngleHit;
};



























#endif //FLECTEST_COMPONENTDECLARATIONS_H