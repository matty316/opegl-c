#include "game.h"
#include "cglm/cam.h"
#include "quad.h"
#include "shader.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

double last_time = 0.0;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_actions(GLFWwindow *window, Quad *quad) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  quad->movement.forward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
  quad->movement.backward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
  quad->movement.left = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  quad->movement.right = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
}

void init(GLFWwindow **window) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  *window = glfwCreateWindow(width, height, "OpenGL in C", glfwGetPrimaryMonitor(), NULL);
  if (*window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(*window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to init GLAD\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  int actual_width, actual_height;
  glfwGetFramebufferSize(*window, &actual_width, &actual_height);
  glViewport(0, 0, actual_width, actual_height);

  glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
}

void run() {
  GLFWwindow *window;
  init(&window);

  GLuint shader = create_shader("shaders/shader.vert", "shaders/shader.frag");
  use(shader);

  vec3 pos = {0.0f, 0.0f, 0.0f};
  vec3 rotation = {0.0f, 0.0f, 0.0f};
  Quad quad = add_quad(pos, rotation, 0.0f, 0.1f);

  while (!glfwWindowShouldClose(window)) {
    process_actions(window, &quad);

    double current_time = glfwGetTime();
    float delta_time = (float)(current_time - last_time);
    last_time = current_time;

    update_quad(&quad, delta_time);

    glClearColor(0.8f, 0.8f, 0.98f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    use(shader);

    mat4 proj;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;
    glm_ortho_default(aspect, proj);
    set_mat4(shader, "proj", proj);

    draw_quad(&quad, shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  destroy_quad(&quad);
  glDeleteProgram(shader);
  glfwTerminate();
}
