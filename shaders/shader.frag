#version 460 core

in vec2 TexCoord;

uniform sampler2D texture1;

out vec4 FragColor;

void main() {
  vec4 color = texture(texture1, TexCoord);
  if (color.a < 0.1)
    discard;
  FragColor = color;
}
