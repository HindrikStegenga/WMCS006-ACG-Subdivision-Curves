#version 410 core
layout (lines_adjacency) in;
layout (line_strip, max_vertices = 44) out;

// Input data per vertex. (Excluding position this is in gl_in)
in VertexData {
    vec4 color;
} vIn[4];

out VertexData {
    vec4 color;
} vOut;


const float PI = 3.1415926535897932384626433832795;

// Generates the circle geometry we need
void generateCircleGeometry(vec4 centre, int steps, float radius) {

    for (int i = 0; i <= steps; i++) {
        // Angle between each side in radians
        float ang = (PI * 2.0) / steps * i;
        // Compute offset from centre point
        vec4 offset = vec4(cos(ang) * radius, -sin(ang) * radius, 0.0, 0.0);
        gl_Position = centre + offset;
        EmitVertex();
    }
    EndPrimitive();
}


// Accepts two non-zero vectors, return curvature between them
float computeCurvature(vec4 v0, vec4 v1) {
    float angle = acos(dot(v0, v1));
    return 2 * sin(angle) / length(v1 - v0);
}


struct VertOpNormal {
    vec4 v0n;
    vec4 v1n;
    vec4 normal;
    float curvature;
};

// Translates the to 0,0 with v1 at 0,0. Effectively turning v0 and v1 into vectors around the origin.
// This allows us to use transformations to compute the appropariate vector we want.
// Namely, the vector addition of v0 and v2. (i.e. the vector halfway).
// We compute the normalized variant of this so we can use this to generate a curvature mesh.
// We need to guard for zero length vectors for normalizing as well.
// It also computes the curvature of the given vectors, where v1 is origin.
// Thus, it's the angle between v0 and v2 that's used for computing curvature.
VertOpNormal computeVertexOppositeNormal(vec4 v0, vec4 v1, vec4 v2) {
    vec4 zero_v0 = v1 - v0;
    vec4 zero_v2 = v1 - v2;

    // Check for zero magnitude, in such case we simply return 0.
    // This has the effect of making curvature and length of the normal vector 0 as well.
    if (!(length(zero_v0) > 0) || !(length(zero_v2) > 0)) {
        return VertOpNormal(vec4(0), vec4(0), vec4(0), 0.0);
    }

    // Normalize them now.
    zero_v0 = normalize(zero_v0);
    zero_v2 = normalize(zero_v2);

    // Compute vector addition.
    vec4 zero_v0v2 = zero_v0 + zero_v2;
    if (!(length(zero_v0v2) > 0)) {
        return VertOpNormal(vec4(0), vec4(0), vec4(0), 0.0);
    }
    // Normalize after check
    zero_v0v2 = normalize(zero_v0v2);

    // Compute curvature
    float curvature = computeCurvature(zero_v0, zero_v2);

    // Return struct containing values. Including normalized values for further computation.
    return VertOpNormal(zero_v0, zero_v2, zero_v0v2, curvature);
}




void main() {

    // Get vectors from input line segment. Including adjacency's.
    // v1 - v2 is the actual line segment. v0 is left, v3 is right.
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 v3 = gl_in[3].gl_Position;

    // Compute the four positions we want to generate a line for.

    VertOpNormal v1v3 = computeVertexOppositeNormal(v1,v2,v3);
    VertOpNormal v0v2 = computeVertexOppositeNormal(v0,v1,v2);

    // Generate the line segment.
    gl_Position = v1;
    vOut.color = vIn[1].color;
    EmitVertex();
    gl_Position = v2;
    vOut.color = vIn[2].color;
    EmitVertex();
    EndPrimitive();

    // Generate the circle geometry along the normals.
    // I just take a constant of 20 segments for the circle.
    generateCircleGeometry(v1 - (v0v2.normal * v0v2.curvature), 20, v0v2.curvature);
    generateCircleGeometry(v2 - (v1v3.normal * v1v3.curvature), 20, v1v3.curvature);
}
