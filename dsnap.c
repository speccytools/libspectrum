/* dsnap.c:  [UTF-8 file]
   Routines for handling Didaktik D40/D80 snapshot files (when extracted from floppy image).
   D40's snapshot creates files on disk with name like SNAPSHOT00.S .. SNAPSHOT99.S .
   Thou only 7 of them fit on a 40 track (D40) and 14 on an 80 track floppy (D80).
   
   Technical details of how the SNAP button works can be found in books:
   - "Komentovany vypis MDOSu" by Kvaksoft from 1995.
   - "Rutiny ROM D40" by Proxima from 1993

   Limitations, specs of this format:
   - Only supporting/saving 48KB spectrum/didaktik.
   - On real HW registers are stored into D40's DRAM (last 128 bytes), not affecting standard RAM.
   - Size of file is always #C080 (49280), starting address is #3F80 (16256).
   - Does not save register "R". IFF1/IFF2 considered together. IM0 not supported.
   - Data is stored into the head (before 16K) as stack. SP address first, followed by registers.
   - Order of saving registers: comments below
   - You can use the app RIDE v.1.4.3 to export and import files from and into .d40 floppy images.
     Just make sure to have at least "R" attribute for newly imported file(s), or it will not work.

   Written by (c) 2022 Miroslav Ďurčík (miro.arki55@gmail.com)
   (with some code lines copied from other files in this lib)
   
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

   Author (of FUSE in general) contact information:

   E-mail: philip-fuse@shadowmagic.org.uk
*/

#include "config.h"

#include <string.h>

#include "internals.h"

/* Parity flag in F reg. */
#define FLAG_P	0x04

/* Length = 128 bytes + 48K */
#define LIBSPECTRUM_DSNAP_HEADER_LENGTH 128

/* File header - to mark snapshot created by FUSE from real HW */
const char * const LIBSPECTRUM_DSNAP_SIGNATURE = "DSNAP@FUSE";

static int identify_machine( size_t buffer_length, libspectrum_snap *snap );
static int libspectrum_dsnap_read_header( const libspectrum_byte *buffer,
					size_t buffer_length,
					libspectrum_snap *snap );
static int libspectrum_dsnap_read_data( const libspectrum_byte *buffer,
				      size_t buffer_length,
				      libspectrum_snap *snap );

static void
write_header( libspectrum_buffer *buffer, libspectrum_snap *snap,
              libspectrum_word sp );
static libspectrum_error
write_48k_dsnap( libspectrum_buffer *buffer, libspectrum_snap *snap,
               libspectrum_word *new_sp );
static void
write_page( libspectrum_buffer *buffer, libspectrum_snap *snap, int page );

libspectrum_error
internal_dsnap_read( libspectrum_snap *snap,
		   const libspectrum_byte *buffer, size_t buffer_length )
{
  int error;

  error = identify_machine( buffer_length, snap );
  if( error != LIBSPECTRUM_ERROR_NONE ) return error;

  error = libspectrum_dsnap_read_header( buffer, buffer_length, snap );
  if( error != LIBSPECTRUM_ERROR_NONE ) return error;

  error = libspectrum_dsnap_read_data(
    &buffer[LIBSPECTRUM_DSNAP_HEADER_LENGTH],
    buffer_length - LIBSPECTRUM_DSNAP_HEADER_LENGTH, snap );
  if( error != LIBSPECTRUM_ERROR_NONE ) return error;

  return LIBSPECTRUM_ERROR_NONE;
}

static int
identify_machine( size_t buffer_length, libspectrum_snap *snap )
{
  switch( buffer_length ) {
  case 49280:
    libspectrum_snap_set_machine( snap, LIBSPECTRUM_MACHINE_48 );
    break;
  default:
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "libspectrum_dsnap_identify: Only 48K machines supported. Size must be 49280." );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  return LIBSPECTRUM_ERROR_NONE;
}

/**
 * Read header part (128B) of Didaktik's snapshot.
 * Registers are stored like stack from 16K-2 downwards:
 * SP, AF, BC, DE, HL, (EXX, EX AF,AF') AF, BC, DE, HL, IX, IY, IReg
 */
static int
libspectrum_dsnap_read_header( const libspectrum_byte *buffer,
			     size_t buffer_length, libspectrum_snap *snap )
{
  if( buffer_length < LIBSPECTRUM_DSNAP_HEADER_LENGTH ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "libspectrum_dsnap_read_header: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Program's SP is at end of header */
  libspectrum_word offset = LIBSPECTRUM_DSNAP_HEADER_LENGTH;
  libspectrum_snap_set_sp ( snap, buffer[offset-2] + buffer[offset-2 +1]*0x100 );
  
  /* Current registers first */
  libspectrum_snap_set_a  ( snap, buffer[offset-4 +1] );
  libspectrum_snap_set_f  ( snap, buffer[offset-4 +0] );
  libspectrum_snap_set_bc ( snap, buffer[offset-6] + buffer[offset-6 +1]*0x100 );
  libspectrum_snap_set_de ( snap, buffer[offset-8] + buffer[offset-8 +1]*0x100 );
  libspectrum_snap_set_hl ( snap, buffer[offset-10] + buffer[offset-10 +1]*0x100 );
  
  /* Shadow registers next */
  libspectrum_snap_set_a_ ( snap, buffer[offset-12 +1] );
  libspectrum_snap_set_f_ ( snap, buffer[offset-12 +0] );
  libspectrum_snap_set_bc_( snap, buffer[offset-14] + buffer[offset-14 +1]*0x100 );
  libspectrum_snap_set_de_( snap, buffer[offset-16] + buffer[offset-16 +1]*0x100 );
  libspectrum_snap_set_hl_( snap, buffer[offset-18] + buffer[offset-18 +1]*0x100 );

  /* IX, IY only once, have no shadow copies */
  libspectrum_snap_set_ix ( snap, buffer[offset-20] + buffer[offset-20 +1]*0x100 );
  libspectrum_snap_set_iy ( snap, buffer[offset-22] + buffer[offset-22 +1]*0x100 );

  /* The last one is I register, interrupt vector + if to use IM1 or IM2 + DI/EI
     Note: "AF" Word is read from -24 . F is checked for "PE" condition,
           then A is copied to I and compared to 63 (operation changes F).
           Source of PE flag was operation LD A,I before which copies IFF2 to Parity. */
  libspectrum_byte interrupt_flag1 = buffer[offset-24];     /* F */
  libspectrum_byte interrupt_flag2 = buffer[offset-24 +1];  /* A */
  
  libspectrum_snap_set_i  ( snap, interrupt_flag2 );

  if (interrupt_flag2 == 63) {
    /* Not IM 2 */
    libspectrum_snap_set_im( snap, 1 );
  } else {
    /* It is IM 2 */
    libspectrum_snap_set_im( snap, 2 );
  }

  if ( interrupt_flag1 & FLAG_P ) {
    /* Allow interrupts ( PE condition flag on F ) */
    libspectrum_snap_set_iff1( snap, 1);
    libspectrum_snap_set_iff2( snap, 1);
  } else {
    /* I would assume default is 0, but debugging showed other wise */
    libspectrum_snap_set_iff1( snap, 0);
    libspectrum_snap_set_iff2( snap, 0);
  }

  /* Activate melodik. No idea how to make it optional. */
  libspectrum_snap_set_melodik_active( snap, 1 );
  
  /*
    PC is in loaded program's stack - will be set when data is loaded
    libspectrum_snap_set_pc ( snap, 0 );

   Not supported by this format:
     libspectrum_snap_set_r ( snap, xxx );
   */

  return LIBSPECTRUM_ERROR_NONE;
}

/**
 * Read data part of Didaktik's snapshot. Whole 48Kb.
 */
static int
libspectrum_dsnap_read_data( const libspectrum_byte *buffer,
			   size_t buffer_length, libspectrum_snap *snap )
{
  int error;
  libspectrum_word sp, offset;

  /* Standard 48K (0xC000) is to be read */
  if( buffer_length < 0xc000 ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "libspectrum_dsnap_read_data: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  switch( libspectrum_snap_machine( snap ) ) {

  case LIBSPECTRUM_MACHINE_48:

    sp = libspectrum_snap_sp( snap );
    if( sp < 0x4000 || sp == 0xffff ) {
      libspectrum_print_error(
        LIBSPECTRUM_ERROR_CORRUPT,
        "libspectrum_dsnap_read_data: SP invalid (0x%04x)", sp
      );
      return LIBSPECTRUM_ERROR_CORRUPT;
    }

    /* Rescue PC from the stack */
    offset = sp - 0x4000;
    libspectrum_snap_set_pc( snap, buffer[offset] + 0x100 * buffer[offset+1] );

    /* Increase SP as PC has been unstacked */
    libspectrum_snap_set_sp( snap, libspectrum_snap_sp( snap ) + 2 );

    /* And split the pages up */
    error = libspectrum_split_to_48k_pages( snap, buffer );
    if( error != LIBSPECTRUM_ERROR_NONE ) return error;

    break;

  default:
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "libspectrum_dsnap_read_data: only 48k machine supported" );
    return LIBSPECTRUM_ERROR_LOGIC;
  }
  
  return LIBSPECTRUM_ERROR_NONE;
}

/**
 * Create snapshot compatible with Didaktik's snap button.
 */
libspectrum_error
libspectrum_dsnap_write( libspectrum_buffer *buffer, int *out_flags,
                       libspectrum_snap *snap, int in_flags GCC_UNUSED )
{
  libspectrum_word snap_sp;
  libspectrum_buffer *buffer_mem;
  libspectrum_error error = LIBSPECTRUM_ERROR_NONE;

  /* Minor info loss already due to "R" register missing, etc.. */
  *out_flags = LIBSPECTRUM_FLAG_SNAPSHOT_MINOR_INFO_LOSS;

  /* We don't store +D info at all */
  if( libspectrum_snap_plusd_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't store Beta info at all */
  if( libspectrum_snap_beta_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't store Opus info at all */
  if( libspectrum_snap_opus_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save IDE interface info at all */
  if( libspectrum_snap_zxatasp_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;
  if( libspectrum_snap_zxcf_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;
  if( libspectrum_snap_simpleide_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;
  if( libspectrum_snap_divide_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the Interface 2 ROM at all */
  if( libspectrum_snap_interface2_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the Timex Dock at all */
  if( libspectrum_snap_dock_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save custom ROMs at all */
  if( libspectrum_snap_custom_rom( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save AY interfaces  at all */
  if( libspectrum_snap_fuller_box_active( snap ) ||
      libspectrum_snap_melodik_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the Specdrum state at all */
  if( libspectrum_snap_specdrum_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the Spectranet state at all */
  if( libspectrum_snap_spectranet_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the uSource state at all */
  if( libspectrum_snap_usource_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the uSpeech state at all */
  if( libspectrum_snap_uspeech_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the DISCiPLE state at all */
  if( libspectrum_snap_disciple_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save complete Didaktik80 state (2KB). D40's snapshot contains 
     last 128 bytes of FDD's DRAM, only for sake of saving registers. */
  if( libspectrum_snap_didaktik80_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the Covox state at all */
  if( libspectrum_snap_covox_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the Multiface state at all */
  if( libspectrum_snap_multiface_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the MMC state at all */
  if( libspectrum_snap_divmmc_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;
  if( libspectrum_snap_zxmmc_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  /* We don't save the TTX2000S state at all */
  if( libspectrum_snap_ttx2000s_active( snap ) )
    *out_flags |= LIBSPECTRUM_FLAG_SNAPSHOT_MAJOR_INFO_LOSS;

  buffer_mem = libspectrum_buffer_alloc();

  switch( libspectrum_snap_machine( snap ) ) {
  case LIBSPECTRUM_MACHINE_48_NTSC:
  case LIBSPECTRUM_MACHINE_48:
    /* Only 48K machine supported */
    error = write_48k_dsnap( buffer_mem, snap, &snap_sp );
    break;
    
  case LIBSPECTRUM_MACHINE_16:
  case LIBSPECTRUM_MACHINE_TC2048:
  case LIBSPECTRUM_MACHINE_TC2068:
  case LIBSPECTRUM_MACHINE_TS2068:
  case LIBSPECTRUM_MACHINE_128:
  case LIBSPECTRUM_MACHINE_128E:
  case LIBSPECTRUM_MACHINE_PENT:
  case LIBSPECTRUM_MACHINE_PENT512:
  case LIBSPECTRUM_MACHINE_PENT1024:
  case LIBSPECTRUM_MACHINE_PLUS2:
  case LIBSPECTRUM_MACHINE_PLUS2A:
  case LIBSPECTRUM_MACHINE_PLUS3:
  case LIBSPECTRUM_MACHINE_PLUS3E:
  case LIBSPECTRUM_MACHINE_SCORP:
  case LIBSPECTRUM_MACHINE_SE:
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "Only 48k machine supported by Didaktik SNAP format!" );
    libspectrum_buffer_free( buffer_mem );
    return LIBSPECTRUM_ERROR_LOGIC;

  default:
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "Emulated machine type is set to 'unknown'!" );
    libspectrum_buffer_free( buffer_mem );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  if( error ) {
    libspectrum_buffer_free( buffer_mem );
    return error;
  }

  write_header( buffer, snap, snap_sp );
  libspectrum_buffer_write_buffer( buffer, buffer_mem );
  libspectrum_buffer_free( buffer_mem );

  return LIBSPECTRUM_ERROR_NONE;
}

/**
 * Write header part of Didaktik's snapshot (128B).
 * Registers are stored like stack from 16K-2 downwards:
 * SP, AF, BC, DE, HL, (swap regs) AF, BC, DE, HL, IX, IY, I+EI/DI
 */
static void
write_header( libspectrum_buffer *buffer, libspectrum_snap *snap,
              libspectrum_word sp )
{
  short padding;
  short b;

  /* Header has 128B, but only last 24 bytes are used. Content before is irrelevant.
     Less by 10 chars - signature to flag snaphost created by fuse */
  libspectrum_buffer_write( buffer, LIBSPECTRUM_DSNAP_SIGNATURE, strlen(LIBSPECTRUM_DSNAP_SIGNATURE) );
  padding = LIBSPECTRUM_DSNAP_HEADER_LENGTH - 24 - strlen(LIBSPECTRUM_DSNAP_SIGNATURE);
  for( b=0; b < padding; b++ ) {
    libspectrum_buffer_write_byte( buffer, 0 );
  }

  /* The last one (when pushing into stack from the end of header) is I register,
     interrupt vector + if to use IM1 or IM2 + DI/EI

     If interrupts allowed (EI), then write 4 ("PE" bit on F reg.), else just 0.
     Source of PE is operation LD A,I which sets IFF2 to Parity flag. */
  if ( libspectrum_snap_iff2 ( snap ) == 1 ) {
    libspectrum_buffer_write_byte( buffer, FLAG_P );
  } else {
    libspectrum_buffer_write_byte( buffer, 0 );
  }

  /* If mode 2: I reg. is written, else "63" + mode 1 assumed */
  if ( libspectrum_snap_im( snap ) == 0x02 ) {
    libspectrum_buffer_write_byte( buffer, libspectrum_snap_i ( snap ) ); 
  } else {
    libspectrum_buffer_write_byte( buffer, 63 ); 
  }
  

  /* IX, IY only once, have no shadow copies */
  libspectrum_buffer_write_word( buffer, libspectrum_snap_iy ( snap ) );
  libspectrum_buffer_write_word( buffer, libspectrum_snap_ix ( snap ) );

  /* Shadow registers next */
  libspectrum_buffer_write_word( buffer, libspectrum_snap_hl_ ( snap ) );
  libspectrum_buffer_write_word( buffer, libspectrum_snap_de_ ( snap ) );
  libspectrum_buffer_write_word( buffer, libspectrum_snap_bc_ ( snap ) );
  libspectrum_buffer_write_byte( buffer, libspectrum_snap_f_  ( snap ) );
  libspectrum_buffer_write_byte( buffer, libspectrum_snap_a_  ( snap ) );

  /* Current registers */
  libspectrum_buffer_write_word( buffer, libspectrum_snap_hl ( snap ) );
  libspectrum_buffer_write_word( buffer, libspectrum_snap_de ( snap ) );
  libspectrum_buffer_write_word( buffer, libspectrum_snap_bc ( snap ) );
  libspectrum_buffer_write_byte( buffer, libspectrum_snap_f  ( snap ) );
  libspectrum_buffer_write_byte( buffer, libspectrum_snap_a  ( snap ) );

  /* Program's SP is at end of header */
  libspectrum_buffer_write_word( buffer, sp );

  /* 
    PC is stored into snapp'ed app's stack in function write_48k_dsnap()
    libspectrum_snap_pc ( snap ); 

    Not supported by this format:
     libspectrum_buffer_write_byte( buffer, libspectrum_snap_r ( snap ) ); 
   */
}

static libspectrum_error
write_48k_dsnap( libspectrum_buffer *buffer, libspectrum_snap *snap,
               libspectrum_word *new_sp )
{
  libspectrum_byte *stack, *memory_base;
  size_t offset;
  libspectrum_word new_stack_address;

  /* Must have somewhere in RAM to store PC */
  if( libspectrum_snap_sp( snap ) < 0x4002 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_INVALID,
			     "SP is too low (0x%04x) to stack PC",
			     libspectrum_snap_sp( snap ) );
    return LIBSPECTRUM_ERROR_INVALID;
  }

  offset = libspectrum_buffer_get_data_size( buffer );

  write_page( buffer, snap, 5 );
  write_page( buffer, snap, 2 );
  write_page( buffer, snap, 0 );

  memory_base = libspectrum_buffer_get_data( buffer ) + offset;

  /* Place PC on the stack */
  new_stack_address = libspectrum_snap_sp( snap ) - 2;
  stack = &( memory_base[ new_stack_address - 0x4000 ] );
  libspectrum_write_word( &stack, libspectrum_snap_pc( snap ) );

  /* Store the new value of SP */
  *new_sp = new_stack_address;

  return LIBSPECTRUM_ERROR_NONE;
}


static void
write_page( libspectrum_buffer *buffer, libspectrum_snap *snap, int page )
{
  libspectrum_byte *ram;

  ram = libspectrum_snap_pages( snap, page );
  if( ram ) {
    libspectrum_buffer_write( buffer, ram, 0x4000 );
  } else {
    libspectrum_buffer_set( buffer, 0xff, 0x4000 );
  }
}
