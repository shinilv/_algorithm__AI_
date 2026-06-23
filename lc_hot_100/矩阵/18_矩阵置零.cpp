/*
题意：
给定一个 m x n 的矩阵，如果一个元素为 0 ，则将其所在行和列的所有元素都设为 0 。

思路：
这里有空间O(m + n) 的做法，遍历标记一下行或列是否有0
O(1) 的做法是：先不处理行列首个元素。
然后把行列首元素当成标记数组去处理
注意要使用两个bool元素标记一下行列中是否有0，然后最后处理一下首行首列元素
*/

class Solution {
   public:
    void setZeroes(vector<vector<int>>& matrix) {
        int n = matrix.size(), m = matrix[0].size();
        bool row = false, col = false;
        for (int i = 0; i < m; i++)
            if (matrix[0][i] == 0) row = true;
        for (int i = 0; i < n; i++)
            if (matrix[i][0] == 0) col = true;
        for (int i = 1; i < n; i++) {
            for (int j = 1; j < m; j++) {
                if (matrix[i][j] == 0) {
                    matrix[i][0] = matrix[0][j] = 0;
                }
            }
        }
        for (int i = 1; i < n; i++) {
            for (int j = 1; j < m; j++) {
                if (matrix[i][0] == 0 || matrix[0][j] == 0) {
                    matrix[i][j] = 0;
                }
            }
        }
        if (row) {
            for (int i = 0; i < m; i++) matrix[0][i] = 0;
        }
        if (col) {
            for (int i = 0; i < n; i++) matrix[i][0] = 0;
        }
    }
};
