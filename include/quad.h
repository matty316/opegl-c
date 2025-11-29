#ifndef quad_h
#define quad_h

#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct Quad {
  mat4 model;
  vec3 pos, rotation;
  float angle, scale;
  GLuint vao, vbo, ebo;
  vec2 velocity;
  struct {
    bool forward;
    bool backward;
    bool left;
    bool right;
  } movement;
} Quad;

Quad add_quad(vec3 pos, vec3 rotation, float angle, float scale);
void draw_quad(Quad *quad, GLuint shader);
void update_quad(Quad *quad, float delta_time);
void destroy_quad(Quad *quad);

#endif
