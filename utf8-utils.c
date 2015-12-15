/* =========================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         https://github.com/rougier/freetype-gl
 * -------------------------------------------------------------------------
 * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
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

#include <string.h>
#include <wchar.h>
#include "utf8-utils.h"

// ----------------------------------------------------- utf8_surrogate_len ---
size_t
utf8_surrogate_len( const char* character )
{
    size_t result = 0;
    char test_char;

    if (!character)
        return 0;

    test_char = character[0];

    if ((test_char & 0x80) == 0)
        return 1;

    while (test_char & 0x80)
    {
        test_char <<= 1;
        result++;
    }

    return result;
}

// ------------------------------------------------------------ utf8_strlen ---
size_t
utf8_strlen( const char* string )
{
    const char* ptr = string;
    size_t result = 0;

    while (*ptr)
    {
        ptr += utf8_surrogate_len(ptr);
        result++;
    }

    return result;
}

// ------------------------------------------------------ str_utf16_to_utf8 ---
char *
str_utf16_to_utf8( const wchar_t * string )
{
    size_t i;

    char * result = NULL;
    char * character = NULL;
    size_t strlength = 0;

    for( i = 0; i < wcslen( string ); ++i )
    {
        character = utf16_to_utf8( string[i] );
        strlength = result ? strlen( result ) : 0;
        result = realloc( result, strlength + utf8_surrogate_len( character ) + 1 );
        result[strlength] = '\0';
        strncat( result, character, utf8_surrogate_len( character ) );
        free( character );
    }

    return result;
}

// ---------------------------------------------------------- utf8_to_utf16 ---
wchar_t
utf8_to_utf16( const char * character )
{
    return utf8_to_utf32( character );
}

// ---------------------------------------------------------- utf16_to_utf8 ---
char *
utf16_to_utf8( wchar_t character )
{
    wchar_t counter = character;
    char * result = NULL;

    if ( character == -1 )
    {
        return '\0';
    }
    else if ( character < 0x80 )
    {
        result = malloc( sizeof(char) * 2 );
        result[0] = character & 0xFF;
        result[1] = '\0';
    }
    else if ( character < 0x800 )
    {
        result = malloc( sizeof(char) * 3 );
        result[0] = 0xC0 | ((character & 0x7C0) >> 6);
        result[1] = 0x80 | (character & 0x3F);
        result[2] = '\0';
    }
    else if ( character < 0x100000 )
    {
        result = malloc( sizeof(char) * 4 );
        result[0] = 0xE0 | ((character & 0xF000) >> 12);
        result[1] = 0x80 | ((character & 0xFC0) >> 6);
        result[2] = 0x80 | (character & 0x3F);
        result[3] = '\0';
    }
    else if ( character < 0x200000 )
    {
        result = malloc( sizeof(char) * 5 );
        result[0] = 0xF0 | ((character & 0x1C0000) >> 18);
        result[1] = 0x80 | ((character & 0x3F000) >> 12);
        result[2] = 0x80 | ((character & 0xFC0) >> 6);
        result[3] = 0x80 | (character & 0x3F);
        result[4] = '\0';
    }
    else if ( character < 0x4000000 )
    {
        result = malloc( sizeof(char) * 6 );
        result[0] = 0xF8 | ((character & 0x3000000) >> 24);
        result[1] = 0x80 | ((character & 0xFC0000) >> 18);
        result[2] = 0x80 | ((character & 0x3F000) >> 12);
        result[3] = 0x80 | ((character & 0xFC0) >> 6);
        result[4] = 0x80 | (character & 0x3F);
        result[5] = '\0';
    }
    else
    {
        result = malloc( sizeof(char) * 7 );
        result[0] = 0xFC | ((character & 0x40000000) >> 30);
        result[1] = 0x80 | ((character & 0x3F000000) >> 24);
        result[2] = 0x80 | ((character & 0xFC0000) >> 18);
        result[3] = 0x80 | ((character & 0x3F000) >> 12);
        result[4] = 0x80 | ((character & 0xFC0) >> 6);
        result[5] = 0x80 | (character & 0x3F);
        result[6] = '\0';
    }

    return result;
}

// ---------------------------------------------------------- utf8_to_utf32 ---
uint32_t
utf8_to_utf32( const char * character )
{
    uint32_t result = -1;

    if( !character )
    {
        return result;
    }

    if( ( character[0] & 0x80 ) == 0x0 )
    {
        result = character[0];
    }

    if( ( character[0] & 0xC0 ) == 0xC0 )
    {
        result = ( ( character[0] & 0x3F ) << 6 ) | ( character[1] & 0x3F );
    }

    if( ( character[0] & 0xE0 ) == 0xE0 )
    {
        result = ( ( character[0] & 0x1F ) << ( 6 + 6 ) ) | ( ( character[1] & 0x3F ) << 6 ) | ( character[2] & 0x3F );
    }

    if( ( character[0] & 0xF0 ) == 0xF0 )
    {
        result = ( ( character[0] & 0x0F ) << ( 6 + 6 + 6 ) ) | ( ( character[1] & 0x3F ) << ( 6 + 6 ) ) | ( ( character[2] & 0x3F ) << 6 ) | ( character[3] & 0x3F );
    }

    if( ( character[0] & 0xF8 ) == 0xF8 )
    {
        result = ( ( character[0] & 0x07 ) << ( 6 + 6 + 6 + 6 ) ) | ( ( character[1] & 0x3F ) << ( 6 + 6 + 6 ) ) | ( ( character[2] & 0x3F ) << ( 6 + 6 ) ) | ( ( character[3] & 0x3F ) << 6 ) | ( character[4] & 0x3F );
    }

    return result;
}
