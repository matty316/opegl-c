#ifndef game_object_h
#define game_object_h

#include "quad.h"

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct GameObject {
  vec3 position, velocity, acceleration, force_accum;
  float damping, angle, inverse_mass;
  Quad quad;
  struct {
    bool forward;
    bool backward;
    bool left;
    bool right;
  } movement;
} GameObject;

GameObject init_game_object(vec3 position, GLuint texture_id);
void add_force_to_game_object(GameObject *obj, vec3 force);
void update_game_object(GameObject *obj, float delta_time);
void draw_game_object(GameObject *obj, GLuint shader);
void destroy_game_object(GameObject *obj);
void set_game_object_mass(GameObject *obj, float mass);
void clear_game_object_accumulator(GameObject *obj);

#endif
