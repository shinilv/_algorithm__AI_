/*
题意：
给你一个整数数组 nums，有一个大小为 k 的滑动窗口从数组的最左侧移动到数组的最右侧。
你只可以看到在滑动窗口内的 k 个数字。滑动窗口每次只向右移动一位。
返回 滑动窗口中的最大值 。

思路：
需要计算窗口内的最大值，可以考虑维护一个单调递减双端队列
如果当前元素大于队列末尾，末尾弹出
这样每个窗口内的最大值可以O(1) 取得（队列头部），
如果队列头部不在这个窗口中的话，就头部弹出
*/

class Solution {
   public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        int n = nums.size();
        deque<int> dq;
        vector<int> ans;
        for (int i = 0; i < n; i++) {
            while (dq.size() && nums[i] >= nums[dq.back()]) {
                dq.pop_back();
            }
            dq.push_back(i);
            if (i < k - 1) continue;
            if (dq.front() + k == i) dq.pop_front();
            ans.push_back(nums[dq.front()]);
        }
        return ans;
    }
};
