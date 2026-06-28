/*
题意： 给你一个整数数组 nums，找到其中最长严格递增子序列的长度。
子序列是由数组派生而来的序列，删除数组中的一些或不删除元素，且不改变其余元素的顺序。

数据范围：
1 <= nums.length <= 2500
-10^4 <= nums[i] <= 10^4

思路：
令dp[i] 0 - i 中严格递增子序列的最大长度
dp[i] = mxa(dp[j] + 1) if nums[j] < nums[i] (0 <= j < i) else 1
但是这个复杂度是n^2 的
我门来优化一下时间复杂度
可以设置一个数组f，里面存放这个最大长度子序列，答案就是len(f)
对于f[i] 这里有个简单贪心，一定是越小越好。
遍历数组nums，对于nums[i]， 如果nums[i] > f.back() f.push_back(nums[i])
否则，我们可以用nums[i] 去更新 位置j，当f[j] > nums[i]，这一步可以二分logn查找
*/

class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        int n = nums.size();
        vector<int> f;
        for (auto x : nums) {
            if (f.empty() || x > f.back()) {
                f.push_back(x);
            } else {
                int pos = lower_bound(f.begin(), f.end(), x) - f.begin();
                f[pos] = x;
            }
        }
        return f.size();
    }
};
