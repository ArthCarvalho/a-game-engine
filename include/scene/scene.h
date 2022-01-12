/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef SCENE_H
#define SCENE_H

#include "model/sgm.h"
#include "global.h"

#include "particles/particles.h"

// Scene objects
#include "actor/flame_common.h"
#include "actor/a_obj_syokudai.h"
#include "actor/a_obj_tsubo.h"
#include "actor/a_obj_grass.h"
#include "actor/a_obj_grass_cut.h"
#include "actor/a_obj_door_shutter.h"
#include "actor/a_obj_swap_plane.h"
#include "actor/a_obj_dekunuts.h"
#include "actor/a_obj_flame.h"
#include "actor/a_obj_collectable.h"

// Animated Flame Texture Source Location
#define FLAME_TEX_X_SRC 496
#define FLAME_TEX_Y_SRC 448
// Animated Flame Texture Final Location
#define FLAME_TEX_X 504
#define FLAME_TEX_Y 384
#define FLAME_TEX_W 8
#define FLAME_TEX_H 64
#define FLAME_TEX_CLUT_X 768
#define FLAME_TEX_CLUT_Y 496
// Gameplay Screen Buttons
#define SCR_BUTTON_CIRCLE_X 492
#define SCR_BUTTON_CIRCLE_Y 0
#define SCR_BUTTON_CIRCLE_CLUT_X 512
#define SCR_BUTTON_CIRCLE_CLUT_Y 243
#define SCR_BUTTON_CROSS_X 500
#define SCR_BUTTON_CROSS_Y 0
#define SCR_BUTTON_CROSS_CLUT_X 512
#define SCR_BUTTON_CROSS_CLUT_Y 245
#define SCR_BUTTON_SQUARE_X 492
#define SCR_BUTTON_SQUARE_Y 20
#define SCR_BUTTON_SQUARE_CLUT_X 512
#define SCR_BUTTON_SQUARE_CLUT_Y 247
#define SCR_BUTTON_TRIANGLE_X 500
#define SCR_BUTTON_TRIANGLE_Y 20
#define SCR_BUTTON_TRIANGLE_CLUT_X 512
#define SCR_BUTTON_TRIANGLE_CLUT_Y 250
// Screen Positions
// Triangle Button: 427,14
#define SCR_POS_BUTTON_TRIANGLE_X 427+6
#define SCR_POS_BUTTON_TRIANGLE_Y 14
// Square Button: 399,30
#define SCR_POS_BUTTON_SQUARE_X 399+6
#define SCR_POS_BUTTON_SQUARE_Y 30
// Cricle Button: 455,30
#define SCR_POS_BUTTON_CIRCLE_X 455+6
#define SCR_POS_BUTTON_CIRCLE_Y 30
// Cross Button: 427,46
#define SCR_POS_BUTTON_CROSS_X 427+6
#define SCR_POS_BUTTON_CROSS_Y 46

// Modern Ui Button Back
#define SCR_BUTTON_BACK_X 480
#define SCR_BUTTON_BACK_Y 0
#define SCR_BUTTON_BACK_CLUT_X 512
#define SCR_BUTTON_BACK_CLUT_Y 251
// Modern Ui Button Labels
#define SCR_BUTTON_LABELS_X 494
#define SCR_BUTTON_LABELS_Y 40
#define SCR_BUTTON_LABELS_CLUT_X 512
#define SCR_BUTTON_LABELS_CLUT_Y 253
// Modern Ui Button Positions
#define SCR_BUTTON_MODERN_TRIANGLE_X 400+8
#define SCR_BUTTON_MODERN_TRIANGLE_Y 15
#define SCR_BUTTON_MODERN_SQUARE_X 372+8
#define SCR_BUTTON_MODERN_SQUARE_Y 32
#define SCR_BUTTON_MODERN_CIRCLE_X 428+8
#define SCR_BUTTON_MODERN_CIRCLE_Y 32
#define SCR_BUTTON_MODERN_CROSS_X 400+8
#define SCR_BUTTON_MODERN_CROSS_Y 49
// Circle Action Label
#define SCR_BUTTON_MODERN_CIRCLE_LABEL_X 423+6
#define SCR_BUTTON_MODERN_CIRCLE_LABEL_Y 38
// Cross Action Label
#define SCR_BUTTON_MODERN_CROSS_LABEL_X 395+6
#define SCR_BUTTON_MODERN_CROSS_LABEL_Y 55
// Action Labels
#define SCR_ACTION_LABELS_X 482
#define SCR_ACTION_LABELS_Y 49
#define SCR_ACTION_LABELS_CLUT_X 528
#define SCR_ACTION_LABELS_CLUT_Y 241
// Circle Action Label
#define SCR_ACTION_LABEL_CIRCLE_X 423
#define SCR_ACTION_LABEL_CIRCLE_Y 38
// Cross Action Label
#define SCR_ACTION_LABEL_CROSS_X 395
#define SCR_ACTION_LABEL_CROSS_Y 55
// Action Label Height/Width
#define SCR_ACTION_LABEL_W 60
#define SCR_ACTION_LABEL_H 13
// Action Icons
#define SCR_ACTION_BOMB_X 473
#define SCR_ACTION_BOMB_Y 49
#define SCR_ACTION_HOOKSHOT_X 473
#define SCR_ACTION_HOOKSHOT_Y 72

#define draw_SimpleSprite(p, ot, x, y, w, h, u, v, cx, cy ) {\
  setSprt(p);\
  setUV0(p, u, v);\
  setXY0(p, x, y);\
  setWH(p, w, h);\
  setRGB0(p,128,128,128);\
  setClut(p, cx, cy);\
  addPrim(ot,p);\
}

#define draw_SimpleSpriteSemi(p, ot, x, y, w, h, u, v, cx, cy ) {\
  setSprt(p);\
  setSemiTrans(p, 1);\
  setUV0(p, u, v);\
  setXY0(p, x, y);\
  setWH(p, w, h);\
  setRGB0(p,128,128,128);\
  setClut(p, cx, cy);\
  addPrim(ot,p);\
}

#define draw_SimpleTile(p, ot, x, y, w, h) {\
  setTile(p);\
  setXY0(p, x, y);\
  setWH(p, w, h);\
  setRGB0(p,0,0,0);\
  addPrim(ot,p);\
}

// TODO: move this to texture.h
// Texture metadata stub, RAM resident, texture data is VRAM resident.
typedef struct TextureEntry {
    unsigned long id;         // Texture unique identifier
    unsigned short x;         // X position in VRAM
    unsigned short y;         // Y position in VRAM
    unsigned short w;          // Texture width
    unsigned short h;          // Texture height
    unsigned short clut_x;    // leftmost address of clut block in VRAM
    unsigned short clut_y;    // topmost address of clut block in VRAM
    unsigned char bitdepth;   // load type* and texture bit depth: 4 bit, 8 bit, 16 bit, 24 bit
    unsigned char clut_count; // number of cluts this texture has
    unsigned short reserved;
    unsigned long * dataoffset;
} TextureEntry;
// ------------------------------------------
// load type*:
// 0: Load Texture and CLUT, don't generate fog
// 1: Load Texture and CLUT, generate fog
// 2: Load Texture only
// ------------------------------------------

// IDEA: The object unique identifier might be created from a hashed string that produces a 32bit value.
// This way editors and exporters can use the same algorithm to decide the ID that will be used inside the game.
typedef struct SceneObjects {
    long object_id;                   // Object unique identifier, used by actors.
    struct AGM_Model * model_agm;     // Scene-specific model id (index inside scene array)
    struct ANM_Animation * model_anm;     // Scene-specific model animation id
    unsigned long model_tex_idx;     // Scene-specific model texture id
} SceneObjects;

// Load triggers are bounding boxes that when the player is inside will start the streaming
// of another room from CD-ROM to memory.
typedef struct LoadTrigger{
    unsigned char room;         // Id/index of the room this trigger belongs to
    unsigned char next_room;    // Id/index of the next room to be loaded
    SVECTOR position;           // Trigger's world position
    // Trigger bounding box (todo: use a common struct for this data)
    short bb_min_x;
    short bb_min_y;
    short bb_min_z;
    short bb_max_x;
    short bb_max_y;
    short bb_max_z;
    unsigned short reserved;
} LoadTrigger;

// This is a list of entrances and starting points for the player when coming from other scenes
typedef struct StartEntry {
    short pos_x;            // Initial position vector
    short pos_y;
    short pos_z;
    short rot_x;            // Initial rotation vector
    short rot_y;
    short rot_z;            
    unsigned long metadata; // Entrance metadata
} StartEntry;

// This is a list of exits for this scene, used to set the exit waypoints to other scenes
typedef struct ExitEntry {
    unsigned short next_scene;  // Id of the next scene to be loaded
    unsigned short entrance_id; // Id of the entrance when entering next scene
    unsigned short metadata;    // Other exit metadata
    unsigned short reserved;
} ExitEntry;

typedef struct Waterbox {
    short pos_x;
    short pos_y;
    short pos_z;
    short size_x;
    short size_z;
    char room;                  // Which room this waterbox belongs to
    char metadata;              // Waterbox config and other properties
} Waterbox;

// Actor Initializer
typedef struct ActorCreator {
    unsigned short id;          // Actor unique identifier to be created
    short pos_x;
    short pos_y;
    short pos_z;
    short rot_x;
    short rot_y;
    short rot_z;
    unsigned short metadata;    // Actor initialization parameters (Actor dependent)
} ActorCreator;

// Actor linked list
typedef struct ActorEntry {
    Actor * actor;      // Pointer to an actor object
    struct ActorEntry * prev;  // Pointer to previous actor in liked list
    struct ActorEntry * next;  // Pointer to next entry in linked list
} ActorEntry;

typedef struct Room {
    unsigned char id;           // This room's id/index inside the Scene. Storing it here avoids needing to get this info from Scene.
    unsigned char bg_color[3];  // Background (framebuffer) clear color
    unsigned char ambient[3];   // Ambient color
    unsigned char sky_settings; // Sky/Fog settings (bitfield)
    char fog_length;            // Fog length (how far away is 100% fog) (currently value is just how many bits to shift right)
    short fog_offset;           // Fog z-depth starting position, how far away from the camera fog starts
    ActorCreator * actor_init;  // Pointer to an actor creation array
    ActorEntry * actors;        // Pointer to this room's actors linked list
    unsigned short actor_count; // How many actors are in the linked list
    //struct _SGM_FILESTR * geometry;         // Room geometry (TODO: Multiple geometry models for LOD and occlusion without changing rooms)
    unsigned long size;         // Total memory size in RAM (including geometry)
} Room;

// roomStatus bitfield:
// 0x0000 - Initial status, never loaded
// 0x0001 - CD-ROM TOC addres loaded to roomCdLocation
// 0x0002 - Room data is currently straming in (being read from disc)
// 0x0004 - Room data has finished loading successfully
// 0x0008 - CD Read error while streaming data/Read failed
typedef struct RoomEntry {
    char roomFilename[0x10];                // Room filename (might be completely replaced by CDROM TOC later)
    DslLOC roomCdLocation;                  // Room file TOC location (to be filled in after a room has been loaded or at scene load)
    unsigned short roomStatus;              // Room loading status: Bitfield (16bits)
    unsigned short reserved;
    unsigned long full_size;                // Size of the whole room file in RAM (decompressed)
    unsigned long compressed_size;          // Size of the compressed room file on disc, used for allocation
    struct Room * room;                            // Pointer to actual room data when loaded
} RoomEntry;

typedef struct TargetContext {
    SVECTOR targetPosition;
    CVECTOR targetColor;
    Actor * targettedActor;
    Actor * nextTargetActor;    
} TargetContext;

//struct COL{
//    unsigned long temp;
//};

enum {
    ROOM_TYPE_EXTERIOR,
    ROOM_TYPE_NORMAL,
    ROOM_TYPE_DUNGEON,
    ROOM_TYPE_BOSS,
    ROOM_TYPE_MAX
};

enum {
    OBJ_PLAYER,
    OBJ_TSUBO,
    OBJ_SYOKUDAI,
    OBJ_GRASS,
    OBJ_GRASS_CUT,
    OBJ_DOOR_SHUTTER,
    OBJ_SWAP_PLANE,
    OBJ_DEKUNUTS,
    OBJ_FLAME,
    OBJ_COLLECTABLE,
    OBJ_MAX
};

typedef struct {
    CVECTOR ambient_color;  // Ambient light color
    CVECTOR fog_color;      // Fog color
    CVECTOR light_color[3]; // Parallel Light Color
    short fog_start;        // Fog Start Distance
    short fog_end;          // Fog End Distance
    u_char skybox_type;     // Skybox type, 0 - disabled, 1 - Standard, 2+ - Other
} KankyouData;
// New Room Format
typedef struct Room_Data {
    u_char id;                          // This Room's ID/Index
    u_char room_type;                   // Room Type
    u_char kankyou_override;            // Enable Enviroment Settings
    KankyouData kankyou_data;           // Environment variables
    Actor_Descriptor * actor_init;      // List of actor initialization parameters
    u_short actor_count;                // Number of actors in initialization list
    void * bg_models;                   // Pointer to list of models in this room's background
    u_char bg_count;                    // Number of background models
} Room_Data;
// New Scene Format
typedef struct {
    u_char scene_type;                  // Scene Type - Outdoors, House, Dungeon, etc.
    KankyouData kankyou_data;           // Global Enviroment Settings
    Actor_Descriptor * transition_init; // Transition Actor Initialization List
    u_char transition_count;            // Number of transition actors
    Room_Data * room_data;              // List of rooms
    u_char room_count;                  // Number of rooms
    struct COL2 * collision_mesh;        // Map Collision Mesh
} Scene_Data;

typedef struct Scene_Ctx {
  Scene_Data * data;
  unsigned char current_room_id; // Current active room id
  unsigned char previous_room_id; // Id of the room that was active before the current one
  struct SGM2 * current_room_m; // Pointer to the active geometry
  struct SGM2 * previous_room_m; // Pointer to the geometry of the previous room
  Actor * player; // pointer to player actor
  struct Camera * camera;
  short draw_dist; // TEMP - Remove later
  CVECTOR ambient;
  u_char room_swap;
  u_char actor_cleanup;
  char cinema_mode;
  char cinema_mode_counter;
  char interface_fade;
  char interface_fade_counter;
  char interface_sub_counter;
} Scene_Ctx;

// Calculating the texture block and where to upload:
// All textures in a scene are stored as a single array containing data
// The only data stored inside a file is where they go in VRAM
// From the bit-deth and their width and height, each texture position inside the block
// can be calculated by calculating the total texture size from each entry's width x height x bit-depth
// plus each texture's clut area. This avoids storing the actual size for each texture allowing for
// a more compact representation in-memory.
// After loading/decompression the texture data block comes right after the scene file structure
// This structure is meant to be used only when the Scene is being initialized, after all it's data has been uploaded to the VRAM,
// it is discarded together with all of the data contained within (textures don't need to reside in RAM and VRAM at the same time)
typedef struct TextureDataBlock {
    unsigned long identifier; // TEX0 identifier
    unsigned long textureid;
    unsigned long size; // Total block data size (excluding this header)
    unsigned long pad;
} TextureDataBlock;

extern RECT anim_tex_flame_src;
extern RECT anim_tex_flame_dest;
//Scene * scene;

extern SVECTOR contact_shadow_verts[];
extern u_char contact_shadow_outter[];
extern u_char contact_shadow_inner[];

extern MATRIX player_bone_matrix[30];

void SceneInitialize();

void SceneLoad(Scene_Data * scene_data);

void SceneMain();

void SceneDraw();

/* Scene Functions */

void Scene_SpawnObject();

void Scene_ScrollTexture2x(RECT * src, RECT * dest, u_char offs);

void * Scene_AllocActor(ActorList * list, u_short type, u_long size);

void * Scene_AddActor(ActorList * list, Actor * actor);

void Scene_RemoveActor(ActorList * list, Actor * actor);

void Scene_RemoveActorRoom(ActorList * list, u_char room, Scene_Ctx * scene);

struct ParticleEmitter * Scene_ParticleCreate(struct ParticleEmitter * emitter, Scene_Ctx * scene);

u_char * Scene_ParticleUpdate(Scene_Ctx * scene, MATRIX * view, u_char * buff);

void Scene_LoadRoom(Room_Data * room, Scene_Ctx * scene);

void Scene_CreateActor(Actor_Descriptor * actdesc, u_short group, Scene_Ctx * scene);

void Draw_SetupContactShadow();

#endif