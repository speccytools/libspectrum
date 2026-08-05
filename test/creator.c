#include "config.h"

#include <stdio.h>
#include <string.h>

#include "test.h"

test_return_t
creator_alloc_free_and_program_getter_setter( void )
{
  /* libspectrum_creator: alloc/free and program getter/setter */
  libspectrum_creator *creator = libspectrum_creator_alloc();
  test_return_t r = TEST_FAIL;

  if( !creator ) {
    fprintf( stderr, "%s: creator_alloc_free_and_program_getter_setter: creator_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_creator_set_program( creator, "TestApp" );

  if( strcmp( (const char *)libspectrum_creator_program( creator ), "TestApp" ) != 0 ) {
    fprintf( stderr, "%s: creator_alloc_free_and_program_getter_setter: expected program \"TestApp\", got \"%s\"\n",
             progname, (const char *)libspectrum_creator_program( creator ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_creator_free( creator );
  return r;
}

test_return_t
creator_major_and_minor_version_getter_setter( void )
{
  /* libspectrum_creator: major and minor version getter/setter */
  libspectrum_creator *creator = libspectrum_creator_alloc();
  test_return_t r = TEST_FAIL;

  if( !creator ) {
    fprintf( stderr, "%s: creator_major_and_minor_version_getter_setter: creator_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_creator_set_major( creator, 2 );
  libspectrum_creator_set_minor( creator, 7 );

  if( libspectrum_creator_major( creator ) != 2 ) {
    fprintf( stderr, "%s: creator_major_and_minor_version_getter_setter: expected major 2, got %d\n",
             progname, libspectrum_creator_major( creator ) );
    goto done;
  }

  if( libspectrum_creator_minor( creator ) != 7 ) {
    fprintf( stderr, "%s: creator_major_and_minor_version_getter_setter: expected minor 7, got %d\n",
             progname, libspectrum_creator_minor( creator ) );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_creator_free( creator );
  return r;
}

test_return_t
creator_competition_code_and_custom_data_getter_setter( void )
{
  /* libspectrum_creator: competition_code and custom data getter/setter */
  libspectrum_creator *creator = libspectrum_creator_alloc();
  libspectrum_byte *custom_data;
  const libspectrum_byte *got_custom;
  test_return_t r = TEST_FAIL;

  if( !creator ) {
    fprintf( stderr, "%s: creator_competition_code_and_custom_data_getter_setter: creator_alloc returned NULL\n", progname );
    return TEST_INCOMPLETE;
  }

  libspectrum_creator_set_competition_code( creator, 0x1234 );

  if( libspectrum_creator_competition_code( creator ) != 0x1234 ) {
    fprintf( stderr, "%s: creator_competition_code_and_custom_data_getter_setter: expected competition_code 0x1234, got 0x%04x\n",
             progname, libspectrum_creator_competition_code( creator ) );
    goto done;
  }

  custom_data = libspectrum_new( libspectrum_byte, 4 );
  custom_data[0] = 0xde; custom_data[1] = 0xad;
  custom_data[2] = 0xbe; custom_data[3] = 0xef;

  libspectrum_creator_set_custom( creator, custom_data, 4 );

  if( libspectrum_creator_custom_length( creator ) != 4 ) {
    fprintf( stderr, "%s: creator_competition_code_and_custom_data_getter_setter: expected custom_length 4, got %lu\n",
             progname, (unsigned long)libspectrum_creator_custom_length( creator ) );
    goto done;
  }

  got_custom = libspectrum_creator_custom( creator );
  if( got_custom[0] != 0xde || got_custom[1] != 0xad ||
      got_custom[2] != 0xbe || got_custom[3] != 0xef ) {
    fprintf( stderr, "%s: creator_competition_code_and_custom_data_getter_setter: custom data mismatch\n", progname );
    goto done;
  }

  r = TEST_PASS;

done:
  libspectrum_creator_free( creator );
  return r;
}
