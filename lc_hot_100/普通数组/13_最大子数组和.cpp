/*
题意：
给你一个整数数组 nums ，
请你找出一个具有最大和的连续子数组（子数组最少包含一个元素），
返回其最大和。

子数组是数组中的一个连续部分。

思路：
计算一个最大和的连续子数组，
计算数组的前缀和，如果前缀和 pre < 0， 就令 pre = 0，
表示从这个位置开始为起点重新计算子数组和。每次更新 ans
*/


class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int pre = 0, ans = nums[0];
        for (auto x : nums) {
            if (pre < 0) pre = 0;
            pre += x;
            ans = max(ans, pre);
        }
        return ans;
    }
};
