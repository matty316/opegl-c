#include "game-object.h"
#include "cglm/vec3.h"
#include "quad.h"

GameObject init_game_object(vec3 position, GLuint texture_id) {
  GameObject obj;
  glm_vec3_copy(position, obj.position);

  glm_vec3_zero(obj.velocity);
  vec3 accel = {10.0f, 10.0f, 0.0f};
  glm_vec3_copy(accel, obj.acceleration);
  obj.damping = 0.98f;

  obj.movement.forward = false;
  obj.movement.backward = false;
  obj.movement.left = false;
  obj.movement.right = false;
  obj.angle = 0.0f;

  vec3 rotation = {0.0f, 0.0f, 1.0f};
  obj.quad = init_quad(position, rotation, 0.0f, 100.0f, texture_id);

  return obj;
}

void update_game_object(GameObject *obj, float delta_time) {
  float rotation = 0.0f;
  float angle = 90.0f;
  float turning_min = 20.0f;

  if (obj->movement.forward)
    glm_vec3_add(obj->velocity, obj->acceleration, obj->velocity);
  if (obj->movement.backward)
    glm_vec3_sub(obj->velocity, obj->acceleration, obj->velocity);
  if (obj->movement.left && (obj->velocity[1] > turning_min || obj->velocity[1] < -turning_min))
    rotation += glm_rad(angle);
  if (obj->movement.right && (obj->velocity[1] > turning_min || obj->velocity[1] < -turning_min))
    rotation -= glm_rad(angle);

  obj->angle += rotation * delta_time;
  obj->quad.angle = obj->angle;

  glm_vec3_scale(obj->velocity, obj->damping, obj->velocity);

  float adjusted_angle = glm_rad(90.0f);

  obj->position[0] += cos(obj->angle + adjusted_angle) * obj->velocity[0] * delta_time;
  obj->position[1] += sin(obj->angle + adjusted_angle) * obj->velocity[1] * delta_time;

  glm_vec3_copy(obj->position, obj->quad.position);

  update_quad(&obj->quad);
}

void draw_game_object(GameObject *obj, GLuint shader) {
  draw_quad(&obj->quad, shader);
}

void destroy_game_object(GameObject *obj) {
  destroy_quad(&obj->quad);
}
