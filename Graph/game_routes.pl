#!perl -w
use strict;
use warnings;
use bignum;
no warnings 'recursion';

my %g_graph;
my $g_max; # amount of vertex
my %g_d;
my %g_v;

sub add
{
  my($src, $dst) = @_;
  if ( exists $g_graph{$dst} )
  {
    push @{ $g_graph{$dst} }, $src;
  } else {
    $g_graph{$dst} = [ $src ];
  }
}

# from https://neerc.ifmo.ru/wiki/index.php?title=%D0%97%D0%B0%D0%B4%D0%B0%D1%87%D0%B0_%D0%BE_%D1%87%D0%B8%D1%81%D0%BB%D0%B5_%D0%BF%D1%83%D1%82%D0%B5%D0%B9_%D0%B2_%D0%B0%D1%86%D0%B8%D0%BA%D0%BB%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%BE%D0%BC_%D0%B3%D1%80%D0%B0%D1%84%D0%B5
# complexity O(n)
sub calc
{
  my $v = shift;
  return $g_d{$v} if ( exists $g_v{$v} );
  my $sum = 0;
  $g_v{$v} = 1;
  my $list = $g_graph{$v};
  foreach my $n ( @$list )
  {
    $sum += calc($n);
# printf("n %d:", $n); print $sum; printf("\n");
  }
  $g_d{$v} = $sum;
  return $sum;
}

# main
my($q, $str);
$str = <>;
chomp $str;
die if ( $str !~ /(\d+) (\d+)$/ );
$g_max = int($1); $q = int($2);
for ( my $i = 0; $i < $q; $i++ )
{
  $str = <>;
  chomp $str;
  die if ( $str !~ /(\d+) (\d+)$/ );
  add(int($1), int($2));
}
# initial vertex is 1
$g_d{1} = 1; $g_v{1} = 1;
my $ans = calc($g_max);
printf("cses answer: %d\n", $ans % 1000000007);
print "real ans: "; print $ans;
