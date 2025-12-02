#ifndef game_h
#define game_h

#include <glad/glad.h>
#include <stdbool.h>

void init(bool fullscreen, int screen_width, int screen_height);
void run(GLuint player_texture_id);

#endif
