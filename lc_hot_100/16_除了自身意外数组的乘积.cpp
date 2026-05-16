/*
题意：
给你一个整数数组 nums，返回 数组 answer ，
其中 answer[i] 等于 nums 中除了 nums[i] 之外其余各元素的乘积 。
保证结果在int范围内
思路：
要求不适用除法，O(n)完成
前后缀分解，ans[i] = pre[i - 1] * suf[i + 1]
*/

class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        int n = nums.size();
        vector<int> suf(n + 1);
        suf[n] = 1;
        for (int i = n - 1; i >= 0; i--) {
            suf[i] = suf[i + 1] * nums[i];
        }
        int pre = 1;
        vector<int> ans(n);
        for (int i = 0; i < n; i++) {
            ans[i] = pre * suf[i + 1];
            pre *= nums[i];
        }
        return ans;
    }
};
