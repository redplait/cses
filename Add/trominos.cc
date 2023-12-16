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
also sample of 9 x 9: https://math.stackexchange.com/questions/2048438/tiling-rectangle-using-triominos
*/

using namespace std;

struct plan
{
  int rep_right;
  int rep_down;
  int kind; // 0 - 2 x 3, 1 - 3 x 2, 2 - 9 x 5, 3 - 5 x 9
};

// 5 colors, any adjacent on borders 0..4
const char d95[9][5] = {
/* 1 */ {0,0,4,4,2},
/* 2 */ {0,3,4,2,2},
/* 3 */ {1,3,3,0,0},
/* 4 */ {1,1,4,4,0},
/* 5 */ {3,3,4,2,2},
/* 6 */ {0,3,1,1,2},
/* 7 */ {0,0,1,4,4},
/* 8 */ {1,1,2,0,4},
/* 9 */ {1,2,2,0,0},
};

struct fill_tr
{
  vector<string> res;
  int rows;
  int cols;
  vector<plan> trom;
  fill_tr(int n, int m): rows(n), cols(m)
  {}
  int check2x3(int r, int c, vector<plan> &t)
  {
    // simple cases - rows is even and cols divisible by 3
    if ( !(r & 1) && !(c % 3) )
    {
      t.push_back( plan{ c / 3, r >> 1, 0 } );
      return 1;
    }
    // cols is even and rows divisible by 3
    if ( !(c & 1) && !(r % 3) )
    {
      t.push_back( plan{ c >> 1, r / 3, 1 } );
      return 1;
    }
    // if one side is divisible by 6 then other - 2 must be also divisible by 3
    if ( !(r % 6) && !((c - 2) % 3) )
    {
      t.push_back( plan{ 1, r / 3, 0 } );
      t.push_back( plan{ (c - 2) / 3, r >> 1, 1 } );
      return 1;
    }
    if ( !(c % 6) && !((r - 2) % 3) )
    {
      t.push_back( plan{ c / 3, 1, 0 } );
      t.push_back( plan{ c >> 1, (r - 2) / 3, 1 } );
      return 1;
    }
    // if one side is divisible by 6 then other - 3 must be even
    if ( !(c % 6) && r > 3 && !((r - 3) & 1) )
    {
      t.push_back( plan{ c >> 1, 1, 1 } );
      t.push_back( plan{ c / 3, (r - 3) >> 1, 0 } );
      return 1;
    }
    if ( !(r % 6) && c > 3 && !((c - 3) & 1) )
    {
      t.push_back( plan{ 1, r >> 1, 0 } );
      t.push_back( plan{ (c - 3) >> 1, r / 3, 1 } );
      return 1;
    }
    return 0;
  }
  int possible()
  {
    int64_t square = rows * cols;
    if ( 0 != (square % 3) ) return 0;
    if ( check2x3(rows, cols, trom) ) return 1;
    // https://topologicalmusings.wordpress.com/2008/07/03/solution-to-pow-6-tiling-with-triominoes/
    if ( square < 45 ) return 0;
    // 9 x 5
    if ( !(rows % 9) && !(cols % 5) )
    {
      trom.push_back( plan{ cols / 5, rows / 9, 2 } );
      return 1;
    }
    // 5 x 9
    if ( !(rows % 5) && !(cols % 9) )
    {
      trom.push_back( plan{ cols / 9, rows / 5, 3 } );
      return 1;
    }
    return 0;
  }
  void draw9x5(int y, int x)
  {
    for ( int i = 0; i < 9; i++ )
     for ( int j = 0; j < 5; j++ )
       res[y+i][x+j] = 'A' + d95[i][j];
  }
  void draw5x9(int y, int x)
  {
    for ( int i = 0; i < 5; i++ )
     for ( int j = 0; j < 9; j++ )
       res[y+i][x+j] = 'A' + d95[j][i];
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
      if ( p.kind == 2 || p.kind == 3 )
      {
        while( y < rows )
        {
          for ( x = 0; x < cols; x += (p.kind == 2) ? 5 : 9 )
            if ( p.kind == 2 ) draw9x5(y, x);
            else draw5x9(y, x);
          y += (p.kind == 2) ? 9 : 5;
        }
      } else
      while( y < rows )
      {
        for ( x = 0; x < cols; x += p.kind ? 2 : 3 )
        {
          if ( p.kind ) draw3x2(y, x, color);
          else draw2x3(y, x, color);
          color ^= 1;
        }
        y += p.kind ? 3 : 2;
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