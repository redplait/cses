#!perl -w
use strict;
use warnings;
use Getopt::Std;
use Time::HiRes qw( time );
use bignum;

# var opts
use vars qw/$opt_v $opt_p $opt_t/;

sub numberOfPaths
{
  my($m,$n) = @_;
  my $path = 1;
    for ( my $i = $n; $i < ($m + $n - 1); $i++) {
        $path *= $i;
        $path /= ($i - $n + 1);
    }
    return $path;
}

my @g_sorted;
my $g_q;

sub calc
{
  my($t, $Y, $X) = @_;
  return $t->[4] if ( $t->[4] );
  my $res = $t->[2];
print("from ", $t->[2], " to ", $t->[3], "\n") if defined($opt_v);
  my $total = 0;
  for ( my $i = 0; $i < $g_q; $i++ )
  {
     # is it my own?
     next if ( $g_sorted[$i]->[0] == $Y && $g_sorted[$i]->[1] == $X );
     last if ( $g_sorted[$i]->[0] >= $Y );
     next if ( $g_sorted[$i]->[1] >= $X );
     my $rest = numberOfPaths($Y - $g_sorted[$i]->[0] + 1, $X - $g_sorted[$i]->[1] + 1);
     my $diff = calc($g_sorted[$i], $g_sorted[$i]->[0], $g_sorted[$i]->[1]) * $rest;
     $total += $diff;
  }
# printf("calc %d %d - %d\n", Y, X, res);
  $t->[4] = $res - $total;
  return $t->[4];
}

sub usage
{
  print STDERR<<EOF;
Usage: $0 [options]
Options:
 -p -- dump sorted vertices
 -t -- dump time of execution
 -v -- verbose mode
EOF
  exit(8);

}

# MAIN
my $status = getopts("ptv");
usage() if ( !$status );
my($N, $str);
$str = <>;
chomp $str;
die if ( $str !~ /(\d+) (\d+)$/ );
$N = int($1); $g_q = int($2);
# calc res
my $Res = numberOfPaths($N, $N);
print("initial res ", $Res, "\n");
my @traps;
my $begin_time = time();
# read traps
for ( my $i = 0; $i < $g_q; $i++ )
{
  $str = <>;
  chomp $str;
  die if ( $str !~ /(\d+) (\d+)$/ );
  my $y = int($1);
  my $x = int($2);
  # 0 - y, 1 - x, 2 - from_s, 3 - to_t, 4 - cached calc
  push @traps, [ $y, $x, numberOfPaths($y, $x), numberOfPaths($N - $y + 1, $N - $x + 1), 0 ];
}
# sort first by Y, then by X in ascending order
@g_sorted = sort { return $a->[0] == $b->[0] ? $a->[1] <=> $b->[1] : $a->[0] <=> $b->[0] } @traps;
if ( defined $opt_p )
{
  for ( my $i = 0; $i < $g_q; $i++ )
  {
    printf("y %d x %d\n", $g_sorted[$i]->[0], $g_sorted[$i]->[1]);
  }
}
if ( defined $opt_t )
{
  my $end_time = time();
  printf("read traps: %f\n", $end_time - $begin_time);
}
# lets calc
my $total = 0;
for ( my $i = 0; $i < $g_q; ++$i )
{
  my $v = calc($g_sorted[$i], $g_sorted[$i]->[0], $g_sorted[$i]->[1]);
print("calc ", $g_sorted[$i]->[0], " ", $g_sorted[$i]->[1], "=", $v, "\n") if defined($opt_v);
  $total += $v * $g_sorted[$i]->[3];
}
if ( defined $opt_t )
{
  my $end_time = time();
  printf("calcs: %f\n", $end_time - $begin_time);
}
$Res -= $total;
print $Res;
printf("\nmod res %d\n", $Res % 1000000007);