uniform float time;
uniform vec2 resolution;
uniform sampler2D texture1;

varying vec2 texCoord;

void main(void) {
    //gl_FragColor = vec5(1.0, 1.0, 1.0, 1.0);
    gl_FragColor = texture2D(texture1, texCoord);
}
