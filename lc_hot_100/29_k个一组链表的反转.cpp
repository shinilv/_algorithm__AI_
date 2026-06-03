/*
题意：k个一组翻转链表，剩余不足k个不翻转

为这个链表创建一个预备节点 dummy(0, head)， 返回dummy.next 即可
然后遍历一遍求出链表的节点个数 n
假设要反转中间一段长度为k的链表，首先需要知道这段链表起点的前一个节点，所以这也是
需要dummy这一预备节点的原因。
令这段链表前一个节点p0，当前节点是cur，前一个节点（当前节点前一个节点）是pre
执行k次反转操作，也就是
for (int i = 0; i < k; i++) {
    nxt = cur->next
    cur->next = pre
    pre = cur;
    cur = next;
}
关键：

这一段链表的前一个节点p0 的下一个节点现在已经是这段链表的尾部节点，是下一段链表的前一个节点
所以执行p0 = p0->next
这一段链表的前一个节点p0 的下一个节点应该执行pre了，
应该执行p0->next = pre
但这里我们还需要用到p0的下下个节点，该节点应该指向cur（下一段链表的第一个节点）

调整，创建一个临时节点
temp = p0->next;
p0->next->next = cur;
po->next = pre;
p0 = temp;

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
    ListNode* reverseKGroup(ListNode* head, int k) {
        int n = 0;
        for (ListNode* temp = head; temp; temp = temp->next, n++);
        ListNode dummy(0, head);
        ListNode *p0 = &dummy, *pre = nullptr, *cur = head;
        for (; n >= k; n -= k) {
            for (int i = 0; i < k; i++) {
                ListNode* nxt = cur->next;
                cur->next = pre;
                pre = cur;
                cur = nxt;
            }
            ListNode* nxt = p0->next;
            p0->next->next = cur;
            p0->next = pre;
            p0 = nxt;
        }
        return dummy.next;
    }
};
