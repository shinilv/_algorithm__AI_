/*
题意： 给你一个只包含正整数的非空数组 nums，判断是否可以将这个数组分割成两个子集，
使得两个子集的元素和相等。

数据范围：
1 <= nums.length <= 200
1 <= nums[i] <= 100

思路：
先计算处数组和 sum， 如果sum 为奇数，返回false 否则返回 true
为偶数，只需要判断数组中是否能选出一些元素，使这些元素的和 = sum / 2
dp[i][j] 表示 在前i个元素中和为j的子集是否存在

还有另一种做法，01背包，容量为sum / 2的情况下，可以取得的最大值是多少。判断dp[sum / 2] 是否 = sum / 2

*/

class Solution {
public:
    bool canPartition(vector<int>& nums) {
        int n = nums.size();
        int sum = 0;
        for (auto x : nums) sum += x;
        if (sum & 1) return false;
        vector<vector<bool>> dp(n + 1, vector<bool>(sum / 2 + 1));
        dp[0][0] = true;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= sum / 2; j++) {
                dp[i + 1][j] = dp[i][j];
                if (j >= nums[i])
                    dp[i + 1][j] = dp[i + 1][j] || dp[i][j - nums[i]];
            }
        }
        return dp[n][sum / 2];
    }
};
