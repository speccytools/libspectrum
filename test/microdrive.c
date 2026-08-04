#include "config.h"

#include <stdio.h>

#include "common.h"
#include "test.h"

/* Tests for bug #152: .mdr code does not correctly handle write protect flag */
test_return_t
mdr_write_protection_1( void )
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

test_return_t
mdr_write_protection_2( void )
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

test_return_t
microdrive_alloc_free_and_write_protect_getter_setter( void )
{
  /* libspectrum_microdrive: alloc/free and write_protect getter/setter */
  libspectrum_microdrive *mdr = libspectrum_microdrive_alloc();
  test_return_t r = TEST_FAIL;

  if( !mdr ) {
    fprintf( stderr, "%s: microdrive_alloc_free_and_write_protect_getter_setter: microdrive_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_microdrive_set_write_protect( mdr, 1 );
  if( libspectrum_microdrive_write_protect( mdr ) != 1 ) {
    fprintf( stderr, "%s: microdrive_alloc_free_and_write_protect_getter_setter: expected write_protect 1, got %d\n",
             progname, libspectrum_microdrive_write_protect( mdr ) );
    goto done;
  }

  libspectrum_microdrive_set_write_protect( mdr, 0 );
  if( libspectrum_microdrive_write_protect( mdr ) != 0 ) {
    fprintf( stderr, "%s: microdrive_alloc_free_and_write_protect_getter_setter: expected write_protect 0, got %d\n",
             progname, libspectrum_microdrive_write_protect( mdr ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_microdrive_free( mdr );
  return r;
}

test_return_t
microdrive_cartridge_len_and_data_getter_setter( void )
{
  /* libspectrum_microdrive: cartridge_len and data getter/setter */
  libspectrum_microdrive *mdr = libspectrum_microdrive_alloc();
  test_return_t r = TEST_FAIL;

  if( !mdr ) {
    fprintf( stderr, "%s: microdrive_cartridge_len_and_data_getter_setter: microdrive_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_microdrive_set_cartridge_len( mdr, 10 );
  if( libspectrum_microdrive_cartridge_len( mdr ) != 10 ) {
    fprintf( stderr, "%s: microdrive_cartridge_len_and_data_getter_setter: expected cartridge_len 10, got %d\n",
             progname, (int)libspectrum_microdrive_cartridge_len( mdr ) );
    goto done;
  }

  libspectrum_microdrive_set_data( mdr, 0, 0xa5 );
  if( libspectrum_microdrive_data( mdr, 0 ) != 0xa5 ) {
    fprintf( stderr, "%s: microdrive_cartridge_len_and_data_getter_setter: expected data[0] 0xa5, got 0x%02x\n",
             progname, (unsigned)libspectrum_microdrive_data( mdr, 0 ) );
    goto done;
  }

  libspectrum_microdrive_set_data( mdr, 100, 0x5a );
  if( libspectrum_microdrive_data( mdr, 100 ) != 0x5a ) {
    fprintf( stderr, "%s: microdrive_cartridge_len_and_data_getter_setter: expected data[100] 0x5a, got 0x%02x\n",
             progname, (unsigned)libspectrum_microdrive_data( mdr, 100 ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_microdrive_free( mdr );
  return r;
}

test_return_t
microdrive_mdr_write_mdr_read_roundtrip( void )
{
  /* libspectrum_microdrive: mdr_write/mdr_read roundtrip */
  libspectrum_microdrive *mdr1 = libspectrum_microdrive_alloc();
  libspectrum_microdrive *mdr2 = libspectrum_microdrive_alloc();
  libspectrum_byte *buf = NULL;
  size_t buf_len = 0;
  test_return_t r = TEST_FAIL;

  if( !mdr1 || !mdr2 ) {
    fprintf( stderr, "%s: microdrive_mdr_write_mdr_read_roundtrip: microdrive_alloc returned NULL\n", progname );
    r = TEST_INCOMPLETE;
    goto done;
  }

  libspectrum_microdrive_set_write_protect( mdr1, 1 );
  libspectrum_microdrive_set_cartridge_len( mdr1, 10 );
  libspectrum_microdrive_set_data( mdr1, 5, 0xcc );

  libspectrum_microdrive_mdr_write( mdr1, &buf, &buf_len );
  if( !buf ) {
    fprintf( stderr, "%s: microdrive_mdr_write_mdr_read_roundtrip: mdr_write returned NULL buffer\n", progname );
    r = TEST_INCOMPLETE;
    goto done;
  }

  if( libspectrum_microdrive_mdr_read( mdr2, buf, buf_len ) !=
      LIBSPECTRUM_ERROR_NONE ) {
    fprintf( stderr, "%s: microdrive_mdr_write_mdr_read_roundtrip: mdr_read failed\n", progname );
    goto done;
  }

  if( libspectrum_microdrive_write_protect( mdr2 ) != 1 ) {
    fprintf( stderr, "%s: microdrive_mdr_write_mdr_read_roundtrip: roundtrip write_protect: expected 1, got %d\n",
             progname, libspectrum_microdrive_write_protect( mdr2 ) );
    goto done;
  }

  if( libspectrum_microdrive_cartridge_len( mdr2 ) != 10 ) {
    fprintf( stderr, "%s: microdrive_mdr_write_mdr_read_roundtrip: roundtrip cartridge_len: expected 10, got %d\n",
             progname, (int)libspectrum_microdrive_cartridge_len( mdr2 ) );
    goto done;
  }

  if( libspectrum_microdrive_data( mdr2, 5 ) != 0xcc ) {
    fprintf( stderr, "%s: microdrive_mdr_write_mdr_read_roundtrip: roundtrip data[5]: expected 0xcc, got 0x%02x\n",
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
