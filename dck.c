/* dck.c: Routines for handling Warajevo DCK files
   Copyright (c) 2003 Darren Salt, Fredrick Meunier

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: linux@youmustbejoking.demon.co.uk

*/

#include <string.h>

#include "internals.h"

/* Initialise a libspectrum_dck_block structure */
libspectrum_error
libspectrum_dck_block_alloc( libspectrum_dck_block **dck )
{
  size_t i;
  libspectrum_dck_block *ld;

  ld = *dck = malloc( sizeof( libspectrum_dck_block ) );
  if( !ld ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "libspectrum_dck_block_alloc: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }

  ld->bank = LIBSPECTRUM_DCK_BANK_DOCK;
  for( i = 0; i < 8; i++ ) {
    ld->access[i] = LIBSPECTRUM_DCK_PAGE_NULL;
    ld->pages[i] = NULL;
  }

  return LIBSPECTRUM_ERROR_NONE;
}

/* Free all memory used by a libspectrum_dck_block structure */
libspectrum_error
libspectrum_dck_block_free( libspectrum_dck_block *dck, int keep_pages )
{
  size_t i;

  if( !keep_pages )
    for( i = 0; i < 8; i++ )
      if( dck->pages[i] )
        free( dck->pages[i] );

  free( dck );

  return LIBSPECTRUM_ERROR_NONE;
}

/* Initialise a libspectrum_dck structure (constructor!) */
libspectrum_error
libspectrum_dck_alloc( libspectrum_dck **dck )
{
  size_t i;
  libspectrum_dck *ld;

  ld = *dck = malloc( sizeof( libspectrum_dck ) );
  if( !ld ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "libspectrum_dck_alloc: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }

  for( i=0; i<256; i++ ) ld->dck[i] = NULL;

  return LIBSPECTRUM_ERROR_NONE;
}

/* Free all memory used by a libspectrum_dck structure (destructor...) */
libspectrum_error
libspectrum_dck_free( libspectrum_dck *dck, int keep_pages )
{
  size_t i;

  for( i=0; i<256; i++ )
    if( dck->dck[i] ) {
      libspectrum_dck_block_free( dck->dck[i], keep_pages );
      dck->dck[i] = NULL;
    }

  return LIBSPECTRUM_ERROR_NONE;
}

/* Read in the DCK file */
libspectrum_error
libspectrum_dck_read( libspectrum_dck *dck, const libspectrum_byte *buffer,
		      const size_t length )
{
  int i;
  int num_dck_block = 0;
  libspectrum_error error;
  const libspectrum_byte *end = buffer + length;

  for( i=0; i<256; i++ ) dck->dck[i]=NULL;

  while( buffer < end ) {
    int pages = 0;

    if( buffer + 9 > end ) {
      libspectrum_print_error( 
        LIBSPECTRUM_ERROR_CORRUPT,
        "libspectrum_dck_read: not enough data in buffer"
      );
      return LIBSPECTRUM_ERROR_CORRUPT;
    }

    switch( buffer[0] ) {
    case LIBSPECTRUM_DCK_BANK_DOCK:
    case LIBSPECTRUM_DCK_BANK_EXROM:
    case LIBSPECTRUM_DCK_BANK_HOME:
      break;
    default:
      libspectrum_print_error( LIBSPECTRUM_ERROR_UNKNOWN,
                               "libspectrum_dck_read: unknown bank ID %d",
			       buffer[0] );
      return LIBSPECTRUM_ERROR_UNKNOWN;
    }

    for( i = 1; i < 9; i++ )
      switch( buffer[i] ) {
      case LIBSPECTRUM_DCK_PAGE_NULL:
      case LIBSPECTRUM_DCK_PAGE_RAM_EMPTY:
        break;
      case LIBSPECTRUM_DCK_PAGE_ROM:
      case LIBSPECTRUM_DCK_PAGE_RAM:
        pages++;
        break;
      default:
        libspectrum_print_error( LIBSPECTRUM_ERROR_UNKNOWN,
                                 "libspectrum_dck_read: unknown page type %d",
				 buffer[i] );
        return LIBSPECTRUM_ERROR_UNKNOWN;
      }

    if( buffer + 9 + 8192 * pages > end ) {
      libspectrum_print_error(
        LIBSPECTRUM_ERROR_CORRUPT,
        "libspectrum_dck_read: not enough data in buffer"
      );
      return LIBSPECTRUM_ERROR_CORRUPT;
    }

    /* Allocate new dck_block */
    error = libspectrum_dck_block_alloc( &dck->dck[num_dck_block] );
    if( error != LIBSPECTRUM_ERROR_NONE ) return error;

    /* Copy the bank ID */
    dck->dck[num_dck_block]->bank = *buffer++;
    /* Copy the page types */
    for( i = 0; i < 8; i++ ) dck->dck[num_dck_block]->access[i] = *buffer++;

    /* Allocate the pages */
    for( i = 0; i < 8; i++ ) {
      switch( dck->dck[num_dck_block]->access[i] ) {
      case LIBSPECTRUM_DCK_PAGE_NULL:
        break;
      case LIBSPECTRUM_DCK_PAGE_RAM_EMPTY:
        dck->dck[num_dck_block]->pages[i] = malloc( 8192 );
        if( !dck->dck[num_dck_block]->pages[i] ) {
          libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
                                   "libspectrum_dck_read: out of memory" );
          return LIBSPECTRUM_ERROR_MEMORY;
        }
        memset( dck->dck[num_dck_block]->pages[i], 0, 8192 );
        break;
      case LIBSPECTRUM_DCK_PAGE_ROM:
      case LIBSPECTRUM_DCK_PAGE_RAM:
        dck->dck[num_dck_block]->pages[i] = malloc( 8192 );
        if( !dck->dck[num_dck_block]->pages[i] ) {
          libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
                                   "libspectrum_dck_read: out of memory" );
          return LIBSPECTRUM_ERROR_MEMORY;
        }
        memcpy( dck->dck[num_dck_block]->pages[i], buffer, 8192 );
        buffer += 8192;
        break;
      }
    }

    num_dck_block++;
    if( num_dck_block == 256 ) {
      libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			       "libspectrum_dck_read: more than 256 banks" );
      return LIBSPECTRUM_ERROR_MEMORY;
    }
  }

  /* Done :-) */
  return LIBSPECTRUM_ERROR_NONE;
}