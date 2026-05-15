/*
题意：
给定一个整数数组 nums，将数组中的元素向右轮转 k 个位置，其中 k 是非负数。

思路：
右移 k 次， 首先对k %= nums.size()
然后相当于 反转 1 - n, 1 - k - 1, k - n
反转三次，就可以达到右移的效果
*/

class Solution {
   public:
    void rotate(vector<int>& nums, int k) {
        int n = nums.size();
        k %= n;
        auto Reverse = [&](int l, int r) -> void {
            while (l < r) {
                swap(nums[l], nums[r]);
                l++, r--;
            }
        };
        Reverse(0, n - 1);
        Reverse(0, k - 1);
        Reverse(k, n - 1);
    }
};
