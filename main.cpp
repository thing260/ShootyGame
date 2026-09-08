#include <cmath>
#include <iostream>
#include <flecs.h>
#include <raylib.h>
#include <filesystem>
#include "ComponentDeclarations.h"
#include "BoxCollisionFunctions.h"
using namespace std;
using namespace flecs;

//Macros
#define DELTATIME (1.0f/60.0f)
#define FRICTION 1.5f

// functions
void RenderOneBox(Position position,Box box) {
   DrawTexturePro(*box.Texture,Rectangle {0,0,(float)box.Texture->width,(float)box.Texture->height}, Rectangle {position.X,position.Y,box.Size.x,box.Size.y},Vector2 {0,0},0,WHITE);
}
void RenderBoxRepeat() {
   TODO:
}

int CurrentFrame = 0;






// We just want to have some Boxs Effected by Velocity and Rendered


int main() {
   float ScreenWidth = 1000;
   float ScreenHeight = 1000;
   SetTargetFPS(60);
   InitWindow(static_cast<int>(ScreenWidth),static_cast<int>(ScreenHeight),"ECS asdasdasdasd");


   cout << filesystem::current_path();
   Texture2D bubbles = LoadTexture("../Random/Tests/BubblesTest.png");
   if (!IsTextureValid(bubbles)) {
      cerr << "oh fuck, texture not loaded\n";
   }

   world MyWorld;

   entity MainPlayer = MyWorld.entity();
   MainPlayer.set<Position>({698,300});
   MainPlayer.set<Velocity>({100,40});
   MainPlayer.set<Box>(Box {Vector2{100,100},&bubbles});
   MainPlayer.add<Player>();
   MainPlayer.add<Dynamic>();
   MainPlayer.add<CameraFocus>();

   entity A = MyWorld.entity();
   A.set<Position>({300,-300});
   A.set<Velocity>({0,40});
   A.set<Box>(Box {Vector2{100,100},&bubbles});
   A.add<Dynamic>();
/*
   entity B = MyWorld.entity();
   B.set<Position>({0,200});
   B.set<Box>(Box {Vector2{100,200},&bubbles});
   B.add<Static>();
*/
   entity D = MyWorld.entity();
   D.set<Position>({100,-150});
   D.set<Velocity>({0,40});
   D.set<Box>(Box {Vector2{100,100},&bubbles});
   D.add<Dynamic>();

   entity C = MyWorld.entity();
   C.set<Position>({-700,400});
   C.set<Box>(Box {Vector2{1600,100},&bubbles});
   C.add<Static>();
/*
   entity E = MyWorld.entity();
   E.set<Position>({-100,100});
   E.set<Box>(Box {Vector2{400,100},&bubbles});
   E.add<Static>();
*/
   entity F = MyWorld.entity();
   F.set<Position>({800,400});
   F.set<Box>(Box {Vector2{400,100},&bubbles});
   F.add<Static>();

   // Cam needs to be attached to object
   Camera2D camera;
   camera.zoom = 1;
   camera.rotation = 0.0f;
   camera.offset = Vector2 {ScreenWidth/2,ScreenHeight/2};

   //Queries
   flecs::query<Position,Box> Rendering = MyWorld.query<Position,Box>();
   flecs::query<Position,Velocity> Moving= MyWorld.query<Position,Velocity>();
   flecs::query<Velocity,Player> Controlling = MyWorld.query<Velocity,Player>();
   flecs::query<Position,Static,Box> StaticBoxs = MyWorld.query<Position,Static,Box>();
   flecs::query<Velocity,Position,Dynamic,Box> DynamicBoxs = MyWorld.query<Velocity,Position,Dynamic,Box>();
   flecs::query<Position,Box,CameraFocus> CamerasFocus = MyWorld.query<Position,Box,CameraFocus>();
   flecs::query<Player> Players = MyWorld.query<Player>();

   flecs::query<Velocity*,Position,Box,Dynamic*,Static*> StaticAndDynamicBoxs = MyWorld.query_builder<Velocity*,Position,Box,Dynamic*,Static*>()
   .with<Velocity>().optional()
   .with<Position>()
   .with<Box>()
   .with<Dynamic>()
   .oper(Or)
   .with<Static>()
   .build();



   while (!WindowShouldClose()) {
      CurrentFrame++;
      //Controls / Velocity get set from other things
      if (IsKeyDown(KEY_Z)) {
         camera.zoom += 0.05f;
      } // Zoom in
      if (IsKeyDown(KEY_X)) {
         camera.zoom -= 0.05f;
      } // Zoom out
      Controlling.each([](Velocity &velocity, Player player) {
         if (IsKeyDown(KEY_LEFT)) {velocity.X -= 16;}
         if (IsKeyDown(KEY_UP)) {velocity.Y -= 4;}
         if (IsKeyDown(KEY_DOWN)) {velocity.Y += 4;}
         if (IsKeyDown(KEY_RIGHT)) {velocity.X += 16;}
         if (player.Jump == true && IsKeyDown(KEY_SPACE)) {velocity.Y -= 400;}
      }); //Controls
      Players.each([](Player &player) {
         player.Jump = false;
      });
      Moving.each([]( Position &Pos, Velocity &Vel) { // Gravity
         Vel.Y += 12;
      }); // Gravity

      //Collision // Movement
      float TimeLeft = DELTATIME;
      while (TimeLeft > 0) {
         CollisionData Earliest {false,INFINITY,-1};
         CollisionData Temp {false,INFINITY,-1};
         entity OuterEntity; //entities that hit
         entity InnerEntity; //entities that hit

         //Sorry for the Amount of captures, it all necessary
         StaticAndDynamicBoxs.each([DynamicBoxs,&Temp,&Earliest,&OuterEntity,&InnerEntity,&TimeLeft](flecs::entity E1,Velocity *VelocityOuter, Position &PositionOuter, Box &BoxOuter,Dynamic*,Static*) {
            DynamicBoxs.each([&VelocityOuter,&PositionOuter,&BoxOuter,&E1,&Temp,&Earliest,&OuterEntity,&InnerEntity,&TimeLeft](flecs::entity E2,Velocity &VelocityInner, Position &PositionInner, Dynamic DynamicInner, Box &BoxInner) {
               if (VelocityOuter != nullptr) { //If our outer has velocity
                  if (E1 == E2) {
                     return; //Skip interation, we are testing the same box on itself
                  }
                  else {
                     Temp = DynamicBoxVsDynamicBox(VelocityInner,PositionInner, BoxInner,*VelocityOuter,PositionOuter,BoxOuter,TimeLeft);
                  }
               }
               else {
                  //Note: we do not check if boxes are the same as its only static Vs dynamic here, so it cant be self v self;
                  Temp = StaticVsDynamicBox(VelocityInner,PositionInner,BoxInner,PositionOuter,BoxOuter,TimeLeft);
               }
               //Did Collision Math, we now check if its earlier
               if (IsEarlier(Earliest,Temp)) {
                        Earliest = Temp;
                        OuterEntity = E1;
                        InnerEntity = E2;
               }
            });
         });
         //We got are earliest hit now (supposedly)
         //And we move stuff
         if (Earliest.Collision) {
            // cout << "hit info, Angle: " << Earliest.AngleHit << " Time: " << Earliest.TimeHit << "\n";
            Moving.each([&Earliest](Position &Pos,Velocity &Vel) {
               Pos.X += (Vel.X * (Earliest.TimeHit)); // The Tiny Value added gives them a Lil room so they dont clip each other due to floating point persicion
               Pos.Y += (Vel.Y * (Earliest.TimeHit));
            });
            TimeLeft -= Earliest.TimeHit;

            // Setting Hit Entities to a velocity of 0
            int HitType = GetNumberForVelocityBasedOnAngle(Earliest.AngleHit); // We need One for StaticVDynamic, and One for DynamicVDynamic, Dynamic needs velocity to throw back
            if (OuterEntity.has<Velocity>()) { // boxs will counter act each others velocities, and give friction, and Each Have a Mass
               float Mass1 = OuterEntity.get<Box>().Size.x * OuterEntity.get<Box>().Size.y;
               float Mass2 = InnerEntity.get<Box>().Size.x * InnerEntity.get<Box>().Size.y;
               //Store Velocities so We Modified use Junk Values
               Velocity InnerVel = InnerEntity.get<Velocity>();
               Velocity OuterVel = OuterEntity.get<Velocity>();
               if (HitType == 1) { //Hit from X
                  OuterEntity.set<Velocity>( Velocity {
                     (((Mass1 - Mass2)/(Mass1 + Mass2)) * OuterVel.X + ((2*Mass2)/(Mass1 + Mass2))*InnerVel.X)/2,
                        Friction(OuterVel.Y,FRICTION)
                     }
                  );
                  InnerEntity.set<Velocity>( Velocity {
                     (((Mass2 - Mass1)/(Mass2 + Mass1)) * InnerVel.X - ((2*Mass1)/(Mass2 + Mass1))*OuterVel.X)/2,
                        Friction(InnerVel.Y,FRICTION)
                     }
                  );

               }
               else if (HitType == 0) { //Hit From Y
                  OuterEntity.set<Velocity>( Velocity {
                     Friction(OuterVel.X,FRICTION),
                     (((Mass1 - Mass2)/(Mass1 + Mass2)) * OuterVel.Y + ((2*Mass2)/(Mass1 + Mass2))*InnerVel.Y)/2
                     }
                  );
                  InnerEntity.set<Velocity>( Velocity {
                     Friction(InnerVel.X,FRICTION),
                     (((Mass2 - Mass1)/(Mass2 + Mass1)) * InnerVel.Y - ((2*Mass1)/(Mass2 + Mass1))*OuterVel.Y)/2
                     }
                  );
               }

               else { //Hit from Corner

                  OuterEntity.set<Velocity>( Velocity {
                     (((Mass1 - Mass2)/(Mass1 + Mass2)) * OuterVel.X + ((2*Mass2)/(Mass1 + Mass2))*InnerVel.X),
                        (((Mass1 - Mass2)/(Mass1 + Mass2)) * OuterVel.Y + ((2*Mass2)/(Mass1 + Mass2))*InnerVel.Y)
                     }
                  );
                  InnerEntity.set<Velocity>( Velocity {
                     (((Mass2 - Mass1)/(Mass2 + Mass1)) * InnerVel.X - ((2*Mass1)/(Mass2 + Mass1))*OuterVel.X),
                        (((Mass2 - Mass1)/(Mass2 + Mass1)) * InnerVel.Y - ((2*Mass1)/(Mass2 + Mass1))*OuterVel.Y)
                     }
                  );

               }
               cout << "OuterVelocity: " << OuterEntity.get<Velocity>().X << "," << OuterEntity.get<Velocity>().Y << "\n";
               cout << "InnerVelocity: " << InnerEntity.get<Velocity>().X << "," << InnerEntity.get<Velocity>().Y << "\n";
               cout << " : CurrentFrame: " << CurrentFrame << "\n"; ;
            }
            else { // One will give the other friction, and stop it on a axis
               if (HitType == 0) { // Hit from Y
                  InnerEntity.set<Velocity>({Friction(InnerEntity.get<Velocity>().X,FRICTION),0});
               }
               else if (HitType == 1) {
                  InnerEntity.set<Velocity>({0,Friction(InnerEntity.get<Velocity>().Y,FRICTION)});
               }
               else {
                  InnerEntity.set<Velocity>({0,0});
               }
            }
            if (Earliest.AngleHit == 90 && InnerEntity.has<Player>()) {
               InnerEntity.set<Player>(Player{true});
            }
            else if (Earliest.AngleHit == 270 && OuterEntity.has<Player>()) {
               OuterEntity.set<Player>(Player{true});
            }

         }
         else {
            Moving.each([TimeLeft](Position &Pos,Velocity &Vel) {
               Pos.X += (Vel.X * TimeLeft);
               Pos.Y += (Vel.Y * TimeLeft);
            });
            TimeLeft = 0;
         }
      }


      CamerasFocus.each([&camera](Position &position, Box &box,CameraFocus Cam) {
         camera.target = Vector2 {position.X + (box.Size.x/2),position.Y + (box.Size.y/2)};
      });
      BeginDrawing();
      BeginMode2D(camera);
      ClearBackground(RAYWHITE);


      Rendering.each([](Position &position, Box &box) {
         RenderOneBox(position,box);
      });
      EndMode2D();
      EndDrawing();
      //cout << "end of frame \n";
   }

}



//DynamicBoxes Test against each other, remove both velocities (or pushes the top one up?) leading to the cube jumping but never having its jump toggled due to "angle relativity"
// Theory, Dynamic Boxe 1 on ground resolves, its now static, on the next iteration of collision (not next frame), the dynamic box on top (dynamic 3) can hit, dynamic 2 (another one touching the floor) get resolved two, then the top box can hit both
// I might be wrong though, as it probaly should start thne, losing the 90's as it gains the 225's/270's,
//
//
