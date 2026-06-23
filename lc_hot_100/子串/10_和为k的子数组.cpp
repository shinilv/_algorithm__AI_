/*
题意：
给你一个整数数组 nums 和一个整数 k ，请你统计并返回 该数组中和为 k 的子数组的个数 。
数组中存在负数
子数组是数组中元素的连续非空序列。

思路：枚举右， 维护左
枚举到 i 这个点的时候看 i 左边有多少个子数组的和是和k相等的
可以使用哈希去记录一下左边的前缀和
那么 i 左边 到第i个位置满足条件的子数组个数就是 mp[prei - k], prei是到位置i的前缀和

*/

class Solution {
   public:
    int subarraySum(vector<int>& nums, int k) {
        int n = nums.size();
        unordered_map<int, int> mp;
        int ans = 0, pre = 0;
        mp[0]++;
        for (auto x : nums) {
            pre += x;
            if (mp.count(pre - k)) {
                ans += mp[pre - k];
            }
            mp[pre] += 1;
        }
        return ans;
    }
};
