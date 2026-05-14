/*
题意：
https://codeforces.com/problemset/problem/1965/A

输入 T(≤1e4) 表示 T 组数据。所有数据的 n 之和 ≤2e5。
每组数据输入 n(1≤n≤2e5) 和长为 n 的数组 a(1≤a[i]≤1e9)。

Alice 和 Bob 玩游戏。Alice 先手。
每回合，当前玩家选择一个正整数 k，满足 k <= min(a)。先把所有 a[i] 都减少 k，然后从 a 中移除所有等于 0 的数。
如果操作之后 a 是空的，当前玩家获胜。

如果双方都采取最优策略，谁会赢？
输出 Alice 或 Bob。

思路：
对数组进行排序去重， 如果 a[0] > 1 Alice必胜：试想如果第一次选 k = a[0]
如果Alice必胜则Alice胜
如果Alice必败则第一次选k = a[0] - 1, 这样相当于Bob先手选择了 k = a[0] Bob必败
所以a[0] > 1 则Alice必胜

a[0] = 1 的情况，可以看成Bob先手 数组从第二个位置开始的子问题（这里可以递归去处理）
*/

#include <bits/stdc++.h>
using namespace std;

void solve() {
    int n;
    cin >> n;
    vector<int> ans;
    unordered_map<int, int> mp;
    for (int i = 0; i < n; i++) {
        int x;
        cin >> x;
        if (mp.count(x)) continue;
        mp[x] = 1;
        ans.push_back(x);
    }
    sort(ans.begin(), ans.end());
    if (ans[0] >= 2 || ans.size() == 1) {
        cout << "Alice\n";
    } else {
        auto dfs = [&](auto&& dfs, int u, int t, int f) -> bool {
            if (u == ans.size() - 1 || ans[u] - t > 1) {
                return f;
            }
            return dfs(dfs, u + 1, t + 1, !f);
        };
        if (dfs(dfs, 1, 1, 0)) {
            cout << "Alice\n";
        } else {
            cout << "Bob\n";
        }
    }
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
