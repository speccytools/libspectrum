/* wav_macos.c: native macOS internals for WAV files
   Copyright (c) 2026 Fredrick Meunier

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

#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>
#include <limits.h>
#include <string.h>

#include "internals.h"
#include "wav_internals.h"

struct libspectrum_wav_reader {
  ExtAudioFileRef file;
  UInt32 channels;
};

static libspectrum_error
wav_macos_error( libspectrum_error error, const char *message,
                 OSStatus status )
{
  libspectrum_print_error( error, "%s (%d)", message, (int)status );
  return error;
}

static libspectrum_signed_word
wav_macos_mix_frame( const SInt16 *samples, UInt32 channels )
{
  long sum = 0;
  UInt32 i;

  for( i = 0; i < channels; i++ ) sum += samples[i];

  sum /= channels;
  if( sum > 32767 ) sum = 32767;
  if( sum < -32768 ) sum = -32768;

  return (libspectrum_signed_word)sum;
}

libspectrum_error
internal_wav_open( libspectrum_wav_reader **reader, const char *filename,
                   size_t *sample_rate, size_t *frame_count )
{
  libspectrum_wav_reader *state;
  AudioStreamBasicDescription file_format, client_format;
  CFURLRef url;
  UInt32 size;
  SInt64 frames;
  OSStatus status;

  state = libspectrum_new0( libspectrum_wav_reader, 1 );

  url = CFURLCreateFromFileSystemRepresentation(
          NULL, (const UInt8*)filename, (CFIndex)strlen( filename ), false );
  if( !url ) {
    libspectrum_free( state );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_LOGIC,
      "internal_wav_open: failed to create file URL for:%s", filename
    );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  status = ExtAudioFileOpenURL( url, &state->file );
  CFRelease( url );
  if( status ) {
    libspectrum_free( state );
    return wav_macos_error( LIBSPECTRUM_ERROR_LOGIC,
                            "internal_wav_open: failed to open audio file",
                            status );
  }

  size = sizeof( file_format );
  status = ExtAudioFileGetProperty( state->file,
                                    kExtAudioFileProperty_FileDataFormat,
                                    &size, &file_format );
  if( status ) {
    internal_wav_close( state );
    return wav_macos_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "internal_wav_open: failed to get audio file format", status
    );
  }

  size = sizeof( frames );
  status = ExtAudioFileGetProperty( state->file,
                                    kExtAudioFileProperty_FileLengthFrames,
                                    &size, &frames );
  if( status ) {
    internal_wav_close( state );
    return wav_macos_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "internal_wav_open: failed to get audio frame count", status
    );
  }

  if( file_format.mSampleRate <= 0 || frames < 0 ||
      file_format.mChannelsPerFrame == 0 ||
      file_format.mChannelsPerFrame > UINT_MAX / sizeof( SInt16 ) ) {
    internal_wav_close( state );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "internal_wav_open: invalid audio format in WAV file"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  state->channels = file_format.mChannelsPerFrame;

  memset( &client_format, 0, sizeof( client_format ) );
  client_format.mSampleRate = file_format.mSampleRate;
  client_format.mFormatID = kAudioFormatLinearPCM;
  client_format.mFormatFlags = kAudioFormatFlagIsSignedInteger |
                               kAudioFormatFlagIsPacked;
  client_format.mBytesPerPacket = state->channels * sizeof( SInt16 );
  client_format.mFramesPerPacket = 1;
  client_format.mBytesPerFrame = state->channels * sizeof( SInt16 );
  client_format.mChannelsPerFrame = state->channels;
  client_format.mBitsPerChannel = 16;

  size = sizeof( client_format );
  status = ExtAudioFileSetProperty( state->file,
                                    kExtAudioFileProperty_ClientDataFormat,
                                    size, &client_format );
  if( status ) {
    internal_wav_close( state );
    return wav_macos_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "internal_wav_open: failed to set PCM client format", status
    );
  }

  *sample_rate = (size_t)file_format.mSampleRate;
  *frame_count = (size_t)frames;
  *reader = state;

  return LIBSPECTRUM_ERROR_NONE;
}

libspectrum_error
internal_wav_read( libspectrum_wav_reader *reader,
                   libspectrum_signed_word *buffer, size_t frames,
                   size_t *frames_read )
{
  SInt16 *raw;
  UInt32 count;
  UInt32 bytes_per_frame;
  AudioBufferList audio;
  OSStatus status;
  size_t i;

  bytes_per_frame = reader->channels * sizeof( SInt16 );
  raw = libspectrum_new( SInt16, frames * reader->channels );

  audio.mNumberBuffers = 1;
  audio.mBuffers[0].mNumberChannels = reader->channels;
  audio.mBuffers[0].mDataByteSize = frames * bytes_per_frame;
  audio.mBuffers[0].mData = raw;

  count = frames;
  status = ExtAudioFileRead( reader->file, &count, &audio );
  if( status ) {
    libspectrum_free( raw );
    return wav_macos_error( LIBSPECTRUM_ERROR_CORRUPT,
                            "internal_wav_read: failed to read audio frames",
                            status );
  }

  *frames_read = count;
  for( i = 0; i < count; i++ ) {
    buffer[i] = wav_macos_mix_frame( raw + i * reader->channels,
                                     reader->channels );
  }

  libspectrum_free( raw );
  return LIBSPECTRUM_ERROR_NONE;
}

void
internal_wav_close( libspectrum_wav_reader *reader )
{
  if( !reader ) return;

  if( reader->file ) {
    OSStatus status = ExtAudioFileDispose( reader->file );
    if( status ) wav_macos_error( LIBSPECTRUM_ERROR_UNKNOWN,
                                  "internal_wav_close: failed to close audio file",
                                  status );
  }

  libspectrum_free( reader );
}
