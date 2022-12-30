#include <bits/stdc++.h>

using namespace std;

vector<int> string_to_vector_int(string str, const char delim = ' ') {
    vector<int> vec;

    replace(str.begin(), str.end(), delim, ' ');
    istringstream iss(str);

    string str_buf;
    while (iss >> str_buf) {
        if (str_buf == "")
            continue;
        vec.emplace_back(stoi(str_buf));
    }

    return vec;
}