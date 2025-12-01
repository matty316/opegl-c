#include "quad.h"
#include "cglm/util.h"
#include "game.h"
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

void update_quad(Quad *quad, float delta_time) {
  float accel = 0.5f;
  float reverse_accel = 0.01f;

  float rotation = 0.0f;
  float angle = 90.0f;
  float epsilon = 0.05f;

  if (quad->movement.forward)
    quad->velocity += accel;
  if (quad->movement.backward)
    quad->velocity -= reverse_accel;
  if (quad->movement.left && (quad->velocity > epsilon || quad->velocity < -epsilon)) {
    rotation += glm_rad(angle);
  }
  if (quad->movement.right && (quad->velocity > epsilon || quad->velocity < -epsilon)) {
    rotation -= glm_rad(angle);
  }

  quad->angle += rotation * delta_time;

  float max_velocity = 20.0f;
  if (quad->velocity > max_velocity)
    quad->velocity = max_velocity;

  if (quad->velocity < -max_velocity)
    quad->velocity = -max_velocity;

  quad->velocity *= 0.98f;

  float adjusted_angle = glm_rad(90.0f);
  quad->pos[0] += cos(quad->angle + adjusted_angle) * quad->velocity * delta_time;
  quad->pos[1] += sin(quad->angle + adjusted_angle) * quad->velocity * delta_time;

  glm_mat4_identity(quad->model);
  glm_translate(quad->model, quad->pos);

  glm_rotate(quad->model, quad->angle, quad->rotation);

  vec3 scale_vec = {quad->scale, quad->scale, quad->scale};
  glm_scale(quad->model, scale_vec);
}

Quad add_quad(vec3 pos, vec3 rotation, float angle, float scale, GLuint texture_id) {
  Quad quad;
  memcpy(quad.pos, pos, sizeof(float) * 3);
  memcpy(quad.rotation, rotation, sizeof(float) * 3);
  quad.angle = angle;
  quad.scale = scale;
  quad.velocity = 0.0f;
  quad.texture_id = texture_id;

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
