#include "config.h"

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "internals.h"
#include "test.h"

/* Test for bug #102: invalid compressed file causes crash */
test_return_t
invalid_compressed_file_1( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  const char *filename = STATIC_TEST_PATH( "invalid.gz" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_UNKNOWN );
}

/* Further test for bug #102: invalid compressed file causes crash */
test_return_t
invalid_compressed_file_2( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return read_snap( STATIC_TEST_PATH( "invalid.gz" ), NULL, LIBSPECTRUM_ERROR_UNKNOWN );
}

/* Test for bug #103: pointer wraparound causes segfault */
test_return_t
pointer_wraparound_in_szx_file( void )
{
  const char *filename = STATIC_TEST_PATH( "invalid.szx" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_CORRUPT );
}

/* Tests for bug #129: SP not sanity checked when reading .sna files;
   also tests bug #130: compressed snapshots cause segfault */
test_return_t
sna_file_with_sp_0x4000( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  const char *filename = STATIC_TEST_PATH( "sp-2000.sna.gz" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_CORRUPT );
}

test_return_t
sna_file_with_sp_0xffff( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  const char *filename = STATIC_TEST_PATH( "sp-ffff.sna.gz" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_CORRUPT );
}

test_return_t
writing_sna_file( void )
{
  const char *filename = STATIC_TEST_PATH( "empty.z80" );
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0, length = 0;
  libspectrum_snap *snap;
  int flags;
  test_return_t r = TEST_INCOMPLETE;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  snap = libspectrum_snap_alloc();

  if( libspectrum_snap_read( snap, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
                             filename ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: reading `%s' failed\n", progname, filename );
    libspectrum_snap_free( snap );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );
  buffer = NULL;

  if( libspectrum_snap_write( &buffer, &length, &flags, snap,
                              LIBSPECTRUM_ID_SNAPSHOT_SNA, NULL, 0 ) !=
      LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: serialising to SNA failed\n", progname );
    libspectrum_snap_free( snap );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_free( snap );
  snap = libspectrum_snap_alloc();

  if( libspectrum_snap_read( snap, buffer, length, LIBSPECTRUM_ID_SNAPSHOT_SNA,
                             NULL ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: restoring from SNA failed\n", progname );
    libspectrum_snap_free( snap );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  if( libspectrum_snap_pc( snap ) != 0x1234 ) {
    fprintf( stderr, "%s: PC is 0x%04x, not the expected 0x1234\n", progname,
             libspectrum_snap_pc( snap ) );
    r = TEST_FAIL;
  } else if( libspectrum_snap_sp( snap ) != 0x8000 ) {
    fprintf( stderr, "%s: SP is 0x%04x, not the expected 0x8000\n", progname,
             libspectrum_snap_sp( snap ) );
    r = TEST_FAIL;
  } else {
    r = TEST_PASS;
  }

  libspectrum_snap_free( snap );

  return r;
}

/* Tests for bug #198: last out to 0x1ffd is not serialised into .z80 files */
test_return_t
writing_plus_3_z80_file( void )
{
  const char *filename = STATIC_TEST_PATH( "plus3.z80" );
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0, length = 0;
  libspectrum_snap *snap;
  int flags;
  test_return_t r = TEST_INCOMPLETE;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  snap = libspectrum_snap_alloc();

  if( libspectrum_snap_read( snap, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
                             filename ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: reading `%s' failed\n", progname, filename );
    libspectrum_snap_free( snap );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );
  buffer = NULL;

  if( libspectrum_snap_write( &buffer, &length, &flags, snap,
                              LIBSPECTRUM_ID_SNAPSHOT_Z80, NULL, 0 ) !=
      LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: serialising to Z80 failed\n", progname );
    libspectrum_snap_free( snap );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_free( snap );
  snap = libspectrum_snap_alloc();

  if( libspectrum_snap_read( snap, buffer, length, LIBSPECTRUM_ID_SNAPSHOT_Z80,
                             NULL ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: restoring from Z80 failed\n", progname );
    libspectrum_snap_free( snap );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_out_plus3_memoryport( snap ) == 0xaa ) {
    r = TEST_PASS;
  } else {
    fprintf( stderr,
             "%s: Last out to 0x1ffd is 0x%02x, not the expected 0xaa\n",
             progname, libspectrum_snap_out_plus3_memoryport( snap ) );
    r = TEST_FAIL;
  }

  libspectrum_snap_free( snap );

  return r;
}

static test_return_t
check_scorpion_z80_pages( int first_page, int page_count,
                           libspectrum_error expected_error )
{
  const size_t header_length = 87;
  const size_t compressed_length = 260;
  size_t length, offset;
  libspectrum_byte *buffer;
  libspectrum_snap *snap;
  libspectrum_error error;
  test_return_t r = TEST_PASS;
  int i, j;

  length = header_length + page_count * ( 3 + compressed_length );
  buffer = libspectrum_new0( libspectrum_byte, length );

  /* A minimal version 3 Z80 header for a Scorpion ZS 256. */
  buffer[30] = 55;
  buffer[32] = 0x34;
  buffer[33] = 0x12;
  buffer[34] = 10;

  offset = header_length;
  for( i = 0; i < page_count; i++ ) {
    int bank = ( i * 5 ) % 16;
    int remaining = 0x4000;

    buffer[offset    ] = compressed_length & 0xff;
    buffer[offset + 1] = compressed_length >> 8;
    buffer[offset + 2] = first_page + bank;
    offset += 3;

    for( j = 0; j < 65; j++ ) {
      int run_length = remaining > 255 ? 255 : remaining;

      buffer[offset++] = 0xed;
      buffer[offset++] = 0xed;
      buffer[offset++] = run_length;
      buffer[offset++] = bank;
      remaining -= run_length;
    }
  }

  snap = libspectrum_snap_alloc();
  error = libspectrum_snap_read( snap, buffer, length,
                                 LIBSPECTRUM_ID_SNAPSHOT_Z80, NULL );

  if( error != expected_error ) {
    fprintf( stderr, "%s: reading Scorpion Z80 returned %d, expected %d\n",
             progname, error, expected_error );
    r = TEST_FAIL;
  } else if( error == LIBSPECTRUM_ERROR_NONE ) {
    for( i = 0; i < 16; i++ ) {
      libspectrum_byte *page = libspectrum_snap_pages( snap, i );

      if( !page || page[0] != i || page[0x3fff] != i ) {
        fprintf( stderr, "%s: Scorpion RAM page %d was mapped incorrectly\n",
                 progname, i );
        r = TEST_FAIL;
        break;
      }
    }
  }

  libspectrum_snap_free( snap );
  libspectrum_free( buffer );
  return r;
}

/* Spectaculator 6.1 used page identifiers 0--15 for Scorpion snapshots. */
test_return_t
reading_spectaculator_61_scorpion_z80_pages( void )
{
  test_return_t r;

  r = check_scorpion_z80_pages( 0, 16, LIBSPECTRUM_ERROR_NONE );
  if( r != TEST_PASS ) return r;

  /* The conventional 3--18 mapping must remain unchanged. */
  r = check_scorpion_z80_pages( 3, 16, LIBSPECTRUM_ERROR_NONE );
  if( r != TEST_PASS ) return r;

  /* Page 0 alone is not enough to identify the Spectaculator workaround. */
  return check_scorpion_z80_pages( 0, 15, LIBSPECTRUM_ERROR_UNKNOWN );
}

/* Tests for bug #184: SZX files were written with A and F reversed */
test_return_t
reading_old_szx_file( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  const char *filename = STATIC_TEST_PATH( "empty.szx" );
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_snap *snap;
  test_return_t r = TEST_INCOMPLETE;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  snap = libspectrum_snap_alloc();

  if( libspectrum_snap_read( snap, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
                             filename ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: reading `%s' failed\n", progname, filename );
    libspectrum_snap_free( snap );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  if( libspectrum_snap_a( snap ) != 0x12 ) {
    fprintf( stderr, "%s: A is 0x%02x, not the expected 0x12\n", progname,
             libspectrum_snap_a( snap ) );
    r = TEST_FAIL;
  } else if( libspectrum_snap_f( snap ) != 0x34 ) {
    fprintf( stderr, "%s: F is 0x%02x, not the expected 0x34\n", progname,
             libspectrum_snap_f( snap ) );
    r = TEST_FAIL;
  } else if( libspectrum_snap_a_( snap ) != 0x56 ) {
    fprintf( stderr, "%s: A' is 0x%02x, not the expected 0x56\n", progname,
             libspectrum_snap_a_( snap ) );
    r = TEST_FAIL;
  } else if( libspectrum_snap_f_( snap ) != 0x78 ) {
    fprintf( stderr, "%s: F' is 0x%02x, not the expected 0x78\n", progname,
             libspectrum_snap_f_( snap ) );
    r = TEST_FAIL;
  } else {
    r = TEST_PASS;
  }

  return r;
}


/* libspectrum_split_to_48k_pages: each 16K region goes to the correct page */
test_return_t
split_to_48k_pages_distributes_memory_to_correct_pages( void )
{
  libspectrum_snap *snap;
  libspectrum_byte data[0xc000];
  test_return_t r = TEST_FAIL;

  memset( &data[0x0000], 0x11, 0x4000 );
  memset( &data[0x4000], 0x22, 0x4000 );
  memset( &data[0x8000], 0x33, 0x4000 );

  snap = libspectrum_snap_alloc();
  if( !snap ) {
    fprintf( stderr, "%s: split_to_48k_pages_distributes_memory_to_correct_pages: snap_alloc failed\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_split_to_48k_pages( snap, data ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: split_to_48k_pages_distributes_memory_to_correct_pages: split failed\n", progname );
    goto done;
  }

  if( !libspectrum_snap_pages( snap, 5 ) || libspectrum_snap_pages( snap, 5 )[0] != 0x11 ) {
    fprintf( stderr, "%s: split_to_48k_pages_distributes_memory_to_correct_pages: page 5 mismatch\n", progname );
    goto done;
  }

  if( !libspectrum_snap_pages( snap, 2 ) || libspectrum_snap_pages( snap, 2 )[0] != 0x22 ) {
    fprintf( stderr, "%s: split_to_48k_pages_distributes_memory_to_correct_pages: page 2 mismatch\n", progname );
    goto done;
  }

  if( !libspectrum_snap_pages( snap, 0 ) || libspectrum_snap_pages( snap, 0 )[0] != 0x33 ) {
    fprintf( stderr, "%s: split_to_48k_pages_distributes_memory_to_correct_pages: page 0 mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

/* libspectrum_split_to_48k_pages: returns LOGIC error if any page already occupied */
test_return_t
split_to_48k_pages_fails_when_page_already_occupied( void )
{
  libspectrum_snap *snap;
  libspectrum_byte data[0xc000];
  libspectrum_byte *page5;
  libspectrum_error_function_t error_function;
  test_return_t r = TEST_FAIL;

  memset( data, 0, sizeof( data ) );

  snap = libspectrum_snap_alloc();
  if( !snap ) {
    fprintf( stderr, "%s: split_to_48k_pages_fails_when_page_already_occupied: snap_alloc failed\n", progname );
    return TEST_INCOMPLETE;
  }

  page5 = libspectrum_new( libspectrum_byte, 0x4000 );
  libspectrum_snap_set_pages( snap, 5, page5 );

  error_function = libspectrum_error_function;
  libspectrum_error_function = NULL;
  if( libspectrum_split_to_48k_pages( snap, data ) != LIBSPECTRUM_ERROR_LOGIC ) {
    libspectrum_error_function = error_function;
    fprintf( stderr, "%s: split_to_48k_pages_fails_when_page_already_occupied: expected LIBSPECTRUM_ERROR_LOGIC\n", progname );
    goto done;
  }
  libspectrum_error_function = error_function;

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

/* libspectrum_write_snap_page: writes the page RAM data to buffer */
test_return_t
write_snap_page_writes_ram_data_to_buffer( void )
{
  libspectrum_snap *snap;
  libspectrum_buffer *buf;
  libspectrum_byte *page;
  const libspectrum_byte *data;
  size_t size;
  test_return_t r = TEST_FAIL;

  snap = libspectrum_snap_alloc();
  if( !snap ) {
    fprintf( stderr, "%s: write_snap_page_writes_ram_data_to_buffer: snap_alloc failed\n", progname );
    return TEST_INCOMPLETE;
  }

  page = libspectrum_new( libspectrum_byte, 0x4000 );
  memset( page, 0xab, 0x4000 );
  libspectrum_snap_set_pages( snap, 5, page );

  buf = libspectrum_buffer_alloc();
  libspectrum_write_snap_page( buf, snap, 5 );

  size = libspectrum_buffer_get_data_size( buf );
  data = libspectrum_buffer_get_data( buf );

  if( size != 0x4000 ) {
    fprintf( stderr, "%s: write_snap_page_writes_ram_data_to_buffer: expected 0x4000 bytes, got %lu\n",
             progname, (unsigned long)size );
    goto done;
  }

  if( data[0] != 0xab || data[0x3fff] != 0xab ) {
    fprintf( stderr, "%s: write_snap_page_writes_ram_data_to_buffer: expected 0xab fill, got 0x%02x/0x%02x\n",
             progname, data[0], data[0x3fff] );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  libspectrum_snap_free( snap );
  return r;
}

/* libspectrum_write_snap_page: fills 0xff when the page pointer is NULL */
test_return_t
write_snap_page_fills_0xff_for_absent_page( void )
{
  libspectrum_snap *snap;
  libspectrum_buffer *buf;
  const libspectrum_byte *data;
  size_t size;
  test_return_t r = TEST_FAIL;

  snap = libspectrum_snap_alloc();
  if( !snap ) {
    fprintf( stderr, "%s: write_snap_page_fills_0xff_for_absent_page: snap_alloc failed\n", progname );
    return TEST_INCOMPLETE;
  }

  /* Page 3 is never set -> NULL -> should fill 0xff */
  buf = libspectrum_buffer_alloc();
  libspectrum_write_snap_page( buf, snap, 3 );

  size = libspectrum_buffer_get_data_size( buf );
  data = libspectrum_buffer_get_data( buf );

  if( size != 0x4000 ) {
    fprintf( stderr, "%s: write_snap_page_fills_0xff_for_absent_page: expected 0x4000 bytes, got %lu\n",
             progname, (unsigned long)size );
    goto done;
  }

  if( data[0] != 0xff || data[0x3fff] != 0xff ) {
    fprintf( stderr, "%s: write_snap_page_fills_0xff_for_absent_page: expected 0xff fill, got 0x%02x/0x%02x\n",
             progname, data[0], data[0x3fff] );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  libspectrum_snap_free( snap );
  return r;
}
