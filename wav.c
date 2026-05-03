/* wav.c: Routines for handling WAV raw audio files
   Copyright (c) 2007-2026 Fredrick Meunier

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

   E-mail: fredm@spamcop.net

*/

#include "config.h"

#include "internals.h"
#include "tape_block.h"
#include "wav_internals.h"

libspectrum_error
libspectrum_wav_read( libspectrum_tape *tape, const char *filename )
{
  libspectrum_wav_reader *reader = NULL;
  libspectrum_signed_word *buffer = NULL;
  libspectrum_byte *tape_buffer = NULL;
  libspectrum_tape_block *block = NULL;
  size_t sample_rate, length, tape_length, data_length;
  size_t total_frames = 0, frames_read;
  libspectrum_error error;

  if( !filename ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_LOGIC,
      "libspectrum_wav_read: no filename provided - wav files can only be loaded from a file"
    );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  error = internal_wav_open( &reader, filename, &sample_rate, &length );
  if( error ) return error;

  if( !length ) {
    internal_wav_close( reader );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "libspectrum_wav_read: empty audio file, nothing to load"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  tape_length = length;
  if( tape_length % 8 ) tape_length += 8 - ( tape_length % 8 );

  buffer = libspectrum_new0( libspectrum_signed_word, tape_length );

  while( total_frames < length ) {
    error = internal_wav_read( reader, buffer + total_frames,
                               length - total_frames, &frames_read );
    if( error ) goto error;

    if( !frames_read ) {
      libspectrum_print_error(
        LIBSPECTRUM_ERROR_CORRUPT,
        "libspectrum_wav_read: audio file ended after %lu frames, expected %lu",
        (unsigned long)total_frames, (unsigned long)length
      );
      error = LIBSPECTRUM_ERROR_CORRUPT;
      goto error;
    }

    total_frames += frames_read;
  }

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_RAW_DATA );

  libspectrum_tape_block_set_bit_length( block, 3500000 / sample_rate );
  libspectrum_set_pause_ms( block, 0 );
  libspectrum_tape_block_set_bits_in_last_byte(
    block,
    length % LIBSPECTRUM_BITS_IN_BYTE ?
      length % LIBSPECTRUM_BITS_IN_BYTE : LIBSPECTRUM_BITS_IN_BYTE
  );
  data_length = tape_length / LIBSPECTRUM_BITS_IN_BYTE;
  libspectrum_tape_block_set_data_length( block, data_length );

  tape_buffer = libspectrum_new0( libspectrum_byte, data_length );

  {
    libspectrum_signed_word *from = buffer;
    libspectrum_byte *to = tape_buffer;
    size_t remaining = tape_length;

    do {
      libspectrum_byte val = 0;
      int i;
      for( i = 7; i >= 0; i-- ) {
        if( *from++ > 0 ) val |= 1 << i;
      }
      *to++ = val;
    } while( ( remaining -= 8 ) > 0 );
  }

  libspectrum_tape_block_set_data( block, tape_buffer );
  libspectrum_tape_append_block( tape, block );

  internal_wav_close( reader );
  libspectrum_free( buffer );
  return LIBSPECTRUM_ERROR_NONE;

error:
  internal_wav_close( reader );
  libspectrum_free( buffer );
  if( tape_buffer ) libspectrum_free( tape_buffer );
  if( block ) libspectrum_tape_block_free( block );
  return error;
}
