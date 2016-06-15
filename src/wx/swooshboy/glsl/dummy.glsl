/* Passthrough shader
 *
 * Vertex and fragment implementation
 */

precision highp float;
precision highp int;
precision highp sampler2D;

varying vec2 v_texcoord;

#ifdef VERTEX

attribute vec4 v_pos;
attribute vec2 texcoord;

uniform vec2 v_src_sz;
uniform vec2 v_dst_sz;

uniform mediump int v_pass_idx;
uniform mediump int f_pass_qty;

uniform bool needs_flip;

void main () {
    vec4 pos = v_pos;
    vec2 tc = texcoord;

    if (needs_flip) {
        pos.xy = -pos.xy;
        tc.x = 1.0 - tc.x;
    }

    gl_Position = pos;
    v_texcoord = tc;
}

#endif

#ifdef FRAGMENT

uniform sampler2D src_tex;

uniform vec2 src_sz;
uniform vec2 dst_sz;

uniform mediump int f_pass_idx;
uniform mediump int f_pass_qty;

void main() {
    vec4 color;

    //vec2 f_texcoord = gl_FragCoord.xy / dst_sz;
    vec2 f_texcoord = v_texcoord;

    color = texture2D( src_tex, f_texcoord );

    gl_FragColor = color;
}

#endif
