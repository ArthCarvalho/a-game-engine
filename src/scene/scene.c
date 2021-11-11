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

//#include "particles/particles.h"

// !!!BIG TODO !!!
// Fix Arena_Free() coalesce crash
// Make make ambient color calculation faster (GTE)

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
Col2 * map_col;
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

// Void Out Y
short void_out = -16383;
u_short void_out_active = 0;

// Particle temp
#define MAX_PARTICLE_EMITTERS 50
struct ParticleEmitter particle_list[MAX_PARTICLE_EMITTERS];
u_char active_particle_count = 0;


//char active_room = 0;
/* Temporary Static Data Files */
extern unsigned long player_tim[];
extern unsigned long player_agm[];
extern unsigned long player_anm[];
extern unsigned long sword_sg2[];
extern unsigned long scene_tim[];
extern unsigned long iwakabe_tim[];
extern unsigned long iwakabe_far_tim[];
extern unsigned long iwayuka_tim[];
extern unsigned long scene_col[];
extern unsigned long scene_room00_sg2[];
extern unsigned long scene_room01_sg2[];
extern unsigned long scene_room02_sg2[];
extern unsigned long scene_room03_sg2[];
extern unsigned long scene_room04_sg2[];
extern unsigned long scene_room05_sg2[];
extern unsigned long scene_room05_sg2_ext[];
extern unsigned long test_image_tim[];
extern unsigned long test_image2_tim[];
extern unsigned long test_image3_tim[];
extern unsigned long test_image4_tim[];
extern unsigned long screen_lifebar_tim[];
extern unsigned long texture_data_start[];
extern unsigned long texture_data_end[];

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
u_char cinema_mode = 0;
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

ActorList Scene_ActorList[ACTOR_LIST_NUM];

enum Actor_Type {
  ACTOR_TYPE_SWAP_PLANE,
  ACTOR_TYPE_LAST_INDEX
};

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

typedef struct Actor_SwapPlane{ 
  Actor base;
  unsigned char display_type;
  unsigned char front_room;
  unsigned char back_room;

} Actor_SwapPlane;

void Actor_SwapPlane_Init(Actor * address, Actor_Descriptor * descriptor, Scene_Ctx * scene);
void Actor_SwapPlane_Destroy(Actor * address, Scene_Ctx * scene);
void Actor_SwapPlane_Update(Actor * address, Scene_Ctx * scene);
u_char * Actor_SwapPlane_Draw(Actor * address, void * view, unsigned char * buffer, void * scene);
/* Swap Plane Class */
void Actor_SwapPlane_Init(Actor * address, Actor_Descriptor * descriptor, Scene_Ctx * scene) {
  Actor_SwapPlane * myself = (Actor_SwapPlane*)address;
  Actor_SwapPlane_Initializer * initial_values;
  myself->base.Initialize = (PTRFUNC_3ARG) Actor_SwapPlane_Init;
  myself->base.Destroy = (PTRFUNC_2ARG) Actor_SwapPlane_Destroy;
  myself->base.Update = (PTRFUNC_2ARG) Actor_SwapPlane_Update;
  myself->base.Draw = (PTRFUNC_4ARGCHAR) Actor_SwapPlane_Draw;
  myself->base.room = 0xFF; // Process independent of current room location

  myself->base.pos.vx = descriptor->x;
  myself->base.pos.vy = descriptor->y;
  myself->base.pos.vz = descriptor->z;
  myself->base.rot.vx = descriptor->rot_x;
  myself->base.rot.vy = descriptor->rot_y;
  myself->base.rot.vz = descriptor->rot_z;
  myself->base.visible = 0;

  initial_values = (Actor_SwapPlane_Initializer *)descriptor->init_variables;

  myself->base.bbox.min_x = initial_values->min_x;
  myself->base.bbox.min_y = initial_values->min_y;
  myself->base.bbox.min_z = initial_values->min_z;

  myself->base.bbox.max_x = initial_values->max_x;
  myself->base.bbox.max_y = initial_values->max_y;
  myself->base.bbox.max_z = initial_values->max_z;

  myself->front_room = initial_values->front;
  myself->back_room = initial_values->back;

  myself->display_type = 0; // No Display
};

void Actor_SwapPlane_Destroy(Actor * address, Scene_Ctx * scene) {

};

void Actor_SwapPlane_Update(Actor * actor, Scene_Ctx * scene) {
  int d_x, d_y, d_z, dist;
  Actor_SwapPlane * myself = (Actor_SwapPlane *)actor;
  //PlayerActor * player = (PlayerActor *)scene->player;
  Actor * player = scene->player; 
  // Bounding check
  //FntPrint("S:X[%d][%d]Y[%d][%d]Z[%d][%d]\n",
  //myself->base.bbox.min_x, myself->base.bbox.max_x,
  //myself->base.bbox.min_y, myself->base.bbox.max_y,
  //myself->base.bbox.min_z, myself->base.bbox.max_z);
  if(player->pos.vy > myself->base.bbox.min_y && player->pos.vy < myself->base.bbox.max_y){
    if(player->pos.vx > myself->base.bbox.min_x && player->pos.vx < myself->base.bbox.max_x){
      if(player->pos.vz > myself->base.bbox.min_z && player->pos.vz < myself->base.bbox.max_z){
          // Get Dist
          d_x = player->pos.vx - myself->base.pos.vx;
          d_y = player->pos.vy - myself->base.pos.vy;
          d_z = player->pos.vz - myself->base.pos.vz;
          dist = d_x * d_x + d_y * d_y + d_z * d_z;
          //FntPrint("Swap Dist: %d [0x%0x]\n", dist, dist);
          // Swap rooms
          if(d_x > 0) {
            scene->current_room_id = myself->front_room;
          } else {
            scene->current_room_id = myself->back_room;
          }
          scene->previous_room_m = NULL;
      }
    }
  }

};

u_char * Actor_SwapPlane_Draw(Actor * address, void * view, unsigned char * buffer, void * scene) {
  return buffer;
};

unsigned long Scene_ActorFactory(unsigned int type) {
  switch(type) {
    case ACTOR_TYPE_SWAP_PLANE:
      return sizeof(Actor_SwapPlane);
    default:
      return 0;
  }
};


void SceneInitialize() {
  void * dataptr;
  unsigned long datasize;
  printf("SceneInitialize\n");
  // The linear stack heap is initialized here because
  // when overlays are implemented it needs to tell
  // where the free memory for that overlay starts and end
  // and create the heap from there.
  // Calling LStack_Alloc before this will cause undefined behavior
  // (actually it will probably crash the game)
  u_long ram_bytes = (RAM_SIZE - (((u_long)&__heap_start) & 0xFFFFFF));
  __heap_size = ram_bytes - __stack_size;
  printf("__heap_start 0x%0X, __heap_size 0x%0X\n", &__heap_start, __heap_size);
  printf("Total heap size: %dKB (%d bytes) %d\n",ram_bytes >> 10, ram_bytes, ((u_long)&__heap_start) & 0xFFFFFF);
  
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

  // Initialize player model
  PlayerSetupData((u_long*)player_model, (u_long*)player_anime);
  PlayerInitialize((Actor*)&playerActor);

  player_prop_sword = SGM2_LoadFile((u_long*)player_prop_sword);
  player_prop_sword->material[0].clut = GetClut(MAP_TEXTURE_CLUT_X, MAP_TEXTURE_CLUT_Y);

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
  printf("Total free memory: %dKB (load start)\n",LStack_GetFree() >> 10);
  // Load initial scene data
  SceneLoad();
}

// Sets up and loads a new scene/map area
void SceneLoad() {
  void * dataptr;
  unsigned long datasize;

  scene = LStack_Alloc(sizeof(Scene_Ctx)); // Allocate scene structure

  // Setup scene data
  scene->num_rooms = 4;
  #define TRANSITION_ACTORS_N 4
  scene->transition_actors_num = TRANSITION_ACTORS_N;

  // Init Actor List
  for(int i = 0; i < ACTOR_LIST_NUM; i++) {
    Scene_ActorList[i].length = 0;
    Scene_ActorList[i].start = NULL;
    Scene_ActorList[i].end = NULL;
  }

  for(int i = 0; i < MAX_PARTICLE_EMITTERS; i++) {
    particle_list[i].flags = 0;
  }
  

  //scene->transition_actors_descriptor = LStack_Alloc(sizeof(Actor_Descriptor)*TRANSITION_ACTORS_N);

  scene->transition_actors = LStack_Alloc(sizeof(Actor*)*TRANSITION_ACTORS_N);

  {
    Actor_Descriptor newactor;
    Actor_SwapPlane_Initializer * plane_init;
    newactor.x = 7818;
    newactor.y = -4096;
    newactor.z = 0;
    newactor.rot_x = 0;
    newactor.rot_y = 0;
    newactor.rot_z = 0;
    newactor.actor_type = ACTOR_TYPE_SWAP_PLANE;

    plane_init = (Actor_SwapPlane_Initializer*)newactor.init_variables;

    plane_init->max_x = 8296;
    plane_init->max_y = -3215;
    plane_init->max_z = 512;
    plane_init->min_x = 7272;
    plane_init->min_y = -4238;
    plane_init->min_z = -512;
    plane_init->front = 0;
    plane_init->back = 1;


    // Allocate
    scene->transition_actors[0] = LStack_Alloc(sizeof(Actor_SwapPlane));
    Scene_AddActor(&Scene_ActorList[ACTORTYPE_DOOR], scene->transition_actors[0]);
    //printf("printf: sizeof: actor=%d swapplane=%d\n",sizeof(Actor),sizeof(Actor_SwapPlane));
    // Create each transition actor
    Actor_SwapPlane_Init(scene->transition_actors[0], &newactor, scene);
  }

  {
    Actor_Descriptor newactor;
    Actor_SwapPlane_Initializer * plane_init;
    newactor.x = 2352;
    newactor.y = -4014;
    newactor.z = 0;
    newactor.rot_x = 0;
    newactor.rot_y = 0;
    newactor.rot_z = 0;
    newactor.actor_type = ACTOR_TYPE_SWAP_PLANE;

    plane_init = (Actor_SwapPlane_Initializer*)newactor.init_variables;

    plane_init->max_x = 2864;
    plane_init->max_y = -3502;
    plane_init->max_z = 512;
    plane_init->min_x = 1840;
    plane_init->min_y = -4526;
    plane_init->min_z = -512;
    plane_init->front = 1;
    plane_init->back = 2;

    // Allocate
    scene->transition_actors[1] = LStack_Alloc(sizeof(Actor_SwapPlane));
    //printf("printf: sizeof: actor=%d swapplane=%d\n",sizeof(Actor),sizeof(Actor_SwapPlane));
    // Create each transition actor
    Scene_AddActor(&Scene_ActorList[ACTORTYPE_DOOR], scene->transition_actors[1]);
    Actor_SwapPlane_Init(scene->transition_actors[1], &newactor, scene);
  }
  printf("Actor_SwapPlane_Init 2\n");
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
    //printf("printf: sizeof: actor=%d swapplane=%d\n",sizeof(Actor),sizeof(Actor_SwapPlane));
    // Create each transition actor
    Scene_AddActor(&Scene_ActorList[ACTORTYPE_DOOR], scene->transition_actors[2]);
    Actor_SwapPlane_Init(scene->transition_actors[2], &newactor, scene);
  }
  printf("Actor_SwapPlane_Init 3 Begin\n");
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
    //printf("printf: sizeof: actor=%d swapplane=%d\n",sizeof(Actor),sizeof(Actor_SwapPlane));
    // Create each transition actor
    Scene_AddActor(&Scene_ActorList[ACTORTYPE_DOOR], scene->transition_actors[3]);
    Actor_SwapPlane_Init(scene->transition_actors[3], &newactor, scene);
  }

  // Load scene data
  //dataptr = LStack_GetTail();

  printf("map_col = (Col2*)scene_col;\n");

  //datasize = file_load_temp_noalloc("\\DATA\\scene.cOL;1", dataptr);
  //map_col = LStack_Alloc(datasize);
  map_col = (Col2*)scene_col;

  // Load starting room model
  //dataptr = LStack_GetTail();
  //datasize = file_load_temp_noalloc("\\DATA\\ROOM00.SG2;1", dataptr);
  //map_model = LStack_Alloc(datasize);
  map_model[0] = (SGM2_File*)scene_room00_sg2;
  map_model[1] = (SGM2_File*)scene_room01_sg2;
  map_model[2] = (SGM2_File*)scene_room02_sg2;
  map_model[3] = (SGM2_File*)scene_room03_sg2;
  map_model[4] = (SGM2_File*)scene_room04_sg2;
  map_model[5] = (SGM2_File*)scene_room05_sg2;
  map_model[6] = (SGM2_File*)scene_room05_sg2_ext;

  printf("Col_LoadFile\n");


  // Initialize collision
  map_col = Col_LoadFile((u_long*)map_col);

  printf("collision addr: %x vertices: %x faces: %x planes: %x\n",map_col,map_col->vertices,map_col->faces,map_col->planes);
  printf("vert num: %d face num: %d plane num: %d\n",map_col->vertex_num, map_col->face_num, map_col->plane_num);

  printf("map: vert num %d (%d bytes)\n",map_model[4]->vertex_count, map_model[4]->vertex_count * sizeof(SVECTOR));
  // Initialize Transform Buffer
  //AGM_Init(map_model->vertex_count * sizeof(SVECTOR)); // Initialize to the size of the largest model to be transformed
  AGM_Init(2000 * sizeof(SVECTOR));

  // Initialize Map Geometry
  map_model[0] = SGM2_LoadFile((u_long*)map_model[0]);
  map_model[1] = SGM2_LoadFile((u_long*)map_model[1]);
  map_model[2] = SGM2_LoadFile((u_long*)map_model[2]);
  map_model[3] = SGM2_LoadFile((u_long*)map_model[3]);
  map_model[4] = SGM2_LoadFile((u_long*)map_model[4]);
  map_model[5] = SGM2_LoadFile((u_long*)map_model[5]);
  map_model[6] = SGM2_LoadFile((u_long*)map_model[6]);

  for(int i = 0; i < 5; i++) {
    map_model[i]->material[0].clut = GetClut(MAP_TEXTURE_CLUT_X, MAP_TEXTURE_CLUT_Y);
    map_model[i]->material[0].tpage = getTPage(1, 0, 0, 0);
  }

  map_model[5]->material[0].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y);
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

  

  //map_model[6]->material[1].clut = GetClut(MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y+10);
  //map_model[6]->material[1].tpage = getTPage(0, 0, 128, 0);

  printf("Room 005 Model Material Count: %d\n", map_model[5]->mat_count);

  printf("Material 0: clut 0x%04X tpage 0x%04X\n", map_model[6]->material[0].clut, map_model[6]->material[0].tpage);
  printf("Material 0: clut 0x%04X tpage 0x%04X\n", map_model[6]->material[1].clut, map_model[6]->material[1].tpage);

  // Initialize player object
  playerActor = LStack_Alloc(sizeof(struct PlayerActor));
  PlayerCreateInstance((Actor*)playerActor, map_col);

  scene->player = (Actor*)playerActor;

  // Initialize camera
  camera = LStack_Alloc(sizeof(struct Camera));
  Camera_Create(camera, map_col);
  camera->target_offset.vy = 256;
  Camera_SetTarget(camera, (Actor*)playerActor);
  Camera_SetStartValues(camera);
  Camera_Update(camera);
  scene->camera = camera;

  G.clear.r = 98;
  G.clear.g = 66;
  G.clear.b = 30;

  //G.clear.r = 50;
  //G.clear.g = 115;
  //G.clear.b = 213;

  


  // Initialize transition objects
  // Set Rooms
  scene->previous_room_id =  scene->current_room_id = 0;
  scene->current_room_m = map_model[scene->current_room_id];
  scene->previous_room_m = NULL;
  printf("Total free memory: %dKB (heap area)\n",LStack_GetFree() >> 10);

  {
    // Initialize actor memory area
    unsigned long arena_size = LStack_GetFree();
    unsigned long * arena_start;
    unsigned long * testptr;
    arena_size = (arena_size >> 2) << 2;
    arena_start = LStack_Alloc(arena_size);
    Arena_Init(arena_start, arena_size);
    printf("arena size: %x\n",arena_size);
    printf("arena start: %x\n", _arena_start_ptr);
    //test_pointer = Arena_Malloc(1);
    //*test_pointer = 0xFCFCFCFC;
    //printf("malloc(1): %x (content: %x)\n", test_pointer, *test_pointer);
    //testptr = Arena_Malloc(1);
    //*testptr = 0xFBFBFBFB;
    //printf("malloc(1): %x (content: %x)\n", testptr, *testptr);
  }

  // Load map (global) textures
  //file_load_temp_noalloc("\\DATA\\TEST.TIM;1", dataptr);
  //SetFarColor(255,255,255);
  //SetFarColor(128,128,128);
  //SetFarColor(98,66,30);
  // Daybreak
  G.clear.r = 237;
  G.clear.g = 192;
  G.clear.b = 255;
  SetFarColor(134,134,215);
  SetBackColor(227,217,255);
  SetFarColor(164/2, 205/2, 230/2);
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
  //load_texture_pos_fog((unsigned long)dataptr, 0, 0, 512, 496,16);
  load_texture_pos_fog((unsigned long)scene_tim, 0, 0, MAP_TEXTURE_CLUT_X, MAP_TEXTURE_CLUT_Y,16);
  load_texture_pos_fog((unsigned long)iwakabe_tim, 128, 0, MAP_TEXTURE_CLUT_X+256+32, MAP_TEXTURE_CLUT_Y,16);
  //load_texture_pos_fog((unsigned long)iwayuka_tim, 128+32, 0, MAP_TEXTURE_CLUT_X+256+32+16, MAP_TEXTURE_CLUT_Y,16);
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
  // Test sprite
  load_texture_pos((unsigned long)test_image_tim, 64, 256, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+8);
  load_texture_pos((unsigned long)test_image2_tim, 64+(13*1), 256, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+9);
  load_texture_pos((unsigned long)test_image3_tim, 64+(13*2), 256, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+10);
  load_texture_pos((unsigned long)test_image4_tim, 64+(13*3), 256, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+13);

  load_texture_pos((unsigned long)screen_lifebar_tim, LIFEMETER_TEX_X, LIFEMETER_TEX_Y, LIFEMETER_CLUT_X, LIFEMETER_CLUT_Y);

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
  printf("Loading Obj Tsubo\n");
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


  Scene_ActorList[ACTORTYPE_PLAYER].length = 1;
  Scene_ActorList[ACTORTYPE_PLAYER].start = (Actor*)playerActor;
  Scene_ActorList[ACTORTYPE_PLAYER].end = (Actor*)playerActor;

  
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
    ObjSyokudaiActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjSyokudaiActor));
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
  u_long actptr;
  for(int i = 0; i < 1; i++) {
    ObjTsuboActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjTsuboActor));
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
  }

  {
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjGrassCutActor));
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
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjGrassCutActor));
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
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjGrassCutActor));
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
    ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjGrassCutActor));
      actor->base.room = 0;
      actor->base.pos.vx = 6782;
      actor->base.pos.vy = -3763;
      actor->base.pos.vz = -23163;
      actor->base.rot.vx = 0;
      actor->base.rot.vy = rand() >> 4;
      actor->base.rot.vz = 0;
      ObjGrassCutActorInitialize((Actor*)actor, NULL, scene);
  }


  printf("Last actor pointer: 0x%0X\n", actptr);  
  scene->draw_dist = 1024;

  scene->ambient.r = 207>>1;
  scene->ambient.g = 191>>1;
  scene->ambient.b = 139>>1;

  printf("Texture Data Start: 0x%08X End: 0x%08X (Size: %d)\n", texture_data_start, texture_data_end, texture_data_end - texture_data_start);
}
    //printf("a actor: 0x%08X <<prev [0x%08X] next>> 0x%08X\n", current->prev, current, current->next);
/* __attribute__((optimize("O0"))) */
/* __attribute__((optimize("Os"))) */
void Scene_RemoveOldActors(int actor_type, Scene_Ctx * scene) {
  for(Actor * c = Scene_ActorList[actor_type].start; c; c = c->next) {
    if(c->room != scene->previous_room_id) continue;
    Scene_RemoveActor(&Scene_ActorList[actor_type], c);
    c->Destroy(c, scene);
    printf("removing actor 0x%08X\n", c);
    Arena_Free(c);
  }
}
  /*for(Actor * c = Scene_ActorList[actor_type].start; c; c = c->next) {
    test_total++;
    if(c->room != scene->previous_room_id)
      continue;

    Scene_RemoveActor(&Scene_ActorList[actor_type], c);
    c->Destroy(c, scene);
    Arena_Free(c);
    test_removed++;
  }
  printf("%d/%d objects removed\n", test_removed, test_total);
}*/
/*void  Scene_RemoveOldActors(int bg_type, Scene_Ctx * scene) {
  int test_removed = 0;
  int test_total = 0;
  for(Actor * c = Scene_ActorList[bg_type].start; c;) {
    test_total++;
    if(c->room != scene->previous_room_id) {
      c = c->next;
      continue;
    }
    Actor * next = c->next;
    Scene_RemoveActor(&Scene_ActorList[bg_type], c);
    c->Destroy(c, scene);
    Arena_Free(c);
    c = next;
    test_removed++;
  }
  printf("%d/%d objects removed\n", test_removed, test_total);
}*/

int bone_select = 0;

void SceneMain() {
  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_start = prof_current_val;
  prof_last_counter = prof_current_val;


  if(g_pad_press & PAD_R2) {
    Actor * current = Scene_ActorList[ACTORTYPE_BG].start;
    int idx = 0;
    while(current != NULL) {
      printf("list[%d] actor->prev = 0x%0X actor->next = 0x%0X\n", idx++, current->prev, current->next);
      current = current->next;
    }
  }

  //if(g_pad & PAD_START) scene->draw_dist++;
  //if(g_pad & PAD_SELECT) scene->draw_dist--;

  //FntPrint("DRAW DIST: %d\n", scene->draw_dist);

  if(scene->current_room_id != scene->previous_room_id) {
    switch(scene->current_room_id) {
      default:
        break;
      case 1:
        for(int i = 0; i < 5; i++) {
          // Move this to ObjSyokudaiActorInitialize
          ObjSyokudaiActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjSyokudaiActor));
          actor->base.room = scene->current_room_id;
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
        }
        break;
      case 2:
        for(int i = 0; i < 64; i++) {
          ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjGrassCutActor));
          actor->base.room = scene->current_room_id;
          actor->base.pos.vx = (3514/2)-(rand() * 3514 / RAND_MAX);
          actor->base.pos.vy = -4096-256;//(rand() >> 8)-4096-200;
          actor->base.pos.vz = (1142/2)-(rand() *1142 / RAND_MAX);
          actor->base.rot.vx = 0;
          actor->base.rot.vy = rand() >> 4;
          actor->base.rot.vz = 0;
          ObjGrassCutActorInitialize((Actor*)actor, NULL, scene);
        }
        break;
      case 4:
        /*for(int i = 0; i < 256; i++) {
          ObjTsuboActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjTsuboActor));
          actor->base.room = scene->current_room_id;
          actor->base.pos.vx = (rand() >> 2)-4096+(4221);
          actor->base.pos.vy = -6157;
          actor->base.pos.vz = (rand() >> 2)-4096-(10864);
          actor->base.rot.vx = 0;
          actor->base.rot.vy = 0;
          actor->base.rot.vz = 0;
          ObjTsuboActorInitialize((Actor*)actor, NULL, scene);
        }*/
        for(int i = 0; i < 512; i++) {
          ObjGrassActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjGrassActor));
          actor->base.room = scene->current_room_id;
          actor->base.pos.vx = (rand() >> 2)-4096+(4221);
          actor->base.pos.vy = -6157;
          actor->base.pos.vz = (rand() >> 2)-4096-(10864);
          actor->base.rot.vx = 0;
          actor->base.rot.vy = rand() >> 4;
          actor->base.rot.vz = 0;
          ObjGrassActorInitialize((Actor*)actor, NULL, scene);
        }
        /*for(int i = 0; i < 1024; i++) {
          ObjGrassCutActor * actor = Scene_AllocActor(&Scene_ActorList[ACTORTYPE_BG], ACTORTYPE_BG, sizeof(ObjGrassCutActor));
          actor->base.room = scene->current_room_id;
          actor->base.pos.vx = (rand() >> 2)-4096+(4221);
          actor->base.pos.vy = -6157;
          actor->base.pos.vz = (rand() >> 2)-4096-(10864);
          actor->base.rot.vx = 0;
          actor->base.rot.vy = rand() >> 4;
          actor->base.rot.vz = 0;
          ObjGrassCutActorInitialize((Actor*)actor, NULL, scene);
        }*/
        break;
    }
    Scene_RemoveOldActors(ACTORTYPE_BG, scene);
    /*Actor *c;

    printf("room change %d<->%d\n", scene->previous_room_id, scene->current_room_id);

    for(c = Scene_ActorList[ACTORTYPE_BG].start; c; c = c->next)
    {
        Actor *t = c;

        printf("actor from room %d ", t->room);

        printf("actor: %08X, next: %08X\n", t, t->next);

        test_total++;

        if(t->room != scene->previous_room_id)
            continue;

        c = c->next;

        Scene_RemoveActor(&Scene_ActorList[ACTORTYPE_BG], t);
        t->Destroy(t, scene);
        Arena_Free(t);
        printf("removing\n");
        test_removed++;
    }*/
    //printf("%d/%d objects removed\n", test_removed, test_total);
  }

  //FntPrint("ACTORTYPE_BG %d\n", Scene_ActorList[ACTORTYPE_BG].length);

  if(scene->current_room_id != scene->previous_room_id) {
    scene->previous_room_id = scene->current_room_id;
    scene->current_room_m = map_model[scene->current_room_id];
  }

  PlayerUpdate(scene->player, scene);

  // TEMP
  //if(g_pad & PAD_R1) playerActor->y_position = -4332 << 12;
  //if(g_pad & PAD_R2) playerActor->y_position = -4096 << 12;
  //if(g_pad & PAD_L2) playerActor->y_position = -6158 << 12;

  Camera_Update(camera);

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
      PlayerCreateInstance((Actor*)playerActor, map_col);
      playerActor->base.pos.vx = playerActor->x_position = playerActor->last_floor_pos_x;
      playerActor->base.pos.vy = playerActor->y_position = playerActor->last_floor_pos_y;
      playerActor->base.pos.vz = playerActor->z_position = playerActor->last_floor_pos_z;
      playerActor->x_position <<= 12;
      playerActor->y_position <<= 12;
      playerActor->z_position <<= 12;
      playerActor->y_move_dir = playerActor->base.rot.vy = playerActor->y_rotation = playerActor->last_floor_dir;
      Camera_Create(camera, map_col);
      void_out_active = 0;
      fade_counter = fade_timer;
      camera->target_offset.vy = 256;
      Camera_SetTarget(camera, (Actor*)playerActor);
      Camera_SetStartValues(camera);
      Camera_Update(camera);
      fade_direction = 0;
      fade_timer = 30;
    }
  }

  // Process Transition Actors
  /*{
    unsigned int i;
    for(i = 0; i < scene->transition_actors_num; i++){
      Actor * current = scene->transition_actors[i];
      current->Update(current, scene);
    }
  }*/

  //test_obj_syokudai[0].base.pos.vx = playerActor->base.pos.vx;
  //test_obj_syokudai[0].base.pos.vy = playerActor->base.pos.vy+256;
  //test_obj_syokudai[0].base.pos.vz = playerActor->base.pos.vz;

  VECTOR test_flame_offset = { 0, 0, 0, 0 };
  VECTOR test_flame_wp;

  ApplyMatrixLV(playerActor->L_Hand_matrix, &test_flame_offset, &test_flame_wp);

  /*
  test_obj_syokudai[0].base.pos.vx = test_flame_wp.vx;
  test_obj_syokudai[0].base.pos.vy = test_flame_wp.vy;
  test_obj_syokudai[0].base.pos.vz = test_flame_wp.vz;
  */

  //test_obj_syokudai[0].base.pos.vx = playerActor->L_Hand_matrix->t[0];
  //test_obj_syokudai[0].base.pos.vy = playerActor->L_Hand_matrix->t[1];
  //test_obj_syokudai[0].base.pos.vz = playerActor->L_Hand_matrix->t[2];

  
  if(g_pad_press & PAD_TRIANGLE) bone_select++;
  if(g_pad_press & PAD_SQUARE) bone_select--;
  if(bone_select > 50) bone_select = 50;
  if(bone_select < 0) bone_select = 0;
  

  /*
  test_obj_syokudai[0].base.pos.vx = player_bone_matrix[bone_select].t[0];
  test_obj_syokudai[0].base.pos.vy = player_bone_matrix[bone_select].t[1];
  test_obj_syokudai[0].base.pos.vz = player_bone_matrix[bone_select].t[2];
  */

  //FntPrint("Bone: %d RAND_MAX %d\n", bone_select, RAND_MAX);

  //test_obj_tsubo.base.pos.vx = player_bone_matrix[2].t[0];
  //test_obj_tsubo.base.pos.vy = player_bone_matrix[2].t[1]+32;
  //test_obj_tsubo.base.pos.vz = player_bone_matrix[2].t[2];
  //test_obj_tsubo.base.rot.vy = playerActor->base.rot.vy;

  

  // Process actor list
  // Exclude player type, process all categories

  /*for(int i = 0; i < NUM_TEST_OBJ_SYOKUDAI; i++) {
    ObjSyokudaiActor * actor = &test_obj_syokudai[i];
    actor->base.Update(actor, scene);
  }*/

  for(int i = ACTORTYPE_BG; i < ACTOR_LIST_NUM; i++) {
    Actor * current = Scene_ActorList[i].start;
    if(Scene_ActorList[i].length == 0) continue;
    while(current) {
      Actor * next = current->next;
      if((current->room == 0xFF || current->room == scene->current_room_id) && current->Update) {
        SetSpadStack(SPAD_STACK_ADDR);
        SVECTOR tdist = playerActor->base.pos;
        tdist.vx -= current->pos.vx;
        tdist.vz -= current->pos.vz;
        tdist.vx = fastabs(tdist.vx);
        tdist.vz = fastabs(tdist.vz);
        // Calculate distance to player
        current->xzDistanceSq = tdist.vx * tdist.vx + tdist.vz * tdist.vz;
        current->xzDistance = SquareRoot0(current->xzDistanceSq);


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


  //test_obj_tsubo.base.Update(&test_obj_tsubo,scene);

  scene_counter++; // Global/General Purpose Counter
}

void SceneDraw() {
  u_char * packet_b_ptr;
  MATRIX local_identity;
  MATRIX local_identity_far;

  local_identity_far = local_identity = m_identity;

  daytime_seg_interp += 16;
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

  G.clear.r = daytime_sky.r;
  G.clear.g = daytime_sky.g;
  G.clear.b = daytime_sky.b;
  SetFarColor(daytime_fog.r, daytime_fog.g, daytime_fog.b);
  SetBackColor(daytime_sun.r, daytime_sun.g, daytime_sun.b);

  BeginDraw();

  //packet_b_ptr = (u_char *)&gpu_packet[G.OTag_id];
  packet_b_ptr = G.pBuffer;

  CompMatrixLV(&camera->matrix, &m_identity, &local_identity);

  //VECTOR bgscale = {4096*2,4096*2,4096*2,0};
  //ScaleMatrixL(&local_identity_far, &bgscale);
  
  //CompMatrixLV(&camera->matrix, &local_identity_far, &local_identity_far);

  local_identity_far = local_identity;
  local_identity_far.t[0] = local_identity.t[0] >> 3;
  local_identity_far.t[1] = local_identity.t[1] >> 3;
  local_identity_far.t[2] = local_identity.t[2] >> 3;

  gte_SetRotMatrix(&local_identity_far);
  gte_SetTransMatrix(&local_identity_far);

  SetSpadStack(SPAD_STACK_ADDR);
  packet_b_ptr = SGM2_UpdateModel(map_model[6], packet_b_ptr, (u_long*)G.pOt, 2048, SGM2_RENDER_AMBIENT, scene);
  ResetSpadStack();

  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  //packet_b_ptr = SGM2_UpdateModel(map_model, packet_b_ptr, (u_long*)G.pOt, 10);

  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_update = prof_current_val - prof_last_counter;
  prof_last_counter = prof_current_val;

  if(g_pad_press & PAD_TRIANGLE) __DEBUG_TOGGLE = !__DEBUG_TOGGLE;
  //if(g_pad_press & PAD_START) __DEBUG_TOGGLE++;// = !__DEBUG_TOGGLE;
  //if(__DEBUG_TOGGLE >= 5) __DEBUG_TOGGLE = 0;

  // Update animated textures
  // Flame texture
  Scene_ScrollTexture2x(&anim_tex_flame_src, &anim_tex_flame_dest, 0x3F - ((scene_counter<<1) & 0x3F));

  //FntPrint("X: %d Y:%d Z:%d\n", playerActor->base.pos.vx, playerActor->base.pos.vy, playerActor->base.pos.vz);

  SetSpadStack(SPAD_STACK_ADDR);
  if(__DEBUG_TOGGLE){ 
    packet_b_ptr = SGM2_UpdateModel(scene->current_room_m, packet_b_ptr, (u_long*)G.pOt, 20, SGM2_RENDER_SUBDIV | SGM2_RENDER_AMBIENT, scene);
  } else {
    packet_b_ptr = SGM2_UpdateModel(scene->current_room_m, packet_b_ptr, (u_long*)G.pOt, 20, SGM2_RENDER_SUBDIV, scene);
  }
  
  //packet_b_ptr = SGM2_UpdateModel(map_model[5], packet_b_ptr, (u_long*)G.pOt, 60, SGM2_RENDER_SUBDIV | SGM2_RENDER_SUBDIV_HIGH | SGM2_RENDER_CLUTFOG, scene);
  packet_b_ptr = SGM2_UpdateModel(map_model[5], packet_b_ptr, (u_long*)G.pOt, 60, SGM2_RENDER_SUBDIV | SGM2_RENDER_SUBDIV_HIGH | SGM2_RENDER_AMBIENT, scene);
  ResetSpadStack();
  if(scene->previous_room_m != NULL && scene->previous_room_m != scene->current_room_m){
    SetSpadStack(SPAD_STACK_ADDR);
      packet_b_ptr = SGM2_UpdateModel(scene->previous_room_m, packet_b_ptr, (u_long*)G.pOt, 20, 0, scene);
    ResetSpadStack();
  }

  //SetSpadStack(SPAD_STACK_ADDR);
  //packet_b_ptr = SGM2_UpdateModel(obj_flame_model, packet_b_ptr, (u_long*)G.pOt, 0, 1);
  //ResetSpadStack();

  /*for(int i = 0; i < NUM_TEST_OBJ_SYOKUDAI; i++) {
    ObjSyokudaiActor * actor = &test_obj_syokudai[i];
    packet_b_ptr = actor->base.Draw((Actor*)actor, &camera->matrix, packet_b_ptr);
  }*/

  for(int i = ACTORTYPE_BG; i < ACTOR_LIST_NUM; i++) {
    Actor * current = Scene_ActorList[i].start;
    if(Scene_ActorList[i].length == 0) continue;
    while(current) {
      if(current->visible){
        if(current->room == 0xFF || current->room == scene->current_room_id) {
          packet_b_ptr = current->Draw(current, &camera->matrix, packet_b_ptr,scene);
        }
      }
      current = current->next;
    }
  }

  //packet_b_ptr = test_obj_tsubo.base.Draw((Actor*)&test_obj_tsubo, &camera->matrix, packet_b_ptr);
  

  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_draw_bg = prof_current_val - prof_last_counter;
  prof_last_counter = prof_current_val;

  packet_b_ptr = PlayerDraw((Actor*)playerActor, &camera->matrix, packet_b_ptr, scene);

  CompMatrixLV(&camera->matrix, playerActor->L_Hand_matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);
  /*SetSpadStack(SPAD_STACK_ADDR);
  packet_b_ptr = SGM2_UpdateModel(player_prop_sword, packet_b_ptr, (u_long*)G.pOt, 0, 0);
  ResetSpadStack();*/


  prof_current_val = GetRCnt(ROOT_COUNTER);
  prof_draw_player = prof_current_val - prof_last_counter;
  prof_last_counter = prof_current_val;

  //CompMatrixLV(&camera->matrix, playerActor->L_Hand_matrix, &local_identity);
  //gte_SetRotMatrix(&local_identity);
  //gte_SetTransMatrix(&local_identity);

  // Draw particles
  packet_b_ptr = Scene_ParticleUpdate(scene, &camera->matrix, packet_b_ptr);

  // Draw sprite (for testing)
  draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52, 16, 52, 32, 0, 0, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+8);
  packet_b_ptr += sizeof(SPRT);
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
  draw_SimpleSpriteSemi((SPRT*)packet_b_ptr, G.pOt, 512-20-52-52, 16, 52, 32, 52*3, 0, FLAME_TEX_CLUT_X, FLAME_TEX_CLUT_Y+13+2);
  packet_b_ptr += sizeof(SPRT);

  setDrawTPage((DR_TPAGE*)packet_b_ptr, 1, 0, getTPage(0, 0, 64, 256));
  addPrim(G.pOt, packet_b_ptr);
  packet_b_ptr += sizeof(DR_TPAGE);

  packet_b_ptr = Screen_LifeMeterDraw(packet_b_ptr);

  if(scene->camera->target_mode) {
    cinema_mode++;
    if(cinema_mode > 4) cinema_mode = 4;
  } else {
    if(cinema_mode != 0) cinema_mode--;
  }

  if(cinema_mode) {
    u_char targ_height = cinema_mode_anim[cinema_mode-1];
    draw_SimpleTile((TILE*)packet_b_ptr, G.pOt, 0, 0, 512, targ_height);
    packet_b_ptr += sizeof(TILE);
    draw_SimpleTile((TILE*)packet_b_ptr, G.pOt, 0, 240-targ_height, 512, targ_height);
    packet_b_ptr += sizeof(TILE);
  }

  if(fade_counter > 0) {
    addPrim(G.pOt, &fade_screen[0]);
    addPrim(G.pOt, &fade_blend[0]);

    //addPrim(G.pOt, &fade_screen[1]);
    //addPrim(G.pOt, &fade_blend[1]);
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

  

  //FntPrint("STARTING FROM SCRATCH scene.c\n");
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

void * Scene_AllocActor(ActorList * list, u_char type, u_long size) {
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

  /*current = list->start;
  int idx = 0;
  while(current != NULL) {
    printf("list[%d] actor->prev = 0x%0X actor->next = 0x%0X\n", idx++, current->prev, current->next);
    current = current->next;
  }*/
}

struct ParticleEmitter * Scene_ParticleCreate(struct ParticleEmitter * emitter, Scene_Ctx * scene) {
  struct ParticleEmitter * itr = particle_list;
  /*u_long idx = 0;
  while(itr->flags && idx < MAX_PARTICLE_EMITTERS) { // Find first inactive
    itr++; idx++;
  }
  if(idx == MAX_PARTICLE_EMITTERS-1 && itr->flags) {
    // No free particles, ignore

    return NULL;
  }
  *itr = *emitter; // Copy emitter to list
  itr->flags = 1;
  Particle3D_Create(itr);
  return itr;*/

  for(int i = 0; i < MAX_PARTICLE_EMITTERS; i++) {
    if(particle_list[i].flags) {
      continue;
    } else {
      // Free previous buffer
      //Particle3D * buff = NULL;
      if(particle_list[i].buffer) {
        //buff = particle_list[i].buffer;
        //printf("Buffer prev 0x%08X\n", particle_list[i].buffer);
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