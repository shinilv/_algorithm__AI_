/*
题意：
给定每根竖线的位置和高度，找两根线组成容器，使它们之间能装的水量最大。
思路：
双指针，i = 0, j = n - 1， 比较两个指针对应高度的大小，让较小的一个++/--
每次更新ans = max(ans, (j - i + 1) * min(height[i], height[j]))
*/

class Solution {
public:
    int maxArea(vector<int>& height) {
        int i = 0, j = height.size() - 1;
        int ans = min(height[0], height[j]) * (j - i);
        while(i < j) {
            if (height[i] < height[j]) i++;
            else j--;
            ans = max(ans, min(height[i], height[j]) * (j - i));
        }
        return ans;
    }
};