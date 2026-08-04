/* utilities.c: unit tests for libspectrum utility functions
   Copyright (c) 2026 Philip Kendall

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: philip-fuse@shadowmagic.org.uk

*/

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "internals.h"
#include "test.h"

/* NULL source is invalid */
test_return_t
utilities_zx_string_to_utf8_null_source_is_invalid( void )
{
  char result[ 46 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), NULL, 5 ) !=
      LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_null_source_is_invalid: "
             "expected LIBSPECTRUM_ERROR_INVALID\n", progname );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* Plain ASCII text is passed through unchanged */
test_return_t
utilities_zx_string_to_utf8_plain_ascii( void )
{
  static const libspectrum_byte src[] = { 'H', 'E', 'L', 'L', 'O' };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_plain_ascii: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "HELLO" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_plain_ascii: "
             "expected \"HELLO\", got \"%s\"\n", progname, result );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* Trailing spaces are stripped before conversion */
test_return_t
utilities_zx_string_to_utf8_trailing_spaces_stripped( void )
{
  static const libspectrum_byte src[] = { 'H', 'I', ' ', ' ', ' ' };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_trailing_spaces_stripped: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "HI" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_trailing_spaces_stripped: "
             "expected \"HI\", got \"%s\"\n", progname, result );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* ZX Spectrum special characters convert to Unicode equivalents:
   0x5C (\) -> \\, 0x5E (^) -> U+2191 (↑), 0x60 (`) -> U+00A3 (£),
   0x7F -> U+00A9 (©) */
test_return_t
utilities_zx_string_to_utf8_special_chars( void )
{
  static const libspectrum_byte src[] = {
    '\\', '^', '`', 0x7f
  };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_special_chars: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  /* expected: "\\" + "↑" + "£" + "©" */
  if( strcmp( result, "\\\\" "\xe2\x86\x91" "\xc2\xa3" "\xc2\xa9" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_special_chars: "
             "unexpected result\n", progname );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* UDG characters (bytes 144–164) render as \a through \u */
test_return_t
utilities_zx_string_to_utf8_udg_char( void )
{
  /* 0x90 = 144 -> UDG 'a' -> rendered as \a */
  static const libspectrum_byte src[] = { 0x90 };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_udg_char: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "\\a" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_udg_char: "
             "expected \"\\\\a\", got \"%s\"\n", progname, result );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* Spectrum BASIC keyword tokens (bytes >= 165) expand to keyword text.
   Byte 0xFB = 251 = 165 + 86 -> spectrum_tokens[86] = "RANDOMIZE" */
test_return_t
utilities_zx_string_to_utf8_spectrum_token( void )
{
  static const libspectrum_byte src[] = { 0xfb };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_spectrum_token: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "RANDOMIZE" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_spectrum_token: "
             "expected \"RANDOMIZE\", got \"%s\"\n", progname, result );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* A buffer without space for the terminating NUL is rejected */
test_return_t
utilities_zx_string_to_utf8_buffer_too_short_is_invalid( void )
{
  static const libspectrum_byte src[] = { 0xfb };
  char result[ 9 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) != LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_buffer_too_short_is_invalid: "
             "expected LIBSPECTRUM_ERROR_INVALID\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_buffer_too_short_is_invalid: "
             "expected empty result\n", progname );
    return TEST_FAIL;
  }

  return TEST_PASS;
}
