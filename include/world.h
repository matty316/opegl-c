#ifndef world_h
#define world_h

#include "contact-resolver.h"
#include "game-object.h"

typedef struct World {
  GameObject *objects;
  size_t objects_size;
  size_t objects_capacity;

  ContactResolver resolver;
  ContactGeneratorType generators[16];

} World;

World init_world(size_t max_contacts, size_t iterations);
void start_frame(World *world);

#endif
