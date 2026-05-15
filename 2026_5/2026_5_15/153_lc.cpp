/*
题意：
给你一个原本升序排列的数组，
但是它被“旋转”了若干次，现在让你在这个旋转后的数组里找到最小值。
要求使用Olog(n) 的复杂度
[0,1,2,4,5,6,7] -> [4,5,6,7,0,1,2]

思路：
令数组末尾元素 = x
如果a[i] > x 那么要找的最小值的位置一定在i的右边
如果a[i] < x 那么要找的最小值的位置一定在i的左边
根据这个性质，我们可以想到二分查找

*/

class Solution {
   public:
    int findMin(vector<int>& nums) {
        int n = nums.size(), x = nums.back();
        int l = 0, r = n - 1;
        while (l < r) {
            int mid = l + r >> 1;
            if (nums[mid] <= x)
                r = mid;
            else
                l = mid + 1;
        }
        return nums[l];
    }
};
