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
#include "actor/a_obj_syokudai.h"
#include "actor/a_obj_tsubo.h"
#include "actor/a_obj_grass.h"
#include "actor/a_obj_grass_cut.h"
#include "actor/a_obj_door_shutter.h"
#include "actor/a_obj_swap_plane.h"

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
    OBJ_MAX
};

typedef struct {
    CVECTOR ambient_color;  // Ambient light color
    CVECTOR fog_color;      // Fog color
    CVECTOR light_color[3]; // Parallel Light Color
    short fog_start;        // Fog Start Distance
    short fog_end;          // Fog End Distance
    u_char skybox_type;     // Skybox type, 0 - disabled, 1 - Standard, 2+ - Other
} Room_Kankyou;
// New Room Format
typedef struct Room_Data {
    u_char id;                          // This Room's ID/Index
    u_char room_type;                   // Room Type
    Room_Kankyou kankyou_data;          // Environment variables
    Actor_Descriptor * actor_init;      // List of actor initialization parameters
    u_short actor_count;                // Number of actors in initialization list
    void * bg_models;                   // Pointer to list of models in this room's background
    u_char bg_count;                    // Number of background models
} Room_Data;

typedef struct Scene {                        // Note: Struct is ordered to avoid padding
    unsigned char ambient[3];                 // Default ambient color
    unsigned char skybox_type;                // 0 - Disabled, 1 - Standard, 2 + Special
    SceneObjects * object_list;               // List of objects metadata to be used by actors in this scene
    TextureEntry * texture_list;              // List of textures (their sizes and their VRAM locations, plus default clut address)
    LoadTrigger * load_triggers;              // List of load triggers
    StartEntry * start_list;                  // List of player starting points
    ExitEntry * exit_list;                    // List of scene exits
    Waterbox * waterbox_list;                 // List of scene waterboxes
    RoomEntry * room_entry;                   // List of rooms, their filenames and status
    ActorCreator * transition_actors_init;    // Pointer to an array of actor initialization commands
    ActorEntry * transition_actors;           // Linked list of actors used for room transitions (doors, transition planes, etc.)
    struct AGM_Model * object_models_data;           // Pointer to data block containing all object models
    struct ANM_Animation * object_animations_data;   // Pointer to data block containing all object animations
    struct COL * collision_mesh;                     // Pointer to collision data
    unsigned short object_list_count;         // Numebr of object entries in object_list
    unsigned short texture_count;             // Number of textures (size of texture_list array)
    unsigned short load_triggers_count;       // Number of load trigers
    unsigned short start_list_count;          // Number of starting points
    unsigned short exit_list_count;           // Number of exit points
    unsigned short waterbox_list_count;             // Number of waterboxes
    unsigned short object_models_count;       // Number of object models
    unsigned short object_animations_count;   // Number of animation pointers
    unsigned short transition_actor_count;    // Number of transition actors in this scene
    unsigned short reserved0;
    unsigned long object_models_size;         // Total size of object models memory area
    unsigned long object_animations_size;     // Total size of object animations memory area
    unsigned long collision_mesh_size;        // Total size of collision mesh data
    unsigned char room_count;                 // Number of rooms in this scene
    unsigned char current_room;               // Current active room
    unsigned short reserved1;
    unsigned long total_size;                 // Total size of the file excluding texture data
    unsigned long * scene_heap;               // Pointer to the beginning of the scene heap, this is where room and actors are allocated
                                              // scene_heap also double as the starting point of texture data inside the Scene data block
} Scene;

typedef struct Scene_Ctx {
  unsigned char num_rooms; // Total rooms in this scene
  unsigned char current_room_id; // Current active room id
  unsigned char previous_room_id; // Id of the room that was active before the current one
  unsigned char transition_actors_num; // Total of transition actors in this scene
  Actor_Descriptor * transition_actors_descriptor;
  struct SGM2 * current_room_m; // Pointer to the active geometry
  struct SGM2 * previous_room_m; // Pointer to the geometry of the previous room
  Actor * player; // pointer to player actor
  Actor ** transition_actors; // Pointer to a list of transition actors in this scene
  struct Camera * camera;
  short draw_dist; // TEMP - Remove later
  CVECTOR ambient;
  u_char room_swap;
  u_char actor_cleanup;
  char cinema_mode;
  char cinema_mode_counter;
  char interface_fade;
  char interface_fade_counter;
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

extern MATRIX player_bone_matrix[30];

void SceneInitialize();

void SceneLoad();

void SceneMain();

void SceneDraw();

/* Scene Functions */

void Scene_SpawnObject();

void Scene_ScrollTexture2x(RECT * src, RECT * dest, u_char offs);

void * Scene_AllocActor(ActorList * list, u_char type, u_long size);

void * Scene_AddActor(ActorList * list, Actor * actor);

void Scene_RemoveActor(ActorList * list, Actor * actor);

void Scene_RemoveActorRoom(ActorList * list, u_char room, Scene_Ctx * scene);

struct ParticleEmitter * Scene_ParticleCreate(struct ParticleEmitter * emitter, Scene_Ctx * scene);

u_char * Scene_ParticleUpdate(Scene_Ctx * scene, MATRIX * view, u_char * buff);

void Scene_LoadRoom(Room_Data * room, Scene_Ctx * scene);

void Scene_CreateActor(Actor_Descriptor * actdesc, u_short group, Scene_Ctx * scene);

#endif