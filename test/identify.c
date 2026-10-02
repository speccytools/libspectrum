/* identify.c: unit tests for libspectrum_identify_class,
               libspectrum_identify_file, and libspectrum_identify_file_with_class
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

#include "internals.h"
#include "libspectrum.h"
#include "test.h"

static test_return_t
check_class( libspectrum_id_t id, libspectrum_class_t expected,
             const char *id_name )
{
  libspectrum_class_t got;
  libspectrum_error err = libspectrum_identify_class( &got, id );

  if( err ) {
    fprintf( stderr, "%s: identify_class(%s): unexpected error %d\n",
             progname, id_name, err );
    return TEST_FAIL;
  }

  if( got != expected ) {
    fprintf( stderr,
             "%s: identify_class(%s): expected class %d, got %d\n",
             progname, id_name, expected, got );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

test_return_t
identify_class_unknown_returns_class_unknown( void )
{
  return check_class( LIBSPECTRUM_ID_UNKNOWN,
                      LIBSPECTRUM_CLASS_UNKNOWN,
                      "LIBSPECTRUM_ID_UNKNOWN" );
}

test_return_t
identify_class_tape_tap_returns_class_tape( void )
{
  return check_class( LIBSPECTRUM_ID_TAPE_TAP,
                      LIBSPECTRUM_CLASS_TAPE,
                      "LIBSPECTRUM_ID_TAPE_TAP" );
}

test_return_t
identify_class_tape_tzx_returns_class_tape( void )
{
  return check_class( LIBSPECTRUM_ID_TAPE_TZX,
                      LIBSPECTRUM_CLASS_TAPE,
                      "LIBSPECTRUM_ID_TAPE_TZX" );
}

test_return_t
identify_class_tape_pzx_returns_class_tape( void )
{
  return check_class( LIBSPECTRUM_ID_TAPE_PZX,
                      LIBSPECTRUM_CLASS_TAPE,
                      "LIBSPECTRUM_ID_TAPE_PZX" );
}

test_return_t
identify_class_tape_csw_returns_class_tape( void )
{
  return check_class( LIBSPECTRUM_ID_TAPE_CSW,
                      LIBSPECTRUM_CLASS_TAPE,
                      "LIBSPECTRUM_ID_TAPE_CSW" );
}

test_return_t
identify_class_snapshot_sna_returns_class_snapshot( void )
{
  return check_class( LIBSPECTRUM_ID_SNAPSHOT_SNA,
                      LIBSPECTRUM_CLASS_SNAPSHOT,
                      "LIBSPECTRUM_ID_SNAPSHOT_SNA" );
}

test_return_t
identify_class_snapshot_szx_returns_class_snapshot( void )
{
  return check_class( LIBSPECTRUM_ID_SNAPSHOT_SZX,
                      LIBSPECTRUM_CLASS_SNAPSHOT,
                      "LIBSPECTRUM_ID_SNAPSHOT_SZX" );
}

test_return_t
identify_class_snapshot_z80_returns_class_snapshot( void )
{
  return check_class( LIBSPECTRUM_ID_SNAPSHOT_Z80,
                      LIBSPECTRUM_CLASS_SNAPSHOT,
                      "LIBSPECTRUM_ID_SNAPSHOT_Z80" );
}

test_return_t
identify_class_recording_rzx_returns_class_recording( void )
{
  return check_class( LIBSPECTRUM_ID_RECORDING_RZX,
                      LIBSPECTRUM_CLASS_RECORDING,
                      "LIBSPECTRUM_ID_RECORDING_RZX" );
}

test_return_t
identify_class_compressed_gz_returns_class_compressed( void )
{
  return check_class( LIBSPECTRUM_ID_COMPRESSED_GZ,
                      LIBSPECTRUM_CLASS_COMPRESSED,
                      "LIBSPECTRUM_ID_COMPRESSED_GZ" );
}

test_return_t
identify_class_compressed_bz2_returns_class_compressed( void )
{
  return check_class( LIBSPECTRUM_ID_COMPRESSED_BZ2,
                      LIBSPECTRUM_CLASS_COMPRESSED,
                      "LIBSPECTRUM_ID_COMPRESSED_BZ2" );
}

test_return_t
identify_class_disk_dsk_returns_class_disk_plus3( void )
{
  return check_class( LIBSPECTRUM_ID_DISK_DSK,
                      LIBSPECTRUM_CLASS_DISK_PLUS3,
                      "LIBSPECTRUM_ID_DISK_DSK" );
}

test_return_t
identify_class_disk_trd_returns_class_disk_trdos( void )
{
  return check_class( LIBSPECTRUM_ID_DISK_TRD,
                      LIBSPECTRUM_CLASS_DISK_TRDOS,
                      "LIBSPECTRUM_ID_DISK_TRD" );
}

test_return_t
identify_class_disk_mdr_returns_class_microdrive( void )
{
  return check_class( LIBSPECTRUM_ID_MICRODRIVE_MDR,
                      LIBSPECTRUM_CLASS_MICRODRIVE,
                      "LIBSPECTRUM_ID_MICRODRIVE_MDR" );
}

test_return_t
identify_class_disk_img_returns_class_disk_plusd( void )
{
  return check_class( LIBSPECTRUM_ID_DISK_IMG,
                      LIBSPECTRUM_CLASS_DISK_PLUSD,
                      "LIBSPECTRUM_ID_DISK_IMG" );
}

test_return_t
identify_class_harddisk_hdf_returns_class_harddisk( void )
{
  return check_class( LIBSPECTRUM_ID_HARDDISK_HDF,
                      LIBSPECTRUM_CLASS_HARDDISK,
                      "LIBSPECTRUM_ID_HARDDISK_HDF" );
}

test_return_t
identify_class_cartridge_dck_returns_class_cartridge_timex( void )
{
  return check_class( LIBSPECTRUM_ID_CARTRIDGE_DCK,
                      LIBSPECTRUM_CLASS_CARTRIDGE_TIMEX,
                      "LIBSPECTRUM_ID_CARTRIDGE_DCK" );
}

test_return_t
identify_class_cartridge_if2_returns_class_cartridge_if2( void )
{
  return check_class( LIBSPECTRUM_ID_CARTRIDGE_IF2,
                      LIBSPECTRUM_CLASS_CARTRIDGE_IF2,
                      "LIBSPECTRUM_ID_CARTRIDGE_IF2" );
}

test_return_t
identify_class_disk_udi_returns_class_disk_generic( void )
{
  return check_class( LIBSPECTRUM_ID_DISK_UDI,
                      LIBSPECTRUM_CLASS_DISK_GENERIC,
                      "LIBSPECTRUM_ID_DISK_UDI" );
}

test_return_t
identify_class_pok_returns_class_auxiliary( void )
{
  return check_class( LIBSPECTRUM_ID_AUX_POK,
                      LIBSPECTRUM_CLASS_AUXILIARY,
                      "LIBSPECTRUM_ID_AUX_POK" );
}

test_return_t
identify_class_screen_scr_returns_class_screenshot( void )
{
  return check_class( LIBSPECTRUM_ID_SCREEN_SCR,
                      LIBSPECTRUM_CLASS_SCREENSHOT,
                      "LIBSPECTRUM_ID_SCREEN_SCR" );
}

test_return_t
identify_class_disk_opd_returns_class_disk_opus( void )
{
  return check_class( LIBSPECTRUM_ID_DISK_OPD,
                      LIBSPECTRUM_CLASS_DISK_OPUS,
                      "LIBSPECTRUM_ID_DISK_OPD" );
}

test_return_t
identify_class_disk_d80_returns_class_disk_didaktik( void )
{
  return check_class( LIBSPECTRUM_ID_DISK_D80,
                      LIBSPECTRUM_CLASS_DISK_DIDAKTIK,
                      "LIBSPECTRUM_ID_DISK_D80" );
}

/* --- libspectrum_identify_file tests --- */

static test_return_t
check_identify_file( const char *label, const unsigned char *buf,
                     size_t len, const char *filename,
                     libspectrum_id_t expected_type )
{
  libspectrum_id_t got_type;
  libspectrum_error err =
    libspectrum_identify_file( &got_type, filename, buf, len );

  if( err ) {
    fprintf( stderr, "%s: identify_file(%s): unexpected error %d\n",
             progname, label, err );
    return TEST_FAIL;
  }

  if( got_type != expected_type ) {
    fprintf( stderr,
             "%s: identify_file(%s): expected type %d, got %d\n",
             progname, label, expected_type, got_type );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

test_return_t
identify_file_tzx_magic_returns_tape_tzx( void )
{
  static const unsigned char tzx_magic[] = {
    'Z','X','T','a','p','e','!', 0x1a, 0x01, 0x14
  };
  return check_identify_file( "TZX magic", tzx_magic, sizeof( tzx_magic ),
                               "test.tzx", LIBSPECTRUM_ID_TAPE_TZX );
}

test_return_t
identify_file_szx_magic_returns_snapshot_szx( void )
{
  static const unsigned char szx_magic[] = {
    'Z','X','S','T', 0x01, 0x04, 0x00, 0x00
  };
  return check_identify_file( "SZX magic", szx_magic, sizeof( szx_magic ),
                               "test.szx", LIBSPECTRUM_ID_SNAPSHOT_SZX );
}

test_return_t
identify_file_rzx_magic_returns_recording_rzx( void )
{
  static const unsigned char rzx_magic[] = {
    'R','Z','X','!', 0x00, 0x0d, 0x00, 0x00
  };
  return check_identify_file( "RZX magic", rzx_magic, sizeof( rzx_magic ),
                               "test.rzx", LIBSPECTRUM_ID_RECORDING_RZX );
}

test_return_t
identify_file_pzx_magic_returns_tape_pzx( void )
{
  static const unsigned char pzx_magic[] = {
    'P','Z','X','T', 0x00, 0x00, 0x00, 0x00
  };
  return check_identify_file( "PZX magic", pzx_magic, sizeof( pzx_magic ),
                               "test.pzx", LIBSPECTRUM_ID_TAPE_PZX );
}

test_return_t
identify_file_unknown_buffer_returns_unknown( void )
{
  static const unsigned char random_data[] = {
    0xde, 0xad, 0xbe, 0xef, 0x12, 0x34, 0x56, 0x78
  };
  return check_identify_file( "unknown data", random_data,
                               sizeof( random_data ),
                               NULL, LIBSPECTRUM_ID_UNKNOWN );
}

/* --- libspectrum_identify_file_with_class tests --- */

test_return_t
identify_file_with_class_tzx_returns_type_and_tape_class( void )
{
  static const unsigned char tzx_magic[] = {
    'Z','X','T','a','p','e','!', 0x1a, 0x01, 0x14
  };
  libspectrum_id_t    type;
  libspectrum_class_t klass;
  libspectrum_error err = libspectrum_identify_file_with_class(
    &type, &klass, "test.tzx", tzx_magic, sizeof( tzx_magic ) );

  if( err ) {
    fprintf( stderr, "%s: identify_file_with_class(TZX): unexpected error %d\n",
             progname, err );
    return TEST_FAIL;
  }

  if( type != LIBSPECTRUM_ID_TAPE_TZX ) {
    fprintf( stderr,
             "%s: identify_file_with_class(TZX): expected type %d, got %d\n",
             progname, LIBSPECTRUM_ID_TAPE_TZX, type );
    return TEST_FAIL;
  }

  if( klass != LIBSPECTRUM_CLASS_TAPE ) {
    fprintf( stderr,
             "%s: identify_file_with_class(TZX): expected class %d, got %d\n",
             progname, LIBSPECTRUM_CLASS_TAPE, klass );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

test_return_t
identify_file_with_class_szx_returns_type_and_snapshot_class( void )
{
  static const unsigned char szx_magic[] = {
    'Z','X','S','T', 0x01, 0x04, 0x00, 0x00
  };
  libspectrum_id_t    type;
  libspectrum_class_t klass;
  libspectrum_error err = libspectrum_identify_file_with_class(
    &type, &klass, "test.szx", szx_magic, sizeof( szx_magic ) );

  if( err ) {
    fprintf( stderr,
             "%s: identify_file_with_class(SZX): unexpected error %d\n",
             progname, err );
    return TEST_FAIL;
  }

  if( type != LIBSPECTRUM_ID_SNAPSHOT_SZX ) {
    fprintf( stderr,
             "%s: identify_file_with_class(SZX): expected type %d, got %d\n",
             progname, LIBSPECTRUM_ID_SNAPSHOT_SZX, type );
    return TEST_FAIL;
  }

  if( klass != LIBSPECTRUM_CLASS_SNAPSHOT ) {
    fprintf( stderr,
             "%s: identify_file_with_class(SZX): expected class %d, got %d\n",
             progname, LIBSPECTRUM_CLASS_SNAPSHOT, klass );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* --- libspectrum_identify_file_raw tests --- */

/* Helper: call libspectrum_identify_file_raw and verify expected type. */
static test_return_t
check_identify_file_raw( const char *label, const unsigned char *buf,
                         size_t len, const char *filename,
                         libspectrum_id_t expected_type )
{
  libspectrum_id_t got_type;
  libspectrum_error err =
    libspectrum_identify_file_raw( &got_type, filename, buf, len );

  if( err ) {
    fprintf( stderr, "%s: identify_file_raw(%s): unexpected error %d\n",
             progname, label, err );
    return TEST_FAIL;
  }

  if( got_type != expected_type ) {
    fprintf( stderr,
             "%s: identify_file_raw(%s): expected type %d, got %d\n",
             progname, label, expected_type, got_type );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

test_return_t
identify_file_raw_tzx_magic_returns_tape_tzx( void )
{
  static const unsigned char tzx_magic[] = {
    'Z','X','T','a','p','e','!', 0x1a, 0x01, 0x14
  };
  return check_identify_file_raw( "TZX magic", tzx_magic, sizeof( tzx_magic ),
                                   "test.tzx", LIBSPECTRUM_ID_TAPE_TZX );
}

test_return_t
identify_file_raw_gz_magic_returns_compressed_gz( void )
{
  /* Unlike identify_file_with_class, identify_file_raw does NOT decompress.
     A GZ buffer must be identified as COMPRESSED_GZ, not the inner type. */
  static const unsigned char gz_magic[] = {
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  return check_identify_file_raw( "GZ magic", gz_magic, sizeof( gz_magic ),
                                   "test.gz", LIBSPECTRUM_ID_COMPRESSED_GZ );
}

test_return_t
identify_file_raw_sna_filename_returns_snapshot_sna( void )
{
  /* SNA has no magic bytes; identification relies on filename extension. */
  static const unsigned char dummy[] = { 0x00, 0x00, 0x00, 0x00 };
  return check_identify_file_raw( "SNA filename", dummy, sizeof( dummy ),
                                   "spectrum.sna",
                                   LIBSPECTRUM_ID_SNAPSHOT_SNA );
}

test_return_t
identify_file_raw_unknown_buffer_returns_unknown( void )
{
  static const unsigned char random_data[] = {
    0xde, 0xad, 0xbe, 0xef, 0x12, 0x34, 0x56, 0x78
  };
  return check_identify_file_raw( "unknown data", random_data,
                                   sizeof( random_data ),
                                   NULL, LIBSPECTRUM_ID_UNKNOWN );
}

#ifdef HAVE_ZLIB_H

static const libspectrum_byte gzip_100_as[] = {
  0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x02, 0xff, 0x73, 0x74, 0xa4, 0x3d, 0x00, 0x00,
  0x8d, 0xbc, 0x97, 0x95, 0x64, 0x00, 0x00, 0x00
};

#endif

test_return_t
identify_gzip_inflate_accepts_exact_limit( void )
{
#ifdef HAVE_ZLIB_H
  libspectrum_byte *output = NULL;
  size_t length = 0;
  libspectrum_error error;

  error = libspectrum_gzip_inflate( gzip_100_as, sizeof( gzip_100_as ),
                                    &output, &length, 100 );
  if( error || length != 100 ) {
    libspectrum_free( output );
    return TEST_FAIL;
  }

  libspectrum_free( output );
#endif

  return TEST_PASS;
}

test_return_t
identify_gzip_inflate_rejects_output_over_limit( void )
{
#ifdef HAVE_ZLIB_H
  libspectrum_byte *output = NULL;
  size_t length = 0;
  libspectrum_error error;

  error = libspectrum_gzip_inflate( gzip_100_as, sizeof( gzip_100_as ),
                                    &output, &length, 99 );
  if( error != LIBSPECTRUM_ERROR_LIMIT || output || length ) {
    libspectrum_free( output );
    return TEST_FAIL;
  }
#endif

  return TEST_PASS;
}
