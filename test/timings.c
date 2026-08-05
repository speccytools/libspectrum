/* timings.c: unit tests for the libspectrum_timings_* API
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

#include "test.h"

/* Helper: check one timing value and report mismatch. */
static test_return_t
check_timing( const char *machine_name, const char *field,
              libspectrum_dword expected, libspectrum_dword got )
{
  if( got != expected ) {
    fprintf( stderr,
             "%s: timings %s %s: expected %lu, got %lu\n",
             progname, machine_name, field,
             (unsigned long)expected, (unsigned long)got );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* 48K machine: 3.5 MHz processor, no AY, Ferranti 5C/6C frame timings.
   Line = 24+128+24+48 = 224 T-states; frame = 312 lines = 69888 T-states. */
test_return_t
timings_48k_processor_speed_and_frame_timing( void )
{
  if( check_timing( "48K", "processor_speed",
                    3500000,
                    libspectrum_timings_processor_speed( LIBSPECTRUM_MACHINE_48 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "48K", "ay_speed",
                    0,
                    libspectrum_timings_ay_speed( LIBSPECTRUM_MACHINE_48 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "48K", "tstates_per_line",
                    224,
                    libspectrum_timings_tstates_per_line( LIBSPECTRUM_MACHINE_48 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "48K", "lines_per_frame",
                    312,
                    libspectrum_timings_lines_per_frame( LIBSPECTRUM_MACHINE_48 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "48K", "tstates_per_frame",
                    69888,
                    libspectrum_timings_tstates_per_frame( LIBSPECTRUM_MACHINE_48 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "48K", "interrupt_length",
                    32,
                    libspectrum_timings_interrupt_length( LIBSPECTRUM_MACHINE_48 ) )
      != TEST_PASS ) return TEST_FAIL;

  return TEST_PASS;
}

/* 128K machine: 3.5469 MHz (7C ULA), AY at ~1.77 MHz, Ferranti 7C timings.
   Line = 24+128+24+52 = 228; frame = 48+192+48+23 = 311 lines = 70908 T-states. */
test_return_t
timings_128k_processor_speed_and_frame_timing( void )
{
  if( check_timing( "128K", "processor_speed",
                    3546900,
                    libspectrum_timings_processor_speed( LIBSPECTRUM_MACHINE_128 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "128K", "ay_speed",
                    1773400,
                    libspectrum_timings_ay_speed( LIBSPECTRUM_MACHINE_128 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "128K", "tstates_per_line",
                    228,
                    libspectrum_timings_tstates_per_line( LIBSPECTRUM_MACHINE_128 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "128K", "lines_per_frame",
                    311,
                    libspectrum_timings_lines_per_frame( LIBSPECTRUM_MACHINE_128 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "128K", "tstates_per_frame",
                    70908,
                    libspectrum_timings_tstates_per_frame( LIBSPECTRUM_MACHINE_128 ) )
      != TEST_PASS ) return TEST_FAIL;

  return TEST_PASS;
}

/* Pentagon 128K: 3.584 MHz, AY at 1.792 MHz, Pentagon frame timings.
   Line = 36+128+28+32 = 224; frame = 64+192+48+16 = 320 lines = 71680 T-states. */
test_return_t
timings_pentagon_processor_speed_and_frame_timing( void )
{
  if( check_timing( "Pentagon", "processor_speed",
                    3584000,
                    libspectrum_timings_processor_speed( LIBSPECTRUM_MACHINE_PENT ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "Pentagon", "ay_speed",
                    1792000,
                    libspectrum_timings_ay_speed( LIBSPECTRUM_MACHINE_PENT ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "Pentagon", "tstates_per_line",
                    224,
                    libspectrum_timings_tstates_per_line( LIBSPECTRUM_MACHINE_PENT ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "Pentagon", "lines_per_frame",
                    320,
                    libspectrum_timings_lines_per_frame( LIBSPECTRUM_MACHINE_PENT ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "Pentagon", "tstates_per_frame",
                    71680,
                    libspectrum_timings_tstates_per_frame( LIBSPECTRUM_MACHINE_PENT ) )
      != TEST_PASS ) return TEST_FAIL;

  return TEST_PASS;
}

/* TS2068: 3.528 MHz, AY at 1.764 MHz, 60 Hz Timex SCLD frame.
   Line = 24+128+24+48 = 224; frame = 24+192+25+21 = 262 lines = 58688 T-states. */
test_return_t
timings_ts2068_processor_speed_and_frame_timing( void )
{
  if( check_timing( "TS2068", "processor_speed",
                    3528000,
                    libspectrum_timings_processor_speed( LIBSPECTRUM_MACHINE_TS2068 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "TS2068", "ay_speed",
                    1764000,
                    libspectrum_timings_ay_speed( LIBSPECTRUM_MACHINE_TS2068 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "TS2068", "tstates_per_line",
                    224,
                    libspectrum_timings_tstates_per_line( LIBSPECTRUM_MACHINE_TS2068 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "TS2068", "lines_per_frame",
                    262,
                    libspectrum_timings_lines_per_frame( LIBSPECTRUM_MACHINE_TS2068 ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "TS2068", "tstates_per_frame",
                    58688,
                    libspectrum_timings_tstates_per_frame( LIBSPECTRUM_MACHINE_TS2068 ) )
      != TEST_PASS ) return TEST_FAIL;

  return TEST_PASS;
}

/* UNKNOWN machine: all timing accessors must return 0 (no timings defined). */
test_return_t
timings_unknown_machine_returns_zero_for_all_frame_timings( void )
{
  if( check_timing( "UNKNOWN", "processor_speed",
                    0,
                    libspectrum_timings_processor_speed( LIBSPECTRUM_MACHINE_UNKNOWN ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "UNKNOWN", "ay_speed",
                    0,
                    libspectrum_timings_ay_speed( LIBSPECTRUM_MACHINE_UNKNOWN ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "UNKNOWN", "tstates_per_line",
                    0,
                    libspectrum_timings_tstates_per_line( LIBSPECTRUM_MACHINE_UNKNOWN ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "UNKNOWN", "lines_per_frame",
                    0,
                    libspectrum_timings_lines_per_frame( LIBSPECTRUM_MACHINE_UNKNOWN ) )
      != TEST_PASS ) return TEST_FAIL;

  if( check_timing( "UNKNOWN", "tstates_per_frame",
                    0,
                    libspectrum_timings_tstates_per_frame( LIBSPECTRUM_MACHINE_UNKNOWN ) )
      != TEST_PASS ) return TEST_FAIL;

  return TEST_PASS;
}
