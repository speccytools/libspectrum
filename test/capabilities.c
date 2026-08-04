#include "config.h"

#include <stdio.h>
#include <string.h>

#include "test.h"

/* Helper: assert that a machine type has exactly the expected capabilities. */
static test_return_t
check_caps( const char *name,
            libspectrum_machine machine,
            int expected )
{
  int got = libspectrum_machine_capabilities( machine );
  if( got != expected ) {
    fprintf( stderr,
             "%s: machine_capabilities %s: expected 0x%04x, got 0x%04x\n",
             progname, name, expected, got );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* 16K and 48K machines have no special capabilities. */
test_return_t
machine_capabilities_16k_and_48k_have_no_capabilities( void )
{
  if( check_caps( "16K",  LIBSPECTRUM_MACHINE_16, 0 ) != TEST_PASS )
    return TEST_FAIL;
  if( check_caps( "48K",  LIBSPECTRUM_MACHINE_48, 0 ) != TEST_PASS )
    return TEST_FAIL;
  if( check_caps( "UNKNOWN", LIBSPECTRUM_MACHINE_UNKNOWN, 0 ) != TEST_PASS )
    return TEST_FAIL;
  return TEST_PASS;
}

/* 48K NTSC has only the NTSC capability. */
test_return_t
machine_capabilities_48k_ntsc_has_ntsc_only( void )
{
  return check_caps(
    "48K NTSC", LIBSPECTRUM_MACHINE_48_NTSC,
    LIBSPECTRUM_MACHINE_CAPABILITY_NTSC );
}

/* Timex TC2048 has Timex memory paging, Timex video modes,
   and Kempston joystick. */
test_return_t
machine_capabilities_tc2048_has_timex_and_kempston( void )
{
  return check_caps(
    "TC2048", LIBSPECTRUM_MACHINE_TC2048,
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_MEMORY   |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_VIDEO    |
    LIBSPECTRUM_MACHINE_CAPABILITY_KEMPSTON_JOYSTICK );
}

/* Timex TC2068 has AY, Timex memory/video, and cartridge dock. */
test_return_t
machine_capabilities_tc2068_has_ay_timex_and_dock( void )
{
  return check_caps(
    "TC2068", LIBSPECTRUM_MACHINE_TC2068,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_MEMORY |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_VIDEO  |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_DOCK );
}

/* Timex TS2068 has AY, Timex memory/video, cartridge dock, and NTSC. */
test_return_t
machine_capabilities_ts2068_has_ay_timex_dock_and_ntsc( void )
{
  return check_caps(
    "TS2068", LIBSPECTRUM_MACHINE_TS2068,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_MEMORY |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_VIDEO  |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_DOCK   |
    LIBSPECTRUM_MACHINE_CAPABILITY_NTSC );
}

/* 128K has AY and 128-style memory paging. */
test_return_t
machine_capabilities_128k_has_ay_and_128_memory( void )
{
  return check_caps(
    "128K", LIBSPECTRUM_MACHINE_128,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY );
}

/* Spectrum +2 adds Sinclair joystick to the 128K feature set. */
test_return_t
machine_capabilities_plus2_has_ay_128_memory_and_sinclair_joystick( void )
{
  return check_caps(
    "+2", LIBSPECTRUM_MACHINE_PLUS2,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY                   |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_SINCLAIR_JOYSTICK );
}

/* Spectrum +2A adds +3-style memory paging and Sinclair joystick. */
test_return_t
machine_capabilities_plus2a_has_ay_128_plus3_memory_and_sinclair_joystick( void )
{
  return check_caps(
    "+2A", LIBSPECTRUM_MACHINE_PLUS2A,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY                   |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_PLUS3_MEMORY         |
    LIBSPECTRUM_MACHINE_CAPABILITY_SINCLAIR_JOYSTICK );
}

/* Spectrum +3 and +3e add disk support on top of +2A. */
test_return_t
machine_capabilities_plus3_and_plus3e_have_full_plus3_capabilities( void )
{
  int expected = LIBSPECTRUM_MACHINE_CAPABILITY_AY                   |
                 LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY           |
                 LIBSPECTRUM_MACHINE_CAPABILITY_PLUS3_MEMORY         |
                 LIBSPECTRUM_MACHINE_CAPABILITY_PLUS3_DISK           |
                 LIBSPECTRUM_MACHINE_CAPABILITY_SINCLAIR_JOYSTICK;
  if( check_caps( "+3",  LIBSPECTRUM_MACHINE_PLUS3,  expected ) != TEST_PASS )
    return TEST_FAIL;
  if( check_caps( "+3e", LIBSPECTRUM_MACHINE_PLUS3E, expected ) != TEST_PASS )
    return TEST_FAIL;
  return TEST_PASS;
}

/* 128Ke has the same capabilities as +2A. */
test_return_t
machine_capabilities_128e_has_ay_128_plus3_memory_and_sinclair_joystick( void )
{
  return check_caps(
    "128Ke", LIBSPECTRUM_MACHINE_128E,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY                   |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_PLUS3_MEMORY         |
    LIBSPECTRUM_MACHINE_CAPABILITY_SINCLAIR_JOYSTICK );
}

/* Pentagon 128K has AY, 128-style memory, and TRDOS disk. */
test_return_t
machine_capabilities_pent_has_ay_128_memory_and_trdos( void )
{
  return check_caps(
    "Pentagon 128K", LIBSPECTRUM_MACHINE_PENT,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY   |
    LIBSPECTRUM_MACHINE_CAPABILITY_TRDOS_DISK );
}

/* Pentagon 512K adds Pentagon 512-style memory paging. */
test_return_t
machine_capabilities_pent512_has_ay_128_trdos_and_pent512_memory( void )
{
  return check_caps(
    "Pentagon 512K", LIBSPECTRUM_MACHINE_PENT512,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY             |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY     |
    LIBSPECTRUM_MACHINE_CAPABILITY_TRDOS_DISK     |
    LIBSPECTRUM_MACHINE_CAPABILITY_PENT512_MEMORY );
}

/* Pentagon 1024K adds Pentagon 1024-style memory paging. */
test_return_t
machine_capabilities_pent1024_has_ay_128_trdos_pent512_and_pent1024_memory( void )
{
  return check_caps(
    "Pentagon 1024K", LIBSPECTRUM_MACHINE_PENT1024,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY              |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY      |
    LIBSPECTRUM_MACHINE_CAPABILITY_TRDOS_DISK      |
    LIBSPECTRUM_MACHINE_CAPABILITY_PENT512_MEMORY  |
    LIBSPECTRUM_MACHINE_CAPABILITY_PENT1024_MEMORY );
}

/* Scorpion has AY, 128-style memory, TRDOS disk, Scorpion memory, and even M1. */
test_return_t
machine_capabilities_scorp_has_ay_128_trdos_scorp_memory_and_even_m1( void )
{
  return check_caps(
    "Scorpion", LIBSPECTRUM_MACHINE_SCORP,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY           |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY   |
    LIBSPECTRUM_MACHINE_CAPABILITY_TRDOS_DISK   |
    LIBSPECTRUM_MACHINE_CAPABILITY_SCORP_MEMORY |
    LIBSPECTRUM_MACHINE_CAPABILITY_EVEN_M1 );
}

/* Spectrum SE has AY, 128-style memory, Timex video, Kempston joystick,
   and SE memory paging. */
test_return_t
machine_capabilities_se_has_ay_128_memory_timex_video_kempston_and_se_memory( void )
{
  return check_caps(
    "SE", LIBSPECTRUM_MACHINE_SE,
    LIBSPECTRUM_MACHINE_CAPABILITY_AY                |
    LIBSPECTRUM_MACHINE_CAPABILITY_128_MEMORY        |
    LIBSPECTRUM_MACHINE_CAPABILITY_TIMEX_VIDEO       |
    LIBSPECTRUM_MACHINE_CAPABILITY_KEMPSTON_JOYSTICK |
    LIBSPECTRUM_MACHINE_CAPABILITY_SE_MEMORY );
}

/* Helper: assert that libspectrum_machine_name returns the expected string. */
static test_return_t
check_machine_name( libspectrum_machine machine, const char *expected )
{
  const char *got = libspectrum_machine_name( machine );
  if( strcmp( got, expected ) != 0 ) {
    fprintf( stderr,
             "%s: machine_name(%d): expected \"%s\", got \"%s\"\n",
             progname, (int)machine, expected, got );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* libspectrum_machine_name returns the correct string for every machine. */
test_return_t
machine_name_returns_correct_string_for_all_machines( void )
{
  if( check_machine_name( LIBSPECTRUM_MACHINE_16,       "Spectrum 16K"         ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_48,       "Spectrum 48K"         ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_48_NTSC,  "Spectrum 48K (NTSC)"  ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_TC2048,   "Timex TC2048"         ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_TC2068,   "Timex TC2068"         ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_TS2068,   "Timex TS2068"         ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_128,      "Spectrum 128K"        ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_128E,     "Spectrum 128Ke"       ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_PLUS2,    "Spectrum +2"          ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_PLUS2A,   "Spectrum +2A"         ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_PLUS3,    "Spectrum +3"          ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_PLUS3E,   "Spectrum +3e"         ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_PENT,     "Pentagon 128K"        ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_PENT512,  "Pentagon 512K"        ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_PENT1024, "Pentagon 1024K"       ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_SCORP,    "Scorpion ZS 256"      ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_SE,       "Spectrum SE"          ) != TEST_PASS ) return TEST_FAIL;
  if( check_machine_name( LIBSPECTRUM_MACHINE_UNKNOWN,  "(unknown)"            ) != TEST_PASS ) return TEST_FAIL;
  return TEST_PASS;
}

/* Helper: assert that libspectrum_joystick_name returns the expected string. */
static test_return_t
check_joystick_name( libspectrum_joystick type, const char *expected )
{
  const char *got = libspectrum_joystick_name( type );
  if( strcmp( got, expected ) != 0 ) {
    fprintf( stderr,
             "%s: joystick_name(%d): expected \"%s\", got \"%s\"\n",
             progname, (int)type, expected, got );
    return TEST_FAIL;
  }
  return TEST_PASS;
}

/* libspectrum_joystick_name returns the correct string for every joystick. */
test_return_t
joystick_name_returns_correct_string_for_all_types( void )
{
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_NONE,       "(None)"      ) != TEST_PASS ) return TEST_FAIL;
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_CURSOR,     "Cursor"      ) != TEST_PASS ) return TEST_FAIL;
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_KEMPSTON,   "Kempston"    ) != TEST_PASS ) return TEST_FAIL;
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_SINCLAIR_1, "Sinclair 1"  ) != TEST_PASS ) return TEST_FAIL;
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_SINCLAIR_2, "Sinclair 2"  ) != TEST_PASS ) return TEST_FAIL;
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_TIMEX_1,    "Timex 1"     ) != TEST_PASS ) return TEST_FAIL;
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_TIMEX_2,    "Timex 2"     ) != TEST_PASS ) return TEST_FAIL;
  if( check_joystick_name( LIBSPECTRUM_JOYSTICK_FULLER,     "Fuller"      ) != TEST_PASS ) return TEST_FAIL;
  return TEST_PASS;
}
