#include "game.h"
#include "cglm/cam.h"
#include "game-object.h"
#include "quad.h"
#include "shader.h"
#include "vert-shader.h"
#include "frag-shader.h"
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

double last_time = 0.0;
GLFWwindow *window;

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_actions(GLFWwindow *window, GameObject *player) {
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
    if (count >= 15) {
      if (buttons[1] == GLFW_PRESS)
        forward = true;
      if (buttons[0] == GLFW_PRESS)
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

  player->movement.forward = forward;
  player->movement.backward = backward;
  player->movement.left = left;
  player->movement.right = right;
}

void ope_init(bool fullscreen, int screen_width, int screen_height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(screen_width, screen_height, "OpenGL in C", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to init GLAD\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  int actual_width, actual_height;
  glfwGetFramebufferSize(window, &actual_width, &actual_height);
  glViewport(0, 0, actual_width, actual_height);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);

  stbi_set_flip_vertically_on_load(true);
}

void ope_run(GLuint player_texture_id) {
  GLuint shader = create_shader((const char*)shaders_shader_vert, shaders_shader_vert_len, (const char*)shaders_shader_frag, shaders_shader_frag_len);
  use(shader);
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  vec3 pos = {(float)width / 2.0f, (float)height / 2.0f, 0.0f};

  GameObject player = init_game_object(pos, player_texture_id);

  while (!glfwWindowShouldClose(window)) {
    process_actions(window, &player);

    double current_time = glfwGetTime();
    float delta_time = (float)(current_time - last_time);
    last_time = current_time;

    update_game_object(&player, delta_time);

    glClearColor(0.8f, 0.8f, 0.98f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    use(shader);

    mat4 proj;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glm_ortho(0, width, 0, height, -1.0f, 1.0f, proj);
    set_mat4(shader, "proj", proj);

    draw_game_object(&player, shader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  destroy_game_object(&player);
  glDeleteProgram(shader);
  glfwTerminate();
}
