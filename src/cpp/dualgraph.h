#pragma once

#include<bits/stdc++.h>
#include"cgal.h"

// no multiple edges, no selfloops, undirected
class DualGraph{
public:
    std::map<int,Polygon> node2pol;
    std::map<int,std::set<int>> adj;
    std::map<std::pair<Point,Point>,std::vector<int>> edgemap;
    int next_node_id = 0;
    DualGraph(std::vector<Polygon> polygons);
    DualGraph();
    int get_mx_node_id();
    int new_node();
    void add_edge(int a, int b);
    void remove_edge(int a, int b);
    int merge_nodes(int a, int b);
    std::array<bool,4> check_union(int a, int b);
    bool check_union_all(int a, int b);
    std::vector<int> get_nodes();
    int size();
    bool does_node_exist(int node);
    Polygon get_polygon(int node);
    std::vector<Polygon> get_polygons();
    std::vector<Polygon> get_polygons(std::vector<int>);
    std::vector<std::pair<Point,Point>> get_sides();
    std::vector<Point> get_points();
    int n();
    std::vector<int> get_dfs_order();
    std::pair<int,int> get_nodes_sharing_edge(Segment seg);
    std::vector<int> get_component(int node);
};
