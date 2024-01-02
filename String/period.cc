#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// solution for Finding Periods
// from https://stackoverflow.com/a/71536496

void z_algorithm(const string& s)
{
    int n = int(s.size());
    if (n == 0) return;
    vector<int> z(n,0);
    int x = 0, y = 0;
    for (int i = 1; i < n; i++)
    {
        z[i] = max(0,min(z[i-x], y-i+1));
        while (i+z[i] < n && s[z[i]] == s[i+z[i]])
        {
          x = i;
          y = i + z[i];
          z[i]++;
        }
        if ( i + z[i] == n ) printf("%d ", i);
    }
    printf("%d\n", n);
}

int main()
{
  ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
    string s;
    cin >> s;
  z_algorithm(s);
}
