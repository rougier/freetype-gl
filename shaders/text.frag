/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * -------------------------------------------------------------------------
 * Copyright 2011 Nicolas P. Rougier. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 * ========================================================================= */


vec3
energy_distribution( vec4 previous, vec4 current, vec4 next )
{
    float primary   = 1.0/3.0;
    float secondary = 1.0/3.0;
    float tertiary  = 0.0;

    // Energy distribution as explained on:
    // http://www.grc.com/freeandclear.htm
    //
    //  .. v..
    // RGB RGB RGB
    // previous.g + previous.b + current.r + current.g + current.b
    //
    //   . .v. .
    // RGB RGB RGB
    // previous.b + current.r + current.g + current.b + next.r
    //
    //     ..v ..
    // RGB RGB RGB
    // current.r + current.g + current.b + next.r + next.g

    float r =
        tertiary  * previous.g +
        secondary * previous.b +
        primary   * current.r  +
        secondary * current.g  +
        tertiary  * current.b;

    float g =
        tertiary  * previous.b +
        secondary * current.r +
        primary   * current.g  +
        secondary * current.b  +
        tertiary  * next.r;

    float b =
        tertiary  * current.r +
        secondary * current.g +
        primary   * current.b +
        secondary * next.r    +
        tertiary  * next.g;

    return vec3(r,g,b);
}


uniform sampler2D texture;
uniform vec3 pixel;
varying float vgamma;
varying float vshift;
void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    float shift = vshift;

    // LCD Off
    if( pixel.z == 1.0)
    {
        float a = texture2D(texture, uv).r;
        gl_FragColor = gl_Color * pow( a, 1.0/vgamma );
        return;
    }

    // LCD On
    vec4 current = texture2D(texture, uv);
    vec4 previous= texture2D(texture, uv+vec2(-1.,0.)*pixel.xy);
    vec4 next    = texture2D(texture, uv+vec2(+1.,0.)*pixel.xy);

    current = pow(current, vec4(1.0/vgamma));
    previous= pow(previous, vec4(1.0/vgamma));

    float r = current.r;
    float g = current.g;
    float b = current.b;

    if( shift <= 0.333 )
    {
        float z = shift/0.333;
        r = mix(current.r, previous.b, z);
        g = mix(current.g, current.r,  z);
        b = mix(current.b, current.g,  z);
    } 
    else if( shift <= 0.666 )
    {
        float z = (shift-0.33)/0.333;
        r = mix(previous.b, previous.g, z);
        g = mix(current.r,  previous.b, z);
        b = mix(current.g,  current.r,  z);
    }
   else if( shift < 1.0 )
    {
        float z = (shift-0.66)/0.334;
        r = mix(previous.g, previous.r, z);
        g = mix(previous.b, previous.g, z);
        b = mix(current.r,  previous.b, z);
    }

   float t = max(max(r,g),b);
   vec4 color = vec4(gl_Color.rgb, (r+g+b)/3.0);
   color = t*color + (1.0-t)*vec4(r,g,b, min(min(r,g),b));
   gl_FragColor = vec4( color.rgb, gl_Color.a*color.a);


//    gl_FragColor = vec4(pow(vec3(r,g,b),vec3(1.0/vgamma)),a);

    /*
    vec3 color = energy_distribution(previous, vec4(r,g,b,1), next);
    color = pow( color, vec3(1.0/vgamma));

    vec3 color = vec3(r,g,b); //pow( vec3(r,g,b), vec3(1.0/vgamma));
    gl_FragColor.rgb = color; //*gl_Color.rgb;
    gl_FragColor.a = (color.r+color.g+color.b)/3.0 * gl_Color.a;
    */

//    gl_FragColor = vec4(pow(vec3(r,g,b),vec3(1.0/vgamma)),a);
    //gl_FragColor = vec4(r,g,b,a);
}
