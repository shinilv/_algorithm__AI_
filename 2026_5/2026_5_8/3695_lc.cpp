/*
题意：

思路：

*/

// 模板来源 https://leetcode.cn/circle/discuss/mOr1u6/
class UnionFind {
    vector<int> fa; // 代表元
public:
    vector<int> odd; // 集合中的奇数个数

    UnionFind(int n) : fa(n), odd(n) {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己
        for (int i = 0; i < n; i++) {
            fa[i] = i;
            odd[i] = i % 2;
        }
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    int find(int x) {
        // 如果 fa[x] == x，则表示 x 是代表元
        if (fa[x] != x) {
            fa[x] = find(fa[x]); // fa 改成代表元
        }
        return fa[x];
    }

    // 把 from 所在集合合并到 to 所在集合中
    // 返回是否合并成功
    void merge(int from, int to) {
        int x = find(from), y = find(to);
        if (x == y) { // from 和 to 在同一个集合，不做合并
            return;
        }
        fa[x] = y; // 合并集合。修改后就可以认为 from 和 to 在同一个集合了
        odd[y] += odd[x]; // 更新集合中的奇数个数
    }
};

class Solution {
public:
    long long maxAlternatingSum(vector<int>& nums, vector<vector<int>>& swaps) {
        int n = nums.size();
        UnionFind uf(n); // 假设并查集已实现
        for (auto& p : swaps) {
            uf.merge(p[0], p[1]);
        }

        vector<vector<int>> g(n);
        for (int i = 0; i < n; i++) {
            g[uf.find(i)].push_back(nums[i]); // 相同集合的元素分到同一组
        }

        long long ans = 0;
        for (int i = 0; i < n; i++) {
            auto& a = g[i];
            if (a.empty()) {
                continue;
            }
            ranges::sort(a);
            int odd = uf.odd[i];
            // 小的取负号，大的取正号
            ans -= reduce(a.begin(), a.begin() + odd, 0LL);
            ans += reduce(a.begin() + odd, a.end(), 0LL);
        }
        return ans;
    }
};