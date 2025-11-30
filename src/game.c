#include "game.h"
#include "cglm/cam.h"
#include "quad.h"
#include "shader.h"
#include "common.h"
#include "vert-shader.h"
#include "frag-shader.h"
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

  bool forward = false, backward = false, left = false, right = false;

  if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
    int count;
    const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
    if (count >= 2) {
      if (axes[0] < -0.5f)
        left = true;
      if (axes[0] > 0.5f)
        right = true;
    }

    const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
    for (int i = 0; i < count; i++)
      if (buttons[i] == GLFW_PRESS)
        printf("%d\n", i);
    if (count >= 15) {
      if (buttons[1] == GLFW_PRESS)
        forward = true;
      if (buttons[3] == GLFW_PRESS)
        backward = true;
      if (buttons[14] == GLFW_PRESS)
        left = true;
      if (buttons[12] == GLFW_PRESS)
        right = true;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    forward = true;
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    backward = true;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    left = true;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    right = true;

  quad->movement.forward = forward;
  quad->movement.backward = backward;
  quad->movement.left = left;
  quad->movement.right = right;
}

void init(GLFWwindow **window, bool fullscreen) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  *window = glfwCreateWindow(width, height, "OpenGL in C", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
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
  glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void run(bool fullscreen) {
  GLFWwindow *window;
  init(&window, fullscreen);

  GLuint shader = create_shader((const char*)shaders_shader_vert, shaders_shader_vert_len, (const char*)shaders_shader_frag, shaders_shader_frag_len);
  use(shader);

  vec3 pos = {0.0f, 0.0f, 0.0f};
  vec3 rotation = {0.0f, 0.0f, 1.0f};
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
