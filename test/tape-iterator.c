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

/* Tape cursors advance independently and can be applied to their source tape. */
test_return_t
tape_cursor_advances_applies_and_invalidates( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape *other = libspectrum_tape_alloc();
  libspectrum_tape_cursor *cursor = NULL, *clone = NULL, *invalidated = NULL;
  libspectrum_tape_block *b1, *b2, *b3;
  libspectrum_dword tstates;
  int flags, tape_pos, cursor_pos, signal_level;
  libspectrum_tape_state_type state;
  test_return_t r = TEST_FAIL;

  if( !tape || !other ) return TEST_INCOMPLETE;

  b1 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b2 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  b3 = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( !b1 || !b2 || !b3 ) goto done;

  libspectrum_tape_block_set_count( b1, 1 );
  libspectrum_tape_block_set_pulse_length( b1, 100 );
  libspectrum_tape_block_set_count( b2, 1 );
  libspectrum_tape_block_set_pulse_length( b2, 200 );
  libspectrum_tape_append_block( tape, b1 );
  libspectrum_tape_append_block( tape, b2 );

  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;

  if( test_tape_cursor_get_next_edge( &tstates, &flags, cursor ) ||
      tstates != 100 || !( flags & LIBSPECTRUM_TAPE_FLAGS_BLOCK ) ) {
    fprintf( stderr, "%s: tape cursor did not return the first block edge\n",
             progname );
    goto done;
  }
  if( libspectrum_tape_cursor_position( &cursor_pos, cursor ) ||
      cursor_pos != 1 ||
      test_tape_cursor_signal_level( &signal_level, cursor ) ||
      signal_level != 0 ) {
    fprintf( stderr, "%s: tape cursor position or signal level is wrong\n",
             progname );
    goto done;
  }
  if( libspectrum_tape_position( &tape_pos, tape ) || tape_pos != 0 ) {
    fprintf( stderr, "%s: tape cursor changed source position to %d\n",
             progname, tape_pos );
    goto done;
  }

  clone = libspectrum_tape_cursor_clone( cursor );
  if( !clone || libspectrum_tape_cursor_position( &cursor_pos, clone ) ||
      cursor_pos != 1 ) {
    fprintf( stderr, "%s: tape cursor clone did not preserve position\n",
             progname );
    goto done;
  }
  if( test_tape_cursor_apply( other, &signal_level, cursor ) !=
      LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: tape cursor applied to the wrong tape\n", progname );
    goto done;
  }
  if( test_tape_cursor_apply( tape, &signal_level, cursor ) ||
      signal_level != 0 || libspectrum_tape_position( &tape_pos, tape ) ||
      tape_pos != 1 ) {
    fprintf( stderr, "%s: applied tape cursor state is wrong\n", progname );
    goto done;
  }

  invalidated = test_tape_cursor_capture( tape, signal_level );
  if( !invalidated ) goto done;
  libspectrum_tape_append_block( tape, b3 );
  b3 = NULL;
  if( test_tape_cursor_get_next_edge( &tstates, &flags, invalidated ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_cursor_position( &cursor_pos, invalidated ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_cursor_state( &state, invalidated ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      test_tape_cursor_signal_level( &signal_level, invalidated ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      test_tape_cursor_apply( tape, &signal_level, invalidated ) !=
        LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: structurally invalidated tape cursor was accepted\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  if( cursor ) libspectrum_tape_cursor_free( cursor );
  if( clone ) libspectrum_tape_cursor_free( clone );
  if( invalidated ) libspectrum_tape_cursor_free( invalidated );
  if( b3 ) libspectrum_tape_block_free( b3 );
  libspectrum_tape_free( tape );
  libspectrum_tape_free( other );
  return r;
}

/* Every successful mutation of a source block invalidates existing cursors. */
test_return_t
tape_cursor_detects_block_mutations( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_cursor *cursor = NULL, *clone = NULL;
  libspectrum_tape_block *blocks[4], *external = NULL;
  libspectrum_dword *old_lengths, *new_lengths;
  libspectrum_dword tstates;
  libspectrum_tape_state_type state;
  int flags, pos, signal_level;
  size_t i;
  test_return_t r = TEST_FAIL;

  if( !tape ) return TEST_INCOMPLETE;

  blocks[0] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  blocks[1] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  blocks[2] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PULSES );
  blocks[3] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_JUMP );
  external = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  for( i = 0; i < 4; i++ ) if( !blocks[i] ) goto done;
  if( !external ) goto done;

  old_lengths = libspectrum_new( libspectrum_dword, 1 );
  old_lengths[0] = 300;
  libspectrum_tape_block_set_count( blocks[0], 1 );
  libspectrum_tape_block_set_pulse_length( blocks[0], 100 );
  libspectrum_tape_block_set_count( blocks[1], 1 );
  libspectrum_tape_block_set_pulse_length( blocks[1], 200 );
  libspectrum_tape_block_set_count( blocks[2], 1 );
  libspectrum_tape_block_set_pulse_lengths( blocks[2], old_lengths );
  libspectrum_tape_block_set_offset( blocks[3], 0 );
  for( i = 0; i < 3; i++ ) libspectrum_tape_append_block( tape, blocks[i] );
  if( libspectrum_tape_insert_block( tape, blocks[3], 3 ) ) goto done;

  /* A generated scalar setter on the current block invalidates every cursor
     operation, before the copied playback state can be used. */
  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;
  libspectrum_tape_block_set_count( blocks[0], 2 );
  if( test_tape_cursor_get_next_edge( &tstates, &flags, cursor ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_cursor_position( &pos, cursor ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_cursor_state( &state, cursor ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      test_tape_cursor_signal_level( &signal_level, cursor ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      test_tape_cursor_apply( tape, &signal_level, cursor ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_cursor_clone( cursor ) != NULL ) {
    fprintf( stderr, "%s: current-block scalar mutation was accepted\n",
             progname );
    goto done;
  }
  libspectrum_tape_cursor_free( cursor ); cursor = NULL;

  /* Strict snapshots include future blocks, not only pointers already reached
     by playback. */
  libspectrum_tape_nth_block( tape, 0 );
  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;
  libspectrum_tape_block_set_pulse_length( blocks[1], 201 );
  if( test_tape_cursor_get_next_edge( &tstates, &flags, cursor ) !=
      LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: future-block mutation was accepted\n", progname );
    goto done;
  }
  libspectrum_tape_cursor_free( cursor ); cursor = NULL;

  /* Control-flow setters are covered even when that block is not reachable
     from the cursor's current path. */
  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;
  libspectrum_tape_block_set_offset( blocks[3], -1 );
  if( test_tape_cursor_signal_level( &signal_level, cursor ) !=
      LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: control-flow mutation was accepted\n", progname );
    goto done;
  }
  libspectrum_tape_cursor_free( cursor ); cursor = NULL;

  /* A pointer-replacing setter may let the caller immediately free the old
     allocation. Both the original cursor and its clone must reject it
     without consulting that stale pointer. */
  libspectrum_tape_nth_block( tape, 2 );
  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;
  clone = libspectrum_tape_cursor_clone( cursor );
  if( !clone ) goto done;
  new_lengths = libspectrum_new( libspectrum_dword, 1 );
  new_lengths[0] = 301;
  libspectrum_tape_block_set_pulse_lengths( blocks[2], new_lengths );
  libspectrum_free( old_lengths );
  old_lengths = NULL;
  if( test_tape_cursor_get_next_edge( &tstates, &flags, cursor ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      test_tape_cursor_get_next_edge( &tstates, &flags, clone ) !=
        LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: pointer mutation was accepted by cursor or clone\n",
             progname );
    goto done;
  }
  libspectrum_tape_cursor_free( cursor ); cursor = NULL;
  libspectrum_tape_cursor_free( clone ); clone = NULL;

  /* Mutating a block which is not in the source tape is unrelated and does
     not invalidate the cursor. */
  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;
  libspectrum_tape_block_set_type( external,
                                   LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  if( libspectrum_tape_cursor_position( &pos, cursor ) || pos != 2 ||
      test_tape_cursor_signal_level( &signal_level, cursor ) ||
      signal_level != 0 ) {
    fprintf( stderr, "%s: unrelated block mutation invalidated cursor\n",
             progname );
    goto done;
  }
  libspectrum_tape_cursor_free( cursor ); cursor = NULL;

  /* set_type is the only hand-written public block setter. */
  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;
  libspectrum_tape_block_set_type( blocks[3], LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( test_tape_cursor_apply( tape, &signal_level, cursor ) !=
      LIBSPECTRUM_ERROR_INVALID ) {
    fprintf( stderr, "%s: block type mutation was accepted\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  if( cursor ) libspectrum_tape_cursor_free( cursor );
  if( clone ) libspectrum_tape_cursor_free( clone );
  if( external ) libspectrum_tape_block_free( external );
  libspectrum_tape_free( tape );
  return r;
}

/* A block cannot be attached to the same or a second tape more than once. */
test_return_t
tape_rejects_duplicate_block_ownership( void )
{
  libspectrum_tape *tape1 = libspectrum_tape_alloc();
  libspectrum_tape *tape2 = libspectrum_tape_alloc();
  libspectrum_tape_cursor *cursor1 = NULL, *cursor2 = NULL;
  libspectrum_tape_block *block;
  int block_attached = 0, signal_level;
  test_return_t r = TEST_FAIL;

  if( !tape1 || !tape2 ) return TEST_INCOMPLETE;

  block = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  if( !block ) goto done;
  libspectrum_tape_block_set_count( block, 1 );
  libspectrum_tape_block_set_pulse_length( block, 100 );
  if( libspectrum_tape_append_block( NULL, block ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_append_block( tape1, NULL ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_insert_block( tape2, NULL, 0 ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_count( tape1 ) != 0 ||
      libspectrum_tape_count( tape2 ) != 0 ) {
    fprintf( stderr, "%s: NULL attachment changed a tape or block\n",
             progname );
    goto done;
  }
  if( libspectrum_tape_append_block( tape1, block ) ) goto done;
  block_attached = 1;

  cursor1 = test_tape_cursor_capture( tape1, 0 );
  cursor2 = test_tape_cursor_capture( tape2, 1 );
  if( !cursor1 || !cursor2 ) goto done;

  /* Append must reject both same-tape and cross-tape duplication without
     changing either tape. */
  if( libspectrum_tape_append_block( tape1, block ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_append_block( tape2, block ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_count( tape1 ) != 1 ||
      libspectrum_tape_count( tape2 ) != 0 ||
      test_tape_cursor_signal_level( &signal_level, cursor1 ) ||
      signal_level != 0 ||
      test_tape_cursor_signal_level( &signal_level, cursor2 ) ||
      signal_level != 0 ) {
    fprintf( stderr, "%s: rejected append changed a tape or cursor\n",
             progname );
    goto done;
  }

  if( libspectrum_tape_insert_block( tape2, block, 0 ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      libspectrum_tape_count( tape1 ) != 1 ||
      libspectrum_tape_count( tape2 ) != 0 ||
      test_tape_cursor_signal_level( &signal_level, cursor1 ) ||
      test_tape_cursor_signal_level( &signal_level, cursor2 ) ) {
    fprintf( stderr, "%s: rejected insert changed a tape or cursor\n",
             progname );
    goto done;
  }

  /* Rejection must not overwrite the original owner. */
  libspectrum_tape_block_set_count( block, 2 );
  if( test_tape_cursor_signal_level( &signal_level, cursor1 ) !=
        LIBSPECTRUM_ERROR_INVALID ||
      test_tape_cursor_signal_level( &signal_level, cursor2 ) ) {
    fprintf( stderr, "%s: rejected attachment changed block ownership\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  if( cursor1 ) libspectrum_tape_cursor_free( cursor1 );
  if( cursor2 ) libspectrum_tape_cursor_free( cursor2 );
  if( block && !block_attached ) libspectrum_tape_block_free( block );
  libspectrum_tape_free( tape2 );
  libspectrum_tape_free( tape1 );
  return r;
}

/* Tape cursors can represent partial and complete standard ROM data. */
test_return_t
tape_cursor_tracks_partial_and_full_rom_blocks( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_cursor *partial = NULL, *full = NULL;
  libspectrum_tape_block *rom, *following;
  libspectrum_tape_state_type state;
  libspectrum_byte *data;
  libspectrum_dword tstates;
  size_t i, pilot_edges = 3223;
  int flags, pos, signal_level;
  test_return_t r = TEST_FAIL;

  if( !tape ) return TEST_INCOMPLETE;

  rom = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_ROM );
  following = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( !rom || !following ) goto done;

  data = libspectrum_malloc( 2 );
  data[0] = 0x80; data[1] = 0x00;
  libspectrum_tape_block_set_data_length( rom, 2 );
  libspectrum_tape_block_set_data( rom, data );
  libspectrum_tape_block_set_pause_tstates( rom, 3500000 );
  libspectrum_tape_append_block( tape, rom );
  libspectrum_tape_append_block( tape, following );

  partial = test_tape_cursor_capture( tape, 0 );
  full = test_tape_cursor_capture( tape, 0 );
  if( !partial || !full ) goto done;

  /* Data-flag blocks have 3223 pilot edges, two sync edges and sixteen
     edges per complete byte. */
  for( i = 0; i < pilot_edges + 2 + 16; i++ ) {
    if( test_tape_cursor_get_next_edge( &tstates, &flags, partial ) )
      goto done;
  }
  if( libspectrum_tape_cursor_state( &state, partial ) ||
      state != LIBSPECTRUM_TAPE_STATE_DATA1 ||
      test_tape_cursor_apply( tape, &signal_level, partial ) ||
      signal_level != 0 ||
      libspectrum_tape_state( tape ) != LIBSPECTRUM_TAPE_STATE_DATA1 ) {
    fprintf( stderr, "%s: partial ROM cursor did not resume at next byte\n",
             progname );
    goto done;
  }
  if( test_tape_get_next_edge( &tstates, &flags, tape ) ||
      tstates != 855 ) {
    fprintf( stderr, "%s: partial ROM cursor returned wrong next edge\n",
             progname );
    goto done;
  }

  /* Generic edge traversal reaches the trailing pause and records its
     post-data handoff polarity as part of the complete cursor state. */
  for( i = 0; i < pilot_edges + 2 + 32; i++ ) {
    if( test_tape_cursor_get_next_edge( &tstates, &flags, full ) )
      goto done;
  }
  if( libspectrum_tape_cursor_state( &state, full ) ||
      state != LIBSPECTRUM_TAPE_STATE_PAUSE ||
      test_tape_cursor_apply( tape, &signal_level, full ) ||
      signal_level != 0 ||
      libspectrum_tape_state( tape ) != LIBSPECTRUM_TAPE_STATE_PAUSE ) {
    fprintf( stderr, "%s: full ROM cursor did not reach trailing pause\n",
             progname );
    goto done;
  }
  if( test_tape_get_next_edge( &tstates, &flags, tape ) ||
      tstates != 3500000 || !( flags & LIBSPECTRUM_TAPE_FLAGS_BLOCK ) ||
      libspectrum_tape_position( &pos, tape ) || pos != 1 ) {
    fprintf( stderr, "%s: full ROM cursor did not preserve block transition\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  if( partial ) libspectrum_tape_cursor_free( partial );
  if( full ) libspectrum_tape_cursor_free( full );
  libspectrum_tape_free( tape );
  return r;
}

/* Cursor control flow must not modify the source tape. */
test_return_t
tape_cursor_processes_jump_and_loop_blocks( void )
{
  libspectrum_tape *tape = libspectrum_tape_alloc();
  libspectrum_tape_cursor *cursor = NULL;
  libspectrum_tape_block *blocks[6];
  libspectrum_dword tstates;
  int flags, tape_pos, cursor_pos, i;
  test_return_t r = TEST_FAIL;

  if( !tape ) return TEST_INCOMPLETE;

  blocks[0] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_JUMP );
  blocks[1] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  blocks[2] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_LOOP_START );
  blocks[3] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  blocks[4] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_LOOP_END );
  blocks[5] = libspectrum_tape_block_alloc( LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  for( i = 0; i < 6; i++ ) if( !blocks[i] ) goto done;

  libspectrum_tape_block_set_offset( blocks[0], 2 );
  libspectrum_tape_block_set_count( blocks[2], 2 );
  libspectrum_tape_block_set_count( blocks[3], 1 );
  libspectrum_tape_block_set_pulse_length( blocks[3], 100 );
  libspectrum_tape_block_set_count( blocks[5], 1 );
  libspectrum_tape_block_set_pulse_length( blocks[5], 200 );
  for( i = 0; i < 6; i++ ) libspectrum_tape_append_block( tape, blocks[i] );

  cursor = test_tape_cursor_capture( tape, 0 );
  if( !cursor ) goto done;

  /* Jump to loop start, execute the one-pulse body twice, then continue. */
  for( i = 0; i < 6; i++ ) {
    if( test_tape_cursor_get_next_edge( &tstates, &flags, cursor ) )
      goto done;
  }
  if( libspectrum_tape_cursor_position( &cursor_pos, cursor ) ||
      cursor_pos != 5 || libspectrum_tape_position( &tape_pos, tape ) ||
      tape_pos != 0 ) {
    fprintf( stderr, "%s: cursor jump/loop changed the wrong playback state\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  if( cursor ) libspectrum_tape_cursor_free( cursor );
  libspectrum_tape_free( tape );
  return r;
}
