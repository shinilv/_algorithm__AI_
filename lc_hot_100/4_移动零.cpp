/*
题意：给定一个数组 nums，编写一个函数将所有 0 移动到数组的末尾，同时保持非零元素的相对顺序。
要求原地操作，且减少操作次数

思路1：
        int n = nums.size();
        int idx = 0;
        for (int i = 0; i < n; i++) {
            if (nums[i] != 0) {
                nums[idx++] = nums[i];
            } 
        }
        while (idx < n) nums[idx++] = 0;
        直接看代码，非零元素按顺序填写，最后补0
思路2：
        int pos = 0;
        for (int i = 0; i < nums.size(); i++) {
            if (nums[i]) {
                swap(nums[i], nums[pos]);
                pos++;
            }
        }
        这个思路符合”尽量减少操作次数“，把左边为0的位置当成空位
         0 0 0 1 2 3 0 1 2
         比如这个样例
         i = 3 时，1放到pos = 0的位置
         i = 4 时，2放到pos = 1的位置
         i = 5 时，3放到pos = 2的位置
         i = 7 时，3放到pos = 3的位置
         i = 8 时，3放到pos = 4的位置
         本质上和思路1是一样的，把非零数字往前放，这种方法操作次数更少

*/

class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        
    }
};