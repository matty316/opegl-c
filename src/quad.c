#include "quad.h"
#include "cglm/types.h"
#include "cglm/vec2-ext.h"
#include "cglm/vec2.h"
#include "shader.h"
#include "cglm/affine-pre.h"
#include "cglm/affine.h"
#include <X11/Xlib.h>
#include <math.h>
#include <string.h>

void setup_quad_vao(GLuint *vao, GLuint *vbo, GLuint *ebo) {
  float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
  };

  unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
  };

  glGenVertexArrays(1, vao);
  glGenBuffers(1, vbo);
  glGenBuffers(1, ebo);

  glBindVertexArray(*vao);

  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void update_quad(Quad *quad, float delta_time) {
  bool diagnol = (quad->movement.forward || quad->movement.backward) && (quad->movement.left && quad->movement.right);
  float accel = 1.0f;

  if (diagnol)
    accel = accel * (1.0 / GLM_SQRT1_2f);

  float damping = 0.1f;
  vec2 velocity;
  glm_vec2_zero(velocity);

  if (quad->movement.forward) {
    velocity[1] += accel * delta_time;
  }
  if (quad->movement.backward) {
    velocity[1] -= accel * delta_time;
  }
  if (quad->movement.left) {
    velocity[0] -= accel * delta_time;
  }
  if (quad->movement.right) {
    velocity[0] += accel * delta_time;
  }

  if (glm_vec2_eqv_eps(velocity, GLM_VEC2_ZERO)) {
    float min = fminf(quad->velocity[0], quad->velocity[1]);
    quad->velocity[0] = min;
    quad->velocity[1] = min;
    vec2 damped;
    glm_vec2_zero(damped);
    glm_vec2_scale(quad->velocity, fminf(1.0f / damping * delta_time, 1.0f), damped);
    glm_vec2_sub(quad->velocity, damped, quad->velocity);
  } else {
    glm_vec2_copy(velocity, quad->velocity);
  }

  quad->pos[1] += quad->velocity[1];
  quad->pos[0] += quad->velocity[0];
  glm_mat4_identity(quad->model);
  glm_translate(quad->model, quad->pos);

  glm_rotate(quad->model, quad->angle, quad->rotation);

  vec3 scale_vec = {quad->scale, quad->scale, quad->scale};
  glm_scale(quad->model, scale_vec);
}

Quad add_quad(vec3 pos, vec3 rotation, float angle, float scale) {
  Quad quad;
  memcpy(quad.pos, pos, sizeof(float) * 3);
  memcpy(quad.rotation, rotation, sizeof(float) * 3);
  quad.angle = angle;
  quad.scale = scale;
  glm_vec2_zero(quad.velocity);

  glm_mat4_identity(quad.model);
  glm_translate(quad.model, quad.pos);

  glm_rotate(quad.model, quad.angle, quad.rotation);

  vec3 scale_vec = {quad.scale, quad.scale, quad.scale};
  glm_scale(quad.model, scale_vec);

  quad.movement.forward = false;
  quad.movement.backward = false;
  quad.movement.left = false;
  quad.movement.right = false;

  setup_quad_vao(&quad.vao, &quad.vbo, &quad.ebo);

  return quad;
}

void draw_quad(Quad *quad, GLuint shader) {
  glBindVertexArray(quad->vao);
  set_mat4(shader, "model", quad->model);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void destroy_quad(Quad *quad) {
  glDeleteVertexArrays(1, &quad->vao);
  glDeleteBuffers(1, &quad->vbo);
  glDeleteBuffers(1, &quad->ebo);
}
