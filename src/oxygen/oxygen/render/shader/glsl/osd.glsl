#version 100
/* Passthrough shader
 *
 * Vertex and fragment implementation
 */

#ifndef IN_VBA
#define DEBUG
#include "common.glsl"
#endif

varying vec2 f_texcoord;

#ifdef VERTEX

attribute vec4 v_pos;
attribute vec2 v_texcoord;

uniform vec2 v_src_sz;
uniform vec2 v_dst_sz;

uniform mediump int v_pass_idx;
uniform mediump int v_pass_qty;

uniform bool needs_flip;

void main () {
    vec4 pos = v_pos;
    vec2 tc = v_texcoord;

    if (needs_flip) {
        pos.xy = -pos.xy;
        tc.x = 1.0 - tc.x;
    }

    gl_Position = pos;
    f_texcoord = tc;
}

#endif

#ifdef FRAGMENT

uniform sampler2D src_tex;

uniform vec2 f_src_sz;
uniform vec2 f_dst_sz;

uniform mediump int f_pass_idx;
uniform mediump int f_pass_qty;

uniform float fade;

void main() {
    vec2 dst_pos = floor(gl_FragCoord.xy);
    vec4 color;

    //vec2 f_texcoord = gl_FragCoord.xy / dst_sz;
    vec2 texcoord = f_texcoord;

    color = texture2D( src_tex, texcoord );
    color.a *= clamp(fade/255.0, 0.0, 1.0);

    gl_FragColor = color;
}

#endif
