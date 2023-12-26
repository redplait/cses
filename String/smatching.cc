#include <iostream>
#include <vector>
#include <map>

// solution for String Matching
// 0.03s

using namespace std;

void calc_border(string &s, string &p)
{
   int p_size = (int)p.size();
   s = p + '|' + s;
   int n = (int)s.length();
   vector<int> border(n);
   // calc prefixes on concatenated string from pattern & original string
   for (int i = 1; i < n; i++)
   {
     int j = border[i-1];
     while (j > 0 && s[i] != s[j])
        j = border[j-1];
     if (s[i] == s[j]) j++;
     border[i] = j;
   }
   int res = 0;
   for ( int i = 0; i < n; i++ )
     if ( border[i] == p_size ) res++;
   printf("%d\n", res);
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
    string s, p;
    cin>>s>>p;
    calc_border(s, p);
}