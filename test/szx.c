/* szx.c: SZX test routines
   Copyright (c) 2017,2023,2026 Philip Kendall

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

/* This file contains a number of routines for checking the contents of an SZX
   file. It very deliberately does not use any of the core libspectrum code for
   this as that would defeat the point of a unit test */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "internals.h"
#include "common.h"
#include "test.h"

typedef struct szx_chunk_t {
  libspectrum_byte *data;
  size_t length;
} szx_chunk_t;

static szx_chunk_t*
find_szx_chunk( libspectrum_byte *in_buffer, size_t in_buffer_length,
                const char *search )
{
  libspectrum_byte *data;
  size_t data_remaining;
  libspectrum_byte id[5] = {0};
  libspectrum_byte length_buffer[4];
  libspectrum_dword length;

  data = in_buffer;
  data_remaining = in_buffer_length;

  if( data_remaining < 8 ) {
    fprintf( stderr, "SZX file is less than 8 bytes long\n" );
    return NULL;
  }

  /* Skip header */
  data += 8; data_remaining -= 8;

  while( data_remaining > 0 ) {
    if( data_remaining < 8 ) {
      fprintf( stderr, "Chunk is less than 8 bytes long\n" );
      return NULL;
    }

    memcpy( id, data, 4 );
    data += 4; data_remaining -= 4;

    memcpy( length_buffer, data, 4 );
    data += 4; data_remaining -= 4;

    length = length_buffer[0] + (length_buffer[1] << 8) +
      (length_buffer[2] << 16) + (length_buffer[3] << 24);

    if( data_remaining < length ) {
      fprintf( stderr, "Not enough data for chunk\n" );
      return NULL;
    }

    if( !memcmp( id, search, 4 ) ) {
      szx_chunk_t *chunk = libspectrum_malloc( sizeof(*chunk) );
      libspectrum_byte *chunk_data = libspectrum_malloc( length );
      memcpy( chunk_data, data, length );
      chunk->data = chunk_data;
      chunk->length = length;
      return chunk;
    }

    data += length; data_remaining -= length;
  }

  return NULL;
}

static test_return_t
szx_write_block_test_with_flags( const char *id, libspectrum_machine machine,
    int flags, void (*setter)( libspectrum_snap* ),
    libspectrum_byte *expected, size_t expected_length, size_t total_length )
{
  libspectrum_snap *snap;
  libspectrum_byte *buffer; size_t length;
  libspectrum_error error;
  int out_flags;
  szx_chunk_t *chunk;
  size_t i;
  test_return_t r = TEST_INCOMPLETE;

  snap = libspectrum_snap_alloc();

  libspectrum_snap_set_machine( snap, machine );

  setter( snap );

  length = 0;
  buffer = NULL;
  error = libspectrum_snap_write( &buffer, &length, &out_flags, snap,
                          LIBSPECTRUM_ID_SNAPSHOT_SZX, NULL, flags );
  if (error != LIBSPECTRUM_ERROR_NONE) {
    fprintf( stderr, "Snap write failed with error %d\n", error );
    if (buffer)
      libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }
  libspectrum_snap_free( snap );

  chunk = find_szx_chunk( buffer, length, id );
  if( !chunk ) {
    fprintf( stderr, "Chunk not found\n" );
    libspectrum_free( buffer );
    return TEST_FAIL;
  }

  libspectrum_free( buffer );

  if( chunk->length == total_length ) {
    if( memcmp( chunk->data, expected, expected_length ) ) {
      fprintf( stderr, "Chunk has wrong initial data\n" );
      r = TEST_FAIL;
    } else {
      r = TEST_PASS;
      for( i = expected_length; i < total_length; i++ ) {
        if( chunk->data[i] ) {
          r = TEST_FAIL;
          break;
        }
      }
    }
  } else {
    fprintf( stderr, "Chunk has wrong length (expected %lu, got %lu)\n",
    (unsigned long)total_length, (unsigned long)chunk->length );
    r = TEST_FAIL;
  }

  libspectrum_free( chunk->data );
  libspectrum_free( chunk );

  return r;
}

static test_return_t
szx_write_block_test( const char *id, libspectrum_machine machine,
    void (*setter)( libspectrum_snap* ),
    libspectrum_byte *expected, size_t expected_length, size_t total_length )
{
  return szx_write_block_test_with_flags( id, machine, 0, setter,
      expected, expected_length, total_length );
}

static test_return_t
szx_write_uncompressed_block_test( const char *id, libspectrum_machine machine,
    void (*setter)( libspectrum_snap* ),
    libspectrum_byte *expected, size_t expected_length, size_t total_length )
{
  return szx_write_block_test_with_flags( id, machine,
      LIBSPECTRUM_FLAG_SNAPSHOT_NO_COMPRESSION, setter,
      expected, expected_length, total_length );
}

static void
z80r_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_a( snap, 0xc4 );
  libspectrum_snap_set_f( snap, 0x1f );
  libspectrum_snap_set_bc( snap, 0x0306 );
  libspectrum_snap_set_de( snap, 0x06e4 );
  libspectrum_snap_set_hl( snap, 0x0154 );

  libspectrum_snap_set_a_( snap, 0x69 );
  libspectrum_snap_set_f_( snap, 0x07 );
  libspectrum_snap_set_bc_( snap, 0xe7dc );
  libspectrum_snap_set_de_( snap, 0xc3d0 );
  libspectrum_snap_set_hl_( snap, 0xdccb );

  libspectrum_snap_set_ix( snap, 0x8ba3 );
  libspectrum_snap_set_iy( snap, 0x1c13 );
  libspectrum_snap_set_sp( snap, 0xf86d );
  libspectrum_snap_set_pc( snap, 0xc81e );

  libspectrum_snap_set_i( snap, 0x19 );
  libspectrum_snap_set_r( snap, 0x84 );
  libspectrum_snap_set_iff1( snap, 1 );
  libspectrum_snap_set_iff2( snap, 0 );
  libspectrum_snap_set_im( snap, 2 );

  libspectrum_snap_set_tstates( snap, 40 );

  libspectrum_snap_set_last_instruction_ei( snap, 1 );
  libspectrum_snap_set_halted( snap, 0 );
  libspectrum_snap_set_last_instruction_set_f( snap, 1 );

  libspectrum_snap_set_memptr( snap, 0xdc03 );
}

static libspectrum_byte
test_31_expected[] = {
  0x1f, 0xc4, 0x06, 0x03, 0xe4, 0x06, 0x54, 0x01, /* AF, BC, DE, HL */
  0x07, 0x69, 0xdc, 0xe7, 0xd0, 0xc3, 0xcb, 0xdc, /* AF', BC', DE', HL' */
  0xa3, 0x8b, 0x13, 0x1c, 0x6d, 0xf8, 0x1e, 0xc8, /* IX, IY, SP, PC */
  0x19, 0x84, 0x01, 0x00, 0x02, /* I, R, IFF1, IFF2, IM */
  0x28, 0x00, 0x00, 0x00, 0x08, /* tstates, tstates until /INT goes high */
  0x05, /* flags */
  0x03, 0xdc /* MEMPTR */
};

test_return_t
write_szx_z80r_chunk( void )
{
  return szx_write_block_test( "Z80R", LIBSPECTRUM_MACHINE_48, z80r_setter,
      test_31_expected, ARRAY_SIZE(test_31_expected), ARRAY_SIZE(test_31_expected) );
}

static void
spcr_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_out_ula( snap, 0xfa );
  libspectrum_snap_set_out_128_memoryport( snap, 0x6f );
  libspectrum_snap_set_out_plus3_memoryport( snap, 0x28 );
}

static libspectrum_byte
test_32_expected[] = {
  0x02, 0x6f, 0x28, 0xfa, /* Border, 128, +3, ULA */
  0x00, 0x00, 0x00, 0x00 /* Reserved */
};

test_return_t
write_szx_spcr_chunk( void )
{
  return szx_write_block_test( "SPCR", LIBSPECTRUM_MACHINE_PLUS3, spcr_setter,
      test_32_expected, ARRAY_SIZE(test_32_expected), ARRAY_SIZE(test_32_expected) );
}

static void
joy_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_joystick_active_count( snap, 2 );
  libspectrum_snap_set_joystick_list( snap, 0, LIBSPECTRUM_JOYSTICK_KEMPSTON );
  libspectrum_snap_set_joystick_inputs( snap, 0,
      LIBSPECTRUM_JOYSTICK_INPUT_JOYSTICK_1 );
  libspectrum_snap_set_joystick_list( snap, 1, LIBSPECTRUM_JOYSTICK_SINCLAIR_1 );
  libspectrum_snap_set_joystick_inputs( snap, 1,
      LIBSPECTRUM_JOYSTICK_INPUT_JOYSTICK_2 );
}

static libspectrum_byte
test_33_expected[] = {
  0x01, 0x00, 0x00, 0x00, /* Flags */
  0x00, 0x03 /* Joystick 1 = Kempston, Joystick 2 = Sinclair 1 */
};

test_return_t
write_szx_joy_chunk( void )
{
  return szx_write_block_test( "JOY\0", LIBSPECTRUM_MACHINE_48, joy_setter,
      test_33_expected, ARRAY_SIZE(test_33_expected), ARRAY_SIZE(test_33_expected) );
}

static void
keyb_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_issue2( snap, 1 );
  libspectrum_snap_set_joystick_active_count( snap, 1 );
  libspectrum_snap_set_joystick_list( snap, 0, LIBSPECTRUM_JOYSTICK_CURSOR );
  libspectrum_snap_set_joystick_inputs( snap, 0,
      LIBSPECTRUM_JOYSTICK_INPUT_KEYBOARD );
}

static libspectrum_byte
test_34_expected[] = {
  0x01, 0x00, 0x00, 0x00, /* Flags */
  0x02 /* Cursor joystick */
};

test_return_t
write_szx_keyb_chunk( void )
{
  return szx_write_block_test( "KEYB", LIBSPECTRUM_MACHINE_48, keyb_setter,
      test_34_expected, ARRAY_SIZE(test_34_expected), ARRAY_SIZE(test_34_expected) );
}

static void
zxpr_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_zx_printer_active( snap, 1 );
}

static libspectrum_byte
test_35_expected[] = {
  0x01, 0x00 /* Flags */
};

test_return_t
write_szx_zxpr_chunk( void )
{
  return szx_write_block_test( "ZXPR", LIBSPECTRUM_MACHINE_48, zxpr_setter,
      test_35_expected, ARRAY_SIZE(test_35_expected), ARRAY_SIZE(test_35_expected) );
}

static libspectrum_byte
ay_registers_data[] = {
  0x73, 0x03, 0xb1, 0x00, 0xbb, 0x0c, 0x19, 0x0f,
  0x1e, 0x07, 0x11, 0x71, 0x6c, 0x0a, 0x2b, 0x41
};

static void
ay_setter( libspectrum_snap *snap )
{
  size_t i;

  libspectrum_snap_set_fuller_box_active( snap, 1 );
  libspectrum_snap_set_melodik_active( snap, 0 );
  libspectrum_snap_set_out_ay_registerport( snap, 0x08 );

  for( i = 0; i < 16; i++ ) {
    libspectrum_snap_set_ay_registers( snap, i, ay_registers_data[i] );
  }
}

static libspectrum_byte
test_36_expected[] = {
  0x01, /* Flags */
  0x08, /* Register port */
  0x73, 0x03, 0xb1, 0x00, 0xbb, 0x0c, 0x19, 0x0f, /* Registers 0x00 - 0x07 */
  0x1e, 0x07, 0x11, 0x71, 0x6c, 0x0a, 0x2b, 0x41 /* Register 0x08 - 0x0f */
};

test_return_t
write_szx_ay_chunk( void )
{
  return szx_write_block_test( "AY\0\0", LIBSPECTRUM_MACHINE_48, ay_setter,
      test_36_expected, ARRAY_SIZE(test_36_expected), ARRAY_SIZE(test_36_expected) );
}

static void
scld_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_out_scld_hsr( snap, 0x49 );
  libspectrum_snap_set_out_scld_dec( snap, 0x9d );
}

static libspectrum_byte
test_37_expected[] = {
  0x49, 0x9d
};

test_return_t
write_szx_scld_chunk( void )
{
  return szx_write_block_test( "SCLD", LIBSPECTRUM_MACHINE_TC2048, scld_setter,
      test_37_expected, ARRAY_SIZE(test_37_expected), ARRAY_SIZE(test_37_expected) );
}

static void
zxat_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_zxatasp_active( snap, 1 );

  libspectrum_snap_set_zxatasp_upload( snap, 1 );
  libspectrum_snap_set_zxatasp_writeprotect( snap, 0 );
  libspectrum_snap_set_zxatasp_port_a( snap, 0xab );
  libspectrum_snap_set_zxatasp_port_b( snap, 0x8c );
  libspectrum_snap_set_zxatasp_port_c( snap, 0x82 );
  libspectrum_snap_set_zxatasp_control( snap, 0xd8 );
  libspectrum_snap_set_zxatasp_pages( snap, 0x18 );
  libspectrum_snap_set_zxatasp_current_page( snap, 0x11 );
}

static libspectrum_byte
test_38_expected[] = {
  0x01, 0x00, /* Flags */
  0xab, 0x8c, 0x82, 0xd8, /* Ports */
  0x18, 0x11 /* Page count and current page */
};

test_return_t
write_szx_zxat_chunk( void )
{
  return szx_write_block_test( "ZXAT", LIBSPECTRUM_MACHINE_48, zxat_setter,
      test_38_expected, ARRAY_SIZE(test_38_expected), ARRAY_SIZE(test_38_expected) );
}

static void
zxcf_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_zxcf_active( snap, 1 );

  libspectrum_snap_set_zxcf_upload( snap, 1 );
  libspectrum_snap_set_zxcf_memctl( snap, 0x37 );
  libspectrum_snap_set_zxcf_pages( snap, 0x35 );
}

static libspectrum_byte
test_39_expected[] = {
  0x01, 0x00, /* Flags */
  0x37, /* Memory control */
  0x35 /* Page count */
};

test_return_t
write_szx_zxcf_chunk( void )
{
  return szx_write_block_test( "ZXCF", LIBSPECTRUM_MACHINE_48, zxcf_setter,
      test_39_expected, ARRAY_SIZE(test_39_expected), ARRAY_SIZE(test_39_expected) );
}

static void
amxm_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_kempston_mouse_active( snap, 1 );
}

static libspectrum_byte
test_40_expected[] = {
  0x02, /* Kempston mouse */
  0x00, 0x00, 0x00, /* AMX mouse CTRLA registers */
  0x00, 0x00, 0x00 /* AMX mouse CTRLB registers */
};

test_return_t
write_szx_amxm_chunk( void )
{
  return szx_write_block_test( "AMXM", LIBSPECTRUM_MACHINE_48, amxm_setter,
      test_40_expected, ARRAY_SIZE(test_40_expected), ARRAY_SIZE(test_40_expected) );
}

static void
side_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_simpleide_active( snap, 1 );
}

static libspectrum_byte
empty_chunk_expected[] = { /* Empty */ };

test_return_t
write_szx_side_chunk( void )
{
  return szx_write_block_test( "SIDE", LIBSPECTRUM_MACHINE_48, side_setter,
      empty_chunk_expected, ARRAY_SIZE(empty_chunk_expected), ARRAY_SIZE(empty_chunk_expected) );
}

static void
drum_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_specdrum_active( snap, 1 );
  libspectrum_snap_set_specdrum_dac( snap, -0x3b );
}

static libspectrum_byte
test_42_expected[] = {
  0x45 /* DAC + 128 */
};

test_return_t
write_szx_drum_chunk( void )
{
  return szx_write_block_test( "DRUM", LIBSPECTRUM_MACHINE_48, drum_setter,
      test_42_expected, ARRAY_SIZE(test_42_expected), ARRAY_SIZE(test_42_expected) );
}

static void
covx_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_covox_active( snap, 1 );
  libspectrum_snap_set_covox_dac( snap, 0xc0 );
}

static libspectrum_byte
test_43_expected[] = {
  0xc0, /* DAC */
  0x00, 0x00, 0x00 /* Reserved */
};

test_return_t
write_szx_covx_chunk( void )
{
  return szx_write_block_test( "COVX", LIBSPECTRUM_MACHINE_48, covx_setter,
      test_43_expected, ARRAY_SIZE(test_43_expected), ARRAY_SIZE(test_43_expected) );
}

static void
zmmc_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_zxmmc_active( snap, 1 );
}

test_return_t
write_szx_zmmc_chunk( void )
{
  return szx_write_block_test( "ZMMC", LIBSPECTRUM_MACHINE_48, zmmc_setter,
      empty_chunk_expected, ARRAY_SIZE(empty_chunk_expected), ARRAY_SIZE(empty_chunk_expected) );
}

static void
uspeech_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_uspeech_active( snap, 1 );
  libspectrum_snap_set_uspeech_paged( snap, 1 );
}

static libspectrum_byte
test_59_expected[] = {
  0x01 /* Paged */
};

test_return_t
write_szx_uspe_chunk( void )
{
  return szx_write_block_test( "USPE", LIBSPECTRUM_MACHINE_48, uspeech_setter,
      test_59_expected, ARRAY_SIZE(test_59_expected), ARRAY_SIZE(test_59_expected) );
}

static void
pltt_setter( libspectrum_snap *snap )
{
  libspectrum_byte *palette = libspectrum_new( libspectrum_byte, 64 );
  memset( palette, 0, 64 );
  palette[0] = 0x11;
  palette[63] = 0x22;

  libspectrum_snap_set_ulaplus_active( snap, 1 );
  libspectrum_snap_set_ulaplus_palette_enabled( snap, 1 );
  libspectrum_snap_set_ulaplus_current_register( snap, 0x15 );
  libspectrum_snap_set_ulaplus_palette( snap, 0, palette );
  libspectrum_snap_set_ulaplus_ff_register( snap, 0x30 );
}

static libspectrum_byte
test_60_expected[] = {
  0x01, /* Flags (ENABLED) */
  0x15, /* Current register */
  0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* palette[0..7] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* palette[8..15] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* palette[16..23] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* palette[24..31] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* palette[32..39] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* palette[40..47] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* palette[48..55] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, /* palette[56..63] */
  0x30  /* ff_register */
};

test_return_t
write_szx_pltt_chunk( void )
{
  return szx_write_block_test( "PLTT", LIBSPECTRUM_MACHINE_48, pltt_setter,
      test_60_expected, ARRAY_SIZE(test_60_expected), ARRAY_SIZE(test_60_expected) );
}

static void
ramp_setter( libspectrum_snap *snap )
{
  libspectrum_byte *ram = libspectrum_malloc0_n( 1, 0x4000 );
  libspectrum_snap_set_pages( snap, 0, ram );
}

#ifdef HAVE_ZLIB_H
static libspectrum_byte
empty_ram_page_expected[] = {
  0x01, 0x00, /* Flags */
  0x00, /* Page number */
  /* 16 Kb of zeros compressed */
  0x78, 0xda, 0xed, 0xc1, 0x31, 0x01, 0x00, 0x00,
  0x00, 0xc2, 0xa0, 0xf5, 0x4f, 0x6d, 0x0c, 0x1f,
  0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0xb7, 0x01, 0x40, 0x00, 0x00, 0x01
};
#else
static libspectrum_byte
empty_ram_page_uncompressed_expected[] = {
  0x00, 0x00, /* Flags */
  0x00, /* Page number */
  /* 16 Kb of zeros uncompressed */
};
#endif

test_return_t
write_szx_ramp_chunk( void )
{
  return szx_write_block_test( "RAMP", LIBSPECTRUM_MACHINE_48, ramp_setter,
    #ifdef HAVE_ZLIB_H
      /* gzip enabled in build, so block written compressed */
      empty_ram_page_expected, ARRAY_SIZE(empty_ram_page_expected),
      ARRAY_SIZE(empty_ram_page_expected)
    #else
      /* gzip not enabled in build, so block written uncompressed */
      empty_ram_page_uncompressed_expected, ARRAY_SIZE(empty_ram_page_uncompressed_expected),
      ARRAY_SIZE(empty_ram_page_uncompressed_expected) + 0x4000
    #endif
  );
}

static void
atrp_setter( libspectrum_snap *snap )
{
  libspectrum_byte *ram;

  libspectrum_snap_set_zxatasp_active( snap, 1 );
  libspectrum_snap_set_zxatasp_pages( snap, 1 );

  ram = libspectrum_malloc0_n( 1, 0x4000 );
  libspectrum_snap_set_zxatasp_ram( snap, 0, ram );
}

test_return_t
write_szx_atrp_chunk( void )
{
  return szx_write_block_test( "ATRP", LIBSPECTRUM_MACHINE_48, atrp_setter,
    #ifdef HAVE_ZLIB_H
      /* gzip enabled in build, so block written compressed */
      empty_ram_page_expected, ARRAY_SIZE(empty_ram_page_expected),
      ARRAY_SIZE(empty_ram_page_expected)
    #else
      /* gzip not enabled in build, so block written uncompressed */
      empty_ram_page_uncompressed_expected, ARRAY_SIZE(empty_ram_page_uncompressed_expected),
      ARRAY_SIZE(empty_ram_page_uncompressed_expected) + 0x4000
    #endif
  );
}

static void
cfrp_setter( libspectrum_snap *snap )
{
  libspectrum_byte *ram;

  libspectrum_snap_set_zxcf_active( snap, 1 );
  libspectrum_snap_set_zxcf_pages( snap, 1 );

  ram = libspectrum_malloc0_n( 1, 0x4000 );
  libspectrum_snap_set_zxcf_ram( snap, 0, ram );
}

test_return_t
write_szx_cfrp_chunk( void )
{
  return szx_write_block_test( "CFRP", LIBSPECTRUM_MACHINE_48, cfrp_setter,
    #ifdef HAVE_ZLIB_H
      /* gzip enabled in build, so block written compressed */
      empty_ram_page_expected, ARRAY_SIZE(empty_ram_page_expected),
      ARRAY_SIZE(empty_ram_page_expected)
    #else
      /* gzip not enabled in build, so block written uncompressed */
      empty_ram_page_uncompressed_expected, ARRAY_SIZE(empty_ram_page_uncompressed_expected),
      ARRAY_SIZE(empty_ram_page_uncompressed_expected) + 0x4000
    #endif
  );
}

static libspectrum_byte
empty_ram_page_start[] = {
  0x00, 0x00, /* Flags */
  0x00, /* Page number */
  /* Followed by 16 Kb of uncompressed zeros */
};

test_return_t
write_uncompressed_szx_ramp_chunk( void )
{
  return szx_write_uncompressed_block_test( "RAMP", LIBSPECTRUM_MACHINE_48,
      ramp_setter, empty_ram_page_start, ARRAY_SIZE(empty_ram_page_start),
      ARRAY_SIZE(empty_ram_page_start) + 0x4000 );
}

test_return_t
write_uncompressed_szx_atrp_chunk( void )
{
  return szx_write_uncompressed_block_test( "ATRP", LIBSPECTRUM_MACHINE_48,
      atrp_setter, empty_ram_page_start, ARRAY_SIZE(empty_ram_page_start),
      ARRAY_SIZE(empty_ram_page_start) + 0x4000 );
}

test_return_t
write_uncompressed_szx_cfrp_chunk( void )
{
  return szx_write_uncompressed_block_test( "CFRP", LIBSPECTRUM_MACHINE_48,
      cfrp_setter, empty_ram_page_start, ARRAY_SIZE(empty_ram_page_start),
      ARRAY_SIZE(empty_ram_page_start) + 0x4000 );
}

static test_return_t
szx_read_block_test_with_template( const char *id, const char *template,
    int (*check_fn)( libspectrum_snap* ) )
{
  char filename[ 256 ];
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_snap *snap;
  int failed = 0;

  snprintf( filename, 256, template, id );

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  snap = libspectrum_snap_alloc();

  if( libspectrum_snap_read( snap, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
			     filename ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: error reading `%s'\n", progname, filename );
    libspectrum_snap_free( snap );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  failed = check_fn( snap );

  libspectrum_snap_free( snap );

  return failed ? TEST_FAIL : TEST_PASS;
}

static test_return_t
szx_read_block_test( const char *id, int (*check_fn)( libspectrum_snap* ) )
{
  return szx_read_block_test_with_template( id,
      STATIC_TEST_PATH( "szx-chunks/%s.szx" ), check_fn );
}

static test_return_t
szx_read_block_from_compressed_snap_test( const char *id,
    int (*check_fn)( libspectrum_snap* ) )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test_with_template( id,
      STATIC_TEST_PATH( "szx-chunks/%s-uncompressed.szx.gz" ), check_fn );
}

static int
test_44_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_a( snap ) != 0xc4 ) failed = 1;

  if( libspectrum_snap_a( snap ) != 0xc4 ) failed = 1;
  if( libspectrum_snap_f( snap ) != 0x1f ) failed = 1;
  if( libspectrum_snap_bc( snap ) != 0x0306 ) failed = 1;
  if( libspectrum_snap_de( snap ) != 0x06e4 ) failed = 1;
  if( libspectrum_snap_hl( snap ) != 0x0154 ) failed = 1;

  if( libspectrum_snap_a_( snap ) != 0x69 ) failed = 1;
  if( libspectrum_snap_f_( snap ) != 0x07 ) failed = 1;
  if( libspectrum_snap_bc_( snap ) != 0xe7dc ) failed = 1;
  if( libspectrum_snap_de_( snap ) != 0xc3d0 ) failed = 1;
  if( libspectrum_snap_hl_( snap ) != 0xdccb ) failed = 1;

  if( libspectrum_snap_ix( snap ) != 0x8ba3 ) failed = 1;
  if( libspectrum_snap_iy( snap ) != 0x1c13 ) failed = 1;
  if( libspectrum_snap_sp( snap ) != 0xf86d ) failed = 1;
  if( libspectrum_snap_pc( snap ) != 0xc81e ) failed = 1;

  if( libspectrum_snap_i( snap ) != 0x19 ) failed = 1;
  if( libspectrum_snap_r( snap ) != 0x84 ) failed = 1;
  if( libspectrum_snap_iff1( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_iff2( snap ) != 0 ) failed = 1;
  if( libspectrum_snap_im( snap ) != 2 ) failed = 1;

  if( libspectrum_snap_tstates( snap ) != 40 ) failed = 1;

  if( libspectrum_snap_last_instruction_ei( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_halted( snap ) != 0 ) failed = 1;
  if( libspectrum_snap_last_instruction_set_f( snap ) != 1 ) failed = 1;

  if( libspectrum_snap_memptr( snap ) != 0xdc03 ) failed = 1;

  return failed;
}

test_return_t
read_szx_z80r_chunk( void )
{
  return szx_read_block_test( "Z80R", test_44_check );
}

static int
test_45_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_out_ula( snap ) != 0xfa ) failed = 1;
  if( libspectrum_snap_out_128_memoryport( snap ) != 0x6f ) failed = 1;
  if( libspectrum_snap_out_plus3_memoryport( snap ) != 0x28 ) failed = 1;

  return failed;
}

test_return_t
read_szx_spcr_chunk( void )
{
  return szx_read_block_test( "SPCR", test_45_check );
}

static int
test_46_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_joystick_active_count( snap ) != 2 ) failed = 1;
  if( libspectrum_snap_joystick_list( snap, 0 ) != LIBSPECTRUM_JOYSTICK_KEMPSTON ) failed = 1;
  if( libspectrum_snap_joystick_inputs( snap, 0 ) != LIBSPECTRUM_JOYSTICK_INPUT_JOYSTICK_1 ) failed = 1;
  if( libspectrum_snap_joystick_list( snap, 1 ) != LIBSPECTRUM_JOYSTICK_SINCLAIR_1 ) failed = 1;
  if( libspectrum_snap_joystick_inputs( snap, 1 ) != LIBSPECTRUM_JOYSTICK_INPUT_JOYSTICK_2 ) failed = 1;

  return failed;
}

test_return_t
read_szx_joy_chunk( void )
{
  return szx_read_block_test( "JOY", test_46_check );
}

static int
test_47_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_issue2( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_joystick_active_count( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_joystick_list( snap, 0 ) != LIBSPECTRUM_JOYSTICK_CURSOR ) failed = 1;
  if( libspectrum_snap_joystick_inputs( snap, 0 ) != LIBSPECTRUM_JOYSTICK_INPUT_KEYBOARD ) failed = 1;

  return failed;
}

test_return_t
read_szx_keyb_chunk( void )
{
  return szx_read_block_test( "KEYB", test_47_check );
}

static int
test_48_check( libspectrum_snap *snap )
{
  return libspectrum_snap_zx_printer_active( snap ) != 1;
}

test_return_t
read_szx_zxpr_chunk( void )
{
  return szx_read_block_test( "ZXPR", test_48_check );
}

static int
test_49_check( libspectrum_snap *snap )
{
  int failed = 0;
  size_t i;

  if( libspectrum_snap_fuller_box_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_melodik_active( snap ) != 0 ) failed = 1;
  if( libspectrum_snap_out_ay_registerport( snap ) != 0x08 ) failed = 1;

  for( i = 0; i < 16; i++ ) {
    if( libspectrum_snap_ay_registers( snap, i ) != ay_registers_data[i] ) failed = 1;
  }

  return failed;
}

test_return_t
read_szx_ay_chunk( void )
{
  return szx_read_block_test( "AY", test_49_check );
}

static int
test_50_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_out_scld_hsr( snap ) != 0x49 ) failed = 1;
  if( libspectrum_snap_out_scld_dec( snap ) != 0x9d ) failed = 1;

  return failed;
}

test_return_t
read_szx_scld_chunk( void )
{
  return szx_read_block_test( "SCLD", test_50_check );
}

static int
test_51_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_zxatasp_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_zxatasp_upload( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_zxatasp_writeprotect( snap ) != 0 ) failed = 1;
  if( libspectrum_snap_zxatasp_port_a( snap ) != 0xab ) failed = 1;
  if( libspectrum_snap_zxatasp_port_b( snap ) != 0x8c ) failed = 1;
  if( libspectrum_snap_zxatasp_port_c( snap ) != 0x82 ) failed = 1;
  if( libspectrum_snap_zxatasp_control( snap ) != 0xd8 ) failed = 1;
  if( libspectrum_snap_zxatasp_pages( snap ) != 0x18 ) failed = 1;
  if( libspectrum_snap_zxatasp_current_page( snap ) != 0x11 ) failed = 1;

  return failed;
}

test_return_t
read_szx_zxat_chunk( void )
{
  return szx_read_block_test( "ZXAT", test_51_check );
}

static int
test_52_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_zxcf_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_zxcf_upload( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_zxcf_memctl( snap ) != 0x37 ) failed = 1;
  if( libspectrum_snap_zxcf_pages( snap ) != 0x55 ) failed = 1;

  return failed;
}

test_return_t
read_szx_zxcf_chunk( void )
{
  return szx_read_block_test( "ZXCF", test_52_check );
}

static int
test_53_check( libspectrum_snap *snap )
{
  return libspectrum_snap_kempston_mouse_active( snap ) != 1;
}

test_return_t
read_szx_amxm_chunk( void )
{
  return szx_read_block_test( "AMXM", test_53_check );
}

static int
test_54_check( libspectrum_snap *snap )
{
  return libspectrum_snap_simpleide_active( snap ) != 1;
}

test_return_t
read_szx_side_chunk( void )
{
  return szx_read_block_test( "SIDE", test_54_check );
}

static int
test_55_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_specdrum_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_specdrum_dac( snap ) != -0x3b ) failed = 1;

  return failed;
}

test_return_t
read_szx_drum_chunk( void )
{
  return szx_read_block_test( "DRUM", test_55_check );
}

static int
test_56_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_covox_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_covox_dac( snap ) != 0xc0 ) failed = 1;

  return failed;
}

test_return_t
read_szx_covx_chunk( void )
{
  return szx_read_block_test( "COVX", test_56_check );
}

static int
test_58_check( libspectrum_snap *snap )
{
  return libspectrum_snap_zxmmc_active( snap ) != 1;
}

test_return_t
read_szx_zmmc_chunk( void )
{
  return szx_read_block_test( "ZMMC", test_58_check );
}

static int
test_59_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_uspeech_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_uspeech_paged( snap ) != 0 ) failed = 1;

  return failed;
}

test_return_t
read_szx_uspe_chunk( void )
{
  return szx_read_block_test( "USPE", test_59_check );
}

static int
test_60_check( libspectrum_snap *snap )
{
  int failed = 0;
  libspectrum_byte *palette;

  if( libspectrum_snap_ulaplus_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_ulaplus_palette_enabled( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_ulaplus_current_register( snap ) != 0x3f ) failed = 1;

  palette = libspectrum_snap_ulaplus_palette( snap, 0 );
  if( !palette || palette[0] != 0x5a ) failed = 1;

  if( libspectrum_snap_ulaplus_ff_register( snap ) != 0x7e ) failed = 1;

  return failed;
}

test_return_t
read_szx_pltt_chunk( void )
{
  return szx_read_block_test( "PLTT", test_60_check );
}

static int
empty_ram_page_check( libspectrum_snap *snap,
    libspectrum_byte* (*get_ram_page)( libspectrum_snap*, int ) )
{
  int failed = 0;
  size_t i;

  libspectrum_byte *page = get_ram_page( snap, 0 );
  if( page ) {
    for( i = 0; i < 0x4000; i++ ) {
      if( page[i] ) {
        failed = 1;
        break;
      }
    }
  } else {
    failed = 1;
  }

  return failed;
}

static int
ramp_check( libspectrum_snap *snap )
{
  return empty_ram_page_check( snap, libspectrum_snap_pages );
}

test_return_t
read_szx_ramp_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test( "RAMP", ramp_check );
}

static int
atrp_check( libspectrum_snap *snap )
{
  return empty_ram_page_check( snap, libspectrum_snap_zxatasp_ram );
}

test_return_t
read_szx_atrp_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test( "ATRP", atrp_check );
}

static int
cfrp_check( libspectrum_snap *snap )
{
  return empty_ram_page_check( snap, libspectrum_snap_zxcf_ram );
}

test_return_t
read_szx_cfrp_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test( "CFRP", cfrp_check );
}

test_return_t
read_uncompressed_szx_ramp_chunk( void )
{
  return szx_read_block_from_compressed_snap_test( "RAMP", ramp_check );
}

test_return_t
read_uncompressed_szx_atrp_chunk( void )
{
  return szx_read_block_from_compressed_snap_test( "ATRP", atrp_check );
}

test_return_t
read_uncompressed_szx_cfrp_chunk( void )
{
  return szx_read_block_from_compressed_snap_test( "CFRP", cfrp_check );
}

/* IF1 chunk: Interface 1 without custom ROM */

static void
if1_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_interface1_active( snap, 1 );
  libspectrum_snap_set_interface1_paged( snap, 1 );
  libspectrum_snap_set_interface1_drive_count( snap, 3 );
}

static libspectrum_byte
if1_expected[] = {
  0x05, 0x00, /* flags: ENABLED(1) | PAGED(4) */
  0x03,       /* drive_count */
  0x00, 0x00, 0x00, /* reserved bytes */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserved dwords */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00  /* expected_length = 0 (no custom ROM) */
};

test_return_t
write_szx_if1_chunk( void )
{
  return szx_write_block_test( "IF1", LIBSPECTRUM_MACHINE_48, if1_setter,
      if1_expected, ARRAY_SIZE(if1_expected), ARRAY_SIZE(if1_expected) );
}

static int
if1_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_interface1_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_interface1_paged( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_interface1_drive_count( snap ) != 3 ) failed = 1;

  return failed;
}

test_return_t
read_szx_if1_chunk( void )
{
  return szx_read_block_test( "IF1", if1_check );
}

/* B128 chunk: Beta 128 disk interface without custom ROM */

static void
b128_setter( libspectrum_snap *snap )
{
  libspectrum_snap_set_beta_active( snap, 1 );
  libspectrum_snap_set_beta_paged( snap, 1 );
  libspectrum_snap_set_beta_autoboot( snap, 1 );
  libspectrum_snap_set_beta_direction( snap, 1 ); /* seek higher, so SEEKLOWER not set */
  libspectrum_snap_set_beta_drive_count( snap, 2 );
  libspectrum_snap_set_beta_system( snap, 0x11 );
  libspectrum_snap_set_beta_track( snap, 5 );
  libspectrum_snap_set_beta_sector( snap, 3 );
  libspectrum_snap_set_beta_data( snap, 0xAA );
  libspectrum_snap_set_beta_status( snap, 0xFF );
}

static libspectrum_byte
b128_expected[] = {
  0x0d, 0x00, 0x00, 0x00, /* flags: CONNECTED(1) | PAGED(4) | AUTOBOOT(8) */
  0x02,                   /* drive_count */
  0x11,                   /* system */
  0x05,                   /* track */
  0x03,                   /* sector */
  0xaa,                   /* data */
  0xff                    /* status */
};

test_return_t
write_szx_b128_chunk( void )
{
  return szx_write_block_test( "B128", LIBSPECTRUM_MACHINE_48, b128_setter,
      b128_expected, ARRAY_SIZE(b128_expected), ARRAY_SIZE(b128_expected) );
}

static int
b128_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_beta_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_beta_paged( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_beta_drive_count( snap ) != 2 ) failed = 1;
  if( libspectrum_snap_beta_system( snap ) != 0x11 ) failed = 1;
  if( libspectrum_snap_beta_track( snap ) != 5 ) failed = 1;
  if( libspectrum_snap_beta_sector( snap ) != 3 ) failed = 1;
  if( libspectrum_snap_beta_data( snap ) != 0xAA ) failed = 1;
  if( libspectrum_snap_beta_status( snap ) != 0xFF ) failed = 1;

  return failed;
}

test_return_t
read_szx_b128_chunk( void )
{
  return szx_read_block_test( "B128", b128_check );
}
static void
snet_setter( libspectrum_snap *snap )
{
  libspectrum_byte *w5100 = libspectrum_new0( libspectrum_byte, 0x30 );
  libspectrum_byte *flash = libspectrum_new0( libspectrum_byte, 0x20000 );
  libspectrum_byte *ram   = libspectrum_new0( libspectrum_byte, 0x20000 );
  libspectrum_snap_set_spectranet_active( snap, 1 );
  libspectrum_snap_set_spectranet_paged( snap, 1 );
  libspectrum_snap_set_spectranet_page_a( snap, 3 );
  libspectrum_snap_set_spectranet_page_b( snap, 5 );
  libspectrum_snap_set_spectranet_w5100( snap, 0, w5100 );
  libspectrum_snap_set_spectranet_flash( snap, 0, flash );
  libspectrum_snap_set_spectranet_ram( snap, 0, ram );
}

/* flags(2) + page_a(1) + page_b(1) + programmable_trap(2) = 6 bytes;
   remaining 48 bytes are W5100 data (verified as zero by total_length check) */
static libspectrum_byte
test_65_expected[] = {
  0x01, 0x00, /* Flags: PAGED */
  0x03,       /* page_a */
  0x05,       /* page_b */
  0x00, 0x00  /* programmable_trap */
};

test_return_t
write_szx_snet_chunk( void )
{
  return szx_write_block_test( "SNET", LIBSPECTRUM_MACHINE_48, snet_setter,
      test_65_expected, ARRAY_SIZE(test_65_expected), 54 );
}

static int
test_65_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( libspectrum_snap_spectranet_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_spectranet_paged( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_spectranet_page_a( snap ) != 3 ) failed = 1;
  if( libspectrum_snap_spectranet_page_b( snap ) != 5 ) failed = 1;

  return failed;
}

test_return_t
read_szx_snet_chunk( void )
{
  return szx_read_block_test( "SNET", test_65_check );
}
static void
mfce_setter( libspectrum_snap *snap )
{
  libspectrum_byte *ram = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_multiface_active( snap, 1 );
  libspectrum_snap_set_multiface_model_one( snap, 1 );
  libspectrum_snap_set_multiface_paged( snap, 1 );
  libspectrum_snap_set_multiface_ram( snap, 0, ram );
  libspectrum_snap_set_multiface_ram_length( snap, 0, 0x2000 );
}

static libspectrum_byte
mfce_expected[] = {
  0x00, /* Model = ZXSTMFM_1 */
  0x01  /* Flags = ZXSTMF_PAGEDIN */
};

test_return_t
write_szx_mfce_chunk( void )
{
  return szx_write_uncompressed_block_test( "MFCE", LIBSPECTRUM_MACHINE_48,
      mfce_setter, mfce_expected, ARRAY_SIZE(mfce_expected),
      ARRAY_SIZE(mfce_expected) + 0x2000 );
}

static int
mfce_check( libspectrum_snap *snap )
{
  int failed = 0;
  size_t i;
  libspectrum_byte *ram;

  if( libspectrum_snap_multiface_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_multiface_model_one( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_multiface_paged( snap ) != 1 ) failed = 1;

  ram = libspectrum_snap_multiface_ram( snap, 0 );
  if( ram ) {
    for( i = 0; i < 0x2000; i++ ) {
      if( ram[i] ) {
        failed = 1;
        break;
      }
    }
  } else {
    failed = 1;
  }

  return failed;
}

test_return_t
read_szx_mfce_chunk( void )
{
  return szx_read_block_test( "MFCE", mfce_check );
}
static void
dide_setter( libspectrum_snap *snap )
{
  libspectrum_byte *eprom = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_divide_active( snap, 1 );
  libspectrum_snap_set_divide_eprom( snap, 0, eprom );
}

#ifdef HAVE_ZLIB_H
static libspectrum_byte
empty_eprom_page_compressed_expected[] = {
  0x04, 0x00, /* Flags: COMPRESSED */
  0x00, /* Control */
  0x00, /* Page count */
  /* 8 KB of zeros compressed */
  0x78, 0xda, 0xed, 0xc1, 0x01, 0x0d, 0x00, 0x00,
  0x00, 0xc2, 0xa0, 0xf7, 0x4f, 0x6d, 0x0e, 0x37,
  0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x77, 0x03, 0x20, 0x00, 0x00, 0x01
};
#else
static libspectrum_byte
empty_eprom_page_uncompressed_expected[] = {
  0x00, 0x00, /* Flags */
  0x00, /* Control */
  0x00, /* Page count */
  /* Followed by 8 KB of uncompressed zeros */
};
#endif

test_return_t
write_szx_dide_chunk( void )
{
  return szx_write_block_test( "DIDE", LIBSPECTRUM_MACHINE_48, dide_setter,
    #ifdef HAVE_ZLIB_H
      /* gzip enabled in build, so block written compressed */
      empty_eprom_page_compressed_expected,
      ARRAY_SIZE(empty_eprom_page_compressed_expected),
      ARRAY_SIZE(empty_eprom_page_compressed_expected)
    #else
      /* gzip not enabled in build, so block written uncompressed */
      empty_eprom_page_uncompressed_expected,
      ARRAY_SIZE(empty_eprom_page_uncompressed_expected),
      ARRAY_SIZE(empty_eprom_page_uncompressed_expected) + 0x2000
    #endif
  );
}

static void
dmmc_setter( libspectrum_snap *snap )
{
  libspectrum_byte *eprom = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_divmmc_active( snap, 1 );
  libspectrum_snap_set_divmmc_eprom( snap, 0, eprom );
}

test_return_t
write_szx_dmmc_chunk( void )
{
  return szx_write_block_test( "DMMC", LIBSPECTRUM_MACHINE_48, dmmc_setter,
    #ifdef HAVE_ZLIB_H
      /* gzip enabled in build, so block written compressed */
      empty_eprom_page_compressed_expected,
      ARRAY_SIZE(empty_eprom_page_compressed_expected),
      ARRAY_SIZE(empty_eprom_page_compressed_expected)
    #else
      /* gzip not enabled in build, so block written uncompressed */
      empty_eprom_page_uncompressed_expected,
      ARRAY_SIZE(empty_eprom_page_uncompressed_expected),
      ARRAY_SIZE(empty_eprom_page_uncompressed_expected) + 0x2000
    #endif
  );
}

static int
dide_check( libspectrum_snap *snap )
{
  int failed = 0;
  libspectrum_byte *eprom;

  if( libspectrum_snap_divide_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_divide_paged( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_divide_control( snap ) != 0xab ) failed = 1;
  if( libspectrum_snap_divide_pages( snap ) != 1 ) failed = 1;

  eprom = libspectrum_snap_divide_eprom( snap, 0 );
  if( !eprom || eprom[0] != 0x12 ) failed = 1;

  return failed;
}

test_return_t
read_szx_dide_chunk( void )
{
  return szx_read_block_test( "DIDE", dide_check );
}

static int
dmmc_check( libspectrum_snap *snap )
{
  int failed = 0;
  libspectrum_byte *eprom;

  if( libspectrum_snap_divmmc_active( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_divmmc_paged( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_divmmc_control( snap ) != 0xcd ) failed = 1;
  if( libspectrum_snap_divmmc_pages( snap ) != 2 ) failed = 1;

  eprom = libspectrum_snap_divmmc_eprom( snap, 0 );
  if( !eprom || eprom[0] != 0x34 ) failed = 1;

  return failed;
}

test_return_t
read_szx_dmmc_chunk( void )
{
  return szx_read_block_test( "DMMC", dmmc_check );
}
/* OPUS (Opus Discovery interface) chunk tests */

static void
opus_setter( libspectrum_snap *snap )
{
  libspectrum_byte *rom, *ram;

  libspectrum_snap_set_opus_active( snap, 1 );
  libspectrum_snap_set_opus_paged( snap, 1 );
  libspectrum_snap_set_opus_direction( snap, 1 );  /* hubwards, no SEEKLOWER */

  libspectrum_snap_set_opus_control_a( snap, 0xca );
  libspectrum_snap_set_opus_data_reg_a( snap, 0xfe );
  libspectrum_snap_set_opus_data_dir_a( snap, 0xef );
  libspectrum_snap_set_opus_control_b( snap, 0xbd );
  libspectrum_snap_set_opus_data_reg_b( snap, 0xdb );
  libspectrum_snap_set_opus_data_dir_b( snap, 0xb7 );
  libspectrum_snap_set_opus_drive_count( snap, 2 );
  libspectrum_snap_set_opus_track( snap, 5 );
  libspectrum_snap_set_opus_sector( snap, 0x11 );
  libspectrum_snap_set_opus_data( snap, 0x22 );
  libspectrum_snap_set_opus_status( snap, 0x33 );

  rom = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_opus_rom( snap, 0, rom );

  ram = libspectrum_malloc0_n( 1, 0x800 );
  libspectrum_snap_set_opus_ram( snap, 0, ram );
}

/* Expected OPUS chunk header: flags(4) + ram_len(4) + rom_len(4) + 11 regs = 23 bytes.
   Full chunk = 23 + 0x800 (RAM) = 2071 bytes. */
static libspectrum_byte
opus_expected[] = {
  0x01, 0x00, 0x00, 0x00,  /* flags: PAGED */
  0x00, 0x08, 0x00, 0x00,  /* RAM length: 0x800 */
  0x00, 0x00, 0x00, 0x00,  /* ROM length: 0 (standard ROM) */
  0xca,                    /* control_a */
  0xfe,                    /* data_reg_a */
  0xef,                    /* data_dir_a */
  0xbd,                    /* control_b */
  0xdb,                    /* data_reg_b */
  0xb7,                    /* data_dir_b */
  0x02,                    /* drive_count */
  0x05,                    /* track */
  0x11,                    /* sector */
  0x22,                    /* data */
  0x33,                    /* status */
};

test_return_t
write_szx_opus_chunk( void )
{
  return szx_write_block_test_with_flags( "OPUS", LIBSPECTRUM_MACHINE_48,
      LIBSPECTRUM_FLAG_SNAPSHOT_NO_COMPRESSION, opus_setter,
      opus_expected, ARRAY_SIZE(opus_expected),
      ARRAY_SIZE(opus_expected) + 0x800 );
}

static int
opus_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( !libspectrum_snap_opus_active( snap ) ) failed = 1;
  if( !libspectrum_snap_opus_paged( snap ) ) failed = 1;
  if( !libspectrum_snap_opus_direction( snap ) ) failed = 1;
  if( libspectrum_snap_opus_control_a( snap ) != 0xca ) failed = 1;
  if( libspectrum_snap_opus_data_reg_a( snap ) != 0xfe ) failed = 1;
  if( libspectrum_snap_opus_data_dir_a( snap ) != 0xef ) failed = 1;
  if( libspectrum_snap_opus_control_b( snap ) != 0xbd ) failed = 1;
  if( libspectrum_snap_opus_data_reg_b( snap ) != 0xdb ) failed = 1;
  if( libspectrum_snap_opus_data_dir_b( snap ) != 0xb7 ) failed = 1;
  if( libspectrum_snap_opus_drive_count( snap ) != 2 ) failed = 1;
  if( libspectrum_snap_opus_track( snap ) != 5 ) failed = 1;
  if( libspectrum_snap_opus_sector( snap ) != 0x11 ) failed = 1;
  if( libspectrum_snap_opus_data( snap ) != 0x22 ) failed = 1;
  if( libspectrum_snap_opus_status( snap ) != 0x33 ) failed = 1;
  if( !libspectrum_snap_opus_ram( snap, 0 ) ) failed = 1;

  return failed;
}

test_return_t
read_szx_opus_chunk( void )
{
  return szx_read_block_test( "OPUS", opus_check );
}

/* PLSD (+D interface) chunk tests */

static void
plusd_setter( libspectrum_snap *snap )
{
  libspectrum_byte *rom, *ram;

  libspectrum_snap_set_plusd_active( snap, 1 );
  libspectrum_snap_set_plusd_paged( snap, 1 );
  libspectrum_snap_set_plusd_direction( snap, 1 );  /* hubwards, no SEEKLOWER */

  libspectrum_snap_set_plusd_control( snap, 0xbc );
  libspectrum_snap_set_plusd_drive_count( snap, 1 );
  libspectrum_snap_set_plusd_track( snap, 7 );
  libspectrum_snap_set_plusd_sector( snap, 0x0a );
  libspectrum_snap_set_plusd_data( snap, 0xff );
  libspectrum_snap_set_plusd_status( snap, 0x00 );

  rom = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_plusd_rom( snap, 0, rom );

  ram = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_plusd_ram( snap, 0, ram );
}

/* Expected PLSD chunk header: flags(4) + ram_len(4) + rom_len(4) + rom_type(1) + 6 regs = 19 bytes.
   Full chunk = 19 + 0x2000 (RAM) = 8211 bytes. */
static libspectrum_byte
plusd_expected[] = {
  0x01, 0x00, 0x00, 0x00,  /* flags: PAGED */
  0x00, 0x20, 0x00, 0x00,  /* RAM length: 0x2000 */
  0x00, 0x00, 0x00, 0x00,  /* ROM length: 0 (standard GDOS ROM) */
  0x00,                    /* rom_type: GDOS */
  0xbc,                    /* control */
  0x01,                    /* drive_count */
  0x07,                    /* track */
  0x0a,                    /* sector */
  0xff,                    /* data */
  0x00,                    /* status */
};

test_return_t
write_szx_plsd_chunk( void )
{
  return szx_write_block_test_with_flags( "PLSD", LIBSPECTRUM_MACHINE_48,
      LIBSPECTRUM_FLAG_SNAPSHOT_NO_COMPRESSION, plusd_setter,
      plusd_expected, ARRAY_SIZE(plusd_expected),
      ARRAY_SIZE(plusd_expected) + 0x2000 );
}

static int
plusd_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( !libspectrum_snap_plusd_active( snap ) ) failed = 1;
  if( !libspectrum_snap_plusd_paged( snap ) ) failed = 1;
  if( !libspectrum_snap_plusd_direction( snap ) ) failed = 1;
  if( libspectrum_snap_plusd_control( snap ) != 0xbc ) failed = 1;
  if( libspectrum_snap_plusd_drive_count( snap ) != 1 ) failed = 1;
  if( libspectrum_snap_plusd_track( snap ) != 7 ) failed = 1;
  if( libspectrum_snap_plusd_sector( snap ) != 0x0a ) failed = 1;
  if( libspectrum_snap_plusd_data( snap ) != 0xff ) failed = 1;
  if( libspectrum_snap_plusd_status( snap ) != 0x00 ) failed = 1;
  if( !libspectrum_snap_plusd_ram( snap, 0 ) ) failed = 1;

  return failed;
}

test_return_t
read_szx_plsd_chunk( void )
{
  return szx_read_block_test( "PLSD", plusd_check );
}

/* DOCK (TC2068 cartridge slot) chunk tests */

static void
dock_setter( libspectrum_snap *snap )
{
  libspectrum_byte *cart;

  libspectrum_snap_set_dock_active( snap, 1 );

  cart = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_dock_cart( snap, 0, cart );
  libspectrum_snap_set_dock_ram( snap, 0, 0 );  /* not writeable */
}

/* Expected DOCK chunk: flags word (EXROMDOCK=4) + page byte + 8192 data bytes.
   Checked prefix = 3 bytes; total = 3 + 0x2000 = 8195 bytes. */
static libspectrum_byte
dock_expected[] = {
  0x04, 0x00,  /* flags: ZXSTDOCKF_EXROMDOCK (DOCK slot, not writeable, uncompressed) */
  0x00,        /* page 0 */
};

test_return_t
write_szx_dock_chunk( void )
{
  return szx_write_block_test_with_flags( "DOCK", LIBSPECTRUM_MACHINE_TC2048,
      LIBSPECTRUM_FLAG_SNAPSHOT_NO_COMPRESSION, dock_setter,
      dock_expected, ARRAY_SIZE(dock_expected),
      ARRAY_SIZE(dock_expected) + 0x2000 );
}

static int
dock_check( libspectrum_snap *snap )
{
  int failed = 0;

  if( !libspectrum_snap_dock_active( snap ) ) failed = 1;
  if( !libspectrum_snap_dock_cart( snap, 0 ) ) failed = 1;
  if( libspectrum_snap_dock_ram( snap, 0 ) ) failed = 1;

  return failed;
}

test_return_t
read_szx_dock_chunk( void )
{
  return szx_read_block_test( "DOCK", dock_check );
}
/* DivIDE RAM page (DIRP) — 8 KB pages */

static void
dirp_setter( libspectrum_snap *snap )
{
  libspectrum_byte *eprom = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_byte *ram   = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_divide_active( snap, 1 );
  libspectrum_snap_set_divide_pages( snap, 1 );
  libspectrum_snap_set_divide_eprom( snap, 0, eprom );
  libspectrum_snap_set_divide_ram( snap, 0, ram );
}

#ifdef HAVE_ZLIB_H
static libspectrum_byte
empty_8k_ram_page_expected[] = {
  0x01, 0x00, /* Flags (COMPRESSED) */
  0x00,       /* Page number */
  /* 8 KB of zeros compressed */
  0x78, 0xda, 0xed, 0xc1, 0x01, 0x0d, 0x00, 0x00,
  0x00, 0xc2, 0xa0, 0xf7, 0x4f, 0x6d, 0x0e, 0x37,
  0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x77, 0x03, 0x20, 0x00, 0x00, 0x01
};
#else
static libspectrum_byte
empty_8k_ram_page_uncompressed_expected[] = {
  0x00, 0x00, /* Flags */
  0x00,       /* Page number */
  /* 8 KB of zeros uncompressed */
};
#endif

test_return_t
write_szx_dirp_chunk( void )
{
  return szx_write_block_test( "DIRP", LIBSPECTRUM_MACHINE_48, dirp_setter,
    #ifdef HAVE_ZLIB_H
      empty_8k_ram_page_expected, ARRAY_SIZE(empty_8k_ram_page_expected),
      ARRAY_SIZE(empty_8k_ram_page_expected)
    #else
      empty_8k_ram_page_uncompressed_expected,
      ARRAY_SIZE(empty_8k_ram_page_uncompressed_expected),
      ARRAY_SIZE(empty_8k_ram_page_uncompressed_expected) + 0x2000
    #endif
  );
}

static int
empty_8k_ram_page_check( libspectrum_snap *snap,
    libspectrum_byte* (*get_ram_page)( libspectrum_snap*, int ) )
{
  int failed = 0;
  size_t i;

  libspectrum_byte *page = get_ram_page( snap, 0 );
  if( page ) {
    for( i = 0; i < 0x2000; i++ ) {
      if( page[i] ) {
        failed = 1;
        break;
      }
    }
  } else {
    failed = 1;
  }

  return failed;
}

static int
dirp_check( libspectrum_snap *snap )
{
  return empty_8k_ram_page_check( snap, libspectrum_snap_divide_ram );
}

test_return_t
read_szx_dirp_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test( "DIRP", dirp_check );
}

/* DivMMC RAM page (DMRP) — 8 KB pages */

static void
dmrp_setter( libspectrum_snap *snap )
{
  libspectrum_byte *eprom = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_byte *ram   = libspectrum_malloc0_n( 1, 0x2000 );
  libspectrum_snap_set_divmmc_active( snap, 1 );
  libspectrum_snap_set_divmmc_pages( snap, 1 );
  libspectrum_snap_set_divmmc_eprom( snap, 0, eprom );
  libspectrum_snap_set_divmmc_ram( snap, 0, ram );
}

test_return_t
write_szx_dmrp_chunk( void )
{
  return szx_write_block_test( "DMRP", LIBSPECTRUM_MACHINE_48, dmrp_setter,
    #ifdef HAVE_ZLIB_H
      empty_8k_ram_page_expected, ARRAY_SIZE(empty_8k_ram_page_expected),
      ARRAY_SIZE(empty_8k_ram_page_expected)
    #else
      empty_8k_ram_page_uncompressed_expected,
      ARRAY_SIZE(empty_8k_ram_page_uncompressed_expected),
      ARRAY_SIZE(empty_8k_ram_page_uncompressed_expected) + 0x2000
    #endif
  );
}

static int
dmrp_check( libspectrum_snap *snap )
{
  return empty_8k_ram_page_check( snap, libspectrum_snap_divmmc_ram );
}

test_return_t
read_szx_dmrp_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test( "DMRP", dmrp_check );
}

/* Interface 2 ROM (IF2R) — 16 KB, always compressed */

#ifdef HAVE_ZLIB_H
static void
if2r_setter( libspectrum_snap *snap )
{
  libspectrum_byte *rom = libspectrum_malloc0_n( 1, 0x4000 );
  libspectrum_snap_set_interface2_active( snap, 1 );
  libspectrum_snap_set_interface2_rom( snap, 0, rom );
}


static libspectrum_byte
if2r_expected[] = {
  /* Compressed length (little-endian dword) */
  0x27, 0x00, 0x00, 0x00,
  /* 16 KB of zeros compressed */
  0x78, 0xda, 0xed, 0xc1, 0x31, 0x01, 0x00, 0x00,
  0x00, 0xc2, 0xa0, 0xf5, 0x4f, 0x6d, 0x0c, 0x1f,
  0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0xb7, 0x01, 0x40, 0x00, 0x00, 0x01
};
#endif

test_return_t
write_szx_if2r_chunk( void )
{
  #ifdef HAVE_ZLIB_H
    return szx_write_block_test( "IF2R", LIBSPECTRUM_MACHINE_48, if2r_setter,
        if2r_expected, ARRAY_SIZE(if2r_expected), ARRAY_SIZE(if2r_expected) );
  #else
    return TEST_SKIPPED; /* IF2R always writes compressed — requires zlib */
  #endif
}

static int
if2r_check( libspectrum_snap *snap )
{
  int failed = 0;
  size_t i;

  if( !libspectrum_snap_interface2_active( snap ) ) return 1;

  libspectrum_byte *rom = libspectrum_snap_interface2_rom( snap, 0 );
  if( !rom ) return 1;

  for( i = 0; i < 0x4000; i++ ) {
    if( rom[i] ) { failed = 1; break; }
  }

  return failed;
}

test_return_t
read_szx_if2r_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* IF2R always reads compressed — requires zlib */
  #endif

  return szx_read_block_test( "IF2R", if2r_check );
}
#ifdef HAVE_ZLIB_H
static void
spectranet_setter( libspectrum_snap *snap )
{
  libspectrum_byte *flash = libspectrum_malloc0_n( 1, 0x20000 );
  libspectrum_byte *ram = libspectrum_malloc0_n( 1, 0x20000 );
  libspectrum_byte *w5100 = libspectrum_malloc0_n( 1, 0x30 );
  libspectrum_snap_set_spectranet_active( snap, 1 );
  libspectrum_snap_set_spectranet_flash( snap, 0, flash );
  libspectrum_snap_set_spectranet_ram( snap, 0, ram );
  libspectrum_snap_set_spectranet_w5100( snap, 0, w5100 );
}

static void
snef_setter( libspectrum_snap *snap )
{
  spectranet_setter( snap );
}

static void
sner_setter( libspectrum_snap *snap )
{
  spectranet_setter( snap );
}

static libspectrum_byte
empty_spectranet_page_expected[] = {
  0x01,                         /* flags: compressed */
  0x95, 0x00, 0x00, 0x00,       /* compressed_data_size: 149 */
  /* 128 KB of zeros compressed */
  0x78, 0xda, 0xed, 0xc1, 0x31, 0x01, 0x00, 0x00,
  0x00, 0xc2, 0xa0, 0xf5, 0x4f, 0xed, 0x61, 0x0d,
  0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x6e, 0x00, 0x1e, 0x00, 0x01,
};
#endif

test_return_t
write_szx_snef_chunk( void )
{
  #ifdef HAVE_ZLIB_H
    return szx_write_block_test( "SNEF", LIBSPECTRUM_MACHINE_48, snef_setter,
        empty_spectranet_page_expected, ARRAY_SIZE(empty_spectranet_page_expected),
        ARRAY_SIZE(empty_spectranet_page_expected) );
  #else
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif
}

test_return_t
write_szx_sner_chunk( void )
{
  #ifdef HAVE_ZLIB_H
    return szx_write_block_test( "SNER", LIBSPECTRUM_MACHINE_48, sner_setter,
        empty_spectranet_page_expected, ARRAY_SIZE(empty_spectranet_page_expected),
        ARRAY_SIZE(empty_spectranet_page_expected) );
  #else
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif
}

static int
snef_check( libspectrum_snap *snap )
{
  int failed = 0;
  size_t i;

  libspectrum_byte *page = libspectrum_snap_spectranet_flash( snap, 0 );
  if( page ) {
    for( i = 0; i < 0x20000; i++ ) {
      if( page[i] ) {
        failed = 1;
        break;
      }
    }
  } else {
    failed = 1;
  }

  return failed;
}

test_return_t
read_szx_snef_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test( "SNEF", snef_check );
}

static int
sner_check( libspectrum_snap *snap )
{
  int failed = 0;
  size_t i;

  libspectrum_byte *page = libspectrum_snap_spectranet_ram( snap, 0 );
  if( page ) {
    for( i = 0; i < 0x20000; i++ ) {
      if( page[i] ) {
        failed = 1;
        break;
      }
    }
  } else {
    failed = 1;
  }

  return failed;
}

test_return_t
read_szx_sner_chunk( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return szx_read_block_test( "SNER", sner_check );
}
