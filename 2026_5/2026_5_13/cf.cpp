#include <bits/stdc++.h>
using namespace std;

const int PRIME_N = 1000010;
int st[PRIME_N];
int primes[PRIME_N];
int cnt;
void fun()
{
    for (int i = 2; i < PRIME_N; i++)
    {
        if (!st[i])
        {
            st[i] = i;
            primes[cnt++] = i;
        }
        for (int j = 0; i * primes[j] < PRIME_N; j++)
        {
            st[i * primes[j]] = primes[j];
            if (i % primes[j] == 0)
                break;
        }
    }
}



void solve() {
    int n;
    cin >> n;
    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    int f = 0;
    for (int i = 1; i < n; i++) {
        if (a[i] < a[i - 1]) {
            f = 1;
        }
    }
    if (f == 0) {
        cout << "Bob\n";
        return;
    }
    vector<int> ans;
    for (auto x : a) {
        if (x == 1) {
            ans.push_back(1);
            continue;
        }
        int cnt = 0;
        int j = x;
        while(j > 1) {
            int p = st[j];
            while(j % p == 0) {
                j /= p;
            }
            ans.push_back(p);
            cnt++;
        }
        if(j > 1) cnt++, ans.push_back(j);
        if (cnt > 1) {
            cout << "Alice\n";
            return;
        }
    }
    f = 0;
    for (int i = 1; i < ans.size(); i++) {
        if (ans[i] < ans[i - 1]) {
            f = 1;
        }
    }
    if (f == 1) {
        cout << "Alice\n";
    } else 
        cout << "Bob\n";
}

int main() {
    fun();
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}