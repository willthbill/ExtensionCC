// from scream&cout cplib

#include<bits/stdc++.h>
#include"rng.h"

using namespace std;

RNG::RNG(){}

RNG::RNG(int l, int r, int seed = -1) {
    if(seed == -1) {
        generator = mt19937(device());
    } else {
        generator = mt19937(seed);
    }
    distribution = uniform_int_distribution<int>(l, r);
}
int RNG::get() {
    return distribution(generator);
}
