/*
题意：
https://leetcode.cn/problems/count-subarrays-with-majority-element-ii/description/
给一个数组nums和一个整数target
返回满足target是主要元素（出现次数大于长度的一半）的子数组数目

思路：
构造一个新数组， 如果nums[i] == targe s[i] = 1, 否则 s[i] = -1
这样问题就转换成了，子数组和 > 0 的个数 ==> 计算s的前准和 就是
求有多少 pre[j] - pre[i] > 0 其中 j > i
也就是pre[j] > pre[i]，可以枚举j， 然后计算[0 - j - 1] 中满足条件的个数
可以使用树状数组维护。初始的时候像树状数组中加入一个0，模仿 -1 位置
*/


class Solution {
public:
    long long countMajoritySubarrays(vector<int>& nums, int target) {
        int n = nums.size();
        int offset = n;  // 偏移量，将范围[-n, n]映射到[0, 2n]
        int size = 2 * n + 1;

        vector<int> bit(size + 1, 0);

        auto update = [&](int idx, int val) {
            for (int i = idx; i <= size; i += i & -i) {
                bit[i] += val;
            }
        };

        auto query = [&](int idx) {
            int sum = 0;
            for (int i = idx; i > 0; i -= i & -i) {
                sum += bit[i];
            }
            return sum;
        };

        long long ans = 0;
        int pre = 0;

        // 初始加入pre = 0（对应索引 offset + 0）
        update(offset + 1, 1);  // 树状数组索引从1开始

        for (int num : nums) {
            pre += (num == target ? 1 : -1);
            int idx = offset + pre + 1;  // 树状数组索引
            // 查询严格小于当前pre的个数
            ans += query(idx - 1);
            // 将当前pre加入树状数组
            update(idx, 1);
        }

        return ans;
    }
};
