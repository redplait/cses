#include <iostream>
#include <vector>
#include <map>

// solution for Finding Borders
// 0.43s - not perfect

using namespace std;

void calc_border(string &s)
{
   int n = (int)s.length();
   vector<int> border(n);
   map<int, int> lens;
   for (int i = 1; i < n; i++)
   {
     int j = border[i-1];
     while (j > 0 && s[i] != s[j])
        j = border[j-1];
     if (s[i] == s[j]) j++;
     border[i] = j;
   }
   int j = border[n-1];
   while(j>0)
   {
     lens[j]++;
     j = border[j-1];
   }
  // dump
  for ( auto &li: lens )
  {
    printf("%d ", li.first);
    if ( li.second > 1 )
     for ( int i = 1; i < li.second; ++i ) printf("%d ", li.first);
  }
  printf("\n");
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
    string s;
    cin >> s;
    calc_border(s);
}