#!/usr/bin/perl

use strict;
use warnings;

my $fixture = shift @ARGV or die "usage: $0 <fixture>\n";

my( $channels, $sample_rate, @samples );

if( $fixture eq 'mono-threshold' ) {
  $channels = 1;
  $sample_rate = 22050;
  @samples = ( 2000, -2000, 12000, -12000,
               32767, -32768, -4000, 8000 );

} elsif( $fixture eq 'stereo-mixdown' ) {
  $channels = 2;
  $sample_rate = 22050;
  @samples = (
    2000, 2000,
    2000, -2000,
    -2000, 2000,
    -2000, -2000,
    4000, 2000,
    2000, 4000,
    1, 0,
    0, 3,
  );

} else {
  die "unknown fixture '$fixture'\n";
}

my $bits_per_sample = 16;
my $bytes_per_sample = $bits_per_sample / 8;
my $block_align = $channels * $bytes_per_sample;
my $byte_rate = $sample_rate * $block_align;
my $data = pack( 's<*', @samples );
my $data_length = length( $data );
my $riff_length = 36 + $data_length;

binmode STDOUT;

print "RIFF";
print pack( 'V', $riff_length );
print "WAVE";

print "fmt ";
print pack( 'VvvVVvv', 16, 1, $channels, $sample_rate, $byte_rate,
            $block_align, $bits_per_sample );

print "data";
print pack( 'V', $data_length );
print $data;
