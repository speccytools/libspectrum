/* file.c: coherent loaded-file representation
   Copyright (c) 2026 Fredrick Meunier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
*/

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "internals.h"

#define LIBSPECTRUM_FILE_MAX_COMPRESSION_DEPTH 32
#define LIBSPECTRUM_FILE_IDENTIFY_LENGTH 64

void
libspectrum_file_init( libspectrum_file *file )
{
  memset( file, 0, sizeof( *file ) );
  file->type = LIBSPECTRUM_ID_UNKNOWN;
  file->file_class = LIBSPECTRUM_CLASS_UNKNOWN;
}

void
libspectrum_file_clear( libspectrum_file *file )
{
  libspectrum_free( file->filename );
  libspectrum_free( file->buffer );
  file->filename = NULL;
  file->buffer = NULL;
  file->length = 0;
  file->type = LIBSPECTRUM_ID_UNKNOWN;
  file->file_class = LIBSPECTRUM_CLASS_UNKNOWN;
  file->storage = LIBSPECTRUM_FILE_STORAGE_NONE;
}

static libspectrum_error
file_resolve( libspectrum_file *file )
{
  size_t depth;

  for( depth = 0; depth < LIBSPECTRUM_FILE_MAX_COMPRESSION_DEPTH; depth++ ) {
    libspectrum_error error;

    error = libspectrum_identify_file_raw( &file->type, file->filename,
                                            file->buffer, file->length );
    if( error ) return error;

    error = libspectrum_identify_class( &file->file_class, file->type );
    if( error ) return error;

    if( file->file_class != LIBSPECTRUM_CLASS_COMPRESSED )
      return LIBSPECTRUM_ERROR_NONE;

    {
      char *new_filename = NULL;
      libspectrum_byte *new_buffer = NULL;
      size_t new_length = 0;

      error = libspectrum_uncompress_file( &new_buffer, &new_length,
                                            &new_filename, file->type,
                                            file->buffer, file->length,
                                            file->filename );
      if( error ) return error;

      libspectrum_free( file->filename );
      libspectrum_free( file->buffer );
      file->filename = new_filename;
      file->buffer = new_buffer;
      file->length = new_length;
      file->type = LIBSPECTRUM_ID_UNKNOWN;
      file->file_class = LIBSPECTRUM_CLASS_UNKNOWN;
    }
  }

  libspectrum_print_error( LIBSPECTRUM_ERROR_LIMIT,
                            "too many nested compression layers" );
  return LIBSPECTRUM_ERROR_LIMIT;
}

static libspectrum_error
file_take_data( libspectrum_file *file, const char *filename,
                libspectrum_byte *buffer, size_t length )
{
  libspectrum_file replacement;
  libspectrum_error error;

  memset( &replacement, 0, sizeof( replacement ) );
  replacement.buffer = buffer;
  replacement.length = length;
  replacement.type = LIBSPECTRUM_ID_UNKNOWN;
  replacement.file_class = LIBSPECTRUM_CLASS_UNKNOWN;

  if( filename ) {
    replacement.filename = libspectrum_safe_strdup( filename );
    if( !replacement.filename ) {
      libspectrum_file_clear( &replacement );
      return LIBSPECTRUM_ERROR_MEMORY;
    }
  }

  error = file_resolve( &replacement );
  if( error ) {
    libspectrum_file_clear( &replacement );
    return error;
  }

  replacement.storage = LIBSPECTRUM_FILE_STORAGE_BUFFER;
  libspectrum_file_clear( file );
  *file = replacement;
  return LIBSPECTRUM_ERROR_NONE;
}

libspectrum_file*
libspectrum_file_alloc( void )
{
  libspectrum_file *file = libspectrum_new( libspectrum_file, 1 );
  if( !file ) return NULL;
  libspectrum_file_init( file );
  return file;
}

void
libspectrum_file_free( libspectrum_file *file )
{
  if( !file ) return;
  libspectrum_file_clear( file );
  libspectrum_free( file );
}

libspectrum_error
libspectrum_file_open( libspectrum_file *file, const char *filename )
{
  FILE *stream;
  long file_length;
  size_t header_length;
  libspectrum_byte header[ LIBSPECTRUM_FILE_IDENTIFY_LENGTH ];
  libspectrum_byte *buffer = NULL;
  libspectrum_id_t type;
  libspectrum_class_t class;
  libspectrum_error error;

  if( !file || !filename ) return LIBSPECTRUM_ERROR_INVALID;

  stream = fopen( filename, "rb" );
  if( !stream ) return LIBSPECTRUM_ERROR_UNKNOWN;
  if( fseek( stream, 0, SEEK_END ) ||
      ( file_length = ftell( stream ) ) < 0 ||
      fseek( stream, 0, SEEK_SET ) ) {
    fclose( stream );
    return LIBSPECTRUM_ERROR_UNKNOWN;
  }

  header_length = (size_t)file_length < sizeof( header ) ?
                  (size_t)file_length : sizeof( header );
  if( header_length && fread( header, 1, header_length, stream ) != header_length ) {
    fclose( stream );
    return LIBSPECTRUM_ERROR_UNKNOWN;
  }

  error = libspectrum_identify_file_raw( &type, NULL, header, header_length );
  if( !error ) error = libspectrum_identify_class( &class, type );
  if( error ) {
    fclose( stream );
    return error;
  }
  if( class == LIBSPECTRUM_CLASS_HARDDISK ) {
    libspectrum_file replacement;
    memset( &replacement, 0, sizeof( replacement ) );
    replacement.filename = libspectrum_safe_strdup( filename );
    if( !replacement.filename ) {
      fclose( stream );
      return LIBSPECTRUM_ERROR_MEMORY;
    }
    replacement.type = type;
    replacement.file_class = class;
    replacement.storage = LIBSPECTRUM_FILE_STORAGE_PATH;
    if( fclose( stream ) ) {
      libspectrum_file_clear( &replacement );
      return LIBSPECTRUM_ERROR_UNKNOWN;
    }
    libspectrum_file_clear( file );
    *file = replacement;
    return LIBSPECTRUM_ERROR_NONE;
  }

  if( fseek( stream, 0, SEEK_SET ) ) {
    fclose( stream );
    return LIBSPECTRUM_ERROR_UNKNOWN;
  }
  if( file_length ) {
    buffer = libspectrum_new( libspectrum_byte, (size_t)file_length );
    if( !buffer ) {
      fclose( stream );
      return LIBSPECTRUM_ERROR_MEMORY;
    }
    if( fread( buffer, 1, (size_t)file_length, stream ) != (size_t)file_length ) {
      libspectrum_free( buffer );
      fclose( stream );
      return LIBSPECTRUM_ERROR_UNKNOWN;
    }
  }
  if( fclose( stream ) ) {
    libspectrum_free( buffer );
    return LIBSPECTRUM_ERROR_UNKNOWN;
  }

  return file_take_data( file, filename, buffer, (size_t)file_length );
}

libspectrum_error
libspectrum_file_read_data( libspectrum_file *file, const char *filename,
                            const libspectrum_byte *buffer, size_t length )
{
  libspectrum_byte *copy = NULL;

  if( !file || ( length && !buffer ) ) return LIBSPECTRUM_ERROR_INVALID;
  if( length ) {
    copy = libspectrum_new( libspectrum_byte, length );
    if( !copy ) return LIBSPECTRUM_ERROR_MEMORY;
    memcpy( copy, buffer, length );
  }
  return file_take_data( file, filename, copy, length );
}

libspectrum_error
libspectrum_file_take_data( libspectrum_file *file, const char *filename,
                            libspectrum_byte *buffer, size_t length )
{
  if( !file || ( length && !buffer ) ) {
    libspectrum_free( buffer );
    return LIBSPECTRUM_ERROR_INVALID;
  }
  return file_take_data( file, filename, buffer, length );
}

const char *libspectrum_file_name( const libspectrum_file *file )
{ return file ? file->filename : NULL; }

const libspectrum_byte *libspectrum_file_data( const libspectrum_file *file )
{ return file ? file->buffer : NULL; }

size_t libspectrum_file_size( const libspectrum_file *file )
{ return file ? file->length : 0; }

libspectrum_id_t libspectrum_file_type( const libspectrum_file *file )
{ return file ? file->type : LIBSPECTRUM_ID_UNKNOWN; }

libspectrum_class_t libspectrum_file_class( const libspectrum_file *file )
{ return file ? file->file_class : LIBSPECTRUM_CLASS_UNKNOWN; }

libspectrum_file_storage
libspectrum_file_get_storage( const libspectrum_file *file )
{ return file ? file->storage : LIBSPECTRUM_FILE_STORAGE_NONE; }
