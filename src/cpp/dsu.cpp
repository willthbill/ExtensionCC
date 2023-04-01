#include<bits/stdc++.h>

#include "dsu.h"

using namespace std;

void DSU::make_set(int a){
    par[a] = a;
    size[a] = 1;
}

int DSU::find_set(int a){
    if(par[a] != a) par[a] = find_set(par[a]);
    return par[a];
}

void DSU::union_sets(int a, int b) {
    a = find_set(a);
    b = find_set(b);
    if(a != b){
        if(size[a] < size[b]) swap(a,b);
        size[a] += size[b];
        par[b] = a;
    }
};
