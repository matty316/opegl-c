#include "quad.h"
#include "cglm/types.h"
#include "cglm/util.h"
#include "cglm/vec3.h"
#include "shader.h"
#include <math.h>
#include <string.h>

void setup_quad_vao(GLuint *vao, GLuint *vbo, GLuint *ebo) {
  float vertices[] = {
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // top left
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void update_quad(Quad *quad) {
  glm_mat4_identity(quad->model);
  glm_translate(quad->model, quad->position);

  glm_rotate(quad->model, quad->angle, quad->rotation);

  vec3 scale_vec = {quad->scale, quad->scale, quad->scale};
  glm_scale(quad->model, scale_vec);
}

Quad init_quad(vec3 pos, vec3 rotation, float angle, float scale, GLuint texture_id) {
  Quad quad;
  glm_vec3_copy(pos, quad.position);
  glm_vec3_copy(rotation, quad.rotation);
  quad.angle = angle;
  quad.scale = scale;
  quad.texture_id = texture_id;

  glm_mat4_identity(quad.model);
  glm_translate(quad.model, quad.position);

  glm_rotate(quad.model, quad.angle, quad.rotation);

  vec3 scale_vec = {quad.scale, quad.scale, quad.scale};
  glm_scale(quad.model, scale_vec);

  setup_quad_vao(&quad.vao, &quad.vbo, &quad.ebo);

  return quad;
}

void draw_quad(Quad *quad, GLuint shader) {
  glBindTexture(GL_TEXTURE_2D, quad->texture_id);
  glBindVertexArray(quad->vao);
  set_mat4(shader, "model", quad->model);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void destroy_quad(Quad *quad) {
  glDeleteVertexArrays(1, &quad->vao);
  glDeleteBuffers(1, &quad->vbo);
  glDeleteBuffers(1, &quad->ebo);
}
