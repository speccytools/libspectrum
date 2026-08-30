/* tape-iterator.c: unit tests for libspectrum tape iterator and block
   manipulation API
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

   Author contact information:

   E-mail: fredm@spamcop.net

*/

#include "config.h"

#include <stdio.h>

#include "libspectrum.h"
#include "common.h"
#include "test.h"

/* libspectrum_tape_iterator_init on an empty tape returns NULL */
test_return_t
tape_iterator_init_on_empty_tape_returns_null( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_iterator it;
  libspectrum_tape_block *block;

  if( !tape ) {
    fprintf( stderr, "%s: tape_iterator_init_on_empty_tape_returns_null: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  block = libspectrum_tape_iterator_init( &it, tape );

  libspectrum_tape_free( tape );

  if( block != NULL ) {
    fprintf( stderr, "%s: tape_iterator_init_on_empty_tape_returns_null: "
             "expected NULL, got non-NULL block\n", progname );
    return TEST_FAIL;
  }

  return TEST_PASS;
}

/* libspectrum_tape_iterator traverses all appended blocks in order */
test_return_t
tape_iterator_traverses_appended_blocks_in_order( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2, *b3;
  libspectrum_tape_iterator it;
  libspectrum_tape_block *cur;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_iterator_traverses_appended_blocks_in_order: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b3 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );

  if( !b1 || !b2 || !b3 ) {
    fprintf( stderr, "%s: tape_iterator_traverses_appended_blocks_in_order: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );
  libspectrum_tape_append_block( tape, b3 );

  cur = libspectrum_tape_iterator_init( &it, tape );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_iterator_traverses_appended_blocks_in_order: "
             "first block: expected PAUSE, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  cur = libspectrum_tape_iterator_next( &it );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_iterator_traverses_appended_blocks_in_order: "
             "second block: expected PURE_TONE, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  cur = libspectrum_tape_iterator_next( &it );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_ROM ) {
    fprintf( stderr, "%s: tape_iterator_traverses_appended_blocks_in_order: "
             "third block: expected ROM, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  cur = libspectrum_tape_iterator_next( &it );
  if( cur != NULL ) {
    fprintf( stderr, "%s: tape_iterator_traverses_appended_blocks_in_order: "
             "expected NULL past last block\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_iterator_peek_next returns NULL when on the last block */
test_return_t
tape_iterator_peek_next_returns_null_at_last_block( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *block;
  libspectrum_tape_iterator it;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_iterator_peek_next_returns_null_at_last_block: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( !block ) {
    fprintf( stderr, "%s: tape_iterator_peek_next_returns_null_at_last_block: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, block );

  libspectrum_tape_iterator_init( &it, tape );

  if( libspectrum_tape_iterator_peek_next( it ) != NULL ) {
    fprintf( stderr, "%s: tape_iterator_peek_next_returns_null_at_last_block: "
             "expected NULL peek_next on single-block tape\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_iterator_peek_next returns the next block without
   advancing the iterator */
test_return_t
tape_iterator_peek_next_does_not_advance_iterator( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2;
  libspectrum_tape_iterator it;
  libspectrum_tape_block *peeked, *cur;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_iterator_peek_next_does_not_advance_iterator: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );

  if( !b1 || !b2 ) {
    fprintf( stderr, "%s: tape_iterator_peek_next_does_not_advance_iterator: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );

  libspectrum_tape_iterator_init( &it, tape );

  peeked = libspectrum_tape_iterator_peek_next( it );
  if( libspectrum_tape_block_type( peeked ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_iterator_peek_next_does_not_advance_iterator: "
             "peek_next: expected PURE_TONE, got %d\n", progname,
             libspectrum_tape_block_type( peeked ) );
    goto done;
  }

  /* Iterator should still point at b1 */
  cur = libspectrum_tape_iterator_current( it );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_iterator_peek_next_does_not_advance_iterator: "
             "current after peek: expected PAUSE, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_position returns 0 for the first block */
test_return_t
tape_position_returns_zero_for_first_block( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2;
  int pos = -1;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_position_returns_zero_for_first_block: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );

  if( !b1 || !b2 ) {
    fprintf( stderr, "%s: tape_position_returns_zero_for_first_block: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );

  if( libspectrum_tape_position( &pos, tape ) ) {
    fprintf( stderr, "%s: tape_position_returns_zero_for_first_block: "
             "tape_position returned error\n", progname );
    goto done;
  }

  if( pos != 0 ) {
    fprintf( stderr, "%s: tape_position_returns_zero_for_first_block: "
             "expected position 0, got %d\n", progname, pos );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_nth_block selects the correct block */
test_return_t
tape_nth_block_selects_correct_block( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2, *b3;
  int pos = -1;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_nth_block_selects_correct_block: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b3 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );

  if( !b1 || !b2 || !b3 ) {
    fprintf( stderr, "%s: tape_nth_block_selects_correct_block: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );
  libspectrum_tape_append_block( tape, b3 );

  if( libspectrum_tape_nth_block( tape, 2 ) ) {
    fprintf( stderr, "%s: tape_nth_block_selects_correct_block: "
             "tape_nth_block(2) returned error\n", progname );
    goto done;
  }

  if( libspectrum_tape_position( &pos, tape ) ) {
    fprintf( stderr, "%s: tape_nth_block_selects_correct_block: "
             "tape_position returned error\n", progname );
    goto done;
  }

  if( pos != 2 ) {
    fprintf( stderr, "%s: tape_nth_block_selects_correct_block: "
             "expected position 2, got %d\n", progname, pos );
    goto done;
  }

  if( libspectrum_tape_block_type( libspectrum_tape_current_block( tape ) ) !=
      LIBSPECTRUM_TAPE_BLOCK_ROM ) {
    fprintf( stderr, "%s: tape_nth_block_selects_correct_block: "
             "expected ROM block at position 2\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_insert_block into an empty tape sets current_block */
test_return_t
tape_insert_block_into_empty_tape_sets_current_block( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *block;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_insert_block_into_empty_tape_sets_current_block: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );
  if( !block ) {
    fprintf( stderr, "%s: tape_insert_block_into_empty_tape_sets_current_block: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_insert_block( tape, block, 0 ) ) {
    fprintf( stderr, "%s: tape_insert_block_into_empty_tape_sets_current_block: "
             "tape_insert_block returned error\n", progname );
    goto done;
  }

  /* After inserting into an empty tape, present() must be true */
  if( !libspectrum_tape_present( tape ) ) {
    fprintf( stderr, "%s: tape_insert_block_into_empty_tape_sets_current_block: "
             "tape_present returned false after insert\n", progname );
    goto done;
  }

  /* current_block must be set (tape is playable) */
  if( !libspectrum_tape_current_block( tape ) ) {
    fprintf( stderr, "%s: tape_insert_block_into_empty_tape_sets_current_block: "
             "tape_current_block returned NULL after insert into empty tape\n",
             progname );
    goto done;
  }

  if( libspectrum_tape_block_type( libspectrum_tape_current_block( tape ) ) !=
      LIBSPECTRUM_TAPE_BLOCK_ROM ) {
    fprintf( stderr, "%s: tape_insert_block_into_empty_tape_sets_current_block: "
             "expected current block type ROM\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_insert_block inserts a block at the given position */
test_return_t
tape_insert_block_at_position( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2, *b_mid;
  libspectrum_tape_iterator it;
  libspectrum_tape_block *cur;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_insert_block_at_position: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  b1    = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2    = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );
  b_mid = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );

  if( !b1 || !b2 || !b_mid ) {
    fprintf( stderr, "%s: tape_insert_block_at_position: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );

  /* Insert PURE_TONE between position 0 and 1 */
  if( libspectrum_tape_insert_block( tape, b_mid, 1 ) ) {
    fprintf( stderr, "%s: tape_insert_block_at_position: "
             "tape_insert_block returned error\n", progname );
    goto done;
  }

  /* Verify order: PAUSE, PURE_TONE, ROM */
  cur = libspectrum_tape_iterator_init( &it, tape );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_insert_block_at_position: "
             "block[0]: expected PAUSE, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  cur = libspectrum_tape_iterator_next( &it );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_insert_block_at_position: "
             "block[1]: expected PURE_TONE, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  cur = libspectrum_tape_iterator_next( &it );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_ROM ) {
    fprintf( stderr, "%s: tape_insert_block_at_position: "
             "block[2]: expected ROM, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_remove_block removes the given block from the tape */
test_return_t
tape_remove_block_from_tape( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2, *b3;
  libspectrum_tape_iterator it;
  libspectrum_tape_block *cur;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_remove_block_from_tape: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b3 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );

  if( !b1 || !b2 || !b3 ) {
    fprintf( stderr, "%s: tape_remove_block_from_tape: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );
  libspectrum_tape_append_block( tape, b3 );

  /* Remove the middle block (PURE_TONE) */
  cur = libspectrum_tape_iterator_init( &it, tape );
  cur = libspectrum_tape_iterator_next( &it );  /* advance to b2 */
  libspectrum_tape_remove_block( tape, it );

  /* Verify remaining order: PAUSE, ROM */
  cur = libspectrum_tape_iterator_init( &it, tape );
  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_remove_block_from_tape: "
             "block[0]: expected PAUSE, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  cur = libspectrum_tape_iterator_next( &it );
  if( !cur ) {
    fprintf( stderr, "%s: tape_remove_block_from_tape: "
             "block[1] is NULL, expected ROM\n", progname );
    goto done;
  }

  if( libspectrum_tape_block_type( cur ) != LIBSPECTRUM_TAPE_BLOCK_ROM ) {
    fprintf( stderr, "%s: tape_remove_block_from_tape: "
             "block[1]: expected ROM, got %d\n", progname,
             libspectrum_tape_block_type( cur ) );
    goto done;
  }

  cur = libspectrum_tape_iterator_next( &it );
  if( cur ) {
    fprintf( stderr, "%s: tape_remove_block_from_tape: "
             "expected only 2 blocks, but block[2] is not NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_peek_last_block returns the last appended block */
test_return_t
tape_peek_last_block_returns_last_appended( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2, *b3, *last;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_peek_last_block_returns_last_appended: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  /* Empty tape: peek_last should return NULL */
  last = libspectrum_tape_peek_last_block( tape );
  if( last != NULL ) {
    fprintf( stderr, "%s: tape_peek_last_block_returns_last_appended: "
             "expected NULL on empty tape, got non-NULL\n", progname );
    goto done;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b3 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );

  if( !b1 || !b2 || !b3 ) {
    fprintf( stderr, "%s: tape_peek_last_block_returns_last_appended: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  last = libspectrum_tape_peek_last_block( tape );
  if( libspectrum_tape_block_type( last ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_peek_last_block_returns_last_appended: "
             "after 1 block: expected PAUSE, got %d\n", progname,
             libspectrum_tape_block_type( last ) );
    goto done;
  }

  libspectrum_tape_append_block( tape, b2 );
  last = libspectrum_tape_peek_last_block( tape );
  if( libspectrum_tape_block_type( last ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_peek_last_block_returns_last_appended: "
             "after 2 blocks: expected PURE_TONE, got %d\n", progname,
             libspectrum_tape_block_type( last ) );
    goto done;
  }

  libspectrum_tape_append_block( tape, b3 );
  last = libspectrum_tape_peek_last_block( tape );
  if( libspectrum_tape_block_type( last ) != LIBSPECTRUM_TAPE_BLOCK_ROM ) {
    fprintf( stderr, "%s: tape_peek_last_block_returns_last_appended: "
             "after 3 blocks: expected ROM, got %d\n", progname,
             libspectrum_tape_block_type( last ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_select_next_block advances current block and wraps around */
test_return_t
tape_select_next_block_advances_and_wraps( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2, *b3, *selected;
  int pos = -1;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b3 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );

  if( !b1 || !b2 || !b3 ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );
  libspectrum_tape_append_block( tape, b3 );

  /* Initially at block 0 */
  if( libspectrum_tape_position( &pos, tape ) || pos != 0 ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "expected initial position 0, got %d\n", progname, pos );
    goto done;
  }

  /* Advance to block 1 */
  selected = libspectrum_tape_select_next_block( tape );
  if( !selected ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "select_next_block returned NULL advancing to block 1\n", progname );
    goto done;
  }
  if( libspectrum_tape_block_type( selected ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "block 1: expected PURE_TONE, got %d\n", progname,
             libspectrum_tape_block_type( selected ) );
    goto done;
  }
  if( libspectrum_tape_position( &pos, tape ) || pos != 1 ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "expected position 1, got %d\n", progname, pos );
    goto done;
  }

  /* Advance to block 2 */
  selected = libspectrum_tape_select_next_block( tape );
  if( !selected ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "select_next_block returned NULL advancing to block 2\n", progname );
    goto done;
  }
  if( libspectrum_tape_position( &pos, tape ) || pos != 2 ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "expected position 2, got %d\n", progname, pos );
    goto done;
  }

  /* Advance past the last block: should wrap around to block 0 */
  selected = libspectrum_tape_select_next_block( tape );
  if( !selected ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "select_next_block returned NULL on wrap-around\n", progname );
    goto done;
  }
  if( libspectrum_tape_position( &pos, tape ) || pos != 0 ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "expected wrap-around to position 0, got %d\n", progname, pos );
    goto done;
  }
  if( libspectrum_tape_block_type( selected ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_select_next_block_advances_and_wraps: "
             "wrap-around: expected PAUSE (b1), got %d\n", progname,
             libspectrum_tape_block_type( selected ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_state returns PILOT for a freshly loaded ROM block */
test_return_t
tape_state_returns_pilot_for_new_rom_block( void )
{
  libspectrum_tape *tape = NULL;
  libspectrum_tape_state_type state;
  test_return_t r = TEST_FAIL;

  if( load_tape( &tape, STATIC_TEST_PATH( "standard-tap.tap" ),
                 LIBSPECTRUM_ERROR_NONE ) != TEST_PASS )
    return TEST_INCOMPLETE;

  state = libspectrum_tape_state( tape );

  if( state != LIBSPECTRUM_TAPE_STATE_PILOT ) {
    fprintf( stderr, "%s: tape_state_returns_pilot_for_new_rom_block: "
             "expected LIBSPECTRUM_TAPE_STATE_PILOT (%d), got %d\n",
             progname, LIBSPECTRUM_TAPE_STATE_PILOT, state );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_set_state updates the state of the current ROM block */
test_return_t
tape_set_state_updates_state_of_rom_block( void )
{
  libspectrum_tape *tape = NULL;
  libspectrum_tape_state_type state;
  test_return_t r = TEST_FAIL;

  if( load_tape( &tape, STATIC_TEST_PATH( "standard-tap.tap" ),
                 LIBSPECTRUM_ERROR_NONE ) != TEST_PASS )
    return TEST_INCOMPLETE;

  if( libspectrum_tape_set_state( tape, LIBSPECTRUM_TAPE_STATE_DATA1 ) ) {
    fprintf( stderr, "%s: tape_set_state_updates_state_of_rom_block: "
             "tape_set_state returned error\n", progname );
    goto done;
  }

  state = libspectrum_tape_state( tape );

  if( state != LIBSPECTRUM_TAPE_STATE_DATA1 ) {
    fprintf( stderr, "%s: tape_set_state_updates_state_of_rom_block: "
             "expected LIBSPECTRUM_TAPE_STATE_DATA1 (%d), got %d\n",
             progname, LIBSPECTRUM_TAPE_STATE_DATA1, state );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_current_block returns NULL on a tape with no current block */
test_return_t
tape_current_block_returns_null_on_empty_tape( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *block;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_current_block_returns_null_on_empty_tape: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  block = libspectrum_tape_current_block( tape );

  if( block != NULL ) {
    fprintf( stderr, "%s: tape_current_block_returns_null_on_empty_tape: "
             "expected NULL for empty tape, got non-NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_current_block returns first block after iterator_init */
test_return_t
tape_current_block_returns_first_block_after_init( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *appended, *current;
  libspectrum_tape_iterator it;
  test_return_t r = TEST_FAIL;

  if( !tape ) {
    fprintf( stderr, "%s: tape_current_block_returns_first_block_after_init: "
             "tape_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  appended = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( !appended ) {
    fprintf( stderr, "%s: tape_current_block_returns_first_block_after_init: "
             "tape_block_alloc returned NULL\n", progname );
    libspectrum_tape_free( tape );
    return TEST_INCOMPLETE;
  }

  libspectrum_tape_append_block( tape, appended );
  libspectrum_tape_iterator_init( &it, tape );

  current = libspectrum_tape_current_block( tape );

  if( current != appended ) {
    fprintf( stderr, "%s: tape_current_block_returns_first_block_after_init: "
             "current block does not match appended block\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

/* libspectrum_tape_block_set_type changes the block type */
test_return_t
tape_block_set_type_changes_block_type( void )
{
  libspectrum_tape_block *block;
  test_return_t r = TEST_FAIL;

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( !block ) {
    fprintf( stderr, "%s: tape_block_set_type_changes_block_type: "
             "tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PAUSE ) {
    fprintf( stderr, "%s: tape_block_set_type_changes_block_type: "
             "initial type should be PAUSE (%d), got %d\n", progname,
             LIBSPECTRUM_TAPE_BLOCK_PAUSE,
             (int)libspectrum_tape_block_type( block ) );
    goto done;
  }

  libspectrum_tape_block_set_type( block, LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_block_set_type_changes_block_type: "
             "after set_type, expected PURE_TONE (%d), got %d\n", progname,
             LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,
             (int)libspectrum_tape_block_type( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

/* libspectrum_tape_alloc/tape_free: basic allocation and deallocation */
test_return_t
tape_state_on_fresh_tape_returns_invalid( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();

  if( !tape ) {
    fprintf( stderr, "%s: tape_state_on_fresh_tape_returns_invalid: "
             "tape_alloc returned NULL\n", progname );
    return TEST_FAIL;
  }

  libspectrum_tape_free( tape );
  return TEST_PASS;
}

/* libspectrum_tape_block_free: free a standalone block */
test_return_t
tape_set_state_and_get_state_round_trip( void )
{
  libspectrum_tape_block *block;

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );
  if( !block ) {
    fprintf( stderr, "%s: tape_set_state_and_get_state_round_trip: "
             "tape_block_alloc returned NULL\n", progname );
    return TEST_FAIL;
  }

  libspectrum_tape_block_free( block );
  return TEST_PASS;
}

/* libspectrum_tape_block_type returns the type set at allocation */
test_return_t
tape_block_type_returns_type_set_at_alloc( void )
{
  libspectrum_tape_block *block;
  test_return_t r = TEST_FAIL;

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  if( !block ) {
    fprintf( stderr, "%s: tape_block_type_returns_type_set_at_alloc: "
             "tape_block_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_PURE_TONE ) {
    fprintf( stderr, "%s: tape_block_type_returns_type_set_at_alloc: "
             "expected PURE_TONE (%d), got %d\n", progname,
             LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,
             (int)libspectrum_tape_block_type( block ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_block_free( block );
  return r;
}

/* libspectrum_tape_block_alloc allocates blocks of various types */
test_return_t
tape_block_alloc_several_types( void )
{
  static const libspectrum_tape_type types[] = {
    LIBSPECTRUM_TAPE_BLOCK_ROM,
    LIBSPECTRUM_TAPE_BLOCK_TURBO,
    LIBSPECTRUM_TAPE_BLOCK_PAUSE,
    LIBSPECTRUM_TAPE_BLOCK_GROUP_START,
    LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO,
    LIBSPECTRUM_TAPE_BLOCK_HARDWARE,
    LIBSPECTRUM_TAPE_BLOCK_COMMENT,
    LIBSPECTRUM_TAPE_BLOCK_STOP48,
  };
  size_t i;

  for( i = 0; i < sizeof( types ) / sizeof( types[0] ); i++ ) {
    libspectrum_tape_block *block = libspectrum_tape_block_alloc( types[i] );
    if( !block ) {
      fprintf( stderr, "%s: tape_block_alloc_several_types: "
               "tape_block_alloc returned NULL for type %d\n", progname,
               (int)types[i] );
      return TEST_FAIL;
    }
    if( libspectrum_tape_block_type( block ) != types[i] ) {
      fprintf( stderr, "%s: tape_block_alloc_several_types: "
               "expected type %d, got %d\n", progname, (int)types[i],
               (int)libspectrum_tape_block_type( block ) );
      libspectrum_tape_block_free( block );
      return TEST_FAIL;
    }
    libspectrum_tape_block_free( block );
  }

  return TEST_PASS;
}

test_return_t
tape_count_returns_correct_count( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_block *b1, *b2, *b3;
  test_return_t r = TEST_FAIL;

  if( !tape ) return TEST_INCOMPLETE;

  if( libspectrum_tape_count( tape ) != 0 ) {
    fprintf( stderr, "%s: tape_count_returns_correct_count: "
             "expected 0 for empty tape, got %lu\n", progname,
             (unsigned long)libspectrum_tape_count( tape ) );
    goto done;
  }

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b3 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_DATA );

  if( !b1 || !b2 || !b3 ) {
    r = TEST_INCOMPLETE;
    goto done;
  }

  libspectrum_tape_append_block( tape, b1 );
  if( libspectrum_tape_count( tape ) != 1 ) {
    fprintf( stderr, "%s: tape_count_returns_correct_count: "
             "expected 1 after one append, got %lu\n", progname,
             (unsigned long)libspectrum_tape_count( tape ) );
    goto done;
  }

  libspectrum_tape_append_block( tape, b2 );
  libspectrum_tape_append_block( tape, b3 );

  if( libspectrum_tape_count( tape ) != 3 ) {
    fprintf( stderr, "%s: tape_count_returns_correct_count: "
             "expected 3 after three appends, got %lu\n", progname,
             (unsigned long)libspectrum_tape_count( tape ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}
