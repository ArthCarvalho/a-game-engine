/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "camera/camera.h"

#include "global.h"

// Realizing all global declarations promised on camera.h

MATRIX m_identity = {
	ONE,   0,   0,
	  0, ONE,   0,
	  0,   0, ONE,
	  0,   0,   0
};

MATRIX m_inv_identity = {
	ONE,   0,   0,
	  0, ONE,   0,
	  0,   0, ONE,
	  0,   0,   0
};

SVECTOR camera_up = { 0, ONE, 0};
//VECTOR aspect = { 6554, 4096, 4096};
VECTOR aspect = { 4096, 4096, 4096};


short player_floor;
short player_floor_forward;
short player_floor_forward2;

short camera_to_back_state = 0;
short camera_to_back_targ = 0;
short camera_to_back_curr = 0;
short camera_to_back_step = 0;
short camera_to_back_counter = 0;
short camera_to_back_hstep = 0;
short camera_fov_mod = 4096;
short camera_target_offs_before;
#define CAMERA_TARG_OFFS 300

Col2 * col_context;

void Camera_Create(struct Camera * cam, void * col){
  cam->target_pos.vx = 0;
  cam->target_pos.vy = 0;
  cam->target_pos.vz = 0;
  cam->position.vx = 0;
  cam->position.vy = 0;
  cam->position.vz = 0;
  cam->matrix = m_identity;
  //cam->aspect.vx = 6554;
	cam->aspect.vx = camera_ASPECT;
  cam->aspect.vy = 4096;
  cam->aspect.vz = 4096;
  cam->target = NULL;
  cam->target_offset.vx = 0;
  cam->target_offset.vy = 0;
  cam->target_offset.vz = 0;
	cam->min_dist = 500;
	cam->max_dist = 1400;
	cam->distance = cam->max_dist;

	cam->rotation_h_s = 1024;
	cam->rotation_v_s = -128;
	cam->rotation_h = 1024;
	cam->rotation_v = -128;
	col_context = (Col2 *)col;
	camera_to_back_state = 0;
	camera_fov_mod = cam->fov = 4096;
	cam->target_mode = 0;
	cam->state = 0;
}

unsigned long iabs(long value) {
	long mask = value >> sizeof(long) * CHAR_BIT - 1;
	return (value + mask) ^ mask;
}

short Smoothing_e(short a, short b, short c, short d) {
  short temp1 = a - b;
  short temp2;
  short ret;
  
  if (iabs(temp1) >= d)
  {
    temp2 = (temp1 * c) >> 12;
    ret = b + temp2;
  }
  else
    ret = b;

  return ret;
}

short Smoothing(short a, short b, short c) {
  short temp1 = a - b;
  short temp2;
  short ret;

	temp2 = (temp1 * c) >> 12;
	ret = b + temp2;

  return ret;
}

void Camera_LookAt(struct Camera * cam) {
  VECTOR taxis;
	SVECTOR zaxis;
	SVECTOR xaxis;
	SVECTOR yaxis;
	VECTOR pos;
	VECTOR vec;
  int temp0;
  
  setVector(&taxis,
    (cam->target_pos_s.vx)+cam->target_offset_s.vx-(cam->position.vx),
    (cam->target_pos_s.vy)+cam->target_offset_s.vy-(cam->position.vy),
    (cam->target_pos_s.vz)+cam->target_offset_s.vz-(cam->position.vz)
  );
	VectorNormalS(&taxis, &zaxis);
  crossProduct(&zaxis, &camera_up, &taxis);
	VectorNormalS(&taxis, &xaxis);
	crossProduct(&zaxis, &xaxis, &taxis);
	VectorNormalS(&taxis, &yaxis);
  
  cam->matrix.m[0][0] = xaxis.vx;	cam->matrix.m[1][0] = yaxis.vx;	cam->matrix.m[2][0] = zaxis.vx;
	cam->matrix.m[0][1] = xaxis.vy;	cam->matrix.m[1][1] = yaxis.vy;	cam->matrix.m[2][1] = zaxis.vy;
	cam->matrix.m[0][2] = xaxis.vz;	cam->matrix.m[1][2] = yaxis.vz;	cam->matrix.m[2][2] = zaxis.vz;

  // Generate XZ plane (For Player Movement)
  // This might be better off moved somewhere else later.
  // Generate a new Z vector from an Identity vector (cam_plane_y = 0, ONE, 0)
  // so that the new vectors are aligned into the XZ plane
  crossProductS(&xaxis, &camera_up, &cam->zaxis);
  // regenerate the X vector from the new Y and Z vectors
  // so it also aligns to the XZ plane
  crossProductS(&camera_up, &cam->zaxis, &cam->xaxis);
	// generate the Y axis
	//crossProductS(&cam->xaxis, &cam->zaxis, &cam->yaxis);
	cam->yaxis = yaxis;

  // Normalize XZ Plane Vectors
  temp0 = vec_mag(cam->zaxis.vx, cam->zaxis.vz);
  if(temp0 > 0){
    cam->zaxis.vx = (cam->zaxis.vx << 12) / temp0;
    cam->zaxis.vz = (cam->zaxis.vz << 12) / temp0;
  }
  temp0 = vec_mag(cam->xaxis.vx, cam->xaxis.vz);
  if(temp0 > 0){
    cam->xaxis.vx = (cam->xaxis.vx << 12) / temp0;
    cam->xaxis.vz = (cam->xaxis.vz << 12) / temp0;
  }
  // TODO: Fix precision issues
  //FntPrint("CAM PLANE\nX: %d, %d\nZ: %d, %d",
  //  ((PlayerActor*)cam->target)->xaxis.vx,
  //  ((PlayerActor*)cam->target)->xaxis.vz,
  //  ((PlayerActor*)cam->target)->zaxis.vx,
  //  ((PlayerActor*)cam->target)->zaxis.vz);

	((PlayerActor*)cam->target)->xaxis = cam->xaxis;
	((PlayerActor*)cam->target)->zaxis = cam->zaxis;

  pos.vx = -cam->position.vx;
	pos.vy = -cam->position.vy;
	pos.vz = -cam->position.vz;
  
	VECTOR aspect_mod = {cam->aspect.vx, cam->aspect.vy, cam->aspect.vz, 0};

	//FntPrint("FOVMOD %d\n", camera_fov_mod);
	aspect_mod.vx = asm_fpmul64(aspect_mod.vx, camera_fov_mod);
	aspect_mod.vy = asm_fpmul64(aspect_mod.vy, camera_fov_mod);
  ScaleMatrixL(&cam->matrix, &aspect_mod);
  ApplyMatrixLV(&cam->matrix, &pos, &vec);
	TransMatrix(&cam->matrix, &vec);
}

short camera_rot_smooth = 0;
short camera_rot_smooth_targ = 0;

void Camera_Update(struct Camera * cam){
	VECTOR last_targ_pos;
	VECTOR targ_pos_diff;

	u_int playerstate = ((PlayerActor*)cam->target)->state;
	if(cam->state != 3) {
		cam->state = 0;
		if(playerstate & PLAYER_STATE_ONAIR) cam->state = 1;
		if(((PlayerActor*)cam->target)->void_out) cam->state = 2;
	}
	
	
  switch(cam->state){
    default:
    case 0: // Normal Game Camera
      if(cam->target != NULL){
				short analog_r_x = pad_analog[ANALOG_R_X] - 0x80;
				short analog_r_y = pad_analog[ANALOG_R_Y] - 0x80;

				cam->fov = 4096;

        //Camera_GetTargetPos(cam);
				cam->target_pos.vx = ((PlayerActor*)cam->target)->base.pos.vx;
				cam->target_pos.vy = ((PlayerActor*)cam->target)->base.pos.vy;
				cam->target_pos.vz = ((PlayerActor*)cam->target)->base.pos.vz;
				

				// Smooth out target position
				last_targ_pos = cam->target_pos_s;
				cam->target_pos_s.vx += (cam->target_pos.vx - cam->target_pos_s.vx);
				cam->target_pos_s.vy += (cam->target_pos.vy - cam->target_pos_s.vy >> 1);
			  cam->target_pos_s.vz += (cam->target_pos.vz - cam->target_pos_s.vz);

				// Get difference between current and last target pos
				targ_pos_diff.vx = cam->target_pos_s.vx - last_targ_pos.vx;
				targ_pos_diff.vy = cam->target_pos_s.vy - last_targ_pos.vy;
				targ_pos_diff.vz = cam->target_pos_s.vz - last_targ_pos.vz;

				// Dot Product Player Movement VS Camera
				cam->dot_p_x = dotProductXZex2(&targ_pos_diff,&cam->xaxis);
				cam->dot_p_z = dotProductXZex2(&targ_pos_diff,&cam->zaxis);

				// Rotate camera based on player movement
				//cam->rotation_h -= (camera_dot_p_x*5120)>>13;
				if(!camera_to_back_state) {
					short rot_abs;
					short rot_r = (fix12_mul(cam->dot_p_x,FIXED(0.5)));
					rot_abs = iabs(rot_r);
					//rot_abs--;
					if(rot_abs < 0) rot_abs = 0;
					if(rot_r <= 0) rot_abs = -rot_abs;
					cam->rotation_h -= rot_abs;
					//if(rot_r < -2 || rot_r > 2) cam->rotation_h -= rot_r;
					

					// Player action (Right Analog Stick)
					if(!(analog_r_x < 35 && analog_r_x > -35)){
						camera_rot_smooth_targ = analog_r_x+1;
					} else {
						camera_rot_smooth_targ = 0;
					}
					camera_rot_smooth += asm_fpmul64(camera_rot_smooth_targ - camera_rot_smooth, FIXED(0.25));
					if(camera_rot_smooth == -3) camera_rot_smooth = 0;
					cam->rotation_h -= asm_fpmul64(camera_rot_smooth, FIXED(0.6));

					if(g_pad_press & PAD_L2) {
						cam->state = 3;
						camera_to_back_state = 1;
					}
				}

				//FntPrint("Cam orbit h %d", cam->rotation_h );

				if(!(analog_r_y < 35 && analog_r_y > -35)){
					cam->rotation_v -= fix12_smul(FIXED(0.015), analog_r_y << 5);
				}
				if(!camera_to_back_state) {
					SVECTOR pforward;
					Col_Result col_result;
					short p_floor_a = 0;
					short p_floor_b = 0;
					pforward.vx = cam->target_pos.vx - (cam->zaxis.vx * 700 >> 12); // vx >> 3
					pforward.vy = cam->target_pos.vy + 512;
					pforward.vz = cam->target_pos.vz - (cam->zaxis.vz * 700 >> 12);

					Col_GroundCheckPoint(col_context, &pforward, 4096*2, &col_result);
					if(col_result.face >= 0) p_floor_a = (col_result.position.vy - cam->target_pos.vy);
					pforward.vx = cam->target_pos.vx - (cam->zaxis.vx * 400 >> 12); // vx >> 3
					pforward.vz = cam->target_pos.vz - (cam->zaxis.vz * 400 >> 12);
					Col_GroundCheckPoint(col_context, &pforward, 4096*2, &col_result);
					if(col_result.face >= 0) p_floor_b = (col_result.position.vy - cam->target_pos.vy);
					if(p_floor_a > 1000) p_floor_a = 1000;
					if(p_floor_a < -1000) p_floor_a = -1000;
					

					cam->rotation_v = ((p_floor_b) + (p_floor_a >> 1)) >> 1;

					cam->rotation_v -= 50;

					if(cam->rotation_v < -900) cam->rotation_v = -900;
					if(cam->rotation_v > 200) cam->rotation_v = 200;

					//FntPrint("Camera V %d H %d\n", cam->rotation_v, cam->rotation_h);

					//cam->rotation_h_s += ((cam->rotation_h - cam->rotation_h_s)>>1);
					//cam->rotation_h_s += ((cam->rotation_h - cam->rotation_h_s));
					
					// value for 20fps: 0.05, 30fps: 0.0333333333333333
					{
						short temp;
						temp = Smoothing(cam->rotation_v, cam->rotation_v_s, 4096 * 0.0333333333333333);
						cam->rotation_v_s = Smoothing(cam->rotation_v_s, temp, 4096 * 0.0625);
					}
				}
				cam->rotation_h_s = cam->rotation_h;
    		//cam->rotation_v_s += ((cam->rotation_v - cam->rotation_v_s)>>5);

				cam->distance -= cam->dot_p_z;

				if(cam->distance > cam->max_dist) cam->distance = cam->max_dist;
				if(cam->distance < cam->min_dist) cam->distance = cam->min_dist;

				// Transform camera position point
				{
					MATRIX camera_rot_mtx;
					SVECTOR camera_dist = {0, 0, cam->distance, 0};
					SVECTOR camera_rot = {cam->rotation_v_s, cam->rotation_h_s, 0, 0};
					VECTOR camera_pos;

					RotMatrixZYX(&camera_rot, &camera_rot_mtx);
					ApplyMatrix(&camera_rot_mtx, &camera_dist, &camera_pos);

					cam->target_offset_s.vx += (cam->target_offset.vx - cam->target_offset_s.vx) >> 2;
					cam->target_offset_s.vy += (cam->target_offset.vy - cam->target_offset_s.vy) >> 2;
					cam->target_offset_s.vz += (cam->target_offset.vz - cam->target_offset_s.vz) >> 2;

					camera_fov_mod += (cam->fov - camera_fov_mod) >> 3;

					cam->position.vx = (cam->target_pos_s.vx) + camera_pos.vx + cam->target_offset_s.vx;
					cam->position.vy = (cam->target_pos_s.vy) + camera_pos.vy + cam->target_offset_s.vy;
					cam->position.vz = (cam->target_pos_s.vz) + camera_pos.vz + cam->target_offset_s.vz;
				}
      }
			break;
			case 1: // On Air Camera
			if(cam->target != NULL){
				long rot_v;
        //Camera_GetTargetPos(cam);
				cam->target_pos.vx = ((PlayerActor*)cam->target)->base.pos.vx;
				cam->target_pos.vy = ((PlayerActor*)cam->target)->base.pos.vy;
				cam->target_pos.vz = ((PlayerActor*)cam->target)->base.pos.vz;
				

				// Get difference between current and last target pos
				last_targ_pos = cam->target_pos_s;
				cam->target_pos_s = cam->target_pos;
				targ_pos_diff.vx = cam->target_pos_s.vx - last_targ_pos.vx;
				targ_pos_diff.vy = cam->target_pos_s.vy - last_targ_pos.vy;
				targ_pos_diff.vz = cam->target_pos_s.vz - last_targ_pos.vz;

				// Dot Product Player Movement VS Camera
				{
					//SVECTOR xaxis = { cam->matrix.m[0][0], cam->matrix.m[0][1], cam->matrix.m[0][2], 0};
					//SVECTOR yaxis = { cam->matrix.m[1][0], cam->matrix.m[1][1], cam->matrix.m[1][2], 0};
					SVECTOR zaxis = { cam->matrix.m[2][0], cam->matrix.m[2][1], cam->matrix.m[2][2], 0};
					//cam->dot_p_x = dotProductXZex2(&targ_pos_diff,&xaxis);
					cam->dot_p_z = dotProductXZex2(&targ_pos_diff,&zaxis);
					//cam->dot_p_y = dotProductXYex2(&targ_pos_diff,&yaxis);
					cam->dot_p_x = dotProductXZex2(&targ_pos_diff,&cam->xaxis);
					//cam->dot_p_z = dotProductXZex2(&targ_pos_diff,&cam->zaxis);
					cam->dot_p_y = dotProductXYex2(&targ_pos_diff,&cam->yaxis);
				}

				cam->rotation_h -= (fix12_mul(cam->dot_p_x,FIXED(0.5)));
				cam->rotation_v -= (fix12_mul(cam->dot_p_y,FIXED(0.5)));

				//FntPrint("rv %d dot y %d diff y %d\n", cam->rotation_v, cam->dot_p_y, targ_pos_diff.vy);


				if(cam->rotation_v < -700) cam->rotation_v = -700;
				if(cam->rotation_v > 900) cam->rotation_v = 900;

				cam->distance += cam->dot_p_z;
				//cam->distance += cam->dot_p_y;

				if(cam->distance > cam->max_dist) cam->distance = cam->max_dist;
				if(cam->distance < cam->min_dist) cam->distance = cam->min_dist;

				cam->rotation_v_s = Smoothing(cam->rotation_v, cam->rotation_v_s, 4096 * 0.5);

				cam->rotation_h_s = cam->rotation_h;
				//cam->rotation_v_s = cam->rotation_v;

				if(g_pad_press & PAD_L2) {
					cam->state = 3;
					camera_to_back_state = 1;
				}

				{
					MATRIX camera_rot_mtx;
					SVECTOR camera_dist = {0, 0, cam->distance, 0};
					SVECTOR camera_rot = {cam->rotation_v_s, cam->rotation_h_s, 0, 0};
					VECTOR camera_pos;

					RotMatrixZYX(&camera_rot, &camera_rot_mtx);
					ApplyMatrix(&camera_rot_mtx, &camera_dist, &camera_pos);

					cam->target_offset_s.vx += (cam->target_offset.vx - cam->target_offset_s.vx) >> 2;
					cam->target_offset_s.vy += (cam->target_offset.vy - cam->target_offset_s.vy) >> 2;
					cam->target_offset_s.vz += (cam->target_offset.vz - cam->target_offset_s.vz) >> 2;

					camera_fov_mod += (cam->fov - camera_fov_mod) >> 3;

					cam->position.vx = (cam->target_pos_s.vx) + camera_pos.vx + cam->target_offset_s.vx;
					cam->position.vy = (cam->target_pos_s.vy) + camera_pos.vy + cam->target_offset_s.vy;
					cam->position.vz = (cam->target_pos_s.vz) + camera_pos.vz + cam->target_offset_s.vz;
				}
			}
			case 2: // Fixed
				cam->target_pos.vx = ((PlayerActor*)cam->target)->base.pos.vx;
				cam->target_pos.vy = ((PlayerActor*)cam->target)->base.pos.vy;
				cam->target_pos.vz = ((PlayerActor*)cam->target)->base.pos.vz;
				

				// Get difference between current and last target pos
				last_targ_pos = cam->target_pos_s;
				cam->target_pos_s = cam->target_pos;
			break;
			case 3: // Targetting
				if(camera_to_back_state == 1) {
					cam->target_mode = 1;
					camera_target_offs_before = cam->target_offset.vy;
					cam->target_offset.vy = CAMERA_TARG_OFFS;
					short target_rot_h = ((PlayerActor*)cam->target)->base.rot.vy & 0xFFF;
					short rot_curr = cam->rotation_h & 0xFFF;
					camera_to_back_targ = (target_rot_h+2048) & 0xFFF;
					//camera_to_back_step = (camera_to_back_targ - rot_curr) / 6;
					camera_to_back_step = shortAngleDist(rot_curr, camera_to_back_targ) / 6;
					camera_to_back_hstep = (0 - cam->rotation_v_s) / 6;
					camera_to_back_state = 2;
					camera_to_back_counter = 6;
				} else if(camera_to_back_state == 2) { // Rotate to target
					short rot_curr = cam->rotation_h & 0xFFF;
					short targdiff = fastabs(camera_to_back_targ - rot_curr);
					short stepabs = fastabs(camera_to_back_step);
					if(targdiff < stepabs || camera_to_back_counter == 0) {
						cam->rotation_h_s = cam->rotation_h = camera_to_back_targ;
						cam->rotation_v_s = cam->rotation_v = 0;
						cam->fov = FIXED(1.25);
						camera_to_back_state = 3;
					} else {
						cam->rotation_h_s = cam->rotation_h += camera_to_back_step;
						cam->rotation_v_s = cam->rotation_v = (cam->rotation_v_s + camera_to_back_hstep);
						camera_to_back_counter--;
						cam->fov = FIXED(1.25);
					}
				} else if(camera_to_back_state == 3) {
					if(g_pad & PAD_L2 || (playerstate & PLAYER_STATE_ONAIR)) {
						cam->fov = FIXED(1.25);	
						cam->target_offset.vy = CAMERA_TARG_OFFS;					
					} else {
						cam->target_offset.vy = camera_target_offs_before;
						camera_to_back_state = 0;
						cam->state = 0;
						cam->target_mode = 0;
					}
				}

				

				{
					MATRIX camera_rot_mtx;
					SVECTOR camera_dist = {0, 0, cam->distance, 0};
					SVECTOR camera_rot = {cam->rotation_v_s, cam->rotation_h_s, 0, 0};
					//SVECTOR camera_rot = {0, 0, 0, 0};
					VECTOR camera_pos;

					RotMatrixZYX(&camera_rot, &camera_rot_mtx);
					ApplyMatrix(&camera_rot_mtx, &camera_dist, &camera_pos);

					cam->target_pos_s.vx += (((PlayerActor*)cam->target)->base.pos.vx - cam->target_pos_s.vx) >> 1;
					cam->target_pos_s.vy += (((PlayerActor*)cam->target)->base.pos.vy - cam->target_pos_s.vy) >> 2;
					cam->target_pos_s.vz += (((PlayerActor*)cam->target)->base.pos.vz - cam->target_pos_s.vz) >> 1;

					cam->target_offset_s.vx += (cam->target_offset.vx - cam->target_offset_s.vx) >> 2;
					cam->target_offset_s.vy += (cam->target_offset.vy - cam->target_offset_s.vy) >> 2;
					cam->target_offset_s.vz += (cam->target_offset.vz - cam->target_offset_s.vz) >> 2;

					camera_fov_mod += (cam->fov - camera_fov_mod) >> 5;

					cam->position.vx = (cam->target_pos_s.vx) + camera_pos.vx + cam->target_offset_s.vx;
					cam->position.vy = (cam->target_pos_s.vy) + camera_pos.vy + cam->target_offset_s.vy;
					cam->position.vz = (cam->target_pos_s.vz) + camera_pos.vz + cam->target_offset_s.vz;
				}

			break;
  }
	Camera_LookAt(cam);
}

void Camera_SetStartValues(struct Camera * cam) {
	if(cam->target != NULL){
		cam->target_pos.vx = ((PlayerActor*)cam->target)->base.pos.vx;
		cam->target_pos.vy = ((PlayerActor*)cam->target)->base.pos.vy;
		cam->target_pos.vz = ((PlayerActor*)cam->target)->base.pos.vz;
		cam->target_pos_s = cam->target_pos;
		cam->target_offset_s = cam->target_offset;
	}
}