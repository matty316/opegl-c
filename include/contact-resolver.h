#ifndef contact_resolver_h
#define contact_resolver_h

#include "cglm/types.h"
#include "game-object.h"
#include <stddef.h>

typedef struct GameObjectContact {
  GameObject *objects[2];
  float restitution;
  vec3 contact_normal;
  float penetration;
  vec3 object_movements[2];
} GameObjectContact;

void resolve(GameObjectContact *contact, float delta_time);
float calculate_separating_vel(GameObjectContact *contact);

typedef struct ContactResolver {
  size_t iterations;
  size_t iterations_used;
} ContactResolver;

ContactResolver init_contact_resolver(size_t iterations);
void set_resolver_iterations(ContactResolver *resolver, size_t iterations);
void resolve_contacts(ContactResolver *resolver, GameObjectContact *contact_array, size_t num_contacts, float delta_time);

typedef enum ContactGeneratorType {
  CG_PLACEHOLDER
} ContactGeneratorType;

#endif
