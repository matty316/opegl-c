#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

GLuint compile_shader(const char *source, unsigned int len, GLuint type) {
  char *shader_source = malloc(len + 1);

  strncpy(shader_source, source, len);
  shader_source[len] = '\0';
  const char *ssource = (const char*)shader_source;

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &ssource, NULL);
  glCompileShader(shader);
  check_compile_errors(shader, type == GL_VERTEX_SHADER ? "vertex" :"fragment");

  free(shader_source);

  return shader;
}

GLuint create_shader(const char *vertex_source, unsigned int vertex_len, const char *fragment_source, unsigned int fragment_len) {
  GLuint vertex = compile_shader(vertex_source, vertex_len, GL_VERTEX_SHADER);
  GLuint fragment = compile_shader(fragment_source, fragment_len, GL_FRAGMENT_SHADER);

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
