#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <queue>
#include <algorithm> // for sort

// New Flight Routes
// simple and wrong algo - just connect all no-in nodes
// then first no-out with last no-in
// then all no-out
// and finally if we have first no-in and last no-out - connect them
using namespace std;

struct node;
struct con
{
  node *f = nullptr, *l = nullptr;
  vector<int> no_in;
  vector<int> no_out;
  bool used = false;
  inline int empty() const
  { return no_in.empty() && no_out.empty(); }
  inline int is_line() const
  { return 1 == no_in.size() && 1 == no_out.size(); }
};

struct node
{
  int n;
  con *c = nullptr;
  void asgn(con *cc)
  {
    if ( !cc ) return;
    c = cc;
    c->l = this;
    if ( !degree() ) c->no_out.push_back(n);
    if ( !in_degree() ) c->no_in.push_back(n);
  }
  unordered_set<int> edges;
  unordered_set<int> in_edges;
  inline int degree() const
  {
    return edges.size();
  }
  inline int in_degree() const
  {
    return in_edges.size();
  }
};

struct graph
{
  vector<node> nodes;
  vector<con *> cons;
  vector<pair<int, int> > add;
  int N_size;
  vector<char> visited; // for tests
  graph(int N)
   : nodes(N), N_size(N), visited(N)
  {
    for ( int i = 0; i < N; ++i )
      nodes[i].n = i;
  }
  void patch(int from, int to, const char *f)
  {
    add.push_back({from, to});
#ifdef DEBUG
    printf("patch %d -> %d at %s\n", from+1, to+1, f);
#endif
    add_edge(from+1, to+1);
  }
  con *add_con(node *n)
  {
    con *res = new con;
    cons.push_back(res);
    n->c = res;
#ifdef DEBUG
printf("add_con for %d\n", 1+n->n);
#endif
    res->l = res->f = n;
    if ( !n->in_degree() && !n->degree() ) return res;
    if ( !n->in_degree() ) res->no_in.push_back(n->n);
    if ( !n->degree() ) res->no_out.push_back(n->n);
    return res;
  }
  inline void add_edge(int a, int b)
  {
    if ( a == b )
      return;
    node &n = nodes[a-1];
    n.edges.insert(b-1);
    node &m = nodes[b-1];
    m.in_edges.insert(a-1);
  }
  void dfs(node *n, con *c)
  {
    for ( auto num: n->edges )
    {
      if ( nodes[num].c ) continue;
      nodes[num].asgn(c);
      dfs(&nodes[num],c);
    }
    for ( auto num: n->in_edges )
    {
      if ( nodes[num].c ) continue;
      nodes[num].asgn(c);
      dfs(&nodes[num],c);
    }
  }
  void test_dfs(node *n, int &res)
  {
    for ( auto num: n->edges )
    {
      if ( visited[num] ) continue;
      visited[num] = 1; res++;
      test_dfs(&nodes[num], res);
    }
  }
  int test()
  {
    int res = 1;
    visited[0] = 1;
    test_dfs(&nodes[0], res);
    if ( res == N_size ) return 0;
    set<con *> missed;
    for ( int i = 0; i < N_size; i++ )
    {
      if ( visited[i] ) continue;
      auto cs = nodes[i].c;
      auto ci = missed.find(cs);
      if ( ci != missed.end() ) continue;
      printf("not connected %p via %d\n", cs, i + 1);
      missed.insert(cs);
    }
    return (int)missed.size();
  }
  void dump_cons()
  {
    printf("conns %ld\n", cons.size());
    for ( auto c: cons )
    {
      printf("%p: f %d l %d no_in %ld, no_out %ld\n", c, c->f->n+1, c->l->n+1, c->no_in.size(), c->no_out.size());
      if ( !c->no_in.empty() )
      {
        printf(" no_in:");
        for ( auto n: c->no_in ) printf(" %d", 1+n);
        printf("\n");
      }
      if ( !c->no_out.empty() )
      {
        printf(" no_out:");
        for ( auto n: c->no_out ) printf(" %d", 1+n);
        printf("\n");
      }
    }
  }
  void conn()
  {
    for ( auto &n: nodes )
    {
      if ( n.c ) continue;
      con *c = add_con(&n);
      dfs(&n, c);
    }
  }
  void analyze()
  {
    // lets check what connection sets we have
    if ( cons.empty() ) { puts("0"); return; }
    if ( 1 == cons.size() && cons[0]->empty() )
    { puts("0"); return; }
    size_t i;
    size_t l_n0 = 0;
    for(i = 0; i<cons.size(); ++i)
    {
      if ( !cons[i]->is_line() ) continue;
      l_n0++;
      // asgn f to no_in[0] and l to no_out[0]
      cons[i]->f = &nodes[cons[i]->no_in[0]];
      cons[i]->l = &nodes[cons[i]->no_out[0]];
    }
    size_t n0 = count_if(cons.begin(), cons.end(), [](const con *c) { return c->empty(); });
    // if they all have not no-in/no-out - just connect them in ring
    if ( n0 + l_n0 == cons.size() )
    {
      printf("%ld\n", n0 + l_n0);
      for ( i = 0; i < cons.size() - 1; i++ )
        printf("%d %d\n", cons[i]->l->n + 1, cons[i+1]->f->n + 1);
      // last connect with first
      printf("%d %d\n", cons.back()->l->n + 1, cons[0]->f->n + 1);
      return;
    }
    node *rf = nullptr, *rl = nullptr;
    // connect all no-in/no-out connected comps into one big path, start store to rf, end to rl
    if ( n0 + l_n0 )
    {
#ifdef DEBUG
 printf("n0 %ld l_n0 %d\n", n0, l_n0);
#endif
     for ( i = 0; i < cons.size(); ++i )
     {
       if ( !cons[i]->empty() && !cons[i]->is_line() ) continue;
       cons[i]->used = true;
       if ( !rf ) { rf = cons[i]->f; rl = cons[i]->l; continue; }
       patch(rl->n, cons[i]->f->n, "n0");
       rl = cons[i]->l;
     }
    }
    node *i_f = nullptr, *i_l = nullptr;
    // connect all no-ins, start in i_f, end in i_l
    for ( i = 0; i < cons.size(); ++i )
    {
      if ( cons[i]->used ) continue; // they are already in res
      if ( cons[i]->no_in.empty() ) continue;
      for ( auto noin: cons[i]->no_in )
      {
        if ( !i_f ) { i_l = i_f = &nodes[noin]; continue; }
        // check if we should insert path rf..rl between i_f and this one
        if ( rf )
        {
          patch(i_f->n, rf->n, "no-ins1"); patch(rl->n, noin, "no-ins2");
          rf = nullptr;
        } else {
          patch(i_l->n, noin, "no-ins");
        }
        i_l = &nodes[noin];
      }
    }
    // connect all no-outs in reverse order, first in o_f, last in o_l
    node *o_f = nullptr, *o_l = nullptr;
    for ( int i = (int)cons.size()-1; i >= 0; --i )
    {
      if ( cons[i]->used ) continue;
      if ( cons[i]->no_out.empty() ) continue;
      for ( auto noout: cons[i]->no_out )
      {
        if ( !o_f )
        {
          // check if we should add edge to i_l
          if ( i_l && i_l->c != cons[i] )
          {
            patch(nodes[noout].n, i_l->n, "add edge to i_l");
          }
          o_f = o_l = &nodes[noout]; continue;
        }
        // check if we should insert path rf..rl between o_f and this one
        if ( rf )
        {
          patch(o_f->n, rf->n, "no-out1"); patch(rl->n, noout, "no-out2");
          rf = nullptr;
        } else {
          patch(o_l->n, noout, "no-out");
        }
        o_l = &nodes[noout];
      }
    }
#ifdef DEBUG
 printf("i_f %d o_l %d\n", i_f ? i_f->n+1 : 0, o_l ? o_l->n+1 : 0);
#endif
    // and finally check what we have remained
    if ( i_f && o_l )
    {
      if ( rf )
      {
        patch(o_l->n, rf->n, "ins rf1"); patch(rl->n, i_f->n, "int rf2");
        rf = nullptr;
      } else
        patch(o_l->n, i_f->n, "ins rf");
    } else if ( i_l ) // add edge from last connection set to i_l
    {
      int num = find_il(i_l->n);
      if ( rf )
      {
        patch(num, rf->n, "i_l rf1"); patch(rl->n, i_l->n, "i_l rf2");
        rf = nullptr;
      } else
        patch(num, i_l->n, "i_l");
    } else if ( o_f ) // add edge from o_f to last connection set
    {
      int num = find_il(o_f->n);
      if ( rf )
      {
        patch(o_f->n, rf->n, "o_f rf1"); patch(rl->n, num, "o_f rf2");
        rf = nullptr;
      } else
        patch(o_f->n, num, "o_f");
    }
#ifdef DEBUG
    test();
#endif
    // dump results
    printf("%ld\n", add.size());
    for ( auto &p: add ) printf("%d %d\n", p.first+1, p.second+1);
  }
  int find_il(int n)
  {
    con *c = nullptr;
    if ( cons.size() == 1 ) c = cons.back();
    for ( size_t i = cons.size() - 1; i >= 0; --i )
      if ( cons[i]->empty() ) continue;
       else { c = cons[i]; break; }
    // find any vertex != n
    if ( c->f->n != n ) return c->f->n;
    return c->l->n;
  }
};

inline float getTime()
{
  return (float)clock()/CLOCKS_PER_SEC;
}

inline void printTime(const char *pfx)
{
#ifdef TIME
  printf("%s: %f\n", pfx, getTime());
#endif
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  graph g(n);
  for ( int i = 0; i < m; ++i )
  {
    int a, b;
    cin>>a>>b;
    g.add_edge(a, b);
  }
printTime("start");
  g.conn();
printTime("conn");
#ifdef DEBUG
  g.dump_cons();
#endif
  g.analyze();
printTime("end");
}