#include <stdio.h>
#include <string.h>
#include <vector>

template <int S>
struct queens {
 std::vector<int> pos;
 int field[S][S];
 int res = 0;
 size_t cnt = 0;
 queens() {
   pos.reserve(S);
   memset(field, 0, sizeof(field));
 }
 // propagate queen fields down row, get column from last item of pos
 inline void mark(int row, int v) {
   int col = pos.back();
   int diff = 1;
   for ( int r = row + 1; r < S; ++r, ++diff )
   {
     field[r][col] += v; // column
     if ( col - diff >= 0 ) field[r][col - diff] += v; // left diag
     if ( col + diff < S )  field[r][col + diff] += v; // right diag
   }
 }
 void dump() const {
   for ( int c: pos ) printf(" %d", 1+c);
   printf("\n");
 }
 int rec(int row) {
   if ( row == S ) {
     dump(); return 1;
   }
   int res = 0;
   for ( int c = 0; c < S; ++c ) {
     if ( field[row][c] ) continue;
     cnt++;
     pos.push_back(c); mark(row, 1);
     res += rec(row + 1);
     mark(row, -1); pos.pop_back();
   }
   return res;
 }
 int run() {
   return rec(0);
 }
};

int main() {
  queens<8> q;
  printf("%d\n", q.run());
  printf("%ld cnt\n", q.cnt);
}