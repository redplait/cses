#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#pragma GCC target("sse,sse2,sse3,ssse3,sse4,sse4.2,popcnt,tune=native")
#pragma GCC optimize("-Ofast")

// Forest Queries 2
// main idea is that for each item[y, x] you can precalculate total for all items with x1 <= x and y1 <= y
//  see method process_line
// then for each query with y1, x1, y2, x2 you have 4 cases
// 1) y1 == 1 and x1 == 1 - then just return value at [y2, x2]
// 2) x1 == 1 - item[y2, x2] - item[y1 - 1, x2]
// 3) y1 == 1 - item[y2, x2] - item[y2, x1 - 1]
// 4) item[y2, x2] - item[y2, x1 - 1] - item[y1 - 1, x2] + item[y1 - 1, x1 - 1]
// so complexity of lookup is O(1) and precalc: there are m rows and n columns
// for each row you visit n columns twice - first to fill current row and second to add prev row
// for m rows overall complexity is O( n * m * 2)
// Updates processing:
//  we can put number of changed cells in another m * n array
//  when change happens at point y, x we patch this array at row y from x till end of row
//  then to calc count of diffs for point y, x we should scan all rows from 0 till y and sum diffs at x
using namespace std;

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

struct sq
{
  vector<int> v;
  vector<char> old;
  vector<int> diff;
  int N;
  int curr_row = 1;
  sq(int n):
   v(n * n), old(n * n), diff(n * n), N(n)
  {
    fill(v.begin(), v.end(), 0);
    fill(diff.begin(), diff.end(), 0);
  }
  // return action bases on old value
  // if it was '*' then -1 - need to deacrease square at right & below, else 1 - need increase
  inline char toggle(int y, int x)
  {
    auto s = &old[(y-1) * N];
    if ( s[x-1] == '*' ) { s[x-1] = '.'; return -1; }
    else { s[x-1] = '*'; return 1; }
  }
  inline int *row_diff(int y)
  {
    y--; return &diff[y * N];
  }
  inline int *row(int y)
  {
    y--; return &v[y * N];
  }
  inline int get(int y, int x)
  {
    int dx = calc_diff(y, x);
    y--; x--;
    int res = v[y * N + x] + dx;
#ifdef DEBUG
if ( res != v[y * N + x]) printf("get y %d x %d dx %d old %d new %d\n", y + 1, x + 1, dx, v[y * N + x], res);
#endif
    return res;
  }
  int calc_diff(int y, int x)
  {
    int res = 0;
    for ( int r = 0; r < y; r++ )
    {
      auto row = row_diff(r + 1);
      res += row[x-1];
    }
    return res;
  }
  void update(int y, int x)
  {
    char what = toggle(y, x);
    auto row = row_diff(y);
    // update right part
    for ( int i = x; i <= N; i++ ) row[i-1] += what;
  }
  void process_line(string &s)
  {
    copy(s.begin(), s.end(), &old[(curr_row - 1) * N]);
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
  int query(int y1, int x1, int y2, int x2) {
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

int main(int argc, char **argv)
{
#ifdef TEST
  if ( argc != 2 )
  {
    printf("in test mode you need peek file in cmd line\n");
    exit(6);
  }
#endif
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
printTime("precalc");
#ifdef TEST
  ifstream ti(argv[1]);
#endif
#ifdef DEBUG
  printf("\ntotals:\n"); g.dump();
#endif
  int last_y, last_x;
  for ( int i = 0; i < q; ++i )
  {
    int what;
    cin>>what;
    if ( what == 1 )
    {
      cin>>last_y>>last_x;
      g.update(last_y,last_x);
#ifdef DEBUG
  printf("\ntotals:\n"); g.dump();
#endif
    } else {
      int y1, x1, y2, x2;
      cin>>y1>>x1>>y2>>x2;
      int res = g.query(y1, x1, y2, x2);
      printf("%d\n", res);
#ifdef TEST
   int must_be;
   ti>>must_be;
   if ( res != must_be )
   {
     printf("wrong answer %d vs %d, last update y %d x %d\n", res, must_be, last_y, last_x);
     exit(1);
   }
#endif
    }
  }
printTime("res");
}