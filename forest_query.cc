#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

// Forest Queries
// main idea is that for each item[y, x] you can precalculate total for all items with x1 <= x and y1 <= y
//  see method process_line
// then for each query with y1, x1, y2, x2 you have 4 cases
// 1) y1 == 1 and x1 == 1 - then just return value at [y2, x2]
// 2) x1 == 1 - item[y2, x2] - item[y1 - 1, x2]
// 3) y1 == 1 - item[y2, x2] - item[y2, x1 - 1]
// 4) item[y2, x2] - item[y2, x1 - 1] - item[y1 - 1, x2] + item[y1 - 1, x1 - 1]
// so complexity of lookup is O(1) and precalc: there are m rows and n columnw
// for each row you visit n columns twice - first to fill current row and second to add prev array
// for m rows overall complexity is O( n * m * 2)
using namespace std;

struct sq
{
  vector<int> v;
  int N;
  int curr_row = 1;
  sq(int n):
   v(n * n), N(n)
  {
    fill(v.begin(), v.end(), 0);
  }
  inline int *row(int y)
  {
    y--; return &v[y * N];
  }
  inline int& get(int y, int x)
  {
    y--; x--;
    return v[y * N + x];
  }
  void process_line(string &s)
  {
    auto item = row(curr_row);
    for ( int i = 0; i < N; i++ )
    {
       if ( s[i] == '*' ) item[i]++;
       if ( i ) item[i] += item[i-1];
    }
    if ( curr_row > 1 )
    {
      item = row(curr_row);
      auto prev = row(curr_row - 1);
      for ( int i = 0; i < N; i++ )
        item[i] += prev[i];
    }
    curr_row++;
  }
  int query(int y1, int x1, int y2, int x2)
  {
    if ( y1 == 1 && x1 == 1 ) return get(y2, x2);
    if ( x1 == 1 ) return get(y2, x2) - get(y1 - 1, x2);
    if ( y1 == 1 ) return get(y2, x2) - get(y2, x1 - 1);
    return get(y2, x2) - get(y2, x1 - 1) - get(y1 - 1, x2) + get(y1 - 1, x1 - 1);
  }
  void dump() {
    for ( int i = 1; i < N + 1; i++ )
    {
      for ( int j = 1; j < N + 1; j++ )
        printf("%d ", get(i, j));
      printf("\n");
    }
  }
};

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
  int n, q;
  cin>>n>>q;
  sq g(n);
  for ( int i = 0; i < n; ++i )
  {
    string s;
    cin>>s;
    g.process_line(s);
  }
#ifdef DEBUG
  printf("\ntotals:\n"); g.dump();
#endif
  for ( int i = 0; i < q; ++i )
  {
    int y1, x1, y2, x2;
    cin>>y1>>x1>>y2>>x2;
    printf("%d\n", g.query(y1, x1, y2, x2));
  }
}