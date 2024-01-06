#include <bits/stdc++.h>
using namespace std;

// solution for Fixed-Length Paths II
// ripped from https://usaco.guide/problems/cses-2081-fixed-length-paths-ii/solution
// 0.31s

using ll = long long;
using db = long double;  // or double, if TL is tight

using pi = pair<int, int>;
using pl = pair<ll, ll>;
using pd = pair<db, db>;

using vi = vector<int>;
using vb = vector<bool>;
using vl = vector<ll>;
using vd = vector<db>;
using vpi = vector<pi>;
using vpl = vector<pl>;
using vpd = vector<pd>;

template <class T> using V = vector<T>;
template <class T, size_t SZ > using AR = array<T, SZ>;
template <class T> using PR = pair<T, T>;

// pairs
#define mp make_pair
#define f first
#define s second

// vectors
// oops size(x), rbegin(x), rend(x) need C++17
#define sz(x) int((x).size())
#define bg(x) begin(x)
#define all(x) bg(x), end(x)
#define rall(x) x.rbegin(), x.rend()
#define sor(x) sort(all(x))
#define rsz resize
#define ins insert
#define ft front()
#define bk back()
#define pb push_back
#define eb emplace_back
#define pf push_front
#define rtn return

#define lb lower_bound
#define ub upper_bound
template <class T> int lwb(V<T> &a, const T &b) { return int(lb(all(a), b) - bg(a)); }

// loops
#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define F0R(i, a) FOR(i, 0, a)
#define ROF(i, a, b) for (int i = (b)-1; i >= (a); --i)
#define R0F(i, a) ROF(i, 0, a)
#define EACH(a, x) for (auto &a : x)

const int MX = 2e5 + 1;
const ll INF = 1e18;  // not too close to LLONG_MAX

// bitwise ops
// also see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
constexpr int pct(int x) { return __builtin_popcount(x); }  // # of bits set
constexpr int bits(int x) {  // assert(x >= 0); // make C++11 compatible until
                             // USACO updates ...
    return x == 0 ? 0 : 31 - __builtin_clz(x);
}  // floor(log2(x))
constexpr int p2(int x) { return 1 << x; }
constexpr int msk2(int x) { return p2(x) - 1; }

ll cdiv(ll a, ll b) {
    return a / b + ((a ^ b) > 0 && a % b);
}  // divide a by b rounded up
ll fdiv(ll a, ll b) {
    return a / b - ((a ^ b) < 0 && a % b);
}  // divide a by b rounded down

template <class T> bool ckmin(T &a, const T &b) {
    return b < a ? a = b, 1 : 0;
}  // set a = min(a,b)

template <class T> bool ckmax(T &a, const T &b) { return a < b ? a = b, 1 : 0; }

template <class T, class U> T fstTrue(T lo, T hi, U f) {
    hi++;
    assert(lo <= hi);  // assuming f is increasing
    while (lo < hi) {  // find first index such that f is true
	T mid = lo + (hi - lo) / 2;
	f(mid) ? hi = mid : lo = mid + 1;
    }
    return lo;
}
template <class T, class U> T lstTrue(T lo, T hi, U f) {
    lo--;
    assert(lo <= hi);  // assuming f is decreasing
    while (lo < hi) {  // find first index such that f is true
	T mid = lo + (hi - lo + 1) / 2;
	f(mid) ? lo = mid : hi = mid - 1;
    }
    return lo;
}
template <class T> void remDup(vector<T> &v) {  // sort and remove duplicates
    sort(all(v));
    v.erase(unique(all(v)), end(v));
}
template <class T, class U> void erase(T &t, const U &u) {  // don't erase
    auto it = t.find(u);
    assert(it != end(t));
    t.erase(it);
}  // element that doesn't exist from (multi)set

void setPrec() { cout << fixed << setprecision(15); }
void unsyncIO() { cin.tie(0)->sync_with_stdio(0); }

int max_len;
ll mul = 1;
ll ans;
int N, K1, K2;
vi adj[MX];

int get_prefix(const deque<int> &a, int mx) {
    if (mx < 0) return 0;
    if (mx + 1 >= sz(a)) return a[0];
    return a[0] - a[mx + 1];
}

void comb(deque<int> &a, deque<int> &b) {
    if (sz(a) < sz(b)) swap(a, b);
    F0R(i, sz(b) - 1) b[i] -= b[i + 1];
    F0R(i, sz(b))
    ans += (ll)b[i] * (get_prefix(a, K2 - i) - get_prefix(a, K1 - 1 - i));
    R0F(i, sz(b) - 1) b[i] += b[i + 1];
    F0R(i, sz(b)) a[i] += b[i];
}

deque<int> dfs(int x, int p) {  // each deque stores prefix sums
    deque<int> res{1};
    EACH(y, adj[x]) if (y != p) {
	deque<int> a = dfs(y, x);
	a.push_front(a.ft);
	comb(res, a);
    }
    return res;
}

int main() {
    ios_base::sync_with_stdio(0); cin.tie(0);cout.tie(0);
    cin>>N>>K1>>K2;
    F0R(i, N - 1) {
	int a, b;
	cin>>a>>b;
        a--, b--;
	adj[a].pb(b), adj[b].pb(a);
    }
    max_len = K1 - 1, mul = -1;
    dfs(0, -1);
    cout << ans;
}
