/*
题意： 给定一个非负整数 numRows，生成杨辉三角的前 numRows 行。
在杨辉三角中，每个数是它左上方和右上方的数之和。

*/

class Solution {
public:
    vector<vector<int>> generate(int numRows) {
        vector<vector<int>> c(numRows);
        for (int i = 0; i < numRows; i++) {
            c[i].resize(i + 1, 1);
            for (int j = 1; j < i; j++) {
                c[i][j] = c[i - 1][j - 1] + c[i - 1][j];
            }
        }
        return c;
    }
};
