#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

/* solution for Filling Trominos
   really hard task: https://cses.fi/problemset/task/2423
Observation 1: square of rectangle must be divisible by 3
Observation 2: you can fill rectangle with 1 size not divisible by 2 AND not divisible by 3
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
  char kind; // 0 - 2 x 3, 1 - 3 x 2, 2 - 9 x 5, 3 - 5 x 9
  char full_bottom = 0;
};

// 5 colors, any adjacent on borders 0..3 bcs all color-4 are inside
const char d95[9][5] = {
/* 1 */ {0,0,3,3,2},
/* 2 */ {0,4,3,2,2},
/* 3 */ {1,4,4,0,0},
/* 4 */ {1,1,3,3,0},
/* 5 */ {2,2,3,1,1},
/* 6 */ {0,2,4,4,1},
/* 7 */ {0,0,4,2,2},
/* 8 */ {3,3,2,1,2},
/* 9 */ {3,2,2,1,1},
};

struct fill_tr
{
  vector<string> res;
  int rows;
  int cols;
  plan *has_95 = nullptr;
  vector<plan> trom, right; // trom is bottom if has_95
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
    if ( rows >= 9 && !(rows % 9) && cols >= 5 && !(cols % 5) )
    {
      trom.push_back( plan{ cols / 5, rows / 9, 2 } );
      return 1;
    }
    // 5 x 9
    if ( rows >= 5 && !(rows % 5) && cols >= 9 && !(cols % 9) )
    {
      trom.push_back( plan{ cols / 9, rows / 5, 3 } );
      return 1;
    }
    // we have 4 possibility here:
    // row - 9 col - 5 and two combinations of bottom & right parts
    // row - 5 col - 9 and two combinations of bottom & right parts
    // bottom will be stored in trom and 9x5 in field has_95
    vector<plan> tb, tr;
    int r, c;
    if ( rows >= 9 && cols >= 5 )
    {
      r = rows - 9;
      c = cols - 5;
      if ( !r && check2x3(rows, c, tr) )
      { // test 9 7
#ifdef DEBUG
 printf("I r %d c %d tr %ld\n", r, c, tr.size());
#endif
        has_95 = new plan{ 1, 1, 2 };
        right = move(tr);
        return 1;
      }
      if ( !c && check2x3(r, cols, tb) )
      { // test 15 5
#ifdef DEBUG
 printf("II r %d c %d tb %ld\n", r, c, tb.size());
#endif
        has_95 = new plan{ 1, 1, 2, 1 };
        trom = move(tb);
        return 1;
      }
#define TRR trom = move(tb); right = move(tr); return 1;
      // check both rows & cols. There are 2 possibilities:
      // 1) bottom part till right and right with 9 rows
      // 2) right part till bottom and bottom only with 5 columns
      if ( check2x3(r, cols, tb) && check2x3(9, c, tr) )
      { // test 21 39
#ifdef DEBUG
 printf("II full bottom, r %d c %d tb %ld, tr %ld\n", rows, cols, tb.size(), tr.size());
#endif
        has_95 = new plan{ 1, 1, 2, 1 }; TRR
      }
#define TC tb.clear(); tr.clear();
      TC
      if ( check2x3(rows, c, tb) && check2x3(r, 5, tr) )
      {
#ifdef DEBUG
 printf("II full right, r %d c %d tb %ld, tr %ld\n", rows, cols, tb.size(), tr.size());
#endif
        has_95 = new plan{ 1, 1, 2, 0 }; TRR
      }
    }
    TC
    if ( rows >= 5 && cols >= 9 )
    {
      r = rows - 5;
      c = cols - 9;
      if ( !r && check2x3(rows, c, tr) )
      { // test 5 15
#ifdef DEBUG
 printf("III r %d c %d tr %ld\n", r, c, tr.size());
#endif
        has_95 = new plan{ 1, 1, 3 };
        right = move(tr);
        return 1;
      }
      if ( !c && check2x3(r, cols, tb) )
      { // test 7 9
#ifdef DEBUG
 printf("IV r %d c %d tb %ld\n", r, c, tb.size());
#endif
        has_95 = new plan{ 1, 1, 3, 1 };
        trom = move(tb);
        return 1;
      }
      // check both rows & cols. There are 2 possibilities:
      // 1) bottom part till right and right with 5 rows
      // 2) right part till bottom and bottom only with 9 columns
      if ( check2x3(r, cols, tb) && check2x3(5, c, tr) )
      {
#ifdef DEBUG
 printf("IV full bottom, r %d c %d tb %ld, tr %ld\n", rows, cols, tb.size(), tr.size());
#endif
        has_95 = new plan{ 1, 1, 3, 1 }; TRR
      }
      TC
      if ( check2x3(rows, c, tb) && check2x3(r, 9, tr) )
      {
#ifdef DEBUG
 printf("IV full right, r %d c %d tb %ld, tr %ld\n", rows, cols, tb.size(), tr.size());
#endif
        has_95 = new plan{ 1, 1, 3, 0 }; TRR
      }
    }
#ifdef DEBUG
 printf("not possible after all checks\n");
#endif
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
  void draw_single(int &y, int &x, int &color, plan &p)
  {
    int old_x = x;
    if ( p.kind == 2 || p.kind == 3 )
    {
      for ( int i = 0; i < p.rep_down; i++ )
      {
        x = old_x;
        for ( int j = 0; j < p.rep_right; ++j, x += (p.kind == 2) ? 5 : 9 )
          if ( p.kind == 2 ) draw9x5(y, x);
          else draw5x9(y, x);
        y += (p.kind == 2) ? 9 : 5;
      }
    } else {
      while( y < rows )
      {
        for ( x = old_x; x < cols; x += p.kind ? 2 : 3 )
        {
          if ( p.kind ) draw3x2(y, x, color);
          else draw2x3(y, x, color);
          color ^= 1;
        }
        y += p.kind ? 3 : 2;
        color ^= 2;
      }
    }
  }
  void draw_list(int &y, int &x, int &color, vector<plan> &t)
  {
    if ( 1 == t.size() ) draw_single(y,x,color,*t.begin());
    else {
      int cdiff = 0, old_color = color;
      int old_y = y, old_x = x;
      plan &p = *t.begin();
      if ( p.rep_right == 1 )
      {
#define DRAW(b)  if ( p.kind ) draw3x2(y, b, color); \
          else draw2x3(y, b, color); \
          cdiff ^= 1; color = old_color + cdiff;
        // draw 1 column
        for ( y = old_y; y < rows; y += p.kind ? 3 : 2 )
        { DRAW(x) }
        x += p.kind ? 2 : 3;
        // draw remaining columns
        cdiff ^= 2; old_color += 2; color = old_color + cdiff;
        p = t[1];
        for ( y = old_y; y < rows; y += p.kind ? 3 : 2 )
        {
          for ( int i = x; i < cols; i += p.kind ? 2 : 3 )
          { DRAW(i) }
          cdiff ^= 2; color = old_color + cdiff;
        }
      } else {
        // draw 1 row
        for ( x = old_x; x < cols; x += p.kind ? 2 : 3 )
        { DRAW(x) }
        y += p.kind ? 3 : 2;
        // draw remaining rows
        cdiff ^= 2; old_color += 2; color = old_color + cdiff;
        p = t[1];
        for ( ; y < rows; y += p.kind ? 3 : 2 )
        {
          for ( x = old_x; x < cols; x += p.kind ? 2 : 3 )
          { DRAW(x) }
          cdiff ^= 2; color = old_color + cdiff;
        }
      }
    }
  }
  void draw()
  {
    // make empty res
    res.resize(rows);
    for ( int i = 0; i < rows; i++ ) res[i].resize(cols, ' ');
    int y = 0, x = 0, color = 0;
    if ( !has_95 )
      draw_list(y, x, color, trom);
    else {
      draw_single(y, x, color, *has_95);
      color = 2; // bcs rect 9x5 can be colored with 4 colors
#ifdef DEBUG
 printf("r %d c %d bottom %ld right %ld\n", rows, cols, trom.size(), right.size()); fflush(stdout);
#endif
      if ( !trom.empty() && !right.empty() )
      {
#ifdef DEBUG
 printf("r %d c %d kind %d full_bottom %d\n", rows, cols, has_95->kind, has_95->full_bottom);
#endif
        int old_x = x, old_y = y, old_r = rows, old_c = cols;
        // draw bottom part
        x = 0;
        // if not full bottom - draw only old_x columns
        if ( !has_95->full_bottom ) cols = old_x;
        draw_list(y, x, color, trom);
        cols = old_c;
        // draw right part, we used in bottom max 4 color pairs
        color = 6;
        y = 0;
        // if full bottom - draw only old_y rows
        if ( has_95->full_bottom ) rows = old_y;
        draw_list(y, old_x, color, right);
        rows = old_r;
      } else if ( !trom.empty() )
      {
        color = 4;
        if ( has_95->full_bottom ) x = 0;
        draw_list(y, x, color, trom);
      } else if ( !right.empty() )
      {
        color = 4;
        y = 0;
        draw_list(y, x, color, right);
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