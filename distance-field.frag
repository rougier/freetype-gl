uniform sampler2D texture;
uniform float AlphaTest;
uniform vec3 GlyphColor;
const float SmoothCenter = 0.5;
void main(void)
{
    // Look up distance from the distance field:
    vec4 color = texture2D(texture,gl_TexCoord[0].st);
    float alpha = color.a;

    // Kill the fragment if it fails the alpha test.
    if (alpha > AlphaTest)
        discard;
    vec3 rgb = color.xyz + GlyphColor;
    // float width = 0.05;
    float width = fwidth(alpha);
    alpha = smoothstep(SmoothCenter - width, SmoothCenter + width, alpha);
    gl_FragColor = vec4(rgb, alpha);
}


