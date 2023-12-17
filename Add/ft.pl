#!perl -w
# generate brute-force test for Filling Trominos
use strict;
use warnings;

printf("%d\n", 101 * 101);
for ( my $i = 9; $i < 110; $i++ )
{
  for ( my $j = 9; $j < 110; $j++ )
  {
    printf("%d %d\n", $i, $j);
  }
}