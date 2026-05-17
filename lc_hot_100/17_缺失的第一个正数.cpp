/*
题意：
给你一个未排序的整数数组 nums ，请你找出其中没有出现的最小的正整数。

请你实现时间复杂度为 O(n) 并且只使用常数级别额外空间的解决方案。

思路：
题目要求O(n)复杂度， 并且只能常数空间
这道题其实就是充分发挥数组下标的作用
我们可以让数组中 <= 0 的元素都变为 n + 1
然后让所有在 1 - n 范围内的元素 放到对应值的下标处元素，让它
变成负数，然后如果一个下标是大于零的那么这个下标就是没有出现过的正数
具体实现有一些细节，这个负数的绝对值要和原来的整数相等，原因看代码
*/


class Solution {
public:
    int firstMissingPositive(vector<int>& nums) {
        int n = nums.size();
        for (auto& x : nums) if (x <= 0) x = n + 1;
        for (auto& x : nums) {{
            int v = abs(x); // 因为可能被变为负数了，这里要取绝对值
            if (v <= n) {
                nums[v - 1] = -abs(nums[v - 1]);
            }
        }}
        for (int i = 0; i < n; i++) {
            if (nums[i] > 0) return i + 1;
        }
        return n + 1;
    }
};
