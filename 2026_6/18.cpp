/*
题意： 给你两个数 hour 和 minutes 。请你返回在时钟上，由给定时间的时针和分针组成的较小角的角度（60 单位制）。

思路：分别计算时针和分针在0 - 360 那个位置，记为 p1 = hour * 360 / 12 + minutes * 30 / 60， p2 = minutes * 360 / 60
*/


class Solution {
public:
    double angleClock(int hour, int minutes) {
        double p1 = hour * 30 + minutes / 2.0, p2 = minutes * 6;
        double ans = abs(p1 - p2);
        return min(ans, 360 - ans);
    }
};
