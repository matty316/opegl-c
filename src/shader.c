#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *get_source(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    printf("failed to open shader file: %s", path);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(fileSize + 1);
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

void check_compile_errors(GLuint shader, const char *type) {
  int success;
  char infoLog[1024];

  if (strcmp(type, "program") == 0) {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      printf("Failed to link program: %s", infoLog);
      exit(EXIT_FAILURE);
    }
  } else {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      printf("Failed to compile shader: %s, %s", type, infoLog);
      exit(EXIT_FAILURE);
    }
  }
}

GLuint compile_shader(const char *path, GLuint type) {
  const char *shader_source = get_source(path);

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_source, NULL);
  glCompileShader(shader);
  check_compile_errors(shader, type == GL_VERTEX_SHADER ? "vertex" :"fragment");

  free((void*)shader_source);

  return shader;
}

GLuint create_shader(const char *vertex_path, const char *fragment_path) {
  GLuint vertex = compile_shader(vertex_path, GL_VERTEX_SHADER);
  GLuint fragment = compile_shader(fragment_path, GL_FRAGMENT_SHADER);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  check_compile_errors(program, "program");
  glDeleteShader(vertex);
  glDeleteProgram(fragment);
  return program;
}

void use(GLuint shader) {
  glUseProgram(shader);
}

void set_mat4(GLuint shader, const char *name, vec4 *mat) {
  glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, mat[0]);
}
