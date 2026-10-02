#include "test.h"
#include "common.h"

static test_edge_sequence_t
complete_edges_list[] =
{
  /* Standard speed data block */
  { 2168, 3223, 0 },	/* Pilot */
  {  667,    1, 0 },	/* Sync 1 */
  {  735,    1, 0 },	/* Sync 2 */

  { 1710,    2, 0 },	/* Bit 1 */
  {  855,    2, 0 },	/* Bit 2 */
  { 1710,    2, 0 },	/* Bit 3 */
  {  855,    2, 0 },	/* Bit 4 */
  { 1710,    2, 0 },	/* Bit 5 */
  {  855,    2, 0 },	/* Bit 6 */
  { 1710,    2, 0 },	/* Bit 7 */
  {  855,    2, 0 },	/* Bit 8 */

  { 8207500, 1, 0 },	/* Pause */

  /* Turbo speed data block */
  { 1000,    5, 0 },	/* Pilot */
  {  123,    1, 0 },	/* Sync 1 */
  {  456,    1, 0 },	/* Sync 2 */

  {  789,   16, 0 },	/* Byte 1, bits 1-8 */
  {  400,   16, 0 },	/* Byte 2, bits 1-8 */

  {  789,    2, 0 },	/* Byte 3, bit 1 */
  {  400,    2, 0 },	/* Byte 3, bit 2 */
  {  789,    2, 0 },	/* Byte 3, bit 3 */
  {  400,    2, 0 },	/* Byte 3, bit 4 */
  {  789,    2, 0 },	/* Byte 3, bit 5 */
  {  400,    2, 0 },	/* Byte 3, bit 6 */
  {  789,    2, 0 },	/* Byte 3, bit 7 */
  {  400,    2, 0 },	/* Byte 3, bit 8 */

  {  400,    2, 0 },	/* Byte 4, bit 1 */
  {  789,    2, 0 },	/* Byte 4, bit 2 */
  {  400,    2, 0 },	/* Byte 4, bit 3 */
  {  789,    2, 0 },	/* Byte 4, bit 4 */

  { 3454500, 1, 0 },	/* Pause */

  /* Pure tone block */
  {  535,  666, 0 },

  /* List of pulses */
  {  772,    1, 0 },
  {  297,    1, 0 },
  {  692,    1, 0 },

  /* Pure data block */
  { 1639,   16, 0 },	/* Byte 1, bits 1-8 */
  {  552,   16, 0 },	/* Byte 2, bits 1-8 */
  { 1639,   12, 0 },	/* Byte 3, bits 1-6 */
  { 1939000, 1, 0 },	/* Pause */

  /* Pause block */
  { 2163000, 1, 0 },

  /* Group start block */
  {    0,    1, 8 },

  /* Group end block */
  {    0,    1, 8 },

  /* Jump block */
  {    0,    1, 8 },

  /* Pure tone block skipped */

  /* Loop start block */
  {    0,    1, 8 },

    /* Iteration 1 */
    {  837,  185, 0 },	/* Pure tone block */
    {    0,    1, 8 },	/* Loop end block */

    /* Iteration 2 */
    {  837,  185, 0 },	/* Pure tone block */
    {    0,    1, 8 },	/* Loop end block */

    /* Iteration 3 */
    {  837,  185, 0 },	/* Pure tone block */
    {    0,    1, 8 },	/* Loop end block */

  /* Stop tape if in 48K mode block */
  {    0,    1, 12 },

  /* Text description block */
  {    0,    1, 8 },

  /* Message block */
  {    0,    1, 8 },

  /* Archive info block */
  {    0,    1, 8 },

  /* Hardware info block */
  {    0,    1, 8 },

  /* Custom info block */
  {    0,    1, 8 },

  /* Pure tone block */
  {  820,  940, 0 },
  {  820,    1, 2 },

  { -1, 0, 0 }		/* End marker */

};

test_return_t
complete_tzx_file( void )
{
  return check_edges( DYNAMIC_TEST_PATH( "complete-tzx.tzx" ), complete_edges_list,
		      LIBSPECTRUM_TAPE_FLAGS_STOP |
		      LIBSPECTRUM_TAPE_FLAGS_STOP48 |
		      TEST_TAPE_FLAGS_NO_EDGE );
}

static test_edge_sequence_t
zero_tail_edges_list[] =
{
  /* Data block with 0 tail */
  {  855,   1,  96 },	/* Data short pulse 1 high */
  {  855,   1,  80 },	/* Data short pulse 2 low */
  {    0,   1,   9 },	/* No edge for tail, end of block */

  /* Pulse block */
  {  200,   1,  32 },	/* Following pulse 1 high */
  {  200,   1, 275 },	/* Following pulse 2 low, end of block, end of tape,
                           stop the tape */

  { -1, 0, 0 }		/* End marker */

};

test_return_t
zero_tail_length_pzx_file( void )
{
  return check_edges( STATIC_TEST_PATH( "zero-tail.pzx" ),
                      zero_tail_edges_list, 0x1ff );
}

static int
same_edge( const libspectrum_tape_edge *a, const libspectrum_tape_edge *b )
{
  return a->tstates == b->tstates && a->level == b->level &&
         a->transition == b->transition && a->flags == b->flags;
}

test_return_t
tape_absolute_edge_levels_follow_tzx_and_pzx( void )
{
  static const libspectrum_dword pzx_tstates[] = { 855, 855, 0, 200, 200 };
  static const libspectrum_tape_signal_level pzx_levels[] = {
    LIBSPECTRUM_TAPE_SIGNAL_HIGH, LIBSPECTRUM_TAPE_SIGNAL_LOW,
    LIBSPECTRUM_TAPE_SIGNAL_LOW, LIBSPECTRUM_TAPE_SIGNAL_HIGH,
    LIBSPECTRUM_TAPE_SIGNAL_LOW
  };
  static const libspectrum_tape_transition pzx_transitions[] = {
    LIBSPECTRUM_TAPE_TRANSITION_FORCE_HIGH,
    LIBSPECTRUM_TAPE_TRANSITION_FORCE_LOW,
    LIBSPECTRUM_TAPE_TRANSITION_NONE,
    LIBSPECTRUM_TAPE_TRANSITION_FORCE_HIGH,
    LIBSPECTRUM_TAPE_TRANSITION_FORCE_LOW
  };
  libspectrum_tape *tape = NULL;
  libspectrum_tape_cursor *cursor = NULL;
  libspectrum_tape_edge live, speculative;
  libspectrum_tape_signal_level level;
  size_t i;
  test_return_t r;

  r = load_tape( &tape, STATIC_TEST_PATH( "no-pilot-gdb.tzx" ),
                 LIBSPECTRUM_ERROR_NONE );
  if( r != TEST_PASS ) return r;
  cursor = libspectrum_tape_cursor_capture( tape );
  if( !cursor || libspectrum_tape_signal_level_get( &level, tape ) ||
      level != LIBSPECTRUM_TAPE_SIGNAL_LOW ||
      libspectrum_tape_get_next_edge( &live, tape ) ||
      libspectrum_tape_cursor_get_next_edge( &speculative, cursor ) ||
      !same_edge( &live, &speculative ) || live.tstates != 0 ||
      live.level != LIBSPECTRUM_TAPE_SIGNAL_HIGH ||
      live.transition != LIBSPECTRUM_TAPE_TRANSITION_FORCE_HIGH ) {
    fprintf( stderr, "%s: TZX Set Signal Level absolute edge mismatch\n",
             progname );
    r = TEST_FAIL;
    goto done;
  }
  {
    libspectrum_tape_block *extra =
      libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
    speculative.tstates = 123; speculative.level = LIBSPECTRUM_TAPE_SIGNAL_LOW;
    speculative.transition = LIBSPECTRUM_TAPE_TRANSITION_TOGGLE;
    speculative.flags = 456;
    if( !extra || libspectrum_tape_append_block( tape, extra ) ||
        libspectrum_tape_cursor_get_next_edge( &speculative, cursor ) !=
          LIBSPECTRUM_ERROR_INVALID || speculative.tstates != 123 ||
        speculative.level != LIBSPECTRUM_TAPE_SIGNAL_LOW ||
        speculative.transition != LIBSPECTRUM_TAPE_TRANSITION_TOGGLE ||
        speculative.flags != 456 ) {
      fprintf( stderr, "%s: failed cursor edge retrieval changed its result\n",
               progname );
      r = TEST_FAIL;
      goto done;
    }
  }
  libspectrum_tape_cursor_free( cursor ); cursor = NULL;
  libspectrum_tape_free( tape ); tape = NULL;

  r = load_tape( &tape, STATIC_TEST_PATH( "zero-tail.pzx" ),
                 LIBSPECTRUM_ERROR_NONE );
  if( r != TEST_PASS ) return r;
  cursor = libspectrum_tape_cursor_capture( tape );
  if( !cursor ) { r = TEST_INCOMPLETE; goto done; }
  for( i = 0; i < sizeof( pzx_tstates ) / sizeof( pzx_tstates[0] ); i++ ) {
    if( libspectrum_tape_get_next_edge( &live, tape ) ||
        libspectrum_tape_cursor_get_next_edge( &speculative, cursor ) ||
        !same_edge( &live, &speculative ) ||
        live.tstates != pzx_tstates[i] || live.level != pzx_levels[i] ||
        live.transition != pzx_transitions[i] ) {
      fprintf( stderr, "%s: PZX absolute edge mismatch at %lu\n", progname,
               (unsigned long)i );
      r = TEST_FAIL;
      goto done;
    }
  }
  r = TEST_PASS;

done:
  if( cursor ) libspectrum_tape_cursor_free( cursor );
  if( tape ) libspectrum_tape_free( tape );
  return r;
}

test_return_t
tape_signal_level_lifecycle_invariants( void )
{
  static const libspectrum_tape_type types[] = {
    LIBSPECTRUM_TAPE_BLOCK_SET_SIGNAL_LEVEL,
    LIBSPECTRUM_TAPE_BLOCK_PAUSE,
    LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,
    LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,
    LIBSPECTRUM_TAPE_BLOCK_PAUSE,
    LIBSPECTRUM_TAPE_BLOCK_PURE_TONE
  };
  static const libspectrum_tape_signal_level levels[] = {
    LIBSPECTRUM_TAPE_SIGNAL_HIGH, LIBSPECTRUM_TAPE_SIGNAL_HIGH,
    LIBSPECTRUM_TAPE_SIGNAL_LOW, LIBSPECTRUM_TAPE_SIGNAL_HIGH,
    LIBSPECTRUM_TAPE_SIGNAL_HIGH, LIBSPECTRUM_TAPE_SIGNAL_LOW
  };
  static const libspectrum_tape_transition transitions[] = {
    LIBSPECTRUM_TAPE_TRANSITION_FORCE_HIGH,
    LIBSPECTRUM_TAPE_TRANSITION_NONE,
    LIBSPECTRUM_TAPE_TRANSITION_TOGGLE,
    LIBSPECTRUM_TAPE_TRANSITION_TOGGLE,
    LIBSPECTRUM_TAPE_TRANSITION_FORCE_HIGH,
    LIBSPECTRUM_TAPE_TRANSITION_FORCE_LOW
  };
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *blocks[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
  libspectrum_tape_edge edge;
  libspectrum_tape_signal_level level;
  size_t i, attached = 0;
  test_return_t r = TEST_FAIL;

  if( !tape ) return TEST_INCOMPLETE;
  for( i = 0; i < sizeof( types ) / sizeof( types[0] ); i++ ) {
    blocks[i] = libspectrum_tape_block_alloc( types[i] );
    if( !blocks[i] ) { r = TEST_INCOMPLETE; goto done; }
  }
  libspectrum_tape_block_set_level( blocks[0], 1 );
  libspectrum_tape_block_set_pause_tstates( blocks[1], 0 );
  for( i = 2; i <= 3; i++ ) {
    libspectrum_tape_block_set_pulse_length( blocks[i], 10 );
    libspectrum_tape_block_set_count( blocks[i], 1 );
  }
  libspectrum_tape_block_set_pause_tstates( blocks[4], 100 );
  libspectrum_tape_block_set_level( blocks[4], 1 );
  libspectrum_tape_block_set_pulse_length( blocks[5], 10 );
  libspectrum_tape_block_set_count( blocks[5], 1 );
  for( i = 0; i < sizeof( blocks ) / sizeof( blocks[0] ); i++ ) {
    if( libspectrum_tape_append_block( tape, blocks[i] ) ) goto done;
    attached++;
  }

  if( libspectrum_tape_signal_level_get( &level, tape ) ||
      level != LIBSPECTRUM_TAPE_SIGNAL_LOW ) goto mismatch;
  for( i = 0; i < sizeof( levels ) / sizeof( levels[0] ); i++ ) {
    if( libspectrum_tape_get_next_edge( &edge, tape ) ||
        edge.level != levels[i] || edge.transition != transitions[i] ) {
      fprintf( stderr, "%s: lifecycle edge %lu got level %d transition %d\n",
               progname, (unsigned long)i, edge.level, edge.transition );
      goto mismatch;
    }
  }
  if( libspectrum_tape_signal_level_get( &level, tape ) ||
      level != LIBSPECTRUM_TAPE_SIGNAL_LOW ) goto mismatch;

  /* Both explicit seek APIs reset low and force the selected block's first
     pulse low; neither inherits the preceding block's high level. */
  if( libspectrum_tape_nth_block( tape, 2 ) ||
      libspectrum_tape_signal_level_get( &level, tape ) ||
      level != LIBSPECTRUM_TAPE_SIGNAL_LOW ||
      libspectrum_tape_get_next_edge( &edge, tape ) ||
      edge.level != LIBSPECTRUM_TAPE_SIGNAL_LOW ||
      edge.transition != LIBSPECTRUM_TAPE_TRANSITION_FORCE_LOW ||
      libspectrum_tape_nth_block( tape, 2 ) ||
      !libspectrum_tape_select_next_block( tape ) ||
      libspectrum_tape_signal_level_get( &level, tape ) ||
      level != LIBSPECTRUM_TAPE_SIGNAL_LOW ||
      libspectrum_tape_get_next_edge( &edge, tape ) ||
      edge.transition != LIBSPECTRUM_TAPE_TRANSITION_FORCE_LOW )
    goto mismatch;

  if( libspectrum_tape_nth_block( tape, 0 ) ||
      libspectrum_tape_get_next_edge( &edge, tape ) ||
      edge.level != LIBSPECTRUM_TAPE_SIGNAL_HIGH ||
      libspectrum_tape_clear( tape ) ||
      libspectrum_tape_signal_level_get( &level, tape ) ||
      level != LIBSPECTRUM_TAPE_SIGNAL_LOW ) goto mismatch;

  r = TEST_PASS;
  goto done;

mismatch:
  fprintf( stderr, "%s: tape signal-level lifecycle invariant failed\n",
           progname );
done:
  for( i = attached; i < sizeof( blocks ) / sizeof( blocks[0] ); i++ )
    if( blocks[i] ) libspectrum_tape_block_free( blocks[i] );
  libspectrum_tape_free( tape );
  return r;
}

static test_edge_sequence_t
no_pilot_gdb_list[] =
{
  /* Set signal level block */
  {    0,   1,  33 },	/* Set signal level high, end of block */

  /* GDB with 0 tail */
  {  771,   1,   0 },	/* Byte 1, bit 1, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 1, pulse 2 */
  {  771,   1,   0 },	/* Byte 1, bit 2, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 2, pulse 2 */
  {  771,   1,   0 },	/* Byte 1, bit 3, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 3, pulse 2 */
  {  771,   1,   0 },	/* Byte 1, bit 4, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 4, pulse 2 */
  {  771,   1,   0 },	/* Byte 1, bit 5, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 5, pulse 2 */
  {  771,   1,   0 },	/* Byte 1, bit 6, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 6, pulse 2 */
  {  771,   1,   0 },	/* Byte 1, bit 7, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 7, pulse 2 */
  {  771,   1,   0 },	/* Byte 1, bit 8, pulse 1 */
  { 1542,   1,   0 },	/* Byte 1, bit 8, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 1, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 1, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 2, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 2, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 3, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 3, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 4, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 4, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 5, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 5, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 6, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 6, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 7, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 7, pulse 2 */
  {  771,   1,   0 },	/* Byte 2, bit 8, pulse 1 */
  { 1542,   1,   0 },	/* Byte 2, bit 8, pulse 2 */
  {    0,   1, 259 },	/* End of block, end of tape, stop the tape (normally no
                           edge but not at end of tape) */

  { -1, 0, 0 }		/* End marker */

};

test_return_t
no_pilot_pulse_gdb_tzx_file( void )
{
  return check_edges( STATIC_TEST_PATH( "no-pilot-gdb.tzx" ),
                      no_pilot_gdb_list, 0x1ff );
}

static test_edge_sequence_t
raw_edges_list[] =
{
  /* RAW block with end of tape edge */
  {    40,   1,  32 },	/* Pulse 1 high */
  {    40,   1,  16 },	/* Pulse 2 low */
  {    40,   1,  32 },	/* Pulse 3 high */
  {    40,   1,  16 },	/* Pulse 4 low */
  {    10,   1,  32 },	/* Pulse 5 high */
  {    10,   1,  16 },	/* Pulse 6 low */
  {    20,   1,  32 },	/* Pulse 7 high */
  {     0,   1, 259 },	/* End of block, end of tape, stop the tape (normally no
                           edge but not at end of tape) */

  { -1, 0, 0 }		/* End marker */

};

/* Test for bugs #369: TZX raw block last edge handling, #444: Spurious
   pulse at the beginning of a raw data block and #445 "Used bits in last
   byte" takes the LSB in raw data blocks*/
test_return_t
read_tzx_raw_block_edge_handling( void )
{
  return check_edges( STATIC_TEST_PATH( "raw-data-block.tzx" ),
                      raw_edges_list, 0xffff );
}

static test_edge_sequence_t
trailing_pause_edges_list[] =
{
  /* Standard speed data block */
  { 2168,    1, 16 },	/* First pilot edge; TZX playback starts low */
  { 2168, 3222,  0 },	/* Remaining pilot edges */
  {  667,    1,  0 },	/* Sync 1 */
  {  735,    1,  0 },	/* Sync 2 */

  { 1710,    2, 0 },	/* Bit 1 */
  { 1710,    2, 0 },	/* Bit 2 */
  { 1710,    2, 0 },	/* Bit 3 */
  { 1710,    2, 0 },	/* Bit 4 */
  { 1710,    2, 0 },	/* Bit 5 */
  { 1710,    2, 0 },	/* Bit 6 */
  { 1710,    2, 0 },	/* Bit 7 */
  { 1710,    2, 0 },	/* Bit 8 */

  { 0, 1, 8 },	/* 0ms Trailing pause End of block, no edge */

  /* 1s Pause block */
  { 3500000, 1, 16 },	/* Low pulse; end of block, tape and playback */

  { -1, 0, 0 }		/* End marker */

};

test_return_t
trailing_pause_block_tzx_file( void )
{
  return check_edges( STATIC_TEST_PATH( "trailing-pause-block.tzx" ),
                      trailing_pause_edges_list,
                      TEST_TAPE_FLAGS_NO_EDGE |
                      TEST_TAPE_FLAGS_LEVEL_LOW |
                      TEST_TAPE_FLAGS_LEVEL_HIGH );
}
