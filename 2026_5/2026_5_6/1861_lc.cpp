class Solution {
public:
    vector<vector<char>> rotateTheBox(vector<vector<char>>& boxGrid) {
        int m = boxGrid.size(), n = boxGrid[0].size();
        vector ans(n, vector<char>(m, '.'));

        for (int i = 0; i < m; i++) {
            auto& row = boxGrid[i];
            int k = n - 1;
            for (int j = n - 1; j >= 0; j--) {
                if (row[j] == '*') { // 障碍物
                    ans[j][m - 1 - i] = '*';
                    k = j - 1; // 障碍物左边最近的石头，在旋转后掉落到 j-1
                } else if (row[j] == '#') { // 石头
                    ans[k][m - 1 - i] = '#'; // 旋转后，石头掉落到 k
                    k--;
                }
            }
        }

        return ans;
    }
};