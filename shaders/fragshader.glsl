#version 410
// Fragment shader

in VertexData {
    vec4 color;
} vIn;

out vec4 out_color;

void main() {
  out_color = vIn.color;
}
