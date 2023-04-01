#pragma once

#include<bits/stdc++.h>

class ComponentFinder {
public:

    const int RAND_SEED = 42;
    const int MXCOMPSIZE = 100;
    const int MNCOMPSIZE = 20;

    std::map<int,std::set<int>> adj;
    std::function<bool(std::vector<int>)> valid;
    ComponentFinder(
        std::map<int,std::set<int>> adj,
        std::function<bool(std::vector<int>)> valid
    );
    std::vector<std::vector<int>> find_components_single_source();
    void _find_components_single_source(
        std::vector<std::vector<int>>& components,
        std::set<int>& done
    );
    std::vector<std::vector<int>> find_components_multi_source();
    void _find_components_multi_source(
        std::vector<std::vector<int>>& components,
        std::set<int>& done,
        std::vector<int>& sources
    );
    std::vector<std::vector<int>> find_components();
    std::vector<int> get_random_sources();
    std::vector<int> get_good_sources();
};
