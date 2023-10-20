#!perl -w
# quick and dirty PoC for CSES Grid Paths using bignums
use strict;
use warnings;
no warnings 'recursion';
use Getopt::Std;
use Time::HiRes qw( time );
use bignum;

# var opts
use vars qw/$opt_v $opt_m $opt_i $opt_p $opt_r $opt_s $opt_t/;

sub numberOfPaths
{
  my($m,$n) = @_;
  # some optimization
  return 1 if ( 1 == $m || 1 == $n );
  return $m if ( 2 == $n );
  return $n if ( 2 == $m );
  my $path = 1;
    for ( my $i = $n; $i < ($m + $n - 1); $i++) {
        $path *= $i;
        $path /= ($i - $n + 1);
    }
    return $path;
}

# both array contains refs to the same traps, just sorted in different order
my(@g_sorted_y, @g_sorted_x);
my($g_q, $Res, $N, $begin_time);

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
# returns count of deleted traps
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
  printf("mark_y %d %d x_min %d\n", $g_sorted_y[$i]->[0], $g_sorted_y[$i]->[1], $x_min) if ( defined $opt_v );
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
# returns count of deleted traps
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
  printf("mark_x %d %d y_min %d\n", $g_sorted_x[$i]->[0], $g_sorted_x[$i]->[1], $y_min) if ( defined $opt_v );
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

# if we have traps located like
#   A
#  BC - then we can remove C
# using g_sorted_y and tracking presence A in previous row
# returns count of deleted traps
sub rm_3
{
  my $l = scalar(@g_sorted_y);
  return 0 if ( $l < 2 );
  my $cnt = 0;
  for ( my $i = 2; $i < $l; $i++ )
  {
    if ( $g_sorted_y[$i]->[0] != 1 && # we must be at least at row 2
         $g_sorted_y[$i]->[0] == $g_sorted_y[$i-1]->[0] && # on the same row with prev
         $g_sorted_y[$i]->[1] == $g_sorted_y[$i-1]->[1] + 1 
       )
    {
      # find on prev row traps with X == [1]
      for ( my $j = $i - 2; $j >= 0; $j-- )
      {
        last if ( $g_sorted_y[$j]->[0] < $g_sorted_y[$i]->[0] - 1 );
        next if ( $g_sorted_y[$j]->[0] != $g_sorted_y[$i]->[0] - 1 );
        if ( $g_sorted_y[$j]->[1] == $g_sorted_y[$i]->[1] )
        {
          $cnt++;
          $g_sorted_y[$i]->[4] = 1; # mark it
  printf("mark_3 %d %d\n", $g_sorted_y[$i]->[0], $g_sorted_y[$i]->[1]) if ( defined $opt_v );
          last;
        }
      }
    }
  }
  return $cnt;
}

# mark traps in the same row with [1] <= X
# returns count of removed traps
sub rm_r
{
  my($from, $x, $l, $v) = @_;
  my $res = 0;
  for ( my $i = $from + 1; $i < $l; $i++ )
  {
    # check if Y is still the same
    last if ( $g_sorted_y[$i]->[0] != $g_sorted_y[$from]->[0] );
    last if ( $g_sorted_y[$i]->[1] > $x );
printf("mark_r %d %d\n", $g_sorted_y[$i]->[0], $g_sorted_y[$i]->[1]) if ( defined $opt_v );    
    $g_sorted_y[$i]->[4] = $v; # mark this trap
    $res++;
  }
  return $res;
}

sub rm_r2
{
  my($from, $x, $l, $v) = @_;
  my $res = 0;
  my $fr = $g_sorted_y[$from]->[5];
  for ( my $i = $from + 1; $i < $l; $i++ )
  {
    # check if Y is still the same
    last if ( $g_sorted_y[$i]->[0] != $g_sorted_y[$from]->[0] );
    last if ( $g_sorted_y[$i]->[1] > $x );
    if ( $g_sorted_y[$i]->[5] == $fr ) {
      # we can remove it only if got chain from it`s upper neightbour 
      if ( defined $g_sorted_y[$i]->[6] )
      {
        $g_sorted_y[$i]->[4] = 4;
        $res++;
      }
      next; 
    }
printf("mark_r2 %d %d\n", $g_sorted_y[$i]->[0], $g_sorted_y[$i]->[1]) if ( defined $opt_v );    
    $g_sorted_y[$i]->[4] = $v; # mark this trap
    $res++;
  }
  return $res;
}

# try to find adjacent trap at right and up sides
# returns [trap with most right $X, number of removed traps]
sub scan_rup
{
  my($from, $l) = @_;
  my $ct = $g_sorted_y[$from]; # currently processed trap
  return [ $ct->[5], 0 ] if ( defined $ct->[5] );
  my $myres = [$ct, 0];
  # check right cell
  if ( $from < $l - 1 && $ct->[0] == $g_sorted_y[$from+1]->[0] &&
       $ct->[1] == $g_sorted_y[$from+1]->[1] - 1 )
  {
    my $rres = scan_rup($from + 1, $l);
    if ( defined $rres )
    {
      printf("rup(%d, %d) right cell at %d %d\n", $ct->[0], $ct->[1], 
        $rres->[0]->[0], $rres->[0]->[1]) if ( defined $opt_v );
      $myres->[0] = $rres->[0] if ( $rres->[0]->[1] > $myres->[0]->[1] );
      $myres->[1] += $rres->[1];
    }
  }
# printf("scan_rup(%d, %d) res %d %d\n", $ct->[0], $ct->[1], $myres->[0]->[0], $myres->[0]->[1]);
  # process 2 adjacent upper cells
  for ( my $i = $from - 1; $i >= 0; $i-- )
  {
    next if ( $g_sorted_y[$i]->[0] == $ct->[0] );    # skip traps on the same Y
    last if ( $g_sorted_y[$i]->[0] + 1 < $ct->[0] ); # stop if trap has Y < ct->Y - 1
    last if ( $g_sorted_y[$i]->[1] < $ct->[1] );     # stop if trap has X < ct->X
    if ( $g_sorted_y[$i]->[1] == $ct->[1] + 1 || $g_sorted_y[$i]->[1] == $ct->[1] )
    {
      my $next = scan_rup($i, $l);
      next if ( !defined $next );
      printf("rup(%d, %d) upper cell at %d %d\n", $ct->[0], $ct->[1], 
        $next->[0]->[0], $next->[0]->[1]) if ( defined $opt_v );
      $myres->[1] += rm_r($from, $next->[0]->[1], $l, 2) if ( $ct->[0] != $next->[0]->[0]);
      $myres->[1] += $next->[1];
      if ( $next->[0]->[1] > $myres->[0]->[1] )
      {
        $myres->[0] = $next->[0];
        $ct->[6] = $g_sorted_y[$i] if ( $g_sorted_y[$i]->[1] == $ct->[1] );
      }
    }
  }
  # mark this trap as visited
  $ct->[5] = $myres->[0];
  printf("rup(%d, %d) ends at %d %d\n", $ct->[0], $ct->[1],
    $myres->[0]->[0], $myres->[0]->[1]) if ( defined $opt_v && $ct->[5] != $ct );
  $myres->[1] += rm_r2($from, $myres->[0]->[1], $l, 3) if ( $ct->[0] != $myres->[0]->[0]);
  return $myres;
}

# generalized version of rm_3
# uses g_sorted_y and tracking presence Y-1, X+1 in previous row
# returns number of removed traps
sub rm_gen
{
  my $l = scalar(@g_sorted_y);
  return 0 if ( $l < 2 );
  my $res = 0;
  # scan in back direction
  for ( my $i = $l - 1; $i >= 0; $i-- )
  {
     next if ( defined $g_sorted_y[$i]->[5] ); # already processed
     next if ( $g_sorted_y[$i]->[4] );
     my $xr = scan_rup($i, $l);
     next if ( !defined $xr || $g_sorted_y[$i] == $xr->[0] );
 printf("rm_gen %d %d X %d\n", $g_sorted_y[$i]->[0], $g_sorted_y[$i]->[1], $xr->[0]->[1]) if ( defined $opt_v );
     $res += $xr->[1];
  }
  return $res;
}

sub dump_map
{
  my $cl = 1;
  my $str;
  for ( my $i = 0; $i < scalar @g_sorted_y; $i++ )
  {
    if ( $g_sorted_y[$i]->[0] != $cl )
    {
      printf("%s\n", $str) if defined($str);
      printf("\n") if ( $g_sorted_y[$i]->[0] > $cl + 1 );
      $cl = $g_sorted_y[$i]->[0];
      undef $str;
    }
    $str = ' ' x $N if ( !defined $str );
    my $c = 'X';
    $c = 'd' if ( $g_sorted_y[$i]->[4] == 1 );
    $c = 'r' if ( $g_sorted_y[$i]->[4] == 2 );
    $c = '?' if ( $g_sorted_y[$i]->[4] == 3 );
    $c = '+' if ( $g_sorted_y[$i]->[4] == 4 );
    $c = '@' if ( $g_sorted_y[$i]->[0] == 56 && $g_sorted_y[$i]->[1] == 96 );
    substr($str, $g_sorted_y[$i]->[1] - 1, 1, $c);
  }
  # last one
  printf("%s\n", $str) if defined($str);
}

sub rm_odd
{
  my $cx = rm_odd_x();
  my $cy = rm_odd_y();
  my $r3 = rm_gen();
  dump_map() if ( defined $opt_m );
  return if ( !$cx && !$cy && !$r3 );
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
      printf("y %d x %d up %d\n", $_->[0], $_->[1], defined($_->[6]) ? 1 : 0);
    }
  }
  exit(0) if ( defined $opt_m );
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

sub propagate
{
  my($t, $from) = @_;
  for ( my $i = $from + 1; $i < scalar @g_sorted_x; $i++ )
  {
    if ( $g_sorted_x[$i]->[0] >= $t->[0] && $g_sorted_x[$i]->[1] >= $t->[1] )
    {
      my $diff = $t->[2] * numberOfPaths($g_sorted_x[$i]->[0] - $t->[0] + 1, $g_sorted_x[$i]->[1] - $t->[1] + 1);
#  printf("prop %d %d: was %d diff %d\n", $g_sorted_x[$i]->[0], $g_sorted_x[$i]->[1], $g_sorted_x[$i]->[2], $diff) 
#    if ( $g_sorted_x[$i]->[2] < $diff );
  #   if ( $g_sorted_x[$i]->[0] == 56 && $g_sorted_x[$i]->[1] == 96 )
  #    {  printf("Duff %d %d ", $t->[0], $t->[1]); print $diff % 1000000007, "\n"; }
      $g_sorted_x[$i]->[2] -= $diff;
    }
  }
}

sub calc2
{
  # calc from_s and to_s
  foreach ( @g_sorted_x )
  {
    $_->[2] = numberOfPaths($_->[0], $_->[1]); # from_s
print("init ", $_->[0], " ", $_->[1], " ", $_->[2], "\n") if defined($opt_i);
    $_->[3] = numberOfPaths($N - $_->[0] + 1, $N - $_->[1] + 1); # to_s
  }
  if ( defined $opt_t )
  {
    my $end_time = time();
    printf("inits: %f\n", $end_time - $begin_time);
  }
  # propagate decreasing of from_s to all traps below and at right
  for ( my $i = 0; $i < scalar @g_sorted_x; $i++ )
  {
    propagate($g_sorted_x[$i], $i);
  }
  if ( defined $opt_t )
  {
    my $end_time = time();
    printf("propagate: %f\n", $end_time - $begin_time);
  }
  # calc total diff
  foreach ( @g_sorted_x )
  {
    $Res -= $_->[3] * $_->[2];
  }
}

sub usage
{
  print STDERR<<EOF;
Usage: $0 [options]
Options:
 -i -- dump initial values
 -m -- dump traps map
 -p -- dump paths
 -r -- dump remained unvisited traps
 -s -- dump sorted vertices
 -t -- dump time of execution
 -v -- verbose mode
EOF
  exit(8);

}

# MAIN
my $status = getopts("imprstv");
usage() if ( !$status );
my $str;
$str = <>;
chomp $str;
die if ( $str !~ /(\d+) (\d+)$/ );
$N = int($1); $g_q = int($2);
# calc res
$Res = numberOfPaths($N, $N);
print("initial res ", $Res, "\n");
my @traps;
$begin_time = time();
# read traps
for ( my $i = 0; $i < $g_q; $i++ )
{
  $str = <>;
  chomp $str;
  die if ( $str !~ /(\d+) (\d+)$/ );
  my $y = int($1);
  my $x = int($2);
  # indexes: 0 - y, 1 - x, 2 - from_s, 3 - to_t, 4 - marked for removal, 5 - part of chain
  # 6 if you got chain from upper element
  push @traps, [ $y, $x, 0, 0, 0, undef, undef ];
}
# sort first by Y, then by X in ascending order
@g_sorted_y = sort { return $a->[0] == $b->[0] ? $a->[1] <=> $b->[1] : $a->[0] <=> $b->[0] } @traps;
# sort first by X, then by Y in ascending order
@g_sorted_x = sort { return $a->[1] == $b->[1] ? $a->[0] <=> $b->[0] : $a->[1] <=> $b->[1] } @traps;
if ( defined $opt_s )
{
  printf("sorted_y:\n");
  foreach ( @g_sorted_y )
  {
    printf("y %d x %d\n", $_->[0], $_->[1]);
  }
  printf("sorted_x:\n");
  foreach ( @g_sorted_x )
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
if ( $g_q ) { calc2(); }
if ( defined $opt_t )
{
  my $end_time = time();
  printf("calcs: %f\n", $end_time - $begin_time);
}
print $Res;
printf("\nmod res %d\n", $Res % 1000000007);
if ( defined $opt_r )
{
  printf("remained traps:\n");
  foreach ( @g_sorted_y )
  {
    printf("%d %d - ", $_->[0], $_->[1]);
    print $_->[2], " % ", $_->[2] % 1000000007, "\n";
  }
}