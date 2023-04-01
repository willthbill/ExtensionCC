#pragma once

// from scream&cout cplib

#include<bits/stdc++.h>

struct RNG {
    std::random_device device;
    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
    RNG();
	RNG(int, int, int);
	int get();
};
