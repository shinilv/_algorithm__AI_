/*
题意： 你是一个专业的小偷，计划偷窃沿街的房屋。每间房内都藏有一定的现金，
唯一的限制是相邻的房屋装有相互连通的防盗系统，如果两间相邻的房屋在同一晚上被闯入，系统会自动报警。
给定一个代表每个房屋存放金额的非负整数数组 nums，计算你在不触动警报装置的情况下，一夜之内能够偷窃到的最高金额。

思路：
定义dp[i] 表示 0 - i 可以获得的最大值
那么有转移方程 dp[i] = max(dp[i - 1], dp[i - 2] + nums[i])
答案就是dp[n - 1]
*/

class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        if (n == 1) return nums[0];
        vector<int> dp(n);
        dp[0] = nums[0], dp[1] = max(nums[0], nums[1]);
        for (int i = 2; i < n; i++) {
            dp[i] = max(dp[i - 1], dp[i - 2] + nums[i]);
        }
        return dp[n - 1];
    }
};
