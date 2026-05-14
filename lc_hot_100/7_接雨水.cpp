/*
题意： 给定 n 个非负整数表示每个宽度为 1 的柱子的高度图，计算按此排列的柱子，下雨之后能接多少雨水。

思路：
考虑计算每一个位置对结果的贡献，
第 i 个位置的贡献是
令 max_l = max(height[0], height[2], .., height[i - 1])
令 max_r = max(height[i + 1], height[i + 2], .., height[n - 1])
ans += max(0, min(max_l, max_r) - height[i])
*/


class Solution {
public:
    int trap(vector<int>& height) {
        int n = height.size();
        vector<int> max_l(n + 2), max_r(n + 2);
        for (int i = 0; i < n; i++) {
            max_l[i + 1] = max(max_l[i], height[i]);
        }
        for (int i = n; i >= 1; i--) {
            max_r[i] = max(max_r[i + 1], height[i - 1]);
        }
        int ans = 0;
        for (int i = 2; i < n; i++) {
            ans += max(0, min(max_l[i - 1], max_r[i + 1]) - height[i - 1]);
        }
        return ans;
    }
};
