/*
题意： 给你一个整数数组 nums，请你找出数组中乘积最大的非空连续子数组，并返回该乘积。

数据范围：
1 <= nums.length <= 2 * 10^4
-10 <= nums[i] <= 10
nums 的任何前缀或后缀的乘积都保证在 32 位整数范围内

思路：
定义dp[i][0/1], dp[i][0] 表示到位置i，乘积最小， dp[i][1] 表示到i，乘积最大
dp[i][0] = min(dp[i - 1][0] * nums[i], dp[i - 1][1] * nums[i], nums[i])
dp[i][1] = max(dp[i - 1][0] * nums[i], dp[i - 1][1] * nums[i], nums[i])
返回max(dp[i][1]) 0 <= i < n
*/

class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int n = nums.size();
        vector<array<int, 2>> dp(n);
        dp[0][0] = nums[0], dp[0][1] = nums[0];
        int ans = nums[0];
        for (int i = 1; i < n; i++) {
            dp[i][0] = min(min(dp[i - 1][0] * nums[i], dp[i - 1][1] * nums[i]), nums[i]);
            dp[i][1] = max(max(dp[i - 1][0] * nums[i], dp[i - 1][1] * nums[i]), nums[i]);
            ans = max(ans, dp[i][1]);
        }
        return ans;
    }
};
