#!/usr/bin/perl -w

# accessor.pl: generate accessor functions for libspectrum_tape_block
# Copyright (c) 2003 Philip Kendall

# $Id$

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA

# Author contact information:

# E-mail: pak21-fuse@srcf.ucam.org
# Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

use strict;

print << "CODE";
/* tape_accessors.c: accessor functions for libspectrum_tape_block
   Copyright (c) 2003 Philip Kendall

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
   Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse\@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

/* NB: this file is autogenerated from $ARGV[0] by $0 */

#include "internals.h"

CODE

my( $name, $default, $indexed, $started );

sub trailer ($) {

    my( $name ) = @_;

    return << "CODE";

    default:
      libspectrum_print_error(
        LIBSPECTRUM_ERROR_INVALID,
        "invalid block type %d given to libspectrum_tape_block_$name",
        block->type
      );
      return $default;
  }
}

CODE
}

while( <> ) {

    s/#.*$//;

    next if /^\s*$/;

    if( /^\s/ ) {
	
	my( $type, $member ) = split;

	$member ||= $name;

	printf "    case LIBSPECTRUM_TAPE_BLOCK_%s: return block->types.$type.$member%s;\n",
	    uc $type, $indexed ? '[ index ]' : '';

    } else {

	print trailer( $name ) if $started;

	my $return_type;

	( $return_type, $name, $indexed, $default ) = split;

	print "$return_type\nlibspectrum_tape_block_$name( libspectrum_tape_block *block";
	print ', size_t index' if $indexed;
	print " )\n{\n  switch( block->type ) {\n\n";

        $started = 1;
    }
}

print trailer( $name ) if $started;
