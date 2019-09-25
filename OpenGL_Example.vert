#version 330
uniform mat4 M;
in  vec3 in_position;
in  vec3 in_color;

// We output the ex_color variable to the next shader in the chain
out vec3 ex_color;
void main(void) {

    gl_Position = M * vec4(in_position.x, in_position.y, in_position.z, 1.0);

    // Since we aren't using projection matrices yet, 
    // the following adjusts for depth buffer "direction".
    // Remove this once projection is added.
    gl_Position.z = -gl_Position.z;

    // We're passing the color to the rasterizer for interpolation
    ex_color = in_color;
}