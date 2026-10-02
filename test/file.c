/* file.c: tests for the coherent loaded-file representation
   Copyright (c) 2026 Fredrick Meunier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
*/

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "libspectrum.h"
#include "test/test.h"

static test_return_t
check_tzx_file( libspectrum_file *file, const char *name )
{
  static const libspectrum_byte tzx[] = {
    'Z', 'X', 'T', 'a', 'p', 'e', '!', 0x1a, 0x01, 0x14
  };

  if( strcmp( libspectrum_file_name( file ), name ) ||
      libspectrum_file_get_storage( file ) != LIBSPECTRUM_FILE_STORAGE_BUFFER ||
      libspectrum_file_size( file ) != sizeof( tzx ) ||
      memcmp( libspectrum_file_data( file ), tzx, sizeof( tzx ) ) ||
      libspectrum_file_type( file ) != LIBSPECTRUM_ID_TAPE_TZX ||
      libspectrum_file_class( file ) != LIBSPECTRUM_CLASS_TAPE )
    return TEST_FAIL;

  return TEST_PASS;
}

test_return_t
file_read_data_identifies_tzx( void )
{
  static const libspectrum_byte tzx[] = {
    'Z', 'X', 'T', 'a', 'p', 'e', '!', 0x1a, 0x01, 0x14
  };
  libspectrum_file *file = libspectrum_file_alloc();
  libspectrum_error error;
  test_return_t result;

  if( !file ) return TEST_INCOMPLETE;
  error = libspectrum_file_read_data( file, "test.tzx", tzx, sizeof( tzx ) );
  result = error ? TEST_FAIL : check_tzx_file( file, "test.tzx" );
  libspectrum_file_free( file );
  return result;
}

test_return_t
file_read_data_resolves_nested_gzip( void )
{
#ifdef HAVE_ZLIB_H
  static const libspectrum_byte compressed[] = {
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff,
    0x93, 0xef, 0xe6, 0x60, 0x00, 0x01, 0xa6, 0xff, 0xdd, 0x5d,
    0x1c, 0x9e, 0x3a, 0x1e, 0xa1, 0x53, 0x92, 0x44, 0x18, 0x19,
    0x4e, 0xde, 0x9c, 0xaa, 0xc1, 0x05, 0x14, 0x05, 0x00, 0x44,
    0x1c, 0x3a, 0x84, 0x1e, 0x00, 0x00, 0x00
  };
  libspectrum_file *file = libspectrum_file_alloc();
  libspectrum_error error;
  test_return_t result;

  if( !file ) return TEST_INCOMPLETE;
  error = libspectrum_file_read_data( file, "test.tzx.gz.gz", compressed,
                                      sizeof( compressed ) );
  result = error ? TEST_FAIL : check_tzx_file( file, "test.tzx" );
  libspectrum_file_free( file );
  return result;
#else
  return TEST_INCOMPLETE;
#endif
}

test_return_t
file_open_resolves_compressed_file( void )
{
#ifdef HAVE_ZLIB_H
  char filename[] = SRCDIR "/test/file.tzx.gz";
  libspectrum_file *file = libspectrum_file_alloc();
  libspectrum_error error;
  test_return_t result = TEST_PASS;

  if( !file ) return TEST_INCOMPLETE;
  error = libspectrum_file_open( file, filename );
  if( error || libspectrum_file_size( file ) != 266 ||
      libspectrum_file_type( file ) != LIBSPECTRUM_ID_TAPE_TZX ||
      libspectrum_file_class( file ) != LIBSPECTRUM_CLASS_TAPE )
    result = TEST_FAIL;
  libspectrum_file_free( file );
  return result;
#else
  return TEST_INCOMPLETE;
#endif
}

test_return_t
file_open_keeps_hdf_path_backed( void )
{
  char filename[] = SRCDIR "/test/file.hdf";
  libspectrum_file *file = libspectrum_file_alloc();
  libspectrum_error error;
  test_return_t result = TEST_PASS;

  if( !file ) return TEST_INCOMPLETE;
  error = libspectrum_file_open( file, filename );
  if( error ||
      libspectrum_file_get_storage( file ) != LIBSPECTRUM_FILE_STORAGE_PATH ||
      libspectrum_file_data( file ) || libspectrum_file_size( file ) ||
      libspectrum_file_type( file ) != LIBSPECTRUM_ID_HARDDISK_HDF ||
      libspectrum_file_class( file ) != LIBSPECTRUM_CLASS_HARDDISK )
    result = TEST_FAIL;
  libspectrum_file_free( file );
  return result;
}
