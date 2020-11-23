#version 410 core
layout (lines_adjacency) in;
layout (line_strip, max_vertices = 12) out;

float computeCurvatureVertex(vec4 v0, vec4 v1, vec4 v2) {
    float angle = acos( dot( normalize( v1 - v0), normalize(v2 - v0) ) );
    return 2 * sin(angle) / length(normalize(v2) - normalize(v0));
}

// Translates the to 0,0 with v1 at 0,0. Effectively turning v0 and v1 into vectors around the origin.
// This allows us to use transformations to compute the appropariate vector we want.
// Namely, the negated vector addition of v0 and v2. (i.e. the vector halfway but opposite direciton).
// We compute the normalized variant of this so we can use this to generate a curvature mesh.
// We need to guard for zero length vectors for normalizing as well.
vec4 computeVertexOppositeNormal(vec4 v0, vec4 v1, vec4 v2) {
    vec4 zero_v0 = v1 - v0;
    vec4 zero_v2 = v1 - v2;

    if (!(length(zero_v0) > 0) || !(length(zero_v2) > 0)) {
        return vec4(0);
    }

    zero_v0 = normalize(zero_v0);
    zero_v2 = normalize(zero_v2);

    vec4 zero_v0v2 = zero_v0 + zero_v2;
    if (!(length(zero_v0v2) > 0)) {
        return vec4(0);
    }
    zero_v0v2 = normalize(zero_v0v2);
    float curvature = computeCurvatureVertex(zero_v0, vec4(0), zero_v2);
    return zero_v0v2 * curvature;
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

    gl_Position = v1;
    EmitVertex();
    gl_Position = v2;
    EmitVertex();

    gl_Position = v2 + (1.5 * computeVertexOppositeNormal(v1,v2,v3));
    EmitVertex();

    gl_Position = v1 + (1.5 * computeVertexOppositeNormal(v0,v1,v2));
    EmitVertex();
    gl_Position = v1;
    EmitVertex();
    EndPrimitive();

}
