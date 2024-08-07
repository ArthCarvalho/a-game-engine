/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "screen/screen.h"

#include "scene/scene.h"

#include "spadstk.h"

#include "texture/texture.h"

#define FOG_LEVELS 16
#define FOG_SHIFT 6

//#include "particles/particles.h"

/* Temporary Static Data Files */
extern unsigned long player_tim[];
extern unsigned long player_tunic_green_tim[];
extern unsigned long player_agm[];
extern unsigned long player_anm[];
extern unsigned long sword_sg2[];
extern unsigned long shield_sgm2[];
extern unsigned long scene_tim[];
extern unsigned long iwakabe_tim[];
extern unsigned long iwakabe_far_tim[];
extern unsigned long iwayuka_tim[];
extern unsigned long deku_shrine_block000a[];
extern unsigned long deku_shrine_block000b[];
extern unsigned long deku_shrine_block000b_000[];
extern unsigned long deku_shrine_block000b_001[];
extern unsigned long deku_shrine_block000b_002[];
extern unsigned long deku_shrine_block000b_003[];
extern unsigned long scene_col[];
extern unsigned long scene_room00_sg2[];
//extern unsigned long scene_room01_sg2[];
//extern unsigned long scene_room02_sg2[];
//extern unsigned long scene_room03_sg2[];
//extern unsigned long scene_room04_sg2[];
//extern unsigned long scene_room05_sg2[];
//extern unsigned long scene_room05_sg2_ext[];
extern unsigned long screen_lifebar_tim[];
extern unsigned long main_button_bg[];
extern unsigned long main_button_labels[];
extern unsigned long button_action_labels[];
extern unsigned long action_icon_bomb[];
extern unsigned long action_icon_hookshot[];
extern unsigned long icon_256_a_16_tim[];
extern unsigned long icon_256_b_16_tim[];
extern unsigned long texture_data_start[];
extern unsigned long texture_data_end[];

// !!!BIG TODO !!!
// Fix Arena_Free() coalesce crash
// Make make ambient color calculation faster (GTE)

/*
    OBJ_PLAYER,
    OBJ_TSUBO,
    OBJ_SYOKUDAI,
    OBJ_GRASS,
    OBJ_GRASS_CUT,
    OBJ_DOOR_SHUTTER,
    OBJ_DEKUNUTS,
    OBJ_FLAME,
    OBJ_MAX
*/
void * ActorInitFuncs[] = {
  PlayerInitialize,
  ObjTsuboActorInitialize,
  ObjSyokudaiActorInitialize,
  ObjGrassActorInitialize,
  ObjGrassCutActorInitialize,
  ObjDoorShutterActorInitialize,
  ObjSwapPlaneActorInitialize,
  ObjDekunutsActorInitialize,
  ObjFlameActorInitialize,
  ObjCollectableActorInitialize
};

u_long ActorDataSizes[] = {
  sizeof(PlayerActor),
  sizeof(ObjTsuboActor),
  sizeof(ObjSyokudaiActor)+64,
  sizeof(ObjGrassActor),
  sizeof(ObjGrassCutActor),
  sizeof(ObjDoorShutterActor),
  sizeof(ObjSwapPlaneActor),
  sizeof(ObjDekunutsActor)+64, // TODO -- FIND OUT ALLOCATION BUG
  sizeof(ObjFlameActor)+64,
  sizeof(ObjCollectableActor)+64
};

/*
  //printf("Actor_SwapPlane_Init 2\n");
  {
    Actor_Descriptor newactor;
    Actor_SwapPlane_Initializer * plane_init;
    newactor.x = -2379;
    newactor.y = -4014;
    newactor.z = 0;
    newactor.rot_x = 0;
    newactor.rot_y = 0;
    newactor.rot_z = 0;
    newactor.actor_type = ACTOR_TYPE_SWAP_PLANE;

    plane_init = (Actor_SwapPlane_Initializer*)newactor.init_variables;

    plane_init->max_x = -1867;
    plane_init->max_y = 593-4096;
    plane_init->max_z = 512;
    plane_init->min_x = -2891;
    plane_init->min_y = (-430)-4096;
    plane_init->min_z = -512;
    plane_init->front = 2;
    plane_init->back = 3;


    // Allocate
    scene->transition_actors[2] = LStack_Alloc(sizeof(Actor_SwapPlane));
    ////printf("//printf: sizeof: actor=%d swapplane=%d\n",sizeof(Actor),sizeof(Actor_SwapPlane));
    // Create each transition actor
    Scene_AddActor(&Scene_ActorList[ACTOR_GROUP_DOOR], scene->transition_actors[2]);
    Actor_SwapPlane_Init(scene->transition_actors[2], &newactor, scene);
  }
  //printf("Actor_SwapPlane_Init 3 Begin\n");
  {
    Actor_Descriptor newactor;
    Actor_SwapPlane_Initializer * plane_init;
    newactor.x = 5332;
    newactor.y = (624)-4096;
    newactor.z = -3170;
    newactor.rot_x = 0;
    newactor.rot_y = 0;
    newactor.rot_z = 0;
    newactor.actor_type = ACTOR_TYPE_SWAP_PLANE;

    plane_init = (Actor_SwapPlane_Initializer*)newactor.init_variables;

    plane_init->max_x = 5844;
    plane_init->max_y = (1392)-4096;
    plane_init->max_z = -2658;
    plane_init->min_x = 4820;
    plane_init->min_y = (-255)-4096;
    plane_init->min_z = -3682;
    plane_init->front = 1;
    plane_init->back = 4;

    // Allocate
    scene->transition_actors[3] = LStack_Alloc(sizeof(Actor_SwapPlane));
    ////printf("//printf: sizeof: actor=%d swapplane=%d\n",sizeof(Actor),sizeof(Actor_SwapPlane));
    // Create each transition actor
    Scene_AddActor(&Scene_ActorList[ACTOR_GROUP_DOOR], scene->transition_actors[3]);
    Actor_SwapPlane_Init(scene->transition_actors[3], &newactor, scene);
  }*/

#define NUM_TRANSITION_ACTORS 5

Actor_Descriptor transition_actors[] = {
  { // Door Rooms 0 - 1
    7833, -4096, 0,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DOOR_SHUTTER,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 1, 0, 0, 0, 0, 0, 0, 0 // [0]Front, [1]Back Rooms
    }
  },
  { // Swap Plane Rooms 1 - 2
    2374, -4330, 0,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DOOR_SHUTTER,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      1, 2, 2864, -3502, 512, 1840, -4526, -512, 0 // [0]Front, [1]Back Rooms
    }
  },
  { // Swap Plane Rooms 2 - 3
    -2383, -4330, 0,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DOOR_SHUTTER,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      2, 3, 2864, -3502, 512, 1840, -4526, -512, 0 // [0]Front, [1]Back Rooms
    }
  },
  { // Swap Plane Rooms 1 - 4
    5332, (624)-4096, -3170,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_SWAP_PLANE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      1, 4, 5844, (1392)-4096, -2658, 4820, (-255)-4096, -3682, 0 // [0]Front, [1]Back Rooms
    }
  },
  { // Swap Plane Rooms 1 - 5
    4608, (-256)-4096, 2664,          // short x, y, z;
    0, 4096, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DOOR_SHUTTER,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      1, 1, 5844, (1392)-4096, -2658, 4820, (-255)-4096, -3682, 0 // [0]Front, [1]Back Rooms
    }
  }
};
/*
Actor_Descriptor room0_actors[] = {
  {
    8832, -4096, 0,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_TSUBO,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    8832, -4096, 256,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_GRASS_CUT,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    8832, -4096, -256,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_SYOKUDAI,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    8832+256, -4096, 0,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_GRASS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    4913, (-256)-4096, 7128,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096*3, 4096*3, 4096*3, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_SYOKUDAI,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      1, 1, 4096*0.6, 0, 0, 0, 0, 0, 0 // Color, Show Flare, Flare Scale
    }
  },
  {
    1182, (-63)-4096, 10821,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096*2.5, 4096*2.5, 4096*2.5, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_FLAME,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      3, 1, 4096*0.6, 256*20, 0, 0, 0, 0, 0 // Color, Show Flare, Flare Scale, Flare Draw Distance
    }
  },
  {
    1182, (-63)-4096, 10153,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096*2.5, 4096*2.5, 4096*2.5, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_FLAME,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      3, 1, 4096*0.6, 256*20, 0, 0, 0, 0, 0 // Color, Show Flare, Flare Scale, Flare Draw Distance
    }
  },
  {
    -3211, (-63)-4096, 15881,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096*2.5, 4096*2.5, 4096*2.5, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_FLAME,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      5, 1, 4096*0.6, 256*20, 0, 0, 0, 0, 0 // Color, Show Flare, Flare Scale, Flare Draw Distance
    }
  },
  {
    -3962, (-63)-4096, 15881,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096*2.5, 4096*2.5, 4096*2.5, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_FLAME,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      5, 1, 4096*0.6, 256*20, 0, 0, 0, 0, 0 // Color, Show Flare, Flare Scale, Flare Draw Distance
    }
  },
  {
    (256*-4.65992), (-63)-4096, (256*46.0892),          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096*2.5, 4096*2.5, 4096*2.5, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_FLAME,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      5, 1, 4096*0.6, 256*20, 0, 0, 0, 0, 0 // Color, Show Flare, Flare Scale, Flare Draw Distance
    }
  },
  {
    (256*-7.32035), (-63)-4096, (256*46.0892),          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096*2.5, 4096*2.5, 4096*2.5, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_FLAME,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      5, 1, 4096*0.6, 256*20, 0, 0, 0, 0, 0 // Color, Show Flare, Flare Scale, Flare Draw Distance
    }
  },
*/

#define ROOM0_ACTORS_COUNT 5

Actor_Descriptor room0_actors[ROOM0_ACTORS_COUNT] = {
  {
    -1132, 0, 5768,
    0, 0, 0,
    4096, 4096, 4096,
    0,
    NULL,
    OBJ_SYOKUDAI,
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    1132, 0, 5768,
    0, 0, 0,
    4096, 4096, 4096,
    0,
    NULL,
    OBJ_SYOKUDAI,
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    -2446, 0, 5121,
    0, 0, 0,
    4096, 4096, 4096,
    0,
    NULL,
    OBJ_SYOKUDAI,
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    2045, 0, -363,
    0, 0, 0,
    4096, 4096, 4096,
    0,
    NULL,
    OBJ_SYOKUDAI,
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    1282, 339, -8860,
    0, 0, 0,
    4096, 4096, 4096,
    0,
    NULL,
    OBJ_SYOKUDAI,
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  }
};
/*  (256.0*(80.9656*-1.0)), (256.0*-16.9229), (256.0*-23.3968)*/
/*  
  {
    4855, -4096-256, 10726,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(71.3411)), (256*-16.9243)-4096, (256*(-24.0123*-1.0)), //8832+512, -4096, 300,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(69.0186)), (256*-16.9243)-4096, (256*(-40.4901*-1.0)), //8832+512, -4096, -300,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(52.0915)), (256*-15.8954)-4096, (256*(-43.9153*-1.0)), //8832+512+600, -4096, 300,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(68.5318)), (256*-16.8696)-4096, (256*(-25.6965*-1.0)),          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_COLLECTABLE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(70.5337)), (256*-16.1994)-4096, (256*(-32.1814*-1.0)),          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_COLLECTABLE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(66.4614)), (256*-16.8839)-4096, (256*(-39.0038*-1.0)),          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_COLLECTABLE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(60.7749)), (256*-16.535)-4096, (256*(-44.6886*-1.0)),          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_COLLECTABLE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(52.4395)), (256*-15.8636)-4096, (256*(-42.493*-1.0)),          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_COLLECTABLE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(45.4695)), (256*-16.1472)-4096, (256*(-39.0017*-1.0)),          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_COLLECTABLE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      2, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    (256*(50.3658)), (256*-17.2402)-4096, (256*(-33.0055*-1.0)),          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    0,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_COLLECTABLE,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      2, 0, 0, 0, 0, 0, 0, 0, 0
    }
  }
};
*/
//
Actor_Descriptor room1_actors[] = {
  {
    7424, -4096, -768,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_SYOKUDAI,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    7424, -4096, 768,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_SYOKUDAI,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    2816, -4096-256, 384,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_SYOKUDAI,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    2816, -4096-256, -384,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_SYOKUDAI,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328, -4096-256, 650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328, -4096-256, -650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466, -4096-256, 650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466, -4096-256, -650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466, -4096-256, 650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466, -4096-256, -650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466+466, -4096-256, 650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466+466, -4096-256, -650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466+466+466, -4096-256, 650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466+466+466, -4096-256, -650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466+466+466+466, -4096-256, 650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  },
  {
    3328+466+466+466+466+466, -4096-256, -650,          // short x, y, z;
    0, 1024, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DEKUNUTS,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  }
  /*{
    7833, -4096, 0,          // short x, y, z;
    0, 0, 0,          // short rot_x, rot_y, rot_z;
    4096, 4096, 4096, // short scale_x, scale_y, scale_z;
    1,                // unsigned char room;
    NULL,             // unsigned char pad;
    OBJ_DOOR_SHUTTER,        // unsigned int actor_type; Actor ID
    {                 // unsigned int init_variables[9];
      0, 1, 0, 0, 0, 0, 0, 0, 0
    }
  },*/
};


Room_Data room_data[] = {
  { // Room 0
    0, ROOM_TYPE_DUNGEON, // Id, Room Type
    0,                      // Enviroment Override
    {
      { 128, 128, 128, 0 }, // Ambient Color
      { 128, 128, 128, 0 }, // Fog Color
      {
        128, 128, 128, 0, // Light Color 1
        128, 128, 128, 0, // Light Color 2
        128, 128, 128, 0  // Light Color 3
      },
      0, 0,                 // Fog Start, End
      0                     // Skybox Type
    },
    room0_actors,           // List of actor initialization parameters
    ROOM0_ACTORS_COUNT,                      // Number of actors in initialization list
    NULL,                   // Pointer to list of models in this room's background
    0,                      // Number of background models
  },
  { // Room 1
    1, ROOM_TYPE_DUNGEON, // Id, Room Type
    0,                      // Enviroment Override
    {
      { 128, 128, 128, 0 }, // Ambient Color
      { 128, 128, 128, 0 }, // Fog Color
      {
        128, 128, 128, 0, // Light Color 1
        128, 128, 128, 0, // Light Color 2
        128, 128, 128, 0  // Light Color 3
      },
      0, 0,                 // Fog Start, End
      0                     // Skybox Type
    },
    room1_actors,           // List of actor initialization parameters
    16,                      // Number of actors in initialization list
    NULL,                   // Pointer to list of models in this room's background
    0,                      // Number of background models
  },
  { // Room 2
    2, ROOM_TYPE_DUNGEON, // Id, Room Type
    0,                      // Enviroment Override
    {
      { 128, 128, 128, 0 }, // Ambient Color
      { 128, 128, 128, 0 }, // Fog Color
      {
        128, 128, 128, 0, // Light Color 1
        128, 128, 128, 0, // Light Color 2
        128, 128, 128, 0  // Light Color 3
      },
      0, 0,                 // Fog Start, End
      0                     // Skybox Type
    },
    room1_actors,           // List of actor initialization parameters
    0,                      // Number of actors in initialization list
    NULL,                   // Pointer to list of models in this room's background
    0,                      // Number of background models
  },
  { // Room 3
    3, ROOM_TYPE_DUNGEON, // Id, Room Type
    0,                      // Enviroment Override
    {
      { 128, 128, 128, 0 }, // Ambient Color
      { 128, 128, 128, 0 }, // Fog Color
      {
        128, 128, 128, 0, // Light Color 1
        128, 128, 128, 0, // Light Color 2
        128, 128, 128, 0  // Light Color 3
      },
      0, 0,                 // Fog Start, End
      0                     // Skybox Type
    },
    room0_actors,           // List of actor initialization parameters
    0,                      // Number of actors in initialization list
    NULL,                   // Pointer to list of models in this room's background
    0,                      // Number of background models
  },
  { // Room 4
    4, ROOM_TYPE_DUNGEON, // Id, Room Type
    0,                      // Enviroment Override
    {
      { 128, 128, 128, 0 }, // Ambient Color
      { 128, 128, 128, 0 }, // Fog Color
      {
        128, 128, 128, 0, // Light Color 1
        128, 128, 128, 0, // Light Color 2
        128, 128, 128, 0  // Light Color 3
      },
      0, 0,                 // Fog Start, End
      0                     // Skybox Type
    },
    room0_actors,           // List of actor initialization parameters
    0,                      // Number of actors in initialization list
    NULL,                   // Pointer to list of models in this room's background
    0,                      // Number of background models
  }
};

Scene_Data scene_data[] = {
  {                           // Scene Settings
    0,                        // Scene Type - Outdoors, House, Dungeon, etc.
    {                         // Global Enviroment Settings
      { 128, 128, 128, 0 },   // Ambient Color
      { 128, 128, 128, 0 },   // Fog Color
      {
        128, 128, 128, 0,     // Light Color 1
        128, 128, 128, 0,     // Light Color 2
        128, 128, 128, 0      // Light Color 3
      },
      0, 0,                   // Fog Start, End
      0                       // Skybox Type
    },
    transition_actors,        // Transition Actor Initialization List
    NUM_TRANSITION_ACTORS,                        // Number of transition actors
    room_data,                // List of rooms
    5,                        // Number of rooms
    (struct COL2*) scene_col                 // Map Collision Mesh
  },
};


unsigned int __DEBUG_TOGGLE;

u_char daytime_segment = 0;
u_short daytime_seg_interp = 0;
CVECTOR daytime_sky, daytime_fog, daytime_sun;

  // Daybreak
  //G.clear.r = 237;
  //G.clear.g = 192;
  //G.clear.b = 255;
  //SetFarColor(134,134,215);
  //SetBackColor(227,217,255);
#define DAYTIME_DAYBREAK_SKY_R 237
#define DAYTIME_DAYBREAK_SKY_G 192
#define DAYTIME_DAYBREAK_SKY_B 255
#define DAYTIME_DAYBREAK_FOG_R 134
#define DAYTIME_DAYBREAK_FOG_G 134
#define DAYTIME_DAYBREAK_FOG_B 215
#define DAYTIME_DAYBREAK_SUN_R 227
#define DAYTIME_DAYBREAK_SUN_G 217
#define DAYTIME_DAYBREAK_SUN_B 255

  // Sky blue
  //G.clear.r = 164;
  //G.clear.g = 209;
  //G.clear.b = 255;
  //SetFarColor(50,115,213);
  //SetBackColor(255,255,255);
#define DAYTIME_NOON_SKY_R 164
#define DAYTIME_NOON_SKY_G 209
#define DAYTIME_NOON_SKY_B 255
#define DAYTIME_NOON_FOG_R 50
#define DAYTIME_NOON_FOG_G 115
#define DAYTIME_NOON_FOG_B 213
#define DAYTIME_NOON_SUN_R 255
#define DAYTIME_NOON_SUN_G 255
#define DAYTIME_NOON_SUN_B 255

  // Dusky dusk
  //G.clear.r = 239;
  //G.clear.g = 139;
  //G.clear.b = 39;
  //SetFarColor(176,88,51);
  //SetBackColor(176,88,51);
#define DAYTIME_DUSK_SKY_R 239
#define DAYTIME_DUSK_SKY_G 139
#define DAYTIME_DUSK_SKY_B 39
#define DAYTIME_DUSK_FOG_R 176
#define DAYTIME_DUSK_FOG_G 88
#define DAYTIME_DUSK_FOG_B 51
#define DAYTIME_DUSK_SUN_R 176
#define DAYTIME_DUSK_SUN_G 88
#define DAYTIME_DUSK_SUN_B 51

  // Night
  //G.clear.r = 21;
  //G.clear.g = 27;
  //G.clear.b = 98;
  //SetFarColor(0,3,30);
  //SetBackColor(89,140,255);
#define DAYTIME_NIGHT_SKY_R 21
#define DAYTIME_NIGHT_SKY_G 27
#define DAYTIME_NIGHT_SKY_B 98
#define DAYTIME_NIGHT_FOG_R 0
#define DAYTIME_NIGHT_FOG_G 3
#define DAYTIME_NIGHT_FOG_B 15
#define DAYTIME_NIGHT_SUN_R 89
#define DAYTIME_NIGHT_SUN_G 140
#define DAYTIME_NIGHT_SUN_B 255

// Player Data
AGM_model * player_model;
ANM_ANIMATION * player_anime;
SGM2_File * player_prop_sword;

// Map Data
//Col2 * map_col;
SGM2_File * map_model[7];

// Camera
MATRIX local_identity;
MATRIX camera_matrix;
VECTOR camera_pos;
VECTOR camera_targ;

// Game Player Actor
struct PlayerActor * playerActor;
// Game Play Camera
struct Camera * camera;

// Fade states
DR_MODE fade_blend[2];
TILE fade_screen[2];
u_short fade_counter;
u_short fade_timer;
u_short fade_direction;
u_short fade_step;

// Prompt Buttons Animation
u_char action_anim_swap = 0;
u_char action_anim_counter = 0;
u_char sub_action_anim_swap = 0;
u_char sub_action_anim_counter = 0;
// Buttons animation
short button_text_anim_x[] = {
  4096, 4096*1.1, 4096*1.3, 4096*1.25
};
short button_text_anim_y[] = {
  4096, 4096*0.5, 4096*1.25, 4096*1.2
};
short button_anim_x[] = {
  4096, 4096*0.95, 4096*1.3, 4096*1.0
};
short button_anim_y[] = {
  4096, 4096*0.5, 4096*1.2, 4096*1.0
};

// Void Out Y
short void_out = -16383;
u_short void_out_active = 0;

// Particle temp
#define MAX_PARTICLE_EMITTERS 50
struct ParticleEmitter particle_list[MAX_PARTICLE_EMITTERS];
u_char active_particle_count = 0;


//char active_room = 0;


/*SGM2_File * current_room = NULL; //
SGM2_File * previous_room = NULL; /* Previous room rendering, if null, skip rendering
                                   * This is used in the transition between two rooms
                                   * so that the previous room shows up with the current
                                   * while transitioning between two different rooms
                                   * so that a hole won't show in the place of the previous */



/* Profiling Data */
u_long prof_start;
u_long prof_end;
u_long prof_update;
u_long prof_draw_bg;
u_long prof_draw_player;
u_long prof_last_counter = 0;
u_long prof_current_val = 0;
u_long prof_start_last;
u_long prof_frametime;

#define ROOT_COUNTER RCntCNT1

#define MAP_TEXTURE_CLUT_X 512
#define MAP_TEXTURE_CLUT_Y 496

RECT anim_tex_flame_src;
RECT anim_tex_flame_dest;

u_int scene_counter;
//u_char cinema_mode_anim[3] = {15, 24 ,30};
// 4 frames
u_char cinema_mode_anim[4] = {10, 20, 25,30};

/* Debug Flags */
int sp_mode = 1;
unsigned long * test_pointer;

#define NUM_TEST_OBJ_SYOKUDAI 10

//u_char test_syokudai_indices[NUM_TEST_OBJ_SYOKUDAI] = {0, 1, 3, 5, 6, 8, 11, 12, 14, 15};

//ObjSyokudaiActor test_obj_syokudai[NUM_TEST_OBJ_SYOKUDAI];
//ObjTsuboActor test_obj_tsubo;

ActorList Scene_ActorList[ACTOR_GROUP_MAX];

// Init Settings, must be 36 bytes
typedef struct Actor_SwapPlane_Initializer {
  short min_x;
  short min_y;
  short min_z;
  short max_x;
  short max_y;
  short max_z;
  unsigned char front;
  unsigned char back;
  short pad0;
  unsigned int padPad[5];
} Actor_SwapPlane_Initializer;

Scene_Ctx * scene;

void SceneInitialize() {
  void * dataptr;
  unsigned long datasize;
  //printf("SceneInitialize\n");
  // The linear stack heap is initialized here because
  // when overlays are implemented it needs to tell
  // where the free memory for that overlay starts and end
  // and create the heap from there.
  // Calling LStack_Alloc before this will cause undefined behavior
  // (actually it will probably crash the game)
  u_long ram_bytes = (RAM_SIZE - (((u_long)&__heap_start) & 0xFFFFFF));
  __heap_size = ram_bytes - __stack_size;
  //printf("__heap_start 0x%0X, __heap_size 0x%0X\n", &__heap_start, __heap_size);
  //printf("Total heap size: %dKB (%d bytes) %d\n",ram_bytes >> 10, ram_bytes, ((u_long)&__heap_start) & 0xFFFFFF);
  
  EnterCriticalSection();
  LStack_InitArea(&__heap_start, __heap_size);
  ExitCriticalSection(); 



  // Load player model
  dataptr = LStack_GetTail(); // Since this is linear allocation, we load first
                             // then allocate later, this way we don't need to waste
                             // space, since CD loading always load in CD sector sizes
  
  // Load player texture to VRAM, no allocation since it will
  // reside only in VRAM for the duration of the game runtime
  //file_load_temp_noalloc("\\DATA\\PLAYER.TIM;1", dataptr); // This one does not need allocation
  // Upload to VRAM
  //load_tex_noclut_pos((unsigned long)dataptr, 0, 256, 0, 0);
  load_tex_noclut_pos((unsigned long)player_tim, 0, 256, 0, 0);
  DrawSync(0); // Wait for the transfer to end

  load_tex_noclut_pos((unsigned long)player_tunic_green_tim,0,510,0,0);

  //datasize = file_load_temp_noalloc("\\DATA\\PLAYER.AGM;1", dataptr);
  //player_model = LStack_Alloc(datasize);
  player_model = (AGM_model*)player_agm;

  //dataptr = LStack_GetTail();
  //datasize = file_load_temp_noalloc("\\DATA\\PLAYER.ANM;1", dataptr);
  //player_anime = LStack_Alloc(datasize);
  player_anime = (ANM_ANIMATION*)player_anm;

  //dataptr = LStack_GetTail();
  //datasize = file_load_temp_noalloc("\\DATA\\SWORD0.SG2;1", dataptr);
  //player_prop_sword = LStack_Alloc(datasize);
  player_prop_sword = (SGM2_File*)sword_sg2;

  shield_prop = (SGM2_File*)shield_sgm2;

  

  // Initialize player model
  PlayerSetupData((u_long*)player_model, (u_long*)player_anime);
  PlayerInitialize((Actor*)&playerActor);

  player_prop_sword = SGM2_LoadFile((u_long*)player_prop_sword);
  player_prop_sword->material[0].clut = GetClut(MAP_TEXTURE_CLUT_X, MAP_TEXTURE_CLUT_Y);

  shield_prop = SGM2_LoadFile((u_long*)shield_prop);
  shield_prop->material[0].tpage = getTPage(0, 0, 0, 256);
  shield_prop->material[0].clut = GetClut(0, 511);
  shield_prop->material[1].tpage = getTPage(0, 0, 0, 256);
  shield_prop->material[1].clut = GetClut(0, 506);

  fade_screen[0].x0 = 0;
  fade_screen[0].y0 = 0;
  fade_screen[0].w = SCREEN_W;
  fade_screen[0].h = SCREEN_H;
  fade_screen[0].b0 = fade_screen[0].g0 = fade_screen[0].r0 = 0;

  fade_screen[1] = fade_screen[0];
  
  setTile(&fade_screen[0]);
  setSemiTrans(&fade_screen[0], 1);

  setTile(&fade_screen[1]);
  setSemiTrans(&fade_screen[1], 1);

  //setDrawTPage(&fade_blend, 1, 1, getTPage(0, 2, 0, 0));
  setDrawMode(&fade_blend[0], 0, 1, getTPage(0, 2, 0, 0), 0);
  setDrawMode(&fade_blend[1], 0, 1, getTPage(0, 1, 0, 0), 0);

  fade_timer = 30;
  fade_counter = fade_timer;
  fade_direction = 0;
  fade_step = 4096 / fade_timer;

  // Save stack pointer so we can restore it when changing scenes
  LStack_SaveCurrentPosition();
  //printf("Total free memory: %dKB (load start)\n",LStack_GetFree() >> 10);
  // Load initial scene data
  SceneLoad(scene_data);
}

// Sets up and loads a new scene/map area
void SceneLoad(Scene_Data * scene_data) {
  void * dataptr;
  unsigned long datasize;

  scene = LStack_Alloc(sizeof(Scene_Ctx)); // Allocate scene structure

  scene->data = scene_data;

  scene->cinema_mode = 0;
  scene->cinema_mode_counter = 0;
  scene->interface_fade = 0;
  scene->interface_fade_counter = 0;

  // Setup scene data
  // Init Actor List
  for(int i = 0; i < ACTOR_GROUP_MAX; i++) {
    Scene_ActorList[i].length = 0;
    Scene_ActorList[i].start = NULL;
    Scene_ActorList[i].end = NULL;
  }

  for(int i = 0; i < MAX_PARTICLE_EMITTERS; i++) {
    particle_list[i].flags = 0;
  }

  scene->room_swap = 0;

  // Load scene data
  //dataptr = LStack_GetTail();

  //datasize = file_load_temp_noalloc("\\DATA\\scene.cOL;1", dataptr);
  //map_col = LStack_Alloc(datasize);
  //map_col = (Col2*)scene_col;

  // Load starting room model
  //dataptr = LStack_GetTail();
  //datasize = file_load_temp_noalloc("\\DATA\\ROOM00.SG2;1", dataptr);
  //map_model = LStack_Alloc(datasize);
  map_model[0] = (SGM2_File*)scene_room00_sg2;
  //map_model[1] = (SGM2_File*)scene_room01_sg2;
  //map_model[2] = (SGM2_File*)scene_room02_sg2;
  //map_model[3] = (SGM2_File*)scene_room03_sg2;
  //map_model[4] = (SGM2_File*)scene_room04_sg2;
  //map_model[5] = (SGM2_File*)scene_room05_sg2;
  //map_model[6] = (SGM2_File*)scene_room05_sg2_ext;

  //printf("Col_LoadFile\n");


  // Initialize collision
  //map_col = Col_LoadFile((u_long*)map_col);

  scene_data->collision_mesh = Col_LoadFile((u_long*)scene_data->collision_mesh);

  //printf("collision addr: %x vertices: %x faces: %x planes: %x\n",scene_data->collision_mesh,scene_data->collision_mesh->vertices,scene_data->collision_mesh->faces,scene_data->collision_mesh->planes);
  //printf("vert num: %d face num: %d plane num: %d\n",scene_data->collision_mesh->vertex_num, scene_data->collision_mesh->face_num, scene_data->collision_mesh->plane_num);

  //printf("map: vert num %d (%d bytes)\n",map_model[4]->vertex_count, map_model[4]->vertex_count * sizeof(SVECTOR));
  // Initialize Transform Buffer
  //AGM_Init(map_model->vertex_count * sizeof(SVECTOR)); // Initialize to the size of the largest model to be transformed
  AGM_Init(2000 * sizeof(SVECTOR));

  // Initialize Map Geometry
  map_model[0] = SGM2_LoadFile((u_long*)map_model[0]);
  /*map_model[1] = SGM2_LoadFile((u_long*)map_model[1]);
  map_model[2] = SGM2_LoadFile((u_long*)map_model[2]);
  map_model[3] = SGM2_LoadFile((u_long*)map_model[3]);
  map_model[4] = SGM2_LoadFile((u_long*)map_model[4]);
  map_model[5] = SGM2_LoadFile((u_long*)map_model[5]);
  map_model[6] = SGM2_LoadFile((u_long*)map_model[6]);*/

  /*for(int i = 0; i < 5; i++) {
    map_model[i]->material[0].clut = GetClut(MAP_TEXTURE_CLUT_X, MAP_TEXTURE_CLUT_Y);
    map_model[i]->material[0].tpage = getTPage(1, 0, 0, 0);
  }*/

  /*map_model[5]->material[0].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y);
  map_model[5]->material[0].tpage = getTPage(0, 0, 128, 0);
  //map_model[5]->material[1].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32+16, MAP_TEXTURE_CLUT_Y);
  map_model[5]->material[1].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y);
  map_model[5]->material[1].tpage = getTPage(0, 0, 128, 0);
  map_model[6]->material[0].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y+8);
  map_model[6]->material[0].tpage = getTPage(0, 0, 128, 0);
  map_model[6]->material[1].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y+10);
  map_model[6]->material[1].tpage = getTPage(0, 0, 128, 0);
  //map_model[6]->material[2].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32+16, MAP_TEXTURE_CLUT_Y+13);
  map_model[6]->material[2].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y+13);
  map_model[6]->material[2].tpage = getTPage(0, 0, 128, 0);

  map_model[5]->material[0].clut = GetClut(128, 496);
  map_model[5]->material[0].tpage = getTPage(1, 0, 128, 0);
  map_model[5]->material[1].clut = GetClut(128, 496-17);
  map_model[5]->material[1].tpage = getTPage(1, 0, 128+128, 0);

  /*map_model[5]->material[2].clut = GetClut(128, 496-17);
  map_model[5]->material[2].tpage = getTPage(1, 0, 128+128, 0);*/
  /*map_model[5]->material[3].clut = GetClut(128, 496);
  map_model[5]->material[3].tpage = getTPage(1, 0, 128, 0);*/
  /*map_model[5]->material[2].clut = GetClut(128+256, 496);
  map_model[5]->material[2].tpage = getTPage(0, 0, 128+128+32, 0);
  map_model[5]->material[3].clut = GetClut(128+256, 496-17);
  map_model[5]->material[3].tpage = getTPage(0, 0, 128+128-32, 0);

  map_model[5]->material[4].clut = GetClut(384+16, 496);
  map_model[5]->material[4].tpage = getTPage(0, 0, 128+128+32, 0);

  map_model[5]->material[5].clut = GetClut(384+16+16, 496);
  map_model[5]->material[5].tpage = getTPage(0, 0, 128+128+32, 64);*/

  map_model[0]->material[0].clut = GetClut(128, 496);
  map_model[0]->material[0].tpage = getTPage(1, 0, 128, 0);
  map_model[0]->material[1].clut = GetClut(128, 496-17);
  map_model[0]->material[1].tpage = getTPage(1, 0, 128+128, 0);

  //map_model[0] = map_model[5];

  

  //map_model[6]->material[1].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y+10);
  //map_model[6]->material[1].tpage = getTPage(0, 0, 128, 0);

  // map_col

  // Initialize player object
  playerActor = LStack_Alloc(sizeof(struct PlayerActor));
  PlayerCreateInstance((Actor*)playerActor, scene_data->collision_mesh);

  scene->player = (Actor*)playerActor;

  // Initialize camera
  camera = LStack_Alloc(sizeof(struct Camera));
  Camera_Create(camera, scene_data->collision_mesh);
  camera->target_offset.vy = 256;
  Camera_SetTarget(camera, (Actor*)playerActor);
  Camera_SetStartValues(camera);
  Camera_Update(camera, scene);
  scene->camera = camera;

  //G.clear.r = 98;
  //G.clear.g = 66;
  G.clear.b = 30;

  G.clear.r = 0x00;
  G.clear.g = 0x00;
  G.clear.b = 0x00;

  //G.clear.r = 50;
  //G.clear.g = 115;
  //G.clear.b = 213;

  


  // Initialize transition objects
  // Set Rooms
  scene->previous_room_id =  scene->current_room_id = 0;
  scene->current_room_m = map_model[scene->current_room_id];
  scene->previous_room_m = NULL;
  //printf("Total free memory: %dKB (heap area)\n",LStack_GetFree() >> 10);

  {
    // Initialize actor memory area
    unsigned long arena_size = LStack_GetFree();
    unsigned long * arena_start;
    unsigned long * testptr;
    arena_size = (arena_size >> 2) << 2;
    arena_start = LStack_Alloc(arena_size);
    Arena_Init(arena_start, arena_size);
    //printf("arena size: %x\n",arena_size);
    //printf("arena start: %x\n", _arena_start_ptr);
    //test_pointer = Arena_Malloc(1);
    //*test_pointer = 0xFCFCFCFC;
    ////printf("malloc(1): %x (content: %x)\n", test_pointer, *test_pointer);
    //testptr = Arena_Malloc(1);
    //*testptr = 0xFBFBFBFB;
    ////printf("malloc(1): %x (content: %x)\n", testptr, *testptr);
  }

  for(int i = 0; i < scene_data->transition_count; i++) {
    Actor_Descriptor * actdesc = &scene_data->transition_init[i];
    Scene_CreateActor(actdesc, ACTOR_GROUP_DOOR, scene);
  }
  // Load map (global) textures
  //file_load_temp_noalloc("\\DATA\\TEST.TIM;1", dataptr);
  //SetFarColor(255,255,255);
  //SetFarColor(128,128,128);
  //SetFarColor(98,66,30);
  // Daybreak
  //G.clear.r = 237;
  //G.clear.g = 192;
  //G.clear.b = 255;
  SetFarColor(134,134,215);
  SetBackColor(227,217,255);
  SetFarColor(164/2, 205/2, 230/2);
  SetFarColor(73, 24, 98);
  // Sky blue
  //G.clear.r = 164;
  //G.clear.g = 209;
  //G.clear.b = 255;
  //SetFarColor(50,115,213);
  //SetBackColor(255,255,255);
  // Dusky dusk
  //G.clear.r = 239;
  //G.clear.g = 139;
  //G.clear.b = 39;
  //SetFarColor(176,88,51);
  //SetBackColor(176,88,51);
  // Night
  //G.clear.r = 21;
  //G.clear.g = 27;
  //G.clear.b = 98;
  //SetFarColor(0,3,30);
  //SetBackColor(89,140,255);
  //load_texture_pos_fog((unsigned long)dataptr, 0, 0, 512, 496, FOG_LEVELS);
  //load_texture_pos_fog((unsigned long)scene_tim, 0, 0, MAP_TEXTURE_CLUT_X, MAP_TEXTURE_CLUT_Y, FOG_LEVELS);
  //load_texture_pos_fog((unsigned long)iwakabe_tim, 128, 0, MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y, FOG_LEVELS);
  load_texture_pos_fog((unsigned long)deku_shrine_block000a, 128, 0, 128, 496,  FOG_LEVELS);
  load_texture_pos_fog((unsigned long)deku_shrine_block000b, 128+128, 0, 128, 496-17,  FOG_LEVELS);
  load_texture_pos_fog((unsigned long)deku_shrine_block000b_000, 128+128, 128, 384, 496,  FOG_LEVELS);
  load_texture_pos_fog((unsigned long)deku_shrine_block000b_001, 128+128-32, 0, 384, 496-17,  FOG_LEVELS);
  load_texture_pos_fog((unsigned long)deku_shrine_block000b_002, 128+128+32, 0, 384+16, 496,  FOG_LEVELS);
  load_texture_pos_fog((unsigned long)deku_shrine_block000b_003, 128+128+32, 64, 384+16+16, 496,  FOG_LEVELS);
  //load_texture_pos_fog((unsigned long)iwayuka_tim, 128+32, 0, MAP_TEXTURE_CLUT_X+256+32+16, MAP_TEXTURE_CLUT_Y, FOG_LEVELS);
  //load_tex_noclut_pos((unsigned long)iwakabe_far_tim, 128+32+16, 0, 0, 0);
  // Load animated fire textures
  // Syokudai
  load_texture_pos((unsigned long)obj_flame_tim, FLAME_TEX_X_SRC, FLAME_TEX_Y_SRC, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y);
  load_tex_noclut_pos((unsigned long)obj_flame_pal_tim, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+1, 0, 0);
  load_texture_pos((unsigned long)obj_syokudai_tim, 36, 256, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+6);
  // Tsubo
  load_texture_pos((unsigned long)obj_tsubo_tim, 36, 320, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+7);
  load_texture_pos((unsigned long)obj_grass_tim, 36, 384, FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y);
  load_texture_pos((unsigned long)obj_grass_cut_tim, 36, 416, FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y+2);
  // Door Textures
  load_texture_pos((unsigned long)obj_dangeon_door_tim, 384, 0, 784, 500);

  load_texture_pos((unsigned long)screen_lifebar_tim, LIFEMETER_TEX_X, LIFEMETER_TEX_Y, LIFEMETER_CLUT_X, LIFEMETER_CLUT_Y);

  load_texture_pos((unsigned long)main_button_bg, SCR_BUTTON_BACK_X, SCR_BUTTON_BACK_Y, SCR_BUTTON_BACK_CLUT_X, SCR_BUTTON_BACK_CLUT_Y);
  load_texture_pos((unsigned long)main_button_labels, SCR_BUTTON_LABELS_X, SCR_BUTTON_LABELS_Y, SCR_BUTTON_LABELS_CLUT_X, SCR_BUTTON_LABELS_CLUT_Y);
  load_texture_pos((unsigned long)button_action_labels, SCR_ACTION_LABELS_X, SCR_ACTION_LABELS_Y, SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y);
  load_texture_pos((unsigned long)action_icon_bomb, SCR_ACTION_BOMB_X, SCR_ACTION_BOMB_Y, SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2);
  load_texture_pos((unsigned long)action_icon_hookshot, SCR_ACTION_HOOKSHOT_X, SCR_ACTION_HOOKSHOT_Y, SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+4);
  load_texture_pos((unsigned long)icon_256_a_16_tim, SCR_ACTION_HOOKSHOT_X, SCR_ACTION_HOOKSHOT_Y+23, SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+6);
  load_texture_pos((unsigned long)icon_256_b_16_tim, SCR_ACTION_HOOKSHOT_X, SCR_ACTION_HOOKSHOT_Y+23+42, SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+8);

  anim_tex_flame_src = (RECT) {FLAME_TEX_X_SRC, FLAME_TEX_Y_SRC, FLAME_TEX_W, FLAME_TEX_H};
  anim_tex_flame_dest = (RECT) {FLAME_TEX_X, FLAME_TEX_Y, FLAME_TEX_W, FLAME_TEX_H*2};
  
  // Load Models
  // Syokudai
  obj_flame_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_flame_sgm2);
  obj_flame_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y);
  obj_flame_model->material[0].tpage = getTPage(0, 1, FLAME_TEX_X, FLAME_TEX_Y);
  SGM2_OffsetTexCoords(obj_flame_model, (FLAME_TEX_X*4-1) & 0xFF, (FLAME_TEX_Y) & 0xFF);
  obj_flame_high_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_flame_high_sgm2);
  obj_flame_high_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y);
  obj_flame_high_model->material[0].tpage = getTPage(0, 1, FLAME_TEX_X, FLAME_TEX_Y);
  SGM2_OffsetTexCoords(obj_flame_high_model, (FLAME_TEX_X*4-1) & 0xFF, (FLAME_TEX_Y) & 0xFF);
  obj_syokudai_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_syokudai_sgm2);
  obj_syokudai_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+6);
  obj_syokudai_model->material[0].tpage = getTPage(0, 0, 36, 256);
  SGM2_OffsetTexCoords(obj_syokudai_model, (36*4-1) & 0xFF, (256) & 0xFF);
  // Tsubo
  //printf("Loading Obj Tsubo\n");
  obj_tsubo_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_tsubo_sgm2);
  obj_tsubo_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+7);
  obj_tsubo_model->material[0].tpage = getTPage(0, 0, 36, 320);
  SGM2_OffsetTexCoords(obj_tsubo_model, (36*4) & 0xFF, (320) & 0xFF);
  obj_tsubo_far_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_tsubo_low_sgm2);
  obj_tsubo_far_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+7);
  obj_tsubo_far_model->material[0].tpage = getTPage(0, 0, 36, 320);
  SGM2_OffsetTexCoords(obj_tsubo_far_model, (36*4) & 0xFF, (320) & 0xFF);

  // Load temp model
  /*obj_tsubo_far_model = (struct SGM2 *) SGM2_LoadFile((u_long*)test_obj_chara_sgm2);
  obj_tsubo_far_model->material[0].clut = GetClut(512, 241);
  obj_tsubo_far_model->material[0].tpage = getTPage(1, 0, 128, 0);*/
  //SGM2_OffsetTexCoords(obj_tsubo_far_model, (36*4) & 0xFF, (320) & 0xFF);

  obj_grass_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_grass_sgm2);
  obj_grass_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y);
  obj_grass_model->material[0].tpage = getTPage(0, 0, 36, 384);
  SGM2_OffsetTexCoords(obj_grass_model, (36*4) & 0xFF, (384) & 0xFF);
  obj_grass_far_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_grass_low_sgm2);
  obj_grass_far_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y);
  obj_grass_far_model->material[0].tpage = getTPage(0, 0, 36, 384);
  SGM2_OffsetTexCoords(obj_grass_far_model, (36*4) & 0xFF, (384) & 0xFF);
  obj_grass_far2_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_grass_low2_sgm2);
  obj_grass_far2_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y);
  obj_grass_far2_model->material[0].tpage = getTPage(0, 0, 36, 384);
  SGM2_OffsetTexCoords(obj_grass_far2_model, (36*4) & 0xFF, (384) & 0xFF);

  // Cutable grass  load_texture_pos((unsigned long)obj_grass_cut_tim, 36, 416, FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y+2);
  obj_grass_cut_full_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_grass_cut_full_sgm2);
  obj_grass_cut_full_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y+2);
  obj_grass_cut_full_model->material[0].tpage = getTPage(0, 0, 36, 384);
  SGM2_OffsetTexCoords(obj_grass_cut_full_model, (36*4) & 0xFF, (416) & 0xFF);
  obj_grass_cut_half_model = (struct SGM2 *) SGM2_LoadFile((u_long*)obj_grass_cut_half_sgm2);
  obj_grass_cut_half_model->material[0].clut = GetClut(FLAME_TEX_CLUT_X+16, FLAME_TEX_CLUT_Y+2);
  obj_grass_cut_half_model->material[0].tpage = getTPage(0, 0, 36, 384);
  SGM2_OffsetTexCoords(obj_grass_cut_half_model, (36*4) & 0xFF, (416) & 0xFF);

  Draw_SetupFlame();
  Draw_SetupContactShadow();
  Lights_Initialize();
  // Load Enemies
  ObjCollectableActorSetup();
  //ObjSyokudaiActorSetup();
  ObjDekunutsActorSetup(NULL, scene);

  Scene_ActorList[ACTOR_GROUP_PLAYER].length = 1;
  Scene_ActorList[ACTOR_GROUP_PLAYER].start = (Actor*)playerActor;
  Scene_ActorList[ACTOR_GROUP_PLAYER].end = (Actor*)playerActor;

  // Init Actors
  /*for(int i = 0; i < NUM_TEST_OBJ_SYOKUDAI; i++) {
    // Move this to ObjSyokudaiActorInitialize
    ObjSyokudaiActor * actor = &test_obj_syokudai[i];
    actor->flame_color = 1;//1+ (rand() % 5);
    actor->flame_rand = rand() >> 9;
    actor->flame_timer = -1;
    actor->base.pos.vx = 3912 + (rand() >> 4);
    actor->base.pos.vy = -4096-256;//(rand() >> 8)-4096-200;
    actor->base.pos.vz = (rand() >> 4)-1024;
    actor->base.rot.vx = 0;
    actor->base.rot.vy = 0;
    actor->base.rot.vz = 0;
    ObjSyokudaiActorInitialize((Actor*)actor, NULL, scene);
  }*/
  // NUM_TEST_OBJ_SYOKUDAI
  /*for(int i = 0; i < 100; i++) {
    // Move this to ObjSyokudaiActorInitialize
    ObjSyokudaiActor * actor = Scene_AllocActor(&Scene_ActorList[ACTOR_GROUP_BG], ACTOR_GROUP_BG, sizeof(ObjSyokudaiActor));
    actor->base.room = 1;
    actor->flame_color = 1;//1+ (rand() % 5);
    actor->flame_rand = rand() >> 9;
    actor->flame_timer = -1;
    actor->base.pos.vx = 3912 + (rand() >> 4);
    actor->base.pos.vy = -4096-256;//(rand() >> 8)-4096-200;
    actor->base.pos.vz = (rand() >> 4)-1024;
    actor->base.rot.vx = 0;
    actor->base.rot.vy = 0;
    actor->base.rot.vz = 0;
    ObjSyokudaiActorInitialize((Actor*)actor, NULL, scene);
  }*/

  // initializer
  Scene_LoadRoom(&room_data[0], scene);


  u_long actptr;
  /*
  for(int i = 0; i < 1; i++) {
    ObjTsuboActor * actor = Scene_AllocActor(&Scene_ActorList[ACTOR_GROUP_BG], ACTOR_GROUP_BG, sizeof(ObjTsuboActor));
    actor->base.room = 0;
    //actor->model = obj_tsubo_model;
    actor->base.pos.vx = 8832;
    actor->base.pos.vy = -4096;
    actor->base.pos.vz = 0;
    actor->base.rot.vx = 0;
    //actor->base.rot.vy = 3072;
    actor->base.rot.vy = 0;
    actor->base.rot.vz = 0;
    //playerActor->base.child = (Actor*)actor;
    actptr = (u_long)actor;
    ObjTsuboActorInitialize((Actor*) actor, NULL, scene);
  }*/

  {
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTOR_GROUP_BG], ACTOR_GROUP_BG, sizeof(ObjGrassCutActor));
      actor->base.room = 0;
      actor->base.pos.vx = 2013;
      actor->base.pos.vy = -3763;
      actor->base.pos.vz = -23509;
      actor->base.rot.vx = 0;
      actor->base.rot.vy = rand() >> 4;
      actor->base.rot.vz = 0;
      ObjGrassCutActorInitialize((Actor*)actor, NULL, scene);
  }

  {
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTOR_GROUP_BG], ACTOR_GROUP_BG, sizeof(ObjGrassCutActor));
      actor->base.room = 0;
      actor->base.pos.vx = 1862;
      actor->base.pos.vy = -3763;
      actor->base.pos.vz = -22836;
      actor->base.rot.vx = 0;
      actor->base.rot.vy = rand() >> 4;
      actor->base.rot.vz = 0;
      ObjGrassCutActorInitialize((Actor*)actor, NULL, scene);
  }

  {
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTOR_GROUP_BG], ACTOR_GROUP_BG, sizeof(ObjGrassCutActor));
      actor->base.room = 0;
      actor->base.pos.vx = 6570;
      actor->base.pos.vy = -3763;
      actor->base.pos.vz = -23650;
      actor->base.rot.vx = 0;
      actor->base.rot.vy = rand() >> 4;
      actor->base.rot.vz = 0;
      ObjGrassCutActorInitialize((Actor*)actor, NULL, scene);
  }

  {
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTOR_GROUP_BG], ACTOR_GROUP_BG, sizeof(ObjGrassCutActor));
      actor->base.room = 0;
      actor->base.pos.vx = 6782;
      actor->base.pos.vy = -3763;
      actor->base.pos.vz = -23163;
      actor->base.rot.vx = 0;
      actor->base.rot.vy = rand() >> 4;
      actor->base.rot.vz = 0;
      ObjGrassCutActorInitialize((Actor*)actor, NULL, scene);
  }


  //printf("Last actor pointer: 0x%0X\n", actptr);  
  scene->draw_dist = 1024;

  scene->ambient.r = 207>>1;
  scene->ambient.g = 191>>1;
  scene->ambient.b = 139>>1;

  //printf("Texture Data Start: 0x%08X End: 0x%08X (Size: %d)\n", texture_data_start, texture_data_end, texture_data_end - texture_data_start);
}
    ////printf("a actor: 0x%08X <<prev [0x%08X] next>> 0x%08X\n", current->prev, current, current->next);
/* __attribute__((optimize("O0"))) */
/* __attribute__((optimize("Os"))) */
void Scene_RemoveOldActors(int actor_group, Scene_Ctx * scene) {
  for(Actor * c = Scene_ActorList[actor_group].start; c; c = c->next) {
    if(c->room == scene->current_room_id) continue;
    Scene_RemoveActor(&Scene_ActorList[actor_group], c);
    c->Destroy(c, scene);
    ////printf("removing actor 0x%08X\n", c);
    Arena_Free(c);
  }
}

int bone_select = 0;

void SceneMain() {
  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_start = prof_current_val;
  prof_last_counter = prof_current_val;


  if(g_pad_press & PAD_R2) {
    Actor * current = Scene_ActorList[ACTOR_GROUP_BG].start;
    int idx = 0;
    while(current != NULL) {
      //printf("list[%d] actor->prev = 0x%0X actor->next = 0x%0X\n", idx++, current->prev, current->next);
      current = current->next;
    }
  }

  if((scene->current_room_id != scene->previous_room_id) && scene->room_swap) {
    scene->current_room_m = map_model[scene->current_room_id];
    scene->previous_room_m = map_model[scene->previous_room_id];
  }


  if(scene->room_swap) {
    scene->room_swap = 0;
    Scene_LoadRoom(&room_data[scene->current_room_id], scene);
  }

  if(scene->actor_cleanup) {
    Scene_RemoveOldActors(ACTOR_GROUP_BG, scene);
    scene->actor_cleanup = 0;
  }
  PlayerUpdate(scene->player, scene);

  Camera_Update(camera, scene);


  if(playerActor->base.pos.vy < void_out && fade_counter == 0 && !void_out_active) {
    fade_timer = 20;
    fade_counter = fade_timer;
    fade_step = 4096 / fade_timer;
    fade_direction = 1;
    void_out_active = 1;
    playerActor->void_out = 1;
  }


  if(fade_counter > 0) {
    u_short fade = fade_step * fade_counter;
    u_short fade2;
    if(fade > 4096) fade = 4096;
    fade2 = fade;

    if(!fade_direction) {
      fade = fix12_lerp(0, 255, fade);
      fade2 = fix12_lerp(0, 128, fade2);
    } else {
      fade = fix12_lerp(255, 0, fade);
      fade2 = fix12_lerp(255, 0, fade2);
    }
    fade_screen[0].r0 = fade_screen[0].g0 = fade_screen[0].b0 = fade;
    fade_screen[1].r0 = fade_screen[1].g0 = fade_screen[1].b0 = fade2;
    fade_counter--;
  }

  if(void_out_active){
    if(fade_counter == 0) {
      //scene->current_room_id = 0;
      PlayerCreateInstance((Actor*)playerActor, scene_data->collision_mesh);
      playerActor->base.pos.vx = playerActor->x_position = playerActor->last_floor_pos_x;
      playerActor->base.pos.vy = playerActor->y_position = playerActor->last_floor_pos_y;
      playerActor->base.pos.vz = playerActor->z_position = playerActor->last_floor_pos_z;
      playerActor->x_position <<= 12;
      playerActor->y_position <<= 12;
      playerActor->z_position <<= 12;
      playerActor->y_move_dir = playerActor->base.rot.vy = playerActor->y_rotation = playerActor->last_floor_dir;
      Camera_Create(camera, scene_data->collision_mesh);
      void_out_active = 0;
      fade_counter = fade_timer;
      camera->target_offset.vy = 256;
      Camera_SetTarget(camera, (Actor*)playerActor);
      Camera_SetStartValues(camera);
      Camera_Update(camera, scene);
      fade_direction = 0;
      fade_timer = 30;
    }
  }

  VECTOR test_flame_offset = { 0, 0, 0, 0 };
  VECTOR test_flame_wp;

  ApplyMatrixLV(playerActor->L_Hand_matrix, &test_flame_offset, &test_flame_wp);
  
  //if(g_pad_press & PAD_TRIANGLE) bone_select++;
  //if(g_pad_press & PAD_SQUARE) bone_select--;
  //if(bone_select > 50) bone_select = 50;
  //if(bone_select < 0) bone_select = 0;

  // Process actor list
  // Exclude player type, process all categories
  for(int i = ACTOR_GROUP_BG; i < ACTOR_GROUP_MAX; i++) {
    Actor * current = Scene_ActorList[i].start;
    if(Scene_ActorList[i].length == 0) continue;
    while(current) {
      Actor * next = current->next;
      if(current->Update) {
        SetSpadStack(SPAD_STACK_ADDR);
        SVECTOR tdist = playerActor->base.pos;
        tdist.vx -= current->pos.vx;
        tdist.vz -= current->pos.vz;
        tdist.vx = fastabs(tdist.vx);
        tdist.vz = fastabs(tdist.vz);
        // Calculate distance to player
        current->xzDistanceSq = tdist.vx * tdist.vx + tdist.vz * tdist.vz;
        //current->xzDistance = SquareRoot0(current->xzDistanceSq);


        ResetSpadStack();
        current->Update(current, scene);
      } else { // No update function means object is to be destroyed
        Scene_RemoveActor(&Scene_ActorList[i], current);
        current->Destroy(current, scene);
        Arena_Free(current);
      }
      current = next;
    }
  }

  scene_counter++; // Global/General Purpose Counter
}

UVCOORD Screen_GetActionLabel(char action) {
  UVCOORD result = {(SCR_ACTION_LABELS_X * 4) % 256, SCR_ACTION_LABELS_Y % 256};
  if(action > 10) result.u += SCR_ACTION_LABEL_W;
  result.v += SCR_ACTION_LABEL_H * (action % 11);
  return result;
}

SVECTOR action_label_vtx[] = {
  -30+5, 7-3, 0, 0,
  30+5, 7-3, 0, 0,
  -30+5, -6-3, 0, 0,
  30+5, -6-3, 0, 0
};

SVECTOR action_button_icon_vtx[] = {
  -21, 11, 0, 0,
  -9, 11, 0, 0,
  -21, 2, 0, 0,
  -9, 2, 0, 0
};

SVECTOR action_button_vtx[] = {
  -21, 11, 0, 0,
  21, 11, 0, 0,
  -21, -12, 0, 0,
  21, -12, 0, 0
};

u_char * Screen_DrawActionButton(short x, short y, short scalex0, short scaley0, short scalex1, short scaley1, u_char fade, u_char button, char action, u_char * buffer) {
  MATRIX matrix = {
    scalex0,        0,    0,
          0, -scaley0,    0,
          0,        0, 4096,
    x-(SCREEN_W/2), y-(SCREEN_H/2), 256
  };

  POLY_FT4 * poly = (POLY_FT4 *)buffer;
  u_short tpage = getTPage(0, 0, 492, 0);

  if(action >= 0) {
    UVCOORD lblcoords = Screen_GetActionLabel(action);
    gte_SetRotMatrix(&matrix);
    gte_SetTransMatrix(&matrix);

    gte_ldv3(
        &action_label_vtx[0],
        &action_label_vtx[1],
        &action_label_vtx[2]
      );
    gte_rtpt();

    *(long *)&poly->r0 = 0x00808080;
    poly->tpage = tpage;
    //setUVWH(poly, 482 * 4 % 256, 49, 60, 13);    
    setUVWH(poly, lblcoords.u, lblcoords.v, 59, 13);
    
    gte_stsxy3((long *)&poly->x0,(long *)&poly->x1,(long *)&poly->x2);
    gte_ldv0(&action_label_vtx[3]);
    gte_rtps();

    poly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+fade);
    setPolyFT4(poly);
    setSemiTrans(poly, 1);

    gte_stsxy((long *)&poly->x3);

    addPrim(G.pOt, poly);
    poly++;
  }

  matrix.m[0][0] = scalex1;
  matrix.m[1][1] = -scaley1;

  gte_SetRotMatrix(&matrix);
  gte_SetTransMatrix(&matrix);

  gte_ldv3(
      &action_button_icon_vtx[0],
      &action_button_icon_vtx[1],
      &action_button_icon_vtx[2]
    );
  gte_rtpt();

  poly->clut = getClut(SCR_BUTTON_LABELS_CLUT_X, SCR_BUTTON_LABELS_CLUT_Y+fade);
  setUVWH(poly, (SCR_BUTTON_LABELS_X * 4 % 256) + 13 * button, SCR_BUTTON_LABELS_Y, 13, 9);

  gte_stsxy3((long *)&poly->x0,(long *)&poly->x1,(long *)&poly->x2);
  gte_ldv0(&action_button_icon_vtx[3]);
  gte_rtps();

  *(long *)&poly->r0 = 0x00808080;
  poly->tpage = tpage;
  setPolyFT4(poly);
  setSemiTrans(poly, 1);
  
  gte_stsxy((long *)&poly->x3);

  addPrim(G.pOt, poly);
  poly++;

  gte_ldv3(
      &action_button_vtx[0],
      &action_button_vtx[1],
      &action_button_vtx[2]
    );
  gte_rtpt();

  *(long *)&poly->r0 = 0x00808080;
  poly->tpage = tpage;
  setUVWH(poly, SCR_BUTTON_BACK_X * 4 % 256, SCR_BUTTON_BACK_Y, 42, 23);

  gte_stsxy3((long *)&poly->x0,(long *)&poly->x1,(long *)&poly->x2);
  gte_ldv0(&action_button_vtx[3]);
  gte_rtps();

  poly->clut = getClut(SCR_BUTTON_BACK_CLUT_X, SCR_BUTTON_BACK_CLUT_Y+fade);
  setPolyFT4(poly);
  setSemiTrans(poly, 1);

  gte_stsxy((long *)&poly->x3);

  addPrim(G.pOt, poly);
  poly++;

  return (u_char*)poly;
}

// Test Temporaries
u_long temp_counter = 0;

void SceneDraw() {
  u_char * packet_b_ptr;
  MATRIX local_identity;
  MATRIX local_identity_far;

  local_identity_far = local_identity = m_identity;

  daytime_seg_interp += 1;
  if(daytime_seg_interp > 4096) {
    daytime_seg_interp = 0;
    daytime_segment++;
    if(daytime_segment > 6) daytime_segment = 0;
  }

  switch(daytime_segment) {
    case 0:
      daytime_sky.r = fix12_lerp(DAYTIME_DAYBREAK_SKY_R, DAYTIME_NOON_SKY_R, daytime_seg_interp);
      daytime_sky.g = fix12_lerp(DAYTIME_DAYBREAK_SKY_G, DAYTIME_NOON_SKY_G, daytime_seg_interp);
      daytime_sky.b = fix12_lerp(DAYTIME_DAYBREAK_SKY_B, DAYTIME_NOON_SKY_B, daytime_seg_interp);

      daytime_fog.r = fix12_lerp(DAYTIME_DAYBREAK_FOG_R, DAYTIME_NOON_FOG_R, daytime_seg_interp);
      daytime_fog.g = fix12_lerp(DAYTIME_DAYBREAK_FOG_G, DAYTIME_NOON_FOG_G, daytime_seg_interp);
      daytime_fog.b = fix12_lerp(DAYTIME_DAYBREAK_FOG_B, DAYTIME_NOON_FOG_B, daytime_seg_interp);

      daytime_sun.r = fix12_lerp(DAYTIME_DAYBREAK_SUN_R, DAYTIME_NOON_SUN_R, daytime_seg_interp);
      daytime_sun.g = fix12_lerp(DAYTIME_DAYBREAK_SUN_G, DAYTIME_NOON_SUN_G, daytime_seg_interp);
      daytime_sun.b = fix12_lerp(DAYTIME_DAYBREAK_SUN_B, DAYTIME_NOON_SUN_B, daytime_seg_interp);
      break;
    case 1:
      daytime_sky.r = DAYTIME_NOON_SKY_R;
      daytime_sky.g = DAYTIME_NOON_SKY_G;
      daytime_sky.b = DAYTIME_NOON_SKY_B;

      daytime_fog.r = DAYTIME_NOON_FOG_R;
      daytime_fog.g = DAYTIME_NOON_FOG_G;
      daytime_fog.b = DAYTIME_NOON_FOG_B;

      daytime_sun.r = DAYTIME_NOON_SUN_R;
      daytime_sun.g = DAYTIME_NOON_SUN_G;
      daytime_sun.b = DAYTIME_NOON_SUN_B;
    break;
    case 2:
      daytime_sky.r = fix12_lerp(DAYTIME_NOON_SKY_R, DAYTIME_DUSK_SKY_R, daytime_seg_interp);
      daytime_sky.g = fix12_lerp(DAYTIME_NOON_SKY_G, DAYTIME_DUSK_SKY_G, daytime_seg_interp);
      daytime_sky.b = fix12_lerp(DAYTIME_NOON_SKY_B, DAYTIME_DUSK_SKY_B, daytime_seg_interp);

      daytime_fog.r = fix12_lerp(DAYTIME_NOON_FOG_R, DAYTIME_DUSK_FOG_R, daytime_seg_interp);
      daytime_fog.g = fix12_lerp(DAYTIME_NOON_FOG_G, DAYTIME_DUSK_FOG_G, daytime_seg_interp);
      daytime_fog.b = fix12_lerp(DAYTIME_NOON_FOG_B, DAYTIME_DUSK_FOG_B, daytime_seg_interp);

      daytime_sun.r = fix12_lerp(DAYTIME_NOON_SUN_R, DAYTIME_DUSK_SUN_R, daytime_seg_interp);
      daytime_sun.g = fix12_lerp(DAYTIME_NOON_SUN_G, DAYTIME_DUSK_SUN_G, daytime_seg_interp);
      daytime_sun.b = fix12_lerp(DAYTIME_NOON_SUN_B, DAYTIME_DUSK_SUN_B, daytime_seg_interp);
      break;
    case 3:
      daytime_sky.r = fix12_lerp(DAYTIME_DUSK_SKY_R, DAYTIME_NIGHT_SKY_R, daytime_seg_interp);
      daytime_sky.g = fix12_lerp(DAYTIME_DUSK_SKY_G, DAYTIME_NIGHT_SKY_G, daytime_seg_interp);
      daytime_sky.b = fix12_lerp(DAYTIME_DUSK_SKY_B, DAYTIME_NIGHT_SKY_B, daytime_seg_interp);

      daytime_fog.r = fix12_lerp(DAYTIME_DUSK_FOG_R, DAYTIME_NIGHT_FOG_R, daytime_seg_interp);
      daytime_fog.g = fix12_lerp(DAYTIME_DUSK_FOG_G, DAYTIME_NIGHT_FOG_G, daytime_seg_interp);
      daytime_fog.b = fix12_lerp(DAYTIME_DUSK_FOG_B, DAYTIME_NIGHT_FOG_B, daytime_seg_interp);

      daytime_sun.r = fix12_lerp(DAYTIME_DUSK_SUN_R, DAYTIME_NIGHT_SUN_R, daytime_seg_interp);
      daytime_sun.g = fix12_lerp(DAYTIME_DUSK_SUN_G, DAYTIME_NIGHT_SUN_G, daytime_seg_interp);
      daytime_sun.b = fix12_lerp(DAYTIME_DUSK_SUN_B, DAYTIME_NIGHT_SUN_B, daytime_seg_interp);
      break;
    case 4:
      daytime_sky.r = DAYTIME_NIGHT_SKY_R;
      daytime_sky.g = DAYTIME_NIGHT_SKY_G;
      daytime_sky.b = DAYTIME_NIGHT_SKY_B;

      daytime_fog.r = DAYTIME_NIGHT_FOG_R;
      daytime_fog.g = DAYTIME_NIGHT_FOG_G;
      daytime_fog.b = DAYTIME_NIGHT_FOG_B;

      daytime_sun.r = DAYTIME_NIGHT_SUN_R;
      daytime_sun.g = DAYTIME_NIGHT_SUN_G;
      daytime_sun.b = DAYTIME_NIGHT_SUN_B;
    break;
    case 5:
      daytime_sky.r = fix12_lerp(DAYTIME_NIGHT_SKY_R, DAYTIME_DAYBREAK_SKY_R, daytime_seg_interp);
      daytime_sky.g = fix12_lerp(DAYTIME_NIGHT_SKY_G, DAYTIME_DAYBREAK_SKY_G, daytime_seg_interp);
      daytime_sky.b = fix12_lerp(DAYTIME_NIGHT_SKY_B, DAYTIME_DAYBREAK_SKY_B, daytime_seg_interp);

      daytime_fog.r = fix12_lerp(DAYTIME_NIGHT_FOG_R, DAYTIME_DAYBREAK_FOG_R, daytime_seg_interp);
      daytime_fog.g = fix12_lerp(DAYTIME_NIGHT_FOG_G, DAYTIME_DAYBREAK_FOG_G, daytime_seg_interp);
      daytime_fog.b = fix12_lerp(DAYTIME_NIGHT_FOG_B, DAYTIME_DAYBREAK_FOG_B, daytime_seg_interp);

      daytime_sun.r = fix12_lerp(DAYTIME_NIGHT_SUN_R, DAYTIME_DAYBREAK_SUN_R, daytime_seg_interp);
      daytime_sun.g = fix12_lerp(DAYTIME_NIGHT_SUN_G, DAYTIME_DAYBREAK_SUN_G, daytime_seg_interp);
      daytime_sun.b = fix12_lerp(DAYTIME_NIGHT_SUN_B, DAYTIME_DAYBREAK_SUN_B, daytime_seg_interp);
      break;
  }

  //G.clear.r = daytime_sky.r;
  //G.clear.g = daytime_sky.g;
  //G.clear.b = daytime_sky.b;
  //SetFarColor(daytime_fog.r, daytime_fog.g, daytime_fog.b);
  //SetBackColor(daytime_sun.r, daytime_sun.g, daytime_sun.b);
  SetBackColor(122-50, 113-50, 164-50);
  //SetBackColor(30, 30, 40);

  MATRIX light_view;
  MATRIX lights = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
  };
  
  //CompMatrixLV(&lights, &camera->matrix, &light_view);

  gte_SetLightMatrix(&light_view);

  MATRIX light_colors = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
  };
  gte_SetColorMatrix(&light_colors);

  BeginDraw();

  packet_b_ptr = G.pBuffer;

  CompMatrixLV(&camera->matrix, &m_identity, &local_identity);

  local_identity_far = local_identity;
  local_identity_far.t[0] = local_identity.t[0] >> 3;
  local_identity_far.t[1] = local_identity.t[1] >> 3;
  local_identity_far.t[2] = local_identity.t[2] >> 3;

  gte_SetRotMatrix(&local_identity_far);
  gte_SetTransMatrix(&local_identity_far);

  //SetSpadStack(SPAD_STACK_ADDR);
  //packet_b_ptr = SGM2_UpdateModel(map_model[6], packet_b_ptr, (u_long*)G.pOt, 2048, SGM2_RENDER_AMBIENT, scene);
  //ResetSpadStack();

  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_update = prof_current_val - prof_last_counter;
  prof_last_counter = prof_current_val;

  if(g_pad_press & PAD_TRIANGLE) __DEBUG_TOGGLE = !__DEBUG_TOGGLE;

  // Update animated textures
  // Flame texture
  Scene_ScrollTexture2x(&anim_tex_flame_src, &anim_tex_flame_dest, 0x3F - ((scene_counter<<1) & 0x3F));

  SetSpadStack(SPAD_STACK_ADDR);
  if(!__DEBUG_TOGGLE){  // SGM2_RENDER_SUBDIV | 
    
  } else {
    //packet_b_ptr = SGM2_UpdateModel(scene->current_room_m, packet_b_ptr, (u_long*)G.pOt, 40, SGM2_RENDER_SUBDIV, scene);
  }

  packet_b_ptr = SGM2_UpdateModel(scene->current_room_m, packet_b_ptr, (u_long*)G.pOt, 30, SGM2_RENDER_SUBDIV | SGM2_RENDER_SUBDIV_HIGH | SGM2_RENDER_CLUTFOG | SGM2_RENDER_SCREEN_CLIP, scene);
  
  //packet_b_ptr = SGM2_UpdateModel(map_model[5], packet_b_ptr, (u_long*)G.pOt, 60, SGM2_RENDER_SUBDIV | SGM2_RENDER_SUBDIV_HIGH | SGM2_RENDER_AMBIENT | SGM2_RENDER_CLUTFOG, scene); // SGM2_RENDER_SUBDIV_HIGH
  //packet_b_ptr = SGM2_UpdateModel(map_model[5], packet_b_ptr, (u_long*)G.pOt, 30, SGM2_RENDER_SUBDIV | SGM2_RENDER_CLUTFOG, scene);
  ResetSpadStack();
  if(scene->previous_room_m){
    SetSpadStack(SPAD_STACK_ADDR);
      packet_b_ptr = SGM2_UpdateModel(scene->previous_room_m, packet_b_ptr, (u_long*)G.pOt, 20, SGM2_RENDER_SUBDIV | SGM2_RENDER_AMBIENT, scene); // SGM2_RENDER_SUBDIV
    ResetSpadStack();
  }

  for(int i = ACTOR_GROUP_BG; i < ACTOR_GROUP_MAX; i++) {
    Actor * current = Scene_ActorList[i].start;
    if(Scene_ActorList[i].length == 0) continue;
    while(current) {
      if(current->visible){
        packet_b_ptr = current->Draw(current, &camera->matrix, packet_b_ptr,scene);
      }
      current = current->next;
    }
  }

  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_draw_bg = prof_current_val - prof_last_counter;
  prof_last_counter = prof_current_val;

  packet_b_ptr = PlayerDraw((Actor*)playerActor, &camera->matrix, packet_b_ptr, scene);

  CompMatrixLV(&camera->matrix, playerActor->L_Hand_matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_draw_player = prof_current_val - prof_last_counter;
  prof_last_counter = prof_current_val;

  // Draw particles
  packet_b_ptr = Scene_ParticleUpdate(scene, &camera->matrix, packet_b_ptr);
  #define INTERFACE_FADE_SEMITRANS_FRAMES 2
  if(scene->interface_fade) {
    if(scene->interface_fade_counter == 0) {
      scene->interface_fade_counter++;
    } else {
      scene->interface_sub_counter++;
      if(scene->interface_sub_counter == INTERFACE_FADE_SEMITRANS_FRAMES) {
        scene->interface_sub_counter = 0;
        scene->interface_fade_counter++;
      }
    }
  } else {
    if(scene->interface_fade_counter != 0) {
      scene->interface_sub_counter++;
      if(scene->interface_sub_counter == INTERFACE_FADE_SEMITRANS_FRAMES) {
        scene->interface_sub_counter = 0;
        scene->interface_fade_counter = 0;
      }
    }
  }

  if(scene->interface_fade_counter <= 1) {
    // Draw sprite (for testing)
    //draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52, 16, 52, 32, 0, 0, FLAME_TEX_CLUT_X, (FLAME_TEX_CLUT_Y+8)+scene->interface_fade_counter);
    //packet_b_ptr += sizeof(SPRT);
  /*
    draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52-52, 16, 52, 32, 52, 0, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+9);
    packet_b_ptr += sizeof(SPRT);

    draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52-52-52, 16, 52, 32, 52*2, 0, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+10+2);
    packet_b_ptr += sizeof(SPRT);

    draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52-52-52-52, 16, 52, 32, 52*3, 0, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+13);
    packet_b_ptr += sizeof(SPRT);

    draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52-52-52-52-52, 16, 52, 32, 52*3, 0, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+13+1);
    packet_b_ptr += sizeof(SPRT);
  */
    //draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52-52-52-52-52-52, 16, 52, 32, 52*3, 0, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+13+2);
    //draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52-52, 16, 52, 32, 52*3, 0, FLAME_TEX_CLUT_X, (FLAME_TEX_CLUT_Y+13+2)+scene->interface_fade_counter);
    //packet_b_ptr += sizeof(SPRT);

    if(((PlayerActor*)scene->player)->action != ((PlayerActor*)scene->player)->action_prev) action_anim_swap = 1;
    if(((PlayerActor*)scene->player)->sub_action != ((PlayerActor*)scene->player)->sub_action_prev) sub_action_anim_swap = 1;

    if(action_anim_swap == 1) {
      action_anim_counter = 0;
      action_anim_swap ++;
    } else if(action_anim_swap == 2) {
      action_anim_counter++;
      if(action_anim_counter > 3){
        action_anim_swap = 0;
        action_anim_counter = 0;
      }
    }

    //FntPrint("action_anim_counter %d\n",action_anim_counter);

    if(sub_action_anim_swap == 1) {
      sub_action_anim_counter = 0;
      sub_action_anim_swap ++;
    } else if(sub_action_anim_swap == 2) {
      sub_action_anim_counter++;
      if(sub_action_anim_counter > 3) {
        sub_action_anim_swap = 0;
        sub_action_anim_counter = 0;
      }
    }

    // Modern UI - Action Labels
    // Circle Button Action Label
    if(action_anim_counter != 0 || action_anim_swap == 0) { 
      /*UVCOORD actlbl = Screen_GetActionLabel(((PlayerActor*)scene->player)->action);
      draw_SimpleSpriteSemi(
        (SPRT*)packet_b_ptr, G.pOt, SCR_ACTION_LABEL_CIRCLE_X, SCR_ACTION_LABEL_CIRCLE_Y,
        SCR_ACTION_LABEL_W, SCR_ACTION_LABEL_H, actlbl.u, actlbl.v,
        SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+scene->interface_fade_counter
      );
      packet_b_ptr += sizeof(SPRT);*/

      packet_b_ptr = Screen_DrawActionButton(
        SCR_BUTTON_MODERN_CIRCLE_X+18+14, SCR_BUTTON_MODERN_CIRCLE_Y+11+1,
        button_text_anim_x[action_anim_counter], button_text_anim_y[action_anim_counter],
        button_anim_x[action_anim_counter], button_anim_y[action_anim_counter],
        scene->interface_fade_counter, 3, ((PlayerActor*)scene->player)->action, packet_b_ptr
      );
    }
    
    if(sub_action_anim_counter != 0 || sub_action_anim_swap == 0) { 
      /*UVCOORD actlbl = Screen_GetActionLabel(((PlayerActor*)scene->player)->sub_action);
      draw_SimpleSpriteSemi(
        (SPRT*)packet_b_ptr, G.pOt, SCR_ACTION_LABEL_CROSS_X, SCR_ACTION_LABEL_CROSS_Y,
        SCR_ACTION_LABEL_W, SCR_ACTION_LABEL_H, actlbl.u, actlbl.v,
        SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+scene->interface_fade_counter
      );
      packet_b_ptr += sizeof(SPRT);*/
      packet_b_ptr = Screen_DrawActionButton(
        SCR_BUTTON_MODERN_CROSS_X+18+8, SCR_BUTTON_MODERN_CROSS_Y+11+3,
        4096, 4096, 4096, 4096,
        scene->interface_fade_counter, 2, ((PlayerActor*)scene->player)->sub_action, packet_b_ptr
      );
    }


    // Modern UI - Draw Button Labels
    draw_SimpleSpriteSemi(
      (SPRT*)packet_b_ptr, G.pOt, SCR_BUTTON_MODERN_TRIANGLE_X+5, SCR_BUTTON_MODERN_TRIANGLE_Y-1,
      13, 9, (SCR_BUTTON_LABELS_X * 4) % 256, SCR_BUTTON_LABELS_Y % 256,
      SCR_BUTTON_LABELS_CLUT_X, SCR_BUTTON_LABELS_CLUT_Y+scene->interface_fade_counter
    );
    packet_b_ptr += sizeof(SPRT);

    draw_SimpleSpriteSemi(
      (SPRT*)packet_b_ptr, G.pOt, SCR_BUTTON_MODERN_SQUARE_X-1, SCR_BUTTON_MODERN_SQUARE_Y+1,
      13, 9, (SCR_BUTTON_LABELS_X * 4) + 13 * 1 % 256, SCR_BUTTON_LABELS_Y % 256,
      SCR_BUTTON_LABELS_CLUT_X, SCR_BUTTON_LABELS_CLUT_Y+scene->interface_fade_counter
    );
    packet_b_ptr += sizeof(SPRT);

    // Modern UI - Draw Action Icons
    draw_SimpleSpriteSemi(
      (SPRT*)packet_b_ptr, G.pOt, SCR_BUTTON_MODERN_TRIANGLE_X+7, SCR_BUTTON_MODERN_TRIANGLE_Y-1,
      36, 23, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256,
      SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+scene->interface_fade_counter
    );
    packet_b_ptr += sizeof(SPRT);

    draw_SimpleSpriteSemi(
      (SPRT*)packet_b_ptr, G.pOt, SCR_BUTTON_MODERN_SQUARE_X, SCR_BUTTON_MODERN_SQUARE_Y,
      36, 23, (SCR_ACTION_HOOKSHOT_X * 4) % 256, SCR_ACTION_HOOKSHOT_Y % 256,
      SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+4+scene->interface_fade_counter
    );
    packet_b_ptr += sizeof(SPRT);


    // Modern UI - Draw Button Back
    draw_SimpleSpriteSemi(
      (SPRT*)packet_b_ptr, G.pOt, SCR_BUTTON_MODERN_TRIANGLE_X+5, SCR_BUTTON_MODERN_TRIANGLE_Y-1,
      42, 23, (SCR_BUTTON_BACK_X * 4) % 256, SCR_BUTTON_BACK_Y % 256,
      SCR_BUTTON_BACK_CLUT_X, SCR_BUTTON_BACK_CLUT_Y+scene->interface_fade_counter
    );
    packet_b_ptr += sizeof(SPRT);

    draw_SimpleSpriteSemi(
      (SPRT*)packet_b_ptr, G.pOt, SCR_BUTTON_MODERN_SQUARE_X-1, SCR_BUTTON_MODERN_SQUARE_Y+1,
      42, 23, (SCR_BUTTON_BACK_X * 4) % 256, SCR_BUTTON_BACK_Y % 256,
      SCR_BUTTON_BACK_CLUT_X, SCR_BUTTON_BACK_CLUT_Y+scene->interface_fade_counter
    );
    packet_b_ptr += sizeof(SPRT);

    // Test - Pseudo Linear Interpolated Sprites
    /*draw_SimpleSpriteSemi(
      (SPRT*)packet_b_ptr, G.pOt, SCR_BUTTON_MODERN_TRIANGLE_X, SCR_BUTTON_MODERN_TRIANGLE_Y,
      36, 23, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256,
      SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+scene->interface_fade_counter
    );*/
    /*
    POLY_FT4 * scalepoly = (POLY_FT4 *)packet_b_ptr;

    setXYWH(scalepoly, 0, 120-21+42, 36*2, 42);
    setUVWH(scalepoly, (SCR_ACTION_HOOKSHOT_X * 4) % 256, SCR_ACTION_HOOKSHOT_Y+23, 35, 41);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+6);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 36*2, 120-21+42, 36*2, 42);
    setUVWH(scalepoly, (SCR_ACTION_HOOKSHOT_X * 4) % 256, SCR_ACTION_HOOKSHOT_Y+23+42, 35, 41);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+8);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 1, 120-21, 36*2, 42);
    setUVWH(scalepoly, (SCR_ACTION_HOOKSHOT_X * 4) % 256, SCR_ACTION_HOOKSHOT_Y+23, 35, 41);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+6+1);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly, 1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 0, 120-21, 36*2, 42);
    setUVWH(scalepoly, (SCR_ACTION_HOOKSHOT_X * 4) % 256, SCR_ACTION_HOOKSHOT_Y+23, 35, 41);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+6);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 36*2+1, 120-21, 36*2, 42);
    setUVWH(scalepoly, (SCR_ACTION_HOOKSHOT_X * 4) % 256, SCR_ACTION_HOOKSHOT_Y+23+42, 35, 41);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+8+1);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly, 1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 36*2, 120-21, 36*2, 42);
    setUVWH(scalepoly, (SCR_ACTION_HOOKSHOT_X * 4) % 256, SCR_ACTION_HOOKSHOT_Y+23+42, 35, 41);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+8);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 256-36-36, 120-22, 36*2, 23*2);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    temp_counter++;

    u_short temp_counter_sel = (temp_counter<<3) > 4096 ? 4096 : temp_counter<<3;

    if(temp_counter > 550) temp_counter = 0;

    short cos_temp = (icos(temp_counter<<7) * (4096-(temp_counter_sel)))>>12;
    short cos_half = 4096 + ((4096-(temp_counter_sel))+(cos_temp>>1));
    short cos_half2 = 4096 + ((4096-(temp_counter_sel))+(cos_temp>>1));
    short cos_half3 = 4096 + ((4096-(temp_counter_sel))+(cos_temp>>1));

    short cos_half_offsx = (36 * cos_half)>>12;
    short cos_half_offsy = (23 * cos_half)>>12;

    short cos_half2_offsx = (37 * cos_half2)>>12;
    short cos_half2_offsy = (24 * cos_half2)>>12;

    short cos_half3_offsx = (35 * cos_half2)>>12;
    short cos_half3_offsy = (22 * cos_half2)>>12;

    
    setXYWH(scalepoly, 256-36-(cos_half_offsx>>1), 120+22+23-(cos_half_offsy>>1), cos_half_offsx, cos_half_offsy);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    
    // 2x
    setXYWH(scalepoly, 256+1, 120-22+1, 36*2, 23*2);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+1);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 256, 120-22, 36*2, 23*2);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    // 2x
    setXYWH(scalepoly, 256+36-(cos_half3_offsx>>1), 120+22+23-(cos_half3_offsy>>1), cos_half3_offsx, cos_half3_offsy);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+1);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;


    setXYWH(scalepoly, 256+36-(cos_half2_offsx>>1), 120+22+23-(cos_half2_offsy>>1), cos_half2_offsx, cos_half2_offsy);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+1);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 256+36-(cos_half_offsx>>1), 120+22+23-(cos_half_offsy>>1), cos_half_offsx, cos_half_offsy);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00808080;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    // 4x
    setXYWH(scalepoly, 256+36+36+1, 120-22+1, 36*4, 23*4);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00202020;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+1);
    scalepoly->tpage = getTPage(0, 1, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 256+36+36+2, 120-22+2, 36*4, 23*4);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00202020;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+1);
    scalepoly->tpage = getTPage(0, 1, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 256+36+36-1, 120-22-1, 36*4, 23*4);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00202020;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2+1);
    scalepoly->tpage = getTPage(0, 1, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    scalepoly++;

    setXYWH(scalepoly, 256+36+36, 120-22, 36*4, 23*4);
    setUVWH(scalepoly, (SCR_ACTION_BOMB_X * 4) % 256, SCR_ACTION_BOMB_Y % 256, 35, 22);
    *(long*)&scalepoly->r0 = 0x00202020;
    scalepoly->clut = getClut(SCR_ACTION_LABELS_CLUT_X, SCR_ACTION_LABELS_CLUT_Y+2);
    scalepoly->tpage = getTPage(0, 0, 492, 0);
    setPolyFT4(scalepoly);
    setSemiTrans(scalepoly,1);
    addPrim(G.pOt, scalepoly);
    
    scalepoly++;

    packet_b_ptr = (u_char *)scalepoly;
    */

    setDrawTPage((DR_TPAGE*)packet_b_ptr, 1, 0, getTPage(0, 0, 492, 0));
    addPrim(G.pOt, packet_b_ptr);
    packet_b_ptr += sizeof(DR_TPAGE);

  }

  if(scene->interface_fade_counter <= 1) {
    packet_b_ptr = Screen_LifeMeterDraw(packet_b_ptr, scene->interface_fade_counter);
  }

  if(scene->cinema_mode) {
    scene->cinema_mode_counter++;
    if(scene->cinema_mode_counter > 4) scene->cinema_mode_counter = 4;
  } else {
    if(scene->cinema_mode_counter > 0) scene->cinema_mode_counter--;
  }

  if(scene->cinema_mode_counter > 0) {
    u_char targ_height = cinema_mode_anim[scene->cinema_mode_counter-1];
    draw_SimpleTile((TILE*)packet_b_ptr, G.pOt, 0, 0, 512, targ_height);
    packet_b_ptr += sizeof(TILE);
    draw_SimpleTile((TILE*)packet_b_ptr, G.pOt, 0, 240-targ_height, 512, targ_height);
    packet_b_ptr += sizeof(TILE);
  }

  if(fade_counter > 0) {
    addPrim(G.pOt, &fade_screen[0]);
    addPrim(G.pOt, &fade_blend[0]);
  }

  //FntPrint("LOGIC TIME=%d\n",prof_update);
  //FntPrint("DRAW BG=%d\n",prof_draw_bg);
  //FntPrint("PLAYER RENDERING=%d\n",prof_draw_player);
  //FntPrint("FRAME TIME=%d\n",prof_frametime);

  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_end = prof_current_val;
  prof_last_counter = prof_current_val;
  prof_start_last = prof_start;
  prof_frametime = prof_end - prof_start;

  EndDraw();

  
  //FntPrint("START= %d END=%d FRAME= %d\n",prof_start_last, prof_end, prof_frametime);
  //FntPrint("PRAYER=X[%d] Y[%d] Z[%d]\n",playerActor->base.pos.vx, playerActor->base.pos.vy, playerActor->base.pos.vz);

  //if(__DEBUG_TOGGLE) {
  //  FntPrint("DEBUG TOGGLE [%d]\n",__DEBUG_TOGGLE);
  //} else {
  //  FntPrint("DEBUG TOGGLE OFF\n");
  //}
  FntFlush(-1);
}  

// Scrolls textures vertically by pixel ammount,
// offset value must be positive and
// largest offset value must be the size of the texture itself - 1
void Scene_ScrollTexture2x(RECT * src, RECT * dest, u_char offs) {
  RECT copy_region;
  u_short paste_x, paste_y;
  u_int offs_temp = offs;
  copy_region = *src;
  
  // Copy 1
  paste_x = dest->x;
  paste_y = dest->y+offs;
  MoveImage(&copy_region, paste_x, paste_y);
  // Copy 2
  copy_region.h = copy_region.h - offs;
  paste_y = dest->y + src->h + offs;
  MoveImage(&copy_region, paste_x, paste_y);
  // Copy 3
  if(offs != 0) {
    copy_region.y = src->y + (src->h - offs);
    copy_region.h = offs;
    MoveImage(&copy_region, dest->x, dest->y);
  }
}

void * Scene_AllocActor(ActorList * list, u_short type, u_long size) {
  Actor * actor = Arena_Malloc(size);
  actor->type = type;
  actor->size = size;
  Scene_AddActor(list, actor);
  return actor;
}

void * Scene_AddActor(ActorList * list, Actor * actor) {
  list->length++;
  if(!list->start) {
    list->start = actor;
    list->end = actor;
    actor->prev = NULL;
    actor->next = NULL;
  } else {
    // Get last actor and append new actor to it
    Actor * last_actor = list->end;
    list->end = last_actor->next = actor;
    actor->prev = last_actor;
    actor->next = NULL;
  }
  return actor;
}

void Scene_RemoveActor(ActorList * list, Actor * actor) {
  if(list->start == actor) list->start = actor->next;
  if(list->end == actor) list->end = actor->prev;

  // remove thyself
  if(actor->prev) actor->prev->next = actor->next;
  if(actor->next) actor->next->prev = actor->prev;
  list->length--;
}

void Scene_RemoveActorRoom(ActorList * list, u_char room, Scene_Ctx * scene) {
  Actor * current = list->start;
  while(current != NULL) {
    // target room
    if(current->room == room) {
      if(current->prev != NULL) current->prev->next = current->next;
      if(current->next != NULL) current->next->prev = current->prev;
      if(current == list->start) {
        if(current->next != NULL) {
          list->start = current->next;
        } else if (current->prev != NULL){
          list->start = current->prev;
        } else {
          list->start = NULL;
        }
        
      }
      if(current == list->end) {
        list->end = current->prev;
      }
      list->length--;
      current->Destroy(current, scene);
      Arena_Free(current);
    }
    // iterate
    current = current->next;
  }
}

struct ParticleEmitter * Scene_ParticleCreate(struct ParticleEmitter * emitter, Scene_Ctx * scene) {
  struct ParticleEmitter * itr = particle_list;

  for(int i = 0; i < MAX_PARTICLE_EMITTERS; i++) {
    if(particle_list[i].flags) {
      continue;
    } else {
      // Free previous buffer
      //Particle3D * buff = NULL;
      if(particle_list[i].buffer) {
        //buff = particle_list[i].buffer;
        ////printf("Buffer prev 0x%08X\n", particle_list[i].buffer);
      }
      particle_list[i] = *emitter;
      particle_list[i].flags = 1;
      particle_list[i].buffer = NULL;

      SetSpadStack(SPAD_STACK_ADDR);
      Particle3D_Create(&particle_list[i]);
      ResetSpadStack();

      return &particle_list[i];
    }
  }
  return NULL;

  /*if(particle_list[0].flags) {
    return NULL;
  } else {
    particle_list[0] = *emitter;
    particle_list[0].flags = 1;
    Particle3D_Create(&particle_list[0]);
    return &particle_list[0];
  }*/
};

u_char * Scene_ParticleUpdate(Scene_Ctx * scene, MATRIX * view, u_char * buff) {
  u_char * buf_ptr = buff;
  for(int i = 0; i < MAX_PARTICLE_EMITTERS; i++) {
    if(particle_list[i].flags) {
      SetSpadStack(SPAD_STACK_ADDR);
      buf_ptr = Particle3D_Update(&particle_list[i], view, buf_ptr);
      ResetSpadStack();
    }
  }
  return buf_ptr;
}

void Scene_LoadRoom(Room_Data * room, Scene_Ctx * scene) {
  // Initialize Actors
  for(long i = 0; i < room->actor_count; i++) {
    Actor_Descriptor * actdesc = &room->actor_init[i];
    if(actdesc->room != -1) actdesc->room = room->id;
    Scene_CreateActor(actdesc, ACTOR_GROUP_BG, scene);
  }
}

void Scene_CreateActor(Actor_Descriptor * actdesc, u_short group, Scene_Ctx * scene) {
  Actor * actor =  Scene_AllocActor(&Scene_ActorList[group], group, ActorDataSizes[actdesc->actor_type]);
  actor->type = actdesc->actor_type;
  actor->Initialize = ActorInitFuncs[actdesc->actor_type];
  actor->Initialize(actor, actdesc, scene);
}

#define CONTACT_SHADOW_SEGMENTS 6
#define CONTACT_SHADOW_SEGMENT_ANG (4096 / CONTACT_SHADOW_SEGMENTS)
#define CONTACT_SHADOW_SEGMENTS2 (CONTACT_SHADOW_SEGMENTS * 2)
#define CONTACT_SHADOW_INNER_RING_SCALE 1024

#define CONTACT_SHADOW_SEGMENTS2 (CONTACT_SHADOW_SEGMENTS * 2) + 1
#define CONTACT_SHADOW_SEGMENTS_INNER CONTACT_SHADOW_SEGMENTS * 3
#define CONTACT_SHADOW_SEGMENTS_OUTTER CONTACT_SHADOW_SEGMENTS * 4
#define CONTACT_SHADOW_RADIUS 45

SVECTOR contact_shadow_verts[CONTACT_SHADOW_SEGMENTS2+32];
u_char contact_shadow_outter[CONTACT_SHADOW_SEGMENTS_OUTTER+32];
u_char contact_shadow_inner[CONTACT_SHADOW_SEGMENTS_INNER+32];

void Draw_SetupContactShadow() {
  // Generate Contact Shadow Mesh
  short gen_ang = 0;
  int step_tri = 0;
  int step_quad = 0;
  for(int i = 0; i < CONTACT_SHADOW_SEGMENTS; i++) {
    gen_ang += CONTACT_SHADOW_SEGMENT_ANG;
    short sin = (isin(gen_ang) * CONTACT_SHADOW_RADIUS) >> 12;
    short cos = (icos(gen_ang) * CONTACT_SHADOW_RADIUS) >> 12;
    contact_shadow_verts[i].vx = sin;
    contact_shadow_verts[i].vy = 0;
    contact_shadow_verts[i].vz = cos;
    if(i == 5) {
      contact_shadow_verts[i].vz += 256*0.8;
    }
    contact_shadow_verts[i+CONTACT_SHADOW_SEGMENTS].vx = (sin * CONTACT_SHADOW_INNER_RING_SCALE) >> 12;
    contact_shadow_verts[i+CONTACT_SHADOW_SEGMENTS].vy = 0;
    contact_shadow_verts[i+CONTACT_SHADOW_SEGMENTS].vz = (cos * CONTACT_SHADOW_INNER_RING_SCALE) >> 12;
    if(i == 5) {
      contact_shadow_verts[i+CONTACT_SHADOW_SEGMENTS].vz += 256*0.4;
    }

    contact_shadow_outter[step_quad] = i;
    contact_shadow_outter[step_quad+1] = i + 1;
    contact_shadow_outter[step_quad+2] = i + CONTACT_SHADOW_SEGMENTS + 0;
    contact_shadow_outter[step_quad+3] = i + CONTACT_SHADOW_SEGMENTS + 1;
    if(i == CONTACT_SHADOW_SEGMENTS-1) {
      contact_shadow_outter[step_quad+1] -= CONTACT_SHADOW_SEGMENTS;
      contact_shadow_outter[step_quad+3] -= CONTACT_SHADOW_SEGMENTS;
    }
    step_quad += 4;

    contact_shadow_inner[step_tri] = i + CONTACT_SHADOW_SEGMENTS;
    contact_shadow_inner[step_tri+1] = i + CONTACT_SHADOW_SEGMENTS + 1;
    contact_shadow_inner[step_tri+2] = CONTACT_SHADOW_SEGMENTS2-1;
    if(i == CONTACT_SHADOW_SEGMENTS-1) {
      contact_shadow_inner[step_tri+1] -= CONTACT_SHADOW_SEGMENTS;
    }
    step_tri += 3;
  }
  SVECTOR zero = { 0, 0, 0, 0 };
  contact_shadow_verts[CONTACT_SHADOW_SEGMENTS2-1] = zero;
}



u_char * Draw_ContactShadow(SVECTOR * pos, short angle, u_short scale, u_short strength, u_char * packet_ptr, void * scene, short offset) {
  Scene_Ctx * scene_ctx = (Scene_Ctx *)scene;
  MATRIX local_matrix, world_view;
  MATRIX * view = &camera->matrix;
  POLY_G4 * dest_pg4_ptr = (POLY_G4*) packet_ptr;
  POLY_G3 * dest_pg3_ptr;
  int step4 = 0;
  int step3 = 0;
  //long inner_color = 0x0;
  u_short strlvl = (64 * strength) >> 12;
  long inner_color = (strlvl << 0) | (strlvl << 8) | (strlvl << 16);
  //long inner_color = 0x0808080;
  long otz;
  SVECTOR ang = {0, angle, 0, 0};
  RotMatrix_gte(&ang, &local_matrix);
  //RotMatrixY(&angle, &local_matrix);

  /*local_matrix.m[0][0] = 4096;
  local_matrix.m[0][1] = 0;
  local_matrix.m[0][2] = 0;
  local_matrix.m[1][0] = 0;
  local_matrix.m[1][1] = 4096;
  local_matrix.m[1][2] = 0;
  local_matrix.m[2][0] = 0;
  local_matrix.m[2][1] = 0;
  local_matrix.m[2][2] = 4096;*/
  local_matrix.t[0] = pos->vx;
  local_matrix.t[1] = pos->vy;
  local_matrix.t[2] = pos->vz;

  VECTOR scl = {scale, scale, scale, 0};
  ScaleMatrix(&local_matrix, &scl);


  CompMatrixLV(view, &local_matrix, &world_view);

  gte_SetRotMatrix(&world_view);
  gte_SetTransMatrix(&world_view);
  
  SVECTOR localPos = { 0, 0, 0, 0 };
  gte_ldv0(&localPos);

  gte_rtps();
  gte_stsz(&otz);
  //otz = 0;
  otz = ((otz >> 2) >> OTSUBDIV);
  otz += offset;

  if(otz <= OTMINCHAR || otz >= OTSIZE) {
    return packet_ptr;
  }

  

  for(int i = 0; i < CONTACT_SHADOW_SEGMENTS; i++) 
    {*(long*)&dest_pg4_ptr->r0 = 0;
    *(long*)&dest_pg4_ptr->r1 = 0;
    *(long*)&dest_pg4_ptr->r2 = *(long*)(&inner_color);
    *(long*)&dest_pg4_ptr->r3 = *(long*)(&inner_color);
    setPolyG4(dest_pg4_ptr);
    setSemiTrans(dest_pg4_ptr, 1);
    gte_ldv3(
      (SVECTOR*)&contact_shadow_verts[contact_shadow_outter[step4]],
      (SVECTOR*)&contact_shadow_verts[contact_shadow_outter[step4+1]],
      (SVECTOR*)&contact_shadow_verts[contact_shadow_outter[step4+2]]
    );
    gte_rtpt();
    gte_stsxy3((long *)&dest_pg4_ptr->x0,(long *)&dest_pg4_ptr->x1,(long *)&dest_pg4_ptr->x2);
    gte_ldv0((SVECTOR*)&contact_shadow_verts[contact_shadow_outter[step4+3]]);
    gte_rtps();
    gte_stsxy((long *)&dest_pg4_ptr->x3);

    addPrim(G.pOt+otz, dest_pg4_ptr);
    dest_pg4_ptr++;
    step4+=4;

    dest_pg3_ptr = (POLY_G3*)dest_pg4_ptr;
    *(long*)&dest_pg3_ptr->r0 = *(long*)(&inner_color);
    *(long*)&dest_pg3_ptr->r1 = *(long*)(&inner_color);
    *(long*)&dest_pg3_ptr->r2 = *(long*)(&inner_color);
    setPolyG3(dest_pg3_ptr);
    setSemiTrans(dest_pg3_ptr, 1);
    gte_ldv3(
        (SVECTOR*)&contact_shadow_verts[contact_shadow_inner[step3]],
        (SVECTOR*)&contact_shadow_verts[contact_shadow_inner[step3+1]],
        (SVECTOR*)&contact_shadow_verts[contact_shadow_inner[step3+2]]
      );
    gte_rtpt();
    gte_stsxy3((long *)&dest_pg3_ptr->x0,(long *)&dest_pg3_ptr->x1,(long *)&dest_pg3_ptr->x2);

    addPrim(G.pOt+otz, dest_pg3_ptr);
    dest_pg3_ptr++;
    
    dest_pg4_ptr = (POLY_G4*)dest_pg3_ptr;
    step3+=3;
  }

  packet_ptr = (u_char *)dest_pg3_ptr;

  setDrawTPage((DR_TPAGE*)packet_ptr, 1, 1, getTPage(0, 2, 0, 0));
  addPrim(G.pOt+otz, packet_ptr);
  packet_ptr += sizeof(DR_TPAGE);

  return packet_ptr;
}

void Draw_CalcNearestLight(Actor * actor, void * scene) {
    if(actor->xzDistanceSq < 4096) {
      Scene_Ctx * scene_ctx = (Scene_Ctx*)scene;
      PlayerActor * player = (PlayerActor *)scene_ctx->player;
      u_short dist_lvl = (4096 - actor->xzDistanceSq) << 1;
      for(int i = 0; i < 2; i++) {
        if(player->nearest_light_dist[i] > actor->xzDistanceSq) {
          short delta_x = (player->base.pos.vx - actor->pos.vx);
          short delta_z = (player->base.pos.vz - actor->pos.vz);
          short dist_x = (delta_x << 12) / actor->xzDistanceSq;
          short dist_z = (delta_z << 12) / actor->xzDistanceSq;
          player->nearest_shadow_ang[i] = fix12_atan2s(dist_x, dist_z);
          player->nearest_light_dist[i] = actor->xzDistanceSq;
          player->nearest_light_str[i] = dist_lvl;
          //player->light_matrix.m[i][0] = -dist_x; 
          //player->light_matrix.m[i][1] = 0;
          //player->light_matrix.m[i][2] = -dist_z;
          return;
        }
      }
    }
  }