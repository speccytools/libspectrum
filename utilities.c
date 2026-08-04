/* utilities.c: miscellaneous utility routines
   Copyright (c) 2011,2021 Philip Kendall

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

   E-mail: Philip Kendall <philip-fuse@shadowmagic.org.uk>

*/

#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif				/* #ifdef HAVE_STRING_H */

#include "internals.h"

#define TZX_HZ 3500000

static const libspectrum_dword tstates_per_ms = TZX_HZ / 1000;

static const char *spectrum_tokens[] = {
  "SPECTRUM",
  "PLAY",      "RND",       "INKEY$",    "PI",
  "FN",        "POINT",     "SCREEN$",   "ATTR",
  "AT",        "TAB",       "VAL$",      "CODE",
  "VAL",       "LEN",       "SIN",       "COS",
  "TAN",       "ASN",       "ACS",       "ATN",
  "LN",        "EXP",       "INT",       "SQR",
  "SGN",       "ABS",       "PEEK",      "IN",
  "USR",       "STR$",      "CHR$",      "NOT",
  "BIN",       "OR",        "AND",       "<=",
  ">=",        "<>",        "LINE",      "THEN",
  "TO",        "STEP",      "DEF FN",    "CAT",
  "FORMAT",    "MOVE",      "ERASE",     "OPEN #",
  "CLOSE #",   "MERGE",     "VERIFY",    "BEEP",
  "CIRCLE",    "INK",       "PAPER",     "FLASH",
  "BRIGHT",    "INVERSE",   "OVER",      "OUT",
  "LPRINT",    "LLIST",     "STOP",      "READ",
  "DATA",      "RESTORE",   "NEW",       "BORDER",
  "CONTINUE",  "DIM",       "REM",       "FOR",
  "GO TO",     "GO SUB",    "INPUT",     "LOAD",
  "LIST",      "LET",       "PAUSE",     "NEXT",
  "POKE",      "PRINT",     "PLOT",      "RUN",
  "SAVE",      "RANDOMIZE", "IF",        "CLS",
  "DRAW",      "CLEAR",     "RETURN",    "COPY"
};

static const char *graphics_tokens[] = {
  "\\  ", "\\ '", "\\' ", "\\''",
  "\\ .", "\\ :", "\\'.", "\\':",
  "\\. ", "\\.'", "\\: ", "\\:'",
  "\\..", "\\.:", "\\:.", "\\::"
};

static libspectrum_error
append_bytes( char **ptr, size_t *remaining, const char *text )
{
  size_t length = strlen( text );

  if( length >= *remaining ) return LIBSPECTRUM_ERROR_INVALID;

  memcpy( *ptr, text, length );
  *ptr += length;
  *remaining -= length;

  return LIBSPECTRUM_ERROR_NONE;
}

libspectrum_dword 
libspectrum_ms_to_tstates( libspectrum_dword ms )
{
  return ms * tstates_per_ms;
}

libspectrum_dword 
libspectrum_tstates_to_ms( libspectrum_dword tstates )
{
  return tstates / tstates_per_ms;
}

void
libspectrum_set_pause_ms( libspectrum_tape_block *block,
                          libspectrum_dword pause_ms )
{
  libspectrum_tape_block_set_pause( block, pause_ms );
  libspectrum_tape_block_set_pause_tstates( block,
                                        libspectrum_ms_to_tstates( pause_ms ) );
}

void
libspectrum_set_pause_tstates( libspectrum_tape_block *block,
                               libspectrum_dword pause_tstates )
{
  libspectrum_tape_block_set_pause_tstates( block, pause_tstates );
  libspectrum_tape_block_set_pause( block,
                                  libspectrum_tstates_to_ms( pause_tstates ) );
}

size_t
libspectrum_bits_to_bytes( size_t bits )
{
  return ( bits + LIBSPECTRUM_BITS_IN_BYTE - 1 ) / LIBSPECTRUM_BITS_IN_BYTE;
}

libspectrum_error
libspectrum_zx_string_to_utf8( char *buffer, size_t length,
                               const libspectrum_byte *src, size_t src_length )
{
  char *ptr;
  const char *text;
  size_t end, i, remaining;
  libspectrum_byte b;
  char udg[3] = "\\a";
  char ascii[2] = " ";

  if( !buffer || !length || !src ) return LIBSPECTRUM_ERROR_INVALID;

  buffer[0] = '\0';
  ptr = buffer;
  remaining = length;

  end = src_length;
  while( end > 0 && src[ end - 1 ] == ' ' ) end--;

  for( i = 0; i < end; i++ ) {
    b = src[ i ];

    switch( b ) {
    case '\\':
      text = "\\\\";
      break;

    case '^':
      text = "↑";
      break;

    case '`':
      text = "£";
      break;

    case 127:
      text = "©";
      break;

    default:
      if( b >= 32 && b < 127 ) {
        ascii[0] = b;
        text = ascii;
      } else if( b >= 128 && b <= 143 ) {
        text = graphics_tokens[ b - 128 ];
      } else if( b >= 144 && b <= 164 ) {
        udg[1] = 'a' + b - 144;
        text = udg;
      } else if( b >= 165 ) {
        text = spectrum_tokens[ b - 165 ];
      } else {
        text = "?";
      }
      break;
    }

    if( append_bytes( &ptr, &remaining, text ) ) {
      *ptr = '\0';
      return LIBSPECTRUM_ERROR_INVALID;
    }
  }

  *ptr = '\0';

  return LIBSPECTRUM_ERROR_NONE;
}

char*
libspectrum_safe_strdup( const char *src )
{
  size_t length;
  char *dest = NULL;

  if( src ) {
    length = strlen( src ) + 1;
    dest = libspectrum_new( char, length );
    memcpy( dest, src, length );
  }

  return dest;
}
