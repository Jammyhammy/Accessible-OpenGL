#version 330
in vec3 ex_color;
out vec4 gl_FragColor;

void main(void) {
    // Pass through the interpolated color with full opacity.
    gl_FragColor = vec4(ex_color,1.0);
}