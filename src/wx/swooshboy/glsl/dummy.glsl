/* Dummy shader */

precision highp float;

//varying vec2 v_texcoord;

#ifdef VERTEX

attribute vec4 v_pos;
attribute vec2 texcoord;
uniform mat4 modVwMtx;
//uniform mat4 projMtx;

void main () {
    gl_Position = modVwMtx * v_pos;
    //v_texcoord=texcoord;
}

#endif

#ifdef FRAGMENT

uniform sampler2D src_tex;
uniform mediump vec2 src_sz;
uniform mediump vec2 dst_sz;
uniform mediump int pass;


void main() {
    vec2 v_texcoord = gl_FragCoord.xy / dst_sz;
    v_texcoord.y = (1.0 - v_texcoord.y);
    vec4 color = texture2D( src_tex, v_texcoord );
    gl_FragColor = color;
}

#endif
