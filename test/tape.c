#include "config.h"

#include <stdio.h>
#include <string.h>

#include "internals.h"
#include "common.h"
#include "test.h"

test_return_t
tape_with_unknown_block( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid.tzx" ), LIBSPECTRUM_ERROR_UNKNOWN );
}

/* Test for bugs #84: TZX turbo blocks with zero pilot pulses and #85: freeing
   a turbo block with no data produces segfault */
test_return_t
tzx_turbo_data_with_zero_pilot_pulses_and_zero_data( void )
{
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_tape *tape;
  const char *filename = STATIC_TEST_PATH( "turbo-zeropilot.tzx" );
  libspectrum_dword tstates;
  int flags;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  tape = libspectrum_tape_alloc();

  if( libspectrum_tape_read( tape, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
			     filename ) ) {
    libspectrum_tape_free( tape );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  if( libspectrum_tape_get_next_edge( &tstates, &flags, tape ) ) {
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  if( flags != LIBSPECTRUM_TAPE_FLAGS_LEVEL_LOW ) {
    fprintf( stderr,
             "%s: reading first edge of `%s' gave unexpected flags 0x%04x; expected 0x%04x\n",
	     progname, filename, flags, LIBSPECTRUM_TAPE_FLAGS_LEVEL_LOW );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( tstates != 667 ) {
    fprintf( stderr, "%s: first edge of `%s' was %u tstates; expected 667\n",
	     progname, filename, tstates );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

/* Test for bug #88: writing empty .tap file causes crash */
test_return_t
writing_empty_tap_file( void )
{
  libspectrum_tape *tape;
  libspectrum_byte *buffer = (libspectrum_byte*)1;
  size_t length = 0;

  tape = libspectrum_tape_alloc();

  if( libspectrum_tape_write( &buffer, &length, tape, LIBSPECTRUM_ID_TAPE_TAP ) ) {
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  /* `buffer' should now have been set to NULL */
  if( buffer ) {
    fprintf( stderr, "%s: `buffer' was not NULL after libspectrum_tape_write()\n", progname );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

/* Test for bug #105: lack of sanity check in GDB code */
test_return_t
invalid_tzx_gdb( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-gdb.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #106: empty DRB causes segfault */
test_return_t
empty_tzx_drb( void )
{
  return read_tape( STATIC_TEST_PATH( "empty-drb.tzx" ), LIBSPECTRUM_ERROR_NONE );
}

/* Test for bug #107: problems with invalid archive info block */
test_return_t
invalid_tzx_archive_info_block( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-archiveinfo.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #108: invalid hardware info blocks can leak memory */
test_return_t
invalid_hardware_info_block_causes_memory_leak( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-hardwareinfo.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #111: invalid Warajevo tape block offset causes segfault */
test_return_t
invalid_warajevo_tape_file( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-warajevo-blockoffset.tap" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #112: invalid custom info block causes memory leak */
test_return_t
invalid_tzx_custom_info_block_causes_memory_leak( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-custominfo.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #113: loop end without a loop start block accesses uninitialised
   memory */
test_return_t
tzx_loop_end_block_with_loop_start_block( void )
{
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_tape *tape;
  const char *filename = STATIC_TEST_PATH( "loopend.tzx" );
  libspectrum_dword tstates;
  int flags;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  tape = libspectrum_tape_alloc();

  if( libspectrum_tape_read( tape, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
			     filename ) ) {
    libspectrum_tape_free( tape );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  if( libspectrum_tape_get_next_edge( &tstates, &flags, tape ) ) {
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

/* Test for bug #113: TZX loop blocks broken */
test_return_t
tzx_loop_blocks( void )
{
  return play_tape( STATIC_TEST_PATH( "loop.tzx" ) );
}

/* Test for bug #118: TZX loop blocks still broken */
test_return_t
tzx_loop_blocks_2( void )
{
  return play_tape( STATIC_TEST_PATH( "loop2.tzx" ) );
}

/* Test for bug #119: TZX jump blocks broken */
test_return_t
tzx_jump_blocks( void )
{
  return play_tape( STATIC_TEST_PATH( "jump.tzx" ) );
}

/* Test for bug #121: crashes writing and reading empty CSW files */
test_return_t
csw_empty_file( void )
{
  return play_tape( STATIC_TEST_PATH( "empty.csw" ) );
}

/* Test for bug #125: .tap writing code does not handle all block types */
test_return_t
complete_tzx_to_tap_conversion( void )
{
  libspectrum_byte *buffer = NULL;
  size_t length = 0;
  libspectrum_tape *tape;
  const char *filename = DYNAMIC_TEST_PATH( "complete-tzx.tzx" );
  test_return_t r;

  r = load_tape( &tape, filename, LIBSPECTRUM_ERROR_NONE );
  if( r ) return r;

  if( libspectrum_tape_write( &buffer, &length, tape,
                              LIBSPECTRUM_ID_TAPE_TAP ) ) {
    fprintf( stderr, "%s: writing `%s' to a .tap file was not successful\n",
             progname, filename );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

test_return_t
complete_tzx_timings( void )
{
  const char *filename = DYNAMIC_TEST_PATH( "complete-tzx.tzx" );
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_tape *tape;
  libspectrum_tape_iterator it;
  libspectrum_tape_block *block;
  libspectrum_dword expected_sizes[20] = {
    15216886,	/* ROM */
    3493371,	/* Turbo */
    356310,	/* Pure tone */
    1761,	/* Pulses */
    1993724,	/* Pure data */
    2163000,	/* Pause */
    0,		/* Group start */
    0,		/* Group end */
    0,		/* Jump */
    205434,	/* Pure tone */
    0,		/* Loop start */
    154845,	/* Pure tone */
    0,		/* Loop end */
    0,		/* Stop tape if in 48K mode */
    0,		/* Comment */
    0,		/* Message */
    0,		/* Archive info */
    0,		/* Hardware */
    0,		/* Custom info */
    771620,	/* Pure tone */
  };
  libspectrum_dword *next_size = &expected_sizes[ 0 ];
  test_return_t r = TEST_PASS;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  tape = libspectrum_tape_alloc();

  if( libspectrum_tape_read( tape, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
			     filename ) ) {
    libspectrum_tape_free( tape );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  block = libspectrum_tape_iterator_init( &it, tape );

  while( block )
  {
    libspectrum_dword actual_size = libspectrum_tape_block_length( block );

    if( actual_size != *next_size )
    {
      fprintf( stderr, "%s: block had length %lu, but expected %lu\n", progname, (unsigned long)actual_size, (unsigned long)*next_size );
      r = TEST_FAIL;
      break;
    }

    block = libspectrum_tape_iterator_next( &it );
    next_size++;
  }

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return r;
}

/* Test for bug #379: converting .tap file to .csw causes crash */
test_return_t
csw_conversion( void )
{
  libspectrum_byte *buffer = NULL;
  size_t length = 0;
  libspectrum_tape *tape;
  const char *filename = STATIC_TEST_PATH( "standard-tap.tap" );
  test_return_t r;

  r = load_tape( &tape, filename, LIBSPECTRUM_ERROR_NONE );
  if( r ) return r;

  if( libspectrum_tape_write( &buffer, &length, tape,
                              LIBSPECTRUM_ID_TAPE_CSW ) ) {
    fprintf( stderr, "%s: writing `%s' to a .csw file was not successful\n",
             progname, filename );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

test_return_t
tape_peek_next_block( void )
{
  const char *filename = DYNAMIC_TEST_PATH( "complete-tzx.tzx" );
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_tape *tape;
  libspectrum_tape_iterator it;
  libspectrum_tape_type expected_next_block_types[19] = {
    LIBSPECTRUM_TAPE_BLOCK_TURBO,       /* ROM */
    LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,   /* Turbo */
    LIBSPECTRUM_TAPE_BLOCK_PULSES,      /* Pure tone */
    LIBSPECTRUM_TAPE_BLOCK_PURE_DATA,   /* Pulses */
    LIBSPECTRUM_TAPE_BLOCK_PAUSE,       /* Pure data */
    LIBSPECTRUM_TAPE_BLOCK_GROUP_START, /* Pause */
    LIBSPECTRUM_TAPE_BLOCK_GROUP_END,   /* Group start */
    LIBSPECTRUM_TAPE_BLOCK_JUMP,        /* Group end */
    LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,   /* Jump */
    LIBSPECTRUM_TAPE_BLOCK_LOOP_START,  /* Pure tone */
    LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,   /* Loop start */
    LIBSPECTRUM_TAPE_BLOCK_LOOP_END,    /* Pure tone */
    LIBSPECTRUM_TAPE_BLOCK_STOP48,      /* Loop end */
    LIBSPECTRUM_TAPE_BLOCK_COMMENT,     /* Stop tape if in 48K mode */
    LIBSPECTRUM_TAPE_BLOCK_MESSAGE,     /* Comment */
    LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO,/* Message */
    LIBSPECTRUM_TAPE_BLOCK_HARDWARE,    /* Archive info */
    LIBSPECTRUM_TAPE_BLOCK_CUSTOM,      /* Hardware */
    LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,   /* Custom info */
  };
  libspectrum_tape_type *next_block_type = &expected_next_block_types[ 0 ];
  test_return_t r = TEST_PASS;
  int blocks_processed = 0;
  /* Expect to check the next block type of 19 of the 20 blocks in the test
     tzx */
  int expected_block_count = ARRAY_SIZE( expected_next_block_types );

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  tape = libspectrum_tape_alloc();

  if( libspectrum_tape_read( tape, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
			     filename ) )
  {
    libspectrum_tape_free( tape );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  libspectrum_tape_iterator_init( &it, tape );

  while( libspectrum_tape_iterator_peek_next( it ) )
  {
    libspectrum_tape_type actual_next_block_type =
      libspectrum_tape_block_type( libspectrum_tape_iterator_peek_next( it ) );

    if( actual_next_block_type != *next_block_type )
    {
      r = TEST_FAIL;
      break;
    }

    libspectrum_tape_iterator_next( &it );
    next_block_type++;
    blocks_processed++;
  }

  if( blocks_processed != expected_block_count )
  {
      r = TEST_FAIL;
  }

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return r;
}

test_return_t
read_mono_wav_threshold_fixture( void )
{
#ifndef HAVE_WAV_BACKEND
  return TEST_SKIPPED;
#else
  return check_wav_block( DYNAMIC_TEST_PATH( "wav-mono-threshold.wav" ),
                          3500000 / 22050, 0xa9 );
#endif
}

test_return_t
read_stereo_wav_mixdown_fixture( void )
{
#ifndef WAV_INTERNAL_MACOS
  return TEST_SKIPPED;
#else
  return check_wav_block( DYNAMIC_TEST_PATH( "wav-stereo-mixdown.wav" ),
                          3500000 / 22050, 0x8d );
#endif
}

/* Test that PZX archive info tags (title + author) are correctly parsed.
   Regression test for the pzx_read_string bug where *ptr was set to end,
   causing all tag-value pairs after the title to be silently ignored. */
test_return_t
pzx_archive_info_tags_title_and_author_correctly_parsed( void )
{
  const char *filename = STATIC_TEST_PATH( "pzx-archive-info-tags.pzx" );
  libspectrum_tape *tape = NULL;
  libspectrum_tape_iterator it;
  libspectrum_tape_block *block;
  test_return_t r = TEST_INCOMPLETE;

  if( load_tape( &tape, filename, LIBSPECTRUM_ERROR_NONE ) ) return TEST_INCOMPLETE;

  /* Find the ARCHIVE_INFO block */
  for( block = libspectrum_tape_iterator_init( &it, tape );
       block;
       block = libspectrum_tape_iterator_next( &it ) ) {
    if( libspectrum_tape_block_type( block ) == LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO )
      break;
  }

  if( !block ) {
    fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: no ARCHIVE_INFO block found\n", progname );
    goto done;
  }

  /* Expect: title (ID 0x00, "Test Game") and Author (ID 0x02, "Joe Bloggs") */
  if( libspectrum_tape_block_count( block ) != 2 ) {
    fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: expected 2 archive info entries, got %zu\n",
             progname, libspectrum_tape_block_count( block ) );
    r = TEST_FAIL;
    goto done;
  }

  if( libspectrum_tape_block_ids( block, 0 ) != 0x00 ) {
    fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: expected ID 0x00 for entry 0, got 0x%02x\n",
             progname, libspectrum_tape_block_ids( block, 0 ) );
    r = TEST_FAIL;
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 0 ), "Test Game" ) != 0 ) {
    fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: expected title 'Test Game', got '%s'\n",
             progname, libspectrum_tape_block_texts( block, 0 ) );
    r = TEST_FAIL;
    goto done;
  }

  if( libspectrum_tape_block_ids( block, 1 ) != 0x02 ) {
    fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: expected ID 0x02 (Author) for entry 1, got 0x%02x\n",
             progname, libspectrum_tape_block_ids( block, 1 ) );
    r = TEST_FAIL;
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 1 ), "Joe Bloggs" ) != 0 ) {
    fprintf( stderr, "%s: snap_ay_registers_array_getter_setter_all_16_registers: expected author 'Joe Bloggs', got '%s'\n",
             progname, libspectrum_tape_block_texts( block, 1 ) );
    r = TEST_FAIL;
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

test_return_t
tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter( void )
{
  /* tape block: TURBO block pilot_length, sync1_length, sync2_length getter/setter */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_TURBO );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_TURBO ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: expected TURBO block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_pilot_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: default pilot_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pilot_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_pilot_length( block, 2168 );
  if( libspectrum_tape_block_pilot_length( block ) != 2168 ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: expected pilot_length=2168, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pilot_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_sync1_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: default sync1_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_sync1_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_sync1_length( block, 667 );
  if( libspectrum_tape_block_sync1_length( block ) != 667 ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: expected sync1_length=667, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_sync1_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_sync2_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: default sync2_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_sync2_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_sync2_length( block, 735 );
  if( libspectrum_tape_block_sync2_length( block ) != 735 ) {
    fprintf( stderr, "%s: tape_turbo_block_pilot_length_sync1_length_sync2_length_getter_setter: expected sync2_length=735, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_sync2_length( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter( void )
{
  /* tape block: TURBO block bit0_length, bit1_length, pilot_pulses, pause getter/setter */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_TURBO );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_bit0_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: default bit0_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit0_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_bit0_length( block, 855 );
  if( libspectrum_tape_block_bit0_length( block ) != 855 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: expected bit0_length=855, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit0_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_bit1_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: default bit1_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit1_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_bit1_length( block, 1710 );
  if( libspectrum_tape_block_bit1_length( block ) != 1710 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: expected bit1_length=1710, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit1_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_pilot_pulses( block ) != 0 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: default pilot_pulses should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pilot_pulses( block ) );
    goto done;
  }
  libspectrum_tape_block_set_pilot_pulses( block, 8063 );
  if( libspectrum_tape_block_pilot_pulses( block ) != 8063 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: expected pilot_pulses=8063, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pilot_pulses( block ) );
    goto done;
  }

  if( libspectrum_tape_block_pause( block ) != 0 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: default pause should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }
  libspectrum_tape_block_set_pause( block, 1000 );
  if( libspectrum_tape_block_pause( block ) != 1000 ) {
    fprintf( stderr, "%s: tape_turbo_block_bit0_length_bit1_length_pilot_pulses_pause_getter_setter: expected pause=1000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_pure_tone_block_pulse_length_and_count_getter_setter( void )
{
  /* tape block: PURE_TONE block pulse_length and count getter/setter */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_pure_tone_block_pulse_length_and_count_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_pure_tone_block_pulse_length_and_count_getter_setter: expected PURE_TONE block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_pulse_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pure_tone_block_pulse_length_and_count_getter_setter: default pulse_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pulse_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_pulse_length( block, 2168 );
  if( libspectrum_tape_block_pulse_length( block ) != 2168 ) {
    fprintf( stderr, "%s: tape_pure_tone_block_pulse_length_and_count_getter_setter: expected pulse_length=2168, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pulse_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pure_tone_block_pulse_length_and_count_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }
  libspectrum_tape_block_set_count( block, 3223 );
  if( libspectrum_tape_block_count( block ) != 3223 ) {
    fprintf( stderr, "%s: tape_pure_tone_block_pulse_length_and_count_getter_setter: expected count=3223, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter( void )
{
  /* tape block: PURE_DATA block bit0_length, bit1_length, bits_in_last_byte, pause getter/setter */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_DATA );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PURE_DATA ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: expected PURE_DATA block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_bit0_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: default bit0_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit0_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_bit0_length( block, 855 );
  if( libspectrum_tape_block_bit0_length( block ) != 855 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: expected bit0_length=855, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit0_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_bit1_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: default bit1_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit1_length( block ) );
    goto done;
  }
  libspectrum_tape_block_set_bit1_length( block, 1710 );
  if( libspectrum_tape_block_bit1_length( block ) != 1710 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: expected bit1_length=1710, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit1_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_bits_in_last_byte( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: default bits_in_last_byte should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bits_in_last_byte( block ) );
    goto done;
  }
  libspectrum_tape_block_set_bits_in_last_byte( block, 8 );
  if( libspectrum_tape_block_bits_in_last_byte( block ) != 8 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: expected bits_in_last_byte=8, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bits_in_last_byte( block ) );
    goto done;
  }

  if( libspectrum_tape_block_pause( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: default pause should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }
  libspectrum_tape_block_set_pause( block, 500 );
  if( libspectrum_tape_block_pause( block ) != 500 ) {
    fprintf( stderr, "%s: tape_pure_data_block_bit0_length_bit1_length_bits_in_last_byte_pause_getter_setter: expected pause=500, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_pause_block_pause_length_and_level_getter_setter( void )
{
  /* tape block: PAUSE block pause length and level getter/setter */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_pause_block_pause_length_and_level_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_pause_block_pause_length_and_level_getter_setter: expected PAUSE block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_pause( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_block_pause_length_and_level_getter_setter: default pause should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }
  libspectrum_tape_block_set_pause( block, 2000 );
  if( libspectrum_tape_block_pause( block ) != 2000 ) {
    fprintf( stderr, "%s: tape_pause_block_pause_length_and_level_getter_setter: expected pause=2000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  if( libspectrum_tape_block_level( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_block_pause_length_and_level_getter_setter: default level should be 0, got %d\n",
             progname, libspectrum_tape_block_level( block ) );
    goto done;
  }
  libspectrum_tape_block_set_level( block, 1 );
  if( libspectrum_tape_block_level( block ) != 1 ) {
    fprintf( stderr, "%s: tape_pause_block_pause_length_and_level_getter_setter: expected level=1, got %d\n",
             progname, libspectrum_tape_block_level( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_rom_block_data_data_length_and_pause_getter_setter( void )
{
  /* tape block: ROM block data, data_length, and pause getter/setter */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );
  test_return_t r = TEST_FAIL;
  libspectrum_byte *data;

  if( !block ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_ROM ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: expected ROM block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data( block ) != NULL ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: default data should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: default data_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  data = libspectrum_malloc( 3 );
  data[0] = 0x00; data[1] = 0x01; data[2] = 0x02;
  libspectrum_tape_block_set_data_length( block, 3 );
  libspectrum_tape_block_set_data( block, data );

  if( libspectrum_tape_block_data_length( block ) != 3 ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: expected data_length=3, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_data( block ) != data ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: data pointer mismatch\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_pause( block ) != 0 ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: default pause should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  libspectrum_tape_block_set_pause( block, 1000 );
  if( libspectrum_tape_block_pause( block ) != 1000 ) {
    fprintf( stderr, "%s: tape_rom_block_data_data_length_and_pause_getter_setter: expected pause=1000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_pulses_block_count_and_pulse_lengths_getter_setter( void )
{
  /* tape block: PULSES block count and pulse_lengths getter/setter (round-trip) */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PULSES );
  libspectrum_dword *lengths = NULL;
  test_return_t r = TEST_FAIL;
  size_t i;

  if( !block ) {
    fprintf( stderr, "%s: tape_pulses_block_count_and_pulse_lengths_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PULSES ) {
    fprintf( stderr, "%s: tape_pulses_block_count_and_pulse_lengths_getter_setter: expected PULSES block type\n", progname );
    goto done;
  }

  /* Default count should be 0 */
  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pulses_block_count_and_pulse_lengths_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  /* Set up 4 pulse lengths and round-trip through the accessors */
  lengths = libspectrum_new( libspectrum_dword, 4 );
  lengths[0] = 667;
  lengths[1] = 735;
  lengths[2] = 855;
  lengths[3] = 1710;

  libspectrum_tape_block_set_count( block, 4 );
  libspectrum_tape_block_set_pulse_lengths( block, lengths );
  lengths = NULL; /* block owns the array now */

  if( libspectrum_tape_block_count( block ) != 4 ) {
    fprintf( stderr, "%s: tape_pulses_block_count_and_pulse_lengths_getter_setter: expected count=4, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  {
    libspectrum_dword expected[] = { 667, 735, 855, 1710 };
    for( i = 0; i < 4; i++ ) {
      if( libspectrum_tape_block_pulse_lengths( block, i ) != expected[i] ) {
        fprintf( stderr, "%s: tape_pulses_block_count_and_pulse_lengths_getter_setter: pulse_lengths[%lu] expected %lu, got %lu\n",
                 progname, (unsigned long)i, (unsigned long)expected[i],
                 (unsigned long)libspectrum_tape_block_pulse_lengths( block, i ) );
        goto done;
      }
    }
  }

  r = TEST_PASS;

done:
  libspectrum_free( lengths );
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter( void )
{
  /* tape block: RAW_DATA block accessor getter/setter round-trip test */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_RAW_DATA );
  test_return_t r = TEST_FAIL;
  libspectrum_byte *data;

  if( !block ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_RAW_DATA ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: expected RAW_DATA block type\n", progname );
    goto done;
  }

  /* bit_length default should be 0 */
  if( libspectrum_tape_block_bit_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: default bit_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit_length( block ) );
    goto done;
  }

  libspectrum_tape_block_set_bit_length( block, 3500 );
  if( libspectrum_tape_block_bit_length( block ) != 3500 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: expected bit_length=3500, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bit_length( block ) );
    goto done;
  }

  /* bits_in_last_byte default should be 0 */
  if( libspectrum_tape_block_bits_in_last_byte( block ) != 0 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: default bits_in_last_byte should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bits_in_last_byte( block ) );
    goto done;
  }

  libspectrum_tape_block_set_bits_in_last_byte( block, 8 );
  if( libspectrum_tape_block_bits_in_last_byte( block ) != 8 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: expected bits_in_last_byte=8, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bits_in_last_byte( block ) );
    goto done;
  }

  /* data default should be NULL; data_length default should be 0 */
  if( libspectrum_tape_block_data( block ) != NULL ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: default data should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: default data_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  data = libspectrum_malloc( 2 );
  data[0] = 0xaa; data[1] = 0x55;
  libspectrum_tape_block_set_data_length( block, 2 );
  libspectrum_tape_block_set_data( block, data );

  if( libspectrum_tape_block_data_length( block ) != 2 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: expected data_length=2, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_data( block ) != data ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: data pointer mismatch\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data( block )[0] != 0xaa ||
      libspectrum_tape_block_data( block )[1] != 0x55 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: data content mismatch\n", progname );
    goto done;
  }

  /* pause default should be 0 */
  if( libspectrum_tape_block_pause( block ) != 0 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: default pause should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  libspectrum_tape_block_set_pause( block, 1000 );
  if( libspectrum_tape_block_pause( block ) != 1000 ) {
    fprintf( stderr, "%s: tape_raw_data_block_bit_length_bits_in_last_byte_data_data_length_and_pause_getter_setter: expected pause=1000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter( void )
{
  /* tape block: PULSE_SEQUENCE block count, pulse_lengths, and pulse_repeats
     getter/setter (round-trip) */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PULSE_SEQUENCE );
  libspectrum_dword *lengths = NULL;
  size_t *repeats = NULL;
  test_return_t r = TEST_FAIL;
  size_t i;

  if( !block ) {
    fprintf( stderr, "%s: tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PULSE_SEQUENCE ) {
    fprintf( stderr, "%s: tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter: expected PULSE_SEQUENCE block type\n", progname );
    goto done;
  }

  /* Default count should be 0 */
  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  /* Set up 3 pulses (lengths and repeat counts) and round-trip */
  lengths = libspectrum_new( libspectrum_dword, 3 );
  lengths[0] = 2168;
  lengths[1] = 667;
  lengths[2] = 735;

  repeats = libspectrum_new( size_t, 3 );
  repeats[0] = 8063;
  repeats[1] = 1;
  repeats[2] = 1;

  libspectrum_tape_block_set_count( block, 3 );
  libspectrum_tape_block_set_pulse_lengths( block, lengths );
  libspectrum_tape_block_set_pulse_repeats( block, repeats );
  lengths = NULL; /* block owns the array now */
  repeats = NULL; /* block owns the array now */

  if( libspectrum_tape_block_count( block ) != 3 ) {
    fprintf( stderr, "%s: tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter: expected count=3, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  {
    libspectrum_dword expected_lengths[] = { 2168, 667, 735 };
    size_t expected_repeats[] = { 8063, 1, 1 };
    for( i = 0; i < 3; i++ ) {
      if( libspectrum_tape_block_pulse_lengths( block, i ) != expected_lengths[i] ) {
        fprintf( stderr, "%s: tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter: pulse_lengths[%lu] expected %lu, got %lu\n",
                 progname, (unsigned long)i, (unsigned long)expected_lengths[i],
                 (unsigned long)libspectrum_tape_block_pulse_lengths( block, i ) );
        goto done;
      }
      if( libspectrum_tape_block_pulse_repeats( block, i ) != expected_repeats[i] ) {
        fprintf( stderr, "%s: tape_pulse_sequence_block_count_pulse_lengths_and_pulse_repeats_getter_setter: pulse_repeats[%lu] expected %lu, got %lu\n",
                 progname, (unsigned long)i, (unsigned long)expected_repeats[i],
                 (unsigned long)libspectrum_tape_block_pulse_repeats( block, i ) );
        goto done;
      }
    }
  }

  r = TEST_PASS;

done:
  libspectrum_free( lengths );
  libspectrum_free( repeats );
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter( void )
{
  /* tape block: DATA_BLOCK accessor getter/setter round-trip test */
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_DATA_BLOCK );
  libspectrum_word *bit0_pulses = NULL, *bit1_pulses = NULL;
  libspectrum_byte *data = NULL;
  test_return_t r = TEST_FAIL;
  size_t i;

  if( !block ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_DATA_BLOCK ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected DATA_BLOCK block type\n", progname );
    goto done;
  }

  /* Default count should be 0 */
  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  libspectrum_tape_block_set_count( block, 16 );
  if( libspectrum_tape_block_count( block ) != 16 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected count=16, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  /* Default tail_length should be 0 */
  if( libspectrum_tape_block_tail_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default tail_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_tail_length( block ) );
    goto done;
  }

  libspectrum_tape_block_set_tail_length( block, 945 );
  if( libspectrum_tape_block_tail_length( block ) != 945 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected tail_length=945, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_tail_length( block ) );
    goto done;
  }

  /* Default level should be 0 */
  if( libspectrum_tape_block_level( block ) != 0 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default level should be 0, got %d\n",
             progname, libspectrum_tape_block_level( block ) );
    goto done;
  }

  libspectrum_tape_block_set_level( block, 1 );
  if( libspectrum_tape_block_level( block ) != 1 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected level=1, got %d\n",
             progname, libspectrum_tape_block_level( block ) );
    goto done;
  }

  /* Default bits_in_last_byte should be 0; data and data_length should
     default to NULL/0 */
  if( libspectrum_tape_block_bits_in_last_byte( block ) != 0 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default bits_in_last_byte should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bits_in_last_byte( block ) );
    goto done;
  }

  if( libspectrum_tape_block_data( block ) != NULL ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default data should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default data_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  data = libspectrum_malloc( 2 );
  data[0] = 0x5a; data[1] = 0xa5;
  libspectrum_tape_block_set_bits_in_last_byte( block, 8 );
  libspectrum_tape_block_set_data_length( block, 2 );
  libspectrum_tape_block_set_data( block, data );
  data = NULL; /* block owns the array now */

  if( libspectrum_tape_block_bits_in_last_byte( block ) != 8 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected bits_in_last_byte=8, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_bits_in_last_byte( block ) );
    goto done;
  }

  if( libspectrum_tape_block_data_length( block ) != 2 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected data_length=2, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_data( block ) == NULL ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: data should not be NULL after set\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data( block )[0] != 0x5a ||
      libspectrum_tape_block_data( block )[1] != 0xa5 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: data content mismatch\n", progname );
    goto done;
  }

  /* Default bit pulse counts should be 0 */
  if( libspectrum_tape_block_bit0_pulse_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default bit0_pulse_count should be 0, got %u\n",
             progname, (unsigned)libspectrum_tape_block_bit0_pulse_count( block ) );
    goto done;
  }

  if( libspectrum_tape_block_bit1_pulse_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: default bit1_pulse_count should be 0, got %u\n",
             progname, (unsigned)libspectrum_tape_block_bit1_pulse_count( block ) );
    goto done;
  }

  /* Set bit0 pulses: 2 pulses of 855 T-states each */
  bit0_pulses = libspectrum_new( libspectrum_word, 2 );
  bit0_pulses[0] = 855; bit0_pulses[1] = 855;
  libspectrum_tape_block_set_bit0_pulse_count( block, 2 );
  libspectrum_tape_block_set_bit0_pulses( block, bit0_pulses );
  bit0_pulses = NULL; /* block owns the array now */

  if( libspectrum_tape_block_bit0_pulse_count( block ) != 2 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected bit0_pulse_count=2, got %u\n",
             progname, (unsigned)libspectrum_tape_block_bit0_pulse_count( block ) );
    goto done;
  }

  {
    libspectrum_word expected_bit0[] = { 855, 855 };
    for( i = 0; i < 2; i++ ) {
      if( libspectrum_tape_block_bit0_pulses( block, i ) != expected_bit0[i] ) {
        fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: bit0_pulses[%lu] expected %u, got %u\n",
                 progname, (unsigned long)i, (unsigned)expected_bit0[i],
                 (unsigned)libspectrum_tape_block_bit0_pulses( block, i ) );
        goto done;
      }
    }
  }

  /* Set bit1 pulses: 2 pulses of 1710 T-states each */
  bit1_pulses = libspectrum_new( libspectrum_word, 2 );
  bit1_pulses[0] = 1710; bit1_pulses[1] = 1710;
  libspectrum_tape_block_set_bit1_pulse_count( block, 2 );
  libspectrum_tape_block_set_bit1_pulses( block, bit1_pulses );
  bit1_pulses = NULL; /* block owns the array now */

  if( libspectrum_tape_block_bit1_pulse_count( block ) != 2 ) {
    fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: expected bit1_pulse_count=2, got %u\n",
             progname, (unsigned)libspectrum_tape_block_bit1_pulse_count( block ) );
    goto done;
  }

  {
    libspectrum_word expected_bit1[] = { 1710, 1710 };
    for( i = 0; i < 2; i++ ) {
      if( libspectrum_tape_block_bit1_pulses( block, i ) != expected_bit1[i] ) {
        fprintf( stderr, "%s: tape_data_block_count_tail_length_level_data_and_bit_pulses_getter_setter: bit1_pulses[%lu] expected %u, got %u\n",
                 progname, (unsigned long)i, (unsigned)expected_bit1[i],
                 (unsigned)libspectrum_tape_block_bit1_pulses( block, i ) );
        goto done;
      }
    }
  }

  r = TEST_PASS;

done:
  libspectrum_free( data );
  libspectrum_free( bit0_pulses );
  libspectrum_free( bit1_pulses );
  libspectrum_tape_block_free( block );
  return r;
}

/* Test that tzx_write_pulse_sequence correctly splits PULSES blocks when
   more than 255 single-repeat pulses are present.
   The TZX ID 0x13 (Pulse sequence) block stores its count in a single byte
   so counts > 255 must be split across multiple blocks. */
test_return_t
tzx_pulse_sequence_over_255_splits_into_multiple_pulses_blocks( void )
{
  libspectrum_tape *tape;
  libspectrum_tape_block *block;
  libspectrum_dword *lengths;
  size_t *repeats;
  libspectrum_byte *output;
  size_t length;
  const size_t pulse_count = 300;
  const size_t first_block_expected = 255;
  const size_t second_block_expected = 45;
  test_return_t r;
  size_t i, offset;
  int pulses_blocks_found, first_count, second_count, blk_count;

  lengths = libspectrum_new( libspectrum_dword, pulse_count );
  repeats = libspectrum_new( size_t, pulse_count );
  for( i = 0; i < pulse_count; i++ ) {
    lengths[i] = 1000 + (libspectrum_dword)i;
    repeats[i] = 1;
  }

  tape = libspectrum_tape_alloc();
  if( !tape ) {
    libspectrum_free( lengths );
    libspectrum_free( repeats );
    return TEST_INCOMPLETE;
  }

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PULSE_SEQUENCE );
  if( !block ) {
    libspectrum_free( lengths );
    libspectrum_free( repeats );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_block_set_count( block, pulse_count );
  libspectrum_tape_block_set_pulse_lengths( block, lengths );
  libspectrum_tape_block_set_pulse_repeats( block, repeats );
  lengths = NULL; /* block owns the array now */
  repeats = NULL;
  libspectrum_tape_append_block( tape, block );

  output = NULL;
  length = 0;
  r = TEST_INCOMPLETE;
  if( libspectrum_tape_write( &output, &length, tape, LIBSPECTRUM_ID_TAPE_TZX ) ) {
    fprintf( stderr, "%s: tzx_pulse_sequence_over_255_splits_into_multiple_pulses_blocks: tape write failed\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }
  libspectrum_tape_free( tape );

  /* TZX layout:
       header: "ZXTape!\x1a" (8) + major (1) + minor (1) = 10 bytes
       SET_SIGNAL_LEVEL (0x2B): ID (1) + length dword (4) + level (1) = 6 bytes
       PULSES block(s) (0x13): ID (1) + count (1) + count*2 bytes */
  r = TEST_FAIL;
  offset = 10; /* skip TZX header */

  if( offset + 6 > length ||
      output[offset] != LIBSPECTRUM_TAPE_BLOCK_SET_SIGNAL_LEVEL ) {
    fprintf( stderr, "%s: tzx_pulse_sequence_over_255_splits_into_multiple_pulses_blocks: expected SET_SIGNAL_LEVEL block at offset %lu\n",
             progname, (unsigned long)offset );
    goto done;
  }
  offset += 6;

  pulses_blocks_found = 0;
  first_count = 0;
  second_count = 0;
  while( offset < length && output[offset] == LIBSPECTRUM_TAPE_BLOCK_PULSES ) {
    offset++; /* skip ID */
    if( offset >= length ) goto done;
    blk_count = output[offset++];
    pulses_blocks_found++;
    if( pulses_blocks_found == 1 ) first_count = blk_count;
    else if( pulses_blocks_found == 2 ) second_count = blk_count;
    offset += 2 * (size_t)blk_count;
  }

  if( pulses_blocks_found != 2 ) {
    fprintf( stderr, "%s: tzx_pulse_sequence_over_255_splits_into_multiple_pulses_blocks: expected 2 PULSES blocks, found %d\n",
             progname, pulses_blocks_found );
    goto done;
  }
  if( (size_t)first_count != first_block_expected ) {
    fprintf( stderr, "%s: tzx_pulse_sequence_over_255_splits_into_multiple_pulses_blocks: expected first PULSES count=%lu, got %d\n",
             progname, (unsigned long)first_block_expected, first_count );
    goto done;
  }
  if( (size_t)second_count != second_block_expected ) {
    fprintf( stderr, "%s: tzx_pulse_sequence_over_255_splits_into_multiple_pulses_blocks: expected second PULSES count=%lu, got %d\n",
             progname, (unsigned long)second_block_expected, second_count );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( output );
  return r;
}

test_return_t
tape_group_start_block_text_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_GROUP_START );
  char *text = NULL;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_group_start_block_text_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_GROUP_START ) {
    fprintf( stderr, "%s: tape_group_start_block_text_getter_setter: expected GROUP_START block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_text( block ) != NULL ) {
    fprintf( stderr, "%s: tape_group_start_block_text_getter_setter: default text should be NULL\n", progname );
    goto done;
  }

  text = libspectrum_new( char, strlen( "TestGroup" ) + 1 );
  strcpy( text, "TestGroup" );
  libspectrum_tape_block_set_text( block, text );
  text = NULL;

  if( strcmp( libspectrum_tape_block_text( block ), "TestGroup" ) != 0 ) {
    fprintf( stderr, "%s: tape_group_start_block_text_getter_setter: expected text=\"TestGroup\", got \"%s\"\n",
             progname, libspectrum_tape_block_text( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( text );
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_comment_block_text_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_COMMENT );
  char *text = NULL;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_comment_block_text_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_COMMENT ) {
    fprintf( stderr, "%s: tape_comment_block_text_getter_setter: expected COMMENT block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_text( block ) != NULL ) {
    fprintf( stderr, "%s: tape_comment_block_text_getter_setter: default text should be NULL\n", progname );
    goto done;
  }

  text = libspectrum_new( char, strlen( "Hello, World!" ) + 1 );
  strcpy( text, "Hello, World!" );
  libspectrum_tape_block_set_text( block, text );
  text = NULL;

  if( strcmp( libspectrum_tape_block_text( block ), "Hello, World!" ) != 0 ) {
    fprintf( stderr, "%s: tape_comment_block_text_getter_setter: expected text=\"Hello, World!\", got \"%s\"\n",
             progname, libspectrum_tape_block_text( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( text );
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_jump_block_offset_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_JUMP );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_jump_block_offset_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_JUMP ) {
    fprintf( stderr, "%s: tape_jump_block_offset_getter_setter: expected JUMP block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_offset( block ) != 0 ) {
    fprintf( stderr, "%s: tape_jump_block_offset_getter_setter: default offset should be 0, got %d\n",
             progname, libspectrum_tape_block_offset( block ) );
    goto done;
  }

  libspectrum_tape_block_set_offset( block, -3 );
  if( libspectrum_tape_block_offset( block ) != -3 ) {
    fprintf( stderr, "%s: tape_jump_block_offset_getter_setter: expected offset=-3, got %d\n",
             progname, libspectrum_tape_block_offset( block ) );
    goto done;
  }

  libspectrum_tape_block_set_offset( block, 5 );
  if( libspectrum_tape_block_offset( block ) != 5 ) {
    fprintf( stderr, "%s: tape_jump_block_offset_getter_setter: expected offset=5, got %d\n",
             progname, libspectrum_tape_block_offset( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_loop_start_block_count_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_LOOP_START );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_loop_start_block_count_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_LOOP_START ) {
    fprintf( stderr, "%s: tape_loop_start_block_count_getter_setter: expected LOOP_START block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_loop_start_block_count_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  libspectrum_tape_block_set_count( block, 5 );
  if( libspectrum_tape_block_count( block ) != 5 ) {
    fprintf( stderr, "%s: tape_loop_start_block_count_getter_setter: expected count=5, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_message_block_text_and_pause_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_MESSAGE );
  char *text = NULL;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_message_block_text_and_pause_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_MESSAGE ) {
    fprintf( stderr, "%s: tape_message_block_text_and_pause_getter_setter: expected MESSAGE block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_text( block ) != NULL ) {
    fprintf( stderr, "%s: tape_message_block_text_and_pause_getter_setter: default text should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_pause( block ) != 0 ) {
    fprintf( stderr, "%s: tape_message_block_text_and_pause_getter_setter: default pause should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  text = libspectrum_new( char, strlen( "Press PLAY on tape." ) + 1 );
  strcpy( text, "Press PLAY on tape." );
  libspectrum_tape_block_set_text( block, text );
  text = NULL;

  if( strcmp( libspectrum_tape_block_text( block ), "Press PLAY on tape." ) != 0 ) {
    fprintf( stderr, "%s: tape_message_block_text_and_pause_getter_setter: expected text=\"Press PLAY on tape.\", got \"%s\"\n",
             progname, libspectrum_tape_block_text( block ) );
    goto done;
  }

  libspectrum_tape_block_set_pause( block, 10 );
  if( libspectrum_tape_block_pause( block ) != 10 ) {
    fprintf( stderr, "%s: tape_message_block_text_and_pause_getter_setter: expected pause=10, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( text );
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_archive_info_block_count_ids_and_texts_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO );
  int *ids = NULL;
  char **strings = NULL;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: expected ARCHIVE_INFO block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  ids = libspectrum_new( int, 2 );
  ids[0] = 0x00;
  ids[1] = 0x02;

  strings = libspectrum_new( char *, 2 );
  strings[0] = libspectrum_new( char, strlen( "Manic Miner" ) + 1 );
  strcpy( strings[0], "Manic Miner" );
  strings[1] = libspectrum_new( char, strlen( "Software Projects" ) + 1 );
  strcpy( strings[1], "Software Projects" );

  libspectrum_tape_block_set_count( block, 2 );
  libspectrum_tape_block_set_ids( block, ids );
  libspectrum_tape_block_set_texts( block, strings );
  ids = NULL;
  strings = NULL;

  if( libspectrum_tape_block_count( block ) != 2 ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: expected count=2, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  if( libspectrum_tape_block_ids( block, 0 ) != 0x00 ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: expected ids[0]=0x00, got 0x%02x\n",
             progname, libspectrum_tape_block_ids( block, 0 ) );
    goto done;
  }

  if( libspectrum_tape_block_ids( block, 1 ) != 0x02 ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: expected ids[1]=0x02, got 0x%02x\n",
             progname, libspectrum_tape_block_ids( block, 1 ) );
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 0 ), "Manic Miner" ) != 0 ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: expected texts[0]=\"Manic Miner\", got \"%s\"\n",
             progname, libspectrum_tape_block_texts( block, 0 ) );
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 1 ), "Software Projects" ) != 0 ) {
    fprintf( stderr, "%s: tape_archive_info_block_count_ids_and_texts_getter_setter: expected texts[1]=\"Software Projects\", got \"%s\"\n",
             progname, libspectrum_tape_block_texts( block, 1 ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( ids );
  if( strings ) {
    libspectrum_free( strings[0] );
    libspectrum_free( strings[1] );
    libspectrum_free( strings );
  }
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_hardware_block_count_types_ids_and_values_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_HARDWARE );
  int *types = NULL, *ids = NULL, *values = NULL;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_hardware_block_count_types_ids_and_values_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_HARDWARE ) {
    fprintf( stderr, "%s: tape_hardware_block_count_types_ids_and_values_getter_setter: expected HARDWARE block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_hardware_block_count_types_ids_and_values_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  types = libspectrum_new( int, 2 );
  types[0] = 0x00;
  types[1] = 0x09;

  ids = libspectrum_new( int, 2 );
  ids[0] = 0x00;
  ids[1] = 0x00;

  values = libspectrum_new( int, 2 );
  values[0] = 0x01;
  values[1] = 0x01;

  libspectrum_tape_block_set_count( block, 2 );
  libspectrum_tape_block_set_types( block, types );
  libspectrum_tape_block_set_ids( block, ids );
  libspectrum_tape_block_set_values( block, values );
  types = NULL;
  ids = NULL;
  values = NULL;

  if( libspectrum_tape_block_count( block ) != 2 ) {
    fprintf( stderr, "%s: tape_hardware_block_count_types_ids_and_values_getter_setter: expected count=2, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  if( libspectrum_tape_block_types( block, 0 ) != 0x00 ||
      libspectrum_tape_block_types( block, 1 ) != 0x09 ) {
    fprintf( stderr, "%s: tape_hardware_block_count_types_ids_and_values_getter_setter: types mismatch\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_ids( block, 0 ) != 0x00 ||
      libspectrum_tape_block_ids( block, 1 ) != 0x00 ) {
    fprintf( stderr, "%s: tape_hardware_block_count_types_ids_and_values_getter_setter: ids mismatch\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_values( block, 0 ) != 0x01 ||
      libspectrum_tape_block_values( block, 1 ) != 0x01 ) {
    fprintf( stderr, "%s: tape_hardware_block_count_types_ids_and_values_getter_setter: values mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( types );
  libspectrum_free( ids );
  libspectrum_free( values );
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_select_block_count_offsets_and_texts_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_SELECT );
  int *offsets = NULL;
  char **descriptions = NULL;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_select_block_count_offsets_and_texts_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_SELECT ) {
    fprintf( stderr, "%s: tape_select_block_count_offsets_and_texts_getter_setter: expected SELECT block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_count( block ) != 0 ) {
    fprintf( stderr, "%s: tape_select_block_count_offsets_and_texts_getter_setter: default count should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  offsets = libspectrum_new( int, 2 );
  offsets[0] = 10;
  offsets[1] = 20;

  descriptions = libspectrum_new( char *, 2 );
  descriptions[0] = libspectrum_new( char, strlen( "Side A" ) + 1 );
  strcpy( descriptions[0], "Side A" );
  descriptions[1] = libspectrum_new( char, strlen( "Side B" ) + 1 );
  strcpy( descriptions[1], "Side B" );

  libspectrum_tape_block_set_count( block, 2 );
  libspectrum_tape_block_set_offsets( block, offsets );
  libspectrum_tape_block_set_texts( block, descriptions );
  offsets = NULL;
  descriptions = NULL;

  if( libspectrum_tape_block_count( block ) != 2 ) {
    fprintf( stderr, "%s: tape_select_block_count_offsets_and_texts_getter_setter: expected count=2, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_count( block ) );
    goto done;
  }

  if( libspectrum_tape_block_offsets( block, 0 ) != 10 ||
      libspectrum_tape_block_offsets( block, 1 ) != 20 ) {
    fprintf( stderr, "%s: tape_select_block_count_offsets_and_texts_getter_setter: offsets mismatch\n", progname );
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 0 ), "Side A" ) != 0 ) {
    fprintf( stderr, "%s: tape_select_block_count_offsets_and_texts_getter_setter: expected texts[0]=\"Side A\", got \"%s\"\n",
             progname, libspectrum_tape_block_texts( block, 0 ) );
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 1 ), "Side B" ) != 0 ) {
    fprintf( stderr, "%s: tape_select_block_count_offsets_and_texts_getter_setter: expected texts[1]=\"Side B\", got \"%s\"\n",
             progname, libspectrum_tape_block_texts( block, 1 ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( offsets );
  if( descriptions ) {
    libspectrum_free( descriptions[0] );
    libspectrum_free( descriptions[1] );
    libspectrum_free( descriptions );
  }
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_set_signal_level_block_level_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_SET_SIGNAL_LEVEL );
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_set_signal_level_block_level_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_SET_SIGNAL_LEVEL ) {
    fprintf( stderr, "%s: tape_set_signal_level_block_level_getter_setter: expected SET_SIGNAL_LEVEL block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_level( block ) != 0 ) {
    fprintf( stderr, "%s: tape_set_signal_level_block_level_getter_setter: default level should be 0, got %d\n",
             progname, libspectrum_tape_block_level( block ) );
    goto done;
  }

  libspectrum_tape_block_set_level( block, 1 );
  if( libspectrum_tape_block_level( block ) != 1 ) {
    fprintf( stderr, "%s: tape_set_signal_level_block_level_getter_setter: expected level=1, got %d\n",
             progname, libspectrum_tape_block_level( block ) );
    goto done;
  }

  libspectrum_tape_block_set_level( block, 0 );
  if( libspectrum_tape_block_level( block ) != 0 ) {
    fprintf( stderr, "%s: tape_set_signal_level_block_level_getter_setter: expected level=0 after reset, got %d\n",
             progname, libspectrum_tape_block_level( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_custom_block_text_data_and_data_length_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_CUSTOM );
  char *description = NULL;
  libspectrum_byte *data = NULL;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_CUSTOM ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: expected CUSTOM block type\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_text( block ) != NULL ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: default text should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data( block ) != NULL ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: default data should be NULL\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: default data_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  description = libspectrum_new( char, strlen( "My Custom Block" ) + 1 );
  strcpy( description, "My Custom Block" );
  libspectrum_tape_block_set_text( block, description );
  description = NULL;

  if( strcmp( libspectrum_tape_block_text( block ), "My Custom Block" ) != 0 ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: expected text=\"My Custom Block\", got \"%s\"\n",
             progname, libspectrum_tape_block_text( block ) );
    goto done;
  }

  data = libspectrum_new( libspectrum_byte, 3 );
  data[0] = 0xde;
  data[1] = 0xad;
  data[2] = 0xbe;
  libspectrum_tape_block_set_data_length( block, 3 );
  libspectrum_tape_block_set_data( block, data );
  data = NULL;

  if( libspectrum_tape_block_data_length( block ) != 3 ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: expected data_length=3, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  if( libspectrum_tape_block_data( block ) == NULL ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: data should not be NULL after set\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_data( block )[0] != 0xde ||
      libspectrum_tape_block_data( block )[1] != 0xad ||
      libspectrum_tape_block_data( block )[2] != 0xbe ) {
    fprintf( stderr, "%s: tape_custom_block_text_data_and_data_length_getter_setter: data content mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( description );
  libspectrum_free( data );
  libspectrum_tape_block_free( block );
  return r;
}

test_return_t
tape_block_description_returns_correct_string_for_all_types( void )
{
  static const struct {
    libspectrum_tape_type type;
    const char *expected;
  } cases[] = {
    { LIBSPECTRUM_TAPE_BLOCK_ROM,             "Standard Speed Data"       },
    { LIBSPECTRUM_TAPE_BLOCK_TURBO,           "Turbo Speed Data"          },
    { LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,       "Pure Tone"                 },
    { LIBSPECTRUM_TAPE_BLOCK_PULSES,          "List of Pulses"            },
    { LIBSPECTRUM_TAPE_BLOCK_PURE_DATA,       "Pure Data"                 },
    { LIBSPECTRUM_TAPE_BLOCK_RAW_DATA,        "Raw Data"                  },
    { LIBSPECTRUM_TAPE_BLOCK_GENERALISED_DATA,"Generalised Data"          },
    { LIBSPECTRUM_TAPE_BLOCK_PAUSE,           "Pause"                     },
    { LIBSPECTRUM_TAPE_BLOCK_GROUP_START,     "Group Start"               },
    { LIBSPECTRUM_TAPE_BLOCK_GROUP_END,       "Group End"                 },
    { LIBSPECTRUM_TAPE_BLOCK_JUMP,            "Jump"                      },
    { LIBSPECTRUM_TAPE_BLOCK_LOOP_START,      "Loop Start Block"          },
    { LIBSPECTRUM_TAPE_BLOCK_LOOP_END,        "Loop End"                  },
    { LIBSPECTRUM_TAPE_BLOCK_SELECT,          "Select"                    },
    { LIBSPECTRUM_TAPE_BLOCK_STOP48,          "Stop Tape If In 48K Mode"  },
    { LIBSPECTRUM_TAPE_BLOCK_SET_SIGNAL_LEVEL,"Set Signal Level"          },
    { LIBSPECTRUM_TAPE_BLOCK_COMMENT,         "Comment"                   },
    { LIBSPECTRUM_TAPE_BLOCK_MESSAGE,         "Message"                   },
    { LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO,    "Archive Info"              },
    { LIBSPECTRUM_TAPE_BLOCK_HARDWARE,        "Hardware Information"      },
    { LIBSPECTRUM_TAPE_BLOCK_CUSTOM,          "Custom Info"               },
    { LIBSPECTRUM_TAPE_BLOCK_RLE_PULSE,       "RLE Pulse"                 },
    { LIBSPECTRUM_TAPE_BLOCK_PULSE_SEQUENCE,  "Pulse Sequence"            },
    { LIBSPECTRUM_TAPE_BLOCK_DATA_BLOCK,      "Data Block"                },
    { LIBSPECTRUM_TAPE_BLOCK_CONCAT,          "Glue Block"                },
  };

  char buf[64];
  size_t i;
  test_return_t r = TEST_PASS;

  for( i = 0; i < ARRAY_SIZE( cases ); i++ ) {
    libspectrum_tape_block *block =
      libspectrum_tape_block_alloc( cases[i].type );
    libspectrum_error err;

    if( !block ) {
      fprintf( stderr, "%s: tape_block_description_all_types: tape_block_alloc returned NULL for type 0x%02x\n",
               progname, cases[i].type );
      return TEST_INCOMPLETE;
    }

    err = libspectrum_tape_block_description( buf, sizeof( buf ), block );
    libspectrum_tape_block_free( block );

    if( err != LIBSPECTRUM_ERROR_NONE ) {
      fprintf( stderr, "%s: tape_block_description_all_types: description failed for type 0x%02x\n",
               progname, cases[i].type );
      r = TEST_FAIL;
      continue;
    }

    if( strcmp( buf, cases[i].expected ) != 0 ) {
      fprintf( stderr, "%s: tape_block_description_all_types: type 0x%02x: expected '%s', got '%s'\n",
               progname, cases[i].type, cases[i].expected, buf );
      r = TEST_FAIL;
    }
  }
  return r;
}

/* Test that pause_tstates getter/setter works correctly across all block
   types that support it: PAUSE, ROM, TURBO, PURE_DATA, RAW_DATA, and MESSAGE. */
test_return_t
tape_pause_tstates_getter_setter_across_block_types( void )
{
  /* We test each block type in a local scope for clarity */
  libspectrum_tape_block *block;
  test_return_t r = TEST_FAIL;

  /* --- PAUSE block --- */
  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( !block ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: PAUSE alloc failed\n", progname );
    return TEST_INCOMPLETE;
  }
  if( libspectrum_tape_block_pause_tstates( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: PAUSE default pause_tstates should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_set_pause_tstates( block, 35000 );
  if( libspectrum_tape_block_pause_tstates( block ) != 35000 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: PAUSE expected 35000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  /* pause_tstates and pause are independent fields */
  if( libspectrum_tape_block_pause( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: PAUSE ms-pause should be unaffected, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_free( block );

  /* --- ROM block --- */
  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );
  if( !block ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: ROM alloc failed\n", progname );
    goto done_no_block;
  }
  if( libspectrum_tape_block_pause_tstates( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: ROM default pause_tstates should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_set_pause_tstates( block, 70000 );
  if( libspectrum_tape_block_pause_tstates( block ) != 70000 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: ROM expected 70000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_free( block );

  /* --- TURBO block --- */
  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_TURBO );
  if( !block ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: TURBO alloc failed\n", progname );
    goto done_no_block;
  }
  if( libspectrum_tape_block_pause_tstates( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: TURBO default pause_tstates should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_set_pause_tstates( block, 12345 );
  if( libspectrum_tape_block_pause_tstates( block ) != 12345 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: TURBO expected 12345, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_free( block );

  /* --- PURE_DATA block --- */
  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_DATA );
  if( !block ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: PURE_DATA alloc failed\n", progname );
    goto done_no_block;
  }
  if( libspectrum_tape_block_pause_tstates( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: PURE_DATA default pause_tstates should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_set_pause_tstates( block, 99999 );
  if( libspectrum_tape_block_pause_tstates( block ) != 99999 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: PURE_DATA expected 99999, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_free( block );

  /* --- RAW_DATA block --- */
  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_RAW_DATA );
  if( !block ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: RAW_DATA alloc failed\n", progname );
    goto done_no_block;
  }
  if( libspectrum_tape_block_pause_tstates( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: RAW_DATA default pause_tstates should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_set_pause_tstates( block, 1000000 );
  if( libspectrum_tape_block_pause_tstates( block ) != 1000000 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: RAW_DATA expected 1000000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_free( block );

  /* --- MESSAGE block --- */
  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_MESSAGE );
  if( !block ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: MESSAGE alloc failed\n", progname );
    goto done_no_block;
  }
  if( libspectrum_tape_block_pause_tstates( block ) != 0 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: MESSAGE default pause_tstates should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_set_pause_tstates( block, 5000 );
  if( libspectrum_tape_block_pause_tstates( block ) != 5000 ) {
    fprintf( stderr, "%s: tape_pause_tstates_getter_setter_across_block_types: MESSAGE expected 5000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_pause_tstates( block ) );
    libspectrum_tape_block_free( block );
    goto done_no_block;
  }
  libspectrum_tape_block_free( block );

  r = TEST_PASS;

done_no_block:
  return r;
}

/* Test that scale, data, and data_length accessors work for the RLE_PULSE
   block (libspectrum's internal type 0x100 used by the PZX format). */
test_return_t
tape_rle_pulse_block_scale_data_and_data_length_getter_setter( void )
{
  libspectrum_tape_block *block =
    libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_RLE_PULSE );
  libspectrum_byte *data;
  test_return_t r = TEST_FAIL;

  if( !block ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_RLE_PULSE ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: expected RLE_PULSE block type\n", progname );
    goto done;
  }

  /* Default scale should be 0 */
  if( libspectrum_tape_block_scale( block ) != 0 ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: default scale should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_scale( block ) );
    goto done;
  }

  libspectrum_tape_block_set_scale( block, 3500000 );
  if( libspectrum_tape_block_scale( block ) != 3500000 ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: expected scale=3500000, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_scale( block ) );
    goto done;
  }

  /* Default data should be NULL, data_length 0 */
  if( libspectrum_tape_block_data( block ) != NULL ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: default data should be NULL\n", progname );
    goto done;
  }
  if( libspectrum_tape_block_data_length( block ) != 0 ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: default data_length should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }

  /* Set data: 4-byte RLE payload {3, 5, 1, 7} */
  data = libspectrum_new( libspectrum_byte, 4 );
  if( !data ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: data alloc failed\n", progname );
    goto done;
  }
  data[0] = 3; data[1] = 5; data[2] = 1; data[3] = 7;

  libspectrum_tape_block_set_data( block, data );
  libspectrum_tape_block_set_data_length( block, 4 );

  if( libspectrum_tape_block_data( block ) != data ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: data pointer mismatch\n", progname );
    goto done;
  }
  if( libspectrum_tape_block_data_length( block ) != 4 ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: expected data_length=4, got %lu\n",
             progname, (unsigned long)libspectrum_tape_block_data_length( block ) );
    goto done;
  }
  if( libspectrum_tape_block_data( block )[0] != 3 ||
      libspectrum_tape_block_data( block )[1] != 5 ||
      libspectrum_tape_block_data( block )[2] != 1 ||
      libspectrum_tape_block_data( block )[3] != 7 ) {
    fprintf( stderr, "%s: tape_rle_pulse_block_scale_data_and_data_length_getter_setter: data contents mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

/* libspectrum_tape_present returns 0 for a freshly allocated (empty) tape */
test_return_t
tape_present_returns_false_for_empty_tape( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();

  if( !tape ) {
    fprintf( stderr, "%s: tape_present_returns_false_for_empty_tape: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_present( tape ) ) {
    fprintf( stderr, "%s: tape_present_returns_false_for_empty_tape: "
             "expected tape_present to return 0 for empty tape\n", progname );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  libspectrum_tape_free( tape );
  return TEST_PASS;
}

/* libspectrum_tape_present returns non-zero after loading blocks from a
   tape file, and libspectrum_tape_clear makes it empty again */
test_return_t
tape_present_true_after_load_and_false_after_clear( void )
{
  const char *filename = STATIC_TEST_PATH( "standard-tap.tap" );
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_tape *tape;
  test_return_t r = TEST_INCOMPLETE;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  tape = libspectrum_tape_alloc();
  if( !tape ) {
    fprintf( stderr, "%s: tape_present_true_after_load_and_false_after_clear: "
             "tape_alloc returned NULL\n", progname );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_read( tape, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
                             filename ) ) {
    fprintf( stderr, "%s: tape_present_true_after_load_and_false_after_clear: "
             "tape_read failed\n", progname );
    libspectrum_tape_free( tape );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  if( !libspectrum_tape_present( tape ) ) {
    fprintf( stderr, "%s: tape_present_true_after_load_and_false_after_clear: "
             "expected tape_present to return non-zero after loading\n",
             progname );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_clear( tape ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: tape_present_true_after_load_and_false_after_clear: "
             "tape_clear returned error\n", progname );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_present( tape ) ) {
    fprintf( stderr, "%s: tape_present_true_after_load_and_false_after_clear: "
             "expected tape_present to return 0 after tape_clear\n", progname );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  r = TEST_PASS;

  libspectrum_tape_free( tape );
  return r;
}
