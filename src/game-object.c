#include "game-object.h"
#include "cglm/vec3.h"
#include "quad.h"
#include <assert.h>
#include <math.h>

GameObject init_game_object(vec3 position, GLuint texture_id) {
  GameObject obj;
  glm_vec3_copy(position, obj.position);
  glm_vec3_zero(obj.velocity);
  glm_vec3_zero(obj.acceleration);
  glm_vec3_zero(obj.force_accum);
  obj.damping = 0.1f;

  obj.movement.forward = false;
  obj.movement.backward = false;
  obj.movement.left = false;
  obj.movement.right = false;
  obj.angle = 0.0f;
  set_game_object_mass(&obj, 10.0f);

  vec3 rotation = {0.0f, 0.0f, 1.0f};
  obj.quad = init_quad(position, rotation, 0.0f, 100.0f, texture_id);

  return obj;
}

void update_game_object(GameObject *obj, float delta_time) {
  if (obj->inverse_mass <= 0.0f) return;

  assert(delta_time > 0.0f);

  float rotation = 0.0f;
  float angle = 90.0f;
  float turning_min = 20.0f;
  float accel_magnitude = 100.0f;

  if (obj->movement.forward) {
    vec3 force = {accel_magnitude, accel_magnitude, 0.0f};
    add_force_to_game_object(obj, force);
  }

  if (obj->movement.backward) {
    float reverse_factor = 0.5f;
    vec3 force = {-accel_magnitude, -accel_magnitude, 0.0f};
    glm_vec3_scale(force, reverse_factor, force);
    add_force_to_game_object(obj, force);
  }


  if (obj->movement.left && (obj->velocity[1] > turning_min || obj->velocity[1] < -turning_min))
    rotation += glm_rad(angle);
  if (obj->movement.right && (obj->velocity[1] > turning_min || obj->velocity[1] < -turning_min))
    rotation -= glm_rad(angle);

  vec3 accum;
  glm_vec3_scale(obj->force_accum, obj->inverse_mass, accum);

  vec3 resulting_accel;
  glm_vec3_add(obj->acceleration, accum, resulting_accel);

  glm_vec3_add(obj->velocity, resulting_accel, obj->velocity);

  obj->angle += rotation * delta_time;
  obj->quad.angle = obj->angle;

  glm_vec3_scale(obj->velocity, powf(obj->damping, delta_time), obj->velocity);

  float adjusted_angle = glm_rad(90.0f);

  obj->position[0] += cos(obj->angle + adjusted_angle) * obj->velocity[0] * delta_time;
  obj->position[1] += sin(obj->angle + adjusted_angle) * obj->velocity[1] * delta_time;
  obj->position[2] += obj->velocity[2] * delta_time;

  glm_vec3_copy(obj->position, obj->quad.position);

  update_quad(&obj->quad);
  clear_game_object_accumulator(obj);
}

void draw_game_object(GameObject *obj, GLuint shader) {
  draw_quad(&obj->quad, shader);
}

void destroy_game_object(GameObject *obj) {
  destroy_quad(&obj->quad);
}

void set_game_object_mass(GameObject *obj, float mass) {
  obj->inverse_mass = 1.0f / mass;
}

void clear_game_object_accumulator(GameObject *obj) {
  glm_vec3_zero(obj->force_accum);
}

void add_force_to_game_object(GameObject *obj, vec3 force) {
  glm_vec3_add(obj->force_accum, force, obj->force_accum);
}
