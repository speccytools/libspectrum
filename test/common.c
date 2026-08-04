#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internals.h"
#include "common.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

int
read_file( libspectrum_byte **buffer, size_t *length, const char *filename )
{
  int fd;
  struct stat info;
  ssize_t bytes;

  fd = open( filename, O_RDONLY | O_BINARY );
  if( fd == -1 ) {
    fprintf( stderr, "%s: couldn't open `%s': %s\n", progname, filename,
	     strerror( errno ) );
    return errno;
  }

  if( fstat( fd, &info ) ) {
    fprintf( stderr, "%s: couldn't stat `%s': %s\n", progname, filename,
	     strerror( errno ) );
    return errno;
  }

  *length = info.st_size;
  *buffer = libspectrum_new( libspectrum_byte, *length );

  bytes = read( fd, *buffer, *length );
  if( bytes == -1 ) {
    fprintf( stderr, "%s: error reading from `%s': %s\n", progname, filename,
	     strerror( errno ) );
    return errno;
  } else if( (size_t)bytes < *length ) {
    fprintf( stderr, "%s: read only %lu of %lu bytes from `%s'\n", progname,
	     (unsigned long)bytes, (unsigned long)*length, filename );
    return 1;
  }

  if( close( fd ) ) {
    fprintf( stderr, "%s: error closing `%s': %s\n", progname, filename,
	     strerror( errno ) );
    return errno;
  }

  return 0;
}

test_return_t
load_tape( libspectrum_tape **tape, const char *filename,
           libspectrum_error expected_result )
{
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  *tape = libspectrum_tape_alloc();

  if( libspectrum_tape_read( *tape, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
			     filename ) != expected_result ) {
    fprintf( stderr, "%s: reading `%s' did not give expected result\n",
	     progname, filename );
    libspectrum_tape_free( *tape );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  return TEST_PASS;
}

test_return_t
read_tape( const char *filename, libspectrum_error expected_result )
{
  libspectrum_tape *tape;
  test_return_t r;

  r = load_tape( &tape, filename, expected_result );
  if( r != TEST_PASS ) return r;

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

test_return_t
read_snap( const char *filename, const char *filename_to_pass,
	   libspectrum_error expected_result )
{
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_snap *snap;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  snap = libspectrum_snap_alloc();

  if( libspectrum_snap_read( snap, buffer, filesize, LIBSPECTRUM_ID_UNKNOWN,
			     filename_to_pass ) != expected_result ) {
    fprintf( stderr, "%s: reading `%s' did not give expected result\n",
	     progname, filename );
    libspectrum_snap_free( snap );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  libspectrum_snap_free( snap );

  return TEST_PASS;
}

test_return_t
play_tape( const char *filename )
{
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_tape *tape;
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

  do {

    if( libspectrum_tape_get_next_edge( &tstates, &flags, tape ) ) {
      libspectrum_tape_free( tape );
      return TEST_INCOMPLETE;
    }

  } while( !( flags & LIBSPECTRUM_TAPE_FLAGS_STOP ) );

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

#ifdef HAVE_WAV_BACKEND
test_return_t
check_wav_block( const char *filename, libspectrum_dword expected_bit_length,
                 libspectrum_byte expected_byte )
{
  libspectrum_tape *tape;
  libspectrum_tape_iterator it;
  libspectrum_tape_block *block;
  libspectrum_byte *data;
  test_return_t r;

  r = load_tape( &tape, filename, LIBSPECTRUM_ERROR_NONE );
  if( r != TEST_PASS ) return r;

  block = libspectrum_tape_iterator_init( &it, tape );
  if( !block ) {
    fprintf( stderr, "%s: `%s' did not produce a tape block\n", progname,
             filename );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_block_type( block ) != LIBSPECTRUM_TAPE_BLOCK_RAW_DATA ) {
    fprintf( stderr, "%s: `%s' produced block type %d, expected raw data\n",
             progname, filename, libspectrum_tape_block_type( block ) );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_block_bit_length( block ) != expected_bit_length ) {
    fprintf( stderr, "%s: `%s' produced bit length %lu, expected %lu\n",
             progname, filename,
             (unsigned long)libspectrum_tape_block_bit_length( block ),
             (unsigned long)expected_bit_length );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_block_bits_in_last_byte( block ) != 8 ) {
    fprintf( stderr, "%s: `%s' used %lu bits in last byte, expected 8\n",
             progname, filename,
             (unsigned long)libspectrum_tape_block_bits_in_last_byte( block ) );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_block_data_length( block ) != 1 ) {
    fprintf( stderr, "%s: `%s' produced data length %lu, expected 1\n",
             progname, filename,
             (unsigned long)libspectrum_tape_block_data_length( block ) );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  data = libspectrum_tape_block_data( block );
  if( !data || data[0] != expected_byte ) {
    fprintf( stderr, "%s: `%s' produced data byte 0x%02x, expected 0x%02x\n",
             progname, filename, data ? data[0] : 0, expected_byte );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_iterator_next( &it ) ) {
    fprintf( stderr, "%s: `%s' produced more than one tape block\n", progname,
             filename );
    libspectrum_tape_free( tape );
    return TEST_FAIL;
  }

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}
#endif
