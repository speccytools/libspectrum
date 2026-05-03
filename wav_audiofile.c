/* wav_audiofile.c: libaudiofile internals for WAV files
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
*/

#include "config.h"

#include <audiofile.h>

#include "internals.h"
#include "wav_internals.h"

struct libspectrum_wav_reader {
  AFfilehandle handle;
  int track;
};

libspectrum_error
internal_wav_open( libspectrum_wav_reader **reader, const char *filename,
                   size_t *sample_rate, size_t *frame_count )
{
  libspectrum_wav_reader *state;
  AFframecount frames;
  double rate;

  state = libspectrum_new( libspectrum_wav_reader, 1 );
  state->track = AF_DEFAULT_TRACK;

  state->handle = afOpenFile( filename, "r", NULL );
  if( state->handle == AF_NULL_FILEHANDLE ) {
    libspectrum_free( state );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_LOGIC,
      "internal_wav_open: audiofile failed to open file:%s", filename
    );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  if( afSetVirtualSampleFormat( state->handle, state->track,
                                AF_SAMPFMT_UNSIGNED, 8 ) ) {
    afCloseFile( state->handle );
    libspectrum_free( state );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_LOGIC,
      "internal_wav_open: audiofile failed to set virtual sample format"
    );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  if( afSetVirtualChannels( state->handle, state->track, 1 ) ) {
    afCloseFile( state->handle );
    libspectrum_free( state );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_LOGIC,
      "internal_wav_open: audiofile failed to set virtual channel count"
    );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  frames = afGetFrameCount( state->handle, state->track );
  if( frames < 0 ) {
    afCloseFile( state->handle );
    libspectrum_free( state );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "internal_wav_open: can't calculate number of frames in audio file"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  rate = afGetRate( state->handle, state->track );
  if( rate <= 0 ) {
    afCloseFile( state->handle );
    libspectrum_free( state );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "internal_wav_open: invalid sample rate in audio file"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  *sample_rate = (size_t)rate;
  *frame_count = (size_t)frames;
  *reader = state;

  return LIBSPECTRUM_ERROR_NONE;
}

libspectrum_error
internal_wav_read( libspectrum_wav_reader *reader,
                   libspectrum_signed_word *buffer, size_t frames,
                   size_t *frames_read )
{
  libspectrum_byte *raw;
  int count, i;

  raw = libspectrum_new( libspectrum_byte, frames );
  count = afReadFrames( reader->handle, reader->track, raw, frames );
  if( count == -1 ) {
    libspectrum_free( raw );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "internal_wav_read: failed to read audio frames"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  *frames_read = count;
  for( i = 0; i < count; i++ ) buffer[i] = raw[i] - 127;

  libspectrum_free( raw );
  return LIBSPECTRUM_ERROR_NONE;
}

void
internal_wav_close( libspectrum_wav_reader *reader )
{
  if( !reader ) return;

  if( afCloseFile( reader->handle ) ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_UNKNOWN,
      "internal_wav_close: failed to close audio file"
    );
  }

  libspectrum_free( reader );
}
