/*
题意：
这里有一个非负整数数组 arr，你最开始位于该数组的起始下标 start 处。
当你位于下标 i 处时，你可以跳到 i + arr[i] 或者 i - arr[i]。
请你判断自己是否能够跳到对应元素值为 0 的 任一 下标处。

思路：
考虑bfd深搜，记录已经到达过的下标，如果没到达过就加入数组
如果遇到一个下下标对应元素 = 0, 返回true
*/


class Solution {
public:
    bool canReach(vector<int>& arr, int start) {
        int n = arr.size();
        vector<int> q, vis(n);
        q.push_back(start);
        vis[start] = 1;
        while (q.size()) {
            vector<int> temp;
            for (auto x : q) {
                if (arr[x] == 0) return true;
                int l = x - arr[x], r = x + arr[x];
                if (l >= 0 && !vis[l]) temp.push_back(l), vis[l] = 1;
                if (r < n && !vis[r]) temp.push_back(r), vis[r] = 1;
            }
            q = move(temp);
        }
        return false;
    }
};
