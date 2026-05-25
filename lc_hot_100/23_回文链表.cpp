/*
题意：
给定一个单链表的头节点，判断该链表是不是回文链表
题解：
快慢指针 + 反转链表，然后遍历比较
*/

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
   public:
    bool isPalindrome(ListNode* head) {
        ListNode *p1 = head, *p2 = head;
        while (p2 && p2->next) {
            p1 = p1->next;
            p2 = p2->next->next;
        }
        ListNode* pre = nullptr;
        while (p1) {
            ListNode* next = p1->next;
            p1->next = pre;
            pre = p1;
            p1 = next;
        }
        p1 = pre;
        while (p1) {
            if (p1->val != head->val) {
                return false;
            }
            p1 = p1->next;
            head = head->next;
        }
        return true;
    }
};
