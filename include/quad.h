#ifndef quad_h
#define quad_h

#include "cglm/types.h"
#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct Quad {
  mat4 model;
  vec3 position, rotation;
  float angle, scale;
  GLuint vao, vbo, ebo, texture_id;
} Quad;

Quad init_quad(vec3 pos, vec3 rotation, float angle, float scale, GLuint texture_id);
void draw_quad(Quad *quad, GLuint shader);
void update_quad(Quad *quad);
void destroy_quad(Quad *quad);

#endif
