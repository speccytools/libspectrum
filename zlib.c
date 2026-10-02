/* zlib.c: routines for zlib (de)compression of data
   Copyright (c) 2002 Darren Salt, Philip Kendall
   Copyright (c) 2015 Stuart Brady

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

   Darren: E-mail: linux@youmustbejoking.demon.co.uk

   Philip: E-mail: philip-fuse@shadowmagic.org.uk

*/

#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif			/* #ifdef HAVE_UNISTD_H */

#define ZLIB_CONST
#include <zlib.h>

#include "internals.h"

static libspectrum_error
zlib_inflate( const libspectrum_byte *gzptr, size_t gzlength,
	      libspectrum_byte **outptr, size_t *outlength, int window_bits,
              size_t max_outlength );

libspectrum_error 
libspectrum_zlib_inflate( const libspectrum_byte *gzptr, size_t gzlength,
			  libspectrum_byte **outptr, size_t *outlength )
/* Inflates a block of data.
 * Input:	gzptr		-> source (deflated) data
 *		*gzlength	== source data length
 * Output:	*outptr		-> inflated data (malloced in this fn)
 *		*outlength	== length of the inflated data
 * Returns:	error flag (libspectrum_error)
 */
{
  return zlib_inflate( gzptr, gzlength, outptr, outlength, 15, SIZE_MAX );
}

libspectrum_error
libspectrum_gzip_inflate( const libspectrum_byte *gzptr, size_t gzlength,
			  libspectrum_byte **outptr, size_t *outlength,
                          size_t max_outlength )
{
  return zlib_inflate( gzptr, gzlength, outptr, outlength, 15 + 16,
                       max_outlength );
}

libspectrum_error
libspectrum_zip_inflate( const libspectrum_byte *zipptr, size_t ziplength,
                         libspectrum_byte **outptr, size_t *outlength,
                         size_t max_outlength )
{
  return zlib_inflate( zipptr, ziplength, outptr, outlength, -15,
                       max_outlength );
}

static libspectrum_error
zlib_inflate( const libspectrum_byte *gzptr, size_t gzlength,
	      libspectrum_byte **outptr, size_t *outlength, int window_bits,
              size_t max_outlength )
{
  z_stream stream;
  int error;

  *outptr = NULL;

  if( *outlength > max_outlength ) {
    *outlength = 0;
    libspectrum_print_error( LIBSPECTRUM_ERROR_LIMIT,
                             "inflated data exceeds size limit" );
    return LIBSPECTRUM_ERROR_LIMIT;
  }

  /* Use default memory management */
  stream.zalloc = Z_NULL; stream.zfree = Z_NULL; stream.opaque = Z_NULL;

  stream.next_in = gzptr; stream.avail_in = gzlength;

  error = inflateInit2( &stream, window_bits );

  switch( error ) {

  case Z_OK: break;

  case Z_MEM_ERROR: 
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "out of memory at %s:%d", __FILE__, __LINE__ );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_MEMORY;

  default:
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "error from inflateInit2: %s", stream.msg );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_MEMORY;

  }

  if( *outlength ) {

    *outptr = libspectrum_new( libspectrum_byte, *outlength );
    stream.next_out = *outptr; stream.avail_out = *outlength;
    error = inflate( &stream, Z_FINISH );

  } else {

    *outptr = stream.next_out = NULL;
    *outlength = stream.avail_out = 0;

    do {

      libspectrum_byte *ptr;
      size_t increment, produced;

      if( *outlength == max_outlength ) {
        libspectrum_byte overflow;

        stream.next_out = &overflow; stream.avail_out = 1;
        error = inflate( &stream, 0 );
        if( error == Z_STREAM_END && stream.avail_out == 1 ) {
          stream.next_out = *outptr + *outlength;
          break;
        }

        libspectrum_print_error( LIBSPECTRUM_ERROR_LIMIT,
                                 "inflated data exceeds size limit" );
        inflateEnd( &stream );
        libspectrum_free( *outptr );
        *outptr = NULL; *outlength = 0;
        return LIBSPECTRUM_ERROR_LIMIT;
      }

      produced = stream.next_out ? stream.next_out - *outptr : 0;
      increment = MIN( (size_t)16384, max_outlength - *outlength );
      *outlength += increment; stream.avail_out = increment;
      ptr = libspectrum_renew( libspectrum_byte, *outptr, *outlength );
      stream.next_out = ptr + produced;
      *outptr = ptr;

      error = inflate( &stream, 0 );

    } while( error == Z_OK );

  }

  *outlength = stream.next_out - *outptr;
  *outptr = libspectrum_renew( libspectrum_byte, *outptr, *outlength );

  switch( error ) {

  case Z_STREAM_END: break;

  case Z_NEED_DICT:
    libspectrum_print_error( LIBSPECTRUM_ERROR_UNKNOWN,
			     "gzip inflation needs dictionary" );
    libspectrum_free( *outptr );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_UNKNOWN;

  case Z_DATA_ERROR:
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT, "corrupt gzip data" );
    libspectrum_free( *outptr );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_CORRUPT;

  case Z_MEM_ERROR:
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "out of memory at %s:%d", __FILE__, __LINE__ );
    libspectrum_free( *outptr );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_MEMORY;

  case Z_BUF_ERROR:
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "not enough space in gzip output buffer" );
    libspectrum_free( *outptr );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_CORRUPT;

  default:
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "gzip error from inflate: %s",
			     stream.msg );
    libspectrum_free( *outptr );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_LOGIC;

  }

  error = inflateEnd( &stream );
  if( error != Z_OK ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "gzip error from inflateEnd: %s", stream.msg );
    libspectrum_free( *outptr );
    inflateEnd( &stream );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  return LIBSPECTRUM_ERROR_NONE;
}

libspectrum_error
libspectrum_zlib_compress( const libspectrum_byte *data, size_t length,
			   libspectrum_byte **gzptr, size_t *gzlength )
/* Deflates a block of data.
 * Input:	data		-> source data
 *		length		== source data length
 * Output:	*gzptr		-> deflated data (malloced in this fn),
 *		*gzlength	== length of the deflated data
 * Returns:	error flag (libspectrum_error)
 */
{
  uLongf gzl = (uLongf)( length * 1.001 ) + 12;
  int gzret;

  *gzptr = libspectrum_new( libspectrum_byte, gzl );
  gzret = compress2( *gzptr, &gzl, data, length, Z_BEST_COMPRESSION );

  switch (gzret) {

  case Z_OK:			/* initialised OK */
    *gzlength = gzl;
    return LIBSPECTRUM_ERROR_NONE;

  case Z_MEM_ERROR:		/* out of memory */
    libspectrum_free( *gzptr ); *gzptr = 0;
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "libspectrum_zlib_compress: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;

  case Z_VERSION_ERROR:		/* unrecognised version */
    libspectrum_free( *gzptr ); *gzptr = 0;
    libspectrum_print_error( LIBSPECTRUM_ERROR_UNKNOWN,
			     "libspectrum_zlib_compress: unknown version" );
    return LIBSPECTRUM_ERROR_UNKNOWN;

  case Z_BUF_ERROR:		/* Not enough space in output buffer.
				   Shouldn't happen */
    libspectrum_free( *gzptr ); *gzptr = 0;
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "libspectrum_zlib_compress: out of space?" ); 
    return LIBSPECTRUM_ERROR_LOGIC;

  default:			/* some other error */
    libspectrum_free( *gzptr ); *gzptr = 0;
    libspectrum_print_error( LIBSPECTRUM_ERROR_LOGIC,
			     "libspectrum_zlib_compress: unexpected error?" ); 
    return LIBSPECTRUM_ERROR_LOGIC;
  }
}
