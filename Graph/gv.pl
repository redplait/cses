#!perl -w
# simple and dirty script to generate graph viz output for CSES graph tasks
use strict;
use warnings;
no warnings 'recursion';
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
# 3 - ref to scc which vertex belongs to
my %G; 

# map of Strongly Connected Components
# key is just some number
# value is ref to array with indexes
# 0 - smallest vertex id
# 1 - map of vertices
my %g_scc;

# add vertex vid to SCC with index cid
# return ref to SCC
sub add2scc
{
  my($cid, $vid) = @_;
  if ( !exists $g_scc{$cid} )
  {
    my $new_cc = [ $vid, { } ];
    $new_cc->[1]->{$vid} = 1;
    $g_scc{$cid} = $new_cc;
    return $new_cc;
  }
  my $cc = $g_scc{$cid};
  $cc->[0] = $vid if ( $vid < $cc->[0] );
  $cc->[1]->{$vid} = 1;
  return $cc;
}

# if any of scc undefined then they are not considered as the same
sub cmp_scc
{
  my($cc, $id) = @_;
  return 0 if ( !defined $cc );
  return 0 if ( !exists $G{$id} );
  my $v = $G{$id};
  return 0 if ( !defined $v->[3] );
  return $cc == $v->[3];
}

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
    $G{$from} = [ $from, {}, {}, undef ];
  }
  if ( !exists $G{$to} )
  {
    $G{$to} = [ $to, {}, {}, undef ];
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
 -k -- condensate graph
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

sub dump_cs
{
  while (my ($key, $value) = each (%g_scc) )
  {
    printf("v%d [shape=box]; /*", $value->[0]);
    foreach ( sort { $a <=> $b } keys %{$value->[1]} )
    {
      printf(" %d", $_);
    }
    printf(" */\n");
  }
}

sub dump_condencated
{
  my $cs_cnt = scalar keys %g_scc;
  if ( !$cs_cnt ) {
    dump_graph();
    return;
  }
  # dump cs
  dump_cs();
  # dump links for non-cs vertices
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( !exists $G{$i} );
    my $v = $G{$i};
    next if ( !defined($v->[1]) or defined($v->[3]) );
    my $out = $v->[1];
    my %cache;
    foreach my $e ( keys %$out )
    {
      my $to = $G{$e};
      if ( defined $to->[3] ) {
        next if ( exists $cache{$to->[3]->[0]} );
        printf("v%d -- v%d;\n", $i, $to->[3]->[0]);
        $cache{$to->[3]->[0]} = 1;
      } else {
        printf("v%d -- v%d;\n", $i, $e);
      }
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

sub dump_dir_cond
{
  my $cs_cnt = scalar keys %g_scc;
  if ( !$cs_cnt ) {
    dump_dir();
    return;
  }
  # dump cs
  dump_cs();
  # dump edges
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( !exists $G{$i} );
    my $v = $G{$i};
    next if ( !defined $v->[1]);
    my $out = $v->[1];
    my %vcache;
    foreach my $e ( keys %$out )
    {
      my $tov = $G{$e};
      # we have 4 cases here
      if ( defined $v->[3] )
      {
        if ( defined $tov->[3] )
        {
          next if ( $v->[3] == $tov->[3] ); # this is edge inside the same SCC
          printf("v%d -> v%d;\n", $v->[3]->[0], $tov->[3]->[0]);
        } else {
          printf("v%d -> v%d;\n", $v->[3]->[0], $e);
        }
      } else {
        if ( defined $tov->[3] )
        {
          next if ( exists $vcache{$tov->[3]->[0]} );
          printf("v%d -> v%d;\n", $i, $tov->[3]->[0]);
          $vcache{$tov->[3]->[0]} = 1;
        } else {
          printf("v%d -> v%d;\n", $i, $e);
        }
      }
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

# find connectivity component for vertex id and remove all remained vertices
sub find_cs
{
  my $id = shift;
  my @vis = (0) x (1 + $g_N);
  my $dfs = sub {
    my($vid, $dfs) = @_;
    return if ( $vis[$vid] );
    return if ( !exists $G{$vid} );
    $vis[$vid] = 1;
    my $v = $G{$vid};
    if ( defined $v->[1] )
    {
      foreach ( keys %{$v->[1]} )
      {
        $dfs->($_, $dfs) if ( !$vis[$_] );
      }
    }
    if ( defined $v->[2] )
    {
      foreach ( keys %{$v->[2]} )
      {
        $dfs->($_, $dfs) if ( !$vis[$_] );
      }
    }
  };
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( $vis[$i] );
    $dfs->($i, $dfs);
  }
  # remove all non-visited
  foreach my $i ( 1 .. 1+$g_N )
  {
    delete $G{$i} if ( !$vis[$i] );
  }
}

# cut-points, based on https://e-maxx.ru/algo/cutpoints
sub find_cutpoints
{
  my $res = shift;
  my @vis = (0) x (1 + $g_N);
  my @tin = (0) x (1 + $g_N);
  my @fup = (0) x (1 + $g_N);
  my $timer = 0;
  my $dfs = sub {
    my($vid, $p, $dfs) = @_;
    return if ( !exists $G{$vid} );
    $vis[$vid] = 1;
    $tin[$vid] = $fup[$vid] = $timer++;
    my $v = $G{$vid};
    my $children = 0;
    return if ( !defined $v->[1] );
    foreach ( keys %{$v->[1]} )
    {
      next if ( $_ == $vid || $_ == $p );
      if ( $vis[$_] )
      {
        $fup[$vid] = ($fup[$vid] < $tin[$_]) ? $fup[$vid] : $tin[$_];
      } else {
        $dfs->($_, $vid, $dfs);
        $fup[$vid] = ($fup[$vid] < $fup[$_]) ? $fup[$vid] : $fup[$_];
        $res->{$vid} = 1 if ( $fup[$_] >= $tin[$vid] && $p );
        $children++;
      }
    }
    $res->{$vid} = 1 if ( !$p && $children > 1 );
  };
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( $vis[$i] || !exists($G{$i}) );
    $dfs->($i, 0, $dfs);
  }
}

# remove all scc with only node bcs they are useless
sub del_odd_sccs
{
  my @rem;
  while (my ($key, $value) = each (%g_scc) )
  {
    if ( 1 == scalar keys %{ $value->[1] } ) {
      push @rem, $key;
      my $v = $G{ $value->[0] };
      $v->[3] = undef;
    }
  }
  delete $g_scc{$_} foreach ( @rem );
}

# condensate directed graph
# based on Kosaraju algo: https://e-maxx.ru/algo/strong_connected_components
sub cond_dir
{
  my @vis = (0) x (1 + $g_N);
  my @stack;
  my $dfs = sub {
    my($vid, $dfs) = @_;
    return if ( !exists $G{$vid} );
    $vis[$vid] = 1;
    my $v = $G{$vid};
    # process all neighbors via out-edges
    if ( defined $v->[1] )
    {
      foreach ( keys %{$v->[1]} )
      {
        next if ( $vis[$_] || !exists($G{$_}) );
        $dfs->($_, $dfs);
      }
    }
    push @stack, $vid;
  };
  # make vertices order in stack
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( $vis[$i] || !exists($G{$i}) );
    $dfs->($i, $dfs);
  }
  # collect SCC
  @vis = (0) x (1 + $g_N);
  my $sc_idx = 0;
  my $dfs2 = sub {
    my($vid, $dfs) = @_;
    $vis[$vid] = 1;
    my $v = $G{$vid};
    $v->[3] = add2scc($sc_idx, $vid);
    return if ( !defined $v->[2] );
    foreach ( keys %{$v->[2]} )
    {
      next if ( $vis[$_] || !exists($G{$_}) );
      $dfs->($_, $dfs);
    }
  };
  while( @stack )
  {
    my $v = pop @stack;
    last if ( !defined $v );
    next if ( $vis[$v] || !exists($G{$v}) );
    $dfs2->($v, $dfs2 );
    $sc_idx++;
  }
  del_odd_sccs();
}

# condensate undirected graph
# first find cut-points
# then run waves for remained vertices ignoring cut-points
sub cond_undir
{
  my %cp;
  find_cutpoints(\%cp);
  my @vis = (0) x (1 + $g_N);
  my $cc_idx = 0;
  my $dfs = sub {
    my($vid, $dfs) = @_;
    return if ( !exists $G{$vid} );
    # add currently processed vertex
    $vis[$vid] = 1;
    my $v = $G{$vid};
    $v->[3] = add2scc($cc_idx, $vid);
    # process all neighbors
    return if ( !defined $v->[1] );
    foreach ( keys %{$v->[1]} )
    {
      next if ( $vis[$_] || exists($cp{$_}) || !exists($G{$_}) );
      $dfs->($_, $dfs);  
    }
  };
  foreach my $i ( 1 .. 1+$g_N )
  {
    next if ( $vis[$i] || exists($cp{$i}) || !exists($G{$i}) );
    $dfs->($i, $dfs);
    $cc_idx++;
  }
  del_odd_sccs();
}

sub dump_cp
{
  my %cp;
  find_cutpoints(\%cp);
  foreach my $i ( keys %cp )
  {
    printf(" v%d [color = cyan];\n", $i);
  }
}

# main
my $status = getopts("dckCV:");
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
if ( defined $opt_V )
{
  my $vid = int($opt_V);
  die("cannot find vertex $opt_V") if ( !exists $G{$vid});
  find_cs($vid);
}
if ( defined $opt_k )
{
  if ( !defined $opt_d ) {
    cond_undir();
  } else {
    cond_dir();
  }
}
# dump results
dump_head();
if ( defined $opt_d )
{
  dump_st();
  if ( defined $opt_k ) {
    dump_dir_cond();
  } else {
    dump_dir();
  }
} else {
  dump_cp() if ( defined $opt_C );
  if ( defined $opt_k ) {
    dump_condencated();
  } else {
    dump_graph();
  }
}
printf("}");