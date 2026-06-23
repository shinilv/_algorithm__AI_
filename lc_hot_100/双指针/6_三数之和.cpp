/*
题意： 给你一个整数数组 nums ，判断是否存在三元组 [nums[i], nums[j],
 nums[k]] 满足 i != j、i != k 且 j != k ，
 同时还满足 nums[i] + nums[j] + nums[k] == 0 。
 请你返回所有和为 0 且不重复的三元组。

思路：
首先可以给数组进行一个升序排序， 找三元组的时候，可以
枚举最左边的端点 i， 令l = i + 1, r = n - 1
由于三元组的条件是sum = nums[i] + nums[l] + nums[r] == 0
这一要求，如果sum > 0 r--, sum == 0, 把这三个三元组加入答案， sum < 0 l++
最后因为不可以包含重复的三元组， 所以 如果nums[i] == nums[i - 1] 就不处理这个
如果nums[l + 1] == nums[l], nums[r] == nums[r - 1] 这些也要跳过，避免重复 
*/


class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        sort(nums.begin(), nums.end());
        int n = nums.size();
        vector<vector<int>> ans;
        for (int i = 0; i < n - 2; i++) {
            if (i && nums[i] == nums[i - 1]) continue;
            int l = i + 1, r = n - 1;
            while (l < r) {
                if (nums[i] + nums[l] + nums[r] == 0) {
                    ans.push_back({nums[i], nums[l], nums[r]});
                    while (l + 1 < n && nums[l] == nums[++l]);
                    while (r - 1 > 0 && nums[r] == nums[--r]);
                } else if (nums[i] + nums[l] + nums[r] > 0) {
                    r--;
                } else {
                    l++;
                }
            }
        }
        return ans;
    }
};