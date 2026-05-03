#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "internals.h"
#include "test.h"

const char *progname;
static const char *LIBSPECTRUM_MIN_VERSION = "0.4.0";

typedef test_return_t (*test_fn)( void );

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
  } else if( bytes < *length ) {
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

static test_return_t
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

static test_return_t
read_tape( const char *filename, libspectrum_error expected_result )
{
  libspectrum_tape *tape;
  test_return_t r; 

  r = load_tape( &tape, filename, expected_result );
  if( r != TEST_PASS ) return r;

  if( libspectrum_tape_free( tape ) ) return TEST_INCOMPLETE;

  return TEST_PASS;
}

static test_return_t
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

static test_return_t
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
static test_return_t
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

/* Specific tests begin here */

/* Test for bugs #47 and #78: tape object incorrectly freed after reading
   invalid tape */
static test_return_t
test_1( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid.tzx" ), LIBSPECTRUM_ERROR_UNKNOWN );
}

/* Test for bugs #84: TZX turbo blocks with zero pilot pulses and #85: freeing
   a turbo block with no data produces segfault */
static test_return_t
test_2( void )
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

  if( flags ) {
    fprintf( stderr, "%s: reading first edge of `%s' gave unexpected flags 0x%04x; expected 0x0000\n",
	     progname, filename, flags );
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
static test_return_t
test_3( void )
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

/* Test for bug #102: invalid compressed file causes crash */
static test_return_t
test_4( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  const char *filename = STATIC_TEST_PATH( "invalid.gz" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_UNKNOWN );
}

/* Further test for bug #102: invalid compressed file causes crash */
static test_return_t
test_5( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  return read_snap( STATIC_TEST_PATH( "invalid.gz" ), NULL, LIBSPECTRUM_ERROR_UNKNOWN );
}

/* Test for bug #103: pointer wraparound causes segfault */
static test_return_t
test_6( void )
{
  const char *filename = STATIC_TEST_PATH( "invalid.szx" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #105: lack of sanity check in GDB code */
static test_return_t
test_7( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-gdb.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #106: empty DRB causes segfault */
static test_return_t
test_8( void )
{
  return read_tape( STATIC_TEST_PATH( "empty-drb.tzx" ), LIBSPECTRUM_ERROR_NONE );
}

/* Test for bug #107: problems with invalid archive info block */
static test_return_t
test_9( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-archiveinfo.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #108: invalid hardware info blocks can leak memory */
static test_return_t
test_10( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-hardwareinfo.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #111: invalid Warajevo tape block offset causes segfault */
static test_return_t
test_11( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-warajevo-blockoffset.tap" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #112: invalid custom info block causes memory leak */
static test_return_t
test_12( void )
{
  return read_tape( STATIC_TEST_PATH( "invalid-custominfo.tzx" ), LIBSPECTRUM_ERROR_CORRUPT );
}

/* Test for bug #113: loop end without a loop start block accesses uninitialised
   memory */
static test_return_t
test_13( void )
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
static test_return_t
test_14( void )
{
  return play_tape( STATIC_TEST_PATH( "loop.tzx" ) );
}

/* Test for bug #118: TZX loop blocks still broken */
static test_return_t
test_16( void )
{
  return play_tape( STATIC_TEST_PATH( "loop2.tzx" ) );
}

/* Test for bug #119: TZX jump blocks broken */
static test_return_t
test_17( void )
{
  return play_tape( STATIC_TEST_PATH( "jump.tzx" ) );
}

/* Test for bug #121: crashes writing and reading empty CSW files */
static test_return_t
test_18( void )
{
  return play_tape( STATIC_TEST_PATH( "empty.csw" ) );
}

/* Test for bug #125: .tap writing code does not handle all block types */
static test_return_t
test_19( void )
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

/* Tests for bug #129: SP not sanity checked when reading .sna files;
   also tests bug #130: compressed snapshots cause segfault */
static test_return_t
test_20( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  const char *filename = STATIC_TEST_PATH( "sp-2000.sna.gz" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_CORRUPT );
} 
  
static test_return_t
test_21( void )
{
  #ifndef HAVE_ZLIB_H
    return TEST_SKIPPED; /* gzip not enabled in build */
  #endif

  const char *filename = STATIC_TEST_PATH( "sp-ffff.sna.gz" );
  return read_snap( filename, filename, LIBSPECTRUM_ERROR_CORRUPT );
} 

/* Tests for bug #152: .mdr code does not correctly handle write protect flag */
static test_return_t
test_22( void )
{
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  libspectrum_microdrive *mdr;
  const char *filename = STATIC_TEST_PATH( "writeprotected.mdr" );
  test_return_t r;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  /* writeprotected.mdr deliberately includes an extra 0 on the end;
     we want this in the buffer so we know what happens if we read off the
     end of the file; however, we don't want it in the length */
  filesize--;

  mdr = libspectrum_microdrive_alloc();

  if( libspectrum_microdrive_mdr_read( mdr, buffer, filesize ) ) {
    libspectrum_microdrive_free( mdr );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer );

  r = libspectrum_microdrive_write_protect( mdr ) ? TEST_PASS : TEST_FAIL;

  libspectrum_microdrive_free( mdr );

  return r;
}

static test_return_t
test_23( void )
{
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0, length;
  libspectrum_microdrive *mdr;
  const char *filename = STATIC_TEST_PATH( "writeprotected.mdr" );
  test_return_t r;

  if( read_file( &buffer, &filesize, filename ) ) return TEST_INCOMPLETE;

  /* writeprotected.mdr deliberately includes an extra 0 on the end;
     we want this in the buffer so we know what happens if we read off the
     end of the file; however, we don't want it in the length */
  filesize--;

  mdr = libspectrum_microdrive_alloc();

  if( libspectrum_microdrive_mdr_read( mdr, buffer, filesize ) ) {
    libspectrum_microdrive_free( mdr );
    libspectrum_free( buffer );
    return TEST_INCOMPLETE;
  }

  libspectrum_free( buffer ); buffer = NULL;

  libspectrum_microdrive_mdr_write( mdr, &buffer, &length );

  libspectrum_microdrive_free( mdr );

  r = ( length == filesize && buffer[ length - 1 ] == 1 ) ? TEST_PASS : TEST_FAIL;

  libspectrum_free( buffer );

  return r;
}

static test_return_t
test_24( void )
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

static test_return_t
test_25( void )
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
static test_return_t
test_26( void )
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

/* Tests for bug #184: SZX files were written with A and F reversed */
static test_return_t
test_27( void )
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

/* Test for bug #379: converting .tap file to .csw causes crash */
static test_return_t
test_30( void )
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

static test_return_t
test_71( void )
{
#ifndef HAVE_ZLIB_H
  return TEST_SKIPPED; /* gzip not enabled in build */
#endif

  const char *filename = STATIC_TEST_PATH( "random.szx" );
  libspectrum_byte *buffer = NULL;
  size_t filesize = 0;
  size_t rzx_length = 0;
  libspectrum_snap *snap;
  libspectrum_rzx *rzx;
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

  rzx = libspectrum_rzx_alloc();

  if( libspectrum_rzx_add_snap( rzx, snap, 0 ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: adding snap failed\n", progname );
    libspectrum_rzx_free( rzx );
    libspectrum_snap_free( snap );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_rzx_write( &buffer, &rzx_length, rzx,
        LIBSPECTRUM_ID_SNAPSHOT_SZX, NULL, 1, NULL ) != LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: error serializing RZX\n", progname );
    libspectrum_rzx_free( rzx );
    return TEST_INCOMPLETE;
  }

  libspectrum_rzx_free( rzx );
  libspectrum_free( buffer );

  if( rzx_length > 49152 ) {
    r = TEST_PASS;
  } else {
    fprintf( stderr, "%s: length %lu too short\n", progname,
             (unsigned long)rzx_length );
    r = TEST_FAIL;
  }

  return r;
}

static test_return_t
test_72( void )
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

static test_return_t
test_75( void )
{
#ifndef HAVE_WAV_BACKEND
  return TEST_SKIPPED;
#else
  return check_wav_block( DYNAMIC_TEST_PATH( "wav-mono-threshold.wav" ),
                          3500000 / 22050, 0xa9 );
#endif
}

static test_return_t
test_76( void )
{
#ifndef WAV_INTERNAL_MACOS
  return TEST_SKIPPED;
#else
  return check_wav_block( DYNAMIC_TEST_PATH( "wav-stereo-mixdown.wav" ),
                          3500000 / 22050, 0x8d );
#endif
}

/* Buffer write tests */

static test_return_t
test_77( void )
{
  /* libspectrum_buffer_write_byte: single byte stored correctly */
  libspectrum_buffer *buf = libspectrum_buffer_alloc();
  test_return_t r = TEST_FAIL;
  const libspectrum_byte *data;

  libspectrum_buffer_write_byte( buf, 0xab );

  if( libspectrum_buffer_get_data_size( buf ) != 1 ) {
    fprintf( stderr, "%s: test_77: expected size 1, got %lu\n", progname,
             (unsigned long)libspectrum_buffer_get_data_size( buf ) );
    goto done;
  }

  data = libspectrum_buffer_get_data( buf );
  if( data[0] != 0xab ) {
    fprintf( stderr, "%s: test_77: expected 0xab, got 0x%02x\n",
             progname, data[0] );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  return r;
}

static test_return_t
test_78( void )
{
  /* libspectrum_buffer_write_word: value stored little-endian */
  libspectrum_buffer *buf = libspectrum_buffer_alloc();
  test_return_t r = TEST_FAIL;
  const libspectrum_byte *data;

  libspectrum_buffer_write_word( buf, 0x1234 );

  if( libspectrum_buffer_get_data_size( buf ) != 2 ) {
    fprintf( stderr, "%s: test_78: expected size 2, got %lu\n", progname,
             (unsigned long)libspectrum_buffer_get_data_size( buf ) );
    goto done;
  }

  data = libspectrum_buffer_get_data( buf );
  if( data[0] != 0x34 || data[1] != 0x12 ) {
    fprintf( stderr, "%s: test_78: expected 0x34 0x12, got 0x%02x 0x%02x\n",
             progname, data[0], data[1] );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  return r;
}

static test_return_t
test_79( void )
{
  /* libspectrum_buffer_write_dword: value stored little-endian */
  libspectrum_buffer *buf = libspectrum_buffer_alloc();
  test_return_t r = TEST_FAIL;
  const libspectrum_byte *data;

  libspectrum_buffer_write_dword( buf, 0x12345678 );

  if( libspectrum_buffer_get_data_size( buf ) != 4 ) {
    fprintf( stderr, "%s: test_79: expected size 4, got %lu\n", progname,
             (unsigned long)libspectrum_buffer_get_data_size( buf ) );
    goto done;
  }

  data = libspectrum_buffer_get_data( buf );
  if( data[0] != 0x78 || data[1] != 0x56 ||
      data[2] != 0x34 || data[3] != 0x12 ) {
    fprintf( stderr,
             "%s: test_79: expected 0x78 0x56 0x34 0x12, got"
             " 0x%02x 0x%02x 0x%02x 0x%02x\n",
             progname, data[0], data[1], data[2], data[3] );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  return r;
}

static test_return_t
test_80( void )
{
  /* libspectrum_buffer_write_buffer: contents of src copied to dest */
  libspectrum_buffer *src = libspectrum_buffer_alloc();
  libspectrum_buffer *dest = libspectrum_buffer_alloc();
  test_return_t r = TEST_FAIL;
  const libspectrum_byte *data;

  libspectrum_buffer_write_byte( src, 0x01 );
  libspectrum_buffer_write_byte( src, 0x02 );
  libspectrum_buffer_write_byte( src, 0x03 );

  libspectrum_buffer_write_buffer( dest, src );

  if( libspectrum_buffer_get_data_size( dest ) != 3 ) {
    fprintf( stderr, "%s: test_80: expected dest size 3, got %lu\n", progname,
             (unsigned long)libspectrum_buffer_get_data_size( dest ) );
    goto done;
  }

  data = libspectrum_buffer_get_data( dest );
  if( data[0] != 0x01 || data[1] != 0x02 || data[2] != 0x03 ) {
    fprintf( stderr,
             "%s: test_80: expected 0x01 0x02 0x03, got 0x%02x 0x%02x 0x%02x\n",
             progname, data[0], data[1], data[2] );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( src );
  libspectrum_buffer_free( dest );
  return r;
}

static test_return_t
test_81( void )
{
  /* libspectrum_buffer_is_empty / is_not_empty */
  libspectrum_buffer *buf = libspectrum_buffer_alloc();
  test_return_t r = TEST_FAIL;

  if( !libspectrum_buffer_is_empty( buf ) ) {
    fprintf( stderr, "%s: test_81: new buffer should be empty\n", progname );
    goto done;
  }
  if( libspectrum_buffer_is_not_empty( buf ) ) {
    fprintf( stderr, "%s: test_81: new buffer should not be non-empty\n",
             progname );
    goto done;
  }

  libspectrum_buffer_write_byte( buf, 0xff );

  if( libspectrum_buffer_is_empty( buf ) ) {
    fprintf( stderr, "%s: test_81: buffer with data should not be empty\n",
             progname );
    goto done;
  }
  if( !libspectrum_buffer_is_not_empty( buf ) ) {
    fprintf( stderr, "%s: test_81: buffer with data should be non-empty\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  return r;
}

static test_return_t
test_82( void )
{
  /* libspectrum_buffer_clear: size resets to zero */
  libspectrum_buffer *buf = libspectrum_buffer_alloc();
  test_return_t r = TEST_FAIL;

  libspectrum_buffer_write_byte( buf, 0x42 );
  libspectrum_buffer_write_byte( buf, 0x43 );

  if( libspectrum_buffer_get_data_size( buf ) != 2 ) {
    fprintf( stderr, "%s: test_82: expected size 2 before clear, got %lu\n",
             progname,
             (unsigned long)libspectrum_buffer_get_data_size( buf ) );
    goto done;
  }

  libspectrum_buffer_clear( buf );

  if( libspectrum_buffer_get_data_size( buf ) != 0 ) {
    fprintf( stderr, "%s: test_82: expected size 0 after clear, got %lu\n",
             progname,
             (unsigned long)libspectrum_buffer_get_data_size( buf ) );
    goto done;
  }
  if( !libspectrum_buffer_is_empty( buf ) ) {
    fprintf( stderr, "%s: test_82: cleared buffer should be empty\n",
             progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  return r;
}

static test_return_t
test_83( void )
{
  /* libspectrum_buffer_set: N bytes are filled with given value */
  libspectrum_buffer *buf = libspectrum_buffer_alloc();
  test_return_t r = TEST_FAIL;
  const libspectrum_byte *data;
  size_t i;

  libspectrum_buffer_set( buf, 0x5a, 4 );

  if( libspectrum_buffer_get_data_size( buf ) != 4 ) {
    fprintf( stderr, "%s: test_83: expected size 4, got %lu\n", progname,
             (unsigned long)libspectrum_buffer_get_data_size( buf ) );
    goto done;
  }

  data = libspectrum_buffer_get_data( buf );
  for( i = 0; i < 4; i++ ) {
    if( data[i] != 0x5a ) {
      fprintf( stderr, "%s: test_83: byte %lu: expected 0x5a, got 0x%02x\n",
               progname, (unsigned long)i, data[i] );
      goto done;
    }
  }

  r = TEST_PASS;

done:
  libspectrum_buffer_free( buf );
  return r;
}

static test_return_t
test_84( void )
{
  /* libspectrum_buffer_append: contents of src are appended to raw buffer */
  libspectrum_buffer *src = libspectrum_buffer_alloc();
  libspectrum_byte *raw = NULL;
  size_t raw_len = 0;
  libspectrum_byte *ptr;
  test_return_t r = TEST_FAIL;

  libspectrum_buffer_write_byte( src, 0x11 );
  libspectrum_buffer_write_byte( src, 0x22 );
  libspectrum_buffer_write_byte( src, 0x33 );

  ptr = raw;
  libspectrum_buffer_append( &raw, &raw_len, &ptr, src );

  if( (size_t)( ptr - raw ) != 3 ) {
    fprintf( stderr, "%s: test_84: expected 3 bytes written, got %lu\n",
             progname, (unsigned long)( ptr - raw ) );
    goto done;
  }

  if( raw[0] != 0x11 || raw[1] != 0x22 || raw[2] != 0x33 ) {
    fprintf( stderr,
             "%s: test_84: expected 0x11 0x22 0x33, got 0x%02x 0x%02x 0x%02x\n",
             progname, raw[0], raw[1], raw[2] );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( raw );
  libspectrum_buffer_free( src );
  return r;
}

static test_return_t
test_85( void )
{
  /* libspectrum_creator: alloc/free and program getter/setter */
  libspectrum_creator *creator = libspectrum_creator_alloc();
  test_return_t r = TEST_FAIL;

  if( !creator ) {
    fprintf( stderr, "%s: test_85: creator_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_creator_set_program( creator, "TestApp" );

  if( strcmp( (const char *)libspectrum_creator_program( creator ), "TestApp" ) != 0 ) {
    fprintf( stderr, "%s: test_85: expected program \"TestApp\", got \"%s\"\n",
             progname, (const char *)libspectrum_creator_program( creator ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_creator_free( creator );
  return r;
}

static test_return_t
test_86( void )
{
  /* libspectrum_creator: major and minor version getter/setter */
  libspectrum_creator *creator = libspectrum_creator_alloc();
  test_return_t r = TEST_FAIL;

  if( !creator ) {
    fprintf( stderr, "%s: test_86: creator_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_creator_set_major( creator, 2 );
  libspectrum_creator_set_minor( creator, 7 );

  if( libspectrum_creator_major( creator ) != 2 ) {
    fprintf( stderr, "%s: test_86: expected major 2, got %d\n",
             progname, libspectrum_creator_major( creator ) );
    goto done;
  }

  if( libspectrum_creator_minor( creator ) != 7 ) {
    fprintf( stderr, "%s: test_86: expected minor 7, got %d\n",
             progname, libspectrum_creator_minor( creator ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_creator_free( creator );
  return r;
}

static test_return_t
test_87( void )
{
  /* libspectrum_creator: competition_code and custom data getter/setter */
  libspectrum_creator *creator = libspectrum_creator_alloc();
  libspectrum_byte *custom_data;
  const libspectrum_byte *got_custom;
  test_return_t r = TEST_FAIL;

  if( !creator ) {
    fprintf( stderr, "%s: test_87: creator_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_creator_set_competition_code( creator, 0x1234 );

  if( libspectrum_creator_competition_code( creator ) != 0x1234 ) {
    fprintf( stderr, "%s: test_87: expected competition_code 0x1234, got 0x%04x\n",
             progname, libspectrum_creator_competition_code( creator ) );
    goto done;
  }

  custom_data = libspectrum_new( libspectrum_byte, 4 );
  custom_data[0] = 0xde; custom_data[1] = 0xad;
  custom_data[2] = 0xbe; custom_data[3] = 0xef;

  libspectrum_creator_set_custom( creator, custom_data, 4 );

  if( libspectrum_creator_custom_length( creator ) != 4 ) {
    fprintf( stderr, "%s: test_87: expected custom_length 4, got %lu\n",
             progname, (unsigned long)libspectrum_creator_custom_length( creator ) );
    goto done;
  }

  got_custom = libspectrum_creator_custom( creator );
  if( got_custom[0] != 0xde || got_custom[1] != 0xad ||
      got_custom[2] != 0xbe || got_custom[3] != 0xef ) {
    fprintf( stderr, "%s: test_87: custom data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_creator_free( creator );
  return r;
}

static test_return_t
test_88( void )
{
  /* libspectrum_snap: main Z80 register getter/setter (a, f, bc, de, hl, alternates) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_88: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_a( snap, 0x12 );
  libspectrum_snap_set_f( snap, 0x34 );
  libspectrum_snap_set_bc( snap, 0x5678 );
  libspectrum_snap_set_de( snap, 0x9abc );
  libspectrum_snap_set_hl( snap, 0xdef0 );
  libspectrum_snap_set_a_( snap, 0xaa );
  libspectrum_snap_set_f_( snap, 0xbb );
  libspectrum_snap_set_bc_( snap, 0x1122 );
  libspectrum_snap_set_de_( snap, 0x3344 );
  libspectrum_snap_set_hl_( snap, 0x5566 );

  if( libspectrum_snap_a( snap ) != 0x12 ) {
    fprintf( stderr, "%s: test_88: expected a=0x12, got 0x%02x\n", progname,
             libspectrum_snap_a( snap ) );
    goto done;
  }
  if( libspectrum_snap_f( snap ) != 0x34 ) {
    fprintf( stderr, "%s: test_88: expected f=0x34, got 0x%02x\n", progname,
             libspectrum_snap_f( snap ) );
    goto done;
  }
  if( libspectrum_snap_bc( snap ) != 0x5678 ) {
    fprintf( stderr, "%s: test_88: expected bc=0x5678, got 0x%04x\n", progname,
             libspectrum_snap_bc( snap ) );
    goto done;
  }
  if( libspectrum_snap_de( snap ) != 0x9abc ) {
    fprintf( stderr, "%s: test_88: expected de=0x9abc, got 0x%04x\n", progname,
             libspectrum_snap_de( snap ) );
    goto done;
  }
  if( libspectrum_snap_hl( snap ) != 0xdef0 ) {
    fprintf( stderr, "%s: test_88: expected hl=0xdef0, got 0x%04x\n", progname,
             libspectrum_snap_hl( snap ) );
    goto done;
  }
  if( libspectrum_snap_a_( snap ) != 0xaa ) {
    fprintf( stderr, "%s: test_88: expected a_=0xaa, got 0x%02x\n", progname,
             libspectrum_snap_a_( snap ) );
    goto done;
  }
  if( libspectrum_snap_f_( snap ) != 0xbb ) {
    fprintf( stderr, "%s: test_88: expected f_=0xbb, got 0x%02x\n", progname,
             libspectrum_snap_f_( snap ) );
    goto done;
  }
  if( libspectrum_snap_bc_( snap ) != 0x1122 ) {
    fprintf( stderr, "%s: test_88: expected bc_=0x1122, got 0x%04x\n", progname,
             libspectrum_snap_bc_( snap ) );
    goto done;
  }
  if( libspectrum_snap_de_( snap ) != 0x3344 ) {
    fprintf( stderr, "%s: test_88: expected de_=0x3344, got 0x%04x\n", progname,
             libspectrum_snap_de_( snap ) );
    goto done;
  }
  if( libspectrum_snap_hl_( snap ) != 0x5566 ) {
    fprintf( stderr, "%s: test_88: expected hl_=0x5566, got 0x%04x\n", progname,
             libspectrum_snap_hl_( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_89( void )
{
  /* libspectrum_snap: index and special register getter/setter (ix, iy, i, r, sp, pc) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_89: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_ix( snap, 0x1234 );
  libspectrum_snap_set_iy( snap, 0x5678 );
  libspectrum_snap_set_i( snap, 0xfe );
  libspectrum_snap_set_r( snap, 0xdc );
  libspectrum_snap_set_sp( snap, 0xba98 );
  libspectrum_snap_set_pc( snap, 0x7654 );

  if( libspectrum_snap_ix( snap ) != 0x1234 ) {
    fprintf( stderr, "%s: test_89: expected ix=0x1234, got 0x%04x\n", progname,
             libspectrum_snap_ix( snap ) );
    goto done;
  }
  if( libspectrum_snap_iy( snap ) != 0x5678 ) {
    fprintf( stderr, "%s: test_89: expected iy=0x5678, got 0x%04x\n", progname,
             libspectrum_snap_iy( snap ) );
    goto done;
  }
  if( libspectrum_snap_i( snap ) != 0xfe ) {
    fprintf( stderr, "%s: test_89: expected i=0xfe, got 0x%02x\n", progname,
             libspectrum_snap_i( snap ) );
    goto done;
  }
  if( libspectrum_snap_r( snap ) != 0xdc ) {
    fprintf( stderr, "%s: test_89: expected r=0xdc, got 0x%02x\n", progname,
             libspectrum_snap_r( snap ) );
    goto done;
  }
  if( libspectrum_snap_sp( snap ) != 0xba98 ) {
    fprintf( stderr, "%s: test_89: expected sp=0xba98, got 0x%04x\n", progname,
             libspectrum_snap_sp( snap ) );
    goto done;
  }
  if( libspectrum_snap_pc( snap ) != 0x7654 ) {
    fprintf( stderr, "%s: test_89: expected pc=0x7654, got 0x%04x\n", progname,
             libspectrum_snap_pc( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_90( void )
{
  /* libspectrum_snap: Z80 status getter/setter (iff1, iff2, im, tstates, halted) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_90: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_iff1( snap, 1 );
  libspectrum_snap_set_iff2( snap, 0 );
  libspectrum_snap_set_im( snap, 2 );
  libspectrum_snap_set_tstates( snap, 69888 );
  libspectrum_snap_set_halted( snap, 1 );

  if( libspectrum_snap_iff1( snap ) != 1 ) {
    fprintf( stderr, "%s: test_90: expected iff1=1, got %d\n", progname,
             libspectrum_snap_iff1( snap ) );
    goto done;
  }
  if( libspectrum_snap_iff2( snap ) != 0 ) {
    fprintf( stderr, "%s: test_90: expected iff2=0, got %d\n", progname,
             libspectrum_snap_iff2( snap ) );
    goto done;
  }
  if( libspectrum_snap_im( snap ) != 2 ) {
    fprintf( stderr, "%s: test_90: expected im=2, got %d\n", progname,
             libspectrum_snap_im( snap ) );
    goto done;
  }
  if( libspectrum_snap_tstates( snap ) != 69888 ) {
    fprintf( stderr, "%s: test_90: expected tstates=69888, got %lu\n", progname,
             (unsigned long)libspectrum_snap_tstates( snap ) );
    goto done;
  }
  if( libspectrum_snap_halted( snap ) != 1 ) {
    fprintf( stderr, "%s: test_90: expected halted=1, got %d\n", progname,
             libspectrum_snap_halted( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_91( void )
{
  /* libspectrum_microdrive: alloc/free and write_protect getter/setter */
  libspectrum_microdrive *mdr = libspectrum_microdrive_alloc();
  test_return_t r = TEST_FAIL;

  if( !mdr ) {
    fprintf( stderr, "%s: test_91: microdrive_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_microdrive_set_write_protect( mdr, 1 );
  if( libspectrum_microdrive_write_protect( mdr ) != 1 ) {
    fprintf( stderr, "%s: test_91: expected write_protect 1, got %d\n",
             progname, libspectrum_microdrive_write_protect( mdr ) );
    goto done;
  }

  libspectrum_microdrive_set_write_protect( mdr, 0 );
  if( libspectrum_microdrive_write_protect( mdr ) != 0 ) {
    fprintf( stderr, "%s: test_91: expected write_protect 0, got %d\n",
             progname, libspectrum_microdrive_write_protect( mdr ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_microdrive_free( mdr );
  return r;
}

static test_return_t
test_92( void )
{
  /* libspectrum_microdrive: cartridge_len and data getter/setter */
  libspectrum_microdrive *mdr = libspectrum_microdrive_alloc();
  test_return_t r = TEST_FAIL;

  if( !mdr ) {
    fprintf( stderr, "%s: test_92: microdrive_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_microdrive_set_cartridge_len( mdr, 10 );
  if( libspectrum_microdrive_cartridge_len( mdr ) != 10 ) {
    fprintf( stderr, "%s: test_92: expected cartridge_len 10, got %d\n",
             progname, (int)libspectrum_microdrive_cartridge_len( mdr ) );
    goto done;
  }

  libspectrum_microdrive_set_data( mdr, 0, 0xa5 );
  if( libspectrum_microdrive_data( mdr, 0 ) != 0xa5 ) {
    fprintf( stderr, "%s: test_92: expected data[0] 0xa5, got 0x%02x\n",
             progname, (unsigned)libspectrum_microdrive_data( mdr, 0 ) );
    goto done;
  }

  libspectrum_microdrive_set_data( mdr, 100, 0x5a );
  if( libspectrum_microdrive_data( mdr, 100 ) != 0x5a ) {
    fprintf( stderr, "%s: test_92: expected data[100] 0x5a, got 0x%02x\n",
             progname, (unsigned)libspectrum_microdrive_data( mdr, 100 ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_microdrive_free( mdr );
  return r;
}

static test_return_t
test_93( void )
{
  /* libspectrum_microdrive: mdr_write/mdr_read roundtrip */
  libspectrum_microdrive *mdr1 = libspectrum_microdrive_alloc();
  libspectrum_microdrive *mdr2 = libspectrum_microdrive_alloc();
  libspectrum_byte *buf = NULL;
  size_t buf_len = 0;
  test_return_t r = TEST_FAIL;

  if( !mdr1 || !mdr2 ) {
    fprintf( stderr, "%s: test_93: microdrive_alloc returned NULL\n", progname );
    r = TEST_INCOMPLETE;
    goto done;
  }

  libspectrum_microdrive_set_write_protect( mdr1, 1 );
  libspectrum_microdrive_set_cartridge_len( mdr1, 10 );
  libspectrum_microdrive_set_data( mdr1, 5, 0xcc );

  libspectrum_microdrive_mdr_write( mdr1, &buf, &buf_len );
  if( !buf ) {
    fprintf( stderr, "%s: test_93: mdr_write returned NULL buffer\n", progname );
    r = TEST_INCOMPLETE;
    goto done;
  }

  if( libspectrum_microdrive_mdr_read( mdr2, buf, buf_len ) !=
      LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: test_93: mdr_read failed\n", progname );
    goto done;
  }

  if( libspectrum_microdrive_write_protect( mdr2 ) != 1 ) {
    fprintf( stderr, "%s: test_93: roundtrip write_protect: expected 1, got %d\n",
             progname, libspectrum_microdrive_write_protect( mdr2 ) );
    goto done;
  }

  if( libspectrum_microdrive_cartridge_len( mdr2 ) != 10 ) {
    fprintf( stderr, "%s: test_93: roundtrip cartridge_len: expected 10, got %d\n",
             progname, (int)libspectrum_microdrive_cartridge_len( mdr2 ) );
    goto done;
  }

  if( libspectrum_microdrive_data( mdr2, 5 ) != 0xcc ) {
    fprintf( stderr, "%s: test_93: roundtrip data[5]: expected 0xcc, got 0x%02x\n",
             progname, (unsigned)libspectrum_microdrive_data( mdr2, 5 ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_free( buf );
  if( mdr1 ) libspectrum_microdrive_free( mdr1 );
  if( mdr2 ) libspectrum_microdrive_free( mdr2 );
  return r;
}

static test_return_t
test_94( void )
{
  /* libspectrum_snap: machine type getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_94: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_machine( snap ) != LIBSPECTRUM_MACHINE_UNKNOWN ) {
    fprintf( stderr, "%s: test_94: default machine should be UNKNOWN, got %d\n",
             progname, (int)libspectrum_snap_machine( snap ) );
    goto done;
  }

  libspectrum_snap_set_machine( snap, LIBSPECTRUM_MACHINE_48 );
  if( libspectrum_snap_machine( snap ) != LIBSPECTRUM_MACHINE_48 ) {
    fprintf( stderr, "%s: test_94: expected MACHINE_48, got %d\n",
             progname, (int)libspectrum_snap_machine( snap ) );
    goto done;
  }

  libspectrum_snap_set_machine( snap, LIBSPECTRUM_MACHINE_128 );
  if( libspectrum_snap_machine( snap ) != LIBSPECTRUM_MACHINE_128 ) {
    fprintf( stderr, "%s: test_94: expected MACHINE_128, got %d\n",
             progname, (int)libspectrum_snap_machine( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_95( void )
{
  /* libspectrum_snap: memptr getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_95: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_memptr( snap, 0xabcd );
  if( libspectrum_snap_memptr( snap ) != 0xabcd ) {
    fprintf( stderr, "%s: test_95: expected memptr=0xabcd, got 0x%04x\n",
             progname, libspectrum_snap_memptr( snap ) );
    goto done;
  }

  libspectrum_snap_set_memptr( snap, 0x0000 );
  if( libspectrum_snap_memptr( snap ) != 0x0000 ) {
    fprintf( stderr, "%s: test_95: expected memptr=0x0000, got 0x%04x\n",
             progname, libspectrum_snap_memptr( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_96( void )
{
  /* libspectrum_snap: ULA and 128K memory port getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_96: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_snap_set_out_ula( snap, 0x07 );
  if( libspectrum_snap_out_ula( snap ) != 0x07 ) {
    fprintf( stderr, "%s: test_96: expected out_ula=0x07, got 0x%02x\n",
             progname, libspectrum_snap_out_ula( snap ) );
    goto done;
  }

  libspectrum_snap_set_out_128_memoryport( snap, 0x05 );
  if( libspectrum_snap_out_128_memoryport( snap ) != 0x05 ) {
    fprintf( stderr, "%s: test_96: expected out_128_memoryport=0x05, got 0x%02x\n",
             progname, libspectrum_snap_out_128_memoryport( snap ) );
    goto done;
  }

  libspectrum_snap_set_out_ay_registerport( snap, 0x0e );
  if( libspectrum_snap_out_ay_registerport( snap ) != 0x0e ) {
    fprintf( stderr, "%s: test_96: expected out_ay_registerport=0x0e, got 0x%02x\n",
             progname, libspectrum_snap_out_ay_registerport( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_97( void )
{
  /* libspectrum_snap: custom_rom flag and custom_rom_pages getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_97: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: test_97: default custom_rom should be 0, got %d\n",
             progname, libspectrum_snap_custom_rom( snap ) );
    goto done;
  }

  libspectrum_snap_set_custom_rom( snap, 1 );
  if( libspectrum_snap_custom_rom( snap ) != 1 ) {
    fprintf( stderr, "%s: test_97: expected custom_rom=1, got %d\n",
             progname, libspectrum_snap_custom_rom( snap ) );
    goto done;
  }

  libspectrum_snap_set_custom_rom( snap, 0 );
  if( libspectrum_snap_custom_rom( snap ) != 0 ) {
    fprintf( stderr, "%s: test_97: expected custom_rom=0, got %d\n",
             progname, libspectrum_snap_custom_rom( snap ) );
    goto done;
  }

  if( libspectrum_snap_custom_rom_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: test_97: default custom_rom_pages should be 0, got %lu\n",
             progname, (unsigned long)libspectrum_snap_custom_rom_pages( snap ) );
    goto done;
  }

  libspectrum_snap_set_custom_rom_pages( snap, 2 );
  if( libspectrum_snap_custom_rom_pages( snap ) != 2 ) {
    fprintf( stderr, "%s: test_97: expected custom_rom_pages=2, got %lu\n",
             progname, (unsigned long)libspectrum_snap_custom_rom_pages( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_98( void )
{
  /* libspectrum_snap: RAM pages getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *page;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_98: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_pages( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: test_98: default pages[0] should be NULL\n", progname );
    goto done;
  }

  page = libspectrum_new( libspectrum_byte, 0x4000 );
  page[0] = 0xaa;
  page[0x3fff] = 0x55;

  libspectrum_snap_set_pages( snap, 0, page );
  if( libspectrum_snap_pages( snap, 0 ) != page ) {
    fprintf( stderr, "%s: test_98: pages[0] pointer mismatch after set\n", progname );
    libspectrum_free( page );
    goto done;
  }

  if( libspectrum_snap_pages( snap, 0 )[0] != 0xaa ||
      libspectrum_snap_pages( snap, 0 )[0x3fff] != 0x55 ) {
    fprintf( stderr, "%s: test_98: pages[0] data mismatch\n", progname );
    goto done;
  }

  if( libspectrum_snap_pages( snap, 1 ) != NULL ) {
    fprintf( stderr, "%s: test_98: pages[1] should still be NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  /* snap_free frees all pages including the one we set */
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_99( void )
{
  /* libspectrum_snap: interface1 active, paged, and drive_count getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_99: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_interface1_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_99: default interface1_active should be 0, got %d\n",
             progname, libspectrum_snap_interface1_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface1_active( snap, 1 );
  if( libspectrum_snap_interface1_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_99: expected interface1_active=1, got %d\n",
             progname, libspectrum_snap_interface1_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_interface1_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: test_99: default interface1_paged should be 0, got %d\n",
             progname, libspectrum_snap_interface1_paged( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface1_paged( snap, 1 );
  if( libspectrum_snap_interface1_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: test_99: expected interface1_paged=1, got %d\n",
             progname, libspectrum_snap_interface1_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_interface1_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: test_99: default interface1_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_interface1_drive_count( snap ) );
    goto done;
  }

  libspectrum_snap_set_interface1_drive_count( snap, 4 );
  if( libspectrum_snap_interface1_drive_count( snap ) != 4 ) {
    fprintf( stderr, "%s: test_99: expected interface1_drive_count=4, got %d\n",
             progname, libspectrum_snap_interface1_drive_count( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_100( void )
{
  /* libspectrum_snap: DivIDE interface flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_100: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divide_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_100: default divide_active should be 0, got %d\n",
             progname, libspectrum_snap_divide_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_active( snap, 1 );
  if( libspectrum_snap_divide_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_100: expected divide_active=1, got %d\n",
             progname, libspectrum_snap_divide_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_eprom_writeprotect( snap ) != 0 ) {
    fprintf( stderr, "%s: test_100: default divide_eprom_writeprotect should be 0, got %d\n",
             progname, libspectrum_snap_divide_eprom_writeprotect( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_eprom_writeprotect( snap, 1 );
  if( libspectrum_snap_divide_eprom_writeprotect( snap ) != 1 ) {
    fprintf( stderr, "%s: test_100: expected divide_eprom_writeprotect=1, got %d\n",
             progname, libspectrum_snap_divide_eprom_writeprotect( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: test_100: default divide_paged should be 0, got %d\n",
             progname, libspectrum_snap_divide_paged( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_paged( snap, 1 );
  if( libspectrum_snap_divide_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: test_100: expected divide_paged=1, got %d\n",
             progname, libspectrum_snap_divide_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_control( snap ) != 0 ) {
    fprintf( stderr, "%s: test_100: default divide_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_divide_control( snap ) );
    goto done;
  }

  libspectrum_snap_set_divide_control( snap, 0xa3 );
  if( libspectrum_snap_divide_control( snap ) != 0xa3 ) {
    fprintf( stderr, "%s: test_100: expected divide_control=0xa3, got 0x%02x\n",
             progname, libspectrum_snap_divide_control( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_101( void )
{
  /* libspectrum_snap: SpecDrum active flag and DAC getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_101: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_specdrum_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_101: default specdrum_active should be 0, got %d\n",
             progname, libspectrum_snap_specdrum_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_specdrum_active( snap, 1 );
  if( libspectrum_snap_specdrum_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_101: expected specdrum_active=1, got %d\n",
             progname, libspectrum_snap_specdrum_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_specdrum_dac( snap ) != 0 ) {
    fprintf( stderr, "%s: test_101: default specdrum_dac should be 0, got %d\n",
             progname, (int)libspectrum_snap_specdrum_dac( snap ) );
    goto done;
  }

  libspectrum_snap_set_specdrum_dac( snap, -42 );
  if( libspectrum_snap_specdrum_dac( snap ) != -42 ) {
    fprintf( stderr, "%s: test_101: expected specdrum_dac=-42, got %d\n",
             progname, (int)libspectrum_snap_specdrum_dac( snap ) );
    goto done;
  }

  libspectrum_snap_set_specdrum_dac( snap, 100 );
  if( libspectrum_snap_specdrum_dac( snap ) != 100 ) {
    fprintf( stderr, "%s: test_101: expected specdrum_dac=100, got %d\n",
             progname, (int)libspectrum_snap_specdrum_dac( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_102( void )
{
  /* libspectrum_snap: Fuller Box active flag getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_102: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_fuller_box_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_102: default fuller_box_active should be 0, got %d\n",
             progname, libspectrum_snap_fuller_box_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_fuller_box_active( snap, 1 );
  if( libspectrum_snap_fuller_box_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_102: expected fuller_box_active=1, got %d\n",
             progname, libspectrum_snap_fuller_box_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_fuller_box_active( snap, 0 );
  if( libspectrum_snap_fuller_box_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_102: expected fuller_box_active=0, got %d\n",
             progname, libspectrum_snap_fuller_box_active( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_103( void )
{
  /* libspectrum_snap: Multiface interface flags getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_103: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_multiface_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_103: default multiface_active should be 0, got %d\n",
             progname, libspectrum_snap_multiface_active( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_active( snap, 1 );
  if( libspectrum_snap_multiface_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_103: expected multiface_active=1, got %d\n",
             progname, libspectrum_snap_multiface_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: test_103: default multiface_paged should be 0, got %d\n",
             progname, libspectrum_snap_multiface_paged( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_paged( snap, 1 );
  if( libspectrum_snap_multiface_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: test_103: expected multiface_paged=1, got %d\n",
             progname, libspectrum_snap_multiface_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_model_one( snap ) != 0 ) {
    fprintf( stderr, "%s: test_103: default multiface_model_one should be 0, got %d\n",
             progname, libspectrum_snap_multiface_model_one( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_model_one( snap, 1 );
  if( libspectrum_snap_multiface_model_one( snap ) != 1 ) {
    fprintf( stderr, "%s: test_103: expected multiface_model_one=1, got %d\n",
             progname, libspectrum_snap_multiface_model_one( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_model_128( snap ) != 0 ) {
    fprintf( stderr, "%s: test_103: default multiface_model_128 should be 0, got %d\n",
             progname, libspectrum_snap_multiface_model_128( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_model_128( snap, 1 );
  if( libspectrum_snap_multiface_model_128( snap ) != 1 ) {
    fprintf( stderr, "%s: test_103: expected multiface_model_128=1, got %d\n",
             progname, libspectrum_snap_multiface_model_128( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_disabled( snap ) != 0 ) {
    fprintf( stderr, "%s: test_103: default multiface_disabled should be 0, got %d\n",
             progname, libspectrum_snap_multiface_disabled( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_disabled( snap, 1 );
  if( libspectrum_snap_multiface_disabled( snap ) != 1 ) {
    fprintf( stderr, "%s: test_103: expected multiface_disabled=1, got %d\n",
             progname, libspectrum_snap_multiface_disabled( snap ) );
    goto done;
  }

  if( libspectrum_snap_multiface_software_lockout( snap ) != 0 ) {
    fprintf( stderr, "%s: test_103: default multiface_software_lockout should be 0, got %d\n",
             progname, libspectrum_snap_multiface_software_lockout( snap ) );
    goto done;
  }

  libspectrum_snap_set_multiface_software_lockout( snap, 1 );
  if( libspectrum_snap_multiface_software_lockout( snap ) != 1 ) {
    fprintf( stderr, "%s: test_103: expected multiface_software_lockout=1, got %d\n",
             progname, libspectrum_snap_multiface_software_lockout( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_104( void )
{
  /* libspectrum_snap: ay_registers array getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;
  int i;

  if( !snap ) {
    fprintf( stderr, "%s: test_104: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  /* Default: all 16 AY registers should be zero */
  for( i = 0; i < 16; i++ ) {
    if( libspectrum_snap_ay_registers( snap, i ) != 0 ) {
      fprintf( stderr, "%s: test_104: default ay_registers[%d] should be 0, got %d\n",
               progname, i, libspectrum_snap_ay_registers( snap, i ) );
      goto done;
    }
  }

  /* Set each register to a unique value and verify round-trip */
  for( i = 0; i < 16; i++ )
    libspectrum_snap_set_ay_registers( snap, i, (libspectrum_byte)(i + 1) );

  for( i = 0; i < 16; i++ ) {
    if( libspectrum_snap_ay_registers( snap, i ) != (libspectrum_byte)(i + 1) ) {
      fprintf( stderr, "%s: test_104: expected ay_registers[%d]=%d, got %d\n",
               progname, i, i + 1, libspectrum_snap_ay_registers( snap, i ) );
      goto done;
    }
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_105( void )
{
  /* libspectrum_snap: beta disk interface getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_105: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_beta_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_105: default beta_active should be 0, got %d\n",
             progname, libspectrum_snap_beta_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_active( snap, 1 );
  if( libspectrum_snap_beta_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_105: expected beta_active=1, got %d\n",
             progname, libspectrum_snap_beta_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: test_105: default beta_paged should be 0, got %d\n",
             progname, libspectrum_snap_beta_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_paged( snap, 1 );
  if( libspectrum_snap_beta_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: test_105: expected beta_paged=1, got %d\n",
             progname, libspectrum_snap_beta_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_autoboot( snap ) != 0 ) {
    fprintf( stderr, "%s: test_105: default beta_autoboot should be 0, got %d\n",
             progname, libspectrum_snap_beta_autoboot( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_autoboot( snap, 1 );
  if( libspectrum_snap_beta_autoboot( snap ) != 1 ) {
    fprintf( stderr, "%s: test_105: expected beta_autoboot=1, got %d\n",
             progname, libspectrum_snap_beta_autoboot( snap ) );
    goto done;
  }

  if( libspectrum_snap_beta_drive_count( snap ) != 0 ) {
    fprintf( stderr, "%s: test_105: default beta_drive_count should be 0, got %d\n",
             progname, libspectrum_snap_beta_drive_count( snap ) );
    goto done;
  }
  libspectrum_snap_set_beta_drive_count( snap, 4 );
  if( libspectrum_snap_beta_drive_count( snap ) != 4 ) {
    fprintf( stderr, "%s: test_105: expected beta_drive_count=4, got %d\n",
             progname, libspectrum_snap_beta_drive_count( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_106( void )
{
  /* libspectrum_snap: ZXATASP active/upload/writeprotect getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_106: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxatasp_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_106: default zxatasp_active should be 0, got %d\n",
             progname, libspectrum_snap_zxatasp_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_active( snap, 1 );
  if( libspectrum_snap_zxatasp_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_106: expected zxatasp_active=1, got %d\n",
             progname, libspectrum_snap_zxatasp_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_upload( snap ) != 0 ) {
    fprintf( stderr, "%s: test_106: default zxatasp_upload should be 0, got %d\n",
             progname, libspectrum_snap_zxatasp_upload( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_upload( snap, 1 );
  if( libspectrum_snap_zxatasp_upload( snap ) != 1 ) {
    fprintf( stderr, "%s: test_106: expected zxatasp_upload=1, got %d\n",
             progname, libspectrum_snap_zxatasp_upload( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_writeprotect( snap ) != 0 ) {
    fprintf( stderr, "%s: test_106: default zxatasp_writeprotect should be 0, got %d\n",
             progname, libspectrum_snap_zxatasp_writeprotect( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_writeprotect( snap, 1 );
  if( libspectrum_snap_zxatasp_writeprotect( snap ) != 1 ) {
    fprintf( stderr, "%s: test_106: expected zxatasp_writeprotect=1, got %d\n",
             progname, libspectrum_snap_zxatasp_writeprotect( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_107( void )
{
  /* libspectrum_snap: ZXATASP port_a/b/c and control getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_107: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxatasp_port_a( snap ) != 0 ) {
    fprintf( stderr, "%s: test_107: default zxatasp_port_a should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_a( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_port_a( snap, 0xaa );
  if( libspectrum_snap_zxatasp_port_a( snap ) != 0xaa ) {
    fprintf( stderr, "%s: test_107: expected zxatasp_port_a=0xaa, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_a( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_port_b( snap ) != 0 ) {
    fprintf( stderr, "%s: test_107: default zxatasp_port_b should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_b( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_port_b( snap, 0xbb );
  if( libspectrum_snap_zxatasp_port_b( snap ) != 0xbb ) {
    fprintf( stderr, "%s: test_107: expected zxatasp_port_b=0xbb, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_b( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_port_c( snap ) != 0 ) {
    fprintf( stderr, "%s: test_107: default zxatasp_port_c should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_c( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_port_c( snap, 0xcc );
  if( libspectrum_snap_zxatasp_port_c( snap ) != 0xcc ) {
    fprintf( stderr, "%s: test_107: expected zxatasp_port_c=0xcc, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_port_c( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_control( snap ) != 0 ) {
    fprintf( stderr, "%s: test_107: default zxatasp_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_control( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_control( snap, 0x9b );
  if( libspectrum_snap_zxatasp_control( snap ) != 0x9b ) {
    fprintf( stderr, "%s: test_107: expected zxatasp_control=0x9b, got 0x%02x\n",
             progname, libspectrum_snap_zxatasp_control( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_108( void )
{
  /* libspectrum_snap: ZXATASP pages and current_page getter/setter */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_108: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_zxatasp_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: test_108: default zxatasp_pages should be 0, got %zu\n",
             progname, libspectrum_snap_zxatasp_pages( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_pages( snap, 8 );
  if( libspectrum_snap_zxatasp_pages( snap ) != 8 ) {
    fprintf( stderr, "%s: test_108: expected zxatasp_pages=8, got %zu\n",
             progname, libspectrum_snap_zxatasp_pages( snap ) );
    goto done;
  }

  if( libspectrum_snap_zxatasp_current_page( snap ) != 0 ) {
    fprintf( stderr, "%s: test_108: default zxatasp_current_page should be 0, got %zu\n",
             progname, libspectrum_snap_zxatasp_current_page( snap ) );
    goto done;
  }
  libspectrum_snap_set_zxatasp_current_page( snap, 5 );
  if( libspectrum_snap_zxatasp_current_page( snap ) != 5 ) {
    fprintf( stderr, "%s: test_108: expected zxatasp_current_page=5, got %zu\n",
             progname, libspectrum_snap_zxatasp_current_page( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_110( void )
{
  /* libspectrum_snap: DivIDE pages count and divide_eprom single-pointer */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *eprom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_110: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divide_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: test_110: default divide_pages should be 0, got %zu\n",
             progname, libspectrum_snap_divide_pages( snap ) );
    goto done;
  }
  libspectrum_snap_set_divide_pages( snap, 3 );
  if( libspectrum_snap_divide_pages( snap ) != 3 ) {
    fprintf( stderr, "%s: test_110: expected divide_pages=3, got %zu\n",
             progname, libspectrum_snap_divide_pages( snap ) );
    goto done;
  }

  if( libspectrum_snap_divide_eprom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: test_110: default divide_eprom[0] should be NULL\n", progname );
    goto done;
  }

  eprom = libspectrum_new( libspectrum_byte, 0x2000 );
  eprom[0]      = 0xde;
  eprom[0x1fff] = 0xad;

  libspectrum_snap_set_divide_eprom( snap, 0, eprom );
  if( libspectrum_snap_divide_eprom( snap, 0 ) != eprom ) {
    fprintf( stderr, "%s: test_110: divide_eprom[0] pointer mismatch after set\n", progname );
    libspectrum_free( eprom );
    goto done;
  }
  if( libspectrum_snap_divide_eprom( snap, 0 )[0]      != 0xde ||
      libspectrum_snap_divide_eprom( snap, 0 )[0x1fff] != 0xad ) {
    fprintf( stderr, "%s: test_110: divide_eprom[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_111( void )
{
  /* libspectrum_snap: DivIDE RAM page pointer array (SNAPSHOT_DIVIDE_PAGES pages) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *page0, *page1;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_111: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divide_ram( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: test_111: default divide_ram[0] should be NULL\n", progname );
    goto done;
  }

  page0 = libspectrum_new( libspectrum_byte, 0x2000 );
  page0[0]      = 0x11;
  page0[0x1fff] = 0x22;

  libspectrum_snap_set_divide_ram( snap, 0, page0 );
  if( libspectrum_snap_divide_ram( snap, 0 ) != page0 ) {
    fprintf( stderr, "%s: test_111: divide_ram[0] pointer mismatch after set\n", progname );
    libspectrum_free( page0 );
    goto done;
  }
  if( libspectrum_snap_divide_ram( snap, 0 )[0]      != 0x11 ||
      libspectrum_snap_divide_ram( snap, 0 )[0x1fff] != 0x22 ) {
    fprintf( stderr, "%s: test_111: divide_ram[0] data mismatch\n", progname );
    goto done;
  }

  page1 = libspectrum_new( libspectrum_byte, 0x2000 );
  page1[0] = 0x33;
  libspectrum_snap_set_divide_ram( snap, 1, page1 );

  if( libspectrum_snap_divide_ram( snap, 2 ) != NULL ) {
    fprintf( stderr, "%s: test_111: divide_ram[2] should still be NULL\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_112( void )
{
  /* libspectrum_snap: DivMMC flags (active, eprom_writeprotect, paged, control) */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_112: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divmmc_active( snap ) != 0 ) {
    fprintf( stderr, "%s: test_112: default divmmc_active should be 0, got %d\n",
             progname, libspectrum_snap_divmmc_active( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_active( snap, 1 );
  if( libspectrum_snap_divmmc_active( snap ) != 1 ) {
    fprintf( stderr, "%s: test_112: expected divmmc_active=1, got %d\n",
             progname, libspectrum_snap_divmmc_active( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_eprom_writeprotect( snap ) != 0 ) {
    fprintf( stderr, "%s: test_112: default divmmc_eprom_writeprotect should be 0, got %d\n",
             progname, libspectrum_snap_divmmc_eprom_writeprotect( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_eprom_writeprotect( snap, 1 );
  if( libspectrum_snap_divmmc_eprom_writeprotect( snap ) != 1 ) {
    fprintf( stderr, "%s: test_112: expected divmmc_eprom_writeprotect=1, got %d\n",
             progname, libspectrum_snap_divmmc_eprom_writeprotect( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_paged( snap ) != 0 ) {
    fprintf( stderr, "%s: test_112: default divmmc_paged should be 0, got %d\n",
             progname, libspectrum_snap_divmmc_paged( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_paged( snap, 1 );
  if( libspectrum_snap_divmmc_paged( snap ) != 1 ) {
    fprintf( stderr, "%s: test_112: expected divmmc_paged=1, got %d\n",
             progname, libspectrum_snap_divmmc_paged( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_control( snap ) != 0x00 ) {
    fprintf( stderr, "%s: test_112: default divmmc_control should be 0, got 0x%02x\n",
             progname, libspectrum_snap_divmmc_control( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_control( snap, 0xc7 );
  if( libspectrum_snap_divmmc_control( snap ) != 0xc7 ) {
    fprintf( stderr, "%s: test_112: expected divmmc_control=0xc7, got 0x%02x\n",
             progname, libspectrum_snap_divmmc_control( snap ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

static test_return_t
test_113( void )
{
  /* libspectrum_snap: DivMMC pages count and divmmc_eprom single-pointer */
  libspectrum_snap *snap = libspectrum_snap_alloc();
  libspectrum_byte *eprom;
  test_return_t r = TEST_FAIL;

  if( !snap ) {
    fprintf( stderr, "%s: test_113: snap_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  if( libspectrum_snap_divmmc_pages( snap ) != 0 ) {
    fprintf( stderr, "%s: test_113: default divmmc_pages should be 0, got %zu\n",
             progname, libspectrum_snap_divmmc_pages( snap ) );
    goto done;
  }
  libspectrum_snap_set_divmmc_pages( snap, 16 );
  if( libspectrum_snap_divmmc_pages( snap ) != 16 ) {
    fprintf( stderr, "%s: test_113: expected divmmc_pages=16, got %zu\n",
             progname, libspectrum_snap_divmmc_pages( snap ) );
    goto done;
  }

  if( libspectrum_snap_divmmc_eprom( snap, 0 ) != NULL ) {
    fprintf( stderr, "%s: test_113: default divmmc_eprom[0] should be NULL\n", progname );
    goto done;
  }

  eprom = libspectrum_new( libspectrum_byte, 0x4000 );
  eprom[0]      = 0xbe;
  eprom[0x3fff] = 0xef;

  libspectrum_snap_set_divmmc_eprom( snap, 0, eprom );
  if( libspectrum_snap_divmmc_eprom( snap, 0 ) != eprom ) {
    fprintf( stderr, "%s: test_113: divmmc_eprom[0] pointer mismatch after set\n", progname );
    libspectrum_free( eprom );
    goto done;
  }
  if( libspectrum_snap_divmmc_eprom( snap, 0 )[0]      != 0xbe ||
      libspectrum_snap_divmmc_eprom( snap, 0 )[0x3fff] != 0xef ) {
    fprintf( stderr, "%s: test_113: divmmc_eprom[0] data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_snap_free( snap );
  return r;
}

/* Test that PZX archive info tags (title + author) are correctly parsed.
   Regression test for the pzx_read_string bug where *ptr was set to end,
   causing all tag-value pairs after the title to be silently ignored. */
static test_return_t
test_109( void )
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
    fprintf( stderr, "%s: test_104: no ARCHIVE_INFO block found\n", progname );
    goto done;
  }

  /* Expect: title (ID 0x00, "Test Game") and Author (ID 0x02, "Joe Bloggs") */
  if( libspectrum_tape_block_count( block ) != 2 ) {
    fprintf( stderr, "%s: test_104: expected 2 archive info entries, got %zu\n",
             progname, libspectrum_tape_block_count( block ) );
    r = TEST_FAIL;
    goto done;
  }

  if( libspectrum_tape_block_ids( block, 0 ) != 0x00 ) {
    fprintf( stderr, "%s: test_104: expected ID 0x00 for entry 0, got 0x%02x\n",
             progname, libspectrum_tape_block_ids( block, 0 ) );
    r = TEST_FAIL;
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 0 ), "Test Game" ) != 0 ) {
    fprintf( stderr, "%s: test_104: expected title 'Test Game', got '%s'\n",
             progname, libspectrum_tape_block_texts( block, 0 ) );
    r = TEST_FAIL;
    goto done;
  }

  if( libspectrum_tape_block_ids( block, 1 ) != 0x02 ) {
    fprintf( stderr, "%s: test_104: expected ID 0x02 (Author) for entry 1, got 0x%02x\n",
             progname, libspectrum_tape_block_ids( block, 1 ) );
    r = TEST_FAIL;
    goto done;
  }

  if( strcmp( libspectrum_tape_block_texts( block, 1 ), "Joe Bloggs" ) != 0 ) {
    fprintf( stderr, "%s: test_104: expected author 'Joe Bloggs', got '%s'\n",
             progname, libspectrum_tape_block_texts( block, 1 ) );
    r = TEST_FAIL;
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_tape_free( tape );
  return r;
}

struct test_description {

  test_fn test;
  const char *description;
  int active;

};

static struct test_description tests[] = {
  { test_1, "Tape with unknown block", 0 },
  { test_2, "TZX turbo data with zero pilot pulses and zero data", 0 },
  { test_3, "Writing empty .tap file", 0 },
  { test_4, "Invalid compressed file 1", 0 },
  { test_5, "Invalid compressed file 2", 0 },
  { test_6, "Pointer wraparound in SZX file", 0 },
  { test_7, "Invalid TZX GDB", 0 },
  { test_8, "Empty TZX DRB", 0 },
  { test_9, "Invalid TZX archive info block", 0 },
  { test_10, "Invalid hardware info block causes memory leak", 0 },
  { test_11, "Invalid Warajevo tape file", 0 },
  { test_12, "Invalid TZX custom info block causes memory leak", 0 },
  { test_13, "TZX loop end block with loop start block", 0 },
  { test_14, "TZX loop blocks", 0 },
  { test_15, "Complete TZX file", 0 },
  { test_16, "TZX loop blocks 2", 0 },
  { test_17, "TZX jump blocks", 0 },
  { test_18, "CSW empty file", 0 },
  { test_19, "Complete TZX to TAP conversion", 0 },
  { test_20, "SNA file with SP < 0x4000", 0 },
  { test_21, "SNA file with SP = 0xffff", 0 },
  { test_22, "MDR write protection 1", 0 },
  { test_23, "MDR write protection 2", 0 },
  { test_24, "Complete TZX timings", 0 },
  { test_25, "Writing SNA file", 0 },
  { test_26, "Writing +3 .Z80 file", 0 },
  { test_27, "Reading old SZX file", 0 },
  { test_28, "Zero tail length PZX file", 0 },
  { test_29, "No pilot pulse GDB TZX file", 0 },
  { test_30, "CSW conversion", 0 },
  { test_31, "Write SZX Z80R chunk", 0 },
  { test_32, "Write SZX SPCR chunk", 0 },
  { test_33, "Write SZX JOY chunk", 0 },
  { test_34, "Write SZX KEYB chunk", 0 },
  { test_35, "Write SZX ZXPR chunk", 0 },
  { test_36, "Write SZX AY chunk", 0 },
  { test_37, "Write SZX SCLD chunk", 0 },
  { test_38, "Write SZX ZXAT chunk", 0 },
  { test_39, "Write SZX ZXCF chunk", 0 },
  { test_40, "Write SZX AMXM chunk", 0 },
  { test_41, "Write SZX SIDE chunk", 0 },
  { test_42, "Write SZX DRUM chunk", 0 },
  { test_43, "Write SZX COVX chunk", 0 },
  { test_44, "Read SZX Z80R chunk", 0 },
  { test_45, "Read SZX SPCR chunk", 0 },
  { test_46, "Read SZX JOY chunk", 0 },
  { test_47, "Read SZX KEYB chunk", 0 },
  { test_48, "Read SZX ZXPR chunk", 0 },
  { test_49, "Read SZX AY chunk", 0 },
  { test_50, "Read SZX SCLD chunk", 0 },
  { test_51, "Read SZX ZXAT chunk", 0 },
  { test_52, "Read SZX ZXCF chunk", 0 },
  { test_53, "Read SZX AMXM chunk", 0 },
  { test_54, "Read SZX SIDE chunk", 0 },
  { test_55, "Read SZX DRUM chunk", 0 },
  { test_56, "Read SZX COVX chunk", 0 },
  { test_57, "Write SZX ZMMC chunk", 0 },
  { test_58, "Read SZX ZMMC chunk", 0 },
  { test_59, "Write SZX RAMP chunk", 0 },
  { test_60, "Read SZX RAMP chunk", 0 },
  { test_61, "Write SZX ATRP chunk", 0 },
  { test_62, "Write SZX CFRP chunk", 0 },
  { test_63, "Read SZX ATRP chunk", 0 },
  { test_64, "Read SZX CFRP chunk", 0 },
  { test_65, "Write uncompressed SZX RAMP chunk", 0 },
  { test_66, "Write uncompressed SZX ATRP chunk", 0 },
  { test_67, "Write uncompressed SZX CFRP chunk", 0 },
  { test_68, "Read uncompressed SZX RAMP chunk", 0 },
  { test_69, "Read uncompressed SZX ATRP chunk", 0 },
  { test_70, "Read uncompressed SZX CFRP chunk", 0 },
  { test_71, "Write RZX with incompressible snap", 0 },
  { test_72, "Tape peek next block", 0 },
  { test_73, "Read TZX RAW block edge handling", 0 },
  { test_74, "Trailing pause block TZX file", 0 },
  { test_75, "Read mono WAV threshold fixture", 0 },
  { test_76, "Read stereo WAV mixdown fixture", 0 },
  { test_77, "Buffer write_byte stores single byte", 0 },
  { test_78, "Buffer write_word stores little-endian word", 0 },
  { test_79, "Buffer write_dword stores little-endian dword", 0 },
  { test_80, "Buffer write_buffer copies src contents to dest", 0 },
  { test_81, "Buffer is_empty and is_not_empty", 0 },
  { test_82, "Buffer clear resets data size to zero", 0 },
  { test_83, "Buffer set fills N bytes with given value", 0 },
  { test_84, "Buffer append copies src to raw byte buffer", 0 },
  { test_85, "Creator alloc/free and program getter/setter", 0 },
  { test_86, "Creator major and minor version getter/setter", 0 },
  { test_87, "Creator competition_code and custom data getter/setter", 0 },
  { test_88, "Snap main Z80 register getter/setter (a, f, bc, de, hl, alternates)", 0 },
  { test_89, "Snap index and special register getter/setter (ix, iy, i, r, sp, pc)", 0 },
  { test_90, "Snap Z80 status getter/setter (iff1, iff2, im, tstates, halted)", 0 },
  { test_91, "Microdrive alloc/free and write_protect getter/setter", 0 },
  { test_92, "Microdrive cartridge_len and data getter/setter", 0 },
  { test_93, "Microdrive mdr_write/mdr_read roundtrip", 0 },
  { test_94, "Snap machine type getter/setter and default value", 0 },
  { test_95, "Snap memptr getter/setter", 0 },
  { test_96, "Snap ULA, 128K memory port, and AY register port getter/setter", 0 },
  { test_97, "Snap custom_rom flag and custom_rom_pages getter/setter", 0 },
  { test_98, "Snap RAM pages getter/setter", 0 },
  { test_99, "Snap interface1 active, paged, and drive_count getter/setter", 0 },
  { test_100, "Snap DivIDE active, eprom_writeprotect, paged, and control getter/setter", 0 },
  { test_101, "Snap SpecDrum active flag and signed DAC getter/setter", 0 },
  { test_102, "Snap Fuller Box active flag getter/setter", 0 },
  { test_103, "Snap Multiface active, paged, model, disabled, and software_lockout getter/setter", 0 },
  { test_104, "Snap AY registers array getter/setter (all 16 registers)", 0 },
  { test_105, "Snap beta disk active, paged, autoboot, and drive_count getter/setter", 0 },
  { test_106, "Snap ZXATASP active, upload, and writeprotect getter/setter", 0 },
  { test_107, "Snap ZXATASP port_a/b/c and control getter/setter", 0 },
  { test_108, "Snap ZXATASP pages and current_page getter/setter", 0 },
  { test_109, "PZX archive info tags (title and Author) correctly parsed", 0 },
  { test_110, "Snap DivIDE pages count and divide_eprom pointer getter/setter", 0 },
  { test_111, "Snap DivIDE RAM page pointer array getter/setter", 0 },
  { test_112, "Snap DivMMC active, eprom_writeprotect, paged, and control getter/setter", 0 },
  { test_113, "Snap DivMMC pages count and divmmc_eprom pointer getter/setter", 0 }
};

static size_t test_count = ARRAY_SIZE( tests );

static void
parse_test_specs( char **specs, int count )
{
  int i, j;

  for( i = 0; i < count; i++ ) {

    const char *spec = specs[i];
    const char *dash = strchr( spec, '-' );

    if( dash ) {
      int begin = atoi( spec ), end = atoi( dash + 1 );
      if( begin < 1 ) begin = 1;
      if( end == 0 || end > test_count ) end = test_count;
      for( j = begin; j <= end; j++ ) tests[j-1].active = 1;
    } else {
      int test = atoi( spec );
      if( test < 1 || test > test_count ) continue;
      tests[ test - 1 ].active = 1;
    }
    
  }
}

int
main( int argc, char *argv[] )
{
  struct test_description *test;
  size_t i;
  int tests_done = 0, tests_skipped = 0;
  int pass = 0, fail = 0, incomplete = 0;

  progname = argv[0];

  if( libspectrum_check_version( LIBSPECTRUM_MIN_VERSION ) ) {
    if( libspectrum_init() ) return 2;
  } else {
    fprintf( stderr, "%s: libspectrum version %s found, but %s required",
	     progname, libspectrum_version(), LIBSPECTRUM_MIN_VERSION );
    return 2;
  }

  if( argc < 2 ) {
    for( i = 0; i < test_count; i++ ) tests[i].active = 1;
  } else {
    parse_test_specs( &argv[1], argc - 1 );
  }

  for( i = 0, test = tests;
       i < test_count;
       i++, test++ ) {
    printf( "Test %d: %s... ", (int)i + 1, test->description );
    if( test->active ) {
      tests_done++;
      switch( test->test() ) {
      case TEST_PASS:
        /* Test executed completely and passed */
	      printf( "passed\n" );
	      pass++;
	      break;
      case TEST_FAIL:
        /* Test executed completely but failed */
	      printf( "FAILED\n" );
	      fail++;
	      break;
      case TEST_INCOMPLETE:
        /* Error occurred while executing test */
	      printf( "NOT COMPLETE\n" );
	      incomplete++;
	      break;
      case TEST_SKIPPED:
        /* Not possible to run this test (missing dependencies) */
	      printf( "skipped\n" );
	      tests_skipped++;
	      break;
      }
    } else {
      tests_skipped++;
      printf( "skipped\n" );
    }
      
  }

  /* Stop silly divisions occuring */
  if( !tests_done ) tests_done = 1;

  printf( "\n%3d tests run\n\n", (int)test_count );
  printf( "%3d     passed (%6.2f%%)\n", pass, 100 * (float)pass/tests_done );
  printf( "%3d     failed (%6.2f%%)\n", fail, 100 * (float)fail/tests_done );
  printf( "%3d incomplete (%6.2f%%)\n", incomplete, 100 * (float)incomplete/tests_done );
  printf( "%3d    skipped\n", tests_skipped );

  if( fail == 0 && incomplete == 0 ) {
    return 0;
  } else {
    return 1;
  }
}
