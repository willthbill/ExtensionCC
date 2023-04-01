#pragma once

#include<bits/stdc++.h>
#include"rng.h"
#include"cgal.h"
#include"chgraph.h"

typedef std::function<std::vector<std::vector<int>>()> GetCliquesLambda;

CHGraph tmp;

class CliqueCover {
private:
    CHGraph& chgraph = tmp;
    std::vector<std::vector<int>> _get_cliques_smalladj_naive();
    std::vector<std::vector<int>> _get_cliques_largeadj_naive();
    std::vector<std::vector<int>> _get_cliques_sorter_naive(std::function<bool(const int&, const int&)> comp);
    std::vector<std::vector<int>> _get_cliques_local();
    std::vector<std::vector<int>> _get_cliques_vcc();
    bool fix_chgraph_clique_remove_all_edges(std::vector<int>& clique);
    std::vector<std::vector<int>> fix_chgraph_clique_split(std::vector<int>& clique);
    bool fix_chgraph_clique_remove_single_edge(std::vector<int>& clique);
    bool fix_chgraph_cliques(std::vector<std::vector<int>>& cliques);
    std::vector<std::vector<int>> find_valid_cliques(GetCliquesLambda get_cliques, int fix_type);
    std::vector<Polygon> find_valid_clique_polygons(GetCliquesLambda get_cliques, int fix_type);
    long long get_hash();
public:
    int n();
    long long m();
    RNG rng;
    CliqueCover(CHGraph& _chgraph);
    std::vector<Polygon> get_cliques_smalladj_naive();
    std::vector<Polygon> get_cliques_largeadj_naive();
    std::vector<Polygon> get_cliques_local();
    std::vector<Polygon> get_cliques_vcc();
};
