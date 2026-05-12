/*
题意：给一个数组，找出其中的两个数，使得它们的和等于目标值。
思路：
1. 使用哈希表来存储数组中的元素和它们的索引。
2. 遍历数组，对于每个元素，计算出目标值与当前元素的差值。
3. 在哈希表中查找这个差值，如果存在且索引，则找到了两个数，返回它们的索引。
*/

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> mp;
        for (int i = 0; i < nums.size(); i++) {
            int x = target - nums[i];
            if (mp.count(x)) {
                return {mp[x], i};
            }
            mp[nums[i]] = i;
        }
        return {};
    }
};