#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "test.h"

int read_file( libspectrum_byte **buffer, size_t *length,
               const char *filename );

test_return_t load_tape( libspectrum_tape **tape, const char *filename,
                         libspectrum_error expected_result );
test_return_t read_tape( const char *filename,
                         libspectrum_error expected_result );
test_return_t read_snap( const char *filename,
                         const char *filename_to_pass,
                         libspectrum_error expected_result );
test_return_t play_tape( const char *filename );

#ifdef HAVE_WAV_BACKEND
test_return_t check_wav_block( const char *filename,
                               libspectrum_dword expected_bit_length,
                               libspectrum_byte expected_byte );
#endif

#endif
