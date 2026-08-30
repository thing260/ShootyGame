//
// Created by matt on 8/20/26.
//
#ifndef FLECTEST_BOXCOLLISIONFUNCTIONS_H
#define FLECTEST_BOXCOLLISIONFUNCTIONS_H

#include "ComponentDeclarations.h"

    float Friction(float VelValue,float Friction);
    int GetNumberForVelocityBasedOnAngle(int Angle);
    int GetAngleFromTimes(float Xtime, float Ytime, bool Xswap, bool Yswap);
    bool IsCornerHit();
    CollisionData StaticVsDynamicBox(Velocity velocity, Position PositionDynamic, Box DynamicBox, Position PositionStatic, Box StaticBox,float TimeToHit);
    CollisionData WhenDoesRayHitBox(RayRay ray,Position position,Box box,float TimeToHit);
    CollisionData DynamicBoxVsDynamicBox(Velocity velocity, Position position, Box DynamicBox, Velocity velocity2, Position position2, Box DynamicBox2,float TimeToHit);
    bool IsEarlier(CollisionData Old,CollisionData New);







#endif //FLECTEST_BOXCOLLISIONFUNCTIONS_H