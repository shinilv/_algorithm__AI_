/*
题意：
给定一个链表，如果这个链表存在环，返回链表的进入环的那个节点
否则 返回nullptr
思路：
设head距离入环点 a， 快慢指针焦点是 b， c = 环的长度 - b
应为快指针比慢指针走的距离多一倍：有公式 a + t(b + c) + b = 2 * (a + b) t是相交之前快指针走了几圈
a + tb + tc + b = 2 * a + 2 * b ==>  a = tb + tc - b ==> a = tb + tc -b - c + c ==> a = (a + b) * (t - 1) + c
那么可知 a的长度就是交点出在走（t - 1）圈 + c， 那么让一个指针从head 开始，slow从交点开始，他们下一次相遇一定在入环点处
*/

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode *detectCycle(ListNode *head) {
        ListNode *slow = head, *fast = head;
        while (slow && fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        if (slow == nullptr) return nullptr;
        ListNode* ptr = head;
        while (ptr != slow) {
            ptr = ptr->next;
            slow = slow->next;
        }
        return ptr;
    }
};
