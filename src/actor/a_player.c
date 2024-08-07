/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "actor/a_player.h"

#include "spadstk.h"

//#include "../../AGAME_PCDATA/player_new.agm.anm.h"
#include "../../zelda_assets/models/link_reference_new_clean.agm.anm.h"

short col_queries_test = 0;

/*
// This multiplies two 32bit fixed point (format: 20.12)
#define asm_fpmul64( result, a, b ) __asm__  volatile ( \
"mult %1, %2;"      \
"mfhi $7;"          \
"mflo $8;"          \
"sll $7, $7, 20;"   \
"srl $8, $8, 12;"   \
"or %0, $7, $8;"    \
: "=r"(result)      \
: "r"(a), "r"(b)    \
: "t0", "t1" )

// Muliply two 32bit fixed point numbers and convert back to integer
#define asm_fpmul64ri( result, a, b ) __asm__  volatile ( \
"mult %1, %2;"      \
"mfhi $7;"          \
"mflo $8;"          \
"sll $7, $7, 8;"    \
"srl $8, $8, 24;"   \
"or %0, $7, $8;"    \
: "=r"(result)      \
: "r"(a), "r"(b)    \
: "t0", "t1" )
*/

#define ANALOG_MIN 35
#define ANALOG_MAX 128
#define ANALOG_RANGE (ANALOG_MAX - ANALOG_MIN)

#define PLAYER_ROT_SPD 150
#define PLAYER_MAX_ROTINT 1536
#define PLAYER_ROT_SPD_AIR 10

/*#define PLAYER_MOVE_SPEED FIXED(35.5)
#define PLAYER_ROLL_SPEED_LOW FIXED(19.36)
//#define PLAYER_ROLL_SPEED FIXED(53.25)
#define PLAYER_ROLL_SPEED FIXED(60.25)
#define PLAYER_MOVE_FALLING FIXED(0.3227272727272728)
#define PLAYER_MOVE_FALLING_DEACCEL FIXED(0.6454545454545455)*/

#define SPEED_MULT 1.0
//#define SPEED_MULT 1.5

//#define PLAYER_MOVE_SPEED FIXED(35.5*SPEED_MULT)
#define PLAYER_MOVE_SPEED FIXED(33.0*SPEED_MULT)
#define PLAYER_ROLL_SPEED_LOW FIXED(19.36*SPEED_MULT)
//#define PLAYER_ROLL_SPEED FIXED(53.25)
#define PLAYER_ROLL_SPEED FIXED(60.25*SPEED_MULT)
#define PLAYER_MOVE_FALLING FIXED(0.3227272727272728*SPEED_MULT)
#define PLAYER_MOVE_FALLING_DEACCEL FIXED(0.6454545454545455*SPEED_MULT)

// Prepare Data (Load Assets)
int PlayerDataLoaded = 0;
short PlayerOToffset = 0;

u_long * PlayerMdlData;
u_long * PlayerAnmData;

struct AGM_Model PlayerMdl;
struct ANM_Animation * PlayerAnm;

struct SGM2 * shield_prop;
MATRIX shield_matrix;

SVECTOR frameBuffer[20];

short turn_bend;
//short turn_bend_limit = 800;
short turn_bend_limit = 1024;

short player_back_arch;
short player_back_arch_limit = 512;
short player_head_updown;
short player_head_updown_limit = 512;

u_long last_state = 0;

u_long anim_interp = 0;

u_short interp = 2048;

Col2 * col_context_pl;

Col_Result col_floor;

MATRIX player_bone_matrix[30];

void PlayerSetupData(u_long * mdl, u_long * anim) {
  PlayerMdlData = mdl;
  PlayerAnmData = anim;

  // Load
}

void PlayerLoadData() {
    if(!(PlayerDataLoaded & 0x0001)){
      file_load_temp("\\DATA\\PLAYER.AGM;1", &PlayerMdlData);
      PlayerDataLoaded |= 0x0001;
    }
    
    if(!(PlayerDataLoaded & 0x0002)){
      file_load_temp("\\DATA\\PLAYER.ANM;1", &PlayerAnmData);
      //printf("PLAYER LOADED\n");
      PlayerDataLoaded |= 0x0002;
    }
}


u_long demo_counter = 0;
short demo_dir = 4096;

void PlayerUnloadData() {
}

// Initialize a player instance
void PlayerCreateInstance(Actor * a, void * col_ctx) {
  PlayerActor * actor = (PlayerActor*)a;
 // a->Initialize = &PlayerInitialize;
//  a->Destroy = &PlayerDestroy;
//  a->Main = &PlayerMainloop;
//  a->Draw = &PlayerDraw;
  actor->holding = NULL;
  actor->interact = NULL;
  actor->interact_dist = 0;
  actor->state = 0;
  actor->current_anim = 0;
  actor->anim_frame = 0;
  actor->anim_frame_sub = 0;

  actor->state = PLAYER_ENABLE_CONTROL;

  actor->floor = &col_floor;

  col_context_pl = (Col2*) col_ctx;

  actor->speed_fall = 0;
  actor->gravity = (-5.25 * 4096) * SPEED_MULT;
  actor->max_speed_y = (-120<<12) * SPEED_MULT;

  actor->process = Player_Normal;

  //actor->base.pos.vx = 43 * 256;
  actor->base.pos.vx = 0;
  actor->base.pos.vy = 0;
  actor->base.pos.vz = 13824;

  actor->base.rot.vx = 0;
  actor->y_rotation = actor->base.rot.vy = 2048;//3072;
  actor->base.rot.vz = 0;
  actor->x_position = actor->base.pos.vx << 12;
  actor->y_position = actor->base.pos.vy << 12;
  actor->z_position = actor->base.pos.vz << 12;

  actor->base.collisionData.radius = 90;
  actor->base.collisionData.height = 333;

  actor->void_out = 0;

  //actor->y_rotation = 0;
  actor->y_move_dir = actor->y_rotation;

  actor->base.child = NULL;
  actor->base.parent = NULL;

  actor->action = ACTION_NO_ACTION;
  actor->sub_action = ACTION_NO_ACTION;

  actor->light_matrix.m[0][0] = 0;
  actor->light_matrix.m[0][1] = 4096;
  actor->light_matrix.m[0][2] = 0;
  actor->light_matrix.m[1][0] = 0;  
  actor->light_matrix.m[1][1] = 0;
  actor->light_matrix.m[1][2] = 0;
  actor->light_matrix.m[2][0] = 0;  
  actor->light_matrix.m[2][1] = -1024;
  actor->light_matrix.m[2][2] = 0;

  actor->base.light_center_offset = 300;

  // Place player on ground
  /*{
    SetSpadStack(SPAD_STACK_ADDR);
    SVECTOR pos = {actor->base.pos.vx, actor->base.pos.vy, actor->base.pos.vz, 0};
    Col_GroundCheckPoint(col_context_pl, &pos, 8129, actor->floor);
    if(actor->floor->face >= 0){
      actor->base.pos.vy = actor->floor->position.vy;
      actor->y_position = actor->floor->position.vy << 12;
    }
    ResetSpadStack();
  }*/

  actor->L_Hand_matrix = &player_bone_matrix[5];
  ////printf("transfomed: %x\n",player_bone_matrix);
}

// Player Functions
void PlayerInitialize(Actor * a) {
  // Setup Model in Memory
  AGM_LoadModel(&PlayerMdl, PlayerMdlData);
  // Setup Animation in Memory
  ANM_LoadAnimation(&PlayerAnm, (u_char*)PlayerAnmData);
}

void PlayerDestroy(Actor * a) {
}

long anim_spd = 4096 * SPEED_MULT;

short analog_l_x_n = 0, analog_l_y_n = 0;
short analog_r_x_n = 0, analog_r_y_n = 0;
short analog_l_mag;
short analog_r_mag;
short mov_vec_x;
short mov_vec_z;
long input_a;

SVECTOR rot_test = {0, 0, 0, 0};

void PlayerUpdate(Actor * a, void * scene) {
  PlayerActor * actor = (PlayerActor*)a;
  short vx = 0;
  short vz = 0;
  analog_l_x_n = 0; analog_l_y_n = 0;
  analog_r_x_n = 0; analog_r_y_n = 0;
  analog_l_mag = 0;
  analog_r_mag = 0;
  mov_vec_x = 0;
  mov_vec_z = 0;
  input_a = 0;

  actor->action_prev = actor->action;
  actor->sub_action_prev = actor->sub_action;

  if(__DEBUG_TOGGLE || actor->state & PLAYER_STATE_ONAIR) {
    PlayerOToffset = 30;
  } else {
    PlayerOToffset = 0;
  }

  
  if(actor->state & PLAYER_ENABLE_CONTROL){
    short analog_l_x = pad_analog[ANALOG_L_X] - 0x80;
    short analog_l_y = pad_analog[ANALOG_L_Y] - 0x80;
    short analog_r_x = pad_analog[ANALOG_R_X] - 0x80;
    short analog_r_y = pad_analog[ANALOG_R_Y] - 0x80;
    //static short analog_l_x_n = 0, analog_l_y_n = 0;
    //static short analog_r_x_n = 0, analog_r_y_n = 0;
    //short analog_l_mag;
    //short analog_r_mag;

    // Player Movement
    //short mov_vec_x;
    //short mov_vec_z;

    // Analog Stick Bypass (DPAD)
    if(demo_counter > 0){
      if(demo_counter > 10){
        //mov_vec_x = -4096;
        analog_l_mag = 128;
        mov_vec_z = 0;
        actor->xzspeed = PLAYER_MOVE_SPEED;
      }
      
      demo_counter--;
    } else {
      /*if(g_pad & PAD_UP) analog_l_y = -128;
      if(g_pad & PAD_DOWN) analog_l_y = 128;

      if(g_pad & PAD_LEFT) analog_l_x = -128;
      if(g_pad & PAD_RIGHT) analog_l_x = 128;*/

      //FntPrint("analog: x=%d y=%d\n",analog_l_x,analog_l_y);

      analog_l_mag = vec_mag(analog_l_x,analog_l_y);
      analog_r_mag = vec_mag(analog_r_x,analog_r_y);

      if(analog_l_mag > 0){
        analog_l_x_n = (analog_l_x * ONE) / analog_l_mag;
        analog_l_y_n = (analog_l_y * ONE) / analog_l_mag;
      }
      if(analog_r_mag > 0){
        analog_r_x_n = (analog_r_x * ONE) / analog_r_mag;
        analog_r_y_n = (analog_r_y * ONE) / analog_r_mag;
      }

      // Clamp Magnitude
      if(analog_l_mag > 128) analog_l_mag = 128;
      if(analog_l_mag < -128) analog_l_mag = -128;

      if(analog_r_mag > 128) analog_r_mag = 128;
      if(analog_r_mag < -128) analog_r_mag = -128;

      mov_vec_x = asm_fpmul64(analog_l_x_n, actor->xaxis.vx) + asm_fpmul64(analog_l_y_n, actor->zaxis.vx);
      mov_vec_z = asm_fpmul64(analog_l_x_n, actor->xaxis.vz) + asm_fpmul64(analog_l_y_n, actor->zaxis.vz);

      //model_arch = (pad_analog[ANALOG_R_Y] - 0x80) << 4;
      
      if(model_arch > 240) model_arch = 240;
      if(model_arch < -240) model_arch = -240;

      //model_head_arch_s = asm_fpmul64(model_arch, 3072);

    }

    if(analog_l_mag > 60){ // Above this value the player starts to move
      input_a = ((analog_l_mag - ANALOG_MIN)<<12) / ANALOG_RANGE;
    }

    if(actor->state & PLAYER_STATE_ONAIR) {
      actor->process = Player_Falling;
    } else {
      if(actor->process == Player_Falling) {
        actor->current_anim = ANM_Landing2Wait;
        anim_spd = 4096;
        interp = 0;
        anim_interp = 0;
        actor->anim_frame = 0;
        actor->state = 0x80000000;
      }
      actor->process = Player_Normal;
    }

    model_head_arch += asm_fpmul64((model_head_arch_s - model_head_arch),FIXED(0.16));

    //FntPrint("HEAD ARCH %d\n", model_head_arch);
    Player_UpdateActions(a, scene);

    // Execute player state action
    actor->process(a);

    // This will display a fixed point number in decimal format
    //FntPrint("Spd: %d.%d\n",(actor->xzspeed>>12),((actor->xzspeed*1000)>>12)%1000);
    actor->speed_fall += actor->gravity;

    if(g_pad & PAD_R1) actor->speed_fall = FIXED(50 * SPEED_MULT);

    if(actor->speed_fall < actor->max_speed_y) actor->speed_fall = actor->max_speed_y;

    // Calculate player movement vector from current rotation
    actor->xzvector.vx = isin(actor->y_move_dir);
    actor->xzvector.vy = icos(actor->y_move_dir);

  
    #define FLOOR_RAY_SIZE 4096*2

    // Collide against geometry
    {
      SVECTOR ray_pos;
      short floor_distance;
      last_state = actor->state;
      ray_pos.vx = actor->base.pos.vx;
      ray_pos.vy = actor->base.pos.vy+400;
      ray_pos.vz = actor->base.pos.vz;

      //FntPrint("col: %d\n",Col_GroundCheckPoint(col_context_pl, &ray_pos, FLOOR_RAY_SIZE, actor->floor));
      Col_GroundCheckPoint(col_context_pl, &ray_pos, FLOOR_RAY_SIZE, actor->floor);


      //FntPrint("FLOOR NORMAL: %d %d %d\n", actor->floor->plane->nx0, actor->floor->plane->ny0, actor->floor->plane->nz0);

      // Floor check
      floor_distance = (ray_pos.vy - 400) - actor->floor->position.vy;
      actor->floor_height = actor->floor->position.vy;


      //FntPrint("result: %d, dist: %d", actor->floor->face, floor_distance);
      model_head_arch_s = 0;
      if(actor->floor->face >= 0 && floor_distance < 50 && actor->speed_fall < 0) { // has floor collision
        // on floor
        actor->base.pos.vy = actor->floor->position.vy;
        actor->y_position = (actor->floor->position.vy) << 12;
        actor->speed_fall = 0;
        actor->state &= ~PLAYER_STATE_ONAIR;        
        if(actor->floor->plane && actor->floor->plane->ny0 < 4000){ // Slide in slopes
          //actor->move.vx += asm_fpmul64(actor->floor->plane->nx0, 4096*32);
          //actor->move.vz += asm_fpmul64(actor->floor->plane->nz0, 4096*32);
          SVECTOR plane_normal = {actor->floor->plane->nx0, actor->floor->plane->ny0, actor->floor->plane->nz0, 0};
          SVECTOR move_vec_dir = {actor->xzvector.vx, 0, actor->xzvector.vy, 0};
          short m_p_dot = dotProductXYZ(&move_vec_dir, &plane_normal);
          model_head_arch_s = (m_p_dot>>4) ;

          m_p_dot = 1638 + (2458 - asm_fpmul64(fastabs(m_p_dot), 2458));

          actor->xzspeed = asm_fpmul64(actor->xzspeed, m_p_dot);
          //actor->move.vx = asm_fpmul64(actor->move.vx, m_p_dot);
          //actor->move.vz = asm_fpmul64(actor->move.vz, m_p_dot);

          //actor->move.vx = asm_fpmul64(actor->move.vx, move_vec_dir.vx - asm_fpmul64(plane_normal.vx, m_p_dot));
          //actor->move.vz = asm_fpmul64(actor->move.vz, move_vec_dir.vz - asm_fpmul64(plane_normal.vz, m_p_dot));
          //actor->xzspeed = asm_fpmul64(actor->xzspeed, m_p_dot);


          //FntPrint("m_p_dot %d\n", m_p_dot);
          //FntPrint("dir (%d, %d, %d)\n", move_vec_dir.vx, move_vec_dir.vy, move_vec_dir.vz);
          //FntPrint("sqmag %d\n", dotProductXYZ(&plane_normal, &plane_normal));
        }

        actor->move.vx = asm_fpmul64(actor->xzvector.vx, actor->xzspeed);
        actor->move.vy = actor->speed_fall;
        actor->move.vz = asm_fpmul64(actor->xzvector.vy, actor->xzspeed);
        //actor->move.vz = asm_fpmul64(actor->move.vz, 4096 - actor->floor->plane->nz0);
      } else {
        // on air
        //actor->y_position -= 80;
        actor->base.pos.vy = FROM_FIXED(actor->y_position);
        actor->state |= PLAYER_STATE_ONAIR;

        actor->move.vx = asm_fpmul64(actor->xzvector.vx, actor->xzspeed);
        actor->move.vy = actor->speed_fall;
        actor->move.vz = asm_fpmul64(actor->xzvector.vy, actor->xzspeed);
      }
    }

    actor->x_position += actor->move.vx + (actor->base.collisionData.displacement_x << 12);
    actor->y_position += actor->move.vy + (actor->base.collisionData.displacement_y << 12);
    actor->z_position += actor->move.vz + (actor->base.collisionData.displacement_z << 12);

    if(!(actor->state & PLAYER_STATE_ONAIR)) {
      actor->last_floor_pos_x = actor->base.pos.vx;
      actor->last_floor_pos_y = actor->base.pos.vy;
      actor->last_floor_pos_z = actor->base.pos.vz;
      actor->last_floor_dir = actor->base.rot.vy;
    }
  
    // Set the actor position from fixed-point space into integer-space
    actor->base.pos.vx = FROM_FIXED(actor->x_position);
    actor->base.pos.vy = FROM_FIXED(actor->y_position);
    actor->base.pos.vz = FROM_FIXED(actor->z_position);

    //FntPrint("pos x%d y%d z%d\n", actor->base.pos.vx, actor->base.pos.vy, actor->base.pos.vz);

    actor->base.rot.vy = actor->y_rotation;

    actor->base.collisionData.displacement_x = 0;
    actor->base.collisionData.displacement_y = 0;
    actor->base.collisionData.displacement_z = 0;

    
  }
  MATRIX local_identity;
  SVECTOR local_rotate; 
  u_short animation_start;
  u_short animation_length;
  u_short animation_framesz;
  u_short framenext;
  u_long prevFrame = 0;

  // ANIMATION
  animation_start = PlayerAnm->animation_list[actor->current_anim].start;
  animation_length = PlayerAnm->animation_list[actor->current_anim].length;
  animation_framesz = PlayerAnm->frame_size;
  prevFrame = anim_interp;
  anim_interp = (anim_interp + anim_spd) % (animation_length << 12);
  interp = anim_interp & 0x00000FFF;
  actor->anim_frame = (anim_interp>>12);
  framenext = (actor->anim_frame + 1) % animation_length;
  if(framenext < actor->anim_frame && actor->anim_no_loop){
    anim_interp = prevFrame;
    interp = 0;
    actor->anim_frame = prevFrame >> 12;
    framenext = actor->anim_frame;
  }

  // Actor Tranformation
  local_identity.t[0] = actor->base.pos.vx;
  local_identity.t[1] = actor->base.pos.vy;
  local_identity.t[2] = actor->base.pos.vz;

  // Actor Rotation
  local_rotate.vx = actor->base.rot.vx;
  local_rotate.vy = actor->base.rot.vy;
  local_rotate.vz = actor->base.rot.vz;

  RotMatrixZYX_gte(&local_rotate,&local_identity);

  actor->light_matrix = local_identity;

  interp = interp % 4096;

  SetSpadStack(SPAD_STACK_ADDR);
  // Interpolate Animation
  ANM_InterpolateFrames(frameBuffer,
                        &PlayerAnm->animation_data[(animation_start + actor->anim_frame)*animation_framesz],
                        &PlayerAnm->animation_data[(animation_start + framenext)*animation_framesz],
                        interp,
                        animation_framesz);
  ResetSpadStack();
/*
  { // Manipulate animation
    QUATERNION quat;
    SVECTOR rot;
    rot_test.vx = 304;
    //if(g_pad & PAD_L2) rot_test.vy+=16;
    //if(g_pad & PAD_R2) rot_test.vy-=16;
    //SVECTOR rot = {0, 1024, 0, 0};
    FntPrint("ARM: X%d Y%d Z%d\n", rot_test.vx, rot_test.vy, rot_test.vz);
    fix12_toQuaternion((QUATERNION*)&frameBuffer[4], &rot_test);
    rot = rot_test;
    rot.vx = -304;
    fix12_toQuaternion((QUATERNION*)&frameBuffer[7], &rot);
    //fix12_quaternionMul((QUATERNION*)&frameBuffer[3], (QUATERNION*)&frameBuffer[3], &quat);
    //fix12_quaternionMul((QUATERNION*)&frameBuffer[6], (QUATERNION*)&frameBuffer[6], &quat);
  }*/

  AGM_TransformBones(&PlayerMdl, &local_identity, frameBuffer, player_bone_matrix);

  // translate child to player position
  if(actor->base.child != NULL) {
    actor->base.child->pos.vx = player_bone_matrix[2].t[0];
    actor->base.child->pos.vy = player_bone_matrix[2].t[1]+32;
    actor->base.child->pos.vz = player_bone_matrix[2].t[2];
    actor->base.child->rot.vy = actor->base.rot.vy;
    actor->base.child->room = 0xFF;
  }

  {
    MATRIX local = {
      0, 0, -4096,
      0, 4096, 0,
      -4096, 0, 0,
      35, 25, 0// hand-5, 30, 0
    };
    SVECTOR shield_offset = { 0, 0, 0 , 0 };
    SVECTOR shield_rot = { (4096 * (35.0 / 360.0)), 3072, 0, 0 };
    //SVECTOR shield_rot = { 0, 0, 3072, 0 };

    //RotMatrixYXZ(&shield_rot, &local);
    RotMatrix_gte(&shield_rot, &local);

    // Draw Props
    CompMatrixLV(&player_bone_matrix[0], &local, &shield_matrix);

    //CompMatrixLV(&player_bone_matrix[7], &local, &shield_matrix);

    //shield_matrix = player_bone_matrix[0];
  }
  SetSpadStack(SPAD_STACK_ADDR);
  Col_Result col_result;
  SVECTOR shadow_pos = { player_bone_matrix[12].t[0], player_bone_matrix[12].t[1]+64, player_bone_matrix[12].t[2], 0 };
  if(Col_GroundCheckPoint(col_context_pl, &shadow_pos, 256, &col_result) != -1) {
    actor->contact_shadow_l.vx = player_bone_matrix[12].t[0];
    actor->contact_shadow_l.vy = col_result.position.vy;
    actor->contact_shadow_l.vz = player_bone_matrix[12].t[2];
    actor->contact_shadow_flags |= 0x01;
  } else {
    actor->contact_shadow_flags &= ~0x01;
  }

  shadow_pos.vx = player_bone_matrix[15].t[0];
  shadow_pos.vy = player_bone_matrix[15].t[1]+64;
  shadow_pos.vz = player_bone_matrix[15].t[2];

  if(Col_GroundCheckPoint(col_context_pl, &shadow_pos, 256, &col_result) != -1) {
    actor->contact_shadow_r.vx = player_bone_matrix[15].t[0];
    actor->contact_shadow_r.vy = col_result.position.vy; // actor->floor_height;
    actor->contact_shadow_r.vz = player_bone_matrix[15].t[2];
    actor->contact_shadow_flags |= 0x02;
  } else {
    actor->contact_shadow_flags &= ~0x02;
  }

  for(int i = 0; i < col_queries_test; i++) { 
    
    shadow_pos.vx = player_bone_matrix[15].t[0];
    shadow_pos.vy = player_bone_matrix[15].t[1]+64;
    shadow_pos.vz = player_bone_matrix[15].t[2];
    Col_GroundCheckPoint(col_context_pl, &shadow_pos, 4096, &col_result);
    actor->contact_shadow_r.vx = player_bone_matrix[15].t[0];
    actor->contact_shadow_r.vy = col_result.position.vy; // actor->floor_height;
    actor->contact_shadow_r.vz = player_bone_matrix[15].t[2];
    
  }
  ResetSpadStack();

  FntPrint("Collision Queries: %d\n", col_queries_test);
  if(g_pad_press & PAD_SELECT) col_queries_test--;
  if(g_pad_press & PAD_START) col_queries_test++;
    
  // Reset Interact Actor
  actor->interact = NULL;
}

//short temp_scale = 0;

u_char * PlayerDraw(Actor * a, MATRIX * view, u_char * pbuff, void * scene) {
  PlayerActor * actor = (PlayerActor*)a;
  Scene_Ctx * scene_ctx = (Scene_Ctx *)scene;
  MATRIX local_identity;


  // test lights
  /*MATRIX idle = {
    4096, 0, 0,
    0, 4096, 0,
    0, 0, 4096,
  };
    /*MATRIX lights = {
      4096, 0, 0, // L1
      0, 4096, 0, // L2
      0, 0, 4096  // L3
    };*/

    /*MATRIX lights = {
      0, -4096, 0, // L1
      0, 4096, 0, // L2
      0, 0, 0  // L3
    };*/
    /*MATRIX light_colors = {
//   L1 L2 L3
      4096,                 4096,                 4096*(75.0/255.0), // R
      4096*(127.0/255.0),   4096*(127.0/255.0),   4096*(168.0/255.0), // G
      4096*(39.0/255.0) ,   4096*(39.0/255.0) ,   4096*(218.0/255.0), // B
    };
    /*MATRIX light_colors = {
//   L1 L2 L3
      4096,                 0, 0, // R
      0,   4096, 0, // G
      0 ,   0, 4096, // B
    };*/
    /*MATRIX local_lights;

    //MulMatrix0(&lights, &player_bone_matrix[0], &local_lights);
    //MulMatrix0(&lights, &idle, &local_lights);
    */

    Lights_CalcNearest(&actor->base, scene);

    MATRIX templocal;
    //MulMatrix0(&actor->base.light_matrix, &actor->light_matrix, &templocal);
    //MulMatrix0(&actor->base.light_matrix, &player_bone_matrix[0], &templocal);
    
    SetColorMatrix(&actor->base.color_matrix);
    //SetLightMatrix(&actor->base.light_matrix);
    SetLightMatrix(&templocal);

  /*SVECTOR local_rotate; 
  u_short animation_start;
  u_short animation_length;
  u_short animation_framesz;
  u_short framenext;
  u_short angA = 1024;
  u_short angB = 4096;
  u_long prevFrame = 0;
  

  // ANIMATION
  animation_start = PlayerAnm->animation_list[actor->current_anim].start;
  animation_length = PlayerAnm->animation_list[actor->current_anim].length;
  animation_framesz = PlayerAnm->frame_size;
  prevFrame = anim_interp;
  anim_interp = (anim_interp + anim_spd) % (animation_length << 12);
  interp = anim_interp & 0x00000FFF;
  actor->anim_frame = (anim_interp>>12);
  framenext = (actor->anim_frame + 1) % animation_length;
  if(framenext < actor->anim_frame && actor->anim_no_loop){
    anim_interp = prevFrame;
    interp = 0;
    actor->anim_frame = prevFrame >> 12;
    framenext = actor->anim_frame;
  }

  //FntPrint("\nanm: frame: %d next: %d int: %d\n", actor->anim_frame, framenext, interp );

  //actor->anim_frame = 295;
  //framenext = 289;
  // ANIMATION END

  // Actor Tranformation
  local_identity.t[0] = actor->base.pos.vx;
  local_identity.t[1] = actor->base.pos.vy;
  local_identity.t[2] = actor->base.pos.vz;

  // Actor Rotation
  local_rotate.vx = actor->base.rot.vx;
  local_rotate.vy = actor->base.rot.vy;
  local_rotate.vz = actor->base.rot.vz;

  RotMatrixZYX_gte(&local_rotate,&local_identity);
  //CompMatrixLV(view, &local_identity, &local_identity);

  interp = interp % 4096;

  SetSpadStack(SPAD_STACK_ADDR);
  // Interpolate Animation
  ANM_InterpolateFrames(frameBuffer,
                        &PlayerAnm->animation_data[(animation_start + actor->anim_frame)*animation_framesz],
                        &PlayerAnm->animation_data[(animation_start + framenext)*animation_framesz],
                        interp,
                        animation_framesz);
  ResetSpadStack();
  */

  //AGM_TransformByBoneCopy(&PlayerMdl, &local_identity, frameBuffer, view, player_bone_matrix);


  
  AGM_TransformModelOnly(&PlayerMdl, player_bone_matrix, view, &actor->base.light_matrix);
  SetSpadStack(SPAD_STACK_ADDR);

  //SetSpadStack(SPAD_STACK_ADDR);
  pbuff = AGM_DrawModel(&PlayerMdl, pbuff, (u_long*)G.pOt, PlayerOToffset);
  ResetSpadStack();

  CompMatrixLV(view, &shield_matrix, &local_identity);
  gte_SetRotMatrix(&local_identity);
  gte_SetTransMatrix(&local_identity);

  SetSpadStack(SPAD_STACK_ADDR);
  pbuff = SGM2_UpdateModel(shield_prop, pbuff, (u_long*)G.pOt, PlayerOToffset-3, SGM2_RENDER_AMBIENT, scene);
  ResetSpadStack();
  
  for(int i = 0; i < 2; i++) { 
    if(actor->contact_shadow_flags & 0x01) pbuff = Draw_ContactShadow(&actor->contact_shadow_l, actor->nearest_shadow_ang[i], 4343, actor->nearest_light_str[i], pbuff, scene, -3);
    if(actor->contact_shadow_flags & 0x02) pbuff = Draw_ContactShadow(&actor->contact_shadow_r, actor->nearest_shadow_ang[i], 4343, actor->nearest_light_str[i], pbuff, scene, -3);
    actor->nearest_shadow_ang[i] = 1024+512;
    actor->nearest_light_dist[i] = 0x7FFF;
    actor->nearest_light_str[i] = 2048;
    actor->light_matrix.m[i][0] = 0; 
    actor->light_matrix.m[i][0] = 4096;
    actor->light_matrix.m[i][0] = 0;
  }

  //if(g_pad & PAD_CIRCLE) temp_scale++;
  //if(g_pad & PAD_CROSS) temp_scale--;

  //FntPrint("TempScale %d\n", 4096+temp_scale);


  return pbuff;
}

//long accel_up = FIXED(12.0f);
//long accel_down = FIXED(-8.5f);

long accel_up = FIXED(12.0f*0.5f);
long accel_down = FIXED(-8.5f);
/*
long sign(long x)
{
    // assumes 32-bit int/unsigned
    return ((unsigned)-x >> 31) - ((unsigned)x >> 31);
}
*/
long PlayerAccelerate(long accel, long current) {
  long diff = accel - current;
  if(diff == 0) return accel;
  if(diff > 0){ // If positive
    if(diff > accel_up) {
      return current + accel_up;
    } else {
      return current + diff;
    }
  } else {
    if(diff < accel_down){
      return current + accel_down;
    } else {
      return current + diff;
    }
  }
}

long PlayerAccelerateVal(long accel, long current, long up, long down) {
  long diff = accel - current;
  if(diff == 0) return accel;
  if(diff > 0){ // If positive
    if(diff > up) {
      return current + up;
    } else {
      return current + diff;
    }
  } else {
    if(diff < down){
      return current + down;
    } else {
      return current + diff;
    }
  }
}

void Player_Normal(Actor * a) {
  PlayerActor * actor = (PlayerActor *)a;
  //FntPrint("Player: Normal\n");

  FntPrint("actor->state %x\n", actor->state);
  FntPrint("actor->anim_frame = %d\n", actor->anim_frame);
  FntPrint("xzspeed %d\n", actor->xzspeed);
  if(actor->current_anim == ANM_Landing2Wait) {
    if(actor->anim_frame >= 17) {
      actor->current_anim = ANM_IdleFree;
      anim_interp = 0;
      interp = 0;
      actor->anim_frame = 0;
      actor->xzspeed = 0;
    }
  } else if((actor->state == 0x80000000)) {
    
    if(actor->current_anim != ANM_IdleFree) {
      if(actor->current_anim != ANM_WalkEndLFree && actor->current_anim != ANM_WalkEndRFree) {
        anim_interp = 0;
        interp = 0;
        actor->anim_frame = 0;
      }
      if(actor->anim_frame <= 9) {
        actor->current_anim = ANM_WalkEndLFree;
      } else {
        actor->current_anim = ANM_WalkEndRFree;
      }
      
      anim_spd = 4096 * SPEED_MULT;
      if(actor->anim_frame >= 9) {
        actor->current_anim = ANM_IdleFree;
        anim_spd = 2048 * SPEED_MULT;
        anim_interp = 0;
        interp = 0;
        actor->anim_frame = 0;
        actor->xzspeed = 0;
      }
    } else {
      actor->current_anim = ANM_IdleFree;
    }
  }
  

  //actor->current_anim = ANM_IdleFree;
  //anim_spd = 2048 * SPEED_MULT;
  actor->anim_no_loop = 0;

  if(last_state & PLAYER_STATE_ONAIR) {
    // From Falling => Stand
    actor->xzspeed = 0;
  }

  switch(actor->state & 0x0000000F){
      default:
      case PLAYER_STATE_WAIT:
      //actor->action = ACTION_NO_ACTION;
      case PLAYER_STATE_MOVE:
      {
        long spdconst;
        short y_dir_diff;
        short new_y_direction;
        
        if(actor->xzspeed <= 0 && actor->state & PLAYER_STATE_MOVE){
          actor->state &= ~PLAYER_STATE_MOVE; // reset movement state
        }
        
        if(analog_l_mag > ANALOG_MIN && !demo_counter){
          // Converts the analog input
          // Convert Vector to Angle
          new_y_direction = (fix12_atan2(mov_vec_x, mov_vec_z) << 7 ) / 0x0324;
          // Smoothly rotate to new y rotation if angle is below 135 degrees
          y_dir_diff = shortAngleDist(actor->y_move_dir, new_y_direction);
          if(y_dir_diff > 0 && y_dir_diff < PLAYER_MAX_ROTINT) {
            if(y_dir_diff > 0 && y_dir_diff < PLAYER_ROT_SPD){
              actor->y_move_dir = new_y_direction;
            }
            if(y_dir_diff > 0 && y_dir_diff > PLAYER_ROT_SPD){
            actor->y_move_dir = (actor->y_move_dir + PLAYER_ROT_SPD) % 4096;
            }
          } else if(y_dir_diff < 0 && y_dir_diff > -PLAYER_MAX_ROTINT) {
            if(y_dir_diff < 0 && y_dir_diff > -PLAYER_ROT_SPD){
              actor->y_move_dir = new_y_direction;
            }
            if(y_dir_diff < 0 && y_dir_diff < -PLAYER_ROT_SPD){
              actor->y_move_dir = (actor->y_move_dir - PLAYER_ROT_SPD) % 4096;
            }
          } else if(y_dir_diff != 0) {
            actor->y_move_dir = new_y_direction;
            actor->xzspeed = 0;
            analog_l_mag = 0;
          }

          //FntPrint("RotDiff %d\n",y_dir_diff);

          turn_bend = y_dir_diff;

          if(turn_bend > turn_bend_limit)
            turn_bend = turn_bend_limit;
          if(turn_bend < -turn_bend_limit)
            turn_bend = -turn_bend_limit;

          model_bend += (turn_bend - model_bend) >> 3;

          //FntPrint("Model Bend: %d\n", model_bend);
          
          if(analog_l_mag > 60){ // Above this value the player starts to move
            actor->state = (actor->state & PLAYER_STATE_MASK) | PLAYER_STATE_MOVE; // Set moving state
          }
        }

        if(!(actor->state & PLAYER_STATE_MOVE)){
          turn_bend = model_bend = 0;
        }

        actor->xzspeed = PlayerAccelerate(fix12_mul(input_a,PLAYER_MOVE_SPEED), actor->xzspeed);

        if(actor->xzspeed > 0) {
          // ANM_AttackRoll ANM_RunFree
          
          if(actor->xzspeed < 90000) {
            actor->current_anim = ANM_WalkFree;
            anim_spd = fix12_mul(actor->xzspeed,256*0.9);
          } else {
            actor->current_anim = ANM_RunFree;
            anim_spd = fix12_mul(actor->xzspeed,256*0.59);
            
          }
          
        }
        //FntPrint("PLAYER SPD: %d\n", actor->xzspeed);
        //if(actor->state & PLAYER_STATE_MOVE && actor->xzspeed > 100000) actor->action = ACTION_ATTACK;
        if(g_pad_press & PAD_CIRCLE && (actor->xzspeed > 100000) && (actor->state & ~PLAYER_STATE_MASK) == PLAYER_STATE_MOVE){
          actor->state = (actor->state & PLAYER_STATE_MASK) | PLAYER_STATE_ROLL;
          actor->state_sub = 0;
        }
      }
      break;
      case PLAYER_STATE_ROLL:
        //anim_spd = 4096;
        anim_spd = 5120 * SPEED_MULT;
        if(actor->state_sub == 0){
          actor->current_anim = ANM_AttackRoll;
          
          anim_interp = 0;
          actor->state_sub = 1;
          actor->anim_frame = 0;
          actor->xzspeed = PlayerAccelerate(fix12_mul(input_a,PLAYER_ROLL_SPEED), actor->xzspeed);
          if(actor->xzspeed < PLAYER_ROLL_SPEED_LOW){
            actor->xzspeed = PLAYER_ROLL_SPEED_LOW;
          }
        }
        if(actor->state_sub == 1){
          actor->current_anim = ANM_AttackRoll;
          //anim_spd = 4096;
          model_bend = 0;
          turn_bend = 0;
          if(actor->anim_frame >= 17){
            //actor->xzspeed = PlayerAccelerate(PLAYER_ROLL_SPEED_LOW, actor->xzspeed);
          } else {
            actor->xzspeed = PlayerAccelerate(fix12_mul(input_a,PLAYER_ROLL_SPEED), actor->xzspeed);
            if(actor->xzspeed < PLAYER_ROLL_SPEED_LOW){
            actor->xzspeed = PLAYER_ROLL_SPEED_LOW;
            }
          }
          if(actor->anim_frame >= 17){
            if(g_pad_press & PAD_CROSS) actor->state_sub = 0;
            if(analog_l_mag > 60){
              actor->state = (actor->state & PLAYER_STATE_MASK) | PLAYER_STATE_MOVE;
              actor->y_move_dir = (fix12_atan2(mov_vec_x, mov_vec_z) << 7 ) / 0x0324;
            }
            actor->xzspeed = PlayerAccelerate(0, actor->xzspeed);
          }
          if(actor->anim_frame >= 26){
            actor->state = (actor->state & PLAYER_STATE_MASK) | PLAYER_STATE_WAIT;
            actor->current_anim = ANM_IdleFree;
          }
        }
      break;
    }
    actor->y_rotation = actor->y_move_dir;


};

void Player_Move(Actor * a) {
  //FntPrint("Player: Move\n");
}
void Player_Jump(Actor * a) {
  //FntPrint("Player: Jump\n");
}
void Player_Falling(Actor * a) {
  PlayerActor * actor = (PlayerActor *)a;
  if(!(last_state & PLAYER_STATE_ONAIR) && actor->xzspeed > FIXED(19.36)) {
    actor->current_anim = ANM_JumpFront;
    actor->speed_fall = 50<<12;
    actor->anim_frame = 0;
    actor->anim_frame_sub = 0;
    anim_interp = 0;
    actor->anim_no_loop = 1;
    anim_spd = FIXED(0.75);
  }
  if(actor->current_anim != ANM_JumpFront){
    actor->current_anim = ANM_LandingWait;
    actor->anim_no_loop = 0;
    anim_spd = 4096;
    interp = 0;
    anim_interp = 0;
    actor->anim_frame = 0;
    actor->anim_frame_sub = 0;
  }
  
  model_bend = turn_bend = 0;

  if(analog_l_mag > 60){ // Above this value the player starts to move
    input_a = ((analog_l_mag - ANALOG_MIN)<<12) / ANALOG_RANGE;

  }

  if(analog_l_mag > 60){
    short new_y_direction;
    short y_dir_diff;
    // Converts the analog input
    // Convert Vector to Angle
    new_y_direction = (fix12_atan2(mov_vec_x, mov_vec_z) << 7 ) / 0x0324;
    // Smoothly rotate to new y rotation if angle is below 135 degrees
    y_dir_diff = shortAngleDist(actor->y_move_dir, new_y_direction);
    if(y_dir_diff > PLAYER_ROT_SPD_AIR) {
       actor->y_move_dir += PLAYER_ROT_SPD_AIR;
    } else if (y_dir_diff < -PLAYER_ROT_SPD_AIR) {
      actor->y_move_dir -= PLAYER_ROT_SPD_AIR;
    } else {
      actor->y_move_dir += y_dir_diff;
    }
  
  }

  actor->xzspeed = PlayerAccelerateVal(fix12_mul(input_a,PLAYER_MOVE_SPEED), actor->xzspeed, PLAYER_MOVE_FALLING, -PLAYER_MOVE_FALLING_DEACCEL);

  //if(actor->xzspeed < 0) actor->xzspeed = 0;

}

void Player_ForceIdle(Actor * player) {
  PlayerActor * p = (PlayerActor *)player;
  p->xzspeed = 0;
  p->state = (p->state & PLAYER_STATE_MASK) | PLAYER_STATE_WAIT;
  p->current_anim = ANM_IdleFree;
}

void Player_UpdateActions(Actor * p, void * Scene) {
  PlayerActor * player = (PlayerActor *)p;
  Actor * interact = player->interact;
  char action = ACTION_NO_ACTION;

  if(player->holding == NULL) {
    if(interact != NULL) {
      if(interact->type == OBJ_DOOR_SHUTTER) {
        action = ACTION_OPEN;
      }
    } else if (player->state & PLAYER_STATE_MOVE) {
      action = ACTION_ATTACK;
    }
  }
  player->action = action;
}