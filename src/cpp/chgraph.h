#pragma once

#include <bits/stdc++.h>
#include <boost/filesystem.hpp>

#include "cgal.h"
#include "dualgraph.h"
#include "visibility.h"
#include "intersection_predicates.h"

class VisibilityGraph {
private:

    std::vector<int> get_neighbours_by_condition(int source, std::function<bool(int)> condition);

    std::map<int,Point> centroids;

    long long get_hash();

    std::string get_cache_filename();
        
    const std::string CACHE_DIR_START = "output/visibility_graphs";

    std::string CACHE_DIR;

    boost::filesystem::path get_full_cache_filename();

    std::set<int> bfs_dual_graph(int source, std::function<bool(int)> f, bool is_symmetric);

    std::map<int,std::set<int>> invisible_cache;
    std::set<std::pair<Point,Point>> segments_on_polygon_edge;

    std::set<int> get_fully_visible_bfs_edges_exact_single_node(int source);
    std::set<int> get_fully_visible_bfs_edges_inexact_single_node(int source);

public:

    std::map<int,std::set<int>> adj;

    Polygon_with_holes polygon;
    std::vector<Polygon> triangulation;
    IntersectionPredicates pred;
    VisibilityDataStructure visds;
    DualGraph dualgraph;

    VisibilityGraph(Polygon_with_holes _polygon, std::vector<Polygon> triangulation, std::string cache_directory); 
    VisibilityGraph();

    void delete_datastructures();

    void write_to_cache();

    bool read_from_cache(); 

    void add_all_edges_parallel(); 

    void add_all_edges(); 

    void add_edges_from_mapping(std::map<int,int>& mapper, VisibilityGraph& other); 

    void add_fully_visible_bfs_edges_exact();

    void add_fully_visible_bfs_edges_inexact();

    void add_edges_to_make_undirected();

    Polygon get_convex_hull_of_nodes(std::vector<int> nodes); 

    std::vector<Polygon> get_convex_hull_of_decomposition(std::vector<std::vector<int>> decomposition); 

    void remove_edge(int a, int b);

    FT get_dist(int a, int b);

    long long m();

};
