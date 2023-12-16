#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

/* solution for Filling Trominos
   really hard task: https://cses.fi/problemset/task/2423
Observation 1: square of rectangle must be divisible by 3
Uvservation 2: you can fill rectangle with 1 size not divisible by 2 AND not divisible by 3
Examples:
block 6x5   block 6x7  block 5x6

aadda       aaddadd     aabaab
abdaa       abdaada     abbabb
bbeec       bbeecaa     ccddee
ccecc       cceccee     dcdced
cdaab       cdaabed     ddccdd
ddabb       ddabbdd

Hovewer square of rectangle must not be divisible by 6, see https://nrich.maths.org/7026
so in test 3 we have strange rectangle 9 x 5
*/

using namespace std;

struct plan
{
  int rep_right;
  int rep_down;
  bool kind; // false - 2 x 3, else 3 x 2
};

struct fill_tr
{
  vector<string> res;
  int rows;
  int cols;
  vector<plan> trom;
  fill_tr(int n, int m): rows(n), cols(m)
  {}
  int possible()
  {
    int64_t square = rows * cols;
    if ( 0 != (square % 6) ) return 0;
    // simple cases - rows is even and cols divisible by 3
    if ( !(rows & 1) && !(cols % 3) )
    {
      trom.push_back( plan{ cols / 3, rows >> 1, false } );
      return 1;
    }
    // cols is even and rows divisible by 3
    if ( !(cols & 1) && !(rows % 3) )
    {
      trom.push_back( plan{ cols >> 1, rows / 3, true } );
      return 1;
    }
    // if one side is divisible by 6 then other - 2 must be also divisible by 3
    if ( !(rows % 6) && !((cols - 2) % 3) )
    {
      trom.push_back( plan{ 1, rows / 3, false } );
      trom.push_back( plan{ (cols - 2) / 3, rows >> 1, true } );
      return 1;
    }
    if ( !(cols % 6) && !((rows - 2) % 3) )
    {
      trom.push_back( plan{ cols / 3, 1, false } );
      trom.push_back( plan{ cols >> 1, (rows - 2) / 3, true } );
      return 1;
    }
    // if one side is divisible by 6 then other - 3 must be even
    if ( !(cols % 6) && rows > 3 && !((rows - 3) & 1) )
    {
      trom.push_back( plan{ cols >> 1, 1, true } );
      trom.push_back( plan{ cols / 3, (rows - 3) >> 1, false } );
      return 1;
    }
    if ( !(rows % 6) && cols > 3 && !((cols - 3) & 1) )
    {
      trom.push_back( plan{ 1, rows >> 1, false } );
      trom.push_back( plan{ (cols - 3) >> 1, rows / 3, true } );
      return 1;
    }
    return 0;
  }
  void draw2x3(int y, int x, int color)
  {
    char A = 'A' + 2 * color;
    char B = A + 1;
    res[y][x] = A;     // **
    res[y][x+1] = A;   // *
    res[y+1][x] = A;
    res[y][x+2] = B;   //  +
    res[y+1][x+1] = B; // ++
    res[y+1][x+2] = B;
  }
  void draw3x2(int y, int x, int color)
  {
    char A = 'A' + 2 * color;
    char B = A + 1;
    res[y][x] = A;     // **
    res[y+1][x] = A;   // *
    res[y][x+1] = A;
    res[y+1][x+1] = B; //  +
    res[y+2][x] = B;   // ++
    res[y+2][x+1] = B;
  }
  void draw()
  {
    // make empty res
    res.resize(rows);
    for ( int i = 0; i < rows; i++ ) res[i].resize(cols, ' ');
    int x = 0, y = 0, color = 0;
    if ( 1 == trom.size() )
    {
      plan &p = *trom.begin();
      while( y < rows )
      {
        for ( x = 0; x < cols; x += p.kind ? 2 : 3 )
        {
          if ( p.kind ) draw3x2(y, x, color);
          else draw2x3(y, x, color);
          color ^= 1;
        }
        y += p.kind ? 3 : 2;
        x = 0;
        color ^= 2;
      }
    } else {
      plan &p = *trom.begin();
      if ( p.rep_right == 1 )
      {
#define DRAW(a, b)  if ( p.kind ) draw3x2(a, b, color); \
          else draw2x3(a, b, color); \
          color ^= 1;
        // draw 1 column
        for ( y = 0; y < rows; y += p.kind ? 3 : 2 )
        { DRAW(y, x) }
        x += p.kind ? 2 : 3;
        // draw remaining columns
        color ^= 2;
        p = trom[1];
        for ( y = 0; y < rows; y += p.kind ? 3 : 2 )
        {
          for ( int i = x; i < cols; i += p.kind ? 2 : 3 )
          { DRAW(y, i) }
          color ^= 2;
        }
      } else {
        // draw 1 row
        for ( x = 0; x < cols; x += p.kind ? 2 : 3 )
        { DRAW(y, x) }
        y += p.kind ? 3 : 2;
        // draw remaining rows
        color ^= 2;
        p = trom[1];
        for ( ; y < rows; y += p.kind ? 3 : 2 )
        {
          for ( x = 0; x < cols; x += p.kind ? 2 : 3 )
          { DRAW(y, x) }
          color ^= 2;
        }
      }
    }
    // dump result
    for ( int i = 0; i < rows; i++ ) printf("%s\n", res[i].c_str());
  }
};

int main()
{
  int t, n, m;
  cin>>t;
  for ( int i = 0; i < t; i++ )
  {
    cin>>n>>m;
    fill_tr f(n, m);
    if ( f.possible() )
    {
      puts("YES");
      f.draw();
    } else
      puts("NO");
  }
}