/*
题意：
给你一个链表，如果链表中存在一个环，返回true，否则返回false

思路：
使用两个指针去判断链表中是否存在环，一个慢指针每次指向当前节点的下一个节点，另一个快指针，每次指向当前节点的下下个节点。
如果链表中存在环的话，那么快指针相对于慢指针每次是执行一次跳跃的（如果走到了这个环上）
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
    bool hasCycle(ListNode* head) {
        ListNode *slow = head, *fast = head;
        while (slow && fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) return true;
        }
        return false;
    }
};
