/* Dummy fragment shader */


uniform sampler2D src_tex;

vec2 texCoord = gl_TexCoord[0];

void main() {
    gl_FragColor = texture2D( src_tex, texCoord.xy );
}
