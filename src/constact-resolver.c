#include "cglm/vec3.h"
#include "contact-resolver.h"
#include "game-object.h"
#include <float.h>
#include <stddef.h>

void resolve_interpentration(GameObjectContact *contact, float delta_time) {
  if (contact->penetration <= 0) return;

  float total_inverse_mass = contact->objects[0]->inverse_mass;
  if (contact->objects[1]) total_inverse_mass += contact->objects[1]->inverse_mass;

  if (total_inverse_mass <= 0) return;

  vec3 move_per_inverse_mass;
  glm_vec3_scale(contact->contact_normal, contact->penetration / total_inverse_mass, move_per_inverse_mass);

  glm_vec3_scale(move_per_inverse_mass, contact->objects[0]->inverse_mass, contact->object_movements[0]);
  if (contact->objects[1]) {
    glm_vec3_scale(move_per_inverse_mass, -contact->objects[1]->inverse_mass, contact->object_movements[1]);
  } else {
    glm_vec3_zero(contact->object_movements[1]);
  }

  vec3 game_one_pos;
  glm_vec3_add(contact->objects[0]->position, contact->object_movements[0], game_one_pos);
  set_game_object_position(contact->objects[0], game_one_pos);

  if (contact->objects[1]) {
    vec3 game_two_pos;
    glm_vec3_add(contact->objects[1]->position, contact->object_movements[1], game_two_pos);
    set_game_object_position(contact->objects[1], game_two_pos);
  }
}

void resolve_velocity(GameObjectContact *contact, float delta_time) {
  float separating_velocity = calculate_separating_vel(contact);

  if (separating_velocity > 0) return;

  float new_separating_vel = -separating_velocity * contact->restitution;

  vec3 acc_caused_vel;
  glm_vec3_copy(contact->objects[0]->acceleration, acc_caused_vel);
  if (contact->objects[1]) glm_vec3_sub(acc_caused_vel, contact->objects[1]->acceleration, acc_caused_vel);
  float acc_cause_sep_vel = glm_vec3_dot(acc_caused_vel, contact->contact_normal) * delta_time;

  if (acc_cause_sep_vel < 0.0f) {
    new_separating_vel += contact->restitution * acc_cause_sep_vel;
    if (new_separating_vel < 0.0f) new_separating_vel = 0.0f;
  }

  float delta_vel = new_separating_vel - separating_velocity;

  float total_inverse_mass = contact->objects[0]->inverse_mass;
  if (contact->objects[1]) total_inverse_mass += contact->objects[1]->inverse_mass;

  if (total_inverse_mass <= 0.0f) return;

  float impulse = delta_vel / total_inverse_mass;

  vec3 impulse_per_inverse_mass;
  glm_vec3_scale(contact->contact_normal, impulse, impulse_per_inverse_mass);

  vec3 obj_one_impulse;
  glm_vec3_scale(impulse_per_inverse_mass, contact->objects[0]->inverse_mass, obj_one_impulse);
  vec3 obj_one_velocity;
  glm_vec3_add(contact->objects[0]->velocity, obj_one_impulse, obj_one_velocity);
  set_game_object_velocity(contact->objects[0], obj_one_velocity);

  if (contact->objects[1]) {
    vec3 obj_two_impulse;
    glm_vec3_scale(impulse_per_inverse_mass, contact->objects[1]->inverse_mass, obj_two_impulse);
    vec3 obj_two_velocity;
    glm_vec3_add(contact->objects[1]->velocity, obj_two_impulse, obj_two_velocity);
    set_game_object_velocity(contact->objects[1], obj_two_velocity);
  }
}

void resolve(GameObjectContact *contact, float delta_time) {
  resolve_velocity(contact, delta_time);
  resolve_interpentration(contact, delta_time);
}

float calculate_separating_vel(GameObjectContact *contact) {
  vec3 relative_velocity;
  glm_vec3_copy(contact->objects[0]->velocity, relative_velocity);
  if (contact->objects[1]) glm_vec3_sub(relative_velocity, contact->objects[1]->velocity, relative_velocity);
  return glm_vec3_dot(relative_velocity, contact->contact_normal);
}

ContactResolver init_contact_resolver(size_t iterations) {
  ContactResolver resolver;
  resolver.iterations = iterations;
  resolver.iterations_used = 0;
  return resolver;
}

void set_resolver_iterations(ContactResolver *resolver, size_t iterations) {
  resolver->iterations = iterations;
}

void resolve_contacts(ContactResolver *resolver, GameObjectContact *contact_array, size_t num_contacts, float delta_time) {
  resolver->iterations_used = 0;

  while (resolver->iterations_used < resolver->iterations) {
    float max = FLT_MAX;
    size_t max_index =num_contacts;
    for (size_t i = 0; i < num_contacts; i++) {
      float sep_vel = calculate_separating_vel(&contact_array[i]);
      if (sep_vel < max && (sep_vel < 0 || contact_array[i].penetration > 0)) {
        max = sep_vel;
        max_index = i;
      }
    }

    if (max_index == num_contacts) break;
    resolve(&contact_array[max_index], delta_time);
    resolver->iterations_used++;
  }
}
