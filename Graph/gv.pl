#!perl -w
# simple and dirty script to generate graph viz output for CSES graph tasks
use strict;
use warnings;
use Getopt::Std;
use Data::Dumper;

# var opts
use vars qw/$opt_d $opt_c $opt_C $opt_k $opt_V/;

# global vars
my $g_N = 0;
# graph - map with key - vertex id, value is ref to array wuth indexes
# 0 - id
# 1 - map with out edges
# 2 - map with in edges (for directed graphs)
my %G; 

sub degree
{
  my $id = shift;
  return 0 if ( !exists $G{$id} );
  my $v = $G{$id};
  return 0 if ( !defined $v->[1] );
  return scalar keys %{ $v->[1] };
}

sub in_degree
{
  my $id = shift;
  return 0 if ( !exists $G{$id} );
  my $v = $G{$id};
  return 0 if ( !defined $v->[2] );
  return scalar keys %{ $v->[2] };
}

# check if this vertex has 1 in and 1 out edges
sub is_d11
{
  my $id = shift;
  return undef if ( !exists $G{$id} );
  my $v = $G{$id};
  return undef if ( !defined $v->[2] || !defined $v->[1] );
  my @kin = keys %{ $v->[2] };
  my @kout = keys %{ $v->[1] };
# printf("%d kin %d kout %d\n", $id, scalar @kin, scalar @kout);  
  if ( (1 == scalar @kin) and (1 == scalar @kout) )
  {
    return [ $kout[0], $kin[0] ];
  }
  return undef;
}

# check if vertex of undrected graph has degree 2
sub is_d2
{
  my $id = shift;
  return undef if ( !exists $G{$id} );
  my $v = $G{$id};
  return undef if ( !defined $v->[1] );
  my @k = keys %{ $v->[1] };
  return undef if ( 2 != scalar @k );
  return [ $k[0], $k[1] ];
}

sub add_edge
{
  my($from, $to) = @_;
  return if ( $from == $to );
# printf("ae %d %d\n", $from, $to);
  if ( !exists $G{$from} )
  {
    $G{$from} = [ $from, {}, {} ];
  }
  if ( !exists $G{$to} )
  {
    $G{$to} = [ $to, {}, {} ];
  }
  my $fv = $G{$from};
  my $tv = $G{$to};
  if ( defined $opt_d )
  {
    my $out = $fv->[1];
    $out->{$to} = 1;
    my $in = $tv->[2];
    $in->{$from} = 1;
  } else {
    my $out = $fv->[1];
    $out->{$to} = 1;
    my $in = $tv->[1];
    $in->{$from} = 1;
  }
}

# check if vertex with id is S - so no in edges and non-zero amount of out
sub is_S
{
  my $id = shift;
  return 0 if ( !exists $G{$id} );
  my $v = $G{$id};
  return 0 if ( !defined $v->[2] || !defined $v->[1] );
  return 0 if ( !scalar(keys %{ $v->[1] }) );
  return ! scalar keys %{ $v->[2] };
}

# check if vertex with id is T - so no out edges and non-zero amount of in
sub is_T
{
  my $id = shift;
  return 0 if ( !exists $G{$id} );
  my $v = $G{$id};
  return 0 if ( !defined $v->[2] || !defined $v->[1] );
  return 0 if ( !scalar keys %{ $v->[2] } );
  return ! scalar keys %{ $v->[1] };
}

sub usage
{
  print STDERR<<EOF;
Usage: $0 [options]
Options:
 -c -- compact
 -C -- find cut-points
 -d -- direct graph
 -V id - draw only connectivity component with vertex id
EOF
  exit(8);
}

# graphviz syntax: https://www.graphviz.org/pdf/dotguide.pdf
sub dump_head
{
  if ( defined $opt_d )
  {
    print "digraph G {\n";
  } else {
    print "graph G {\n";
  }
}

# dump S/T vertices. Magnet has blue color for N and red for S, traditionally S located in top and S at botton
# so color for S in blue and for T is red
sub dump_st
{
  foreach my $i ( 1 .. 1+$g_N )
  {
    if ( is_S($i) )
    {
      printf(" v%d [color = blue];\n", $i);
    } elsif ( is_T($i) )
    {
      printf(" v%d [color = red];\n", $i);
    }
  }
}

sub dump_graph
{
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( !exists $G{$i} );
    my $v = $G{$i};
    next if ( !defined $v->[1]);
    my $out = $v->[1];
    foreach my $e ( keys %$out )
    {
      printf("v%d -- v%d;\n", $i, $e);
      # remove edge e-i
      $v = $G{$e};
      delete $v->[1]->{$i};
    }
  }
}

# dump edges of directed graph
sub dump_dir
{
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( !exists $G{$i} );
    my $v = $G{$i};
    next if ( !defined $v->[1]);
    my $out = $v->[1];
    foreach my $e ( keys %$out )
    {
      printf("v%d -> v%d;\n", $i, $e);
    }
  }
}

sub compact
{
  my $res = 0;
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( !exists $G{$i} );
    my $v = $G{$i};
    if ( defined $opt_d )
    {
      my $oi = is_d11($i);
      next if ( !defined $oi );
 # printf("c %d out %d in %d\n", $i, $oi->[0], $oi->[1]);    
      # patch out-edge
      my $oe = $G{$oi->[0]};
      my $ie = $G{$oi->[1]};
      delete $ie->[1]->{$i};
      $ie->[1]->{$oe->[0]} = 1;
      # patch in-edge
      delete $oe->[2]->{$i};
      $oe->[2]->{$ie->[0]} = 1;
    } else {
      my $e2 = is_d2($i);
      next if ( !defined $e2 );
      my $v1 = $G{$e2->[0]};
      my $v2 = $G{$e2->[1]};
# printf("c %d v1 %d v2 %d\n", $i, $e2->[0], $e2->[1]);      
      delete $v1->[1]->{$i};
      delete $v2->[1]->{$i};
      # add edges v1 -> v2
      $v1->[1]->{$e2->[1]} = 1;
      $v2->[1]->{$e2->[0]} = 1;
    }
    delete $G{$i};
    $res++;
  }
  return $res;
}

# main
my $status = getopts("dcCV:");
usage() if ( !$status );

# format usually start wuth N M - number of nodes & number of edges
my $str = <>;
die("bad header") if ( $str !~ /^(\d+) (\d+)/ );
$g_N = int($1);
my $m = int($2);

# read
while( $str = <> )
{
  die("bad data") if ( $str !~ /^(\d+) (\d+)/ );
  add_edge(int($1), int($2));
  # read m edges
  last if ( ! --$m );
}
# print Dumper(\%G);
# process
compact() if defined($opt_c); 
# dump results
dump_head();
if ( defined $opt_d )
{
  dump_st();
  dump_dir();
} else {
  dump_graph();
}
printf("}");