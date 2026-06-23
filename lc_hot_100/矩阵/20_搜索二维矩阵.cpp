/*
题意：
给你一个二维矩阵，每行升序，每列升序
快速确认矩阵中是否含有目标值target
思路：
初始令i = n - 1, j = 0， 然后每次和target比较，如果matrix[i][j] > target j++; matrix[i][j] < target i--
如果相等直接返回true
复杂度是O(n + m)
*/

class Solution {
   public:
    bool searchMatrix(vector<vector<int>>& matrix, int target) {
        int n = matrix.size(), m = matrix[0].size();
        int i = n - 1, j = 0;
        while (i >= 0 && j < m) {
            if (matrix[i][j] > target) {
                j++;
            } else if (matrix[i][j] < target) {
                i--;
            } else {
                return true;
            }
        }
        return false;
    }
};
