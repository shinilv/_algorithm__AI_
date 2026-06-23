/*
题意：
给定一个二叉树根节点 root， 判断该二叉树是否是对称二叉树
思路： 递归比较左右子树，
核心代码
if (!l && !r) return true;
if (!l || !r) return false;
return l->val == r->val && check(l->left, r->right) && check(l->right, r->left);
*/

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    bool check(TreeNode* l, TreeNode* r) {
        if (!l && !r) return true;
        if (!l || !r) return false;
        return l->val == r->val && check(l->left, r->right) && check(l->right, r->left);
    }
    bool isSymmetric(TreeNode* root) {
        if (root == nullptr) return true;
        return check(root->left, root->right);
    }
};
