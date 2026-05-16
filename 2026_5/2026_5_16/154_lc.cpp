/*
本题和2026_5_15 题目区别就是有重复元素
加一行相等时的判断即可
*/

class Solution {
   public:
    int findMin(vector<int>& nums) {
        int l = 0, r = nums.size() - 1;
        while (l < r) {
            int mid = (l + r) >> 1;
            // 注意这里要和nums[r] 进行比较
            if (nums[mid] == nums[r]) {
                r--;
                continue;
            }
            if (nums[mid] < nums[r])
                r = mid;
            else
                l = mid + 1;
        }
        return nums[l];
    }
};
