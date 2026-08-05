/* identify.c: unit tests for libspectrum_identify_class
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
