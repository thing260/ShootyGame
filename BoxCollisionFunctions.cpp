//
// Created by matt on 8/20/26.
//
#include "BoxCollisionFunctions.h"

#include <cmath>
#include <concepts>
#include <iostream>
#include <ostream>

#include "raylib.h"

using namespace std;
// For testing
//void RenderBoxTest(Position position,Box box) {
//    DrawRectangle(position.X,position.Y,box.Size.x,box.Size.y,GREEN);
//}

float Friction(float VelValue,float Friction) { // Add/Subs Value towards 0
    if (VelValue - Friction > 0) {
        return (VelValue - Friction);
    }
    else if (VelValue + Friction < 0) {
        return (VelValue + Friction);
    }
    else {
        return 0.0f;
    }
}
int GetNumberForVelocityBasedOnAngle(int Angle) { // 0 = Hit from Y, 1 = Hit from X, 2 = Hit from corner
    if (Angle == 90 || Angle == 270) {
        return 0;
    }
    else if (Angle == 0 || Angle == 180) {
        return 1;
    }
    else {
        return 2;
    }
}
int GetAngleFromTimes(float Xtime, float Ytime, bool Xswap, bool Yswap) { // sorry for the If Statement Hell
    if (Xswap) {
        if (Yswap) {// 11
            if (Xtime > Ytime) { // hit from X
                return 0;
            }
            if (Ytime > Xtime) { // hit from Y
                return 270;
            }
                return 315; // Hit From Corner
        }
        else { // 10
            if (Xtime > Ytime) { // hit from X
                return 0;
            }
            if (Ytime > Xtime) { // hit from Y
                return 90;
            }
                return 45; // Hit from Corner
        }
    }
    else {
        if (Yswap) { // 01
            if (Xtime > Ytime) { // hit from X
                return 180;
            }
            if (Ytime > Xtime) { // hit from Y
                return 270;
            }
            return 225; // Hit from Corner
        }
        else { // 00
            if (Xtime > Ytime) { // hit from X
                return 180;
            }
            if (Ytime > Xtime) { // hit from Y
                return 90;
            }
            return 135; // Hit from Corner
        }
    }
} // ew
bool IsCornerHit(int Angle) {
    if (Angle % 10 == 5) {
        return true;
    }
    else {
        return false;
    }
}
CollisionData StaticVsDynamicBox(Velocity velocity, Position PositionDynamic, Box DynamicBox, Position PositionStatic, Box StaticBox,float TimeToHit) {


    //RenderBoxTest(Position {PositionStatic.X - (DynamicBox.Size.x/2),PositionStatic.Y - (DynamicBox.Size.y/2)},Box {Vector2 {DynamicBox.Size.x + StaticBox.Size.x, DynamicBox.Size.y + StaticBox.Size.y}});
    return WhenDoesRayHitBox(
        RayRay {Position {PositionDynamic.x + (DynamicBox.Size.x/2),PositionDynamic.y + (DynamicBox.Size.y/2)}, velocity},
        Position {PositionStatic.x - (DynamicBox.Size.x/2),PositionStatic.y - (DynamicBox.Size.y/2)},
        Box {Vector2 {DynamicBox.Size.x + StaticBox.Size.x, DynamicBox.Size.y + StaticBox.Size.y}, nullptr},
        TimeToHit
    );
    // We need to make a Ray that starts in the middle of our DynamicBox, We then Need to make a box that is a combination of the two
}
CollisionData WhenDoesRayHitBox(RayRay ray,Position position,Box box, float TimeToHit) {
    // Get Times Hit
    float Xnear, Ynear, Xfar, Yfar;
    Xnear = (position.x - ray.Start.x)/ray.End.X;
    Ynear = (position.y - ray.Start.y)/ray.End.Y;
    Xfar = (position.x + box.Size.x - ray.Start.x)/ray.End.X;
    Yfar = (position.y + box.Size.y - ray.Start.y)/ray.End.Y;
    // Check For NAN values

    if (isnan(Xnear)) {Xnear = INFINITY;} //Due to the ways the Lines are tested, I believe this works (difference between -inf and inf)
    if (isnan(Ynear)) {Ynear = INFINITY;}
    if (isnan(Xfar)) {Xfar = -INFINITY;}
    if (isnan(Yfar)) {Yfar = -INFINITY;}

    // Swap Values incase ray comes from a different angle, also decides Angle
    bool Yswap = false;
    bool Xswap = false;
    if (Xnear > Xfar) {ranges::swap(Xnear, Xfar);
        Xswap = true;}
    if (Ynear > Yfar) {ranges::swap(Ynear, Yfar);
        Yswap = true;}


    //Test 1
    if (Xnear > Yfar || Ynear > Xfar) { // First Condition to check if something hit, if true, the boxs do not hit
        return CollisionData {false,INFINITY,-1};
    }

    //Test 2
    float ClosestTime = fmax(Xnear,Ynear);
    if (ClosestTime > TimeToHit || ClosestTime < 0) {
        return CollisionData {false,INFINITY,-1};
    }

    // After this, its more of Checking with Times, and Returning Data Back
    //cout << GetAngleFromTimes(Xnear,Ynear,Xswap,Yswap) << "\n";
    //cout << ClosestTime << "\n";
    //cout << Xnear << " , " << Ynear << "\n";
    return {
        CollisionData {
            true,
            ClosestTime,
            GetAngleFromTimes(Xnear,Ynear,Xswap,Yswap)
        }
    };
}
CollisionData DynamicBoxVsDynamicBox(Velocity velocity, Position position, Box DynamicBox, Velocity velocity2, Position position2, Box DynamicBox2,float TimeToHit) { // TODO:
    return WhenDoesRayHitBox(
        RayRay {Position {position.x + (DynamicBox.Size.x/2),position.y + (DynamicBox.Size.y/2)}, Velocity {velocity.X - velocity2.X,velocity.Y - velocity2.Y}},
        Position {position2.x - (DynamicBox.Size.x/2),position2.y - (DynamicBox.Size.y/2)},
        Box {Vector2 {DynamicBox.Size.x + DynamicBox2.Size.x, DynamicBox.Size.y + DynamicBox2.Size.y}, nullptr},
        TimeToHit
    );
}
bool IsEarlier(CollisionData Old,CollisionData New) {
     if (Old.TimeHit > New.TimeHit) {
         return true;
     }
     else if (Old.TimeHit == New.TimeHit ) { // Only could be One Hitting a Corner and One hitting a Side, or some Overlap, inwhichCase it does not matter
          if (IsCornerHit(New.AngleHit)) { // If it hit a corner, it does not get Priority at the same time hit;
              return false;
          }
          else {
             return true;
          }
     }
     else {
         return false;
     }
}