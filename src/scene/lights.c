#include "scene/lights.h"

/*typedef struct Light {
  SVECTOR pos;
  CVECTOR color;
  u_short distance;
  u_short flags;
} Light;*/

Light Lights_List[LIGHTS_MAX];
u_char Lights_Count;
Light Lights_Directional[2] = {
  {
    {0,4096,0,0},                                //SVECTOR pos;
    {128,128,128,0},                          //CVECTOR color;
    0x7FFFFFFF,                               //long distanceSq;
    0x7FFFFFFF,                               //u_short distance;
    LIGHT_ALLOCATED | LIGHT_TYPE_DIRECTIONAL, //u_short flags;
  },
  {
    {0,-4096,0,0},                                //SVECTOR pos;
    {32,48, 48,0},                          //CVECTOR color;
    0x7FFFFFFF,                               //long distanceSq;
    0x7FFFFFFF,                               //u_short distance;
    LIGHT_ALLOCATED | LIGHT_TYPE_DIRECTIONAL, //u_short flags;
  },
};

void Lights_Initialize() {
  for(int i = 0; i < LIGHTS_MAX; i++) {
    Light * light = &Lights_List[i];
    light->flags = 0;
  }
}

Light * Lights_Create(SVECTOR * position, CVECTOR * color, u_short distance, u_short flags, void * scene) {
  for(int i = 0; i < LIGHTS_MAX; i++) {
    Light * light = &Lights_List[i];
    if(light->flags & LIGHT_ALLOCATED) continue;
    // Allocate new light
    light->flags = flags | LIGHT_ALLOCATED;

    light->pos = *position;
    light->color = *color;
    light->distance = distance;
    light->distanceSq = distance * distance;
    Lights_Count++;
    return light;
    break;
  }
  // All lights have been allocated
  return NULL;
}

void Lights_Destroy(Light * light) {
  light->flags = 0;
  Lights_Count--;
}

void Lights_CalcNearest(Actor * a, void * scene) {
  long distance[3] = { -1, -1, -1 };
  for(int i = 0; i < LIGHTS_MAX; i++) {
    Light * light = &Lights_List[i];
    if(light->flags & LIGHT_ALLOCATED){
      if(!(light->flags & LIGHT_TYPE_DIRECTIONAL)) {
        // Process Point Light
        short delta_x = light->pos.vx - a->pos.vx;
        short delta_y = light->pos.vy - (a->pos.vy + a->light_center_offset);
        short delta_z = light->pos.vz - a->pos.vz;
        long distSq = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
        // Low accuracy check first (Avoid square rooting distance of far away lights)
        if(distSq > light->distanceSq) continue;
        // Check each light slot for this actor
        for(int j = 0; j < 3; j++) {
          // Check distance, if larger, continue
          if(distance[j] != -1 && distSq > distance[j]) continue;
          distance[j] = distSq;
          //Get accurate distance for attenuation (todo)
          long dist = SquareRoot0(distSq);
          long n_dist_x = (delta_x<<12) / dist;
          long n_dist_y = (delta_y<<12) / dist;
          long n_dist_z = (delta_z<<12) / dist;
          long la = 1024, lb = light->distance;
          //long attn_l = (dist<<12)/light->distance;
          //long attenuation = (4096<<12) / (1 + dist + (lb * distSq >> 12));
          long attenuation = (light->distance - dist<<12)/light->distance;

          a->color_matrix.m[0][j] = ((light->color.r << 4) * attenuation) >> 12;
          a->color_matrix.m[1][j] = ((light->color.g << 4) * attenuation) >> 12;
          a->color_matrix.m[2][j] = ((light->color.b << 4) * attenuation) >> 12;
          a->light_matrix.m[j][0] = n_dist_x;
          a->light_matrix.m[j][1] = n_dist_y;
          a->light_matrix.m[j][2] = n_dist_z;
          
          break;
        }
      }
    }
  }
  for(int i = 0; i < 2; i++) { 
    // Process directional light
    Light * light = &Lights_Directional[i];
    for(int j = 0; j < 3; j++) {
      // Find empty slots
      if(distance[j] != -1 || distance[j] == 0x7FFFFFFF) continue;
      distance[j] = 0x7FFFFFFF; // Maximum distance
      a->light_matrix.m[j][0] = light->pos.vz;
      a->light_matrix.m[j][1] = light->pos.vy;
      a->light_matrix.m[j][2] = light->pos.vz;
      a->color_matrix.m[0][j] = light->color.r << 4;
      a->color_matrix.m[1][j] = light->color.g << 4;
      a->color_matrix.m[2][j] = light->color.b << 4;
      break;
    }
  }
  for(int j = 0; j < 3; j++) {
      if(distance[j] != -1) continue;
      a->color_matrix.m[0][j] = 0;
      a->color_matrix.m[1][j] = 0;
      a->color_matrix.m[2][j] = 0;
    }
}
