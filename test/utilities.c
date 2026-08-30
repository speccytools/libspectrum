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

/* UDG characters (bytes 144–162) render as \a through \s */
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

/* Spectrum BASIC keyword tokens expand to keyword text.  128K-only
   tokens 0xA3 and 0xA4 are SPECTRUM and PLAY; 0xF7 is RUN and 0xF9
   is RANDOMIZE. */
test_return_t
utilities_zx_string_to_utf8_spectrum_token( void )
{
  static const libspectrum_byte src[] = {
    0xa3, ' ', 0xa4, ' ', 0xf7, ' ', 0xf9
  };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_spectrum_token: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "SPECTRUM PLAY RUN RANDOMIZE" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_spectrum_token: "
             "expected \"SPECTRUM PLAY RUN RANDOMIZE\", got \"%s\"\n", progname, result );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* A buffer without space for the terminating NUL is rejected */
test_return_t
utilities_zx_string_to_utf8_buffer_too_short_is_invalid( void )
{
  static const libspectrum_byte src[] = { 0xf9 };
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

/* libspectrum_version: returns a non-NULL, non-empty string */
test_return_t
utilities_version_returns_nonempty_string( void )
{
  const char *ver = libspectrum_version();
  if( !ver || ver[0] == '\0' ) {
    fprintf( stderr, "%s: utilities_version_returns_nonempty_string: "
             "expected non-empty version string\n", progname );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* libspectrum_check_version: current version satisfies itself */
test_return_t
utilities_check_version_current_version_returns_true( void )
{
  const char *ver = libspectrum_version();
  if( !libspectrum_check_version( ver ) ) {
    fprintf( stderr, "%s: utilities_check_version_current_version_returns_true: "
             "expected 1 for version \"%s\"\n", progname, ver );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* libspectrum_check_version: very old required version (0.0.0) is satisfied */
test_return_t
utilities_check_version_very_old_required_returns_true( void )
{
  if( !libspectrum_check_version( "0.0.0" ) ) {
    fprintf( stderr, "%s: utilities_check_version_very_old_required_returns_true: "
             "expected 1 for version \"0.0.0\"\n", progname );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* libspectrum_check_version: future major version (99.0.0) is not satisfied */
test_return_t
utilities_check_version_future_major_returns_false( void )
{
  if( libspectrum_check_version( "99.0.0" ) ) {
    fprintf( stderr, "%s: utilities_check_version_future_major_returns_false: "
             "expected 0 for version \"99.0.0\"\n", progname );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* Graphics block tokens (bytes 128-143) render as Unicode block elements. */
test_return_t
utilities_zx_string_to_utf8_graphics_token( void )
{
  static const libspectrum_byte src[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f
  };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_graphics_token: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result,
              " " "\xe2\x96\x9d" "\xe2\x96\x98" "\xe2\x96\x80"
              "\xe2\x96\x97" "\xe2\x96\x90" "\xe2\x96\x9a" "\xe2\x96\x9c"
              "\xe2\x96\x96" "\xe2\x96\x9e" "\xe2\x96\x8c" "\xe2\x96\x9b"
              "\xe2\x96\x84" "\xe2\x96\x9f" "\xe2\x96\x99" "\xe2\x96\x88" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_graphics_token: "
             "unexpected result\n", progname );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* Control characters (bytes 0-31, excluding handled specials) render as "?" */
test_return_t
utilities_zx_string_to_utf8_control_char( void )
{
  /* 0x01 is a control character, not specially handled */
  static const libspectrum_byte src[] = { 0x01 };
  char result[ sizeof( src ) * 9 + 1 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src,
                                     sizeof( src ) ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_control_char: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "?" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_control_char: "
             "expected \"?\", got \"%s\"\n", progname, result );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* Empty source string produces empty output */
test_return_t
utilities_zx_string_to_utf8_empty_source( void )
{
  static const libspectrum_byte src[] = { 0 };
  char result[ 16 ];

  if( libspectrum_zx_string_to_utf8( result, sizeof( result ), src, 0 ) ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_empty_source: "
             "conversion failed\n", progname );
    return TEST_FAIL;
  }

  if( strcmp( result, "" ) != 0 ) {
    fprintf( stderr, "%s: utilities_zx_string_to_utf8_empty_source: "
             "expected empty string, got \"%s\"\n", progname, result );
    return TEST_FAIL;
  }

  return TEST_PASS;
}
