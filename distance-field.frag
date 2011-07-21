uniform sampler2D texture;
const float outline_center = 0.10;
const float smooth_center  = 0.50;

void main(void)
{
    vec4 color  = texture2D( texture, gl_TexCoord[0].st );
    float dist  = color.a;
    vec3 rgb    = color.xyz;
    float width = fwidth(dist);
    float alpha = smoothstep(smooth_center - width,
                             smooth_center + width,
                             dist);
    gl_FragColor = vec4(rgb, alpha);
}


