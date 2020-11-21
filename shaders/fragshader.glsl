#version 410
// Fragment shader

out vec4 fColor;
uniform vec3 inputColor;

void main() {

  fColor = vec4(inputColor, 1.0);

}
