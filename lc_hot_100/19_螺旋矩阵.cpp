/*
题意：
给你一个数组，按照顺时针旋转的顺序，把所有的元素放到一个一维数组中


思路：
dfs 按照顺序搜索一遍即可，
技巧：
std::vector<std::array<int, 2>> dis({{0, 1}, {1, 0}, {0, -1}, {-1, 0}});
使用这个方向数组
已经搜到过的元素可以更改为 101(矩阵中的最大值不超过100)
遇到边界或者已经搜过的可以按照dis 改变方向，具体看代码
*/

class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        int n = matrix.size(), m = matrix[0].size();
        std::vector<int> ans(n * m);
        std::vector<std::array<int, 2>> dis({{0, 1}, {1, 0}, {0, -1}, {-1, 0}});
        int cnt = 0;
        auto dfs = [&](auto&& dfs, int x, int y, int idx) -> void {
            if (x < 0 || x >= n || y < 0 || y >= m || matrix[x][y] == 101) {
                return;
            }
            ans[cnt++] = matrix[x][y];
            matrix[x][y] = 101;
            int dx = x + dis[idx][0], dy = y + dis[idx][1];
            if (dx < 0 || dx >= n || dy < 0 || dy >= m || matrix[dx][dy] == 101) {
                idx = (idx + 1) % 4;
            }
            dx = x + dis[idx][0], dy = y + dis[idx][1];
            dfs(dfs, dx, dy, idx);
        };
        dfs(dfs, 0, 0, 0);
        return ans;
    }
};
