#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

// solution for Labyrinth
// try to run wave as soon as possible
using namespace std;
typedef pair<int, int> point;

struct lab
{
  queue<point> wave[2];
  queue<point> *curr, *next;
  // wall -1. A 1, B n * m + 1
  vector<vector<int> > data;
  // height & width
  int N, M, curr_line = 0;
  int color = 1, last;
  // answer
  vector<char> res;
  lab(int n, int m): data(n), N(n), M(m)
  {
    last = N * M + 1;
    curr = &wave[0]; next = &wave[1];
    for ( int i = 0; i < N; i++ )
      data[i].resize(M);
  }
  inline bool has_A()
  {
    return !curr->empty();
  }
  void process_line(string &s)
  {
    bool had_a = has_A();
    auto &row = data[curr_line++];
    for ( int i = 0; i < M; i++ )
    {
      switch(s[i])
      {
        case '#': row[i] = -1; break;
        case 'A': row[i] = color++; curr->push( { curr_line - 1, i} ); break;
        case 'B': row[i] = last; break;
      }
    }
    if ( had_a ) next_step();
  }
  void recover(point p)
  {
    puts("YES");
    int cc = data[p.first][p.second];
    while( cc != 1 )
    {
      // left
      if ( p.second > 0 && cc - 1 == data[p.first][p.second-1] )
      {
        res.push_back('R');
        p = { p.first, p.second - 1 };
        --cc;
        continue;
      }
      // right
      if ( p.second < M - 1 && cc - 1 == data[p.first][p.second+1] )
      {
        res.push_back('L');
        p = { p.first, p.second + 1 };
        --cc;
        continue;
      }
      // down
      if ( p.first < N -1 && cc - 1 == data[p.first+1][p.second] )
      {
        res.push_back('U');
        p = { p.first + 1, p.second };
        --cc;
        continue;
      }
      // up
      if ( p.first > 0 && cc - 1 == data[p.first-1][p.second] )
      {
        res.push_back('D');
        p = { p.first - 1, p.second };
        --cc;
        continue;
      }
      printf("BIG: %d %d color %d\n", p.first, p.second, cc);
      exit(3);
    }
    reverse(res.begin(), res.end());
    printf("%ld\n", res.size());
    for ( auto c: res ) printf("%c", c);
    printf("\n");
    exit(0);
  }
  // 0 if should continue, 1 if path found, 2 if must stop
  int next_step()
  {
    while( !curr->empty() )
    {
      auto p = curr->front(); curr->pop();
      // left
      if ( p.second > 0 && data[p.first][p.second-1] != -1 )
      {
        if ( last == data[p.first][p.second-1] ) { res.push_back('L'); recover(p); return 1; }
        else if ( !data[p.first][p.second-1] ) { data[p.first][p.second-1] = color; next->push( {p.first, p.second - 1}); }
      }
      // right
      if ( p.second < M - 1 && data[p.first][p.second+1] != -1 )
      {
        if ( last == data[p.first][p.second+1] ) { res.push_back('R'); recover(p); return 1; }
        else if ( !data[p.first][p.second+1] ) { data[p.first][p.second+1] = color; next->push( {p.first, p.second + 1}); }
      }
      // up
      if ( p.first > 0 && data[p.first-1][p.second] != -1 )
      {
        if ( last == data[p.first-1][p.second] ) { res.push_back('U'); recover(p); return 1; }
        else if ( !data[p.first-1][p.second] ) { data[p.first-1][p.second] = color; next->push( {p.first-1, p.second}); }
      }
      // down
      if ( p.first < N - 1 && data[p.first+1][p.second] != -1 )
      {
        if ( last == data[p.first+1][p.second] ) { res.push_back('D'); recover(p); return 1; }
        else if ( !data[p.first+1][p.second] ) { data[p.first+1][p.second] = color; next->push( {p.first+1, p.second}); }
      }
    }
    ++color;
    if ( next->empty() ) { puts("NO"); exit(0); return 2; }
    swap(curr, next);
    return 0;
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, m;
  cin>>n>>m;
  lab l(n, m);
  for ( int i = 0; i < n; i++ )
  {
    string s;
    cin>>s;
    l.process_line(s);
  }
#ifdef DEBUG
  printf("curr %ld\n", l.curr->size());
#endif
  while( !l.next_step() ) ;
}
