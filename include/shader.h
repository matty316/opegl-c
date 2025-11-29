#ifndef shader_h
#define shader_h

#include <glad/glad.h>
#include <cglm/cglm.h>

GLuint create_shader(const char *vertex_path, const char *fragment_path);
void use(GLuint shader);
void set_mat4(GLuint shader, const char *name, vec4 *mat);
#endif
