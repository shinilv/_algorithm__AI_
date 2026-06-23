/*
题意：给定一个未排序的整数数组 nums ，找出数字连续的最长序列（不要求序列元素在原数组中连续）的长度。要求时间复杂度O(n)
思路：用一个unordered_set存放这个数组的所有元素
遍历这个 set的所有元素，如果此时是元素x，set中含有x - 1, 则continue，这个已经统计过了
否则就令y = x + 1
int y = x + 1;
while(st.contains(y))
    y++;
执行这个操作，可以得到 y - x 去更新结果

*/

class Solution {
public:
    int longestConsecutive(vector<int>& nums) {
        unordered_set<int> se(nums.begin(), nums.end());
        int ans = 0;
        for (auto x : se) {
            if (se.count(x - 1)) continue;
            int y = x + 1;
            while (se.count(y)) y++;
            ans = max(ans, y - x);
            if (ans > nums.size() / 2) break;
        }      
        return ans;
    }
};