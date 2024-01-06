#include "bits/stdc++.h"

// solution for Path Queries I
// 0.25s

#ifdef __has_builtin
#define CPT_BIT_OPS_HAS_BUILTIN(x) __has_builtin(x)
#else
#define CPT_BIT_OPS_HAS_BUILTIN(x) 0
#endif

#ifdef ENABLE_DEBUG
#define LOCAL_ASSERT(condition, message)                                       \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion (" << #condition << ") failed\non line "   \
                      << __LINE__ << " in " << __FILE__ << '#' << __FUNCTION__ \
                      << "()\n";                                               \
            std::cerr << message << '\n';                                      \
            abort();                                                           \
        }                                                                      \
    } while (0)
#else
#define LOCAL_ASSERT(...) \
    do {                  \
    } while (0)
#endif

using namespace std;
using usize = size_t;
using u32 = uint32_t;
using i64 = int64_t;

template <class T, auto v>
constexpr auto checked_cast = []() -> T {
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_integral_v<decltype(v)>);
    if constexpr (std::is_signed_v<T> && std::is_signed_v<decltype(v)>) {
        static_assert(std::numeric_limits<T>::min() <= v);
    } else if (std::is_signed_v<decltype(v)>) {
        static_assert(v >= 0);
    }
    static_assert(v <= std::numeric_limits<T>::max());
    return static_cast<T>(v);
}();

template <class T>
constexpr int type_bit_width = checked_cast<int, sizeof(T) * CHAR_BIT>;
template <class T>
constexpr auto mask_of_width(int width) -> T {
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_unsigned_v<T>);
    LOCAL_ASSERT(0 <= width && width <= type_bit_width<T>,
                 "width of mask must be in [0, type_bit_width<T>]");
    return width == type_bit_width<T> ? std::numeric_limits<T>::max()
                                      : static_cast<T>((T(1) << width) - 1);
}
template <class T>
constexpr auto clz(T x) -> int {
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_unsigned_v<T>);
    if (x == 0) return type_bit_width<T>;
#if CPT_BIT_OPS_HAS_BUILTIN(__builtin_clz)
    if constexpr (type_bit_width<T> <= 32) {
        return type_bit_width<T> - (32 - __builtin_clz(x));
    }
#endif
#if CPT_BIT_OPS_HAS_BUILTIN(__builtin_clzll)
    if constexpr (type_bit_width<T> <= 64) {
        return type_bit_width<T> - (64 - __builtin_clzll(x));
    }
#endif
    int res = 0;
    int w = type_bit_width<T> / 2;
    T mask = mask_of_width<T>(w) << w;
    for (; w != 0; w /= 2, mask <<= w) {
        if ((x & mask) == 0) {
            x <<= w;
            res += w;
        }
    }
    return res;
}

template <class T>
constexpr auto bit_width(T x) -> int {
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_unsigned_v<T>);
    return type_bit_width<T> - clz(x);
}

template <class T>
constexpr auto bit_ceil(T x) -> T {
    static_assert(std::is_integral_v<T>);
    static_assert(std::is_unsigned_v<T>);
    if (x == 0) return T(1);
    auto w = bit_width(static_cast<T>(x - 1));
    if (w == type_bit_width<T>) return T(0);
    return static_cast<T>(T(1) << w);
}

constexpr usize segtree_dynamic_extent = ~usize(0);
template <class S, usize extent = segtree_dynamic_extent>
class SegTree {
#ifdef ENABLE_DEBUG
    usize n_;
#endif
    static constexpr bool is_dynamic_extent = extent == segtree_dynamic_extent;
    static_assert(is_dynamic_extent || (extent & (extent - 1)) == 0,
                  "extent must be a power of 2");
    struct Nothing {
        template <class... Args>
        Nothing(Args &&...) {}
    };
    struct NodeMeta {
        usize len = 0;
        void pull(const NodeMeta &left, const NodeMeta &right) {
            len = left.len + right.len;
        }
    };
    std::conditional_t<is_dynamic_extent, usize, Nothing> rounded_n_rt_;
    std::unique_ptr<S[]> data_;
    constexpr auto rounded_n() const -> usize {
        if constexpr (is_dynamic_extent) {
            return rounded_n_rt_;
        } else {
            return extent;
        }
    }
    template <class T, class = void>
    struct CheckIfUsesMeta : std::false_type {};
    template <class T>
    struct CheckIfUsesMeta<
        T, std::conditional_t<false, typename T::with_meta, void>>
        : std::true_type {};
    static constexpr bool s_uses_meta = CheckIfUsesMeta<S>::value;
    void check_node([[maybe_unused]] usize i) const {
        LOCAL_ASSERT(0 < i && i < 2 * rounded_n(),
                     "invalid node index " << i << " for n = " << n_);
    }
    void check_internal_node([[maybe_unused]] usize i) const {
        LOCAL_ASSERT(0 < i && i < rounded_n(),
                     "invalid node index " << i << " for n = " << n_
                                           << " (must be internal node)");
    }
    void check_range([[maybe_unused]] usize l, [[maybe_unused]] usize r) const {
        LOCAL_ASSERT(l <= r, "invalid range");
        LOCAL_ASSERT(r <= n_, "end of range is out of bounds");
    }
    auto levels() const -> usize { return bit_width(rounded_n()); }
    auto level_of(usize i) const -> usize {
        check_node(i);
        return levels() - bit_width(i);
    }
    auto meta_for(usize i) const -> NodeMeta {
        check_node(i);
        return NodeMeta{.len = usize(1) << level_of(i)};
    }
    void pull_node(usize i) {
        check_internal_node(i);
        if constexpr (s_uses_meta) {
            data_[i].pull(data_[2 * i], data_[2 * i + 1], meta_for(i));
        } else {
            data_[i].pull(data_[2 * i], data_[2 * i + 1]);
        }
    }
    auto node_in_level(i64 i, usize level) -> i64 {
        if (level == 0) return i64(rounded_n()) + i;
        return node_in_level(i, 0) >> level;
    }
  public:
    SegTree(usize n)
        :
#ifdef ENABLE_DEBUG
          n_(n),
#endif
          rounded_n_rt_(bit_ceil(n)),
          data_(std::make_unique<S[]>(2 * rounded_n())) {
    }
    template <class Begin, class End>
    SegTree(Begin begin, End end) : SegTree(std::distance(begin, end)) {
        std::copy(begin, end, data_.get() + rounded_n());
        for (usize i = rounded_n(); i-- > 1;) pull_node(i);
    }
    auto fold(usize l, usize r) -> S {
        check_range(l, r);
        if (l == r) return S();
        auto left_fold = S();
        auto right_fold = S();
        auto left_node = node_in_level(i64(l) - 1, 0);
        auto right_node = node_in_level(i64(r), 0);
        auto left_meta = std::conditional_t<s_uses_meta, NodeMeta, Nothing>();
        auto right_meta = std::conditional_t<s_uses_meta, NodeMeta, Nothing>();
        while (left_node + 1 < right_node) {
            if (left_node % 2 == 0) {
                if constexpr (s_uses_meta) {
                    left_meta.pull(left_meta, meta_for(left_node + 1));
                    left_fold.pull(left_fold, data_[left_node + 1], left_meta);
                } else {
                    left_fold.pull(left_fold, data_[left_node + 1]);
                }
            }
            if (right_node % 2 == 1) {
                if constexpr (s_uses_meta) {
                    right_meta.pull(meta_for(right_node - 1), right_meta);
                    right_fold.pull(data_[right_node - 1], right_fold,
                                    right_meta);
                } else {
                    right_fold.pull(data_[right_node - 1], right_fold);
                }
            }
            left_node >>= 1;
            right_node >>= 1;
        }
        if constexpr (s_uses_meta) {
            left_meta.pull(left_meta, right_meta);
            left_fold.pull(left_fold, right_fold, left_meta);
        } else {
            left_fold.pull(left_fold, right_fold);
        }
        return left_fold;
    }
    void update(usize i, const S &x) {
        check_range(i, i + 1);
        usize j = i + rounded_n();
        data_[j] = x;
        while (j >>= 1) pull_node(j);
    }
};
 
struct HLD {
    struct DoNothing {
        void operator()(u32, u32) {}
    };
 
    vector<u32> subtree_size;
    vector<u32> depth;
    vector<u32> parent;
    vector<u32> heavy_starts;
    vector<u32> in_order;
    vector<u32> positions;
 
    template <class G>
    HLD(const G &g, u32 root)
        : subtree_size(g.size()),
          depth(g.size()),
          parent(g.size()),
          heavy_starts(g.size()) {
        positions.resize(g.size());
        in_order.reserve(g.size());
        find_sizes(g, root, ~0u);
        hld(g, root, ~0u, root);
    }
 
    template <class G>
    void find_sizes(const G &g, u32 cur, u32 prv) {
        parent[cur] = prv;
        subtree_size[cur] = 1;
        for (auto nxt : g[cur]) {
            if (nxt == prv) continue;
            depth[nxt] = depth[cur] + 1;
            find_sizes(g, nxt, cur);
            subtree_size[cur] += subtree_size[nxt];
        }
    }
 
    template <class G>
    void hld(const G &g, u32 cur, u32 prv, u32 heavy_start) {
        positions[cur] = u32(in_order.size());
        in_order.push_back(cur);
        heavy_starts[cur] = heavy_start;
 
        u32 heavy_child = u32(g.size());
        for (auto nxt : g[cur]) {
            if (nxt == prv) continue;
            if (heavy_child == g.size() ||
                subtree_size[nxt] > subtree_size[heavy_child]) {
                heavy_child = nxt;
            }
        }
 
        if (heavy_child != g.size()) {
            hld(g, heavy_child, cur, heavy_start);
        }
 
        for (auto nxt : g[cur]) {
            if (nxt == prv || nxt == heavy_child) continue;
            hld(g, nxt, cur, nxt);
        }
    }
 
    auto pos(u32 u) const -> u32 { return positions[u]; }
 
    template <class Callback = DoNothing>
    auto operator()(u32 u, u32 v, Callback &&callback = Callback()) -> u32 {
        while (heavy_starts[u] != heavy_starts[v]) {
            if (depth[heavy_starts[u]] > depth[heavy_starts[v]]) {
                callback(pos(heavy_starts[u]), pos(u) + 1);
                u = parent[heavy_starts[u]];
            } else {
                callback(pos(heavy_starts[v]), pos(v) + 1);
                v = parent[heavy_starts[v]];
            }
        }
        if (depth[u] < depth[v]) {
            callback(pos(u), pos(v) + 1);
            return u;
        } else {
            callback(pos(v), pos(u) + 1);
            return v;
        }
    }
};
 
struct S {
    int64_t m_v = 0;
    void pull(const S &l, const S &r) { m_v = l.m_v + r.m_v; }
};
 
int main()
{
    cin.tie(nullptr)->sync_with_stdio(false);
    usize n, q;
    cin >> n >> q;
 
    auto initial_values = vector<S>(n);
    for (auto &x : initial_values) cin >> x.m_v;
 
    auto g = vector<basic_string<u32> >(n);
    for (usize nn = 0; nn != n - 1; ++nn) {
        u32 u, v;
        cin >> u >> v;
        --u, --v;
        g[u].push_back(v);
        g[v].push_back(u);
    }
 
    auto hld = HLD(g, 0);
 
    auto permuted_initial_values = vector<S>(n);
    for (u32 i = 0; i != n; ++i)
        permuted_initial_values[hld.pos(i)] = initial_values[i];
 
    auto st = SegTree<S>(permuted_initial_values.begin(),
                         permuted_initial_values.end());
 
    while ( q--)
    {
        char qt;
        std::cin >> qt;
        if (qt == '1') {
            u32 u, x;
            std::cin >> u >> x;
            --u;
            st.update(hld.pos(u), S{x});
        } else {
            u32 s;
            std::cin >> s;
            --s;
            int64_t ans = 0;
            hld(s, 0,
                [&](int64_t l, int64_t r) { ans += st.fold(l, r).m_v; });
            std::cout << ans << '\n';
        }
    }
}