#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

// solution for Monsters
// we need to run 2 waves - one for hero and another for all monsters
using namespace std;
typedef pair<int, int> point;

struct lab
{
  queue<point> wave[4];
  queue<point> *curr, *next, *mcurr, *mnext;
  // wall -1. A 1, first value for monsters  n * m + 1
  vector<vector<pair<int, int> > > data;
  // height & width
  int N, M, curr_line = 0;
  int color = 1, mcolor;
  // answer
  vector<char> res;
  lab(int n, int m): data(n), N(n), M(m)
  {
    mcolor = N * M + 1;
    curr = &wave[0]; next = &wave[1];
    mcurr = &wave[2]; mnext = &wave[3];
    for ( int i = 0; i < N; i++ )
      data[i].resize(M);
  }
  void process_line(string &s)
  {
    auto &row = data[curr_line++];
    for ( int i = 0; i < M; i++ )
    {
      switch(s[i])
      {
        case '#': row[i].first = -1; break;
        case 'A':
          // check if our hero already at border
          if ( !i || !(curr_line - 1) || i == M - 1 || curr_line - 1 == N - 1 ) saved();
          row[i].first = color++; curr->push( { curr_line - 1, i} ); 
          break;
        case 'M': row[i].first = row[i].second = mcolor;
         // put monsters in mcurr queue
         mcurr->push( { curr_line - 1, i } );
         break;
      }
    }
  }
  void saved()
  {
    puts("YES\n0");
    exit(0);
  }
  void recover(point p)
  {
    puts("YES");
    int cc = data[p.first][p.second].first;
    while( cc != 1 )
    {
      // left
      if ( p.second > 0 && cc - 1 == data[p.first][p.second-1].first )
      {
        res.push_back('R');
        p = { p.first, p.second - 1 };
        --cc;
        continue;
      }
      // right
      if ( p.second < M - 1 && cc - 1 == data[p.first][p.second+1].first )
      {
        res.push_back('L');
        p = { p.first, p.second + 1 };
        --cc;
        continue;
      }
      // down
      if ( p.first < N -1 && cc - 1 == data[p.first+1][p.second].first )
      {
        res.push_back('U');
        p = { p.first + 1, p.second };
        --cc;
        continue;
      }
      // up
      if ( p.first > 0 && cc - 1 == data[p.first-1][p.second].first )
      {
        res.push_back('D');
        p = { p.first - 1, p.second };
        --cc;
        continue;
      }
      printf("BUG: %d %d color %d\n", p.first, p.second, cc);
      exit(3);
    }
    reverse(res.begin(), res.end());
    printf("%ld\n", res.size());
    for ( auto c: res ) printf("%c", c);
    printf("\n");
    exit(0);
  }
  // wave for monsters
  void next_mstep()
  {
    while( !mcurr->empty() )
    {
      auto p = mcurr->front(); mcurr->pop();
      // left
      if ( p.second > 0 && data[p.first][p.second-1].second == 0 && -1 != data[p.first][p.second-1].first )
      {
        data[p.first][p.second-1].second = mcolor;
        mnext->push( { p.first, p.second - 1 });
      }
      // right
      if ( p.second < M - 1 && data[p.first][p.second+1].second == 0 && -1 != data[p.first][p.second+1].first )
      {
        data[p.first][p.second+1].second = mcolor;
        mnext->push( { p.first, p.second + 1 });
      }
      // up
      if ( p.first > 0 && data[p.first-1][p.second].second == 0 && -1 != data[p.first-1][p.second].first )
      {
        data[p.first-1][p.second].second = mcolor;
        mnext->push( { p.first - 1, p.second });
      }
      // down
      if ( p.first < N - 1 && data[p.first+1][p.second].second == 0 && -1 != data[p.first+1][p.second].first )
      {
        data[p.first+1][p.second].second = mcolor;
        mnext->push( { p.first + 1, p.second });
      }
    }
    ++mcolor;
    swap(mcurr, mnext);
  }
  // 0 if should continue, 1 if path found, 2 if must stop
  int next_step()
  {
    while( !curr->empty() )
    {
      auto p = curr->front(); curr->pop();
      // left
      if ( p.second > 0 && data[p.first][p.second-1].first == 0 && 0 == data[p.first][p.second-1].second )
      {
        if ( !(p.second-1) ) { res.push_back('L'); recover(p); return 1; }
        data[p.first][p.second-1].first = color; next->push( {p.first, p.second - 1});
      }
      // right
      if ( p.second < M - 1 && data[p.first][p.second+1].first == 0 && 0 == data[p.first][p.second+1].second )
      {
        if ( p.second + 1 == M - 1 ) { res.push_back('R'); recover(p); return 1; }
        data[p.first][p.second+1].first = color; next->push( {p.first, p.second + 1});
      }
      // up
      if ( p.first > 0 && data[p.first-1][p.second].first == 0 && 0 == data[p.first-1][p.second].second )
      {
        if ( !(p.first-1) ) { res.push_back('U'); recover(p); return 1; }
        data[p.first-1][p.second].first = color; next->push( {p.first-1, p.second});
      }
      // down
      if ( p.first < N - 1 && data[p.first+1][p.second].first == 0 && 0 == data[p.first+1][p.second].second )
      {
        if ( p.first+1 == N - 1 ) { res.push_back('D'); recover(p); return 1; }
        data[p.first+1][p.second].first = color; next->push( {p.first+1, p.second});
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
  do
  {
    l.next_mstep();
  } while( !l.next_step() );
}
