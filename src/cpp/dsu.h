#pragma once

#include<bits/stdc++.h>

struct DSU {
    std::map<int,int> par, size;
    DSU(){}
    void make_set(int a);
    int find_set(int a);
    void union_sets(int a, int b);
};
