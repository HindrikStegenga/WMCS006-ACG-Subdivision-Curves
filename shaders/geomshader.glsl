#version 410 core
layout (lines_adjacency) in;
layout (line_strip, max_vertices = 12) out;

float computeCurvatureVertex(vec2 v0, vec2 v1, vec2 v2) {
    float angle = acos( dot( normalize( v1 - v0), normalize(v2 - v0) ) );
    return 2 * sin(angle) / length(normalize(v2) - normalize(v0));
}


void main() {

    // Get vectors from input line segment. Including adjacency's.
    // v1 - v2 is the actual line segment. v0 is left, v3 is right.
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 v3 = gl_in[3].gl_Position;


    // We need to guard for zero divisions when normalizing.
    // Therefore, we check this, if this is the case, our vertex normal length must be zero.
    // Otherwise UB will be triggered, and on my system geometry is not drawn.
    // Under normal circumstances this occurs on endpoint rendering for example.

    bool v0_is_zero_div = true;
    bool v3_is_zero_div = true;


    // Transform to 0,0 in order to compute appropriate direction vector
    vec2 zero_v0 = v1.xy - v0.xy;
    if (zero_v0 != vec2(0)) { zero_v0 = normalize(zero_v0); v0_is_zero_div = false; }

    vec2 zero_v2 = normalize(v1.xy - v2.xy);
    vec2 zero_v0v2 = normalize(zero_v0 + zero_v2);

    vec2 zero_v1 = normalize(v2.xy - v1.xy);
    vec2 zero_v3 = v2.xy - v3.xy;
    if (zero_v3 != vec2(0)) { zero_v3 = normalize(zero_v3); v3_is_zero_div = false; }

    vec2 zero_v1v3 = normalize(zero_v1 + zero_v3);

    gl_Position = v1;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();

    if(v0_is_zero_div) {
        zero_v1v3 = vec2(0);
    }
    if(v3_is_zero_div) {
        zero_v0v2 = vec2(0);
    }

    gl_Position = v2 + vec4(0.1 * zero_v1v3, 0.0, 0.0);
    EmitVertex();

    gl_Position = v1 + vec4(0.1 * zero_v0v2, 0.0, 0.0);
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
    EndPrimitive();

}
