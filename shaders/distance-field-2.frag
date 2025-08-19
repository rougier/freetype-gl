/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
uniform sampler2D u_texture;

float contour(in float d, in float w) {
    return smoothstep(0.5 - w, 0.5 + w, d);
}

float samp(in vec2 uv, float w) {
    return contour(texture2D(u_texture, uv).r, w);
}

void main(void)
{
    vec2 UV = gl_TexCoord[0].st;
    float dist = texture2D(u_texture, UV).r;
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
    //

    gl_FragColor = vec4(gl_Color.rgb, alpha*gl_Color.a);
}
