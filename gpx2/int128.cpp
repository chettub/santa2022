// https://kenkoooo.hatenablog.com/entry/2016/11/30/163533
#include <bits/stdc++.h>
using namespace std;
using int128 = __int128;

std::ostream& operator<<(std::ostream& dest, __int128_t value) {
    std::ostream::sentry s(dest);
    if (s) {
        __uint128_t tmp = value < 0 ? -value : value;
        char buffer[128];
        char* d = std::end(buffer);
        do {
            --d;
            *d = "0123456789"[tmp % 10];
            tmp /= 10;
        } while (tmp != 0);
        if (value < 0) {
            --d;
            *d = '-';
        }
        int len = std::end(buffer) - d;
        if (dest.rdbuf()->sputn(d, len) != len) {
            dest.setstate(std::ios_base::badbit);
        }
    }
    return dest;
}

__int128 parse(string& s) {
    __int128 ret = 0;
    for (int i = 0; i < s.length(); i++)
        if ('0' <= s[i] && s[i] <= '9')
            ret = 10 * ret + s[i] - '0';
    return ret;
}

string to_string_int128(int128 value) {
    assert(value >= 0);
    string s;
    do {
        s += "0123456789"[value % 10];
        value /= 10;
    } while (value != 0);
    reverse(s.begin(), s.end());
    return s;
}

int128 abs_int128(int128 a) {
    return a >= 0 ? a : -a;
}