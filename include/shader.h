#ifndef shader_h
#define shader_h

#include <glad/glad.h>
#include <cglm/cglm.h>

GLuint create_shader(const char *vertex_source, unsigned int vertex_len, const char *fragment_source, unsigned int fragment_len);
void use(GLuint shader);
void set_mat4(GLuint shader, const char *name, vec4 *mat);
#endif
