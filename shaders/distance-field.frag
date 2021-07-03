/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
uniform sampler2D u_texture;

       vec3 glyph_color    = vec3(1.0,1.0,1.0);
const float glyph_center   = 0.50;
       vec3 outline_color  = vec3(0.0,0.0,0.0);
const float outline_center = 0.55;
       vec3 glow_color     = vec3(1.0,1.0,1.0);
const float glow_center    = 1.25;

float contour(in float d, in float w) {
    return smoothstep(0.5 - w, 0.5 + w, d);
}

float samp(in vec2 uv, float w) {
    return contour(texture2D(u_texture, uv).r, w);
}

void main(void)
{
    vec2 UV = gl_TexCoord[0].st;
    vec4  color = texture2D(u_texture, UV);
    float dist  = color.r;
    float width = fwidth(dist);
    float alpha = contour( dist, width );

    // Supersampling: comment the following block when using large
    // magnifications.
    float dscale = 0.354; // half of 1/sqrt2, can be played with
    vec2 duv = dscale * (dFdx(UV) + dFdy(UV));
    vec4 box = vec4(UV-duv, UV+duv);

    float asum = samp( box.xy, width )
               + samp( box.zw, width )
               + samp( box.xw, width )
               + samp( box.zy, width );
    alpha = (alpha + 0.5 * asum) / 3.0;

    // Smooth
    // gl_FragColor = vec4(glyph_color, alpha);

    // Outline
    // float mu = smoothstep(outline_center-width, outline_center+width, dist);
    // vec3 rgb = mix(outline_color, glyph_color, mu);
    // gl_FragColor = vec4(rgb, max(alpha,mu));

    // Glow
    //vec3 rgb = mix(glow_color, glyph_color, alpha);
    //float mu = smoothstep(glyph_center, glow_center, sqrt(dist));
    //gl_FragColor = vec4(rgb, max(alpha,mu));

    // Glow + outline
    vec3 rgb = mix(glow_color, glyph_color, alpha);
    float mu = smoothstep(glyph_center, glow_center, sqrt(dist));
    color = vec4(rgb, max(alpha,mu));
    float beta = smoothstep(outline_center-width, outline_center+width, dist);
    rgb = mix(outline_color, color.rgb, beta);
    gl_FragColor = vec4(rgb, max(color.a,beta));

}


