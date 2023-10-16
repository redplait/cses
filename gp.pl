#!perl -w
# quick and dirty PoC for CSES Grid Paths
use strict;
use warnings;
no warnings 'recursion';
use Getopt::Std;
use Time::HiRes qw( time );
use bignum;

# var opts
use vars qw/$opt_v $opt_p $opt_s $opt_t/;

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

# both array contains refs to the same traps, just sorted in different order
my(@g_sorted_y, @g_sorted_x);
my $g_q;

# find nearest Y less or eq $item->[0] wuth X < $item->[1]
# returns index of item in g_sorted_y or -1 if no such items can be found
sub find_near_y
{
  my $item = shift;
  return -1 if ( $g_sorted_y[0]->[0] > $item->[0] ); # no lesser in whole array
  # some kind of binary search
  my $min = 0;
  my $max = scalar(@g_sorted_y) - 1;
  while ($min <= $max) {
# printf("min %d max %d\n", $min, $max);
    if ( $min == $max || $min + 1 == $max )
    {
      for ( my $i = $max; $i >= 0; --$i )
      {
        next if ( $g_sorted_y[$i]->[0] > $item->[0] || $g_sorted_y[$i]->[1] > $item->[1] );
        next if ( $g_sorted_y[$i]->[0] == $item->[0] && $g_sorted_y[$i]->[1] == $item->[1] ); # skip myself
        return $i;
      }
      return -1;
    }
    my $middle = int(($max+$min) / 2);
    if ( $g_sorted_y[$middle]->[0] < $item->[0] )
    {
      $min = $middle;
      next;
    }
    if ( $g_sorted_y[$middle]->[0] > $item->[0] )
    {
      $max = $middle;
      next;
    }
    for ( my $i = $max; $i >= 0; --$i )
    {
      next if ( $g_sorted_y[$i]->[0] > $item->[0] || $g_sorted_y[$i]->[1] > $item->[1] );
      next if ( $g_sorted_y[$i]->[0] == $item->[0] && $g_sorted_y[$i]->[1] == $item->[1] ); # skip myself
      return $i;
    }
    return -1;
  }
}

# find nearest X less or eq $item->[1] wuth Y < $item->[0]
# returns index of item in g_sorted_x or -1 if no such items can be found
sub find_near_x
{
  my $item = shift;
  return -1 if ( $g_sorted_x[0]->[1] > $item->[1] ); # no lesser in whole array
  # some kind of binary search
  my $min = 0;
  my $max = scalar(@g_sorted_x) - 1;
  while ($min <= $max) {
# printf("min %d max %d\n", $min, $max);
    if ( $min == $max || $min + 1 == $max )
    {
      for ( my $i = $max; $i >= 0; --$i )
      {
        next if ( $g_sorted_x[$i]->[1] > $item->[1] || $g_sorted_x[$i]->[0] > $item->[0]);
        next if ( $g_sorted_x[$i]->[0] == $item->[0] && $g_sorted_x[$i]->[1] == $item->[1] ); # skip myself
        return $i;
      }
      return -1;
    }
    my $middle = int(($max+$min) / 2);
    if ( $g_sorted_x[$middle]->[1] < $item->[1] )
    {
      $min = $middle;
      next;
    }
    if ( $g_sorted_x[$middle]->[1] > $item->[1] )
    {
      $max = $middle;
      next;
    }
    for ( my $i = $max; $i >= 0; --$i )
    {
      next if ( $g_sorted_x[$i]->[1] > $item->[1] || $g_sorted_x[$i]->[0] > $item->[0] );
      next if ( $g_sorted_x[$i]->[0] == $item->[0] && $g_sorted_x[$i]->[1] == $item->[1] ); # skip myself
      return $i;
    }
    return -1;
  }
}

# remove odd traps - like in the 1st row x1 < x2, in second row all traps with x > (x3 >= x1) etc
# so we need sorted_y array
# removed elements mark with 1 at trap->[4]
# return count of deleted traps
sub rm_odd_x
{
  my $l = scalar(@g_sorted_y);
  return 0 if ( $l < 2 );
  return 0 if ( $g_sorted_y[0]->[0] != 1 );
  my $x_min = $g_sorted_y[0]->[1];
  my $res = 0;
OUTER:
  for ( my $i = 1; $i < $l; $i++ )
  {
    if ( $g_sorted_y[$i]->[0] == $g_sorted_y[$i-1]->[0] ) # yep, it`s still the same row
    {
      if ( $g_sorted_y[$i]->[1] > $x_min )
      {
        $res++;
        $g_sorted_y[$i]->[4] = 1; # mark it
  printf("mark_y %X %d %d x_min %d\n", $g_sorted_y[$i], $g_sorted_y[$i]->[0], $g_sorted_y[$i]->[1], $x_min) if ( defined $opt_v );
      }
      next;
    }
    last if ( $g_sorted_y[$i]->[0] != $g_sorted_y[$i-1]->[0] + 1 ); # if next trap not on following row
    # lets find new x_min on this row >= current x_min
    if ( $g_sorted_y[$i]->[1] >= $x_min )
    {
      $x_min = $g_sorted_y[$i]->[1];
      next;
    }
    for ( my $j = $i + 1; $j < $l; $j++ )
    {
      return $res if ( $g_sorted_y[$i]->[0] != $g_sorted_y[$j]->[0] );
      if ( $g_sorted_y[$j]->[1] >= $x_min )
      {
        $x_min = $g_sorted_y[$j]->[1];
        next OUTER;
      }
    }
    last;
  }
  return $res;
}

# remove odd traps - like in the 1st column y1 < y2, in second column all traps with y > (y3 >= y1) etc
# so we need sorted_x array
# removed elements mark with 1 at trap->[4]
# return count of deleted traps
sub rm_odd_y
{
  my $l = scalar(@g_sorted_x);
  return 0 if ( $l < 2 );
  return 0 if ( $g_sorted_x[0]->[1] != 1 );
  my $y_min = $g_sorted_x[0]->[0];
  my $res = 0;
OUTER:
  for ( my $i = 1; $i < $l; $i++ )
  {
    if ( $g_sorted_x[$i]->[1] == $g_sorted_x[$i-1]->[1] ) # yep, it`s still the same column
    {
      if ( $g_sorted_x[$i]->[0] > $y_min )
      {
        $res++;
        $g_sorted_x[$i]->[4] = 1; # mark it
  printf("mark_x %X %d %d y_min %d\n", $g_sorted_x[$i], $g_sorted_x[$i]->[0], $g_sorted_x[$i]->[1], $y_min) if ( defined $opt_v );
      }
      next;
    }
    last if ( $g_sorted_x[$i]->[1] != $g_sorted_x[$i-1]->[1] + 1 ); # if next trap not on following column
    # lets find new y_min on this column >= current y_min
    if ( $g_sorted_x[$i]->[0] >= $y_min )
    {
      $y_min = $g_sorted_x[$i]->[0];
      next;
    }
    for ( my $j = $i + 1; $j < $l; $j++ )
    {
      return $res if ( $g_sorted_x[$i]->[1] != $g_sorted_x[$j]->[1] );
      if ( $g_sorted_x[$j]->[0] >= $y_min )
      {
        $y_min = $g_sorted_x[$j]->[0];
        next OUTER;
      }
    }
    last;
  }
  return $res;
}

sub rm_odd
{
  my $cx = rm_odd_x();
  my $cy = rm_odd_y();
  return if ( !$cx && !$cy );
  # remove from g_sorted_y
  my @tmp2;
  foreach ( @g_sorted_y )
  {
    push @tmp2, $_ if ( !$_->[4] );
  }
  @g_sorted_y = @tmp2;
  if ( defined $opt_s )
  {
    printf("sorted_y with removed odd traps size %d:\n", scalar @g_sorted_y);
    foreach ( @g_sorted_y )
    {
      printf("y %d x %d\n", $_->[0], $_->[1]);
    }
  }
  # remove from g_sorted_x
  my @tmp;
  foreach ( @g_sorted_x )
  {
    push @tmp, $_ if ( !$_->[4] );
  }
  @g_sorted_x = @tmp;
  if ( defined $opt_s )
  {
    printf("sorted_x with removed odd traps size %d:\n", scalar @g_sorted_x);
    foreach ( @g_sorted_x )
    {
      printf("y %d x %d\n", $_->[0], $_->[1]);
    }
  }
}

sub calc
{
  my $t = shift;
  return $t->[4] if ( $t->[4] );
  my $prev_y = find_near_y($t);
  my $prev_x = find_near_x($t);
  printf("prev y for %d %d is %d, prev_x %d\n", $t->[0], $t->[1], $prev_y, $prev_x) if ( defined $opt_p );
  # no previous traps - t is last one
  if ( -1 == $prev_x && -1 == $prev_y )
  {
     $t->[4] = numberOfPaths($t->[0], $t->[1]);
     return $t->[4]; 
  }
  if ( -1 == $prev_x )
  {
    my $py = $g_sorted_y[$prev_y];
    my $v = calc($py);
    $t->[2] = numberOfPaths($t->[0], $t->[1]) if ( !$t->[2] );
    $t->[4] = $t->[2] - $v * numberOfPaths($t->[0] - $py->[0] + 1, $t->[1] - $py->[1] + 1);
    return $t->[4];
  }
  if ( -1 == $prev_y )
  {
    my $px = $g_sorted_x[$prev_x];
    my $v = calc($px);
    $t->[2] = numberOfPaths($t->[0], $t->[1]) if ( !$t->[2] );
    $t->[4] = $t->[2] - $v * numberOfPaths($t->[0] - $px->[0] + 1, $t->[1] - $px->[1] + 1);
    return $t->[4];
  }
  my $py = $g_sorted_y[$prev_y];
  my $px = $g_sorted_x[$prev_x];
  if ( $py == $px )
  {
    my $v = calc($px);
    $t->[2] = numberOfPaths($t->[0], $t->[1]) if ( !$t->[2] );
    $t->[4] = $t->[2] - $v * numberOfPaths($t->[0] - $px->[0] + 1, $t->[1] - $px->[1] + 1);
    return $t->[4];
  }
  # finally calc both
  my $vx = calc($px);
  my $vy = calc($py);
  $t->[2] = numberOfPaths($t->[0], $t->[1]) if ( !$t->[2] );
  $t->[4] = $t->[2] - 
    $vx * numberOfPaths($t->[0] - $px->[0] + 1, $t->[1] - $px->[1] + 1) -
    $vy * numberOfPaths($t->[0] - $py->[0] + 1, $t->[1] - $py->[1] + 1)
  ;
  return $t->[4];
}

sub usage
{
  print STDERR<<EOF;
Usage: $0 [options]
Options:
 -p -- dump paths
 -s -- dump sorted vertices
 -t -- dump time of execution
 -v -- verbose mode
EOF
  exit(8);

}

# MAIN
my $status = getopts("pstv");
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
  push @traps, [ $y, $x, 0, 0, 0 ];
}
# sort first by Y, then by X in ascending order
@g_sorted_y = sort { return $a->[0] == $b->[0] ? $a->[1] <=> $b->[1] : $a->[0] <=> $b->[0] } @traps;
# sort first by X, then by Y in ascending order
@g_sorted_x = sort { return $a->[1] == $b->[1] ? $a->[0] <=> $b->[0] : $a->[1] <=> $b->[1] } @traps;
if ( defined $opt_s )
{
  foreach ( @g_sorted_y )
  {
    printf("y %d x %d\n", $_->[0], $_->[1]);
  }
}
rm_odd();
if ( defined $opt_t )
{
  my $end_time = time();
  printf("read traps: %f\n", $end_time - $begin_time);
}
# lets calc
if ( $g_q ) {
  my $last = [ $N, $N, $Res, 1, 0 ];
  calc($last);
  $Res -= $last->[4];
}
if ( defined $opt_t )
{
  my $end_time = time();
  printf("calcs: %f\n", $end_time - $begin_time);
}
print $Res;
printf("\nmod res %d\n", $Res % 1000000007);